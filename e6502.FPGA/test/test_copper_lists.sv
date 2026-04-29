// Advanced copper list unit tests — iverilog testbench
// Tests multi-list management, double buffering, CopperList/CopperUse/CopperListEnd

`timescale 1ns/1ps

module test_copper_lists;

    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    logic        rst;
    logic        cpu_ce;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    wire  [7:0]  cpu_rdata;
    logic        cpu_we;
    logic        cpu_re;
    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;

    wire  [15:0] tile_dma_addr;
    wire         tile_dma_active;

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

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr; cpu_wdata <= data; cpu_we <= 1; cpu_ce <= 1;
        @(posedge clk);
        cpu_we <= 0; cpu_ce <= 0;
    endtask

    task automatic write_param(input int idx, input logic [7:0] data);
        write_reg(16'hA011 + idx, data);
    endtask

    task automatic write_cmd(input logic [7:0] cmd);
        write_reg(16'hA010, cmd);
    endtask

    task automatic copper_add(input int x, input int y, input int reg_idx, input logic [7:0] value);
        write_param(0, x[7:0]);
        write_param(1, x[15:8]);
        write_param(2, y[7:0]);
        write_param(3, reg_idx[7:0]);
        write_param(4, reg_idx[15:8]);
        write_param(5, value);
        write_cmd(8'h1B);
    endtask

    task automatic copper_enable();  write_cmd(8'h1D); endtask
    task automatic copper_disable(); write_cmd(8'h1E); endtask
    task automatic copper_clear();   write_cmd(8'h1C); endtask

    // Advanced copper list commands
    task automatic copper_list(input int idx);
        write_param(0, idx[7:0]);
        write_cmd(8'h20);
    endtask

    task automatic copper_use(input int idx);
        write_param(0, idx[7:0]);
        write_cmd(8'h21);
    endtask

    task automatic copper_list_end();
        write_cmd(8'h22);
    endtask

    task automatic run_clocks(input int n);
        repeat(n) @(posedge clk);
    endtask

    task automatic run_to_scanline(input int target);
        while (dut.v_count != target || dut.h_count != 0)
            @(posedge clk);
    endtask

    task automatic run_frame();
        run_clocks(794 * 525);
    endtask

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

    initial begin
        $display("=== Advanced Copper List Unit Tests ===");
        $display("");

        rst = 1;
        cpu_we = 0; cpu_re = 0; cpu_ce = 0;
        cpu_addr = 0; cpu_wdata = 0;
        run_clocks(50);
        rst = 0;
        run_clocks(10);

        // ----- Test 1: Initial multi-list state -----
        $display("Test: Initial multi-list state");
        check("target list starts at 0", dut.copper_target_list == 0);
        check("active list starts at 0", dut.copper_active_list == 0);
        check("pending list starts at 0", dut.copper_pending_list == 0);

        // ----- Test 2: CopperList selects target -----
        $display("Test: CopperList command");
        copper_list(3);
        run_clocks(2);
        check("target list set to 3", dut.copper_target_list == 3);
        check("active list unchanged", dut.copper_active_list == 0);

        // ----- Test 3: Add entries to target list -----
        $display("Test: Add entries to non-active list");
        // Add to list 3 (not active, so should NOT update live arrays)
        copper_add(0, 50, 1, 8'd5); // at line 50, bg=green
        run_clocks(2);
        check("list 3 count is 1", dut.copper_list_count[3] == 1);
        // Active list (0) count should still be 0
        check("active list count unchanged", dut.copper_count == 0);

        // ----- Test 4: Add entries to active list (list 0) -----
        $display("Test: Add entries to active list (0)");
        copper_list(0); // switch back to list 0
        run_clocks(2);
        copper_add(0, 30, 1, 8'd2); // at line 30, bg=red
        run_clocks(2);
        check("list 0 count is 1", dut.copper_list_count[0] == 1);
        check("active arrays also updated", dut.copper_count == 1);

        // ----- Test 5: CopperUse selects pending list -----
        $display("Test: CopperUse command");
        copper_use(3);
        run_clocks(2);
        check("pending list set to 3", dut.copper_pending_list == 3);
        check("active list NOT yet changed", dut.copper_active_list == 0);

        // ----- Test 6: Vblank swaps pending→active -----
        $display("Test: Vblank list swap");
        copper_enable();
        run_clocks(2);

        // Reset bg manually for testing
        write_reg(16'hA001, 8'd6);
        run_clocks(2);

        // Run to vblank — should trigger swap
        run_to_scanline(480);
        run_clocks(10); // let the swap settle

        // After vblank, active should now be list 3
        check("active list swapped to 3 at vblank", dut.copper_active_list == 3);

        // The copper loading state machine should have copied list 3 entries
        // Wait for loading to complete
        while (dut.copper_loading) @(posedge clk);
        run_clocks(5);

        check("active count matches list 3", dut.copper_count == dut.copper_list_count[3]);

        // ----- Test 7: Swapped list fires correctly -----
        $display("Test: Swapped list fires");
        write_reg(16'hA001, 8'd6); // reset bg
        run_clocks(2);
        run_to_scanline(0); // start of new frame
        run_to_scanline(40 + 50*2 + 1); // past line 50

        check("bg changed to 5 (green) from list 3", dut.bg_color == 4'd5);

        // ----- Test 8: CopperListEnd resets target to active -----
        $display("Test: CopperListEnd");
        copper_list(7); // set target to 7
        run_clocks(2);
        check("target is 7", dut.copper_target_list == 7);

        copper_list_end();
        run_clocks(2);
        check("target reset to active (3)", dut.copper_target_list == dut.copper_active_list);

        // ----- Test 9: Clear target list independently -----
        $display("Test: Independent list clear");
        copper_list(3);
        run_clocks(2);
        // List 3 should have 1 entry
        check("list 3 has entries before clear", dut.copper_list_count[3] > 0);

        copper_clear();
        run_clocks(2);
        check("list 3 cleared to 0", dut.copper_list_count[3] == 0);
        // Since list 3 IS the active list, active arrays should also be cleared
        check("active arrays cleared too", dut.copper_count == 0);

        // ----- Test 10: Double buffering workflow -----
        $display("Test: Double buffering workflow");

        // Build list 0 while list 3 is active (but empty now)
        copper_list(0);
        run_clocks(2);
        copper_clear();  // clear stale entries from earlier tests
        run_clocks(2);
        copper_add(0, 20, 1, 8'd7);   // line 20: bg=yellow
        copper_add(0, 80, 1, 8'd2);   // line 80: bg=red
        run_clocks(2);
        check("list 0 has 2 entries", dut.copper_list_count[0] == 2);

        // Schedule list 0 to become active at next vblank
        copper_use(0);
        run_clocks(2);

        // Run to vblank
        run_to_scanline(480);
        run_clocks(10);
        while (dut.copper_loading) @(posedge clk);
        run_clocks(5);

        check("active swapped to list 0", dut.copper_active_list == 0);
        check("active count is 2", dut.copper_count == 2);

        // Verify the list fires
        write_reg(16'hA001, 8'd6);
        run_clocks(2);
        run_to_scanline(0);
        run_to_scanline(40 + 20*2 + 1);
        check("bg=7 after line 20", dut.bg_color == 4'd7);

        run_to_scanline(40 + 80*2 + 1);
        check("bg=2 after line 80", dut.bg_color == 4'd2);

        // ----- Test 11: SysReset clears multi-list state -----
        $display("Test: SysReset");
        write_cmd(8'h1F);
        run_clocks(5);
        check("target reset to 0", dut.copper_target_list == 0);
        check("active reset to 0", dut.copper_active_list == 0);
        check("pending reset to 0", dut.copper_pending_list == 0);
        check("copper disabled after reset", dut.copper_enabled == 0);

        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
