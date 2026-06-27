// osd_logic.sv — VERBATIM extraction of the parked OSD button-debounce snippet
// (arty_z7_full.sv) and the system_pause clock-gates (top.sv: cpu_ce + the SID
// 1 MHz tick), for a focused Verilator unit test before the bitstream. The
// Vivado build then verifies the real wiring; this proves the logic.
`default_nettype none
module osd_logic (
    input  wire        clk,
    input  wire        rst,
    input  wire [3:0]  btn,
    input  wire        system_pause,
    input  wire        cpu_reset,
    input  wire        xram_stall,
    output reg  [3:0]  btn_level,
    output reg         cpu_ce,
    output reg         sid_ce_1m
);
    // ---- button debounce (verbatim from arty_z7_full.sv) -------------------
    (* ASYNC_REG="TRUE" *) logic [3:0] btn_s1, btn_s2;
    logic [3:0]  btn_db_prev;
    logic [16:0] btn_db_div;                  // ~4.85 ms window at 27 MHz
    always_ff @(posedge clk) begin
        btn_s1 <= btn; btn_s2 <= btn_s1;
        btn_db_div <= btn_db_div + 1'b1;
        if (btn_db_div == 17'd0) begin
            if (btn_s2 == btn_db_prev) btn_level <= btn_s2;   // stable across a full window
            btn_db_prev <= btn_s2;
        end
    end

    // ---- cpu_ce pause gate (verbatim from top.sv) --------------------------
    always_ff @(posedge clk) begin
        if (cpu_reset)         cpu_ce <= 0;
        else if (xram_stall)   cpu_ce <= 0;
        else if (system_pause) cpu_ce <= 0;   // OSD pause: freeze 6502
        else                   cpu_ce <= ~cpu_ce;
    end

    // ---- SID 1 MHz tick pause gate (verbatim pattern from top.sv) ----------
    localparam [25:0] SID_PIXEL_HZ = 26'd27000000;
    logic [25:0] sid_clk_acc;
    wire  [26:0] sid_clk_next = {1'b0, sid_clk_acc} + 27'd1000000;
    always_ff @(posedge clk) begin
        if (rst) begin
            sid_clk_acc <= 26'd0; sid_ce_1m <= 0;
        end else if (system_pause) begin
            sid_ce_1m <= 1'b0;                            // OSD pause: freeze SID oscillators
        end else if (sid_clk_next >= {1'b0, SID_PIXEL_HZ}) begin
            sid_clk_acc <= sid_clk_next - {1'b0, SID_PIXEL_HZ};
            sid_ce_1m   <= 1'b1;
        end else begin
            sid_clk_acc <= sid_clk_next[25:0];
            sid_ce_1m   <= 1'b0;
        end
    end
endmodule
`default_nettype wire
