// ESP32 passthru for this ULX3S board's ESP routing.
//
// FTDI is active-low (ndtr/nrts go LOW when asserted by host):
//   DTR -> GPIO0
//   RTS -> EN
//
// This follows the official ULX3S passthrough truth table. In particular,
// when both DTR and RTS are asserted, EN/GPIO0 are released high instead of
// holding the ESP32 in reset.

module passthru (
    input  logic clk25_mhz,
    input  logic ftdi_txd,
    output logic ftdi_rxd,
    input  logic ftdi_ndtr,
    input  logic ftdi_nrts,
    output logic wifi_rxd,
    input  logic wifi_txd,
    output logic wifi_en,
    output logic wifi_gpio0,
    output logic wifi_gpio2,
    output logic [7:0] leds
);
    // Serial passthru
    assign wifi_rxd = ftdi_txd;
    assign ftdi_rxd = wifi_txd;

    logic [1:0] prog_out;
    always_comb begin
        case ({ftdi_ndtr, ftdi_nrts})
            2'b10:   prog_out = 2'b01; // RTS asserted: EN=0, GPIO0=1
            2'b01:   prog_out = 2'b10; // DTR asserted: EN=1, GPIO0=0
            default: prog_out = 2'b11; // released, or both asserted
        endcase
    end

    assign wifi_en    = prog_out[1];
    assign wifi_gpio0 = prog_out[0];
    assign wifi_gpio2 = 1'b0;

    // LED indicators
    assign leds[0] = ~ftdi_ndtr;  // DTR asserted
    assign leds[1] = ~ftdi_nrts;  // RTS asserted
    assign leds[2] = wifi_en;
    assign leds[3] = wifi_gpio0;
    assign leds[4] = ftdi_txd;
    assign leds[5] = wifi_txd;
    assign leds[6] = 1'b1;        // passthru v2 indicator
    assign leds[7] = 1'b1;        // second indicator = new version
endmodule
