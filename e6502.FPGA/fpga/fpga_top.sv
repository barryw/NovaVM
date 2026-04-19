// FPGA top-level for ULX3S (ECP5-85F)
// Wraps the simulation 'top' module with board-level I/O:
//   - PLL: 25 MHz osc -> 25 MHz pixel clock + 125 MHz TMDS shift clock
//   - HDMI output via GPDI (vga2dvid + fake differential DDR)
//   - UART keyboard input via FTDI serial
//   - 4-bit R-2R DAC audio output
//   - LEDs for debug, buttons for reset

module fpga_top (
    input  logic        clk25_mhz,

    // HDMI (GPDI)
    output logic [3:0]  gpdi_dp,
    output logic [3:0]  gpdi_dn,

    // LEDs and buttons
    output logic [7:0]  leds,
    input  logic [6:0]  btn,

    // UART via FTDI
    input  logic        ftdi_txd,     // FPGA receives from FTDI
    output logic        ftdi_rxd,     // FPGA transmits to FTDI

    // ESP32 control
    output logic        wifi_gpio0,   // keep ESP32 out of download mode

    // Audio DAC (4-bit R-2R)
    output logic [3:0]  audio_l,
    output logic [3:0]  audio_r
);

    // =========================================================================
    // PLL: 25 MHz -> 25 MHz (pixel) + 125 MHz (TMDS shift, 5x DDR)
    // =========================================================================
    wire [3:0] pll_clk;
    wire       pll_locked;

    ecp5pll #(
        .in_hz  (25000000),
        .out0_hz(25000000),   // pixel clock
        .out1_hz(125000000)   // 5x pixel clock for DDR TMDS
    ) pll_inst (
        .clk_i      (clk25_mhz),
        .clk_o      (pll_clk),
        .locked     (pll_locked),
        .reset      (1'b0),
        .standby    (1'b0),
        .phasesel   (2'b0),
        .phasedir   (1'b0),
        .phasestep  (1'b0),
        .phaseloadreg(1'b0)
    );

    wire clk_pixel = pll_clk[0];  // 25 MHz
    wire clk_shift = pll_clk[1];  // 125 MHz

    // =========================================================================
    // Reset: hold reset until PLL locks, then release
    // btn[0] is active-low power button — directly usable as manual reset
    // =========================================================================
    reg [7:0] rst_cnt = 8'hFF;
    wire      rst = (rst_cnt != 0);

    always_ff @(posedge clk_pixel) begin
        if (!pll_locked || !btn[0])
            rst_cnt <= 8'hFF;
        else if (rst_cnt != 0)
            rst_cnt <= rst_cnt - 1;
    end

    // =========================================================================
    // UART receiver — keyboard input via FTDI serial (115200 8N1)
    // =========================================================================
    wire [7:0] uart_data;
    wire       uart_valid;

    uart_rx #(
        .CLK_HZ(25000000),
        .BAUD  (9600)
    ) uart_inst (
        .clk  (clk_pixel),
        .rst  (rst),
        .rx   (ftdi_txd),
        .data (uart_data),
        .valid(uart_valid)
    );

    // =========================================================================
    // Core system — 6502 + VGC + Blitter + SID
    // =========================================================================
    wire [3:0]  vid_r, vid_g, vid_b;
    wire        vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] sid_audio_l, sid_audio_r;

    top core (
        .clk        (clk_pixel),
        .rst        (rst),

        .key_valid  (uart_valid),
        .key_data   (uart_data),

        .irq_n      (1'b1),
        .nmi_n      (1'b1),

        .vid_r      (vid_r),
        .vid_g      (vid_g),
        .vid_b      (vid_b),
        .vid_hsync  (vid_hsync),
        .vid_vsync  (vid_vsync),
        .vid_de     (vid_de),

        .audio_l    (sid_audio_l),
        .audio_r    (sid_audio_r),

        // Debug interface — unused on real hardware
        .dbg_peek_en  (1'b0),
        .dbg_peek_addr(16'h0000),
        .dbg_peek_data(),
        .dbg_poke_en  (1'b0),
        .dbg_poke_addr(16'h0000),
        .dbg_poke_data(8'h00),
        .dbg_pause    (1'b0),
        .dbg_cpu_pc   (),
        .dbg_cpu_a    (),
        .dbg_cpu_x    (),
        .dbg_cpu_y    (),
        .dbg_cpu_sp   (),
        .dbg_cpu_flags()
    );

    // =========================================================================
    // HDMI output — VGA signals through TMDS encoder to GPDI
    // =========================================================================

    // Pad 4-bit color to 8-bit
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};

    // VGA timing — use VGC's own sync/DE signals (pipeline-aligned with pixel data)
    // VGC outputs active-LOW hsync/vsync; vga2dvid expects active-HIGH hsync/vsync
    wire [1:0] tmds_red, tmds_green, tmds_blue, tmds_clock;

    vga2dvid vga2dvid_inst (
        .clk_pixel (clk_pixel),
        .clk_shift (clk_shift),
        .in_color  (rgb24),
        .in_blank  (~vid_de),
        .in_hsync  (~vid_hsync),
        .in_vsync  (~vid_vsync),
        .resetn    (~rst),
        .out_red   (tmds_red),
        .out_green (tmds_green),
        .out_blue  (tmds_blue),
        .out_clock (tmds_clock)
    );

    // DDR output via fake_differential (drives both P and N pins)
    fake_differential #(.C_ddr(1)) fake_diff_inst (
        .clk_shift(clk_shift),
        .in_clock (tmds_clock),
        .in_red   (tmds_red),
        .in_green (tmds_green),
        .in_blue  (tmds_blue),
        .out_p    (gpdi_dp),
        .out_n    (gpdi_dn)
    );

    // =========================================================================
    // Audio — convert signed 18-bit SID output to unsigned 4-bit for DAC
    // =========================================================================
    // Offset binary: add 2^17 to make unsigned, take top 4 bits
    wire [17:0] audio_l_unsigned = sid_audio_l + 18'sd131072;
    wire [17:0] audio_r_unsigned = sid_audio_r + 18'sd131072;

    assign audio_l = audio_l_unsigned[17:14];
    assign audio_r = audio_r_unsigned[17:14];

    // =========================================================================
    // LEDs — useful debug indicators
    // =========================================================================
    reg [23:0] heartbeat;
    always_ff @(posedge clk_pixel)
        heartbeat <= heartbeat + 1;

    reg [7:0] last_uart_byte;
    reg       uart_ever_received;
    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            last_uart_byte <= 0;
            uart_ever_received <= 0;
        end else if (uart_valid) begin
            last_uart_byte <= uart_data;
            uart_ever_received <= 1;
        end
    end

    assign leds[7]   = heartbeat[23];
    assign leds[6]   = pll_locked;
    assign leds[5]   = ~rst;
    assign leds[4]   = uart_ever_received;
    assign leds[3:0] = last_uart_byte[3:0];

    // =========================================================================
    // FTDI TX — unused for now, drive idle high
    // =========================================================================
    assign ftdi_rxd = 1'b1;

    // Keep ESP32 out of download mode
    assign wifi_gpio0 = 1'b1;

endmodule
