// VGC — Virtual Graphics Controller (NOVA — top level)
// 80x25 text + 320x200 block graphics + tile engine + command processor
// Generates standard VGA 640x480@60Hz RGB + sync signals
//
// Modes: 0=text only, 1=gfx over text, 2=text over gfx, 3=gfx only, 4=tiles+sprites
// Drawing: command register at $A010, parameters at $A011-$A01F
//
// Instantiates sub-modules: vgc_timing (VESTA), vgc_text (SCRIBE),
// vgc_gfx (CANVAS), vgc_sprites (PIXIE), vgc_copper (CONDUCTOR)
// Plus the existing tile_engine (MOSAIC)

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

    // Blitter memory port — read/write access to VGC internal memories.
    // Width is 17 bits so gfx_mem (76800 pixels) is fully addressable.
    // Other spaces use fewer bits (char/color 13, sprite 11) and ignore the top.
    input  logic [2:0]  blt_space,    // 1=char, 2=color, 3=gfx, 4=sprite, 6=tile
    input  logic [16:0] blt_addr,
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

    localparam COLS     = 80,  ROWS    = 60;
    localparam CHAR_W   = 8,   CHAR_H  = 8;
    localparam TEXT_H   = ROWS * CHAR_H;               // 480 pixels (1:1, no doubling)
    localparam V_BORDER = (V_ACTIVE - TEXT_H) / 2;     // 0 lines (no border)

    localparam GFX_W    = 320, GFX_H   = 240;
    localparam GFX_SIZE = GFX_W * GFX_H;               // 76800 pixels

    // Address map
    localparam VGC_BASE       = 16'hA000;
    localparam VGC_REGS_END   = 16'hA01F;
    localparam SPR_REG_BASE   = 16'hA040;
    localparam SPR_REG_END    = 16'hA0BF;
    localparam CHAR_RAM_BASE  = 16'hAA00;
    localparam CHAR_RAM_END   = 16'hB1CF;
    localparam COLOR_RAM_BASE = 16'hB1D0;
    localparam COLOR_RAM_END  = 16'hB99F;

    // Register offsets
    localparam REG_MODE    = 0;
    localparam REG_BGCOL   = 1;
    localparam REG_FGCOL   = 2;
    localparam REG_CURSORX = 3;
    localparam REG_CURSORY = 4;
    localparam REG_BORDER  = 13;
    localparam REG_CHAROUT = 14;
    localparam REG_CHARIN  = 15;
    localparam REG_CMD     = 16;

    // Drawing commands
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
    localparam CMD_SPRDEF  = 8'h10;
    localparam CMD_SPRROW  = 8'h11;
    localparam CMD_SPRCLR  = 8'h12;
    localparam CMD_SPRCOPY = 8'h13;
    localparam CMD_SPRPOS  = 8'h14;
    localparam CMD_SPRENA  = 8'h15;
    localparam CMD_SPRDIS  = 8'h16;
    localparam CMD_SPRFLIP = 8'h17;
    localparam CMD_SPRPRI  = 8'h18;

    // Copper commands
    localparam CMD_COPPERLIST    = 8'h20;
    localparam CMD_COPPERUSE     = 8'h21;
    localparam CMD_COPPERLISTEND = 8'h22;

    // Internal ops
    localparam CMD_TXTCLS  = 8'hF0;

    localparam NUM_SPRITES = 16;
    localparam SPR_W       = 16;
    localparam SPR_H       = 16;

    // =========================================================================
    // Timing sub-module (VESTA)
    // =========================================================================
    logic [9:0] h_count, v_count;
    logic       h_sync_area, v_sync_area, h_visible, v_visible, visible, in_text_area;
    logic [9:0] h_count_d1, h_count_d2, v_count_d1, v_count_d2;
    logic       visible_d1, visible_d2;
    logic       in_text_area_d1, in_text_area_d2;
    logic       h_sync_area_d1, h_sync_area_d2;
    logic       v_sync_area_d1, v_sync_area_d2;
    logic [6:0] text_col;
    logic [5:0] text_row, real_row;
    logic [2:0] font_line, font_pixel;
    logic [8:0] gfx_x;
    logic [7:0] gfx_y;
    logic [9:0] text_line;
    logic [6:0] text_col_d1, text_col_d2;
    logic [5:0] text_row_d1, text_row_d2;
    logic [2:0] font_pixel_d1, font_pixel_d2;
    logic [8:0] gfx_x_d1, gfx_x_d2;
    logic [7:0] gfx_y_d1, gfx_y_d2;
    logic [2:0] font_line_d1;
    logic [8:0] pre_gfx_x;
    logic [7:0] pre_gfx_y;
    logic [5:0] scroll_offset;

    vgc_timing timing_inst (
        .clk(clk), .rst(rst),
        .h_count(h_count), .v_count(v_count),
        .h_sync_area(h_sync_area), .v_sync_area(v_sync_area),
        .h_visible(h_visible), .v_visible(v_visible),
        .visible(visible), .in_text_area(in_text_area),
        .h_count_d1(h_count_d1), .h_count_d2(h_count_d2),
        .v_count_d1(v_count_d1), .v_count_d2(v_count_d2),
        .visible_d1(visible_d1), .visible_d2(visible_d2),
        .in_text_area_d1(in_text_area_d1), .in_text_area_d2(in_text_area_d2),
        .h_sync_area_d1(h_sync_area_d1), .h_sync_area_d2(h_sync_area_d2),
        .v_sync_area_d1(v_sync_area_d1), .v_sync_area_d2(v_sync_area_d2),
        .text_col(text_col), .text_row(text_row),
        .font_line(font_line), .font_pixel(font_pixel),
        .gfx_x(gfx_x), .gfx_y(gfx_y),
        .text_line(text_line), .real_row(real_row),
        .text_col_d1(text_col_d1), .text_col_d2(text_col_d2),
        .text_row_d1(text_row_d1), .text_row_d2(text_row_d2),
        .font_pixel_d1(font_pixel_d1), .font_pixel_d2(font_pixel_d2),
        .gfx_x_d1(gfx_x_d1), .gfx_x_d2(gfx_x_d2),
        .gfx_y_d1(gfx_y_d1), .gfx_y_d2(gfx_y_d2),
        .font_line_d1(font_line_d1),
        .scroll_offset(scroll_offset),
        .pre_gfx_x(pre_gfx_x), .pre_gfx_y(pre_gfx_y)
    );

    // =========================================================================
    // Palette (16 C64-style colors → 12-bit RGB)
    // =========================================================================
    logic [11:0] palette [0:15];
    initial begin
        palette[0]  = 12'h000; palette[1]  = 12'hFFF; palette[2]  = 12'h800; palette[3]  = 12'hAFE;
        palette[4]  = 12'hC4C; palette[5]  = 12'h0C5; palette[6]  = 12'h00A; palette[7]  = 12'hEE7;
        palette[8]  = 12'hD85; palette[9]  = 12'h640; palette[10] = 12'hF77; palette[11] = 12'h333;
        palette[12] = 12'h777; palette[13] = 12'h8F6; palette[14] = 12'h08F; palette[15] = 12'hBBB;
    end

    // =========================================================================
    // Text sub-module (SCRIBE) — dpram port A signals
    // =========================================================================
    logic [12:0] char_a_addr;
    logic [7:0]  char_a_din;
    logic        char_a_we;
    logic [7:0]  char_a_dout;
    logic [12:0] color_a_addr;
    logic [7:0]  color_a_din;
    logic        color_a_we;
    logic [7:0]  color_a_dout;
    logic [10:0] font_a_addr;
    logic [7:0]  font_a_din;
    logic        font_a_we;
    logic [7:0]  font_a_dout;
    logic [7:0]  char_b_dout, color_b_dout, font_b_dout;

    vgc_text text_inst (
        .clk(clk), .rst(rst),
        .char_a_addr(char_a_addr), .char_a_din(char_a_din),
        .char_a_we(char_a_we), .char_a_dout(char_a_dout),
        .color_a_addr(color_a_addr), .color_a_din(color_a_din),
        .color_a_we(color_a_we), .color_a_dout(color_a_dout),
        .font_a_addr(font_a_addr), .font_a_din(font_a_din),
        .font_a_we(font_a_we), .font_a_dout(font_a_dout),
        .real_row(real_row), .text_col(text_col), .font_line(font_line), .font_line_d1(font_line_d1),
        .char_b_dout(char_b_dout), .color_b_dout(color_b_dout),
        .font_b_dout(font_b_dout)
    );

    // =========================================================================
    // Graphics sub-module (CANVAS) — dpram port A signals
    // =========================================================================
    // 17-bit for 76800 pixel GFX memory (320×240)
    logic [16:0] gfx_a_addr;
    logic [3:0]  gfx_a_din;
    logic        gfx_a_we;
    logic [3:0]  gfx_a_dout;
    logic [3:0]  gfx_b_dout;

    vgc_gfx gfx_inst (
        .clk(clk), .rst(rst),
        .gfx_a_addr(gfx_a_addr), .gfx_a_din(gfx_a_din),
        .gfx_a_we(gfx_a_we), .gfx_a_dout(gfx_a_dout),
        .gfx_y(gfx_y), .gfx_x(gfx_x),
        .gfx_b_dout(gfx_b_dout)
    );

    // =========================================================================
    // Drawing coprocessor (ARTIST)
    // =========================================================================
    // Pack fio_name array into flat vector for ARTIST
    logic [64*8-1:0] fio_name_flat;
    always_comb begin
        for (int i = 0; i < 64; i++)
            fio_name_flat[i*8 +: 8] = fio_name[i];
    end

    artist artist_inst (
        .clk(clk), .rst(rst),
        .cmd_valid(artist_cmd_valid),
        .cmd_code(artist_cmd_code),
        .cmd_x0({regs[18][0], regs[17]}),
        .cmd_y0(regs[19]),
        .cmd_x1({regs[22][0], regs[21]}),
        .cmd_y1(regs[23]),
        .cmd_color(gfx_color),
        .cmd_radius(regs[21]),
        .busy(artist_busy),
        .gfx_addr(artist_gfx_addr),
        .gfx_wdata(artist_gfx_wdata),
        .gfx_we(artist_gfx_we),
        .gfx_raddr(artist_gfx_raddr),
        .gfx_rdata_raw(artist_gfx_rdata),
        .gfx_re(artist_gfx_re),
        .font_addr(artist_font_addr),
        .font_data(artist_font_data),
        .font_re(artist_font_re),
        .gt_char_flat(fio_name_flat),
        .gt_char_len(fio_name_len),
        .gt_scale_in(regs[22])
    );

    // ARTIST read port: connect to gfx_ram dout_a (flood fill reads)
    assign artist_gfx_rdata = gfx_a_dout;

    // ARTIST font port: connect to font_rom dout_a
    assign artist_font_data = font_a_dout;

    // =========================================================================
    // Sprite sub-module (PIXIE)
    // =========================================================================
    logic [10:0] spr_a_addr;
    logic [7:0]  spr_a_din;
    logic        spr_a_we;
    logic [7:0]  spr_a_dout;

    // Sprite attributes — small register arrays (no dpram needed)
    logic [8:0]  spr_x [0:15];
    logic [7:0]  spr_y [0:15];
    logic        spr_enable [0:15];
    logic        spr_flip_h [0:15];
    logic        spr_flip_v [0:15];
    logic [1:0]  spr_pri [0:15];
    logic [3:0]  spr_shape [0:15];
    logic [3:0]  spr_trans [0:15];

    initial begin
        for (int i = 0; i < 16; i++) begin
            spr_x[i] = 0; spr_y[i] = 0;
            spr_enable[i] = 0; spr_flip_h[i] = 0; spr_flip_v[i] = 0;
            spr_pri[i] = 2'd1; spr_shape[i] = i[3:0];
        end
    end

    logic [3:0]  spr_pixel;
    logic [1:0]  spr_pixel_pri;
    logic        spr_pixel_hit;
    logic [7:0]  spr_collision_bg_bits;

    // Pack sprite attribute arrays into flat vectors for sub-module
    logic [16*9-1:0]  spr_x_flat;
    logic [16*8-1:0]  spr_y_flat;
    logic [15:0]      spr_enable_flat;
    logic [15:0]      spr_flip_h_flat;
    logic [15:0]      spr_flip_v_flat;
    logic [16*2-1:0]  spr_pri_flat;
    logic [16*4-1:0]  spr_shape_flat;
    logic [16*4-1:0]  spr_trans_flat;

    always_comb begin
        for (int i = 0; i < 16; i++) begin
            spr_x_flat[i*9 +: 9]     = spr_x[i];
            spr_y_flat[i*8 +: 8]     = spr_y[i];
            spr_enable_flat[i]        = spr_enable[i];
            spr_flip_h_flat[i]        = spr_flip_h[i];
            spr_flip_v_flat[i]        = spr_flip_v[i];
            spr_pri_flat[i*2 +: 2]   = spr_pri[i];
            spr_shape_flat[i*4 +: 4] = spr_shape[i];
            spr_trans_flat[i*4 +: 4] = spr_trans[i];
        end
    end

    vgc_sprites sprite_inst (
        .clk(clk), .rst(rst),
        .spr_a_addr(spr_a_addr), .spr_a_din(spr_a_din),
        .spr_a_we(spr_a_we), .spr_a_dout(spr_a_dout),
        .h_count(h_count), .v_count(v_count),
        .in_text_area_d2(in_text_area_d2), .gfx_x_d2(gfx_x_d2),
        .spr_x_flat(spr_x_flat), .spr_y_flat(spr_y_flat),
        .spr_enable_flat(spr_enable_flat),
        .spr_flip_h_flat(spr_flip_h_flat), .spr_flip_v_flat(spr_flip_v_flat),
        .spr_pri_flat(spr_pri_flat), .spr_shape_flat(spr_shape_flat),
        .spr_trans_flat(spr_trans_flat),
        .spr_pixel(spr_pixel), .spr_pixel_pri(spr_pixel_pri),
        .spr_pixel_hit(spr_pixel_hit),
        .collision_bg_bits(spr_collision_bg_bits)
    );

    // =========================================================================
    // Copper sub-module (CONDUCTOR)
    // =========================================================================
    // Copper — raster-synchronized register writer with multi-list support
    localparam COPPER_MAX   = 32;
`ifdef SYNTHESIS
    localparam COPPER_LISTS = 8;    // 8 lists for FPGA (saves ~127K bits of registers)
`else
    localparam COPPER_LISTS = 128;  // 128 lists for simulation (full software compat)
`endif

    // Active list: used during rendering
    logic [16:0] copper_pos   [0:COPPER_MAX-1];
    logic [7:0]  copper_reg   [0:COPPER_MAX-1];
    logic [7:0]  copper_val   [0:COPPER_MAX-1];
    logic [8:0]  copper_count;
    logic [8:0]  copper_index;
    logic        copper_enabled;

    // Multi-list storage
    logic [16:0] copper_list_pos [0:COPPER_LISTS*COPPER_MAX-1];
    logic [7:0]  copper_list_reg [0:COPPER_LISTS*COPPER_MAX-1];
    logic [7:0]  copper_list_val [0:COPPER_LISTS*COPPER_MAX-1];
    logic [8:0]  copper_list_count [0:COPPER_LISTS-1];
    logic [6:0]  copper_target_list;
    logic [6:0]  copper_active_list;
    logic [6:0]  copper_pending_list;

    // Copper list copy state machine
    logic        copper_loading;
    logic [8:0]  copper_load_idx;
    logic [6:0]  copper_load_src;

    // Copper fire signals from sub-module
    logic        copper_fire;
    logic [7:0]  copper_fire_reg;
    logic [7:0]  copper_fire_val;

    // Pack copper active list arrays into flat vectors for sub-module
    logic [32*17-1:0] copper_pos_flat;
    logic [32*8-1:0]  copper_reg_flat;
    logic [32*8-1:0]  copper_val_flat;

    always_comb begin
        for (int i = 0; i < COPPER_MAX; i++) begin
            copper_pos_flat[i*17 +: 17] = copper_pos[i];
            copper_reg_flat[i*8 +: 8]   = copper_reg[i];
            copper_val_flat[i*8 +: 8]   = copper_val[i];
        end
    end

    vgc_copper copper_inst (
        .clk(clk), .rst(rst),
        .h_count(h_count), .v_count(v_count),
        .in_text_area(in_text_area),
        .gfx_x(gfx_x), .gfx_y(gfx_y),
        .copper_pos_flat(copper_pos_flat), .copper_reg_flat(copper_reg_flat),
        .copper_val_flat(copper_val_flat), .copper_count(copper_count),
        .copper_enabled(copper_enabled),
        .copper_index(copper_index),
        .copper_fire(copper_fire),
        .copper_fire_reg(copper_fire_reg),
        .copper_fire_val(copper_fire_val)
    );

    initial begin
        for (int i = 0; i < COPPER_MAX; i++) begin
            copper_pos[i] = 0; copper_reg[i] = 0; copper_val[i] = 0;
        end
        for (int i = 0; i < COPPER_LISTS * COPPER_MAX; i++) begin
            copper_list_pos[i] = 0; copper_list_reg[i] = 0; copper_list_val[i] = 0;
        end
        for (int i = 0; i < COPPER_LISTS; i++)
            copper_list_count[i] = 0;
        copper_count = 0; copper_enabled = 0;
        copper_target_list = 0; copper_active_list = 0;
        copper_pending_list = 0;
        copper_loading = 0; copper_load_idx = 0; copper_load_src = 0;
    end

    // =========================================================================
    // VGC Registers
    // =========================================================================
    logic [7:0]  regs [0:31];
    logic [6:0]  cursor_x;
    logic [5:0]  cursor_y;
    logic [3:0]  fg_color, bg_color, border_color, gfx_color;
    logic [2:0]  mode;
    logic        cursor_enable;
    // Frame counter at $A008 (VGC_FRAME in basic.asm:8806). Increments once
    // per vblank entry — EhBASIC's SPRITESET/SPRPRI use `LDA VGC_FRAME;
    // CMP VGC_FRAME; BEQ loop` to wait for vblank sync. Without this,
    // any BASIC command that syncs on VGC_FRAME hangs the CPU forever.
    logic [7:0]  frame_counter;
    logic [7:0]  scroll_x, scroll_y;
    // Keyboard input — 256-byte ring buffer (ZipCPU sfifo, public domain,
    // formally verified). Replaces the single-entry char_in_reg that
    // previously dropped rapid keystrokes. See rtl/thirdparty/sfifo.v.
    wire        key_fifo_full;
    wire        key_fifo_empty;
    wire [7:0]  key_fifo_head;   // head byte (valid when !empty)
    wire [8:0]  key_fifo_fill;   // 0..256
    logic       key_fifo_rd;     // pulsed high for one cycle to pop head
    logic       charin_sel_prev; // previous cycle's "CPU addressing CHARIN" state (for edge detect)

    // "char_in_reg" — legacy head-read wire so existing register-mux code
    // keeps working. Returns 0 when empty (matches original behavior).
    wire [7:0]  char_in_reg = key_fifo_empty ? 8'h00 : key_fifo_head;

    // DEL (0x7F) → BS (0x08) translation applied at push time.
    // Previously done in top.sv's key_reg latch which has been removed.
    wire [7:0] key_data_xlat = (key_data == 8'h7F) ? 8'h08 : key_data;

    sfifo #(
        .BW(8),
        .LGFLEN(8)               // 2^8 = 256 entries
    ) key_fifo_inst (
        .i_clk  (clk),
        .i_reset(rst),
        .i_wr   (key_valid && !key_fifo_full),
        .i_data (key_data_xlat),
        .o_full (key_fifo_full),
        .o_fill (key_fifo_fill),
        .i_rd   (key_fifo_rd),
        .o_data (key_fifo_head),
        .o_empty(key_fifo_empty)
    );
    logic [2:0]  font_slot;
    logic [7:0]  collision_ss;
    logic [7:0]  collision_bg;
    logic [7:0]  irq_ctrl;
    logic        irq_raster_pending;
    logic        scroll_pending;
    logic [6:0]  scroll_col;

    // Gtext / FIO name buffer
    localparam FIO_NAME_LEN = 16'hB9A3;
    localparam FIO_NAME     = 16'hB9B0;

    // FIO name buffer shadow (snooped from CPU writes for Gtext)
    logic [7:0]  fio_name [0:63];
    logic [5:0]  fio_name_len;

    initial begin
        for (int i = 0; i < 64; i++) fio_name[i] = 0;
        fio_name_len = 0;
    end

    // Command state machine
    logic        cmd_busy;
    logic [7:0]  cmd_op;
    logic signed [9:0] cmd_x, cmd_y, cmd_x2, cmd_y2;
    logic [8:0]  cmd_cx;
    logic [7:0]  cmd_cy;

    // SPRROW serialization
    logic [2:0]  sprrow_count;
    logic [7:0]  sprrow_data [0:7];
    logic [3:0]  sprrow_spr;
    logic [3:0]  sprrow_row;

    // SprCopy state
    // sprcopy_phase: 0=issue read, 1=wait for dpram latency, 2=capture+write
    logic [1:0]  sprcopy_phase;
    logic [7:0]  sprcopy_data;
    // sprdef_wait: delays the SPRDEF read-modify-write completion by one
    // cycle so spr_a_dout reflects the freshly-issued read address.
    logic        sprdef_wait;

    // MemRead latency state — 2-bit counter: 2 = just dispatched, 1 = dpram
    // now valid to capture, 0 = idle. The original 1-bit flag captured
    // *_a_dout the same cycle the read address took effect (stale read).
    logic [1:0]  memread_pending;
    logic [2:0]  memread_space;

    // Port A write signals from command processor
    logic [12:0] cmd_char_addr;
    logic [7:0]  cmd_char_din;
    logic        cmd_char_we;
    logic [12:0] cmd_color_addr;
    logic [7:0]  cmd_color_din;
    logic        cmd_color_we;
    logic [16:0] cmd_gfx_addr;
    logic [3:0]  cmd_gfx_din;
    logic        cmd_gfx_we;
    logic        cmd_gfx_re;
    logic [10:0] cmd_spr_addr;
    logic [7:0]  cmd_spr_din;
    logic        cmd_spr_we;
    logic        cmd_spr_re;

    // ARTIST drawing coprocessor signals
    logic        artist_cmd_valid;
    logic [7:0]  artist_cmd_code;
    logic [16:0] artist_gfx_addr;
    logic [3:0]  artist_gfx_wdata;
    logic        artist_gfx_we;
    logic [16:0] artist_gfx_raddr;
    logic [3:0]  artist_gfx_rdata;
    logic        artist_gfx_re;
    logic [10:0] artist_font_addr;
    logic [7:0]  artist_font_data;
    logic        artist_font_re;
    logic        artist_busy;

    initial begin
        for (int i = 0; i < 32; i++) regs[i] = 0;
        cursor_x = 0; cursor_y = 0;
        fg_color = 4'd1; bg_color = 4'd6; border_color = 4'd6;
        gfx_color = 4'd1; mode = 0; cursor_enable = 1;
        frame_counter = 0;
        scroll_x = 0; scroll_y = 0;
        for (int i = 0; i < 16; i++) spr_trans[i] = 4'd0;
        scroll_offset = 0; scroll_pending = 0; scroll_col = 0;
        key_fifo_rd = 0; cmd_busy = 0;
        font_slot = 0; collision_ss = 0; collision_bg = 0;
        irq_ctrl = 0; irq_raster_pending = 0;
        sprrow_count = 0; sprcopy_phase = 0;
        memread_pending = 2'd0;
        vgc_tile_addr = 0; vgc_tile_wdata = 0; vgc_tile_we = 0; vgc_tile_re = 0;
        artist_cmd_valid = 0;
    end

    // Keyboard input is handled inside the main always_ff block below
    // to avoid a multi-driver conflict on char_in_reg (which is also
    // cleared in the reset branch of that block).

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
    wire [7:0]  tile_rdata;
    wire [4:0]  reg_offset   = cpu_addr[4:0];
    wire [3:0]  spr_index    = cpu_addr[6:3];
    wire [2:0]  spr_field    = cpu_addr[2:0];
    wire [12:0] char_offset  = {2'b0, cpu_addr - CHAR_RAM_BASE};
    wire [12:0] color_offset = {2'b0, cpu_addr - COLOR_RAM_BASE};

    // =========================================================================
    // Screen address helper
    // =========================================================================
    function automatic logic [12:0] screen_addr(input logic [6:0] col, input logic [5:0] row);
        logic [5:0] rr;
        rr = row + scroll_offset;
        if (rr >= ROWS) rr = rr - ROWS;
        screen_addr = {7'b0, rr} * COLS + {6'b0, col};
    endfunction

    // =========================================================================
    // CPU reads
    // =========================================================================
    logic        cpu_rd_pending;
    logic [2:0]  cpu_rd_source;
    logic [7:0]  cpu_rd_latch;

    always_ff @(posedge clk) begin
        if (rst) begin
            cpu_rd_pending <= 0;
            cpu_rd_source <= 0;
        end else begin
            cpu_rd_pending <= 0;
            cpu_rd_source <= 0;
            if (cpu_re && cpu_ce) begin
                if (char_ram_sel) begin
                    cpu_rd_pending <= 1;
                    cpu_rd_source <= 3'd1;
                end else if (color_ram_sel) begin
                    cpu_rd_pending <= 1;
                    cpu_rd_source <= 3'd2;
                end
            end
            if (cpu_rd_pending) begin
                case (cpu_rd_source)
                    3'd1: cpu_rd_latch <= char_a_dout;
                    3'd2: cpu_rd_latch <= color_a_dout;
                    default: ;
                endcase
            end
        end
    end

    always_comb begin
        cpu_rdata = 8'h00;
        if (vgc_reg_sel) begin
            case (reg_offset)
                REG_MODE:    cpu_rdata = {5'b0, mode};
                REG_BGCOL:   cpu_rdata = {4'b0, bg_color};
                REG_FGCOL:   cpu_rdata = {4'b0, fg_color};
                REG_CURSORX: cpu_rdata = {1'b0, cursor_x};
                REG_CURSORY: cpu_rdata = {2'b0, cursor_y};
                5'd5:        cpu_rdata = scroll_x;
                5'd6:        cpu_rdata = scroll_y;
                5'd7:        cpu_rdata = {5'b0, font_slot};
                5'd8:        cpu_rdata = frame_counter;  // VGC_FRAME
                5'd9:        begin
                    cpu_rdata = 0;
                    for (int i = 0; i < 16; i++)
                        if (spr_enable[i]) cpu_rdata = cpu_rdata + 1;
                end
                5'd10:       cpu_rdata = {7'b0, cursor_enable};
                5'd11:       cpu_rdata = collision_ss;
                5'd12:       cpu_rdata = collision_bg;
                REG_BORDER:  cpu_rdata = {4'b0, border_color};
                REG_CHARIN:  cpu_rdata = char_in_reg;
                REG_CMD:     cpu_rdata = {7'b0, cmd_busy || artist_busy};
                5'd31:       cpu_rdata = irq_ctrl;
                default:     cpu_rdata = regs[reg_offset];
            endcase
        end
        else if (tile_reg_sel)   cpu_rdata = tile_rdata;
        else if (char_ram_sel)  cpu_rdata = cpu_rd_latch;
        else if (color_ram_sel) cpu_rdata = cpu_rd_latch;
    end

    // Debug read port
    wire dbg_char_sel  = (dbg_addr >= CHAR_RAM_BASE && dbg_addr <= CHAR_RAM_END);
    wire dbg_color_sel = (dbg_addr >= COLOR_RAM_BASE && dbg_addr <= COLOR_RAM_END);
    wire dbg_vgc_sel   = (dbg_addr >= VGC_BASE && dbg_addr <= VGC_REGS_END);
    wire dbg_tile_sel  = (dbg_addr >= 16'hA0C0 && dbg_addr <= 16'hA0DF);
    wire dbg_spr_sel   = (dbg_addr >= SPR_REG_BASE && dbg_addr <= SPR_REG_END);
    // dbg_owns driven inside always_comb below to match dbg_rdata pattern

    logic        dbg_rd_pending;
    logic [1:0]  dbg_rd_source;
    logic [7:0]  dbg_rd_latch;

    always_ff @(posedge clk) begin
        if (rst) begin
            dbg_rd_pending <= 0;
            dbg_rd_source <= 0;
        end else begin
            dbg_rd_pending <= 0;
            dbg_rd_source <= 0;
            if (dbg_char_sel) begin
                dbg_rd_pending <= 1;
                dbg_rd_source <= 2'd1;
            end else if (dbg_color_sel) begin
                dbg_rd_pending <= 1;
                dbg_rd_source <= 2'd2;
            end
            if (dbg_rd_pending) begin
                case (dbg_rd_source)
                    2'd1: dbg_rd_latch <= char_a_dout;
                    2'd2: dbg_rd_latch <= color_a_dout;
                    default: ;
                endcase
            end
        end
    end

    always_comb begin
        dbg_rdata = 8'h00;
        if (dbg_char_sel)       dbg_rdata = dbg_rd_latch;
        else if (dbg_color_sel) dbg_rdata = dbg_rd_latch;
        else if (dbg_vgc_sel) begin
            case (dbg_addr[4:0])
                REG_MODE:    dbg_rdata = {5'b0, mode};
                REG_BGCOL:   dbg_rdata = {4'b0, bg_color};
                REG_FGCOL:   dbg_rdata = {4'b0, fg_color};
                REG_CURSORX: dbg_rdata = {1'b0, cursor_x};
                REG_CURSORY: dbg_rdata = {2'b0, cursor_y};
                5'd7:        dbg_rdata = {5'b0, font_slot};
                5'd8:        dbg_rdata = frame_counter;  // VGC_FRAME
                REG_CHARIN:  dbg_rdata = char_in_reg;
                default:     dbg_rdata = regs[dbg_addr[4:0]];
            endcase
        end
    end

    // =========================================================================
    // CPU writes + command processor
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            cursor_x <= 0; cursor_y <= 0; mode <= 0;
            fg_color <= 4'd1; bg_color <= 4'd6; border_color <= 4'd6;
            gfx_color <= 4'd1; cursor_enable <= 1; font_slot <= 0;
            frame_counter <= 0;
            scroll_offset <= 0; scroll_pending <= 0; scroll_col <= 0;
            scroll_x <= 0; scroll_y <= 0;
            cmd_busy <= 0;
            key_fifo_rd <= 0;
            collision_ss <= 0; collision_bg <= 0;
            irq_ctrl <= 0; irq_raster_pending <= 0;
            copper_enabled <= 0; copper_count <= 0;
            copper_target_list <= 0; copper_pending_list <= 0;
            copper_loading <= 0;
            sprrow_count <= 0; sprcopy_phase <= 0; sprdef_wait <= 0;
            memread_pending <= 2'd0;
            vgc_tile_we <= 0; vgc_tile_re <= 0;
            artist_cmd_valid <= 0;
            for (int i = 0; i < 32; i++) regs[i] <= 0;
            for (int i = 0; i < 16; i++) begin
                spr_x[i] <= 0; spr_y[i] <= 0; spr_enable[i] <= 0;
                spr_flip_h[i] <= 0; spr_flip_v[i] <= 0;
                spr_pri[i] <= 0; spr_shape[i] <= 0; spr_trans[i] <= 0;
            end
        end else begin

            // Default: no command writes this cycle
            cmd_char_we <= 0;
            cmd_color_we <= 0;
            cmd_gfx_we <= 0;
            cmd_gfx_re <= 0;
            cmd_spr_we <= 0;
            cmd_spr_re <= 0;
            artist_cmd_valid <= 0;

            // Keyboard input — pop FIFO head exactly once per CPU read of
            // REG_CHARIN, on the RISING edge of the CPU's address-match.
            // cpu_addr stays at $A00F for ~2 cycles (cpu_ce alternates half
            // rate), so we must not pop every cycle. Rising-edge detection
            // via charin_sel_prev.
            charin_sel_prev <= (cpu_re && cpu_addr == (VGC_BASE + REG_CHARIN));
            key_fifo_rd <= (cpu_re && cpu_addr == (VGC_BASE + REG_CHARIN))
                        && !charin_sel_prev
                        && !key_fifo_empty;

            // Scroll state machine
            if (scroll_pending) begin
                cmd_char_addr <= screen_addr(scroll_col, ROWS - 1);
                cmd_char_din <= 8'h20;
                cmd_char_we <= 1;
                cmd_color_addr <= screen_addr(scroll_col, ROWS - 1);
                cmd_color_din <= {4'b0, fg_color};
                cmd_color_we <= 1;
                if (scroll_col == COLS - 1) begin
                    scroll_pending <= 0; scroll_col <= 0;
                end else
                    scroll_col <= scroll_col + 1;
            end

            // Clear VGC tile command strobes
            vgc_tile_we <= 0;
            vgc_tile_re <= 0;

            // MemRead latency: 2-cycle pipeline.
            //   memread_pending==2 after dispatch — re-assert the read
            //     strobe / keep the address alive so the dpram samples it
            //     during this cycle; data will be valid next cycle.
            //   memread_pending==1 — capture *_a_dout into regs[20].
            if (memread_pending == 2'd2) begin
                case (memread_space)
                    3'd2: cmd_gfx_re <= 1;       // keep gfx read active
                    3'd3: cmd_spr_re <= 1;       // keep sprite read active
                    3'd4: vgc_tile_re <= 1;      // keep tile read active
                    // spaces 0/1 (char/color) use the port A default-addr
                    // path which holds cmd_char_addr / cmd_color_addr.
                    default: ;
                endcase
                memread_pending <= 2'd1;
            end else if (memread_pending == 2'd1) begin
                case (memread_space)
                    3'd0: regs[20] <= char_a_dout;
                    3'd1: regs[20] <= color_a_dout;
                    3'd2: regs[20] <= {4'b0, gfx_a_dout};
                    3'd3: regs[20] <= spr_a_dout;
                    3'd4: regs[20] <= tile_blt_rdata;
                    default: ;
                endcase
                memread_pending <= 2'd0;
            end

            // Drawing command state machine (non-drawing ops remain here)
            if (cmd_busy) begin
                case (cmd_op)
                    CMD_SPRCLR: begin
                        cmd_spr_addr <= {cmd_x[4:1], cmd_cy[3:0], cmd_cx[2:0]};
                        cmd_spr_din <= 8'h00;
                        cmd_spr_we <= 1;
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
                        cmd_char_addr <= {7'b0, cmd_cy[5:0]} * COLS + {6'b0, cmd_cx[6:0]};
                        cmd_char_din <= 8'h20;
                        cmd_char_we <= 1;
                        cmd_color_addr <= {7'b0, cmd_cy[5:0]} * COLS + {6'b0, cmd_cx[6:0]};
                        cmd_color_din <= {4'b0, fg_color};
                        cmd_color_we <= 1;
                        if (cmd_cx == COLS - 1) begin
                            cmd_cx <= 0;
                            if (cmd_cy == ROWS - 1)
                                cmd_busy <= 0;
                            else
                                cmd_cy <= cmd_cy + 1;
                        end else
                            cmd_cx <= cmd_cx + 1;
                    end
                    8'h13: begin // SprCopy
                        // 3-phase machine: issue read → wait 1 cycle for
                        // dpram latency → capture spr_a_dout and write to
                        // destination. The original 2-phase code sampled
                        // spr_a_dout the same cycle the read address took
                        // effect, so every byte written was stale data.
                        case (sprcopy_phase)
                            2'd0: begin
                                cmd_spr_addr  <= {cmd_y[3:0], cmd_cy[3:0], cmd_cx[2:0]};
                                cmd_spr_re    <= 1;
                                sprcopy_phase <= 2'd1;
                            end
                            2'd1: begin
                                // dpram is reading this cycle — spr_a_dout
                                // will be valid at the next posedge.
                                sprcopy_phase <= 2'd2;
                            end
                            2'd2: begin
                                cmd_spr_addr  <= {cmd_x[3:0], cmd_cy[3:0], cmd_cx[2:0]};
                                cmd_spr_din   <= spr_a_dout;
                                cmd_spr_we    <= 1;
                                sprcopy_phase <= 2'd0;
                                if (cmd_cx == 7) begin
                                    cmd_cx <= 0;
                                    if (cmd_cy == 15)
                                        cmd_busy <= 0;
                                    else
                                        cmd_cy <= cmd_cy + 1;
                                end else
                                    cmd_cx <= cmd_cx + 1;
                            end
                            default: sprcopy_phase <= 2'd0;
                        endcase
                    end
                    CMD_SPRROW: begin
                        cmd_spr_addr <= {sprrow_spr, sprrow_row, sprrow_count};
                        cmd_spr_din <= sprrow_data[sprrow_count];
                        cmd_spr_we <= 1;
                        if (sprrow_count == 7)
                            cmd_busy <= 0;
                        else
                            sprrow_count <= sprrow_count + 1;
                    end
                    // SPRDEF is handled in a separate always_ff section
                    // below (the read-modify-write gated by sprdef_wait).
                    // If we don't list it here, the default arm would
                    // clear cmd_busy on the first cycle and the RMW would
                    // never fire.
                    CMD_SPRDEF: ;
                    default: cmd_busy <= 0;
                endcase
            end

            // Copper list loading state machine
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

            // Vblank-aligned frame counter tick. BASIC uses $A008 (VGC_FRAME)
            // as a vblank barrier — it reads the value, loops until it
            // changes, then proceeds. 8-bit wrap is fine for that pattern.
            if (h_count == 0 && v_count == V_ACTIVE)
                frame_counter <= frame_counter + 1'b1;

            // Copper vblank: swap pending→active list if changed
            if (h_count == 0 && v_count == V_ACTIVE) begin
                if (copper_pending_list != copper_active_list && !copper_loading) begin
                    copper_active_list <= copper_pending_list;
                    copper_loading <= 1;
                    copper_load_idx <= 0;
                    copper_load_src <= copper_pending_list;
                end
            end

            // Copper fire: apply register writes from sub-module
            if (copper_fire) begin
                case (copper_fire_reg)
                    8'd1: bg_color     <= copper_fire_val[3:0];
                    8'd2: fg_color     <= copper_fire_val[3:0];
                    8'd13: border_color <= copper_fire_val[3:0];
                    default: regs[copper_fire_reg[4:0]] <= copper_fire_val;
                endcase
            end

            // CPU writes — gated by cpu_ce so the VGC only captures
            // each bus transaction once (the CPU holds WE between
            // clock-enable cycles when stalled via RDY).
            if (cpu_we && cpu_ce) begin
                if (vgc_reg_sel) begin
                    case (reg_offset)
                        REG_MODE:    mode <= cpu_wdata[2:0];
                        REG_BGCOL:   bg_color <= cpu_wdata[3:0];
                        REG_FGCOL:   fg_color <= cpu_wdata[3:0];
                        REG_CURSORX: cursor_x <= cpu_wdata[6:0];
                        REG_CURSORY: cursor_y <= cpu_wdata[5:0];
                        5'd5:        scroll_x <= cpu_wdata;
                        5'd6:        scroll_y <= cpu_wdata;
                        5'd7:        font_slot <= cpu_wdata[2:0];
                        5'd8:        gfx_color <= cpu_wdata[3:0];
                        5'd10:       cursor_enable <= cpu_wdata[0];
                        5'd11:       collision_ss <= 0;
                        5'd12:       collision_bg <= 0;
                        REG_BORDER:  border_color <= cpu_wdata[3:0];
                        5'd31:       irq_ctrl <= cpu_wdata;
                        REG_CHAROUT: begin
                            case (cpu_wdata)
                                8'h08: begin
                                    if (cursor_x > 0) cursor_x <= cursor_x - 1;
                                    cmd_char_addr <= screen_addr(cursor_x > 0 ? cursor_x - 1 : 0, cursor_y);
                                    cmd_char_din <= 8'h20;
                                    cmd_char_we <= 1;
                                end
                                8'h0A: begin
                                    if (cursor_y >= ROWS - 1) begin
                                        scroll_offset <= (scroll_offset >= ROWS - 1) ? 6'd0 : scroll_offset + 1;
                                        scroll_pending <= 1; scroll_col <= 0;
                                    end else
                                        cursor_y <= cursor_y + 1;
                                end
                                8'h0C: begin
                                    cursor_x <= 0; cursor_y <= 0;
                                    scroll_offset <= 0;
                                    cmd_cx <= 0; cmd_cy <= 0;
                                    cmd_busy <= 1; cmd_op <= CMD_TXTCLS;
                                end
                                8'h0D: cursor_x <= 0;
                                8'h13: begin cursor_x <= 0; cursor_y <= 0; end
                                default: begin
                                    if (cpu_wdata >= 8'h20) begin
                                        cmd_char_addr <= screen_addr(cursor_x, cursor_y);
                                        cmd_char_din <= cpu_wdata;
                                        cmd_char_we <= 1;
                                        cmd_color_addr <= screen_addr(cursor_x, cursor_y);
                                        cmd_color_din <= {4'b0, fg_color};
                                        cmd_color_we <= 1;
                                        if (cursor_x >= COLS - 1) begin
                                            cursor_x <= 0;
                                            if (cursor_y >= ROWS - 1) begin
                                                scroll_offset <= (scroll_offset >= ROWS - 1) ? 6'd0 : scroll_offset + 1;
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
                            if (!cmd_busy && !artist_busy) begin
                                cmd_x  <= {regs[18][1:0], regs[17]};
                                cmd_y  <= {regs[20][1:0], regs[19]};
                                cmd_x2 <= {regs[22][1:0], regs[21]};
                                cmd_y2 <= {regs[24][1:0], regs[23]};

                                case (cpu_wdata)
                                    // Drawing commands → ARTIST
                                    CMD_PLOT, CMD_UNPLOT, CMD_LINE,
                                    CMD_CIRCLE, CMD_RECT, CMD_FILL,
                                    CMD_GCLS, CMD_PAINT, CMD_GTEXT: begin
                                        artist_cmd_valid <= 1;
                                        artist_cmd_code <= cpu_wdata;
                                    end

                                    CMD_GCOLOR: begin
                                        gfx_color <= regs[17][3:0];
                                    end
                                    CMD_SPRDEF: begin
                                        if (regs[17] < NUM_SPRITES && regs[18] < SPR_W && regs[19] < SPR_H) begin
                                            cmd_spr_addr <= {regs[17][3:0], regs[19][3:0], regs[18][3:1]};
                                            cmd_spr_re <= 1;
                                            cmd_busy <= 1; cmd_op <= CMD_SPRDEF;
                                            // Defer RMW one cycle so spr_a_dout
                                            // reflects the new read address.
                                            sprdef_wait <= 1;
                                        end
                                    end
                                    CMD_SPRROW: begin
                                        if (regs[17] < NUM_SPRITES && regs[18] < SPR_H) begin
                                            sprrow_spr <= regs[17][3:0];
                                            sprrow_row <= regs[18][3:0];
                                            sprrow_data[0] <= regs[19];
                                            sprrow_data[1] <= regs[20];
                                            sprrow_data[2] <= regs[21];
                                            sprrow_data[3] <= regs[22];
                                            sprrow_data[4] <= regs[23];
                                            sprrow_data[5] <= regs[24];
                                            sprrow_data[6] <= regs[25];
                                            sprrow_data[7] <= regs[26];
                                            sprrow_count <= 0;
                                            cmd_busy <= 1; cmd_op <= CMD_SPRROW;
                                        end
                                    end
                                    CMD_SPRCLR: begin
                                        if (regs[17] < NUM_SPRITES) begin
                                            cmd_cx <= 0; cmd_cy <= 0;
                                            cmd_x <= {5'b0, regs[17][3:0], 1'b0};
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
                                    8'h13: begin // CmdSprCopy
                                        if (regs[17] < NUM_SPRITES && regs[18] < NUM_SPRITES) begin
                                            cmd_x <= {6'b0, regs[17][3:0]};
                                            cmd_y <= {6'b0, regs[18][3:0]};
                                            cmd_cx <= 0; cmd_cy <= 0;
                                            sprcopy_phase <= 0;
                                            cmd_busy <= 1; cmd_op <= 8'h13;
                                        end
                                    end
                                    8'h1F: begin // SysReset
                                        cursor_x <= 0; cursor_y <= 0;
                                        mode <= 0; fg_color <= 4'd1; bg_color <= 4'd6;
                                        border_color <= 4'd6; gfx_color <= 4'd1;
                                        scroll_offset <= 0; scroll_x <= 0; scroll_y <= 0;
                                        cursor_enable <= 1; copper_enabled <= 0;
                                        copper_count <= 0;
                                        copper_target_list <= 0; copper_active_list <= 0;
                                        copper_pending_list <= 0; copper_loading <= 0;
                                        for (int i = 0; i < COPPER_LISTS; i++)
                                            copper_list_count[i] <= 0;
                                    end
                                    8'h1B: begin // CmdCopperAdd
                                        if (copper_list_count[copper_target_list] < COPPER_MAX) begin
                                            copper_list_pos[{copper_target_list, copper_list_count[copper_target_list][4:0]}]
                                                <= {1'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]};
                                            copper_list_reg[{copper_target_list, copper_list_count[copper_target_list][4:0]}]
                                                <= regs[20];
                                            copper_list_val[{copper_target_list, copper_list_count[copper_target_list][4:0]}]
                                                <= regs[22];
                                            copper_list_count[copper_target_list] <= copper_list_count[copper_target_list] + 1;
                                            if (copper_target_list == copper_active_list) begin
                                                copper_pos[copper_count] <= {1'b0, regs[19]} * GFX_W + {8'b0, regs[18][0], regs[17]};
                                                copper_reg[copper_count] <= regs[20];
                                                copper_val[copper_count] <= regs[22];
                                                copper_count <= copper_count + 1;
                                            end
                                        end
                                    end
                                    8'h1C: begin // CmdCopperClear
                                        copper_list_count[copper_target_list] <= 0;
                                        if (copper_target_list == copper_active_list) begin
                                            copper_count <= 0;
                                        end
                                    end
                                    8'h1D: begin // CmdCopperEnable
                                        copper_enabled <= 1;
                                    end
                                    8'h1E: begin // CmdCopperDisable
                                        copper_enabled <= 0;
                                    end
                                    CMD_COPPERLIST: begin
                                        copper_target_list <= regs[17][6:0];
                                    end
                                    CMD_COPPERUSE: begin
                                        copper_pending_list <= regs[17][6:0];
                                    end
                                    CMD_COPPERLISTEND: begin
                                        copper_target_list <= copper_active_list;
                                    end
                                    8'h19: begin // MemRead
                                        case (regs[17])
                                            8'd0: begin
                                                cmd_char_addr <= {regs[19], regs[18]};
                                                memread_pending <= 2'd2;
                                                memread_space <= 3'd0;
                                            end
                                            8'd1: begin
                                                cmd_color_addr <= {regs[19], regs[18]};
                                                memread_pending <= 2'd2;
                                                memread_space <= 3'd1;
                                            end
                                            8'd2: begin
                                                cmd_gfx_addr <= {regs[19], regs[18]};
                                                cmd_gfx_re <= 1;
                                                memread_pending <= 2'd2;
                                                memread_space <= 3'd2;
                                            end
                                            8'd3: begin
                                                cmd_spr_addr <= {regs[19], regs[18]};
                                                cmd_spr_re <= 1;
                                                memread_pending <= 2'd2;
                                                memread_space <= 3'd3;
                                            end
                                            8'd4: begin
                                                vgc_tile_addr <= {regs[19][6:0], regs[18]};
                                                vgc_tile_re <= 1;
                                                memread_pending <= 2'd2;
                                                memread_space <= 3'd4;
                                            end
                                            default: ;
                                        endcase
                                    end
                                    8'h1A: begin // MemWrite
                                        case (regs[17])
                                            8'd0: begin
                                                cmd_char_addr <= {regs[19], regs[18]};
                                                cmd_char_din <= regs[20];
                                                cmd_char_we <= 1;
                                            end
                                            8'd1: begin
                                                cmd_color_addr <= {regs[19], regs[18]};
                                                cmd_color_din <= regs[20];
                                                cmd_color_we <= 1;
                                            end
                                            8'd2: begin
                                                cmd_gfx_addr <= {regs[19], regs[18]};
                                                cmd_gfx_din <= regs[20][3:0];
                                                cmd_gfx_we <= 1;
                                            end
                                            8'd3: begin
                                                cmd_spr_addr <= {regs[19], regs[18]};
                                                cmd_spr_din <= regs[20];
                                                cmd_spr_we <= 1;
                                            end
                                            8'd4: begin
                                                vgc_tile_addr <= {regs[19][6:0], regs[18]};
                                                vgc_tile_wdata <= regs[20];
                                                vgc_tile_we <= 1;
                                            end
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

                if (char_ram_sel) begin
                    cmd_char_addr <= char_offset;
                    cmd_char_din <= cpu_wdata;
                    cmd_char_we <= 1;
                end
                if (color_ram_sel) begin
                    cmd_color_addr <= color_offset;
                    cmd_color_din <= cpu_wdata;
                    cmd_color_we <= 1;
                end

                // Shadow FIO name buffer for Gtext
                if (fio_len_sel)   fio_name_len <= cpu_wdata[5:0];
                if (fio_name_sel)  fio_name[cpu_addr - FIO_NAME] <= cpu_wdata;

                // Sprite register writes
                if (spr_reg_sel) begin
                    case (spr_field)
                        3'd0: spr_x[spr_index][7:0] <= cpu_wdata;
                        3'd1: spr_x[spr_index][8]   <= cpu_wdata[0];
                        3'd2: spr_y[spr_index]       <= cpu_wdata;
                        3'd3: ;
                        3'd4: spr_shape[spr_index]   <= cpu_wdata[3:0];
                        3'd5: begin
                            spr_flip_h[spr_index] <= cpu_wdata[0];
                            spr_flip_v[spr_index] <= cpu_wdata[1];
                            spr_enable[spr_index] <= cpu_wdata[7];
                        end
                        3'd6: spr_pri[spr_index]     <= cpu_wdata[1:0];
                        3'd7: spr_trans[spr_index]    <= cpu_wdata[3:0];
                    endcase
                end
            end

            // SPRDEF: complete the read-modify-write, delayed one cycle so
            // spr_a_dout reflects the freshly-issued read address. We must
            // re-assert cmd_spr_re during the wait — otherwise the port A
            // mux falls through to its default (addr=0) and dpram reads
            // mem[0] instead of the target byte.
            if (sprdef_wait) begin
                cmd_spr_addr <= {regs[17][3:0], regs[19][3:0], regs[18][3:1]};
                cmd_spr_re   <= 1;
                sprdef_wait  <= 0;
            end else if (cmd_busy && cmd_op == CMD_SPRDEF && !memread_pending) begin
                cmd_spr_addr <= {regs[17][3:0], regs[19][3:0], regs[18][3:1]};
                if (regs[18][0])
                    cmd_spr_din <= {spr_a_dout[7:4], regs[20][3:0]};
                else
                    cmd_spr_din <= {regs[20][3:0], spr_a_dout[3:0]};
                cmd_spr_we <= 1;
                cmd_busy <= 0;
            end

            // Collision detection — accumulate during active display
            if (in_text_area_d2 && gfx_x_d2 < GFX_W) begin
                if (spr_pixel_hit && gfx_b_dout != 0)
                    collision_bg <= collision_bg | 8'hFF;
            end
        end
    end

    // =========================================================================
    // Port A write mux — blitter takes priority over command processor
    // =========================================================================
    // Blitter read latching
    logic        blt_rd_pending;
    logic [2:0]  blt_rd_space;
    logic [7:0]  blt_rd_latch;

    always_ff @(posedge clk) begin
        if (rst) begin
            blt_rd_pending <= 0;
            blt_rd_space <= 0;
        end else begin
            blt_rd_pending <= 0;
            if (blt_re) begin
                blt_rd_pending <= 1;
                blt_rd_space <= blt_space;
            end
            if (blt_rd_pending) begin
                case (blt_rd_space)
                    3'd1: blt_rd_latch <= char_a_dout;
                    3'd2: blt_rd_latch <= color_a_dout;
                    3'd3: blt_rd_latch <= {4'b0, gfx_a_dout};
                    3'd4: blt_rd_latch <= spr_a_dout;
                    3'd6: blt_rd_latch <= tile_blt_rdata;
                    default: ;
                endcase
            end
        end
    end

    always_comb begin
        blt_rdata = blt_rd_latch;
    end

    // Port A address/data/we mux for each memory
    always_comb begin
        // char_ram port A
        char_a_we = 0;
        char_a_addr = 13'd0;
        char_a_din = 8'd0;
        if (blt_we && blt_space == 3'd1) begin
            char_a_addr = blt_addr[12:0];
            char_a_din = blt_wdata;
            char_a_we = 1;
        end else if (cmd_char_we) begin
            char_a_addr = cmd_char_addr;
            char_a_din = cmd_char_din;
            char_a_we = 1;
        end else if (blt_re && blt_space == 3'd1) begin
            char_a_addr = blt_addr[12:0];
        end else if (cpu_re && cpu_ce && char_ram_sel) begin
            char_a_addr = char_offset;
        end else if (dbg_char_sel) begin
            char_a_addr = {2'b0, dbg_addr - CHAR_RAM_BASE};
        end else begin
            char_a_addr = cmd_char_addr;
        end

        // color_ram port A
        color_a_we = 0;
        color_a_addr = 13'd0;
        color_a_din = 8'd0;
        if (blt_we && blt_space == 3'd2) begin
            color_a_addr = blt_addr[12:0];
            color_a_din = blt_wdata;
            color_a_we = 1;
        end else if (cmd_color_we) begin
            color_a_addr = cmd_color_addr;
            color_a_din = cmd_color_din;
            color_a_we = 1;
        end else if (blt_re && blt_space == 3'd2) begin
            color_a_addr = blt_addr[12:0];
        end else if (cpu_re && cpu_ce && color_ram_sel) begin
            color_a_addr = color_offset;
        end else if (dbg_color_sel) begin
            color_a_addr = {2'b0, dbg_addr - COLOR_RAM_BASE};
        end else begin
            color_a_addr = cmd_color_addr;
        end

        // font_rom port A — ARTIST font reads take priority
        font_a_we = 0;
        font_a_addr = 11'd0;
        font_a_din = 8'd0;
        if (artist_font_re) begin
            font_a_addr = artist_font_addr;
        end

        // gfx_ram port A — priority: blitter > ARTIST > cmd > blitter-read > cmd-read
        gfx_a_we = 0;
        gfx_a_addr = 17'd0;
        gfx_a_din = 4'd0;
        if (blt_we && blt_space == 3'd3) begin
            gfx_a_addr = blt_addr;
            gfx_a_din = blt_wdata[3:0];
            gfx_a_we = 1;
        end else if (artist_gfx_we) begin
            gfx_a_addr = artist_gfx_addr;
            gfx_a_din = artist_gfx_wdata;
            gfx_a_we = 1;
        end else if (cmd_gfx_we) begin
            gfx_a_addr = cmd_gfx_addr;
            gfx_a_din = cmd_gfx_din;
            gfx_a_we = 1;
        end else if (blt_re && blt_space == 3'd3) begin
            gfx_a_addr = blt_addr;
        end else if (artist_gfx_re) begin
            gfx_a_addr = artist_gfx_raddr;
        end else if (cmd_gfx_re) begin
            gfx_a_addr = cmd_gfx_addr;
        end

        // sprite_shapes port A
        spr_a_we = 0;
        spr_a_addr = 11'd0;
        spr_a_din = 8'd0;
        if (blt_we && blt_space == 3'd4) begin
            spr_a_addr = blt_addr[10:0];
            spr_a_din = blt_wdata;
            spr_a_we = 1;
        end else if (cmd_spr_we) begin
            spr_a_addr = cmd_spr_addr;
            spr_a_din = cmd_spr_din;
            spr_a_we = 1;
        end else if (blt_re && blt_space == 3'd4) begin
            spr_a_addr = blt_addr[10:0];
        end else if (cmd_spr_re) begin
            spr_a_addr = cmd_spr_addr;
        end
    end

    // =========================================================================
    // Tile engine instance
    // =========================================================================
    wire [11:0] tile_rgb;
    wire [1:0]  tile_opaque;
    wire [7:0]  tile_blt_rdata;

    logic [14:0] vgc_tile_addr;
    logic [7:0]  vgc_tile_wdata;
    logic        vgc_tile_we;
    logic        vgc_tile_re;

    wire [14:0] tile_blt_addr  = (vgc_tile_we || vgc_tile_re) ? vgc_tile_addr : blt_addr[14:0];
    wire [7:0]  tile_blt_wdata = vgc_tile_we ? vgc_tile_wdata : blt_wdata;
    wire        tile_blt_we    = vgc_tile_we || (blt_we && blt_space == 3'd6);
    wire        tile_blt_re    = vgc_tile_re || (blt_re && blt_space == 3'd6);

    tile_engine tile_inst (
        .clk        (clk),
        .rst        (rst),
        .cpu_addr   (cpu_addr),
        .cpu_wdata  (cpu_wdata),
        .cpu_we     (cpu_we & cpu_ce),
        .cpu_rdata  (tile_rdata),
        .cpu_re     (cpu_re),
        .dma_addr   (tile_dma_addr),
        .dma_data   (tile_dma_data),
        .dma_active (tile_dma_active),
        .blt_tile_addr (tile_blt_addr),
        .blt_tile_wdata(tile_blt_wdata),
        .blt_tile_we   (tile_blt_we),
        .blt_tile_re   (tile_blt_re),
        .blt_tile_rdata(tile_blt_rdata),
        .pixel_x    (pre_gfx_x),
        .pixel_y    (pre_gfx_y),
        .pixel_valid(in_text_area),
        .tile_rgb   (tile_rgb),
        .tile_opaque(tile_opaque)
    );

    // =========================================================================
    // Pixel compositing
    // =========================================================================
    logic [3:0]  cur_fg_d2;
    logic [3:0]  cur_gfx_d2;
    logic        pixel_on_d2;
    logic [11:0] text_pixel_d2, gfx_pixel_d2, pixel_color;

    // Tile engine output delayed to match pipeline
    logic [11:0] tile_rgb_d1, tile_rgb_d2;
    logic [1:0]  tile_opaque_d1, tile_opaque_d2;
    always_ff @(posedge clk) begin
        tile_rgb_d1 <= tile_rgb;       tile_rgb_d2 <= tile_rgb_d1;
        tile_opaque_d1 <= tile_opaque; tile_opaque_d2 <= tile_opaque_d1;
    end

    always_comb begin
        cur_fg_d2     = color_b_dout[3:0];
        pixel_on_d2   = font_b_dout[3'd7 - font_pixel_d2];
        text_pixel_d2 = pixel_on_d2 ? palette[cur_fg_d2] : palette[bg_color];

        cur_gfx_d2    = gfx_b_dout;
        gfx_pixel_d2  = palette[cur_gfx_d2];

        // Layer compositing with sprite priorities
        if (!visible_d2)
            pixel_color = 12'h000;
        else if (!in_text_area_d2)
            pixel_color = palette[border_color];
        else begin
            case (mode)
                3'd0: pixel_color = text_pixel_d2;
                3'd1: pixel_color = (cur_gfx_d2 != 0) ? gfx_pixel_d2 : text_pixel_d2;
                3'd2: pixel_color = pixel_on_d2 ? palette[cur_fg_d2] :
                                    (cur_gfx_d2 != 0) ? gfx_pixel_d2 : palette[bg_color];
                3'd3: pixel_color = (cur_gfx_d2 != 0) ? gfx_pixel_d2 : palette[bg_color];
                3'd4: pixel_color = (tile_opaque_d2 != 0) ? tile_rgb_d2 : palette[bg_color];
                default: pixel_color = text_pixel_d2;
            endcase

            if (spr_pixel_hit) begin
                if (mode == 3'd4) begin
                    if (spr_pixel_pri == 2'd0) begin
                        if (tile_opaque_d2 == 0) pixel_color = palette[spr_pixel];
                    end else if (spr_pixel_pri == 2'd1) begin
                        if (tile_opaque_d2 < 2'd2) pixel_color = palette[spr_pixel];
                    end else begin
                        pixel_color = palette[spr_pixel];
                    end
                end else begin
                    if (spr_pixel_pri == 2'd0) begin
                        if (!pixel_on_d2 && cur_gfx_d2 == 0)
                            pixel_color = palette[spr_pixel];
                    end else begin
                        pixel_color = palette[spr_pixel];
                    end
                end
            end
        end
    end

    // Cursor
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
    wire cursor_here = cursor_enable && in_text_area_d2 && (text_col_d2 == cursor_x) && (text_row_d2 == cursor_y);

    // Raster IRQ
    assign irq_out = irq_ctrl[0] && (v_count == {3'b0, irq_ctrl[7:1]} * 2 + V_BORDER);

    // Output
    always_ff @(posedge clk) begin
        vid_hsync <= ~h_sync_area_d2;
        vid_vsync <= ~v_sync_area_d2;
        vid_de    <= visible_d2;

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

endmodule
