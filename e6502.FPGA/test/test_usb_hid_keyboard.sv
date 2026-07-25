// Unit tests for usb_hid_keyboard.sv's report-to-key event filter.
//
// A held USB key can briefly disappear from the low-speed HID report stream.
// That single bad report must not re-arm the same scan code and enqueue a
// duplicate character.

`timescale 1ns/1ps

module usb_hid_host #(
    parameter FULL_SPEED = 1,
    parameter KEYBOARD_SUPPORT = 1,
    parameter MOUSE_SUPPORT = 1,
    parameter GAME_SUPPORT = 1
) (
    input  wire       clk,
    input  wire       reset,
    input  wire       cs,
    input  wire       usb_dm_i,
    input  wire       usb_dp_i,
    output wire       usb_dm_o,
    output wire       usb_dp_o,
    output wire       usb_oe,
    output reg [1:0]  typ,
    output reg        full_report,
    output reg        connerr,
    output reg        busy,
    output reg [7:0]  key_modifiers,
    output reg [7:0]  key_0,
    output reg [7:0]  key_1,
    output reg [7:0]  key_2,
    output reg [7:0]  key_3,
    output reg [7:0]  key_4,
    output reg [7:0]  key_5,
    output reg [2:0]  mouse_btn,
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
    output reg [3:0]  game_extra,
    output wire [63:0] dbg_hid_report,
    output wire [63:0] dbg_hid_regs,
    output wire [7:0]  dbg_core_status,
    output wire [9:0]  rom_addr,
    input  wire [3:0]  rom_dout,
    output wire        rom_en
);
    assign usb_dm_o = 1'b0;
    assign usb_dp_o = 1'b0;
    assign usb_oe = 1'b0;
    assign dbg_hid_report = 64'h0;
    assign dbg_hid_regs = 64'h0;
    assign dbg_core_status = 8'h00;
    assign rom_addr = 10'h0;
    assign rom_en = 1'b0;

    initial begin
        typ = 2'd0;
        full_report = 1'b0;
        connerr = 1'b0;
        busy = 1'b0;
        key_modifiers = 8'h00;
        key_0 = 8'h00;
        key_1 = 8'h00;
        key_2 = 8'h00;
        key_3 = 8'h00;
        key_4 = 8'h00;
        key_5 = 8'h00;
        mouse_btn = 3'b000;
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
        game_extra = 4'h0;
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
    wire [7:0] dbg_core_status;

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    int event_count = 0;
    logic [7:0] events [0:63];

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
        .dbg_key_count(dbg_key_count),
        .dbg_core_status(dbg_core_status)
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
        emit_report_mod(scan, 8'h00);
    endtask

    // Press one key and assert the byte it delivers to the 6502.
    task automatic check_nav(input string name, input logic [7:0] scan,
                             input logic [7:0] mods, input logic [7:0] expected);
        int unsigned prior_count;
        begin
            prior_count = event_count;
            press(scan, mods);
            if (event_count != prior_count + 1) begin
                fail_count = fail_count + 1;
                $display("FAIL: %s produced no key event", name);
            end else begin
                check_eq8(name, events[event_count - 1], expected);
            end
        end
    endtask

    // Two blank reports are a stable release, so this models a fresh press of a
    // key that may have been pressed before.
    task automatic press(input logic [7:0] scan, input logic [7:0] mods);
        emit_report(8'h00);
        emit_report(8'h00);
        emit_report_mod(scan, mods);
    endtask

    task automatic emit_report_mod(input logic [7:0] scan, input logic [7:0] mods);
        @(negedge clk_usb);
        dut.usb_host.typ = 2'd1;
        dut.usb_host.key_modifiers = mods;
        dut.usb_host.key_0 = scan;
        dut.usb_host.key_1 = 8'h00;
        dut.usb_host.key_2 = 8'h00;
        dut.usb_host.key_3 = 8'h00;
        dut.usb_host.key_4 = 8'h00;
        dut.usb_host.key_5 = 8'h00;
        dut.usb_host.full_report = 1'b1;
        @(negedge clk_usb);
        dut.usb_host.full_report = 1'b0;
        repeat (8) @(posedge clk_usb);
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

        // Navigation and function keys. Without these the full-screen editor
        // has no cursor movement on hardware. Codes are the ones defined by
        // software/runtime/asm/editui.inc.
        check_nav("Left",         8'd80, 8'h00, 8'h1C);
        check_nav("Right",        8'd79, 8'h00, 8'h1D);
        check_nav("Up",           8'd82, 8'h00, 8'h1E);
        check_nav("Down",         8'd81, 8'h00, 8'h1F);
        check_nav("Home",         8'd74, 8'h00, 8'h83);
        check_nav("End",          8'd77, 8'h00, 8'h05);
        check_nav("Page Up",      8'd75, 8'h00, 8'h10);
        check_nav("Page Down",    8'd78, 8'h00, 8'h12);
        check_nav("Ctrl-Home",    8'd74, 8'h01, 8'h80);
        check_nav("Ctrl-End",     8'd77, 8'h01, 8'h81);
        check_nav("Ctrl-Left",    8'd80, 8'h01, 8'h94);
        check_nav("Ctrl-Right",   8'd79, 8'h01, 8'h95);
        check_nav("Shift-Tab",    8'd43, 8'h02, 8'h8F);
        check_nav("Ctrl-Backspace", 8'd42, 8'h01, 8'h96);
        check_nav("F1",           8'd58, 8'h00, 8'h86);
        check_nav("F3",           8'd60, 8'h00, 8'h82);
        check_nav("F6",           8'd63, 8'h00, 8'h84);
        check_nav("Shift-F6",     8'd63, 8'h02, 8'h85);
        check_nav("F9 build",     8'd66, 8'h00, 8'h8C);
        check_nav("Ctrl-F9 run",  8'd66, 8'h01, 8'h92);
        check_nav("F12",          8'd69, 8'h00, 8'h91);
        // Shift + navigation extends a selection.
        check_nav("Shift-Left",   8'd80, 8'h02, 8'h97);
        check_nav("Shift-Right",  8'd79, 8'h02, 8'h98);
        check_nav("Shift-Up",     8'd82, 8'h02, 8'h99);
        check_nav("Shift-Down",   8'd81, 8'h02, 8'h9A);
        check_nav("Shift-Home",   8'd74, 8'h02, 8'h9B);
        check_nav("Shift-End",    8'd77, 8'h02, 8'h9C);

        if (fail_count == 0)
            $display("=== PASS: %0d checks ===", pass_count);
        else
            $display("=== FAIL: %0d failed / %0d passed ===",
                     fail_count, pass_count);
        $finish(fail_count == 0 ? 0 : 1);
    end
endmodule
