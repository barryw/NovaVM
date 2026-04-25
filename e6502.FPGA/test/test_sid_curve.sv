// SID filter curve reader test. Stands alone — instantiates
// sid_curve_reader.sv with a mock SDRAM port B that echoes addr[7:0]
// back as the byte. Verifies:
//   - Fc write triggers a curve fetch
//   - Latency is within spec (~32 pixel clocks, HOLD_CYCLES=8 gives
//     two 8-cycle reads + state transitions)
//   - Returned f0 = {hi_byte, lo_byte} matches mocked SDRAM contents
//   - Both SID1 and SID2 get served (round-robin)
//   - Fc change after initial fetch triggers new fetch
`timescale 1ns/1ps

module test_sid_curve;
    // ───── Clock / reset ──────────────────────────────────────────────
    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz pixel clock
    logic rst;

    // ───── DUT signals ────────────────────────────────────────────────
    logic [10:0] sid1_Fc = 0, sid2_Fc = 0;
    wire  [15:0] sid1_f0, sid2_f0;

    wire  [24:0] sdram_addrB;
    wire         sdram_weB;
    wire  [7:0]  sdram_dinB;
    wire         sdram_oeB;
    logic [7:0]  sdram_doutB;

    // ───── Mock SDRAM port B ─────────────────────────────────────────
    // Whenever sdram_oeB is high, drive sdram_doutB = addr[7:0] on the
    // next clock. This fakes "SDRAM returns (addr & 0xFF)" — easy to
    // verify without loading a 512KB backing store.
    always_ff @(posedge clk) begin
        if (rst) sdram_doutB <= 8'h00;
        else if (sdram_oeB) sdram_doutB <= sdram_addrB[7:0];
    end

    sid_curve_reader uut (
        .clk(clk), .rst(rst),
        .sid1_Fc(sid1_Fc), .sid1_f0(sid1_f0),
        .sid2_Fc(sid2_Fc), .sid2_f0(sid2_f0),
        .sdram_addrB(sdram_addrB),
        .sdram_weB(sdram_weB),
        .sdram_dinB(sdram_dinB),
        .sdram_oeB(sdram_oeB),
        .sdram_doutB(sdram_doutB)
    );

    // ───── Test accounting ────────────────────────────────────────────
    int pass_count = 0, fail_count = 0, test_num = 0;

    task automatic check(input string name, input logic cond);
        test_num++;
        if (cond) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    task automatic check_eq(input string name, input int a, input int e);
        test_num++;
        if (a == e) begin
            $display("  PASS [%0d] %s (=0x%0h)", test_num, name, a);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%0h, want 0x%0h)",
                     test_num, name, a, e);
            fail_count++;
        end
    endtask

    task automatic check_in_range(input string name,
                                   input int a, input int lo, input int hi);
        test_num++;
        if (a >= lo && a <= hi) begin
            $display("  PASS [%0d] %s (=%0d in [%0d..%0d])",
                     test_num, name, a, lo, hi);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got %0d, want [%0d..%0d])",
                     test_num, name, a, lo, hi);
            fail_count++;
        end
    endtask

    // ───── Helpers ────────────────────────────────────────────────────
    // CURVE_BASE = 25'h0_8_00_00 = 0x080000 = 524288.
    localparam int CURVE_BASE = 32'h080000;

    // Expected f0 when mock SDRAM echoes addr[7:0].
    function automatic logic [15:0] expected_f0(input [10:0] fc);
        logic [24:0] lo_a, hi_a;
        lo_a = CURVE_BASE + {14'b0, fc[10:1], 1'b0};
        hi_a = CURVE_BASE + {14'b0, fc[10:1], 1'b1};
        expected_f0 = {hi_a[7:0], lo_a[7:0]};
    endfunction

    // Wait until sid1_f0 / sid2_f0 changes from `prev`. Two separate
    // tasks because SystemVerilog `ref` args can't bind to module-level
    // wire/output signals (Verilator PROCASSWIRE).
    task automatic wait_sid1_f0(input  logic [15:0] prev,
                                 input  int         max_clks,
                                 output int         ticks);
        ticks = 0;
        while (sid1_f0 == prev && ticks < max_clks) begin
            @(posedge clk);
            ticks++;
        end
    endtask

    task automatic wait_sid2_f0(input  logic [15:0] prev,
                                 input  int         max_clks,
                                 output int         ticks);
        ticks = 0;
        while (sid2_f0 == prev && ticks < max_clks) begin
            @(posedge clk);
            ticks++;
        end
    endtask

    // ───── Test body ──────────────────────────────────────────────────
    initial begin
        int ticks;
        logic [15:0] prev;

        $display("=== SID curve reader ===");

        // Reset — the init sequence inside the module raises pending for
        // both chips so that an initial fetch happens. Fc is 0, so the
        // expected value is {0x01, 0x00} = 0x0100.
        rst = 1;
        repeat(20) @(posedge clk);
        rst = 0;

        // ── Initial fetch: SID1 served first after reset ──
        prev = 16'h0000;
        wait_sid1_f0(prev, 200, ticks);
        $display("  initial SID1 fetch: %0d clocks, f0=0x%04h",
                 ticks, sid1_f0);
        check_in_range("initial SID1 fetch ≤ 40 clocks", ticks, 1, 40);
        check_eq("initial SID1 f0 matches mock", int'(sid1_f0),
                 int'(expected_f0(11'h000)));

        // SID2 fetched right after — may have already completed in
        // flight with SID1 (ticks=0) or still pending (≤40 clks).
        wait_sid2_f0(prev, 200, ticks);
        $display("  initial SID2 fetch: %0d additional clocks, f0=0x%04h",
                 ticks, sid2_f0);
        check_in_range("initial SID2 fetch ≤ 40 clocks", ticks, 0, 40);
        check_eq("initial SID2 f0 matches mock", int'(sid2_f0),
                 int'(expected_f0(11'h000)));

        // ── Change SID1 Fc, verify new fetch ──
        prev = sid1_f0;
        sid1_Fc = 11'h1B0;  // arbitrary
        wait_sid1_f0(prev, 200, ticks);
        $display("  SID1 Fc=0x1B0 fetch: %0d clocks, f0=0x%04h",
                 ticks, sid1_f0);
        check_in_range("SID1 Fc-change fetch ≤ 40 clocks", ticks, 1, 40);
        check_eq("SID1 f0 after Fc=0x1B0", int'(sid1_f0),
                 int'(expected_f0(11'h1B0)));

        // ── Change SID2 Fc, verify round-robin ──
        prev = sid2_f0;
        sid2_Fc = 11'h2A0;
        wait_sid2_f0(prev, 200, ticks);
        $display("  SID2 Fc=0x2A0 fetch: %0d clocks, f0=0x%04h",
                 ticks, sid2_f0);
        check_in_range("SID2 Fc-change fetch ≤ 40 clocks", ticks, 1, 40);
        check_eq("SID2 f0 after Fc=0x2A0", int'(sid2_f0),
                 int'(expected_f0(11'h2A0)));

        // ── Both Fc change simultaneously — both get served ──
        prev = sid1_f0;
        sid1_Fc = 11'h3C0;
        sid2_Fc = 11'h4D0;
        wait_sid1_f0(prev, 200, ticks);
        prev = sid2_f0;
        wait_sid2_f0(prev, 200, ticks);
        check_eq("SID1 f0 after combined update", int'(sid1_f0),
                 int'(expected_f0(11'h3C0)));
        check_eq("SID2 f0 after combined update", int'(sid2_f0),
                 int'(expected_f0(11'h4D0)));

        // ── Address check: write Fc that selects a non-trivial byte ──
        // Fc[10:1] = 0x7F → lo_addr = CURVE_BASE + 0xFE = 0x0800FE
        // lo byte = 0xFE, hi byte = 0xFF → f0 = 0xFFFE
        sid1_Fc = 11'h0FF;
        prev = sid1_f0;
        wait_sid1_f0(prev, 200, ticks);
        check_eq("SID1 f0 for Fc=0x0FF (Fc[10:1]=0x7F)",
                 int'(sid1_f0), int'(expected_f0(11'h0FF)));

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end
endmodule
