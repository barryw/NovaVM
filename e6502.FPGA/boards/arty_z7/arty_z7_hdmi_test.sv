// arty_z7_hdmi_test.sv — Phase 1 HDMI-OUT bring-up for the Arty Z7-20.
//
// Proves the device-specific HDMI path on the Zynq PL before wiring the VGC:
//   125 MHz -> MMCM -> 25 MHz pixel + 125 MHz (5x) serial
//   hdl-util/hdmi (DVI mode) -> OSERDESE2 serializer (auto-selected because
//   LATTICE_ECP5 is NOT defined) -> OBUFDS -> HDMI source J11.
//
// Output is 640x480p60 (CEA VIDEO_ID_CODE 1) with a moving color-bar pattern.
// NOTE: the HDMI *source* needs the board powered from a wall adapter (USB 5V
// can sag below the 4.8V HDMI minimum). Connect a monitor to J11 (HDMI OUT).

`default_nettype none

module arty_z7_hdmi_test (
    input  wire        clk,            // 125 MHz, H16
    input  wire [3:0]  btn,            // btn[0] = reset
    output wire [3:0]  led,
    output wire        led4_b,         // heartbeat
    output wire        led4_g,         // MMCM locked

    // HDMI source (J11) — differential TMDS
    output wire        hdmi_tx_clk_p,
    output wire        hdmi_tx_clk_n,
    output wire [2:0]  hdmi_tx_d_p,
    output wire [2:0]  hdmi_tx_d_n,
    output wire        hdmi_tx_hpdn    // drive HPD/output-enable low-side; see XDC
);

    // ---------------------------------------------------------------
    // Clocking: 125 MHz in -> 1000 MHz VCO -> 125 MHz (x5) + 25 MHz (pixel).
    // 25.0 MHz pixel (vs 25.175 nominal) is within monitor tolerance and
    // gives clean integer MMCM dividers.
    // ---------------------------------------------------------------
    wire clk_fb, clk_fb_buf;
    wire clk_x5_raw, clk_pix_raw;
    wire clk_pixel, clk_pixel_x5;
    wire mmcm_locked;

    MMCME2_BASE #(
        .BANDWIDTH        ("OPTIMIZED"),
        .CLKIN1_PERIOD    (8.000),      // 125 MHz
        .DIVCLK_DIVIDE    (1),
        .CLKFBOUT_MULT_F  (8.000),      // VCO = 1000 MHz
        .CLKFBOUT_PHASE   (0.000),
        .CLKOUT0_DIVIDE_F (8.000),      // 125 MHz  -> clk_pixel_x5
        .CLKOUT1_DIVIDE   (40),         // 25  MHz  -> clk_pixel
        .CLKOUT0_DUTY_CYCLE(0.5),
        .CLKOUT1_DUTY_CYCLE(0.5),
        .CLKOUT0_PHASE    (0.000),
        .CLKOUT1_PHASE    (0.000),
        .STARTUP_WAIT     ("FALSE")
    ) mmcm (
        .CLKIN1   (clk),
        .CLKFBIN  (clk_fb_buf),
        .CLKFBOUT (clk_fb),
        .CLKFBOUTB(),
        .CLKOUT0  (clk_x5_raw),
        .CLKOUT0B (),
        .CLKOUT1  (clk_pix_raw),
        .CLKOUT1B (),
        .CLKOUT2  (), .CLKOUT2B(),
        .CLKOUT3  (), .CLKOUT3B(),
        .CLKOUT4  (), .CLKOUT5(), .CLKOUT6(),
        .LOCKED   (mmcm_locked),
        .PWRDWN   (1'b0),
        .RST      (btn[0])
    );

    BUFG bufg_fb  (.I(clk_fb),     .O(clk_fb_buf));
    BUFG bufg_x5  (.I(clk_x5_raw), .O(clk_pixel_x5));
    BUFG bufg_pix (.I(clk_pix_raw),.O(clk_pixel));

    // ---------------------------------------------------------------
    // Reset: hold until MMCM locked (locked is async -> 2FF sync).
    // ---------------------------------------------------------------
    (* ASYNC_REG = "TRUE" *) logic lock_s1, lock_s2;
    always_ff @(posedge clk_pixel) begin
        lock_s1 <= mmcm_locked;
        lock_s2 <= lock_s1;
    end

    logic [7:0] rst_cnt = 8'd0;
    logic       reset   = 1'b1;
    always_ff @(posedge clk_pixel) begin
        if (!lock_s2) begin
            rst_cnt <= 8'd0;
            reset   <= 1'b1;
        end else if (rst_cnt != 8'hFF) begin
            rst_cnt <= rst_cnt + 8'd1;
            reset   <= 1'b1;
        end else begin
            reset   <= 1'b0;
        end
    end

    // ---------------------------------------------------------------
    // HDMI core (DVI mode -> no audio islands). It owns the video timing
    // and emits cx/cy; we generate rgb from them.
    // ---------------------------------------------------------------
    logic [9:0] cx, cy;
    logic [9:0] screen_w, screen_h, frame_w, frame_h;
    logic [23:0] rgb;
    wire  [2:0] tmds;
    wire        tmds_clock;

    hdmi #(
        .VIDEO_ID_CODE (1),        // 640x480p60
        .DVI_OUTPUT    (1'b1),
        .IT_CONTENT    (1'b1),
        .VIDEO_REFRESH_RATE_MILLIHZ(60000)
    ) hdmi_inst (
        .clk_pixel_x5 (clk_pixel_x5),
        .clk_pixel    (clk_pixel),
        .clk_audio    (1'b0),
        .reset        (reset),
        .rgb          (rgb),
        .audio_sample_word('0),
        .tmds         (tmds),
        .tmds_clock   (tmds_clock),
        .cx           (cx),
        .cy           (cy),
        .frame_width  (frame_w),
        .frame_height (frame_h),
        .screen_width (screen_w),
        .screen_height(screen_h)
    );

    // ---------------------------------------------------------------
    // Test pattern: 8 vertical color bars + 1px white border + a box that
    // bounces horizontally once per frame (motion proves the pixel clock).
    // ---------------------------------------------------------------
    localparam int H_ACT = 640;
    localparam int V_ACT = 480;

    logic [9:0] box_x;
    logic       box_dir;
    logic [9:0] cy_d;
    always_ff @(posedge clk_pixel) begin
        if (reset) begin
            box_x   <= 10'd16;
            box_dir <= 1'b0;
            cy_d    <= 10'd0;
        end else begin
            cy_d <= cy;
            // advance once per frame at the top-left pixel
            if (cx == 10'd0 && cy == 10'd0 && cy_d != 10'd0) begin
                if (!box_dir) begin
                    if (box_x >= H_ACT - 48) box_dir <= 1'b1;
                    else                     box_x   <= box_x + 10'd4;
                end else begin
                    if (box_x <= 10'd16)     box_dir <= 1'b0;
                    else                     box_x   <= box_x - 10'd4;
                end
            end
        end
    end

    wire active = (cx < H_ACT) && (cy < V_ACT);
    wire border = active && (cx == 0 || cx == H_ACT-1 || cy == 0 || cy == V_ACT-1);
    wire box    = active && (cx >= box_x) && (cx < box_x + 10'd32)
                         && (cy >= 10'd208) && (cy < 10'd272);

    // 8 SMPTE-ish bars across 640 px (80 px each).
    logic [23:0] bar;
    always_comb begin
        case (cx[9:7])  // cx/128 ~ but use /80 boundaries below instead
            default: bar = 24'h000000;
        endcase
        // explicit 80px bars:
        if      (cx < 10'd80)  bar = 24'hC0C0C0; // gray
        else if (cx < 10'd160) bar = 24'hC0C000; // yellow
        else if (cx < 10'd240) bar = 24'h00C0C0; // cyan
        else if (cx < 10'd320) bar = 24'h00C000; // green
        else if (cx < 10'd400) bar = 24'hC000C0; // magenta
        else if (cx < 10'd480) bar = 24'hC00000; // red
        else if (cx < 10'd560) bar = 24'h0000C0; // blue
        else                   bar = 24'h202020; // near-black
    end

    always_ff @(posedge clk_pixel) begin
        if (!active)     rgb <= 24'h000000;
        else if (border) rgb <= 24'hFFFFFF;
        else if (box)    rgb <= 24'hFF4040;
        else             rgb <= bar;
    end

    // ---------------------------------------------------------------
    // Differential TMDS output buffers (single-ended core -> _p/_n pins).
    // ---------------------------------------------------------------
    OBUFDS obufds_clk (.I(tmds_clock), .O(hdmi_tx_clk_p), .OB(hdmi_tx_clk_n));
    genvar i;
    generate
        for (i = 0; i < 3; i++) begin : g_tmds
            OBUFDS obufds_d (.I(tmds[i]), .O(hdmi_tx_d_p[i]), .OB(hdmi_tx_d_n[i]));
        end
    endgenerate

    // HDMI source HPD line: drive inactive (we don't gate on hot-plug here).
    assign hdmi_tx_hpdn = 1'b0;

    // ---------------------------------------------------------------
    // Status LEDs.
    // ---------------------------------------------------------------
    logic [24:0] heartbeat;
    always_ff @(posedge clk_pixel) heartbeat <= heartbeat + 25'd1;

    assign led4_b = heartbeat[23];     // ~1.5 Hz alive blink
    assign led4_g = mmcm_locked;
    assign led    = {reset, ~reset & ~box_dir, box_dir, mmcm_locked};

endmodule

`default_nettype wire
