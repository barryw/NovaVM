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
    input  logic [10:0] spr_a_addr,
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
    input  logic [16*4-1:0]  spr_shape_flat,
    input  logic [16*4-1:0]  spr_trans_flat,

    // --- Scanline buffer read outputs (for compositing) ---
    output logic [3:0]  spr_pixel,
    output logic [1:0]  spr_pixel_pri,
    output logic        spr_pixel_hit,

    // --- Collision detection outputs ---
    output logic [7:0]  collision_bg_bits
);

`ifdef VIDEO_720X480
    localparam H_ACTIVE  = 720;
`else
    localparam H_ACTIVE  = 640;
`endif
    localparam V_ACTIVE  = 480;
    localparam SPR_PLANE_W = H_ACTIVE / 2;
    localparam SPR_PLANE_H = V_ACTIVE / 2;
    localparam NUM_SPRITES = 16;
    localparam SPR_W     = 16;
    localparam SPR_H     = 16;

    // Unpack flat vectors into local arrays for readability
    logic [15:0] spr_x [0:15];
    logic [7:0]  spr_y [0:15];
    logic        spr_enable [0:15];
    logic        spr_flip_h [0:15];
    logic        spr_flip_v [0:15];
    logic [1:0]  spr_pri [0:15];
    logic [3:0]  spr_shape [0:15];
    logic [3:0]  spr_trans [0:15];

    always_comb begin
        for (int i = 0; i < 16; i++) begin
            spr_x[i]      = spr_x_flat[i*16 +: 16];
            spr_y[i]      = spr_y_flat[i*16 +: 8];
            spr_enable[i]  = spr_enable_flat[i];
            spr_flip_h[i]  = spr_flip_h_flat[i];
            spr_flip_v[i]  = spr_flip_v_flat[i];
            spr_pri[i]     = spr_pri_flat[i*2 +: 2];
            spr_shape[i]   = spr_shape_flat[i*4 +: 4];
            spr_trans[i]   = spr_trans_flat[i*4 +: 4];
        end
    end

    // =========================================================================
    // Sprite shape memory — active/pending banks
    // =========================================================================
    logic [10:0] spr_b_addr;
    logic [7:0]  spr_b_dout;

    logic        active_shape_bank;
    wire         pending_shape_bank = ~active_shape_bank;
    logic        shape_pending_dirty;

    localparam SHCOPY_IDLE    = 2'd0;
    localparam SHCOPY_READ    = 2'd1;
    localparam SHCOPY_CAPTURE = 2'd2;
    localparam SHCOPY_WRITE   = 2'd3;

    logic [1:0]  shape_copy_state;
    logic [10:0] shape_copy_addr;
    logic [10:0] shape_copy_addr_latched;
    logic [7:0]  shape_copy_data;
    logic [2047:0] shape_copy_dirty;
    logic        shape_reset_clearing;
    logic [10:0] shape_reset_addr;

    logic [10:0] spr0_a_addr, spr1_a_addr;
    logic [7:0]  spr0_a_din,  spr1_a_din;
    logic        spr0_a_we,   spr1_a_we;
    logic [7:0]  spr0_a_dout, spr1_a_dout;
    logic [10:0] spr0_b_addr, spr1_b_addr;
    logic [7:0]  spr0_b_dout, spr1_b_dout;

    dpram #(.WIDTH(8), .DEPTH(2048)) spr_mem0 (
        .clk(clk),
        .addr_a(spr0_a_addr), .din_a(spr0_a_din), .we_a(spr0_a_we), .dout_a(spr0_a_dout),
        .addr_b(spr0_b_addr), .dout_b(spr0_b_dout)
    );

    dpram #(.WIDTH(8), .DEPTH(2048)) spr_mem1 (
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
    wire spr_a_addr_synced = !shape_sync_busy || shape_copy_dirty[spr_a_addr] ||
                             (spr_a_addr < shape_copy_addr);
    wire spr_a_read_uses_pending = spr_a_addr_synced;
    wire spr_a_read_bank = spr_a_read_uses_pending ? pending_shape_bank : active_shape_bank;
    wire copy_write_fire = shape_sync_busy &&
                           shape_copy_state == SHCOPY_WRITE &&
                           !user_shape_access &&
                           !shape_copy_dirty[shape_copy_addr_latched];

    logic spr_a_read_bank_d;

    assign spr_a_dout = spr_a_read_bank_d ? spr1_a_dout : spr0_a_dout;

    always_comb begin
        spr0_a_addr = 11'd0; spr0_a_din = 8'd0; spr0_a_we = 1'b0;
        spr1_a_addr = 11'd0; spr1_a_din = 8'd0; spr1_a_we = 1'b0;
        spr0_b_addr = active_shape_bank ? 11'd0 : spr_b_addr;
        spr1_b_addr = active_shape_bank ? spr_b_addr : 11'd0;

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
                if (pending_shape_bank) begin
                    spr1_a_din = spr_a_din;
                    spr1_a_we  = 1'b1;
                end else begin
                    spr0_a_din = spr_a_din;
                    spr0_a_we  = 1'b1;
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
            shape_copy_addr <= 11'd0;
            shape_copy_addr_latched <= 11'd0;
            shape_copy_data <= 8'd0;
            shape_copy_dirty <= '0;
            spr_a_read_bank_d <= 1'b0;
            shape_reset_clearing <= 1'b1;
            shape_reset_addr <= 11'd0;
        end else begin
            if (shape_reset_clearing) begin
                shape_pending_dirty <= 1'b0;
                shape_sync_busy <= 1'b0;
                shape_copy_state <= SHCOPY_IDLE;
                shape_copy_dirty <= '0;
                if (shape_reset_addr == 11'd2047)
                    shape_reset_clearing <= 1'b0;
                else
                    shape_reset_addr <= shape_reset_addr + 11'd1;
            end else if (spr_a_re)
                spr_a_read_bank_d <= spr_a_read_bank;

            if (!shape_reset_clearing && shape_publish_fire) begin
                active_shape_bank <= pending_shape_bank;
                shape_pending_dirty <= spr_a_we;
                shape_sync_busy <= 1'b1;
                shape_copy_state <= SHCOPY_READ;
                shape_copy_addr <= 11'd0;
                shape_copy_addr_latched <= 11'd0;
                shape_copy_dirty <= '0;
            end else if (!shape_reset_clearing && spr_a_we) begin
                shape_pending_dirty <= 1'b1;
            end

            if (!shape_reset_clearing && spr_a_we && shape_sync_busy)
                shape_copy_dirty[spr_a_addr] <= 1'b1;

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
                            if (shape_copy_addr == 11'd2047) begin
                                shape_sync_busy <= 1'b0;
                                shape_copy_state <= SHCOPY_IDLE;
                            end else begin
                                shape_copy_addr <= shape_copy_addr + 11'd1;
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
    // Sprite scanline buffers — EXPLICIT FF ARRAYS (not memory inference)
    //
    // History 2026-04-28/29: Three attempts to force yosys to map this as
    // DP16KD BRAM all failed — the dpram module, ram_style="block" attribute,
    // -nolutram synth flag, and inline 512-deep pattern all produced
    // distributed LUT4-D cells with a cascade-write bug that silently dropped
    // SPR_CLEAR writes, leaving stale sprite data in the buffer forever. The
    // visible symptom was a phantom-sprite vertical bar at every previously
    // written sprite-X position, persisting across cold_starts.
    //
    // Resolution: synthesize as raw FFs. ram_style="registers" hint plus an
    // explicit per-FF write-decode pattern bypasses memory_libmap entirely.
    //
    // The buffers are ping-ponged. One bank is read by compositing while the
    // other is cleared/filled for the next native VGA line. A single buffer
    // cannot work reliably here: clear alone takes the full sprite-plane
    // width (320 or 360 cycles), so the old design modified the buffer while
    // the next visible line was already reading it. That matches the observed
    // "solid 16-pixel gray bar" artifact when stale sprite pixels survive.
    // =========================================================================
    logic [8:0]  slb_a_addr;
    logic [6:0]  slb_a_din;
    logic        slb_a_we;
    logic        slb_a_bank;
    logic [8:0]  slb_b_addr;
    logic [6:0]  slb_b_dout;
    logic        slb_display_bank;

    (* ram_style = "registers" *)
    reg [6:0] slb_mem [0:1][0:SPR_PLANE_W-1];

    initial begin
        slb_display_bank = 1'b0;
        slb_a_bank = 1'b1;
        for (int bank = 0; bank < 2; bank++)
            for (int i = 0; i < SPR_PLANE_W; i++)
                slb_mem[bank][i] = 7'd0;
    end

    always_ff @(posedge clk) begin
        if (slb_a_we && slb_a_addr < SPR_PLANE_W)
            slb_mem[slb_a_bank][slb_a_addr] <= slb_a_din;
        slb_b_dout <= (slb_b_addr < SPR_PLANE_W) ? slb_mem[slb_display_bank][slb_b_addr] : 7'd0;
    end

    // Unpack port B read for rendering
    wire [3:0] slb_rd_color = slb_b_dout[4:1];
    wire       slb_rd_valid = slb_b_dout[0];
    wire [1:0] slb_rd_pri   = slb_b_dout[6:5];

    // Sprite evaluation state machine
    localparam SPR_IDLE   = 3'd0;
    localparam SPR_CLEAR  = 3'd1;
    localparam SPR_CHECK  = 3'd2;
    localparam SPR_READ   = 3'd3;
    localparam SPR_DECODE = 3'd4;
    localparam SPR_DONE   = 3'd5;
    localparam SPR_WRITE  = 3'd6;

    logic [2:0]  spr_eval_state;
    logic [4:0]  spr_eval_idx;
    logic [8:0]  spr_clear_x;
    logic [2:0]  spr_read_byte;
    logic [7:0]  spr_row_data [0:7];
    logic [7:0]  spr_eval_y_line;
    logic        spr_eval_flip_h;
    logic        spr_eval_flip_v;
    logic [1:0]  spr_eval_pri_r;
    logic [15:0] spr_eval_x_r;
    logic [3:0]  spr_eval_shape_r;
    logic [3:0]  spr_eval_trans_r;
    logic [7:0]  spr_next_scanline;
    logic [4:0]  spr_write_px;
    logic        scanline_output_valid;

    wire        visible_line_start = (h_count == 10'd0) && (v_count < V_ACTIVE);
    wire [9:0]  spr_prep_v = (v_count == V_ACTIVE - 1) ? 10'd0 : (v_count + 10'd1);
    wire [7:0]  spr_prep_y = spr_prep_v[8:1];  // 480 native lines -> 240 sprite rows

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
        spr_write_px = 0;
        slb_a_addr = 0;
        slb_a_din = 7'd0;
        slb_a_we = 0;
        scanline_output_valid = 1'b0;
        for (int i = 0; i < 8; i++)
            spr_row_data[i] = 8'h00;
    end

    // =========================================================================
    // Scanline buffer read (port B, 1-cycle latency — address set in pipeline)
    // =========================================================================
    // sprite_x_d2 is the full active-video sprite coordinate at pipeline d2.
    // We keep the same 1-cycle line-buffer read behavior as the old canvas
    // path. The parent VGC pipeline already delays the active-video flags.
    // So we just wire slb_b_addr = sprite_x_d2 and the outputs are read next cycle.
    assign slb_b_addr = sprite_x_d2;

    always_comb begin
        spr_pixel     = 4'h0;
        spr_pixel_pri = 2'd0;
        spr_pixel_hit = 0;
        collision_bg_bits = 8'h00;
        if (scanline_output_valid && visible_d2 && sprite_x_d2 < SPR_PLANE_W) begin
            if (slb_rd_valid) begin
                spr_pixel     = slb_rd_color;
                spr_pixel_pri = slb_rd_pri;
                spr_pixel_hit = 1;
            end
        end
    end

    // =========================================================================
    // Sprite scanline evaluation — during hblank
    // =========================================================================
    always_ff @(posedge clk) begin
        slb_a_we <= 0;  // default: no write unless a state asserts it
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
            spr_write_px <= 0;
            slb_a_addr <= 0;
            slb_a_din <= 7'd0;
            slb_a_we <= 0;
            slb_display_bank <= 1'b0;
            slb_a_bank <= 1'b1;
            scanline_output_valid <= 1'b0;
            for (int bank = 0; bank < 2; bank++)
                for (int i = 0; i < SPR_PLANE_W; i++)
                    slb_mem[bank][i] <= 7'd0;
            for (int i = 0; i < 8; i++)
                spr_row_data[i] <= 8'h00;
        end else begin
            if (visible_line_start) begin
                // Swap in the bank prepared during the previous visible line,
                // then reuse the old display bank as the next prep target.
                slb_display_bank <= ~slb_display_bank;
                slb_a_bank <= slb_display_bank;
                spr_eval_state <= SPR_CLEAR;
                spr_clear_x <= 0;
                spr_eval_idx <= 0;
                spr_next_scanline <= spr_prep_y;
            end else case (spr_eval_state)
                SPR_IDLE: begin
                    // Work is started at visible_line_start so preparation has
                    // a whole native line (800 cycles), not just hblank.
                end
                SPR_CLEAR: begin
                    slb_a_addr <= spr_clear_x;
                    slb_a_din  <= 7'd0;
                    slb_a_we   <= 1;
                    if (spr_clear_x >= SPR_PLANE_W - 1) begin
                        spr_eval_state <= SPR_CHECK;
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
                    begin
                        logic [3:0] fy;
                        fy = spr_eval_flip_v ? (4'd15 - spr_eval_y_line[3:0]) : spr_eval_y_line[3:0];
                        spr_b_addr <= {spr_eval_shape_r, fy, spr_read_byte};
                    end
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
                            slb_a_din  <= {spr_eval_pri_r, color, 1'b1};
                            slb_a_we   <= 1;
                        end
                    end
                    if (spr_write_px == 15) begin
                        spr_eval_idx <= spr_eval_idx + 1;
                        spr_eval_state <= SPR_CHECK;
                    end else
                        spr_write_px <= spr_write_px + 1;
                end
                SPR_DONE: begin
                    scanline_output_valid <= 1'b1;
                    spr_eval_state <= SPR_IDLE;
                end
                default: spr_eval_state <= SPR_IDLE;
            endcase
        end
    end

endmodule
