// FPGA top-level for ULX3S (ECP5-85F)
// Wraps the simulation 'top' module with board-level I/O:
//   - PLL: 25 MHz osc -> 25 MHz pixel clock + 125 MHz TMDS shift clock
//     (or 720x480p: 27 MHz pixel + 135 MHz TMDS shift)
//   - HDMI output via GPDI (vga2dvid + fake differential DDR)
//   - UART keyboard input via FTDI serial
//   - 4-bit R-2R DAC audio output
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

    // UART via FTDI
    input  logic        ftdi_txd,     // FPGA receives from FTDI
    output logic        ftdi_rxd,     // FPGA transmits to FTDI

    // ESP32 control and debug UART
    output logic        wifi_gpio0,   // keep ESP32 out of download mode
    input  logic        wifi_txd,     // ESP32 TX -> FPGA RX (debug commands)
    output logic        wifi_rxd,     // FPGA TX -> ESP32 RX (debug responses)

    // Audio DAC (4-bit R-2R)
    output logic [3:0]  audio_l,
    output logic [3:0]  audio_r,

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
    wire test_hsync_pulse = (test_h >= CEA_H_ACTIVE + CEA_H_FRONT) &&
                            (test_h <  CEA_H_ACTIVE + CEA_H_FRONT + CEA_H_SYNC);
    wire test_vsync_pulse = (test_v >= CEA_V_ACTIVE + CEA_V_FRONT) &&
                            (test_v <  CEA_V_ACTIVE + CEA_V_FRONT + CEA_V_SYNC);
`ifdef VIDEO_VGA640_TIMING
    // Match the known-good ULX3S reference project: active-low H, active-high V.
    wire test_hsync  = ~test_hsync_pulse;
    wire test_vsync  =  test_vsync_pulse;
`else
    // CEA 720x480p DTDs use negative H/V sync polarity: idle high, pulse low.
    wire test_hsync  = ~test_hsync_pulse;
    wire test_vsync  = ~test_vsync_pulse;
`endif
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

    wire [1:0] tmds_red, tmds_green, tmds_blue, tmds_clock;
`ifdef VIDEO_REF_HDMI
    hdmi_device_ref #(
        .DDR_ENABLED(1)
    ) hdmi_ref_inst (
        .pclk          (clk_pixel),
        .tmds_clk      (clk_shift),
        .in_vga_red    (test_rgb24[23:16]),
        .in_vga_green  (test_rgb24[15:8]),
        .in_vga_blue   (test_rgb24[7:0]),
        .in_vga_blank  (~test_active),
        .in_vga_vsync  (test_vsync),
        .in_vga_hsync  (test_hsync),
        .out_tmds_red  (tmds_red),
        .out_tmds_green(tmds_green),
        .out_tmds_blue (tmds_blue),
        .out_tmds_clk  (tmds_clock)
    );

    ODDRX1F ref_ddr_clock (
        .D0(tmds_clock[0]),
        .D1(tmds_clock[1]),
        .Q(gpdi_dp[3]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
    ODDRX1F ref_ddr_red (
        .D0(tmds_red[0]),
        .D1(tmds_red[1]),
        .Q(gpdi_dp[2]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
    ODDRX1F ref_ddr_green (
        .D0(tmds_green[0]),
        .D1(tmds_green[1]),
        .Q(gpdi_dp[1]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
    ODDRX1F ref_ddr_blue (
        .D0(tmds_blue[0]),
        .D1(tmds_blue[1]),
        .Q(gpdi_dp[0]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );

`ifndef GPDI_P_ONLY
    ODDRX1F ref_ddr_clock_n (
        .D0(~tmds_clock[0]),
        .D1(~tmds_clock[1]),
        .Q(gpdi_dn[3]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
    ODDRX1F ref_ddr_red_n (
        .D0(~tmds_red[0]),
        .D1(~tmds_red[1]),
        .Q(gpdi_dn[2]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
    ODDRX1F ref_ddr_green_n (
        .D0(~tmds_green[0]),
        .D1(~tmds_green[1]),
        .Q(gpdi_dn[1]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
    ODDRX1F ref_ddr_blue_n (
        .D0(~tmds_blue[0]),
        .D1(~tmds_blue[1]),
        .Q(gpdi_dn[0]),
        .SCLK(clk_shift),
        .RST(1'b0)
    );
`endif
`else
`ifdef GPDI_P_ONLY
    wire [3:0] gpdi_dn_unused;
`endif

    vga2dvid vga2dvid_inst (
        .clk_pixel (clk_pixel),
        .clk_shift (clk_shift),
        .in_color  (test_rgb24),
        .in_blank  (~test_active),
        .in_hsync  (test_hsync),
        .in_vsync  (test_vsync),
        .resetn    (~rst),
        .out_red   (tmds_red),
        .out_green (tmds_green),
        .out_blue  (tmds_blue),
        .out_clock (tmds_clock)
    );

    fake_differential #(
        .C_ddr(1),
`ifdef GPDI_P_ONLY
        .C_drive_n(0)
`else
        .C_drive_n(1)
`endif
    ) fake_diff_inst (
        .clk_shift(clk_shift),
        .in_clock (tmds_clock),
        .in_red   (tmds_red),
        .in_green (tmds_green),
        .in_blue  (tmds_blue),
        .out_p    (gpdi_dp),
`ifdef GPDI_P_ONLY
        .out_n    (gpdi_dn_unused)
`else
        .out_n    (gpdi_dn)
`endif
    );
`endif

    reg [24:0] heartbeat = 0;
    always_ff @(posedge clk_pixel)
        heartbeat <= heartbeat + 1;

    assign leds[7]   = heartbeat[24];
    assign leds[6]   = pll_locked;
    assign leds[5]   = ~rst;
    assign leds[4:0] = btn[4:0];

    assign audio_l = 4'h8;
    assign audio_r = 4'h8;

    assign ftdi_rxd = 1'b1;
    assign wifi_gpio0 = 1'b1;
    assign wifi_rxd = 1'b1;

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
    // Debug UART — ESP32 WiFi serial (115200 8N1)
    // =========================================================================
    wire [7:0] dbg_rx_data;
    wire       dbg_rx_valid;
    wire       dbg_rx_ready;
    wire [7:0] dbg_tx_data;
    wire       dbg_tx_start;
    wire       dbg_tx_busy;
    wire       dbg_tx_out;
    wire [7:0] dbg_uart_rx_data;
    wire       dbg_uart_rx_valid;
    wire       dbg_uart_rx_ready;
    wire       dbg_uart_rx_overrun;
    wire       dbg_uart_rx_frame_error;
    wire       dbg_uart_tx_ready;
    wire       dbg_uart_tx_busy;
    wire       dbg_uart_fifo_overflow;

    // Debug UART runs at 115200. RX is a Nova-owned ready/valid UART with
    // explicit framing/overrun reporting, followed by a narrow byte FIFO.
    // One Nova protocol block is at most 256 payload bytes plus a 5-byte
    // header, so a 512-byte FIFO covers a full packet with headroom without
    // dragging in a generic AXI-stream FIFO.
    debug_uart_rx #(
        .CLK_HZ(UART_CLK_HZ),
        .BAUD  (115200)
    ) dbg_uart_rx (
        .clk           (clk_pixel),
        .rst           (rst),
        .rx            (wifi_txd),
        .data          (dbg_uart_rx_data),
        .valid         (dbg_uart_rx_valid),
        .ready         (dbg_uart_rx_ready),
        .busy          (),
        .overrun_error (dbg_uart_rx_overrun),
        .frame_error   (dbg_uart_rx_frame_error)
    );

    debug_byte_fifo #(
        .ADDR_WIDTH(9)
    ) dbg_uart_rx_fifo (
        .clk      (clk_pixel),
        .rst      (rst),
        .s_data   (dbg_uart_rx_data),
        .s_valid  (dbg_uart_rx_valid),
        .s_ready  (dbg_uart_rx_ready),
        .m_data   (dbg_rx_data),
        .m_valid  (dbg_rx_valid),
        .m_ready  (dbg_rx_ready),
        .overflow (dbg_uart_fifo_overflow),
        .fill     ()
    );

    debug_uart_tx #(
        .CLK_HZ(UART_CLK_HZ),
        .BAUD  (115200)
    ) dbg_uart_tx (
        .clk   (clk_pixel),
        .rst   (rst),
        .data  (dbg_tx_data),
        .valid (dbg_tx_start),
        .ready (dbg_uart_tx_ready),
        .tx    (dbg_tx_out),
        .busy  (dbg_uart_tx_busy)
    );

    assign dbg_tx_busy = dbg_uart_tx_busy || !dbg_uart_tx_ready;
    assign wifi_rxd = dbg_tx_out;

    // =========================================================================
    // Debug bridge — binary protocol between ESP32 UART and core debug ports
    // =========================================================================
    wire        brg_peek_en;
    wire [15:0] brg_peek_addr;
    wire [7:0]  brg_peek_data;
    wire        brg_poke_en;
    wire [15:0] brg_poke_addr;
    wire [7:0]  brg_poke_data;
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
    wire        brg_sdram_b_we;
    wire        brg_sdram_b_oe;
    wire [24:0] brg_sdram_b_addr;
    wire [7:0]  brg_sdram_b_din;
    wire [7:0]  brg_sdram_b_dout;
    wire        brg_sdram_b_done_toggle;
    wire [7:0]  brg_host_status;

    // File I/O event — pulsed by core's fio.sv on CPU write to $B9A0.
    // Bridge latches it and emits an async EVENT_FIO sequence to the ESP.
    wire        core_fio_event;

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
        .sdram_b_we      (brg_sdram_b_we),
        .sdram_b_oe      (brg_sdram_b_oe),
        .sdram_b_addr    (brg_sdram_b_addr),
        .sdram_b_din     (brg_sdram_b_din),
        .sdram_b_dout    (brg_sdram_b_dout),
        .sdram_b_done_toggle(brg_sdram_b_done_toggle),
        .fio_event       (core_fio_event),
        .host_status     (brg_host_status)
    );

    // Key input: debug bridge overrides UART keyboard
    wire       key_valid_mux = brg_key_valid ? 1'b1    : uart_valid;
    wire [7:0] key_data_mux  = brg_key_valid ? brg_key_data : uart_data;

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

        // Debug interface — driven by debug bridge via ESP32 UART
        .dbg_peek_en  (brg_peek_en),
        .dbg_peek_addr(brg_peek_addr),
        .dbg_peek_data(brg_peek_data),
        .dbg_poke_en  (brg_poke_en),
        .dbg_poke_addr(brg_poke_addr),
        .dbg_poke_data(brg_poke_data),
        .dbg_pause    (brg_pause),
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

        .fio_event  (core_fio_event)
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
    // HDMI output — VGA signals through TMDS encoder to GPDI
    // =========================================================================

    // Pad 4-bit color to 8-bit
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};

    // VGA timing — use VGC's own sync/DE signals (pipeline-aligned with pixel data)
    // VGC outputs active-low sync. Preserve the established 640x480 polarity
    // behavior, but emit negative-polarity CEA-style sync bits in 720x480 mode.
`ifdef VIDEO_720X480
    wire hdmi_hsync = vid_hsync;
    wire hdmi_vsync = vid_vsync;
`else
    wire hdmi_hsync = ~vid_hsync;
    wire hdmi_vsync = ~vid_vsync;
`endif
    wire [1:0] tmds_red, tmds_green, tmds_blue, tmds_clock;
`ifdef GPDI_P_ONLY
    wire [3:0] gpdi_dn_unused;
`endif

    vga2dvid vga2dvid_inst (
        .clk_pixel (clk_pixel),
        .clk_shift (clk_shift),
        .in_color  (rgb24),
        .in_blank  (~vid_de),
        .in_hsync  (hdmi_hsync),
        .in_vsync  (hdmi_vsync),
        .resetn    (~rst),
        .out_red   (tmds_red),
        .out_green (tmds_green),
        .out_blue  (tmds_blue),
        .out_clock (tmds_clock)
    );

    // DDR output via fake_differential (drives both P and N pins)
    fake_differential #(
        .C_ddr(1),
`ifdef GPDI_P_ONLY
        .C_drive_n(0)
`else
        .C_drive_n(1)
`endif
    ) fake_diff_inst (
        .clk_shift(clk_shift),
        .in_clock (tmds_clock),
        .in_red   (tmds_red),
        .in_green (tmds_green),
        .in_blue  (tmds_blue),
        .out_p    (gpdi_dp),
`ifdef GPDI_P_ONLY
        .out_n    (gpdi_dn_unused)
`else
        .out_n    (gpdi_dn)
`endif
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
        .doutB(core_sdram_doutB), .doneB(core_sdram_doneB)
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
    //   3 system reset asserted, 2 ESP debug UART seen, 1 FTDI UART seen,
    //   0 CPU paused.
    // =========================================================================
    reg [23:0] heartbeat = 0;  // POR init for clean LED phase from boot
    always_ff @(posedge clk_pixel)
        heartbeat <= heartbeat + 1;

    reg [1:0]  btn6_sync = 2'b00;
    reg        btn6_stable = 1'b0;
    reg [19:0] btn6_debounce = 20'd0;
    reg        led_operator_mode = 1'b0;
    reg        dbg_uart_ever_received = 1'b0;
    reg        ftdi_uart_ever_received = 1'b0;

    always_ff @(posedge clk_pixel) begin
        if (rst) begin
            btn6_sync <= 2'b00;
            btn6_stable <= 1'b0;
            btn6_debounce <= 20'd0;
            led_operator_mode <= 1'b0;
            dbg_uart_ever_received <= 1'b0;
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

            if (dbg_rx_valid)
                dbg_uart_ever_received <= 1'b1;
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
        dbg_uart_ever_received,
        ftdi_uart_ever_received,
        brg_pause
    };

    assign leds = led_operator_mode ? operator_leds : user_leds;

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
