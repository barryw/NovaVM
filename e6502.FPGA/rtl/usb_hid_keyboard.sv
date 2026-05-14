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
    output logic       connection_error
);

    wire       usb_report;
    wire [7:0] key_modifiers;
    wire [7:0] key1;
    wire [7:0] key2;
    wire [7:0] key3;
    wire [7:0] key4;
    wire [7:0] mouse_btn;
    wire signed [7:0] mouse_dx;
    wire signed [7:0] mouse_dy;
    wire game_l, game_r, game_u, game_d;
    wire game_a, game_b, game_x, game_y, game_sel, game_sta;
    wire [63:0] dbg_hid_report;

    usb_hid_host usb_host (
        .usbclk       (clk_usb),
        .usbrst_n     (usb_reset_n),
        .usb_dm       (usb_dm),
        .usb_dp       (usb_dp),
        .typ          (device_type),
        .report       (usb_report),
        .conerr       (connection_error),
        .key_modifiers(key_modifiers),
        .key1         (key1),
        .key2         (key2),
        .key3         (key3),
        .key4         (key4),
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
        .dbg_hid_report(dbg_hid_report)
    );

    assign report_seen = usb_report;

    logic [7:0] prev_key1;
    logic [7:0] prev_key2;
    logic [7:0] prev_key3;
    logic [7:0] prev_key4;
    logic [7:0] pending_data_usb;
    logic       event_toggle_usb;
    logic       ack_toggle_usb_meta;
    logic       ack_toggle_usb;
    logic       ack_toggle_sys;

    function automatic logic key_was_down(
        input logic [7:0] key,
        input logic [7:0] old1,
        input logic [7:0] old2,
        input logic [7:0] old3,
        input logic [7:0] old4
    );
        key_was_down = key != 8'h00 &&
                       (key == old1 || key == old2 || key == old3 || key == old4);
    endfunction

    function automatic logic [7:0] first_new_key(
        input logic [7:0] cur1,
        input logic [7:0] cur2,
        input logic [7:0] cur3,
        input logic [7:0] cur4,
        input logic [7:0] old1,
        input logic [7:0] old2,
        input logic [7:0] old3,
        input logic [7:0] old4
    );
        begin
            first_new_key = 8'h00;
            if (cur1 != 8'h00 && !key_was_down(cur1, old1, old2, old3, old4))
                first_new_key = cur1;
            else if (cur2 != 8'h00 && !key_was_down(cur2, old1, old2, old3, old4))
                first_new_key = cur2;
            else if (cur3 != 8'h00 && !key_was_down(cur3, old1, old2, old3, old4))
                first_new_key = cur3;
            else if (cur4 != 8'h00 && !key_was_down(cur4, old1, old2, old3, old4))
                first_new_key = cur4;
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

    wire [7:0] new_scan = first_new_key(
        key1, key2, key3, key4,
        prev_key1, prev_key2, prev_key3, prev_key4
    );
    wire [7:0] new_ascii = hid_key_to_ascii(new_scan, key_modifiers);
    wire       event_busy_usb = event_toggle_usb != ack_toggle_usb;

    always_ff @(posedge clk_usb) begin
        if (!usb_reset_n) begin
            prev_key1 <= 8'h00;
            prev_key2 <= 8'h00;
            prev_key3 <= 8'h00;
            prev_key4 <= 8'h00;
            pending_data_usb <= 8'h00;
            event_toggle_usb <= 1'b0;
            ack_toggle_usb_meta <= 1'b0;
            ack_toggle_usb <= 1'b0;
        end else begin
            ack_toggle_usb_meta <= ack_toggle_sys;
            ack_toggle_usb <= ack_toggle_usb_meta;

            if (usb_report && device_type == 2'd1) begin
                if (new_ascii != 8'h00 && !event_busy_usb) begin
                    pending_data_usb <= new_ascii;
                    event_toggle_usb <= ~event_toggle_usb;
                end
                prev_key1 <= key1;
                prev_key2 <= key2;
                prev_key3 <= key3;
                prev_key4 <= key4;
            end
        end
    end

    logic event_toggle_sys_meta;
    logic event_toggle_sys;
    logic event_toggle_sys_last;

    always_ff @(posedge clk_sys) begin
        if (sys_rst) begin
            event_toggle_sys_meta <= 1'b0;
            event_toggle_sys <= 1'b0;
            event_toggle_sys_last <= 1'b0;
            ack_toggle_sys <= 1'b0;
            key_valid <= 1'b0;
            key_data <= 8'h00;
        end else begin
            key_valid <= 1'b0;
            event_toggle_sys_meta <= event_toggle_usb;
            event_toggle_sys <= event_toggle_sys_meta;
            if (event_toggle_sys != event_toggle_sys_last) begin
                key_data <= pending_data_usb;
                key_valid <= 1'b1;
                event_toggle_sys_last <= event_toggle_sys;
                ack_toggle_sys <= event_toggle_sys;
            end
        end
    end

endmodule
