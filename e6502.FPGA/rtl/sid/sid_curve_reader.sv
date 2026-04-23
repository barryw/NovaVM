// SID filter curve reader — pulls f6581_curve entries from SDRAM port B.
//
// The curve table is 4096 entries × 16 bits = 8192 bytes, loaded into
// SDRAM at boot by NovaHost via debug_bridge's POKE_SDRAM command.
// This module replaces the per-sid_tables 4K×16 BRAM (4 DP16KD each,
// two sid_chips → 8 DP16KD) with on-demand SDRAM lookups. Fc changes
// are rare (only when CPU writes $D415/16) so the SDRAM port doesn't
// need to keep up with per-clock reads — we cache f0 per chip and
// refresh only on Fc change.
//
// Address layout:
//   SDRAM byte address = CURVE_BASE + Fc[10:1] * 2 + {low=0, high=1}
//
// Two SID chips share this one reader via round-robin arbitration.
// Each chip sees its own f0 register with ~32-pixel-clock refresh
// latency after Fc changes — well inside SID's ~1 MHz filter tick.

module sid_curve_reader #(
    parameter [24:0] CURVE_BASE = 25'h0_8_00_00  // 512 KB offset (XRAM ends at 512 KB)
) (
    input  logic        clk,          // pixel clock
    input  logic        rst,

    // Per-chip Fc inputs + f0 outputs
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
    logic       serving;           // 0 = SID1, 1 = SID2 (round-robin)

    // Pending-refresh tracking: new Fc → raise flag, cleared when read completes
    logic [10:0] sid1_fc_prev, sid2_fc_prev;
    logic        sid1_pending,  sid2_pending;
    logic [10:0] latched_fc;         // Fc for the in-flight request

    logic [7:0]  low_byte_r;
    logic [15:0] sid1_f0_r, sid2_f0_r;

    assign sid1_f0 = sid1_f0_r;
    assign sid2_f0 = sid2_f0_r;

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
            serving      <= 0;
            sid1_fc_prev <= 11'h7FF;  // impossible initial value — forces first read
            sid2_fc_prev <= 11'h7FF;
            sid1_pending <= 1'b1;
            sid2_pending <= 1'b1;
            sid1_f0_r    <= 16'd0;
            sid2_f0_r    <= 16'd0;
            latched_fc   <= 0;
            low_byte_r   <= 0;
        end else begin
            // Detect Fc changes — raise pending for that chip
            if (sid1_Fc != sid1_fc_prev) begin
                sid1_pending <= 1'b1;
                sid1_fc_prev <= sid1_Fc;
            end
            if (sid2_Fc != sid2_fc_prev) begin
                sid2_pending <= 1'b1;
                sid2_fc_prev <= sid2_Fc;
            end

            case (state)
                S_IDLE: begin
                    // Round-robin: prefer the other chip if our last serve was
                    // for this one, but serve whichever has work.
                    if (serving == 0) begin
                        if (sid2_pending) begin
                            serving    <= 1;
                            latched_fc <= sid2_Fc;
                            hold_cnt   <= HOLD_CYCLES - 1;
                            state      <= S_READ_LO;
                        end else if (sid1_pending) begin
                            latched_fc <= sid1_Fc;
                            hold_cnt   <= HOLD_CYCLES - 1;
                            state      <= S_READ_LO;
                        end
                    end else begin
                        if (sid1_pending) begin
                            serving    <= 0;
                            latched_fc <= sid1_Fc;
                            hold_cnt   <= HOLD_CYCLES - 1;
                            state      <= S_READ_LO;
                        end else if (sid2_pending) begin
                            latched_fc <= sid2_Fc;
                            hold_cnt   <= HOLD_CYCLES - 1;
                            state      <= S_READ_LO;
                        end
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
                    // Assemble the 16-bit curve value and commit to the
                    // serving chip's f0 register. Clear that chip's pending.
                    if (serving == 0) begin
                        sid1_f0_r    <= {sdram_doutB, low_byte_r};
                        sid1_pending <= 1'b0;
                    end else begin
                        sid2_f0_r    <= {sdram_doutB, low_byte_r};
                        sid2_pending <= 1'b0;
                    end
                    state <= S_IDLE;
                end
                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
