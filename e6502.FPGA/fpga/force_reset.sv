// Force ESP32 into permanent reset - wifi_en always LOW
module force_reset(
    input  logic clk25_mhz,
    output logic wifi_en,
    output logic wifi_gpio0,
    output logic [7:0] leds
);
    assign wifi_en    = 1'b0;   // FORCE EN low - ESP32 should be held in reset
    assign wifi_gpio0 = 1'b1;
    assign leds       = 8'b11110000;  // top 4 on = force_reset indicator
endmodule
