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
    output logic [7:0]  spr_a_dout,

    // --- Timing inputs ---
    input  logic [9:0]  h_count,
    input  logic [9:0]  v_count,
    input  logic        in_text_area_d2,
    input  logic [8:0]  gfx_x_d2,

    // --- Sprite attributes (packed: 16 entries per vector) ---
    // Sprite X/Y are signed 16-bit so firmware can place sprites
    // partly or fully off the left/top edges (negative coords).
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

    localparam H_ACTIVE  = 640;
    localparam V_ACTIVE  = 480;
    localparam GFX_W     = 320;
    localparam GFX_H     = 240;
    localparam NUM_SPRITES = 16;
    localparam SPR_W     = 16;
    localparam SPR_H     = 16;

    // Unpack flat vectors into local arrays for readability
    logic signed [15:0] spr_x [0:15];
    logic signed [15:0] spr_y [0:15];
    logic        spr_enable [0:15];
    logic        spr_flip_h [0:15];
    logic        spr_flip_v [0:15];
    logic [1:0]  spr_pri [0:15];
    logic [3:0]  spr_shape [0:15];
    logic [3:0]  spr_trans [0:15];

    always_comb begin
        for (int i = 0; i < 16; i++) begin
            spr_x[i]      = spr_x_flat[i*16 +: 16];
            spr_y[i]      = spr_y_flat[i*16 +: 16];
            spr_enable[i]  = spr_enable_flat[i];
            spr_flip_h[i]  = spr_flip_h_flat[i];
            spr_flip_v[i]  = spr_flip_v_flat[i];
            spr_pri[i]     = spr_pri_flat[i*2 +: 2];
            spr_shape[i]   = spr_shape_flat[i*4 +: 4];
            spr_trans[i]   = spr_trans_flat[i*4 +: 4];
        end
    end

    // =========================================================================
    // Memory — dpram instance
    // =========================================================================
    logic [10:0] spr_b_addr;
    logic [7:0]  spr_b_dout;

    dpram #(.WIDTH(8), .DEPTH(2048)) spr_mem (
        .clk(clk),
        .addr_a(spr_a_addr), .din_a(spr_a_din), .we_a(spr_a_we), .dout_a(spr_a_dout),
        .addr_b(spr_b_addr), .dout_b(spr_b_dout)
    );

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
    // cannot work reliably here: clear alone takes 320 cycles and hblank is
    // only 160 cycles at 640x480, so the old design modified the buffer while
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
    reg [6:0] slb_mem [0:1][0:GFX_W-1];

    initial begin
        slb_display_bank = 1'b0;
        slb_a_bank = 1'b1;
        for (int bank = 0; bank < 2; bank++)
            for (int i = 0; i < GFX_W; i++)
                slb_mem[bank][i] = 7'd0;
    end

    always_ff @(posedge clk) begin
        if (slb_a_we && slb_a_addr < GFX_W)
            slb_mem[slb_a_bank][slb_a_addr] <= slb_a_din;
        slb_b_dout <= (slb_b_addr < GFX_W) ? slb_mem[slb_display_bank][slb_b_addr] : 7'd0;
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
    logic [3:0]  spr_eval_idx;
    logic [8:0]  spr_clear_x;
    logic [2:0]  spr_read_byte;
    logic [7:0]  spr_row_data [0:7];
    logic [7:0]  spr_eval_y_line;
    logic        spr_eval_flip_h;
    logic        spr_eval_flip_v;
    logic [1:0]  spr_eval_pri_r;
    logic signed [15:0] spr_eval_x_r;
    logic [3:0]  spr_eval_shape_r;
    logic [3:0]  spr_eval_trans_r;
    logic [7:0]  spr_next_scanline;
    logic [4:0]  spr_write_px;

    wire        visible_line_start = (h_count == 10'd0) && (v_count < V_ACTIVE);
    wire [9:0]  spr_prep_v = (v_count == V_ACTIVE - 1) ? 10'd0 : (v_count + 10'd1);
    wire [7:0]  spr_prep_y = spr_prep_v[8:1];  // 640x480 native → 320x240 sprite plane

    initial begin
        spr_eval_state = SPR_IDLE;
        spr_eval_idx = 0;
        spr_clear_x = 0;
    end

    // =========================================================================
    // Scanline buffer read (port B, 1-cycle latency — address set in pipeline)
    // =========================================================================
    // gfx_x_d2 is the pixel coordinate at pipeline stage d2.
    // We need the read result available at d2, so drive address from gfx_x_d1
    // (one stage earlier). gfx_x_d1 = gfx_x_d2 will be on next cycle.
    // The render pipeline in vgc.sv provides gfx_x_d2; we derive d1 as d2's
    // next value: simply use (h_count[9:1] - border offset) but the simplest
    // correct approach is to drive slb_b_addr combinationally from gfx_x_d2
    // and accept that the read data (slb_rd_*) corresponds to the *previous*
    // cycle's address. The parent vgc.sv already accounts for dpram latency
    // by using the d2 pipeline stage for the address and d3 for the data.
    // So we just wire slb_b_addr = gfx_x_d2 and the outputs are read next cycle.
    assign slb_b_addr = gfx_x_d2;

    always_comb begin
        spr_pixel     = 4'h0;
        spr_pixel_pri = 2'd0;
        spr_pixel_hit = 0;
        collision_bg_bits = 8'h00;
        if (in_text_area_d2 && gfx_x_d2 < GFX_W) begin
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
            slb_display_bank <= 1'b0;
            slb_a_bank <= 1'b1;
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
                    if (spr_clear_x >= 319) begin
                        spr_eval_state <= SPR_CHECK;
                        spr_eval_idx <= 0;
                    end else
                        spr_clear_x <= spr_clear_x + 1;
                end
                SPR_CHECK: begin
                    if (spr_eval_idx < 16) begin
                        if (spr_enable[spr_eval_idx]) begin
                            // Signed compare: spr_y may be negative (partially
                            // off-screen sprite sliding in from the top).
                            if ($signed({8'b0, spr_next_scanline}) >= spr_y[spr_eval_idx] &&
                                $signed({8'b0, spr_next_scanline}) <  spr_y[spr_eval_idx] + 16'sd16) begin
                                spr_eval_y_line <= 8'($signed({8'b0, spr_next_scanline}) - spr_y[spr_eval_idx]);
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
                        // Signed screen_x so spr_eval_x_r < 0 clips naturally —
                        // the left-edge guard rejects negatives.
                        logic signed [15:0] screen_x;

                        fx = spr_eval_flip_h ? (4'd15 - spr_write_px[3:0]) : spr_write_px[3:0];
                        if (fx[0])
                            color = spr_row_data[fx[3:1]][3:0];
                        else
                            color = spr_row_data[fx[3:1]][7:4];

                        screen_x = spr_eval_x_r + {12'b0, spr_write_px[3:0]};
                        if (screen_x >= 0 && screen_x < 16'sd320 && color != spr_eval_trans_r) begin
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
                    spr_eval_state <= SPR_IDLE;
                end
                default: spr_eval_state <= SPR_IDLE;
            endcase
        end
    end

endmodule
