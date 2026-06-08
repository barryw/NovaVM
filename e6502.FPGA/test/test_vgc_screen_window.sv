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

    // Model a 6502 loop read of the window: present the address for the CPU's
    // active read cycle, then SAMPLE EARLY — with only the register-read settle
    // the CPU gives — so the multi-stage plane-BRAM pipeline has NOT delivered
    // this cell yet. The trailing cycles model the editor loop's per-cell overhead
    // (CMP/INY/branch/next-LDA fetch), during which the pipeline settles to THIS
    // address before the next read. Result: sequential reads are off by one — each
    // returns the PREVIOUSLY addressed cell, exactly the HW garbling that corrupted
    // BASIC line input (Ibuffs held " 1234567" for a typed "1234567").
    task automatic cpu_loop_read(input int off, output logic [7:0] data);
        @(posedge clk);
        cpu_addr <= win_addr(off);
        cpu_we   <= 0;
        cpu_re   <= 1;
        cpu_ce   <= 1;
        @(posedge clk);
        cpu_re   <= 0;
        cpu_ce   <= 0;
        @(posedge clk);
        data = cpu_rdata;          // CPU latches here — too early for the window pipeline
        repeat(6) @(posedge clk);  // editor per-cell overhead: pipeline settles to this addr
    endtask

    // The srl_read_cell fix (modules/system/system.s): a priming read presents the
    // address; the second read — issued after the loop overhead — returns it.
    task automatic cpu_loop_read2(input int off, output logic [7:0] data);
        logic [7:0] junk;
        cpu_loop_read(off, junk);  // prime: pushes this address into the read pipeline
        cpu_loop_read(off, data);  // settled: returns THIS cell
    endtask

    // HAZARD: a single CPU-timed read per cell returns the previous cell.
    task automatic test_window_read_offbyone_hazard();
        logic [7:0] v0, v1, v2, v3, v4;
        int base;
        do_reset();
        bus_write(WIN_PLANE, 8'd0);            // char plane
        base = 20 * COLS_TB;                    // row 20, cols 0..4
        bus_write(win_addr(base + 0), 8'h41);   // 'A'
        bus_write(win_addr(base + 1), 8'h42);   // 'B'
        bus_write(win_addr(base + 2), 8'h43);   // 'C'
        bus_write(win_addr(base + 3), 8'h44);   // 'D'
        bus_write(win_addr(base + 4), 8'h45);   // 'E'
        cpu_loop_read(base + 0, v0);            // seed (indeterminate — not asserted)
        cpu_loop_read(base + 1, v1);
        cpu_loop_read(base + 2, v2);
        cpu_loop_read(base + 3, v3);
        cpu_loop_read(base + 4, v4);
        // Off-by-one: the read at col i returns col (i-1)'s byte.
        check_eq("single read col1 returns col0 ('A') — off-by-one", v1, 8'h41);
        check_eq("single read col2 returns col1 ('B') — off-by-one", v2, 8'h42);
        check_eq("single read col3 returns col2 ('C') — off-by-one", v3, 8'h43);
        check_eq("single read col4 returns col3 ('D') — off-by-one", v4, 8'h44);
    endtask

    // FIX: srl_read_cell's double read returns each cell correctly.
    task automatic test_window_read_double_read_fix();
        logic [7:0] v0, v1, v2, v3, v4;
        int base;
        do_reset();
        bus_write(WIN_PLANE, 8'd0);
        base = 30 * COLS_TB;
        bus_write(win_addr(base + 0), 8'h50);   // 'P'
        bus_write(win_addr(base + 1), 8'h51);   // 'Q'
        bus_write(win_addr(base + 2), 8'h52);   // 'R'
        bus_write(win_addr(base + 3), 8'h53);   // 'S'
        bus_write(win_addr(base + 4), 8'h54);   // 'T'
        cpu_loop_read2(base + 0, v0);
        cpu_loop_read2(base + 1, v1);
        cpu_loop_read2(base + 2, v2);
        cpu_loop_read2(base + 3, v3);
        cpu_loop_read2(base + 4, v4);
        check_eq("double read col0 == 'P'", v0, 8'h50);
        check_eq("double read col1 == 'Q'", v1, 8'h51);
        check_eq("double read col2 == 'R'", v2, 8'h52);
        check_eq("double read col3 == 'S'", v3, 8'h53);
        check_eq("double read col4 == 'T'", v4, 8'h54);
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
        test_window_read_offbyone_hazard();
        test_window_read_double_read_fix();
        summary();
        $finish;
    end

    initial begin
        #200000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
