// pl_reset_gen — PL reset generator for the Arty Zynq design.
//
// Holds `reset` asserted until the fabric is ready, then releases it after a
// settle window, and re-asserts if readiness drops. "Ready" must include BOTH:
//   - the MMCM is locked (PL clock is stable), and
//   - the PS is out of reset (FCLK_RESET0_N high),
// so the PL AXI slaves (fio_bridge / axi_xram / capture) reset in lockstep with
// the PS-side AXI conv/smc, which reset from FCLK_RESET0_N via the BD's rstgen.
// Without folding in the PS reset, a PS-only warm reboot clears one side of the
// AXI fabric but not the other and an in-flight beat can desync/hang.
//
// Both status inputs are asynchronous and 2-FF synchronized here.
module pl_reset_gen #(
    parameter int HOLD_CYCLES = 4095
) (
    input  logic clk,
    input  logic mmcm_locked,   // async: PL clock (MMCM) locked
    input  logic fclk_resetn,   // async, active-low: PS FCLK_RESET0_N
    output logic reset
);
    localparam int CW = $clog2(HOLD_CYCLES + 1);

    (* ASYNC_REG="TRUE" *) logic ls1 = 1'b0, ls2 = 1'b0;
    (* ASYNC_REG="TRUE" *) logic ps1 = 1'b0, ps2 = 1'b0;
    always_ff @(posedge clk) begin
        ls1 <= mmcm_locked; ls2 <= ls1;
        ps1 <= fclk_resetn; ps2 <= ps1;
    end

    logic [CW-1:0] rst_cnt = '0;
    logic          reset_r = 1'b1;
    always_ff @(posedge clk) begin
        // Assert reset whenever the fabric isn't ready: MMCM unlocked OR the PS is
        // in reset (fclk_resetn low). Folding the PS reset in keeps the PL AXI
        // slaves resetting in lockstep with the PS-side conv/smc (audit finding #3).
        if (!ls2 || !ps2) begin
            rst_cnt <= '0;
            reset_r <= 1'b1;
        end else if (rst_cnt != HOLD_CYCLES[CW-1:0]) begin
            rst_cnt <= rst_cnt + 1'b1;
            reset_r <= 1'b1;
        end else begin
            reset_r <= 1'b0;
        end
    end
    assign reset = reset_r;
endmodule
