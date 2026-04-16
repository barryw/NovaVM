// Tile Engine — NES-style tile map renderer
// 256 tiles (8×8 or 16×16), 4 nametables with mirroring, 16 sub-palettes
// Registers at $A0C0-$A0DF, commands triggered via $A0C7
// Outputs per-pixel color + priority for VGC compositing
//
// Memory uses dpram instances for BRAM inference.
// Rendering pipeline: 4-stage prefetch to account for 1-cycle BRAM latency.

module tile_engine (
    input  logic        clk,
    input  logic        rst,

    // CPU bus (snooped from main bus)
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    output logic [7:0]  cpu_rdata,
    input  logic        cpu_re,

    // CPU RAM read port (for DMA: TileDef, TileLoad, etc.)
    output logic [15:0] dma_addr,
    input  logic [7:0]  dma_data,
    output logic        dma_active,

    // Blitter access port (tile_data only)
    input  logic [14:0] blt_tile_addr,
    input  logic [7:0]  blt_tile_wdata,
    input  logic        blt_tile_we,
    input  logic        blt_tile_re,
    output logic [7:0]  blt_tile_rdata,

    // Scanline pixel interface (active during rendering)
    input  logic [8:0]  pixel_x,      // 0-319
    input  logic [7:0]  pixel_y,      // 0-199
    input  logic        pixel_valid,  // in active gfx area
    output logic [11:0] tile_rgb,     // 12-bit RGB from sub-palette
    output logic [1:0]  tile_opaque   // 0=transparent, 1=behind sprites, 2=in front
);

    // =========================================================================
    // Constants
    // =========================================================================
    localparam TILE_REG_BASE = 16'hA0C0;
    localparam TILE_REG_END  = 16'hA0DF;

    // Register offsets from TILE_REG_BASE
    localparam TR_CONFIG     = 0;   // $A0C0: bit0=size(0=8x8,1=16x16), bits2-1=mirror
    localparam TR_TRANS      = 1;   // $A0C1: transparent color index (0-15)
    localparam TR_SCROLLXL   = 2;   // $A0C2
    localparam TR_SCROLLXH   = 3;   // $A0C3
    localparam TR_SCROLLYL   = 4;   // $A0C4
    localparam TR_SCROLLYH   = 5;   // $A0C5
    localparam TR_STATUS     = 6;   // $A0C6: read-only status
    localparam TR_CMD        = 7;   // $A0C7: write triggers command
    localparam TR_P0         = 8;   // $A0C8
    localparam TR_P1         = 9;   // $A0C9
    localparam TR_P2         = 10;  // $A0CA
    localparam TR_P3         = 11;  // $A0CB
    localparam TR_ADDRL      = 12;  // $A0CC
    localparam TR_ADDRH      = 13;  // $A0CD
    localparam TR_PALP0      = 14;  // $A0CE: sub-palette index
    localparam TR_PALP1      = 15;  // $A0CF: color index
    localparam TR_COLL       = 16;  // $A0D0: collision low (read-clears)
    localparam TR_COLH       = 17;  // $A0D1: collision high (read-clears)
    localparam TR_PEEKVAL    = 18;  // $A0D2: peek tile index
    localparam TR_PEEKATTR   = 19;  // $A0D3: peek attribute

    // Tile commands
    localparam TCMD_DEF      = 8'h01;
    localparam TCMD_DEFBULK  = 8'h02;
    localparam TCMD_PUT      = 8'h03;
    localparam TCMD_ATTR     = 8'h04;
    localparam TCMD_FILL     = 8'h05;
    localparam TCMD_ROW      = 8'h06;
    localparam TCMD_COL      = 8'h07;
    localparam TCMD_LOAD     = 8'h08;
    localparam TCMD_ALOAD    = 8'h09;
    localparam TCMD_PAL      = 8'h0A;
    localparam TCMD_PALC     = 8'h0B;
    localparam TCMD_PEEK     = 8'h0C;
    localparam TCMD_ROWATTR  = 8'h0D;
    localparam TCMD_COLATTR  = 8'h0E;
    localparam TCMD_CLS      = 8'h0F;
    localparam TCMD_BUFFILL  = 8'h10;
    localparam TCMD_BUFSET   = 8'h11;
    localparam TCMD_BUFRANGE = 8'h12;
    localparam TCMD_BUFPUT   = 8'h13;

    // Mirror modes
    localparam MIRROR_H      = 2'd0;  // horizontal: NT0=NT1, NT2=NT3
    localparam MIRROR_V      = 2'd1;  // vertical: NT0=NT2, NT1=NT3
    localparam MIRROR_FOUR   = 2'd2;  // all independent
    localparam MIRROR_SINGLE = 2'd3;  // all = NT0

    // Tile/nametable dimensions
    localparam NT_COLS = 40;
    localparam NT_ROWS = 25;
    localparam NT_SIZE = NT_COLS * NT_ROWS;  // 1000

    localparam GFX_W = 320;
    localparam GFX_H = 200;

    // Tile sizes
    localparam TILE_SIZE_8   = 32;    // 8×8 × 4-bit = 32 bytes
    localparam TILE_SIZE_16  = 128;   // 16×16 × 4-bit = 128 bytes

    // =========================================================================
    // Registers (small — kept as register file)
    // =========================================================================
    logic [7:0] tregs [0:31];   // raw register file

    // Decoded config
    logic        tile_size16;   // 0=8×8, 1=16×16
    logic [1:0]  mirror_mode;
    logic [3:0]  trans_color;
    logic [15:0] scroll_x, scroll_y;
    logic [7:0]  status_reg;
    logic [7:0]  peek_val, peek_attr;
    logic [15:0] collision_bits;  // sprite-tile collision (read-clears)

    // Column buffer (25 entries — kept as registers)
    logic [7:0]  col_buffer [0:NT_ROWS-1];

    initial begin
        for (int i = 0; i < 32; i++) tregs[i] = 0;
        // Initialize sub-palette 0 to match VGC palette (done via pal_ram init file or initial block)
        for (int i = 0; i < NT_ROWS; i++) col_buffer[i] = 0;
        tile_size16 = 0; mirror_mode = 0; trans_color = 0;
        scroll_x = 0; scroll_y = 0; status_reg = 0;
        peek_val = 0; peek_attr = 0; collision_bits = 0;
    end

    // =========================================================================
    // BRAM instances via dpram
    // =========================================================================

    // --- tile_data: 32K × 8-bit ---
    logic [14:0] td_addr_a;
    logic [7:0]  td_din_a;
    logic        td_we_a;
    logic [7:0]  td_dout_a;
    logic [14:0] td_addr_b;
    logic [7:0]  td_dout_b;

    dpram #(.WIDTH(8), .DEPTH(32768)) tile_data_ram (
        .clk(clk),
        .addr_a(td_addr_a), .din_a(td_din_a), .we_a(td_we_a), .dout_a(td_dout_a),
        .addr_b(td_addr_b), .dout_b(td_dout_b)
    );

    // --- nametable: 4K × 8-bit (4000 entries used) ---
    logic [11:0] nt_addr_a;
    logic [7:0]  nt_din_a;
    logic        nt_we_a;
    logic [7:0]  nt_dout_a;
    logic [11:0] nt_addr_b;
    logic [7:0]  nt_dout_b;

    dpram #(.WIDTH(8), .DEPTH(4096)) nametable_ram (
        .clk(clk),
        .addr_a(nt_addr_a), .din_a(nt_din_a), .we_a(nt_we_a), .dout_a(nt_dout_a),
        .addr_b(nt_addr_b), .dout_b(nt_dout_b)
    );

    // --- attr_table: 4K × 8-bit (4000 entries used) ---
    logic [11:0] at_addr_a;
    logic [7:0]  at_din_a;
    logic        at_we_a;
    logic [7:0]  at_dout_a;
    logic [11:0] at_addr_b;
    logic [7:0]  at_dout_b;

    dpram #(.WIDTH(8), .DEPTH(4096)) attr_table_ram (
        .clk(clk),
        .addr_a(at_addr_a), .din_a(at_din_a), .we_a(at_we_a), .dout_a(at_dout_a),
        .addr_b(at_addr_b), .dout_b(at_dout_b)
    );

    // --- pal_ram: 256 × 12-bit ---
    logic [7:0]  pr_addr_a;
    logic [11:0] pr_din_a;
    logic        pr_we_a;
    logic [11:0] pr_dout_a;
    logic [7:0]  pr_addr_b;
    logic [11:0] pr_dout_b;

    dpram #(.WIDTH(12), .DEPTH(256), .INIT_FILE("rom/tile_pal0.hex")) pal_ram_inst (
        .clk(clk),
        .addr_a(pr_addr_a), .din_a(pr_din_a), .we_a(pr_we_a), .dout_a(pr_dout_a),
        .addr_b(pr_addr_b), .dout_b(pr_dout_b)
    );

    // =========================================================================
    // Address decode
    // =========================================================================
    wire tile_reg_sel = (cpu_addr >= TILE_REG_BASE && cpu_addr <= TILE_REG_END);
    wire [4:0] reg_off = cpu_addr[4:0];

    // =========================================================================
    // Read mux
    // =========================================================================
    always_comb begin
        cpu_rdata = 8'h00;
        if (tile_reg_sel) begin
            case (reg_off)
                TR_CONFIG:   cpu_rdata = {5'b0, mirror_mode, tile_size16};
                TR_TRANS:    cpu_rdata = {4'b0, trans_color};
                TR_SCROLLXL: cpu_rdata = scroll_x[7:0];
                TR_SCROLLXH: cpu_rdata = scroll_x[15:8];
                TR_SCROLLYL: cpu_rdata = scroll_y[7:0];
                TR_SCROLLYH: cpu_rdata = scroll_y[15:8];
                TR_STATUS:   cpu_rdata = status_reg;
                TR_CMD:      cpu_rdata = {7'b0, dma_active};
                TR_COLL:     cpu_rdata = collision_bits[7:0];
                TR_COLH:     cpu_rdata = collision_bits[15:8];
                TR_PEEKVAL:  cpu_rdata = peek_val;
                TR_PEEKATTR: cpu_rdata = peek_attr;
                default:     cpu_rdata = tregs[reg_off];
            endcase
        end
    end

    // =========================================================================
    // DMA state machine
    // =========================================================================
    typedef enum logic [4:0] {
        DMA_IDLE,
        DMA_TILE_DEF,       // define tiles from RAM
        DMA_NT_LOAD,        // load nametable from RAM
        DMA_AT_LOAD,        // load attribute table from RAM
        DMA_PAL_LOAD,       // load sub-palette from RAM
        DMA_ROW_LOAD,       // load nametable row from RAM
        DMA_COL_LOAD,       // load nametable column from RAM
        DMA_ROWATTR_LOAD,   // load attribute row from RAM
        DMA_COLATTR_LOAD,   // load attribute column from RAM
        DMA_CLS,            // clear all nametables + attrs (serialized)
        DMA_FILL,           // fill one nametable (serialized, 1000 cycles)
        DMA_BUFPUT,         // write col_buffer to nametable column (serialized, 25 cycles)
        DMA_PEEK_NT,        // peek: read nametable (1 cycle latency)
        DMA_PEEK_AT,        // peek: read attr_table (1 cycle latency)
        DMA_PEEK_DONE       // peek: capture results
    } dma_state_t;

    dma_state_t dma_state;
    logic [15:0] dma_src;       // source address in CPU RAM
    logic [15:0] dma_count;     // bytes remaining
    logic [15:0] dma_dst;       // destination offset in tile memory
    logic [1:0]  dma_nt;        // target nametable (0-3)
    logic [5:0]  dma_col_row;   // column X or row Y for row/col operations
    logic [4:0]  dma_col_idx;   // index within column (0-24)
    logic        dma_data_valid; // pipeline: data from RAM is valid this clock
    logic [7:0]  dma_pal_phase; // phase within palette load (0=R, 1=G, 2=B)
    logic [7:0]  dma_pal_r;     // captured R byte
    logic [7:0]  dma_pal_g;     // captured G byte
    logic [3:0]  dma_pal_sub;   // sub-palette being loaded
    logic [3:0]  dma_pal_cidx;  // color index within palette
    logic [7:0]  dma_fill_val;  // tile value for fill
    logic [11:0] dma_fill_addr; // current write address for fill/cls

    assign dma_active = (dma_state != DMA_IDLE);

    initial begin
        dma_state = DMA_IDLE;
        dma_src = 0; dma_count = 0; dma_dst = 0;
        dma_nt = 0; dma_col_row = 0; dma_col_idx = 0;
        dma_data_valid = 0; dma_pal_phase = 0;
        dma_pal_r = 0; dma_pal_g = 0;
        dma_pal_sub = 0; dma_pal_cidx = 0;
        dma_fill_val = 0; dma_fill_addr = 0;
    end

    // =========================================================================
    // Mirror resolution
    // =========================================================================
    function automatic logic [1:0] resolve_nt(input logic [1:0] nt_col, input logic [1:0] nt_row);
        logic [1:0] raw_nt;
        raw_nt = {nt_row[0], nt_col[0]};  // 0=TL, 1=TR, 2=BL, 3=BR
        case (mirror_mode)
            MIRROR_H:      resolve_nt = (raw_nt == 2'd1) ? 2'd0 : (raw_nt == 2'd3) ? 2'd2 : raw_nt;
            MIRROR_V:      resolve_nt = (raw_nt == 2'd2) ? 2'd0 : (raw_nt == 2'd3) ? 2'd1 : raw_nt;
            MIRROR_FOUR:   resolve_nt = raw_nt;
            MIRROR_SINGLE: resolve_nt = 2'd0;
        endcase
    endfunction

    // =========================================================================
    // Port A mux for all 4 BRAMs
    // Priority: DMA/command writes > blitter write > blitter read > idle
    // =========================================================================

    // Signals for command-initiated single writes (PUT, ATTR, PALC, etc.)
    logic        cmd_nt_we, cmd_at_we, cmd_pal_we;
    logic [11:0] cmd_nt_addr, cmd_at_addr;
    logic [7:0]  cmd_nt_din, cmd_at_din;
    logic [7:0]  cmd_pal_addr_reg;
    logic [11:0] cmd_pal_din;

    // Blitter read data latch (1-cycle latency)
    logic        blt_tile_rd_pending;
    logic [7:0]  blt_tile_rd_latch;

    always_comb begin
        // Defaults: no writes, address idle
        td_addr_a = 15'd0;
        td_din_a  = 8'd0;
        td_we_a   = 1'b0;
        nt_addr_a = 12'd0;
        nt_din_a  = 8'd0;
        nt_we_a   = 1'b0;
        at_addr_a = 12'd0;
        at_din_a  = 8'd0;
        at_we_a   = 1'b0;
        pr_addr_a = 8'd0;
        pr_din_a  = 12'd0;
        pr_we_a   = 1'b0;

        case (dma_state)
            DMA_TILE_DEF: begin
                if (dma_data_valid) begin
                    td_addr_a = dma_dst[14:0];
                    td_din_a  = dma_data;
                    td_we_a   = 1'b1;
                end
            end

            DMA_NT_LOAD: begin
                if (dma_data_valid) begin
                    nt_addr_a = 12'(dma_nt * NT_SIZE) + {2'b0, dma_dst[9:0]};
                    nt_din_a  = dma_data;
                    nt_we_a   = 1'b1;
                end
            end

            DMA_AT_LOAD: begin
                if (dma_data_valid) begin
                    at_addr_a = 12'(dma_nt * NT_SIZE) + {2'b0, dma_dst[9:0]};
                    at_din_a  = dma_data;
                    at_we_a   = 1'b1;
                end
            end

            DMA_ROW_LOAD: begin
                if (dma_data_valid) begin
                    nt_addr_a = 12'(dma_nt * NT_SIZE + dma_col_row * NT_COLS) + {6'b0, dma_dst[5:0]};
                    nt_din_a  = dma_data;
                    nt_we_a   = 1'b1;
                end
            end

            DMA_COL_LOAD: begin
                if (dma_data_valid) begin
                    nt_addr_a = 12'(dma_nt * NT_SIZE) + 12'(dma_col_idx) * 12'd40 + {6'b0, dma_col_row[5:0]};
                    nt_din_a  = dma_data;
                    nt_we_a   = 1'b1;
                end
            end

            DMA_ROWATTR_LOAD: begin
                if (dma_data_valid) begin
                    at_addr_a = 12'(dma_nt * NT_SIZE + dma_col_row * NT_COLS) + {6'b0, dma_dst[5:0]};
                    at_din_a  = dma_data;
                    at_we_a   = 1'b1;
                end
            end

            DMA_COLATTR_LOAD: begin
                if (dma_data_valid) begin
                    at_addr_a = 12'(dma_nt * NT_SIZE) + 12'(dma_col_idx) * 12'd40 + {6'b0, dma_col_row[5:0]};
                    at_din_a  = dma_data;
                    at_we_a   = 1'b1;
                end
            end

            DMA_PAL_LOAD: begin
                if (dma_data_valid && dma_pal_phase == 8'd2) begin
                    pr_addr_a = {dma_pal_sub, dma_pal_cidx};
                    pr_din_a  = {dma_pal_r[7:4], dma_pal_g[7:4], dma_data[7:4]};
                    pr_we_a   = 1'b1;
                end
            end

            DMA_CLS: begin
                if (dma_fill_addr < 4000) begin
                    nt_addr_a = dma_fill_addr;
                    nt_din_a  = 8'd0;
                    nt_we_a   = 1'b1;
                    at_addr_a = dma_fill_addr;
                    at_din_a  = 8'd0;
                    at_we_a   = 1'b1;
                end
            end

            DMA_FILL: begin
                if (dma_count < NT_SIZE) begin
                    nt_addr_a = 12'(dma_nt * NT_SIZE) + dma_count[11:0];
                    nt_din_a  = dma_fill_val;
                    nt_we_a   = 1'b1;
                end
            end

            DMA_BUFPUT: begin
                if (dma_col_idx < NT_ROWS) begin
                    nt_addr_a = 12'(dma_nt * NT_SIZE) + 12'(dma_col_idx) * 12'd40 + {6'b0, dma_col_row[5:0]};
                    nt_din_a  = col_buffer[dma_col_idx];
                    nt_we_a   = 1'b1;
                end
            end

            DMA_PEEK_NT: begin
                // Read nametable — address is set, wait for data
                nt_addr_a = cmd_nt_addr;
                at_addr_a = cmd_at_addr;
            end

            DMA_PEEK_AT: begin
                // Read attr — address still set, capture nt result
                nt_addr_a = cmd_nt_addr;
                at_addr_a = cmd_at_addr;
            end

            DMA_PEEK_DONE: ;

            default: begin
                // DMA_IDLE: handle single-cycle command writes and blitter
                if (cmd_nt_we) begin
                    nt_addr_a = cmd_nt_addr;
                    nt_din_a  = cmd_nt_din;
                    nt_we_a   = 1'b1;
                end
                if (cmd_at_we) begin
                    at_addr_a = cmd_at_addr;
                    at_din_a  = cmd_at_din;
                    at_we_a   = 1'b1;
                end
                if (cmd_pal_we) begin
                    pr_addr_a = cmd_pal_addr_reg;
                    pr_din_a  = cmd_pal_din;
                    pr_we_a   = 1'b1;
                end
                // Tile data port A: blitter has priority, then command writes
                if (blt_tile_we) begin
                    td_addr_a = blt_tile_addr;
                    td_din_a  = blt_tile_wdata;
                    td_we_a   = 1'b1;
                end else if (blt_tile_re) begin
                    td_addr_a = blt_tile_addr;
                    // read — no write
                end
            end
        endcase
    end

    // Blitter read: td_dout_a is available 1 cycle after address is presented
    always_ff @(posedge clk) begin
        blt_tile_rd_pending <= blt_tile_re && !blt_tile_we && (dma_state == DMA_IDLE);
        if (blt_tile_rd_pending)
            blt_tile_rd_latch <= td_dout_a;
    end

    assign blt_tile_rdata = blt_tile_rd_pending ? td_dout_a : blt_tile_rd_latch;

    // =========================================================================
    // CPU writes + command processor
    // =========================================================================
    always_ff @(posedge clk) begin
        // Default: clear single-cycle command write strobes
        cmd_nt_we <= 0;
        cmd_at_we <= 0;
        cmd_pal_we <= 0;

        if (rst) begin
            tile_size16 <= 0; mirror_mode <= 0; trans_color <= 0;
            scroll_x <= 0; scroll_y <= 0; status_reg <= 0;
            collision_bits <= 0; dma_state <= DMA_IDLE;
            dma_data_valid <= 0;
        end else begin

            // Clear collision on read
            if (cpu_re && tile_reg_sel) begin
                if (reg_off == TR_COLL)  collision_bits[7:0]  <= 0;
                if (reg_off == TR_COLH)  collision_bits[15:8] <= 0;
            end

            // DMA state machine
            dma_data_valid <= 0;
            case (dma_state)
                DMA_IDLE: ; // waiting for command

                DMA_TILE_DEF: begin
                    if (dma_data_valid) begin
                        // Write happens in port A mux (combinational)
                        dma_dst <= dma_dst + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_NT_LOAD: begin
                    if (dma_data_valid) begin
                        dma_dst <= dma_dst + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_AT_LOAD: begin
                    if (dma_data_valid) begin
                        dma_dst <= dma_dst + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_ROW_LOAD: begin
                    if (dma_data_valid) begin
                        dma_dst <= dma_dst + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_COL_LOAD: begin
                    if (dma_data_valid) begin
                        dma_col_idx <= dma_col_idx + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_ROWATTR_LOAD: begin
                    if (dma_data_valid) begin
                        dma_dst <= dma_dst + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_COLATTR_LOAD: begin
                    if (dma_data_valid) begin
                        dma_col_idx <= dma_col_idx + 1;
                        dma_count <= dma_count - 1;
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 1) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_PAL_LOAD: begin
                    if (dma_data_valid) begin
                        case (dma_pal_phase)
                            8'd0: begin dma_pal_r <= dma_data; dma_pal_phase <= 1; end
                            8'd1: begin dma_pal_g <= dma_data; dma_pal_phase <= 2; end
                            8'd2: begin
                                // Write happens in port A mux
                                dma_pal_phase <= 0;
                                if (dma_pal_cidx == 4'd15)
                                    dma_count <= 0;  // done
                                else
                                    dma_pal_cidx <= dma_pal_cidx + 1;
                            end
                            default: dma_pal_phase <= 0;
                        endcase
                    end
                    if (dma_count == 0 && !dma_data_valid)
                        dma_state <= DMA_IDLE;
                    else if (!dma_data_valid || dma_count > 0) begin
                        dma_addr <= dma_src;
                        dma_src <= dma_src + 1;
                        dma_data_valid <= 1;
                    end
                end

                DMA_CLS: begin
                    // Serialized: 1 write per cycle to both nt + attr
                    if (dma_fill_addr < 4000)
                        dma_fill_addr <= dma_fill_addr + 1;
                    else
                        dma_state <= DMA_IDLE;
                end

                DMA_FILL: begin
                    // Serialized: 1 write per cycle
                    if (dma_count < (NT_SIZE - 1)) begin
                        dma_count <= dma_count + 1;
                    end else
                        dma_state <= DMA_IDLE;
                end

                DMA_BUFPUT: begin
                    // Serialized: 1 write per cycle
                    if (dma_col_idx < (NT_ROWS - 1))
                        dma_col_idx <= dma_col_idx + 1;
                    else
                        dma_state <= DMA_IDLE;
                end

                DMA_PEEK_NT: begin
                    // Address was set, data will be available next cycle
                    dma_state <= DMA_PEEK_AT;
                end

                DMA_PEEK_AT: begin
                    // Capture nametable result (available this cycle from port A)
                    peek_val  <= nt_dout_a;
                    dma_state <= DMA_PEEK_DONE;
                end

                DMA_PEEK_DONE: begin
                    // Capture attr result
                    peek_attr <= at_dout_a;
                    dma_state <= DMA_IDLE;
                end

                default: dma_state <= DMA_IDLE;
            endcase

            // CPU register writes
            if (cpu_we && tile_reg_sel) begin
                tregs[reg_off] <= cpu_wdata;

                case (reg_off)
                    TR_CONFIG: begin
                        tile_size16 <= cpu_wdata[0];
                        mirror_mode <= cpu_wdata[2:1];
                    end
                    TR_TRANS:    trans_color <= cpu_wdata[3:0];
                    TR_SCROLLXL: scroll_x[7:0]  <= cpu_wdata;
                    TR_SCROLLXH: scroll_x[15:8] <= cpu_wdata;
                    TR_SCROLLYL: scroll_y[7:0]  <= cpu_wdata;
                    TR_SCROLLYH: scroll_y[15:8] <= cpu_wdata;

                    TR_CMD: begin
                        if (dma_state == DMA_IDLE) begin
                            case (cpu_wdata)
                                TCMD_DEF: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= tile_size16
                                        ? {1'b0, tregs[TR_P0], 7'b0}
                                        : {3'b0, tregs[TR_P0], 5'b0};
                                    dma_count <= tile_size16 ? 16'd128 : 16'd32;
                                    dma_state <= DMA_TILE_DEF;
                                    dma_data_valid <= 0;
                                end

                                TCMD_DEFBULK: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= tile_size16
                                        ? {1'b0, tregs[TR_P0], 7'b0}
                                        : {3'b0, tregs[TR_P0], 5'b0};
                                    dma_count <= tile_size16
                                        ? 16'(tregs[TR_P1]) * 128
                                        : 16'(tregs[TR_P1]) * 32;
                                    dma_state <= DMA_TILE_DEF;
                                    dma_data_valid <= 0;
                                end

                                TCMD_PUT: begin
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS && tregs[TR_P2] < NT_ROWS) begin
                                        cmd_nt_addr <= 12'(tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]);
                                        cmd_nt_din  <= tregs[TR_P3];
                                        cmd_nt_we   <= 1;
                                    end
                                end

                                TCMD_ATTR: begin
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS && tregs[TR_P2] < NT_ROWS) begin
                                        cmd_at_addr <= 12'(tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]);
                                        cmd_at_din  <= tregs[TR_P3];
                                        cmd_at_we   <= 1;
                                    end
                                end

                                TCMD_FILL: begin
                                    if (tregs[TR_P0] < 4) begin
                                        dma_nt <= tregs[TR_P0][1:0];
                                        dma_fill_val <= tregs[TR_P1];
                                        dma_count <= 0;
                                        dma_state <= DMA_FILL;
                                    end
                                end

                                TCMD_ROW: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_count <= NT_COLS;
                                    dma_state <= DMA_ROW_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_COL: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_col_idx <= 0;
                                    dma_count <= NT_ROWS;
                                    dma_state <= DMA_COL_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_LOAD: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_count <= NT_SIZE;
                                    dma_state <= DMA_NT_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_ALOAD: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_count <= NT_SIZE;
                                    dma_state <= DMA_AT_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_PAL: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_pal_sub <= tregs[TR_PALP0][3:0];
                                    dma_pal_cidx <= 0;
                                    dma_pal_phase <= 0;
                                    dma_count <= 16'd48;
                                    dma_state <= DMA_PAL_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_PALC: begin
                                    cmd_pal_addr_reg <= {tregs[TR_PALP0][3:0], tregs[TR_PALP1][3:0]};
                                    cmd_pal_din  <= {tregs[TR_P0][7:4], tregs[TR_P1][7:4], tregs[TR_P2][7:4]};
                                    cmd_pal_we   <= 1;
                                end

                                TCMD_PEEK: begin
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS && tregs[TR_P2] < NT_ROWS) begin
                                        cmd_nt_addr <= 12'(tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]);
                                        cmd_at_addr <= 12'(tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]);
                                        dma_state <= DMA_PEEK_NT;
                                    end
                                end

                                TCMD_ROWATTR: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_count <= NT_COLS;
                                    dma_state <= DMA_ROWATTR_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_COLATTR: begin
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_col_idx <= 0;
                                    dma_count <= NT_ROWS;
                                    dma_state <= DMA_COLATTR_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_CLS: begin
                                    dma_fill_addr <= 0;
                                    dma_state <= DMA_CLS;
                                end

                                TCMD_BUFFILL: begin
                                    for (int i = 0; i < NT_ROWS; i++)
                                        col_buffer[i] <= tregs[TR_P0];
                                end

                                TCMD_BUFSET: begin
                                    if (tregs[TR_P0] < NT_ROWS)
                                        col_buffer[tregs[TR_P0]] <= tregs[TR_P1];
                                end

                                TCMD_BUFRANGE: begin
                                    for (int i = 0; i < NT_ROWS; i++)
                                        if (i >= tregs[TR_P0] && i <= tregs[TR_P1])
                                            col_buffer[i] <= tregs[TR_P2];
                                end

                                TCMD_BUFPUT: begin
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS) begin
                                        dma_nt <= tregs[TR_P0][1:0];
                                        dma_col_row <= tregs[TR_P1][5:0];
                                        dma_col_idx <= 0;
                                        dma_state <= DMA_BUFPUT;
                                    end
                                end

                                default: ;
                            endcase
                        end
                    end
                    default: ;
                endcase
            end
        end
    end

    // =========================================================================
    // Scanline rasterizer — pipelined BRAM reads
    // =========================================================================
    // Tiles are 8 pixels wide = 8 pixel-clock cycles per tile.
    // BRAM reads take 1 cycle latency. Pipeline:
    //   Stage 0: compute world coords, nametable address → present to nt port B
    //   Stage 1: nametable data available → present attr address (same addr) to at port B
    //            (actually nt and at use same address, so both presented in stage 0)
    //   Stage 1: nt_dout_b + at_dout_b available → compute tile_data address → present to td port B
    //   Stage 2: td_dout_b available → compute color index → present to pal port B
    //   Stage 3: pr_dout_b available → output pixel

    // Tile dimensions
    wire [3:0] tile_w = tile_size16 ? 4'd15 : 4'd7;
    wire [3:0] tile_h = tile_size16 ? 4'd15 : 4'd7;

    // Map dimensions in pixels (depends on mirror mode)
    wire [9:0] screen_w = GFX_W;
    wire [8:0] screen_h = GFX_H;
    wire [10:0] map_w = (mirror_mode == MIRROR_H || mirror_mode == MIRROR_SINGLE) ? 11'd320 : 11'd640;
    wire [9:0]  map_h = (mirror_mode == MIRROR_V || mirror_mode == MIRROR_SINGLE) ? 10'd200 : 10'd400;

    // --- Stage 0: coordinate computation (combinational) ---
    wire [15:0] raw_wx = 16'(pixel_x) + scroll_x;
    wire [15:0] raw_wy = 16'(pixel_y) + scroll_y;
    wire [10:0] world_x = raw_wx[10:0] % {5'b0, map_w[9:0], 1'b0};
    wire [9:0]  world_y = raw_wy[9:0]  % map_h;

    wire        nt_col = (world_x >= screen_w);
    wire        nt_row = (world_y >= screen_h);
    wire [9:0]  local_x = nt_col ? (world_x - screen_w) : world_x[9:0];
    wire [8:0]  local_y = nt_row ? (world_y - screen_h) : world_y[8:0];

    wire [5:0]  tile_col = tile_size16 ? local_x[9:4] : local_x[9:3];
    wire [4:0]  tile_row = tile_size16 ? local_y[8:4] : local_y[8:3];
    wire [3:0]  pix_in_x = tile_size16 ? local_x[3:0] : {1'b0, local_x[2:0]};
    wire [3:0]  pix_in_y = tile_size16 ? local_y[3:0] : {1'b0, local_y[2:0]};

    wire [1:0]  cur_nt = resolve_nt({1'b0, nt_col}, {1'b0, nt_row});
    wire [11:0] nt_lookup_addr = 12'(cur_nt * NT_SIZE) + 12'(tile_row * NT_COLS + tile_col);

    // Present nametable + attr addresses to port B (same address)
    assign nt_addr_b = nt_lookup_addr;
    assign at_addr_b = nt_lookup_addr;

    // --- Stage 1 registers (capture after 1-cycle BRAM latency) ---
    logic [3:0] s1_pix_in_x, s1_pix_in_y;
    logic       s1_pixel_valid;
    logic       s1_tile_size16;
    logic [3:0] s1_tile_w, s1_tile_h;

    always_ff @(posedge clk) begin
        s1_pix_in_x    <= pix_in_x;
        s1_pix_in_y    <= pix_in_y;
        s1_pixel_valid <= pixel_valid;
        s1_tile_size16 <= tile_size16;
        s1_tile_w      <= tile_w;
        s1_tile_h      <= tile_h;
    end

    // Stage 1: nt_dout_b and at_dout_b are now valid
    wire [7:0] s1_cur_tile = nt_dout_b;
    wire [7:0] s1_cur_attr = at_dout_b;

    // Apply flip using registered pixel coords and stage-1 data
    wire [3:0] s1_sample_x = s1_cur_attr[6] ? (s1_tile_w - s1_pix_in_x) : s1_pix_in_x;
    wire [3:0] s1_sample_y = s1_cur_attr[7] ? (s1_tile_h - s1_pix_in_y) : s1_pix_in_y;

    // Compute tile_data byte address
    wire [14:0] s1_tile_byte_offset = s1_tile_size16
        ? {s1_cur_tile, s1_sample_y, s1_sample_x[3:1]}
        : {2'b0, s1_cur_tile, s1_sample_y[2:0], s1_sample_x[2:1]};

    // Present tile_data address to port B
    assign td_addr_b = s1_tile_byte_offset;

    // --- Stage 2 registers ---
    logic       s2_pixel_valid;
    logic [3:0] s2_sub_pal;
    logic [0:0] s2_sample_x_lsb;
    logic [1:0] s2_tile_pri;

    always_ff @(posedge clk) begin
        s2_pixel_valid   <= s1_pixel_valid;
        s2_sub_pal       <= s1_cur_attr[3:0];
        s2_sample_x_lsb <= s1_sample_x[0];
        s2_tile_pri      <= s1_cur_attr[5] ? 2'd2 : 2'd1;
    end

    // Stage 2: td_dout_b is now valid
    wire [7:0] s2_packed_byte = td_dout_b;
    wire [3:0] s2_color_idx = s2_sample_x_lsb ? s2_packed_byte[3:0] : s2_packed_byte[7:4];

    // Present palette address to port B
    assign pr_addr_b = {s2_sub_pal, s2_color_idx};

    // --- Stage 3 registers ---
    logic       s3_pixel_valid;
    logic [3:0] s3_color_idx;
    logic [1:0] s3_tile_pri;
    logic [3:0] s3_trans_color;

    always_ff @(posedge clk) begin
        s3_pixel_valid <= s2_pixel_valid;
        s3_color_idx   <= s2_color_idx;
        s3_tile_pri    <= s2_tile_pri;
        s3_trans_color <= trans_color;
    end

    // Stage 3: pr_dout_b is now valid — output pixel
    always_comb begin
        if (s3_pixel_valid && s3_color_idx != s3_trans_color) begin
            tile_rgb = pr_dout_b;
            tile_opaque = s3_tile_pri;
        end else begin
            tile_rgb = 12'h000;
            tile_opaque = 2'd0;
        end
    end

endmodule
