`timescale 1ns/1ps

module usb_hid_keyboard (
    input  logic       clk_usb,
    input  logic       usb_reset_n,
    input  logic       clk_sys,
    input  logic       sys_rst,

    inout  wire        usb_dm,
    inout  wire        usb_dp,

    output logic       key_valid,
    output logic [7:0] key_data,
    output logic [1:0] device_type,
    output logic       report_seen,
    output logic       connection_error,
    output logic [7:0] dbg_status,
    output logic [7:0] dbg_device_type,
    output logic [7:0] dbg_last_scan,
    output logic [7:0] dbg_last_ascii,
    output logic [7:0] dbg_report_count,
    output logic [7:0] dbg_key_count,
    output logic [7:0] dbg_core_status,
    output logic [63:0] dbg_hid_regs
);

    wire       usb_report;
    wire       usb_busy;
    wire       usb_oe;
    wire       usb_dm_o;
    wire       usb_dp_o;
    wire [9:0] usb_rom_addr;
    wire [3:0] usb_rom_dout;
    wire       usb_rom_en;
    wire [7:0] key_modifiers;
    wire [7:0] key0;
    wire [7:0] key1;
    wire [7:0] key2;
    wire [7:0] key3;
    wire [7:0] key4;
    wire [7:0] key5;
    wire [2:0] mouse_btn;
    wire signed [7:0] mouse_dx;
    wire signed [7:0] mouse_dy;
    wire game_l, game_r, game_u, game_d;
    wire game_a, game_b, game_x, game_y, game_sel, game_sta;
    wire [7:0] dbg_core_status_usb;
    wire [63:0] dbg_hid_regs_usb;

    assign usb_dm = usb_oe ? usb_dm_o : 1'bz;
    assign usb_dp = usb_oe ? usb_dp_o : 1'bz;

`ifndef USB_HID_HOST_EXTERNAL_STUB
    usb_hid_host_rom usb_rom (
        .clk (clk_usb),
        .addr(usb_rom_addr),
        .dout(usb_rom_dout),
        .en  (usb_rom_en)
    );
`else
    assign usb_rom_dout = 4'h0;
`endif

    usb_hid_host #(
        .KEYBOARD_SUPPORT(1),
        .MOUSE_SUPPORT   (0),
        .GAME_SUPPORT    (0)
    ) usb_host (
        .clk          (clk_usb),
        .reset        (!usb_reset_n),
        .cs           (1'b1),
        .usb_dm_i     (usb_dm),
        .usb_dp_i     (usb_dp),
        .usb_dm_o     (usb_dm_o),
        .usb_dp_o     (usb_dp_o),
        .usb_oe       (usb_oe),
        .typ          (device_type),
        .full_report  (usb_report),
        .connerr      (connection_error),
        .busy         (usb_busy),
        .key_modifiers(key_modifiers),
        .key_0        (key0),
        .key_1        (key1),
        .key_2        (key2),
        .key_3        (key3),
        .key_4        (key4),
        .key_5        (key5),
        .mouse_btn    (mouse_btn),
        .mouse_dx     (mouse_dx),
        .mouse_dy     (mouse_dy),
        .game_l       (game_l),
        .game_r       (game_r),
        .game_u       (game_u),
        .game_d       (game_d),
        .game_a       (game_a),
        .game_b       (game_b),
        .game_x       (game_x),
        .game_y       (game_y),
        .game_sel     (game_sel),
        .game_sta     (game_sta),
        .game_extra   (),
        .dbg_hid_report(),
        .dbg_hid_regs (dbg_hid_regs_usb),
        .dbg_core_status(dbg_core_status_usb),
        .rom_addr     (usb_rom_addr),
        .rom_dout     (usb_rom_dout),
        .rom_en       (usb_rom_en)
    );

    assign report_seen = usb_report;

    logic [7:0] prev_key0;
    logic [7:0] prev_key1;
    logic [7:0] prev_key2;
    logic [7:0] prev_key3;
    logic [7:0] prev_key4;
    logic [7:0] prev_key5;
    logic [7:0] pending_data_usb;
    logic       event_toggle_usb;
    logic       ack_toggle_usb_meta;
    logic       ack_toggle_usb;
    logic       ack_toggle_sys;
    logic [7:0] last_event_scan_usb;
    logic       last_event_active_usb;
    logic [1:0] last_event_release_reports_usb;
    logic       report_toggle_usb;
    logic [7:0] last_report_scan_usb;
    logic [1:0] usb_lines_usb;
    logic       report_stage_valid_usb;
    logic [7:0] report_modifiers_usb;
    logic [7:0] report_key0_usb;
    logic [7:0] report_key1_usb;
    logic [7:0] report_key2_usb;
    logic [7:0] report_key3_usb;
    logic [7:0] report_key4_usb;
    logic [7:0] report_key5_usb;
    logic       scan_stage_valid_usb;
    logic [7:0] scan_stage_usb;
    logic [7:0] scan_modifiers_stage_usb;
    logic       scan_last_event_still_down_usb;
    logic       ascii_stage_valid_usb;
    logic [7:0] ascii_scan_stage_usb;
    logic [7:0] ascii_stage_usb;
    logic       ascii_last_event_still_down_usb;

    localparam logic [1:0] RELEASE_STABLE_REPORTS = 2'd2;

    function automatic logic key_was_down(
        input logic [7:0] key,
        input logic [7:0] old0,
        input logic [7:0] old1,
        input logic [7:0] old2,
        input logic [7:0] old3,
        input logic [7:0] old4,
        input logic [7:0] old5
    );
        key_was_down = key != 8'h00 &&
                       (key == old0 || key == old1 || key == old2 ||
                        key == old3 || key == old4 || key == old5);
    endfunction

    function automatic logic key_is_down(
        input logic [7:0] key,
        input logic [7:0] cur0,
        input logic [7:0] cur1,
        input logic [7:0] cur2,
        input logic [7:0] cur3,
        input logic [7:0] cur4,
        input logic [7:0] cur5
    );
        key_is_down = key != 8'h00 &&
                      (key == cur0 || key == cur1 || key == cur2 ||
                       key == cur3 || key == cur4 || key == cur5);
    endfunction

    function automatic logic [7:0] first_new_key(
        input logic [7:0] cur0,
        input logic [7:0] cur1,
        input logic [7:0] cur2,
        input logic [7:0] cur3,
        input logic [7:0] cur4,
        input logic [7:0] cur5,
        input logic [7:0] old0,
        input logic [7:0] old1,
        input logic [7:0] old2,
        input logic [7:0] old3,
        input logic [7:0] old4,
        input logic [7:0] old5
    );
        begin
            first_new_key = 8'h00;
            if (cur0 != 8'h00 && !key_was_down(cur0, old0, old1, old2, old3, old4, old5))
                first_new_key = cur0;
            else if (cur1 != 8'h00 && !key_was_down(cur1, old0, old1, old2, old3, old4, old5))
                first_new_key = cur1;
            else if (cur2 != 8'h00 && !key_was_down(cur2, old0, old1, old2, old3, old4, old5))
                first_new_key = cur2;
            else if (cur3 != 8'h00 && !key_was_down(cur3, old0, old1, old2, old3, old4, old5))
                first_new_key = cur3;
            else if (cur4 != 8'h00 && !key_was_down(cur4, old0, old1, old2, old3, old4, old5))
                first_new_key = cur4;
            else if (cur5 != 8'h00 && !key_was_down(cur5, old0, old1, old2, old3, old4, old5))
                first_new_key = cur5;
        end
    endfunction

    function automatic logic [7:0] hid_key_to_ascii(
        input logic [7:0] scan,
        input logic [7:0] modifiers
    );
        logic shift;
        logic ctrl;
        begin
            shift = modifiers[1] || modifiers[5];
            ctrl  = modifiers[0] || modifiers[4];
            hid_key_to_ascii = 8'h00;

            if (scan >= 8'd4 && scan <= 8'd29) begin
                if (ctrl)
                    hid_key_to_ascii = scan - 8'd3;          // Ctrl+A..Z -> 1..26
                else if (shift)
                    hid_key_to_ascii = 8'h41 + (scan - 8'd4);
                else
                    hid_key_to_ascii = 8'h61 + (scan - 8'd4);
            end else begin
                unique case (scan)
                    8'd30: hid_key_to_ascii = shift ? 8'h21 : 8'h31; // 1 !
                    8'd31: hid_key_to_ascii = shift ? 8'h40 : 8'h32; // 2 @
                    8'd32: hid_key_to_ascii = shift ? 8'h23 : 8'h33; // 3 #
                    8'd33: hid_key_to_ascii = shift ? 8'h24 : 8'h34; // 4 $
                    8'd34: hid_key_to_ascii = shift ? 8'h25 : 8'h35; // 5 %
                    8'd35: hid_key_to_ascii = shift ? 8'h5E : 8'h36; // 6 ^
                    8'd36: hid_key_to_ascii = shift ? 8'h26 : 8'h37; // 7 &
                    8'd37: hid_key_to_ascii = shift ? 8'h2A : 8'h38; // 8 *
                    8'd38: hid_key_to_ascii = shift ? 8'h28 : 8'h39; // 9 (
                    8'd39: hid_key_to_ascii = shift ? 8'h29 : 8'h30; // 0 )
                    8'd40: hid_key_to_ascii = 8'h0D;                 // Enter
                    8'd41: hid_key_to_ascii = 8'h1B;                 // Escape
                    8'd42: hid_key_to_ascii = 8'h08;                 // Backspace
                    8'd43: hid_key_to_ascii = 8'h09;                 // Tab
                    8'd44: hid_key_to_ascii = 8'h20;                 // Space
                    8'd45: hid_key_to_ascii = shift ? 8'h5F : 8'h2D; // - _
                    8'd46: hid_key_to_ascii = shift ? 8'h2B : 8'h3D; // = +
                    8'd47: hid_key_to_ascii = shift ? 8'h7B : 8'h5B; // [ {
                    8'd48: hid_key_to_ascii = shift ? 8'h7D : 8'h5D; // ] }
                    8'd49: hid_key_to_ascii = shift ? 8'h7C : 8'h5C; // \ |
                    8'd50: hid_key_to_ascii = shift ? 8'h7E : 8'h23; // non-US # ~
                    8'd51: hid_key_to_ascii = shift ? 8'h3A : 8'h3B; // ; :
                    8'd52: hid_key_to_ascii = shift ? 8'h22 : 8'h27; // ' "
                    8'd53: hid_key_to_ascii = shift ? 8'h7E : 8'h60; // ` ~
                    8'd54: hid_key_to_ascii = shift ? 8'h3C : 8'h2C; // , <
                    8'd55: hid_key_to_ascii = shift ? 8'h3E : 8'h2E; // . >
                    8'd56: hid_key_to_ascii = shift ? 8'h3F : 8'h2F; // / ?
                    8'd76: hid_key_to_ascii = 8'h7F;                 // Delete
                    8'd84: hid_key_to_ascii = 8'h2F;                 // Keypad /
                    8'd85: hid_key_to_ascii = 8'h2A;                 // Keypad *
                    8'd86: hid_key_to_ascii = 8'h2D;                 // Keypad -
                    8'd87: hid_key_to_ascii = 8'h2B;                 // Keypad +
                    8'd88: hid_key_to_ascii = 8'h0D;                 // Keypad Enter
                    8'd89: hid_key_to_ascii = 8'h31;
                    8'd90: hid_key_to_ascii = 8'h32;
                    8'd91: hid_key_to_ascii = 8'h33;
                    8'd92: hid_key_to_ascii = 8'h34;
                    8'd93: hid_key_to_ascii = 8'h35;
                    8'd94: hid_key_to_ascii = 8'h36;
                    8'd95: hid_key_to_ascii = 8'h37;
                    8'd96: hid_key_to_ascii = 8'h38;
                    8'd97: hid_key_to_ascii = 8'h39;
                    8'd98: hid_key_to_ascii = 8'h30;
                    8'd99: hid_key_to_ascii = 8'h2E;                 // Keypad .
                    default: hid_key_to_ascii = 8'h00;
                endcase
            end
        end
    endfunction

    wire [7:0] report_new_scan_usb = first_new_key(
        report_key0_usb, report_key1_usb, report_key2_usb,
        report_key3_usb, report_key4_usb, report_key5_usb,
        prev_key0, prev_key1, prev_key2, prev_key3, prev_key4, prev_key5
    );
    wire [7:0] scan_ascii_usb = hid_key_to_ascii(scan_stage_usb, scan_modifiers_stage_usb);
    wire       event_busy_usb = event_toggle_usb != ack_toggle_usb;
    wire       report_last_event_still_down_usb =
        key_is_down(
            last_event_scan_usb,
            report_key0_usb, report_key1_usb, report_key2_usb,
            report_key3_usb, report_key4_usb, report_key5_usb
        );
    wire       ascii_repeat_suppressed_usb =
        last_event_active_usb && ascii_scan_stage_usb == last_event_scan_usb;

    always_ff @(posedge clk_usb) begin
        if (!usb_reset_n) begin
            prev_key0 <= 8'h00;
            prev_key1 <= 8'h00;
            prev_key2 <= 8'h00;
            prev_key3 <= 8'h00;
            prev_key4 <= 8'h00;
            prev_key5 <= 8'h00;
            pending_data_usb <= 8'h00;
            event_toggle_usb <= 1'b0;
            ack_toggle_usb_meta <= 1'b0;
            ack_toggle_usb <= 1'b0;
            last_event_scan_usb <= 8'h00;
            last_event_active_usb <= 1'b0;
            last_event_release_reports_usb <= 2'd0;
            report_toggle_usb <= 1'b0;
            last_report_scan_usb <= 8'h00;
            usb_lines_usb <= 2'b00;
            report_stage_valid_usb <= 1'b0;
            report_modifiers_usb <= 8'h00;
            report_key0_usb <= 8'h00;
            report_key1_usb <= 8'h00;
            report_key2_usb <= 8'h00;
            report_key3_usb <= 8'h00;
            report_key4_usb <= 8'h00;
            report_key5_usb <= 8'h00;
            scan_stage_valid_usb <= 1'b0;
            scan_stage_usb <= 8'h00;
            scan_modifiers_stage_usb <= 8'h00;
            scan_last_event_still_down_usb <= 1'b0;
            ascii_stage_valid_usb <= 1'b0;
            ascii_scan_stage_usb <= 8'h00;
            ascii_stage_usb <= 8'h00;
            ascii_last_event_still_down_usb <= 1'b0;
        end else begin
            ack_toggle_usb_meta <= ack_toggle_sys;
            ack_toggle_usb <= ack_toggle_usb_meta;
            usb_lines_usb <= {usb_dp, usb_dm};

            report_stage_valid_usb <= usb_report && device_type == 2'd1;
            scan_stage_valid_usb <= report_stage_valid_usb;
            ascii_stage_valid_usb <= scan_stage_valid_usb;

            if (usb_report && device_type == 2'd1) begin
                report_modifiers_usb <= key_modifiers;
                report_key0_usb <= key0;
                report_key1_usb <= key1;
                report_key2_usb <= key2;
                report_key3_usb <= key3;
                report_key4_usb <= key4;
                report_key5_usb <= key5;
            end

            if (report_stage_valid_usb) begin
                scan_stage_usb <= report_new_scan_usb;
                scan_modifiers_stage_usb <= report_modifiers_usb;
                scan_last_event_still_down_usb <= report_last_event_still_down_usb;
                prev_key0 <= report_key0_usb;
                prev_key1 <= report_key1_usb;
                prev_key2 <= report_key2_usb;
                prev_key3 <= report_key3_usb;
                prev_key4 <= report_key4_usb;
                prev_key5 <= report_key5_usb;
            end

            if (scan_stage_valid_usb) begin
                ascii_scan_stage_usb <= scan_stage_usb;
                ascii_stage_usb <= scan_ascii_usb;
                ascii_last_event_still_down_usb <= scan_last_event_still_down_usb;
            end

            if (ascii_stage_valid_usb) begin
                report_toggle_usb <= ~report_toggle_usb;
                last_report_scan_usb <= ascii_scan_stage_usb;

                if (last_event_active_usb && !ascii_last_event_still_down_usb &&
                    last_event_release_reports_usb >= RELEASE_STABLE_REPORTS - 2'd1) begin
                    last_event_active_usb <= 1'b0;
                    last_event_release_reports_usb <= 2'd0;
                end else if (last_event_active_usb && !ascii_last_event_still_down_usb) begin
                    last_event_release_reports_usb <= last_event_release_reports_usb + 1'b1;
                end else if (last_event_active_usb) begin
                    last_event_release_reports_usb <= 2'd0;
                end

                if (ascii_stage_usb != 8'h00 && !event_busy_usb && !ascii_repeat_suppressed_usb) begin
                    pending_data_usb <= ascii_stage_usb;
                    event_toggle_usb <= ~event_toggle_usb;
                    last_event_scan_usb <= ascii_scan_stage_usb;
                    last_event_active_usb <= 1'b1;
                    last_event_release_reports_usb <= 2'd0;
                end
            end
        end
    end

    logic event_toggle_sys_meta;
    logic event_toggle_sys;
    logic event_toggle_sys_last;
    logic report_toggle_sys_meta;
    logic report_toggle_sys;
    logic report_toggle_sys_last;
    logic [1:0] device_type_meta;
    logic [1:0] device_type_sys;
    logic connection_error_meta;
    logic connection_error_sys;
    logic usb_reset_n_meta;
    logic usb_reset_n_sys;
    logic [1:0] usb_lines_meta;
    logic [1:0] usb_lines_sys;
    logic [7:0] core_status_meta;
    logic [7:0] core_status_sys;
    logic [63:0] hid_regs_meta;
    logic [63:0] hid_regs_sys;
    logic report_seen_latched;
    logic key_seen_latched;

    always_ff @(posedge clk_sys) begin
        if (sys_rst) begin
            event_toggle_sys_meta <= 1'b0;
            event_toggle_sys <= 1'b0;
            event_toggle_sys_last <= 1'b0;
            report_toggle_sys_meta <= 1'b0;
            report_toggle_sys <= 1'b0;
            report_toggle_sys_last <= 1'b0;
            device_type_meta <= 2'b00;
            device_type_sys <= 2'b00;
            connection_error_meta <= 1'b0;
            connection_error_sys <= 1'b0;
            usb_reset_n_meta <= 1'b0;
            usb_reset_n_sys <= 1'b0;
            usb_lines_meta <= 2'b00;
            usb_lines_sys <= 2'b00;
            core_status_meta <= 8'h00;
            core_status_sys <= 8'h00;
            hid_regs_meta <= 64'h0;
            hid_regs_sys <= 64'h0;
            report_seen_latched <= 1'b0;
            key_seen_latched <= 1'b0;
            ack_toggle_sys <= 1'b0;
            key_valid <= 1'b0;
            key_data <= 8'h00;
            dbg_status <= 8'h00;
            dbg_device_type <= 8'h00;
            dbg_last_scan <= 8'h00;
            dbg_last_ascii <= 8'h00;
            dbg_report_count <= 8'h00;
            dbg_key_count <= 8'h00;
            dbg_core_status <= 8'h00;
            dbg_hid_regs <= 64'h0;
        end else begin
            key_valid <= 1'b0;
            event_toggle_sys_meta <= event_toggle_usb;
            event_toggle_sys <= event_toggle_sys_meta;
            report_toggle_sys_meta <= report_toggle_usb;
            report_toggle_sys <= report_toggle_sys_meta;
            device_type_meta <= device_type;
            device_type_sys <= device_type_meta;
            connection_error_meta <= connection_error;
            connection_error_sys <= connection_error_meta;
            usb_reset_n_meta <= usb_reset_n;
            usb_reset_n_sys <= usb_reset_n_meta;
            usb_lines_meta <= usb_lines_usb;
            usb_lines_sys <= usb_lines_meta;
            core_status_meta <= dbg_core_status_usb;
            core_status_sys <= core_status_meta;
            hid_regs_meta <= dbg_hid_regs_usb;
            hid_regs_sys <= hid_regs_meta;

            if (report_toggle_sys != report_toggle_sys_last) begin
                report_toggle_sys_last <= report_toggle_sys;
                report_seen_latched <= 1'b1;
                dbg_report_count <= dbg_report_count + 1'b1;
                dbg_last_scan <= last_report_scan_usb;
            end

            if (event_toggle_sys != event_toggle_sys_last) begin
                key_data <= pending_data_usb;
                key_valid <= 1'b1;
                event_toggle_sys_last <= event_toggle_sys;
                ack_toggle_sys <= event_toggle_sys;
            end
            if (key_valid) begin
                key_seen_latched <= 1'b1;
                dbg_key_count <= dbg_key_count + 1'b1;
                dbg_last_ascii <= key_data;
            end

            dbg_device_type <= {6'h00, device_type_sys};
            dbg_core_status <= core_status_sys;
            dbg_hid_regs <= hid_regs_sys;
            dbg_status <= {
                connection_error_sys,
                key_seen_latched,
                report_seen_latched,
                device_type_sys == 2'd1,
                core_status_sys[5],
                usb_lines_sys[1],
                usb_lines_sys[0],
                usb_reset_n_sys
            };
        end
    end

endmodule
