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
// The ext_rom write port is muxed in top.sv (boot bridge dbg_rom_* vs page_dma
// pgd_*); pgd_active selects page_dma. They never overlap (a page-in only runs
// at a runtime switch, with the boot bridge idle).

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
    input  wire        stream_busy,
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
                    stream_addr  <= src_base;
                    stream_words <= words;
                    word_idx     <= 14'd0;
                    pgd_active   <= 1'b1;   // owns ext_rom from the request on
                    state        <= S_REQ;
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
