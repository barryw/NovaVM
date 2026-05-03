// Ready/valid UART transmitter for the ESP32 debug link, 8N1.

module debug_uart_tx #(
    parameter int CLK_HZ = 25_000_000,
    parameter int BAUD   = 115_200
) (
    input  logic       clk,
    input  logic       rst,

    input  logic [7:0] data,
    input  logic       valid,
    output logic       ready,

    output logic       tx,
    output logic       busy
);

    localparam int CLKS_PER_BIT = (CLK_HZ + (BAUD / 2)) / BAUD;
    localparam int CTR_WIDTH    = (CLKS_PER_BIT <= 2) ? 1 : $clog2(CLKS_PER_BIT);
    localparam logic [CTR_WIDTH-1:0] BIT_LAST = CLKS_PER_BIT - 1;

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

    assign ready = (state == S_IDLE);
    assign busy  = (state != S_IDLE) || (valid && ready);

    always_ff @(posedge clk) begin
        if (rst) begin
            state   <= S_IDLE;
            clk_cnt <= '0;
            bit_idx <= '0;
            shift   <= '0;
            tx      <= 1'b1;
        end else begin
            case (state)
                S_IDLE: begin
                    tx      <= 1'b1;
                    clk_cnt <= '0;
                    bit_idx <= '0;
                    if (valid) begin
                        shift <= data;
                        state <= S_START;
                    end
                end

                S_START: begin
                    tx <= 1'b0;
                    if (clk_cnt == BIT_LAST) begin
                        clk_cnt <= '0;
                        bit_idx <= '0;
                        state   <= S_DATA;
                    end else begin
                        clk_cnt <= clk_cnt + 1'b1;
                    end
                end

                S_DATA: begin
                    tx <= shift[bit_idx];
                    if (clk_cnt == BIT_LAST) begin
                        clk_cnt <= '0;
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
                    tx <= 1'b1;
                    if (clk_cnt == BIT_LAST) begin
                        clk_cnt <= '0;
                        state   <= S_IDLE;
                    end else begin
                        clk_cnt <= clk_cnt + 1'b1;
                    end
                end

                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
