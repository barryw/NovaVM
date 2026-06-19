// Pin probe — verify L1/N3 are the right ESP32 Serial2 pins on v3.1.x
// Routes: ESP32 GPIO27 (FPGA input N3) -> FTDI RX (FPGA output L4 to PC)
//         PC FTDI TX (FPGA input M1) -> ESP32 GPIO26 (FPGA output L1)
// If ESP32 firmware Serial2.println("HELLO") appears on USB serial, the pins match.
module pin_probe (
    input  logic ftdi_txd,      // from PC
    output logic ftdi_rxd,      // to PC
    output logic wifi_rxd,      // FPGA -> ESP32 GPIO26 (on L1)
    input  logic wifi_txd,      // ESP32 GPIO27 (on N3) -> FPGA
    output logic [7:0] leds
);
    assign wifi_rxd = ftdi_txd;
    assign ftdi_rxd = wifi_txd;

    assign leds[0] = ftdi_txd;     // PC TX activity
    assign leds[1] = wifi_txd;     // ESP32 TX activity (this flickers if wiring is right)
    assign leds[7:2] = 6'b111111;  // "pin_probe loaded" indicator
endmodule
