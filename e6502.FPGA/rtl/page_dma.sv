// page_dma.sv — page-in consumer (Regime 1, in-domain), §3.3 "D2" engine.
//
// Runs in the sdram_clk domain. On `start`, kicks the sdram.v page-mode stream
// port (stream_req/stream_addr/stream_words) and, for every stream word that
// comes back (stream_valid, 1 word/clk, free-running, no back-pressure), writes
// the word's two bytes into bank-1 `ext_rom` BRAM.
//
// BYTE ORDER (load-bearing — matches the single-access dout decode
// a[0]?data[7:0]:data[15:8], so the paged ROM is NOT byte-swapped):
//   even byte = stream_dout[15:8] -> ext_rom[2k]
//   odd  byte = stream_dout[7:0]  -> ext_rom[2k+1]
//
// WORD -> 2-BYTE RATE.  The stream gives one 16-bit word per clk with no
// back-pressure; the real `ext_rom` is 8-bit (one byte/clk), so the 16->8-bit
// serialization cannot happen on a single 8-bit port at this rate. Task 11's
// async FIFO + slower pixel-domain drain formalizes that split. THIS module
// therefore presents the byte-pair as a single 16-bit write per stream word
// (erom_we, erom_addr = WORD index k, erom_data = {even, odd}); the Task-11
// FIFO/pixel-drain (or, in this unit test, a 16-bit-wide ext_rom stand-in)
// consumes one word/clk and serializes the two 8-bit ext_rom writes. erom_addr
// is the word index k = 0..(words-1); the real ext_rom byte addresses are
// 2k / 2k+1.
//
// CURRENT REALITY (this Task): page_dma is sdram_clk-ONLY and emits word-wide
// writes — 16-bit erom_data at word-index erom_addr. There is NO CDC FIFO and
// NO pixel-domain byte drain in this module yet. The async sdram_clk->pixel
// FIFO and the pixel-domain 8-bit byte drain (the pgd_erom_* path) are added in
// Task 11. In top.sv the ext_rom write port is muxed (boot bridge dbg_rom_* vs
// page_dma pgd_*) with pgd_active as the select, but as of this Task the pgd_*
// wires there are placeholder tie-offs (pgd_active=0); Task 11 connects the
// real page_dma outputs through the FIFO. A page-in and the boot bridge never
// overlap (a page-in only runs at a runtime switch, with the boot bridge idle).

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
    input  wire [15:0] stream_dout,
    input  wire        stream_valid,
    input  wire        stream_busy,    // observed-only / reserved: the FSM keys off
                                       // stream_done, not busy. Kept for Task 11.
    input  wire        stream_done,

    // ext_rom write port (dpram port A). Word-wide pair per stream word; see
    // header for the 16->8-bit serialization deferred to Task 11.
    output reg         erom_we,
    output reg  [13:0] erom_addr,      // WORD index k (0..words-1)
    output reg  [15:0] erom_data,      // {even byte [15:8], odd byte [7:0]}

    output reg         pgd_active,     // high for the whole copy
    output reg         done            // 1-clk pulse at completion
);

    // ----- FSM states -----
    localparam [1:0] S_IDLE = 2'd0;   // wait for start
    localparam [1:0] S_REQ  = 2'd1;   // assert stream_req for one clk
    localparam [1:0] S_RUN  = 2'd2;   // capture stream words -> ext_rom writes

    reg [1:0]  state;
    reg [13:0] word_idx;   // index of the next stream word to store

    always @(posedge clk) begin
        // default single-clk strobes
        stream_req <= 1'b0;
        erom_we    <= 1'b0;
        done       <= 1'b0;

        if (rst) begin
            state        <= S_IDLE;
            word_idx     <= 14'd0;
            stream_addr  <= 25'd0;
            stream_words <= 14'd0;
            erom_addr    <= 14'd0;
            erom_data    <= 16'd0;
            pgd_active   <= 1'b0;
        end else begin
            case (state)

            S_IDLE: begin
                pgd_active <= 1'b0;
                if (start) begin
                    // words==0 must NOT issue a stream request: sdram.v's
                    // S_IDLE requires stream_words!=0 and would ignore it, so
                    // stream_done would never fire and the FSM would wedge in
                    // S_RUN with pgd_active stuck high forever (permanent CPU
                    // stall once rdy_out ties to pgd_active). Treat it as an
                    // immediate completion: pulse `done`, never raise
                    // pgd_active, never assert stream_req, stay idle.
                    if (words == 14'd0) begin
                        done <= 1'b1;
                        // pgd_active already defaulted low above; stay in S_IDLE.
                    end else begin
                        stream_addr  <= src_base;
                        stream_words <= words;
                        word_idx     <= 14'd0;
                        pgd_active   <= 1'b1;   // owns ext_rom from the request on
                        state        <= S_REQ;
                    end
                end
            end

            // Pulse stream_req for one clk; sdram.v latches it in its S_IDLE.
            S_REQ: begin
                stream_req <= 1'b1;
                state      <= S_RUN;
            end

            // For every stream word, write the byte-pair into ext_rom at the
            // running word index. The stream is 1 word/clk, free-running; one
            // 16-bit word write per valid keeps up exactly. stream_done ends it.
            S_RUN: begin
                if (stream_valid) begin
                    erom_we   <= 1'b1;
                    erom_addr <= word_idx;
                    // even byte -> high lane, odd byte -> low lane (byte order)
                    erom_data <= {stream_dout[15:8], stream_dout[7:0]};
                    word_idx  <= word_idx + 14'd1;
                end
                if (stream_done) begin
                    done       <= 1'b1;
                    pgd_active <= 1'b0;
                    state      <= S_IDLE;
                end
            end

            default: state <= S_IDLE;
            endcase
        end
    end

endmodule
