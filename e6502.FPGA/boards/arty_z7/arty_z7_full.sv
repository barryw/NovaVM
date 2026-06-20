// arty_z7_full.sv — full NovaVM SoC on Arty Z7-20 with XRAM backed by PS DDR3.
//
// = arty_z7_cpu.sv (6502 + BRAM + VGC -> HDMI) but the stubbed SDRAM ports are
// replaced by the axi_xram bridge into PS DDR3 (ps_full BD: PS7 + SmartConnect
// + S_AXI_HP0). With real XRAM the NovaBASIC library page-ins complete, so boot
// should reach READY (the FIO NAK still handles file I/O until a PS backend).
//
// Clocking: PL MMCM 125 -> ~27 MHz pixel (CPU+VGC), ~135 MHz x5 (HDMI). The AXI
// XRAM path (axi_xram + SmartConnect + S_AXI_HP0) runs on clk_pixel too (single
// clock domain, no CDC). PS DDR must be initialized (ps7_init/FSBL) before XRAM
// works. DDR/FIXED_IO are PS dedicated pins (no XDC needed).

`default_nettype none

module arty_z7_full (
    input  wire        clk,            // 125 MHz, H16
    input  wire [3:0]  btn,
    output wire [3:0]  led,
    output wire        led4_b,
    output wire        led4_g,

    output wire        hdmi_tx_clk_p,
    output wire        hdmi_tx_clk_n,
    output wire [2:0]  hdmi_tx_d_p,
    output wire [2:0]  hdmi_tx_d_n,
    output wire        hdmi_tx_hpdn,

    // Zynq PS dedicated I/O (DDR3 + MIO) — pass straight to the PS7 BD.
    inout  wire [14:0] DDR_addr,
    inout  wire [2:0]  DDR_ba,
    inout  wire        DDR_cas_n,
    inout  wire        DDR_ck_n,
    inout  wire        DDR_ck_p,
    inout  wire        DDR_cke,
    inout  wire        DDR_cs_n,
    inout  wire [3:0]  DDR_dm,
    inout  wire [31:0] DDR_dq,
    inout  wire [3:0]  DDR_dqs_n,
    inout  wire [3:0]  DDR_dqs_p,
    inout  wire        DDR_odt,
    inout  wire        DDR_ras_n,
    inout  wire        DDR_reset_n,
    inout  wire        DDR_we_n,
    inout  wire        FIXED_IO_ddr_vrn,
    inout  wire        FIXED_IO_ddr_vrp,
    inout  wire [53:0] FIXED_IO_mio,
    inout  wire        FIXED_IO_ps_clk,
    inout  wire        FIXED_IO_ps_porb,
    inout  wire        FIXED_IO_ps_srstb
);
    // ---- Clocking (same as arty_z7_cpu) ------------------------------------
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
    BUFG bufg_fb (.I(clk_fb), .O(clk_fb_buf));
    BUFG bufg_x5 (.I(clk_x5_raw), .O(clk_pixel_x5));
    BUFG bufg_px (.I(clk_pix_raw), .O(clk_pixel));

    (* ASYNC_REG="TRUE" *) logic ls1, ls2;
    always_ff @(posedge clk_pixel) begin ls1 <= mmcm_locked; ls2 <= ls1; end
    logic [11:0] rst_cnt = 0; logic reset = 1'b1;
    always_ff @(posedge clk_pixel) begin
        if (!ls2) begin rst_cnt <= 0; reset <= 1'b1; end
        else if (rst_cnt != 12'hFFF) begin rst_cnt <= rst_cnt + 1; reset <= 1'b1; end
        else reset <= 1'b0;
    end

    // ---- nova_core <-> axi_xram SDRAM-contract wires -----------------------
    wire [24:0] xa_addrA, xa_addrB, xa_saddr;
    wire [7:0]  xa_dinA, xa_dinB, xa_doutA, xa_doutB;
    wire        xa_weA, xa_oeA, xa_doneA, xa_weB, xa_oeB, xa_doneB;
    wire [13:0] xa_swords;
    wire        xa_sreq, xa_sready, xa_svalid, xa_sbusy, xa_sdone;
    wire [3:0]  xa_dbg_state;
    wire [13:0] xa_sleft;
    wire [15:0] xa_sdout;

    // ---- axi_xram master <-> PS BD S_AXI_XRAM ------------------------------
    wire [31:0] x_awaddr, x_wdata, x_araddr, x_rdata;
    wire [7:0]  x_awlen, x_arlen;
    wire [2:0]  x_awsize, x_arsize;
    wire [1:0]  x_awburst, x_arburst, x_bresp, x_rresp;
    wire [3:0]  x_wstrb;
    wire        x_awvalid, x_awready, x_wvalid, x_wready, x_wlast;
    wire        x_bvalid, x_bready, x_arvalid, x_arready;
    wire        x_rvalid, x_rready, x_rlast;

    // ---- CPU debug taps (for ILA) ------------------------------------------
    wire [15:0] d_pc, d_addr;
    wire        d_we, d_rdy, d_waiting, d_stopped;

    // ---- VGC video ---------------------------------------------------------
    wire [3:0] vid_r, vid_g, vid_b;
    wire       vid_hsync, vid_vsync, vid_de;

    // ---- PS FIO host bridge ------------------------------------------------
    // The Zynq PS A9 is the host (via M_AXI_GP0 -> fio_bridge). It pokes/peeks
    // CPU RAM + the FIO bank ($B9A0-$B9EF), injects keys, holds/releases the
    // 6502 reset (held at config until the PS stages the resident loader), and
    // services fio_event. XRAM (the library shelf) is staged by the PS writing
    // PS DDR3 directly (axi_xram maps XRAM -> DDR), not through this bridge.
    wire         fio_event;
    wire         fb_poke_en;  wire [15:0] fb_poke_addr; wire [7:0] fb_poke_data;
    wire [15:0]  fb_peek_addr; wire fb_peek_en; wire [7:0] fb_peek_data;
    wire         fb_key_valid; wire [7:0] fb_key_data; wire fb_key_ready;
    wire         fb_cpu_reset;
    wire         fb_rom_we, fb_rom_idx; wire [13:0] fb_rom_addr; wire [7:0] fb_rom_data;
    wire         fb_vmem_re; wire [2:0] fb_vmem_space; wire [16:0] fb_vmem_addr; wire [7:0] fb_vmem_rdata;

    top core (
        .clk(clk_pixel), .rst(reset),
        .key_valid(fb_key_valid), .key_data(fb_key_data), .key_ready(fb_key_ready),
        .board_buttons(8'd0), .board_switches(8'd0),
        .usb_hid_status(8'd0), .usb_hid_device_type(8'd0),
        .usb_hid_last_scan(8'd0), .usb_hid_last_ascii(8'd0),
        .usb_hid_report_count(8'd0), .usb_hid_key_count(8'd0),
        .usb_hid_core_status(8'd0), .usb_hid_regs(64'd0),
        .irq_n(1'b1), .nmi_n(1'b1),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(), .audio_r(),
        .dbg_peek_en(fb_peek_en), .dbg_peek_addr(fb_peek_addr), .dbg_peek_data(fb_peek_data),
        .dbg_poke_en(fb_poke_en), .dbg_poke_addr(fb_poke_addr), .dbg_poke_data(fb_poke_data),
        .dbg_pause(1'b0),
        .dbg_nic_buf_we(1'b0), .dbg_nic_buf_re(1'b0), .dbg_nic_buf_sel(1'b0),
        .dbg_nic_buf_addr(8'd0), .dbg_nic_buf_data(8'd0), .dbg_nic_buf_rdata(),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(fb_vmem_re), .dbg_vmem_space(fb_vmem_space),
        .dbg_vmem_addr(fb_vmem_addr), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(fb_vmem_rdata),
        .dbg_rom_we(fb_rom_we), .dbg_rom_idx(fb_rom_idx),
        .dbg_rom_addr(fb_rom_addr), .dbg_rom_data(fb_rom_data),
        .dbg_cpu_reset(fb_cpu_reset), .dbg_system_reset(1'b0), .dbg_cpu_resume(1'b0),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0),
        .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .host_wts_event_we(1'b0), .host_wts_event_data(8'd0), .host_wts_event_ready(),
        .dbg_cpu_pc(d_pc), .dbg_cpu_a(), .dbg_cpu_x(), .dbg_cpu_y(), .dbg_cpu_sp(),
        .dbg_cpu_flags(), .dbg_cpu_state(), .dbg_cpu_ir(), .dbg_cpu_addr(d_addr),
        .dbg_cpu_din(), .dbg_cpu_dout(), .dbg_cpu_we(d_we), .dbg_cpu_rdy(d_rdy),
        .dbg_cpu_irq(), .dbg_cpu_nmi(), .dbg_cpu_waiting(d_waiting), .dbg_cpu_stopped(d_stopped),
        // SDRAM ports -> axi_xram (real XRAM in PS DDR3)
        .sdram_clk(clk_pixel),
        .sdram_addrA(xa_addrA), .sdram_dinA(xa_dinA), .sdram_weA(xa_weA), .sdram_oeA(xa_oeA),
        .sdram_doutA(xa_doutA), .sdram_doneA(xa_doneA),
        .sdram_addrB(xa_addrB), .sdram_dinB(xa_dinB), .sdram_weB(xa_weB), .sdram_oeB(xa_oeB),
        .sdram_doutB(xa_doutB),
        .sdram_stream_req(xa_sreq), .sdram_stream_addr(xa_saddr), .sdram_stream_words(xa_swords),
        .sdram_stream_ready(xa_sready),
        .sdram_stream_dout(xa_sdout), .sdram_stream_valid(xa_svalid),
        .sdram_stream_busy(xa_sbusy), .sdram_stream_done(xa_sdone),
        .fio_event(fio_event), .nic_event()
    );

    // Port B done is consumed inside sdram.v on the real board; top doesn't use
    // a doneB input, so axi_xram's doneB is left unconnected here.
    wire xa_doneB_unused;

    axi_xram #(.XRAM_BASE(32'h1000_0000)) xram (
        .clk(clk_pixel), .rst(reset),
        .addrA(xa_addrA), .weA(xa_weA), .dinA(xa_dinA), .oeA(xa_oeA),
        .doutA(xa_doutA), .doneA(xa_doneA),
        .addrB(xa_addrB), .weB(xa_weB), .dinB(xa_dinB), .oeB(xa_oeB),
        .doutB(xa_doutB), .doneB(xa_doneB_unused),
        .stream_req(xa_sreq), .stream_addr(xa_saddr), .stream_words(xa_swords),
        .stream_ready(xa_sready),
        .stream_dout(xa_sdout), .stream_valid(xa_svalid),
        .stream_busy(xa_sbusy), .stream_done(xa_sdone), .dbg_state(xa_dbg_state),
        .dbg_sleft(xa_sleft),
        .m_axi_awaddr(x_awaddr), .m_axi_awlen(x_awlen), .m_axi_awsize(x_awsize),
        .m_axi_awburst(x_awburst), .m_axi_awvalid(x_awvalid), .m_axi_awready(x_awready),
        .m_axi_wdata(x_wdata), .m_axi_wstrb(x_wstrb), .m_axi_wlast(x_wlast),
        .m_axi_wvalid(x_wvalid), .m_axi_wready(x_wready),
        .m_axi_bresp(x_bresp), .m_axi_bvalid(x_bvalid), .m_axi_bready(x_bready),
        .m_axi_araddr(x_araddr), .m_axi_arlen(x_arlen), .m_axi_arsize(x_arsize),
        .m_axi_arburst(x_arburst), .m_axi_arvalid(x_arvalid), .m_axi_arready(x_arready),
        .m_axi_rdata(x_rdata), .m_axi_rresp(x_rresp), .m_axi_rlast(x_rlast),
        .m_axi_rvalid(x_rvalid), .m_axi_rready(x_rready)
    );

    // ---- PS M_AXI_GP0 -> fio_bridge (AXI4-Lite) ----------------------------
    wire [31:0] f_awaddr, f_wdata, f_araddr, f_rdata;
    wire [2:0]  f_awprot, f_arprot;
    wire [3:0]  f_wstrb;
    wire [1:0]  f_bresp, f_rresp;
    wire        f_awvalid, f_awready, f_wvalid, f_wready, f_bvalid, f_bready;
    wire        f_arvalid, f_arready, f_rvalid, f_rready;

    ps_full_wrapper ps (
        .DDR_addr(DDR_addr), .DDR_ba(DDR_ba), .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n), .DDR_ck_p(DDR_ck_p), .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n), .DDR_dm(DDR_dm), .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n), .DDR_dqs_p(DDR_dqs_p), .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n), .DDR_reset_n(DDR_reset_n), .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn), .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio), .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb), .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .hp_aclk(clk_pixel),
        .S_AXI_XRAM_awaddr(x_awaddr), .S_AXI_XRAM_awlen(x_awlen), .S_AXI_XRAM_awsize(x_awsize),
        .S_AXI_XRAM_awburst(x_awburst), .S_AXI_XRAM_awlock(1'b0), .S_AXI_XRAM_awcache(4'b0011),
        .S_AXI_XRAM_awprot(3'b000), .S_AXI_XRAM_awqos(4'b0000),
        .S_AXI_XRAM_awvalid(x_awvalid), .S_AXI_XRAM_awready(x_awready),
        .S_AXI_XRAM_wdata(x_wdata), .S_AXI_XRAM_wstrb(x_wstrb), .S_AXI_XRAM_wlast(x_wlast),
        .S_AXI_XRAM_wvalid(x_wvalid), .S_AXI_XRAM_wready(x_wready),
        .S_AXI_XRAM_bresp(x_bresp), .S_AXI_XRAM_bvalid(x_bvalid), .S_AXI_XRAM_bready(x_bready),
        .S_AXI_XRAM_araddr(x_araddr), .S_AXI_XRAM_arlen(x_arlen), .S_AXI_XRAM_arsize(x_arsize),
        .S_AXI_XRAM_arburst(x_arburst), .S_AXI_XRAM_arlock(1'b0), .S_AXI_XRAM_arcache(4'b0011),
        .S_AXI_XRAM_arprot(3'b000), .S_AXI_XRAM_arqos(4'b0000),
        .S_AXI_XRAM_arvalid(x_arvalid), .S_AXI_XRAM_arready(x_arready),
        .S_AXI_XRAM_rdata(x_rdata), .S_AXI_XRAM_rresp(x_rresp), .S_AXI_XRAM_rlast(x_rlast),
        .S_AXI_XRAM_rvalid(x_rvalid), .S_AXI_XRAM_rready(x_rready),
        .M_AXI_FIO_awaddr(f_awaddr), .M_AXI_FIO_awprot(f_awprot),
        .M_AXI_FIO_awvalid(f_awvalid), .M_AXI_FIO_awready(f_awready),
        .M_AXI_FIO_wdata(f_wdata), .M_AXI_FIO_wstrb(f_wstrb),
        .M_AXI_FIO_wvalid(f_wvalid), .M_AXI_FIO_wready(f_wready),
        .M_AXI_FIO_bresp(f_bresp), .M_AXI_FIO_bvalid(f_bvalid), .M_AXI_FIO_bready(f_bready),
        .M_AXI_FIO_araddr(f_araddr), .M_AXI_FIO_arprot(f_arprot),
        .M_AXI_FIO_arvalid(f_arvalid), .M_AXI_FIO_arready(f_arready),
        .M_AXI_FIO_rdata(f_rdata), .M_AXI_FIO_rresp(f_rresp),
        .M_AXI_FIO_rvalid(f_rvalid), .M_AXI_FIO_rready(f_rready)
    );

    // ---- VGC RGB -> HDMI ---------------------------------------------------
    wire [23:0] rgb24 = {vid_r, vid_r, vid_g, vid_g, vid_b, vid_b};
    wire [23:0] hdmi_rgb = vid_de ? rgb24 : 24'h000000;
    localparam int HDMI_START_X = 858 - 3;
    localparam int HDMI_START_Y = 525 - 1;
    wire [2:0] tmds; wire tmds_clock;
    logic [9:0] cx, cy, fw, fh, sw, sh;
    // hdmi is provided as an out-of-context checkpoint (build/hdmi_ooc.dcp) with
    // these generics baked in (VIDEO_ID_CODE=2, DVI_OUTPUT=1, IT_CONTENT=1,
    // VIDEO_REFRESH_RATE_MILLIHZ=59940, START_X=855, START_Y=524). OOC isolates
    // hdmi/tmds_channel synth from the full design, which otherwise crashes
    // Vivado synth on tmds_channel once fio_bridge is present. Instantiated with
    // NO params so it links the dcp. (HDMI_START_X/Y above == 855/524.)
    hdmi hdmi_inst (
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

    // ---- PS FIO host bridge (AXI4-Lite slave on M_AXI_GP0) -----------------
    fio_bridge fio_host (
        .aclk(clk_pixel), .aresetn(~reset),
        .s_awaddr(f_awaddr), .s_awvalid(f_awvalid), .s_awready(f_awready),
        .s_wdata(f_wdata), .s_wstrb(f_wstrb), .s_wvalid(f_wvalid), .s_wready(f_wready),
        .s_bresp(f_bresp), .s_bvalid(f_bvalid), .s_bready(f_bready),
        .s_araddr(f_araddr), .s_arvalid(f_arvalid), .s_arready(f_arready),
        .s_rdata(f_rdata), .s_rresp(f_rresp), .s_rvalid(f_rvalid), .s_rready(f_rready),
        .dbg_poke_en(fb_poke_en), .dbg_poke_addr(fb_poke_addr), .dbg_poke_data(fb_poke_data),
        .dbg_peek_addr(fb_peek_addr), .dbg_peek_en(fb_peek_en), .dbg_peek_data(fb_peek_data),
        .key_valid(fb_key_valid), .key_data(fb_key_data), .key_ready(fb_key_ready),
        .dbg_rom_we(fb_rom_we), .dbg_rom_idx(fb_rom_idx),
        .dbg_rom_addr(fb_rom_addr), .dbg_rom_data(fb_rom_data),
        .dbg_cpu_reset(fb_cpu_reset),
        .dbg_vmem_re(fb_vmem_re), .dbg_vmem_space(fb_vmem_space),
        .dbg_vmem_addr(fb_vmem_addr), .dbg_vmem_rdata(fb_vmem_rdata),
        .fio_event(fio_event), .dbg_cpu_pc(d_pc),
        // dbg_bus: [0]=cpu_rdy [1]=stream_busy [2]=stream_valid [3]=stream_done
        //          [4]=arvalid [5]=arready [6]=rvalid [7]=rready [11:8]=axi state
        .dbg_bus({4'd0, xa_dbg_state, x_rready, x_rvalid, x_arready, x_arvalid,
                  xa_sdone, xa_svalid, xa_sbusy, d_rdy}),
        .dbg_aux({xa_swords, xa_sleft})

    );

    // (debug ILA removed — no longer needed; shrinks the design)

    // ---- Status LEDs -------------------------------------------------------
    // led[0] CPU ALIVE : counter advances only while the 6502 is running (rdy=1).
    //                    Blinks ~1.6Hz when alive; FREEZES if the CPU stalls
    //                    (e.g. a page-in hang) -> instant "is the CPU stuck?" tell.
    // led[1] I/O       : SD->XRAM page-in stream + FIO host requests, stretched so
    //                    brief transfers are visible.
    // led[2] NETWORK   : reserved (PS Ethernet) -- off for now.
    // led[3] SYSTEM    : reset / fault.
    logic [23:0] alive_cnt;
    always_ff @(posedge clk_pixel) if (d_rdy) alive_cnt <= alive_cnt + 24'd1;

    logic [22:0] io_stretch;
    always_ff @(posedge clk_pixel)
        if (xa_sbusy || fio_event) io_stretch <= 23'h7FFFFF;
        else if (io_stretch != 0)  io_stretch <= io_stretch - 23'd1;

    logic [23:0] heartbeat;
    always_ff @(posedge clk_pixel) heartbeat <= heartbeat + 24'd1;
    assign led4_b = heartbeat[23];     // board/clock alive (free-running)
    assign led4_g = mmcm_locked;       // pixel clock locked
    assign led    = {reset, 1'b0, |io_stretch, alive_cnt[23]};

endmodule

`default_nettype wire
