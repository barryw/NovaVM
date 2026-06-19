// arty_z7_vgc.sv — Phase 2: NovaVM VGC rendered to HDMI OUT on the Arty Z7-20.
//
//   125 MHz -> MMCM -> ~27 MHz pixel + ~135 MHz (5x) serial
//   vgc (text mode, 720x480 profile) generates RGB + sync/DE
//   -> hdl-util/hdmi (VIDEO_ID_CODE 2, 720x480p) -> OSERDESE2 -> OBUFDS -> J11
//
// No 6502 CPU: a small loader writes a text banner straight into VGC char RAM
// through the debug-vmem port at boot, then writes a spinning glyph once per
// frame so the display is visibly live. Default color RAM (0x01) renders
// white-on-black, and mode defaults to 0 (text), so only char codes are needed.

`default_nettype none

module arty_z7_vgc (
    input  wire        clk,            // 125 MHz, H16
    input  wire [3:0]  btn,            // btn[0] = reset
    output wire [3:0]  led,
    output wire        led4_b,         // heartbeat
    output wire        led4_g,         // MMCM locked

    output wire        hdmi_tx_clk_p,
    output wire        hdmi_tx_clk_n,
    output wire [2:0]  hdmi_tx_d_p,
    output wire [2:0]  hdmi_tx_d_n,
    output wire        hdmi_tx_hpdn
);

    // ---------------------------------------------------------------
    // Clocking: 125 MHz -> VCO 1078.125 MHz -> 134.77 MHz (x5) + 26.95 MHz.
    // 5:1 ratio is exact (divides 8 and 40 from one VCO); pixel ~0.18% low of
    // 27 MHz so 720x480p refreshes at ~59.83 Hz, well within TV tolerance.
    // ---------------------------------------------------------------
    wire clk_fb, clk_fb_buf, clk_x5_raw, clk_pix_raw;
    wire clk_pixel, clk_pixel_x5, mmcm_locked;

    MMCME2_BASE #(
        .BANDWIDTH        ("OPTIMIZED"),
        .CLKIN1_PERIOD    (8.000),
        .DIVCLK_DIVIDE    (1),
        .CLKFBOUT_MULT_F  (8.625),    // VCO = 1078.125 MHz
        .CLKOUT0_DIVIDE_F (8.000),    // 134.77 MHz -> clk_pixel_x5
        .CLKOUT1_DIVIDE   (40),       //  26.95 MHz -> clk_pixel
        .CLKOUT0_DUTY_CYCLE(0.5), .CLKOUT1_DUTY_CYCLE(0.5),
        .CLKOUT0_PHASE    (0.0),  .CLKOUT1_PHASE(0.0),
        .CLKFBOUT_PHASE   (0.0),
        .STARTUP_WAIT     ("FALSE")
    ) mmcm (
        .CLKIN1   (clk),
        .CLKFBIN  (clk_fb_buf), .CLKFBOUT(clk_fb), .CLKFBOUTB(),
        .CLKOUT0  (clk_x5_raw), .CLKOUT0B(),
        .CLKOUT1  (clk_pix_raw),.CLKOUT1B(),
        .CLKOUT2(), .CLKOUT2B(), .CLKOUT3(), .CLKOUT3B(),
        .CLKOUT4(), .CLKOUT5(), .CLKOUT6(),
        .LOCKED   (mmcm_locked), .PWRDWN(1'b0), .RST(btn[0])
    );
    BUFG bufg_fb  (.I(clk_fb),     .O(clk_fb_buf));
    BUFG bufg_x5  (.I(clk_x5_raw), .O(clk_pixel_x5));
    BUFG bufg_pix (.I(clk_pix_raw),.O(clk_pixel));

    // ---------------------------------------------------------------
    // Reset: hold until MMCM locked (+ a margin). Drives both the VGC scanout
    // reset and the HDMI core so they start aligned.
    // ---------------------------------------------------------------
    (* ASYNC_REG = "TRUE" *) logic lock_s1, lock_s2;
    always_ff @(posedge clk_pixel) begin
        lock_s1 <= mmcm_locked;
        lock_s2 <= lock_s1;
    end
    logic [9:0] rst_cnt = 10'd0;
    logic       reset   = 1'b1;
    always_ff @(posedge clk_pixel) begin
        if (!lock_s2)               begin rst_cnt <= 0;            reset <= 1'b1; end
        else if (rst_cnt != 10'h3FF) begin rst_cnt <= rst_cnt + 1; reset <= 1'b1; end
        else                         reset <= 1'b0;
    end

    // ---------------------------------------------------------------
    // VGC
    // ---------------------------------------------------------------
    logic [3:0] vid_r, vid_g, vid_b;
    logic       vid_hsync, vid_vsync, vid_de;

    // Debug-vmem write port (driven by the loader below).
    logic        dbg_vmem_we;
    logic [16:0] dbg_vmem_addr;
    logic [7:0]  dbg_vmem_wdata;

    vgc vgc_inst (
        .clk        (clk_pixel),
        .rst        (reset),
        .video_rst  (reset),

        // CPU bus — idle
        .cpu_ce(1'b0), .cpu_addr(16'd0), .cpu_raddr(16'd0), .cpu_wdata(8'd0),
        .cpu_rdata(), .cpu_we(1'b0), .cpu_re(1'b0),

        // Keyboard — idle
        .key_valid(1'b0), .key_data(8'd0), .key_ready(),

        // Blitter — idle
        .blt_space(3'd0), .blt_addr(17'd0), .blt_rdata(), .blt_wdata(8'd0),
        .blt_we(1'b0), .blt_re(1'b0), .video_blit_safe(),

        // Debug read — idle
        .dbg_addr(16'd0), .dbg_rdata(),

        // Debug write: registers idle, vmem driven by loader
        .dbg_we(1'b0), .dbg_waddr(16'd0), .dbg_wdata(8'd0),
        .dbg_vmem_we   (dbg_vmem_we),
        .dbg_vmem_re   (1'b0),
        .dbg_vmem_space(3'd1),               // 1 = char RAM
        .dbg_vmem_addr (dbg_vmem_addr),
        .dbg_vmem_wdata(dbg_vmem_wdata),
        .dbg_vmem_rdata(),

        // Video out
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),

        .irq_out(), .rdy_out(), .sys_reset_req()
    );

    // ---------------------------------------------------------------
    // Startup screen loader + live spinner (writes char RAM via dbg_vmem).
    // 80-col text grid: cell addr = row*80 + col.
    // ---------------------------------------------------------------
    localparam int L1_LEN  = 18;
    localparam int L2_LEN  = 20;
    localparam int L1_BASE = 22*80 + 31;     // centered "NOVA VGC OVER HDMI"
    localparam int L2_BASE = 24*80 + 30;     // centered "ARTY Z7-20  720X480P"
    localparam int SPIN_CELL = 26*80 + 39;   // a glyph that rotates each frame
    // Explicit ASCII byte arrays (no packed-string part-selects — those were
    // returning blanks under Vivado). "NOVA VGC OVER HDMI" / "ARTY Z7-20  720X480P".
    localparam logic [7:0] L1A [0:L1_LEN-1] = '{
        8'h4E,8'h4F,8'h56,8'h41,8'h20,8'h56,8'h47,8'h43,8'h20,
        8'h4F,8'h56,8'h45,8'h52,8'h20,8'h48,8'h44,8'h4D,8'h49 };
    localparam logic [7:0] L2A [0:L2_LEN-1] = '{
        8'h41,8'h52,8'h54,8'h59,8'h20,8'h5A,8'h37,8'h2D,8'h32,8'h30,
        8'h20,8'h20,8'h37,8'h32,8'h30,8'h58,8'h34,8'h38,8'h30,8'h50 };

    function automatic logic [7:0] spin_char(input logic [1:0] s);
        case (s) 2'd0: spin_char = 8'h2D;  // '-'
                 2'd1: spin_char = 8'h5C;  // '\'
                 2'd2: spin_char = 8'h7C;  // '|'
                 default: spin_char = 8'h2F; // '/'
        endcase
    endfunction

    // Repaint the whole banner every frame during vblank. The VGC clears its
    // char RAM as part of its post-reset sequence, so a one-shot write at boot
    // gets wiped; continuous repaint is immune to that (and to any later clear).
    localparam int N1   = L1_LEN;            // line 1 chars
    localparam int N2   = L2_LEN;            // line 2 chars
    localparam int NTOT = N1 + N2 + 1;       // + spinner cell

    logic [6:0] wi;          // write index 0..NTOT-1
    logic       writing;
    logic       vsync_d;
    logic [1:0] spin_idx;

    // Address/char for the current write index.
    logic [16:0] cur_addr;
    logic [7:0]  cur_char;
    always_comb begin
        if (wi < N1) begin
            cur_addr = L1_BASE + wi;
            cur_char = L1A[wi];
        end else if (wi < N1 + N2) begin
            cur_addr = L2_BASE + (wi - N1);
            cur_char = L2A[wi - N1];
        end else begin
            cur_addr = SPIN_CELL;
            cur_char = spin_char(spin_idx);
        end
    end

    always_ff @(posedge clk_pixel) begin
        if (reset) begin
            writing        <= 1'b0;
            wi             <= 7'd0;
            vsync_d        <= 1'b0;
            spin_idx       <= 2'd0;
            dbg_vmem_we    <= 1'b0;
            dbg_vmem_addr  <= 17'd0;
            dbg_vmem_wdata <= 8'd0;
        end else begin
            vsync_d     <= vid_vsync;
            dbg_vmem_we <= 1'b0;
            if (!writing) begin
                if (vid_vsync && !vsync_d) begin   // frame start -> repaint
                    writing <= 1'b1;
                    wi      <= 7'd0;
                end
            end else begin
                dbg_vmem_we    <= 1'b1;
                dbg_vmem_addr  <= cur_addr;
                dbg_vmem_wdata <= cur_char;
                if (wi == NTOT-1) begin
                    writing  <= 1'b0;
                    spin_idx <= spin_idx + 2'd1;   // advance spinner per frame
                end else begin
                    wi <= wi + 7'd1;
                end
            end
        end
    end

    // ---------------------------------------------------------------
    // RGB to HDMI (4-bit -> 8-bit nibble duplication; black when blanked).
    // ---------------------------------------------------------------
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};
    wire [23:0] hdmi_rgb = vid_de ? rgb24 : 24'h000000;

    // Start HDMI 3 px early so its active pixel 0 samples VGC active pixel 0
    // (VGC RGB/DE come through a 3-pixel output pipeline). 858x525 totals.
    localparam int HDMI_START_X = 858 - 3;
    localparam int HDMI_START_Y = 525 - 1;

    wire [2:0] tmds;
    wire       tmds_clock;
    logic [9:0] cx, cy, fw, fh, sw, sh;

    hdmi #(
        .VIDEO_ID_CODE (2),            // 720x480p
        .DVI_OUTPUT    (1'b1),
        .IT_CONTENT    (1'b1),
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
        .START_X       (HDMI_START_X),
        .START_Y       (HDMI_START_Y)
    ) hdmi_inst (
        .clk_pixel_x5 (clk_pixel_x5),
        .clk_pixel    (clk_pixel),
        .clk_audio    (1'b0),
        .reset        (reset),
        .rgb          (hdmi_rgb),
        .audio_sample_word('0),
        .tmds         (tmds),
        .tmds_clock   (tmds_clock),
        .cx(cx), .cy(cy),
        .frame_width(fw), .frame_height(fh),
        .screen_width(sw), .screen_height(sh)
    );

    OBUFDS obufds_clk (.I(tmds_clock), .O(hdmi_tx_clk_p), .OB(hdmi_tx_clk_n));
    genvar i;
    generate
        for (i = 0; i < 3; i++) begin : g_tmds
            OBUFDS obufds_d (.I(tmds[i]), .O(hdmi_tx_d_p[i]), .OB(hdmi_tx_d_n[i]));
        end
    endgenerate
    assign hdmi_tx_hpdn = 1'b0;

    // ---------------------------------------------------------------
    // Status LEDs.
    // ---------------------------------------------------------------
    logic [24:0] heartbeat;
    always_ff @(posedge clk_pixel) heartbeat <= heartbeat + 25'd1;
    assign led4_b = heartbeat[23];
    assign led4_g = mmcm_locked;
    assign led    = {~writing, reset, vid_vsync, mmcm_locked};

endmodule

`default_nettype wire
