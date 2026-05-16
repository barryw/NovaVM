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
    output logic signed [17:0] audio_out,

    // Filter curve pulled from SDRAM by an external sid_curve_reader.
    // We publish our Fc so it can detect changes; f0_in returns the
    // 16-bit cached curve value.
    output logic [10:0] filter_fc_out,
    input  logic [15:0] filter_f0_in
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

    // reDIP SID bus bridge. CPU/debug writes may arrive on any FPGA clock, so
    // queue them and present one write per SID phi2-low window.
    sid::bus_i_t bus_i;
    sid::cs_t    redip_cs;
    sid::pot_i_t redip_pot_i;
    sid::pot_o_t redip_pot_o;
    sid::audio_t redip_audio_i;
    sid::audio_t redip_audio_o;
    sid::reg8_t  redip_data_o;
    sid::reg8_t  osc3_out;
    sid::reg8_t  env3_out;

    logic [4:0] write_fifo_addr [32];
    logic [7:0] write_fifo_data [32];
    logic [4:0] write_fifo_rd;
    logic [4:0] write_fifo_wr;
    logic [5:0] write_fifo_count;

    logic [4:0] active_addr;
    logic [7:0] active_data;
    logic       write_active;
    logic [4:0] sid_phase;

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
    //
    // Write-registers shadow the last write so PEEK returns the written value.
    // Real 6581 returns bus-latched garbage for write-only regs; shadowing is a
    // convenience for the integration suite and matches the software emulator.
    // =========================================================================
    always_comb begin
        case (addr)
            // Voice 1
            5'h00: dout = voice_freq[0][7:0];
            5'h01: dout = voice_freq[0][15:8];
            5'h02: dout = voice_pw[0][7:0];
            5'h03: dout = {4'h0, voice_pw[0][11:8]};
            5'h04: dout = voice_ctrl[0];
            5'h05: dout = voice_ad[0];
            5'h06: dout = voice_sr[0];
            // Voice 2
            5'h07: dout = voice_freq[1][7:0];
            5'h08: dout = voice_freq[1][15:8];
            5'h09: dout = voice_pw[1][7:0];
            5'h0A: dout = {4'h0, voice_pw[1][11:8]};
            5'h0B: dout = voice_ctrl[1];
            5'h0C: dout = voice_ad[1];
            5'h0D: dout = voice_sr[1];
            // Voice 3
            5'h0E: dout = voice_freq[2][7:0];
            5'h0F: dout = voice_freq[2][15:8];
            5'h10: dout = voice_pw[2][7:0];
            5'h11: dout = {4'h0, voice_pw[2][11:8]};
            5'h12: dout = voice_ctrl[2];
            5'h13: dout = voice_ad[2];
            5'h14: dout = voice_sr[2];
            // Filter
            5'h15: dout = {5'h00, filter_fc[2:0]};
            5'h16: dout = filter_fc[10:3];
            5'h17: dout = filter_res_filt;
            5'h18: dout = filter_mode_vol;
            // External inputs / per-voice extensions
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
    // reDIP SID engine
    // =========================================================================
    localparam logic [4:0] SID_PHI2_LOW_CYCLES = 5'd12;

    wire enqueue_write = cs && we && (write_fifo_count < 6'd32);
    wire start_write = ce_1m && !write_active && (write_fifo_count != 0);
    wire sid_phi2 = sid_phase >= SID_PHI2_LOW_CYCLES;

    always_ff @(posedge clk) begin
        if (rst) begin
            write_fifo_rd    <= 0;
            write_fifo_wr    <= 0;
            write_fifo_count <= 0;
            active_addr      <= 0;
            active_data      <= 0;
            write_active     <= 1'b0;
            if (ce_1m)
                sid_phase <= 0;
            else if (sid_phase != 5'd31)
                sid_phase <= sid_phase + 5'd1;
        end else begin
            if (ce_1m)
                sid_phase <= 0;
            else if (sid_phase != 5'd31)
                sid_phase <= sid_phase + 5'd1;

            if (enqueue_write) begin
                write_fifo_addr[write_fifo_wr] <= addr;
                write_fifo_data[write_fifo_wr] <= din;
                write_fifo_wr <= write_fifo_wr + 5'd1;
            end

            if (start_write) begin
                active_addr  <= write_fifo_addr[write_fifo_rd];
                active_data  <= write_fifo_data[write_fifo_rd];
                write_fifo_rd <= write_fifo_rd + 5'd1;
                write_active <= 1'b1;
            end else if (write_active && sid_phase == SID_PHI2_LOW_CYCLES) begin
                write_active <= 1'b0;
            end

            unique case ({enqueue_write, start_write})
                2'b10: write_fifo_count <= write_fifo_count + 6'd1;
                2'b01: write_fifo_count <= write_fifo_count - 6'd1;
                default: write_fifo_count <= write_fifo_count;
            endcase
        end
    end

    assign bus_i.addr = active_addr;
    assign bus_i.data = active_data;
    assign bus_i.phi2 = sid_phi2;
    assign bus_i.r_w_n = ~(write_active && !sid_phi2);
    assign bus_i.res = rst;

    assign redip_cs.cs_n = ~write_active;
    assign redip_cs.cs_io1_n = 1'b1;
    assign redip_cs.a8 = 1'b0;
    assign redip_cs.a5 = 1'b0;

    assign redip_pot_i.charged = 2'b11;
    assign redip_audio_i.left = 24'sd0;
    assign redip_audio_i.right = 24'sd0;

    nova_redip_sid_api redip (
        .clk        (clk),
        .bus_i      (bus_i),
        .cs         (redip_cs),
        .model_8580 (mode),
        .voice0_vol (voice_vol[0]),
        .voice1_vol (voice_vol[1]),
        .voice2_vol (voice_vol[2]),
        .data_o     (redip_data_o),
        .osc3_o     (osc3_out),
        .env3_o     (env3_out),
        .pot_i      (redip_pot_i),
        .pot_o      (redip_pot_o),
        .audio_i    (redip_audio_i),
        .audio_o    (redip_audio_o)
    );

    // Publish the shadowed Fc for legacy top-level wiring. reDIP computes its
    // own filter curve internally, so filter_f0_in is intentionally unused.
    assign filter_fc_out = filter_fc;

    always_ff @(posedge clk) begin
        if (rst)
            audio_out <= 18'sd0;
        else
            audio_out <= redip_audio_o.left >>> 6;
    end

endmodule
