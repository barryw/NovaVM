// VGC scroll-offset wrap regression test.
//
// The render pipeline computes which character row to display from which
// the user sees on-screen via:
//     real_row = (text_row + scroll_offset) mod ROWS
//
// In vgc_timing.sv this was written as `real_row = text_row + scroll_offset`
// with both operands 5-bit. SystemVerilog's implicit self-determined
// addition truncated the sum back to 5 bits BEFORE the subsequent
// `if (real_row >= ROWS) real_row -= ROWS` fixup could run. As soon as
// text_row + scroll_offset reached 32 the sum wrapped to 0, leaving the
// fixup unreachable — rows 18..24 and 11..17 then mapped to the same
// real_row values, producing the 7-row mirror users saw on hardware
// after the screen had scrolled past the first full page.
//
// This test drives v_count and scroll_offset through every legal
// combination and verifies real_row matches the mathematical
// (text_row + scroll_offset) mod ROWS for each.

`timescale 1ns/1ps

module test_vgc_scroll_wrap;

    logic clk = 0;
    always #20 clk = ~clk;

    localparam int ROWS = 50;
    localparam int V_BORDER = 40;          // 80x50 centered in 480 active lines
    localparam int CHAR_H_NATIVE = 8;      // 8x8 cells 1:1 (no pixel doubling)

    logic [9:0] h_count = 0;
    logic [9:0] v_count = 0;
    logic [5:0] scroll_offset = 0;
    logic [8:0] scroll_x = 0;
    logic [7:0] scroll_y = 0;
    logic       scroll_gfx_enable = 0;
    logic       scroll_text_enable = 0;
    logic       rst = 0;

    wire  [9:0] h_count_d1, h_count_d2, v_count_d1, v_count_d2;
    wire        visible, h_visible, v_visible, in_text_area;
    wire        visible_d1, visible_d2, in_text_area_d1, in_text_area_d2;
    wire        h_sync_area, v_sync_area;
    wire        h_sync_area_d1, h_sync_area_d2, v_sync_area_d1, v_sync_area_d2;
    wire [6:0]  text_col;
    wire [5:0]  text_row;
    wire [9:0]  text_line;
    wire [5:0]  real_row;
    wire [2:0]  font_pixel, font_line;
    wire [8:0]  pre_gfx_x;
    wire [7:0]  pre_gfx_y;
    wire [8:0]  gfx_x;
    wire [7:0]  gfx_y;
    wire [6:0]  text_col_d1, text_col_d2;
    wire [5:0]  text_row_d1, text_row_d2;
    wire [2:0]  font_pixel_d1, font_pixel_d2;
    wire [8:0]  gfx_x_d1, gfx_x_d2;
    wire [7:0]  gfx_y_d1, gfx_y_d2;
    wire [2:0]  font_line_d1;

    vgc_timing dut (
        .clk(clk),
        .rst(rst),
        .h_count(h_count), .v_count(v_count),
        .h_count_d1(h_count_d1), .h_count_d2(h_count_d2),
        .v_count_d1(v_count_d1), .v_count_d2(v_count_d2),
        .visible(visible), .h_visible(h_visible), .v_visible(v_visible),
        .visible_d1(visible_d1), .visible_d2(visible_d2),
        .in_text_area(in_text_area),
        .in_text_area_d1(in_text_area_d1), .in_text_area_d2(in_text_area_d2),
        .h_sync_area(h_sync_area), .v_sync_area(v_sync_area),
        .h_sync_area_d1(h_sync_area_d1), .h_sync_area_d2(h_sync_area_d2),
        .v_sync_area_d1(v_sync_area_d1), .v_sync_area_d2(v_sync_area_d2),
        .text_col(text_col), .text_row(text_row), .text_line(text_line),
        .real_row(real_row),
        .font_pixel(font_pixel), .font_line(font_line),
        .pre_gfx_x(pre_gfx_x), .pre_gfx_y(pre_gfx_y),
        .gfx_x(gfx_x), .gfx_y(gfx_y),
        .text_col_d1(text_col_d1), .text_col_d2(text_col_d2),
        .text_row_d1(text_row_d1), .text_row_d2(text_row_d2),
        .font_pixel_d1(font_pixel_d1), .font_pixel_d2(font_pixel_d2),
        .gfx_x_d1(gfx_x_d1), .gfx_x_d2(gfx_x_d2),
        .gfx_y_d1(gfx_y_d1), .gfx_y_d2(gfx_y_d2),
        .font_line_d1(font_line_d1),
        .scroll_x(scroll_x),
        .scroll_y(scroll_y),
        .scroll_gfx_enable(scroll_gfx_enable),
        .scroll_text_enable(scroll_text_enable),
        .scroll_offset(scroll_offset)
    );

    int pass_count = 0, fail_count = 0;

    task automatic check_row(input int t_row, input int s_off, input int expected);
        // Drive v_count to a line inside the t_row character row. Each row
        // occupies 8 native scanlines at 8x8 1:1: V_BORDER + N*8 .. +7.
        v_count = V_BORDER + t_row * CHAR_H_NATIVE + 4;  // anywhere within cell
        scroll_offset = 6'(s_off);
        #1;  // let combinational propagate
        if (real_row == 6'(expected)) begin
            pass_count++;
        end else begin
            $display("  FAIL text_row=%0d scroll=%0d: got real_row=%0d want %0d",
                     t_row, s_off, real_row, expected);
            fail_count++;
        end
    endtask

    task automatic check_gfx_coord(
        input int phys_x,
        input int phys_y,
        input int sx,
        input int sy,
        input bit enable,
        input int expected_x,
        input int expected_y
    );
        h_count = phys_x * 2;
        v_count = V_BORDER + phys_y * 2;
        scroll_x = 9'(sx);
        scroll_y = 8'(sy);
        scroll_gfx_enable = enable;
        #1;
        if (gfx_x == 9'(expected_x) && gfx_y == 8'(expected_y)) begin
            pass_count++;
        end else begin
            $display("  FAIL gfx phys=%0d,%0d scroll=%0d,%0d enable=%0d: got %0d,%0d want %0d,%0d",
                     phys_x, phys_y, sx, sy, enable, gfx_x, gfx_y, expected_x, expected_y);
            fail_count++;
        end
    endtask

    task automatic check_text_coord(
        input int phys_px,
        input int phys_py,
        input int sx,
        input int sy,
        input bit enable,
        input int expected_col,
        input int expected_row,
        input int expected_font_pixel,
        input int expected_font_line
    );
        h_count = phys_px;
        v_count = V_BORDER + phys_py;
        scroll_x = 9'(sx);
        scroll_y = 8'(sy);
        scroll_text_enable = enable;
        scroll_offset = 0;
        #1;
        if (text_col == 7'(expected_col) &&
            real_row == 6'(expected_row) &&
            font_pixel == 3'(expected_font_pixel) &&
            font_line == 3'(expected_font_line)) begin
            pass_count++;
        end else begin
            $display("  FAIL text phys=%0d,%0d scroll=%0d,%0d enable=%0d: got col=%0d row=%0d fp=%0d fl=%0d want col=%0d row=%0d fp=%0d fl=%0d",
                     phys_px, phys_py, sx, sy, enable,
                     text_col, real_row, font_pixel, font_line,
                     expected_col, expected_row, expected_font_pixel, expected_font_line);
            fail_count++;
        end
    endtask

    int duplicates_found;

    initial begin
        $display("=== VGC scroll-offset wrap regression test ===");

        // Brute-force the full space: every legal text_row × every legal
        // scroll_offset. Expected result is canonical (text_row + scroll_offset) % ROWS.
        // NOTE: `automatic` is required on the local — SV's default static
        // lifetime in initial blocks would freeze the initializer at time 0
        // (same trap that bit test_key_inject.sv earlier).
        for (int s = 0; s < ROWS; s++) begin
            for (int r = 0; r < ROWS; r++) begin
                automatic int expected = (r + s) % ROWS;
                check_row(r, s, expected);
            end
        end

        $display("  PASS/FAIL totals for full 60x60 row-mapping grid");

        // Regression: every scroll_offset ∈ [0,59] must produce a bijective
        // row mapping. No two text_rows should map to the same real_row.
        // This was the 7-row mirror bug class at ROWS=25; widened real_row_sum
        // now carries an extra bit so the fixup runs before wrap.
        $display("-- bijectivity scan at scroll_offset=30 --");
        duplicates_found = 0;
        begin : mirror_scan
            logic [5:0] seen_for_row [0:59];
            for (int r = 0; r < ROWS; r++) seen_for_row[r] = 6'h3F;
            for (int r = 0; r < ROWS; r++) begin
                v_count = V_BORDER + r * CHAR_H_NATIVE + 4;
                scroll_offset = 6'd30;
                #1;
                for (int q = 0; q < r; q++)
                    if (seen_for_row[q] == real_row) duplicates_found++;
                seen_for_row[r] = real_row;
            end
        end
        if (duplicates_found == 0)
            $display("  PASS scroll_offset=30: all 60 rows map to distinct real_rows");
        else begin
            $display("  FAIL scroll_offset=30: %0d row collisions found",
                     duplicates_found);
            fail_count++;
        end

        $display("-- visual scroll coordinate checks --");
        check_gfx_coord(0, 0, 319, 199, 1, 319, 199);
        check_gfx_coord(1, 1, 319, 199, 1, 0, 0);
        check_gfx_coord(17, 23, 9'd320, 8'd200, 1, 17, 23);
        check_gfx_coord(17, 23, 100, 50, 0, 17, 23);
        check_text_coord(6, 6, 1, 1, 1, 1, 1, 0, 0);
        check_text_coord(6, 6, 1, 1, 0, 0, 0, 6, 6);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count + (duplicates_found == 0 ? 1 : 0), fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #10000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
