// Lightweight debug UART tests.
//
// Verifies the Nova-owned ready/valid UART pair handles normal byte transfer,
// RX backpressure, RX overrun reporting, and stop-bit framing errors.

`timescale 1ns/1ps

module test_debug_uart;

    localparam int CLK_HZ = 1_000_000;
    localparam int BAUD   = 100_000;
    localparam int BIT_CYCLES = (CLK_HZ + (BAUD / 2)) / BAUD;

    logic clk = 0;
    always #500 clk = ~clk;  // 1 MHz

    logic rst;

    logic [7:0] tx_data;
    logic       tx_valid;
    wire        tx_ready;
    wire        tx_busy;
    wire        tx_line;

    logic       manual_drive;
    logic       manual_rx;
    wire        rx_line = manual_drive ? manual_rx : tx_line;

    wire [7:0] rx_data;
    wire       rx_valid;
    logic      rx_ready;
    wire       rx_busy;
    wire       rx_overrun;
    wire       rx_frame_error;

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;

    debug_uart_tx #(
        .CLK_HZ(CLK_HZ),
        .BAUD  (BAUD)
    ) tx (
        .clk(clk),
        .rst(rst),
        .data(tx_data),
        .valid(tx_valid),
        .ready(tx_ready),
        .tx(tx_line),
        .busy(tx_busy)
    );

    debug_uart_rx #(
        .CLK_HZ(CLK_HZ),
        .BAUD  (BAUD)
    ) rx (
        .clk(clk),
        .rst(rst),
        .rx(rx_line),
        .data(rx_data),
        .valid(rx_valid),
        .ready(rx_ready),
        .busy(rx_busy),
        .overrun_error(rx_overrun),
        .frame_error(rx_frame_error)
    );

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

    task automatic send_byte(input logic [7:0] b);
        while (!tx_ready) @(posedge clk);
        tx_data  <= b;
        tx_valid <= 1'b1;
        @(posedge clk);
        tx_valid <= 1'b0;
    endtask

    task automatic wait_rx_valid();
        int timeout = 1000;
        while (!rx_valid && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0) begin
            $display("  FAIL timeout waiting for rx_valid");
            fail_count++;
        end
    endtask

    task automatic wait_overrun();
        int timeout = 1000;
        while (!rx_overrun && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0) begin
            $display("  FAIL timeout waiting for rx_overrun");
            fail_count++;
        end
    endtask

    task automatic wait_frame_error();
        int timeout = 1000;
        while (!rx_frame_error && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        if (timeout == 0) begin
            $display("  FAIL timeout waiting for rx_frame_error");
            fail_count++;
        end
    endtask

    task automatic drive_serial_bit(input logic bit_value);
        manual_rx <= bit_value;
        repeat(BIT_CYCLES) @(posedge clk);
    endtask

    task automatic test_loopback_bytes();
        logic [7:0] values [0:3];
        values[0] = 8'h00;
        values[1] = 8'h55;
        values[2] = 8'hA6;
        values[3] = 8'hFF;

        $display("");
        $display("Test: TX -> RX loopback");
        rx_ready <= 1'b1;
        for (int i = 0; i < 4; i++) begin
            send_byte(values[i]);
            wait_rx_valid();
            check_eq8($sformatf("received byte %0d", i), rx_data, values[i]);
            @(posedge clk);
        end
    endtask

    task automatic test_rx_backpressure_holds_byte();
        $display("");
        $display("Test: RX valid holds under backpressure");
        rx_ready <= 1'b0;
        send_byte(8'h3C);
        wait_rx_valid();
        check_eq8("held byte", rx_data, 8'h3C);
        repeat(20) @(posedge clk);
        check("rx_valid still asserted", rx_valid);
        check_eq8("held byte still stable", rx_data, 8'h3C);
        rx_ready <= 1'b1;
        @(posedge clk);
        @(posedge clk);
        check("rx_valid cleared after ready", !rx_valid);
    endtask

    task automatic test_rx_overrun_is_explicit();
        $display("");
        $display("Test: RX overrun reports instead of replacing held byte");
        rx_ready <= 1'b0;
        send_byte(8'h11);
        wait_rx_valid();
        check_eq8("first held byte", rx_data, 8'h11);
        send_byte(8'h22);
        wait_overrun();
        check("overrun pulse observed", rx_overrun);
        check_eq8("held byte not replaced", rx_data, 8'h11);
        rx_ready <= 1'b1;
        repeat(3) @(posedge clk);
        check("rx_valid cleared after overrun drain", !rx_valid);
    endtask

    task automatic test_frame_error_bad_stop_bit();
        $display("");
        $display("Test: bad stop bit raises frame_error and emits no byte");
        rx_ready     <= 1'b1;
        manual_drive <= 1'b1;
        manual_rx    <= 1'b1;
        repeat(5) @(posedge clk);

        drive_serial_bit(1'b0);  // start
        for (int i = 0; i < 8; i++)
            drive_serial_bit(1'b1);
        manual_rx <= 1'b0;       // invalid stop, held past synchronizer delay
        wait_frame_error();
        manual_rx <= 1'b1;

        check("frame_error pulse observed", rx_frame_error);
        repeat(5) @(posedge clk);
        check("bad frame produced no valid byte", !rx_valid);
        manual_drive <= 1'b0;
    endtask

    initial begin
        $display("=== debug UART tests ===");
        rst          = 1'b1;
        tx_data      = 8'h00;
        tx_valid     = 1'b0;
        rx_ready     = 1'b1;
        manual_drive = 1'b0;
        manual_rx    = 1'b1;
        repeat(10) @(posedge clk);
        rst = 1'b0;
        repeat(10) @(posedge clk);

        test_loopback_bytes();
        test_rx_backpressure_holds_byte();
        test_rx_overrun_is_explicit();
        test_frame_error_bad_stop_bit();

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #20000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
