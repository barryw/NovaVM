// VGC — Virtual Graphics Controller (NOVA — top level)
// 80x50 text + 320x200 graphics + tile engine + command processor
// Generates VGA/480p-like RGB + sync signals
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
    // Width is 17 bits so bus masters can share one address shape. Current
    // spaces use fewer bits (char/color 12, gfx 16, sprite 11) and ignore the top.
    input  logic [2:0]  blt_space,    // 1=char, 2=color, 3=gfx, 4=sprite, 6=tile
    input  logic [16:0] blt_addr,
    output logic [7:0]  blt_rdata,
    input  logic [7:0]  blt_wdata,
    input  logic        blt_we,
    input  logic        blt_re,

    // Debug read port — separate from CPU bus to avoid side effects
    input  logic [15:0] dbg_addr,
    output logic [7:0]  dbg_rdata,

    // Debug write ports. dbg_we writes CPU-visible VGC registers without
    // needing the 6502 to run; dbg_vmem_we streams directly into VGC memory.
    input  logic        dbg_we,
    input  logic [15:0] dbg_waddr,
    input  logic [7:0]  dbg_wdata,
    input  logic        dbg_vmem_we,
    input  logic [2:0]  dbg_vmem_space,
    input  logic [16:0] dbg_vmem_addr,
    input  logic [7:0]  dbg_vmem_wdata,

    // Video output
    output logic [3:0]  vid_r,
    output logic [3:0]  vid_g,
    output logic [3:0]  vid_b,
    output logic        vid_hsync,
    output logic        vid_vsync,
    output logic        vid_de,
    output logic        irq_out,    // raster IRQ (active high)
    output logic        rdy_out     // low while VGC is holding CPU for vblank-safe text scroll
`ifndef SYNTHESIS
    ,
    // Simulation-only VRAM peek path. This is intentionally not a
    // CPU-visible memory window and is excluded from synthesis.
    input  logic        dbg_vram_read_en,
    input  logic [2:0]  dbg_vram_space,
    input  logic [16:0] dbg_vram_addr,
    output logic [7:0]  dbg_vram_rdata
`endif
);

    // =========================================================================
    // Parameters
    // =========================================================================
    // Match vgc_timing.sv. At the ULX3S 25.000 MHz pixel clock, 794*525 lands
    // near 60 Hz and avoids the visible cadence judder of 800*525 at 59.52 Hz.
`ifdef VIDEO_720X480
    // Full-system CEA-style 720x480p timing at 27 MHz. The internal Nova
    // canvas is 640x400 centered with 40px borders on all sides.
    localparam H_ACTIVE = 720, H_FRONT = 16, H_SYNC = 62, H_BACK = 60, H_TOTAL = 858;
    localparam V_ACTIVE = 480, V_FRONT = 9,  V_SYNC = 6,  V_BACK = 30, V_TOTAL = 525;
`elsif VGC_TIMING_5994
    // Diagnostic timing profile: 25e6/(796*524)=59.937Hz, much closer to
    // VGA's 59.940Hz vertical cadence while keeping hsync near 31.47kHz.
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 44, H_TOTAL = 796;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 32, V_TOTAL = 524;
`else
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 42, H_TOTAL = 794;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 33, V_TOTAL = 525;
`endif

    localparam COLS     = 80,  ROWS    = 50;
    localparam CHAR_W   = 8,   CHAR_H  = 8;
    localparam TEXT_H   = ROWS * CHAR_H;               // 400 pixels (1:1, no doubling)
    localparam V_BORDER = (V_ACTIVE - TEXT_H) / 2;     // 40 lines top/bottom
    localparam TEXT_SIZE = COLS * ROWS;                 // 4000 cells
    localparam logic [5:0] LAST_TEXT_ROW = ROWS - 1;

    localparam GFX_W    = 320, GFX_H   = 200;
    localparam GFX_SIZE = GFX_W * GFX_H;               // 64000 pixels
    localparam SPR_SIZE  = 2048;
    localparam TILE_SIZE = 32768;

    // Address map
    localparam VGC_BASE       = 16'hA000;
    localparam VGC_REGS_END   = 16'hA01F;
    localparam VGC_IRQ_BASE   = 16'hA0F0;
    localparam VGC_IRQ_END    = 16'hA0FF;
    localparam SPR_REG_BASE   = 16'hA040;
    localparam SPR_REG_END    = 16'hA0BF;
    localparam VRAM_REG_BASE  = 16'hA0E0;
    localparam VRAM_REG_END   = 16'hA0E4;
    localparam DIM_REG_ADDR   = 16'hA0E5;

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

    // Flexible VGC IRQ block at $A0F0-$A0FF. Copper can raise any enabled
    // source by writing COPPER_REG_IRQ with the desired source mask.
    localparam IRQ_ENABLE  = 4'h0;       // R/W: enabled source mask
    localparam IRQ_STATUS  = 4'h1;       // R/W1C: pending source mask
    localparam IRQ_FORCE   = 4'h2;       // W: set enabled pending bits
    localparam IRQ_VALID   = 8'h1F;
    localparam IRQ_VBLANK  = 8'h01;
    localparam IRQ_COPPER0 = 8'h02;
    localparam COPPER_REG_IRQ = 8'hFE;

    // VDC-style VRAM port registers at $A0E0-$A0E4
    localparam VR_PLANE = 0;
    localparam VR_ADDRL = 1;
    localparam VR_ADDRH = 2;
    localparam VR_DATA  = 3;
    localparam VR_CTRL  = 4;

    // VRAM plane IDs match DMA/blitter VGC spaces.
    localparam SPACE_CHAR   = 3'd1;
    localparam SPACE_COLOR  = 3'd2;
    localparam SPACE_GFX    = 3'd3;
    localparam SPACE_SPRITE = 3'd4;
    localparam SPACE_TILE   = 3'd6;

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
    logic [11:0] char_a_addr;
    logic [7:0]  char_a_din;
    logic        char_a_we;
    logic [7:0]  char_a_dout;
    logic [11:0] color_a_addr;
    logic [7:0]  color_a_din;
    logic        color_a_we;
    logic [7:0]  color_a_dout;
    logic [12:0] font_a_addr;
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
        .font_slot(font_slot[1:0]),
        .real_row(real_row), .text_col(text_col), .font_line(font_line), .font_line_d1(font_line_d1),
        .char_b_dout(char_b_dout), .color_b_dout(color_b_dout),
        .font_b_dout(font_b_dout)
    );

    // =========================================================================
    // Graphics sub-module (CANVAS) — dpram port A signals
    // =========================================================================
    // 17-bit external bus address, internally clipped to 64000 pixels (320×200)
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
    logic        spr_a_re;
    logic [7:0]  spr_a_dout;

    // Sprite attributes — small register arrays (no dpram needed).
    // Sprite X is an unsigned word. Sprite Y is an unsigned byte over the
    // 240-line sprite plane; the register high byte is reserved/reads as 0.
    logic [15:0] spr_x [0:15];
    logic [7:0]  spr_y [0:15];
    logic        spr_enable [0:15];
    logic        spr_flip_h [0:15];
    logic        spr_flip_v [0:15];
    logic [1:0]  spr_pri [0:15];
    logic [3:0]  spr_shape [0:15];
    logic [3:0]  spr_trans [0:15];

    // CPU/command writes land in the next-frame sprite state. The renderer
    // consumes spr_* only, and vblank atomically copies spr_next_* into spr_*.
    // This prevents BASIC movement/shape/flag writes from tearing mid-frame.
    logic [15:0] spr_next_x [0:15];
    logic [7:0]  spr_next_y [0:15];
    logic        spr_next_enable [0:15];
    logic        spr_next_flip_h [0:15];
    logic        spr_next_flip_v [0:15];
    logic [1:0]  spr_next_pri [0:15];
    logic [3:0]  spr_next_shape [0:15];
    logic [3:0]  spr_next_trans [0:15];

    initial begin
        for (int i = 0; i < 16; i++) begin
            spr_x[i] = 0; spr_y[i] = 0;
            spr_enable[i] = 0; spr_flip_h[i] = 0; spr_flip_v[i] = 0;
            spr_pri[i] = 2'd1; spr_shape[i] = i[3:0];
            spr_trans[i] = 4'd0;
            spr_next_x[i] = 0; spr_next_y[i] = 0;
            spr_next_enable[i] = 0; spr_next_flip_h[i] = 0; spr_next_flip_v[i] = 0;
            spr_next_pri[i] = 2'd1; spr_next_shape[i] = i[3:0];
            spr_next_trans[i] = 4'd0;
        end
    end

    logic [3:0]  spr_pixel;
    logic [1:0]  spr_pixel_pri;
    logic        spr_pixel_hit;
    logic [7:0]  spr_collision_bg_bits;

    // Pack sprite attribute arrays into flat vectors for sub-module
    logic [16*16-1:0] spr_x_flat;
    logic [16*16-1:0] spr_y_flat;
    logic [15:0]      spr_enable_flat;
    logic [15:0]      spr_flip_h_flat;
    logic [15:0]      spr_flip_v_flat;
    logic [16*2-1:0]  spr_pri_flat;
    logic [16*4-1:0]  spr_shape_flat;
    logic [16*4-1:0]  spr_trans_flat;

    always_comb begin
        for (int i = 0; i < 16; i++) begin
            spr_x_flat[i*16 +: 16]   = spr_x[i];
            spr_y_flat[i*16 +: 16]   = {8'd0, spr_y[i]};
            spr_enable_flat[i]        = spr_enable[i];
            spr_flip_h_flat[i]        = spr_flip_h[i];
            spr_flip_v_flat[i]        = spr_flip_v[i];
            spr_pri_flat[i*2 +: 2]   = spr_pri[i];
            spr_shape_flat[i*4 +: 4] = spr_shape[i];
            spr_trans_flat[i*4 +: 4] = spr_trans[i];
        end
    end

    // PIXIE's scanline buffer is synchronous: an address presented this pixel
    // returns data on the next pixel. Address one physical pixel ahead so the
    // sprite pixel and delayed border/canvas flags reach the compositor in
    // the same cycle. Without this, sprites draw one physical pixel late and
    // leave a 1px blue gap at the inner border edge.
    wire [9:0] sprite_h_read_d2 = h_count_d2 + 10'd1;
    wire [8:0] sprite_x_read_d2 = sprite_h_read_d2[9:1];

    vgc_sprites sprite_inst (
        .clk(clk), .rst(rst),
        .spr_a_addr(spr_a_addr), .spr_a_din(spr_a_din),
        .spr_a_we(spr_a_we), .spr_a_re(spr_a_re), .spr_a_dout(spr_a_dout),
        .h_count(h_count), .v_count(v_count),
        .sprite_frame_commit(sprite_frame_publish),
        .shape_publish_block(sprite_shape_publish_block),
        .shape_sync_busy(sprite_shape_sync_busy),
        .visible_d2(visible_d2), .sprite_x_d2(sprite_x_read_d2),
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
    logic [3:0]  border_color, fg_color, bg_color, gfx_color, display_dim;
    logic [2:0]  mode;
    logic        cursor_enable;
    logic [2:0]  vram_plane;
    logic [15:0] vram_addr;
    logic [7:0]  vram_ctrl;
    logic        vram_cpu_read_pending;
    logic [2:0]  vram_cpu_read_space;
    logic [7:0]  vram_cpu_read_latch;
    // Frame counter at $A008 (VGC_FRAME in basic.asm:8806). Increments once
    // per vblank entry — EhBASIC's SPRITESET/SPRPRI use `LDA VGC_FRAME;
    // CMP VGC_FRAME; BEQ loop` to wait for vblank sync. Without this,
    // any BASIC command that syncs on VGC_FRAME hangs the CPU forever.
    logic [7:0]  frame_counter;
    logic        cmd_busy;
    logic [7:0]  cmd_op;
    logic        sprdef_wait;
    logic        sprite_shape_sync_busy;
    wire         vblank_start = (h_count == 10'd0 && v_count == V_ACTIVE);
    // PIXIE prepares native row 0 while native row 479 is still being displayed.
    // Commit sprite attributes here so the whole next frame uses one coherent
    // snapshot; committing at V_ACTIVE would leave row 0 one frame stale.
    wire         sprite_frame_commit = (h_count == 10'd0 && v_count == V_ACTIVE - 1);
    wire         sprite_shape_publish_block =
        (cmd_busy && (cmd_op == CMD_SPRDEF || cmd_op == CMD_SPRROW ||
                      cmd_op == CMD_SPRCLR || cmd_op == CMD_SPRCOPY)) ||
        sprdef_wait;
    wire         sprite_frame_publish = sprite_frame_commit &&
                                        !sprite_shape_publish_block &&
                                        !sprite_shape_sync_busy;
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
    logic [7:0]  irq_enable;
    logic [7:0]  irq_pending;
    logic        scroll_pending;
    logic        scroll_clearing;
    logic [6:0]  scroll_col;

    assign rdy_out = !scroll_pending;
    wire [7:0] irq_event_mask =
        ((vblank_start && irq_enable[0]) ? IRQ_VBLANK : 8'h00) |
        ((copper_fire && copper_fire_reg == COPPER_REG_IRQ)
            ? (copper_fire_val & IRQ_VALID & irq_enable)
            : 8'h00);

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
    // sprdef_wait is declared with the frame-publish gating wires above.

    // MemRead latency state — 2-bit counter: 2 = just dispatched, 1 = dpram
    // now valid to capture, 0 = idle. The original 1-bit flag captured
    // *_a_dout the same cycle the read address took effect (stale read).
    logic [1:0]  memread_pending;
    logic [2:0]  memread_space;

    // Port A write signals from command processor
    logic [11:0] cmd_char_addr;
    logic [7:0]  cmd_char_din;
    logic        cmd_char_we;
    logic [11:0] cmd_color_addr;
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

    // Auto-clear-on-reset trigger. Armed at POR and on every rst assertion;
    // fires once after rst is released to dispatch TXTCLS + GCLS, clearing
    // both framebuffers. Also re-armed by the BASIC RESET command (SysReset)
    // so a soft reset gets the same display clear treatment as a hard reset.
    // Without this, cold_start (FPGA rst) clears all FF state but leaves
    // char_mem and gfx_mem with whatever the previous test program drew.
    logic        boot_clear_armed = 1'b1;

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
        border_color = 4'd11; fg_color = 4'd15; bg_color = 4'd0;
        display_dim = 4'd15;
        gfx_color = 4'd1; mode = 0; cursor_enable = 1;
        vram_plane = SPACE_CHAR; vram_addr = 0; vram_ctrl = 8'h01;
        vram_cpu_read_pending = 0; vram_cpu_read_space = SPACE_CHAR; vram_cpu_read_latch = 0;
        frame_counter = 0;
        scroll_x = 0; scroll_y = 0;
        scroll_offset = 0; scroll_pending = 0; scroll_clearing = 0; scroll_col = 0;
        key_fifo_rd = 0; cmd_busy = 0;
        font_slot = 0; collision_ss = 0; collision_bg = 0;
        irq_enable = 0; irq_pending = 0;
        sprrow_count = 0; sprcopy_phase = 0;
        memread_pending = 2'd0;
        vgc_tile_addr = 0; vgc_tile_wdata = 0; vgc_tile_we = 0; vgc_tile_re = 0;
        artist_cmd_valid = 0;
    end

    // Keyboard input is handled inside the main always_ff block below
    // to avoid a multi-driver conflict on char_in_reg (which is also
    // cleared in the reset branch of that block).

    // =========================================================================
    // Address decode — READ side uses live cpu_addr (combinational into
    // cpu_rdata mux). WRITE side uses r_cpu_addr_w / r_cpu_we_w / r_cpu_wdata_w
    // registered one pixel cycle to break the BRAM→CPU→cmd_*_addr critical
    // path. Net: 41ns single-cycle path becomes two short paths in two pixel
    // cycles. Writes still fire on the same cpu_ce=1 edge as before because
    // CPU drives cpu_we/cpu_addr stably for 2 pixel cycles per CPU T-cycle —
    // r_*_w lags by 1 cycle but the sampled value at the firing edge matches.
    // See feedback_register_slice_at_peripheral_writes.md.
    // =========================================================================
    // POR-determinism via DECLARATION init (not initial block + rst clause).
    // Yosys+nextpnr quirk: when an FF has a non-trivial LSR (e.g., LSR=rst),
    // the bitstream INITVAL is dropped — the FF then comes up at silicon
    // default and depends on rst signal arrival. With LSR tied to 1'h0
    // (no rst clause), INITVAL is honored and the FF starts deterministically
    // from the declaration `= 0` value at FPGA configuration time.
    // Diagnosed 2026-04-27 after rst-clause version caused total HW failure.
    logic [15:0] r_cpu_addr_w  = 16'h0;
    logic [7:0]  r_cpu_wdata_w = 8'h0;
    // `write_active` is the firing flag for the write-side block. It pulses
    // for one pixel cycle following any `cpu_we && cpu_ce` event. The block
    // fires on write_active=1; r_cpu_addr_w / r_cpu_wdata_w are aligned to
    // the same cycle because they were captured at the previous posedge from
    // the same cpu_addr / cpu_wdata that produced the cpu_we&&cpu_ce event.
    logic        write_active = 1'b0;
    always_ff @(posedge clk) begin
        r_cpu_addr_w  <= cpu_addr;
        r_cpu_wdata_w <= cpu_wdata;
        write_active  <= cpu_we && cpu_ce;
    end

    // READ-side decoders — combinational on live cpu_addr.
    wire vgc_reg_sel   = (cpu_addr >= VGC_BASE && cpu_addr <= VGC_REGS_END);
    wire vgc_irq_sel   = (cpu_addr >= VGC_IRQ_BASE && cpu_addr <= VGC_IRQ_END);
    wire spr_reg_sel   = (cpu_addr >= SPR_REG_BASE && cpu_addr <= SPR_REG_END);
    wire vram_reg_sel  = (cpu_addr >= VRAM_REG_BASE && cpu_addr <= VRAM_REG_END);
    wire dim_reg_sel   = (cpu_addr == DIM_REG_ADDR);
    wire fio_name_sel  = (cpu_addr >= FIO_NAME && cpu_addr <= 16'hB9EF);
    wire fio_len_sel   = (cpu_addr == FIO_NAME_LEN);
    wire tile_reg_sel  = (cpu_addr >= 16'hA0C0 && cpu_addr <= 16'hA0DF);
    wire [7:0]  tile_rdata;
    wire [4:0]  reg_offset   = cpu_addr[4:0];
    wire [3:0]  irq_offset   = cpu_addr[3:0];
    wire [2:0]  vram_reg_off = cpu_addr[2:0];
    // Sprite register map starts at $A040 and spans $A040-$A0BF.
    // Offsets from the base are 0..127 = sprite (4 bits) << 3 | field (3 bits).
    // Earlier versions used `cpu_addr[6:3]` directly, which worked only as long
    // as the read and write paths were SYMMETRIC (both wrong in the same way —
    // sprites 0-7 decoded as 8-15 and vice versa). That breaks as soon as any
    // command path (e.g. CMD_SPRPOS via regs[17]=P0) indexes by the REAL
    // sprite number — SPRITESET-set state and SPRITE-command-set state land
    // in different sprites, and sprite register readback reports the wrong
    // sprite's state.
    wire [6:0]  spr_offset   = cpu_addr[6:0] - 7'h40;
    wire [3:0]  spr_index    = spr_offset[6:3];
    wire [2:0]  spr_field    = spr_offset[2:0];

    // WRITE-side decoders — derived from r_cpu_addr_w (registered). The wide
    // direct memory-window subtractors used to live here; removing the
    // direct window leaves only narrow register decodes on the CPU path.
    wire vgc_reg_sel_w   = (r_cpu_addr_w >= VGC_BASE && r_cpu_addr_w <= VGC_REGS_END);
    wire vgc_irq_sel_w   = (r_cpu_addr_w >= VGC_IRQ_BASE && r_cpu_addr_w <= VGC_IRQ_END);
    wire spr_reg_sel_w   = (r_cpu_addr_w >= SPR_REG_BASE && r_cpu_addr_w <= SPR_REG_END);
    wire vram_reg_sel_w  = (r_cpu_addr_w >= VRAM_REG_BASE && r_cpu_addr_w <= VRAM_REG_END);
    wire dim_reg_sel_w   = (r_cpu_addr_w == DIM_REG_ADDR);
    wire fio_name_sel_w  = (r_cpu_addr_w >= FIO_NAME && r_cpu_addr_w <= 16'hB9EF);
    wire fio_len_sel_w   = (r_cpu_addr_w == FIO_NAME_LEN);
    wire [4:0]  reg_offset_w   = r_cpu_addr_w[4:0];
    wire [3:0]  irq_offset_w   = r_cpu_addr_w[3:0];
    wire [2:0]  vram_reg_off_w = r_cpu_addr_w[2:0];
    wire [6:0]  spr_offset_w   = r_cpu_addr_w[6:0] - 7'h40;
    wire [3:0]  spr_index_w    = spr_offset_w[6:3];
    wire [2:0]  spr_field_w    = spr_offset_w[2:0];

    // =========================================================================
    // Screen address helper
    // =========================================================================
    function automatic logic [11:0] screen_addr(input logic [6:0] col, input logic [5:0] row);
        logic [5:0] rr;
        rr = row + scroll_offset;
        if (rr >= ROWS) rr = rr - ROWS;
        screen_addr = {7'b0, rr} * COLS + {6'b0, col};
    endfunction

    wire vram_data_read = cpu_re && cpu_ce && vram_reg_sel && vram_reg_off == VR_DATA;
    wire vram_char_read = vram_data_read && vram_plane == SPACE_CHAR   && vram_addr < TEXT_SIZE;
    wire vram_color_read= vram_data_read && vram_plane == SPACE_COLOR  && vram_addr < TEXT_SIZE;
    wire vram_gfx_read  = vram_data_read && vram_plane == SPACE_GFX    && vram_addr < GFX_SIZE;
    wire vram_spr_read  = vram_data_read && vram_plane == SPACE_SPRITE && vram_addr < SPR_SIZE;
    wire vram_tile_read = vram_data_read && vram_plane == SPACE_TILE   && vram_addr < TILE_SIZE;

    // =========================================================================
    // CPU VRAM reads. VDATA uses BRAM-backed storage, so the byte returned
    // from a VDATA read is latched one cycle after the address is presented.
    // CPU-facing ROM routines should treat VDATA like a VDC port, not RAM.
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            vram_cpu_read_pending <= 0;
            vram_cpu_read_space <= SPACE_CHAR;
            vram_cpu_read_latch <= 0;
        end else begin
            vram_cpu_read_pending <= 0;
            if (vram_data_read) begin
                vram_cpu_read_pending <= 1;
                vram_cpu_read_space <= vram_plane;
            end
            if (vram_cpu_read_pending) begin
                case (vram_cpu_read_space)
                    SPACE_CHAR:   vram_cpu_read_latch <= char_a_dout;
                    SPACE_COLOR:  vram_cpu_read_latch <= color_a_dout;
                    SPACE_GFX:    vram_cpu_read_latch <= {4'b0, gfx_a_dout};
                    SPACE_SPRITE: vram_cpu_read_latch <= spr_a_dout;
                    SPACE_TILE:   vram_cpu_read_latch <= tile_blt_rdata;
                    default:      vram_cpu_read_latch <= 8'h00;
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
                        if (spr_next_enable[i]) cpu_rdata = cpu_rdata + 1;
                end
                5'd10:       cpu_rdata = {7'b0, cursor_enable};
                5'd11:       cpu_rdata = collision_ss;
                5'd12:       cpu_rdata = collision_bg;
                REG_BORDER:  cpu_rdata = {4'b0, border_color};
                REG_CHARIN:  cpu_rdata = char_in_reg;
                REG_CMD:     cpu_rdata = {7'b0, cmd_busy || artist_busy};
                default:     cpu_rdata = regs[reg_offset];
            endcase
        end
        else if (vgc_irq_sel) begin
            case (irq_offset)
                IRQ_ENABLE: cpu_rdata = irq_enable;
                IRQ_STATUS: cpu_rdata = irq_pending;
                4'h3:       cpu_rdata = IRQ_VALID;
                default:    cpu_rdata = 8'h00;
            endcase
        end
        else if (vram_reg_sel) begin
            case (vram_reg_off)
                VR_PLANE: cpu_rdata = {5'b0, vram_plane};
                VR_ADDRL: cpu_rdata = vram_addr[7:0];
                VR_ADDRH: cpu_rdata = vram_addr[15:8];
                VR_DATA:  cpu_rdata = vram_cpu_read_latch;
                VR_CTRL:  cpu_rdata = vram_ctrl;
                default:  cpu_rdata = 8'h00;
            endcase
        end
        else if (dim_reg_sel) cpu_rdata = {4'b0, display_dim};
        else if (tile_reg_sel)   cpu_rdata = tile_rdata;
        else if (spr_reg_sel) begin
            // 8 regs per sprite: X lo, X hi, Y lo, Y hi, shape, flags, pri, trans
            // Flags byte layout matches the write path (bit 7 = enable,
            // bit 1 = flip_v, bit 0 = flip_h) and Avalonia's ReadSpriteRegister.
            case (spr_field)
                3'd0: cpu_rdata = spr_next_x[spr_index][7:0];
                3'd1: cpu_rdata = spr_next_x[spr_index][15:8];
                3'd2: cpu_rdata = spr_next_y[spr_index][7:0];
                3'd3: cpu_rdata = 8'h00;
                3'd4: cpu_rdata = {4'b0, spr_next_shape[spr_index]};
                3'd5: cpu_rdata = {spr_next_enable[spr_index], 5'b0,
                                   spr_next_flip_v[spr_index], spr_next_flip_h[spr_index]};
                3'd6: cpu_rdata = {6'b0, spr_next_pri[spr_index]};
                3'd7: cpu_rdata = {4'b0, spr_next_trans[spr_index]};
            endcase
        end
    end

    // Debug read port. Video/color memory is no longer directly mapped; debug
    // can inspect registers and the VDC-style VRAM port state only.
    wire dbg_vgc_sel   = (dbg_addr >= VGC_BASE && dbg_addr <= VGC_REGS_END);
    wire dbg_irq_sel   = (dbg_addr >= VGC_IRQ_BASE && dbg_addr <= VGC_IRQ_END);
    wire dbg_tile_sel  = (dbg_addr >= 16'hA0C0 && dbg_addr <= 16'hA0DF);
    wire dbg_spr_sel   = (dbg_addr >= SPR_REG_BASE && dbg_addr <= SPR_REG_END);
    wire dbg_vram_sel  = (dbg_addr >= VRAM_REG_BASE && dbg_addr <= VRAM_REG_END);
    wire dbg_dim_sel   = (dbg_addr == DIM_REG_ADDR);
    wire dbg_write_vgc_sel  = dbg_we && (dbg_waddr >= VGC_BASE && dbg_waddr <= VGC_REGS_END);
    wire dbg_write_irq_sel  = dbg_we && (dbg_waddr >= VGC_IRQ_BASE && dbg_waddr <= VGC_IRQ_END);
    wire dbg_write_spr_sel  = dbg_we && (dbg_waddr >= SPR_REG_BASE && dbg_waddr <= SPR_REG_END);
    wire dbg_write_vram_sel = dbg_we && (dbg_waddr >= VRAM_REG_BASE && dbg_waddr <= VRAM_REG_END);
    wire dbg_write_dim_sel  = dbg_we && (dbg_waddr == DIM_REG_ADDR);
    wire [4:0] dbg_reg_offset_w = dbg_waddr[4:0];
    wire [2:0] dbg_vram_reg_off_w = dbg_waddr[2:0];

    // Debug-side sprite register decode (dbg_addr indexing mirrors the
    // cpu_addr path so peek returns exactly what PEEK from BASIC does).
    // Subtract the $A040 base — see the note on spr_offset above for why.
    wire [6:0] dbg_spr_offset = dbg_addr[6:0] - 7'h40;
    wire [3:0] dbg_spr_index  = dbg_spr_offset[6:3];
    wire [2:0] dbg_spr_field  = dbg_spr_offset[2:0];
    wire [6:0] dbg_spr_offset_w = dbg_waddr[6:0] - 7'h40;
    wire [3:0] dbg_spr_index_w  = dbg_spr_offset_w[6:3];
    wire [2:0] dbg_spr_field_w  = dbg_spr_offset_w[2:0];

    always_comb begin
        dbg_rdata = 8'h00;
        if (dbg_vgc_sel) begin
            case (dbg_addr[4:0])
                REG_MODE:    dbg_rdata = {5'b0, mode};
                REG_BGCOL:   dbg_rdata = {4'b0, bg_color};
                REG_FGCOL:   dbg_rdata = {4'b0, fg_color};
                REG_CURSORX: dbg_rdata = {1'b0, cursor_x};
                REG_CURSORY: dbg_rdata = {2'b0, cursor_y};
                5'd7:        dbg_rdata = {5'b0, font_slot};
                5'd8:        dbg_rdata = frame_counter;  // VGC_FRAME
                REG_BORDER:  dbg_rdata = {4'b0, border_color};
                REG_CHARIN:  dbg_rdata = char_in_reg;
                default:     dbg_rdata = regs[dbg_addr[4:0]];
            endcase
        end
        else if (dbg_irq_sel) begin
            case (dbg_addr[3:0])
                IRQ_ENABLE: dbg_rdata = irq_enable;
                IRQ_STATUS: dbg_rdata = irq_pending;
                4'h3:       dbg_rdata = IRQ_VALID;
                default:    dbg_rdata = 8'h00;
            endcase
        end
        else if (dbg_vram_sel) begin
            case (dbg_addr[2:0])
                VR_PLANE: dbg_rdata = {5'b0, vram_plane};
                VR_ADDRL: dbg_rdata = vram_addr[7:0];
                VR_ADDRH: dbg_rdata = vram_addr[15:8];
                VR_DATA:  dbg_rdata = vram_cpu_read_latch;
                VR_CTRL:  dbg_rdata = vram_ctrl;
                default:  dbg_rdata = 8'h00;
            endcase
        end
        else if (dbg_dim_sel) dbg_rdata = {4'b0, display_dim};
        else if (dbg_spr_sel) begin
            case (dbg_spr_field)
                3'd0: dbg_rdata = spr_next_x[dbg_spr_index][7:0];
                3'd1: dbg_rdata = spr_next_x[dbg_spr_index][15:8];
                3'd2: dbg_rdata = spr_next_y[dbg_spr_index][7:0];
                3'd3: dbg_rdata = 8'h00;
                3'd4: dbg_rdata = {4'b0, spr_next_shape[dbg_spr_index]};
                3'd5: dbg_rdata = {spr_next_enable[dbg_spr_index], 5'b0,
                                   spr_next_flip_v[dbg_spr_index], spr_next_flip_h[dbg_spr_index]};
                3'd6: dbg_rdata = {6'b0, spr_next_pri[dbg_spr_index]};
                3'd7: dbg_rdata = {4'b0, spr_next_trans[dbg_spr_index]};
            endcase
        end
    end

    // =========================================================================
    // CPU writes + command processor
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            cursor_x <= 0; cursor_y <= 0; mode <= 0;
            border_color <= 4'd11; fg_color <= 4'd15; bg_color <= 4'd0;
            gfx_color <= 4'd1; display_dim <= 4'd15;
            cursor_enable <= 1; font_slot <= 0;
            frame_counter <= 0;
            vram_plane <= SPACE_CHAR; vram_addr <= 16'd0; vram_ctrl <= 8'h01;
            scroll_offset <= 0; scroll_pending <= 0; scroll_clearing <= 0; scroll_col <= 0;
            scroll_x <= 0; scroll_y <= 0;
            cmd_busy <= 0;
            key_fifo_rd <= 0;
            collision_ss <= 0; collision_bg <= 0;
            irq_enable <= 0; irq_pending <= 0;
            copper_enabled <= 0; copper_count <= 0;
            copper_target_list <= 0; copper_pending_list <= 0;
            copper_loading <= 0;
            sprrow_count <= 0; sprcopy_phase <= 0; sprdef_wait <= 0;
            memread_pending <= 2'd0;
            vgc_tile_we <= 0; vgc_tile_re <= 0;
            artist_cmd_valid <= 0;
            // Re-arm the auto-clear so the next non-rst cycle dispatches
            // TXTCLS + GCLS to wipe the framebuffers from the previous run.
            boot_clear_armed <= 1'b1;
            for (int i = 0; i < 32; i++) regs[i] <= 0;
            for (int i = 0; i < 16; i++) begin
                spr_x[i] <= 0; spr_y[i] <= 0; spr_enable[i] <= 0;
                spr_flip_h[i] <= 0; spr_flip_v[i] <= 0;
                spr_pri[i] <= 0; spr_shape[i] <= 0; spr_trans[i] <= 0;
                spr_next_x[i] <= 0; spr_next_y[i] <= 0; spr_next_enable[i] <= 0;
                spr_next_flip_h[i] <= 0; spr_next_flip_v[i] <= 0;
                spr_next_pri[i] <= 0; spr_next_shape[i] <= 0; spr_next_trans[i] <= 0;
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
            irq_pending <= irq_pending | irq_event_mask;

            if (vram_data_read && vram_ctrl[0])
                vram_addr <= vram_addr + 16'd1;

            // Auto-clear-on-reset DISABLED 2026-04-27 — running TXTCLS+GCLS
            // in parallel with BASIC's cold-start banner caused TXTCLS to
            // clobber banner cells and corrupted REG_CHARIN reads, producing
            // BASIC syntax errors on every cold_start. The right design is
            // to hold the CPU in reset (gate CPU.RDY in top.sv) until the
            // auto-clear completes, but that's a multi-module change. For
            // now, framebuffer carryover between tests is accepted; tests
            // that need clean state can issue BASIC RESET (which still
            // dispatches via the SysReset CMD case below). boot_clear_armed
            // is kept declared so future re-enable just removes this comment.
            // if (boot_clear_armed && !cmd_busy && !artist_busy) begin
            //     cmd_busy <= 1; cmd_op <= CMD_TXTCLS;
            //     cmd_cx <= 0; cmd_cy <= 0;
            //     artist_cmd_valid <= 1; artist_cmd_code <= CMD_GCLS;
            //     boot_clear_armed <= 0;
            // end

            // Keyboard input — pop FIFO head exactly once per CPU read of
            // REG_CHARIN, on the RISING edge of the CPU's address-match.
            // cpu_addr stays at $A00F for ~2 cycles (cpu_ce alternates half
            // rate), so we must not pop every cycle. Rising-edge detection
            // via charin_sel_prev.
            charin_sel_prev <= (cpu_re && cpu_addr == (VGC_BASE + REG_CHARIN));
            key_fifo_rd <= (cpu_re && cpu_addr == (VGC_BASE + REG_CHARIN))
                        && !charin_sel_prev
                        && !key_fifo_empty;

            // Text scroll state machine. CHAROUT requests a scroll immediately
            // but the visible row-map changes only during vblank. While this
            // is pending, rdy_out stalls the 6502 so no language runtime needs
            // its own WAITVBLANK before printing.
            if (scroll_pending) begin
                if (!scroll_clearing) begin
                    if (vblank_start) begin
                        scroll_offset <= (scroll_offset >= ROWS - 1) ? 6'd0 : scroll_offset + 1;
                        scroll_clearing <= 1;
                        scroll_col <= 0;
                    end
                end else begin
                    cmd_char_addr <= screen_addr(scroll_col, ROWS - 1);
                    cmd_char_din <= 8'h20;
                    cmd_char_we <= 1;
                    cmd_color_addr <= screen_addr(scroll_col, ROWS - 1);
                    cmd_color_din <= {4'b0, fg_color};
                    cmd_color_we <= 1;
                    if (scroll_col == COLS - 1) begin
                        scroll_pending <= 0; scroll_clearing <= 0; scroll_col <= 0;
                    end else
                        scroll_col <= scroll_col + 1;
                end
            end

            // Clear VGC tile command strobes
            vgc_tile_we <= 0;
            vgc_tile_re <= 0;

            // MemRead latency: 2-cycle pipeline. Memory-space IDs match the
            // VDC-style VRAM port/DMA/blitter plane IDs.
            //   memread_pending==2 after dispatch — re-assert the read
            //     strobe / keep the address alive so the dpram samples it
            //     during this cycle; data will be valid next cycle.
            //   memread_pending==1 — capture *_a_dout into regs[20].
            if (memread_pending == 2'd2) begin
                case (memread_space)
                    SPACE_GFX:    cmd_gfx_re <= 1;       // keep gfx read active
                    SPACE_SPRITE: cmd_spr_re <= 1;       // keep sprite read active
                    SPACE_TILE:   vgc_tile_re <= 1;      // keep tile read active
                    // char/color use the port A default-addr
                    // path which holds cmd_char_addr / cmd_color_addr.
                    default: ;
                endcase
                memread_pending <= 2'd1;
            end else if (memread_pending == 2'd1) begin
                case (memread_space)
                    SPACE_CHAR:   regs[20] <= char_a_dout;
                    SPACE_COLOR:  regs[20] <= color_a_dout;
                    SPACE_GFX:    regs[20] <= {4'b0, gfx_a_dout};
                    SPACE_SPRITE: regs[20] <= spr_a_dout;
                    SPACE_TILE:   regs[20] <= tile_blt_rdata;
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
            if (vblank_start)
                frame_counter <= frame_counter + 1'b1;

            // Copper vblank: swap pending→active list if changed
            if (vblank_start) begin
                if (copper_pending_list != copper_active_list && !copper_loading) begin
                    copper_active_list <= copper_pending_list;
                    copper_loading <= 1;
                    copper_load_idx <= 0;
                    copper_load_src <= copper_pending_list;
                end
            end

            // Sprite frame boundary: atomically publish CPU/command-visible
            // pending sprite attributes to the renderer.
            if (sprite_frame_publish) begin
                for (int i = 0; i < NUM_SPRITES; i++) begin
                    spr_x[i] <= spr_next_x[i];
                    spr_y[i] <= spr_next_y[i];
                    spr_enable[i] <= spr_next_enable[i];
                    spr_flip_h[i] <= spr_next_flip_h[i];
                    spr_flip_v[i] <= spr_next_flip_v[i];
                    spr_pri[i] <= spr_next_pri[i];
                    spr_shape[i] <= spr_next_shape[i];
                    spr_trans[i] <= spr_next_trans[i];
                end
            end

            // Copper fire: apply register writes from sub-module
            if (copper_fire) begin
                case (copper_fire_reg)
                    8'd1: bg_color     <= copper_fire_val[3:0];
                    8'd2: fg_color     <= copper_fire_val[3:0];
                    8'd13: border_color <= copper_fire_val[3:0];
                    COPPER_REG_IRQ: ; // irq_event_mask latches enabled source bits
                    default: regs[copper_fire_reg[4:0]] <= copper_fire_val;
                endcase
            end

            // CPU writes — fires on `write_active` (1 pixel cycle after
            // cpu_we && cpu_ce). All write-side signals are the registered
            // _w variants (see register-slice section above). Net effect:
            // VGC writes complete 1 pixel cycle later than today, but the
            // BRAM→CPU→FF.D combinational chain is broken at r_cpu_addr_w.
            if (write_active) begin
                if (vgc_reg_sel_w) begin
                    case (reg_offset_w)
                        REG_MODE:    mode <= r_cpu_wdata_w[2:0];
                        REG_BGCOL:   bg_color <= r_cpu_wdata_w[3:0];
                        REG_FGCOL:   fg_color <= r_cpu_wdata_w[3:0];
                        REG_CURSORX: cursor_x <= r_cpu_wdata_w[6:0];
                        REG_CURSORY: cursor_y <= (r_cpu_wdata_w[5:0] > LAST_TEXT_ROW)
                                                   ? LAST_TEXT_ROW
                                                   : r_cpu_wdata_w[5:0];
                        5'd5:        scroll_x <= r_cpu_wdata_w;
                        5'd6:        scroll_y <= r_cpu_wdata_w;
                        5'd7:        font_slot <= (r_cpu_wdata_w[2:0] >= 3'd3) ? 3'd0
                                                                               : r_cpu_wdata_w[2:0];
                        5'd8:        gfx_color <= r_cpu_wdata_w[3:0];
                        5'd10:       cursor_enable <= r_cpu_wdata_w[0];
                        5'd11:       collision_ss <= 0;
                        5'd12:       collision_bg <= 0;
                        REG_BORDER:  border_color <= r_cpu_wdata_w[3:0];
                        REG_CHAROUT: begin
                            case (r_cpu_wdata_w)
                                8'h08: begin
                                    if (cursor_x > 0) cursor_x <= cursor_x - 1;
                                    cmd_char_addr <= screen_addr(cursor_x > 0 ? cursor_x - 1 : 0, cursor_y);
                                    cmd_char_din <= 8'h20;
                                    cmd_char_we <= 1;
                                end
                                8'h0A: begin
                                    if (cursor_y >= ROWS - 1) begin
                                        scroll_pending <= 1; scroll_clearing <= 0; scroll_col <= 0;
                                    end else
                                        cursor_y <= cursor_y + 1;
                                end
                                8'h0C: begin
                                    cursor_x <= 0; cursor_y <= 0;
                                    scroll_offset <= 0;
                                    scroll_pending <= 0; scroll_clearing <= 0; scroll_col <= 0;
                                    cmd_cx <= 0; cmd_cy <= 0;
                                    cmd_busy <= 1; cmd_op <= CMD_TXTCLS;
                                end
                                8'h0D: cursor_x <= 0;
                                8'h13: begin cursor_x <= 0; cursor_y <= 0; end
                                default: begin
                                    if (r_cpu_wdata_w >= 8'h20) begin
                                        cmd_char_addr <= screen_addr(cursor_x, cursor_y);
                                        cmd_char_din <= r_cpu_wdata_w;
                                        cmd_char_we <= 1;
                                        cmd_color_addr <= screen_addr(cursor_x, cursor_y);
                                        cmd_color_din <= {4'b0, fg_color};
                                        cmd_color_we <= 1;
                                        if (cursor_x >= COLS - 1) begin
                                            cursor_x <= 0;
                                            if (cursor_y >= ROWS - 1) begin
                                                scroll_pending <= 1; scroll_clearing <= 0; scroll_col <= 0;
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

                                case (r_cpu_wdata_w)
                                    // Drawing commands → ARTIST
                                    CMD_PLOT, CMD_UNPLOT, CMD_LINE,
                                    CMD_CIRCLE, CMD_RECT, CMD_FILL,
                                    CMD_GCLS, CMD_PAINT, CMD_GTEXT: begin
                                        artist_cmd_valid <= 1;
                                        artist_cmd_code <= r_cpu_wdata_w;
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
                                        // Unsigned coords: regs[18]/regs[19] = X lo/hi,
                                        // regs[20] = Y. regs[21] is reserved/ignored.
                                        if (regs[17] < NUM_SPRITES) begin
                                            spr_next_x[regs[17][3:0]] <= {regs[19], regs[18]};
                                            spr_next_y[regs[17][3:0]] <= regs[20];
                                        end
                                    end
                                    CMD_SPRENA: begin
                                        if (regs[17] < NUM_SPRITES)
                                            spr_next_enable[regs[17][3:0]] <= 1;
                                    end
                                    CMD_SPRDIS: begin
                                        if (regs[17] < NUM_SPRITES)
                                            spr_next_enable[regs[17][3:0]] <= 0;
                                    end
                                    CMD_SPRFLIP: begin
                                        if (regs[17] < NUM_SPRITES) begin
                                            spr_next_flip_h[regs[17][3:0]] <= regs[18][0];
                                            spr_next_flip_v[regs[17][3:0]] <= regs[18][1];
                                        end
                                    end
                                    CMD_SPRPRI: begin
                                        if (regs[17] < NUM_SPRITES)
                                            spr_next_pri[regs[17][3:0]] <= regs[18][1:0];
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
                                    8'h1F: begin // SysReset — bring VGC to its FPGA-rst
                                        // initial state, plus clear the framebuffers (which
                                        // the rst clause can't do — they're multi-cycle FSMs).
                                        // KEEP THIS LIST IN SYNC WITH THE if(rst) BRANCH ABOVE.
                                        // Diagnosed 2026-04-27: SysReset previously only cleared
                                        // a subset of state, so sprites stayed enabled and
                                        // GTEXT bitmaps in gfx_mem persisted across RESETs.
                                        cursor_x <= 0; cursor_y <= 0; mode <= 0;
                                        border_color <= 4'd11; fg_color <= 4'd15; bg_color <= 4'd0;
                                        gfx_color <= 4'd1; display_dim <= 4'd15;
                                        cursor_enable <= 1; font_slot <= 0;
                                        scroll_offset <= 0; scroll_pending <= 0; scroll_clearing <= 0; scroll_col <= 0;
                                        scroll_x <= 0; scroll_y <= 0;
                                        key_fifo_rd <= 0;
                                        collision_ss <= 0; collision_bg <= 0;
                                        irq_enable <= 0; irq_pending <= 0;
                                        copper_enabled <= 0; copper_count <= 0;
                                        copper_target_list <= 0; copper_active_list <= 0;
                                        copper_pending_list <= 0; copper_loading <= 0;
                                        for (int i = 0; i < COPPER_LISTS; i++)
                                            copper_list_count[i] <= 0;
                                        sprrow_count <= 0; sprcopy_phase <= 0; sprdef_wait <= 0;
                                        memread_pending <= 2'd0;
                                        vgc_tile_we <= 0; vgc_tile_re <= 0;
                                        vram_plane <= SPACE_CHAR; vram_addr <= 16'd0; vram_ctrl <= 8'h01;
                                        for (int i = 0; i < 32; i++) regs[i] <= 0;
                                        for (int i = 0; i < NUM_SPRITES; i++) begin
                                            spr_x[i] <= 0; spr_y[i] <= 0; spr_enable[i] <= 0;
                                            spr_flip_h[i] <= 0; spr_flip_v[i] <= 0;
                                            spr_pri[i] <= 0; spr_shape[i] <= 0; spr_trans[i] <= 0;
                                            spr_next_x[i] <= 0; spr_next_y[i] <= 0; spr_next_enable[i] <= 0;
                                            spr_next_flip_h[i] <= 0; spr_next_flip_v[i] <= 0;
                                            spr_next_pri[i] <= 0; spr_next_shape[i] <= 0; spr_next_trans[i] <= 0;
                                        end
                                        // Re-arm the auto-clear trigger — the post-rst
                                        // dispatcher above will fire TXTCLS + GCLS on the
                                        // next cycle (when cmd_busy and artist_busy are 0).
                                        // Single source of truth for framebuffer clearing.
                                        boot_clear_armed <= 1'b1;
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
                                            SPACE_CHAR: begin
                                                cmd_char_addr <= {regs[19], regs[18]};
                                                memread_pending <= 2'd2;
                                                memread_space <= SPACE_CHAR;
                                            end
                                            SPACE_COLOR: begin
                                                cmd_color_addr <= {regs[19], regs[18]};
                                                memread_pending <= 2'd2;
                                                memread_space <= SPACE_COLOR;
                                            end
                                            SPACE_GFX: begin
                                                cmd_gfx_addr <= {regs[19], regs[18]};
                                                cmd_gfx_re <= 1;
                                                memread_pending <= 2'd2;
                                                memread_space <= SPACE_GFX;
                                            end
                                            SPACE_SPRITE: begin
                                                cmd_spr_addr <= {regs[19], regs[18]};
                                                cmd_spr_re <= 1;
                                                memread_pending <= 2'd2;
                                                memread_space <= SPACE_SPRITE;
                                            end
                                            SPACE_TILE: begin
                                                vgc_tile_addr <= {regs[19][6:0], regs[18]};
                                                vgc_tile_re <= 1;
                                                memread_pending <= 2'd2;
                                                memread_space <= SPACE_TILE;
                                            end
                                            default: ;
                                        endcase
                                    end
                                    8'h1A: begin // MemWrite
                                        case (regs[17])
                                            SPACE_CHAR: begin
                                                cmd_char_addr <= {regs[19], regs[18]};
                                                cmd_char_din <= regs[20];
                                                cmd_char_we <= 1;
                                            end
                                            SPACE_COLOR: begin
                                                cmd_color_addr <= {regs[19], regs[18]};
                                                cmd_color_din <= regs[20];
                                                cmd_color_we <= 1;
                                            end
                                            SPACE_GFX: begin
                                                cmd_gfx_addr <= {regs[19], regs[18]};
                                                cmd_gfx_din <= regs[20][3:0];
                                                cmd_gfx_we <= 1;
                                            end
                                            SPACE_SPRITE: begin
                                                cmd_spr_addr <= {regs[19], regs[18]};
                                                cmd_spr_din <= regs[20];
                                                cmd_spr_we <= 1;
                                            end
                                            SPACE_TILE: begin
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
                        default: regs[reg_offset_w] <= r_cpu_wdata_w;
                    endcase
                end

                if (vram_reg_sel_w) begin
                    case (vram_reg_off_w)
                        VR_PLANE: vram_plane <= r_cpu_wdata_w[2:0];
                        VR_ADDRL: vram_addr[7:0] <= r_cpu_wdata_w;
                        VR_ADDRH: vram_addr[15:8] <= r_cpu_wdata_w;
                        VR_CTRL:  vram_ctrl <= r_cpu_wdata_w;
                        VR_DATA: begin
                            case (vram_plane)
                                SPACE_CHAR: begin
                                    if (vram_addr < TEXT_SIZE) begin
                                        cmd_char_addr <= vram_addr[11:0];
                                        cmd_char_din <= r_cpu_wdata_w;
                                        cmd_char_we <= 1;
                                    end
                                end
                                SPACE_COLOR: begin
                                    if (vram_addr < TEXT_SIZE) begin
                                        cmd_color_addr <= vram_addr[11:0];
                                        cmd_color_din <= r_cpu_wdata_w;
                                        cmd_color_we <= 1;
                                    end
                                end
                                SPACE_GFX: begin
                                    if (vram_addr < GFX_SIZE) begin
                                        cmd_gfx_addr <= {1'b0, vram_addr};
                                        cmd_gfx_din <= r_cpu_wdata_w[3:0];
                                        cmd_gfx_we <= 1;
                                    end
                                end
                                SPACE_SPRITE: begin
                                    if (vram_addr < SPR_SIZE) begin
                                        cmd_spr_addr <= vram_addr[10:0];
                                        cmd_spr_din <= r_cpu_wdata_w;
                                        cmd_spr_we <= 1;
                                    end
                                end
                                SPACE_TILE: begin
                                    if (vram_addr < TILE_SIZE) begin
                                        vgc_tile_addr <= vram_addr[14:0];
                                        vgc_tile_wdata <= r_cpu_wdata_w;
                                        vgc_tile_we <= 1;
                                    end
                                end
                                default: ;
                            endcase
                            if (vram_ctrl[0])
                                vram_addr <= vram_addr + 16'd1;
                        end
                        default: ;
                    endcase
                end

                if (dim_reg_sel_w)
                    display_dim <= r_cpu_wdata_w[3:0];

                // Shadow FIO name buffer for Gtext
                if (fio_len_sel_w)   fio_name_len <= r_cpu_wdata_w[5:0];
                if (fio_name_sel_w)  fio_name[r_cpu_addr_w - FIO_NAME] <= r_cpu_wdata_w;

                // Sprite register writes
                if (spr_reg_sel_w) begin
                    case (spr_field_w)
                        3'd0: spr_next_x[spr_index_w][7:0]  <= r_cpu_wdata_w;
                        3'd1: spr_next_x[spr_index_w][15:8] <= r_cpu_wdata_w;
                        3'd2: spr_next_y[spr_index_w][7:0]  <= r_cpu_wdata_w;
                        3'd3: ; // Y high byte reserved; Y is unsigned 0..239.
                        3'd4: spr_next_shape[spr_index_w]   <= r_cpu_wdata_w[3:0];
                        3'd5: begin
                            spr_next_flip_h[spr_index_w] <= r_cpu_wdata_w[0];
                            spr_next_flip_v[spr_index_w] <= r_cpu_wdata_w[1];
                            spr_next_enable[spr_index_w] <= r_cpu_wdata_w[7];
                        end
                        3'd6: spr_next_pri[spr_index_w]     <= r_cpu_wdata_w[1:0];
                        3'd7: spr_next_trans[spr_index_w]    <= r_cpu_wdata_w[3:0];
                    endcase
                end

                if (vgc_irq_sel_w) begin
                    case (irq_offset_w)
                        IRQ_ENABLE: irq_enable <= r_cpu_wdata_w & IRQ_VALID;
                        IRQ_STATUS: irq_pending <= (irq_pending | irq_event_mask) &
                                                   ~(r_cpu_wdata_w & IRQ_VALID);
                        IRQ_FORCE:  irq_pending <= irq_pending |
                                                   (r_cpu_wdata_w & IRQ_VALID & irq_enable) |
                                                   irq_event_mask;
                        default: ;
                    endcase
                end
            end

            // Debug writes are independent of the 6502 bus. NovaHost uses
            // this while the CPU is held in reset for boot-time splash setup.
            if (dbg_write_vgc_sel) begin
                case (dbg_reg_offset_w)
                    REG_MODE:    mode <= dbg_wdata[2:0];
                    REG_BGCOL:   bg_color <= dbg_wdata[3:0];
                    REG_FGCOL:   fg_color <= dbg_wdata[3:0];
                    REG_CURSORX: cursor_x <= dbg_wdata[6:0];
                    REG_CURSORY: cursor_y <= (dbg_wdata[5:0] > LAST_TEXT_ROW)
                                               ? LAST_TEXT_ROW
                                               : dbg_wdata[5:0];
                    5'd5:        scroll_x <= dbg_wdata;
                    5'd6:        scroll_y <= dbg_wdata;
                    5'd7:        font_slot <= (dbg_wdata[2:0] >= 3'd3) ? 3'd0
                                                                        : dbg_wdata[2:0];
                    5'd8:        gfx_color <= dbg_wdata[3:0];
                    5'd10:       cursor_enable <= dbg_wdata[0];
                    5'd11:       collision_ss <= 0;
                    5'd12:       collision_bg <= 0;
                    REG_BORDER:  border_color <= dbg_wdata[3:0];
                    default: ;
                endcase
            end

            if (dbg_write_irq_sel) begin
                case (dbg_waddr[3:0])
                    IRQ_ENABLE: irq_enable <= dbg_wdata & IRQ_VALID;
                    IRQ_STATUS: irq_pending <= (irq_pending | irq_event_mask) &
                                               ~(dbg_wdata & IRQ_VALID);
                    IRQ_FORCE:  irq_pending <= irq_pending |
                                               (dbg_wdata & IRQ_VALID & irq_enable) |
                                               irq_event_mask;
                    default: ;
                endcase
            end

            if (dbg_write_spr_sel) begin
                case (dbg_spr_field_w)
                    3'd0: spr_next_x[dbg_spr_index_w][7:0]  <= dbg_wdata;
                    3'd1: spr_next_x[dbg_spr_index_w][15:8] <= dbg_wdata;
                    3'd2: spr_next_y[dbg_spr_index_w][7:0]  <= dbg_wdata;
                    3'd3: ; // Y high byte reserved; Y is unsigned 0..239.
                    3'd4: spr_next_shape[dbg_spr_index_w]   <= dbg_wdata[3:0];
                    3'd5: begin
                        spr_next_flip_h[dbg_spr_index_w] <= dbg_wdata[0];
                        spr_next_flip_v[dbg_spr_index_w] <= dbg_wdata[1];
                        spr_next_enable[dbg_spr_index_w] <= dbg_wdata[7];
                    end
                    3'd6: spr_next_pri[dbg_spr_index_w]     <= dbg_wdata[1:0];
                    3'd7: spr_next_trans[dbg_spr_index_w]    <= dbg_wdata[3:0];
                endcase
            end

            if (dbg_write_vram_sel) begin
                case (dbg_vram_reg_off_w)
                    VR_PLANE: vram_plane <= dbg_wdata[2:0];
                    VR_ADDRL: vram_addr[7:0] <= dbg_wdata;
                    VR_ADDRH: vram_addr[15:8] <= dbg_wdata;
                    VR_CTRL:  vram_ctrl <= dbg_wdata;
                    VR_DATA: begin
                        case (vram_plane)
                            SPACE_CHAR: begin
                                if (vram_addr < TEXT_SIZE) begin
                                    cmd_char_addr <= vram_addr[11:0];
                                    cmd_char_din <= dbg_wdata;
                                    cmd_char_we <= 1;
                                end
                            end
                            SPACE_COLOR: begin
                                if (vram_addr < TEXT_SIZE) begin
                                    cmd_color_addr <= vram_addr[11:0];
                                    cmd_color_din <= dbg_wdata;
                                    cmd_color_we <= 1;
                                end
                            end
                            SPACE_GFX: begin
                                if (vram_addr < GFX_SIZE) begin
                                    cmd_gfx_addr <= {1'b0, vram_addr};
                                    cmd_gfx_din <= dbg_wdata[3:0];
                                    cmd_gfx_we <= 1;
                                end
                            end
                            SPACE_SPRITE: begin
                                if (vram_addr < SPR_SIZE) begin
                                    cmd_spr_addr <= vram_addr[10:0];
                                    cmd_spr_din <= dbg_wdata;
                                    cmd_spr_we <= 1;
                                end
                            end
                            SPACE_TILE: begin
                                if (vram_addr < TILE_SIZE) begin
                                    vgc_tile_addr <= vram_addr[14:0];
                                    vgc_tile_wdata <= dbg_wdata;
                                    vgc_tile_we <= 1;
                                end
                            end
                            default: ;
                        endcase
                        if (vram_ctrl[0])
                            vram_addr <= vram_addr + 16'd1;
                    end
                    default: ;
                endcase
            end

            if (dbg_write_dim_sel)
                display_dim <= dbg_wdata[3:0];

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

    wire dbg_vmem_char_we = dbg_vmem_we && dbg_vmem_space == SPACE_CHAR &&
                            (dbg_vmem_addr < TEXT_SIZE);
    wire dbg_vmem_color_we = dbg_vmem_we && dbg_vmem_space == SPACE_COLOR &&
                             (dbg_vmem_addr < TEXT_SIZE);
    wire dbg_vmem_gfx_we = dbg_vmem_we && dbg_vmem_space == SPACE_GFX &&
                           (dbg_vmem_addr < GFX_SIZE);
    wire dbg_vmem_spr_we = dbg_vmem_we && dbg_vmem_space == SPACE_SPRITE &&
                           (dbg_vmem_addr < SPR_SIZE);
    wire dbg_vmem_tile_we = dbg_vmem_we && dbg_vmem_space == SPACE_TILE &&
                            (dbg_vmem_addr < TILE_SIZE);

    // Port A address/data/we mux for each memory
    always_comb begin
        // char_ram port A
        char_a_we = 0;
        char_a_addr = 12'd0;
        char_a_din = 8'd0;
        if (dbg_vmem_char_we) begin
            char_a_addr = dbg_vmem_addr[11:0];
            char_a_din = dbg_vmem_wdata;
            char_a_we = 1;
        end else if (blt_we && blt_space == 3'd1) begin
            char_a_addr = blt_addr[11:0];
            char_a_din = blt_wdata;
            char_a_we = 1;
        end else if (cmd_char_we) begin
            char_a_addr = cmd_char_addr;
            char_a_din = cmd_char_din;
            char_a_we = 1;
        end else if (blt_re && blt_space == 3'd1) begin
            char_a_addr = blt_addr[11:0];
        end else if (vram_char_read) begin
            char_a_addr = vram_addr[11:0];
        end else begin
            char_a_addr = cmd_char_addr;
        end

        // color_ram port A
        color_a_we = 0;
        color_a_addr = 12'd0;
        color_a_din = 8'd0;
        if (dbg_vmem_color_we) begin
            color_a_addr = dbg_vmem_addr[11:0];
            color_a_din = dbg_vmem_wdata;
            color_a_we = 1;
        end else if (blt_we && blt_space == 3'd2) begin
            color_a_addr = blt_addr[11:0];
            color_a_din = blt_wdata;
            color_a_we = 1;
        end else if (cmd_color_we) begin
            color_a_addr = cmd_color_addr;
            color_a_din = cmd_color_din;
            color_a_we = 1;
        end else if (blt_re && blt_space == 3'd2) begin
            color_a_addr = blt_addr[11:0];
        end else if (vram_color_read) begin
            color_a_addr = vram_addr[11:0];
        end else begin
            color_a_addr = cmd_color_addr;
        end

        // font_rom port A — ARTIST font reads take priority.
        // artist_font_addr is per-slot (11-bit glyph index); prefix with the
        // active font_slot so GTEXT uses the same bank the text renderer does.
        font_a_we = 0;
        font_a_addr = 13'd0;
        font_a_din = 8'd0;
        if (artist_font_re) begin
            font_a_addr = {font_slot[1:0], artist_font_addr};
        end

        // gfx_ram port A — priority: blitter > ARTIST > cmd > blitter-read > cmd-read
        gfx_a_we = 0;
        gfx_a_addr = 17'd0;
        gfx_a_din = 4'd0;
        if (dbg_vmem_gfx_we) begin
            gfx_a_addr = dbg_vmem_addr;
            gfx_a_din = dbg_vmem_wdata[3:0];
            gfx_a_we = 1;
        end else if (blt_we && blt_space == 3'd3) begin
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
        end else if (vram_gfx_read) begin
            gfx_a_addr = {1'b0, vram_addr};
        end

        // sprite_shapes port A
        spr_a_we = 0;
        spr_a_re = 0;
        spr_a_addr = 11'd0;
        spr_a_din = 8'd0;
        if (dbg_vmem_spr_we) begin
            spr_a_addr = dbg_vmem_addr[10:0];
            spr_a_din = dbg_vmem_wdata;
            spr_a_we = 1;
        end else if (blt_we && blt_space == 3'd4) begin
            spr_a_addr = blt_addr[10:0];
            spr_a_din = blt_wdata;
            spr_a_we = 1;
        end else if (cmd_spr_we) begin
            spr_a_addr = cmd_spr_addr;
            spr_a_din = cmd_spr_din;
            spr_a_we = 1;
        end else if (blt_re && blt_space == 3'd4) begin
            spr_a_addr = blt_addr[10:0];
            spr_a_re = 1;
        end else if (cmd_spr_re) begin
            spr_a_addr = cmd_spr_addr;
            spr_a_re = 1;
        end else if (vram_spr_read) begin
            spr_a_addr = vram_addr[10:0];
            spr_a_re = 1;
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

    wire [14:0] tile_blt_addr  = dbg_vmem_tile_we ? dbg_vmem_addr[14:0] :
                                  (vgc_tile_we || vgc_tile_re) ? vgc_tile_addr :
                                  vram_tile_read ? vram_addr[14:0] : blt_addr[14:0];
    wire [7:0]  tile_blt_wdata = dbg_vmem_tile_we ? dbg_vmem_wdata :
                                  vgc_tile_we ? vgc_tile_wdata : blt_wdata;
    wire        tile_blt_we    = dbg_vmem_tile_we || vgc_tile_we || (blt_we && blt_space == 3'd6);
    wire        tile_blt_re    = vgc_tile_re || vram_tile_read || (blt_re && blt_space == 3'd6);

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

`ifndef SYNTHESIS
    always_comb begin
        dbg_vram_rdata = 8'h00;

        if (dbg_vram_read_en) begin
            unique case (dbg_vram_space)
                SPACE_CHAR: begin
                    if (dbg_vram_addr < TEXT_SIZE)
                        dbg_vram_rdata = text_inst.char_mem.mem[dbg_vram_addr[11:0]];
                end
                SPACE_COLOR: begin
                    if (dbg_vram_addr < TEXT_SIZE)
                        dbg_vram_rdata = text_inst.color_mem.mem[dbg_vram_addr[11:0]];
                end
                SPACE_GFX: begin
                    if (dbg_vram_addr < GFX_SIZE)
                        dbg_vram_rdata = {4'h0, gfx_inst.gfx_mem.mem[dbg_vram_addr[15:0]]};
                end
                SPACE_SPRITE: begin
                    if (dbg_vram_addr < SPR_SIZE) begin
                        dbg_vram_rdata = sprite_inst.pending_shape_bank
                            ? sprite_inst.spr_mem1.mem[dbg_vram_addr[10:0]]
                            : sprite_inst.spr_mem0.mem[dbg_vram_addr[10:0]];
                    end
                end
                SPACE_TILE: begin
                    if (dbg_vram_addr < TILE_SIZE)
                        dbg_vram_rdata = tile_inst.tile_data_ram.mem[dbg_vram_addr[14:0]];
                end
                default: ;
            endcase
        end
    end
`endif

    // =========================================================================
    // Pixel compositing
    // =========================================================================
    logic [3:0]  cur_fg_d2;
    logic [3:0]  cur_gfx_d2;
    logic        pixel_on_d2;
    logic [11:0] text_pixel_d2, gfx_pixel_d2, pixel_color;

    // Tile engine output delayed to match pipeline. POR determinism via
    // declaration init — ECP5 trellis honors `= 0` as bitstream FF init,
    // costs nothing in fabric. Without init, mode-4 boot starts with garbage
    // tile pixels until the first valid tile_rgb propagates.
    logic [11:0] tile_rgb_d1 = 0, tile_rgb_d2 = 0;
    logic [1:0]  tile_opaque_d1 = 0, tile_opaque_d2 = 0;
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

        // Layer compositing with sprite priorities. `visible_d2` is the
        // physical active-video window; `in_text_area_d2` is the Nova canvas.
        // In 720x480 mode the difference paints the active side borders.
        if (!visible_d2)
            pixel_color = 12'h000;
        else begin
            if (!in_text_area_d2) begin
                pixel_color = palette[border_color];
            end else begin
                case (mode)
                    3'd0: pixel_color = text_pixel_d2;
                    3'd1: pixel_color = (cur_gfx_d2 != 0) ? gfx_pixel_d2 : text_pixel_d2;
                    3'd2: pixel_color = pixel_on_d2 ? palette[cur_fg_d2] :
                                        (cur_gfx_d2 != 0) ? gfx_pixel_d2 : palette[bg_color];
                    3'd3: pixel_color = (cur_gfx_d2 != 0) ? gfx_pixel_d2 : palette[bg_color];
                    3'd4: pixel_color = (tile_opaque_d2 != 0) ? tile_rgb_d2 : palette[bg_color];
                    default: pixel_color = text_pixel_d2;
                endcase
            end

            if (spr_pixel_hit && in_text_area_d2) begin
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

    // Cursor blink — POR determinism via declaration init.
    logic [4:0] blink_count = 0;
    logic       cursor_blink = 0;
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

    assign irq_out = |(irq_pending & irq_enable);

`ifdef VGC_HW_MOTION_DIAG
    // Hardware-only motion diagnostic. This bypasses the CPU, BASIC, sprite
    // register publish path, and sprite scanline buffer.
    //
    // Orange lane: 320x240 logical coordinates, matching sprite motion.
    // Cyan lane: raw 640x480 physical coordinates, one physical pixel/frame.
    // If both lanes hitch together, the problem is downstream cadence/clocking.
    // If only orange looks steppy, that is the visible 2x pixel scale.
    logic [8:0] hwdiag_log_x = 9'd0;
    logic       hwdiag_log_left = 1'b0;
    logic [9:0] hwdiag_phys_x = 10'd0;
    logic       hwdiag_phys_left = 1'b0;

    always_ff @(posedge clk) begin
        if (rst) begin
            hwdiag_log_x <= 9'd0;
            hwdiag_log_left <= 1'b0;
            hwdiag_phys_x <= 10'd0;
            hwdiag_phys_left <= 1'b0;
        end else if (vblank_start) begin
            if (hwdiag_log_left) begin
                if (hwdiag_log_x == 9'd0)
                    hwdiag_log_left <= 1'b0;
                else
                    hwdiag_log_x <= hwdiag_log_x - 9'd1;
            end else begin
                if (hwdiag_log_x >= 9'd304)
                    hwdiag_log_left <= 1'b1;
                else
                    hwdiag_log_x <= hwdiag_log_x + 9'd1;
            end

            if (hwdiag_phys_left) begin
                if (hwdiag_phys_x == 10'd0)
                    hwdiag_phys_left <= 1'b0;
                else
                    hwdiag_phys_x <= hwdiag_phys_x - 10'd1;
            end else begin
                if (hwdiag_phys_x >= 10'd608)
                    hwdiag_phys_left <= 1'b1;
                else
                    hwdiag_phys_x <= hwdiag_phys_x + 10'd1;
            end
        end
    end

    wire [9:0] hwdiag_log_x0 = {1'b0, hwdiag_log_x};
    wire [9:0] hwdiag_log_x1 = {1'b0, hwdiag_log_x} + 10'd16;
    wire [9:0] hwdiag_phys_x1 = hwdiag_phys_x + 10'd32;

    wire       hwdiag_log_hit = visible_d2 &&
                                ({1'b0, gfx_x_d2} >= hwdiag_log_x0) &&
                                ({1'b0, gfx_x_d2} <  hwdiag_log_x1) &&
                                (gfx_y_d2 >= 8'd88) &&
                                (gfx_y_d2 <  8'd104);
    wire       hwdiag_phys_hit = visible_d2 &&
                                 (h_count_d2 >= hwdiag_phys_x) &&
                                 (h_count_d2 <  hwdiag_phys_x1) &&
                                 (v_count_d2 >= 10'd272) &&
                                 (v_count_d2 <  10'd304);
`endif

    // Output — POR-deterministic via internal regs with declaration init.
    // We CAN'T put init on output ports directly. The internal `vid_*_r`
    // regs get INITVAL encoded into the bitstream (LSR tied to 0 when no
    // `if (rst)` clause). Continuous assigns drive the output ports.
    //
    // MUST start at known idle. If vid_hsync comes up at 0 (sync asserted)
    // for the first cycles after PLL lock + rst release, the HDMI sink
    // interprets that as the start-of-line sync pulse at the wrong h_count,
    // locks onto the wrong horizontal phase, and never re-syncs → variable
    // horizontal shift across reflashes of the same bitstream (50px black
    // bar / 1-char shift / clean). Idle = HIGH for both syncs (negative-
    // polarity). Diagnosed 2026-04-26 via prior session's POR-init work.
    // Re-broken 2026-04-27 when register-slice change reshuffled PnR; root
    // cause: yosys+nextpnr drop INITVAL from any TRELLIS_FF whose LSR is
    // non-trivial (i.e., has an `if (rst)` clause). Fix: remove the `if(rst)`
    // clause and rely on declaration init `= X` to encode INITVAL with
    // LSR tied to 1'h0. h_sync_area_d2 / v_sync_area_d2 / visible_d2 are
    // already POR-init'd to 0 in vgc_timing.sv, so on the very first clock
    // edge after config, vid_hsync_r samples ~0 = 1 (idle), preserving the
    // `if(rst)` behavior without depending on rst signal arrival timing.
    // Internal regs with BOTH declaration init AND rst clause. INITVAL is
    // encoded from declaration init; runtime rst behavior is preserved by
    // the if(rst) clause. yosys+nextpnr-ecp5 honors both: the FF starts at
    // INITVAL at config time AND resets to the same value on rst assertion.
    // This is the correct combination — POR-deterministic AND runtime-safe.
    logic       vid_hsync_r = 1'b1;
    logic       vid_vsync_r = 1'b1;
    logic       vid_de_r    = 1'b0;
    logic [3:0] vid_r_r     = 4'h0;
    logic [3:0] vid_g_r     = 4'h0;
    logic [3:0] vid_b_r     = 4'h0;
    logic [7:0] dim_r;
    logic [7:0] dim_g;
    logic [7:0] dim_b;
    logic [11:0] post_cursor_color;
    logic [3:0] dimmed_r;
    logic [3:0] dimmed_g;
    logic [3:0] dimmed_b;

    assign vid_hsync = vid_hsync_r;
    assign vid_vsync = vid_vsync_r;
    assign vid_de    = vid_de_r;
    assign vid_r     = vid_r_r;
    assign vid_g     = vid_g_r;
    assign vid_b     = vid_b_r;

    always_comb begin
        post_cursor_color = (cursor_here && cursor_blink) ? palette[fg_color] : pixel_color;
        dim_r = post_cursor_color[11:8] * display_dim;
        dim_g = post_cursor_color[7:4] * display_dim;
        dim_b = post_cursor_color[3:0] * display_dim;
        dimmed_r = (display_dim == 4'hF) ? post_cursor_color[11:8] : dim_r[7:4];
        dimmed_g = (display_dim == 4'hF) ? post_cursor_color[7:4]  : dim_g[7:4];
        dimmed_b = (display_dim == 4'hF) ? post_cursor_color[3:0]  : dim_b[7:4];
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            vid_hsync_r <= 1'b1;
            vid_vsync_r <= 1'b1;
            vid_de_r    <= 1'b0;
            vid_r_r     <= 4'h0;
            vid_g_r     <= 4'h0;
            vid_b_r     <= 4'h0;
        end else begin
            vid_hsync_r <= ~h_sync_area_d2;
            vid_vsync_r <= ~v_sync_area_d2;
            vid_de_r    <= visible_d2;

`ifdef VGC_HW_MOTION_DIAG
            if (hwdiag_log_hit) begin
                vid_r_r <= 4'hF;
                vid_g_r <= 4'h4;
                vid_b_r <= 4'h0;
            end else if (hwdiag_phys_hit) begin
                vid_r_r <= 4'h0;
                vid_g_r <= 4'hC;
                vid_b_r <= 4'hF;
            end else begin
`endif
                vid_r_r <= dimmed_r;
                vid_g_r <= dimmed_g;
                vid_b_r <= dimmed_b;
`ifdef VGC_HW_MOTION_DIAG
            end
`endif
        end
    end

endmodule
