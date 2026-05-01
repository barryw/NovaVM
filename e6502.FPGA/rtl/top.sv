// Top-level: 6502 CPU + VGC + Blitter + DMA + SID + 64KB RAM + 512KB XRAM
// VGC snoops bus for register/charout writes, generates video
// Blitter performs 2D DMA across all memory spaces ($BA83-$BA9B), stalls CPU via RDY
// DMA performs 1D bulk copy/fill ($BA63-$BA75), stalls CPU via RDY
// Blitter and DMA are mutually exclusive via RDY stall; top.sv muxes their
// memory-port outputs into a single bus-master signal set (bm_*).
// SID: dual 6581/8580 sound chips, runtime model select via $D440
// XMC bank switching: 4x256-byte windows at $BC00-$BFFF into 512KB XRAM
//
// All large memories use dpram instances for BRAM inference during synthesis.

module top (
    input  logic        clk,        // pixel clock (~25 MHz)
    input  logic        rst,

    // Keyboard input
    input  logic        key_valid,
    input  logic [7:0]  key_data,

    input  logic        irq_n,
    input  logic        nmi_n,

    // Video output
    output logic [3:0]  vid_r,
    output logic [3:0]  vid_g,
    output logic [3:0]  vid_b,
    output logic        vid_hsync,
    output logic        vid_vsync,
    output logic        vid_de,

    // Audio output (signed 18-bit, active each frame)
    output logic signed [17:0] audio_l,
    output logic signed [17:0] audio_r,

    // Debug/simulation interface
    input  logic        dbg_peek_en,
    input  logic [15:0] dbg_peek_addr,
    output logic [7:0]  dbg_peek_data,
    input  logic        dbg_poke_en,
    input  logic [15:0] dbg_poke_addr,
    input  logic [7:0]  dbg_poke_data,
    input  logic        dbg_pause,        // halt CPU when high
    input  logic        dbg_vmem_we,
    input  logic        dbg_vmem_re,
    input  logic [2:0]  dbg_vmem_space,
    input  logic [16:0] dbg_vmem_addr,
    input  logic [7:0]  dbg_vmem_data,
    output logic [7:0]  dbg_vmem_rdata,
    // ROM-load port — write one byte at a time into either ROM bank.
    // idx=0 → basic_rom, idx=1 → ext_rom. Used by NovaHost to stream the
    // EhBASIC + extension binaries into BRAM at boot.
    input  logic        dbg_rom_we,
    input  logic        dbg_rom_idx,
    input  logic [13:0] dbg_rom_addr,
    input  logic [7:0]  dbg_rom_data,
    input  logic        dbg_cpu_reset,     // ESP32-driven soft reset, ORs with rst
    input  logic        dbg_system_reset,  // ESP32-driven custom-chip reset, ORs with rst
    input  logic        dbg_cpu_resume,    // one-cycle pulse; releases CPU STP halt
    // Debug-bridge SDRAM port B path. NovaHost uses this for boot preload and
    // direct XRAM file streaming.
    input  logic        brg_sdram_b_we,
    input  logic        brg_sdram_b_oe,
    input  logic [24:0] brg_sdram_b_addr,
    input  logic [7:0]  brg_sdram_b_din,
    output logic [15:0] dbg_cpu_pc,
    output logic [7:0]  dbg_cpu_a,
    output logic [7:0]  dbg_cpu_x,
    output logic [7:0]  dbg_cpu_y,
    output logic [7:0]  dbg_cpu_sp,
    output logic [7:0]  dbg_cpu_flags,
    output logic [5:0]  dbg_cpu_state,
    output logic [7:0]  dbg_cpu_ir,
    output logic [15:0] dbg_cpu_addr,
    output logic [7:0]  dbg_cpu_din,
    output logic [7:0]  dbg_cpu_dout,
    output logic        dbg_cpu_we,
    output logic        dbg_cpu_rdy,
    output logic        dbg_cpu_irq,
    output logic        dbg_cpu_nmi,
    output logic        dbg_cpu_waiting,
    output logic        dbg_cpu_stopped,

    // SDRAM port A — driven by the XRAM-over-SDRAM wrapper. fpga_top
    // connects these to sdram.v's port A; in sim the test harness
    // instantiates its own sdram + mock chip.
    output logic [24:0] sdram_addrA,
    output logic [7:0]  sdram_dinA,
    output logic        sdram_weA,
    output logic        sdram_oeA,
    input  logic [7:0]  sdram_doutA,

    // SDRAM port B — driven by the SID curve reader (Phase 2.5 Step 3).
    output logic [24:0] sdram_addrB,
    output logic [7:0]  sdram_dinB,
    output logic        sdram_weB,
    output logic        sdram_oeB,
    input  logic [7:0]  sdram_doutB,

    // File I/O event — pulses one clock when the CPU writes a non-zero
    // value to FioCmd ($B9A0). fpga_top.sv routes this to the debug
    // bridge, which emits an async EVENT_FIO to the ESP32.
    output logic        fio_event
`ifndef SYNTHESIS
    ,
    // Simulation-only VRAM peek path. Excluded from synthesis so the
    // hardware map still exposes only the VGC VRAM port.
    input  logic        dbg_vram_read_en,
    input  logic [2:0]  dbg_vram_space,
    input  logic [16:0] dbg_vram_addr,
    output logic [7:0]  dbg_vram_rdata
`endif
);

    localparam ROM_BASE  = 16'hC000;
    localparam CHARIN    = 16'hA00F;

    // ROM bank switching
    localparam REG_ROMSWAP  = 16'hA03F;
    localparam ROMSWAP_BASIC = 8'h02;
    localparam ROMSWAP_EXT   = 8'h04;

    // XMC address map
    localparam XMC_REG_BASE = 16'hBA00;  // XMC registers $BA00-$BA3F
    localparam XMC_REG_END  = 16'hBA3F;
    localparam XMC_WIN_BASE = 16'hBC00;  // 4x256-byte windows
    localparam XMC_WIN_END  = 16'hBFFF;

    // SID address map
    localparam SID1_BASE    = 16'hD400;  // SID 1 registers $D400-$D41F
    localparam SID1_END     = 16'hD41F;
    localparam SID2_BASE    = 16'hD420;  // SID 2 registers $D420-$D43F
    localparam SID2_END     = 16'hD43F;
    localparam SID_CFG      = 16'hD440;  // SID config: bit0=SID1 mode, bit1=SID2 mode

    // CPU bus
    wire [15:0] cpu_addr;
    wire [7:0]  cpu_dout;
    wire        cpu_we;
    logic [7:0] cpu_din;
    wire [15:0] cpu_dbg_pc;
    wire  [7:0] cpu_dbg_a, cpu_dbg_x, cpu_dbg_y, cpu_dbg_s, cpu_dbg_flags;
    wire  [5:0] cpu_dbg_state;
    wire  [7:0] cpu_dbg_ir;
    wire        cpu_dbg_waiting, cpu_dbg_stopped;
    wire        vgc_sys_reset_req;

    // A VGC SYSRESET command is a machine reset, not just a custom-chip
    // scrub request. Latch it long enough that the CPU and every custom
    // block see a clean reset pulse before execution restarts at $FFFC.
    localparam int VGC_SYS_RESET_HOLD_CYCLES = 8;
    logic [$clog2(VGC_SYS_RESET_HOLD_CYCLES+1)-1:0] vgc_sys_reset_hold = '0;
    wire        vgc_sys_reset_active = vgc_sys_reset_req | (vgc_sys_reset_hold != '0);
    wire        custom_rst = rst | dbg_system_reset | vgc_sys_reset_active;
    wire        cpu_reset = rst | dbg_cpu_reset | dbg_system_reset | vgc_sys_reset_active;

    always_ff @(posedge clk) begin
        if (rst || dbg_system_reset) begin
            vgc_sys_reset_hold <= '0;
        end else if (vgc_sys_reset_req) begin
            vgc_sys_reset_hold <= VGC_SYS_RESET_HOLD_CYCLES[$bits(vgc_sys_reset_hold)-1:0];
        end else if (vgc_sys_reset_hold != '0) begin
            vgc_sys_reset_hold <= vgc_sys_reset_hold - 1'b1;
        end
    end

    // =========================================================================
    // 64KB main memory — dpram for BRAM inference
    // Port A: CPU read/write (+ blitter write when CPU stalled)
    // Port B: tile DMA / blitter reads / debug reads
    // =========================================================================
`ifdef SYNTHESIS
    // --- SYNTHESIS: use dpram instances for BRAM inference ---

    // CPU clock enable — halve to ~12.5 MHz for timing closure.
    // The DI→state→AB combinational path exceeds 25 MHz.
    logic cpu_ce;
    // xram_stall holds cpu_ce low whenever an XMC-window access is pending
    // or in flight in the xram_sdram wrapper, so the CPU doesn't advance
    // past the access before its data is ready.
    wire xram_stall;
    always_ff @(posedge clk) begin
        if (cpu_reset)      cpu_ce <= 0;
        else if (xram_stall) cpu_ce <= 0;
        else                cpu_ce <= ~cpu_ce;
    end

    // Held CPU address: freeze during stall cycle so dpram keeps
    // outputting the correct data (no overwrite by the new address).
    logic [15:0] held_cpu_addr;
    always_ff @(posedge clk) begin
        if (cpu_ce)
            held_cpu_addr <= cpu_addr;
    end
    // Use held address for all dpram reads and decode during stall
    wire [15:0] mem_addr = cpu_ce ? cpu_addr : held_cpu_addr;

    // Main RAM: 64KB
    logic [15:0] ram_a_addr;
    logic [7:0]  ram_a_din;
    logic        ram_a_we;
    logic [7:0]  ram_a_dout;
    logic [15:0] ram_b_addr;
    logic [7:0]  ram_b_dout;

    dpram #(.WIDTH(8), .DEPTH(65536)) main_ram (
        .clk(clk),
        .addr_a(ram_a_addr), .din_a(ram_a_din), .we_a(ram_a_we), .dout_a(ram_a_dout),
        .addr_b(ram_b_addr), .dout_b(ram_b_dout)
    );

    // EhBASIC ROM: 16KB. Initialized from the bitstream so the FPGA can
    // self-boot if NovaHost is absent/stale; NovaHost may still overwrite it
    // at boot via dbg_rom_we (idx=0) before releasing CPU reset.
    logic [13:0] brom_b_addr;
    logic [7:0]  brom_b_dout;
    wire         brom_we = dbg_rom_we && (dbg_rom_idx == 1'b0);

    dpram #(.WIDTH(8), .DEPTH(16384), .INIT_FILE("rom/ehbasic.hex")) basic_rom_inst (
        .clk(clk),
        .addr_a(dbg_rom_addr), .din_a(dbg_rom_data), .we_a(brom_we), .dout_a(),
        .addr_b(brom_b_addr), .dout_b(brom_b_dout)
    );

    // Extension ROM: 16KB. Bitstream initialized, runtime-overwritable.
    logic [13:0] erom_b_addr;
    logic [7:0]  erom_b_dout;
    wire         erom_we = dbg_rom_we && (dbg_rom_idx == 1'b1);

    dpram #(.WIDTH(8), .DEPTH(16384), .INIT_FILE("rom/extension.hex")) ext_rom_inst (
        .clk(clk),
        .addr_a(dbg_rom_addr), .din_a(dbg_rom_data), .we_a(erom_we), .dout_a(),
        .addr_b(erom_b_addr), .dout_b(erom_b_dout)
    );

    // XRAM: 512KB backed by SDRAM via xram_sdram wrapper. Phase 2.5 Step 2
    // replaces the old 4KB BRAM stub; interface signals keep their dpram
    // names at this layer so the CPU/blitter decode logic is unchanged.
    localparam XRAM_SIZE = 524288;       // 512 KB — full SDRAM-backed region
    logic [18:0] xram_a_addr;
    logic [7:0]  xram_a_din;
    logic        xram_a_we;
    logic        xram_a_re;
    logic [7:0]  xram_a_dout;
    logic        xram_busy;

    xram_sdram xram_sdram_inst (
        .clk      (clk),
        .rst      (custom_rst),
        .req_addr (xram_a_addr),
        .req_din  (xram_a_din),
        .req_we   (xram_a_we),
        .req_re   (xram_a_re),
        .req_dout (xram_a_dout),
        .busy     (xram_busy),
        .sdram_addr(sdram_addrA),
        .sdram_din (sdram_dinA),
        .sdram_we  (sdram_weA),
        .sdram_oe  (sdram_oeA),
        .sdram_dout(sdram_doutA)
    );

    logic ext_rom_active;

    initial begin
        ext_rom_active = 0;
    end

    // XMC register file ($BA00-$BA3F)
    logic [7:0] xmc_regs [0:63];

    localparam XMC_BANKS_REG = 6'h0D;
    localparam XMC_WINCTL_REG = 6'h16;

    initial begin
        for (int i = 0; i < 64; i++) xmc_regs[i] = 0;
        xmc_regs[XMC_BANKS_REG] = 8'd8;
        xmc_regs[XMC_WINCTL_REG] = 8'h0F;
    end

    // XMC address decode — use mem_addr (frozen during CPU stall)
    wire xmc_reg_sel = (mem_addr >= XMC_REG_BASE && mem_addr <= XMC_REG_END);
    wire xmc_win_sel = (mem_addr >= XMC_WIN_BASE && mem_addr <= XMC_WIN_END);
    wire [5:0]  xmc_reg_off = mem_addr[5:0];
    wire [1:0]  xmc_page    = mem_addr[9:8];
    wire [7:0]  xmc_offset  = mem_addr[7:0];

    // Each 256-byte XMC window is page-aligned. The low base register is
    // writable/readable for software symmetry, but address translation uses
    // the CPU address low byte as the window offset.
    logic [7:0] xmc_win_mid;
    logic [7:0] xmc_win_high;

    always_comb begin
        unique case (xmc_page)
            2'd0: begin xmc_win_mid = xmc_regs[6'h19]; xmc_win_high = xmc_regs[6'h1A]; end
            2'd1: begin xmc_win_mid = xmc_regs[6'h1C]; xmc_win_high = xmc_regs[6'h1D]; end
            2'd2: begin xmc_win_mid = xmc_regs[6'h1F]; xmc_win_high = xmc_regs[6'h20]; end
            2'd3: begin xmc_win_mid = xmc_regs[6'h22]; xmc_win_high = xmc_regs[6'h23]; end
        endcase
    end

    wire [18:0] xmc_addr_raw = {xmc_win_high[2:0], xmc_win_mid, xmc_offset};
    // Full 19-bit XMC address maps directly into the 512 KB SDRAM region.
    wire [18:0] xmc_addr = xmc_addr_raw;
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];

    // =========================================================================
    // Memory read — pipelined for dpram 1-cycle latency
    // mem_addr is held stable during CPU stall cycles (cpu_ce=0)
    // so dpram outputs don't get overwritten by the new address.
    // =========================================================================
    wire cpu_in_rom = (mem_addr >= ROM_BASE);
    wire dma_reg_sel = (mem_addr >= 16'hBA63 && mem_addr <= 16'hBA75);
    wire blt_reg_sel = (mem_addr >= 16'hBA83 && mem_addr <= 16'hBA9B);
    wire fio_reg_sel = (mem_addr >= 16'hB9A0 && mem_addr <= 16'hB9EF);
    wire vgc_read_sel = (mem_addr >= 16'hA000 && mem_addr <= 16'hA01F) ||
                        (mem_addr >= 16'hA040 && mem_addr <= 16'hA0BF) ||
                        (mem_addr >= 16'hA0C0 && mem_addr <= 16'hA0DF) ||
                        (mem_addr >= 16'hA0E0 && mem_addr <= 16'hA0E7) ||
                        (mem_addr >= 16'hA0F0 && mem_addr <= 16'hA0FF);

    // Register the decode signals for next-cycle mux
    logic r_xmc_win_sel, r_xmc_win_enabled, r_xmc_reg_sel;
    logic r_dma_reg_sel, r_blt_reg_sel;
    logic r_cpu_in_rom, r_ext_rom_active;
    logic r_vgc_read_sel;
    logic r_fio_reg_sel;
    logic [5:0] r_xmc_reg_off;

    // Register decode signals every cycle — they align with dpram outputs
    // since both are captured from the same cpu_addr at the same posedge.
    always_ff @(posedge clk) begin
        r_xmc_win_sel     <= xmc_win_sel;
        r_xmc_win_enabled <= xmc_win_enabled;
        r_xmc_reg_sel     <= xmc_reg_sel;
        r_dma_reg_sel     <= dma_reg_sel;
        r_blt_reg_sel     <= blt_reg_sel;
        r_cpu_in_rom      <= cpu_in_rom;
        r_ext_rom_active  <= ext_rom_active;
        r_vgc_read_sel    <= vgc_read_sel;
        r_fio_reg_sel     <= fio_reg_sel;
        r_xmc_reg_off     <= xmc_reg_off;
    end

    // Present addresses to dpram read ports.
    // Normally the CPU's mem_addr drives port B for instruction fetch.
    // When a debug peek targets the ROM range, route the debug address in
    // instead so peek returns the actual ROM byte rather than whatever the
    // CPU happens to be fetching.
    wire dbg_rom_read = dbg_peek_en && (dbg_peek_addr >= ROM_BASE);
    assign brom_b_addr = dbg_rom_read ? dbg_peek_addr[13:0] : mem_addr[13:0];
    assign erom_b_addr = dbg_rom_read ? dbg_peek_addr[13:0] : mem_addr[13:0];

    // XRAM: drive the xram_sdram wrapper. The wrapper is edge-triggered:
    // req_re / req_we must be asserted for exactly one pixel clock per
    // transaction. Some 6502 addressing modes keep the same XMC address on the
    // bus for multiple CPU states (for example ($zp),Y store does a read cycle
    // before the write). Track the fulfilled CPU bus cycle by state/address/WE
    // so each real CPU cycle fires once, even when the address stays in-window.
    logic xmc_cycle_done;
    logic xmc_pending;
    logic [5:0] xmc_last_state;
    logic [15:0] xmc_last_addr;
    logic xmc_last_we;

    wire  xmc_access     = xmc_win_sel && xmc_win_enabled;
    wire  xmc_cpu_access = xmc_access && cpu_active;
    wire  xmc_same_cycle =
        xmc_cycle_done &&
        (xmc_last_state == cpu_dbg_state) &&
        (xmc_last_addr  == mem_addr) &&
        (xmc_last_we    == cpu_we);
    wire  xmc_new_cycle  = xmc_cpu_access && !xmc_same_cycle && !xmc_pending;
    wire  xmc_need_fire  = xmc_pending || xmc_new_cycle;
    wire  xmc_fire       = xmc_need_fire && !xram_busy;
    wire  xmc_cpu_read   = xmc_fire && !cpu_we;
    wire  xmc_cpu_write  = xmc_fire &&  cpu_we;

    // Bus-master XRAM write (blitter or DMA). Previously used a `bm_xram_armed`
    // single-shot latch that cleared on fire and re-armed only when `bm_xram_we`
    // dropped. That worked for COPY (which cycles through S_READ between
    // bytes, dropping we=0) but BROKE FILL: fill-mode stays in S_WRITE
    // continuously with we=1 across bytes, so armed stayed 0 after the first
    // byte and every subsequent byte was dropped. XRESET's 1280-byte XRAM
    // fill left pages 1-4 uncleared, which then tripped `xmc_alloc`'s
    // directory-entry scan into ERR_NOSPACE and returned FCER from BASIC.
    //
    // xram_sdram.sv already protects against double-latch: it only accepts
    // req_we in S_IDLE (busy=0). Holding we=1 during busy is harmless —
    // the wrapper ignores it. With the blitter/DMA back-pressure stall
    // (blitter.sv / dma.sv hold we/addr/data stable while xram_busy=1),
    // exactly one byte lands per intended write. Removing the arm latch.
    wire  bm_xram_fire = bm_xram_we && !xram_busy;

    always_ff @(posedge clk) begin
        if (custom_rst) begin
            xmc_cycle_done <= 1'b0;
            xmc_pending    <= 1'b0;
            xmc_last_state <= 6'd0;
            xmc_last_addr  <= 16'd0;
            xmc_last_we    <= 1'b0;
        end else begin
            if (!xmc_access) begin
                xmc_cycle_done <= 1'b0;
                xmc_pending    <= 1'b0;
            end else begin
                if (xmc_new_cycle && xram_busy)
                    xmc_pending <= 1'b1;

                if (xmc_fire) begin
                    xmc_cycle_done <= 1'b1;
                    xmc_pending    <= 1'b0;
                    xmc_last_state <= cpu_dbg_state;
                    xmc_last_addr  <= mem_addr;
                    xmc_last_we    <= cpu_we;
                end
            end
        end
    end

    // CPU-side stall: hold cpu_ce low any time a CPU XMC access is
    // outstanding (needed but not fired, or fired and still busy).
    assign xram_stall = xmc_access && (xram_busy || xmc_need_fire);

    // Bus-master read fire gate — same shape as bm_xram_fire. Separate so
    // a read during write-busy doesn't accidentally convert to a write.
    wire bm_xram_read_fire = bm_xram_re && !xram_busy;

    always_comb begin
        // CPU path has priority; bus master gets the bus when CPU isn't firing.
        if (xmc_cpu_read || xmc_cpu_write) begin
            xram_a_addr = xmc_addr;
            xram_a_din  = cpu_dout;
            xram_a_we   = xmc_cpu_write;
            xram_a_re   = xmc_cpu_read;
        end else if (bm_xram_fire) begin
            xram_a_addr = bm_xram_addr[18:0];
            xram_a_din  = bm_xram_wdata;
            xram_a_we   = 1'b1;
            xram_a_re   = 1'b0;
        end else if (bm_xram_read_fire) begin
            xram_a_addr = bm_xram_addr[18:0];
            xram_a_din  = 8'd0;
            xram_a_we   = 1'b0;
            xram_a_re   = 1'b1;
        end else begin
            xram_a_addr = 19'd0;
            xram_a_din  = 8'd0;
            xram_a_we   = 1'b0;
            xram_a_re   = 1'b0;
        end
    end

    // Registered XMC register read (for cpu_din mux)
    logic [7:0] r_xmc_reg_data;
    always_ff @(posedge clk)
        r_xmc_reg_data <= xmc_regs[xmc_reg_off];

    // Registered blitter register read. blt_cpu_rdata is combinational on
    // cpu_addr (blitter.sv uses cpu_addr directly for blt_sel + reg_off), so
    // this capture also needs cpu_ce gating — same bug class as r_vgc_cpu_rdata.
    logic [7:0] r_blt_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_blt_cpu_rdata <= blt_cpu_rdata;

    // Registered DMA register read — same rationale as r_blt_cpu_rdata.
    logic [7:0] r_dma_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_dma_cpu_rdata <= dma_cpu_rdata;

    // Registered VGC read data. Must be cpu_ce-gated like r_key_snapshot
    // used to be — otherwise the value captured during cpu_ce=0 cycles
    // (when Arlet has already moved cpu_addr past $A00F) overwrites the
    // correct value before the CPU samples DI on its next cpu_ce=1 edge.
    // See ASCII trace in project_key_fifo_in_progress.md.
    logic [7:0] r_vgc_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_vgc_cpu_rdata <= vgc_cpu_rdata;

    // ==========================================================================
    // File I/O register bank ($B9A0-$B9EF). CPU writes captured here;
    // fio_event pulses on CPU write of non-zero to $B9A0 (FioCmd).
    // Debug-bridge pokes to this range land in fio.sv via dbg_poke
    // (different offset calculation), NOT in main RAM.
    // ==========================================================================
    wire [7:0] fio_cpu_rdata;
    wire [7:0] fio_dbg_rdata;
    wire       dbg_poke_fio = dbg_poke_en &&
                              (dbg_poke_addr >= 16'hB9A0) &&
                              (dbg_poke_addr <= 16'hB9EF);
    wire       dbg_poke_vgc = dbg_poke_en &&
                              (((dbg_poke_addr >= 16'hA000) && (dbg_poke_addr <= 16'hA01F)) ||
                               ((dbg_poke_addr >= 16'hA040) && (dbg_poke_addr <= 16'hA0BF)) ||
                               ((dbg_poke_addr >= 16'hA0E0) && (dbg_poke_addr <= 16'hA0E7)) ||
                               ((dbg_poke_addr >= 16'hA0F0) && (dbg_poke_addr <= 16'hA0FF)));

    fio fio_inst (
        .clk       (clk),
        .rst       (custom_rst),
        .cpu_ce    (cpu_ce),
        .cpu_addr  (cpu_addr),
        .cpu_wdata (cpu_dout),
        .cpu_we    (cpu_we),
        .cpu_rdata (fio_cpu_rdata),
        .dbg_we    (dbg_poke_fio),
        .dbg_addr  (dbg_poke_addr[6:0] - 7'h20),   // $B9A0[6:0]=$20
        .dbg_wdata (dbg_poke_data),
        .dbg_raddr (dbg_peek_addr[6:0] - 7'h20),
        .dbg_rdata (fio_dbg_rdata),
        .fio_event (fio_event)
    );

    // Registered FIO read data. Same cpu_ce gating as the other
    // combinational MMIO reads.
    logic [7:0] r_fio_cpu_rdata;
    always_ff @(posedge clk)
        if (cpu_ce)
            r_fio_cpu_rdata <= fio_cpu_rdata;

    // Bus-master register reads share one slot in the cpu_din mux chain so
    // DMA doesn't add a LUT level to the already-long ram_a_dout path.
    // DMA ($BA63-$BA75) and blitter ($BA83-$BA9B) disjoint, so the sub-mux
    // is a simple 2:1 that resolves in parallel with the main chain.
    wire        r_bm_reg_sel   = r_dma_reg_sel | r_blt_reg_sel;
    wire [7:0]  r_bm_cpu_rdata = r_dma_reg_sel ? r_dma_cpu_rdata
                                               : r_blt_cpu_rdata;

    // CPU read data mux — two-level grouped design to keep the critical
    // path short. The previous 10-branch priority chain put ram_a_dout
    // (the else-case fallthrough) through all 10 muxes in series — ~10
    // LUT levels, the worst clk_pixel path. Splitting into three parallel
    // group muxes + one 3:1 final cuts that to ~6 LUT levels.
    //
    // Selects are mutually exclusive (disjoint address ranges), so the
    // within-group priority order is for readability only.
    //
    // Group A — XMC window + XMC regs + bus-master ($BA63-$BAA2).
    wire [7:0] xmc_group_data =
        (r_xmc_win_sel && r_xmc_win_enabled) ? xram_a_dout  :
         r_xmc_win_sel                        ? 8'hFF        :
         r_xmc_reg_sel                        ? r_xmc_reg_data :
                                                r_bm_cpu_rdata;
    wire xmc_group_sel = r_xmc_win_sel | r_xmc_reg_sel | r_bm_reg_sel;

    // Group B — VGC register space. SID register reads DELIBERATELY fall
    // through to the ROM / RAM group: $D400-$D43F overlaps BASIC ROM
    // (EhBASIC's array-handling code — LAB_1F45/1F48/1F7B — happens to
    // land at $D3FF-$D430), so an unconditional SID-read intercept
    // returns SID register values for legitimate opcode fetches into
    // that ROM block. CPU executes SID-reg-as-opcode, BRKs, and drops
    // back to Ready. The symptom is silent array failure (DIM/PRINT
    // returns no output) and caused the math.array-operations and
    // xram STASH/FETCH integration test failures. Removing SID from the
    // read mux makes BASIC PEEK($D400) return the ROM byte — the same
    // trade-off Avalonia makes. SID is still writable, per-register
    // shadow is still in sid_chip.sv for future use via an alternate
    // non-ROM-overlapping mirror.
    // $D440 (sid_cfg_reg) is also inside ROM — ROM byte at that offset
    // is $A2 = LDX #$00, part of LAB_1F7C (array-index multiply). An
    // interception would replace a live instruction with the config
    // register's stored byte. Fall through to ROM for all SID addresses.
    wire [7:0] io_group_data = r_fio_reg_sel ? r_fio_cpu_rdata
                                              : r_vgc_cpu_rdata;
    wire io_group_sel        = r_vgc_read_sel | r_fio_reg_sel;

    // Group C — ROM (active bank) + main RAM (default fallback).
    wire [7:0] mem_group_data =
        r_cpu_in_rom ? (r_ext_rom_active ? erom_b_dout : brom_b_dout)
                     : ram_a_dout;

    // Level-2 select: one 3-way mux. ram_a_dout critical path is now
    // mem_group_data → level-2 only (~2 LUT levels) instead of walking
    // a 10-deep else-if tail.
    always_comb begin
        if      (xmc_group_sel) cpu_din = xmc_group_data;
        else if (io_group_sel)  cpu_din = io_group_data;
        else                    cpu_din = mem_group_data;
    end

    // =========================================================================
    // Main RAM port A: CPU reads/writes + blitter writes
    // When blitter is writing to RAM, CPU is stalled — port A available
    // =========================================================================
    always_comb begin
        ram_a_addr = mem_addr;  // default: held address for reads
        ram_a_din  = cpu_dout;
        ram_a_we   = 1'b0;

        if (bm_ram_we && bm_ram_addr < ROM_BASE) begin
            // Bus master (blitter or DMA) write — CPU stalled
            ram_a_addr = bm_ram_addr;
            ram_a_din  = bm_ram_wdata;
            ram_a_we   = 1'b1;
        end else if (dbg_poke_en && !dbg_poke_vgc && !dbg_poke_fio && dbg_poke_addr < ROM_BASE) begin
            // Debug poke
            ram_a_addr = dbg_poke_addr;
            ram_a_din  = dbg_poke_data;
            ram_a_we   = 1'b1;
        end else if (cpu_we && cpu_active && cpu_addr < ROM_BASE &&
                     !xmc_win_sel && !xmc_reg_sel && !vgc_read_sel &&
                     !fio_reg_sel && !dma_reg_sel && !blt_reg_sel) begin
            // CPU write to RAM
            ram_a_we = 1'b1;
        end
    end

    // Keyboard input — handled entirely by VGC's sfifo (256-byte ring
    // buffer in rtl/thirdparty/sfifo.v). Previously duplicated here with a
    // single-entry latch that had the same drop-rate bug as vgc.sv's pre-
    // fifo char_in_reg. CHARIN reads now flow through r_vgc_read_sel →
    // vgc_cpu_rdata → char_in_reg which reads the sfifo head.

    // =========================================================================
    // Main RAM port B: tile DMA / blitter reads / debug reads
    // Priority: tile DMA > blitter > debug
    // =========================================================================
    always_comb begin
        if (tile_dma_active)
            ram_b_addr = tile_dma_addr;
        else if (bm_ram_read_active)
            ram_b_addr = bm_ram_addr;
        else
            ram_b_addr = dbg_peek_addr;
    end

    // Bus-master RAM read: blitter or DMA presents ram_addr during their
    // S_READ state. dpram port B output (ram_b_dout) is available next cycle.
    // Both FSMs have S_READ→S_WRITE pipelines that consume data in S_WRITE.
    wire bm_ram_read_active = ((blt_rdy == 1'b0) && !blt_ram_we)
                           || ((dma_rdy == 1'b0) && !dma_ram_we);
    wire [7:0] blt_ram_rdata  = ram_b_dout;
    wire [7:0] blt_xram_rdata = xram_a_dout;
    wire [7:0] dma_ram_rdata  = ram_b_dout;
    wire [7:0] dma_xram_rdata = xram_a_dout;

    // Tile DMA read: tile engine sets dma_addr on cycle N (registered output),
    // dpram port B outputs data on cycle N+1. The tile engine's dma_data_valid
    // flag goes high on cycle N+1, so data arrives exactly when needed.
    wire [7:0] tile_dma_data = ram_b_dout;

    // =========================================================================
    // Memory writes — non-RAM (XMC regs, ROM bank switching)
    // =========================================================================
    always_ff @(posedge clk) begin
        if (custom_rst) begin
            for (int i = 0; i < 64; i++) xmc_regs[i] <= 8'h00;
            xmc_regs[XMC_BANKS_REG] <= 8'd8;
            xmc_regs[XMC_WINCTL_REG] <= 8'h0F;
            ext_rom_active <= 0;
        end else begin
        if (cpu_we && cpu_active) begin
            if (xmc_reg_sel) begin
                if (xmc_reg_off != XMC_BANKS_REG)
                    xmc_regs[xmc_reg_off] <= cpu_dout;
            end
        end

        // ROM bank switching: write to $A03F toggles active ROM bank
        if (cpu_we && cpu_active && cpu_addr == REG_ROMSWAP) begin
            if (cpu_dout == ROMSWAP_EXT)
                ext_rom_active <= 1;
            else if (cpu_dout == ROMSWAP_BASIC)
                ext_rom_active <= 0;
        end

        end
    end

    // =========================================================================
    // Keyboard — write to RAM via port A needs special handling
    // Since we can't write to dpram directly from here, we snoop key input
    // and write CHARIN via a registered path
    // =========================================================================
    // Key input buffer — small register, written to RAM on next cycle
    logic [7:0] key_buf;
    logic       key_buf_valid;
    logic       clear_input;

    initial begin
        key_buf = 0;
        key_buf_valid = 0;
        clear_input = 0;
    end

    // For keyboard CHARIN: the CPU reads $A00F through the VGC, not through
    // main RAM. The VGC has its own char_in_reg. In top.sv the old code wrote
    // key_data to ram[CHARIN] and cleared it on CPU read — but the CPU read
    // path for $A00F goes through the VGC (it's in VGC address space $A000-$A01F).
    // So ram[CHARIN] was only used for the "is there a key" check.
    // With dpram we can't easily do this. Instead, keyboard input is handled
    // entirely by the VGC's char_in_reg. The ram[CHARIN] write was redundant.
    // We keep the clear_input logic for behavioral compatibility with simulation.
    // (On FPGA, the CPU reads CHARIN from VGC, not from RAM.)

`else
    // --- SIMULATION: keep original register arrays for Verilator compatibility ---

    logic [7:0] ram [0:65535];
    logic [7:0] basic_rom [0:16383];
    logic [7:0] ext_rom [0:16383];
    logic       ext_rom_active;

    localparam XRAM_SIZE = 524288;
    logic [7:0] xram [0:XRAM_SIZE-1];
    wire xram_busy = 1'b0;
    wire dbg_poke_vgc = dbg_poke_en &&
                        (((dbg_poke_addr >= 16'hA000) && (dbg_poke_addr <= 16'hA01F)) ||
                         ((dbg_poke_addr >= 16'hA040) && (dbg_poke_addr <= 16'hA0BF)) ||
                         ((dbg_poke_addr >= 16'hA0E0) && (dbg_poke_addr <= 16'hA0E7)) ||
                         ((dbg_poke_addr >= 16'hA0F0) && (dbg_poke_addr <= 16'hA0FF)));

    initial begin
        for (int i = 0; i < 65536; i++)
            ram[i] = 8'h00;
        for (int i = 0; i < 16384; i++) begin
            basic_rom[i] = 8'h00;
            ext_rom[i] = 8'h00;
        end
        $readmemh("rom/ehbasic.hex", basic_rom, 0, 16'h3FFF);
        $readmemh("rom/extension.hex", ext_rom, 0, 16'h3FFF);
        ext_rom_active = 0;
    end

    initial begin
        for (int i = 0; i < XRAM_SIZE; i++)
            xram[i] = 8'h00;
    end

    // XMC register file ($BA00-$BA3F)
    logic [7:0] xmc_regs [0:63];

    localparam XMC_BANKS_REG = 6'h0D;
    localparam XMC_WINCTL_REG = 6'h16;

    initial begin
        for (int i = 0; i < 64; i++) xmc_regs[i] = 0;
        xmc_regs[XMC_BANKS_REG] = 8'd8;
        xmc_regs[XMC_WINCTL_REG] = 8'h0F;
    end

    // XMC address decode
    wire xmc_reg_sel = (cpu_addr >= XMC_REG_BASE && cpu_addr <= XMC_REG_END);
    wire xmc_win_sel = (cpu_addr >= XMC_WIN_BASE && cpu_addr <= XMC_WIN_END);
    wire [5:0]  xmc_reg_off = cpu_addr[5:0];
    wire [1:0]  xmc_page    = cpu_addr[9:8];
    wire [7:0]  xmc_offset  = cpu_addr[7:0];

    // Keep the simulation-only memory model aligned with the synthesized
    // SDRAM-backed path: windows are page-aligned and ignore WnAL.
    logic [7:0] xmc_win_mid;
    logic [7:0] xmc_win_high;

    always_comb begin
        unique case (xmc_page)
            2'd0: begin xmc_win_mid = xmc_regs[6'h19]; xmc_win_high = xmc_regs[6'h1A]; end
            2'd1: begin xmc_win_mid = xmc_regs[6'h1C]; xmc_win_high = xmc_regs[6'h1D]; end
            2'd2: begin xmc_win_mid = xmc_regs[6'h1F]; xmc_win_high = xmc_regs[6'h20]; end
            2'd3: begin xmc_win_mid = xmc_regs[6'h22]; xmc_win_high = xmc_regs[6'h23]; end
        endcase
    end

    wire [18:0] xmc_addr_raw = {xmc_win_high[2:0], xmc_win_mid, xmc_offset};
    wire [18:0] xmc_addr = xmc_addr_raw & (XRAM_SIZE - 1);
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];

    // =========================================================================
    // Memory read mux — registered (Arlet 6502 expects DI one cycle after AB)
    // =========================================================================
    wire cpu_in_rom = (cpu_addr >= ROM_BASE);
    wire dma_reg_sel = (cpu_addr >= 16'hBA63 && cpu_addr <= 16'hBA75);
    wire blt_reg_sel = (cpu_addr >= 16'hBA83 && cpu_addr <= 16'hBA9B);
    wire sid1_reg_sel = (cpu_addr >= SID1_BASE && cpu_addr <= SID1_END);
    wire sid2_reg_sel = (cpu_addr >= SID2_BASE && cpu_addr <= SID2_END);
    wire sid_cfg_sel  = (cpu_addr == SID_CFG);
    wire vgc_read_sel = (cpu_addr >= 16'hA000 && cpu_addr <= 16'hA01F) ||
                        (cpu_addr >= 16'hA040 && cpu_addr <= 16'hA0BF) ||
                        (cpu_addr >= 16'hA0C0 && cpu_addr <= 16'hA0DF) ||
                        (cpu_addr >= 16'hA0E0 && cpu_addr <= 16'hA0E7) ||
                        (cpu_addr >= 16'hA0F0 && cpu_addr <= 16'hA0FF);

    always_ff @(posedge clk) begin
        if (xmc_win_sel && xmc_win_enabled)
            cpu_din <= xram[xmc_addr];
        else if (xmc_win_sel && !xmc_win_enabled)
            cpu_din <= 8'hFF;
        else if (xmc_reg_sel)
            cpu_din <= xmc_regs[xmc_reg_off];
        else if (dma_reg_sel)
            cpu_din <= dma_cpu_rdata;
        else if (blt_reg_sel)
            cpu_din <= blt_cpu_rdata;
        else if (sid1_reg_sel)
            cpu_din <= sid1_dout;
        else if (sid2_reg_sel)
            cpu_din <= sid2_dout;
        else if (sid_cfg_sel)
            cpu_din <= sid_cfg_reg;
        else if (vgc_read_sel)
            cpu_din <= vgc_cpu_rdata;
        else if (cpu_in_rom)
            cpu_din <= ext_rom_active ? ext_rom[cpu_addr - ROM_BASE]
                                     : basic_rom[cpu_addr - ROM_BASE];
        else
            cpu_din <= ram[cpu_addr];
    end

    // =========================================================================
    // Memory writes
    // =========================================================================
    always_ff @(posedge clk) begin
        if (custom_rst) begin
            for (int i = 0; i < 64; i++) xmc_regs[i] <= 8'h00;
            xmc_regs[XMC_BANKS_REG] <= 8'd8;
            xmc_regs[XMC_WINCTL_REG] <= 8'h0F;
            ext_rom_active <= 0;
        end else begin
        if (cpu_we) begin
            if (xmc_win_sel && xmc_win_enabled)
                xram[xmc_addr] <= cpu_dout;
            else if (xmc_reg_sel) begin
                if (xmc_reg_off != XMC_BANKS_REG)
                    xmc_regs[xmc_reg_off] <= cpu_dout;
            end else if (cpu_addr < ROM_BASE && !vgc_read_sel)
                ram[cpu_addr] <= cpu_dout;
        end

        // Bus-master (blitter/DMA) RAM/XRAM writes
        if (bm_ram_we && bm_ram_addr < ROM_BASE)
            ram[bm_ram_addr] <= bm_ram_wdata;
        if (bm_xram_we)
            xram[bm_xram_addr] <= bm_xram_wdata;

        // Debug ROM-load writes (mirrors the SYNTHESIS dpram port A path)
        if (dbg_rom_we) begin
            if (dbg_rom_idx == 1'b0)
                basic_rom[dbg_rom_addr] <= dbg_rom_data;
            else
                ext_rom[dbg_rom_addr] <= dbg_rom_data;
        end

        // ROM bank switching
        if (cpu_we && cpu_addr == REG_ROMSWAP) begin
            if (cpu_dout == ROMSWAP_EXT)
                ext_rom_active <= 1;
            else if (cpu_dout == ROMSWAP_BASIC)
                ext_rom_active <= 0;
        end

        end
    end

    // =========================================================================
    // Keyboard
    // =========================================================================
    always_ff @(posedge clk) begin
        if (key_valid)
            ram[CHARIN] <= key_data;
    end

    logic clear_input;
    always_ff @(posedge clk) begin
        clear_input <= (!cpu_we && cpu_addr == CHARIN && ram[CHARIN] != 0);
        if (clear_input && !key_valid)
            ram[CHARIN] <= 0;
    end

    // Combinational RAM/XRAM reads for blitter, DMA, and tile engine (sim only).
    // The bus-master is whichever of blitter/DMA is currently active; its
    // addr drives these reads. The idle master's addr is 0 and sees ram[0],
    // which the idle-state machine ignores.
    wire [7:0] blt_ram_rdata  = ram[bm_ram_addr];
    wire [7:0] blt_xram_rdata = xram[bm_xram_addr];
    wire [7:0] dma_ram_rdata  = ram[bm_ram_addr];
    wire [7:0] dma_xram_rdata = xram[bm_xram_addr];
    wire [7:0] tile_dma_data  = ram[tile_dma_addr];

    // Sim path: xram_sdram_inst isn't instantiated here (`ifdef SYNTHESIS`
    // branch owns it), so port A must be zeroed. Port B is always driven
    // by sid_curve_reader which sits outside this ifdef.
    assign sdram_addrA = 25'd0;
    assign sdram_dinA  = 8'd0;
    assign sdram_weA   = 1'b0;
    assign sdram_oeA   = 1'b0;
    assign fio_event   = 1'b0;

`endif

    // cpu_active: high on cycles when the CPU is actually executing.
`ifdef SYNTHESIS
    wire cpu_active = cpu_ce;
`else
    wire cpu_active = 1'b1;
`endif

    // =========================================================================
    // Blitter + DMA — share the same memory ports; mutually exclusive
    // via CPU RDY stall, muxed at top-level into bm_* signals.
    // =========================================================================
    wire [15:0] blt_ram_addr;
    wire [7:0]  blt_ram_wdata;
    wire        blt_ram_we;
    wire [18:0] blt_xram_addr;
    wire [7:0]  blt_xram_wdata;
    wire        blt_xram_we;
    wire        blt_xram_re;
    wire [2:0]  blt_vgc_space;
    wire [16:0] blt_vgc_addr;     // shared VGC-space address bus
    wire [7:0]  blt_vgc_wdata;
    wire        blt_vgc_we;
    wire        blt_vgc_re;
    wire [7:0]  blt_cpu_rdata;
    wire        blt_rdy;

    wire [15:0] dma_ram_addr;
    wire [7:0]  dma_ram_wdata;
    wire        dma_ram_we;
    wire [18:0] dma_xram_addr;
    wire [7:0]  dma_xram_wdata;
    wire        dma_xram_we;
    wire        dma_xram_re;
    wire [2:0]  dma_vgc_space;
    wire [16:0] dma_vgc_addr;
    wire [7:0]  dma_vgc_wdata;
    wire        dma_vgc_we;
    wire        dma_vgc_re;
    wire [7:0]  dma_cpu_rdata;
    wire        dma_rdy;

    // Bus-master mux — DMA takes priority when busy, blitter otherwise.
    // Both are idle (rdy=1) → signals default to 0 in each module.
    wire        dma_busy = !dma_rdy;
    wire [15:0] bm_ram_addr   = dma_busy ? dma_ram_addr   : blt_ram_addr;
    wire [7:0]  bm_ram_wdata  = dma_busy ? dma_ram_wdata  : blt_ram_wdata;
    wire        bm_ram_we     = dma_busy ? dma_ram_we     : blt_ram_we;
    wire [18:0] bm_xram_addr  = dma_busy ? dma_xram_addr  : blt_xram_addr;
    wire [7:0]  bm_xram_wdata = dma_busy ? dma_xram_wdata : blt_xram_wdata;
    wire        bm_xram_we    = dma_busy ? dma_xram_we    : blt_xram_we;
    wire        bm_xram_re    = dma_busy ? dma_xram_re    : blt_xram_re;
    wire [2:0]  bm_vgc_space  = dma_busy ? dma_vgc_space  : blt_vgc_space;
    wire [16:0] bm_vgc_addr   = dma_busy ? dma_vgc_addr   : blt_vgc_addr;
    wire [7:0]  bm_vgc_wdata  = dma_busy ? dma_vgc_wdata  : blt_vgc_wdata;
    wire        bm_vgc_we     = dma_busy ? dma_vgc_we     : blt_vgc_we;
    wire        bm_vgc_re     = dma_busy ? dma_vgc_re     : blt_vgc_re;
    // VGC rdata + memory reads feed both masters (the idle one ignores them)
    wire [7:0]  bm_vgc_rdata;
    wire [7:0]  blt_vgc_rdata = bm_vgc_rdata;
    wire [7:0]  dma_vgc_rdata = bm_vgc_rdata;

    blitter blt_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .cpu_addr   (cpu_addr),
        .cpu_wdata  (cpu_dout),
        .cpu_we     (cpu_we & cpu_active),
        .cpu_rdata  (blt_cpu_rdata),
        .cpu_re     (1'b0),
        .rdy_out    (blt_rdy),
        .ram_addr   (blt_ram_addr),
        .ram_rdata  (blt_ram_rdata),
        .ram_wdata  (blt_ram_wdata),
        .ram_we     (blt_ram_we),
        .xram_addr  (blt_xram_addr),
        .xram_rdata (blt_xram_rdata),
        .xram_wdata (blt_xram_wdata),
        .xram_we    (blt_xram_we),
        .xram_re    (blt_xram_re),
        .xram_busy  (xram_busy),
        .vgc_space  (blt_vgc_space),
        .vgc_addr   (blt_vgc_addr),
        .vgc_rdata  (blt_vgc_rdata),
        .vgc_wdata  (blt_vgc_wdata),
        .vgc_we     (blt_vgc_we),
        .vgc_re     (blt_vgc_re)
    );

    dma dma_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .cpu_addr   (cpu_addr),
        .cpu_wdata  (cpu_dout),
        .cpu_we     (cpu_we & cpu_active),
        .cpu_rdata  (dma_cpu_rdata),
        .cpu_re     (1'b0),
        .rdy_out    (dma_rdy),
        .ram_addr   (dma_ram_addr),
        .ram_rdata  (dma_ram_rdata),
        .ram_wdata  (dma_ram_wdata),
        .ram_we     (dma_ram_we),
        .xram_addr  (dma_xram_addr),
        .xram_rdata (dma_xram_rdata),
        .xram_wdata (dma_xram_wdata),
        .xram_we    (dma_xram_we),
        .xram_re    (dma_xram_re),
        .xram_busy  (xram_busy),
        .vgc_space  (dma_vgc_space),
        .vgc_addr   (dma_vgc_addr),
        .vgc_rdata  (dma_vgc_rdata),
        .vgc_wdata  (dma_vgc_wdata),
        .vgc_we     (dma_vgc_we),
        .vgc_re     (dma_vgc_re)
    );

    // =========================================================================
    // SID — dual 6581/8580 sound chips
    // =========================================================================

    // ~1 MHz clock enable from pixel clock: 25.175 MHz / 25 ~ 1.007 MHz
    logic [4:0] sid_clk_div;
    logic       sid_ce_1m;
    always_ff @(posedge clk) begin
        if (custom_rst) begin
            sid_clk_div <= 0;
            sid_ce_1m   <= 0;
        end else begin
            sid_ce_1m <= (sid_clk_div == 5'd24);
            sid_clk_div <= (sid_clk_div == 5'd24) ? 5'd0 : sid_clk_div + 1'd1;
        end
    end

    // SID config register ($D440): bit 0 = SID1 mode, bit 1 = SID2 mode
    logic [7:0] sid_cfg_reg;
    always_ff @(posedge clk) begin
        if (custom_rst)
            sid_cfg_reg <= 8'h00;
        else if (cpu_we && cpu_active && cpu_addr == SID_CFG)
            sid_cfg_reg <= cpu_dout;
    end

    // Chip select and address decode
    wire sid1_cs = cpu_we && cpu_active && (cpu_addr >= SID1_BASE && cpu_addr <= SID1_END);
    wire sid2_cs = cpu_we && cpu_active && (cpu_addr >= SID2_BASE && cpu_addr <= SID2_END);
    wire [7:0] sid1_dout, sid2_dout;

    // SID filter curve pulled from SDRAM (Phase 2.5 Step 3). One reader
    // serves both chips via SDRAM port B; each chip caches its own f0.
    wire [10:0] sid1_filter_fc, sid2_filter_fc;
    wire [15:0] sid1_filter_f0, sid2_filter_f0;

    sid_chip sid1_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .ce_1m      (sid_ce_1m),
        .mode       (sid_cfg_reg[0]),
        .cs         (sid1_cs),
        .we         (1'b1),
        .addr       (cpu_addr[4:0]),
        .din        (cpu_dout),
        .dout       (sid1_dout),
        .audio_out  (audio_l),
        .filter_fc_out(sid1_filter_fc),
        .filter_f0_in (sid1_filter_f0)
    );

    sid_chip sid2_inst (
        .clk        (clk),
        .rst        (custom_rst),
        .ce_1m      (sid_ce_1m),
        .mode       (sid_cfg_reg[1]),
        .cs         (sid2_cs),
        .we         (1'b1),
        .addr       (cpu_addr[4:0]),
        .din        (cpu_dout),
        .dout       (sid2_dout),
        .audio_out  (audio_r),
        .filter_fc_out(sid2_filter_fc),
        .filter_f0_in (sid2_filter_f0)
    );

    // Curve reader SDRAM port B — muxed with bridge preload below. Held
    // in reset whenever NovaHost holds the CPU reset so that it doesn't
    // issue reads while the bridge is preloading the filter curve.
    wire [24:0] curve_addrB;
    wire        curve_weB;
    wire [7:0]  curve_dinB;
    wire        curve_oeB;

    sid_curve_reader curve_reader_inst (
        .clk         (clk),
        .rst         (custom_rst | dbg_cpu_reset),
        .sid1_Fc     (sid1_filter_fc),
        .sid1_f0     (sid1_filter_f0),
        .sid2_Fc     (sid2_filter_fc),
        .sid2_f0     (sid2_filter_f0),
        .sdram_addrB (curve_addrB),
        .sdram_weB   (curve_weB),
        .sdram_dinB  (curve_dinB),
        .sdram_oeB   (curve_oeB),
        .sdram_doutB (sdram_doutB)
    );

    // Port B arbitration: bridge transfers win while active. The SID curve
    // reader is held in reset during boot preload, and direct XRAM streaming
    // uses short bridge-owned bursts while BASIC waits for FIO completion.
    wire brg_sdram_b_active = brg_sdram_b_we | brg_sdram_b_oe;
    assign sdram_addrB = brg_sdram_b_active ? brg_sdram_b_addr : curve_addrB;
    assign sdram_weB   = brg_sdram_b_we     ? 1'b1             : curve_weB;
    assign sdram_dinB  = brg_sdram_b_we     ? brg_sdram_b_din  : curve_dinB;
    assign sdram_oeB   = brg_sdram_b_oe     ? 1'b1             :
                         brg_sdram_b_we     ? 1'b0             : curve_oeB;

    // =========================================================================
    // CPU
    // =========================================================================
    wire vgc_rdy;
    wire vgc_irq;
    wire cpu_irq = ~irq_n | vgc_irq;
    wire cpu_nmi = ~nmi_n;
    wire cpu_rdy = blt_rdy & dma_rdy & vgc_rdy & ~dbg_pause & cpu_active;

    cpu cpu_inst (
        .clk    (clk),
        .reset  (cpu_reset),
        .AB     (cpu_addr),
        .DI     (cpu_din),
        .DO     (cpu_dout),
        .WE     (cpu_we),
        .IRQ    (cpu_irq),
        .NMI    (cpu_nmi),
        .RDY    (cpu_rdy),
        .dbg_resume (dbg_cpu_resume),
        .dbg_waiting(cpu_dbg_waiting),
        .dbg_stopped(cpu_dbg_stopped),
        .dbg_pc    (cpu_dbg_pc),
        .dbg_a     (cpu_dbg_a),
        .dbg_x     (cpu_dbg_x),
        .dbg_y     (cpu_dbg_y),
        .dbg_s     (cpu_dbg_s),
        .dbg_flags (cpu_dbg_flags),
        .dbg_state (cpu_dbg_state),
        .dbg_ir    (cpu_dbg_ir)
    );

    // =========================================================================
    // VGC — snoops bus, generates video
    // =========================================================================
    wire [7:0]  vgc_cpu_rdata;
    wire [7:0]  vgc_dbg_rdata;
    wire        vgc_dbg_owns;
    wire [15:0] tile_dma_addr;
    wire        tile_dma_active;

    vgc vgc_inst (
        .clk            (clk),
        .rst            (custom_rst),
        .video_rst      (rst),
        .cpu_ce         (cpu_active),
        .cpu_addr       (cpu_addr),
        .cpu_wdata      (cpu_dout),
        .cpu_rdata      (vgc_cpu_rdata),
        .cpu_we         (cpu_we),
        .cpu_re         (~cpu_we),
        .key_valid      (key_valid),
        .key_data       (key_data),
        .tile_dma_addr  (tile_dma_addr),
        .tile_dma_data  (tile_dma_data),
        .tile_dma_active(tile_dma_active),
        .blt_space      (bm_vgc_space),
        .blt_addr       (bm_vgc_addr),
        .blt_rdata      (bm_vgc_rdata),
        .blt_wdata      (bm_vgc_wdata),
        .blt_we         (bm_vgc_we),
        .blt_re         (bm_vgc_re),
        .dbg_addr       (dbg_peek_addr),
        .dbg_rdata      (vgc_dbg_rdata),
        .dbg_we         (dbg_poke_vgc),
        .dbg_waddr      (dbg_poke_addr),
        .dbg_wdata      (dbg_poke_data),
        .dbg_vmem_we    (dbg_vmem_we),
        .dbg_vmem_re    (dbg_vmem_re),
        .dbg_vmem_space (dbg_vmem_space),
        .dbg_vmem_addr  (dbg_vmem_addr),
        .dbg_vmem_wdata (dbg_vmem_data),
        .dbg_vmem_rdata (dbg_vmem_rdata),
        .vid_r          (vid_r),
        .vid_g          (vid_g),
        .vid_b          (vid_b),
        .vid_hsync      (vid_hsync),
        .vid_vsync      (vid_vsync),
        .vid_de         (vid_de),
        .irq_out        (vgc_irq),
        .rdy_out        (vgc_rdy),
        .sys_reset_req  (vgc_sys_reset_req)
`ifndef SYNTHESIS
        ,
        .dbg_vram_read_en(dbg_vram_read_en),
        .dbg_vram_space  (dbg_vram_space),
        .dbg_vram_addr   (dbg_vram_addr),
        .dbg_vram_rdata  (dbg_vram_rdata)
`endif
    );

    // =========================================================================
    // Debug/simulation interface
    // =========================================================================

`ifdef SYNTHESIS
    // On FPGA: debug peek mirrors the CPU-visible memory map.
    // Compute VGC ownership here in top.sv — cross-module ref failed (ea510bd
    // class), output port also failed to drive through synth. Duplicating the
    // small range-decode here is reliable.
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire dbg_vgc_regs  = (dbg_peek_addr >= 16'hA000 && dbg_peek_addr <= 16'hA01F) ||
                         (dbg_peek_addr >= 16'hA0F0 && dbg_peek_addr <= 16'hA0FF);
    wire dbg_vgc_tile  = (dbg_peek_addr >= 16'hA0C0 && dbg_peek_addr <= 16'hA0DF);
    wire dbg_vgc_spr   = (dbg_peek_addr >= 16'hA040 && dbg_peek_addr <= 16'hA0BF);
    wire dbg_vgc_vram  = (dbg_peek_addr >= 16'hA0E0 && dbg_peek_addr <= 16'hA0E4);
    wire dbg_vgc_dim   = (dbg_peek_addr == 16'hA0E5);
    wire dbg_vgc_text  = (dbg_peek_addr >= 16'hA0E6 && dbg_peek_addr <= 16'hA0E7);
    wire dbg_fio       = (dbg_peek_addr >= 16'hB9A0 && dbg_peek_addr <= 16'hB9EF);
    assign vgc_dbg_owns = dbg_vgc_regs | dbg_vgc_tile | dbg_vgc_spr | dbg_vgc_vram | dbg_vgc_dim | dbg_vgc_text;

    // For debug reads, RAM port B serves as the read port.
    // ROM reads use the same brom_b/erom_b ports (address set combinationally).
    // SID overlaps ROM and is write-only in the CPU-visible map, so debug reads
    // fall through to ROM/RAM instead of sampling live SID register shadows.
    wire [7:0] dbg_peek_data_mux = dbg_peek_en ? (vgc_dbg_owns ? vgc_dbg_rdata :
                                                  dbg_fio      ? fio_dbg_rdata :
                                                  dbg_in_rom   ? (ext_rom_active ? erom_b_dout : brom_b_dout) :
                                                                  ram_b_dout) : 8'h00;

    // Keep debug peeks off the main timing path. The bridge already waits for
    // BRAM read latency before sampling, so this register is protocol-neutral.
    always_ff @(posedge clk) begin
        dbg_peek_data <= dbg_peek_data_mux;
    end
`else
    // Simulation: full debug with combinational reads — ownership decoded
    // directly in top.sv (symmetric with synthesis path)
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire dbg_vgc_regs  = (dbg_peek_addr >= 16'hA000 && dbg_peek_addr <= 16'hA01F) ||
                         (dbg_peek_addr >= 16'hA0F0 && dbg_peek_addr <= 16'hA0FF);
    wire dbg_vgc_tile  = (dbg_peek_addr >= 16'hA0C0 && dbg_peek_addr <= 16'hA0DF);
    wire dbg_vgc_spr   = (dbg_peek_addr >= 16'hA040 && dbg_peek_addr <= 16'hA0BF);
    wire dbg_vgc_vram  = (dbg_peek_addr >= 16'hA0E0 && dbg_peek_addr <= 16'hA0E4);
    wire dbg_vgc_dim   = (dbg_peek_addr == 16'hA0E5);
    wire dbg_vgc_text  = (dbg_peek_addr >= 16'hA0E6 && dbg_peek_addr <= 16'hA0E7);
    assign vgc_dbg_owns = dbg_vgc_regs | dbg_vgc_tile | dbg_vgc_spr | dbg_vgc_vram | dbg_vgc_dim | dbg_vgc_text;
    wire [7:0] dbg_rom_read_data = ext_rom_active ? ext_rom[dbg_peek_addr - ROM_BASE]
                                                  : basic_rom[dbg_peek_addr - ROM_BASE];
    wire [7:0] dbg_peek_data_mux = dbg_peek_en ? (vgc_dbg_owns     ? vgc_dbg_rdata :
                                                  dbg_in_rom       ? dbg_rom_read_data :
                                                                     ram[dbg_peek_addr]) : 8'h00;

    always_ff @(posedge clk) begin
        dbg_peek_data <= dbg_peek_data_mux;
    end

    // Poke: write to main RAM on clock edge (ROM-protected)
    always_ff @(posedge clk) begin
        if (dbg_poke_en && !dbg_poke_vgc && dbg_poke_addr < ROM_BASE)
            ram[dbg_poke_addr] <= dbg_poke_data;
    end
`endif

    // CPU state — use proper debug output ports on the cpu module instead of
    // cross-module references which don't reliably survive synthesis.
    assign dbg_cpu_pc    = cpu_dbg_pc;
    assign dbg_cpu_a     = cpu_dbg_a;
    assign dbg_cpu_x     = cpu_dbg_x;
    assign dbg_cpu_y     = cpu_dbg_y;
    assign dbg_cpu_sp    = cpu_dbg_s;
    assign dbg_cpu_flags = cpu_dbg_flags;
    assign dbg_cpu_state = cpu_dbg_state;
    assign dbg_cpu_ir    = cpu_dbg_ir;
    assign dbg_cpu_addr  = cpu_addr;
    assign dbg_cpu_din   = cpu_din;
    assign dbg_cpu_dout  = cpu_dout;
    assign dbg_cpu_we    = cpu_we;
    assign dbg_cpu_rdy   = cpu_rdy;
    assign dbg_cpu_irq   = cpu_irq;
    assign dbg_cpu_nmi   = cpu_nmi;
    assign dbg_cpu_waiting = cpu_dbg_waiting;
    assign dbg_cpu_stopped = cpu_dbg_stopped;

endmodule
