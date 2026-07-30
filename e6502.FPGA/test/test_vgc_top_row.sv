// Screen-base (ring-scroll) register tests for the VGC.
//
// RegTextTopRow ($A0ED) is the ring base for the text scroll WINDOW named by
// $A0EE/$A0EF: rows inside the window rotate around it, rows outside it are
// physically fixed. That is what lets a full-screen app (the editor) scroll a
// document body while its menu bar and status line stay put, and it is the
// contract the reference machine implements in
// VirtualGraphicsController.PhysicalTextRow. Folding this ring together with
// the CHAROUT scroll offset -- which does rotate the whole plane -- rotated the
// editor's chrome into the middle of the document on hardware, so these tests
// pin the window registers and the row mapping that keeps them separate.

`timescale 1ns/1ps

module test_vgc_top_row;

    `include "vgc_tb.svh"

    localparam logic [15:0] TEXT_TOP_ROW_A = 16'hA0ED;
    localparam logic [15:0] TEXT_SCROLL_START_A = 16'hA0EE;
    localparam logic [15:0] TEXT_SCROLL_ROWS_A  = 16'hA0EF;

    // Display row -> physical plane row, as vgc_timing maps it: the window ring
    // first (rows outside the window are fixed), then the plane-wide CHAROUT
    // ring. Mirrors VirtualGraphicsController.PhysicalTextRow.
    function automatic int phys_row(input int display_row);
        int start, rows, rel, mapped;
        start = int'(dut.text_scroll_start);
        rows  = int'(dut.text_scroll_rows);
        if (rows == 0 || display_row < start || display_row >= start + rows)
            mapped = display_row;
        else begin
            rel = (display_row - start + int'(dut.text_top_row)) % rows;
            mapped = start + rel;
        end
        phys_row = (mapped + int'(dut.scroll_offset)) % ROWS_TB;
    endfunction

    // T1: register reads back, resets to 0, and clamps writes into 0..ROWS-1.
    task automatic test_register_rw();
        logic [7:0] v;
        do_reset();
        bus_read(TEXT_TOP_ROW_A, v);
        check_eq("topRow resets to 0", v, 0);

        bus_write(TEXT_TOP_ROW_A, 8'd7);  step(3);
        bus_read(TEXT_TOP_ROW_A, v);
        check_eq("topRow=7 reads back", v, 7);
        check_eq("text_top_row reg holds 7", dut.text_top_row, 7);

        bus_write(TEXT_TOP_ROW_A, 8'd49); step(3);
        bus_read(TEXT_TOP_ROW_A, v);
        check_eq("topRow=49 stays 49", v, 49);

        bus_write(TEXT_TOP_ROW_A, 8'd50); step(3);
        bus_read(TEXT_TOP_ROW_A, v);
        check_eq("topRow=50 wraps to 0", v, 0);

        bus_write(TEXT_TOP_ROW_A, 8'd53); step(3);
        bus_read(TEXT_TOP_ROW_A, v);
        check_eq("topRow=53 wraps to 3", v, 3);
    endtask

    // T2: the ring base reaches the renderer unfolded, so the window mapping
    // (not a plane-wide offset) decides which physical row each display row
    // shows.
    task automatic test_top_row_reaches_renderer();
        do_reset();
        check_eq("scroll_offset is 0 after reset", dut.scroll_offset, 0);
        for (int t = 0; t < ROWS_TB; t++) begin
            bus_write(TEXT_TOP_ROW_A, 8'(t)); step(3);
            check_eq($sformatf("text_top_row == topRow (%0d)", t),
                     dut.text_top_row, t);
        end
    endtask

    // T3: the window registers exist, default to the whole plane, clamp to it,
    // and reset the ring base when the window moves.
    task automatic test_window_registers();
        logic [7:0] v;
        do_reset();
        bus_read(TEXT_SCROLL_START_A, v);
        check_eq("scroll start resets to 0", v, 0);
        bus_read(TEXT_SCROLL_ROWS_A, v);
        check_eq("scroll rows resets to the whole plane", v, ROWS_TB);

        bus_write(TEXT_TOP_ROW_A, 8'd5); step(3);
        bus_write(TEXT_SCROLL_START_A, 8'd1); step(3);
        bus_read(TEXT_SCROLL_START_A, v);
        check_eq("scroll start=1 reads back", v, 1);
        check_eq("moving the window clears the ring base", dut.text_top_row, 0);

        bus_write(TEXT_SCROLL_ROWS_A, 8'd48); step(3);
        bus_read(TEXT_SCROLL_ROWS_A, v);
        check_eq("scroll rows=48 reads back", v, 48);

        // A window that would run past the plane is clamped to what is left.
        bus_write(TEXT_SCROLL_ROWS_A, 8'd60); step(3);
        bus_read(TEXT_SCROLL_ROWS_A, v);
        check_eq("oversized window clamps to the plane", v, ROWS_TB - 1);
    endtask

    // T4: the editor's layout. With the window covering rows 1..48 and the ring
    // advanced, row 0 (menu bar) and row 49 (status line) must still map to
    // themselves while the body rotates.
    task automatic test_chrome_stays_put();
        do_reset();
        bus_write(TEXT_SCROLL_START_A, 8'd1);  step(3);
        bus_write(TEXT_SCROLL_ROWS_A, 8'd48);  step(3);
        bus_write(TEXT_TOP_ROW_A, 8'd12);      step(3);

        check_eq("menu row is fixed", phys_row(0), 0);
        check_eq("status row is fixed", phys_row(ROWS_TB - 1), ROWS_TB - 1);
        check_eq("body top shows the ring base", phys_row(1), 13);
        check_eq("body wraps inside the window", phys_row(37), 1);
    endtask

    initial begin
        $display("=== VGC screen-base (RegTextTopRow) test ===");
        test_register_rw();
        test_top_row_reaches_renderer();
        test_window_registers();
        test_chrome_stays_put();
        summary();
        $finish;
    end

    initial begin
        #60000000;   // three extra window scenarios need a longer budget
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
