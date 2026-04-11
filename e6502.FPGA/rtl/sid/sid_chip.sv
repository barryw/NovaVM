// SID chip wrapper for e6502 FPGA.
// Instantiates one complete SID: 3 voices + filter + tables.
// Based on MiSTer C64 SID (MiSTer-devel/C64_MiSTer).
// Filter/DAC from reDIP-SID by Dag Lem (CERN-OHL-S v2).
//
// Register interface: 32 bytes ($00-$1F per chip).
// Extension registers $1D-$1F: per-voice volume (0-15), default $0F.
// Mode input selects 6581 (0) or 8580 (1) behavior at runtime.

module sid_chip (
    input  logic        clk,
    input  logic        rst,
    input  logic        ce_1m,      // ~1 MHz clock enable
    input  logic        mode,       // 0 = MOS 6581, 1 = MOS 8580

    // CPU bus (active when cs is high)
    input  logic        cs,
    input  logic        we,
    input  logic [4:0]  addr,       // register offset 0-31
    input  logic [7:0]  din,
    output logic [7:0]  dout,

    // Audio output (signed 18-bit)
    output logic signed [17:0] audio_out
);

    // =========================================================================
    // Register file
    // =========================================================================
    reg [15:0] voice_freq  [3];
    reg [11:0] voice_pw    [3];
    reg  [7:0] voice_ctrl  [3];
    reg  [7:0] voice_ad    [3];
    reg  [7:0] voice_sr    [3];
    reg  [3:0] voice_vol   [3];    // per-voice volume extension

    reg [10:0] filter_fc;
    reg  [7:0] filter_res_filt;
    reg  [7:0] filter_mode_vol;

    reg  [7:0] last_written;        // bus data latch for read-back

    // Voice outputs
    wire signed [21:0] voice_out [3];
    wire        osc_msb [3];
    wire  [7:0] osc3_out;
    wire  [7:0] env3_out;

    // Combined waveform table I/O (assigned procedurally from table mux)
    reg   [7:0] _st_out [6];
    reg   [7:0] p_t_out [6];
    reg   [7:0] ps__out [6];
    reg   [7:0] pst_out [6];
    wire [11:0] acc_t   [3];

    // =========================================================================
    // Register write
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            for (int i = 0; i < 3; i++) begin
                voice_freq[i] <= 0;
                voice_pw[i]   <= 0;
                voice_ctrl[i] <= 0;
                voice_ad[i]   <= 0;
                voice_sr[i]   <= 0;
                voice_vol[i]  <= 4'hF;  // default full volume
            end
            filter_fc       <= 0;
            filter_res_filt <= 0;
            filter_mode_vol <= 0;
            last_written    <= 0;
        end
        else if (cs && we) begin
            last_written <= din;
            case (addr)
                // Voice 1
                5'h00: voice_freq[0][7:0]  <= din;
                5'h01: voice_freq[0][15:8] <= din;
                5'h02: voice_pw[0][7:0]    <= din;
                5'h03: voice_pw[0][11:8]   <= din[3:0];
                5'h04: voice_ctrl[0]       <= din;
                5'h05: voice_ad[0]         <= din;
                5'h06: voice_sr[0]         <= din;
                // Voice 2
                5'h07: voice_freq[1][7:0]  <= din;
                5'h08: voice_freq[1][15:8] <= din;
                5'h09: voice_pw[1][7:0]    <= din;
                5'h0A: voice_pw[1][11:8]   <= din[3:0];
                5'h0B: voice_ctrl[1]       <= din;
                5'h0C: voice_ad[1]         <= din;
                5'h0D: voice_sr[1]         <= din;
                // Voice 3
                5'h0E: voice_freq[2][7:0]  <= din;
                5'h0F: voice_freq[2][15:8] <= din;
                5'h10: voice_pw[2][7:0]    <= din;
                5'h11: voice_pw[2][11:8]   <= din[3:0];
                5'h12: voice_ctrl[2]       <= din;
                5'h13: voice_ad[2]         <= din;
                5'h14: voice_sr[2]         <= din;
                // Filter
                5'h15: filter_fc[2:0]      <= din[2:0];
                5'h16: filter_fc[10:3]     <= din;
                5'h17: filter_res_filt     <= din;
                5'h18: filter_mode_vol     <= din;
                // Per-voice volume (extension)
                5'h1D: voice_vol[0]        <= din[3:0];
                5'h1E: voice_vol[1]        <= din[3:0];
                5'h1F: voice_vol[2]        <= din[3:0];
            endcase
        end
    end

    // =========================================================================
    // Register read
    // =========================================================================
    always_comb begin
        case (addr)
            5'h19: dout = 8'h00;       // pot_x (not connected)
            5'h1A: dout = 8'h00;       // pot_y (not connected)
            5'h1B: dout = osc3_out;    // voice 3 oscillator
            5'h1C: dout = env3_out;    // voice 3 envelope
            5'h1D: dout = {4'h0, voice_vol[0]};
            5'h1E: dout = {4'h0, voice_vol[1]};
            5'h1F: dout = {4'h0, voice_vol[2]};
            default: dout = last_written;
        endcase
    end

    // =========================================================================
    // Voices
    // =========================================================================
    // Sync chain: voice 3 MSB -> voice 1 -> voice 2 -> voice 3
    sid_voice v1 (
        .clock(clk), .ce_1m(ce_1m), .reset(rst), .mode(mode),
        .freq(voice_freq[0]), .pw(voice_pw[0]),
        .control(voice_ctrl[0]), .att_dec(voice_ad[0]), .sus_rel(voice_sr[0]),
        .osc_msb_in(osc_msb[2]),
        .osc_msb_out(osc_msb[0]),
        .voice_out(voice_out[0]),
        .osc_out(), .env_out(),
        ._st_out(_st_out[0]), .p_t_out(p_t_out[0]),
        .ps__out(ps__out[0]),  .pst_out(pst_out[0]),
        .acc_t(acc_t[0])
    );

    sid_voice v2 (
        .clock(clk), .ce_1m(ce_1m), .reset(rst), .mode(mode),
        .freq(voice_freq[1]), .pw(voice_pw[1]),
        .control(voice_ctrl[1]), .att_dec(voice_ad[1]), .sus_rel(voice_sr[1]),
        .osc_msb_in(osc_msb[0]),
        .osc_msb_out(osc_msb[1]),
        .voice_out(voice_out[1]),
        .osc_out(), .env_out(),
        ._st_out(_st_out[1]), .p_t_out(p_t_out[1]),
        .ps__out(ps__out[1]),  .pst_out(pst_out[1]),
        .acc_t(acc_t[1])
    );

    sid_voice v3 (
        .clock(clk), .ce_1m(ce_1m), .reset(rst), .mode(mode),
        .freq(voice_freq[2]), .pw(voice_pw[2]),
        .control(voice_ctrl[2]), .att_dec(voice_ad[2]), .sus_rel(voice_sr[2]),
        .osc_msb_in(osc_msb[1]),
        .osc_msb_out(osc_msb[2]),
        .voice_out(voice_out[2]),
        .osc_out(osc3_out), .env_out(env3_out),
        ._st_out(_st_out[2]), .p_t_out(p_t_out[2]),
        .ps__out(ps__out[2]),  .pst_out(pst_out[2]),
        .acc_t(acc_t[2])
    );

    // =========================================================================
    // Combined waveform tables — time-multiplexed across 3 voices
    // =========================================================================
    wire [15:0] F0;
    reg  [3:0]  tbl_state;
    reg  [7:0]  f__st_out;
    reg  [7:0]  f_p_t_out;
    reg  [7:0]  f_ps__out;
    reg  [7:0]  f_pst_out;
    reg  [11:0] f_acc_t;

    sid_tables tbl (
        .clock(clk),
        .mode(mode),
        .acc_t(f_acc_t),
        ._st_out(f__st_out),
        .p_t_out(f_p_t_out),
        .ps__out(f_ps__out),
        .pst_out(f_pst_out),
        .Fc(filter_fc),
        .F0(F0)
    );

    // Time-multiplex table lookups across 3 voices
    always_ff @(posedge clk) begin
        if (~&tbl_state) tbl_state <= tbl_state + 1'd1;
        if (ce_1m) tbl_state <= 0;

        case (tbl_state)
            1, 3, 5: f_acc_t <= acc_t[tbl_state[2:1]];
        endcase

        case (tbl_state)
            3, 5, 7: begin
                _st_out[tbl_state[2:1] - 1] <= f__st_out;
                p_t_out[tbl_state[2:1] - 1] <= f_p_t_out;
                ps__out[tbl_state[2:1] - 1] <= f_ps__out;
                pst_out[tbl_state[2:1] - 1] <= f_pst_out;
            end
        endcase
    end

    // =========================================================================
    // Filter + mixer
    // =========================================================================
    wire signed [17:0] filter_audio;

    // Apply per-voice volume before filter
    wire signed [21:0] v1_scaled = (voice_out[0] * $signed({1'b0, voice_vol[0]})) >>> 4;
    wire signed [21:0] v2_scaled = (voice_out[1] * $signed({1'b0, voice_vol[1]})) >>> 4;
    wire signed [21:0] v3_scaled = (voice_out[2] * $signed({1'b0, voice_vol[2]})) >>> 4;

    sid_filter flt (
        .clk(clk),
        .state(tbl_state[2:0]),
        .mode(mode),
        .F0(F0),
        .Res_Filt(filter_res_filt),
        .Mode_Vol(filter_mode_vol),
        .voice1(v1_scaled),
        .voice2(v2_scaled),
        .voice3(v3_scaled),
        .ext_in(22'sd0),
        .audio(filter_audio)
    );

    // Latch audio on correct state
    always_ff @(posedge clk) begin
        if (tbl_state == 6)
            audio_out <= filter_audio;
    end

endmodule
