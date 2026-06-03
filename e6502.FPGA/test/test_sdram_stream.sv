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

    // -----------------------------------------------------------------
    // Stream capture — pulse stream_req for one clk, then collect each
    // stream_valid word into got[]. A watchdog bounds the wait so a broken
    // engine FAILS deterministically rather than hanging --timing forever.
    // -----------------------------------------------------------------
    localparam int MAX_WORDS = 1024;          // capture buffer cap
    localparam int WATCHDOG  = 60000;         // generous sdram_clk-cycle bound

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

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
