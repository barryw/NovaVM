// Ready/valid UART receiver for the ESP32 debug link.
//
// Good ideas intentionally kept from production UART cores:
// - 2FF input synchronizer
// - start-bit validation at the midpoint
// - data and stop-bit sampling at bit centers
// - output valid holds until ready
// - explicit framing and overrun pulses instead of silent byte replacement

module debug_uart_rx #(
    parameter int CLK_HZ = 25_000_000,
    parameter int BAUD   = 115_200
) (
    input  logic       clk,
    input  logic       rst,

    input  logic       rx,

    output logic [7:0] data,
    output logic       valid,
    input  logic       ready,

    output logic       busy,
    output logic       overrun_error,
    output logic       frame_error
);

    localparam int CLKS_PER_BIT = (CLK_HZ + (BAUD / 2)) / BAUD;
    localparam int HALF_BIT     = CLKS_PER_BIT / 2;
    localparam int CTR_WIDTH    = (CLKS_PER_BIT <= 2) ? 1 : $clog2(CLKS_PER_BIT);
    localparam logic [CTR_WIDTH-1:0] START_LAST = HALF_BIT - 1;
    localparam logic [CTR_WIDTH-1:0] BIT_LAST   = CLKS_PER_BIT - 1;

    typedef enum logic [1:0] {
        S_IDLE,
        S_START,
        S_DATA,
        S_STOP
    } state_t;

    state_t state;
    logic [CTR_WIDTH-1:0] clk_cnt;
    logic [2:0]           bit_idx;
    logic [7:0]           shift;
    logic                 rx_meta;
    logic                 rx_sync;

    assign busy = (state != S_IDLE);

    always_ff @(posedge clk) begin
        if (rst) begin
            state         <= S_IDLE;
            clk_cnt       <= '0;
            bit_idx       <= '0;
            shift         <= '0;
            rx_meta       <= 1'b1;
            rx_sync       <= 1'b1;
            data          <= 8'h00;
            valid         <= 1'b0;
            overrun_error <= 1'b0;
            frame_error   <= 1'b0;
        end else begin
            rx_meta       <= rx;
            rx_sync       <= rx_meta;
            overrun_error <= 1'b0;
            frame_error   <= 1'b0;

            if (valid && ready)
                valid <= 1'b0;

            case (state)
                S_IDLE: begin
                    clk_cnt <= '0;
                    bit_idx <= '0;
                    if (!rx_sync)
                        state <= S_START;
                end

                S_START: begin
                    if (clk_cnt == START_LAST) begin
                        clk_cnt <= '0;
                        if (!rx_sync) begin
                            bit_idx <= '0;
                            state   <= S_DATA;
                        end else begin
                            state <= S_IDLE;
                        end
                    end else begin
                        clk_cnt <= clk_cnt + 1'b1;
                    end
                end

                S_DATA: begin
                    if (clk_cnt == BIT_LAST) begin
                        clk_cnt        <= '0;
                        shift[bit_idx] <= rx_sync;
                        if (bit_idx == 3'd7) begin
                            state <= S_STOP;
                        end else begin
                            bit_idx <= bit_idx + 1'b1;
                        end
                    end else begin
                        clk_cnt <= clk_cnt + 1'b1;
                    end
                end

                S_STOP: begin
                    if (clk_cnt == BIT_LAST) begin
                        clk_cnt <= '0;
                        state   <= S_IDLE;
                        if (!rx_sync) begin
                            frame_error <= 1'b1;
                        end else if (valid && !ready) begin
                            overrun_error <= 1'b1;
                        end else begin
                            data  <= shift;
                            valid <= 1'b1;
                        end
                    end else begin
                        clk_cnt <= clk_cnt + 1'b1;
                    end
                end

                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
