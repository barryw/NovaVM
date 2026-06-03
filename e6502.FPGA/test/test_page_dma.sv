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
// consecutive clocks). The real `ext_rom` is an 8-bit byte-addressed dpram
// (one write/clk), so a single 8-bit write port physically cannot absorb 2
// bytes/word at 1 word/clk during the dense in-row run. page_dma resolves this
// by SELF-PACING the stream via `stream_ready` (Task 11a) and buffering the
// 1->0.5 word/clk rate step in a small SYNCHRONOUS skid buffer (both the stream
// and page_dma are in sdram_clk — no CDC). The byte-writer then emits the two
// 8-bit ext_rom writes per word (2 clk/word) to the REAL pgd_erom_* port.
//
// This test models the ext_rom stand-in as the REAL 8-bit / 16384-deep dpram:
// page_dma writes BYTE address pgd_erom_addr=2k with the even byte
// (stream_dout[15:8]) then 2k+1 with the odd byte (stream_dout[7:0]). The
// readback below asserts the per-byte image directly:
//   ext_rom[2k]   = stream_dout[15:8]  (even byte, HIGH lane)
//   ext_rom[2k+1] = stream_dout[7:0]   (odd  byte, LOW  lane)
//
// page_dma's stream_ready output is wired to sdram.stream_ready, so this test
// exercises the real pacing (the in-flight CAS pipeline + the skid buffer).
//
// ---------------------------------------------------------------------------
// Pattern (UNIQUE per word so a dropped/wrong-row word is caught, mirrors
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
    wire         stream_ready;   // page_dma -> sdram self-pacing
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
        .stream_ready(stream_ready),   // Task 11c: driven by page_dma for self-pacing
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
    // page_dma drives the stream port, self-paces via stream_ready, and emits
    // TWO 8-bit byte-writes per stream word to the REAL ext_rom port:
    //   pgd_erom_addr=2k, pgd_erom_data=dout[15:8] (even, HIGH lane), then
    //   pgd_erom_addr=2k+1, pgd_erom_data=dout[7:0] (odd, LOW lane).
    // The stand-in dpram is 8-bit-wide / 16384-deep (the real ext_rom byte
    // image).
    // -----------------------------------------------------------------
    logic        start = 1'b0;
    logic [24:0] src_base = 25'd0;
    logic [13:0] words = 14'd0;

    wire         pgd_erom_we;
    wire [13:0]  pgd_erom_addr;   // BYTE address (0..16383)
    wire [7:0]   pgd_erom_data;   // byte lane
    wire         pgd_active;
    wire         done;

    page_dma pgd (
        .clk          (clk),
        .rst          (init),
        .start        (start),
        .src_base     (src_base),
        .words        (words),
        .stream_req   (stream_req),
        .stream_addr  (stream_addr),
        .stream_words (stream_words),
        .stream_ready (stream_ready),
        .stream_dout  (stream_dout),
        .stream_valid (stream_valid),
        .stream_busy  (stream_busy),
        .stream_done  (stream_done),
        .pgd_erom_we  (pgd_erom_we),
        .pgd_erom_addr(pgd_erom_addr),
        .pgd_erom_data(pgd_erom_data),
        .pgd_active   (pgd_active),
        .done         (done)
    );

    // ext_rom stand-in: REAL 8-bit-wide, 16384-deep dpram. Port A written by
    // page_dma (byte address + byte lane), port B read back by the test
    // (1-cycle registered latency).
    logic [13:0] erom_rd_addr = 14'd0;   // BYTE read address
    wire  [7:0]  erom_rd_data;

    dpram #(.WIDTH(8), .DEPTH(16384)) erom (
        .clk    (clk),
        .addr_a (pgd_erom_addr),
        .din_a  (pgd_erom_data),
        .we_a   (pgd_erom_we),
        .dout_a (),
        .addr_b (erom_rd_addr),
        .dout_b (erom_rd_data)
    );

    // -----------------------------------------------------------------
    // Skid-buffer margin monitor (hierarchical ref into the page_dma instance).
    // Proves the no-drop margin is a TESTED invariant: the FIFO must stay below
    // full (no word dropped) AND must actually be exercised near peak (so a
    // light load that never stresses the margin would fail the >=10 assert).
    // -----------------------------------------------------------------
    int   fifo_peak  = 0;   // high-water mark of pgd.fifo_cnt
    logic ever_full  = 1'b0;
    always @(posedge clk) begin
        if (pgd.fifo_cnt > fifo_peak) fifo_peak <= pgd.fifo_cnt;
        if (pgd.fifo_cnt == pgd.FIFO_DEPTH[pgd.FIFO_AW:0]) ever_full <= 1'b1;
    end

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

    // words==0 hazard-guard bookkeeping (separate so a wedge can't corrupt the
    // main-run counters). A tight bound — a non-wedging machine completes in a
    // couple cycles; a wedged one trips the bound with done never seen.
    localparam int ZW_BOUND = 16;      // generous for an immediate completion
    logic zw_done_seen;
    int   zw_done_pulse_count;
    logic zw_active_low_after;

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
    // words==0 trigger with a TIGHT watchdog. A correct machine completes
    // immediately (done within a couple cycles, pgd_active never stuck). A
    // wedged machine never sees `done` and leaves pgd_active high -> the
    // bound trips with zw_done_seen=0 and zw_active_low_after=0.
    // -----------------------------------------------------------------
    task automatic run_zero_word_page_dma(input logic [24:0] base);
        int i;
        zw_done_seen        = 1'b0;
        zw_done_pulse_count = 0;
        zw_active_low_after = 1'b0;

        @(posedge clk);
        src_base <= base;
        words    <= 14'd0;
        start    <= 1'b1;
        // Deassert `start` and begin observing on the SAME edge the FSM first
        // samples `start`. An immediate-completion `done` pulses on this very
        // edge, so the observation window must open here (a separate
        // start-deassert edge before the loop would skip past the 1-clk pulse).
        for (i = 0; (i < ZW_BOUND) && !zw_done_seen; i++) begin
            @(posedge clk);
            start <= 1'b0;
            if (done) begin
                zw_done_pulse_count++;
                zw_done_seen = 1'b1;
            end
        end
        // Settle, then confirm the FSM returned to idle: pgd_active low and
        // `done` is a single 1-clk pulse (no level stuck high).
        repeat (8) begin
            @(posedge clk);
            if (done) zw_done_pulse_count++;
        end
        zw_active_low_after = !pgd_active;
    endtask

    // -----------------------------------------------------------------
    // Read back all 16384 bytes from the REAL 8-bit ext_rom stand-in (port B,
    // 1-cycle latency) and verify byte-exact + correct byte order against the
    // unique pattern. Two byte reads per word.
    //   ext_rom[2k]   == k[7:0]    (even byte, HIGH lane)
    //   ext_rom[2k+1] == k[15:8]   (odd  byte, LOW  lane)
    // -----------------------------------------------------------------
    task automatic verify_ext_rom(input string name, input int n_words);
        int k, mism;
        logic [7:0] even_b, odd_b;
        logic [7:0] exp_even, exp_odd;
        mism = 0;
        for (k = 0; k < n_words; k++) begin
            exp_even = k[7:0];     // ext_rom[2k]
            exp_odd  = k[15:8];    // ext_rom[2k+1]

            // even byte at 2k (dpram registers dout_b one clk after addr)
            @(posedge clk);
            erom_rd_addr <= 14'(2*k);
            @(posedge clk);
            even_b = erom_rd_data;

            // odd byte at 2k+1
            erom_rd_addr <= 14'(2*k+1);
            @(posedge clk);
            odd_b = erom_rd_data;

            if (even_b !== exp_even || odd_b !== exp_odd) begin
                if (mism < 6)
                    $display("  word %0d: ext_rom[%0d]=%02h (exp %02h) ext_rom[%0d]=%02h (exp %02h)",
                             k, 2*k, even_b, exp_even, 2*k+1, odd_b, exp_odd);
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

        // Skid-buffer margin invariant: the FIFO must never have hit full (no
        // word dropped) AND must have been driven near peak during the dense
        // in-row run — otherwise the back-pressure margin was never exercised.
        $display("Skid FIFO peak occupancy: %0d / %0d", fifo_peak, pgd.FIFO_DEPTH);
        check("skid FIFO never full (no word dropped)", !ever_full);
        check("skid buffer exercised near peak (>=10)", fifo_peak >= 10);

        // -----------------------------------------------------------------
        // words==0 latent-hazard guard. A zero-word `start` must NOT wedge
        // the FSM. sdram.v's S_IDLE requires stream_words!=0, so it ignores
        // stream_req for a zero-word request and never asserts stream_done.
        // If page_dma issued the request anyway it would sit in S_RUN with
        // pgd_active high forever (permanent CPU stall once Task 11 ties
        // rdy_out to pgd_active). The contract: treat words==0 as immediate
        // completion -- `done` pulses within a couple cycles, pgd_active is
        // never raised (or returns low promptly), and the FSM is back in idle.
        // -----------------------------------------------------------------
        $display("Triggering page_dma with words=0 (must not wedge)...");
        run_zero_word_page_dma(25'd0);
        check("words==0: done pulsed within tight bound",   zw_done_seen);
        check("words==0: done was a single 1-clk pulse",    zw_done_pulse_count == 1);
        check("words==0: pgd_active never stuck high",       zw_active_low_after);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
