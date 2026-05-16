// SID filter curve reader — pulls f6581_curve entries from SDRAM port B.
//
// The curve table is 4096 entries × 16 bits = 8192 bytes, loaded into
// SDRAM at boot by NovaHost via debug_bridge's POKE_SDRAM command.
// This module replaces the per-sid_tables 4K×16 BRAM (4 DP16KD each,
// two sid_chips → 8 DP16KD) with on-demand SDRAM lookups. Fc changes
// are rare (only when CPU writes $D415/16) so the SDRAM port doesn't
// need to keep up with per-clock reads — we cache f0 and refresh only
// on Fc change.
//
// Address layout:
//   SDRAM byte address = CURVE_BASE + Fc[10:1] * 2 + {low=0, high=1}
//
module sid_curve_reader #(
    parameter [24:0] CURVE_BASE = 25'h0_8_00_00  // 512 KB offset (XRAM ends at 512 KB)
) (
    input  logic        clk,          // pixel clock
    input  logic        rst,

    // SID Fc inputs + cached f0 outputs. A single SDRAM port services both
    // chips because Fc changes are rare and the lookup result is cached.
    input  logic [10:0] sid1_Fc,
    output logic [15:0] sid1_f0,
    input  logic [10:0] sid2_Fc,
    output logic [15:0] sid2_f0,

    // SDRAM port B (pixel-clock domain; sdram.v synchronizes internally)
    output logic [24:0] sdram_addrB,
    output logic        sdram_weB,
    output logic [7:0]  sdram_dinB,
    output logic        sdram_oeB,
    input  logic [7:0]  sdram_doutB
);

    // Number of pixel clocks to hold an SDRAM port B request asserted.
    // Same budgeting as xram_sdram.sv: covers one full clkref period
    // worst-case plus the CAS pipeline.
    localparam int HOLD_CYCLES = 8;

    typedef enum logic [2:0] {
        S_IDLE,
        S_READ_LO,        // issue read for low byte
        S_CAPTURE_LO,
        S_READ_HI,        // issue read for high byte
        S_CAPTURE_HI
    } state_t;

    state_t     state;
    logic [3:0] hold_cnt;

    // Pending-refresh tracking: new Fc -> raise flag, cleared when read completes.
    logic [10:0] sid1_fc_prev;
    logic [10:0] sid2_fc_prev;
    logic [1:0]  sid_pending;
    logic        active_sid;         // 0 = SID1, 1 = SID2
    logic [10:0] latched_fc;         // Fc for the in-flight request
    logic [24:0] lo_addr_r;
    logic [24:0] hi_addr_r;

    logic [7:0]  low_byte_r;
    logic [15:0] sid1_f0_r;
    logic [15:0] sid2_f0_r;

    assign sid1_f0 = sid1_f0_r;
    assign sid2_f0 = sid2_f0_r;

    // Output drives
    wire         is_reading  = (state == S_READ_LO) || (state == S_READ_HI);
    assign sdram_addrB = (state == S_READ_LO)    ? lo_addr_r :
                         (state == S_CAPTURE_LO) ? lo_addr_r :
                         (state == S_READ_HI)    ? hi_addr_r :
                         (state == S_CAPTURE_HI) ? hi_addr_r :
                                                   25'd0;
    assign sdram_oeB   = is_reading;
    assign sdram_weB   = 1'b0;
    assign sdram_dinB  = 8'd0;

    always_ff @(posedge clk) begin
        if (rst) begin
            state        <= S_IDLE;
            hold_cnt     <= 0;
            sid1_fc_prev <= 11'h7FF;  // impossible initial values force first reads
            sid2_fc_prev <= 11'h7FE;
            sid_pending  <= 2'b11;
            active_sid   <= 1'b0;
            sid1_f0_r    <= 16'd0;
            sid2_f0_r    <= 16'd0;
            latched_fc   <= 0;
            lo_addr_r    <= 25'd0;
            hi_addr_r    <= 25'd0;
            low_byte_r   <= 0;
        end else begin
            // Detect Fc changes and remember the newest requested value.
            if (sid1_Fc != sid1_fc_prev) begin
                sid_pending[0] <= 1'b1;
                sid1_fc_prev <= sid1_Fc;
            end
            if (sid2_Fc != sid2_fc_prev) begin
                sid_pending[1] <= 1'b1;
                sid2_fc_prev <= sid2_Fc;
            end

            case (state)
                S_IDLE: begin
                    if (sid_pending[0] || (sid1_Fc != sid1_fc_prev)) begin
                        active_sid     <= 1'b0;
                        latched_fc     <= sid1_Fc;
                        lo_addr_r      <= CURVE_BASE + {14'b0, sid1_Fc[10:1], 1'b0};
                        hi_addr_r      <= CURVE_BASE + {14'b0, sid1_Fc[10:1], 1'b1};
                        sid_pending[0] <= 1'b0;
                        hold_cnt       <= HOLD_CYCLES - 1;
                        state          <= S_READ_LO;
                    end else if (sid_pending[1] || (sid2_Fc != sid2_fc_prev)) begin
                        active_sid     <= 1'b1;
                        latched_fc     <= sid2_Fc;
                        lo_addr_r      <= CURVE_BASE + {14'b0, sid2_Fc[10:1], 1'b0};
                        hi_addr_r      <= CURVE_BASE + {14'b0, sid2_Fc[10:1], 1'b1};
                        sid_pending[1] <= 1'b0;
                        hold_cnt       <= HOLD_CYCLES - 1;
                        state          <= S_READ_LO;
                    end
                end
                S_READ_LO: begin
                    if (hold_cnt == 0) begin
                        state <= S_CAPTURE_LO;
                    end else begin
                        hold_cnt <= hold_cnt - 1;
                    end
                end
                S_CAPTURE_LO: begin
                    low_byte_r <= sdram_doutB;
                    hold_cnt   <= HOLD_CYCLES - 1;
                    state      <= S_READ_HI;
                end
                S_READ_HI: begin
                    if (hold_cnt == 0) begin
                        state <= S_CAPTURE_HI;
                    end else begin
                        hold_cnt <= hold_cnt - 1;
                    end
                end
                S_CAPTURE_HI: begin
                    if (active_sid == 1'b0) begin
                        sid1_f0_r <= {sdram_doutB, low_byte_r};
                        sid_pending[0] <= (sid1_Fc != latched_fc);
                    end else begin
                        sid2_f0_r <= {sdram_doutB, low_byte_r};
                        sid_pending[1] <= (sid2_Fc != latched_fc);
                    end
                    state <= S_IDLE;
                end
                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
