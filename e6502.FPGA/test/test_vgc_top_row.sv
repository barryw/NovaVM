// Screen-base (ring-scroll) register tests for the VGC.
//
// RegTextTopRow ($A0ED) sets the physical text row shown at display row 0:
// the renderer fetches real_row = (text_row + scroll_offset + text_top_row)
// mod ROWS. vgc.sv folds (scroll_offset + text_top_row) mod ROWS into the
// registered `combined_text_scroll` that feeds vgc_timing, leaving the
// timing-critical real_row path unchanged. vgc_timing's real_row mod math is
// covered by test_vgc_scroll_wrap; this test covers the register itself and
// that combined_text_scroll reflects text_top_row (the editor's case, where
// the terminal scroll_offset stays 0).

`timescale 1ns/1ps

module test_vgc_top_row;

    `include "vgc_tb.svh"

    localparam logic [15:0] TEXT_TOP_ROW_A = 16'hA0ED;

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

    // T2: with no terminal scroll (scroll_offset == 0), the registered ring
    // base fed to the renderer must equal text_top_row for every legal value.
    task automatic test_combined_equals_top_row();
        do_reset();
        check_eq("scroll_offset is 0 after reset", dut.scroll_offset, 0);
        for (int t = 0; t < ROWS_TB; t++) begin
            bus_write(TEXT_TOP_ROW_A, 8'(t)); step(3);
            check_eq($sformatf("combined_text_scroll == topRow (%0d)", t),
                     dut.combined_text_scroll, t);
        end
    endtask

    initial begin
        $display("=== VGC screen-base (RegTextTopRow) test ===");
        test_register_rw();
        test_combined_equals_top_row();
        summary();
        $finish;
    end

    initial begin
        #10000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
