// page_dma.sv — page-in consumer (Regime 1, in-domain), §3.3 "D2" engine.
//
// Runs ENTIRELY in the sdram_clk domain. On `start`, kicks the sdram.v
// page-mode stream port (stream_req/stream_addr/stream_words) and, for every
// stream word that comes back (stream_valid), serializes it into the two 8-bit
// bytes of the real bank-1 `ext_rom` write port (pgd_erom_*).
//
// BYTE ORDER (load-bearing — matches the single-access dout decode
// a[0]?data[7:0]:data[15:8], so the paged ROM is NOT byte-swapped):
//   even byte = stream_dout[15:8] -> ext_rom[2k]
//   odd  byte = stream_dout[7:0]  -> ext_rom[2k+1]
//
// ---------------------------------------------------------------------------
// RATE-MATCH (the crux). The stream is 1 word/clk, free-running WITHIN a row,
// but the real `ext_rom` write port is 8-bit (one byte/clk). Serializing a word
// into two byte-writes is 2 clk/word, so page_dma cannot consume in lockstep
// with a 1 word/clk producer. It self-paces with `stream_ready` (added to
// sdram.v in Task 11a): drive it low when the skid buffer is filling, high when
// it can accept more.
//
// IN-FLIGHT REALITY: deasserting `stream_ready` stops NEW READ issuance, but the
// sdram CAS+capture pipeline (rd_inflight is 5 deep) plus the 1-cycle
// registration latency of stream_ready means up to ~6 in-flight READs still
// surface `stream_valid` over the following clocks AFTER ready goes low. So the
// skid buffer must reserve enough free slots, when ready drops, to swallow that
// whole trailing burst without dropping a word.
//
// SKID BUFFER: a small SYNCHRONOUS FIFO (sdram_clk, same domain as both the
// stream and the byte-writer — NOT a CDC FIFO). Depth 16, explicit FF array
// (ram_style=registers, deterministic POR reset — see fifo_mem). It captures
// every stream_valid word; the byte-writer pops it at 2 clk/word.
//   stream_ready = (free slots >= READY_MARGIN). With DEPTH=16 and
//   READY_MARGIN=8, ready drops once fewer than 8 slots remain free, leaving 8
//   free slots to absorb the worst-case ~6 trailing in-flight words + margin.
//   => can never overflow / never drop a word.
//
// THROUGHPUT: drain-bound ~= 2 clk/word * 8192 ~= 16384 clk ~= 164 us at
// ~100 MHz sdram_clk (Tier 2). The buffer just smooths the 1->0.5 word/clk rate
// step; it is small and entirely in one clock domain (no CDC).
//
// DONE: pulses for one clk only when stream_done has been observed AND the skid
// buffer is empty AND the byte-writer has finished the last word (no half-
// written word pending). pgd_active stays high for the whole copy and drops the
// cycle done pulses.
//
// words==0 GUARD: sdram.v's S_IDLE requires stream_words!=0, so a zero-word
// request would never produce stream_done and would wedge the FSM with
// pgd_active stuck high. Treat words==0 as immediate completion: pulse done,
// never raise pgd_active, never assert stream_req, stay idle.
//
// PORT NAMES match top.sv's ext_rom write mux (Task 9/11b): pgd_erom_we,
// pgd_erom_addr[13:0] (BYTE address), pgd_erom_data[7:0], pgd_active.

module page_dma (
    input  wire        clk,            // sdram_clk
    input  wire        rst,            // synchronous reset (tie to init/POR)
    input  wire        start,          // 1-clk pulse to begin a page-in
    input  wire [24:0] src_base,       // SDRAM start BYTE address
    input  wire [13:0] words,          // # 16-bit words to copy (<=16384)

    // sdram.v stream port
    output reg         stream_req,
    output reg  [24:0] stream_addr,
    output reg  [13:0] stream_words,
    output reg         stream_ready,   // self-pacing back-pressure to sdram.v
    input  wire [15:0] stream_dout,
    input  wire        stream_valid,
    input  wire        stream_busy,    // observed-only / reserved
    input  wire        stream_done,

    // ext_rom write port (dpram port A) — REAL 8-bit byte interface.
    output reg         pgd_erom_we,
    output reg  [13:0] pgd_erom_addr,  // BYTE address (2k even / 2k+1 odd)
    output reg  [7:0]  pgd_erom_data,  // byte lane (even=[15:8], odd=[7:0])

    output reg         pgd_active,     // high for the whole copy
    output reg         done            // 1-clk pulse at completion
);

    // ===================================================================
    // Synchronous skid buffer (sdram_clk) — depth 16, 16-bit words.
    // Explicit FF array (ram_style=registers), deterministic POR reset — small,
    // same-domain. Captures every stream_valid word; byte-writer pops at 2
    // clk/word. NOTE: without the ram_style attribute, synth_ecp5 maps this
    // small dual-port memory to TRELLIS_DPR16X4 distributed LUT-RAM (which has
    // no deterministic reset). Forcing registers keeps the stated intent.
    // ===================================================================
    localparam integer FIFO_AW    = 4;            // 16 entries
    localparam integer FIFO_DEPTH = (1 << FIFO_AW);
    // Drop stream_ready when fewer than READY_MARGIN free slots remain. 8 free
    // slots cover the worst-case ~6 in-flight READs (5-deep CAS pipe + 1-clk
    // ready registration latency) plus margin, so a deassert never overflows.
    // NOTE: READY_MARGIN must be >= sdram.v rd_inflight depth (5) + 1; if you
    // widen rd_inflight, bump this.
    localparam integer READY_MARGIN = 8;

    (* ram_style = "registers" *)
    reg [15:0] fifo_mem [0:FIFO_DEPTH-1];
    reg [FIFO_AW-1:0] fifo_wr;
    reg [FIFO_AW-1:0] fifo_rd;
    reg [FIFO_AW:0]   fifo_cnt;        // 0..FIFO_DEPTH (one extra bit)

    wire fifo_empty = (fifo_cnt == 0);
    wire fifo_full  = (fifo_cnt == FIFO_DEPTH[FIFO_AW:0]);
    // free slots = DEPTH - cnt; ready when free >= READY_MARGIN, i.e.
    // cnt <= DEPTH - READY_MARGIN.
    wire fifo_can_accept = (fifo_cnt <= (FIFO_DEPTH - READY_MARGIN));

    // ----- byte-writer (drains FIFO at 2 clk/word) -----
    // phase 0: write even byte (high lane) to BYTE addr 2k
    // phase 1: write odd  byte (low  lane) to BYTE addr 2k+1, then pop
    reg        bw_busy;        // a word is mid-serialization
    reg        bw_phase;       // 0 = even byte, 1 = odd byte
    reg [13:0] bw_byte_idx;    // running BYTE address (2k / 2k+1)
    reg [15:0] bw_word;        // the word being serialized

    // pop happens when we begin serializing a fresh word (read head consumed)
    wire bw_start_word = (!bw_busy) && (!fifo_empty);
    wire fifo_pop      = bw_start_word;
    wire fifo_push     = stream_valid && !fifo_full;  // (full never reached by design)

    // ----- main FSM -----
    localparam [1:0] S_IDLE = 2'd0;   // wait for start
    localparam [1:0] S_REQ  = 2'd1;   // assert stream_req for one clk
    localparam [1:0] S_RUN  = 2'd2;   // stream -> FIFO -> byte-writer
    reg [1:0]  state;
    reg        stream_done_seen;      // latched: the producer has finished

    always @(posedge clk) begin
        // default single-clk strobes
        stream_req  <= 1'b0;
        pgd_erom_we <= 1'b0;
        done        <= 1'b0;

        if (rst) begin
            state            <= S_IDLE;
            stream_addr      <= 25'd0;
            stream_words     <= 14'd0;
            stream_ready     <= 1'b0;
            pgd_erom_addr    <= 14'd0;
            pgd_erom_data    <= 8'd0;
            pgd_active       <= 1'b0;
            stream_done_seen <= 1'b0;
            fifo_wr          <= {FIFO_AW{1'b0}};
            fifo_rd          <= {FIFO_AW{1'b0}};
            fifo_cnt         <= {(FIFO_AW+1){1'b0}};
            bw_busy          <= 1'b0;
            bw_phase         <= 1'b0;
            bw_byte_idx      <= 14'd0;
            bw_word          <= 16'd0;
        end else begin
            // ---------------------------------------------------------
            // FIFO push (every stream_valid word) + pop (byte-writer start).
            // Both can happen on the same clk; net count adjust below.
            // ---------------------------------------------------------
            if (fifo_push) begin
                fifo_mem[fifo_wr] <= stream_dout;
                fifo_wr           <= fifo_wr + 1'b1;
            end
            if (fifo_pop) begin
                fifo_rd <= fifo_rd + 1'b1;
            end
            case ({fifo_push, fifo_pop})
                2'b10: fifo_cnt <= fifo_cnt + 1'b1;
                2'b01: fifo_cnt <= fifo_cnt - 1'b1;
                default: ; // both or neither -> unchanged
            endcase

            // ---------------------------------------------------------
            // Byte-writer: serialize one popped word into two 8-bit writes.
            // ---------------------------------------------------------
            if (bw_start_word) begin
                // load the word being popped this clk; emit its EVEN byte now.
                bw_busy       <= 1'b1;
                bw_phase      <= 1'b1;                 // next clk -> odd byte
                bw_word       <= fifo_mem[fifo_rd];
                pgd_erom_we   <= 1'b1;
                pgd_erom_addr <= bw_byte_idx;          // 2k
                pgd_erom_data <= fifo_mem[fifo_rd][15:8]; // even = high lane
                bw_byte_idx   <= bw_byte_idx + 14'd1;  // -> 2k+1
            end else if (bw_busy && bw_phase) begin
                // odd byte
                bw_busy       <= 1'b0;
                bw_phase      <= 1'b0;
                pgd_erom_we   <= 1'b1;
                pgd_erom_addr <= bw_byte_idx;          // 2k+1
                pgd_erom_data <= bw_word[7:0];         // odd = low lane
                bw_byte_idx   <= bw_byte_idx + 14'd1;  // -> next word's 2k
            end

            // ---------------------------------------------------------
            // Main FSM
            // ---------------------------------------------------------
            case (state)

            S_IDLE: begin
                pgd_active   <= 1'b0;
                stream_ready <= 1'b0;
                if (start) begin
                    if (words == 14'd0) begin
                        // immediate completion — never touch the stream/erom.
                        done <= 1'b1;
                    end else begin
                        stream_addr      <= src_base;
                        stream_words     <= words;
                        stream_done_seen <= 1'b0;
                        bw_byte_idx      <= 14'd0;
                        fifo_wr          <= {FIFO_AW{1'b0}};
                        fifo_rd          <= {FIFO_AW{1'b0}};
                        fifo_cnt         <= {(FIFO_AW+1){1'b0}};
                        bw_busy          <= 1'b0;
                        bw_phase         <= 1'b0;
                        pgd_active       <= 1'b1;   // owns ext_rom from the request on
                        stream_ready     <= 1'b1;   // ready to accept words
                        state            <= S_REQ;
                    end
                end
            end

            // Pulse stream_req for one clk; sdram.v latches it in its S_IDLE.
            S_REQ: begin
                stream_req <= 1'b1;
                state      <= S_RUN;
            end

            S_RUN: begin
                // Self-pace: accept words only while the FIFO has headroom for
                // the trailing in-flight burst.
                stream_ready <= fifo_can_accept;

                if (stream_done) stream_done_seen <= 1'b1;

                // Completion: producer finished AND the buffer is fully drained
                // AND no half-written word is pending. fifo_pop here means a word
                // is being started THIS clk (so not yet drained); guard against it.
                if ((stream_done_seen || stream_done) &&
                    fifo_empty && !bw_busy && !fifo_pop) begin
                    done       <= 1'b1;
                    pgd_active <= 1'b0;
                    stream_ready <= 1'b0;
                    state      <= S_IDLE;
                end
            end

            default: state <= S_IDLE;
            endcase
        end
    end

endmodule
