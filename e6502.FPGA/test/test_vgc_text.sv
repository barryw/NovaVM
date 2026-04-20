// Text-layer RTL tests for the VGC.
// Goal: isolate the "7-row mirror" typing bug in Verilator before touching hardware.
//
// Strategy: drive the VGC directly (no CPU, no EhBASIC), poke REG_CHAROUT and
// char RAM, then read char_mem.mem back through Verilator hierarchy. Any bug
// the user sees on hardware should be reproducible here OR be provably above
// the VGC (i.e. in ehbasic / screen editor).

`timescale 1ns/1ps

module test_vgc_text;

    `include "vgc_tb.svh"

    // -----------------------------------------------------------------------
    // Tests
    // -----------------------------------------------------------------------

    // T1: direct char-RAM pokes round-trip via internal mem access and via dbg port.
    task automatic test_char_ram_roundtrip();
        logic [7:0] rb;
        $display("");
        $display("Test: char RAM poke round-trip");
        // Write 0x41 ('A') through 0x48 ('H') to the first 8 cells
        for (int i = 0; i < 8; i++)
            bus_write(CHAR_RAM_BASE_A + 16'(i), 8'h41 + 8'(i));
        step(4);
        for (int i = 0; i < 8; i++)
            check_eq($sformatf("char_mem[%0d] via hierarchy", i),
                     peek_char(i), 8'h41 + i);
        // Same 8 cells via debug port
        for (int i = 0; i < 8; i++) begin
            @(posedge clk);
            dbg_addr <= CHAR_RAM_BASE_A + 16'(i);
            // dpram has 1-cycle read latency, dbg_rd_latch adds another.
            @(posedge clk);
            @(posedge clk);
            rb = dbg_rdata;
            check_eq($sformatf("dbg_rdata[%0d]", i), rb, 8'h41 + i);
        end
        dbg_addr <= 16'h0000;
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
        check_eq("cursor_y pinned at 24",    dut.cursor_y,     ROWS_TB - 1);
        check_eq("scroll_offset advanced",   dut.scroll_offset, 1);
        // The "new bottom row" in char RAM (where writes at cursor_y=24
        // will land) is (24 + 1) mod 25 = 0. Its first cell should have
        // been cleared to space (0x20) by the scroll_pending machine.
        check_eq("wrapped bottom cleared col 0",  peek_char_cell(0, 0),   8'h20);
        check_eq("wrapped bottom cleared col 79", peek_char_cell(79, 0),  8'h20);
    endtask

    // T6: the "7-row mirror" repro. After N scrolls, a char typed at
    // cursor_y=K should land at char-RAM row (K + scroll_offset) mod 25.
    // If it lands anywhere else, we've caught the bug.
    task automatic test_scroll_offset_write_alignment();
        int so;
        int expected_row;
        int x;
        $display("");
        $display("Test: typed chars land at cursor_y + scroll_offset mod 25");
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
        // char RAM row (0 + scroll_offset) mod 25 = scroll_offset.
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

        // Now type at (0, 5): expected row = (5 + scroll_offset) mod 25.
        bus_write(REG_CURSORX_A, 8'h00);
        bus_write(REG_CURSORY_A, 8'h05);
        step(4);
        type_char(8'h59);  // 'Y'
        expected_row = (5 + so) % ROWS_TB;
        check_eq("'Y' landed at (5+scroll_offset) mod 25 col 0",
                 peek_char_cell(0, expected_row), 8'h59);
        x = 0;
        for (int r = 0; r < ROWS_TB; r++)
            if (r != expected_row && peek_char_cell(0, r) == 8'h59)
                x++;
        check_eq("no stray 'Y' mirrors", x, 0);
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
