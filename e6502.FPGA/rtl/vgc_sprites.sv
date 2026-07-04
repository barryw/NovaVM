// VGC Sprites (PIXIE) — Sprite engine with sprite_shapes dpram
// Owns the sprite shape BRAM, scanline buffer, and hblank evaluator
// Sprite attribute registers and command handling remain in vgc.sv top
// Port A is exposed for command processor and blitter access
//
// Sprite attributes are passed as flat packed vectors for Yosys compatibility

module vgc_sprites (
    input  logic        clk,
    input  logic        rst,

    // --- sprite_shapes port A (from top-level mux) ---
    input  logic [14:0] spr_a_addr,
    input  logic [7:0]  spr_a_din,
    input  logic        spr_a_we,
    input  logic        spr_a_re,
    output logic [7:0]  spr_a_dout,

    // --- Timing inputs ---
    input  logic [9:0]  h_count,
    input  logic [9:0]  v_count,
    input  logic        sprite_frame_commit,
    input  logic        shape_publish_block,
    output logic        shape_sync_busy,
    input  logic        visible_d2,
    input  logic [8:0]  sprite_x_d2,

    // --- Sprite attributes (packed: 16 entries per vector) ---
    // Sprite X is an unsigned 16-bit logical coordinate. Sprite Y uses the
    // low byte of its packed register; the high byte is reserved/ignored.
    input  logic [16*16-1:0] spr_x_flat,
    input  logic [16*16-1:0] spr_y_flat,
    input  logic [15:0]      spr_enable_flat,
    input  logic [15:0]      spr_flip_h_flat,
    input  logic [15:0]      spr_flip_v_flat,
    input  logic [16*2-1:0]  spr_pri_flat,
    input  logic [16*8-1:0]  spr_shape_flat,
    input  logic [16*4-1:0]  spr_trans_flat,

    // --- Dedicated mouse cursor overlay inputs ---
    input  logic [8:0]       mouse_x,
    input  logic [7:0]       mouse_y,
    input  logic             mouse_enable,
    input  logic [7:0]       mouse_shape,
    input  logic [3:0]       mouse_hot_x,
    input  logic [3:0]       mouse_hot_y,

    // --- Scanline buffer read outputs (for compositing) ---
    output logic [3:0]  spr_pixel,
    output logic [1:0]  spr_pixel_pri,
    output logic        spr_pixel_hit,
    output logic [3:0]  spr_pixel_owner,
    output logic [3:0]  mouse_cursor_pixel,
    output logic        mouse_cursor_hit,

    // --- Collision detection outputs ---
    output logic [15:0] collision_ss_bits
);

`ifdef VIDEO_720X480
    localparam H_ACTIVE  = 720;
`else
    localparam H_ACTIVE  = 640;
`endif
    localparam V_ACTIVE  = 480;
    localparam SPR_PLANE_W = H_ACTIVE / 2;
    localparam SPR_PLANE_H = V_ACTIVE / 2;
    localparam NOVA_Y0   = 40;
    localparam NOVA_H    = 400;
    localparam NOVA_Y1   = NOVA_Y0 + NOVA_H;
    localparam NUM_SPRITES = 16;
    localparam SPR_W     = 16;
    localparam SPR_H     = 16;
    localparam SHAPE_RAM_SIZE = 32768;
    localparam logic [14:0] SHAPE_LAST_ADDR = SHAPE_RAM_SIZE - 1;

    // Unpack flat vectors into local arrays for readability
    logic [15:0] spr_x [0:15];
    logic [7:0]  spr_y [0:15];
    logic        spr_enable [0:15];
    logic        spr_flip_h [0:15];
    logic        spr_flip_v [0:15];
    logic [1:0]  spr_pri [0:15];
    logic [7:0]  spr_shape [0:15];
    logic [3:0]  spr_trans [0:15];

    always_comb begin
        for (int i = 0; i < 16; i++) begin
            spr_x[i]      = spr_x_flat[i*16 +: 16];
            spr_y[i]      = spr_y_flat[i*16 +: 8];
            spr_enable[i]  = spr_enable_flat[i];
            spr_flip_h[i]  = spr_flip_h_flat[i];
            spr_flip_v[i]  = spr_flip_v_flat[i];
            spr_pri[i]     = spr_pri_flat[i*2 +: 2];
            spr_shape[i]   = spr_shape_flat[i*8 +: 8];
            spr_trans[i]   = spr_trans_flat[i*4 +: 4];
        end
    end

    // =========================================================================
    // Sprite shape memory — active/pending banks
    // =========================================================================
    logic [14:0] spr_b_addr;
    logic [7:0]  spr_b_dout;

    logic        active_shape_bank;
    wire         pending_shape_bank = ~active_shape_bank;
    logic        shape_pending_dirty;

    localparam SHCOPY_IDLE    = 2'd0;
    localparam SHCOPY_READ    = 2'd1;
    localparam SHCOPY_CAPTURE = 2'd2;
    localparam SHCOPY_WRITE   = 2'd3;

    logic [1:0]  shape_copy_state;
    logic [14:0] shape_copy_addr;
    logic [14:0] shape_copy_addr_latched;
    logic [7:0]  shape_copy_data;
    logic        shape_reset_clearing;
    logic [14:0] shape_reset_addr;

    logic [14:0] spr0_a_addr, spr1_a_addr;
    logic [7:0]  spr0_a_din,  spr1_a_din;
    logic        spr0_a_we,   spr1_a_we;
    logic [7:0]  spr0_a_dout, spr1_a_dout;
    logic [14:0] spr0_b_addr, spr1_b_addr;
    logic [7:0]  spr0_b_dout, spr1_b_dout;

    dpram #(.WIDTH(8), .DEPTH(SHAPE_RAM_SIZE)) spr_mem0 (
        .clk(clk),
        .addr_a(spr0_a_addr), .din_a(spr0_a_din), .we_a(spr0_a_we), .dout_a(spr0_a_dout),
        .addr_b(spr0_b_addr), .dout_b(spr0_b_dout)
    );

    dpram #(.WIDTH(8), .DEPTH(SHAPE_RAM_SIZE)) spr_mem1 (
        .clk(clk),
        .addr_a(spr1_a_addr), .din_a(spr1_a_din), .we_a(spr1_a_we), .dout_a(spr1_a_dout),
        .addr_b(spr1_b_addr), .dout_b(spr1_b_dout)
    );

    wire [7:0] active_shape_a_dout  = active_shape_bank ? spr1_a_dout : spr0_a_dout;
    wire [7:0] active_shape_b_dout  = active_shape_bank ? spr1_b_dout : spr0_b_dout;

    wire user_shape_access = spr_a_we || spr_a_re;
    wire shape_publish_fire = sprite_frame_commit && shape_pending_dirty &&
                              !shape_sync_busy && !shape_publish_block &&
                              !user_shape_access;
    wire spr_a_read_bank = (!shape_sync_busy || (spr_a_addr < shape_copy_addr))
                           ? pending_shape_bank : active_shape_bank;
    wire copy_write_fire = shape_sync_busy &&
                           shape_copy_state == SHCOPY_WRITE &&
                           !user_shape_access;

    logic spr_a_read_bank_d;

    assign spr_a_dout = spr_a_read_bank_d ? spr1_a_dout : spr0_a_dout;

    always_comb begin
        spr0_a_addr = 15'd0; spr0_a_din = 8'd0; spr0_a_we = 1'b0;
        spr1_a_addr = 15'd0; spr1_a_din = 8'd0; spr1_a_we = 1'b0;
        spr0_b_addr = active_shape_bank ? 15'd0 : spr_b_addr;
        spr1_b_addr = active_shape_bank ? spr_b_addr : 15'd0;

        if (shape_reset_clearing) begin
            spr0_a_addr = shape_reset_addr;
            spr0_a_din  = 8'd0;
            spr0_a_we   = 1'b1;
            spr1_a_addr = shape_reset_addr;
            spr1_a_din  = 8'd0;
            spr1_a_we   = 1'b1;
        end else if (user_shape_access) begin
            // Read both banks so RMW paths can select the coherent source after
            // one-cycle BRAM latency. Writes always target the pending bank.
            spr0_a_addr = spr_a_addr;
            spr1_a_addr = spr_a_addr;
            if (spr_a_we) begin
                if (!pending_shape_bank || shape_sync_busy) begin
                    spr0_a_din = spr_a_din;
                    spr0_a_we  = 1'b1;
                end
                if (pending_shape_bank || shape_sync_busy) begin
                    spr1_a_din = spr_a_din;
                    spr1_a_we  = 1'b1;
                end
            end
        end else if (shape_sync_busy) begin
            case (shape_copy_state)
                SHCOPY_READ: begin
                    if (active_shape_bank)
                        spr1_a_addr = shape_copy_addr;
                    else
                        spr0_a_addr = shape_copy_addr;
                end
                SHCOPY_WRITE: begin
                    if (pending_shape_bank) begin
                        spr1_a_addr = shape_copy_addr_latched;
                        spr1_a_din  = shape_copy_data;
                        spr1_a_we   = copy_write_fire;
                    end else begin
                        spr0_a_addr = shape_copy_addr_latched;
                        spr0_a_din  = shape_copy_data;
                        spr0_a_we   = copy_write_fire;
                    end
                end
                default: ;
            endcase
        end
    end

    assign spr_b_dout = active_shape_b_dout;

    always_ff @(posedge clk) begin
        if (rst) begin
            active_shape_bank <= 1'b0;
            shape_pending_dirty <= 1'b0;
            shape_sync_busy <= 1'b0;
            shape_copy_state <= SHCOPY_IDLE;
            shape_copy_addr <= 15'd0;
            shape_copy_addr_latched <= 15'd0;
            shape_copy_data <= 8'd0;
            spr_a_read_bank_d <= 1'b0;
            shape_reset_clearing <= 1'b1;
            shape_reset_addr <= 15'd0;
        end else begin
            if (shape_reset_clearing) begin
                shape_pending_dirty <= 1'b0;
                shape_sync_busy <= 1'b0;
                shape_copy_state <= SHCOPY_IDLE;
                if (shape_reset_addr == SHAPE_LAST_ADDR)
                    shape_reset_clearing <= 1'b0;
                else
                    shape_reset_addr <= shape_reset_addr + 15'd1;
            end else if (spr_a_re)
                spr_a_read_bank_d <= spr_a_read_bank;

            if (!shape_reset_clearing && shape_publish_fire) begin
                active_shape_bank <= pending_shape_bank;
                shape_pending_dirty <= spr_a_we;
                shape_sync_busy <= 1'b1;
                shape_copy_state <= SHCOPY_READ;
                shape_copy_addr <= 15'd0;
                shape_copy_addr_latched <= 15'd0;
            end else if (!shape_reset_clearing && spr_a_we) begin
                shape_pending_dirty <= 1'b1;
            end

            if (!shape_reset_clearing && !shape_publish_fire && shape_sync_busy) begin
                case (shape_copy_state)
                    SHCOPY_READ: begin
                        if (!user_shape_access)
                            shape_copy_state <= SHCOPY_CAPTURE;
                    end
                    SHCOPY_CAPTURE: begin
                        shape_copy_addr_latched <= shape_copy_addr;
                        shape_copy_data <= active_shape_a_dout;
                        shape_copy_state <= SHCOPY_WRITE;
                    end
                    SHCOPY_WRITE: begin
                        if (!user_shape_access) begin
                            if (shape_copy_addr == SHAPE_LAST_ADDR) begin
                                shape_sync_busy <= 1'b0;
                                shape_copy_state <= SHCOPY_IDLE;
                            end else begin
                                shape_copy_addr <= shape_copy_addr + 15'd1;
                                shape_copy_state <= SHCOPY_READ;
                            end
                        end
                    end
                    default: shape_copy_state <= SHCOPY_READ;
                endcase
            end
        end
    end

    // =========================================================================
    // Sprite scanline buffers — ping-ponged BRAM
    //
    // One bank is read by compositing while the other is cleared/filled for the
    // next native VGA line. The dpram wrapper forces DP16KD mapping so the small
    // line buffer does not become distributed RAM with wide write muxes.
    // =========================================================================
    localparam SLB_DEPTH = 1024; // 2 banks x active-video sprite line buffer

    logic [8:0]  slb_a_addr;
    logic [10:0] slb_a_din;
    logic        slb_a_we;
    logic        slb_a_bank;
    logic [8:0]  slb_b_addr;
    logic [10:0] slb_b_dout;
    logic [10:0] slb_b_raw;
    logic [10:0] slb_a_dout_unused;
    logic        slb_display_bank;
    logic        slb_b_valid_d;

    wire [9:0] slb_a_ram_addr = {slb_a_bank, slb_a_addr};
    wire [9:0] slb_b_ram_addr = {slb_display_bank, slb_b_addr};
    wire       slb_a_we_safe  = slb_a_we && (slb_a_addr < SPR_PLANE_W);

    dpram #(.WIDTH(11), .DEPTH(SLB_DEPTH)) slb_ram (
        .clk(clk),
        .addr_a(slb_a_ram_addr),
        .din_a(slb_a_din),
        .we_a(slb_a_we_safe),
        .dout_a(slb_a_dout_unused),
        .addr_b(slb_b_ram_addr),
        .dout_b(slb_b_raw)
    );

    always_ff @(posedge clk) begin
        slb_b_valid_d <= (slb_b_addr < SPR_PLANE_W);
    end

    assign slb_b_dout = slb_b_valid_d ? slb_b_raw : 7'd0;

    // Unpack port B read for rendering
    wire [3:0] slb_rd_color = slb_b_dout[4:1];
    wire       slb_rd_valid = slb_b_dout[0];
    wire [3:0] slb_rd_owner = slb_b_dout[8:5];
    wire [1:0] slb_rd_pri   = slb_b_dout[10:9];
    logic [4:0] slb_owner [0:SPR_PLANE_W-1]; // {valid, sprite index}

    // Sprite evaluation state machine
    localparam SPR_IDLE         = 4'd0;
    localparam SPR_CLEAR        = 4'd1;
    localparam SPR_CHECK        = 4'd2;
    localparam SPR_READ         = 4'd3;
    localparam SPR_DECODE       = 4'd4;
    localparam SPR_DONE         = 4'd5;
    localparam SPR_WRITE        = 4'd6;
    localparam SPR_MOUSE_READ   = 4'd7;
    localparam SPR_MOUSE_DECODE = 4'd8;

    logic [3:0]  spr_eval_state;
    logic [4:0]  spr_eval_idx;
    logic [8:0]  spr_clear_x;
    logic [2:0]  spr_read_byte;
    logic [7:0]  spr_row_data [0:7];
    logic [7:0]  spr_eval_y_line;
    logic        spr_eval_flip_h;
    logic        spr_eval_flip_v;
    logic [1:0]  spr_eval_pri_r;
    logic [15:0] spr_eval_x_r;
    logic [7:0]  spr_eval_shape_r;
    logic [3:0]  spr_eval_trans_r;
    logic [7:0]  spr_next_scanline;
    logic        spr_next_line_in_canvas;
    logic [4:0]  spr_write_px;
    logic        scanline_output_valid;
    logic        scanline_ready;
    logic        mouse_line_active;
    logic signed [10:0] mouse_line_left;
    logic [7:0]  mouse_row_data [0:7];
    logic        mouse_next_line_active;
    logic signed [10:0] mouse_next_line_left;
    logic [7:0]  mouse_next_shape;
    logic [3:0]  mouse_next_row;
    logic [2:0]  mouse_read_byte;

    wire        visible_line_start = (h_count == 10'd0) && (v_count < V_ACTIVE);
    wire [9:0]  spr_prep_v = (v_count == V_ACTIVE - 1) ? 10'd0 : (v_count + 10'd1);
    wire        spr_prep_in_canvas = (spr_prep_v >= NOVA_Y0) && (spr_prep_v < NOVA_Y1);
    wire [9:0]  spr_prep_canvas_y = spr_prep_v - NOVA_Y0;
    wire [7:0]  spr_prep_y = spr_prep_canvas_y[8:1];  // 400 canvas lines -> 200 sprite rows

    initial begin
        spr_eval_state = SPR_IDLE;
        spr_eval_idx = 0;
        spr_clear_x = 0;
        spr_read_byte = 0;
        spr_eval_y_line = 0;
        spr_eval_flip_h = 0;
        spr_eval_flip_v = 0;
        spr_eval_pri_r = 0;
        spr_eval_x_r = 0;
        spr_eval_shape_r = 0;
        spr_eval_trans_r = 0;
        spr_next_scanline = 0;
        spr_next_line_in_canvas = 0;
        spr_write_px = 0;
        slb_a_addr = 0;
        slb_a_din = 7'd0;
        slb_a_we = 0;
        slb_b_valid_d = 1'b0;
        slb_display_bank = 1'b0;
        slb_a_bank = 1'b1;
        scanline_output_valid = 1'b0;
        scanline_ready = 1'b0;
        mouse_line_active = 1'b0;
        mouse_line_left = 11'sd0;
        mouse_next_line_active = 1'b0;
        mouse_next_line_left = 11'sd0;
        mouse_next_shape = 8'd255;
        mouse_next_row = 4'd0;
        mouse_read_byte = 3'd0;
        collision_ss_bits = 16'h0000;
        for (int i = 0; i < 8; i++)
            spr_row_data[i] = 8'h00;
        for (int i = 0; i < 8; i++)
            mouse_row_data[i] = 8'h00;
        for (int i = 0; i < SPR_PLANE_W; i++)
            slb_owner[i] = 5'd0;
    end

    // Shape BRAM reads are synchronous, but the address itself must be stable
    // before the clock edge. Keeping this combinational lets the existing
    // one-byte-per-cycle read pipeline capture bytes in order.
    always_comb begin
        logic [3:0] fy;

        fy = spr_eval_flip_v ? (4'd15 - spr_eval_y_line[3:0]) : spr_eval_y_line[3:0];
        if (spr_eval_state == SPR_READ)
            spr_b_addr = {spr_eval_shape_r, fy, spr_read_byte};
        else if (spr_eval_state == SPR_MOUSE_READ)
            spr_b_addr = {mouse_next_shape, mouse_next_row, mouse_read_byte};
        else
            spr_b_addr = 15'd0;
    end

    // =========================================================================
    // Scanline buffer read (port B, 1-cycle latency — address set in pipeline)
    // =========================================================================
    // sprite_x_d2 is the Nova canvas sprite coordinate at pipeline d2. The
    // parent VGC masks sprite pixels outside the canvas, so sprites never draw
    // into the hardware border.
    assign slb_b_addr = sprite_x_d2;

    always_comb begin
        spr_pixel     = 4'h0;
        spr_pixel_pri = 2'd0;
        spr_pixel_hit = 0;
        spr_pixel_owner = 4'd0;
        mouse_cursor_pixel = 4'h0;
        mouse_cursor_hit = 1'b0;
        if (scanline_output_valid && visible_d2 && sprite_x_d2 < SPR_PLANE_W) begin
            if (slb_rd_valid) begin
                spr_pixel     = slb_rd_color;
                spr_pixel_pri = slb_rd_pri;
                spr_pixel_owner = slb_rd_owner;
                spr_pixel_hit = 1;
            end
            if (mouse_line_active) begin
                logic signed [10:0] local_x_s;
                logic [3:0] local_x;
                logic [3:0] raw;

                local_x_s = $signed({2'b00, sprite_x_d2}) - mouse_line_left;
                if (local_x_s >= 11'sd0 && local_x_s < 11'sd16) begin
                    local_x = local_x_s[3:0];
                    raw = local_x[0] ? mouse_row_data[local_x[3:1]][3:0] :
                                       mouse_row_data[local_x[3:1]][7:4];
                    if (raw != 4'h0) begin
                        mouse_cursor_pixel = raw;
                        mouse_cursor_hit = 1'b1;
                    end
                end
            end
        end
    end

    // =========================================================================
    // Sprite scanline evaluation — during hblank
    // =========================================================================
    always_ff @(posedge clk) begin
        slb_a_we <= 0;  // default: no write unless a state asserts it
        collision_ss_bits <= 16'h0000;
        if (rst) begin
            spr_eval_state <= SPR_IDLE;
            spr_eval_idx <= 0;
            spr_clear_x <= 0;
            spr_read_byte <= 0;
            spr_eval_y_line <= 0;
            spr_eval_flip_h <= 0;
            spr_eval_flip_v <= 0;
            spr_eval_pri_r <= 0;
            spr_eval_x_r <= 0;
            spr_eval_shape_r <= 0;
            spr_eval_trans_r <= 0;
            spr_next_scanline <= 0;
            spr_next_line_in_canvas <= 0;
            spr_write_px <= 0;
            slb_a_addr <= 0;
            slb_a_din <= 11'd0;
            slb_a_we <= 0;
            collision_ss_bits <= 16'h0000;
            slb_display_bank <= 1'b0;
            slb_a_bank <= 1'b1;
            scanline_output_valid <= 1'b0;
            scanline_ready <= 1'b0;
            mouse_line_active <= 1'b0;
            mouse_line_left <= 11'sd0;
            mouse_next_line_active <= 1'b0;
            mouse_next_line_left <= 11'sd0;
            mouse_next_shape <= 8'd255;
            mouse_next_row <= 4'd0;
            mouse_read_byte <= 3'd0;
            for (int i = 0; i < 8; i++)
                spr_row_data[i] <= 8'h00;
            for (int i = 0; i < 8; i++)
                mouse_row_data[i] <= 8'h00;
        end else begin
            if (visible_line_start) begin
                logic signed [10:0] prep_y_s;
                logic signed [10:0] mouse_top_y_s;
                logic signed [10:0] mouse_left_s;
                logic signed [10:0] mouse_row_s;

                // Swap in the bank prepared during the previous visible line,
                // then reuse the old display bank as the next prep target.
                slb_display_bank <= ~slb_display_bank;
                slb_a_bank <= slb_display_bank;
                scanline_output_valid <= scanline_ready;
                scanline_ready <= 1'b0;
                mouse_line_active <= mouse_next_line_active;
                mouse_line_left <= mouse_next_line_left;
                spr_eval_state <= SPR_CLEAR;
                spr_clear_x <= 0;
                spr_eval_idx <= 0;
                spr_next_scanline <= spr_prep_y;
                spr_next_line_in_canvas <= spr_prep_in_canvas;

                prep_y_s = $signed({3'b000, spr_prep_y});
                mouse_top_y_s = $signed({3'b000, mouse_y}) - $signed({7'b0000000, mouse_hot_y});
                mouse_left_s = $signed({2'b00, mouse_x}) - $signed({7'b0000000, mouse_hot_x});
                mouse_row_s = prep_y_s - mouse_top_y_s;
                mouse_next_line_active <= mouse_enable && spr_prep_in_canvas &&
                                          prep_y_s >= mouse_top_y_s &&
                                          prep_y_s < (mouse_top_y_s + 11'sd16);
                mouse_next_line_left <= mouse_left_s;
                mouse_next_shape <= mouse_shape;
                mouse_next_row <= mouse_row_s[3:0];
            end else case (spr_eval_state)
                SPR_IDLE: begin
                // Work is started at visible_line_start so preparation has
                // a whole native line (800 cycles), not just hblank.
                end
                SPR_CLEAR: begin
                    slb_a_addr <= spr_clear_x;
                    slb_a_din  <= 11'd0;
                    slb_a_we   <= 1;
                    slb_owner[spr_clear_x] <= 5'd0;
                    if (spr_clear_x >= SPR_PLANE_W - 1) begin
                        spr_eval_state <= spr_next_line_in_canvas ? SPR_CHECK : SPR_DONE;
                        spr_eval_idx <= 0;
                    end else
                        spr_clear_x <= spr_clear_x + 1;
                end
                SPR_CHECK: begin
                    if (spr_eval_idx < 16) begin
                        if (spr_enable[spr_eval_idx]) begin
                            begin
                                logic [8:0] scanline_y;
                                logic [8:0] sprite_y;
                                scanline_y = {1'b0, spr_next_scanline};
                                sprite_y = {1'b0, spr_y[spr_eval_idx]};
                                if (scanline_y >= sprite_y &&
                                    scanline_y <  sprite_y + 9'd16) begin
                                    spr_eval_y_line <= 8'(scanline_y - sprite_y);
                                    spr_eval_flip_h <= spr_flip_h[spr_eval_idx];
                                    spr_eval_flip_v <= spr_flip_v[spr_eval_idx];
                                    spr_eval_pri_r <= spr_pri[spr_eval_idx];
                                    spr_eval_x_r <= spr_x[spr_eval_idx];
                                    spr_eval_shape_r <= spr_shape[spr_eval_idx];
                                    spr_eval_trans_r <= spr_trans[spr_eval_idx];
                                    spr_read_byte <= 0;
                                    spr_eval_state <= SPR_READ;
                                end else begin
                                    spr_eval_idx <= spr_eval_idx + 1;
                                end
                            end
                        end else begin
                            spr_eval_idx <= spr_eval_idx + 1;
                        end
                    end else begin
                        spr_eval_state <= SPR_DONE;
                    end
                end
                SPR_READ: begin
                    if (spr_read_byte > 0) begin
                        spr_row_data[spr_read_byte - 1] <= spr_b_dout;
                    end
                    if (spr_read_byte == 7) begin
                        spr_eval_state <= SPR_DECODE;
                    end
                    spr_read_byte <= spr_read_byte + 1;
                end
                SPR_DECODE: begin
                    // Capture last byte from sprite shape read
                    spr_row_data[7] <= spr_b_dout;
                    spr_write_px <= 0;
                    spr_eval_state <= SPR_WRITE;
                end
                SPR_WRITE: begin
                    begin
                        logic [3:0] fx;
                        logic [3:0] color;
                        logic [16:0] screen_x;

                        fx = spr_eval_flip_h ? (4'd15 - spr_write_px[3:0]) : spr_write_px[3:0];
                        if (fx[0])
                            color = spr_row_data[fx[3:1]][3:0];
                        else
                            color = spr_row_data[fx[3:1]][7:4];

                        screen_x = {1'b0, spr_eval_x_r} + {13'b0, spr_write_px[3:0]};
                        if (screen_x < SPR_PLANE_W && color != spr_eval_trans_r) begin
                            slb_a_addr <= screen_x[8:0];
                            slb_a_din  <= {spr_eval_pri_r, spr_eval_idx[3:0], color, 1'b1};
                            slb_a_we   <= 1;
                            if (slb_owner[screen_x[8:0]][4] &&
                                slb_owner[screen_x[8:0]][3:0] != spr_eval_idx[3:0]) begin
                                collision_ss_bits[slb_owner[screen_x[8:0]][3:0]] <= 1'b1;
                                collision_ss_bits[spr_eval_idx[3:0]] <= 1'b1;
                            end
                            slb_owner[screen_x[8:0]] <= {1'b1, spr_eval_idx[3:0]};
                        end
                    end
                    if (spr_write_px == 15) begin
                        spr_eval_idx <= spr_eval_idx + 1;
                        spr_eval_state <= SPR_CHECK;
                    end else
                        spr_write_px <= spr_write_px + 1;
                end
                SPR_DONE: begin
                    if (mouse_next_line_active) begin
                        mouse_read_byte <= 3'd0;
                        spr_eval_state <= SPR_MOUSE_READ;
                    end else begin
                        scanline_ready <= 1'b1;
                        spr_eval_state <= SPR_IDLE;
                    end
                end
                SPR_MOUSE_READ: begin
                    if (mouse_read_byte > 0)
                        mouse_row_data[mouse_read_byte - 1] <= spr_b_dout;
                    if (mouse_read_byte == 7)
                        spr_eval_state <= SPR_MOUSE_DECODE;
                    mouse_read_byte <= mouse_read_byte + 3'd1;
                end
                SPR_MOUSE_DECODE: begin
                    mouse_row_data[7] <= spr_b_dout;
                    scanline_ready <= 1'b1;
                    spr_eval_state <= SPR_IDLE;
                end
                default: spr_eval_state <= SPR_IDLE;
            endcase
        end
    end

endmodule
