// Simple UART receiver — 8N1, no parity
// Active-high 'valid' pulses for one clock when a byte is received

module uart_rx #(
    parameter CLK_HZ   = 25000000,
    parameter BAUD     = 115200
)(
    input  logic       clk,
    input  logic       rst,
    input  logic       rx,        // serial input (active low)
    output logic [7:0] data,
    output logic       valid
);

    localparam CLKS_PER_BIT = CLK_HZ / BAUD;
    localparam HALF_BIT     = CLKS_PER_BIT / 2;

    typedef enum logic [1:0] {
        IDLE, START, DATA, STOP
    } state_t;

    state_t state;
    logic [$clog2(CLKS_PER_BIT)-1:0] clk_cnt;
    logic [2:0] bit_idx;
    logic [7:0] shift;

    // Double-register RX for metastability
    logic rx_r1, rx_r2;
    always_ff @(posedge clk) begin
        rx_r1 <= rx;
        rx_r2 <= rx_r1;
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            state   <= IDLE;
            valid   <= 0;
            data    <= 0;
            clk_cnt <= 0;
            bit_idx <= 0;
            shift   <= 0;
        end else begin
            valid <= 0;

            case (state)
                IDLE: begin
                    if (!rx_r2) begin           // start bit detected
                        state   <= START;
                        clk_cnt <= 0;
                    end
                end

                START: begin
                    if (clk_cnt == HALF_BIT) begin
                        if (!rx_r2) begin       // still low at midpoint — valid start
                            state   <= DATA;
                            clk_cnt <= 0;
                            bit_idx <= 0;
                        end else begin
                            state <= IDLE;      // glitch, abort
                        end
                    end else begin
                        clk_cnt <= clk_cnt + 1;
                    end
                end

                DATA: begin
                    if (clk_cnt == CLKS_PER_BIT - 1) begin
                        shift[bit_idx] <= rx_r2;
                        clk_cnt <= 0;
                        if (bit_idx == 7) begin
                            state <= STOP;
                        end else begin
                            bit_idx <= bit_idx + 1;
                        end
                    end else begin
                        clk_cnt <= clk_cnt + 1;
                    end
                end

                STOP: begin
                    if (clk_cnt == CLKS_PER_BIT - 1) begin
                        valid <= 1;
                        data  <= shift;
                        state <= IDLE;
                    end else begin
                        clk_cnt <= clk_cnt + 1;
                    end
                end
            endcase
        end
    end

endmodule
