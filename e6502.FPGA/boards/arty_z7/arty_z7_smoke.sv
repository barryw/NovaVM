// arty_z7_smoke.sv — Arty Z7-20 (XC7Z020) bring-up smoke test.
//
// Purpose: prove the AMD/Vivado toolchain, constraints, and JTAG programming
// path end to end on a pure-PL design — no Zynq PS block design required.
//
// What it does:
//   * Heartbeat:  led4_b blinks at ~1 Hz so "alive" is unmistakable.
//   * Counter:    led[3:0] show a free-running binary counter (~3.7 Hz on LSB).
//   * Inputs:     btn[0] resets; sw[0]->led4_r, sw[1]->led4_g prove inputs.
//                 sw[0] also freezes the counter so you can confirm it stops.
//   * UART echo:  115200-8N1 on Pmod JA (uart_rx_in=JA1, uart_tx_out=JA2).
//                 Each received byte is echoed back. led5_r flashes on RX,
//                 led5_g tracks TX-busy.
//   * Visual UART: hold sw[1] high to show the last received byte's low nibble
//                 on led[3:0] — lets you verify RX without a serial terminal.
//
// NOTE: the Arty Z7's onboard USB-UART is wired to the Zynq PS (MIO), NOT the
// PL, so a PL-only design cannot use /dev/ttyUSB1. Connect a 3.3V USB-UART
// adapter to Pmod JA (see README) to exercise the echo path.

`default_nettype none

module arty_z7_smoke (
    input  wire        clk,          // 125 MHz, H16

    input  wire [3:0]  btn,          // D19/D20/L20/L19, active-high
    input  wire [1:0]  sw,           // M20/M19

    output wire [3:0]  led,          // R14/P14/N16/M14
    output wire        led4_r,       // RGB LD4
    output wire        led4_g,
    output wire        led4_b,
    output wire        led5_r,       // RGB LD5
    output wire        led5_g,
    output wire        led5_b,

    input  wire        uart_rx_in,   // Pmod JA1 (Y18) <- adapter TX
    output wire        uart_tx_out   // Pmod JA2 (Y19) -> adapter RX
);

    localparam int CLK_HZ = 125_000_000;
    localparam int BAUD   = 115_200;

    // ---------------------------------------------------------------
    // Reset: btn[0], synchronized into the clk domain.
    // ---------------------------------------------------------------
    logic rst_meta, rst;
    always_ff @(posedge clk) begin
        rst_meta <= btn[0];
        rst      <= rst_meta;
    end

    // ---------------------------------------------------------------
    // Free-running counter (frozen while sw[0] is high).
    // ---------------------------------------------------------------
    logic [31:0] cnt;
    always_ff @(posedge clk) begin
        if (rst)        cnt <= 32'd0;
        else if (!sw[0]) cnt <= cnt + 32'd1;
    end

    // ---------------------------------------------------------------
    // UART receive -> echo back via transmit.
    // ---------------------------------------------------------------
    logic [7:0] rx_data;
    logic       rx_valid;
    logic [7:0] last_rx;
    logic       tx_busy;
    logic       tx_start;
    logic [7:0] tx_data;

    uart_rx #(.CLK_HZ(CLK_HZ), .BAUD(BAUD)) u_rx (
        .clk   (clk),
        .rst   (rst),
        .rx    (uart_rx_in),
        .data  (rx_data),
        .valid (rx_valid)
    );

    uart_tx #(.CLK_HZ(CLK_HZ), .BAUD(BAUD)) u_tx (
        .clk   (clk),
        .rst   (rst),
        .data  (tx_data),
        .start (tx_start),
        .tx    (uart_tx_out),
        .busy  (tx_busy)
    );

    // One-deep "pending byte" handshake: latch a received byte and emit it
    // as soon as the transmitter is free. A second byte arriving before the
    // first is sent is dropped (fine for a smoke test).
    logic       pending;
    logic [7:0] pending_data;
    always_ff @(posedge clk) begin
        if (rst) begin
            pending      <= 1'b0;
            pending_data <= 8'd0;
            last_rx      <= 8'd0;
            tx_start     <= 1'b0;
            tx_data      <= 8'd0;
        end else begin
            tx_start <= 1'b0;
            if (rx_valid) begin
                pending      <= 1'b1;
                pending_data <= rx_data;
                last_rx      <= rx_data;
            end
            if (pending && !tx_busy && !tx_start) begin
                tx_data  <= pending_data;
                tx_start <= 1'b1;
                pending  <= 1'b0;
            end
        end
    end

    // ---------------------------------------------------------------
    // RX activity LED: stretch each received byte so it's visible.
    // ---------------------------------------------------------------
    logic [23:0] rx_flash;
    always_ff @(posedge clk) begin
        if (rst)            rx_flash <= 24'd0;
        else if (rx_valid)  rx_flash <= 24'hFF_FFFF;   // ~0.13 s at 125 MHz
        else if (rx_flash != 0) rx_flash <= rx_flash - 24'd1;
    end

    // ---------------------------------------------------------------
    // Outputs.
    // ---------------------------------------------------------------
    // sw[1] high -> show last received byte nibble; else the counter.
    assign led    = sw[1] ? last_rx[3:0] : cnt[27:24];
    assign led4_r = sw[0];
    assign led4_g = sw[1];
    assign led4_b = cnt[26];            // ~1 Hz heartbeat
    assign led5_r = (rx_flash != 0);
    assign led5_g = tx_busy;
    assign led5_b = 1'b0;

endmodule

`default_nettype wire
