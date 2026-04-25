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
    // PLL: 25 MHz -> 25 MHz (pixel) + 125 MHz (TMDS shift, 5x DDR)
    // =========================================================================
    wire [3:0] pll_clk;
    wire       pll_locked;

    ecp5pll #(
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

    wire clk_pixel  = pll_clk[0];  // 25 MHz
    wire clk_shift  = pll_clk[1];  // 125 MHz
    wire clk_sdram  = pll_clk[2];  // 100 MHz
    wire clk_sdref  = pll_clk[3];  // 6.25 MHz — clkref for sdram.v (16:1)

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
    // Debug UART — ESP32 WiFi serial (115200 8N1)
    // =========================================================================
    wire [7:0] dbg_rx_data;
    wire       dbg_rx_valid;
    wire [7:0] dbg_tx_data;
    wire       dbg_tx_start;
    wire       dbg_tx_busy;
    wire       dbg_tx_out;

    // Debug UART runs at 115200 — known-reliable on flying wires between
    // ESP32 and FPGA. 3.125 Mbaud attempted and failed on this wiring; can
    // revisit with proper signal conditioning later.
    uart_rx #(
        .CLK_HZ(25000000),
        .BAUD  (115200)
    ) dbg_uart_rx (
        .clk  (clk_pixel),
        .rst  (rst),
        .rx   (wifi_txd),
        .data (dbg_rx_data),
        .valid(dbg_rx_valid)
    );

    uart_tx #(
        .CLK_HZ(25000000),
        .BAUD  (115200)
    ) dbg_uart_tx (
        .clk  (clk_pixel),
        .rst  (rst),
        .data (dbg_tx_data),
        .start(dbg_tx_start),
        .tx   (dbg_tx_out),
        .busy (dbg_tx_busy)
    );

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
    wire        brg_pause;
    wire        brg_rom_we;
    wire        brg_rom_idx;
    wire [13:0] brg_rom_addr;
    wire [7:0]  brg_rom_data;
    wire        brg_cpu_reset;
    wire [15:0] brg_cpu_pc;
    wire [7:0]  brg_cpu_a, brg_cpu_x, brg_cpu_y, brg_cpu_sp, brg_cpu_flags;
    wire        brg_key_valid;
    wire [7:0]  brg_key_data;
    wire        brg_sdram_b_we;
    wire [24:0] brg_sdram_b_addr;
    wire [7:0]  brg_sdram_b_din;

    // File I/O event — pulsed by core's fio.sv on CPU write to $B9A0.
    // Bridge latches it and emits an async EVENT_FIO sequence to the ESP.
    wire        core_fio_event;

    debug_bridge dbg_bridge (
        .clk             (clk_pixel),
        .rst             (rst),
        .rx_data         (dbg_rx_data),
        .rx_valid        (dbg_rx_valid),
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
        .dbg_rom_we      (brg_rom_we),
        .dbg_rom_idx     (brg_rom_idx),
        .dbg_rom_addr    (brg_rom_addr),
        .dbg_rom_data    (brg_rom_data),
        .dbg_cpu_reset   (brg_cpu_reset),
        .dbg_cpu_pc      (brg_cpu_pc),
        .dbg_cpu_a       (brg_cpu_a),
        .dbg_cpu_x       (brg_cpu_x),
        .dbg_cpu_y       (brg_cpu_y),
        .dbg_cpu_sp      (brg_cpu_sp),
        .dbg_cpu_flags   (brg_cpu_flags),
        .key_inject_valid(brg_key_valid),
        .key_inject_data (brg_key_data),
        .sdram_b_we      (brg_sdram_b_we),
        .sdram_b_addr    (brg_sdram_b_addr),
        .sdram_b_din     (brg_sdram_b_din),
        .fio_event       (core_fio_event)
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
        .dbg_rom_we   (brg_rom_we),
        .dbg_rom_idx  (brg_rom_idx),
        .dbg_rom_addr (brg_rom_addr),
        .dbg_rom_data (brg_rom_data),
        .dbg_cpu_reset(brg_cpu_reset),
        .brg_sdram_b_we  (brg_sdram_b_we),
        .brg_sdram_b_addr(brg_sdram_b_addr),
        .brg_sdram_b_din (brg_sdram_b_din),
        .dbg_cpu_pc   (brg_cpu_pc),
        .dbg_cpu_a    (brg_cpu_a),
        .dbg_cpu_x    (brg_cpu_x),
        .dbg_cpu_y    (brg_cpu_y),
        .dbg_cpu_sp   (brg_cpu_sp),
        .dbg_cpu_flags(brg_cpu_flags),

        // SDRAM port A — xram_sdram wrapper (XRAM BRAM replacement)
        .sdram_addrA(core_sdram_addrA),
        .sdram_dinA (core_sdram_dinA),
        .sdram_weA  (core_sdram_weA),
        .sdram_oeA  (core_sdram_oeA),
        .sdram_doutA(core_sdram_doutA),

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

    // SDRAM port B wires from core
    wire [24:0] core_sdram_addrB;
    wire [7:0]  core_sdram_dinB;
    wire        core_sdram_weB;
    wire        core_sdram_oeB;
    wire [7:0]  core_sdram_doutB;

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
        .doutA(core_sdram_doutA),
        // Port B — SID filter curve via sid_curve_reader (Phase 2.5 Step 3)
        .addrB(core_sdram_addrB), .weB(core_sdram_weB),
        .dinB(core_sdram_dinB),   .oeB(core_sdram_oeB),
        .doutB(core_sdram_doutB)
    );

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

    // Keep ESP32 out of download mode and drive debug LED
    assign wifi_gpio0 = 1'b1;

endmodule
