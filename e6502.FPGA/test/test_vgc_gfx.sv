// Graphics-layer RTL tests for the VGC — covers the Artist engine:
// GCLS, GCOLOR, PLOT, UNPLOT, LINE, RECT, FILL, CIRCLE, PAINT, GTEXT bounds.
//
// Invariant-based tests validate each command's correctness directly
// (no oracle-replay yet). When Avalonia divergence is suspected, Phase 2
// of the harness plan adds capture/replay.

`timescale 1ns/1ps

module test_vgc_gfx;

    `include "vgc_tb.svh"

    // VGC drawing command opcodes
    localparam logic [7:0] CMD_PLOT   = 8'h01;
    localparam logic [7:0] CMD_UNPLOT = 8'h02;
    localparam logic [7:0] CMD_LINE   = 8'h03;
    localparam logic [7:0] CMD_CIRCLE = 8'h04;
    localparam logic [7:0] CMD_RECT   = 8'h05;
    localparam logic [7:0] CMD_FILL   = 8'h06;
    localparam logic [7:0] CMD_GCLS   = 8'h07;
    localparam logic [7:0] CMD_GCOLOR = 8'h08;
    localparam logic [7:0] CMD_PAINT  = 8'h09;
    localparam logic [7:0] CMD_SYSRESET = 8'h1F;

    // -----------------------------------------------------------------------
    // Convenience helpers — pack 16-bit coordinates into the VGC param regs.
    // Param layout: P0..P1 = x, P2..P3 = y, P4..P5 = x2, P6..P7 = y2.
    // -----------------------------------------------------------------------
    task automatic set_xy(input int x, input int y);
        write_param(0, x[7:0]);   write_param(1, x[15:8]);
        write_param(2, y[7:0]);   write_param(3, y[15:8]);
    endtask

    task automatic set_xy2(input int x, input int y);
        write_param(4, x[7:0]);   write_param(5, x[15:8]);
        write_param(6, y[7:0]);   write_param(7, y[15:8]);
    endtask

    task automatic gcls();
        write_cmd(CMD_GCLS);
        wait_cmd_done();
    endtask

    task automatic gcolor(input int c);
        write_param(0, c[7:0]);
        write_cmd(CMD_GCOLOR);
        wait_cmd_done();
    endtask

    task automatic plot(input int x, input int y);
        set_xy(x, y);
        write_cmd(CMD_PLOT);
        wait_cmd_done();
    endtask

    task automatic unplot(input int x, input int y);
        set_xy(x, y);
        write_cmd(CMD_UNPLOT);
        wait_cmd_done();
    endtask

    task automatic line(input int x0, input int y0, input int x1, input int y1);
        set_xy(x0, y0);
        set_xy2(x1, y1);
        write_cmd(CMD_LINE);
        wait_cmd_done();
    endtask

    task automatic rect(input int x0, input int y0, input int x1, input int y1);
        set_xy(x0, y0);
        set_xy2(x1, y1);
        write_cmd(CMD_RECT);
        wait_cmd_done();
    endtask

    task automatic fill_rect(input int x0, input int y0, input int x1, input int y1);
        set_xy(x0, y0);
        set_xy2(x1, y1);
        write_cmd(CMD_FILL);
        wait_cmd_done();
    endtask

    task automatic circle(input int cx, input int cy, input int r);
        set_xy(cx, cy);
        // radius lives in x2
        write_param(4, r[7:0]); write_param(5, r[15:8]);
        write_cmd(CMD_CIRCLE);
        wait_cmd_done();
    endtask

    task automatic paint(input int x, input int y);
        set_xy(x, y);
        write_cmd(CMD_PAINT);
        wait_cmd_done();
    endtask

    // -----------------------------------------------------------------------
    // Tests
    // -----------------------------------------------------------------------

    task automatic test_gcls_then_gcolor();
        $display("");
        $display("Test: GCLS + GCOLOR");
        // Plant a pixel directly so we have something to clear
        dut.gfx_inst.gfx_mem.mem[0]                         = 4'd7;
        dut.gfx_inst.gfx_mem.mem[320*199 + 319]             = 4'd7;
        dut.gfx_inst.gfx_mem.mem[320*100 + 160]             = 4'd7;
        gcls();
        check_eq("GCLS: origin cleared",          int'(peek_gfx(0, 0)),       0);
        check_eq("GCLS: bottom-right cleared",    int'(peek_gfx(319, 199)),   0);
        check_eq("GCLS: center cleared",          int'(peek_gfx(160, 100)),   0);

        gcolor(8);
        check_eq("GCOLOR: dut.gfx_color updated", int'(dut.gfx_color),        8);
    endtask

    task automatic test_plot_unplot();
        $display("");
        $display("Test: PLOT / UNPLOT");
        gcls();
        gcolor(5);

        plot(100, 50);
        check_eq("PLOT: pixel at (100,50) == 5", int'(peek_gfx(100, 50)), 5);
        check_eq("PLOT: adjacent pixel untouched",
                 int'(peek_gfx(101, 50)), 0);
        check_eq("PLOT: entire screen has exactly 1 set pixel",
                 count_gfx_pixels(0, 0, 319, 199), 1);

        unplot(100, 50);
        check_eq("UNPLOT: pixel cleared", int'(peek_gfx(100, 50)), 0);
        check_eq("UNPLOT: screen empty",
                 count_gfx_pixels(0, 0, 319, 199), 0);
    endtask

    task automatic test_line_horizontal();
        $display("");
        $display("Test: LINE horizontal (0,10)→(50,10)");
        gcls();
        gcolor(3);
        line(0, 10, 50, 10);
        // All 51 pixels on the row should be color 3
        begin
            int correct = 0;
            for (int x = 0; x <= 50; x++)
                if (peek_gfx(x, 10) == 4'd3) correct++;
            check_eq("horizontal line: all 51 pixels on row 10 set", correct, 51);
        end
        // No other pixels anywhere
        check_eq("horizontal line: no stray pixels elsewhere",
                 count_gfx_pixels(0, 0, 319, 199), 51);
    endtask

    task automatic test_line_vertical();
        $display("");
        $display("Test: LINE vertical (20,5)→(20,55)");
        gcls();
        gcolor(4);
        line(20, 5, 20, 55);
        begin
            int correct = 0;
            for (int y = 5; y <= 55; y++)
                if (peek_gfx(20, y) == 4'd4) correct++;
            check_eq("vertical line: 51 pixels set along col 20", correct, 51);
        end
        check_eq("vertical line: no strays",
                 count_gfx_pixels(0, 0, 319, 199), 51);
    endtask

    task automatic test_line_diagonal();
        $display("");
        $display("Test: LINE diagonal (0,0)→(30,30)");
        gcls();
        gcolor(6);
        line(0, 0, 30, 30);
        // Pure 45° line should hit exactly (x,x) for x=0..30 → 31 pixels
        begin
            int correct = 0;
            for (int i = 0; i <= 30; i++)
                if (peek_gfx(i, i) == 4'd6) correct++;
            check_eq("45° diag: pixels on y=x", correct, 31);
        end
        check_eq("45° diag: no strays",
                 count_gfx_pixels(0, 0, 319, 199), 31);
    endtask

    task automatic test_line_shallow_slope();
        $display("");
        $display("Test: LINE shallow slope (0,0)→(100,20)");
        gcls();
        gcolor(7);
        line(0, 0, 100, 20);
        // Bresenham result: exactly 101 pixels (dx=100 dominant → 101 x-steps)
        check_eq("shallow line: 101 pixels total",
                 count_gfx_pixels(0, 0, 319, 199), 101);
        // Endpoints must both be set
        check_eq("shallow line: start (0,0) set",    int'(peek_gfx(0, 0)),     7);
        check_eq("shallow line: end (100,20) set",   int'(peek_gfx(100, 20)),  7);
    endtask

    task automatic test_line_reversed_endpoints();
        $display("");
        $display("Test: LINE reversed — each quadrant direction");
        // For each reversal, draw the line, check both endpoints set.
        // Pixel counts depend on octant so we just spot-check endpoints +
        // cardinal direction correctness.
        gcls(); gcolor(2);
        line(100, 50, 0, 10);   // both x and y decreasing
        check_eq("rev XY: start endpoint (100,50)", int'(peek_gfx(100, 50)), 2);
        check_eq("rev XY: end endpoint (0,10)",     int'(peek_gfx(  0, 10)), 2);

        gcls(); gcolor(3);
        line(100, 10, 0, 50);   // x decreasing, y increasing
        check_eq("rev X:  start endpoint (100,10)", int'(peek_gfx(100, 10)), 3);
        check_eq("rev X:  end endpoint (0,50)",     int'(peek_gfx(  0, 50)), 3);

        gcls(); gcolor(4);
        line(0, 50, 100, 10);   // x increasing, y decreasing
        check_eq("rev Y:  start endpoint (0,50)",   int'(peek_gfx(  0, 50)), 4);
        check_eq("rev Y:  end endpoint (100,10)",   int'(peek_gfx(100, 10)), 4);

        // Pure vertical reversed
        gcls(); gcolor(5);
        line(20, 55, 20, 5);
        check_eq("vert rev: (20,55)", int'(peek_gfx(20, 55)), 5);
        check_eq("vert rev: (20, 5)", int'(peek_gfx(20,  5)), 5);

        // Pure horizontal reversed
        gcls(); gcolor(6);
        line(50, 10, 0, 10);
        check_eq("horz rev: (50,10)", int'(peek_gfx(50, 10)), 6);
        check_eq("horz rev: ( 0,10)", int'(peek_gfx( 0, 10)), 6);

        // Reversed 45° diagonal
        gcls(); gcolor(7);
        line(30, 30, 0, 0);
        check_eq("diag rev: (30,30)", int'(peek_gfx(30, 30)), 7);
        check_eq("diag rev: ( 0, 0)", int'(peek_gfx( 0,  0)), 7);
        // Should hit same 31 pixels as forward
        check_eq("diag rev: 31 pixels total",
                 count_gfx_pixels(0, 0, 319, 199), 31);
    endtask

    task automatic test_rect_outline();
        $display("");
        $display("Test: RECT outline only (10,10)→(30,20)");
        gcls();
        gcolor(9);
        rect(10, 10, 30, 20);
        // Four corners set
        check_eq("RECT: corner (10,10)", int'(peek_gfx(10, 10)), 9);
        check_eq("RECT: corner (30,10)", int'(peek_gfx(30, 10)), 9);
        check_eq("RECT: corner (10,20)", int'(peek_gfx(10, 20)), 9);
        check_eq("RECT: corner (30,20)", int'(peek_gfx(30, 20)), 9);
        // Interior should be empty
        check_eq("RECT: interior pixel (20,15) empty", int'(peek_gfx(20, 15)), 0);
        // Top edge fully drawn (10..30 inclusive = 21 pixels)
        begin
            int n = 0;
            for (int x = 10; x <= 30; x++)
                if (peek_gfx(x, 10) == 4'd9) n++;
            check_eq("RECT: top edge 21 pixels", n, 21);
        end
        // Perimeter count = 2*(w+h)-4 overlap = 2*(21+11)-4 = 60
        check_eq("RECT: perimeter == 60 pixels",
                 count_gfx_pixels(0, 0, 319, 199), 60);
    endtask

    task automatic test_fill_solid();
        $display("");
        $display("Test: FILL solid (50,50)→(70,60)");
        gcls();
        gcolor(11);
        fill_rect(50, 50, 70, 60);
        // (70-50+1) * (60-50+1) = 21 * 11 = 231 pixels
        check_eq("FILL: interior completely set",
                 count_gfx_pixels(50, 50, 70, 60), 231);
        check_eq("FILL: corner (50,50)", int'(peek_gfx(50, 50)), 11);
        check_eq("FILL: corner (70,60)", int'(peek_gfx(70, 60)), 11);
        // Everything outside must be empty
        check_eq("FILL: pixel just left of region",  int'(peek_gfx(49, 55)), 0);
        check_eq("FILL: pixel just right of region", int'(peek_gfx(71, 55)), 0);
    endtask

    task automatic test_circle_basic();
        $display("");
        $display("Test: CIRCLE at (160,100) radius 20");
        gcls();
        gcolor(12);
        circle(160, 100, 20);
        // Center must NOT be set (circle is hollow)
        check_eq("CIRCLE: center empty", int'(peek_gfx(160, 100)), 0);
        // Cardinal extreme points must be set
        check_eq("CIRCLE: east  pixel (180,100)", int'(peek_gfx(180, 100)), 12);
        check_eq("CIRCLE: west  pixel (140,100)", int'(peek_gfx(140, 100)), 12);
        check_eq("CIRCLE: north pixel (160, 80)", int'(peek_gfx(160,  80)), 12);
        check_eq("CIRCLE: south pixel (160,120)", int'(peek_gfx(160, 120)), 12);
        // Pixel just inside the perimeter (one step toward center) should NOT be set
        check_eq("CIRCLE: interior (161,100) empty",
                 int'(peek_gfx(161, 100)), 0);
        // Perimeter pixel count should be roughly 2*pi*r ≈ 126; give a generous
        // band [100..160] to allow for Bresenham step counting.
        begin
            int n = count_gfx_pixels(0, 0, 319, 199);
            check("CIRCLE: pixel count in [100..160]", n >= 100 && n <= 160);
            $display("  info: CIRCLE r=20 pixel count = %0d (expected ~126)", n);
        end
    endtask

    task automatic test_paint_flood_fill();
        $display("");
        $display("Test: PAINT floods inside RECT outline");
        gcls();
        gcolor(2);
        rect(30, 30, 50, 40);  // outline color 2
        gcolor(5);
        paint(40, 35);          // seed inside
        // Inside pixels should now be color 5
        check_eq("PAINT: seed pixel is color 5", int'(peek_gfx(40, 35)), 5);
        // Outline retains color 2
        check_eq("PAINT: outline corner still 2", int'(peek_gfx(30, 30)), 2);
        // A pixel outside the rect (beyond outline) should still be 0
        check_eq("PAINT: outside pixel untouched", int'(peek_gfx(10, 10)), 0);
        // Interior count: (50-30-1)*(40-30-1) = 19*9 = 171 pixels color 5
        begin
            int n = 0;
            for (int y = 31; y <= 39; y++)
                for (int x = 31; x <= 49; x++)
                    if (peek_gfx(x, y) == 4'd5) n++;
            check_eq("PAINT: interior 171 pixels color 5", n, 171);
        end
    endtask

    task automatic test_gcls_full_coverage();
        int cleared;
        $display("");
        $display("Test: GCLS clears EVERY pixel of 320x200 gfx RAM");
        // Pre-dirty every pixel
        for (int i = 0; i < 64000; i++)
            dut.gfx_inst.gfx_mem.mem[i] = 4'hF;
        step(2);
        gcls();
        step(10);
        cleared = 0;
        for (int i = 0; i < 64000; i++)
            if (dut.gfx_inst.gfx_mem.mem[i] == 4'h0) cleared++;
        check_eq("GCLS: all 64000 pixels == 0", cleared, 64000);
    endtask

    task automatic test_out_of_bounds_plot();
        $display("");
        $display("Test: out-of-bounds PLOT does not corrupt screen");
        gcls();
        gcolor(14);
        // Plot beyond (320,200) — should be a no-op / clipped
        plot(500, 500);
        plot(320, 0);       // exactly 1 beyond right edge
        plot(0, 200);       // exactly 1 below bottom edge
        check_eq("OOB: screen still empty", count_gfx_pixels(0, 0, 319, 199), 0);
    endtask

    task automatic wait_reset_clear_done();
        int timeout;
        timeout = 0;
        repeat(4) @(posedge clk);
        while (!(dut.vgc_reset_pending || !vgc_rdy) && timeout < 1000000) begin
            @(posedge clk);
            timeout++;
        end
        while ((dut.vgc_reset_pending || !vgc_rdy) && timeout < 2000000) begin
            @(posedge clk);
            timeout++;
        end
        check("reset scrubber released RDY", vgc_rdy && !dut.vgc_reset_pending);
        repeat(4) @(posedge clk);
    endtask

    task automatic test_sysreset_clears_vgc_state();
        logic [7:0] b;
        $display("");
        $display("Test: SYSRESET clears VGC registers and BRAM-backed state");

        bus_write(REG_MODE_A, 8'd3);
        bus_write(REG_FGCOL_A, 8'd4);
        vram_write(VPLANE_CHAR_A, 0, 8'h58);
        vram_write(VPLANE_COLOR_A, 0, 8'h04);
        vram_write(VPLANE_GFX_A, 1234, 8'h0F);
        vram_write(VPLANE_SPRITE_A, 0, 8'hFF);

        write_cmd(CMD_SYSRESET);
        wait_reset_clear_done();

        check_eq("SYSRESET: mode reset to text", int'(dut.mode), 0);
        check_eq("SYSRESET: char[0] cleared to space", int'(peek_char(0)), 8'h20);
        check_eq("SYSRESET: color[0] reset to fg", int'(peek_color(0)), 8'h0F);
        check_eq("SYSRESET: gfx pixel cleared", int'(dut.gfx_inst.gfx_mem.mem[1234]), 0);
        check_eq("SYSRESET: pending sprite byte cleared", int'(peek_spr_pending_addr(11'd0)), 0);
        check_eq("SYSRESET: active sprite byte cleared", int'(peek_spr_active_addr(11'd0)), 0);

        vram_read(VPLANE_GFX_A, 1234, b);
        check_eq("SYSRESET: VRAM read sees cleared gfx", int'(b), 0);
    endtask

    task automatic test_sysreset_clears_all_stale_vmem();
        int bad_gfx;
        int bad_char;
        int bad_color;
        int bad_sprite;
        int bad_tile;
        int bad_nt;
        int bad_attr;

        $display("");
        $display("Test: SYSRESET removes stale data from every VGC memory plane");

        for (int i = 0; i < 4000; i++) begin
            dut.text_inst.char_mem.mem[i] = 8'h53;
            dut.text_inst.color_mem.mem[i] = 8'h06;
        end

        for (int i = 0; i < 64000; i++)
            dut.gfx_inst.gfx_mem.mem[i] = 4'hD;

        for (int i = 0; i < 2048; i++) begin
            dut.sprite_inst.spr_mem0.mem[i] = 8'hA5;
            dut.sprite_inst.spr_mem1.mem[i] = 8'h5A;
        end

        for (int i = 0; i < 32768; i++)
            dut.tile_inst.tile_data_ram.mem[i] = 8'hC3;

        for (int i = 0; i < 4096; i++) begin
            dut.tile_inst.nametable_ram.mem[i] = 8'h77;
            dut.tile_inst.attr_table_ram.mem[i] = 8'h88;
        end

        write_cmd(CMD_SYSRESET);
        wait_reset_clear_done();

        bad_char = 0;
        bad_color = 0;
        for (int i = 0; i < 4000; i++) begin
            if (dut.text_inst.char_mem.mem[i] != 8'h20) bad_char++;
            if (dut.text_inst.color_mem.mem[i] != 8'h0F) bad_color++;
        end

        bad_gfx = 0;
        for (int i = 0; i < 64000; i++)
            if (dut.gfx_inst.gfx_mem.mem[i] != 4'h0) bad_gfx++;

        bad_sprite = 0;
        for (int i = 0; i < 2048; i++) begin
            if (dut.sprite_inst.spr_mem0.mem[i] != 8'h00) bad_sprite++;
            if (dut.sprite_inst.spr_mem1.mem[i] != 8'h00) bad_sprite++;
        end

        bad_tile = 0;
        for (int i = 0; i < 32768; i++)
            if (dut.tile_inst.tile_data_ram.mem[i] != 8'h00) bad_tile++;

        bad_nt = 0;
        bad_attr = 0;
        for (int i = 0; i < 4096; i++) begin
            if (dut.tile_inst.nametable_ram.mem[i] != 8'h00) bad_nt++;
            if (dut.tile_inst.attr_table_ram.mem[i] != 8'h00) bad_attr++;
        end

        check_eq("SYSRESET: no stale text chars remain", bad_char, 0);
        check_eq("SYSRESET: no stale text colors remain", bad_color, 0);
        check_eq("SYSRESET: no stale bitmap pixels remain", bad_gfx, 0);
        check_eq("SYSRESET: no stale sprite shape bytes remain", bad_sprite, 0);
        check_eq("SYSRESET: no stale tile pattern bytes remain", bad_tile, 0);
        check_eq("SYSRESET: no stale tile map bytes remain", bad_nt, 0);
        check_eq("SYSRESET: no stale tile attr bytes remain", bad_attr, 0);
    endtask

    // -----------------------------------------------------------------------
    // Runner
    // -----------------------------------------------------------------------
    initial begin
        $display("=== VGC Graphics-Layer Tests ===");
        do_reset();

        test_gcls_then_gcolor();
        test_plot_unplot();
        test_line_horizontal();
        test_line_vertical();
        test_line_diagonal();
        test_line_shallow_slope();
        test_line_reversed_endpoints();
        test_rect_outline();
        test_fill_solid();
        test_circle_basic();
        test_paint_flood_fill();
        test_gcls_full_coverage();
        test_out_of_bounds_plot();
        test_sysreset_clears_vgc_state();
        test_sysreset_clears_all_stale_vmem();

        summary();
        $finish;
    end

    initial begin
        #2000000000;  // 2s sim time global timeout
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
