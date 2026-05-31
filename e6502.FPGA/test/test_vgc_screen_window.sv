// Direct screen window tests for the VGC.
//
// $A200-$B19F maps 4000 cells of the plane named by the plane-select register
// ($B1A0, 0=char/1=color/2=attr), so the CPU writes the screen with a plain STA.
// Writes inject through the same cmd_*_we path as the VRAM port; reads reuse the
// VRAM read-latch pipeline. This test drives the VGC bus directly and checks the
// plane RAMs via Verilator hierarchy.

`timescale 1ns/1ps

module test_vgc_screen_window;

    `include "vgc_tb.svh"

    localparam logic [15:0] WIN_BASE  = 16'hA200;
    localparam logic [15:0] WIN_PLANE = 16'hB1A0;

    function automatic logic [15:0] win_addr(input int off);
        win_addr = WIN_BASE + 16'(off);
    endfunction

    // Window read with extra settle cycles for the plane-BRAM read pipeline.
    task automatic win_read(input int off, output logic [7:0] data);
        @(posedge clk);
        cpu_addr <= win_addr(off);
        cpu_we   <= 0;
        cpu_re   <= 1;
        cpu_ce   <= 1;
        @(posedge clk);
        cpu_re   <= 0;
        cpu_ce   <= 0;
        repeat(5) @(posedge clk);
        data = cpu_rdata;
    endtask

    task automatic test_char_write();
        int off;
        do_reset();
        bus_write(WIN_PLANE, 8'd0);          // select char plane
        off = 2 * COLS_TB + 3;               // (col 3, row 2)
        bus_write(win_addr(off), 8'h51);     // 'Q'
        check_eq("char window write lands in char plane", peek_char(off), 8'h51);
    endtask

    task automatic test_planes_independent();
        int off;
        do_reset();
        off = 5;
        bus_write(WIN_PLANE, 8'd0);
        bus_write(win_addr(off), 8'h41);     // char 'A'
        bus_write(WIN_PLANE, 8'd1);
        bus_write(win_addr(off), 8'h2A);     // color
        bus_write(WIN_PLANE, 8'd2);
        bus_write(win_addr(off), 8'h01);     // attr
        check_eq("char plane holds 'A'", peek_char(off), 8'h41);
        check_eq("color plane holds 0x2A", peek_color(off), 8'h2A);
        check_eq("attr plane holds 0x01", peek_text_attr(off), 8'h01);
    endtask

    task automatic test_last_cell();
        do_reset();
        bus_write(WIN_PLANE, 8'd0);
        bus_write(16'hB19F, 8'h5A);          // last window address
        check_eq("last window cell (=$B19F) maps to cell 3999", peek_char(3999), 8'h5A);
    endtask

    task automatic test_plane_select_readback();
        logic [7:0] v;
        do_reset();
        bus_write(WIN_PLANE, 8'd2);
        bus_read(WIN_PLANE, v);
        check_eq("plane-select reads back", v, 2);
    endtask

    task automatic test_window_read_roundtrip();
        logic [7:0] v;
        int off;
        do_reset();
        off = 7 * COLS_TB + 10;
        bus_write(WIN_PLANE, 8'd0);
        bus_write(win_addr(off), 8'h5A);     // 'Z'
        win_read(off, v);
        check_eq("window read returns the written char", v, 8'h5A);
    endtask

    initial begin
        $display("=== VGC direct screen window test ===");
        test_char_write();
        test_planes_independent();
        test_last_cell();
        test_plane_select_readback();
        test_window_read_roundtrip();
        summary();
        $finish;
    end

    initial begin
        #200000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
