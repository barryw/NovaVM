// arty_z7_cpu.sv — full NovaVM SoC (6502 + RAM/ROM + VGC) over HDMI on Arty Z7-20.
//
// Instantiates rtl/top.sv (the whole core: arlet 6502, 64KB BRAM, EhBASIC ROM
// in BRAM via INIT_FILE, VGC, SID, NIC, WTS, blitter/DMA, math copro) with:
//   - clk = ~27 MHz pixel clock (top runs CPU+VGC on this; CPU half-rate via ce)
//   - SDRAM/XMC stubbed for BRAM-only operation (sdram_doneA=1 so the XMC state
//     machine never stalls; stream/port-B idle). EhBASIC never touches XMC at boot.
//   - all debug / NovaHost / keyboard / peripheral inputs tied idle.
// VGC video -> hdl-util/hdmi (720x480p) -> OSERDESE2 -> OBUFDS -> HDMI J11.
//
// Expected: EhBASIC cold-starts from BRAM ROM and renders its banner + READY
// prompt to the VGC text layer over HDMI. (Keyboard input is a later step.)

`default_nettype none

module arty_z7_cpu (
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
    // Clocking: 125 -> 134.77 MHz (x5) + 26.95 MHz pixel (exact 5:1).
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
    logic [11:0] rst_cnt = 12'd0;
    logic        reset   = 1'b1;
    always_ff @(posedge clk_pixel) begin
        if (!lock_s2)                 begin rst_cnt <= 0;            reset <= 1'b1; end
        else if (rst_cnt != 12'hFFF)  begin rst_cnt <= rst_cnt + 1; reset <= 1'b1; end
        else                           reset <= 1'b0;
    end

    // ---------------------------------------------------------------
    // NovaVM core (top.sv) — BRAM-only, peripherals/debug idle.
    // ---------------------------------------------------------------
    wire [3:0] vid_r, vid_g, vid_b;
    wire       vid_hsync, vid_vsync, vid_de;

    // Fake file-I/O host. The FIO ($B9A0) is pure storage + an event pulse; the
    // real system relies on NovaHost (ESP32) to service fio_event and write back
    // FioStatus. There's no NovaHost on the Arty yet, so EhBASIC's autoboot
    // FILE_LOAD would poll FioStatus forever. We NAK every command (FioStatus=
    // ERR, FioErrCode=NOTFOUND) via the debug-poke writeback port so file ops
    // return "not found" and boot reaches READY. Replace with a real SD backend.
    wire         fio_event;
    logic        dbg_poke_en;
    logic [15:0] dbg_poke_addr;
    logic [7:0]  dbg_poke_data;

    top core (
        .clk(clk_pixel),
        .rst(reset),

        // Keyboard — idle (no input yet)
        .key_valid(1'b0), .key_data(8'd0), .key_ready(),

        // Board inputs — idle
        .board_buttons(8'd0), .board_switches(8'd0),

        // USB HID diagnostics — idle
        .usb_hid_status(8'd0), .usb_hid_device_type(8'd0),
        .usb_hid_last_scan(8'd0), .usb_hid_last_ascii(8'd0),
        .usb_hid_report_count(8'd0), .usb_hid_key_count(8'd0),
        .usb_hid_core_status(8'd0), .usb_hid_regs(64'd0),

        // Interrupts — inactive (active-low)
        .irq_n(1'b1), .nmi_n(1'b1),

        // Video out
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),

        // Audio out — unused
        .audio_l(), .audio_r(),

        // Debug/sim interface — all idle
        .dbg_peek_en(1'b0), .dbg_peek_addr(16'd0), .dbg_peek_data(),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr), .dbg_poke_data(dbg_poke_data),
        .dbg_pause(1'b0),
        .dbg_nic_buf_we(1'b0), .dbg_nic_buf_re(1'b0), .dbg_nic_buf_sel(1'b0),
        .dbg_nic_buf_addr(8'd0), .dbg_nic_buf_data(8'd0), .dbg_nic_buf_rdata(),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(),
        .dbg_rom_we(1'b0), .dbg_rom_idx(1'b0), .dbg_rom_addr(14'd0), .dbg_rom_data(8'd0),
        .dbg_cpu_reset(1'b0), .dbg_system_reset(1'b0), .dbg_cpu_resume(1'b0),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0),
        .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .host_wts_event_we(1'b0), .host_wts_event_data(8'd0), .host_wts_event_ready(),
        .dbg_cpu_pc(), .dbg_cpu_a(), .dbg_cpu_x(), .dbg_cpu_y(), .dbg_cpu_sp(),
        .dbg_cpu_flags(), .dbg_cpu_state(), .dbg_cpu_ir(), .dbg_cpu_addr(),
        .dbg_cpu_din(), .dbg_cpu_dout(), .dbg_cpu_we(), .dbg_cpu_rdy(),
        .dbg_cpu_irq(), .dbg_cpu_nmi(), .dbg_cpu_waiting(), .dbg_cpu_stopped(),

        // SDRAM — stubbed (BRAM-only). doneA=1 so XMC FSM never stalls.
        .sdram_clk(clk_pixel),
        .sdram_addrA(), .sdram_dinA(), .sdram_weA(), .sdram_oeA(),
        .sdram_doutA(8'd0), .sdram_doneA(1'b1),
        .sdram_addrB(), .sdram_dinB(), .sdram_weB(), .sdram_oeB(),
        .sdram_doutB(8'd0),
        .sdram_stream_req(), .sdram_stream_addr(), .sdram_stream_words(),
        .sdram_stream_ready(),
        .sdram_stream_dout(16'd0), .sdram_stream_valid(1'b0),
        .sdram_stream_busy(1'b0), .sdram_stream_done(1'b0),

        // Host-service events
        .fio_event(fio_event), .nic_event()
    );

    // FIO NAK state machine — see comment above. On each fio_event, wait a few
    // cycles (let the CPU reach its FioStatus poll loop after STZ FIO_STATUS),
    // then write FioErrCode=NOTFOUND ($B9A2) and FioStatus=ERR ($B9A1) through
    // the debug-poke port (which top routes to fio.dbg_we for $B9A0-$B9EF).
    typedef enum logic [1:0] { NAK_IDLE, NAK_WAIT, NAK_ERR, NAK_STATUS } nak_t;
    nak_t       nak_state;
    logic [4:0] nak_cnt;
    always_ff @(posedge clk_pixel) begin
        if (reset) begin
            nak_state     <= NAK_IDLE;
            nak_cnt       <= 5'd0;
            dbg_poke_en   <= 1'b0;
            dbg_poke_addr <= 16'd0;
            dbg_poke_data <= 8'd0;
        end else begin
            dbg_poke_en <= 1'b0;
            case (nak_state)
                NAK_IDLE:   if (fio_event) begin nak_cnt <= 5'd0; nak_state <= NAK_WAIT; end
                NAK_WAIT:   if (nak_cnt == 5'd31) nak_state <= NAK_ERR; else nak_cnt <= nak_cnt + 5'd1;
                NAK_ERR: begin
                    dbg_poke_en   <= 1'b1;
                    dbg_poke_addr <= 16'hB9A2;   // FioErrCode
                    dbg_poke_data <= 8'h01;       // FIO_ERR_NOTFOUND
                    nak_state     <= NAK_STATUS;
                end
                NAK_STATUS: begin
                    dbg_poke_en   <= 1'b1;
                    dbg_poke_addr <= 16'hB9A1;   // FioStatus
                    dbg_poke_data <= 8'h03;       // FIO_STATUS_ERROR
                    nak_state     <= NAK_IDLE;
                end
                default: nak_state <= NAK_IDLE;
            endcase
        end
    end

    // ---------------------------------------------------------------
    // VGC RGB -> HDMI (720x480p).
    // ---------------------------------------------------------------
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};
    wire [23:0] hdmi_rgb = vid_de ? rgb24 : 24'h000000;

    localparam int HDMI_START_X = 858 - 3;
    localparam int HDMI_START_Y = 525 - 1;

    wire [2:0]  tmds;
    wire        tmds_clock;
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
    assign led    = {reset, vid_vsync, mmcm_locked, heartbeat[24]};

endmodule

`default_nettype wire
