// hdmi_capture.sv — post-OSD HDMI/audio capture writer for Arty Z7.
//
// Samples the exact RGB stream sent to the HDMI encoder and the exact final
// mixed PCM samples, then writes them into reserved PS DDR through an AXI4
// master. The scanout path is never stalled; FIFO overflow is counted.

`default_nettype none

module hdmi_capture #(
    parameter [31:0] VIDEO_BASE = 32'h1100_0000,
    parameter [31:0] AUDIO_BASE = 32'h1180_0000,
    parameter int    FRAME_PIXELS = 720 * 480,
    parameter [31:0] FRAME_STRIDE = 32'h0020_0000,
    parameter int    FRAME_COUNT = 4,
    parameter [31:0] AUDIO_BYTES = 32'h0010_0000
) (
    input  wire        clk,
    input  wire        rst,
    input  wire        enable,

    input  wire [23:0] rgb,
    input  wire        active,
    input  wire        audio_sample_strobe,
    input  wire [1:0][15:0] audio_sample_word,

    output reg  [31:0] frame_seq,
    output reg  [31:0] audio_seq,
    output reg  [31:0] video_drops,
    output reg  [31:0] audio_drops,
    output reg  [31:0] last_frame_index,
    output wire [31:0] audio_write_offset,

    output reg  [31:0] m_axi_awaddr,
    output reg  [7:0]  m_axi_awlen,
    output wire [2:0]  m_axi_awsize,
    output wire [1:0]  m_axi_awburst,
    output reg         m_axi_awvalid,
    input  wire        m_axi_awready,
    output wire [31:0] m_axi_wdata,
    output reg  [3:0]  m_axi_wstrb,
    output reg         m_axi_wlast,
    output reg         m_axi_wvalid,
    input  wire        m_axi_wready,
    input  wire [1:0]  m_axi_bresp,
    input  wire        m_axi_bvalid,
    output reg         m_axi_bready
);
    localparam int FRAME_INDEX_BITS = $clog2(FRAME_COUNT);
    localparam logic [FRAME_INDEX_BITS-1:0] LAST_FRAME_INDEX = FRAME_INDEX_BITS'(FRAME_COUNT - 1);

    localparam [6:0] VIDEO_BURST_BEATS = 7'd64;

    localparam [1:0]
        S_IDLE = 2'd0,
        S_AW   = 2'd1,
        S_W    = 2'd2;

    assign m_axi_awsize  = 3'd2;
    assign m_axi_awburst = 2'b01;

    reg [1:0]  state;
    reg [31:0] pixel_index;
    reg [FRAME_INDEX_BITS-1:0] frame_index;
    reg [31:0] audio_offset;
    reg [6:0]  burst_remaining;

    wire        video_full, video_empty;
    wire [10:0] video_fill;
    wire [63:0] video_word;
    wire        video_rd;
    wire [63:0] video_stream_word;
    wire [6:0]  video_stream_count;
    wire        video_stream_empty;

    wire        audio_full, audio_empty;
    wire [8:0]  audio_fill;
    wire [63:0] audio_word;
    wire        audio_rd;
    wire [63:0] audio_stream_word;
    wire [2:0]  audio_stream_count;
    wire        audio_stream_empty;
    reg         load_video;

    wire [31:0] video_addr = VIDEO_BASE + (FRAME_STRIDE * frame_index) + (pixel_index << 2);
    wire [31:0] audio_addr = AUDIO_BASE + audio_offset;
    wire        video_wr = enable && active && !video_full;
    wire        audio_wr = enable && audio_sample_strobe && !audio_full;
    wire        video_stream_full_burst = (video_stream_count >= VIDEO_BURST_BEATS);
    wire [6:0]  video_burst_count =
        video_stream_full_burst ? VIDEO_BURST_BEATS : video_stream_count;
    wire        video_burst_ready = video_stream_full_burst ||
                                    (!active && !video_stream_empty);
    wire        axi_w_fire = m_axi_wvalid && m_axi_wready;
    wire        video_stream_pop = axi_w_fire && load_video;
    wire        audio_stream_pop = axi_w_fire && !load_video;
    assign m_axi_wdata = load_video ? video_stream_word[31:0] : audio_stream_word[31:0];
    assign audio_write_offset = audio_offset;

    capture_bram_fifo #(.WIDTH(64), .LG_DEPTH(10)) video_fifo (
        .clk(clk), .rst(rst),
        .wr(video_wr), .wr_data({video_addr, 8'h00, rgb}),
        .full(video_full), .fill(video_fill),
        .rd(video_rd), .rd_data(video_word), .empty(video_empty)
    );

    capture_prefetch_queue #(.WIDTH(64), .LG_DEPTH(6)) video_stream (
        .clk(clk), .rst(rst),
        .fifo_empty(video_empty), .fifo_word(video_word), .fifo_rd(video_rd),
        .pop(video_stream_pop),
        .out_word(video_stream_word), .count(video_stream_count), .empty(video_stream_empty)
    );

    capture_bram_fifo #(.WIDTH(64), .LG_DEPTH(8)) audio_fifo (
        .clk(clk), .rst(rst),
        .wr(audio_wr), .wr_data({audio_addr, audio_sample_word[1], audio_sample_word[0]}),
        .full(audio_full), .fill(audio_fill),
        .rd(audio_rd), .rd_data(audio_word), .empty(audio_empty)
    );

    capture_prefetch_queue #(.WIDTH(64), .LG_DEPTH(2)) audio_stream (
        .clk(clk), .rst(rst),
        .fifo_empty(audio_empty), .fifo_word(audio_word), .fifo_rd(audio_rd),
        .pop(audio_stream_pop),
        .out_word(audio_stream_word), .count(audio_stream_count), .empty(audio_stream_empty)
    );

    always_ff @(posedge clk) begin
        if (rst) begin
            frame_seq <= 32'd0;
            audio_seq <= 32'd0;
            video_drops <= 32'd0;
            audio_drops <= 32'd0;
            last_frame_index <= 32'd0;
            pixel_index <= 32'd0;
            frame_index <= '0;
            audio_offset <= 32'd0;
        end else begin
            if (enable && active) begin
                if (video_full)
                    video_drops <= video_drops + 32'd1;

                if (pixel_index == FRAME_PIXELS - 1) begin
                    pixel_index <= 32'd0;
                    frame_seq <= frame_seq + 32'd1;
                    last_frame_index <= {{(32-FRAME_INDEX_BITS){1'b0}}, frame_index};
                    frame_index <= (frame_index == LAST_FRAME_INDEX) ? '0 : frame_index + 1'b1;
                end else begin
                    pixel_index <= pixel_index + 32'd1;
                end
            end

            if (enable && audio_sample_strobe) begin
                if (audio_full)
                    audio_drops <= audio_drops + 32'd1;
                audio_seq <= audio_seq + 32'd1;
                audio_offset <= (audio_offset >= AUDIO_BYTES - 4) ? 32'd0 : audio_offset + 32'd4;
            end

            if (!enable) begin
                pixel_index <= 32'd0;
                frame_index <= '0;
                audio_offset <= 32'd0;
            end
        end
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            state <= S_IDLE;
            m_axi_awaddr <= 32'd0;
            m_axi_awvalid <= 1'b0;
            m_axi_wstrb <= 4'h0;
            m_axi_awlen <= 8'd0;
            m_axi_wlast <= 1'b0;
            m_axi_wvalid <= 1'b0;
            m_axi_bready <= 1'b0;
            load_video <= 1'b0;
            burst_remaining <= 7'd0;
        end else begin
            m_axi_bready <= 1'b1;
            case (state)
            S_IDLE: begin
                if (video_burst_ready) begin
                    load_video      <= 1'b1;
                    burst_remaining <= video_burst_count;
                    m_axi_awaddr    <= video_stream_word[63:32];
                    m_axi_awlen     <= {1'd0, video_burst_count} - 8'd1;
                    m_axi_awvalid   <= 1'b1;
                    m_axi_wstrb     <= 4'hF;
                    state           <= S_AW;
                end else if (!audio_stream_empty) begin
                    load_video <= 1'b0;
                    burst_remaining <= 7'd1;
                    m_axi_awaddr <= audio_stream_word[63:32];
                    m_axi_awlen <= 8'd0;
                    m_axi_awvalid <= 1'b1;
                    m_axi_wstrb <= 4'hF;
                    state <= S_AW;
                end
            end

            S_AW: begin
                if (m_axi_awvalid && m_axi_awready) begin
                    m_axi_awvalid <= 1'b0;
                    m_axi_wvalid  <= 1'b1;
                    m_axi_wlast   <= (burst_remaining == 7'd1);
                    state         <= S_W;
                end
            end

            S_W: begin
                if (axi_w_fire) begin
                    if (burst_remaining == 7'd1) begin
                        m_axi_wvalid <= 1'b0;
                        m_axi_wlast <= 1'b0;
                        state <= S_IDLE;
                    end else begin
                        burst_remaining <= burst_remaining - 7'd1;
                        m_axi_wlast <= (burst_remaining == 7'd2);
                    end
                end
            end

            default: state <= S_IDLE;
            endcase
        end
    end

    wire unused = &{1'b0, m_axi_bresp, m_axi_bvalid, video_fill, audio_fill, audio_stream_count};

endmodule

module capture_prefetch_queue #(
    parameter int WIDTH = 64,
    parameter int LG_DEPTH = 5
) (
    input  wire                 clk,
    input  wire                 rst,
    input  wire                 fifo_empty,
    input  wire [WIDTH-1:0]     fifo_word,
    output wire                 fifo_rd,
    input  wire                 pop,
    output wire [WIDTH-1:0]     out_word,
    output reg  [LG_DEPTH:0]    count,
    output wire                 empty
);
    localparam int DEPTH = 1 << LG_DEPTH;

    reg [WIDTH-1:0] queue [0:DEPTH-1];
    reg [LG_DEPTH-1:0] rd_ptr;
    reg [LG_DEPTH-1:0] wr_ptr;
    reg fetch_pending;

    assign empty = (count == { (LG_DEPTH + 1) {1'b0} });
    assign out_word = queue[rd_ptr];

    wire do_pop = pop && !empty;
    wire [LG_DEPTH+1:0] reserved_after_pop =
        {1'b0, count} + {{(LG_DEPTH+1){1'b0}}, fetch_pending} -
        {{(LG_DEPTH+1){1'b0}}, do_pop};
    assign fifo_rd = !rst && !fifo_empty && (reserved_after_pop < (LG_DEPTH + 2)'(DEPTH));

    always_ff @(posedge clk) begin
        if (rst) begin
            count <= '0;
            rd_ptr <= '0;
            wr_ptr <= '0;
            fetch_pending <= 1'b0;
        end else begin
            fetch_pending <= fifo_rd;

            if (fetch_pending) begin
                queue[wr_ptr] <= fifo_word;
                wr_ptr <= wr_ptr + 1'b1;
            end

            if (do_pop)
                rd_ptr <= rd_ptr + 1'b1;

            case ({fetch_pending, do_pop})
            2'b10: count <= count + 1'b1;
            2'b01: count <= count - 1'b1;
            default: begin end
            endcase
        end
    end
endmodule

module capture_bram_fifo #(
    parameter int WIDTH = 64,
    parameter int LG_DEPTH = 10
) (
    input  wire                 clk,
    input  wire                 rst,
    input  wire                 wr,
    input  wire [WIDTH-1:0]     wr_data,
    output wire                 full,
    output reg  [LG_DEPTH:0]    fill,
    input  wire                 rd,
    output reg  [WIDTH-1:0]     rd_data,
    output wire                 empty
);
    localparam int DEPTH = 1 << LG_DEPTH;

    (* ram_style = "block" *) reg [WIDTH-1:0] mem [0:DEPTH-1];
    reg [LG_DEPTH-1:0] wr_ptr;
    reg [LG_DEPTH-1:0] rd_ptr;

    assign empty = (fill == { (LG_DEPTH + 1) {1'b0} });
    assign full  = fill[LG_DEPTH];

    wire do_wr = wr && !full;
    wire do_rd = rd && !empty;

    always_ff @(posedge clk) begin
        if (rst) begin
            fill <= '0;
            wr_ptr <= '0;
            rd_ptr <= '0;
            rd_data <= '0;
        end else begin
            if (do_wr) begin
                mem[wr_ptr] <= wr_data;
                wr_ptr <= wr_ptr + 1'b1;
            end

            if (do_rd) begin
                rd_data <= mem[rd_ptr];
                rd_ptr <= rd_ptr + 1'b1;
            end

            case ({do_wr, do_rd})
            2'b10: fill <= fill + 1'b1;
            2'b01: fill <= fill - 1'b1;
            default: begin end
            endcase
        end
    end
endmodule

`default_nettype wire
