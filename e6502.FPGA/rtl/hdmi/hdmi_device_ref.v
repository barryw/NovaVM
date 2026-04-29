/*
 * Reference HDMI/DVI encoder path for ULX3S diagnostics.
 *
 * Derived from my_hdmi_device by Hirosh Dabui <hirosh@dabui.de>.
 * The original license permits use, copy, modification, and distribution
 * with this notice retained.
 */

module hdmi_device_ref #(
    parameter DDR_ENABLED = 1
) (
    input pclk,
    input tmds_clk,

    input [7:0] in_vga_red,
    input [7:0] in_vga_green,
    input [7:0] in_vga_blue,

    input in_vga_blank,
    input in_vga_vsync,
    input in_vga_hsync,

    output [1:0] out_tmds_red,
    output [1:0] out_tmds_green,
    output [1:0] out_tmds_blue,
    output [1:0] out_tmds_clk
);
    localparam OUT_TMDS_MSB = DDR_ENABLED ? 1 : 0;

    wire [9:0] tmds_red;
    wire [9:0] tmds_green;
    wire [9:0] tmds_blue;

    tmds_encoder_ref tmds_encoder_red (
        .clk(pclk),
        .DE(~in_vga_blank),
        .D(in_vga_red),
        .C1(1'b0),
        .C0(1'b0),
        .q_out(tmds_red)
    );

    tmds_encoder_ref tmds_encoder_green (
        .clk(pclk),
        .DE(~in_vga_blank),
        .D(in_vga_green),
        .C1(1'b0),
        .C0(1'b0),
        .q_out(tmds_green)
    );

    tmds_encoder_ref tmds_encoder_blue (
        .clk(pclk),
        .DE(~in_vga_blank),
        .D(in_vga_blue),
        .C1(in_vga_vsync),
        .C0(in_vga_hsync),
        .q_out(tmds_blue)
    );

    reg       tmds_shift_load  = 1'b0;
    reg [3:0] tmds_modulo      = 4'd0;
    reg [9:0] tmds_shift_red   = 10'd0;
    reg [9:0] tmds_shift_green = 10'd0;
    reg [9:0] tmds_shift_blue  = 10'd0;
    reg [9:0] tmds_shift_clk   = 10'd0;

    wire [9:0] tmds_pixel_clk = 10'b00000_11111;
    wire max_shifts_reached = tmds_modulo == (DDR_ENABLED ? 4 : 9);

    always @(posedge tmds_clk)
        tmds_modulo <= max_shifts_reached ? 4'd0 : tmds_modulo + 4'd1;

    always @(posedge tmds_clk)
        tmds_shift_load <= max_shifts_reached;

    always @(posedge tmds_clk) begin
        tmds_shift_red   <= tmds_shift_load ? tmds_red       : {DDR_ENABLED ? 2'b00 : 1'b0, tmds_shift_red[9: DDR_ENABLED ? 2 : 1]};
        tmds_shift_green <= tmds_shift_load ? tmds_green     : {DDR_ENABLED ? 2'b00 : 1'b0, tmds_shift_green[9: DDR_ENABLED ? 2 : 1]};
        tmds_shift_blue  <= tmds_shift_load ? tmds_blue      : {DDR_ENABLED ? 2'b00 : 1'b0, tmds_shift_blue[9: DDR_ENABLED ? 2 : 1]};
        tmds_shift_clk   <= tmds_shift_load ? tmds_pixel_clk : {DDR_ENABLED ? 2'b00 : 1'b0, tmds_shift_clk[9: DDR_ENABLED ? 2 : 1]};
    end

    assign out_tmds_clk   = DDR_ENABLED ? tmds_shift_clk[1:0]   : {1'b0, tmds_shift_clk[0]};
    assign out_tmds_red   = DDR_ENABLED ? tmds_shift_red[1:0]   : {1'b0, tmds_shift_red[0]};
    assign out_tmds_green = DDR_ENABLED ? tmds_shift_green[1:0] : {1'b0, tmds_shift_green[0]};
    assign out_tmds_blue  = DDR_ENABLED ? tmds_shift_blue[1:0]  : {1'b0, tmds_shift_blue[0]};
endmodule

module tmds_encoder_ref (
    input clk,
    input DE,
    input [7:0] D,
    input C1,
    input C0,
    output reg [9:0] q_out = 10'd0
);
    function [3:0] N0;
        input [7:0] d;
        integer i;
        begin
            N0 = 4'd0;
            for (i = 0; i < 8; i = i + 1)
                N0 = N0 + !d[i];
        end
    endfunction

    function [3:0] N1;
        input [7:0] d;
        integer i;
        begin
            N1 = 4'd0;
            for (i = 0; i < 8; i = i + 1)
                N1 = N1 + d[i];
        end
    endfunction

    reg signed [7:0] cnt_prev = 8'sd0;
    reg signed [7:0] cnt = 8'sd0;
    reg [8:0] q_m;

    always @(*) begin
        if ((N1(D) > 4) || ((N1(D) == 4) && (D[0] == 1'b0))) begin
            q_m[0] = D[0];
            q_m[1] = q_m[0] ~^ D[1];
            q_m[2] = q_m[1] ~^ D[2];
            q_m[3] = q_m[2] ~^ D[3];
            q_m[4] = q_m[3] ~^ D[4];
            q_m[5] = q_m[4] ~^ D[5];
            q_m[6] = q_m[5] ~^ D[6];
            q_m[7] = q_m[6] ~^ D[7];
            q_m[8] = 1'b0;
        end else begin
            q_m[0] = D[0];
            q_m[1] = q_m[0] ^ D[1];
            q_m[2] = q_m[1] ^ D[2];
            q_m[3] = q_m[2] ^ D[3];
            q_m[4] = q_m[3] ^ D[4];
            q_m[5] = q_m[4] ^ D[5];
            q_m[6] = q_m[5] ^ D[6];
            q_m[7] = q_m[6] ^ D[7];
            q_m[8] = 1'b1;
        end
    end

    always @(posedge clk) begin
        if (DE) begin
            if ((cnt_prev == 0) || (N1(q_m[7:0]) == N0(q_m[7:0]))) begin
                q_out[9]   <= ~q_m[8];
                q_out[8]   <=  q_m[8];
                q_out[7:0] <= q_m[8] ? q_m[7:0] : ~q_m[7:0];

                if (q_m[8] == 1'b0)
                    cnt = cnt_prev + (N0(q_m[7:0]) - N1(q_m[7:0]));
                else
                    cnt = cnt_prev + (N1(q_m[7:0]) - N0(q_m[7:0]));
            end else begin
                if ((cnt_prev > 0 && (N1(q_m[7:0]) > N0(q_m[7:0]))) ||
                    (cnt_prev < 0 && (N0(q_m[7:0]) > N1(q_m[7:0])))) begin
                    q_out[9]   <= 1'b1;
                    q_out[8]   <= q_m[8];
                    q_out[7:0] <= ~q_m[7:0];
                    cnt = cnt_prev + 2 * q_m[8] + (N0(q_m[7:0]) - N1(q_m[7:0]));
                end else begin
                    q_out[9]   <= 1'b0;
                    q_out[8]   <= q_m[8];
                    q_out[7:0] <= q_m[7:0];
                    cnt = cnt_prev - {~q_m[8], 1'b0} + (N1(q_m[7:0]) - N0(q_m[7:0]));
                end
            end
        end else begin
            cnt = 8'sd0;
            case ({C1, C0})
                2'b00: q_out <= 10'b1101010100;
                2'b01: q_out <= 10'b0010101011;
                2'b10: q_out <= 10'b0101010100;
                2'b11: q_out <= 10'b1010101011;
            endcase
        end

        cnt_prev <= cnt;
    end
endmodule
