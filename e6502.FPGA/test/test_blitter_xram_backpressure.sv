// Blitter XRAM back-pressure regression — proves that blitter RAM → XRAM
// copies preserve every byte even when the XRAM wrapper periodically
// withholds its write-accept (xram_busy=1).
//
// The integration-level symptom is `STASH $2000,0,256` + `FETCH $2000,0,256`
// dropping bytes on FPGA HW. In top.sv, `bm_xram_fire = bm_xram_we &&
// bm_xram_armed && !xram_busy` — if xram_busy is high during the blitter's
// single-cycle S_WRITE state, the fire pulse never happens and the byte
// is silently lost. The blitter state machine advances regardless.
//
// Harness models this by delaying byte acceptance: when the test holds
// xram_busy high, the sim memory does NOT accept the write that cycle;
// the blitter must continue to drive xram_addr/xram_wdata/xram_we until
// xram_busy drops AND at least one cycle passes with the write-accept
// gate open. Before the fix, bytes are dropped. After, all land.

`timescale 1ns/1ps

module test_blitter_xram_backpressure;

    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic        rst;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    logic        cpu_we;
    logic        cpu_re;

    wire  [7:0]  blt_cpu_rdata;
    wire         blt_rdy;

    wire  [15:0] blt_ram_addr;
    wire  [7:0]  blt_ram_wdata;
    wire         blt_ram_we;

    wire  [18:0] blt_xram_addr;
    wire  [7:0]  blt_xram_wdata;
    wire         blt_xram_we;

    wire  [2:0]  blt_vgc_space;
    wire  [16:0] blt_vgc_addr;
    wire  [7:0]  blt_vgc_wdata;
    wire         blt_vgc_we;
    wire         blt_vgc_re;

    logic [7:0] sim_ram  [0:65535];
    logic [7:0] sim_xram [0:524287];

    logic [7:0] blt_ram_rdata;
    logic [7:0] blt_xram_rdata;

    // RAM (port B) — 1-cycle synchronous read like real dpram.
    always_ff @(posedge clk) begin
        blt_ram_rdata <= sim_ram[blt_ram_addr];
    end
    // blt_xram_rdata is driven by the multi-cycle read handshake below.

    // ----- Back-pressure + multi-cycle read model -------------------------
    // Real xram_sdram latches write on the first cycle (req_we && !busy)
    // and asserts busy for ~10 cycles. Reads are similar but the data
    // (r_dout) becomes valid only after busy drops.  Model a simple 4-cycle
    // read pipeline: req_re → busy for 3 cycles → drop busy, data valid.
    logic xram_busy_sim;
    logic [2:0] xram_read_cnt;
    logic [18:0] xram_read_addr_latch;
    logic xram_read_pending;

    always_ff @(posedge clk) begin
        if (blt_xram_we && !xram_busy_sim)
            sim_xram[blt_xram_addr] <= blt_xram_wdata;

        // Multi-cycle read handshake
        if (rst) begin
            xram_read_cnt <= 0;
            xram_read_pending <= 0;
        end else if (blt_xram_re && !xram_busy_sim && !xram_read_pending) begin
            xram_read_addr_latch <= blt_xram_addr;
            xram_read_cnt <= 3;
            xram_read_pending <= 1;
        end else if (xram_read_pending) begin
            if (xram_read_cnt == 0) begin
                blt_xram_rdata <= sim_xram[xram_read_addr_latch];
                xram_read_pending <= 0;
            end else begin
                xram_read_cnt <= xram_read_cnt - 1;
            end
        end
    end

    // busy is high during pending read OR injected busy oscillator.
    wire xram_busy_effective = xram_busy_sim | xram_read_pending;

    // RAM writes (destination) — no back-pressure on RAM side.
    always_ff @(posedge clk) begin
        if (blt_ram_we)
            sim_ram[blt_ram_addr] <= blt_ram_wdata;
    end

    wire blt_xram_re;

    blitter dut (
        .clk(clk), .rst(rst),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
        .cpu_we(cpu_we), .cpu_rdata(blt_cpu_rdata), .cpu_re(cpu_re),
        .rdy_out(blt_rdy),
        .ram_addr(blt_ram_addr), .ram_rdata(blt_ram_rdata),
        .ram_wdata(blt_ram_wdata), .ram_we(blt_ram_we),
        .xram_addr(blt_xram_addr), .xram_rdata(blt_xram_rdata),
        .xram_wdata(blt_xram_wdata), .xram_we(blt_xram_we),
        .xram_re(blt_xram_re),
        .xram_busy(xram_busy_effective),
        .vgc_space(blt_vgc_space), .vgc_addr(blt_vgc_addr),
        .vgc_rdata(8'h00), .vgc_wdata(blt_vgc_wdata),
        .vgc_we(blt_vgc_we), .vgc_re(blt_vgc_re)
    );

    int pass = 0, fail = 0, tnum = 0;

    localparam BLT = 16'hBA83;

    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr; cpu_wdata <= data; cpu_we <= 1;
        @(posedge clk);
        cpu_we <= 0;
    endtask

    task automatic blt_reg(input int off, input logic [7:0] data);
        write_reg(BLT + 16'(off), data);
    endtask

    task automatic wait_blt_done();
        int timeout = 20_000;
        while (!blt_rdy && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0) $fatal(1, "blitter timeout");
    endtask

    task automatic check(input string name, input logic cond);
        tnum++;
        if (cond) begin $display("  PASS [%0d] %s", tnum, name); pass++; end
        else      begin $display("  FAIL [%0d] %s", tnum, name); fail++; end
    endtask

    // Copy N bytes from sim_ram[src] to sim_xram[dst] via blitter.
    task automatic run_ram_to_xram_copy(input [15:0] src, input [18:0] dst,
                                         input [15:0] count);
        blt_reg(3, 8'h00);                    // SRCSPACE = CPU RAM
        blt_reg(4, 8'h05);                    // DSTSPACE = XRAM
        blt_reg(5, src[7:0]);  blt_reg(6, src[15:8]); blt_reg(7, 8'h00);
        blt_reg(8, dst[7:0]);  blt_reg(9, dst[15:8]); blt_reg(10, {5'h00, dst[18:16]});
        blt_reg(11, count[7:0]); blt_reg(12, count[15:8]);
        blt_reg(13, 8'h01); blt_reg(14, 8'h00);    // height = 1
        blt_reg(15, count[7:0]); blt_reg(16, count[15:8]); // src stride
        blt_reg(17, count[7:0]); blt_reg(18, count[15:8]); // dst stride
        blt_reg(19, 8'h00);                    // MODE = copy (not fill)
        blt_reg(20, 8'h00);                    // fillval
        blt_reg(21, 8'h00);                    // color key
        blt_reg(0, 8'h01);                     // CMD_START
        wait_blt_done();
    endtask

    // Copy N bytes from sim_xram[src] to sim_ram[dst] — the FETCH path.
    task automatic run_xram_to_ram_copy(input [18:0] src, input [15:0] dst,
                                         input [15:0] count);
        blt_reg(3, 8'h05);                    // SRCSPACE = XRAM
        blt_reg(4, 8'h00);                    // DSTSPACE = CPU RAM
        blt_reg(5, src[7:0]);  blt_reg(6, src[15:8]); blt_reg(7, {5'h00, src[18:16]});
        blt_reg(8, dst[7:0]);  blt_reg(9, dst[15:8]); blt_reg(10, 8'h00);
        blt_reg(11, count[7:0]); blt_reg(12, count[15:8]);
        blt_reg(13, 8'h01); blt_reg(14, 8'h00);    // height = 1
        blt_reg(15, count[7:0]); blt_reg(16, count[15:8]);
        blt_reg(17, count[7:0]); blt_reg(18, count[15:8]);
        blt_reg(19, 8'h00);
        blt_reg(20, 8'h00);
        blt_reg(21, 8'h00);
        blt_reg(0, 8'h01);
        wait_blt_done();
    endtask

    // Fill N bytes of sim_xram[dst] with fill_value via blitter fill mode.
    // Fill differs from copy: stays in S_WRITE across bytes, xram_we stays
    // high continuously. Exercises the arm-latch / back-pressure interaction
    // that COPY's S_READ→S_WRITE cycling accidentally masked. Real-world
    // trigger: XMC's XRESET clears pages 0-4 (1280 bytes) via blitter fill.
    task automatic run_xram_fill(input [18:0] dst, input [15:0] count, input [7:0] val);
        blt_reg(3, 8'h00);                    // SRCSPACE ignored in fill mode
        blt_reg(4, 8'h05);                    // DSTSPACE = XRAM
        blt_reg(5, 8'h00); blt_reg(6, 8'h00); blt_reg(7, 8'h00);
        blt_reg(8, dst[7:0]); blt_reg(9, dst[15:8]); blt_reg(10, {5'h00, dst[18:16]});
        blt_reg(11, count[7:0]); blt_reg(12, count[15:8]);
        blt_reg(13, 8'h01); blt_reg(14, 8'h00);    // height = 1
        blt_reg(15, count[7:0]); blt_reg(16, count[15:8]);
        blt_reg(17, count[7:0]); blt_reg(18, count[15:8]);
        blt_reg(19, 8'h01);                    // MODE = fill
        blt_reg(20, val);                      // fillval
        blt_reg(21, 8'h00);
        blt_reg(0, 8'h01);                     // CMD_START
        wait_blt_done();
    endtask

    initial begin
        $display("=== Blitter RAM→XRAM back-pressure test ===");
        rst = 1;
        cpu_addr = 0; cpu_wdata = 0; cpu_we = 0; cpu_re = 0;
        xram_busy_sim = 0;
        for (int i = 0; i < 65536; i++) sim_ram[i]  = 0;
        for (int i = 0; i < 524288; i++) sim_xram[i] = 0;
        repeat(10) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);

        // Seed source pattern.
        for (int i = 0; i < 256; i++)
            sim_ram[16'h2000 + 16'(i)] = 8'(i);

        // --- Test 1: copy with xram_busy=0 throughout (baseline) ---
        $display("");
        $display("Test: 256-byte RAM→XRAM copy, no back-pressure");
        run_ram_to_xram_copy(16'h2000, 19'd0, 16'd256);
        repeat(10) @(posedge clk);
        begin
            automatic int errors = 0;
            for (int i = 0; i < 256; i++)
                if (sim_xram[i] !== 8'(i)) errors++;
            check($sformatf("all 256 bytes landed (%0d mismatches)", errors),
                  errors == 0);
        end

        // --- Test 2: copy with periodic xram_busy pulses ---
        // Reset destination, then run the copy while an async process
        // holds xram_busy_sim high every ~3rd cycle.
        $display("");
        $display("Test: 256-byte RAM→XRAM copy with xram_busy=1 every 3rd cycle");
        for (int i = 0; i < 256; i++) sim_xram[i] = 8'h00;

        fork
            begin
                // Busy generator — toggles every ~3 cycles for the duration.
                repeat(3000) begin
                    @(posedge clk);
                    xram_busy_sim <= ($urandom_range(0, 2) == 0);
                end
                xram_busy_sim <= 0;
            end
            begin
                run_ram_to_xram_copy(16'h2000, 19'd0, 16'd256);
                repeat(50) @(posedge clk);
            end
        join_any
        disable fork;
        xram_busy_sim = 0;
        repeat(10) @(posedge clk);

        begin
            automatic int errors = 0;
            for (int i = 0; i < 256; i++)
                if (sim_xram[i] !== 8'(i)) errors++;
            check($sformatf("all 256 bytes landed under back-pressure (%0d mismatches)", errors),
                  errors == 0);
        end

        // --- Test 3: XRAM fill (no back-pressure) ---
        $display("");
        $display("Test: 256-byte XRAM fill, no back-pressure");
        for (int i = 0; i < 256; i++) sim_xram[i] = 8'h00;
        run_xram_fill(19'd0, 16'd256, 8'h55);
        repeat(10) @(posedge clk);
        begin
            automatic int errors = 0;
            for (int i = 0; i < 256; i++)
                if (sim_xram[i] !== 8'h55) errors++;
            check($sformatf("all 256 bytes = 0x55 (%0d mismatches)", errors),
                  errors == 0);
        end

        // --- Test 4: XRAM fill with back-pressure pulses ---
        // This is the XRESET path. Pre-fix: fill stays in S_WRITE continuously
        // with xram_we=1 across all bytes, so the arm-latch disarmed after
        // the first byte and every subsequent byte was dropped.
        $display("");
        $display("Test: 256-byte XRAM fill with xram_busy=1 every 3rd cycle");
        for (int i = 0; i < 256; i++) sim_xram[i] = 8'h00;

        fork
            begin
                repeat(3000) begin
                    @(posedge clk);
                    xram_busy_sim <= ($urandom_range(0, 2) == 0);
                end
                xram_busy_sim <= 0;
            end
            begin
                run_xram_fill(19'd0, 16'd256, 8'hAA);
                repeat(50) @(posedge clk);
            end
        join_any
        disable fork;
        xram_busy_sim = 0;
        repeat(10) @(posedge clk);

        begin
            automatic int errors = 0;
            for (int i = 0; i < 256; i++)
                if (sim_xram[i] !== 8'hAA) errors++;
            check($sformatf("all 256 fill bytes landed under back-pressure (%0d mismatches)", errors),
                  errors == 0);
        end

        // --- Test 5: XRAM → RAM copy (FETCH path) ---
        $display("");
        $display("Test: 64-byte XRAM→RAM copy exercising multi-cycle read");
        // Seed distinct XRAM pattern.
        for (int i = 0; i < 64; i++) sim_xram[19'h1000 + 19'(i)] = 8'((i * 7) ^ 8'h5A);
        for (int i = 0; i < 64; i++) sim_ram[16'h3000 + 16'(i)] = 8'h00;

        run_xram_to_ram_copy(19'h1000, 16'h3000, 16'd64);
        repeat(20) @(posedge clk);
        begin
            automatic int errors = 0;
            for (int i = 0; i < 64; i++)
                if (sim_ram[16'h3000 + 16'(i)] !== 8'((i * 7) ^ 8'h5A)) errors++;
            check($sformatf("FETCH 64 bytes XRAM→RAM correct (%0d mismatches)", errors),
                  errors == 0);
        end

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass, fail);
        if (fail == 0) $display("ALL TESTS PASSED");
        else           $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #20_000_000;
        $display("FAIL: global timeout");
        $finish;
    end
endmodule
