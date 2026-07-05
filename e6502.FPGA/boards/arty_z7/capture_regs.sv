// capture_regs.sv — AXI4-Lite control/status for the NovaVM HDMI capture path.

`default_nettype none

module capture_regs #(
    parameter [31:0] VIDEO_BASE   = 32'h1100_0000,
    parameter [31:0] AUDIO_BASE   = 32'h1180_0000,
    parameter [31:0] FRAME_STRIDE = 32'h0020_0000,
    parameter [31:0] FRAME_COUNT  = 32'd4,
    parameter [31:0] WIDTH        = 32'd720,
    parameter [31:0] HEIGHT       = 32'd480,
    parameter [31:0] AUDIO_BYTES  = 32'h0010_0000
) (
    input  wire        aclk,
    input  wire        aresetn,

    input  wire [31:0] s_awaddr,
    input  wire        s_awvalid,
    output reg         s_awready,
    input  wire [31:0] s_wdata,
    input  wire [3:0]  s_wstrb,
    input  wire        s_wvalid,
    output reg         s_wready,
    output reg  [1:0]  s_bresp,
    output reg         s_bvalid,
    input  wire        s_bready,
    input  wire [31:0] s_araddr,
    input  wire        s_arvalid,
    output reg         s_arready,
    output reg  [31:0] s_rdata,
    output reg  [1:0]  s_rresp,
    output reg         s_rvalid,
    input  wire        s_rready,

    output reg         capture_enable,
    input  wire [31:0] frame_seq,
    input  wire [31:0] audio_seq,
    input  wire [31:0] video_drops,
    input  wire [31:0] audio_drops,
    input  wire [31:0] last_frame_index,
    input  wire [31:0] audio_write_offset
);
    localparam [31:0] MAGIC = 32'h4E43_4150; // "NCAP"

    always_ff @(posedge aclk) begin
        if (!aresetn) begin
            s_awready <= 1'b0; s_wready <= 1'b0; s_bvalid <= 1'b0; s_bresp <= 2'b00;
            s_arready <= 1'b0; s_rvalid <= 1'b0; s_rdata <= 32'd0; s_rresp <= 2'b00;
            capture_enable <= 1'b1;
        end else begin
            if (s_awvalid && s_wvalid && !s_awready && !s_bvalid) begin
                s_awready <= 1'b1;
                s_wready <= 1'b1;
                if (s_awaddr[7:2] == 6'h01)
                    capture_enable <= s_wdata[0];
                s_bresp <= 2'b00;
                s_bvalid <= 1'b1;
            end else begin
                s_awready <= 1'b0;
                s_wready <= 1'b0;
            end
            if (s_bvalid && s_bready) s_bvalid <= 1'b0;

            if (s_arvalid && !s_arready && !s_rvalid) begin
                s_arready <= 1'b1;
                case (s_araddr[7:2])
                6'h00: s_rdata <= MAGIC;
                6'h01: s_rdata <= {31'd0, capture_enable};
                6'h02: s_rdata <= frame_seq;
                6'h03: s_rdata <= audio_seq;
                6'h04: s_rdata <= video_drops;
                6'h05: s_rdata <= audio_drops;
                6'h06: s_rdata <= last_frame_index;
                6'h07: s_rdata <= WIDTH;
                6'h08: s_rdata <= HEIGHT;
                6'h09: s_rdata <= FRAME_STRIDE;
                6'h0A: s_rdata <= FRAME_COUNT;
                6'h0B: s_rdata <= VIDEO_BASE;
                6'h0C: s_rdata <= AUDIO_BASE;
                6'h0D: s_rdata <= AUDIO_BYTES;
                6'h0E: s_rdata <= audio_write_offset;
                default: s_rdata <= 32'hDEAD_0000;
                endcase
                s_rresp <= 2'b00;
                s_rvalid <= 1'b1;
            end else begin
                s_arready <= 1'b0;
            end
            if (s_rvalid && s_rready) s_rvalid <= 1'b0;
        end
    end
endmodule

module axi_lite_demux2 (
    input  wire        aclk,
    input  wire        aresetn,

    input  wire [31:0] s_awaddr,
    input  wire        s_awvalid,
    output wire        s_awready,
    input  wire [31:0] s_wdata,
    input  wire [3:0]  s_wstrb,
    input  wire        s_wvalid,
    output wire        s_wready,
    output wire [1:0]  s_bresp,
    output wire        s_bvalid,
    input  wire        s_bready,
    input  wire [31:0] s_araddr,
    input  wire        s_arvalid,
    output wire        s_arready,
    output wire [31:0] s_rdata,
    output wire [1:0]  s_rresp,
    output wire        s_rvalid,
    input  wire        s_rready,

    output wire [31:0] m0_awaddr,
    output wire        m0_awvalid,
    input  wire        m0_awready,
    output wire [31:0] m0_wdata,
    output wire [3:0]  m0_wstrb,
    output wire        m0_wvalid,
    input  wire        m0_wready,
    input  wire [1:0]  m0_bresp,
    input  wire        m0_bvalid,
    output wire        m0_bready,
    output wire [31:0] m0_araddr,
    output wire        m0_arvalid,
    input  wire        m0_arready,
    input  wire [31:0] m0_rdata,
    input  wire [1:0]  m0_rresp,
    input  wire        m0_rvalid,
    output wire        m0_rready,

    output wire [31:0] m1_awaddr,
    output wire        m1_awvalid,
    input  wire        m1_awready,
    output wire [31:0] m1_wdata,
    output wire [3:0]  m1_wstrb,
    output wire        m1_wvalid,
    input  wire        m1_wready,
    input  wire [1:0]  m1_bresp,
    input  wire        m1_bvalid,
    output wire        m1_bready,
    output wire [31:0] m1_araddr,
    output wire        m1_arvalid,
    input  wire        m1_arready,
    input  wire [31:0] m1_rdata,
    input  wire [1:0]  m1_rresp,
    input  wire        m1_rvalid,
    output wire        m1_rready
);
    wire write_sel = s_awaddr[16];
    wire read_sel  = s_araddr[16];

    assign m0_awaddr  = s_awaddr;
    assign m0_awvalid = s_awvalid && !write_sel;
    assign m0_wdata   = s_wdata;
    assign m0_wstrb   = s_wstrb;
    assign m0_wvalid  = s_wvalid && !write_sel;
    assign m0_bready  = s_bready && !write_sel;
    assign m0_araddr  = s_araddr;
    assign m0_arvalid = s_arvalid && !read_sel;
    assign m0_rready  = s_rready && !read_sel;

    assign m1_awaddr  = s_awaddr;
    assign m1_awvalid = s_awvalid && write_sel;
    assign m1_wdata   = s_wdata;
    assign m1_wstrb   = s_wstrb;
    assign m1_wvalid  = s_wvalid && write_sel;
    assign m1_bready  = s_bready && write_sel;
    assign m1_araddr  = s_araddr;
    assign m1_arvalid = s_arvalid && read_sel;
    assign m1_rready  = s_rready && read_sel;

    assign s_awready = write_sel ? m1_awready : m0_awready;
    assign s_wready  = write_sel ? m1_wready  : m0_wready;
    assign s_bresp   = write_sel ? m1_bresp   : m0_bresp;
    assign s_bvalid  = write_sel ? m1_bvalid  : m0_bvalid;
    assign s_arready = read_sel ? m1_arready : m0_arready;
    assign s_rdata   = read_sel ? m1_rdata   : m0_rdata;
    assign s_rresp   = read_sel ? m1_rresp   : m0_rresp;
    assign s_rvalid  = read_sel ? m1_rvalid  : m0_rvalid;

    wire unused = &{1'b0, aclk, aresetn};
endmodule

`default_nettype wire
