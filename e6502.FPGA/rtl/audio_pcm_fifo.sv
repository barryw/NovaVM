// Host PCM audio FIFO.
//
// NovaHost streams signed 16-bit stereo PCM frames as four little-endian bytes:
//   left_lo, left_hi, right_lo, right_hi.
// The FIFO lives in the HDMI pixel clock domain. The debug bridge writes bytes
// into it; the HDMI audio strobe pops one complete stereo frame.

module audio_pcm_fifo #(
    parameter int FRAME_ADDR_WIDTH = 10
) (
    input  logic             clk,
    input  logic             rst,

    input  logic             byte_we,
    input  logic [7:0]       byte_data,
    output logic             byte_ready,

    input  logic             sample_en,
    output logic [1:0][15:0] sample_word,
    output logic             sample_valid,
    output logic [15:0]      byte_space,
    output logic [15:0]      underrun_count
);
    localparam int FRAME_DEPTH = 1 << FRAME_ADDR_WIDTH;
    localparam int COUNT_WIDTH = FRAME_ADDR_WIDTH + 1;

    logic [31:0] frame_mem [0:FRAME_DEPTH-1];
    logic [FRAME_ADDR_WIDTH-1:0] wr_ptr;
    logic [FRAME_ADDR_WIDTH-1:0] rd_ptr;
    logic [COUNT_WIDTH-1:0] frame_count;

    logic [1:0]  byte_phase;
    logic [31:0] pending_frame;

    wire frame_full = (frame_count == FRAME_DEPTH[COUNT_WIDTH-1:0]);
    assign byte_ready = !frame_full || (byte_phase != 2'd0);

    wire byte_accept = byte_we && byte_ready;
    wire frame_push = byte_accept && (byte_phase == 2'd3);
    wire frame_pop = sample_en && (frame_count != 0);

    logic [COUNT_WIDTH-1:0] frames_free;
    logic [31:0] byte_space_calc;

    always_comb begin
        frames_free = FRAME_DEPTH[COUNT_WIDTH-1:0] - frame_count;
        if (byte_phase == 2'd0) begin
            byte_space_calc = {27'd0, frames_free} << 2;
        end else if (frames_free == '0) begin
            byte_space_calc = 32'(3'd4 - {1'b0, byte_phase});
        end else begin
            byte_space_calc = ({27'd0, (frames_free - 1'b1)} << 2) +
                              32'(3'd4 - {1'b0, byte_phase});
        end
        byte_space = (byte_space_calc > 32'd65535)
            ? 16'hFFFF
            : byte_space_calc[15:0];
    end

    wire [31:0] completed_frame = {
        byte_data,
        pending_frame[23:0]
    };

    always_ff @(posedge clk) begin
        if (rst) begin
            wr_ptr <= '0;
            rd_ptr <= '0;
            frame_count <= '0;
            byte_phase <= 2'd0;
            pending_frame <= 32'd0;
            sample_word <= '0;
            sample_valid <= 1'b0;
            underrun_count <= 16'd0;
        end else begin
            sample_valid <= 1'b0;

            if (byte_accept) begin
                case (byte_phase)
                    2'd0: pending_frame[7:0]   <= byte_data;
                    2'd1: pending_frame[15:8]  <= byte_data;
                    2'd2: pending_frame[23:16] <= byte_data;
                    default: begin
                        pending_frame[31:24] <= byte_data;
                        frame_mem[wr_ptr] <= completed_frame;
                        wr_ptr <= wr_ptr + 1'b1;
                    end
                endcase
                byte_phase <= byte_phase + 2'd1;
            end

            if (sample_en) begin
                if (frame_count != 0) begin
                    sample_word[0] <= frame_mem[rd_ptr][15:0];
                    sample_word[1] <= frame_mem[rd_ptr][31:16];
                    sample_valid <= 1'b1;
                    rd_ptr <= rd_ptr + 1'b1;
                end else begin
                    sample_word <= '0;
                    if (underrun_count != 16'hFFFF)
                        underrun_count <= underrun_count + 16'd1;
                end
            end

            case ({frame_push, frame_pop})
                2'b10: frame_count <= frame_count + 1'b1;
                2'b01: frame_count <= frame_count - 1'b1;
                default: ;
            endcase
        end
    end
endmodule
