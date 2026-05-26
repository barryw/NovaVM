// Unit tests for usb_hid_keyboard.sv's report-to-key event filter.
//
// A held USB key can briefly disappear from the low-speed HID report stream.
// That single bad report must not re-arm the same scan code and enqueue a
// duplicate character.

`timescale 1ns/1ps

module usb_hid_host (
    input  wire       usbclk,
    input  wire       usbrst_n,
    inout  wire       usb_dm,
    inout  wire       usb_dp,
    output reg [1:0]  typ,
    output reg        report,
    output reg        conerr,
    output reg [7:0]  key_modifiers,
    output reg [7:0]  key1,
    output reg [7:0]  key2,
    output reg [7:0]  key3,
    output reg [7:0]  key4,
    output reg [7:0]  mouse_btn,
    output reg signed [7:0] mouse_dx,
    output reg signed [7:0] mouse_dy,
    output reg        game_l,
    output reg        game_r,
    output reg        game_u,
    output reg        game_d,
    output reg        game_a,
    output reg        game_b,
    output reg        game_x,
    output reg        game_y,
    output reg        game_sel,
    output reg        game_sta,
    output wire [63:0] dbg_hid_report
);
    assign usb_dm = 1'bz;
    assign usb_dp = 1'bz;
    assign dbg_hid_report = 64'h0;

    initial begin
        typ = 2'd0;
        report = 1'b0;
        conerr = 1'b0;
        key_modifiers = 8'h00;
        key1 = 8'h00;
        key2 = 8'h00;
        key3 = 8'h00;
        key4 = 8'h00;
        mouse_btn = 8'h00;
        mouse_dx = 8'sh00;
        mouse_dy = 8'sh00;
        game_l = 1'b0;
        game_r = 1'b0;
        game_u = 1'b0;
        game_d = 1'b0;
        game_a = 1'b0;
        game_b = 1'b0;
        game_x = 1'b0;
        game_y = 1'b0;
        game_sel = 1'b0;
        game_sta = 1'b0;
    end
endmodule

module test_usb_hid_keyboard;
    logic clk_usb = 1'b0;
    logic clk_sys = 1'b0;
    always #41 clk_usb = ~clk_usb; // ~12 MHz
    always #10 clk_sys = ~clk_sys; // 50 MHz

    logic usb_reset_n;
    logic sys_rst;
    tri usb_dm;
    tri usb_dp;
    wire key_valid;
    wire [7:0] key_data;
    wire [1:0] device_type;
    wire report_seen;
    wire connection_error;
    wire [7:0] dbg_status;
    wire [7:0] dbg_device_type;
    wire [7:0] dbg_last_scan;
    wire [7:0] dbg_last_ascii;
    wire [7:0] dbg_report_count;
    wire [7:0] dbg_key_count;

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    int event_count = 0;
    logic [7:0] events [0:15];

    usb_hid_keyboard dut (
        .clk_usb(clk_usb),
        .usb_reset_n(usb_reset_n),
        .clk_sys(clk_sys),
        .sys_rst(sys_rst),
        .usb_dm(usb_dm),
        .usb_dp(usb_dp),
        .key_valid(key_valid),
        .key_data(key_data),
        .device_type(device_type),
        .report_seen(report_seen),
        .connection_error(connection_error),
        .dbg_status(dbg_status),
        .dbg_device_type(dbg_device_type),
        .dbg_last_scan(dbg_last_scan),
        .dbg_last_ascii(dbg_last_ascii),
        .dbg_report_count(dbg_report_count),
        .dbg_key_count(dbg_key_count)
    );

    always_ff @(posedge clk_sys) begin
        if (sys_rst) begin
            event_count <= 0;
        end else if (key_valid) begin
            events[event_count] <= key_data;
            event_count <= event_count + 1;
        end
    end

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

    task automatic emit_report(input logic [7:0] scan);
        @(negedge clk_usb);
        dut.usb_host.typ = 2'd1;
        dut.usb_host.key_modifiers = 8'h00;
        dut.usb_host.key1 = scan;
        dut.usb_host.key2 = 8'h00;
        dut.usb_host.key3 = 8'h00;
        dut.usb_host.key4 = 8'h00;
        dut.usb_host.report = 1'b1;
        @(negedge clk_usb);
        dut.usb_host.report = 1'b0;
        repeat (8) @(posedge clk_sys);
    endtask

    initial begin
        $display("=== USB HID keyboard duplicate suppression test ===");

        usb_reset_n = 1'b0;
        sys_rst = 1'b1;
        repeat (4) @(posedge clk_usb);
        repeat (4) @(posedge clk_sys);
        usb_reset_n = 1'b1;
        sys_rst = 1'b0;
        repeat (8) @(posedge clk_sys);

        // HID scan 21 is R. One blank report between two identical R reports
        // models a transient low-speed HID glitch while the key is still held.
        emit_report(8'd21);
        emit_report(8'h00);
        emit_report(8'd21);
        emit_report(8'd21);

        check("held key with one blank report emits only once", event_count == 1);
        check_eq8("first event is lowercase r", events[0], 8'h72);
        check("USB diagnostics latch reports", dbg_status[5]);
        check("USB diagnostics latch key events", dbg_status[6]);
        check_eq8("USB diagnostics count key events", dbg_key_count, 8'd1);

        // Two consecutive blank reports are a stable release. The next R is a
        // real second press and must be accepted.
        emit_report(8'h00);
        emit_report(8'h00);
        emit_report(8'd21);

        check("same key after stable release emits again", event_count == 2);
        check_eq8("second event is lowercase r", events[1], 8'h72);

        // A different key should not be blocked by the previous scan's
        // suppression state.
        emit_report(8'd24); // U
        check("different scan code emits while previous key is tracked",
              event_count == 3);
        check_eq8("third event is lowercase u", events[2], 8'h75);

        if (fail_count == 0)
            $display("=== PASS: %0d checks ===", pass_count);
        else
            $display("=== FAIL: %0d failed / %0d passed ===",
                     fail_count, pass_count);
        $finish(fail_count == 0 ? 0 : 1);
    end
endmodule
