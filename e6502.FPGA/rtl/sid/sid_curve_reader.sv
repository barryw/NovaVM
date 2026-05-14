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

    // SID Fc input + cached f0 output
    input  logic [10:0] sid_Fc,
    output logic [15:0] sid_f0,

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

    // Pending-refresh tracking: new Fc → raise flag, cleared when read completes
    logic [10:0] sid_fc_prev;
    logic        sid_pending;
    logic [10:0] latched_fc;         // Fc for the in-flight request

    logic [7:0]  low_byte_r;
    logic [15:0] sid_f0_r;

    assign sid_f0 = sid_f0_r;

    // Output drives
    wire         is_reading  = (state == S_READ_LO) || (state == S_READ_HI);
    wire [24:0]  lo_addr     = CURVE_BASE + {14'b0, latched_fc[10:1], 1'b0};
    wire [24:0]  hi_addr     = CURVE_BASE + {14'b0, latched_fc[10:1], 1'b1};
    assign sdram_addrB = (state == S_READ_LO)    ? lo_addr :
                         (state == S_CAPTURE_LO) ? lo_addr :
                         (state == S_READ_HI)    ? hi_addr :
                         (state == S_CAPTURE_HI) ? hi_addr :
                                                   25'd0;
    assign sdram_oeB   = is_reading;
    assign sdram_weB   = 1'b0;
    assign sdram_dinB  = 8'd0;

    always_ff @(posedge clk) begin
        if (rst) begin
            state        <= S_IDLE;
            hold_cnt     <= 0;
            sid_fc_prev  <= 11'h7FF;  // impossible initial value — forces first read
            sid_pending  <= 1'b1;
            sid_f0_r     <= 16'd0;
            latched_fc   <= 0;
            low_byte_r   <= 0;
        end else begin
            // Detect Fc changes — raise pending for a refresh.
            if (sid_Fc != sid_fc_prev) begin
                sid_pending <= 1'b1;
                sid_fc_prev <= sid_Fc;
            end

            case (state)
                S_IDLE: begin
                    if (sid_pending) begin
                        latched_fc <= sid_Fc;
                        hold_cnt   <= HOLD_CYCLES - 1;
                        state      <= S_READ_LO;
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
                    sid_f0_r <= {sdram_doutB, low_byte_r};
                    sid_pending <= (sid_Fc != latched_fc);
                    state <= S_IDLE;
                end
                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
