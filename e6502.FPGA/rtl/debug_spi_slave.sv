// SPI slave byte transport for the ESP32 debug bridge.
//
// Mode 0, MSB first. The ESP32 is the SPI master. This is not a raw UART-like
// byte pipe: SPI reads require dummy MOSI clocks, so every transaction starts
// with an operation byte:
//   WRITE_OP: subsequent MOSI bytes are queued into the bridge RX stream.
//   READ_OP:  MOSI bytes are ignored; MISO returns DATA/EMPTY tokens.
//
// READ_OP response bytes are framed as TOKEN,DATA pairs so payload value
// collisions with the idle/empty token cannot corrupt binary reads.
//
// This intentionally starts as a conservative oversampled slave in clk domain.
// Keep SCK in the 500 kHz-1 MHz range for hardware bring-up on the 25/27 MHz
// pixel clock; raise it only after timing validation or a true SCK-domain path.

module debug_spi_slave #(
    parameter int ADDR_WIDTH = 9,
    parameter logic [7:0] IDLE_BYTE = 8'hA5,
    parameter logic [7:0] WRITE_OP = 8'h57,       // 'W'
    parameter logic [7:0] READ_OP = 8'h52,        // 'R'
    parameter logic [7:0] READ_TOKEN_EMPTY = 8'h00,
    parameter logic [7:0] READ_TOKEN_DATA = 8'h01
) (
    input  logic       clk,
    input  logic       rst,

    input  logic       spi_sck,
    input  logic       spi_cs_n,
    input  logic       spi_mosi,
    output logic       spi_miso,
    output logic       spi_miso_oe,

    output logic [7:0] rx_data,
    output logic       rx_valid,
    input  logic       rx_ready,

    input  logic [7:0] tx_data,
    input  logic       tx_start,
    output logic       tx_busy,

    output logic       rx_overflow,
    output logic       tx_overflow,
    output logic       tx_underflow,
    output logic       selected_seen
);

    logic [2:0] sck_sync;
    logic [2:0] cs_sync;
    logic [2:0] mosi_sync;

    always_ff @(posedge clk) begin
        if (rst) begin
            sck_sync  <= 3'b000;
            cs_sync   <= 3'b111;
            mosi_sync <= 3'b000;
        end else begin
            sck_sync  <= {sck_sync[1:0], spi_sck};
            cs_sync   <= {cs_sync[1:0], spi_cs_n};
            mosi_sync <= {mosi_sync[1:0], spi_mosi};
        end
    end

    wire selected = !cs_sync[1];
    wire cs_fall  =  cs_sync[2] && !cs_sync[1];
    wire cs_rise  = !cs_sync[2] &&  cs_sync[1];
    wire sck_rise = selected && !sck_sync[2] &&  sck_sync[1];
    wire sck_fall = selected &&  sck_sync[2] && !sck_sync[1];

    // MISO shares the ESP32 SD-card bus, so release the pad immediately when
    // the FPGA chip-select is high. The byte state machine can tolerate the
    // synchronized CS latency; the shared bus cannot tolerate stale drive.
    assign spi_miso_oe = !spi_cs_n;

    typedef enum logic [1:0] {
        SPI_MODE_OPCODE = 2'd0,
        SPI_MODE_WRITE  = 2'd1,
        SPI_MODE_READ   = 2'd2
    } spi_mode_t;

    spi_mode_t spi_mode;

    logic [7:0] rx_shift;
    logic [2:0] rx_bit_count;
    logic [7:0] rx_fifo_in;
    logic       rx_fifo_push;
    wire        rx_fifo_ready;

    logic [7:0] tx_shift;
    logic [2:0] tx_bit_count;
    logic       read_data_phase;
    wire [7:0]  tx_fifo_out;
    wire        tx_fifo_valid;
    logic       tx_fifo_pop;
    wire        tx_fifo_ready;

    assign tx_busy = !tx_fifo_ready;

    debug_byte_fifo #(
        .ADDR_WIDTH(ADDR_WIDTH)
    ) rx_fifo (
        .clk      (clk),
        .rst      (rst),
        .s_data   (rx_fifo_in),
        .s_valid  (rx_fifo_push),
        .s_ready  (rx_fifo_ready),
        .m_data   (rx_data),
        .m_valid  (rx_valid),
        .m_ready  (rx_ready),
        .overflow (rx_overflow),
        .fill     ()
    );

    debug_byte_fifo #(
        .ADDR_WIDTH(ADDR_WIDTH)
    ) tx_fifo (
        .clk      (clk),
        .rst      (rst),
        .s_data   (tx_data),
        .s_valid  (tx_start),
        .s_ready  (tx_fifo_ready),
        .m_data   (tx_fifo_out),
        .m_valid  (tx_fifo_valid),
        .m_ready  (tx_fifo_pop),
        .overflow (tx_overflow),
        .fill     ()
    );

    wire [7:0] rx_next_byte = {rx_shift[6:0], mosi_sync[1]};

    logic [7:0] tx_next_byte;
    always_comb begin
        tx_next_byte = IDLE_BYTE;
        if (spi_mode == SPI_MODE_READ) begin
            if (read_data_phase)
                tx_next_byte = tx_fifo_valid ? tx_fifo_out : IDLE_BYTE;
            else
                tx_next_byte = tx_fifo_valid ? READ_TOKEN_DATA : READ_TOKEN_EMPTY;
        end
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            rx_shift      <= 8'h00;
            rx_bit_count  <= 3'd0;
            rx_fifo_in    <= 8'h00;
            rx_fifo_push  <= 1'b0;
            tx_shift      <= IDLE_BYTE;
            tx_bit_count  <= 3'd0;
            read_data_phase <= 1'b0;
            tx_fifo_pop   <= 1'b0;
            spi_miso      <= 1'b1;
            tx_underflow  <= 1'b0;
            selected_seen <= 1'b0;
            spi_mode      <= SPI_MODE_OPCODE;
        end else begin
            rx_fifo_push <= 1'b0;
            tx_fifo_pop  <= 1'b0;

            if (cs_rise) begin
                rx_bit_count <= 3'd0;
                tx_bit_count <= 3'd0;
                tx_shift     <= IDLE_BYTE;
                spi_miso     <= 1'b1;
                spi_mode     <= SPI_MODE_OPCODE;
                read_data_phase <= 1'b0;
            end

            if (cs_fall) begin
                selected_seen <= 1'b1;
                rx_shift      <= 8'h00;
                rx_bit_count  <= 3'd0;
                tx_bit_count  <= 3'd0;
                tx_shift      <= IDLE_BYTE;
                spi_miso      <= IDLE_BYTE[7];
                spi_mode      <= SPI_MODE_OPCODE;
                read_data_phase <= 1'b0;
            end

            if (sck_rise) begin
                rx_shift <= rx_next_byte;
                if (rx_bit_count == 3'd7) begin
                    rx_bit_count <= 3'd0;
                    unique case (spi_mode)
                        SPI_MODE_OPCODE: begin
                            if (rx_next_byte == WRITE_OP)
                                spi_mode <= SPI_MODE_WRITE;
                            else if (rx_next_byte == READ_OP)
                                spi_mode <= SPI_MODE_READ;
                        end

                        SPI_MODE_WRITE: begin
                            rx_fifo_in   <= rx_next_byte;
                            rx_fifo_push <= 1'b1;
                        end

                        default: ;
                    endcase
                end else begin
                    rx_bit_count <= rx_bit_count + 1'b1;
                end
            end

            if (sck_fall) begin
                if (tx_bit_count == 3'd7) begin
                    tx_bit_count <= 3'd0;
                    tx_shift     <= tx_next_byte;
                    spi_miso     <= tx_next_byte[7];
                    if (spi_mode == SPI_MODE_READ) begin
                        if (read_data_phase) begin
                            if (tx_fifo_valid) begin
                                tx_fifo_pop <= 1'b1;
                            end else begin
                                tx_underflow <= 1'b1;
                            end
                            read_data_phase <= 1'b0;
                        end else if (tx_fifo_valid) begin
                            read_data_phase <= 1'b1;
                        end else begin
                            tx_underflow <= 1'b1;
                        end
                    end
                end else begin
                    tx_bit_count <= tx_bit_count + 1'b1;
                    tx_shift     <= {tx_shift[6:0], 1'b0};
                    spi_miso     <= tx_shift[6];
                end
            end
        end
    end

endmodule
