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
    localparam int CANVAS_X0_TB = 40;
`else
    localparam int PROBE_SPRITE_PLANE_W = 320;
    localparam int CANVAS_X0_TB = 0;
`endif
    localparam int CANVAS_Y0_TB = 40;

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
    localparam logic [15:0] MOUSE_XL_A    = 16'hA0D0;
    localparam logic [15:0] MOUSE_XH_A    = 16'hA0D1;
    localparam logic [15:0] MOUSE_Y_A     = 16'hA0D2;
    localparam logic [15:0] MOUSE_BTN_A   = 16'hA0D3;
    localparam logic [15:0] MOUSE_CTRL_A  = 16'hA0D4;
    localparam logic [15:0] MOUSE_SHAPE_A = 16'hA0D6;
    localparam logic [15:0] MOUSE_DBG_A   = 16'hA0D9;   // debug render-path capture

    // Stand-alone PIXIE instance used for pixel/scanline-buffer regressions.
    // The top-level VGC tests above exercise command wiring; this instance
    // lets us drive exact VGA line starts and verify PIXIE itself.
    logic [14:0] probe_spr_a_addr = 0;
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
    logic [16*8-1:0]  probe_spr_shape_flat = 0;
    logic [16*4-1:0]  probe_spr_trans_flat = 0;
    logic [8:0]        probe_mouse_x = 0;
    logic [7:0]        probe_mouse_y = 0;
    logic              probe_mouse_enable = 0;
    logic [7:0]        probe_mouse_shape = 0;
    logic [3:0]        probe_mouse_hot_x = 0;
    logic [3:0]        probe_mouse_hot_y = 0;
    wire [3:0]        probe_spr_pixel;
    wire [1:0]        probe_spr_pixel_pri;
    wire              probe_spr_pixel_hit;
    wire [3:0]        probe_spr_pixel_owner;
    wire [3:0]        probe_mouse_cursor_pixel;
    wire              probe_mouse_cursor_hit;
    wire [15:0]       probe_collision_ss_bits;

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
        .mouse_x(probe_mouse_x),
        .mouse_y(probe_mouse_y),
        .mouse_enable(probe_mouse_enable),
        .mouse_shape(probe_mouse_shape),
        .mouse_hot_x(probe_mouse_hot_x),
        .mouse_hot_y(probe_mouse_hot_y),
        .spr_pixel(probe_spr_pixel),
        .spr_pixel_pri(probe_spr_pixel_pri),
        .spr_pixel_hit(probe_spr_pixel_hit),
        .spr_pixel_owner(probe_spr_pixel_owner),
        .mouse_cursor_pixel(probe_mouse_cursor_pixel),
        .mouse_cursor_hit(probe_mouse_cursor_hit),
        .collision_ss_bits(probe_collision_ss_bits)
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

    task automatic wait_vblank_start();
        int timeout = 0;
        while (!(dut.h_count == 10'd0 && dut.v_count == 10'd480) &&
               timeout < 500000) begin
            @(posedge clk);
            timeout++;
        end
        check("reached vblank start", timeout < 500000);
        @(posedge clk);
        step(2);
    endtask

    task automatic wait_shape_sync_done();
        int timeout = 0;
        while (dut.sprite_inst.shape_sync_busy && timeout < 150000) begin
            @(posedge clk);
            timeout++;
        end
        check("sprite shape background sync completed", timeout < 150000);
    endtask

    task automatic probe_clear_line_buffers();
        for (int bank = 0; bank < 2; bank++)
            for (int x = 0; x < PROBE_SPRITE_PLANE_W; x++)
                sprite_probe.slb_ram.mem[(bank << 9) + x] = 7'd0;
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
        probe_spr_shape_flat[0 +: 8] = 8'd0;
        probe_spr_trans_flat[0 +: 4] = 4'd0;

        for (int row = 0; row < 16; row++)
            for (int col_pair = 0; col_pair < 8; col_pair++)
                if (sprite_probe.active_shape_bank)
                    sprite_probe.spr_mem1.mem[row * 8 + col_pair] = 8'hFF;
                else
                    sprite_probe.spr_mem0.mem[row * 8 + col_pair] = 8'hFF;
    endtask

    task automatic probe_config_pattern_sprite(input int y);
        logic [7:0] row_bytes[8];

        row_bytes[0] = 8'h12;
        row_bytes[1] = 8'h34;
        row_bytes[2] = 8'h56;
        row_bytes[3] = 8'h78;
        row_bytes[4] = 8'h9A;
        row_bytes[5] = 8'hBC;
        row_bytes[6] = 8'hDE;
        row_bytes[7] = 8'hF1;

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
        probe_spr_shape_flat[0 +: 8] = 8'd0;
        probe_spr_trans_flat[0 +: 4] = 4'd0;

        for (int row = 0; row < 16; row++) begin
            for (int col_pair = 0; col_pair < 8; col_pair++) begin
                logic [7:0] value;
                value = (row == 0) ? row_bytes[col_pair] : 8'h00;
                if (sprite_probe.active_shape_bank)
                    sprite_probe.spr_mem1.mem[row * 8 + col_pair] = value;
                else
                    sprite_probe.spr_mem0.mem[row * 8 + col_pair] = value;
            end
        end
    endtask

    task automatic probe_config_overlapping_sprites(input int sprite_base, input int y);
        probe_spr_x_flat = '0;
        probe_spr_y_flat = '0;
        probe_spr_enable_flat = '0;
        probe_spr_flip_h_flat = '0;
        probe_spr_flip_v_flat = '0;
        probe_spr_pri_flat = '0;
        probe_spr_shape_flat = '0;
        probe_spr_trans_flat = '0;

        probe_spr_x_flat[sprite_base * 16 +: 16] = 16'd0;
        probe_spr_y_flat[sprite_base * 16 +: 16] = 16'(y);
        probe_spr_enable_flat[sprite_base] = 1'b1;
        probe_spr_pri_flat[sprite_base * 2 +: 2] = 2'd1;
        probe_spr_shape_flat[sprite_base * 8 +: 8] = 8'd0;
        probe_spr_trans_flat[sprite_base * 4 +: 4] = 4'd0;

        probe_spr_x_flat[(sprite_base + 1) * 16 +: 16] = 16'd8;
        probe_spr_y_flat[(sprite_base + 1) * 16 +: 16] = 16'(y);
        probe_spr_enable_flat[sprite_base + 1] = 1'b1;
        probe_spr_pri_flat[(sprite_base + 1) * 2 +: 2] = 2'd1;
        probe_spr_shape_flat[(sprite_base + 1) * 8 +: 8] = 8'd1;
        probe_spr_trans_flat[(sprite_base + 1) * 4 +: 4] = 4'd0;

        for (int row = 0; row < 16; row++) begin
            for (int col_pair = 0; col_pair < 8; col_pair++) begin
                if (sprite_probe.active_shape_bank) begin
                    sprite_probe.spr_mem1.mem[row * 8 + col_pair] = 8'hFF;
                    sprite_probe.spr_mem1.mem[128 + row * 8 + col_pair] = 8'hEE;
                end else begin
                    sprite_probe.spr_mem0.mem[row * 8 + col_pair] = 8'hFF;
                    sprite_probe.spr_mem0.mem[128 + row * 8 + col_pair] = 8'hEE;
                end
            end
        end
    endtask

    task automatic probe_sample_pixel(input int x, output int hit, output int color);
        probe_sprite_x_d2 <= x[8:0];
        @(posedge clk);
        @(posedge clk);
        hit = int'(probe_spr_pixel_hit);
        color = int'(probe_spr_pixel);
    endtask

    task automatic probe_sample_mouse_pixel(input int x, output int hit, output int color);
        probe_sprite_x_d2 <= x[8:0];
        @(posedge clk);
        @(posedge clk);
        hit = int'(probe_mouse_cursor_hit);
        color = int'(probe_mouse_cursor_pixel);
    endtask

    // Write one shape byte through the real spr_a/VMEM write port (the path the
    // Linux host's vmem_write uses), NOT by poking the dpram backing array.
    task automatic probe_write_shape_byte(input logic [14:0] addr, input logic [7:0] data);
        @(negedge clk);
        probe_spr_a_addr = addr;
        probe_spr_a_din  = data;
        probe_spr_a_we   = 1'b1;
        @(posedge clk);
        @(negedge clk);
        probe_spr_a_we   = 1'b0;
        probe_spr_a_addr = 15'd0;
        probe_spr_a_din  = 8'd0;
    endtask

    // Pulse a sprite frame commit so a dirty pending bank publishes to active.
    task automatic probe_publish_pulse();
        @(negedge clk);
        probe_sprite_frame_commit = 1'b1;
        @(posedge clk);
        @(negedge clk);
        probe_sprite_frame_commit = 1'b0;
    endtask

    task automatic probe_wait_sync_done();
        int timeout = 0;
        while (probe_shape_sync_busy && timeout < 200000) begin
            @(posedge clk);
            timeout++;
        end
        check("probe shape background sync completed", timeout < 200000);
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

    task automatic probe_prepare_then_display_capture_collisions(
        input int prep_v,
        input int display_v,
        output int mask
    );
        mask = 0;
        probe_h_count <= 10'd0;
        probe_v_count <= prep_v[9:0];
        probe_sprite_x_d2 <= 9'd0;
        @(posedge clk);

        probe_h_count <= 10'd1;
        for (int i = 0; i < 760; i++) begin
            @(posedge clk);
            mask |= int'(probe_collision_ss_bits);
        end

        probe_h_count <= 10'd0;
        probe_v_count <= display_v[9:0];
        @(posedge clk);

        probe_h_count <= 10'd1;
        probe_sprite_x_d2 <= 9'd0;
        @(posedge clk);
        @(posedge clk);
    endtask

    task automatic wait_top_timing_d2(input int h, input int v, input string label);
        int timeout = 0;
        while (!((dut.h_count_d2 == 10'(h)) && (dut.v_count_d2 == 10'(v))) &&
               timeout < 900000) begin
            @(posedge clk);
            timeout++;
        end
        check($sformatf("reached top-level timing sample: %s", label), timeout < 900000);
    endtask

    // -----------------------------------------------------------------------
    // Tests
    // -----------------------------------------------------------------------

    task automatic test_sprclr();
        $display("");
        $display("Test: SPRCLR zeros all 128 bytes of a sprite");
        // Pre-dirty sprite 3 with arbitrary data
        for (int i = 0; i < 128; i++)
            poke_spr_pending_addr(15'(3 * 128 + i), 8'hAA);
        spr_clr(3);
        begin
            int nz = 0;
            for (int i = 0; i < 128; i++)
                if (peek_spr_pending_addr(15'(3 * 128 + i)) != 8'h00) nz++;
            check_eq("SPRCLR: sprite 3 is all zeros", nz, 0);
        end
        // Sprite 4 must be untouched (still 0 from reset; pre-dirty it too)
        for (int i = 0; i < 128; i++)
            poke_spr_pending_addr(15'(4 * 128 + i), 8'h77);
        spr_clr(3);  // clear 3 again, check 4 stays dirty
        check_eq("SPRCLR: sprite 4 untouched",
                 int'(peek_spr_pending_addr(15'(4 * 128))), 8'h77);
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
        logic [14:0] addr;

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

    task automatic test_shape_write_during_sync_mirrors_banks();
        logic [14:0] addr;

        $display("");
        $display("Test: sprite shape write during background sync mirrors both banks");
        addr = spr_byte_addr(10, 3, 0);

        wait_shape_sync_done();
        poke_spr_pending_addr(addr, 8'h11);
        dut.sprite_inst.shape_pending_dirty = 1'b1;
        // Backdoor pokes bypass the dirty-range tracker, so force a full-length
        // range: the mid-sync write below needs the background copy still running.
        dut.sprite_inst.shape_dirty_min = 15'd0;
        dut.sprite_inst.shape_dirty_max = 15'd32767;
        wait_sprite_frame_commit();
        step(4);
        check("background shape sync is active", dut.sprite_inst.shape_sync_busy);
        check_eq("precondition: active bank has published byte",
                 int'(peek_spr_active_addr(addr)), 8'h11);

        tb_blt_space <= 3'd4;
        tb_blt_addr  <= {1'b0, addr};
        tb_blt_wdata <= 8'h55;
        tb_blt_we    <= 1'b1;
        @(posedge clk);
        tb_blt_we    <= 1'b0;
        tb_blt_space <= 3'd0;
        step(3);

        check_eq("sync-time write updates pending bank",
                 int'(peek_spr_pending_addr(addr)), 8'h55);
        check_eq("sync-time write also mirrors active bank to avoid a dirty bitmap",
                 int'(peek_spr_active_addr(addr)), 8'h55);
        wait_shape_sync_done();
        check_eq("mirrored byte survives background sync completion",
                 int'(peek_spr_pending_addr(addr)), 8'h55);
    endtask

    // Regression guard for the live-Arty bug: a single-slot shape write must
    // reconcile the banks in-frame (tight dirty range), NOT walk the whole 32K
    // RAM. The full-RAM copy never caught up under paced writes, so the render
    // read a stale/empty active bank and the mouse cursor + sprites never showed.
    task automatic test_shape_single_slot_reconciles_fast();
        int cycles;
        logic [14:0] base;
        $display("");
        $display("Test: single-slot shape write => tight range => fast reconcile");
        base = spr_byte_addr(255, 0, 0);   // mouse cursor slot

        wait_shape_sync_done();
        // Clean the range (a prior test backdoor-forced it; real operation resets
        // it at every publish) so this measures only slot 255's writes.
        dut.sprite_inst.shape_dirty_min = 15'd32767;
        dut.sprite_inst.shape_dirty_max = 15'd0;
        // Write all 128 bytes of slot 255 through the real port (feeds the range).
        for (int i = 0; i < 128; i++) begin
            tb_blt_space <= 3'd4;
            tb_blt_addr  <= {1'b0, base + i[14:0]};
            tb_blt_wdata <= 8'hC0 | i[5:0];
            tb_blt_we    <= 1'b1;
            @(posedge clk);
        end
        tb_blt_we <= 1'b0; tb_blt_space <= 3'd0;
        @(posedge clk);

        wait_sprite_frame_commit();
        cycles = 0;
        while (dut.sprite_inst.shape_sync_busy && cycles < 200000) begin
            @(posedge clk); cycles++;
        end
        check("slot-255 sync completes (not stuck)", !dut.sprite_inst.shape_sync_busy);
        check("slot-255 reconcile is fast: range copy, not full-RAM walk", cycles < 2000);
        check_eq("slot255 pending byte0 after sync",   int'(peek_spr_pending_addr(base)),       8'hC0);
        check_eq("slot255 active  byte0 after sync",   int'(peek_spr_active_addr(base)),        8'hC0);
        check_eq("slot255 pending byte127 after sync", int'(peek_spr_pending_addr(base + 127)), 8'hFF);
        check_eq("slot255 active  byte127 after sync", int'(peek_spr_active_addr(base + 127)),  8'hFF);
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
                if (peek_spr_pending_addr(15'(9 * 128 + i)) !=
                    peek_spr_pending_addr(15'(2 * 128 + i))) diffs++;
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
        $display("Test: PIXIE uses Nova canvas Y coordinates");

        probe_clear_line_buffers();
        probe_config_solid_sprite(0);
        // Native line 39 prepares the first Nova canvas line at physical line
        // 40. Sprite Y is in canvas coordinates, matching graphics/copper Y.
        probe_prepare_then_display(39, 40);
        check_eq("probe canvas top row pixel hit", int'(probe_spr_pixel_hit), 1);
        check_eq("probe canvas top row color", int'(probe_spr_pixel), 15);

        probe_clear_line_buffers();
        probe_config_solid_sprite(199);
        // Native line 438 prepares physical line 439, the final Nova canvas
        // line. This keeps canvas bottom sprite Y aligned with gfx row 199.
        probe_prepare_then_display(438, 439);
        check_eq("probe canvas bottom row pixel hit", int'(probe_spr_pixel_hit), 1);
        check_eq("probe canvas bottom row color", int'(probe_spr_pixel), 15);
    endtask

    task automatic test_sprite_probe_reads_shape_bytes_in_order();
        int hit;
        int color;
        int expected[16];

        $display("");
        $display("Test: PIXIE reads asymmetric sprite shape bytes in order");

        expected = '{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 1};

        probe_clear_line_buffers();
        probe_config_pattern_sprite(0);
        probe_prepare_then_display(39, 40);

        for (int x = 0; x < 16; x++) begin
            probe_sample_pixel(x, hit, color);
            check_eq($sformatf("probe pattern pixel %0d hit", x), hit, 1);
            check_eq($sformatf("probe pattern pixel %0d color", x), color, expected[x]);
        end
    endtask

    task automatic test_sprite_probe_reports_sprite_sprite_collision_mask();
        int mask;

        $display("");
        $display("Test: PIXIE reports 16-bit sprite-sprite collision mask");

        probe_clear_line_buffers();
        probe_config_overlapping_sprites(0, 0);
        probe_prepare_then_display_capture_collisions(39, 40, mask);

        check_eq("overlapping sprite 0/1 set both collision bits", mask, 8'h03);

        probe_clear_line_buffers();
        probe_config_overlapping_sprites(8, 0);
        probe_prepare_then_display_capture_collisions(39, 40, mask);

        check_eq("overlapping sprite 8/9 set both collision bits", mask, 16'h0300);
    endtask

    task automatic test_mouse_registers_commit_at_vblank();
        logic [7:0] readback;

        $display("");
        $display("Test: VGC mouse registers commit visible state at vblank");

        bus_write(MOUSE_XL_A, 8'd42);
        bus_write(MOUSE_XH_A, 8'd1);
        bus_write(MOUSE_Y_A, 8'd99);
        bus_write(MOUSE_BTN_A, 8'h07);
        bus_write(MOUSE_SHAPE_A, 8'd255);
        bus_write(MOUSE_CTRL_A, 8'h01);

        bus_read(MOUSE_XL_A, readback);
        check_eq("mouse pending X low is CPU-readable", int'(readback), 42);
        check_eq("mouse active X unchanged before vblank", int'(dut.mouse_active_x), 0);
        check_eq("mouse active ctrl unchanged before vblank", int'(dut.mouse_active_ctrl), 0);

        wait_vblank_start();

        check_eq("mouse active X committed at vblank", int'(dut.mouse_active_x), 298);
        check_eq("mouse active Y committed at vblank", int'(dut.mouse_active_y), 99);
        check_eq("mouse active buttons committed at vblank", int'(dut.mouse_active_buttons), 7);
        check_eq("mouse active shape committed at vblank", int'(dut.mouse_active_shape), 255);
        check_eq("mouse active ctrl committed at vblank", int'(dut.mouse_active_ctrl), 1);
    endtask

    task automatic test_mouse_probe_reads_shape_slot();
        int hit;
        int color;
        int base = 255 * 128;

        $display("");
        $display("Test: PIXIE samples dedicated mouse cursor shape slot");

        probe_clear_line_buffers();
        if (sprite_probe.active_shape_bank)
            sprite_probe.spr_mem1.mem[base] = 8'hD0;
        else
            sprite_probe.spr_mem0.mem[base] = 8'hD0;

        probe_mouse_x = 9'd6;
        probe_mouse_y = 8'd0;
        probe_mouse_enable = 1'b1;
        probe_mouse_shape = 8'd255;
        probe_mouse_hot_x = 4'd0;
        probe_mouse_hot_y = 4'd0;

        probe_prepare_then_display(39, 40);
        probe_sample_mouse_pixel(6, hit, color);
        check_eq("mouse cursor pixel hits from slot 255", hit, 1);
        check_eq("mouse cursor pixel color from high nibble", color, 13);

        probe_sample_mouse_pixel(7, hit, color);
        check_eq("mouse cursor transparent low nibble does not hit", hit, 0);
    endtask

    // Faithful reproduction of the live-hardware path: the host writes the
    // mouse cursor shape into slot 255 through the spr_a/VMEM port, the design
    // publishes pending->active, and only THEN can the mouse overlay sample it.
    // The existing mouse test hand-pokes the active dpram bank, so it never
    // exercised this. Slot 255 row 0 = solid (0xFF) -> mouse must hit color 15.
    task automatic test_mouse_shape_via_write_port();
        int hit;
        int color;
        int base = 255 * 128;

        $display("");
        $display("Test: mouse shape written via spr_a port publishes to the bank the mouse reads");

        probe_wait_sync_done();
        probe_clear_line_buffers();
        // Write slot 255 row 0 (8 bytes) through the real write port.
        for (int b = 0; b < 8; b++)
            probe_write_shape_byte(15'(base + b), 8'hFF);
        // Publish pending->active, let the background bank-sync finish.
        probe_publish_pulse();
        probe_wait_sync_done();

        probe_mouse_x = 9'd6;
        probe_mouse_y = 8'd0;
        probe_mouse_enable = 1'b1;
        probe_mouse_shape = 8'd255;
        probe_mouse_hot_x = 4'd0;
        probe_mouse_hot_y = 4'd0;

        probe_prepare_then_display(39, 40);
        probe_sample_mouse_pixel(6, hit, color);
        check_eq("port-written mouse shape hits after publish", hit, 1);
        check_eq("port-written mouse shape color is 15", color, 15);
    endtask

    // Same, but a frame commit lands BETWEEN each byte write, modelling the
    // network-paced host writes racing the 60Hz publish on real hardware.
    task automatic test_mouse_shape_write_port_interleaved();
        int hit;
        int color;
        int base = 255 * 128;

        $display("");
        $display("Test: mouse shape survives publishes interleaved with per-byte writes");

        probe_wait_sync_done();
        probe_clear_line_buffers();
        for (int b = 0; b < 8; b++) begin
            probe_write_shape_byte(15'(base + b), 8'hFF);
            probe_publish_pulse();   // a 60Hz commit fires mid-fill
        end
        probe_publish_pulse();
        probe_wait_sync_done();

        probe_mouse_x = 9'd6;
        probe_mouse_y = 8'd0;
        probe_mouse_enable = 1'b1;
        probe_mouse_shape = 8'd255;
        probe_mouse_hot_x = 4'd0;
        probe_mouse_hot_y = 4'd0;

        probe_prepare_then_display(39, 40);
        probe_sample_mouse_pixel(6, hit, color);
        check_eq("interleaved-write mouse shape still hits", hit, 1);
        check_eq("interleaved-write mouse shape color is 15", color, 15);
    endtask

    // DUT-level: does the full VGC publish the MOUSE slot (255) written through
    // the VMEM/blitter port (space 4) exactly like the host's vmem_write? The
    // existing publish test only ever exercises low sprite slots (0-15).
    task automatic test_dut_publishes_mouse_slot_255();
        logic [14:0] a0;

        $display("");
        $display("Test: DUT publishes mouse shape slot 255 written via VMEM/blitter port");
        a0 = spr_byte_addr(255, 0, 0);   // 32640

        // Backdoor-clear both banks at slot 255, settle any prior sync.
        poke_spr_pending_addr(a0, 8'h00);
        poke_spr_active_addr(a0, 8'h00);
        wait_sprite_frame_commit();
        wait_shape_sync_done();

        // Write slot 255 byte 0 through the real VMEM/blitter port (space 4).
        @(negedge clk);
        tb_blt_space <= 3'd4;
        tb_blt_addr  <= {1'b0, a0};
        tb_blt_wdata <= 8'hFF;
        tb_blt_we    <= 1'b1;
        @(posedge clk);
        @(negedge clk);
        tb_blt_we    <= 1'b0;
        tb_blt_space <= 3'd0;
        step(3);

        check_eq("slot255 pending byte written via VMEM port",
                 int'(peek_spr_pending_addr(a0)), 8'hFF);

        wait_sprite_frame_commit();
        check_eq("slot255 active byte PUBLISHED after commit (mouse can read it)",
                 int'(peek_spr_active_addr(a0)), 8'hFF);
    endtask

    // Count cycles a top-level DUT signal-of-interest is asserted across exactly
    // one full frame (v_count/h_count free-run off clk). Used to prove the DUT
    // RASTERIZES sprite/mouse pixels — no existing test checks the rendered pixel,
    // only registers/memory. This is the gap that let both HW render bugs hide.
    task automatic dut_count_spr_hits_one_frame(output int count);
        int guard;
        count = 0;
        guard = 0;
        while (!(dut.v_count == 10'd0 && dut.h_count == 10'd0) && guard < 700000) begin
            @(posedge clk); guard++;
        end
        @(posedge clk);
        guard = 0;
        while (!(dut.v_count == 10'd0 && dut.h_count == 10'd0) && guard < 700000) begin
            if (dut.spr_pixel_hit) count++;
            @(posedge clk); guard++;
        end
    endtask

    task automatic dut_count_mouse_hits_one_frame(output int count);
        int guard;
        count = 0;
        guard = 0;
        while (!(dut.v_count == 10'd0 && dut.h_count == 10'd0) && guard < 700000) begin
            @(posedge clk); guard++;
        end
        @(posedge clk);
        guard = 0;
        while (!(dut.v_count == 10'd0 && dut.h_count == 10'd0) && guard < 700000) begin
            if (dut.mouse_cursor_hit) count++;
            @(posedge clk); guard++;
        end
    endtask

    // DUT-level end-to-end sprite RENDER: load slot 0 via the real SPRROW command
    // path, position/enable sprite 0, and confirm the raster pipeline actually
    // produces spr_pixel_hit. Reproduces the live-hardware "sprite is blank"
    // symptom if the DUT render pipeline is broken.
    task automatic test_dut_renders_enabled_sprite();
        int hits_off, hits_on;
        logic [3:0] pix[16];

        $display("");
        $display("Test: DUT rasterizes an enabled sprite (spr_pixel_hit fires)");

        // Isolate: earlier tests leave several sprites enabled — start clean.
        for (int i = 0; i < 16; i++) spr_dis(i);
        wait_sprite_frame_commit();

        spr_clr(0);
        for (int i = 0; i < 16; i++) pix[i] = 4'hF;   // solid color 15
        for (int r = 0; r < 16; r++) spr_row(0, r, pix);
        spr_pos(0, 100, 80);
        spr_pri(0, 2);
        wait_sprite_frame_commit();
        wait_shape_sync_done();

        dut_count_spr_hits_one_frame(hits_off);      // sprite still DISABLED
        check_eq("disabled sprite produces no rasterized pixels", hits_off, 0);

        spr_ena(0);
        wait_sprite_frame_commit();
        dut_count_spr_hits_one_frame(hits_on);       // sprite ENABLED
        check("enabled sprite IS rasterized (spr_pixel_hit fires)", hits_on > 0);

        // The $A0D9 debug capture is what we read on LIVE HARDWARE to see where
        // the render dies. Prove here (where render works) that it tracks the
        // signal: clear it, run a frame, bit0 must latch spr_pixel_hit.
        begin
            logic [7:0] cap;
            int junk;
            bus_write(MOUSE_DBG_A, 8'h00);           // clear sticky bits
            dut_count_spr_hits_one_frame(junk);      // one frame re-accumulates
            bus_read(MOUSE_DBG_A, cap);
            check("dbg $A0D9 bit0 latches spr_pixel_hit when sprite renders",
                  (cap & 8'h01) != 8'h00);
            check("dbg $A0D9 bit7 latches shape-read-nonzero when sprite renders",
                  (cap & 8'h80) != 8'h00);
        end
    endtask

    // DUT-level end-to-end mouse RENDER: load slot 255, enable+position the mouse
    // via its registers, confirm the overlay produces mouse_cursor_hit.
    task automatic test_dut_renders_mouse_cursor();
        int hits_off, hits_on;
        int base = 255 * 128;

        $display("");
        $display("Test: DUT rasterizes the mouse cursor (mouse_cursor_hit fires)");

        // Load slot 255 solid via the VMEM/blitter port, publish.
        for (int b = 0; b < 128; b++) begin
            @(negedge clk);
            tb_blt_space <= 3'd4;
            tb_blt_addr  <= 16'(base + b);
            tb_blt_wdata <= 8'hFF;
            tb_blt_we    <= 1'b1;
            @(posedge clk);
        end
        @(negedge clk);
        tb_blt_we    <= 1'b0;
        tb_blt_space <= 3'd0;
        wait_sprite_frame_commit();
        wait_shape_sync_done();

        // Position mouse mid-canvas, shape 255, but DISABLED (ctrl=0) → baseline.
        bus_write(MOUSE_XL_A, 8'd100);
        bus_write(MOUSE_XH_A, 8'd0);
        bus_write(MOUSE_Y_A, 8'd80);
        bus_write(MOUSE_SHAPE_A, 8'd255);
        bus_write(MOUSE_CTRL_A, 8'h00);
        wait_vblank_start();
        dut_count_mouse_hits_one_frame(hits_off);
        check_eq("disabled mouse cursor produces no rasterized pixels", hits_off, 0);

        bus_write(MOUSE_CTRL_A, 8'h03);   // ENABLE | AUTO
        wait_vblank_start();
        dut_count_mouse_hits_one_frame(hits_on);
        check("enabled mouse cursor IS rasterized (mouse_cursor_hit fires)", hits_on > 0);
    endtask

    task automatic test_top_sprite_read_uses_canvas_x();
        $display("");
        $display("Test: top-level VGC samples sprite buffer in Nova canvas X coordinates");

        wait_top_timing_d2(CANVAS_X0_TB, CANVAS_Y0_TB, "canvas left");
        check_eq("sprite read x at canvas left edge", int'(dut.sprite_x_read_d2), 0);

        wait_top_timing_d2(CANVAS_X0_TB + 40, CANVAS_Y0_TB, "20 logical pixels into canvas");
        check_eq("sprite read x inside canvas", int'(dut.sprite_x_read_d2), 20);
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
        test_shape_write_during_sync_mirrors_banks();
        test_shape_single_slot_reconciles_fast();
        test_sprpos();
        test_sprena_dis();
        test_sprflip();
        test_sprpri();
        test_sprcopy();
        test_sprdef_preserves_other_pixel();
        test_oob_sprite_index();
        test_multiple_sprites_independent();
        test_sprite_probe_covers_full_sprite_plane();
        test_sprite_probe_reads_shape_bytes_in_order();
        test_sprite_probe_reports_sprite_sprite_collision_mask();
        test_mouse_registers_commit_at_vblank();
        test_mouse_probe_reads_shape_slot();
        test_mouse_shape_via_write_port();
        test_mouse_shape_write_port_interleaved();
        test_dut_publishes_mouse_slot_255();
        test_dut_renders_enabled_sprite();
        test_dut_renders_mouse_cursor();
        test_top_sprite_read_uses_canvas_x();

        summary();
        $finish;
    end

    initial begin
        #2000000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
