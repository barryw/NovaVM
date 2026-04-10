// VGC — Virtual Graphics Controller
// 80x25 text + 320x200 block graphics + tile engine + command processor
// Generates standard VGA 640x480@60Hz RGB + sync signals
//
// Modes: 0=text only, 1=gfx over text, 2=text over gfx, 3=gfx only, 4=tiles+sprites
// Drawing: command register at $A010, parameters at $A011-$A01F

module vgc (
    input  logic        clk,        // pixel clock (~25 MHz)
    input  logic        rst,

    // CPU bus
    input  logic        cpu_ce,     // CPU clock enable
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    output logic [7:0]  cpu_rdata,
    input  logic        cpu_we,
    input  logic        cpu_re,

    // Keyboard input
    input  logic        key_valid,
    input  logic [7:0]  key_data,

    // Tile engine DMA — CPU RAM read port
    output logic [15:0] tile_dma_addr,
    input  logic [7:0]  tile_dma_data,
    output logic        tile_dma_active,

    // Blitter memory port — read/write access to VGC internal memories
    input  logic [2:0]  blt_space,    // 1=char, 2=color, 3=gfx, 4=sprite, 6=tile
    input  logic [15:0] blt_addr,
    output logic [7:0]  blt_rdata,
    input  logic [7:0]  blt_wdata,
    input  logic        blt_we,
    input  logic        blt_re,

    // Debug read port — separate from CPU bus to avoid side effects
    input  logic [15:0] dbg_addr,
    output logic [7:0]  dbg_rdata,

    // Video output
    output logic [3:0]  vid_r,
    output logic [3:0]  vid_g,
    output logic [3:0]  vid_b,
    output logic        vid_hsync,
    output logic        vid_vsync,
    output logic        vid_de,
    output logic        irq_out     // raster IRQ (active high)
);

    // =========================================================================
    // Parameters
    // =========================================================================
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 48, H_TOTAL = 800;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 33, V_TOTAL = 525;

    localparam COLS     = 80,  ROWS    = 25;
    localparam CHAR_W   = 8,   CHAR_H  = 8;
    localparam TEXT_H   = ROWS * CHAR_H * 2;           // 400 pixels (doubled)
    localparam V_BORDER = (V_ACTIVE - TEXT_H) / 2;     // 40 lines

    localparam GFX_W    = 320, GFX_H   = 200;
    localparam GFX_SIZE = GFX_W * GFX_H;               // 64000 pixels

    // Address map
    localparam VGC_BASE       = 16'hA000;
    localparam VGC_REGS_END   = 16'hA01F;
    localparam SPR_REG_BASE   = 16'hA040;  // sprite registers: 16 × 8 bytes
    localparam SPR_REG_END    = 16'hA0BF;
    localparam CHAR_RAM_BASE  = 16'hAA00;
    localparam CHAR_RAM_END   = 16'hB1CF;
    localparam COLOR_RAM_BASE = 16'hB1D0;
    localparam COLOR_RAM_END  = 16'hB99F;

    // Register offsets
    localparam REG_MODE    = 0;    // $A000 — display mode (0-3)
    localparam REG_BGCOL   = 1;    // $A001 — background color
    localparam REG_FGCOL   = 2;    // $A002 — foreground color
    localparam REG_CURSORX = 3;    // $A003
    localparam REG_CURSORY = 4;    // $A004
    localparam REG_BORDER  = 13;   // $A00D
    localparam REG_CHAROUT = 14;   // $A00E
    localparam REG_CHARIN  = 15;   // $A00F
    localparam REG_CMD     = 16;   // $A010 — command register (write triggers)
    // Parameters: regs[17..30] = P0..P13 ($A011-$A01E)
    // 16-bit params: p0=regs[17]|regs[18]<<8 (X0), p1=regs[19]|regs[20]<<8 (Y0)
    //               p2=regs[21]|regs[22]<<8 (X1), p3=regs[23]|regs[24]<<8 (Y1)

    // Drawing commands (match Avalonia VgcConstants)
    localparam CMD_PLOT    = 8'h01;
    localparam CMD_UNPLOT  = 8'h02;
    localparam CMD_LINE    = 8'h03;
    localparam CMD_CIRCLE  = 8'h04;
    localparam CMD_RECT    = 8'h05;
    localparam CMD_FILL    = 8'h06;
    localparam CMD_GCLS    = 8'h07;
    localparam CMD_GCOLOR  = 8'h08;
    localparam CMD_PAINT   = 8'h09;
    localparam CMD_GTEXT   = 8'h0A;

    // Sprite commands
    localparam CMD_SPRDEF  = 8'h10;   // define shape: P0=sprite#, shape data follows via MemWrite
    localparam CMD_SPRROW  = 8'h11;   // set row: P0=sprite#, P1=row#, P2..P9=8 bytes packed
    localparam CMD_SPRCLR  = 8'h12;   // clear sprite shape: P0=sprite#
    localparam CMD_SPRCOPY = 8'h13;   // copy shape: P0=dest, P1=src
    localparam CMD_SPRPOS  = 8'h14;   // position: P0=sprite#, P1|P2=X, P3|P4=Y
    localparam CMD_SPRENA  = 8'h15;   // enable: P0=sprite#
    localparam CMD_SPRDIS  = 8'h16;   // disable: P0=sprite#
    localparam CMD_SPRFLIP = 8'h17;   // flip: P0=sprite#, P1=mode (b0=H, b1=V)
    localparam CMD_SPRPRI  = 8'h18;   // priority: P0=sprite#, P1=pri (0-2)

    // Advanced copper commands
    localparam CMD_COPPERLIST    = 8'h20;  // select target list: P0=index (0-127)
    localparam CMD_COPPERUSE     = 8'h21;  // activate list at vblank: P0=index (0-127)
    localparam CMD_COPPERLISTEND = 8'h22;  // reset target to active list

    // Internal ops (not CPU-triggered, used by charout)
    localparam CMD_TXTCLS  = 8'hF0;   // text clear screen (internal)

    localparam NUM_SPRITES = 16;
    localparam SPR_W       = 16;
    localparam SPR_H       = 16;

    // =========================================================================
    // Video timing
    // =========================================================================
    logic [9:0] h_count, v_count;

    always_ff @(posedge clk) begin
        if (rst) begin
            h_count <= 0; v_count <= 0;
        end else begin
            if (h_count == H_TOTAL - 1) begin
                h_count <= 0;
                v_count <= (v_count == V_TOTAL - 1) ? 10'd0 : v_count + 1;
            end else
                h_count <= h_count + 1;
        end
    end

    wire h_sync_area = (h_count >= H_ACTIVE + H_FRONT) &&
                       (h_count <  H_ACTIVE + H_FRONT + H_SYNC);
    wire v_sync_area = (v_count >= V_ACTIVE + V_FRONT) &&
                       (v_count <  V_ACTIVE + V_FRONT + V_SYNC);
    wire h_visible   = (h_count < H_ACTIVE);
    wire v_visible   = (v_count < V_ACTIVE);
    wire visible     = h_visible && v_visible;
    wire in_text_area = visible && (v_count >= V_BORDER) && (v_count < V_BORDER + TEXT_H);

    // =========================================================================
    // Palette (16 C64-style colors → 12-bit RGB) — declared early for use in pixel pipeline
    // =========================================================================
    logic [11:0] palette [0:15];
    initial begin
        palette[0]  = 12'h000; palette[1]  = 12'hFFF; palette[2]  = 12'h800; palette[3]  = 12'hAFE;
        palette[4]  = 12'hC4C; palette[5]  = 12'h0C5; palette[6]  = 12'h00A; palette[7]  = 12'hEE7;
        palette[8]  = 12'hD85; palette[9]  = 12'h640; palette[10] = 12'hF77; palette[11] = 12'h333;
        palette[12] = 12'h777; palette[13] = 12'h8F6; palette[14] = 12'h08F; palette[15] = 12'hBBB;
    end

    // =========================================================================
    // Memory
    // =========================================================================
    logic [7:0] char_ram [0:1999];
    logic [7:0] color_ram [0:1999];
    logic [7:0] font_rom [0:2047];
    logic [3:0] gfx_ram [0:GFX_SIZE-1];   // 320x200 x 4-bit color

    // Sprite shape RAM: 16 shapes × 128 bytes (16×16 pixels, 4-bit, packed nibbles)
    logic [7:0] sprite_shapes [0:2047];

    // Sprite attributes
    logic [8:0]  spr_x [0:15];       // X position
    logic [7:0]  spr_y [0:15];       // Y position
    logic        spr_enable [0:15];
    logic        spr_flip_h [0:15];
    logic        spr_flip_v [0:15];
    logic [1:0]  spr_pri [0:15];     // 0=behind gfx, 1=over gfx, 2=topmost
    logic [3:0]  spr_shape [0:15];   // which shape (0-15)

    initial begin
        for (int i = 0; i < 2000; i++) begin
            char_ram[i] = 8'h20;
            color_ram[i] = 8'h01;
        end
        for (int i = 0; i < GFX_SIZE; i++)
            gfx_ram[i] = 4'h0;
        for (int i = 0; i < 2048; i++)
            sprite_shapes[i] = 8'h00;
        for (int i = 0; i < 16; i++) begin
            spr_x[i] = 0; spr_y[i] = 0;
            spr_enable[i] = 0; spr_flip_h[i] = 0; spr_flip_v[i] = 0;
            spr_pri[i] = 2'd1; spr_shape[i] = i[3:0];
        end
        $readmemh("rom/cp437.hex", font_rom);
    end

    // =========================================================================
    // VGC Registers
    // =========================================================================
    logic [7:0]  regs [0:31];
    logic [6:0]  cursor_x;
    logic [4:0]  cursor_y;
    logic [3:0]  fg_color, bg_color, border_color, gfx_color;
    logic [2:0]  mode;
    logic        cursor_enable;
    logic [7:0]  scroll_x, scroll_y;   // pixel scroll offsets
    logic [3:0]  spr_trans [0:15];     // per-sprite transparent color
    logic [7:0]  char_in_reg;
    logic [2:0]  font_slot;            // active font (0-7)
    logic [7:0]  collision_ss;         // sprite-sprite collision (read-clears)
    logic [7:0]  collision_bg;         // sprite-background collision (read-clears)
    logic [7:0]  irq_ctrl;             // bit0=raster IRQ enable, bits 1-7=raster line
    logic        irq_raster_pending;
    logic [4:0]  scroll_offset;
    logic        scroll_pending;
    logic [6:0]  scroll_col;

    // Flood fill stack (for Paint command)
    localparam FILL_STACK_SIZE = 512;
    logic [16:0] fill_stack [0:FILL_STACK_SIZE-1]; // packed x|y coordinates
    logic [9:0]  fill_sp;              // stack pointer
    logic [3:0]  fill_target;          // color being replaced

    // Gtext state machine
    localparam FIO_NAME_LEN = 16'hB9A3;   // filename length register
    localparam FIO_NAME     = 16'hB9B0;   // filename buffer (64 bytes)
    logic [5:0]  gt_char_idx;              // current character index in string
    logic [5:0]  gt_char_len;              // total string length
    logic [2:0]  gt_font_row;             // current row within glyph (0-7)
    logic [2:0]  gt_font_col;             // current column within glyph (0-7)
    logic [7:0]  gt_scale;                // scale factor
    logic [7:0]  gt_scale_x;             // current scale X counter
    logic [7:0]  gt_scale_y;             // current scale Y counter
    logic [8:0]  gt_pen_x;               // current pen X position (start of current char)
    logic [7:0]  gt_start_y;             // Y position (top of string)
    logic [7:0]  gt_cur_char;            // current character code
    logic [7:0]  gt_font_byte;           // current font row bitmap

    // Copper — raster-synchronized register writer with multi-list support
    localparam COPPER_MAX   = 32;    // max entries per list (C# allows 256; 32 is practical)
    localparam COPPER_LISTS = 128;   // 128 lists — matches C# API
    // Active list: used during rendering
    logic [16:0] copper_pos   [0:COPPER_MAX-1];
    logic [7:0]  copper_reg   [0:COPPER_MAX-1];
    logic [7:0]  copper_val   [0:COPPER_MAX-1];
    logic [8:0]  copper_count;                   // active list entry count
    logic [8:0]  copper_index;                   // current scan position during frame
    logic        copper_enabled;

    // Multi-list storage: 128 lists × 32 entries = 4096 entries (~16.5KB BRAM)
    logic [16:0] copper_list_pos [0:COPPER_LISTS*COPPER_MAX-1];
    logic [7:0]  copper_list_reg [0:COPPER_LISTS*COPPER_MAX-1];
    logic [7:0]  copper_list_val [0:COPPER_LISTS*COPPER_MAX-1];
    logic [8:0]  copper_list_count [0:COPPER_LISTS-1];
    logic [6:0]  copper_target_list;   // list being edited (0-127)
    logic [6:0]  copper_active_list;   // list currently rendering
    logic [6:0]  copper_pending_list;  // becomes active at next vblank

    // Copper list copy state machine (loads pending→active at vblank)
    logic        copper_loading;
    logic [8:0]  copper_load_idx;
    logic [6:0]  copper_load_src;

    // FIO name buffer shadow (snooped from CPU writes for Gtext)
    logic [7:0]  fio_name [0:63];     // 64-byte string buffer ($B9B0-$B9EF)
    logic [5:0]  fio_name_len;        // string length ($B9A3)

    initial begin
        for (int i = 0; i < COPPER_MAX; i++) begin
            copper_pos[i] = 0; copper_reg[i] = 0; copper_val[i] = 0;
        end
        for (int i = 0; i < COPPER_LISTS * COPPER_MAX; i++) begin
            copper_list_pos[i] = 0; copper_list_reg[i] = 0; copper_list_val[i] = 0;
        end
        for (int i = 0; i < COPPER_LISTS; i++)
            copper_list_count[i] = 0;
        copper_count = 0; copper_index = 0; copper_enabled = 0;
        copper_target_list = 0; copper_active_list = 0;
        copper_pending_list = 0;
        copper_loading = 0; copper_load_idx = 0; copper_load_src = 0;
        for (int i = 0; i < 64; i++) fio_name[i] = 0;
        fio_name_len = 0;
    end

    // Command state machine
    logic        cmd_busy;
    logic [7:0]  cmd_op;
    // Line drawing state
    logic signed [9:0] cmd_x, cmd_y, cmd_x2, cmd_y2;
    logic signed [9:0] cmd_dx, cmd_dy, cmd_err;
    logic signed [9:0] cmd_sx, cmd_sy;
    // Rect/clear state
    logic [8:0]  cmd_cx;
    logic [7:0]  cmd_cy;

    initial begin
        for (int i = 0; i < 32; i++) regs[i] = 0;
        cursor_x = 0; cursor_y = 0;
        fg_color = 4'd1; bg_color = 4'd6; border_color = 4'd6;
        gfx_color = 4'd1; mode = 0; cursor_enable = 1;
        scroll_x = 0; scroll_y = 0;
        for (int i = 0; i < 16; i++) spr_trans[i] = 4'd0;
        scroll_offset = 0; scroll_pending = 0; scroll_col = 0;
        char_in_reg = 0; cmd_busy = 0;
        font_slot = 0; collision_ss = 0; collision_bg = 0;
        irq_ctrl = 0; irq_raster_pending = 0; fill_sp = 0;
    end

    // =========================================================================
    // Keyboard input
    // =========================================================================
    always_ff @(posedge clk) begin
        if (key_valid)
            char_in_reg <= key_data;
        else if (cpu_re && cpu_ce && cpu_addr == (VGC_BASE + REG_CHARIN) && char_in_reg != 0)
            char_in_reg <= 0;
    end

    // =========================================================================
    // Address decode
    // =========================================================================
    wire vgc_reg_sel   = (cpu_addr >= VGC_BASE && cpu_addr <= VGC_REGS_END);
    wire spr_reg_sel   = (cpu_addr >= SPR_REG_BASE && cpu_addr <= SPR_REG_END);
    wire char_ram_sel  = (cpu_addr >= CHAR_RAM_BASE && cpu_addr <= CHAR_RAM_END);
    wire color_ram_sel = (cpu_addr >= COLOR_RAM_BASE && cpu_addr <= COLOR_RAM_END);
    wire fio_name_sel  = (cpu_addr >= FIO_NAME && cpu_addr <= 16'hB9EF);
    wire fio_len_sel   = (cpu_addr == FIO_NAME_LEN);
    wire tile_reg_sel  = (cpu_addr >= 16'hA0C0 && cpu_addr <= 16'hA0DF);
    wire [7:0]  tile_rdata;  // forward declaration — driven by tile_engine instance below
    wire [4:0]  reg_offset   = cpu_addr[4:0];
    wire [3:0]  spr_index    = cpu_addr[6:3];  // sprite number (0-15)
    wire [2:0]  spr_field    = cpu_addr[2:0];  // field within sprite (0-7)
    wire [10:0] char_offset  = cpu_addr - CHAR_RAM_BASE;
    wire [10:0] color_offset = cpu_addr - COLOR_RAM_BASE;

    // Read mux
    always_comb begin
        cpu_rdata = 8'h00;
        if (vgc_reg_sel) begin
            case (reg_offset)
                REG_MODE:    cpu_rdata = {5'b0, mode};
                REG_BGCOL:   cpu_rdata = {4'b0, bg_color};
                REG_FGCOL:   cpu_rdata = {4'b0, fg_color};
                REG_CURSORX: cpu_rdata = {1'b0, cursor_x};
                REG_CURSORY: cpu_rdata = {3'b0, cursor_y};
                5'd5:        cpu_rdata = scroll_x;              // ScrollX
                5'd6:        cpu_rdata = scroll_y;              // ScrollY
                5'd7:        cpu_rdata = {5'b0, font_slot};     // Font
                5'd8:        cpu_rdata = {4'b0, gfx_color};    // gfx draw color
                5'd9:        begin                               // SpriteCount (read-only)
                    cpu_rdata = 0;
                    for (int i = 0; i < 16; i++)
                        if (spr_enable[i]) cpu_rdata = cpu_rdata + 1;
                end
                5'd10:       cpu_rdata = {7'b0, cursor_enable}; // CursorEnable
                5'd11:       cpu_rdata = collision_ss;           // ColSt (read-clears)
                5'd12:       cpu_rdata = collision_bg;           // ColBg (read-clears)
                REG_BORDER:  cpu_rdata = {4'b0, border_color};
                REG_CHARIN:  cpu_rdata = char_in_reg;
                REG_CMD:     cpu_rdata = {7'b0, cmd_busy};
                5'd31:       cpu_rdata = irq_ctrl;              // IrqCtrl
                default:     cpu_rdata = regs[reg_offset];
            endcase
        end
        else if (tile_reg_sel)   cpu_rdata = tile_rdata;
        else if (char_ram_sel)  cpu_rdata = char_ram[char_offset];
        else if (color_ram_sel) cpu_rdata = color_ram[color_offset];
    end

    // Debug read port — mirrors cpu_rdata mux but uses dbg_addr
    // No side effects (no CHARIN clear, no collision register clear)
    wire dbg_char_sel  = (dbg_addr >= CHAR_RAM_BASE && dbg_addr <= CHAR_RAM_END);
    wire dbg_color_sel = (dbg_addr >= COLOR_RAM_BASE && dbg_addr <= COLOR_RAM_END);
    wire dbg_vgc_sel   = (dbg_addr >= VGC_BASE && dbg_addr <= VGC_REGS_END);
    wire dbg_tile_sel  = (dbg_addr >= 16'hA0C0 && dbg_addr <= 16'hA0DF);
    wire dbg_spr_sel   = (dbg_addr >= SPR_REG_BASE && dbg_addr <= SPR_REG_END);
    wire dbg_owns      = dbg_char_sel || dbg_color_sel || dbg_vgc_sel || dbg_tile_sel || dbg_spr_sel;

    always_comb begin
        dbg_rdata = 8'h00;
        if (dbg_char_sel)       dbg_rdata = char_ram[dbg_addr - CHAR_RAM_BASE];
        else if (dbg_color_sel) dbg_rdata = color_ram[dbg_addr - COLOR_RAM_BASE];
        else if (dbg_vgc_sel) begin
            case (dbg_addr[4:0])
                REG_MODE:    dbg_rdata = {5'b0, mode};
                REG_BGCOL:   dbg_rdata = {4'b0, bg_color};
                REG_FGCOL:   dbg_rdata = {4'b0, fg_color};
                REG_CURSORX: dbg_rdata = {1'b0, cursor_x};
                REG_CURSORY: dbg_rdata = {3'b0, cursor_y};
                5'd7:        dbg_rdata = {5'b0, font_slot};
                REG_CHARIN:  dbg_rdata = char_in_reg;
                default:     dbg_rdata = regs[dbg_addr[4:0]];
            endcase
        end
    end

    // =========================================================================
    // Screen address helper
    // =========================================================================
    function automatic logic [10:0] screen_addr(input logic [6:0] col, input logic [4:0] row);
        logic [4:0] rr;
        rr = row + scroll_offset;
        if (rr >= ROWS) rr = rr - ROWS;
        return {4'b0, rr} * COLS + {4'b0, col};
    endfunction

    // =========================================================================
    // GFX pixel write helper
    // =========================================================================
    function automatic logic [16:0] gfx_addr(input logic [8:0] x, input logic [7:0] y);
        return {9'b0, y} * GFX_W + {8'b0, x};
    endfunction

    // =========================================================================
    // CPU writes + command processor
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            cursor_x <= 0; cursor_y <= 0; mode <= 0;
            fg_color <= 4'd1; bg_color <= 4'd6; border_color <= 4'd6;
            gfx_color <= 4'd1; cursor_enable <= 1; font_slot <= 0;
            scroll_offset <= 0; scroll_pending <= 0; scroll_col <= 0;
            scroll_x <= 0; scroll_y <= 0;
            cmd_busy <= 0; char_in_reg <= 0;
            collision_ss <= 0; collision_bg <= 0;
            irq_ctrl <= 0; irq_raster_pending <= 0;
            copper_enabled <= 0; copper_count <= 0; copper_index <= 0;
            copper_target_list <= 0; copper_pending_list <= 0;
            copper_loading <= 0;
            fill_sp <= 0;
            for (int i = 0; i < 32; i++) regs[i] <= 0;
            for (int i = 0; i < 16; i++) begin
                spr_x[i] <= 0; spr_y[i] <= 0; spr_enable[i] <= 0;
                spr_flip_h[i] <= 0; spr_flip_v[i] <= 0;
                spr_pri[i] <= 0; spr_shape[i] <= 0; spr_trans[i] <= 0;
            end
            // Clear screen on reset
            for (int i = 0; i < 2000; i++) begin
                char_ram[i] <= 8'h20;    // space
                color_ram[i] <= 8'h01;   // default fg color
            end
            // Clear graphics and sprite data
            for (int i = 0; i < GFX_SIZE; i++) gfx_ram[i] <= 4'h0;
            for (int i = 0; i < 2048; i++) sprite_shapes[i] <= 8'h0;
        end else begin

            // Scroll state machine
            if (scroll_pending) begin
                char_ram[screen_addr(scroll_col, ROWS - 1)] <= 8'h20;
                color_ram[screen_addr(scroll_col, ROWS - 1)] <= {4'b0, fg_color};
                if (scroll_col == COLS - 1) begin
                    scroll_pending <= 0; scroll_col <= 0;
                end else
                    scroll_col <= scroll_col + 1;
            end

            // Drawing command state machine
            if (cmd_busy) begin
                case (cmd_op)
                    CMD_LINE: begin
                        // Bresenham line — one pixel per clock
                        if (cmd_x >= 0 && cmd_x < GFX_W && cmd_y >= 0 && cmd_y < GFX_H)
                            gfx_ram[cmd_y * GFX_W + cmd_x] <= gfx_color;
                        if (cmd_x == cmd_x2 && cmd_y == cmd_y2 ||
                            cmd_x < -16 || cmd_x > GFX_W + 16 ||
                            cmd_y < -16 || cmd_y > GFX_H + 16) begin
                            cmd_busy <= 0;  // done or out of bounds safety
                        end else begin
                            // Compute both step conditions from current err
                            // Must update err atomically to avoid dual-assignment bug
                            if (cmd_err * 2 >= cmd_dy && cmd_err * 2 <= cmd_dx) begin
                                cmd_err <= cmd_err + cmd_dy + cmd_dx;
                                cmd_x <= cmd_x + cmd_sx;
                                cmd_y <= cmd_y + cmd_sy;
                            end else if (cmd_err * 2 >= cmd_dy) begin
                                cmd_err <= cmd_err + cmd_dy;
                                cmd_x <= cmd_x + cmd_sx;
                            end else if (cmd_err * 2 <= cmd_dx) begin
                                cmd_err <= cmd_err + cmd_dx;
                                cmd_y <= cmd_y + cmd_sy;
                            end
                        end
                    end
                    CMD_RECT: begin
                        // Rectangle outline — one pixel per clock
                        // Draw top/bottom edges and left/right edges
                        if (cmd_cx < GFX_W && cmd_cy < GFX_H) begin
                            if (cmd_cy == cmd_y[7:0] || cmd_cy == cmd_y2[7:0] ||
                                cmd_cx == cmd_x[8:0] || cmd_cx == cmd_x2[8:0])
                                gfx_ram[{9'b0, cmd_cy} * GFX_W + {8'b0, cmd_cx}] <= gfx_color;
                        end
                        if (cmd_cx >= cmd_x2[8:0]) begin
                            cmd_cx <= cmd_x[8:0];
                            if (cmd_cy >= cmd_y2[7:0])
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    CMD_FILL: begin
                        // Filled rectangle — one pixel per clock
                        if (cmd_cx < GFX_W && cmd_cy < GFX_H)
                            gfx_ram[{9'b0, cmd_cy} * GFX_W + {8'b0, cmd_cx}] <= gfx_color;
                        if (cmd_cx >= cmd_x2[8:0]) begin
                            cmd_cx <= cmd_x[8:0];
                            if (cmd_cy >= cmd_y2[7:0])
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    CMD_GCLS: begin
                        // Clear entire framebuffer
                        gfx_ram[{9'b0, cmd_cy} * GFX_W + {8'b0, cmd_cx}] <= 4'h0;
                        if (cmd_cx == GFX_W - 1) begin
                            cmd_cx <= 0;
                            if (cmd_cy == GFX_H - 1)
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    CMD_SPRCLR: begin
                        // Clear 128 bytes of sprite shape data
                        sprite_shapes[{cmd_x[4:1], cmd_cy[3:0], cmd_cx[2:0]}] <= 8'h00;
                        if (cmd_cx == 7) begin
                            cmd_cx <= 0;
                            if (cmd_cy == 15)
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    CMD_TXTCLS: begin
                        // Clear text screen — 80 cols × 25 rows
                        char_ram[{4'b0, cmd_cy[4:0]} * COLS + {2'b0, cmd_cx[6:0]}] <= 8'h20;
                        color_ram[{4'b0, cmd_cy[4:0]} * COLS + {2'b0, cmd_cx[6:0]}] <= {4'b0, fg_color};
                        if (cmd_cx == COLS - 1) begin
                            cmd_cx <= 0;
                            if (cmd_cy == ROWS - 1)
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    8'h04: begin // Circle — midpoint algorithm
                        // Plot 8 octant points from center (cmd_x, cmd_y) with dx, dy
                        // cmd_dx = current x offset, cmd_dy = current y offset
                        if (cmd_dx <= cmd_dy) begin
                            // Plot 8 symmetric points
                            if (cmd_x+cmd_dx >= 0 && cmd_x+cmd_dx < GFX_W && cmd_y+cmd_dy >= 0 && cmd_y+cmd_dy < GFX_H)
                                gfx_ram[(cmd_y+cmd_dy) * GFX_W + (cmd_x+cmd_dx)] <= gfx_color;
                            if (cmd_x-cmd_dx >= 0 && cmd_x-cmd_dx < GFX_W && cmd_y+cmd_dy >= 0 && cmd_y+cmd_dy < GFX_H)
                                gfx_ram[(cmd_y+cmd_dy) * GFX_W + (cmd_x-cmd_dx)] <= gfx_color;
                            if (cmd_x+cmd_dx >= 0 && cmd_x+cmd_dx < GFX_W && cmd_y-cmd_dy >= 0 && cmd_y-cmd_dy < GFX_H)
                                gfx_ram[(cmd_y-cmd_dy) * GFX_W + (cmd_x+cmd_dx)] <= gfx_color;
                            if (cmd_x-cmd_dx >= 0 && cmd_x-cmd_dx < GFX_W && cmd_y-cmd_dy >= 0 && cmd_y-cmd_dy < GFX_H)
                                gfx_ram[(cmd_y-cmd_dy) * GFX_W + (cmd_x-cmd_dx)] <= gfx_color;
                            if (cmd_x+cmd_dy >= 0 && cmd_x+cmd_dy < GFX_W && cmd_y+cmd_dx >= 0 && cmd_y+cmd_dx < GFX_H)
                                gfx_ram[(cmd_y+cmd_dx) * GFX_W + (cmd_x+cmd_dy)] <= gfx_color;
                            if (cmd_x-cmd_dy >= 0 && cmd_x-cmd_dy < GFX_W && cmd_y+cmd_dx >= 0 && cmd_y+cmd_dx < GFX_H)
                                gfx_ram[(cmd_y+cmd_dx) * GFX_W + (cmd_x-cmd_dy)] <= gfx_color;
                            if (cmd_x+cmd_dy >= 0 && cmd_x+cmd_dy < GFX_W && cmd_y-cmd_dx >= 0 && cmd_y-cmd_dx < GFX_H)
                                gfx_ram[(cmd_y-cmd_dx) * GFX_W + (cmd_x+cmd_dy)] <= gfx_color;
                            if (cmd_x-cmd_dy >= 0 && cmd_x-cmd_dy < GFX_W && cmd_y-cmd_dx >= 0 && cmd_y-cmd_dx < GFX_H)
                                gfx_ram[(cmd_y-cmd_dx) * GFX_W + (cmd_x-cmd_dy)] <= gfx_color;
                            // Step
                            cmd_dx <= cmd_dx + 1;
                            if (cmd_err < 0) begin
                                cmd_err <= cmd_err + 2 * cmd_dx + 3;
                            end else begin
                                cmd_err <= cmd_err + 2 * (cmd_dx - cmd_dy) + 5;
                                cmd_dy <= cmd_dy - 1;
                            end
                        end else
                            cmd_busy <= 0;
                    end
                    8'h13: begin // SprCopy — copy 128 bytes of shape data
                        sprite_shapes[{cmd_x[3:0], cmd_cy[3:0], cmd_cx[2:0]}]
                            <= sprite_shapes[{cmd_y[3:0], cmd_cy[3:0], cmd_cx[2:0]}];
                        if (cmd_cx == 7) begin
                            cmd_cx <= 0;
                            if (cmd_cy == 15)
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    8'h09: begin // Paint — flood fill
                        if (fill_sp > 0 && fill_sp < FILL_STACK_SIZE) begin
                            // Pop coordinate from stack
                            fill_sp <= fill_sp - 1;
                            cmd_cx <= fill_stack[fill_sp - 1][8:0];  // x
                            cmd_cy <= fill_stack[fill_sp - 1][16:9]; // y
                        end else if (fill_sp == 0) begin
                            cmd_busy <= 0; // stack empty, done
                        end else begin
                            // Process current pixel
                            if (cmd_cx < GFX_W && cmd_cy < GFX_H &&
                                gfx_ram[{9'b0, cmd_cy} * GFX_W + {8'b0, cmd_cx}] == fill_target &&
                                fill_target != gfx_color) begin
                                gfx_ram[{9'b0, cmd_cy} * GFX_W + {8'b0, cmd_cx}] <= gfx_color;
                                // Push 4 neighbors
                                if (fill_sp + 3 < FILL_STACK_SIZE) begin
                                    fill_stack[fill_sp]   <= {cmd_cy, cmd_cx + 9'd1};
                                    fill_stack[fill_sp+1] <= {cmd_cy, cmd_cx - 9'd1};
                                    fill_stack[fill_sp+2] <= {cmd_cy + 8'd1, cmd_cx};
                                    fill_stack[fill_sp+3] <= {cmd_cy - 8'd1, cmd_cx};
                                    fill_sp <= fill_sp + 4;
                                end
                            end
                        end
                    end
                    CMD_GTEXT: begin
                        // Render text string to gfx framebuffer using font ROM
                        // One pixel per clock through nested loops:
                        //   char_idx → font_row → scale_y → font_col → scale_x
                        if (gt_char_idx < gt_char_len) begin
                            if (gt_font_byte[3'd7 - gt_font_col]) begin
                                // Font bit set — compute and plot pixel
                                // px = pen_x + col*scale + scale_x,  py = start_y + row*scale + scale_y
                                if ((16'(gt_pen_x) + 16'(gt_font_col) * 16'(gt_scale) + 16'(gt_scale_x)) < GFX_W &&
                                    (16'(gt_start_y) + 16'(gt_font_row) * 16'(gt_scale) + 16'(gt_scale_y)) < GFX_H)
                                    gfx_ram[(17'(gt_start_y) + 17'(gt_font_row) * 17'(gt_scale) + 17'(gt_scale_y)) * GFX_W +
                                            (17'(gt_pen_x) + 17'(gt_font_col) * 17'(gt_scale) + 17'(gt_scale_x))] <= gfx_color;
                            end
                            // Advance innermost to outermost
                            if (gt_scale_x + 8'd1 < gt_scale)
                                gt_scale_x <= gt_scale_x + 1;
                            else begin
                                gt_scale_x <= 0;
                                if (gt_font_col < 7)
                                    gt_font_col <= gt_font_col + 1;
                                else begin
                                    gt_font_col <= 0;
                                    if (gt_scale_y + 8'd1 < gt_scale)
                                        gt_scale_y <= gt_scale_y + 1;
                                    else begin
                                        gt_scale_y <= 0;
                                        if (gt_font_row < 7) begin
                                            gt_font_row <= gt_font_row + 1;
                                            gt_font_byte <= font_rom[{gt_cur_char, gt_font_row + 3'd1}];
                                        end else begin
                                            gt_font_row <= 0;
                                            gt_char_idx <= gt_char_idx + 1;
                                            gt_pen_x <= gt_pen_x + 16'(8) * 16'(gt_scale);
                                            if (gt_char_idx + 1 < gt_char_len) begin
                                                gt_cur_char <= fio_name[gt_char_idx + 1];
                                                gt_font_byte <= font_rom[{fio_name[gt_char_idx + 1], 3'b000}];
                                            end
                                        end
                                    end
                                end
                            end
                        end else
                            cmd_busy <= 0;
                    end
                    default: cmd_busy <= 0;
                endcase
            end

            // Copper list loading state machine (copies list entries to active arrays)
            if (copper_loading) begin
                if (copper_load_idx < copper_list_count[copper_load_src]) begin
                    copper_pos[copper_load_idx] <= copper_list_pos[{copper_load_src, copper_load_idx[4:0]}];
                    copper_reg[copper_load_idx] <= copper_list_reg[{copper_load_src, copper_load_idx[4:0]}];
                    copper_val[copper_load_idx] <= copper_list_val[{copper_load_src, copper_load_idx[4:0]}];
                    copper_load_idx <= copper_load_idx + 1;
                end else begin
                    copper_count <= copper_list_count[copper_load_src];
                    copper_loading <= 0;
                end
            end

            // CPU writes
            if (cpu_we) begin
                if (vgc_reg_sel) begin
                    case (reg_offset)
                        REG_MODE:    mode <= cpu_wdata[2:0];
                        REG_BGCOL:   bg_color <= cpu_wdata[3:0];
                        REG_FGCOL:   fg_color <= cpu_wdata[3:0];
                        REG_CURSORX: cursor_x <= cpu_wdata[6:0];
                        REG_CURSORY: cursor_y <= cpu_wdata[4:0];
                        5'd5:        scroll_x <= cpu_wdata;         // ScrollX
                        5'd6:        scroll_y <= cpu_wdata;         // ScrollY
                        5'd7:        font_slot <= cpu_wdata[2:0];  // Font
                        5'd8:        gfx_color <= cpu_wdata[3:0];  // direct color write
                        5'd10:       cursor_enable <= cpu_wdata[0]; // CursorEnable
                        5'd11:       collision_ss <= 0;             // ColSt write-clears
                        5'd12:       collision_bg <= 0;             // ColBg write-clears
                        REG_BORDER:  border_color <= cpu_wdata[3:0];
                        5'd31:       irq_ctrl <= cpu_wdata;         // IrqCtrl
                        REG_CHAROUT: begin
                            case (cpu_wdata)
                                8'h08: begin
                                    if (cursor_x > 0) cursor_x <= cursor_x - 1;
                                    char_ram[screen_addr(cursor_x > 0 ? cursor_x - 1 : 0, cursor_y)] <= 8'h20;
                                end
                                8'h0A: begin
                                    if (cursor_y >= ROWS - 1) begin
                                        scroll_offset <= (scroll_offset >= ROWS - 1) ? 5'd0 : scroll_offset + 1;
                                        scroll_pending <= 1; scroll_col <= 0;
                                    end else
                                        cursor_y <= cursor_y + 1;
                                end
                                8'h0C: begin
                                    // Form feed — clear screen via state machine
                                    cursor_x <= 0; cursor_y <= 0;
                                    scroll_offset <= 0;
                                    cmd_cx <= 0; cmd_cy <= 0;
                                    cmd_busy <= 1; cmd_op <= CMD_TXTCLS;
                                end
                                8'h0D: cursor_x <= 0;
                                8'h13: begin cursor_x <= 0; cursor_y <= 0; end // HOME
                                default: begin
                                    if (cpu_wdata >= 8'h20) begin
                                        char_ram[screen_addr(cursor_x, cursor_y)] <= cpu_wdata;
                                        color_ram[screen_addr(cursor_x, cursor_y)] <= {4'b0, fg_color};
                                        if (cursor_x >= COLS - 1) begin
                                            cursor_x <= 0;
                                            if (cursor_y >= ROWS - 1) begin
                                                scroll_offset <= (scroll_offset >= ROWS - 1) ? 5'd0 : scroll_offset + 1;
                                                scroll_pending <= 1; scroll_col <= 0;
                                            end else
                                                cursor_y <= cursor_y + 1;
                                        end else
                                            cursor_x <= cursor_x + 1;
                                    end
                                end
                            endcase
                        end
                        REG_CMD: begin
                            // Command dispatch — parameter layout matches Avalonia:
                            // p0 (X0) = regs[17]|regs[18]<<8
                            // p1 (Y0) = regs[19]|regs[20]<<8
                            // p2 (X1) = regs[21]|regs[22]<<8
                            // p3 (Y1) = regs[23]|regs[24]<<8
                            if (!cmd_busy) begin
                                // Extract 16-bit params
                                cmd_x  <= {regs[18][1:0], regs[17]};
                                cmd_y  <= {regs[20][1:0], regs[19]};
                                cmd_x2 <= {regs[22][1:0], regs[21]};
                                cmd_y2 <= {regs[24][1:0], regs[23]};

                                case (cpu_wdata)
                                    CMD_PLOT: begin
                                        // Immediate: plot at (p0, p1)
                                        if ({regs[18][0], regs[17]} < GFX_W && regs[19] < GFX_H)
                                            gfx_ram[{8'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]}] <= gfx_color;
                                    end
                                    CMD_UNPLOT: begin
                                        if ({regs[18][0], regs[17]} < GFX_W && regs[19] < GFX_H)
                                            gfx_ram[{8'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]}] <= 4'h0;
                                    end
                                    CMD_LINE: begin
                                        // Bresenham setup
                                        cmd_dx <= ({regs[22][0], regs[21]} >= {regs[18][0], regs[17]})
                                                  ? ({regs[22][0], regs[21]} - {regs[18][0], regs[17]})
                                                  : ({regs[18][0], regs[17]} - {regs[22][0], regs[21]});
                                        cmd_dy <= (regs[23] >= regs[19])
                                                  ? -(10'(regs[23] - regs[19]))
                                                  : 10'(regs[19] - regs[23]);
                                        cmd_sx <= ({regs[22][0], regs[21]} >= {regs[18][0], regs[17]}) ? 10'd1 : -10'd1;
                                        cmd_sy <= (regs[23] >= regs[19]) ? 10'd1 : -10'd1;
                                        cmd_err <= (({regs[22][0], regs[21]} >= {regs[18][0], regs[17]})
                                                   ? ({regs[22][0], regs[21]} - {regs[18][0], regs[17]})
                                                   : ({regs[18][0], regs[17]} - {regs[22][0], regs[21]}))
                                                 + ((regs[23] >= regs[19])
                                                   ? -(10'(regs[23] - regs[19]))
                                                   : 10'(regs[19] - regs[23]));
                                        cmd_busy <= 1; cmd_op <= CMD_LINE;
                                    end
                                    CMD_RECT, CMD_FILL: begin
                                        // Rect outline or filled rect
                                        cmd_cx <= {regs[18][0], regs[17]};
                                        cmd_cy <= regs[19];
                                        cmd_busy <= 1;
                                        cmd_op <= cpu_wdata; // RECT or FILL
                                    end
                                    CMD_GCLS: begin
                                        cmd_cx <= 0; cmd_cy <= 0;
                                        cmd_busy <= 1; cmd_op <= CMD_GCLS;
                                    end
                                    CMD_GCOLOR: begin
                                        gfx_color <= regs[17][3:0];
                                    end
                                    // Sprite commands — P0 (regs[17]) = sprite index
                                    CMD_SPRDEF: begin
                                        // Set individual sprite pixel: P0=sprite#, P1=x, P2=y, P3=color
                                        if (regs[17] < NUM_SPRITES && regs[18] < SPR_W && regs[19] < SPR_H) begin
                                            if (regs[18][0]) // odd x: low nibble
                                                sprite_shapes[{regs[17][3:0], regs[19][3:0], regs[18][3:1]}]
                                                    <= {sprite_shapes[{regs[17][3:0], regs[19][3:0], regs[18][3:1]}][7:4], regs[20][3:0]};
                                            else // even x: high nibble
                                                sprite_shapes[{regs[17][3:0], regs[19][3:0], regs[18][3:1]}]
                                                    <= {regs[20][3:0], sprite_shapes[{regs[17][3:0], regs[19][3:0], regs[18][3:1]}][3:0]};
                                        end
                                    end
                                    CMD_SPRROW: begin
                                        // Set one row: P0=sprite#, P1=row, P2..P9=8 bytes
                                        if (regs[17] < NUM_SPRITES && regs[18] < SPR_H) begin
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b000}] <= regs[19];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b001}] <= regs[20];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b010}] <= regs[21];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b011}] <= regs[22];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b100}] <= regs[23];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b101}] <= regs[24];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b110}] <= regs[25];
                                            sprite_shapes[{regs[17][3:0], regs[18][3:0], 3'b111}] <= regs[26];
                                        end
                                    end
                                    CMD_SPRCLR: begin
                                        // Clear sprite shape — runs as state machine
                                        if (regs[17] < NUM_SPRITES) begin
                                            cmd_cx <= 0; cmd_cy <= 0;
                                            cmd_x <= {5'b0, regs[17][3:0], 1'b0};  // store sprite# in cmd_x
                                            cmd_busy <= 1; cmd_op <= CMD_SPRCLR;
                                        end
                                    end
                                    CMD_SPRPOS: begin
                                        if (regs[17] < NUM_SPRITES) begin
                                            spr_x[regs[17][3:0]] <= {regs[19][0], regs[18]};
                                            spr_y[regs[17][3:0]] <= regs[20];
                                        end
                                    end
                                    CMD_SPRENA: begin
                                        if (regs[17] < NUM_SPRITES)
                                            spr_enable[regs[17][3:0]] <= 1;
                                    end
                                    CMD_SPRDIS: begin
                                        if (regs[17] < NUM_SPRITES)
                                            spr_enable[regs[17][3:0]] <= 0;
                                    end
                                    CMD_SPRFLIP: begin
                                        if (regs[17] < NUM_SPRITES) begin
                                            spr_flip_h[regs[17][3:0]] <= regs[18][0];
                                            spr_flip_v[regs[17][3:0]] <= regs[18][1];
                                        end
                                    end
                                    CMD_SPRPRI: begin
                                        if (regs[17] < NUM_SPRITES)
                                            spr_pri[regs[17][3:0]] <= regs[18][1:0];
                                    end
                                    // Circle: P0|P1=cx, P2|P3=cy, P4|P5=radius
                                    8'h04: begin // CmdCircle
                                        cmd_x  <= {regs[18][1:0], regs[17]};  // center x
                                        cmd_y  <= {regs[20][1:0], regs[19]};  // center y
                                        cmd_x2 <= {regs[22][1:0], regs[21]};  // radius
                                        cmd_dx <= 0;                           // Bresenham x = 0
                                        cmd_dy <= {regs[22][1:0], regs[21]};  // Bresenham y = radius
                                        cmd_err <= 1 - {regs[22][1:0], regs[21]}; // err = 1 - radius
                                        cmd_busy <= 1; cmd_op <= 8'h04;
                                    end
                                    // SprCopy: P0=dest sprite#, P1=src sprite#
                                    8'h13: begin // CmdSprCopy
                                        if (regs[17] < NUM_SPRITES && regs[18] < NUM_SPRITES) begin
                                            cmd_x <= {6'b0, regs[17][3:0]};  // dest
                                            cmd_y <= {6'b0, regs[18][3:0]};  // src
                                            cmd_cx <= 0; cmd_cy <= 0;
                                            cmd_busy <= 1; cmd_op <= 8'h13;
                                        end
                                    end
                                    // SysReset
                                    8'h1F: begin
                                        cursor_x <= 0; cursor_y <= 0;
                                        mode <= 0; fg_color <= 4'd1; bg_color <= 4'd6;
                                        border_color <= 4'd6; gfx_color <= 4'd1;
                                        scroll_offset <= 0; scroll_x <= 0; scroll_y <= 0;
                                        cursor_enable <= 1; copper_enabled <= 0;
                                        copper_count <= 0; copper_index <= 0;
                                        copper_target_list <= 0; copper_active_list <= 0;
                                        copper_pending_list <= 0; copper_loading <= 0;
                                        for (int i = 0; i < COPPER_LISTS; i++)
                                            copper_list_count[i] <= 0;
                                    end
                                    // Gtext: P0|P1=x, P2|P3=y, P4=font slot, P5=scale
                                    CMD_GTEXT: begin
                                        if (fio_name_len > 0) begin
                                            gt_pen_x <= {regs[18][0], regs[17]};
                                            gt_start_y <= regs[19];
                                            gt_scale <= (regs[22] < 1) ? 8'd1 : regs[22];
                                            gt_char_idx <= 0;
                                            gt_char_len <= fio_name_len;
                                            gt_font_row <= 0;
                                            gt_font_col <= 0;
                                            gt_scale_x <= 0;
                                            gt_scale_y <= 0;
                                            gt_cur_char <= fio_name[0];
                                            gt_font_byte <= font_rom[{fio_name[0], 3'b000}];
                                            cmd_busy <= 1; cmd_op <= CMD_GTEXT;
                                        end
                                    end
                                    // Copper commands — write to target list storage
                                    8'h1B: begin // CmdCopperAdd
                                        // P0/P1=x, P2=y, P3/P4=reg, P5=value
                                        if (copper_list_count[copper_target_list] < COPPER_MAX) begin
                                            copper_list_pos[{copper_target_list, copper_list_count[copper_target_list][4:0]}]
                                                <= {1'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]};
                                            copper_list_reg[{copper_target_list, copper_list_count[copper_target_list][4:0]}]
                                                <= regs[20];
                                            copper_list_val[{copper_target_list, copper_list_count[copper_target_list][4:0]}]
                                                <= regs[22];
                                            copper_list_count[copper_target_list] <= copper_list_count[copper_target_list] + 1;
                                            // If editing active list, also update live arrays
                                            if (copper_target_list == copper_active_list) begin
                                                copper_pos[copper_count] <= {1'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]};
                                                copper_reg[copper_count] <= regs[20];
                                                copper_val[copper_count] <= regs[22];
                                                copper_count <= copper_count + 1;
                                            end
                                        end
                                    end
                                    8'h1C: begin // CmdCopperClear — clear target list
                                        copper_list_count[copper_target_list] <= 0;
                                        if (copper_target_list == copper_active_list) begin
                                            copper_count <= 0;
                                            copper_index <= 0;
                                        end
                                    end
                                    8'h1D: begin // CmdCopperEnable
                                        copper_enabled <= 1;
                                    end
                                    8'h1E: begin // CmdCopperDisable
                                        copper_enabled <= 0;
                                    end
                                    // Advanced copper list management
                                    CMD_COPPERLIST: begin // Select target list for editing
                                        copper_target_list <= regs[17][6:0];
                                    end
                                    CMD_COPPERUSE: begin // Select list to activate at next vblank
                                        copper_pending_list <= regs[17][6:0];
                                    end
                                    CMD_COPPERLISTEND: begin // Reset target to active list
                                        copper_target_list <= copper_active_list;
                                    end
                                    // Paint (flood fill): P0|P1=x, P2|P3=y
                                    8'h09: begin
                                        cmd_x <= {regs[18][1:0], regs[17]};
                                        cmd_y <= {regs[20][1:0], regs[19]};
                                        if ({regs[18][0], regs[17]} < GFX_W && regs[19] < GFX_H) begin
                                            fill_target <= gfx_ram[{8'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]}];
                                            fill_stack[0] <= {regs[19][7:0], regs[18][0], regs[17]};
                                            fill_sp <= 1;
                                            cmd_busy <= 1; cmd_op <= 8'h09;
                                        end
                                    end
                                    // MemRead: P0=space, P1|P2=addr → P3=data
                                    8'h19: begin
                                        case (regs[17]) // memory space
                                            8'd0: regs[20] <= char_ram[{regs[19], regs[18]}]; // screen
                                            8'd1: regs[20] <= color_ram[{regs[19], regs[18]}]; // color
                                            8'd2: regs[20] <= {4'b0, gfx_ram[{regs[19], regs[18]}]}; // gfx
                                            8'd3: regs[20] <= sprite_shapes[{regs[19], regs[18]}]; // sprite
                                            8'd4: regs[20] <= tile_inst.tile_data[{regs[19], regs[18]}]; // tile
                                            default: ;
                                        endcase
                                    end
                                    // MemWrite: P0=space, P1|P2=addr, P3=data
                                    8'h1A: begin
                                        case (regs[17])
                                            8'd0: char_ram[{regs[19], regs[18]}] <= regs[20];
                                            8'd1: color_ram[{regs[19], regs[18]}] <= regs[20];
                                            8'd2: gfx_ram[{regs[19], regs[18]}] <= regs[20][3:0];
                                            8'd3: sprite_shapes[{regs[19], regs[18]}] <= regs[20];
                                            8'd4: tile_inst.tile_data[{regs[19], regs[18]}] <= regs[20]; // tile
                                            default: ;
                                        endcase
                                    end
                                    default: ;
                                endcase
                            end
                        end
                        default: regs[reg_offset] <= cpu_wdata;
                    endcase
                end

                if (char_ram_sel)  char_ram[char_offset]   <= cpu_wdata;
                if (color_ram_sel) color_ram[color_offset]  <= cpu_wdata;

                // Shadow FIO name buffer for Gtext
                if (fio_len_sel)   fio_name_len <= cpu_wdata[5:0];
                if (fio_name_sel)  fio_name[cpu_addr - FIO_NAME] <= cpu_wdata;

                // Sprite register writes ($A040-$A0BF): 8 bytes per sprite
                if (spr_reg_sel) begin
                    case (spr_field)
                        3'd0: spr_x[spr_index][7:0] <= cpu_wdata;          // X low
                        3'd1: spr_x[spr_index][8]   <= cpu_wdata[0];       // X high
                        3'd2: spr_y[spr_index]       <= cpu_wdata;          // Y low
                        3'd3: ;                                              // Y high (unused, Y < 200)
                        3'd4: spr_shape[spr_index]   <= cpu_wdata[3:0];     // shape index
                        3'd5: begin                                          // flags
                            spr_flip_h[spr_index] <= cpu_wdata[0];
                            spr_flip_v[spr_index] <= cpu_wdata[1];
                            spr_enable[spr_index] <= cpu_wdata[7];
                        end
                        3'd6: spr_pri[spr_index]     <= cpu_wdata[1:0];     // priority
                        3'd7: spr_trans[spr_index]    <= cpu_wdata[3:0];     // trans color
                    endcase
                end
            end
        end
    end

    // =========================================================================
    // Tile engine instance
    // =========================================================================
    wire [11:0] tile_rgb;
    wire [1:0]  tile_opaque;

    // Pixel coordinates (computed early for tile engine)
    logic [6:0]  text_col;
    logic [4:0]  text_row, real_row;
    logic [2:0]  font_line, font_pixel;
    logic [9:0]  text_line;
    logic [8:0]  gfx_x;
    logic [7:0]  gfx_y;

    // Pre-compute gfx coordinates for tile engine (combinational)
    wire [8:0] pre_gfx_x = h_count[9:1];
    wire [7:0] pre_gfx_y = ((v_count - V_BORDER) >> 1);

    tile_engine tile_inst (
        .clk        (clk),
        .rst        (rst),
        .cpu_addr   (cpu_addr),
        .cpu_wdata  (cpu_wdata),
        .cpu_we     (cpu_we),
        .cpu_rdata  (tile_rdata),
        .cpu_re     (cpu_re),
        .dma_addr   (tile_dma_addr),
        .dma_data   (tile_dma_data),
        .dma_active (tile_dma_active),
        .pixel_x    (pre_gfx_x),
        .pixel_y    (pre_gfx_y),
        .pixel_valid(in_text_area),
        .tile_rgb   (tile_rgb),
        .tile_opaque(tile_opaque)
    );

    // =========================================================================
    // Pixel pipeline
    // =========================================================================

    always_comb begin
        text_col   = h_count[9:3];
        font_pixel = h_count[2:0];
        text_line  = (v_count - V_BORDER) >> 1;
        text_row   = text_line[7:3];
        font_line  = text_line[2:0];
        real_row   = text_row + scroll_offset;
        if (real_row >= ROWS) real_row = real_row - ROWS;

        // Graphics coordinates (320x200, pixel doubled to 640x400)
        gfx_x = h_count[9:1];           // h_count / 2
        gfx_y = text_line[7:0];         // 0-199
    end

    // Pixel generation
    logic [7:0]  cur_char;
    logic [3:0]  cur_fg, cur_gfx;
    logic [7:0]  font_byte;
    logic        pixel_on;
    logic [11:0] text_pixel, gfx_pixel, pixel_color;

    // Sprite scratch signals
    logic signed [9:0] spr_dx [0:15];
    logic signed [9:0] spr_dy [0:15];
    logic [3:0]  spr_fx [0:15];
    logic [3:0]  spr_fy [0:15];
    logic [10:0] spr_sa [0:15];
    logic [3:0]  spr_pix [0:15];
    logic        spr_vis [0:15];

    // Sprite rendering — check all 16 sprites for current pixel
    logic [3:0]  spr_pixel;      // winning sprite color (0 = transparent)
    logic [1:0]  spr_pixel_pri;  // winning sprite priority
    logic        spr_pixel_hit;  // any sprite visible here

    always_comb begin
        // Text layer
        cur_char   = char_ram[real_row * COLS + text_col];
        cur_fg     = color_ram[real_row * COLS + text_col][3:0];
        font_byte  = font_rom[{cur_char, font_line}];
        pixel_on   = font_byte[3'd7 - font_pixel];
        text_pixel = pixel_on ? palette[cur_fg] : palette[bg_color];

        // Graphics layer
        cur_gfx    = gfx_ram[{8'b0, gfx_y} * GFX_W + {7'b0, gfx_x}];
        gfx_pixel  = palette[cur_gfx];

        // Sprite layer — compute per-sprite visibility and pixel
        for (int s = 0; s < 16; s++) begin
            spr_dx[s] = {1'b0, gfx_x} - {1'b0, spr_x[s]};
            spr_dy[s] = {2'b0, gfx_y} - {2'b0, spr_y[s]};
            spr_fx[s] = spr_flip_h[s] ? (4'd15 - spr_dx[s][3:0]) : spr_dx[s][3:0];
            spr_fy[s] = spr_flip_v[s] ? (4'd15 - spr_dy[s][3:0]) : spr_dy[s][3:0];
            spr_sa[s] = {spr_shape[s], spr_fy[s], spr_fx[s][3:1]};
            spr_pix[s] = spr_fx[s][0] ? sprite_shapes[spr_sa[s]][3:0]
                                       : sprite_shapes[spr_sa[s]][7:4];
            spr_vis[s] = spr_enable[s] && in_text_area &&
                         spr_dx[s] >= 0 && spr_dx[s] < SPR_W &&
                         spr_dy[s] >= 0 && spr_dy[s] < SPR_H &&
                         spr_pix[s] != spr_trans[s];
        end

        // Pick winning sprite — highest index wins on overlap
        spr_pixel     = 4'h0;
        spr_pixel_pri = 2'd0;
        spr_pixel_hit = 0;
        for (int s = 0; s < 16; s++) begin
            if (spr_vis[s]) begin
                spr_pixel     = spr_pix[s];
                spr_pixel_pri = spr_pri[s];
                spr_pixel_hit = 1;
            end
        end

        // Layer compositing with sprite priorities
        // Priority 0: behind gfx/text, 1: over gfx/text, 2: topmost
        if (!visible)
            pixel_color = 12'h000;
        else if (!in_text_area)
            pixel_color = palette[border_color];
        else begin
            // Base layer from mode
            case (mode)
                3'd0: pixel_color = text_pixel;
                3'd1: pixel_color = (cur_gfx != 0) ? gfx_pixel : text_pixel;
                3'd2: pixel_color = pixel_on ? palette[cur_fg] :
                                    (cur_gfx != 0) ? gfx_pixel : palette[bg_color];
                3'd3: pixel_color = (cur_gfx != 0) ? gfx_pixel : palette[bg_color];
                3'd4: pixel_color = (tile_opaque != 0) ? tile_rgb : palette[bg_color];
                default: pixel_color = text_pixel;
            endcase

            // Sprite overlay based on priority
            if (spr_pixel_hit) begin
                if (mode == 3'd4) begin
                    // Mode 4: tiles + sprites with tile priority
                    // Pri 0 sprites: behind tiles
                    // Pri 1 sprites: between tile layers (behind pri=2 tiles, over pri=1 tiles)
                    // Pri 2 sprites: over everything
                    if (spr_pixel_pri == 2'd0) begin
                        if (tile_opaque == 0) pixel_color = palette[spr_pixel];
                    end else if (spr_pixel_pri == 2'd1) begin
                        if (tile_opaque < 2'd2) pixel_color = palette[spr_pixel];
                    end else begin
                        pixel_color = palette[spr_pixel];
                    end
                end else begin
                    if (spr_pixel_pri == 2'd0) begin
                        // Priority 0: behind text/gfx — only shows through background
                        if (!pixel_on && cur_gfx == 0)
                            pixel_color = palette[spr_pixel];
                    end else begin
                        // Priority 1+2: over everything
                        pixel_color = palette[spr_pixel];
                    end
                end
            end
        end
    end

    // Cursor — 250ms toggle = 15 frames at 60Hz, 2 full blinks/sec
    logic [4:0] blink_count;
    logic       cursor_blink;
    always_ff @(posedge clk) begin
        if (h_count == 0 && v_count == 0) begin
            if (blink_count >= 14) begin
                blink_count <= 0;
                cursor_blink <= ~cursor_blink;
            end else
                blink_count <= blink_count + 1;
        end
    end
    wire cursor_here  = cursor_enable && in_text_area && (text_col == cursor_x) && (text_row == cursor_y);

    // =========================================================================
    // Copper execution — apply register writes at beam positions
    // =========================================================================
    wire [16:0] beam_pos = {8'b0, gfx_y} * GFX_W + {7'b0, gfx_x};

    always_ff @(posedge clk) begin
        if (rst) begin
            copper_index <= 0;
            copper_active_list <= 0;
        end else begin
            // Vblank: reset index and swap pending→active list if changed
            if (h_count == 0 && v_count == V_ACTIVE) begin
                copper_index <= 0;
                if (copper_pending_list != copper_active_list && !copper_loading) begin
                    copper_active_list <= copper_pending_list;
                    copper_loading <= 1;
                    copper_load_idx <= 0;
                    copper_load_src <= copper_pending_list;
                end
            end

            // Fire copper entries when beam reaches their position
            if (copper_enabled && in_text_area &&
                copper_index < copper_count &&
                beam_pos >= copper_pos[copper_index]) begin
                // Apply the register write
                case (copper_reg[copper_index])
                    8'd1: bg_color     <= copper_val[copper_index][3:0]; // REG_BGCOL
                    8'd2: fg_color     <= copper_val[copper_index][3:0]; // REG_FGCOL
                    8'd13: border_color <= copper_val[copper_index][3:0]; // REG_BORDER
                    default: regs[copper_reg[copper_index][4:0]] <= copper_val[copper_index];
                endcase
                copper_index <= copper_index + 1;
            end
        end
    end

    // Raster IRQ — fire when beam reaches configured line
    assign irq_out = irq_ctrl[0] && (v_count == {3'b0, irq_ctrl[7:1]} * 2 + V_BORDER);

    // Collision detection — accumulate during sprite rendering
    always_ff @(posedge clk) begin
        if (h_count == 0 && v_count == 0) begin
            // Start of frame — clear collision for this frame
            // (actual clearing happens on CPU read, but we accumulate here)
        end
        // Check for sprite overlaps during rendering
        if (in_text_area) begin
            for (int s1 = 0; s1 < 15; s1++)
                for (int s2 = s1 + 1; s2 < 16; s2++)
                    if (spr_vis[s1] && spr_vis[s2])
                        collision_ss <= collision_ss | (1 << (s1 < 8 ? s1 : s2));
            // Sprite-background: any visible sprite over non-zero gfx pixel
            for (int s = 0; s < 16; s++)
                if (spr_vis[s] && cur_gfx != 0)
                    collision_bg <= collision_bg | (1 << s[2:0]);
        end
    end

    // Output
    always_ff @(posedge clk) begin
        vid_hsync <= ~h_sync_area;
        vid_vsync <= ~v_sync_area;
        vid_de    <= visible;

        if (cursor_here && cursor_blink) begin
            vid_r <= palette[fg_color][11:8];
            vid_g <= palette[fg_color][7:4];
            vid_b <= palette[fg_color][3:0];
        end else begin
            vid_r <= pixel_color[11:8];
            vid_g <= pixel_color[7:4];
            vid_b <= pixel_color[3:0];
        end
    end

    // =========================================================================
    // Blitter memory port — read/write access to VGC internal memories
    // =========================================================================
    // Combinational read
    always_comb begin
        blt_rdata = 8'h00;
        if (blt_re) begin
            case (blt_space)
                3'd1: blt_rdata = char_ram[blt_addr[10:0]];
                3'd2: blt_rdata = color_ram[blt_addr[10:0]];
                3'd3: blt_rdata = {4'b0, gfx_ram[blt_addr]};
                3'd4: blt_rdata = sprite_shapes[blt_addr[10:0]];
                3'd6: blt_rdata = tile_inst.tile_data[blt_addr[14:0]];
                default: blt_rdata = 8'h00;
            endcase
        end
    end

    // Sequential write
    always_ff @(posedge clk) begin
        if (blt_we) begin
            case (blt_space)
                3'd1: char_ram[blt_addr[10:0]]   <= blt_wdata;
                3'd2: color_ram[blt_addr[10:0]]   <= blt_wdata;
                3'd3: gfx_ram[blt_addr]           <= blt_wdata[3:0];
                3'd4: sprite_shapes[blt_addr[10:0]] <= blt_wdata;
                3'd6: tile_inst.tile_data[blt_addr[14:0]] <= blt_wdata;
                default: ;
            endcase
        end
    end

endmodule
