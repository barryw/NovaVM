// debug_sdram_port_b_cdc regression.
//
// Proves the debug-side request/ack crossing ignores stale SDRAM done pulses.
// This is important because port B is shared with sid_curve_reader; a curve
// read can already be in flight when a debug XRAM write takes over the mux.

`timescale 1ns/1ps

module test_debug_sdram_port_b_cdc;

    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz source/debug domain

    logic sdram_clk = 0;
    always #5 sdram_clk = ~sdram_clk;  // 100 MHz SDRAM domain

    logic rst;

    logic        req_we;
    logic        req_oe;
    logic [24:0] req_addr;
    logic [7:0]  req_din;
    wire  [7:0]  req_dout;
    wire         done_toggle;

    wire        sdram_we;
    wire        sdram_oe;
    wire [24:0] sdram_addr;
    wire [7:0]  sdram_din;
    logic [7:0] sdram_dout;
    logic       sdram_done;
    wire        sdram_busy;

    debug_sdram_port_b_cdc dut (
        .clk(clk), .sdram_clk(sdram_clk), .rst(rst),
        .req_we(req_we), .req_oe(req_oe),
        .req_addr(req_addr), .req_din(req_din),
        .req_dout(req_dout), .done_toggle(done_toggle),
        .sdram_we(sdram_we), .sdram_oe(sdram_oe),
        .sdram_addr(sdram_addr), .sdram_din(sdram_din),
        .sdram_dout(sdram_dout), .sdram_done(sdram_done),
        .sdram_busy(sdram_busy)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

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

    task automatic check_eq8(input string name, input logic [7:0] actual,
                             input logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)",
                     test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic pulse_stale_done();
        repeat (4) @(posedge sdram_clk);
        sdram_done <= 1'b1;
        @(posedge sdram_clk);
        sdram_done <= 1'b0;
    endtask

    task automatic complete_real_op(input logic [7:0] read_value);
        repeat (50) @(posedge sdram_clk);
        sdram_dout <= read_value;
        sdram_done <= 1'b1;
        @(posedge sdram_clk);
        sdram_done <= 1'b0;
    endtask

    task automatic wait_done_change(input logic previous_done);
        int timeout = 1000;
        while (done_toggle == previous_done && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0)
            $fatal(1, "timeout waiting for done_toggle");
    endtask

    initial begin
        logic done_before;

        $display("=== debug_sdram_port_b_cdc regression ===");

        rst = 1'b1;
        req_we = 1'b0;
        req_oe = 1'b0;
        req_addr = 25'd0;
        req_din = 8'd0;
        sdram_dout = 8'd0;
        sdram_done = 1'b0;

        repeat (8) @(posedge clk);
        rst = 1'b0;
        repeat (4) @(posedge clk);

        done_before = done_toggle;
        req_addr <= 25'h000123;
        req_din  <= 8'hA6;
        req_we   <= 1'b1;

        fork
            begin
                pulse_stale_done();
                repeat (4) @(posedge clk);
                check("write ignored stale done", done_toggle == done_before);
                complete_real_op(8'h00);
            end
        join_none

        wait_done_change(done_before);
        req_we <= 1'b0;
        repeat (4) @(posedge clk);

        check("write completed after real done", done_toggle != done_before);
        check("write drove SDRAM busy", sdram_busy == 1'b0);

        done_before = done_toggle;
        req_addr <= 25'h000456;
        req_oe   <= 1'b1;

        fork
            begin
                pulse_stale_done();
                repeat (4) @(posedge clk);
                check("read ignored stale done", done_toggle == done_before);
                complete_real_op(8'h5C);
            end
        join_none

        wait_done_change(done_before);
        req_oe <= 1'b0;
        repeat (4) @(posedge clk);

        check("read completed after real done", done_toggle != done_before);
        check_eq8("read data captured from real completion", req_dout, 8'h5C);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
