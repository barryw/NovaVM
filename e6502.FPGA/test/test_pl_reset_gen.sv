// PL reset generator regression (boards/arty_z7/pl_reset_gen.sv).
//
// The PL reset must assert on MMCM-unlock AND on a PS reset (FCLK_RESET0_N low),
// so the PL AXI slaves reset in lockstep with the PS-side AXI conv/smc. This test
// drives both async status inputs and checks the reset follows both. The PS-reset
// case is the #3 fix — it fails until pl_reset_gen folds fclk_resetn in.

`timescale 1ns/1ps

module test_pl_reset_gen;

    logic clk = 0;
    always #5 clk = ~clk;

    logic mmcm_locked = 1'b0;
    logic fclk_resetn = 1'b1;
    wire  reset;

    localparam int HOLD = 8;   // small hold so the sim is fast

    pl_reset_gen #(.HOLD_CYCLES(HOLD)) dut (
        .clk(clk), .mmcm_locked(mmcm_locked), .fclk_resetn(fclk_resetn), .reset(reset)
    );

    int fails = 0;
    task automatic check(input string name, input logic cond);
        if (cond) $display("  PASS %s", name);
        else begin $display("  FAIL %s", name); fails++; end
    endtask
    task automatic wait_cyc(input int n); repeat (n) @(posedge clk); endtask

    initial begin
        // 1) Power-on, MMCM not yet locked -> reset asserted.
        wait_cyc(4);
        check("reset asserted before MMCM lock", reset === 1'b1);

        // 2) MMCM locks, PS out of reset -> reset releases after the hold window.
        mmcm_locked = 1'b1;
        wait_cyc(HOLD + 6);                     // 2-FF sync + counter + margin
        check("reset released after MMCM lock + hold", reset === 1'b0);

        // 3) PS-only reset: FCLK_RESET0_N low, MMCM STAYS locked -> reset re-asserts.
        fclk_resetn = 1'b0;
        wait_cyc(6);                            // 2-FF sync + register
        check("reset re-asserts on PS reset (fclk_resetn low)", reset === 1'b1);

        // 4) PS reset clears -> reset releases again after the hold window.
        fclk_resetn = 1'b1;
        wait_cyc(HOLD + 6);
        check("reset released again after PS reset clears", reset === 1'b0);

        // 5) MMCM unlock -> reset asserts.
        mmcm_locked = 1'b0;
        wait_cyc(6);
        check("reset re-asserts on MMCM unlock", reset === 1'b1);

        if (fails == 0) $display("\nTEST PASSED");
        else            $display("\nTEST FAILED (%0d checks failed)", fails);
        $finish;
    end

    initial begin
        repeat (100000) @(posedge clk);
        $display("\nTEST FAILED (timeout)");
        $finish;
    end

endmodule
