// test_sdram_stream.sv — byte-exact stream readback bench (Phase 2.5 page-mode
// streaming read engine).
//
// Wires the REAL sdram.v controller to the behavioral sdram_model.svh (the
// validated page-mode timing oracle), exactly like test_sdram_model_vs_ctrl.
// The harness single-access *writes* a known pattern via port A (exercising the
// already-proven write path), then pulses the stream port and captures every
// stream_valid word, checking byte order against the model decode:
//   word i carries byte 2i in the HIGH lane [15:8] and byte 2i+1 in the LOW
//   lane [7:0] (matches sdram.v dout decode: even byte a[0]=0 -> [15:8]).
//
// Pattern (UNIQUE per word so a wrong-ROW read is detectable, not aliased):
//   mem[byte 2i]   = i[7:0]   -> HIGH lane [15:8]   (even byte)
//   mem[byte 2i+1] = i[15:8]  -> LOW  lane [7:0]    (odd  byte)
//   => expected captured word got[i] == {i[7:0], i[15:8]}  (byte-swapped index)
// Every physical word holds its own distinct index, so reading the wrong row
// (the C1 bug) yields a different word's value and is caught immediately.
//
// Two cases:
//   (a) ROW-ALIGNED: stream_addr=0, 256 words (= exactly one 512-byte row).
//   (b) UNALIGNED MULTI-ROW: stream_addr=100 (NOT 512-aligned), 600 words
//       spanning multiple physical 512-byte rows. With the C1 bug
//       (row_full keyed on read count==256 instead of the physical column
//       wrap cur_addr[8:1]==0xFF) this case reads the wrong row after the
//       first physical boundary and mismatches.
//
// Run: make -C test test_sdram_stream

`timescale 1ns/1ps

`include "sdram_model.svh"

module test_sdram_stream;

    // -----------------------------------------------------------------
    // Clocking — identical scheme to test_sdram_model_vs_ctrl:
    //   clk ~85 MHz, clkref toggled every 8 clk edges (16:1).
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
    // Controller <-> chip wiring. The data bus is tri-state in hardware;
    // for sim we mux on the controller's we_out (controller drives on
    // write, model drives on read).
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

    // Stream port — driven/observed by the harness (NOT tied off).
    logic        stream_req   = 1'b0;
    logic [24:0] stream_addr  = 25'd0;
    logic [13:0] stream_words = 14'd0;
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

        // Stream port wired to real harness signals.
        .stream_req  (stream_req),
        .stream_addr (stream_addr),
        .stream_words(stream_words),
        .stream_dout (stream_dout),
        .stream_valid(stream_valid),
        .stream_busy (stream_busy),
        .stream_done (stream_done)
    );

    // The timing oracle. Its sd_data_in is the controller-input port — i.e.
    // the chip's *output* to the controller (read data). The controller drives
    // the model's sd_data_out (write data).
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
    // SDRAM command counters (case (c) command-sequence proof).
    //
    // Decode the command pins {sd_cs,sd_ras,sd_cas,sd_we} on every clk
    // posedge while a burst is in flight (stream_busy high). Encodings match
    // sdram.v STR_CMD_* (~line 99) and the model decode (sdram_model.svh ~64):
    //   ACTIVE=4'b0011  READ=4'b0101  PRECHARGE=4'b0010  AUTO_REFRESH=4'b0001
    // PRECHARGE is further split by A10 = sd_addr[10]:
    //   A10=1 -> precharge-ALL (the S_PRE_ALL clean-entry command)
    //   A10=0 -> single-bank precharge (the per-row S_PRECHARGE command)
    // count_en && stream_busy gates counting to the burst-of-interest only:
    // stream_busy ensures only commands issued while the stream FSM owns the
    // bus are tallied, so a stray idle AUTO_REFRESH at the count_en arm/disarm
    // edges cannot leak in and break the exact counts.
    // -----------------------------------------------------------------
    wire [3:0] cmd_pins = {sd_cs, sd_ras, sd_cas, sd_we};
    logic count_en = 1'b0;
    int cnt_active   = 0;
    int cnt_read     = 0;
    int cnt_pre_all  = 0;   // PRECHARGE with A10=1
    int cnt_pre_bank = 0;   // PRECHARGE with A10=0
    int cnt_refresh  = 0;

    always @(posedge clk) begin
        if (count_en && stream_busy) begin
            case (cmd_pins)
                4'b0011: cnt_active  <= cnt_active  + 1;   // ACTIVE
                4'b0101: cnt_read    <= cnt_read    + 1;   // READ
                4'b0001: cnt_refresh <= cnt_refresh + 1;   // AUTO_REFRESH
                4'b0010: begin                             // PRECHARGE
                    if (sd_addr[10]) cnt_pre_all  <= cnt_pre_all  + 1;
                    else             cnt_pre_bank <= cnt_pre_bank + 1;
                end
                default: ; // NOP / INHIBIT / etc — ignore
            endcase
        end
    end

    // -----------------------------------------------------------------
    // Stream cycle-count measurement (case (d)).
    // A free-running counter on clk (== sdram_clk in this bench). cyc_arm
    // starts the count on the stream_req edge and cyc_run gates it; the count
    // stops on stream_done. cyc_count then holds the exact number of sdram_clk
    // cycles from stream_req to stream_done — the MEASURED burst latency that
    // replaces the design-doc "~84 us @ 100 MHz" estimate.
    // -----------------------------------------------------------------
    logic cyc_arm = 1'b0;   // 1 once we want to measure this burst
    logic cyc_run = 1'b0;   // 1 while counting (req..done window)
    int   cyc_count = 0;

    always @(posedge clk) begin
        if (cyc_arm) begin
            if (stream_req) begin
                // burst kicked off: start a fresh count from this edge
                cyc_run   <= 1'b1;
                cyc_count <= 0;
            end else if (cyc_run) begin
                cyc_count <= cyc_count + 1;
                if (stream_done)
                    cyc_run <= 1'b0;   // freeze count at stream_done
            end
        end
    end

    // -----------------------------------------------------------------
    // Refresh-obligation monitor (case (e)).
    //
    // The page-mode FSM issues one UNCONDITIONAL AUTO_REFRESH per physical
    // 512-byte row boundary (sdram.v S_REFRESH, ~line 307). The JEDEC tREF
    // obligation is 64 ms / 8192 rows = 7.8 us/row = 780 sdram_clk cycles
    // @ 100 MHz. This monitor proves the obligation is met across the WHOLE
    // 8192-word/32-row burst: no AUTO_REFRESH gap is dangerously long.
    //
    // It watches the command pins for AUTO_REFRESH (4'b0001 == STR_CMD_AUTO_
    // REFRESH) and records, in sdram_clk cycles:
    //   - rf_gap_max : max gap between consecutive in-burst refreshes
    //   - rf_first   : cycles from stream_req to the FIRST refresh
    //   - rf_last_to_done : cycles from the LAST refresh to stream_done
    //   - rf_count   : number of in-burst refreshes seen
    // rf_en (armed by the test around the burst) AND stream_busy gate it to
    // in-burst commands only — identical discipline to the count_en/stream_
    // busy command counter above, so the idle single-access AUTO_REFRESH
    // (sdram.v ~line 483, issued when NOT streaming) cannot leak in.
    //
    // rf_clk is a free-running sdram_clk index used to timestamp each event;
    // rf_last_idx holds the timestamp of the previous refresh so the gap is
    // a simple subtract. rf_req_idx is stamped on stream_req, rf_done_idx on
    // stream_done, giving the req->first and last->done tail gaps.
    // -----------------------------------------------------------------
    logic        rf_en      = 1'b0;
    int          rf_clk     = 0;     // free-running sdram_clk index
    int          rf_count   = 0;     // in-burst AUTO_REFRESH count
    int          rf_last_idx = 0;    // timestamp of previous refresh
    int          rf_gap_max = 0;     // max consecutive-refresh gap
    int          rf_req_idx = 0;     // timestamp of stream_req
    int          rf_done_idx = 0;    // timestamp of stream_done
    int          rf_first    = 0;    // req -> first refresh gap
    logic        rf_first_seen = 1'b0;

    always @(posedge clk) begin
        rf_clk <= rf_clk + 1;
        if (rf_en) begin
            // Stamp the request edge so the req->first tail can be measured.
            if (stream_req) begin
                rf_req_idx    <= rf_clk;
                rf_last_idx   <= rf_clk;   // gap baseline starts at req
                rf_first_seen <= 1'b0;
            end
            // Count + gap only for AUTO_REFRESH issued while the stream FSM
            // owns the bus (mirrors the count_en && stream_busy discipline).
            if (stream_busy && (cmd_pins == 4'b0001)) begin
                int gap;
                gap = rf_clk - rf_last_idx;
                if (gap > rf_gap_max) rf_gap_max <= gap;
                rf_last_idx <= rf_clk;
                rf_count    <= rf_count + 1;
                if (!rf_first_seen) begin
                    rf_first      <= rf_clk - rf_req_idx;
                    rf_first_seen <= 1'b1;
                end
            end
            // Stamp completion to measure the last-refresh -> done tail.
            if (stream_done)
                rf_done_idx <= rf_clk;
        end
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
    // Single-access port-A write — same pattern as the proven harnesses.
    // The request bridges in sdram.v hold weA/addr/din until done, so we
    // keep each request asserted across multiple full q 0..7 cycles.
    // -----------------------------------------------------------------
    task automatic port_a_write(input logic [24:0] addr, input logic [7:0] data);
        @(posedge clk);
        addrA <= addr;
        dinA  <= data;
        weA   <= 1'b1;
        oeA   <= 1'b0;
        repeat (24) @(posedge clk);
        weA   <= 1'b0;
        repeat (4) @(posedge clk);
    endtask

    // Write a unique-per-word pattern over `n_words` words starting at WORD 0:
    //   mem[byte 2i]   = i[7:0]   (even byte -> HIGH lane)
    //   mem[byte 2i+1] = i[15:8]  (odd  byte -> LOW  lane)
    task automatic write_unique_pattern(input int n_words);
        int i;
        logic [24:0] ba_even, ba_odd;
        for (i = 0; i < n_words; i++) begin
            ba_even = (2*i);
            ba_odd  = (2*i + 1);
            port_a_write(ba_even, i[7:0]);    // even byte -> high lane
            port_a_write(ba_odd,  i[15:8]);   // odd  byte -> low  lane
        end
    endtask

    // Same unique-per-word pattern, but seeded directly into the model via the
    // test-only poke_word backdoor (zero clk cycles). Used by case (d) so the
    // 16 KB / 8192-word burst doesn't pay ~460k clk for the (already-proven)
    // single-access write path. The stored word matches the run-time decode:
    //   HIGH lane [15:8] = even byte 2i = i[7:0]
    //   LOW  lane [7:0]  = odd  byte 2i+1 = i[15:8]
    //   => word(i) == {i[7:0], i[15:8]}  (same value verify_unique expects)
    // i ranges 0..8191 (13-bit); {i[7:0], i[15:8]} packs all 13 bits, so every
    // word is distinct over 0..8191 — non-aliasing, a wrong-row read is caught.
    task automatic preload_unique_pattern(input int n_words);
        int i;
        for (i = 0; i < n_words; i++)
            chip.poke_word(25'(2*i), {i[7:0], i[15:8]});
    endtask

    // -----------------------------------------------------------------
    // Stream capture — pulse stream_req for one clk, then collect each
    // stream_valid word into got[]. A watchdog bounds the wait so a broken
    // engine FAILS deterministically rather than hanging --timing forever.
    // -----------------------------------------------------------------
    localparam int MAX_WORDS = 8192;          // capture buffer cap (case (d) 16 KB burst)
    localparam int WATCHDOG  = 60000;         // generous sdram_clk-cycle bound
                                              // (>> ~8700-cycle 8192-word burst)

    logic [15:0] got [0:MAX_WORDS-1];
    int          got_count;
    logic        done_seen;

    // Run one stream burst: pulse req, capture words, watchdog-bounded.
    task automatic run_stream(input logic [24:0] saddr, input int n_words);
        int i;
        for (i = 0; i < MAX_WORDS; i++) got[i] = 16'hxxxx;
        got_count = 0;
        done_seen = 1'b0;

        @(posedge clk);
        stream_addr  <= saddr;
        stream_words <= n_words[13:0];
        stream_req   <= 1'b1;
        @(posedge clk);
        stream_req   <= 1'b0;

        for (i = 0; (i < WATCHDOG) && !done_seen; i++) begin
            @(posedge clk);
            if (stream_valid && (got_count < MAX_WORDS)) begin
                got[got_count] = stream_dout;
                got_count++;
            end
            // stream_done is a 1-clk pulse; the valid capture above runs on the
            // same edge, so a word presented alongside done is collected first.
            if (stream_done)
                done_seen = 1'b1;
        end
    endtask

    // Verify the captured words are byte-exact against the unique pattern,
    // for `n_words` words whose first WORD index is (saddr_byte/2). The
    // controller streams whole 16-bit words starting at the word containing
    // stream_addr (decode uses a[24:1] as the word key), so for an even
    // stream_addr=2*w0 the j-th captured word is physical word (w0 + j):
    //   got[j] == { word(w0+j)[7:0], word(w0+j)[15:8] } == {(w0+j)[7:0],(w0+j)[15:8]}
    task automatic verify_unique(input string name, input int w0, input int n_words);
        int    j, mism;
        logic [7:0] exp_hi, exp_lo;
        mism = 0;
        for (j = 0; j < n_words; j++) begin
            int widx;
            widx   = w0 + j;
            exp_hi = widx[7:0];    // even byte 2*widx   -> HIGH lane [15:8]
            exp_lo = widx[15:8];   // odd  byte 2*widx+1 -> LOW  lane [7:0]
            if (got[j][15:8] !== exp_hi || got[j][7:0] !== exp_lo) begin
                if (mism < 6)
                    $display("  word %0d (phys %0d): got %04h expected hi=%02h lo=%02h",
                             j, widx, got[j], exp_hi, exp_lo);
                mism++;
            end
        end
        if (mism != 0)
            $display("  %s: %0d mismatches", name, mism);
        check(name, mism == 0);
    endtask

    initial begin
        $display("=== SDRAM page-mode stream readback (byte-exact, unique pattern) ===");
        $display("");

        // Run sdram.v through its init sequence (PRECHARGE + LOAD_MODE).
        init = 1'b1;
        repeat (10) @(posedge clk);
        init = 1'b0;
        repeat (2000) @(posedge clk);
        check("controller exited reset (reset==0)", dut.reset == 0);

        // =============================================================
        // CASE (a): ROW-ALIGNED — 256 words from word 0 (one 512-B row).
        // =============================================================
        $display("Writing 256-word unique pattern (words 0..255) via port A...");
        write_unique_pattern(256);
        $display("Streaming 256 words from byte addr 0 (row-aligned)...");

        run_stream(25'd0, 256);
        check("(a) stream_done asserted within watchdog", done_seen);
        check("(a) captured exactly 256 words", got_count == 256);
        verify_unique("(a) row-aligned 256 words byte-exact", 0, 256);

        // =============================================================
        // CASE (b): UNALIGNED MULTI-ROW — start byte 100 (word 50, col 50,
        // NOT 512-aligned), 600 words. 600 words from word 50 spans
        // physical words 50..649: crosses the 256-word/512-byte physical
        // row boundary at word 256 (and again at 512). With the C1 bug the
        // FSM closes a row after 256 *reads* (not at the physical column
        // wrap cur_addr[8:1]==0xFF), so after the first 256-read window it
        // reads the WRONG row -> mismatches starting ~word 206.
        // =============================================================
        $display("");
        $display("Writing 650-word unique pattern (words 0..649) via port A...");
        write_unique_pattern(650);
        $display("Streaming 600 words from byte addr 100 (UNALIGNED, multi-row)...");

        run_stream(25'd100, 600);
        check("(b) stream_done asserted within watchdog", done_seen);
        check("(b) captured exactly 600 words", got_count == 600);
        verify_unique("(b) unaligned multi-row 600 words byte-exact", 50, 600);

        // =============================================================
        // CASE (c): ALIGNED 4-ROW BURST + COMMAND-SEQUENCE PROOF.
        // stream_addr=0 (512-aligned), 1024 words = 4 physical 256-word
        // (512-byte) rows. This case proves the FSM issues the CORRECT
        // SDRAM command sequence at every row boundary, not just correct
        // data. We decode the command pins on each clk edge during the
        // burst and assert exact counts.
        //
        // FSM trace (sdram.v, this exact start/length):
        //   S_PRE_ALL  -> 1x PRECHARGE-ALL  (A10=1, clean entry)
        //   per row (x4):
        //     S_ACTIVATE  -> 1x ACTIVE
        //     S_READ      -> 256x READ (stops at row_full cur_addr[8:1]==0xFF)
        //     S_PRECHARGE -> 1x PRECHARGE  (A10=0, single bank)
        //     S_REFRESH   -> 1x AUTO_REFRESH (one per row boundary)
        // Expected totals over the burst:
        //   ACTIVE=4, READ=1024, single-bank PRECHARGE=4, AUTO_REFRESH=4,
        //   plus the one entry PRECHARGE-ALL (A10=1). The pins therefore
        //   show 5 PRECHARGE commands total (1 all + 4 per-row); the A10
        //   split lets us account for the entry PRECHARGE-ALL exactly.
        // The sdram_model oracle $fatal's on any illegal sequence, so a
        // clean run with these counts also proves the sequence is legal.
        // =============================================================
        $display("");
        $display("Writing 1024-word unique pattern (words 0..1023) via port A...");
        write_unique_pattern(1024);
        $display("Streaming 1024 words from byte addr 0 (ALIGNED, 4 physical rows)...");

        // Zero counters and arm decode just before the burst.
        cnt_active = 0; cnt_read = 0; cnt_pre_all = 0; cnt_pre_bank = 0; cnt_refresh = 0;
        @(posedge clk);
        count_en = 1'b1;
        run_stream(25'd0, 1024);
        @(posedge clk);
        count_en = 1'b0;

        $display("  measured: ACTIVE=%0d READ=%0d PRECHARGE-ALL=%0d PRECHARGE-bank=%0d AUTO_REFRESH=%0d",
                 cnt_active, cnt_read, cnt_pre_all, cnt_pre_bank, cnt_refresh);

        check("(c) stream_done asserted within watchdog", done_seen);
        check("(c) captured exactly 1024 words", got_count == 1024);
        verify_unique("(c) aligned 4-row 1024 words byte-exact", 0, 1024);

        // Command-sequence assertions (the NEW value of this case).
        check("(c) exactly 4 ACTIVATE (one per row)",        cnt_active   == 4);
        check("(c) exactly 4 single-bank PRECHARGE (A10=0)", cnt_pre_bank == 4);
        check("(c) exactly 1 PRECHARGE-ALL entry (A10=1)",   cnt_pre_all  == 1);
        check("(c) >=3 AUTO_REFRESH (one per row boundary)", cnt_refresh  >= 3);
        check("(c) exactly 4 AUTO_REFRESH (one per row)",    cnt_refresh  == 4);
        check("(c) exactly 1024 READ",                       cnt_read     == 1024);

        // =============================================================
        // CASE (d): 16 KB / 8192-word CYCLE-COUNT MEASUREMENT.
        // stream_addr=0 (512-aligned), 8192 words = 16 KB = 32 physical
        // 256-word (512-byte) rows. This case MEASURES the real
        // stream_req -> stream_done latency in sdram_clk cycles, replacing
        // the design-doc "~84 us @ 100 MHz" estimate with the recorded
        // number. The window 7000 < cycles < 11000 is generous around the
        // ~8400-8700 expected (32 rows x ~270 cyc/row).
        //
        // Pattern is preloaded via the model backdoor (poke_word) — the
        // write path is already proven byte-exact in cases (a)/(b)/(c), and
        // single-access writing 8192 words is ~460k clk. Byte-exactness of
        // all 8192 captured words is still fully checked below.
        //
        // The sdram_model oracle $fatal's on any illegal command/timing, so
        // a clean run also proves the 32-row burst is a legal sequence.
        // =============================================================
        $display("");
        $display("Preloading 8192-word unique pattern (words 0..8191) via model backdoor...");
        preload_unique_pattern(8192);
        $display("Streaming 8192 words from byte addr 0 (16 KB, 32 physical rows)...");

        // Arm the cycle counter; run_stream pulses stream_req inside.
        cyc_arm = 1'b1;
        run_stream(25'd0, 8192);
        cyc_arm = 1'b0;

        $display("measured stream cycles: %0d", cyc_count);
        $display("  (= %.2f us @ 100 MHz)", cyc_count / 100.0);

        check("(d) stream_done asserted within watchdog", done_seen);
        check("(d) captured exactly 8192 words", got_count == 8192);
        verify_unique("(d) 16 KB 8192 words byte-exact", 0, 8192);
        check("(d) measured cycles in window (7000 < n < 11000)",
              (cyc_count > 7000) && (cyc_count < 11000));

        // =============================================================
        // CASE (e): 16 KB / 8192-word REFRESH-OBLIGATION proof.
        // stream_addr=0 (512-aligned), 8192 words = 16 KB = 32 physical
        // rows. The page-mode FSM issues one unconditional AUTO_REFRESH per
        // row boundary; this case proves the JEDEC tREF obligation is met
        // across the WHOLE burst: NO gap between consecutive refreshes (nor
        // the req->first and last->done tails) exceeds 780 sdram_clk cycles
        // (= 7.8 us/row @ 100 MHz). Per-row unconditional refresh gives
        // ~one per ~270 cycles, well inside the 780-cycle bound.
        //
        // Pattern stays preloaded from case (d) (same words 0..8191), so no
        // re-write is needed. The sdram_model oracle $fatal's on any illegal
        // sequence, so a clean run also proves the 32-row burst is legal.
        // =============================================================
        $display("");
        $display("Streaming 8192 words from byte addr 0 (16 KB) for REFRESH-GAP proof...");

        // Reset + arm the refresh monitor just before the burst.
        rf_count = 0; rf_gap_max = 0; rf_first = 0;
        rf_first_seen = 1'b0;
        @(posedge clk);
        rf_en = 1'b1;
        run_stream(25'd0, 8192);
        @(posedge clk);
        rf_en = 1'b0;

        begin
            int rf_last_to_done;
            rf_last_to_done = rf_done_idx - rf_last_idx;
            $display("  refreshes=%0d  max consecutive gap=%0d cyc  req->first=%0d cyc  last->done=%0d cyc",
                     rf_count, rf_gap_max, rf_first, rf_last_to_done);
            $display("  (bound = 780 sdram_clk cycles = 7.8 us @ 100 MHz)");

            check("(e) stream_done asserted within watchdog", done_seen);
            check("(e) captured exactly 8192 words", got_count == 8192);
            verify_unique("(e) 16 KB 8192 words byte-exact", 0, 8192);
            // 32 physical rows -> >=31 in-burst boundary refreshes; at least
            // one per row boundary must be present for the gap proof to mean
            // anything.
            check("(e) >= 31 in-burst AUTO_REFRESH (one per row)", rf_count >= 31);
            // THE OBLIGATION: no consecutive-refresh gap exceeds 780 cycles.
            check("(e) max consecutive refresh gap <= 780 cyc",  rf_gap_max <= 780);
            // No unguarded head: first refresh within 780 of stream_req.
            check("(e) req -> first refresh <= 780 cyc",         rf_first <= 780);
            // No unguarded tail: last refresh within 780 of stream_done.
            check("(e) last refresh -> done <= 780 cyc",         rf_last_to_done <= 780);
        end

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
