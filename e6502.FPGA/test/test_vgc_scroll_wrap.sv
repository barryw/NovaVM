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

    localparam int ROWS = 25;
    localparam int V_BORDER = 40;
    localparam int CHAR_H_DOUBLED = 16;  // CHAR_H=8 doubled for VGA

    logic [9:0] h_count = 0;
    logic [9:0] v_count = 0;
    logic [4:0] scroll_offset = 0;

    wire  [9:0] h_count_d1, h_count_d2, v_count_d1, v_count_d2;
    wire        visible, h_visible, v_visible, in_text_area;
    wire        visible_d1, visible_d2, in_text_area_d1, in_text_area_d2;
    wire        h_sync_area, v_sync_area;
    wire        h_sync_area_d1, h_sync_area_d2, v_sync_area_d1, v_sync_area_d2;
    wire [6:0]  text_col;
    wire [4:0]  text_row;
    wire [9:0]  text_line;
    wire [4:0]  real_row;
    wire [2:0]  font_pixel, font_line;
    wire [8:0]  pre_gfx_x;
    wire [7:0]  pre_gfx_y;
    wire [8:0]  gfx_x;
    wire [7:0]  gfx_y;
    wire [6:0]  text_col_d1, text_col_d2;
    wire [4:0]  text_row_d1, text_row_d2;
    wire [2:0]  font_pixel_d1, font_pixel_d2;
    wire [8:0]  gfx_x_d1, gfx_x_d2;
    wire [7:0]  gfx_y_d1, gfx_y_d2;
    wire [2:0]  font_line_d1;

    vgc_timing dut (
        .clk(clk),
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
        .scroll_offset(scroll_offset)
    );

    int pass_count = 0, fail_count = 0;

    task automatic check_row(input int t_row, input int s_off, input int expected);
        // Drive v_count to a line inside the t_row character row (center of
        // the doubled 16-px cell). Start of text row N occupies screen lines
        // V_BORDER + N*16 .. V_BORDER + N*16+15.
        v_count = V_BORDER + t_row * CHAR_H_DOUBLED + 4;  // anywhere within cell
        scroll_offset = 5'(s_off);
        #1;  // let combinational propagate
        if (real_row == 5'(expected)) begin
            pass_count++;
        end else begin
            $display("  FAIL text_row=%0d scroll=%0d: got real_row=%0d want %0d",
                     t_row, s_off, real_row, expected);
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

        $display("  PASS/FAIL totals for full 25x25 row-mapping grid");

        // Also explicitly prove the specific 7-row mirror scenario the user
        // saw: with scroll_offset=14, rows 18..24 must NOT map to the same
        // real_row as any of rows 11..17.
        $display("-- explicit 7-row mirror scenario (scroll_offset=14) --");
        duplicates_found = 0;
        begin : mirror_scan
            logic [4:0] seen_for_row [0:24];
            for (int r = 0; r < ROWS; r++) seen_for_row[r] = 5'h1F;
            for (int r = 0; r < ROWS; r++) begin
                v_count = V_BORDER + r * CHAR_H_DOUBLED + 4;
                scroll_offset = 5'd14;
                #1;
                for (int q = 0; q < r; q++)
                    if (seen_for_row[q] == real_row) duplicates_found++;
                seen_for_row[r] = real_row;
            end
        end
        if (duplicates_found == 0)
            $display("  PASS scroll_offset=14: all 25 rows map to distinct real_rows");
        else begin
            $display("  FAIL scroll_offset=14: %0d row collisions found",
                     duplicates_found);
            fail_count++;
        end

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
