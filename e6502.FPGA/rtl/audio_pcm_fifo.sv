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

    logic [FRAME_ADDR_WIDTH-1:0] wr_ptr;
    logic [FRAME_ADDR_WIDTH-1:0] rd_ptr;
    logic [COUNT_WIDTH-1:0] stored_count;

    logic [1:0]  byte_phase;
    logic [31:0] pending_frame;
    logic [31:0] head_frame;
    logic        head_valid;
    logic        prefetch_pending;
    wire [31:0] mem_dout;
    wire [31:0] unused_mem_dout;

    wire [COUNT_WIDTH-1:0] queued_count = stored_count +
        COUNT_WIDTH'(head_valid) + COUNT_WIDTH'(prefetch_pending);
    wire frame_full = (queued_count == FRAME_DEPTH[COUNT_WIDTH-1:0]);
    assign byte_ready = !frame_full || (byte_phase != 2'd0);

    wire byte_accept = byte_we && byte_ready;
    wire frame_push = byte_accept && (byte_phase == 2'd3);
    wire frame_pop = sample_en && head_valid;
    wire direct_push_to_head = frame_push && !prefetch_pending &&
        ((!head_valid && stored_count == '0) || (frame_pop && stored_count == '0));
    wire mem_push = frame_push && !direct_push_to_head;
    wire start_prefetch = !prefetch_pending &&
        ((frame_pop && stored_count != '0) || (!head_valid && stored_count != '0));

    wire [31:0] completed_frame = {
        byte_data,
        pending_frame[23:0]
    };

`ifdef NOVA_VIVADO
    xpm_memory_sdpram #(
        .MEMORY_SIZE             (FRAME_DEPTH * 32),
        .MEMORY_PRIMITIVE        ("block"),
        .CLOCKING_MODE           ("common_clock"),
        .MEMORY_INIT_FILE        ("none"),
        .MEMORY_INIT_PARAM       (""),
        .USE_MEM_INIT            (0),
        .WAKEUP_TIME             ("disable_sleep"),
        .MESSAGE_CONTROL         (0),
        .ECC_MODE                ("no_ecc"),
        .AUTO_SLEEP_TIME         (0),
        .USE_EMBEDDED_CONSTRAINT (0),
        .WRITE_DATA_WIDTH_A      (32),
        .BYTE_WRITE_WIDTH_A      (32),
        .ADDR_WIDTH_A            (FRAME_ADDR_WIDTH),
        .READ_DATA_WIDTH_B       (32),
        .ADDR_WIDTH_B            (FRAME_ADDR_WIDTH),
        .READ_RESET_VALUE_B      ("0"),
        .READ_LATENCY_B          (1),
        .WRITE_MODE_B            ("read_first")
    ) frame_mem (
        .sleep          (1'b0),
        .clka           (clk),
        .ena            (mem_push),
        .wea            (mem_push),
        .addra          (wr_ptr),
        .dina           (completed_frame),
        .injectsbiterra (1'b0),
        .injectdbiterra (1'b0),
        .clkb           (clk),
        .rstb           (1'b0),
        .enb            (1'b1),
        .regceb         (1'b1),
        .addrb          (rd_ptr),
        .doutb          (mem_dout),
        .sbiterrb       (),
        .dbiterrb       ()
    );
`else
    dpram #(.WIDTH(32), .DEPTH(FRAME_DEPTH)) frame_mem (
        .clk(clk),
        .addr_a(wr_ptr),
        .din_a(completed_frame),
        .we_a(mem_push),
        .dout_a(unused_mem_dout),
        .addr_b(rd_ptr),
        .dout_b(mem_dout)
    );
`endif

    logic [COUNT_WIDTH-1:0] frames_free;
    logic [31:0] byte_space_calc;

    always_comb begin
        frames_free = FRAME_DEPTH[COUNT_WIDTH-1:0] - queued_count;
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

    always_ff @(posedge clk) begin
        if (rst) begin
            wr_ptr <= '0;
            rd_ptr <= '0;
            stored_count <= '0;
            byte_phase <= 2'd0;
            pending_frame <= 32'd0;
            head_frame <= 32'd0;
            head_valid <= 1'b0;
            prefetch_pending <= 1'b0;
            sample_word <= '0;
            sample_valid <= 1'b0;
            underrun_count <= 16'd0;
        end else begin
            sample_valid <= 1'b0;

            if (prefetch_pending) begin
                head_frame <= mem_dout;
                head_valid <= 1'b1;
                prefetch_pending <= 1'b0;
            end

            if (byte_accept) begin
                case (byte_phase)
                    2'd0: pending_frame[7:0]   <= byte_data;
                    2'd1: pending_frame[15:8]  <= byte_data;
                    2'd2: pending_frame[23:16] <= byte_data;
                    default: pending_frame[31:24] <= byte_data;
                endcase
                byte_phase <= byte_phase + 2'd1;
            end

            if (sample_en) begin
                if (head_valid) begin
                    sample_word[0] <= head_frame[15:0];
                    sample_word[1] <= head_frame[31:16];
                    sample_valid <= 1'b1;
                    head_valid <= 1'b0;
                end else begin
                    sample_word <= '0;
                    if (underrun_count != 16'hFFFF)
                        underrun_count <= underrun_count + 16'd1;
                end
            end

            if (direct_push_to_head) begin
                head_frame <= completed_frame;
                head_valid <= 1'b1;
            end

            if (mem_push)
                wr_ptr <= wr_ptr + 1'b1;

            if (start_prefetch) begin
                rd_ptr <= rd_ptr + 1'b1;
                prefetch_pending <= 1'b1;
            end

            case ({mem_push, start_prefetch})
                2'b10: stored_count <= stored_count + 1'b1;
                2'b01: stored_count <= stored_count - 1'b1;
                default: ;
            endcase
        end
    end
endmodule
