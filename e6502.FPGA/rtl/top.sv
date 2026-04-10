// Top-level: 6502 CPU + VGC + Blitter + 64KB RAM + 512KB XRAM
// VGC snoops bus for register/charout writes, generates video
// Blitter performs 2D DMA across all memory spaces, stalls CPU via RDY
// XMC bank switching: 4×256-byte windows at $BC00-$BFFF into 512KB XRAM

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
    localparam XMC_WIN_BASE = 16'hBC00;  // 4×256-byte windows
    localparam XMC_WIN_END  = 16'hBFFF;

    // CPU bus
    wire [15:0] cpu_addr;
    wire [7:0]  cpu_dout;
    wire        cpu_we;
    logic [7:0] cpu_din;

    // =========================================================================
    // 64KB main memory (RAM + ROM) + extension ROM bank
    // =========================================================================
    logic [7:0] ram [0:65535];
    logic [7:0] basic_rom [0:16383];   // EhBASIC ROM backup
    logic [7:0] ext_rom [0:16383];     // Extension ROM
    logic       ext_rom_active;         // 1 = extension ROM mapped at $C000

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

    // =========================================================================
    // 512KB extended memory (XRAM) — external SRAM on real FPGA
    // =========================================================================
    logic [7:0] xram [0:524287];  // 512KB

    initial begin
        for (int i = 0; i < 524288; i++)
            xram[i] = 8'h00;
    end

    // XMC register file ($BA00-$BA3F)
    logic [7:0] xmc_regs [0:63];

    localparam XMC_BANKS_REG = 6'h0D;
    localparam XMC_WINCTL_REG = 6'h16;  // window enable: bit N = window N enabled

    initial begin
        for (int i = 0; i < 64; i++) xmc_regs[i] = 0;
        xmc_regs[XMC_BANKS_REG] = 8'd8;    // 8 × 64KB = 512KB
        xmc_regs[XMC_WINCTL_REG] = 8'h0F;  // all 4 windows enabled by default
    end

    // XMC address decode
    wire xmc_reg_sel = (cpu_addr >= XMC_REG_BASE && cpu_addr <= XMC_REG_END);
    wire xmc_win_sel = (cpu_addr >= XMC_WIN_BASE && cpu_addr <= XMC_WIN_END);
    wire [5:0]  xmc_reg_off = cpu_addr[5:0];
    wire [1:0]  xmc_page    = cpu_addr[9:8];
    wire [7:0]  xmc_offset  = cpu_addr[7:0];

    wire [18:0] xmc_addr = {xmc_regs[6'h1A + {xmc_page, 1'b0} + xmc_page],
                            xmc_regs[6'h19 + {xmc_page, 1'b0} + xmc_page],
                            xmc_offset};

    // Window enable check: bit N of WinCtl enables window N
    wire xmc_win_enabled = xmc_regs[XMC_WINCTL_REG][xmc_page];

    // =========================================================================
    // Memory read mux — registered (Arlet 6502 expects DI one cycle after AB)
    // ROM reads come from banked ROM arrays, not ram[]
    // =========================================================================
    wire cpu_in_rom = (cpu_addr >= ROM_BASE);
    wire blt_reg_sel = (cpu_addr >= 16'hBA83 && cpu_addr <= 16'hBA9B);

    always_ff @(posedge clk) begin
        if (xmc_win_sel && xmc_win_enabled)
            cpu_din <= xram[xmc_addr];
        else if (xmc_win_sel && !xmc_win_enabled)
            cpu_din <= 8'hFF;
        else if (xmc_reg_sel)
            cpu_din <= xmc_regs[xmc_reg_off];
        else if (blt_reg_sel)
            cpu_din <= blt_cpu_rdata;
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

        // ROM bank switching: write to $A03F toggles active ROM bank
        if (cpu_we && cpu_addr == REG_ROMSWAP) begin
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

    // Combinational RAM/XRAM reads for blitter
    wire [7:0] blt_ram_rdata  = ram[blt_ram_addr];
    wire [7:0] blt_xram_rdata = xram[blt_xram_addr];

    blitter blt_inst (
        .clk        (clk),
        .rst        (rst),
        .cpu_addr   (cpu_addr),
        .cpu_wdata  (cpu_dout),
        .cpu_we     (cpu_we),
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
    // CPU
    // =========================================================================
    cpu cpu_inst (
        .clk    (clk),
        .reset  (rst),
        .AB     (cpu_addr),
        .DI     (cpu_din),
        .DO     (cpu_dout),
        .WE     (cpu_we),
        .IRQ    (~irq_n),
        .NMI    (~nmi_n),
        .RDY    (blt_rdy & ~dbg_pause)
    );

    // =========================================================================
    // VGC — snoops bus, generates video
    // =========================================================================
    wire [7:0]  vgc_rdata_unused;
    wire [7:0]  vgc_dbg_rdata;
    wire        vgc_dbg_owns;
    wire [15:0] tile_dma_addr;
    wire        tile_dma_active;

    // Tile engine DMA read port — combinational read from main RAM
    wire [7:0] tile_dma_data = ram[tile_dma_addr];

    vgc vgc_inst (
        .clk            (clk),
        .rst            (rst),
        .cpu_ce         (1'b1),
        .cpu_addr       (cpu_addr),
        .cpu_wdata      (cpu_dout),
        .cpu_rdata      (vgc_rdata_unused),
        .cpu_we         (cpu_we),
        .cpu_re         (1'b0),
        .key_valid      (1'b0),
        .key_data       (8'h00),
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

    // Peek: route through VGC for VGC-owned addresses, ROM banks for ROM space, RAM otherwise
    // Mirrors Avalonia CompositeBusDevice routing
    assign vgc_dbg_owns = vgc_inst.dbg_owns;
    wire dbg_in_rom = (dbg_peek_addr >= ROM_BASE);
    wire [7:0] dbg_rom_data = ext_rom_active ? ext_rom[dbg_peek_addr - ROM_BASE]
                                             : basic_rom[dbg_peek_addr - ROM_BASE];
    assign dbg_peek_data = dbg_peek_en ? (vgc_dbg_owns ? vgc_dbg_rdata :
                                          dbg_in_rom   ? dbg_rom_data  :
                                                          ram[dbg_peek_addr]) : 8'h00;

    // Poke: write to main RAM on clock edge (ROM-protected)
    always_ff @(posedge clk) begin
        if (dbg_poke_en && dbg_poke_addr < ROM_BASE)
            ram[dbg_poke_addr] <= dbg_poke_data;
    end

    // CPU state: route from Arlet 6502 internal signals
    assign dbg_cpu_pc    = cpu_inst.PC;
    assign dbg_cpu_a     = cpu_inst.AXYS[0];  // A
    assign dbg_cpu_x     = cpu_inst.AXYS[1];  // X
    assign dbg_cpu_y     = cpu_inst.AXYS[2];  // Y
    assign dbg_cpu_sp    = cpu_inst.AXYS[3];  // S
    assign dbg_cpu_flags = {cpu_inst.N, cpu_inst.V, 2'b11,
                            cpu_inst.D, cpu_inst.I, cpu_inst.Z, cpu_inst.C};

endmodule
