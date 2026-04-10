// Tile Engine — NES-style tile map renderer
// 256 tiles (8×8 or 16×16), 4 nametables with mirroring, 16 sub-palettes
// Registers at $A0C0-$A0DF, commands triggered via $A0C7
// Outputs per-pixel color + priority for VGC compositing

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
    // Registers
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

    // Column buffer (25 entries for column DMA)
    logic [7:0]  col_buffer [0:NT_ROWS-1];

    // =========================================================================
    // Memory
    // =========================================================================
    // Tile data: 32KB max (256 tiles × 128 bytes in 16×16 mode)
    logic [7:0]  tile_data [0:32767];

    // Nametables: 4 × 1000 bytes
    logic [7:0]  nametable [0:3999];   // [nt*1000 + row*40 + col]

    // Attribute tables: 4 × 1000 bytes
    logic [7:0]  attr_table [0:3999];  // [nt*1000 + row*40 + col]

    // Sub-palette RAM: 16 palettes × 16 colors × 12-bit RGB
    logic [11:0] pal_ram [0:255];      // [sub*16 + color_idx]

    initial begin
        for (int i = 0; i < 32; i++) tregs[i] = 0;
        for (int i = 0; i < 32768; i++) tile_data[i] = 0;
        for (int i = 0; i < 4000; i++) begin
            nametable[i] = 0;
            attr_table[i] = 0;
        end
        // Initialize sub-palette 0 to match VGC palette
        pal_ram[0]  = 12'h000; pal_ram[1]  = 12'hFFF; pal_ram[2]  = 12'h800; pal_ram[3]  = 12'hAFE;
        pal_ram[4]  = 12'hC4C; pal_ram[5]  = 12'h0C5; pal_ram[6]  = 12'h00A; pal_ram[7]  = 12'hEE7;
        pal_ram[8]  = 12'hD85; pal_ram[9]  = 12'h640; pal_ram[10] = 12'hF77; pal_ram[11] = 12'h333;
        pal_ram[12] = 12'h777; pal_ram[13] = 12'h8F6; pal_ram[14] = 12'h08F; pal_ram[15] = 12'hBBB;
        for (int i = 16; i < 256; i++) pal_ram[i] = 12'h000;
        for (int i = 0; i < NT_ROWS; i++) col_buffer[i] = 0;
        tile_size16 = 0; mirror_mode = 0; trans_color = 0;
        scroll_x = 0; scroll_y = 0; status_reg = 0;
        peek_val = 0; peek_attr = 0; collision_bits = 0;
    end

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
    typedef enum logic [3:0] {
        DMA_IDLE,
        DMA_TILE_DEF,       // define tiles from RAM
        DMA_NT_LOAD,        // load nametable from RAM
        DMA_AT_LOAD,        // load attribute table from RAM
        DMA_PAL_LOAD,       // load sub-palette from RAM
        DMA_ROW_LOAD,       // load nametable row from RAM
        DMA_COL_LOAD,       // load nametable column from RAM
        DMA_ROWATTR_LOAD,   // load attribute row from RAM
        DMA_COLATTR_LOAD,   // load attribute column from RAM
        DMA_CLS             // clear all nametables
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

    assign dma_active = (dma_state != DMA_IDLE);

    initial begin
        dma_state = DMA_IDLE;
        dma_src = 0; dma_count = 0; dma_dst = 0;
        dma_nt = 0; dma_col_row = 0; dma_col_idx = 0;
        dma_data_valid = 0; dma_pal_phase = 0;
        dma_pal_r = 0; dma_pal_g = 0;
        dma_pal_sub = 0; dma_pal_cidx = 0;
    end

    // =========================================================================
    // Mirror resolution
    // =========================================================================
    function automatic logic [1:0] resolve_nt(input logic [1:0] nt_col, input logic [1:0] nt_row);
        logic [1:0] raw_nt;
        raw_nt = {nt_row[0], nt_col[0]};  // 0=TL, 1=TR, 2=BL, 3=BR
        case (mirror_mode)
            MIRROR_H:      return (raw_nt == 2'd1) ? 2'd0 : (raw_nt == 2'd3) ? 2'd2 : raw_nt;
            MIRROR_V:      return (raw_nt == 2'd2) ? 2'd0 : (raw_nt == 2'd3) ? 2'd1 : raw_nt;
            MIRROR_FOUR:   return raw_nt;
            MIRROR_SINGLE: return 2'd0;
        endcase
    endfunction

    // =========================================================================
    // CPU writes + command processor
    // =========================================================================
    always_ff @(posedge clk) begin
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
                        tile_data[dma_dst] <= dma_data;
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
                        nametable[dma_nt * NT_SIZE + dma_dst[9:0]] <= dma_data;
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
                        attr_table[dma_nt * NT_SIZE + dma_dst[9:0]] <= dma_data;
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
                        nametable[dma_nt * NT_SIZE + dma_col_row * NT_COLS + dma_dst[5:0]] <= dma_data;
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
                        nametable[dma_nt * NT_SIZE + dma_col_idx * NT_COLS + dma_col_row] <= dma_data;
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
                        attr_table[dma_nt * NT_SIZE + dma_col_row * NT_COLS + dma_dst[5:0]] <= dma_data;
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
                        attr_table[dma_nt * NT_SIZE + dma_col_idx * NT_COLS + dma_col_row] <= dma_data;
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
                                pal_ram[{dma_pal_sub, dma_pal_cidx}] <= {dma_pal_r[7:4], dma_pal_g[7:4], dma_data[7:4]};
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
                    // Clear all 4 nametables and attribute tables
                    if (dma_count < 4000) begin
                        nametable[dma_count] <= 0;
                        attr_table[dma_count] <= 0;
                        dma_count <= dma_count + 1;
                    end else
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
                            // Extract parameters
                            // p0=$A0C8, p1=$A0C9, p2=$A0CA, p3=$A0CB
                            // addr = {$A0CD, $A0CC}
                            case (cpu_wdata)
                                TCMD_DEF: begin
                                    // Define one tile: P0=tile#, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= tile_size16
                                        ? {1'b0, tregs[TR_P0], 7'b0}      // tile# × 128
                                        : {3'b0, tregs[TR_P0], 5'b0};     // tile# × 32
                                    dma_count <= tile_size16 ? 16'd128 : 16'd32;
                                    dma_state <= DMA_TILE_DEF;
                                    dma_data_valid <= 0;
                                end

                                TCMD_DEFBULK: begin
                                    // P0=start tile, P1=count, TileAddr=src
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
                                    // P0=NT, P1=X, P2=Y, P3=tile#
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS && tregs[TR_P2] < NT_ROWS)
                                        nametable[tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]]
                                            <= tregs[TR_P3];
                                end

                                TCMD_ATTR: begin
                                    // P0=NT, P1=X, P2=Y, P3=attr
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS && tregs[TR_P2] < NT_ROWS)
                                        attr_table[tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]]
                                            <= tregs[TR_P3];
                                end

                                TCMD_FILL: begin
                                    // P0=NT, P1=tile# — fill entire nametable
                                    if (tregs[TR_P0] < 4) begin
                                        for (int i = 0; i < NT_SIZE; i++)
                                            nametable[tregs[TR_P0] * NT_SIZE + i] <= tregs[TR_P1];
                                    end
                                end

                                TCMD_ROW: begin
                                    // P0=NT, P1=Y, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_count <= NT_COLS;
                                    dma_state <= DMA_ROW_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_COL: begin
                                    // P0=NT, P1=X, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_col_idx <= 0;
                                    dma_count <= NT_ROWS;
                                    dma_state <= DMA_COL_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_LOAD: begin
                                    // P0=NT, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_count <= NT_SIZE;
                                    dma_state <= DMA_NT_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_ALOAD: begin
                                    // P0=NT, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_count <= NT_SIZE;
                                    dma_state <= DMA_AT_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_PAL: begin
                                    // PalP0=sub-palette index, TileAddr=src (48 bytes RGB)
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_pal_sub <= tregs[TR_PALP0][3:0];
                                    dma_pal_cidx <= 0;
                                    dma_pal_phase <= 0;
                                    dma_count <= 16'd48;
                                    dma_state <= DMA_PAL_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_PALC: begin
                                    // PalP0=sub, PalP1=color, P0=R, P1=G, P2=B — immediate
                                    pal_ram[{tregs[TR_PALP0][3:0], tregs[TR_PALP1][3:0]}]
                                        <= {tregs[TR_P0][7:4], tregs[TR_P1][7:4], tregs[TR_P2][7:4]};
                                end

                                TCMD_PEEK: begin
                                    // P0=NT, P1=X, P2=Y → PeekVal, PeekAttr
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS && tregs[TR_P2] < NT_ROWS) begin
                                        peek_val  <= nametable[tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]];
                                        peek_attr <= attr_table[tregs[TR_P0] * NT_SIZE + tregs[TR_P2] * NT_COLS + tregs[TR_P1]];
                                    end
                                end

                                TCMD_ROWATTR: begin
                                    // P0=NT, P1=Y, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_dst <= 0;
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_count <= NT_COLS;
                                    dma_state <= DMA_ROWATTR_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_COLATTR: begin
                                    // P0=NT, P1=X, TileAddr=src
                                    dma_src <= {tregs[TR_ADDRH], tregs[TR_ADDRL]};
                                    dma_nt <= tregs[TR_P0][1:0];
                                    dma_col_row <= tregs[TR_P1][5:0];
                                    dma_col_idx <= 0;
                                    dma_count <= NT_ROWS;
                                    dma_state <= DMA_COLATTR_LOAD;
                                    dma_data_valid <= 0;
                                end

                                TCMD_CLS: begin
                                    dma_count <= 0;
                                    dma_state <= DMA_CLS;
                                end

                                TCMD_BUFFILL: begin
                                    // P0=tile# — fill entire column buffer
                                    for (int i = 0; i < NT_ROWS; i++)
                                        col_buffer[i] <= tregs[TR_P0];
                                end

                                TCMD_BUFSET: begin
                                    // P0=row, P1=tile#
                                    if (tregs[TR_P0] < NT_ROWS)
                                        col_buffer[tregs[TR_P0]] <= tregs[TR_P1];
                                end

                                TCMD_BUFRANGE: begin
                                    // P0=y1, P1=y2, P2=tile#
                                    for (int i = 0; i < NT_ROWS; i++)
                                        if (i >= tregs[TR_P0] && i <= tregs[TR_P1])
                                            col_buffer[i] <= tregs[TR_P2];
                                end

                                TCMD_BUFPUT: begin
                                    // P0=NT, P1=col — write buffer to nametable column
                                    if (tregs[TR_P0] < 4 && tregs[TR_P1] < NT_COLS)
                                        for (int i = 0; i < NT_ROWS; i++)
                                            nametable[tregs[TR_P0] * NT_SIZE + i * NT_COLS + tregs[TR_P1]]
                                                <= col_buffer[i];
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
    // Scanline rasterizer (combinational pixel lookup)
    // =========================================================================
    // Tile dimensions
    wire [3:0] tile_w = tile_size16 ? 4'd15 : 4'd7;   // max pixel index
    wire [3:0] tile_h = tile_size16 ? 4'd15 : 4'd7;
    wire [4:0] tile_shift = tile_size16 ? 5'd4 : 5'd3; // log2(tile_size)

    // Map dimensions in pixels (depends on mirror mode)
    wire [9:0] screen_w = GFX_W;  // 320 pixels per NT
    wire [8:0] screen_h = GFX_H;  // 200 pixels per NT
    wire [10:0] map_w = (mirror_mode == MIRROR_H || mirror_mode == MIRROR_SINGLE) ? 11'd320 : 11'd640;
    wire [9:0]  map_h = (mirror_mode == MIRROR_V || mirror_mode == MIRROR_SINGLE) ? 10'd200 : 10'd400;

    // World coordinates with scroll wrapping
    wire [15:0] raw_wx = 16'(pixel_x) + scroll_x;
    wire [15:0] raw_wy = 16'(pixel_y) + scroll_y;
    wire [10:0] world_x = raw_wx[10:0] % {5'b0, map_w[9:0], 1'b0};  // use wider for mod
    wire [9:0]  world_y = raw_wy[9:0]  % map_h;

    // Nametable quadrant
    wire        nt_col = (world_x >= screen_w);
    wire        nt_row = (world_y >= screen_h);
    wire [9:0]  local_x = nt_col ? (world_x - screen_w) : world_x[9:0];
    wire [8:0]  local_y = nt_row ? (world_y - screen_h) : world_y[8:0];

    // Tile grid position
    wire [5:0]  tile_col = tile_size16 ? local_x[9:4] : local_x[9:3];
    wire [4:0]  tile_row = tile_size16 ? local_y[8:4] : local_y[8:3];
    wire [3:0]  pix_in_x = tile_size16 ? local_x[3:0] : {1'b0, local_x[2:0]};
    wire [3:0]  pix_in_y = tile_size16 ? local_y[3:0] : {1'b0, local_y[2:0]};

    // Resolve mirrored nametable
    wire [1:0]  cur_nt = resolve_nt({1'b0, nt_col}, {1'b0, nt_row});
    wire [9:0]  nt_index = tile_row * NT_COLS + tile_col;

    // Look up tile and attribute
    wire [7:0]  cur_tile = nametable[cur_nt * NT_SIZE + nt_index];
    wire [7:0]  cur_attr = attr_table[cur_nt * NT_SIZE + nt_index];

    // Apply flip
    wire [3:0]  sample_x = cur_attr[6] ? (tile_w - pix_in_x) : pix_in_x;  // HFlip = bit 6
    wire [3:0]  sample_y = cur_attr[7] ? (tile_h - pix_in_y) : pix_in_y;  // VFlip = bit 7

    // Compute byte offset in tile data
    wire [2:0]  bytes_per_row = tile_size16 ? 3'd4 : 3'd4;  // always 4 pixels per byte → row/2 bytes
    // Actually: 4-bit packed, 2 pixels per byte
    // 8×8 tile: 4 bytes per row, 32 bytes total
    // 16×16 tile: 8 bytes per row, 128 bytes total
    wire [14:0] tile_byte_offset = tile_size16
        ? {cur_tile, sample_y, sample_x[3:1]}           // tile*128 + y*8 + x/2
        : {2'b0, cur_tile, sample_y[2:0], sample_x[2:1]}; // tile*32 + y*4 + x/2

    wire [7:0]  packed_byte = tile_data[tile_byte_offset];
    wire [3:0]  color_idx = sample_x[0] ? packed_byte[3:0] : packed_byte[7:4];

    // Sub-palette lookup
    wire [3:0]  sub_pal = cur_attr[3:0];
    wire [11:0] pal_color = pal_ram[{sub_pal, color_idx}];

    // Priority: attr bit 5 = 0 → behind between-sprites, 1 → in front
    wire [1:0]  tile_pri = cur_attr[5] ? 2'd2 : 2'd1;

    // Output
    always_comb begin
        if (pixel_valid && color_idx != trans_color) begin
            tile_rgb = pal_color;
            tile_opaque = tile_pri;
        end else begin
            tile_rgb = 12'h000;
            tile_opaque = 2'd0;
        end
    end

endmodule
