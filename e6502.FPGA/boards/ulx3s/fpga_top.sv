// FPGA top-level for ULX3S (ECP5-85F)
// Wraps the simulation 'top' module with board-level I/O:
//   - PLL: 25 MHz osc -> 25 MHz pixel clock + 125 MHz TMDS shift clock
//     (or 720x480p: 27 MHz pixel + 135 MHz TMDS shift)
//   - HDMI output via GPDI (hdl-util/hdmi with PCM audio)
//   - UART keyboard input via FTDI serial
//   - Optional USB low/full-speed HID keyboard input via US2
//   - SPI command bridge from ESP32/NovaHost
//   - LEDs for debug, buttons for reset

module fpga_top (
    input  logic        clk25_mhz,

    // HDMI (GPDI)
    output logic [3:0]  gpdi_dp,
`ifndef GPDI_P_ONLY
    output logic [3:0]  gpdi_dn,
`endif

    // LEDs and buttons
    output logic [7:0]  leds,
    input  logic [6:0]  btn,
    input  logic [3:0]  sw,

    // UART via FTDI
    input  logic        ftdi_txd,     // FPGA receives from FTDI
    output logic        ftdi_rxd,     // FPGA transmits to FTDI

    // ESP32 control pins. The command bridge uses shared SPI below.
    output logic        wifi_gpio0,   // keep ESP32 out of download mode
    input  logic        wifi_txd,     // unused legacy ESP UART TX pin
    output logic        wifi_rxd,     // held idle; SPI is the host bridge

    // ESP32 shared-SPI debug bridge. These pins share the ULX3S SD-card SPI
    // bus; esp_spi_cs_n is a separate FPGA chip-select on SD D1/GPIO4.
    input  logic        esp_spi_sck,
    input  logic        esp_spi_mosi,
    input  logic        esp_spi_cs_n,
    inout  wire         esp_spi_miso,

    // FPGA-side USB port (US2) for low/full-speed HID host devices.
    inout  wire         usb_fpga_bd_dn,
    inout  wire         usb_fpga_bd_dp,
    output logic        usb_fpga_pu_dn,
    output logic        usb_fpga_pu_dp,

    // SDRAM — MT48LC16M16 (32MB). Not yet used by the core, just brought up
    // on the pads so Phase 2.5 Step 2 can migrate XRAM into it.
    output logic        sdram_clk,
    output logic        sdram_cke,
    output logic        sdram_csn,
    output logic        sdram_wen,
    output logic        sdram_rasn,
    output logic        sdram_casn,
    output logic [1:0]  sdram_ba,
    output logic [1:0]  sdram_dqm,
    output logic [12:0] sdram_a,
    inout  wire  [15:0] sdram_d
);

    // =========================================================================
    // PLL:
    //   Normal: 25 MHz -> 25 MHz pixel + 125 MHz TMDS shift + SDRAM clocks
    //   VIDEO_480P_TESTPATTERN/VIDEO_720X480:
    //     25 MHz -> 27 MHz pixel + 135 MHz TMDS shift
    // =========================================================================
    wire [3:0] pll_clk;
    wire       pll_video_locked;
    wire       pll_locked;

    ecp5pll #(
`ifdef VIDEO_480P_TESTPATTERN
`ifndef VIDEO_480P_PIXEL_PHASE
`define VIDEO_480P_PIXEL_PHASE 0
`endif
        // Primary output must be the 5x clock so the simple PLL wrapper can
        // hit exact CEA 480p rates from the 25 MHz ULX3S oscillator.
        .in_hz  (25000000),
`ifdef VIDEO_VGA640_TIMING
        .out0_hz(125000000),  // 5x pixel clock for DDR TMDS
        .out1_hz(25000000),   // VGA 640x480 diagnostic timing
`else
        .out0_hz(135000000),  // 5x pixel clock for DDR TMDS
        .out1_hz(27000000),   // CEA 720x480p59.94 pixel clock
`endif
        .out1_deg(`VIDEO_480P_PIXEL_PHASE),
        .out2_hz(0),
        .out3_hz(0)
`elsif VIDEO_720X480
        // Full NovaVM build at CEA-style 720x480p. Keep the video PLL focused
        // on exact 27/135 MHz; SDRAM stays on a separate 100/6.25 MHz PLL.
        .in_hz  (25000000),
        .out0_hz(135000000),  // 5x pixel clock for DDR TMDS
        .out1_hz(27000000),   // CEA 720x480p pixel clock
        .out2_hz(0),
        .out3_hz(0)
`else
        .in_hz  (25000000),
        .out0_hz(25000000),   // pixel clock
        .out1_hz(125000000),  // 5x pixel clock for DDR TMDS
        .out2_hz(100000000),  // SDRAM clock
        .out3_hz(6250000)     // SDRAM clkref — each port gets a full q=0..7
                              // state machine cycle per clkref HALF-period,
                              // so SDRAM:clkref must be 16:1. At 8:1 port A's
                              // READ data becomes valid at q=7 but clkref has
                              // flipped to LOW by then, so doutA never latches
                              // (controller only latches port A when clkref is
                              // HIGH at q=7). Ratios verified in sim.
`endif
    ) pll_inst (
        .clk_i      (clk25_mhz),
        .clk_o      (pll_clk),
        .locked     (pll_video_locked),
        .reset      (1'b0),
        .standby    (1'b0),
        .phasesel   (2'b0),
        .phasedir   (1'b0),
        .phasestep  (1'b0),
        .phaseloadreg(1'b0)
    );

`ifdef VIDEO_720X480
    wire [3:0] pll_sdram_clk;
    wire       pll_sdram_locked;

    ecp5pll #(
        .in_hz  (25000000),
        .out0_hz(100000000),  // SDRAM clock
        .out1_hz(6250000),    // SDRAM clkref — 16:1 ratio vs clk_sdram
        .out2_hz(0),
        .out3_hz(0)
    ) pll_sdram_inst (
        .clk_i      (clk25_mhz),
        .clk_o      (pll_sdram_clk),
        .locked     (pll_sdram_locked),
        .reset      (1'b0),
        .standby    (1'b0),
        .phasesel   (2'b0),
        .phasedir   (1'b0),
        .phasestep  (1'b0),
        .phaseloadreg(1'b0)
    );

    assign pll_locked = pll_video_locked & pll_sdram_locked;
`else
    assign pll_locked = pll_video_locked;
`endif

`ifdef VIDEO_480P_TESTPATTERN
`ifdef VIDEO_VGA640_TIMING
    wire clk_shift  = pll_clk[0];  // 125 MHz
    wire clk_pixel  = pll_clk[1];  // 25 MHz
`else
    wire clk_shift  = pll_clk[0];  // 135 MHz
    wire clk_pixel  = pll_clk[1];  // 27 MHz
`endif
    wire clk_sdram  = 1'b0;
    wire clk_sdref  = 1'b0;
`elsif VIDEO_720X480
    wire clk_shift  = pll_clk[0];        // 135 MHz
    wire clk_pixel  = pll_clk[1];        // 27 MHz
    wire clk_sdram  = pll_sdram_clk[0];  // 100 MHz
    wire clk_sdref  = pll_sdram_clk[1];  // 6.25 MHz — clkref for sdram.v (16:1)
`else
    wire clk_pixel  = pll_clk[0];  // 25 MHz
    wire clk_shift  = pll_clk[1];  // 125 MHz
    wire clk_sdram  = pll_clk[2];  // 100 MHz
    wire clk_sdref  = pll_clk[3];  // 6.25 MHz — clkref for sdram.v (16:1)
`endif

`ifndef VIDEO_480P_TESTPATTERN
`ifdef ENABLE_USB_HID
    wire [3:0] pll_usb_clk;
    wire       pll_usb_locked;

    ecp5pll #(
        .in_hz  (100000000),
        .out0_hz(48000000),
        .out1_hz(0),
        .out2_hz(0),
        .out3_hz(0)
    ) pll_usb_inst (
        .clk_i      (clk_sdram),
        .clk_o      (pll_usb_clk),
        .locked     (pll_usb_locked),
        .reset      (1'b0),
        .standby    (1'b0),
        .phasesel   (2'b0),
        .phasedir   (1'b0),
        .phasestep  (1'b0),
        .phaseloadreg(1'b0)
    );

    wire clk_usb = pll_usb_clk[0];  // 48 MHz for full-speed usb_hid_host
`endif
`endif

    // =========================================================================
    // Reset: hold reset until PLL locks, then release.
    // btn[0] is active-low power button — directly usable as manual reset.
    //
    // pll_locked is async to clk_pixel (PLL has its own lock-detect circuit).
    // btn[0] is a raw external button input. Both must be 2FF-synchronized
    // before being sampled into the clk_pixel domain — otherwise metastability
    // can momentarily glitch rst_cnt mid-edge → variable rst pulse length →
    // variable boot state. Diagnosed 2026-04-26 as a co-cause of POR
    // non-determinism.
    // =========================================================================
    reg [7:0] rst_cnt = 8'hFF;
    wire      rst = (rst_cnt != 0);

    reg [1:0] pll_lock_sync = 2'b00;
    reg [1:0] btn0_sync     = 2'b00;
    always_ff @(posedge clk_pixel) begin
        pll_lock_sync <= {pll_lock_sync[0], pll_locked};
        btn0_sync     <= {btn0_sync[0],     btn[0]};
    end
    wire pll_locked_safe = pll_lock_sync[1];
    wire btn0_safe       = btn0_sync[1];

    always_ff @(posedge clk_pixel) begin
        if (!pll_locked_safe || !btn0_safe)
            rst_cnt <= 8'hFF;
        else if (rst_cnt != 0)
            rst_cnt <= rst_cnt - 1;
    end

`ifdef VIDEO_480P_TESTPATTERN
    // =========================================================================
    // HDMI/DVI test pattern
    // =========================================================================
`ifdef VIDEO_VGA640_TIMING
    localparam int CEA_H_ACTIVE = 640;
    localparam int CEA_H_FRONT  = 16;
    localparam int CEA_H_SYNC   = 96;
    localparam int CEA_H_BACK   = 48;
    localparam int CEA_H_TOTAL  = CEA_H_ACTIVE + CEA_H_FRONT + CEA_H_SYNC + CEA_H_BACK;

    localparam int CEA_V_ACTIVE = 480;
    localparam int CEA_V_FRONT  = 10;
    localparam int CEA_V_SYNC   = 2;
    localparam int CEA_V_BACK   = 33;
    localparam int CEA_V_TOTAL  = CEA_V_ACTIVE + CEA_V_FRONT + CEA_V_SYNC + CEA_V_BACK;

    localparam int NOVA_X0 = 0;
    localparam int NOVA_X1 = 640;
`else
    localparam int CEA_H_ACTIVE = 720;
    localparam int CEA_H_FRONT  = 16;
    localparam int CEA_H_SYNC   = 62;
    localparam int CEA_H_BACK   = 60;
    localparam int CEA_H_TOTAL  = CEA_H_ACTIVE + CEA_H_FRONT + CEA_H_SYNC + CEA_H_BACK;

    localparam int CEA_V_ACTIVE = 480;
    localparam int CEA_V_FRONT  = 9;
    localparam int CEA_V_SYNC   = 6;
    localparam int CEA_V_BACK   = 30;
    localparam int CEA_V_TOTAL  = CEA_V_ACTIVE + CEA_V_FRONT + CEA_V_SYNC + CEA_V_BACK;

    localparam int NOVA_X0 = 40;
    localparam int NOVA_X1 = 680;
`endif

    logic [9:0] test_h;
    logic [9:0] test_v;
    logic [9:0] test_bar_x;
    logic       test_bar_dir;

    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            test_h       <= 10'd0;
            test_v       <= 10'd0;
            test_bar_x   <= 10'd40;
            test_bar_dir <= 1'b0;
        end else begin
            if (test_h == CEA_H_TOTAL - 1) begin
                test_h <= 10'd0;
                if (test_v == CEA_V_TOTAL - 1) begin
                    test_v <= 10'd0;
                    if (!test_bar_dir) begin
                        if (test_bar_x == NOVA_X1 - 17)
                            test_bar_dir <= 1'b1;
                        else
                            test_bar_x <= test_bar_x + 10'd1;
                    end else begin
                        if (test_bar_x == NOVA_X0)
                            test_bar_dir <= 1'b0;
                        else
                            test_bar_x <= test_bar_x - 10'd1;
                    end
                end else begin
                    test_v <= test_v + 10'd1;
                end
            end else begin
                test_h <= test_h + 10'd1;
            end
        end
    end

    wire test_active = (test_h < CEA_H_ACTIVE) && (test_v < CEA_V_ACTIVE);
    wire test_canvas = test_active && (test_h >= NOVA_X0) && (test_h < NOVA_X1);
    wire test_grid   = test_canvas && (((test_h - NOVA_X0) == 10'd0) ||
                                       ((test_h - NOVA_X0) == 10'd319) ||
                                       ((test_h - NOVA_X0) == 10'd639) ||
                                       (test_v == 10'd0) ||
                                       (test_v == 10'd239) ||
                                       (test_v == 10'd479));
    wire test_bar    = test_canvas &&
                       (test_h >= test_bar_x) && (test_h < test_bar_x + 10'd16) &&
                       (test_v >= 10'd208) && (test_v < 10'd272);

    logic [23:0] test_rgb24;
    always_comb begin
        if (!test_active)
            test_rgb24 = 24'h000000;
        else if (!test_canvas)
            test_rgb24 = 24'h103050;  // side border: visible but not eye-searing
        else if (test_bar)
            test_rgb24 = 24'hff4040;
        else if (test_grid)
            test_rgb24 = 24'hf0f0f0;
        else
            test_rgb24 = 24'h0010a0;
    end

    logic       clk_audio = 1'b0;
    logic [8:0] clk_audio_div = 9'd280;
    logic [1:0] clk_audio_phase = 2'd0;

    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            clk_audio       <= 1'b0;
            clk_audio_div   <= 9'd280;
            clk_audio_phase <= 2'd0;
        end else if (clk_audio_div == 9'd0) begin
            clk_audio <= ~clk_audio;
            clk_audio_div <= (clk_audio_phase == 2'd3) ? 9'd281 : 9'd280;
            clk_audio_phase <= clk_audio_phase + 2'd1;
        end else begin
            clk_audio_div <= clk_audio_div - 9'd1;
        end
    end

    logic [1:0][15:0] test_audio_sample_word;
    assign test_audio_sample_word = '0;

    wire [2:0] test_hdmi_tmds;
    wire       test_hdmi_tmds_clock;
    wire [9:0] test_hdmi_cx, test_hdmi_cy;
    wire [9:0] test_hdmi_frame_width, test_hdmi_frame_height;
    wire [9:0] test_hdmi_screen_width, test_hdmi_screen_height;

    hdmi #(
`ifdef VIDEO_VGA640_TIMING
        .VIDEO_ID_CODE(1),
`else
        .VIDEO_ID_CODE(2),
`endif
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
        .AUDIO_RATE(48000),
        .AUDIO_BIT_WIDTH(16),
        .VENDOR_NAME({"Nova", 32'd0}),
        .PRODUCT_DESCRIPTION({"NovaVM", 80'd0}),
        .SOURCE_DEVICE_INFORMATION(8'h09)
    ) test_hdmi_inst (
        .clk_pixel_x5(clk_shift),
        .clk_pixel   (clk_pixel),
        .clk_audio   (clk_audio),
        .reset       (rst),
        .rgb         (test_rgb24),
        .audio_sample_word(test_audio_sample_word),
        .tmds        (test_hdmi_tmds),
        .tmds_clock  (test_hdmi_tmds_clock),
        .cx          (test_hdmi_cx),
        .cy          (test_hdmi_cy),
        .frame_width (test_hdmi_frame_width),
        .frame_height(test_hdmi_frame_height),
        .screen_width(test_hdmi_screen_width),
        .screen_height(test_hdmi_screen_height)
    );

    assign gpdi_dp[0] = test_hdmi_tmds[0];
    assign gpdi_dp[1] = test_hdmi_tmds[1];
    assign gpdi_dp[2] = test_hdmi_tmds[2];
    assign gpdi_dp[3] = test_hdmi_tmds_clock;
`ifndef GPDI_P_ONLY
    assign gpdi_dn = ~gpdi_dp;
`endif

    reg [24:0] heartbeat = 0;
    always_ff @(posedge clk_pixel)
        heartbeat <= heartbeat + 1;

    assign leds[7]   = heartbeat[24];
    assign leds[6]   = pll_locked;
    assign leds[5]   = ~rst;
    assign leds[4:0] = btn[4:0];

    assign ftdi_rxd = 1'b1;
    assign wifi_gpio0 = 1'b1;
    assign wifi_rxd = 1'b1;
    assign esp_spi_miso = 1'bz;
    assign usb_fpga_bd_dp = 1'bz;
    assign usb_fpga_bd_dn = 1'bz;
    assign usb_fpga_pu_dp = 1'b0;
    assign usb_fpga_pu_dn = 1'b0;

    assign sdram_clk  = 1'b0;
    assign sdram_cke  = 1'b0;
    assign sdram_csn  = 1'b1;
    assign sdram_wen  = 1'b1;
    assign sdram_rasn = 1'b1;
    assign sdram_casn = 1'b1;
    assign sdram_ba   = 2'b00;
    assign sdram_dqm  = 2'b11;
    assign sdram_a    = 13'd0;
    assign sdram_d    = 16'bzzzzzzzzzzzzzzzz;

`ifdef SYNTHESIS
    GSR GSR_INST (.GSR(1'b1));
`endif

`else
    // =========================================================================
    // UART receiver — keyboard input via FTDI serial (115200 8N1)
    // =========================================================================
`ifdef VIDEO_720X480
    localparam int UART_CLK_HZ = 27000000;
`else
    localparam int UART_CLK_HZ = 25000000;
`endif

    wire [7:0] uart_data;
    wire       uart_valid;

    uart_rx #(
        .CLK_HZ(UART_CLK_HZ),
        .BAUD  (9600)
    ) uart_inst (
        .clk  (clk_pixel),
        .rst  (rst),
        .rx   (ftdi_txd),
        .data (uart_data),
        .valid(uart_valid)
    );

    // =========================================================================
    // USB HID host — US2 low/full-speed keyboard input.
    //
    // ULX3S USB pull-control pins are diode-steered: HIGH selects the 1.1k
    // device pull-up path; LOW selects the 15k host pull-down path. Keep both
    // LOW for host mode.
    // =========================================================================
    assign usb_fpga_pu_dp = 1'b0;
    assign usb_fpga_pu_dn = 1'b0;

`ifdef ENABLE_USB_HID
    logic [1:0] usb_rst_req_sync = 2'b11;
    logic [2:0] usb_reset_release = 3'b000;
    always_ff @(posedge clk_usb) begin
        usb_rst_req_sync <= {usb_rst_req_sync[0], rst || !pll_usb_locked};
        if (usb_rst_req_sync[1])
            usb_reset_release <= 3'b000;
        else
            usb_reset_release <= {usb_reset_release[1:0], 1'b1};
    end

    wire usb_reset_n = usb_reset_release[2];
    wire usb_key_valid;
    wire [7:0] usb_key_data;
    wire [1:0] usb_device_type;
    wire usb_report_seen;
    wire usb_connection_error;
    wire [7:0] usb_hid_status;
    wire [7:0] usb_hid_device_type;
    wire [7:0] usb_hid_last_scan;
    wire [7:0] usb_hid_last_ascii;
    wire [7:0] usb_hid_report_count;
    wire [7:0] usb_hid_key_count;
    wire [7:0] usb_hid_core_status;
    wire [63:0] usb_hid_regs;

    usb_hid_keyboard usb_keyboard (
        .clk_usb         (clk_usb),
        .usb_reset_n     (usb_reset_n),
        .clk_sys         (clk_pixel),
        .sys_rst         (rst),
        .usb_dm          (usb_fpga_bd_dn),
        .usb_dp          (usb_fpga_bd_dp),
        .key_valid       (usb_key_valid),
        .key_data        (usb_key_data),
        .device_type     (usb_device_type),
        .report_seen     (usb_report_seen),
        .connection_error(usb_connection_error),
        .dbg_status      (usb_hid_status),
        .dbg_device_type (usb_hid_device_type),
        .dbg_last_scan   (usb_hid_last_scan),
        .dbg_last_ascii  (usb_hid_last_ascii),
        .dbg_report_count(usb_hid_report_count),
        .dbg_key_count   (usb_hid_key_count),
        .dbg_core_status (usb_hid_core_status),
        .dbg_hid_regs    (usb_hid_regs)
    );
`else
    assign usb_fpga_bd_dp = 1'bz;
    assign usb_fpga_bd_dn = 1'bz;

    wire       usb_key_valid = 1'b0;
    wire [7:0] usb_key_data = 8'h00;
    wire [7:0] usb_hid_status = 8'h00;
    wire [7:0] usb_hid_device_type = 8'h00;
    wire [7:0] usb_hid_last_scan = 8'h00;
    wire [7:0] usb_hid_last_ascii = 8'h00;
    wire [7:0] usb_hid_report_count = 8'h00;
    wire [7:0] usb_hid_key_count = 8'h00;
    wire [7:0] usb_hid_core_status = 8'h00;
    wire [63:0] usb_hid_regs = 64'h0;
`endif

    // =========================================================================
    // Host debug bridge stream. NovaHost talks to the FPGA over SPI only; the
    // ESP UART pins are intentionally not a command fallback.
    // =========================================================================
    wire [7:0] dbg_rx_data;
    wire       dbg_rx_valid;
    wire       dbg_rx_ready;
    wire [7:0] dbg_tx_data;
    wire       dbg_tx_start;
    wire       dbg_tx_busy;
    assign wifi_rxd = 1'b1;

    // =========================================================================
    // Debug SPI — ESP32 shared-SPI byte transport
    // =========================================================================
    wire [7:0] dbg_spi_rx_data;
    wire       dbg_spi_rx_valid;
    wire       dbg_spi_rx_ready;
    wire       dbg_spi_tx_busy;
    wire       dbg_spi_miso;
    wire       dbg_spi_miso_oe;
    wire       dbg_spi_rx_overflow;
    wire       dbg_spi_tx_overflow;
    wire       dbg_spi_tx_underflow;
    wire       dbg_spi_selected_seen;

    debug_spi_slave #(
        .RX_ADDR_WIDTH(12),
        .TX_ADDR_WIDTH(9),
        .IDLE_BYTE(8'hA5)
    ) dbg_spi (
        .clk          (clk_pixel),
        .rst          (rst),
        .spi_sck      (esp_spi_sck),
        .spi_cs_n     (esp_spi_cs_n),
        .spi_mosi     (esp_spi_mosi),
        .spi_miso     (dbg_spi_miso),
        .spi_miso_oe  (dbg_spi_miso_oe),
        .rx_data      (dbg_spi_rx_data),
        .rx_valid     (dbg_spi_rx_valid),
        .rx_ready     (dbg_spi_rx_ready),
        .tx_data      (dbg_tx_data),
        .tx_start     (dbg_tx_start),
        .tx_busy      (dbg_spi_tx_busy),
        .rx_overflow  (dbg_spi_rx_overflow),
        .tx_overflow  (dbg_spi_tx_overflow),
        .tx_underflow (dbg_spi_tx_underflow),
        .selected_seen(dbg_spi_selected_seen)
    );

    assign esp_spi_miso = dbg_spi_miso_oe ? dbg_spi_miso : 1'bz;
    assign dbg_rx_data = dbg_spi_rx_data;
    assign dbg_rx_valid = dbg_spi_rx_valid;
    assign dbg_spi_rx_ready = dbg_rx_ready && dbg_spi_rx_valid;
    assign dbg_tx_busy = dbg_spi_tx_busy;

    // =========================================================================
    // Debug bridge — binary protocol between ESP32 SPI and core debug ports
    // =========================================================================
    wire        brg_peek_en;
    wire [15:0] brg_peek_addr;
    wire [7:0]  brg_peek_data;
    wire        brg_poke_en;
    wire [15:0] brg_poke_addr;
    wire [7:0]  brg_poke_data;
    wire        brg_nic_buf_we;
    wire        brg_nic_buf_re;
    wire        brg_nic_buf_sel;
    wire [7:0]  brg_nic_buf_addr;
    wire [7:0]  brg_nic_buf_data;
    wire [7:0]  brg_nic_buf_rdata;
    wire        brg_vmem_we;
    wire        brg_vmem_re;
    wire [2:0]  brg_vmem_space;
    wire [16:0] brg_vmem_addr;
    wire [7:0]  brg_vmem_data;
    wire [7:0]  brg_vmem_rdata;
    wire        brg_pause;
    wire        brg_rom_we;
    wire        brg_rom_idx;
    wire [13:0] brg_rom_addr;
    wire [7:0]  brg_rom_data;
    wire        brg_cpu_reset;
    wire        brg_system_reset;
    wire        brg_cpu_resume;
    wire [15:0] brg_cpu_pc;
    wire [7:0]  brg_cpu_a, brg_cpu_x, brg_cpu_y, brg_cpu_sp, brg_cpu_flags;
    wire [5:0]  brg_cpu_state;
    wire [7:0]  brg_cpu_ir;
    wire [15:0] brg_cpu_addr;
    wire [7:0]  brg_cpu_din, brg_cpu_dout;
    wire        brg_cpu_we, brg_cpu_rdy, brg_cpu_irq, brg_cpu_nmi;
    wire        brg_cpu_waiting, brg_cpu_stopped;
    wire        brg_key_valid;
    wire [7:0]  brg_key_data;
    wire        brg_key_ready;
    wire        brg_sdram_b_we;
    wire        brg_sdram_b_oe;
    wire [24:0] brg_sdram_b_addr;
    wire [7:0]  brg_sdram_b_din;
    wire [7:0]  brg_sdram_b_dout;
    wire        brg_sdram_b_done_toggle;
    wire [7:0]  brg_host_status;
    wire        brg_audio_pcm_we;
    wire [7:0]  brg_audio_pcm_data;
    wire        brg_audio_pcm_ready;
    wire        brg_wts_event_we;
    wire [7:0]  brg_wts_event_data;
    wire        brg_wts_event_ready;
    wire [15:0] host_audio_byte_space;
    wire [15:0] host_audio_underruns;

    // Host-service events — pulsed by core register banks on CPU command
    // writes. Bridge latches them and emits async event sequences to the ESP.
    wire        core_fio_event;
    wire        core_nic_event;

    debug_bridge dbg_bridge (
        .clk             (clk_pixel),
        .rst             (rst),
        .rx_data         (dbg_rx_data),
        .rx_valid        (dbg_rx_valid),
        .rx_ready        (dbg_rx_ready),
        .tx_data         (dbg_tx_data),
        .tx_start        (dbg_tx_start),
        .tx_busy         (dbg_tx_busy),
        .dbg_peek_en     (brg_peek_en),
        .dbg_peek_addr   (brg_peek_addr),
        .dbg_peek_data   (brg_peek_data),
        .dbg_poke_en     (brg_poke_en),
        .dbg_poke_addr   (brg_poke_addr),
        .dbg_poke_data   (brg_poke_data),
        .dbg_pause       (brg_pause),
        .dbg_nic_buf_we  (brg_nic_buf_we),
        .dbg_nic_buf_re  (brg_nic_buf_re),
        .dbg_nic_buf_sel (brg_nic_buf_sel),
        .dbg_nic_buf_addr(brg_nic_buf_addr),
        .dbg_nic_buf_data(brg_nic_buf_data),
        .dbg_nic_buf_rdata(brg_nic_buf_rdata),
        .dbg_vmem_we     (brg_vmem_we),
        .dbg_vmem_re     (brg_vmem_re),
        .dbg_vmem_space  (brg_vmem_space),
        .dbg_vmem_addr   (brg_vmem_addr),
        .dbg_vmem_data   (brg_vmem_data),
        .dbg_vmem_rdata  (brg_vmem_rdata),
        .dbg_rom_we      (brg_rom_we),
        .dbg_rom_idx     (brg_rom_idx),
        .dbg_rom_addr    (brg_rom_addr),
        .dbg_rom_data    (brg_rom_data),
        .dbg_cpu_reset   (brg_cpu_reset),
        .dbg_system_reset(brg_system_reset),
        .dbg_cpu_resume  (brg_cpu_resume),
        .dbg_cpu_pc      (brg_cpu_pc),
        .dbg_cpu_a       (brg_cpu_a),
        .dbg_cpu_x       (brg_cpu_x),
        .dbg_cpu_y       (brg_cpu_y),
        .dbg_cpu_sp      (brg_cpu_sp),
        .dbg_cpu_flags   (brg_cpu_flags),
        .dbg_cpu_state   (brg_cpu_state),
        .dbg_cpu_ir      (brg_cpu_ir),
        .dbg_cpu_addr    (brg_cpu_addr),
        .dbg_cpu_din     (brg_cpu_din),
        .dbg_cpu_dout    (brg_cpu_dout),
        .dbg_cpu_we      (brg_cpu_we),
        .dbg_cpu_rdy     (brg_cpu_rdy),
        .dbg_cpu_irq     (brg_cpu_irq),
        .dbg_cpu_nmi     (brg_cpu_nmi),
        .dbg_cpu_waiting (brg_cpu_waiting),
        .dbg_cpu_stopped (brg_cpu_stopped),
        .key_inject_valid(brg_key_valid),
        .key_inject_data (brg_key_data),
        .key_inject_ready(brg_key_ready),
        .sdram_b_we      (brg_sdram_b_we),
        .sdram_b_oe      (brg_sdram_b_oe),
        .sdram_b_addr    (brg_sdram_b_addr),
        .sdram_b_din     (brg_sdram_b_din),
        .sdram_b_dout    (brg_sdram_b_dout),
        .sdram_b_done_toggle(brg_sdram_b_done_toggle),
        .fio_event       (core_fio_event),
        .nic_event       (core_nic_event),
        .host_status     (brg_host_status),
        .audio_pcm_we    (brg_audio_pcm_we),
        .audio_pcm_data  (brg_audio_pcm_data),
        .audio_pcm_ready (brg_audio_pcm_ready),
        .audio_pcm_space (host_audio_byte_space),
        .audio_pcm_underruns(host_audio_underruns),
        .wts_event_we    (brg_wts_event_we),
        .wts_event_data  (brg_wts_event_data),
        .wts_event_ready (brg_wts_event_ready)
    );

    // Key input: NovaHost/debug injection wins, then direct USB keyboard,
    // then legacy FTDI UART keyboard input.
    wire       key_valid_mux = brg_key_valid ? 1'b1 :
                               usb_key_valid ? 1'b1 :
                               uart_valid;
    wire [7:0] key_data_mux  = brg_key_valid ? brg_key_data :
                               usb_key_valid ? usb_key_data :
                               uart_data;

    // =========================================================================
    // Board input — expose ULX3S pushbuttons and DIP switches to Nova software.
    // Register layout is active-high logical state:
    //   buttons bit0=PWR, bit1=FIRE1, bit2=FIRE2, bit3=UP,
    //           bit4=DOWN, bit5=LEFT, bit6=RIGHT
    //   switches bit0=SW1, bit1=SW2, bit2=SW3, bit3=SW4
    // =========================================================================
    localparam int BOARD_INPUT_COUNT = 11;
    localparam int BOARD_INPUT_DEBOUNCE_BITS = 20;

    wire [BOARD_INPUT_COUNT-1:0] board_input_raw = {sw[3:0], btn[6:1], ~btn[0]};
    logic [BOARD_INPUT_COUNT-1:0] board_input_meta = '0;
    logic [BOARD_INPUT_COUNT-1:0] board_input_sync = '0;
    logic [BOARD_INPUT_COUNT-1:0] board_input_stable = '0;
    logic [BOARD_INPUT_DEBOUNCE_BITS-1:0] board_input_debounce [0:BOARD_INPUT_COUNT-1];

    initial begin
        for (int i = 0; i < BOARD_INPUT_COUNT; i++)
            board_input_debounce[i] = '0;
    end

    always_ff @(posedge clk_pixel) begin
        if (!pll_locked_safe) begin
            board_input_meta <= '0;
            board_input_sync <= '0;
            board_input_stable <= '0;
            for (int i = 0; i < BOARD_INPUT_COUNT; i++)
                board_input_debounce[i] <= '0;
        end else begin
            board_input_meta <= board_input_raw;
            board_input_sync <= board_input_meta;

            for (int i = 0; i < BOARD_INPUT_COUNT; i++) begin
                if (board_input_sync[i] == board_input_stable[i]) begin
                    board_input_debounce[i] <= '0;
                end else if (&board_input_debounce[i]) begin
                    board_input_stable[i] <= board_input_sync[i];
                    board_input_debounce[i] <= '0;
                end else begin
                    board_input_debounce[i] <= board_input_debounce[i] + 1'b1;
                end
            end
        end
    end

    wire [7:0] board_buttons = {1'b0, board_input_stable[6:0]};
    wire [7:0] board_switches = {4'h0, board_input_stable[10:7]};

    // =========================================================================
    // Core system — 6502 + VGC + Blitter + SID
    // =========================================================================
    wire [3:0]  vid_r, vid_g, vid_b;
    wire        vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] sid_audio_l, sid_audio_r;

    top core (
        .clk        (clk_pixel),
        .rst        (rst),

        .key_valid  (key_valid_mux),
        .key_data   (key_data_mux),
        .key_ready  (brg_key_ready),

        .board_buttons(board_buttons),
        .board_switches(board_switches),
        .usb_hid_status(usb_hid_status),
        .usb_hid_device_type(usb_hid_device_type),
        .usb_hid_last_scan(usb_hid_last_scan),
        .usb_hid_last_ascii(usb_hid_last_ascii),
        .usb_hid_report_count(usb_hid_report_count),
        .usb_hid_key_count(usb_hid_key_count),
        .usb_hid_core_status(usb_hid_core_status),
        .usb_hid_regs(usb_hid_regs),

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

        // Debug interface — driven by debug bridge via ESP32 SPI
        .dbg_peek_en  (brg_peek_en),
        .dbg_peek_addr(brg_peek_addr),
        .dbg_peek_data(brg_peek_data),
        .dbg_poke_en  (brg_poke_en),
        .dbg_poke_addr(brg_poke_addr),
        .dbg_poke_data(brg_poke_data),
        .dbg_pause    (brg_pause),
        .dbg_nic_buf_we  (brg_nic_buf_we),
        .dbg_nic_buf_re  (brg_nic_buf_re),
        .dbg_nic_buf_sel (brg_nic_buf_sel),
        .dbg_nic_buf_addr(brg_nic_buf_addr),
        .dbg_nic_buf_data(brg_nic_buf_data),
        .dbg_nic_buf_rdata(brg_nic_buf_rdata),
        .dbg_vmem_we  (brg_vmem_we),
        .dbg_vmem_re  (brg_vmem_re),
        .dbg_vmem_space(brg_vmem_space),
        .dbg_vmem_addr(brg_vmem_addr),
        .dbg_vmem_data(brg_vmem_data),
        .dbg_vmem_rdata(brg_vmem_rdata),
        .dbg_rom_we   (brg_rom_we),
        .dbg_rom_idx  (brg_rom_idx),
        .dbg_rom_addr (brg_rom_addr),
        .dbg_rom_data (brg_rom_data),
        .dbg_cpu_reset(brg_cpu_reset),
        .dbg_system_reset(brg_system_reset),
        .dbg_cpu_resume(brg_cpu_resume),
        .brg_sdram_b_we  (1'b0),
        .brg_sdram_b_oe  (1'b0),
        .brg_sdram_b_addr(25'd0),
        .brg_sdram_b_din (8'd0),
        .host_wts_event_we(brg_wts_event_we),
        .host_wts_event_data(brg_wts_event_data),
        .host_wts_event_ready(brg_wts_event_ready),
        .dbg_cpu_pc   (brg_cpu_pc),
        .dbg_cpu_a    (brg_cpu_a),
        .dbg_cpu_x    (brg_cpu_x),
        .dbg_cpu_y    (brg_cpu_y),
        .dbg_cpu_sp   (brg_cpu_sp),
        .dbg_cpu_flags(brg_cpu_flags),
        .dbg_cpu_state(brg_cpu_state),
        .dbg_cpu_ir   (brg_cpu_ir),
        .dbg_cpu_addr (brg_cpu_addr),
        .dbg_cpu_din  (brg_cpu_din),
        .dbg_cpu_dout (brg_cpu_dout),
        .dbg_cpu_we   (brg_cpu_we),
        .dbg_cpu_rdy  (brg_cpu_rdy),
        .dbg_cpu_irq  (brg_cpu_irq),
        .dbg_cpu_nmi  (brg_cpu_nmi),
        .dbg_cpu_waiting(brg_cpu_waiting),
        .dbg_cpu_stopped(brg_cpu_stopped),

        // SDRAM port A — xram_sdram wrapper (XRAM BRAM replacement)
        .sdram_clk  (clk_sdram),
        .sdram_addrA(core_sdram_addrA),
        .sdram_dinA (core_sdram_dinA),
        .sdram_weA  (core_sdram_weA),
        .sdram_oeA  (core_sdram_oeA),
        .sdram_doutA(core_sdram_doutA),
        .sdram_doneA(core_sdram_doneA),

        // SDRAM port B — sid_curve_reader (f6581_curve lookup)
        .sdram_addrB(core_sdram_addrB),
        .sdram_dinB (core_sdram_dinB),
        .sdram_weB  (core_sdram_weB),
        .sdram_oeB  (core_sdram_oeB),
        .sdram_doutB(core_sdram_doutB),

        // SDRAM page-mode STREAM port — Tier-2 page-in engine (Task 11d).
        .sdram_stream_req  (core_stream_req),
        .sdram_stream_addr (core_stream_addr),
        .sdram_stream_words(core_stream_words),
        .sdram_stream_ready(core_stream_ready),
        .sdram_stream_dout (core_stream_dout),
        .sdram_stream_valid(core_stream_valid),
        .sdram_stream_busy (core_stream_busy),
        .sdram_stream_done (core_stream_done),

        .fio_event  (core_fio_event),
        .nic_event  (core_nic_event)
    );

    // SDRAM port A wires from core
    wire [24:0] core_sdram_addrA;
    wire [7:0]  core_sdram_dinA;
    wire        core_sdram_weA;
    wire        core_sdram_oeA;
    wire [7:0]  core_sdram_doutA;
    wire        core_sdram_doneA;

    // SDRAM port B wires from core
    wire [24:0] core_sdram_addrB;
    wire [7:0]  core_sdram_dinB;
    wire        core_sdram_weB;
    wire        core_sdram_oeB;
    wire [7:0]  core_sdram_doutB;
    wire        core_sdram_doneB;

    // SDRAM stream-port wires from core (Tier-2 page-in engine)
    wire        core_stream_req;
    wire [24:0] core_stream_addr;
    wire [13:0] core_stream_words;
    wire        core_stream_ready;
    wire [15:0] core_stream_dout;
    wire        core_stream_valid;
    wire        core_stream_busy;
    wire        core_stream_done;

    wire        dbg_sdram_b_we;
    wire        dbg_sdram_b_oe;
    wire [24:0] dbg_sdram_b_addr;
    wire [7:0]  dbg_sdram_b_din;
    wire        dbg_sdram_b_busy;

    debug_sdram_port_b_cdc dbg_sdram_port_b_cdc (
        .clk        (clk_pixel),
        .sdram_clk  (clk_sdram),
        .rst        (rst),
        .req_we     (brg_sdram_b_we),
        .req_oe     (brg_sdram_b_oe),
        .req_addr   (brg_sdram_b_addr),
        .req_din    (brg_sdram_b_din),
        .req_dout   (brg_sdram_b_dout),
        .done_toggle(brg_sdram_b_done_toggle),
        .sdram_we   (dbg_sdram_b_we),
        .sdram_oe   (dbg_sdram_b_oe),
        .sdram_addr (dbg_sdram_b_addr),
        .sdram_din  (dbg_sdram_b_din),
        .sdram_dout (core_sdram_doutB),
        .sdram_done (core_sdram_doneB),
        .sdram_busy (dbg_sdram_b_busy)
    );

    wire        dbg_sdram_b_active = dbg_sdram_b_busy | dbg_sdram_b_we | dbg_sdram_b_oe;
    wire [24:0] mux_sdram_addrB = dbg_sdram_b_active ? dbg_sdram_b_addr : core_sdram_addrB;
    wire [7:0]  mux_sdram_dinB  = dbg_sdram_b_active ? dbg_sdram_b_din  : core_sdram_dinB;
    wire        mux_sdram_weB   = dbg_sdram_b_active ? dbg_sdram_b_we   : core_sdram_weB;
    wire        mux_sdram_oeB   = dbg_sdram_b_active ? dbg_sdram_b_oe   : core_sdram_oeB;

    // =========================================================================
    // HDMI output — true HDMI video + 16-bit stereo PCM over GPDI.
    // =========================================================================

    // Pad 4-bit color to 8-bit
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};
    wire [23:0] hdmi_rgb24 = vid_de ? rgb24 : 24'h000000;

    // hdl-util/hdmi samples audio on a separate sample-rate clock. Generate
    // 48 kHz from the 27 MHz CEA pixel clock by alternating 281/282-cycle
    // half periods: 27 MHz / (2 * 281.25) = 48 kHz.
    logic       clk_audio = 1'b0;
    logic [8:0] clk_audio_div = 9'd280;
    logic [1:0] clk_audio_phase = 2'd0;

    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            clk_audio       <= 1'b0;
            clk_audio_div   <= 9'd280;
            clk_audio_phase <= 2'd0;
        end else if (clk_audio_div == 9'd0) begin
            clk_audio <= ~clk_audio;
            clk_audio_div <= (clk_audio_phase == 2'd3) ? 9'd281 : 9'd280;
            clk_audio_phase <= clk_audio_phase + 2'd1;
        end else begin
            clk_audio_div <= clk_audio_div - 9'd1;
        end
    end

    wire audio_sample_strobe = !rst && !clk_audio && (clk_audio_div == 9'd1);

    logic [1:0][15:0] sid_audio_sample_word;
    logic [1:0][15:0] host_audio_sample_word;
    logic [1:0][15:0] hdmi_audio_sample_word;
    wire              host_audio_sample_valid;

    sid_hdmi_audio sid_hdmi_audio_inst (
        .clk              (clk_pixel),
        .rst              (rst),
        .sample_en        (audio_sample_strobe),
        .sid_audio_l      (sid_audio_l),
        .sid_audio_r      (sid_audio_r),
        .audio_sample_word(sid_audio_sample_word)
    );

    audio_pcm_fifo #(
        .FRAME_ADDR_WIDTH(13)
    ) host_audio_fifo (
        .clk           (clk_pixel),
        .rst           (rst),
        .byte_we       (brg_audio_pcm_we),
        .byte_data     (brg_audio_pcm_data),
        .byte_ready    (brg_audio_pcm_ready),
        .sample_en     (audio_sample_strobe),
        .sample_word   (host_audio_sample_word),
        .sample_valid  (host_audio_sample_valid),
        .byte_space    (host_audio_byte_space),
        .underrun_count(host_audio_underruns)
    );

    function automatic logic [15:0] mix_pcm16(
        input logic [15:0] a,
        input logic [15:0] b
    );
        logic signed [15:0] sa;
        logic signed [15:0] sb;
        logic signed [16:0] sum;
        begin
            sa = a;
            sb = b;
            sum = {sa[15], sa} + {sb[15], sb};
            if (sum > 17'sd32767)
                mix_pcm16 = 16'h7fff;
            else if (sum < -17'sd32768)
                mix_pcm16 = 16'h8000;
            else
                mix_pcm16 = sum[15:0];
        end
    endfunction

    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            hdmi_audio_sample_word <= '0;
        end else begin
            hdmi_audio_sample_word[0] <= mix_pcm16(
                sid_audio_sample_word[0],
                host_audio_sample_valid ? host_audio_sample_word[0] : 16'd0);
            hdmi_audio_sample_word[1] <= mix_pcm16(
                sid_audio_sample_word[1],
                host_audio_sample_valid ? host_audio_sample_word[1] : 16'd0);
        end
    end

    wire [7:0] music_leds;
    wire       music_leds_active;

    audio_led_meter music_led_meter (
        .clk       (clk_pixel),
        .rst       (rst),
        .sample_en (audio_sample_strobe),
        .sample_l  (hdmi_audio_sample_word[0]),
        .sample_r  (hdmi_audio_sample_word[1]),
        .leds      (music_leds),
        .active    (music_leds_active)
    );

    wire [2:0] hdmi_tmds;
    wire       hdmi_tmds_clock;
    wire [9:0] hdmi_cx, hdmi_cy;
    wire [9:0] hdmi_frame_width, hdmi_frame_height;
    wire [9:0] hdmi_screen_width, hdmi_screen_height;

`ifdef VIDEO_720X480
    // VGC scanout registers RGB/DE through a three-pixel output pipeline before
    // fpga_top presents it to hdl-util/hdmi. Start the HDMI frame three pixels
    // early so HDMI active pixel 0 samples VGC active pixel 0 instead of the
    // black pre-active pipeline tail.
    localparam int HDMI_VGC_LATENCY_PIXELS = 3;
    localparam int HDMI_START_X = 858 - HDMI_VGC_LATENCY_PIXELS;
    localparam int HDMI_START_Y = 525 - 1;
`else
    localparam int HDMI_START_X = 0;
    localparam int HDMI_START_Y = 0;
`endif

    hdmi #(
`ifdef VIDEO_720X480
        .VIDEO_ID_CODE(2),
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
`else
        .VIDEO_ID_CODE(1),
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
`endif
        .START_X(HDMI_START_X),
        .START_Y(HDMI_START_Y),
        .AUDIO_RATE(48000),
        .AUDIO_BIT_WIDTH(16),
        .VENDOR_NAME({"Nova", 32'd0}),
        .PRODUCT_DESCRIPTION({"NovaVM", 80'd0}),
        .SOURCE_DEVICE_INFORMATION(8'h09)
    ) hdmi_inst (
        .clk_pixel_x5(clk_shift),
        .clk_pixel   (clk_pixel),
        .clk_audio   (clk_audio),
        .reset       (rst),
        .rgb         (hdmi_rgb24),
        .audio_sample_word(hdmi_audio_sample_word),
        .tmds        (hdmi_tmds),
        .tmds_clock  (hdmi_tmds_clock),
        .cx          (hdmi_cx),
        .cy          (hdmi_cy),
        .frame_width (hdmi_frame_width),
        .frame_height(hdmi_frame_height),
        .screen_width(hdmi_screen_width),
        .screen_height(hdmi_screen_height)
    );

    assign gpdi_dp[0] = hdmi_tmds[0];      // Blue
    assign gpdi_dp[1] = hdmi_tmds[1];      // Green
    assign gpdi_dp[2] = hdmi_tmds[2];      // Red
    assign gpdi_dp[3] = hdmi_tmds_clock;   // Clock
`ifndef GPDI_P_ONLY
    assign gpdi_dn = ~gpdi_dp;
`endif

    // =========================================================================
    // SDRAM bring-up — instantiated idle for Phase 2.5 Step 1
    //
    // Port A and port B are held idle; the controller will auto-refresh on
    // otherwise-unused cycles. Data-bus tri-state follows sd_we_out: when
    // the controller is writing it drives sdram_d, otherwise it floats so
    // the chip can drive reads back.
    //
    // clk (100 MHz) vs clkref (6.25 MHz dedicated PLL output) is 16:1. Each
    // port gets its own full q=0..7 state machine cycle per clkref HALF-
    // period: clkref HIGH = port A slot, clkref LOW = port B slot. The
    // MiST controller only latches doutA at q=7 when clkref is still HIGH,
    // so port A's full transaction must fit in one half-period = 8 SDRAM
    // clocks. Tighter ratios broke in sim: 4:1 stuck in reset, 8:1 never
    // latched reads.
    // =========================================================================
    wire        sd_we_out;
    wire [15:0] sd_data_out_ctrl;
    wire [15:0] sd_data_in_ctrl = sdram_d;

    // Direct SDRAM clock output — registered by the IOBUF on the pad.
    assign sdram_clk = clk_sdram;
    // Chip enable is tied high while the controller is running (init pulse
    // is what takes the chip from reset to idle, not CKE).
    assign sdram_cke = 1'b1;

    assign sdram_d = sd_we_out ? sd_data_out_ctrl : 16'bzzzzzzzzzzzzzzzz;

    sdram sdram_inst (
        .sd_data_in (sd_data_in_ctrl),
        .sd_data_out(sd_data_out_ctrl),
        .sd_addr    (sdram_a),
        .sd_dqm     (sdram_dqm),
        .sd_ba      (sdram_ba),
        .sd_cs      (sdram_csn),
        .sd_we      (sdram_wen),
        .sd_ras     (sdram_rasn),
        .sd_cas     (sdram_casn),

        .init   (rst),
        .clk    (clk_sdram),
        .clkref (clk_sdref),
        .we_out (sd_we_out),

        // Port A — CPU / XMC via xram_sdram wrapper inside top core
        .addrA(core_sdram_addrA), .weA(core_sdram_weA),
        .dinA(core_sdram_dinA),  .oeA(core_sdram_oeA),
        .doutA(core_sdram_doutA), .doneA(core_sdram_doneA),
        // Port B — debug bridge has priority over the SID filter curve reader.
        .addrB(mux_sdram_addrB), .weB(mux_sdram_weB),
        .dinB(mux_sdram_dinB),   .oeB(mux_sdram_oeB),
        .doutB(core_sdram_doutB), .doneB(core_sdram_doneB),
        // Stream port — Tier-2 page-in engine (page_in_ctrl/page_dma, Task 11d)
        .stream_req  (core_stream_req),
        .stream_addr (core_stream_addr),
        .stream_words(core_stream_words),
        .stream_ready(core_stream_ready),
        .stream_dout (core_stream_dout),
        .stream_valid(core_stream_valid),
        .stream_busy (core_stream_busy),
        .stream_done (core_stream_done)

    );

    // =========================================================================
    // LEDs — board/runtime status. btn[6] toggles between the default
    // user panel and an operator panel with lower-level board/debug signals.
    //
    // User panel:
    //   7 heartbeat, 6 WiFi off/configured flash/connected solid,
    //   5 NovaHost ready/degraded flash, 4 SD mounted,
    //   3 FIO drive LED, 2 CPU running, 1 CPU paused, 0 NovaHost seen.
    // Operator panel:
    //   7 heartbeat, 6 PLL locked, 5 top reset released, 4 CPU reset released,
    //   3 system reset asserted, 2 ESP debug SPI selected, 1 FTDI UART seen,
    //   0 CPU paused.
    // =========================================================================
    reg [23:0] heartbeat = 0;  // POR init for clean LED phase from boot
    always_ff @(posedge clk_pixel)
        heartbeat <= heartbeat + 1;

    reg [1:0]  btn6_sync = 2'b00;
    reg        btn6_stable = 1'b0;
    reg [19:0] btn6_debounce = 20'd0;
    reg        led_operator_mode = 1'b0;
    reg        ftdi_uart_ever_received = 1'b0;

    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            btn6_sync <= 2'b00;
            btn6_stable <= 1'b0;
            btn6_debounce <= 20'd0;
            led_operator_mode <= 1'b0;
            ftdi_uart_ever_received <= 1'b0;
        end else begin
            btn6_sync <= {btn6_sync[0], btn[6]};
            if (btn6_sync[1] == btn6_stable) begin
                btn6_debounce <= 20'd0;
            end else if (&btn6_debounce) begin
                btn6_stable <= btn6_sync[1];
                btn6_debounce <= 20'd0;
                if (!btn6_stable && btn6_sync[1])
                    led_operator_mode <= ~led_operator_mode;
            end else begin
                btn6_debounce <= btn6_debounce + 20'd1;
            end

            if (uart_valid)
                ftdi_uart_ever_received <= 1'b1;
        end
    end

    wire status_flash = heartbeat[23];
    wire fio_led = brg_host_status[2] ? status_flash : brg_host_status[1];
    wire boot_led = brg_host_status[5] ? status_flash : brg_host_status[4];
    wire wifi_led = brg_host_status[0] ? 1'b1 :
                    (brg_host_status[6] ? status_flash : 1'b0);
    wire cpu_running_led = ~rst & ~brg_cpu_reset & ~brg_system_reset & ~brg_pause;

    wire [7:0] user_leds = {
        heartbeat[23],
        wifi_led,
        boot_led,
        brg_host_status[3],
        fio_led,
        cpu_running_led,
        brg_pause,
        brg_host_status[7]
    };

    wire [7:0] operator_leds = {
        heartbeat[23],
        pll_locked,
        ~rst,
        ~brg_cpu_reset,
        brg_system_reset,
        dbg_spi_selected_seen,
        ftdi_uart_ever_received,
        brg_pause
    };

    assign leds = led_operator_mode ? operator_leds :
                  music_leds_active ? music_leds :
                                      user_leds;

    // =========================================================================
    // FTDI TX — unused for now, drive idle high
    // =========================================================================
    assign ftdi_rxd = 1'b1;

    // Keep ESP32 out of download mode and drive debug LED
    assign wifi_gpio0 = 1'b1;

    // ECP5 POR — GSR(.GSR(1'b1)) is config-time-only (.GSR is active-LOW per
    // nextpnr ecp5/pack.cc; tied HIGH = inactive at runtime). After flatten,
    // yosys's lattice_gsr pass resolves every TRELLIS_FF to GSR("ENABLED"),
    // so every FF lands at its REGSET state at config-done — deterministic
    // POR, zero fabric cost, no LSR fanout. Requires `synth_ecp5` WITHOUT
    // `-noflatten` so the pass sees one module containing this primitive.
    // Verify: grep -c 'GSR("ENABLED")' build/post_synth.v == total FF count.
    //
    // SGSR(.GSR(~rst), .CLK(clk_pixel)) was tried 2026-04-27 — feedback loop:
    // rst deasserts → ~rst HIGH → SGSR fires → resets rst_cnt → rst back HIGH
    // → forever. Signature: LED6 (pll_locked) on, LED5 (~rst) off, LED7
    // (heartbeat) dead. Don't drive runtime reset through GSR.
    //
    // SYNTHESIS-only: verilator has no GSR blackbox (yosys cells_bb_ecp5.v).
    // Reference: Lattice "How to Use GSR/PUR/TSALL" technote.
`ifdef SYNTHESIS
    GSR GSR_INST (.GSR(1'b1));
`endif

`endif

endmodule
