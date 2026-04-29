// Copper unit tests — iverilog testbench
// Tests the copper's ability to change VGC registers at specific beam positions

`timescale 1ns/1ps

module test_copper;

    // Clock
    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    // DUT signals
    logic        rst;
    logic        cpu_ce;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    wire  [7:0]  cpu_rdata;
    logic        cpu_we;
    logic        cpu_re;
    logic        key_valid;
    logic [7:0]  key_data;
    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;

    // Tile DMA (unused in copper tests)
    wire  [15:0] tile_dma_addr;
    wire         tile_dma_active;

    // Instantiate VGC
    vgc dut (
        .clk(clk), .rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
        .cpu_rdata(cpu_rdata), .cpu_we(cpu_we), .cpu_re(cpu_re),
        .key_valid(1'b0), .key_data(8'h00),
        .tile_dma_addr(tile_dma_addr),
        .tile_dma_data(8'h00),
        .tile_dma_active(tile_dma_active),
        .blt_space(3'd0), .blt_addr(16'd0), .blt_rdata(),
        .blt_wdata(8'd0), .blt_we(1'b0), .blt_re(1'b0),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de)
    );

    // Test counters
    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    // ---------------------------------------------------------------
    // Helper tasks
    // ---------------------------------------------------------------

    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr;
        cpu_wdata <= data;
        cpu_we <= 1;
        cpu_ce <= 1;
        @(posedge clk);
        cpu_we <= 0;
        cpu_ce <= 0;
    endtask

    // Write a VGC parameter register (P0-P13 at $A011-$A01E)
    task automatic write_param(input int idx, input logic [7:0] data);
        write_reg(16'hA011 + idx, data);
    endtask

    // Trigger a VGC command
    task automatic write_cmd(input logic [7:0] cmd);
        write_reg(16'hA010, cmd);
    endtask

    // Add a copper entry: at position (x, y), set register reg_idx to value
    task automatic copper_add(input int x, input int y,
                              input int reg_idx, input logic [7:0] value);
        write_param(0, x[7:0]);        // P0 = x low
        write_param(1, x[15:8]);       // P1 = x high
        write_param(2, y[7:0]);        // P2 = y
        write_param(3, reg_idx[7:0]);  // P3 = reg index low
        write_param(4, reg_idx[15:8]); // P4 = reg index high
        write_param(5, value);         // P5 = value
        write_cmd(8'h1B);             // CmdCopperAdd
    endtask

    task automatic copper_enable();
        write_cmd(8'h1D);             // CmdCopperEnable
    endtask

    task automatic copper_disable();
        write_cmd(8'h1E);             // CmdCopperDisable
    endtask

    task automatic copper_clear();
        write_cmd(8'h1C);             // CmdCopperClear
    endtask

    // Run simulation for N pixel clocks
    task automatic run_clocks(input int n);
        repeat(n) @(posedge clk);
    endtask

    // Run until beam reaches a specific scanline (v_count == target)
    task automatic run_to_scanline(input int target);
        while (dut.v_count != target || dut.h_count != 0)
            @(posedge clk);
    endtask

    // Run one full frame (794 * 525 clocks)
    task automatic run_frame();
        run_clocks(794 * 525);
    endtask

    // Assert with message
    task automatic check(input string name, input logic condition);
        test_num++;
        if (condition) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    // ---------------------------------------------------------------
    // Tests
    // ---------------------------------------------------------------

    initial begin
        $display("=== Copper Unit Tests ===");
        $display("");

        // Init
        rst = 1;
        cpu_we = 0;
        cpu_re = 0;
        cpu_ce = 0;
        cpu_addr = 0;
        cpu_wdata = 0;
        key_valid = 0;
        key_data = 0;

        run_clocks(50);
        rst = 0;
        run_clocks(10);

        // ----- Test 1: Initial state -----
        $display("Test: Initial state");
        check("bg_color starts as 6 (blue)", dut.bg_color == 4'd6);
        check("copper disabled by default", dut.copper_enabled == 0);

        // ----- Test 2: Add copper entry and enable -----
        $display("Test: Copper add + enable");

        // Add entry: at beam position (0, 50), change bg_color to 2 (red)
        // bg_color is VGC register index 1 ($A001)
        copper_add(0, 50, 1, 8'd2);
        copper_enable();
        run_clocks(2); // settle

        check("copper enabled after command", dut.copper_enabled == 1);

        // ----- Test 3: Copper fires at correct position -----
        $display("Test: Copper trigger timing");
        // Run to start of a new frame (vblank resets copper index)
        // Then go to just before trigger — bg should still be 6
        run_to_scanline(480); // reach vblank
        // Reset bg manually so we can detect the copper change
        write_reg(16'hA001, 8'd6);
        run_clocks(2);
        run_to_scanline(0);  // start of new frame
        // V_BORDER=0 now (no border). Gfx line N maps to scanlines N*2..N*2+1.
        run_to_scanline(50*2 - 1);  // just before gfx line 50
        check("bg_color still 6 before trigger", dut.bg_color == 4'd6);

        // Now cross the trigger line
        run_to_scanline(50*2 + 1);
        check("bg_color changed to 2 after trigger", dut.bg_color == 4'd2);

        // ----- Test 5: Copper disable -----
        $display("Test: Copper disable");
        // Reset bg_color manually
        write_reg(16'hA001, 8'd6);
        run_clocks(2);
        check("bg_color reset to 6", dut.bg_color == 4'd6);

        copper_disable();
        run_clocks(2);
        run_frame();  // run full frame — copper should NOT fire
        run_to_scanline(141);
        check("bg_color stays 6 when copper disabled", dut.bg_color == 4'd6);

        // ----- Test 6: Copper clear -----
        $display("Test: Copper clear");
        copper_clear();       // clear before enable to avoid stale entries firing
        write_reg(16'hA001, 8'd6); // ensure bg is 6
        run_clocks(2);
        copper_enable();
        run_frame();
        run_to_scanline(141);
        check("bg_color stays 6 after copper clear", dut.bg_color == 4'd6);

        // ----- Test 7: Multiple copper entries -----
        $display("Test: Multiple entries");
        // Line 10: bg = green (5), Line 100: bg = red (2), Line 150: bg = yellow (7)
        copper_add(0, 10, 1, 8'd5);
        copper_add(0, 100, 1, 8'd2);
        copper_add(0, 150, 1, 8'd7);

        // Reset bg and run to vblank to start fresh
        write_reg(16'hA001, 8'd6);
        run_clocks(2);
        run_to_scanline(480);
        run_to_scanline(0);

        run_to_scanline(10*2 + 1);
        check("bg=5 (green) after line 10", dut.bg_color == 4'd5);

        run_to_scanline(100*2 + 1);
        check("bg=2 (red) after line 100", dut.bg_color == 4'd2);

        run_to_scanline(150*2 + 1);
        check("bg=7 (yellow) after line 150", dut.bg_color == 4'd7);

        // ---------------------------------------------------------------
        // Summary
        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0)
            $display("ALL TESTS PASSED");
        else
            $display("SOME TESTS FAILED");

        $finish;
    end

endmodule
