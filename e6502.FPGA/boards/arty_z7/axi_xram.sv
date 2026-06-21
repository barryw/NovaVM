// axi_xram.sv — XRAM-over-AXI bridge for the Arty Z7-20.
//
// Presents the SAME port-A / port-B / stream contract that rtl/top.sv expects
// from rtl/sdram/sdram.v, but backs it with an AXI4 master into PS DDR3 (via
// S_AXI_HP). Replaces the ULX3S SDR-SDRAM controller so NovaVM's XRAM /
// expansion memory (and the library page-ins that gate READY) work on the Arty.
//
// Contract (from sdram.v):
//   Port A/B: present byte addr + we/din (write) or oe (read); the bridge runs
//             the access and pulses doneX for one clk (doutX valid on reads).
//   Stream:   stream_req pulse + byte start addr + 16-bit word count -> read
//             burst, stream_dout/stream_valid per word (honoring stream_ready
//             back-pressure), stream_busy while active, stream_done 1-clk pulse.
//
// AXI: 32-bit data, single-beat transactions (byte writes via WSTRB; reads
// extract the addressed byte; the stream reads 32-bit beats and emits two
// 16-bit words each from a latched beat). Connect to PS S_AXI_HP via an AXI
// SmartConnect (32<->64 width). XRAM byte addr -> AXI addr = XRAM_BASE + addr.
// Assumes stream_addr is 2-byte aligned (NovaVM pages are 16 KB aligned).
//
// Written from the sdram.v contract; functional validation pends PS DDR
// bring-up (the PS must init DDR before XRAM works).

`default_nettype none

module axi_xram #(
    parameter [31:0] XRAM_BASE = 32'h1000_0000  // DDR byte offset for XRAM region
) (
    input  wire        clk,
    input  wire        rst,

    // ---- Port A (CPU/XMC) ---------------------------------------------------
    input  wire [24:0] addrA,
    input  wire        weA,
    input  wire [7:0]  dinA,
    input  wire        oeA,
    output reg  [7:0]  doutA,
    output reg         doneA,

    // ---- Port B (SID curve / WTS) ------------------------------------------
    input  wire [24:0] addrB,
    input  wire        weB,
    input  wire [7:0]  dinB,
    input  wire        oeB,
    output reg  [7:0]  doutB,
    output reg         doneB,

    // ---- Stream burst read port --------------------------------------------
    input  wire        stream_req,
    input  wire [24:0] stream_addr,
    input  wire [13:0] stream_words,
    input  wire        stream_ready,
    output reg  [15:0] stream_dout,
    output reg         stream_valid,
    output reg         stream_busy,
    output reg         stream_done,
    output wire [3:0]  dbg_state,
    output wire [13:0] dbg_sleft,

    // ---- AXI4 master (to PS S_AXI_HP via SmartConnect) ----------------------
    output reg  [31:0] m_axi_awaddr,
    output wire [7:0]  m_axi_awlen,
    output wire [2:0]  m_axi_awsize,
    output wire [1:0]  m_axi_awburst,
    output reg         m_axi_awvalid,
    input  wire        m_axi_awready,
    output reg  [31:0] m_axi_wdata,
    output reg  [3:0]  m_axi_wstrb,
    output wire        m_axi_wlast,
    output reg         m_axi_wvalid,
    input  wire        m_axi_wready,
    input  wire [1:0]  m_axi_bresp,
    input  wire        m_axi_bvalid,
    output reg         m_axi_bready,
    output reg  [31:0] m_axi_araddr,
    output wire [7:0]  m_axi_arlen,
    output wire [2:0]  m_axi_arsize,
    output wire [1:0]  m_axi_arburst,
    output reg         m_axi_arvalid,
    input  wire        m_axi_arready,
    input  wire [31:0] m_axi_rdata,
    input  wire [1:0]  m_axi_rresp,
    input  wire        m_axi_rlast,
    input  wire        m_axi_rvalid,
    output reg         m_axi_rready
);
    // Fixed AXI attributes: 32-bit (size=2) INCR, single beat.
    assign m_axi_awsize  = 3'd2;
    assign m_axi_arsize  = 3'd2;
    assign m_axi_awburst = 2'b01;
    assign m_axi_arburst = 2'b01;
    assign m_axi_awlen   = 8'd0;
    assign m_axi_arlen   = 8'd0;
    assign m_axi_wlast   = 1'b1;

    localparam [3:0]
        S_IDLE   = 4'd0,
        S_AW     = 4'd1,   // write addr+data
        S_B      = 4'd2,   // write response
        S_AR_AB  = 4'd3,   // A/B read: addr
        S_R_AB   = 4'd4,   // A/B read: data
        S_STR_AR = 4'd5,   // stream: addr
        S_STR_R  = 4'd6,   // stream: latch beat
        S_STR_EM = 4'd7,   // stream: emit two words from latched beat
        S_SDONE  = 4'd8;   // stream completion pulse

    reg [3:0]  state;
    assign dbg_state = state;
    assign dbg_sleft = stream_left;
    reg        cur_is_b;       // current A/B job target
    reg [1:0]  cur_byte;       // byte lane for A/B
    // oeA/weA are held HIGH (level) by xram_sdram for the whole SD_ACTIVE window.
    // Without a guard, axi_xram finishes the access, returns to S_IDLE while the
    // line is still high, and re-triggers a SPURIOUS second access — whose extra
    // doneA desyncs xram_sdram (the next read returns the previous byte). At high
    // DDR latency this manifests as an off-by-one in DMA/blitter XRAM reads.
    // Service each assertion once: set on accept, clear when the line drops.
    reg        oeA_done, weA_done, oeB_done, weB_done;
    reg [31:0] beat;           // latched stream read beat
    reg [13:0] stream_left;    // words remaining
    reg [31:0] stream_axi;     // current 4-aligned AXI read addr
    reg        stream_hi;      // which 16-bit half to emit next (0=lo,1=hi)

    function automatic [3:0] strb_for(input [1:0] lane);
        strb_for = 4'b0001 << lane;
    endfunction

    always_ff @(posedge clk) begin
        if (rst) begin
            state         <= S_IDLE;
            doneA <= 0; doneB <= 0; doutA <= 0; doutB <= 0;
            m_axi_awvalid <= 0; m_axi_wvalid <= 0; m_axi_bready <= 0;
            m_axi_arvalid <= 0; m_axi_rready <= 0;
            stream_valid  <= 0; stream_busy <= 0; stream_done <= 0;
            stream_left   <= 0; stream_hi <= 0;
            oeA_done <= 0; weA_done <= 0; oeB_done <= 0; weB_done <= 0;
        end else begin
            doneA <= 0; doneB <= 0; stream_valid <= 0; stream_done <= 0;

            // Clear the per-assertion guard once the request line returns low.
            if (!oeA) oeA_done <= 0;
            if (!weA) weA_done <= 0;
            if (!oeB) oeB_done <= 0;
            if (!weB) weB_done <= 0;

            case (state)
            // ----------------------------------------------------------------
            S_IDLE: begin
                if ((weA && !weA_done) || (oeA && !oeA_done)) begin
                    cur_is_b <= 0;
                    cur_byte <= addrA[1:0];
                    if (weA) begin
                        weA_done      <= 1;
                        m_axi_awaddr  <= XRAM_BASE + {7'd0, addrA[24:2], 2'b00};
                        m_axi_wdata   <= {4{dinA}};
                        m_axi_wstrb   <= strb_for(addrA[1:0]);
                        m_axi_awvalid <= 1; m_axi_wvalid <= 1;
                        state         <= S_AW;
                    end else begin
                        oeA_done      <= 1;
                        m_axi_araddr  <= XRAM_BASE + {7'd0, addrA[24:2], 2'b00};
                        m_axi_arvalid <= 1;
                        state         <= S_AR_AB;
                    end
                end else if ((weB && !weB_done) || (oeB && !oeB_done)) begin
                    cur_is_b <= 1;
                    cur_byte <= addrB[1:0];
                    if (weB) begin
                        weB_done <= 1;
                        m_axi_awaddr  <= XRAM_BASE + {7'd0, addrB[24:2], 2'b00};
                        m_axi_wdata   <= {4{dinB}};
                        m_axi_wstrb   <= strb_for(addrB[1:0]);
                        m_axi_awvalid <= 1; m_axi_wvalid <= 1;
                        state         <= S_AW;
                    end else begin
                        oeB_done      <= 1;
                        m_axi_araddr  <= XRAM_BASE + {7'd0, addrB[24:2], 2'b00};
                        m_axi_arvalid <= 1;
                        state         <= S_AR_AB;
                    end
                end else if (stream_req && !stream_busy) begin
                    // ARTY PAGE-IN BYPASS: the lib_call page-in stream is a NO-OP.
                    // The PS writes each module image directly into bank-1 (ext_rom)
                    // via the fio_bridge ROM-load port, so the page-in only needs to
                    // signal completion. This sidesteps the unreliable HP0 burst-read
                    // path (which flakily stalled in S_STR_R / never reached S_SDONE).
                    // No stream_valid words are emitted -> page_dma writes nothing to
                    // bank-1, preserving the PS-loaded image. (S_STR_* states below
                    // are now dead code, kept for reference / future real-DMA use.)
                    stream_busy <= 1;
                    state       <= S_SDONE;
                end
            end

            // ---- write ------------------------------------------------------
            S_AW: begin
                if (m_axi_awvalid && m_axi_awready) m_axi_awvalid <= 0;
                if (m_axi_wvalid  && m_axi_wready ) m_axi_wvalid  <= 0;
                if ((!m_axi_awvalid || m_axi_awready) && (!m_axi_wvalid || m_axi_wready)) begin
                    m_axi_bready <= 1;
                    state        <= S_B;
                end
            end
            S_B: if (m_axi_bvalid && m_axi_bready) begin
                m_axi_bready <= 0;
                if (cur_is_b) doneB <= 1; else doneA <= 1;
                state <= S_IDLE;
            end

            // ---- A/B read ---------------------------------------------------
            S_AR_AB: if (m_axi_arvalid && m_axi_arready) begin
                m_axi_arvalid <= 0; m_axi_rready <= 1;
                state <= S_R_AB;
            end
            S_R_AB: if (m_axi_rvalid && m_axi_rready) begin
                m_axi_rready <= 0;
                if (cur_is_b) begin doutB <= m_axi_rdata[8*cur_byte +: 8]; doneB <= 1; end
                else          begin doutA <= m_axi_rdata[8*cur_byte +: 8]; doneA <= 1; end
                state <= S_IDLE;
            end

            // ---- stream read ------------------------------------------------
            S_STR_AR: if (m_axi_arvalid && m_axi_arready) begin
                m_axi_arvalid <= 0; m_axi_rready <= 1;
                state <= S_STR_R;
            end
            S_STR_R: if (m_axi_rvalid && m_axi_rready) begin
                beat         <= m_axi_rdata;
                m_axi_rready <= 0;
                state        <= S_STR_EM;
            end
            S_STR_EM: begin
                if (stream_ready) begin
                    stream_dout  <= stream_hi ? beat[31:16] : beat[15:0];
                    stream_valid <= 1;
                    stream_left  <= stream_left - 14'd1;
                    if (stream_left == 14'd1) begin
                        state <= S_SDONE;            // last word
                    end else if (!stream_hi) begin
                        stream_hi <= 1;              // emit high half of this beat next
                    end else begin
                        stream_hi  <= 0;             // beat exhausted -> next beat
                        stream_axi <= stream_axi + 32'd4;
                        m_axi_araddr  <= stream_axi + 32'd4;
                        m_axi_arvalid <= 1;
                        state         <= S_STR_AR;
                    end
                end
            end
            S_SDONE: begin
                stream_busy <= 0;
                stream_done <= 1;
                state       <= S_IDLE;
            end

            default: state <= S_IDLE;
            endcase
        end
    end

endmodule

`default_nettype wire
