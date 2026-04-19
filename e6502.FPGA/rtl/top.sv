// Top-level: 6502 CPU + VGC + Blitter + SID + 64KB RAM + 512KB XRAM
// VGC snoops bus for register/charout writes, generates video
// Blitter performs 2D DMA across all memory spaces, stalls CPU via RDY
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
    output logic [15:0] dbg_cpu_pc,
    output logic [7:0]  dbg_cpu_a,
    output logic [7:0]  dbg_cpu_x,
    output logic [7:0]  dbg_cpu_y,
    output logic [7:0]  dbg_cpu_sp,
    output logic [7:0]  dbg_cpu_flags
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
    always_ff @(posedge clk) begin
        if (rst) cpu_ce <= 0;
        else     cpu_ce <= ~cpu_ce;
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

    // EhBASIC ROM: 16KB
    logic [13:0] brom_b_addr;
    logic [7:0]  brom_b_dout;

    dpram #(.WIDTH(8), .DEPTH(16384), .INIT_FILE("rom/ehbasic.hex")) basic_rom_inst (
        .clk(clk),
        .addr_a(14'd0), .din_a(8'd0), .we_a(1'b0), .dout_a(),
        .addr_b(brom_b_addr), .dout_b(brom_b_dout)
    );

    // Extension ROM: 16KB
    logic [13:0] erom_b_addr;
    logic [7:0]  erom_b_dout;

    dpram #(.WIDTH(8), .DEPTH(16384), .INIT_FILE("rom/extension.hex")) ext_rom_inst (
        .clk(clk),
        .addr_a(14'd0), .din_a(8'd0), .we_a(1'b0), .dout_a(),
        .addr_b(erom_b_addr), .dout_b(erom_b_dout)
    );

    // XRAM: 4KB BRAM stub for initial bring-up
    localparam XRAM_SIZE = 4096;
    logic [11:0] xram_a_addr;
    logic [7:0]  xram_a_din;
    logic        xram_a_we;
    logic [7:0]  xram_a_dout;

    dpram #(.WIDTH(8), .DEPTH(XRAM_SIZE)) xram_inst (
        .clk(clk),
        .addr_a(xram_a_addr), .din_a(xram_a_din), .we_a(xram_a_we), .dout_a(xram_a_dout),
        .addr_b(12'd0), .dout_b()
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

    wire [18:0] xmc_addr_raw = {xmc_regs[6'h1A + {xmc_page, 1'b0} + xmc_page],
                                xmc_regs[6'h19 + {xmc_page, 1'b0} + xmc_page],
                                xmc_offset};
    wire [18:0] xmc_addr = xmc_addr_raw[18:0] & (XRAM_SIZE - 1);
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];

    // =========================================================================
    // Memory read — pipelined for dpram 1-cycle latency
    // mem_addr is held stable during CPU stall cycles (cpu_ce=0)
    // so dpram outputs don't get overwritten by the new address.
    // =========================================================================
    wire cpu_in_rom = (mem_addr >= ROM_BASE);
    wire blt_reg_sel = (mem_addr >= 16'hBA83 && mem_addr <= 16'hBA9B);
    wire sid1_reg_sel = (mem_addr >= SID1_BASE && mem_addr <= SID1_END);
    wire sid2_reg_sel = (mem_addr >= SID2_BASE && mem_addr <= SID2_END);
    wire sid_cfg_sel  = (mem_addr == SID_CFG);
    wire vgc_read_sel = (mem_addr >= 16'hA000 && mem_addr <= 16'hB99F);

    // Register the decode signals for next-cycle mux
    logic r_xmc_win_sel, r_xmc_win_enabled, r_xmc_reg_sel;
    logic r_blt_reg_sel, r_sid1_reg_sel, r_sid2_reg_sel, r_sid_cfg_sel;
    logic r_cpu_in_rom, r_ext_rom_active;
    logic r_vgc_read_sel;
    logic [5:0] r_xmc_reg_off;

    // Register decode signals every cycle — they align with dpram outputs
    // since both are captured from the same cpu_addr at the same posedge.
    always_ff @(posedge clk) begin
        r_xmc_win_sel     <= xmc_win_sel;
        r_xmc_win_enabled <= xmc_win_enabled;
        r_xmc_reg_sel     <= xmc_reg_sel;
        r_blt_reg_sel     <= blt_reg_sel;
        r_sid1_reg_sel    <= sid1_reg_sel;
        r_sid2_reg_sel    <= sid2_reg_sel;
        r_sid_cfg_sel     <= sid_cfg_sel;
        r_cpu_in_rom      <= cpu_in_rom;
        r_ext_rom_active  <= ext_rom_active;
        r_vgc_read_sel    <= vgc_read_sel;
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

    // XRAM: present address for CPU reads via port A
    // Port A serves both CPU reads and writes; writes are gated by xram_a_we
    always_comb begin
        xram_a_addr = xmc_addr[11:0];
        xram_a_din  = cpu_dout;
        xram_a_we   = 1'b0;

        // CPU write to XRAM
        if (cpu_we && cpu_active && xmc_win_sel && xmc_win_enabled)
            xram_a_we = 1'b1;

        // Blitter XRAM write
        if (blt_xram_we)  begin
            xram_a_addr = blt_xram_addr[11:0];
            xram_a_din  = blt_xram_wdata;
            xram_a_we   = 1'b1;
        end
    end

    // Registered XMC register read (for cpu_din mux)
    logic [7:0] r_xmc_reg_data;
    always_ff @(posedge clk)
        r_xmc_reg_data <= xmc_regs[xmc_reg_off];

    // Registered SID read data (SID dout is combinational)
    logic [7:0] r_sid1_dout, r_sid2_dout, r_sid_cfg_reg;
    always_ff @(posedge clk) begin
        r_sid1_dout   <= sid1_dout;
        r_sid2_dout   <= sid2_dout;
        r_sid_cfg_reg <= sid_cfg_reg;
    end

    // Registered blitter register read
    logic [7:0] r_blt_cpu_rdata;
    always_ff @(posedge clk)
        r_blt_cpu_rdata <= blt_cpu_rdata;

    // Registered VGC read data
    logic [7:0] r_vgc_cpu_rdata;
    always_ff @(posedge clk)
        r_vgc_cpu_rdata <= vgc_cpu_rdata;

    // Keyboard: snapshot key_reg on cpu_ce edges (before clear-on-read races)
    logic r_charin_sel;
    logic [7:0] r_key_snapshot;
    always_ff @(posedge clk) begin
        r_charin_sel <= (mem_addr == CHARIN);
        if (cpu_ce)
            r_key_snapshot <= key_reg;
    end

    // CPU read data mux — all sources are registered/synchronous.
    always_comb begin
        if (r_charin_sel)
            cpu_din = r_key_snapshot;
        else if (r_xmc_win_sel && r_xmc_win_enabled)
            cpu_din = xram_a_dout;
        else if (r_xmc_win_sel && !r_xmc_win_enabled)
            cpu_din = 8'hFF;
        else if (r_xmc_reg_sel)
            cpu_din = r_xmc_reg_data;
        else if (r_blt_reg_sel)
            cpu_din = r_blt_cpu_rdata;
        else if (r_sid1_reg_sel)
            cpu_din = r_sid1_dout;
        else if (r_sid2_reg_sel)
            cpu_din = r_sid2_dout;
        else if (r_sid_cfg_sel)
            cpu_din = r_sid_cfg_reg;
        else if (r_vgc_read_sel)
            cpu_din = r_vgc_cpu_rdata;
        else if (r_cpu_in_rom)
            cpu_din = r_ext_rom_active ? erom_b_dout : brom_b_dout;
        else
            cpu_din = ram_a_dout;
    end

    // =========================================================================
    // Main RAM port A: CPU reads/writes + blitter writes
    // When blitter is writing to RAM, CPU is stalled — port A available
    // =========================================================================
    always_comb begin
        ram_a_addr = mem_addr;  // default: held address for reads
        ram_a_din  = cpu_dout;
        ram_a_we   = 1'b0;

        if (blt_ram_we && blt_ram_addr < ROM_BASE) begin
            // Blitter write (CPU stalled)
            ram_a_addr = blt_ram_addr;
            ram_a_din  = blt_ram_wdata;
            ram_a_we   = 1'b1;
        end else if (dbg_poke_en && dbg_poke_addr < ROM_BASE) begin
            // Debug poke
            ram_a_addr = dbg_poke_addr;
            ram_a_din  = dbg_poke_data;
            ram_a_we   = 1'b1;
        end else if (cpu_we && cpu_active && cpu_addr < ROM_BASE &&
                     !xmc_win_sel && !xmc_reg_sel) begin
            // CPU write to RAM
            ram_a_we = 1'b1;
        end
    end

    // Keyboard input register — intercepts CPU reads of $A00F
    // instead of writing to RAM (which would conflict with port A)
    logic [7:0] key_reg;
    always_ff @(posedge clk) begin
        if (rst)
            key_reg <= 8'h00;
        else if (key_valid)
            key_reg <= (key_data == 8'h7F) ? 8'h08 : key_data; // DEL→BS
        else if (!cpu_we && cpu_active && cpu_addr == CHARIN && key_reg != 0)
            key_reg <= 8'h00;  // clear after CPU read
    end

    // =========================================================================
    // Main RAM port B: tile DMA / blitter reads / debug reads
    // Priority: tile DMA > blitter > debug
    // =========================================================================
    always_comb begin
        if (tile_dma_active)
            ram_b_addr = tile_dma_addr;
        else if (blt_ram_read_active)
            ram_b_addr = blt_ram_addr;
        else
            ram_b_addr = dbg_peek_addr;
    end

    // Blitter RAM read: the blitter presents ram_addr during S_READ state.
    // dpram port B output (ram_b_dout) is available next cycle.
    // The blitter FSM already has S_READ→S_WRITE pipeline, consuming data
    // in S_WRITE via mem_read_data → read_byte. We register this properly.
    wire blt_ram_read_active = (blt_rdy == 1'b0) && !blt_ram_we;
    wire [7:0] blt_ram_rdata  = ram_b_dout;
    wire [7:0] blt_xram_rdata = xram_a_dout;

    // Tile DMA read: tile engine sets dma_addr on cycle N (registered output),
    // dpram port B outputs data on cycle N+1. The tile engine's dma_data_valid
    // flag goes high on cycle N+1, so data arrives exactly when needed.
    wire [7:0] tile_dma_data = ram_b_dout;

    // =========================================================================
    // Memory writes — non-RAM (XMC regs, ROM bank switching)
    // =========================================================================
    always_ff @(posedge clk) begin
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

        // Reset restores BASIC ROM
        if (rst)
            ext_rom_active <= 0;
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

    wire [18:0] xmc_addr_raw = {xmc_regs[6'h1A + {xmc_page, 1'b0} + xmc_page],
                                xmc_regs[6'h19 + {xmc_page, 1'b0} + xmc_page],
                                xmc_offset};
    wire [18:0] xmc_addr = xmc_addr_raw[18:0] & (XRAM_SIZE - 1);
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];

    // =========================================================================
    // Memory read mux — registered (Arlet 6502 expects DI one cycle after AB)
    // =========================================================================
    wire cpu_in_rom = (cpu_addr >= ROM_BASE);
    wire blt_reg_sel = (cpu_addr >= 16'hBA83 && cpu_addr <= 16'hBA9B);
    wire sid1_reg_sel = (cpu_addr >= SID1_BASE && cpu_addr <= SID1_END);
    wire sid2_reg_sel = (cpu_addr >= SID2_BASE && cpu_addr <= SID2_END);
    wire sid_cfg_sel  = (cpu_addr == SID_CFG);

    always_ff @(posedge clk) begin
        if (xmc_win_sel && xmc_win_enabled)
            cpu_din <= xram[xmc_addr];
        else if (xmc_win_sel && !xmc_win_enabled)
            cpu_din <= 8'hFF;
        else if (xmc_reg_sel)
            cpu_din <= xmc_regs[xmc_reg_off];
        else if (blt_reg_sel)
            cpu_din <= blt_cpu_rdata;
        else if (sid1_reg_sel)
            cpu_din <= sid1_dout;
        else if (sid2_reg_sel)
            cpu_din <= sid2_dout;
        else if (sid_cfg_sel)
            cpu_din <= sid_cfg_reg;
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
        if (cpu_we) begin
            if (xmc_win_sel && xmc_win_enabled)
                xram[xmc_addr] <= cpu_dout;
            else if (xmc_reg_sel) begin
                if (xmc_reg_off != XMC_BANKS_REG)
                    xmc_regs[xmc_reg_off] <= cpu_dout;
            end else if (cpu_addr < ROM_BASE)
                ram[cpu_addr] <= cpu_dout;
        end

        // Blitter RAM/XRAM writes
        if (blt_ram_we && blt_ram_addr < ROM_BASE)
            ram[blt_ram_addr] <= blt_ram_wdata;
        if (blt_xram_we)
            xram[blt_xram_addr] <= blt_xram_wdata;

        // ROM bank switching
        if (cpu_we && cpu_addr == REG_ROMSWAP) begin
            if (cpu_dout == ROMSWAP_EXT)
                ext_rom_active <= 1;
            else if (cpu_dout == ROMSWAP_BASIC)
                ext_rom_active <= 0;
        end

        if (rst)
            ext_rom_active <= 0;
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

    // Combinational RAM/XRAM reads for blitter and tile DMA (simulation only)
    wire [7:0] blt_ram_rdata  = ram[blt_ram_addr];
    wire [7:0] blt_xram_rdata = xram[blt_xram_addr];
    wire [7:0] tile_dma_data  = ram[tile_dma_addr];

`endif

    // cpu_active: high on cycles when the CPU is actually executing.
`ifdef SYNTHESIS
    wire cpu_active = cpu_ce;
`else
    wire cpu_active = 1'b1;
`endif

    // =========================================================================
    // Blitter
    // =========================================================================
    wire [15:0] blt_ram_addr;
    wire [7:0]  blt_ram_wdata;
    wire        blt_ram_we;
    wire [18:0] blt_xram_addr;
    wire [7:0]  blt_xram_wdata;
    wire        blt_xram_we;
    wire [2:0]  blt_vgc_space;
    wire [15:0] blt_vgc_addr;
    wire [7:0]  blt_vgc_wdata;
    wire        blt_vgc_we;
    wire        blt_vgc_re;
    wire [7:0]  blt_vgc_rdata;
    wire [7:0]  blt_cpu_rdata;
    wire        blt_rdy;

    blitter blt_inst (
        .clk        (clk),
        .rst        (rst),
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
        .vgc_space  (blt_vgc_space),
        .vgc_addr   (blt_vgc_addr),
        .vgc_rdata  (blt_vgc_rdata),
        .vgc_wdata  (blt_vgc_wdata),
        .vgc_we     (blt_vgc_we),
        .vgc_re     (blt_vgc_re)
    );

    // =========================================================================
    // SID — dual 6581/8580 sound chips
    // =========================================================================

    // ~1 MHz clock enable from pixel clock: 25.175 MHz / 25 ~ 1.007 MHz
    logic [4:0] sid_clk_div;
    logic       sid_ce_1m;
    always_ff @(posedge clk) begin
        if (rst) begin
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
        if (rst)
            sid_cfg_reg <= 8'h00;
        else if (cpu_we && cpu_active && cpu_addr == SID_CFG)
            sid_cfg_reg <= cpu_dout;
    end

    // Chip select and address decode
    wire sid1_cs = cpu_we && cpu_active && (cpu_addr >= SID1_BASE && cpu_addr <= SID1_END);
    wire sid2_cs = cpu_we && cpu_active && (cpu_addr >= SID2_BASE && cpu_addr <= SID2_END);
    wire sid1_rd = !cpu_we && (cpu_addr >= SID1_BASE && cpu_addr <= SID1_END);
    wire sid2_rd = !cpu_we && (cpu_addr >= SID2_BASE && cpu_addr <= SID2_END);

    wire [7:0] sid1_dout, sid2_dout;

    sid_chip sid1_inst (
        .clk        (clk),
        .rst        (rst),
        .ce_1m      (sid_ce_1m),
        .mode       (sid_cfg_reg[0]),
        .cs         (sid1_cs),
        .we         (1'b1),
        .addr       (cpu_addr[4:0]),
        .din        (cpu_dout),
        .dout       (sid1_dout),
        .audio_out  (audio_l)
    );

    sid_chip sid2_inst (
        .clk        (clk),
        .rst        (rst),
        .ce_1m      (sid_ce_1m),
        .mode       (sid_cfg_reg[1]),
        .cs         (sid2_cs),
        .we         (1'b1),
        .addr       (cpu_addr[4:0]),
        .din        (cpu_dout),
        .dout       (sid2_dout),
        .audio_out  (audio_r)
    );

    // =========================================================================
    // CPU
    // =========================================================================
    wire [15:0] cpu_dbg_pc;
    wire  [7:0] cpu_dbg_a, cpu_dbg_x, cpu_dbg_y, cpu_dbg_s, cpu_dbg_flags;

    cpu cpu_inst (
        .clk    (clk),
        .reset  (rst),
        .AB     (cpu_addr),
        .DI     (cpu_din),
        .DO     (cpu_dout),
        .WE     (cpu_we),
        .IRQ    (~irq_n),
        .NMI    (~nmi_n),
        .RDY    (blt_rdy & ~dbg_pause & cpu_active),
        .dbg_pc    (cpu_dbg_pc),
        .dbg_a     (cpu_dbg_a),
        .dbg_x     (cpu_dbg_x),
        .dbg_y     (cpu_dbg_y),
        .dbg_s     (cpu_dbg_s),
        .dbg_flags (cpu_dbg_flags)
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
        .rst            (rst),
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
        .blt_space      (blt_vgc_space),
        .blt_addr       (blt_vgc_addr),
        .blt_rdata      (blt_vgc_rdata),
        .blt_wdata      (blt_vgc_wdata),
        .blt_we         (blt_vgc_we),
        .blt_re         (blt_vgc_re),
        .dbg_addr       (dbg_peek_addr),
        .dbg_rdata      (vgc_dbg_rdata),
        .vid_r          (vid_r),
        .vid_g          (vid_g),
        .vid_b          (vid_b),
        .vid_hsync      (vid_hsync),
        .vid_vsync      (vid_vsync),
        .vid_de         (vid_de),
        .irq_out        ()
    );

    // =========================================================================
    // Debug/simulation interface
    // =========================================================================

`ifdef SYNTHESIS
    // On FPGA: debug peek routed through VGC, SID, ROM dprams, or main RAM dpram
    assign vgc_dbg_owns = vgc_inst.dbg_owns;
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire dbg_sid1   = (dbg_peek_addr >= SID1_BASE && dbg_peek_addr <= SID1_END);
    wire dbg_sid2   = (dbg_peek_addr >= SID2_BASE && dbg_peek_addr <= SID2_END);
    wire dbg_sidcfg = (dbg_peek_addr == SID_CFG);

    // For debug reads, RAM port B serves as the read port.
    // ROM reads use the same brom_b/erom_b ports (address set combinationally).
    // This is acceptable since debug is not used on real FPGA hardware.
    assign dbg_peek_data = dbg_peek_en ? (vgc_dbg_owns ? vgc_dbg_rdata :
                                          dbg_sid1     ? sid1_dout     :
                                          dbg_sid2     ? sid2_dout     :
                                          dbg_sidcfg   ? sid_cfg_reg   :
                                          dbg_in_rom   ? (ext_rom_active ? erom_b_dout : brom_b_dout) :
                                                          ram_b_dout) : 8'h00;
`else
    // Simulation: full debug with combinational reads
    assign vgc_dbg_owns = vgc_inst.dbg_owns;
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire dbg_sid1   = (dbg_peek_addr >= SID1_BASE && dbg_peek_addr <= SID1_END);
    wire dbg_sid2   = (dbg_peek_addr >= SID2_BASE && dbg_peek_addr <= SID2_END);
    wire dbg_sidcfg = (dbg_peek_addr == SID_CFG);
    wire [7:0] dbg_rom_data = ext_rom_active ? ext_rom[dbg_peek_addr - ROM_BASE]
                                             : basic_rom[dbg_peek_addr - ROM_BASE];
    assign dbg_peek_data = dbg_peek_en ? (vgc_dbg_owns ? vgc_dbg_rdata :
                                          dbg_sid1     ? sid1_dout     :
                                          dbg_sid2     ? sid2_dout     :
                                          dbg_sidcfg   ? sid_cfg_reg   :
                                          dbg_in_rom   ? dbg_rom_data  :
                                                          ram[dbg_peek_addr]) : 8'h00;

    // Poke: write to main RAM on clock edge (ROM-protected)
    always_ff @(posedge clk) begin
        if (dbg_poke_en && dbg_poke_addr < ROM_BASE)
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

endmodule
