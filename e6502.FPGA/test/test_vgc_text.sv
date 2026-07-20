// Text-layer RTL tests for the VGC.
// Goal: isolate the "7-row mirror" typing bug in Verilator before touching hardware.
//
// Strategy: drive the VGC directly (no CPU, no EhBASIC), poke REG_CHAROUT and
// VRAM port, then read char_mem.mem back through Verilator hierarchy. Any bug
// the user sees on hardware should be reproducible here OR be provably above
// the VGC (i.e. in ehbasic / screen editor).

`timescale 1ns/1ps

module test_vgc_text;

    `include "vgc_tb.svh"

    // -----------------------------------------------------------------------
    // Tests
    // -----------------------------------------------------------------------

    // T1: VDC-style VRAM port writes and reads char RAM.
    task automatic test_char_ram_roundtrip();
        logic [7:0] rb;
        $display("");
        $display("Test: char RAM VRAM-port round-trip");
        // Write 0x41 ('A') through 0x48 ('H') to the first 8 cells
        for (int i = 0; i < 8; i++)
            vram_write(VPLANE_CHAR_A, i, 8'h41 + 8'(i));
        step(4);
        for (int i = 0; i < 8; i++)
            check_eq($sformatf("char_mem[%0d] via hierarchy", i),
                     peek_char(i), 8'h41 + i);
        // Same 8 cells via VDATA reads
        for (int i = 0; i < 8; i++) begin
            vram_read(VPLANE_CHAR_A, i, rb);
            check_eq($sformatf("vram_rdata[%0d]", i), rb, 8'h41 + i);
        end
    endtask

    // T2: writing to REG_CHAROUT with a fresh cursor places the char at
    // the expected char-RAM address and advances cursor_x.
    task automatic test_regcharout_basic();
        $display("");
        $display("Test: REG_CHAROUT places char at cursor");
        // Form-feed clears screen + resets cursor + scroll_offset
        type_char(8'h0C);
        wait_cmd_done();
        step(4);
        check_eq("cursor_x after FF", dut.cursor_x, 0);
        check_eq("cursor_y after FF", dut.cursor_y, 0);
        check_eq("scroll_offset after FF", dut.scroll_offset, 0);

        type_char(8'h41); // 'A'
        check_eq("char_mem[0] == 'A'",     peek_char(0),     8'h41);
        check_eq("cursor_x after one char", dut.cursor_x,    1);
        check_eq("cursor_y after one char", dut.cursor_y,    0);

        type_char(8'h42); // 'B'
        check_eq("char_mem[1] == 'B'",     peek_char(1),     8'h42);
        check_eq("cursor_x after two chars", dut.cursor_x,   2);
    endtask

    task automatic test_regcharout_text_attributes();
        $display("");
        $display("Test: REG_CHAROUT writes packed colors and text attrs");
        type_char(8'h0C);
        wait_cmd_done();
        step(4);

        bus_write(REG_BGCOL_A, 8'd4);
        bus_write(REG_FGCOL_A, 8'd2);
        step(2);
        type_char(8'h41);
        check_eq("normal transparent color attr fg=2", peek_color(0), 8'h02);
        check_eq("normal text attr has transparent bg", peek_text_attr(0), 8'h08);

        bus_write(REG_TEXTFLAGS_A, 8'h01);
        step(2);
        type_char(8'h42);
        check_eq("reverse default swaps transparent bg/fg", peek_color(1), 8'h20);

        bus_write(REG_TEXTREV_A, 8'hA3);
        bus_write(REG_TEXTFLAGS_A, 8'h03);
        step(2);
        type_char(8'h43);
        check_eq("explicit reverse attr", peek_color(2), 8'hA3);

        bus_write(REG_TEXTFLAGS_A, 8'h04);
        step(2);
        type_char(8'h44);
        check_eq("flash attr recorded with transparent bg", peek_text_attr(3), 8'h09);

        bus_write(REG_TEXTFLAGS_A, 8'h08);
        step(2);
        type_char(8'h45);
        check_eq("bold attr recorded with transparent bg", peek_text_attr(4), 8'h0C);

        bus_write(REG_TEXTFLAGS_A, 8'h00);
        step(2);
        type_char(8'h46);
        check_eq("style flags off leaves only transparent bg", peek_text_attr(5), 8'h08);
    endtask

    task automatic test_font_lookup_uses_delayed_scanline();
        $display("");
        $display("Test: font lookup uses the delayed scanline that matches char_b_dout");

        dut.text_inst.char_mem.mem[0] = 8'h01;
        dut.text_inst.font_mem.mem[{8'h01, 3'd0}] = 8'h11;
        dut.text_inst.font_mem.mem[{8'h01, 3'd1}] = 8'h22;

        force dut.real_row = 6'd0;
        force dut.text_col = 7'd0;
        force dut.font_slot = 3'd0;
        force dut.font_line = 3'd1;
        force dut.font_line_d1 = 3'd0;
        step(1);
        #1;
        check_eq("font address uses delayed line bits",
                 dut.text_inst.font_b_addr[2:0], 3'd0);
        step(1);

        check_eq("font data came from delayed line 0, not current line 1",
                 dut.text_inst.font_b_dout, 8'h11);

        release dut.real_row;
        release dut.text_col;
        release dut.font_slot;
        release dut.font_line;
        release dut.font_line_d1;
    endtask

    task automatic test_scrollmixed_command_scrolls_all_planes();
        $display("");
        $display("Test: VCMD_SCROLLMIXED scrolls text/color/attr/gfx together");

        dut.text_inst.char_mem.mem[2 * COLS_TB + 1] = 8'h41;
        dut.text_inst.color_mem.mem[2 * COLS_TB + 1] = 8'h12;
        dut.text_inst.attr_mem.mem[2 * COLS_TB + 1] = 8'h01;
        dut.text_inst.char_mem.mem[3 * COLS_TB + 1] = 8'h42;
        dut.text_inst.color_mem.mem[3 * COLS_TB + 1] = 8'h34;
        dut.text_inst.attr_mem.mem[3 * COLS_TB + 1] = 8'h04;
        dut.gfx_inst.gfx_mem.mem[5 * 320 + 4] = 4'hA;
        dut.gfx_inst.gfx_mem.mem[6 * 320 + 4] = 4'hB;
        step(2);

        write_param(0, 8'd1);  // text left
        write_param(1, 8'd2);  // text top
        write_param(2, 8'd2);  // text width
        write_param(3, 8'd2);  // text height
        write_param(4, 8'd1);  // text rows
        write_param(5, 8'd7);  // gfx fill
        write_param(6, 8'd4);  // gfx left low
        write_param(7, 8'd0);  // gfx left high
        write_param(8, 8'd5);  // gfx top
        write_param(9, 8'd2);  // gfx width low
        write_param(10, 8'd0); // gfx width high
        write_param(11, 8'd2); // gfx height
        write_param(12, 8'd1); // gfx rows
        write_param(13, 8'hE1);
        write_param(14, 8'h01);
        write_cmd(CMD_SCROLLMIXED_A);
        wait_cmd_done();

        check_eq("text char moved from row 3 to row 2", peek_char_cell(1, 2), 8'h42);
        check_eq("text color moved with char", peek_color(2 * COLS_TB + 1), 8'h34);
        check_eq("text attr moved with char", peek_text_attr(2 * COLS_TB + 1), 8'h04);
        check_eq("vacated text char cleared", peek_char_cell(1, 3), 8'h20);
        check_eq("vacated text color filled", peek_color(3 * COLS_TB + 1), 8'hE1);
        check_eq("vacated text attr filled", peek_text_attr(3 * COLS_TB + 1), 8'h01);
        check_eq("gfx pixel moved upward", peek_gfx(4, 5), 4'hB);
        check_eq("vacated gfx pixel filled", peek_gfx(4, 6), 4'h7);
    endtask

    task automatic test_regcmd_read_is_busy_while_command_write_is_pending();
        $display("");
        $display("Test: REG_CMD reports busy while a command write is in the write slice");

        write_param(0, 8'd11);
        write_param(1, 8'd9);
        write_param(2, 8'd58);
        write_param(3, 8'd40);
        write_param(4, 8'd1);
        write_param(5, 8'h0F);
        write_param(6, 8'd43);
        write_param(7, 8'd0);
        write_param(8, 8'd39);
        write_param(9, 8'd234);
        write_param(10, 8'd0);
        write_param(11, 8'd160);
        write_param(12, 8'd4);
        write_param(13, 8'hF0);
        write_param(14, 8'h00);

        @(posedge clk);
        cpu_addr  <= REG_CMD_A;
        cpu_wdata <= CMD_SCROLLMIXED_A;
        cpu_we    <= 1;
        cpu_re    <= 0;
        cpu_ce    <= 1;
        @(posedge clk);
        cpu_addr  <= REG_CMD_A;
        cpu_wdata <= 8'h00;
        cpu_we    <= 0;
        cpu_re    <= 1;
        cpu_ce    <= 1;
        #1;
        check("REG_CMD read sees pending command write as busy", cpu_rdata[0] == 1'b1);
        @(posedge clk);
        cpu_re <= 0;
        cpu_ce <= 0;
        wait_cmd_done();
    endtask

    task automatic issue_scrollmixed(
        input int text_left,
        input int text_top,
        input int text_width,
        input int text_height,
        input int text_rows,
        input int gfx_left,
        input int gfx_top,
        input int gfx_width,
        input int gfx_height,
        input int gfx_rows
    );
        write_param(0, 8'(text_left));
        write_param(1, 8'(text_top));
        write_param(2, 8'(text_width));
        write_param(3, 8'(text_height));
        write_param(4, 8'(text_rows));
        write_param(5, 8'h0);
        write_param(6, 8'(gfx_left & 8'hFF));
        write_param(7, 8'((gfx_left >> 8) & 8'h01));
        write_param(8, 8'(gfx_top));
        write_param(9, 8'(gfx_width & 8'hFF));
        write_param(10, 8'((gfx_width >> 8) & 8'h01));
        write_param(11, 8'(gfx_height));
        write_param(12, 8'(gfx_rows));
        write_param(13, 8'h0F);
        write_param(14, 8'h00);
        write_cmd(CMD_SCROLLMIXED_A);
        wait_cmd_done();
    endtask

    task automatic test_scrollmixed_zork_zero_body_region();
        localparam int LEFT = 11;
        localparam int TOP = 9;
        localparam int WIDTH = 58;
        localparam int HEIGHT = 40;
        $display("");
        $display("Test: VCMD_SCROLLMIXED preserves the real Zork Zero body region");

        for (int row = TOP; row < TOP + HEIGHT; row++) begin
            for (int col = LEFT; col < LEFT + WIDTH; col++) begin
                dut.text_inst.char_mem.mem[row * COLS_TB + col] = 8'h41 + 8'((row - TOP) % 26);
                dut.text_inst.color_mem.mem[row * COLS_TB + col] = 8'hF0;
                dut.text_inst.attr_mem.mem[row * COLS_TB + col] = 8'h00;
            end
        end
        dut.text_inst.char_mem.mem[1 * COLS_TB + 8] = 8'h4B; // status/header row must not move.
        // Kitchen-like icon block: yellow frame/art over gray paper inside the
        // real Zork Zero gfx scroll rect. The paper pixels are the ones that
        // visibly mutate when the mixed scroll path corrupts inline icons.
        for (int y = 0; y < 21; y++) begin
            for (int x = 0; x < 21; x++)
                dut.gfx_inst.gfx_mem.mem[(153 + y) * 320 + 43 + x] = 4'hF;
        end
        for (int x = 0; x < 21; x++) begin
            dut.gfx_inst.gfx_mem.mem[153 * 320 + 43 + x] = 4'hB;
            dut.gfx_inst.gfx_mem.mem[173 * 320 + 43 + x] = 4'hB;
        end
        for (int y = 0; y < 21; y++) begin
            dut.gfx_inst.gfx_mem.mem[(153 + y) * 320 + 43] = 4'hB;
            dut.gfx_inst.gfx_mem.mem[(153 + y) * 320 + 63] = 4'hB;
        end
        dut.gfx_inst.gfx_mem.mem[158 * 320 + 48] = 4'hB;
        dut.gfx_inst.gfx_mem.mem[162 * 320 + 54] = 4'hB;
        dut.gfx_inst.gfx_mem.mem[168 * 320 + 58] = 4'hB;
        step(2);

        for (int i = 0; i < 8; i++)
            issue_scrollmixed(LEFT, TOP, WIDTH, HEIGHT, 1, 43, 39, 234, 160, 4);

        check_eq("header row survived body scroll", peek_char_cell(8, 1), 8'h4B);
        check_eq("left gutter before body is untouched", peek_char_cell(LEFT - 1, TOP), 8'h20);
        check_eq("body top row shifted by 8 rows", peek_char_cell(LEFT, TOP), 8'h49);
        check_eq("body middle row still contains shifted text", peek_char_cell(LEFT + 10, TOP + 10), 8'h53);
        check_eq("vacated body bottom row cleared", peek_char_cell(LEFT, TOP + HEIGHT - 1), 8'h20);
        check_eq("right gutter after body is untouched", peek_char_cell(LEFT + WIDTH, TOP), 8'h20);
        check_eq("kitchen icon top border scrolled with gfx", peek_gfx(43, 121), 4'hB);
        check_eq("kitchen icon bottom border scrolled with gfx", peek_gfx(63, 141), 4'hB);
        check_eq("kitchen icon interior paper scrolled unchanged", peek_gfx(50, 126), 4'hF);
        check_eq("kitchen icon interior art scrolled unchanged", peek_gfx(54, 130), 4'hB);
        check_eq("kitchen icon old location was cleared to fill color", peek_gfx(43, 153), 4'h0);
    endtask

    // Regression for the Zork Zero "text disappears on hardware scroll" bug.
    // nservers.c do_read_screen() reads the whole char plane through the dbg
    // bridge; when one of those reads landed DURING an atomic SCROLLMIXED it
    // stole port A from the scroll's source read (char/color/attr had no
    // cmd_*_re priority, unlike gfx's cmd_gfx_re), so the copied rows came back
    // as the polled cell instead of the real text. Reproduced by holding a
    // char-plane debug read asserted across the whole scroll: fails before the
    // mux fix (body == POISON), passes after (body keeps its shifted text).
    task automatic test_scrollmixed_survives_concurrent_host_char_read();
        localparam int LEFT = 11;
        localparam int TOP = 9;
        localparam int WIDTH = 58;
        localparam int HEIGHT = 40;
        localparam logic [7:0] POISON = 8'h2A; // '*' — must never end up in the body
        $display("");
        $display("Test: VCMD_SCROLLMIXED body survives a concurrent host char read");

        for (int row = TOP; row < TOP + HEIGHT; row++) begin
            for (int col = LEFT; col < LEFT + WIDTH; col++) begin
                dut.text_inst.char_mem.mem[row * COLS_TB + col] = 8'h41 + 8'((row - TOP) % 26);
                dut.text_inst.color_mem.mem[row * COLS_TB + col] = 8'hF0;
                dut.text_inst.attr_mem.mem[row * COLS_TB + col] = 8'h00;
            end
        end
        // The cell the hijacked read lands on (row 0, well outside the scroll).
        dut.text_inst.char_mem.mem[0] = POISON;
        step(2);

        // Hold a char-plane debug read asserted for the entire scroll, mimicking
        // the host hammering do_read_screen while the game scrolls the window.
        dbg_vmem_space <= VPLANE_CHAR_A;
        dbg_vmem_addr  <= 17'd0;
        dbg_vmem_re    <= 1'b1;
        @(posedge clk);

        for (int i = 0; i < 8; i++)
            issue_scrollmixed(LEFT, TOP, WIDTH, HEIGHT, 1, 43, 39, 234, 160, 4);

        dbg_vmem_re <= 1'b0;
        step(2);

        check_eq("body top row shifted by 8 rows despite host poll", peek_char_cell(LEFT, TOP), 8'h49);
        check_eq("body middle row still holds shifted text", peek_char_cell(LEFT + 10, TOP + 10), 8'h53);
        check("body top not overwritten by the polled cell", peek_char_cell(LEFT, TOP) != POISON);
        check("body middle not overwritten by the polled cell", peek_char_cell(LEFT + 10, TOP + 10) != POISON);
    endtask

    task automatic test_debug_writes_parameters_and_scrollmixed_command();
        logic [7:0] rb;
        localparam int LEFT = 11;
        localparam int TOP = 9;
        localparam int WIDTH = 58;
        localparam int HEIGHT = 40;
        $display("");
        $display("Test: debug writes update VGC params and can issue VCMD_SCROLLMIXED");

        dbg_vmem_write(VPLANE_CHAR_A, (TOP + 1) * COLS_TB + LEFT, 8'h51);
        dbg_vmem_write(VPLANE_COLOR_A, (TOP + 1) * COLS_TB + LEFT, 8'hF0);
        dbg_vmem_write(VPLANE_TEXTATTR_A, (TOP + 1) * COLS_TB + LEFT, 8'h04);

        dbg_write(REG_PARAM0_A + 0, 8'(LEFT));
        dbg_write(REG_PARAM0_A + 1, 8'(TOP));
        dbg_write(REG_PARAM0_A + 2, 8'(WIDTH));
        dbg_write(REG_PARAM0_A + 3, 8'(HEIGHT));
        dbg_write(REG_PARAM0_A + 4, 8'd1);
        dbg_write(REG_PARAM0_A + 5, 8'h0F);
        dbg_write(REG_PARAM0_A + 6, 8'd43);
        dbg_write(REG_PARAM0_A + 7, 8'd0);
        dbg_write(REG_PARAM0_A + 8, 8'd39);
        dbg_write(REG_PARAM0_A + 9, 8'd234);
        dbg_write(REG_PARAM0_A + 10, 8'd0);
        dbg_write(REG_PARAM0_A + 11, 8'd160);
        dbg_write(REG_PARAM0_A + 12, 8'd4);
        dbg_write(REG_PARAM0_A + 13, 8'hF0);
        dbg_write(REG_PARAM0_A + 14, 8'h00);

        dbg_read(REG_PARAM0_A + 0, rb);
        check_eq("debug P0 readback", rb, LEFT);
        dbg_read(REG_PARAM0_A + 4, rb);
        check_eq("debug P4 readback", rb, 1);
        dbg_read(REG_PARAM0_A + 14, rb);
        check_eq("debug P14 readback", rb, 0);

        dbg_write(REG_CMD_A, CMD_SCROLLMIXED_A);
        wait_cmd_done();

        check_eq("debug mixed scroll moved char into body top", peek_char_cell(LEFT, TOP), 8'h51);
        check_eq("debug mixed scroll moved color with char", peek_color(TOP * COLS_TB + LEFT), 8'hF0);
        check_eq("debug mixed scroll moved attr with char", peek_text_attr(TOP * COLS_TB + LEFT), 8'h04);
        check_eq("debug mixed scroll filled vacated row", peek_char_cell(LEFT, TOP + HEIGHT - 1), 8'h20);
    endtask

    // T3: typing a full row advances cursor_y to the next row and the next
    // char lands at the start of that row (col 0, row 1).
    task automatic test_row_wrap();
        $display("");
        $display("Test: row wrap at col 80");
        type_char(8'h0C);
        wait_cmd_done();
        step(4);
        for (int i = 0; i < 80; i++)
            type_char(8'h2E);  // '.'
        check_eq("cursor_x wrapped to 0", dut.cursor_x, 0);
        check_eq("cursor_y advanced to 1", dut.cursor_y, 1);
        check_eq("scroll_offset still 0",  dut.scroll_offset, 0);
        check_eq("row 0 col 0",  peek_char_cell(0, 0),  8'h2E);
        check_eq("row 0 col 79", peek_char_cell(79, 0), 8'h2E);

        type_char(8'h58);  // 'X'
        check_eq("next char lands at row 1 col 0", peek_char_cell(0, 1), 8'h58);
        check_eq("row 1 col 1 untouched",           peek_char_cell(1, 1), 8'h20);
    endtask

    // T4: LF starts at column 0 of the next row without touching scroll_offset
    // (until we reach the bottom).
    task automatic test_newline_no_scroll();
        $display("");
        $display("Test: newline before bottom row does not scroll");
        type_char(8'h0C);
        wait_cmd_done();
        step(4);
        type_char(8'h58);  // prove LF resets a non-zero column
        for (int i = 0; i < 5; i++) begin
            type_char(8'h0A);  // LF
            // The VGC advances cursor_y immediately (no cmd state).
            step(2);
        end
        check_eq("cursor_x after LF",        dut.cursor_x,     0);
        check_eq("cursor_y after 5 LFs",     dut.cursor_y,     5);
        check_eq("scroll_offset still 0",    dut.scroll_offset, 0);
    endtask

    // T5: newline on the bottom row scrolls — scroll_offset advances by one
    // and the new bottom row is cleared to spaces.
    task automatic test_newline_at_bottom_scrolls();
        $display("");
        $display("Test: newline at bottom row triggers scroll");
        type_char(8'h0C);
        wait_cmd_done();
        step(4);
        // Park cursor on the bottom row by poking cursor_y directly.
        bus_write(REG_CURSORY_A, 8'(ROWS_TB - 1));
        step(4);
        // Now a newline should not advance cursor_y — it should scroll.
        type_char(8'h0A);
        step(400);  // let scroll_pending clear the new bottom row
        check_eq("cursor_y pinned at bottom", dut.cursor_y,    ROWS_TB - 1);
        check_eq("scroll_offset advanced",   dut.scroll_offset, 1);
        // The "new bottom row" in char RAM (where writes at cursor_y=24
        // will land) is (24 + 1) mod 25 = 0. Its first cell should have
        // been cleared to space (0x20) by the scroll_pending machine.
        check_eq("wrapped bottom cleared col 0",  peek_char_cell(0, 0),   8'h20);
        check_eq("wrapped bottom cleared col 79", peek_char_cell(79, 0),  8'h20);
    endtask

    // T6: typed-char row landing after scroll. After N scrolls, a char typed
    // at cursor_y=K should land at char-RAM row (K + scroll_offset) mod ROWS.
    // If it lands anywhere else, we've caught the bug.
    task automatic test_scroll_offset_write_alignment();
        int so;
        int expected_row;
        int x;
        $display("");
        $display("Test: typed chars land at cursor_y + scroll_offset mod ROWS");
        type_char(8'h0C);
        wait_cmd_done();
        step(4);
        // Scroll by 7 via explicit bottom-row newlines.
        bus_write(REG_CURSORY_A, 8'(ROWS_TB - 1));
        step(4);
        for (int i = 0; i < 7; i++) begin
            type_char(8'h0A);
            step(400);
        end
        so = dut.scroll_offset;
        check_eq("scroll_offset after 7 bottom newlines", so, 7);

        // Move cursor to (0, 0) and type 'Z'. The byte should land at
        // char RAM row (0 + scroll_offset) mod ROWS = scroll_offset.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h00);
        step(4);
        type_char(8'h5A);  // 'Z'
        expected_row = so % ROWS_TB;
        check_eq("'Z' landed at scroll_offset row col 0",
                 peek_char_cell(0, expected_row), 8'h5A);
        // No row other than expected_row should have 'Z' in col 0.
        x = 0;
        for (int r = 0; r < ROWS_TB; r++)
            if (r != expected_row && peek_char_cell(0, r) == 8'h5A)
                x++;
        check_eq("no stray 'Z' mirrors", x, 0);

        // Now type at (0, 5): expected row = (5 + scroll_offset) mod ROWS.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h05);
        step(4);
        type_char(8'h59);  // 'Y'
        expected_row = (5 + so) % ROWS_TB;
        check_eq("'Y' landed at (5+scroll_offset) mod ROWS col 0",
                 peek_char_cell(0, expected_row), 8'h59);
        x = 0;
        for (int r = 0; r < ROWS_TB; r++)
            if (r != expected_row && peek_char_cell(0, r) == 8'h59)
                x++;
        check_eq("no stray 'Y' mirrors", x, 0);
    endtask

    // T-FF: FF (0x0C) runs CMD_TXTCLS which iterates all ROWS*COLS cells.
    // Confirm every cell gets cleared to 0x20 and the fg-color RAM is filled.
    task automatic test_txtcls_full_coverage();
        localparam int SCREEN_CELLS = COLS_TB * ROWS_TB;
        int char_dirty_count;
        int color_dirty_count;
        int char_cleared_count;
        int color_matched_count;
        $display("");
        $display("Test: FF (0x0C) CMD_TXTCLS clears ALL %0d cells", SCREEN_CELLS);
        // Pre-dirty every char and color cell with a non-default value
        for (int i = 0; i < SCREEN_CELLS; i++) begin
            dut.text_inst.char_mem.mem[i]  = 8'hFE;
            dut.text_inst.color_mem.mem[i] = 8'hFF;
        end
        step(2);
        // Verify pre-dirty state actually took
        char_dirty_count = 0;
        color_dirty_count = 0;
        for (int i = 0; i < SCREEN_CELLS; i++) begin
            if (dut.text_inst.char_mem.mem[i]  == 8'hFE) char_dirty_count++;
            if (dut.text_inst.color_mem.mem[i] == 8'hFF) color_dirty_count++;
        end
        check_eq("pre-FF: all char cells dirtied",  char_dirty_count,  SCREEN_CELLS);
        check_eq("pre-FF: all color cells dirtied", color_dirty_count, SCREEN_CELLS);

        // Set a recognizable fg_color so TXTCLS fills color RAM with it
        bus_write(REG_FGCOL_A, 8'd7);
        step(2);
        type_char(8'h0C);            // FF
        wait_cmd_done();
        step(10);

        // EVERY char cell must be 0x20 (space), EVERY color cell must be fg_color
        char_cleared_count  = 0;
        color_matched_count = 0;
        for (int i = 0; i < SCREEN_CELLS; i++) begin
            if (dut.text_inst.char_mem.mem[i]  == 8'h20) char_cleared_count++;
            if (dut.text_inst.color_mem.mem[i][3:0] == 4'd7) color_matched_count++;
        end
        check_eq("post-FF: all char cells == 0x20",
                 char_cleared_count, SCREEN_CELLS);
        check_eq("post-FF: all color cells == fg_color(7)",
                 color_matched_count, SCREEN_CELLS);

        // cursor and scroll state should also reset
        check_eq("post-FF: cursor_x = 0",    dut.cursor_x,      0);
        check_eq("post-FF: cursor_y = 0",    dut.cursor_y,      0);
        check_eq("post-FF: scroll_offset=0", dut.scroll_offset, 0);
    endtask

    task automatic test_txtcls_stalls_cpu_until_clear_done();
        $display("");
        $display("Test: FF (0x0C) holds VGC RDY low until text clear completes");
        bus_write(REG_CHAROUT_A, 8'h0C);
        check("RDY low while CMD_TXTCLS is active", !vgc_rdy && dut.cmd_busy);
        wait_vgc_ready();
        check("RDY high after CMD_TXTCLS completes", vgc_rdy && !dut.cmd_busy);
    endtask

    // T6b: push scroll_offset past its wrap point. The visible scroll now waits
    // for vblank, so do not burn 50 full frames just to reach the wrap point.
    // Seed the internal offset to ROWS-1, trigger one real scroll, and verify
    // the vblank-safe scroll FSM wraps it to 0.
    task automatic test_scroll_wraparound();
        int scroll_count;
        int expected;
        $display("");
        $display("Test: scroll_offset wraps back to 0 at ROWS-1");
        type_char(8'h0C);
        wait_cmd_done(); step(4);
        dut.scroll_offset = 6'(ROWS_TB - 1);
        bus_write(REG_CURSORY_A, 8'(ROWS_TB - 1));  // park at bottom
        step(4);
        type_char(8'h0A);           // LF; waits for vblank-safe scroll release
        scroll_count = dut.scroll_offset;
        check_eq("scroll_offset after one LF from ROWS-1 wraps to 0", scroll_count, 0);

        // Now type 'W' at (0,0). It should land at char RAM row 0 since
        // scroll_offset=0. This exercises the wrap-back path.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h00);
        step(4);
        type_char(8'h57);  // 'W'
        check_eq("post-wrap: 'W' at char RAM row 0 col 0",
                 peek_char_cell(0, 0), 8'h57);

        // And spot check another offset value without waiting 12 frames.
        dut.scroll_offset = 6'd12;
        step(2);
        check_eq("seeded scroll_offset=12", dut.scroll_offset, 12);

        // Write 'Q' at (0,3) → should land at row (3+12) mod ROWS.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h03);
        step(4);
        type_char(8'h51);  // 'Q'
        expected = (3 + 12) % ROWS_TB;
        check_eq("'Q' lands at (3+12) mod ROWS",
                 peek_char_cell(0, expected), 8'h51);

        // Regression for 50-row mode: screen_addr must not do row+offset in
        // 6 bits, because 20+49 wraps at 64 before the modulo-50 correction.
        dut.scroll_offset = 6'(ROWS_TB - 1);
        step(2);
        check_eq("seeded scroll_offset=ROWS-1", dut.scroll_offset, ROWS_TB - 1);

        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'd20);
        step(4);
        type_char(8'h52);  // 'R'
        expected = (20 + (ROWS_TB - 1)) % ROWS_TB;
        check_eq("'R' lands at (20+ROWS-1) mod ROWS",
                 peek_char_cell(0, expected), 8'h52);

        scroll_count = 0;
        for (int r = 0; r < ROWS_TB; r++)
            if (r != expected && peek_char_cell(0, r) == 8'h52)
                scroll_count++;
        check_eq("no stray 'R' mirrors", scroll_count, 0);
    endtask

    // T7: REG_CHAROUT control chars — BS (0x08), CR (0x0D), Reverse-CR (0x13), FF (0x0C)
    task automatic test_regcharout_control_chars();
        $display("");
        $display("Test: REG_CHAROUT control chars (BS/CR/FF/reverse-CR)");
        // BS: move cursor left, erase char behind, don't underflow
        type_char(8'h0C);                    // FF: clean slate
        wait_cmd_done(); step(4);
        type_char(8'h41);                    // 'A'
        type_char(8'h42);                    // 'B'
        type_char(8'h43);                    // 'C'  — cursor now at col 3
        check_eq("pre-BS: cursor_x",    dut.cursor_x, 3);
        check_eq("pre-BS: char col 2",  peek_char_cell(2, 0), 8'h43);
        type_char(8'h08);                    // BS
        step(4);
        check_eq("BS: cursor_x decremented to 2", dut.cursor_x, 2);
        check_eq("BS: erased char at col 2",      peek_char_cell(2, 0), 8'h20);
        check_eq("BS: char col 1 preserved",      peek_char_cell(1, 0), 8'h42);

        // BS at col 0 must not underflow
        type_char(8'h0C); wait_cmd_done(); step(4);
        type_char(8'h08);
        step(4);
        check_eq("BS at col 0: cursor_x stays 0", dut.cursor_x, 0);

        // CR (0x0D): return cursor to column 0, y unchanged
        type_char(8'h0C); wait_cmd_done(); step(4);
        for (int i = 0; i < 10; i++) type_char(8'h58);  // 10 'X's → col 10
        type_char(8'h0A); step(4);                       // LF → row 1, col 0
        // Set the cursor directly to isolate CR's carriage-only behavior.
        bus_write(REG_CURSORX_A, 8'd5);
        bus_write(REG_CURSORY_A, 8'd3);
        step(4);
        type_char(8'h0D);                    // CR
        step(4);
        check_eq("CR: cursor_x reset to 0", dut.cursor_x, 0);
        check_eq("CR: cursor_y unchanged",   dut.cursor_y, 3);

        // Reverse-CR (0x13): home cursor to (0,0)
        bus_write(REG_CURSORX_A, 8'd40);
        bus_write(REG_CURSORY_A, 8'd15);
        step(4);
        type_char(8'h13);                    // reverse-CR
        step(4);
        check_eq("Rev-CR: cursor_x = 0", dut.cursor_x, 0);
        check_eq("Rev-CR: cursor_y = 0", dut.cursor_y, 0);
    endtask

    // -----------------------------------------------------------------------
    // Runner
    // -----------------------------------------------------------------------
    initial begin
        $display("=== VGC Text-Layer Tests ===");
        do_reset();

        test_char_ram_roundtrip();
        test_regcharout_basic();
        test_regcharout_text_attributes();
        test_font_lookup_uses_delayed_scanline();
        test_scrollmixed_command_scrolls_all_planes();
        test_regcmd_read_is_busy_while_command_write_is_pending();
        test_scrollmixed_zork_zero_body_region();
        test_scrollmixed_survives_concurrent_host_char_read();
        test_debug_writes_parameters_and_scrollmixed_command();
        test_row_wrap();
        test_newline_no_scroll();
        test_newline_at_bottom_scrolls();
        test_scroll_offset_write_alignment();
        test_scroll_wraparound();
        test_regcharout_control_chars();
        test_txtcls_full_coverage();
        test_txtcls_stalls_cpu_until_clear_done();

        summary();
        $finish;
    end

    // Global safety timeout so a hung test never blocks the suite.
    initial begin
        #500000000;  // 500 ms of sim time
        $display("FAIL: global timeout hit");
        $finish;
    end

endmodule
