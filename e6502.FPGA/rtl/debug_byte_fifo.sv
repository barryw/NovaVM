// Lightweight byte FIFO for the ESP32 debug UART path.
//
// This intentionally implements only the ready/valid byte stream Nova needs:
// no AXI sidebands, frame mode, pause logic, or packet metadata. Depth is a
// power of two so pointer arithmetic stays cheap; 512 bytes covers one full
// Nova debug packet (5-byte header + 256-byte payload) with headroom.

module debug_byte_fifo #(
    parameter int ADDR_WIDTH = 9
) (
    input  logic       clk,
    input  logic       rst,

    input  logic [7:0] s_data,
    input  logic       s_valid,
    output logic       s_ready,

    output logic [7:0] m_data,
    output logic       m_valid,
    input  logic       m_ready,

    output logic       overflow,
    output logic [ADDR_WIDTH:0] fill
);

    localparam int DEPTH = 1 << ADDR_WIDTH;
    localparam logic [ADDR_WIDTH:0] DEPTH_COUNT = {1'b1, {ADDR_WIDTH{1'b0}}};

    (* ram_style = "distributed", ramstyle = "logic" *)
    logic [7:0] mem [0:DEPTH-1];

    logic [ADDR_WIDTH-1:0] wr_ptr;
    logic [ADDR_WIDTH-1:0] rd_ptr;
    logic [ADDR_WIDTH:0]   count;

    wire full  = (count == DEPTH_COUNT);
    wire empty = (count == '0);
    wire push  = s_valid && !full;
    wire pop   = m_ready && !empty;

    assign s_ready = !full;
    assign m_valid = !empty;
    assign m_data  = mem[rd_ptr];
    assign fill    = count;

    always_ff @(posedge clk) begin
        if (rst) begin
            wr_ptr   <= '0;
            rd_ptr   <= '0;
            count    <= '0;
            overflow <= 1'b0;
        end else begin
            if (s_valid && full)
                overflow <= 1'b1;

            if (push) begin
                mem[wr_ptr] <= s_data;
                wr_ptr      <= wr_ptr + 1'b1;
            end

            if (pop)
                rd_ptr <= rd_ptr + 1'b1;

            unique case ({push, pop})
                2'b10: count <= count + 1'b1;
                2'b01: count <= count - 1'b1;
                default: ;
            endcase
        end
    end

endmodule
