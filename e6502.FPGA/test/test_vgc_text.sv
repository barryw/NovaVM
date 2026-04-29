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

    // T4: explicit newlines advance cursor_y without touching scroll_offset
    // (until we reach the bottom).
    task automatic test_newline_no_scroll();
        $display("");
        $display("Test: newline before bottom row does not scroll");
        type_char(8'h0C);
        wait_cmd_done();
        step(4);
        for (int i = 0; i < 5; i++) begin
            type_char(8'h0A);  // LF
            // The VGC advances cursor_y immediately (no cmd state).
            step(2);
        end
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

    // T6b: push scroll_offset past its wrap point.
    // The formula is: scroll_offset = (scroll_offset >= ROWS-1) ? 0 : scroll_offset + 1.
    // Scrolling ROWS times from a fresh screen must wrap scroll_offset back to 0,
    // and writes at cursor_y=0 must then land at char RAM row 0.
    task automatic test_scroll_wraparound();
        int scroll_count;
        int expected;
        $display("");
        $display("Test: scroll_offset wraps back to 0 after %0d scrolls", ROWS_TB);
        type_char(8'h0C);
        wait_cmd_done(); step(4);
        bus_write(REG_CURSORY_A, 8'(ROWS_TB - 1));  // park at bottom
        step(4);
        for (int i = 0; i < ROWS_TB; i++) begin
            type_char(8'h0A);           // LF
            step(400);                  // let scroll_pending clear 80 cells
        end
        scroll_count = dut.scroll_offset;
        check_eq("scroll_offset after ROWS LFs wraps to 0", scroll_count, 0);

        // Now type 'W' at (0,0). It should land at char RAM row 0 since
        // scroll_offset=0. This exercises the wrap-back path.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h00);
        step(4);
        type_char(8'h57);  // 'W'
        check_eq("post-wrap: 'W' at char RAM row 0 col 0",
                 peek_char_cell(0, 0), 8'h57);

        // And spot check another offset value: scroll 12 more times → offset=12.
        for (int i = 0; i < 12; i++) begin
            bus_write(REG_CURSORY_A, 8'(ROWS_TB - 1));
            step(4);
            type_char(8'h0A);
            step(400);
        end
        check_eq("scroll_offset after 12 more LFs", dut.scroll_offset, 12);

        // Write 'Q' at (0,3) → should land at row (3+12) mod ROWS.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h03);
        step(4);
        type_char(8'h51);  // 'Q'
        expected = (3 + 12) % ROWS_TB;
        check_eq("'Q' lands at (3+12) mod ROWS",
                 peek_char_cell(0, expected), 8'h51);
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
        type_char(8'h0A); step(4);                       // LF → row 1 col 0... wait, LF only moves y
        // Actually LF advances y but leaves x alone in this VGC; let's just set
        // cursor via direct poke to (5, 3), then CR, and check cursor.
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
        test_row_wrap();
        test_newline_no_scroll();
        test_newline_at_bottom_scrolls();
        test_scroll_offset_write_alignment();
        test_scroll_wraparound();
        test_regcharout_control_chars();
        test_txtcls_full_coverage();

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
