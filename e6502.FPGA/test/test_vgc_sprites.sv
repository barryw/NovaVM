// Sprite-layer RTL tests for the VGC — covers the sprite command surface:
// SPRCLR, SPRROW, SPRPOS, SPRENA/DIS, SPRFLIP, SPRPRI, SPRCOPY, and OOB
// handling. Renders are validated at the state level (spr_x/y/enable/etc
// registers and spr_mem dpram contents); pixel-level compositing is a
// separate future test module.

`timescale 1ns/1ps

module test_vgc_sprites;

    `include "vgc_tb.svh"

`ifdef VIDEO_720X480
    localparam int PROBE_SPRITE_PLANE_W = 360;
`else
    localparam int PROBE_SPRITE_PLANE_W = 320;
`endif

    // VGC sprite command opcodes
    localparam logic [7:0] CMD_SPRDEF  = 8'h10;
    localparam logic [7:0] CMD_SPRROW  = 8'h11;
    localparam logic [7:0] CMD_SPRCLR  = 8'h12;
    localparam logic [7:0] CMD_SPRCOPY = 8'h13;
    localparam logic [7:0] CMD_SPRPOS  = 8'h14;
    localparam logic [7:0] CMD_SPRENA  = 8'h15;
    localparam logic [7:0] CMD_SPRDIS  = 8'h16;
    localparam logic [7:0] CMD_SPRFLIP = 8'h17;
    localparam logic [7:0] CMD_SPRPRI  = 8'h18;

    // Stand-alone PIXIE instance used for pixel/scanline-buffer regressions.
    // The top-level VGC tests above exercise command wiring; this instance
    // lets us drive exact VGA line starts and verify the sprite plane itself.
    logic [10:0] probe_spr_a_addr = 0;
    logic [7:0]  probe_spr_a_din = 0;
    logic        probe_spr_a_we = 0;
    wire [7:0]   probe_spr_a_dout;
    logic [9:0]  probe_h_count = 1;
    logic [9:0]  probe_v_count = 10'd480;
    logic        probe_sprite_frame_commit = 0;
    logic        probe_shape_publish_block = 0;
    wire         probe_shape_sync_busy;
    logic        probe_visible_d2 = 1;
    logic [8:0]  probe_sprite_x_d2 = 0;
    logic [16*16-1:0] probe_spr_x_flat = 0;
    logic [16*16-1:0] probe_spr_y_flat = 0;
    logic [15:0]      probe_spr_enable_flat = 0;
    logic [15:0]      probe_spr_flip_h_flat = 0;
    logic [15:0]      probe_spr_flip_v_flat = 0;
    logic [16*2-1:0]  probe_spr_pri_flat = 0;
    logic [16*4-1:0]  probe_spr_shape_flat = 0;
    logic [16*4-1:0]  probe_spr_trans_flat = 0;
    wire [3:0]        probe_spr_pixel;
    wire [1:0]        probe_spr_pixel_pri;
    wire              probe_spr_pixel_hit;
    wire [7:0]        probe_collision_bg_bits;

    vgc_sprites sprite_probe (
        .clk(clk), .rst(rst),
        .spr_a_addr(probe_spr_a_addr),
        .spr_a_din(probe_spr_a_din),
        .spr_a_we(probe_spr_a_we),
        .spr_a_re(1'b0),
        .spr_a_dout(probe_spr_a_dout),
        .h_count(probe_h_count),
        .v_count(probe_v_count),
        .sprite_frame_commit(probe_sprite_frame_commit),
        .shape_publish_block(probe_shape_publish_block),
        .shape_sync_busy(probe_shape_sync_busy),
        .visible_d2(probe_visible_d2),
        .sprite_x_d2(probe_sprite_x_d2),
        .spr_x_flat(probe_spr_x_flat),
        .spr_y_flat(probe_spr_y_flat),
        .spr_enable_flat(probe_spr_enable_flat),
        .spr_flip_h_flat(probe_spr_flip_h_flat),
        .spr_flip_v_flat(probe_spr_flip_v_flat),
        .spr_pri_flat(probe_spr_pri_flat),
        .spr_shape_flat(probe_spr_shape_flat),
        .spr_trans_flat(probe_spr_trans_flat),
        .spr_pixel(probe_spr_pixel),
        .spr_pixel_pri(probe_spr_pixel_pri),
        .spr_pixel_hit(probe_spr_pixel_hit),
        .collision_bg_bits(probe_collision_bg_bits)
    );

    // -----------------------------------------------------------------------
    // Command helpers
    // -----------------------------------------------------------------------
    task automatic spr_clr(input int idx);
        write_param(0, idx[7:0]);
        write_cmd(CMD_SPRCLR);
        wait_cmd_done();
    endtask

    // Write one 16-pixel row of a sprite. `pixels` is a 16-entry array of 4-bit
    // color indices. The VGC packs 2 pixels per byte: hi nibble = even col.
    task automatic spr_row(input int idx, input int row,
                            input logic [3:0] pixels[16]);
        write_param(0, idx[7:0]);
        write_param(1, row[7:0]);
        for (int i = 0; i < 8; i++)
            write_param(2 + i, {pixels[2*i], pixels[2*i + 1]});
        write_cmd(CMD_SPRROW);
        wait_cmd_done();
    endtask

    task automatic spr_pos(input int idx, input int x, input int y);
        write_param(0, idx[7:0]);
        write_param(1, x[7:0]);
        write_param(2, x[15:8]);       // x_hi
        write_param(3, y[7:0]);
        write_param(4, y[15:8]);       // y_hi is reserved/ignored by hardware
        write_cmd(CMD_SPRPOS);
        wait_cmd_done();
    endtask

    task automatic spr_ena(input int idx);
        write_param(0, idx[7:0]);
        write_cmd(CMD_SPRENA);
        wait_cmd_done();
    endtask

    task automatic spr_dis(input int idx);
        write_param(0, idx[7:0]);
        write_cmd(CMD_SPRDIS);
        wait_cmd_done();
    endtask

    task automatic spr_flip(input int idx, input bit h, input bit v);
        write_param(0, idx[7:0]);
        write_param(1, {6'd0, v, h});
        write_cmd(CMD_SPRFLIP);
        wait_cmd_done();
    endtask

    task automatic spr_pri(input int idx, input int pri);
        write_param(0, idx[7:0]);
        write_param(1, pri[7:0]);
        write_cmd(CMD_SPRPRI);
        wait_cmd_done();
    endtask

    task automatic spr_copy(input int dst, input int src);
        write_param(0, dst[7:0]);
        write_param(1, src[7:0]);
        write_cmd(CMD_SPRCOPY);
        wait_cmd_done();
    endtask

    // Set one pixel via SPRDEF (the read-modify-write path — must preserve
    // the *other* pixel sharing the byte).
    task automatic spr_def(input int idx, input int x, input int y, input int color);
        write_param(0, idx[7:0]);
        write_param(1, x[7:0]);
        write_param(2, y[7:0]);
        write_param(3, color[7:0]);
        write_cmd(CMD_SPRDEF);
        wait_cmd_done();
    endtask

    task automatic wait_sprite_frame_commit();
        int timeout = 0;
        // vgc.sv commits pending sprite attributes at h=0,v=479 so PIXIE's
        // prefetch of row 0 sees the new snapshot for the whole next frame.
        while (!(dut.h_count == 10'd0 && dut.v_count == 10'd479) &&
               timeout < 500000) begin
            @(posedge clk);
            timeout++;
        end
        check("reached sprite frame commit boundary", timeout < 500000);
        @(posedge clk);  // commit uses h=0,v=479 from the previous cycle
        step(2);
    endtask

    task automatic wait_shape_sync_done();
        int timeout = 0;
        while (dut.sprite_inst.shape_sync_busy && timeout < 20000) begin
            @(posedge clk);
            timeout++;
        end
        check("sprite shape background sync completed", timeout < 20000);
    endtask

    task automatic probe_clear_line_buffers();
        for (int bank = 0; bank < 2; bank++)
            for (int x = 0; x < PROBE_SPRITE_PLANE_W; x++)
                sprite_probe.slb_mem[bank][x] = 7'd0;
    endtask

    task automatic probe_config_solid_sprite(input int y);
        probe_spr_x_flat = '0;
        probe_spr_y_flat = '0;
        probe_spr_enable_flat = '0;
        probe_spr_flip_h_flat = '0;
        probe_spr_flip_v_flat = '0;
        probe_spr_pri_flat = '0;
        probe_spr_shape_flat = '0;
        probe_spr_trans_flat = '0;

        probe_spr_x_flat[0 +: 16] = 16'd0;
        probe_spr_y_flat[0 +: 16] = 16'(y);
        probe_spr_enable_flat[0] = 1'b1;
        probe_spr_pri_flat[0 +: 2] = 2'd2;
        probe_spr_shape_flat[0 +: 4] = 4'd0;
        probe_spr_trans_flat[0 +: 4] = 4'd0;

        for (int row = 0; row < 16; row++)
            for (int col_pair = 0; col_pair < 8; col_pair++)
                if (sprite_probe.active_shape_bank)
                    sprite_probe.spr_mem1.mem[row * 8 + col_pair] = 8'hFF;
                else
                    sprite_probe.spr_mem0.mem[row * 8 + col_pair] = 8'hFF;
    endtask

    task automatic probe_prepare_then_display(input int prep_v, input int display_v);
        probe_h_count <= 10'd0;
        probe_v_count <= prep_v[9:0];
        probe_sprite_x_d2 <= 9'd0;
        @(posedge clk);

        probe_h_count <= 10'd1;
        step(760);

        probe_h_count <= 10'd0;
        probe_v_count <= display_v[9:0];
        @(posedge clk);

        probe_h_count <= 10'd1;
        probe_sprite_x_d2 <= 9'd0;
        @(posedge clk);
        @(posedge clk);
    endtask

    // -----------------------------------------------------------------------
    // Tests
    // -----------------------------------------------------------------------

    task automatic test_sprclr();
        $display("");
        $display("Test: SPRCLR zeros all 128 bytes of a sprite");
        // Pre-dirty sprite 3 with arbitrary data
        for (int i = 0; i < 128; i++)
            poke_spr_pending_addr(11'(3 * 128 + i), 8'hAA);
        spr_clr(3);
        begin
            int nz = 0;
            for (int i = 0; i < 128; i++)
                if (peek_spr_pending_addr(11'(3 * 128 + i)) != 8'h00) nz++;
            check_eq("SPRCLR: sprite 3 is all zeros", nz, 0);
        end
        // Sprite 4 must be untouched (still 0 from reset; pre-dirty it too)
        for (int i = 0; i < 128; i++)
            poke_spr_pending_addr(11'(4 * 128 + i), 8'h77);
        spr_clr(3);  // clear 3 again, check 4 stays dirty
        check_eq("SPRCLR: sprite 4 untouched",
                 int'(peek_spr_pending_addr(11'(4 * 128))), 8'h77);
    endtask

    task automatic test_sprrow();
        $display("");
        $display("Test: SPRROW writes row data → readback matches");
        spr_clr(0);
        begin
            logic [3:0] pix[16];
            for (int i = 0; i < 16; i++) pix[i] = 4'(i);   // 0..15
            spr_row(0, 5, pix);
        end
        // Row 5, pixel 0 should be 0; pixel 15 should be 15
        for (int i = 0; i < 16; i++)
            check_eq($sformatf("SPRROW pixel %0d color", i),
                     int'(peek_spr_pixel(0, i, 5)), i);
        // Other rows in sprite 0 stay zero
        for (int i = 0; i < 16; i++)
            check_eq($sformatf("row 4 col %0d still zero", i),
                     int'(peek_spr_pixel(0, i, 4)), 0);
    endtask

    task automatic test_shape_publish_at_frame_boundary();
        $display("");
        $display("Test: sprite shape RAM publishes only at frame boundary");
        spr_clr(6);
        wait_sprite_frame_commit();
        wait_shape_sync_done();

        begin
            logic [3:0] pix[16];
            for (int i = 0; i < 16; i++) pix[i] = 4'hC;
            spr_row(6, 2, pix);
        end

        check_eq("pending sprite shape byte changed immediately",
                 int'(peek_spr_pending_addr(spr_byte_addr(6, 2, 0))), 8'hCC);
        check_eq("active sprite shape byte unchanged before commit",
                 int'(peek_spr_active_addr(spr_byte_addr(6, 2, 0))), 8'h00);

        wait_sprite_frame_commit();
        check_eq("active sprite shape byte updated after commit",
                 int'(peek_spr_active_addr(spr_byte_addr(6, 2, 0))), 8'hCC);
        wait_shape_sync_done();
        check_eq("pending bank resynced after publish",
                 int'(peek_spr_pending_addr(spr_byte_addr(6, 2, 0))), 8'hCC);
    endtask

    task automatic test_shape_write_on_publish_cycle_defers();
        logic [10:0] addr;

        $display("");
        $display("Test: sprite shape write on publish cycle defers bank flip");
        addr = spr_byte_addr(9, 1, 0);

        spr_clr(9);
        wait_sprite_frame_commit();
        wait_shape_sync_done();

        // Dirty the pending bank so the next sprite frame boundary wants to
        // publish. Then assert a blitter sprite write on that exact boundary.
        poke_spr_pending_addr(addr, 8'hAA);
        dut.sprite_inst.shape_pending_dirty = 1'b1;
        while (!(dut.h_count == 10'd0 && dut.v_count == 10'd479))
            @(posedge clk);
        tb_blt_space <= 3'd4;
        tb_blt_addr  <= {5'd0, addr};
        tb_blt_wdata <= 8'hEE;
        tb_blt_we    <= 1'b1;
        @(posedge clk);
        tb_blt_we    <= 1'b0;
        tb_blt_space <= 3'd0;
        step(3);

        check_eq("publish blocked while sprite shape port is active",
                 int'(peek_spr_active_addr(addr)), 8'h00);
        check_eq("pending shape write still lands",
                 int'(peek_spr_pending_addr(addr)), 8'hEE);

        wait_sprite_frame_commit();
        check_eq("deferred publish updates active bank next frame",
                 int'(peek_spr_active_addr(addr)), 8'hEE);
    endtask

    task automatic test_sprpos();
        $display("");
        $display("Test: SPRPOS stages x/y and commits at frame boundary");
        spr_pos(7, 100, 50);
        check_eq("SPRPOS: sprite 7 pending x = 100", int'(dut.spr_next_x[7]), 100);
        check_eq("SPRPOS: sprite 7 pending y = 50",  int'(dut.spr_next_y[7]), 50);
        check_eq("SPRPOS: sprite 7 active x unchanged before commit", int'(dut.spr_x[7]), 0);
        wait_sprite_frame_commit();
        check_eq("SPRPOS: sprite 7 active x = 100 after commit", int'(dut.spr_x[7]), 100);
        check_eq("SPRPOS: sprite 7 active y = 50 after commit",  int'(dut.spr_y[7]), 50);

        // 9-bit X — test a value > 255
        spr_pos(7, 300, 120);
        check_eq("SPRPOS: sprite 7 pending x = 300 (9-bit)", int'(dut.spr_next_x[7]), 300);
        check_eq("SPRPOS: sprite 7 pending y = 120",         int'(dut.spr_next_y[7]), 120);
        check_eq("SPRPOS: sprite 7 active x still 100 before commit", int'(dut.spr_x[7]), 100);
        wait_sprite_frame_commit();
        check_eq("SPRPOS: sprite 7 active x = 300 after commit", int'(dut.spr_x[7]), 300);
        check_eq("SPRPOS: sprite 7 active y = 120 after commit", int'(dut.spr_y[7]), 120);

        // Neighbor sprite unaffected
        check_eq("SPRPOS: sprite 6 pending x untouched", int'(dut.spr_next_x[6]), 0);
        check_eq("SPRPOS: sprite 8 pending x untouched", int'(dut.spr_next_x[8]), 0);
    endtask

    task automatic test_sprena_dis();
        $display("");
        $display("Test: SPRENA / SPRDIS");
        check_eq("initial pending enable state for sprite 2", int'(dut.spr_next_enable[2]), 0);
        spr_ena(2);
        check_eq("after SPRENA 2, pending enable=1", int'(dut.spr_next_enable[2]), 1);
        check_eq("after SPRENA 2, active enable still 0 before commit", int'(dut.spr_enable[2]), 0);
        spr_dis(2);
        check_eq("after SPRDIS 2, pending enable=0", int'(dut.spr_next_enable[2]), 0);

        // Enabling one sprite doesn't enable others
        spr_ena(5);
        check_eq("sprite 5 pending enabled", int'(dut.spr_next_enable[5]), 1);
        check_eq("sprite 4 pending still disabled", int'(dut.spr_next_enable[4]), 0);
        check_eq("sprite 6 pending still disabled", int'(dut.spr_next_enable[6]), 0);
        wait_sprite_frame_commit();
        check_eq("sprite 5 active enabled after commit", int'(dut.spr_enable[5]), 1);
    endtask

    task automatic test_sprflip();
        $display("");
        $display("Test: SPRFLIP sets h / v independently");
        spr_flip(1, 1'b1, 1'b0);
        check_eq("sprite 1 pending flip_h set",       int'(dut.spr_next_flip_h[1]), 1);
        check_eq("sprite 1 pending flip_v clear",     int'(dut.spr_next_flip_v[1]), 0);
        spr_flip(1, 1'b0, 1'b1);
        check_eq("sprite 1 pending flip_h clear",     int'(dut.spr_next_flip_h[1]), 0);
        check_eq("sprite 1 pending flip_v set",       int'(dut.spr_next_flip_v[1]), 1);
        spr_flip(1, 1'b1, 1'b1);
        check_eq("sprite 1 both pending flips set",
                 int'({dut.spr_next_flip_v[1], dut.spr_next_flip_h[1]}), 2'b11);
        wait_sprite_frame_commit();
        check_eq("sprite 1 both active flips set after commit",
                 int'({dut.spr_flip_v[1], dut.spr_flip_h[1]}), 2'b11);
    endtask

    task automatic test_sprpri();
        $display("");
        $display("Test: SPRPRI sets priority");
        spr_pri(8, 0);
        check_eq("sprite 8 pending priority 0", int'(dut.spr_next_pri[8]), 0);
        spr_pri(8, 1);
        check_eq("sprite 8 pending priority 1", int'(dut.spr_next_pri[8]), 1);
        spr_pri(8, 2);
        check_eq("sprite 8 pending priority 2", int'(dut.spr_next_pri[8]), 2);
        wait_sprite_frame_commit();
        check_eq("sprite 8 active priority 2 after commit", int'(dut.spr_pri[8]), 2);
    endtask

    task automatic test_sprcopy();
        $display("");
        $display("Test: SPRCOPY duplicates sprite data");
        // Write a recognizable pattern into sprite 2
        spr_clr(2);
        begin
            logic [3:0] pix[16];
            for (int i = 0; i < 16; i++) pix[i] = 4'(15 - i);   // 15..0
            spr_row(2, 0, pix);
            spr_row(2, 15, pix);
        end
        // Also verify pre-clear sprite 9 is empty
        spr_clr(9);
        // Copy 2 → 9
        spr_copy(9, 2);

        // Byte-by-byte diff across all 128 bytes of source/destination
        begin
            int diffs = 0;
            for (int i = 0; i < 128; i++) begin
                if (peek_spr_pending_addr(11'(9 * 128 + i)) !=
                    peek_spr_pending_addr(11'(2 * 128 + i))) diffs++;
            end
            check_eq("SPRCOPY: all 128 bytes match source", diffs, 0);
        end

        // Spot-check a pixel we know should be non-zero in both
        check_eq("SPRCOPY: pixel (0,0) of dst == 15",
                 int'(peek_spr_pixel(9, 0, 0)), 15);
        check_eq("SPRCOPY: pixel (15,0) of dst == 0",
                 int'(peek_spr_pixel(9, 15, 0)), 0);
    endtask

    task automatic test_oob_sprite_index();
        logic [8:0] x0;
        logic       e0;
        $display("");
        $display("Test: OOB sprite index (>=16) silently ignored");
        // Record some initial state
        x0 = dut.spr_next_x[0];
        e0 = dut.spr_next_enable[0];

        // These should all be no-ops (idx 16 is out of range)
        write_param(0, 8'd16);
        write_param(1, 8'd99);
        write_param(2, 8'd0);
        write_param(3, 8'd55);
        write_cmd(CMD_SPRPOS);
        wait_cmd_done();

        write_param(0, 8'd16);
        write_cmd(CMD_SPRENA);
        wait_cmd_done();

        // Verify idx 0 untouched (no spillover)
        check_eq("OOB SPRPOS: sprite 0 pending x unchanged", int'(dut.spr_next_x[0]), int'(x0));
        check_eq("OOB SPRENA: sprite 0 pending enable unchanged",
                 int'(dut.spr_next_enable[0]), int'(e0));
    endtask

    task automatic test_sprdef_preserves_other_pixel();
        $display("");
        $display("Test: SPRDEF modifies only one pixel (byte holds 2)");
        spr_clr(0);
        begin
            logic [3:0] pix[16];
            for (int i = 0; i < 16; i++) pix[i] = 4'(i);
            spr_row(0, 7, pix);
        end
        // Preconditions from SPRROW
        check_eq("pre: pixel (2,7) == 2", int'(peek_spr_pixel(0, 2, 7)), 2);
        check_eq("pre: pixel (3,7) == 3", int'(peek_spr_pixel(0, 3, 7)), 3);
        check_eq("pre: pixel (4,7) == 4", int'(peek_spr_pixel(0, 4, 7)), 4);
        check_eq("pre: pixel (5,7) == 5", int'(peek_spr_pixel(0, 5, 7)), 5);

        // Modify only pixel (3,7) — odd column, so low nibble of byte 1
        spr_def(0, 3, 7, 9);
        check_eq("post: pixel (3,7) == 9",             int'(peek_spr_pixel(0, 3, 7)), 9);
        check_eq("post: neighbor (2,7) still == 2",    int'(peek_spr_pixel(0, 2, 7)), 2);
        check_eq("post: byte-mate (4,7) still == 4",   int'(peek_spr_pixel(0, 4, 7)), 4);
        check_eq("post: adjacent (5,7) still == 5",    int'(peek_spr_pixel(0, 5, 7)), 5);

        // Now modify even column (4,7) → color 12; ensure (5,7) preserved
        spr_def(0, 4, 7, 12);
        check_eq("post: pixel (4,7) == 12",            int'(peek_spr_pixel(0, 4, 7)), 12);
        check_eq("post: byte-mate (5,7) still == 5",   int'(peek_spr_pixel(0, 5, 7)), 5);
    endtask

    task automatic test_multiple_sprites_independent();
        $display("");
        $display("Test: 16 sprites hold independent state");
        // Start with all sprites disabled (clear leaks from earlier tests)
        for (int i = 0; i < 16; i++) spr_dis(i);
        // Fingerprint each sprite: position (i*10, i*5), enable=i even, pri=i%3
        for (int i = 0; i < 16; i++) begin
            spr_pos(i, i * 10, i * 5);
            if (i % 2 == 0) spr_ena(i);
            spr_pri(i, i % 3);
        end
        // Verify each
        for (int i = 0; i < 16; i++) begin
            check_eq($sformatf("sprite %0d pending x",      i), int'(dut.spr_next_x[i]),      i * 10);
            check_eq($sformatf("sprite %0d pending y",      i), int'(dut.spr_next_y[i]),      i * 5);
            check_eq($sformatf("sprite %0d pending enable", i), int'(dut.spr_next_enable[i]), (i % 2 == 0) ? 1 : 0);
            check_eq($sformatf("sprite %0d pending pri",    i), int'(dut.spr_next_pri[i]),    i % 3);
        end
        wait_sprite_frame_commit();
        for (int i = 0; i < 16; i++) begin
            check_eq($sformatf("sprite %0d active x after commit",      i), int'(dut.spr_x[i]),      i * 10);
            check_eq($sformatf("sprite %0d active y after commit",      i), int'(dut.spr_y[i]),      i * 5);
            check_eq($sformatf("sprite %0d active enable after commit", i), int'(dut.spr_enable[i]), (i % 2 == 0) ? 1 : 0);
            check_eq($sformatf("sprite %0d active pri after commit",    i), int'(dut.spr_pri[i]),    i % 3);
        end
    endtask

    task automatic test_sprite_probe_covers_full_sprite_plane();
        $display("");
        $display("Test: PIXIE prepares top and bottom rows of the sprite plane");

        probe_clear_line_buffers();
        probe_config_solid_sprite(0);
        // Native line 479 prepares gfx/sprite row 0 for the next frame's
        // native line 0. This catches the old 320x200/V_BORDER=40 evaluator.
        probe_prepare_then_display(479, 0);
        check_eq("probe top row pixel hit", int'(probe_spr_pixel_hit), 1);
        check_eq("probe top row color", int'(probe_spr_pixel), 15);

        probe_clear_line_buffers();
        probe_config_solid_sprite(239);
        // Native line 478 prepares native line 479, which maps to sprite y=239.
        probe_prepare_then_display(478, 479);
        check_eq("probe bottom row pixel hit", int'(probe_spr_pixel_hit), 1);
        check_eq("probe bottom row color", int'(probe_spr_pixel), 15);
    endtask

    // -----------------------------------------------------------------------
    // Runner
    // -----------------------------------------------------------------------
    initial begin
        $display("=== VGC Sprite-Layer Tests ===");
        do_reset();

        test_sprclr();
        test_sprrow();
        test_shape_publish_at_frame_boundary();
        test_shape_write_on_publish_cycle_defers();
        test_sprpos();
        test_sprena_dis();
        test_sprflip();
        test_sprpri();
        test_sprcopy();
        test_sprdef_preserves_other_pixel();
        test_oob_sprite_index();
        test_multiple_sprites_independent();
        test_sprite_probe_covers_full_sprite_plane();

        summary();
        $finish;
    end

    initial begin
        #2000000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
