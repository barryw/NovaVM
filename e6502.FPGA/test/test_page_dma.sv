// test_page_dma.sv — page-in consumer (Regime 1, in-domain) byte-exact proof.
//
// Wires the REAL sdram.v controller to the validated sdram_model.svh page-mode
// timing oracle (same harness as test_sdram_stream), then attaches the
// `page_dma` consumer to the stream port. page_dma reads the page-mode stream
// (stream_valid/stream_dout, 1 word/clk, free-running) and writes the bytes
// into a dpram standing in for bank-1 `ext_rom`.
//
// ---------------------------------------------------------------------------
// THE WORD -> 2-BYTE RATE QUESTION (the crux this test resolves).
//
// The sdram.v stream emits ONE 16-bit word per clk while a row is open
// (S_READ issues 256 back-to-back READs -> stream_valid pulses on 256
// consecutive clocks). There is NO back-pressure on the stream. The real
// `ext_rom` is an 8-bit byte-addressed dpram (one write/clk), so a single
// 8-bit write port physically cannot absorb 2 bytes/word at 1 word/clk during
// the dense in-row run. That 16-bit -> 8-bit serialization is exactly what
// Task 11's async FIFO + slower pixel-domain drain formalizes.
//
// For THIS single-clock unit test we model the ext_rom stand-in as a 16-bit-
// wide dpram: page_dma writes one 16-bit word per stream_valid at WORD address
// k, packing {even_byte, odd_byte} = {stream_dout[15:8], stream_dout[7:0]}.
// This matches the stream rate exactly (1 write/clk, no fictional back-
// pressure) and carries the byte order explicitly. The readback below
// decomposes each stored 16-bit word into the real 8-bit ext_rom byte image:
//   ext_rom[2k]   = stored_word[15:8]  (even byte, from stream_dout[15:8])
//   ext_rom[2k+1] = stored_word[7:0]   (odd  byte, from stream_dout[7:0])
// so the byte-order assertion is on the real per-byte ext_rom image, not the
// packed word. page_dma's erom_addr is the WORD index k (0..8191), faithful to
// a 16384-byte / 8192-word ext_rom; Task 11 splits each word into the two
// 8-bit pixel-domain writes via the FIFO.
//
// ---------------------------------------------------------------------------
// Pattern (UNIQUE per word so a wrong-row read is caught, mirrors
// test_sdram_stream case (d)):
//   mem[byte 2i]   = i[7:0]   -> HIGH lane [15:8]   (even byte)
//   mem[byte 2i+1] = i[15:8]  -> LOW  lane [7:0]    (odd  byte)
//   => captured word(i) == {i[7:0], i[15:8]}
//   => expected ext_rom[2i] = i[7:0], ext_rom[2i+1] = i[15:8]
//
// Trigger: start, src_base=0, words=8192 (16 KB / 32 physical rows). After
// page_dma `done`, read all 16384 bytes from the stand-in dpram (port B,
// 1-cycle latency honored) and assert byte-exact + correct byte order.
//
// Run: make -C test test_page_dma

`timescale 1ns/1ps

`include "sdram_model.svh"

module test_page_dma;

    // -----------------------------------------------------------------
    // Clocking — identical to test_sdram_stream: clk ~85 MHz, clkref
    // toggled every 8 clk edges (16:1).
    // -----------------------------------------------------------------
    logic clk = 0;
    always #5.88 clk = ~clk;

    logic [3:0] clkref_div = 0;
    logic       clkref = 0;
    always @(posedge clk) begin
        clkref_div <= clkref_div + 1'b1;
        if (clkref_div == 4'd7) begin
            clkref     <= ~clkref;
            clkref_div <= 0;
        end
    end

    // -----------------------------------------------------------------
    // Controller <-> chip wiring (we_out-muxed bus, same as the proven
    // test_sdram_stream harness).
    // -----------------------------------------------------------------
    wire [15:0] sd_data_ctrl_out;   // controller -> chip (write data)
    wire [15:0] sd_data_chip_out;   // chip -> controller (read data)
    wire [12:0] sd_addr;
    wire [1:0]  sd_dqm;
    wire [1:0]  sd_ba;
    wire        sd_cs, sd_we, sd_ras, sd_cas;
    wire        we_out;

    wire [15:0] sd_data_bus = we_out ? sd_data_ctrl_out : sd_data_chip_out;

    logic        init = 1'b1;
    logic [24:0] addrA = 0, addrB = 0;
    logic        weA = 0, weB = 0;
    logic        oeA = 0, oeB = 0;
    logic [7:0]  dinA = 0, dinB = 0;
    wire  [7:0]  doutA, doutB;
    wire         doneA, doneB;

    // Stream port — driven by page_dma, observed here.
    wire         stream_req;
    wire [24:0]  stream_addr;
    wire [13:0]  stream_words;
    wire  [15:0] stream_dout;
    wire         stream_valid;
    wire         stream_busy;
    wire         stream_done;

    sdram dut (
        .sd_data_in (sd_data_bus),
        .sd_data_out(sd_data_ctrl_out),
        .sd_addr    (sd_addr),
        .sd_dqm     (sd_dqm),
        .sd_ba      (sd_ba),
        .sd_cs      (sd_cs),
        .sd_we      (sd_we),
        .sd_ras     (sd_ras),
        .sd_cas     (sd_cas),

        .init   (init),
        .clk    (clk),
        .clkref (clkref),
        .we_out (we_out),

        .addrA(addrA), .weA(weA), .dinA(dinA), .oeA(oeA), .doutA(doutA), .doneA(doneA),
        .addrB(addrB), .weB(weB), .dinB(dinB), .oeB(oeB), .doutB(doutB), .doneB(doneB),

        .stream_req  (stream_req),
        .stream_addr (stream_addr),
        .stream_words(stream_words),
        .stream_dout (stream_dout),
        .stream_valid(stream_valid),
        .stream_busy (stream_busy),
        .stream_done (stream_done)
    );

    sdram_model chip (
        .clk         (clk),
        .sd_data_out (sd_data_ctrl_out),   // controller -> chip
        .sd_data_in  (sd_data_chip_out),   // chip -> controller (read data)
        .sd_addr     (sd_addr),
        .sd_dqm      (sd_dqm),
        .sd_ba       (sd_ba),
        .sd_cs       (sd_cs),
        .sd_we       (sd_we),
        .sd_ras      (sd_ras),
        .sd_cas      (sd_cas)
    );

    // -----------------------------------------------------------------
    // page_dma DUT + the ext_rom stand-in dpram.
    //
    // page_dma drives the stream port and emits one 16-bit word per
    // stream_valid (erom_we) at WORD address erom_addr=k carrying
    // erom_data={even,odd}={dout[15:8],dout[7:0]}. The stand-in dpram is
    // 16-bit-wide / 8192-deep (one word per ext_rom byte-pair).
    // -----------------------------------------------------------------
    logic        start = 1'b0;
    logic [24:0] src_base = 25'd0;
    logic [13:0] words = 14'd0;

    wire         erom_we;
    wire [13:0]  erom_addr;   // WORD index k (0..8191)
    wire [15:0]  erom_data;   // {even byte, odd byte}
    wire         pgd_active;
    wire         done;

    page_dma pgd (
        .clk         (clk),
        .rst         (init),
        .start       (start),
        .src_base    (src_base),
        .words       (words),
        .stream_req  (stream_req),
        .stream_addr (stream_addr),
        .stream_words(stream_words),
        .stream_dout (stream_dout),
        .stream_valid(stream_valid),
        .stream_busy (stream_busy),
        .stream_done (stream_done),
        .erom_we     (erom_we),
        .erom_addr   (erom_addr),
        .erom_data   (erom_data),
        .pgd_active  (pgd_active),
        .done        (done)
    );

    // ext_rom stand-in: 16-bit-wide, 8192-deep dpram (models the 16384-byte
    // ext_rom as 8192 byte-pairs). Port A written by page_dma, port B read
    // back by the test (1-cycle registered latency).
    logic [12:0] erom_rd_addr = 13'd0;   // WORD read address
    wire  [15:0] erom_rd_data;

    dpram #(.WIDTH(16), .DEPTH(8192)) erom (
        .clk    (clk),
        .addr_a (erom_addr[12:0]),
        .din_a  (erom_data),
        .we_a   (erom_we),
        .dout_a (),
        .addr_b (erom_rd_addr),
        .dout_b (erom_rd_data)
    );

    // -----------------------------------------------------------------
    // Pass/fail bookkeeping
    // -----------------------------------------------------------------
    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    task automatic check(input string name, input logic condition);
        test_num++;
        if (condition) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    // -----------------------------------------------------------------
    // Preload the unique-per-word pattern via the model backdoor (fast).
    //   word(i) == {i[7:0], i[15:8]} -> even byte=i[7:0], odd byte=i[15:8].
    // -----------------------------------------------------------------
    task automatic preload_unique_pattern(input int n_words);
        int i;
        for (i = 0; i < n_words; i++)
            chip.poke_word(25'(2*i), {i[7:0], i[15:8]});
    endtask

    // -----------------------------------------------------------------
    // Trigger page_dma and wait for `done`, watchdog-bounded.
    // -----------------------------------------------------------------
    localparam int WATCHDOG = 60000;   // generous (>> ~8700-cycle 8192-word burst)

    logic done_seen;
    int   active_high_cycles;          // count of cycles pgd_active was high
    int   done_pulse_count;            // # of `done` pulses (must be exactly 1)

    task automatic run_page_dma(input logic [24:0] base, input int n_words);
        int i;
        done_seen          = 1'b0;
        active_high_cycles = 0;
        done_pulse_count   = 0;

        @(posedge clk);
        src_base <= base;
        words    <= n_words[13:0];
        start    <= 1'b1;
        @(posedge clk);
        start    <= 1'b0;

        for (i = 0; (i < WATCHDOG) && !done_seen; i++) begin
            @(posedge clk);
            if (pgd_active)  active_high_cycles++;
            if (done) begin
                done_pulse_count++;
                done_seen = 1'b1;
            end
        end
        // Sample a few extra cycles to ensure `done` is a single 1-clk pulse,
        // not a level that stays high.
        repeat (8) begin
            @(posedge clk);
            if (done) done_pulse_count++;
        end
    endtask

    // -----------------------------------------------------------------
    // Read back all 16384 bytes from the ext_rom stand-in (port B, 1-cycle
    // latency) and verify byte-exact + correct byte order against the unique
    // pattern. The dpram is 16-bit-wide indexed by WORD; decompose each word.
    //   ext_rom[2k]   = word[15:8] == k[7:0]    (even byte)
    //   ext_rom[2k+1] = word[7:0]  == k[15:8]   (odd  byte)
    // -----------------------------------------------------------------
    task automatic verify_ext_rom(input string name, input int n_words);
        int k, mism;
        logic [15:0] w;
        logic [7:0]  exp_even, exp_odd;
        mism = 0;
        for (k = 0; k < n_words; k++) begin
            // Drive the word read address; dpram registers dout_b one clk later.
            @(posedge clk);
            erom_rd_addr <= k[12:0];
            @(posedge clk);   // dout_b now reflects mem[k]
            w        = erom_rd_data;
            exp_even = k[7:0];     // ext_rom[2k]
            exp_odd  = k[15:8];    // ext_rom[2k+1]
            if (w[15:8] !== exp_even || w[7:0] !== exp_odd) begin
                if (mism < 6)
                    $display("  word %0d: ext_rom[%0d]=%02h (exp %02h) ext_rom[%0d]=%02h (exp %02h)",
                             k, 2*k, w[15:8], exp_even, 2*k+1, w[7:0], exp_odd);
                mism++;
            end
        end
        if (mism != 0)
            $display("  %s: %0d mismatches", name, mism);
        check(name, mism == 0);
    endtask

    initial begin
        $display("=== page_dma: stream -> ext_rom 16 KB byte-exact page-in ===");
        $display("");

        // Run sdram.v through its init sequence.
        init = 1'b1;
        repeat (10) @(posedge clk);
        init = 1'b0;
        repeat (2000) @(posedge clk);
        check("controller exited reset (reset==0)", dut.reset == 0);

        // Preload the 8192-word (16 KB) unique pattern via the model backdoor.
        $display("Preloading 8192-word unique pattern (words 0..8191)...");
        preload_unique_pattern(8192);

        // Trigger the page-in: src_base=0, words=8192.
        $display("Triggering page_dma (src_base=0, words=8192)...");
        run_page_dma(25'd0, 8192);

        check("done pulsed within watchdog",                 done_seen);
        check("done was a single 1-clk pulse",               done_pulse_count == 1);
        check("pgd_active high during the copy (>= 8192 cyc)", active_high_cycles >= 8192);

        // Verify all 16384 bytes byte-exact + correct order.
        $display("Reading back 16384 bytes from ext_rom stand-in...");
        verify_ext_rom("16 KB ext_rom byte-exact + byte order", 8192);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
