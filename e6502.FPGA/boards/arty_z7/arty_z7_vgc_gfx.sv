// arty_z7_vgc_gfx.sv — VGC GRAPHICS demo over HDMI on the Arty Z7-20.
//
// Puts the VGC in mode 3 (gfx + sprites, no text) and continuously streams an
// animated pattern into the 320x200 4-bit graphics framebuffer via the debug
// vmem port (space 3). This exercises the graphics scanout path that the
// vgc.sv forward-reference fix unblocked under Vivado (previously the artist/
// gfx nets synthesized as implicit 1-bit wires).
//
//   125 MHz -> MMCM -> ~27 MHz pixel + ~135 MHz (5x) -> hdl-util/hdmi
//   (VIDEO_ID_CODE 2, 720x480p) -> OSERDESE2 -> OBUFDS -> J11.

`default_nettype none

module arty_z7_vgc_gfx (
    input  wire        clk,            // 125 MHz, H16
    input  wire [3:0]  btn,            // btn[0] = reset
    output wire [3:0]  led,
    output wire        led4_b,
    output wire        led4_g,

    output wire        hdmi_tx_clk_p,
    output wire        hdmi_tx_clk_n,
    output wire [2:0]  hdmi_tx_d_p,
    output wire [2:0]  hdmi_tx_d_n,
    output wire        hdmi_tx_hpdn
);

    // ---------------------------------------------------------------
    // Clocking (same as the text demo): 125 -> 134.77 (x5) + 26.95 (pixel).
    // ---------------------------------------------------------------
    wire clk_fb, clk_fb_buf, clk_x5_raw, clk_pix_raw;
    wire clk_pixel, clk_pixel_x5, mmcm_locked;

    MMCME2_BASE #(
        .BANDWIDTH("OPTIMIZED"), .CLKIN1_PERIOD(8.000), .DIVCLK_DIVIDE(1),
        .CLKFBOUT_MULT_F(8.625), .CLKOUT0_DIVIDE_F(8.000), .CLKOUT1_DIVIDE(40),
        .CLKOUT0_DUTY_CYCLE(0.5), .CLKOUT1_DUTY_CYCLE(0.5),
        .CLKOUT0_PHASE(0.0), .CLKOUT1_PHASE(0.0), .CLKFBOUT_PHASE(0.0),
        .STARTUP_WAIT("FALSE")
    ) mmcm (
        .CLKIN1(clk), .CLKFBIN(clk_fb_buf), .CLKFBOUT(clk_fb), .CLKFBOUTB(),
        .CLKOUT0(clk_x5_raw), .CLKOUT0B(), .CLKOUT1(clk_pix_raw), .CLKOUT1B(),
        .CLKOUT2(), .CLKOUT2B(), .CLKOUT3(), .CLKOUT3B(),
        .CLKOUT4(), .CLKOUT5(), .CLKOUT6(),
        .LOCKED(mmcm_locked), .PWRDWN(1'b0), .RST(btn[0])
    );
    BUFG bufg_fb  (.I(clk_fb),     .O(clk_fb_buf));
    BUFG bufg_x5  (.I(clk_x5_raw), .O(clk_pixel_x5));
    BUFG bufg_pix (.I(clk_pix_raw),.O(clk_pixel));

    (* ASYNC_REG = "TRUE" *) logic lock_s1, lock_s2;
    always_ff @(posedge clk_pixel) begin lock_s1 <= mmcm_locked; lock_s2 <= lock_s1; end
    logic [9:0] rst_cnt = 10'd0;
    logic       reset   = 1'b1;
    always_ff @(posedge clk_pixel) begin
        if (!lock_s2)                begin rst_cnt <= 0;            reset <= 1'b1; end
        else if (rst_cnt != 10'h3FF) begin rst_cnt <= rst_cnt + 1; reset <= 1'b1; end
        else                          reset <= 1'b0;
    end

    // ---------------------------------------------------------------
    // VGC + debug write ports.
    // ---------------------------------------------------------------
    logic [3:0] vid_r, vid_g, vid_b;
    logic       vid_hsync, vid_vsync, vid_de;

    logic        dbg_we;
    logic [15:0] dbg_waddr;
    logic [7:0]  dbg_wdata;
    logic        dbg_vmem_we;
    logic [2:0]  dbg_vmem_space;
    logic [16:0] dbg_vmem_addr;
    logic [7:0]  dbg_vmem_wdata;

    vgc vgc_inst (
        .clk(clk_pixel), .rst(reset), .video_rst(reset),
        .cpu_ce(1'b0), .cpu_addr(16'd0), .cpu_raddr(16'd0), .cpu_wdata(8'd0),
        .cpu_rdata(), .cpu_we(1'b0), .cpu_re(1'b0),
        .key_valid(1'b0), .key_data(8'd0), .key_ready(),
        .blt_space(3'd0), .blt_addr(17'd0), .blt_rdata(), .blt_wdata(8'd0),
        .blt_we(1'b0), .blt_re(1'b0), .video_blit_safe(),
        .dbg_addr(16'd0), .dbg_rdata(),
        .dbg_we(dbg_we), .dbg_waddr(dbg_waddr), .dbg_wdata(dbg_wdata),
        .dbg_vmem_we(dbg_vmem_we), .dbg_vmem_re(1'b0),
        .dbg_vmem_space(dbg_vmem_space), .dbg_vmem_addr(dbg_vmem_addr),
        .dbg_vmem_wdata(dbg_vmem_wdata), .dbg_vmem_rdata(),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .irq_out(), .rdy_out(), .sys_reset_req()
    );

    // ---------------------------------------------------------------
    // Demo loader: set mode=3 each frame; stream an animated pattern into the
    // 320x200 framebuffer (one pixel/cycle, looping forever -> immune to the
    // VGC's post-reset clear). Diagonal colour bands that drift over time.
    // ---------------------------------------------------------------
    logic [8:0]  gx;        // 0..319
    logic [7:0]  gy;        // 0..199
    logic [16:0] gaddr;     // 0..63999
    logic [7:0]  fcount;    // animation frame counter
    logic        vsync_d;

    wire [3:0] gpix = gx[8:4] + gy[7:4] + fcount[6:3];   // drifting diagonal bands

    always_ff @(posedge clk_pixel) begin
        if (reset) begin
            gx <= 9'd0; gy <= 8'd0; gaddr <= 17'd0; fcount <= 8'd0; vsync_d <= 1'b0;
            dbg_we <= 1'b0; dbg_waddr <= 16'd0; dbg_wdata <= 8'd0;
            dbg_vmem_we <= 1'b0; dbg_vmem_space <= 3'd3; dbg_vmem_addr <= 17'd0; dbg_vmem_wdata <= 8'd0;
        end else begin
            vsync_d <= vid_vsync;

            // Set mode = 3 (gfx + sprites, no text) once per frame, and advance
            // the animation. dbg_we is a separate port from dbg_vmem.
            dbg_we <= 1'b0;
            if (vid_vsync && !vsync_d) begin
                dbg_we    <= 1'b1;
                dbg_waddr <= 16'hA000;       // VGC mode register (VGC_BASE+0)
                dbg_wdata <= 8'h03;          // gfx + sprites, no text
                fcount    <= fcount + 8'd1;
            end

            // Stream one framebuffer pixel per cycle (space 3 = gfx).
            dbg_vmem_we    <= 1'b1;
            dbg_vmem_space <= 3'd3;
            dbg_vmem_addr  <= gaddr;
            dbg_vmem_wdata <= {4'd0, gpix};
            if (gx == 9'd319) begin
                gx <= 9'd0;
                if (gy == 8'd199) begin gy <= 8'd0; gaddr <= 17'd0; end
                else begin gy <= gy + 8'd1; gaddr <= gaddr + 17'd1; end
            end else begin
                gx <= gx + 9'd1; gaddr <= gaddr + 17'd1;
            end
        end
    end

    // ---------------------------------------------------------------
    // RGB -> HDMI.
    // ---------------------------------------------------------------
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};
    wire [23:0] hdmi_rgb = vid_de ? rgb24 : 24'h000000;

    localparam int HDMI_START_X = 858 - 3;
    localparam int HDMI_START_Y = 525 - 1;

    wire [2:0] tmds;
    wire       tmds_clock;
    logic [9:0] cx, cy, fw, fh, sw, sh;

    hdmi #(
        .VIDEO_ID_CODE(2), .DVI_OUTPUT(1'b1), .IT_CONTENT(1'b1),
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
        .START_X(HDMI_START_X), .START_Y(HDMI_START_Y)
    ) hdmi_inst (
        .clk_pixel_x5(clk_pixel_x5), .clk_pixel(clk_pixel), .clk_audio(1'b0),
        .reset(reset), .rgb(hdmi_rgb), .audio_sample_word('0),
        .tmds(tmds), .tmds_clock(tmds_clock),
        .cx(cx), .cy(cy), .frame_width(fw), .frame_height(fh),
        .screen_width(sw), .screen_height(sh)
    );

    OBUFDS obufds_clk (.I(tmds_clock), .O(hdmi_tx_clk_p), .OB(hdmi_tx_clk_n));
    genvar i;
    generate for (i = 0; i < 3; i++) begin : g_tmds
        OBUFDS obufds_d (.I(tmds[i]), .O(hdmi_tx_d_p[i]), .OB(hdmi_tx_d_n[i]));
    end endgenerate
    assign hdmi_tx_hpdn = 1'b0;

    logic [24:0] heartbeat;
    always_ff @(posedge clk_pixel) heartbeat <= heartbeat + 25'd1;
    assign led4_b = heartbeat[23];
    assign led4_g = mmcm_locked;
    assign led    = {fcount[3], reset, vid_vsync, mmcm_locked};

endmodule

`default_nettype wire
