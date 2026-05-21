module arty_z7_blinky(
    input wire sys_clk,
    output wire [3:0] leds_4bits_tri_o,
    output wire [5:0] rgb_led_tri_o
);

    reg [31:0] counter = 32'd0;

    always @(posedge sys_clk) begin
        counter <= counter + 32'd1;
    end

    wire [1:0] led_phase = counter[27:26];
    wire [2:0] rgb_phase = counter[28:26];

    assign leds_4bits_tri_o =
        (led_phase == 2'd0) ? 4'b0001 :
        (led_phase == 2'd1) ? 4'b0010 :
        (led_phase == 2'd2) ? 4'b0100 :
                              4'b1000;

    assign rgb_led_tri_o[2:0] =
        (rgb_phase == 3'd0) ? 3'b100 :
        (rgb_phase == 3'd1) ? 3'b010 :
        (rgb_phase == 3'd2) ? 3'b001 :
        (rgb_phase == 3'd3) ? 3'b110 :
        (rgb_phase == 3'd4) ? 3'b011 :
        (rgb_phase == 3'd5) ? 3'b101 :
                              3'b111;

    assign rgb_led_tri_o[5:3] = ~rgb_led_tri_o[2:0];

endmodule

