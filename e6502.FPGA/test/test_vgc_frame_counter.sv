// VGC $A008 frame counter regression test.
//
// BASIC's SPRITESET (basic.asm:9651) and SPRPRI (:9820) use VGC_FRAME at
// $A008 as a vblank barrier:
//
//     LDA VGC_FRAME
// @wait:
//     CMP VGC_FRAME
//     BEQ @wait        ; loop while equal = waiting for tick
//
// Before the fix (commit tbd), $A008 was mapped to gfx_color which is
// static. That wait loop never exits. Any SPRITESET from BASIC hangs
// the CPU and wedges the hardware — all subsequent integration tests
// fail because the board can't recover without a power cycle.
//
// This test verifies frame_counter DOES tick by sampling the signal
// before and after a full frame's worth of cycles. V_TOTAL=525 lines at
// H_TOTAL pixels each is the frame period; the counter should change
// at least once across that span.

`timescale 1ns/1ps

module test_vgc_frame_counter;

    logic clk = 0;
    always #20 clk = ~clk;  // ~25 MHz

    logic rst;

    logic        cpu_ce = 1;
    logic [15:0] cpu_addr = 0;
    logic [7:0]  cpu_wdata = 0;
    wire  [7:0]  cpu_rdata;
    logic        cpu_we = 0;
    logic        cpu_re = 0;

    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;

    // Tie off unused
    wire [15:0] tile_dma_addr; wire [7:0] tile_dma_data = 0; wire tile_dma_active;
    wire [2:0] blt_space; wire [15:0] blt_addr; wire [7:0] blt_rdata;
    logic [7:0] blt_wdata = 0; logic blt_we = 0, blt_re = 0;
    logic [15:0] dbg_addr = 0;
    wire [7:0] dbg_rdata;
    wire [3:0] vid_r, vid_g, vid_b;
    wire vid_hsync, vid_vsync, vid_de;
    wire irq_out;

    vgc dut (
        .clk(clk), .rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata), .cpu_rdata(cpu_rdata),
        .cpu_we(cpu_we), .cpu_re(cpu_re),
        .key_valid(key_valid), .key_data(key_data),
        .tile_dma_addr(tile_dma_addr), .tile_dma_data(tile_dma_data),
        .tile_dma_active(tile_dma_active),
        .blt_space(blt_space), .blt_addr(blt_addr),
        .blt_rdata(blt_rdata), .blt_wdata(blt_wdata),
        .blt_we(blt_we), .blt_re(blt_re),
        .dbg_addr(dbg_addr), .dbg_rdata(dbg_rdata),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .irq_out(irq_out)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;

    task automatic check_ne8(input string name, input logic [7:0] a, input logic [7:0] b);
        test_num++;
        if (a !== b) begin
            $display("  PASS [%0d] %s (got 0x%02X vs 0x%02X — differ)", test_num, name, a, b);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (both 0x%02X — frame counter NOT ticking)", test_num, name, a);
            fail_count++;
        end
    endtask

    task automatic check_eq8(input string name, input logic [7:0] a, input logic [7:0] b);
        test_num++;
        if (a === b) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, a);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)", test_num, name, a, b);
            fail_count++;
        end
    endtask

    // Read $A008 via the cpu_addr/cpu_rdata path. Combinational read —
    // present the address for one cycle and sample cpu_rdata.
    task automatic read_frame_byte(output logic [7:0] result);
        cpu_addr <= 16'hA008;
        cpu_re   <= 1;
        @(posedge clk);
        result = cpu_rdata;
        cpu_re   <= 0;
        cpu_addr <= 0;
        @(posedge clk);
    endtask

    // H_TOTAL at the VGA-mode VGC uses is 800 (640 + borders+sync).
    // V_TOTAL is 525. Full frame = 800*525 = 420,000 pixel cycles.
    // That's too slow to simulate in real wall time, so we sample at the
    // RTL-internal frame_counter signal directly — this proves the tick
    // logic works without burning ~1M cycles.

    logic [7:0] fc_a, fc_b;

    initial begin
        $display("=== VGC $A008 frame counter regression test ===");

        rst = 1;
        repeat(20) @(posedge clk);
        rst = 0;
        repeat(20) @(posedge clk);

        // Sample initial value via CPU read.
        read_frame_byte(fc_a);
        $display("initial frame_counter via cpu_rdata: 0x%02X", fc_a);

        // Force a vblank tick by driving the internal counters. Avoids
        // simulating a full 420k-cycle frame. The increment fires when
        // h_count==0 && v_count==V_ACTIVE (480) — we can nudge that by
        // observing the natural counter progression over ~500k cycles,
        // OR by directly checking the signal increments by walking it.
        // We use the direct-signal probe for speed.
        fc_a = dut.frame_counter;

        // Wait long enough for multiple vblanks. Full frame ~420k pixel
        // cycles. Run 5 frames so frame_counter crosses 0x04 — this
        // puts it past the gfx_color default (0x01) so the assertions
        // can't false-pass via a coincidental value match.
        repeat(800 * 525 * 5) @(posedge clk);

        fc_b = dut.frame_counter;
        $display("after ~5 frames frame_counter: 0x%02X", fc_b);

        check_ne8("frame_counter increments across vblanks", fc_a, fc_b);

        // fc_b must be >= 4 for this test to be meaningful (diverging
        // from any 4-bit default like gfx_color=0x01). Assert the
        // value is in a plausible range.
        test_num++;
        if (fc_b >= 8'd4) begin
            $display("  PASS [%0d] frame_counter value plausible (=0x%02X, >=4)", test_num, fc_b);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] frame_counter only advanced to 0x%02X — expected >=4 after 5 frames", test_num, fc_b);
            fail_count++;
        end

        // Verify CPU-visible read sees the same updated value. This is
        // the assertion that catches a regression where $A008 is wired
        // back to gfx_color (0x01) instead of the ticking counter.
        read_frame_byte(fc_a);
        check_eq8("cpu_rdata at $A008 matches internal frame_counter",
                  fc_a, dut.frame_counter);

        // Verify debug-peek path also returns frame_counter.
        dbg_addr <= 16'hA008;
        @(posedge clk);
        check_eq8("dbg_rdata at $A008 matches frame_counter",
                  dbg_rdata, dut.frame_counter);
        dbg_addr <= 0;

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #200000000;
        $display("FAIL: global timeout hit");
        $finish;
    end

endmodule
