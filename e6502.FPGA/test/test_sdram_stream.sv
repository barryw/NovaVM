// test_sdram_stream.sv — byte-exact stream readback bench (Phase 2.5 page-mode
// streaming read engine, bench case (a)).
//
// Wires the REAL sdram.v controller to the behavioral sdram_model.svh (the
// validated page-mode timing oracle), exactly like test_sdram_model_vs_ctrl.
// First the harness single-access *writes* a known pattern (mem[byte k]=k&0xFF)
// for 512 bytes via port A — exercising the already-proven write path. It then
// pulses the stream port (stream_req / stream_addr=0 / stream_words=256) and
// captures every stream_valid word. After stream_done it checks byte order:
//   word i carries byte 2i in the HIGH lane [15:8] and byte 2i+1 in the LOW
//   lane [7:0] (matches sdram.v dout decode: even byte a[0]=0 -> [15:8]).
//
// TDD RED: against the current Task-2 STUB (sdram.v holds stream_* at 0 and
// never asserts stream_done/stream_valid) this test MUST fail — no valid words
// arrive and stream_done never fires. A cycle watchdog forces a deterministic
// FAILURE+$finish so the harness reports FAIL instead of hanging forever.
// Task 4 implements the page-mode FSM; this SAME test then goes green unchanged.
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
    logic [12:0] stream_words = 13'd0;
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

    // -----------------------------------------------------------------
    // Stream capture — pulse stream_req for one clk, then collect each
    // stream_valid word into got[]. A watchdog bounds the wait so the stub
    // (which never asserts stream_done) FAILS deterministically rather than
    // hanging the --timing simulation forever.
    // -----------------------------------------------------------------
    localparam int N_WORDS  = 256;
    localparam int WATCHDOG = 20000;   // generous sdram_clk-cycle bound

    logic [15:0] got [0:N_WORDS-1];
    int          got_count;
    logic        done_seen;

    initial begin
        int i;

        for (i = 0; i < N_WORDS; i++) got[i] = 16'hxxxx;
        got_count = 0;
        done_seen = 1'b0;

        $display("=== SDRAM page-mode stream readback (byte-exact) ===");
        $display("");

        // Run sdram.v through its init sequence (PRECHARGE + LOAD_MODE).
        init = 1'b1;
        repeat (10) @(posedge clk);
        init = 1'b0;
        repeat (2000) @(posedge clk);
        check("controller exited reset (reset==0)", dut.reset == 0);

        // ---- Step 1: write the known pattern mem[byte k] = k & 0xFF for
        //      512 bytes (bytes 0..511 -> words 0..255 in row 0).
        $display("Writing 512-byte ramp (byte k = k & 0xFF) via port A...");
        for (i = 0; i < 512; i++)
            port_a_write(i[24:0], i[7:0]);
        $display("Write complete; starting stream of %0d words from addr 0.", N_WORDS);

        // ---- Step 2: pulse the stream request.
        @(posedge clk);
        stream_addr  <= 25'd0;
        stream_words <= N_WORDS[12:0];
        stream_req   <= 1'b1;
        @(posedge clk);
        stream_req   <= 1'b0;

        // ---- Step 3: capture stream_valid words, watchdog-bounded.
        for (i = 0; (i < WATCHDOG) && !done_seen; i++) begin
            @(posedge clk);
            if (stream_valid && (got_count < N_WORDS)) begin
                got[got_count] = stream_dout;
                got_count++;
            end
            // stream_done is a 1-clk pulse; the valid capture above runs on the
            // same edge, so a word presented alongside done is collected first.
            if (stream_done)
                done_seen = 1'b1;
        end

        // ---- Watchdog verdict ----
        check("stream_done asserted within watchdog", done_seen);
        check("captured all 256 words before done", got_count == N_WORDS);

        if (!done_seen) begin
            $display("");
            $display("  WATCHDOG: stream_done never fired in %0d cycles (got_count=%0d). Stream engine is a stub.",
                     WATCHDOG, got_count);
        end

        // ---- Step 4: byte-order assertions over all 256 words.
        //   word i = bytes (2i, 2i+1); even byte (a[0]=0) -> HIGH lane [15:8],
        //   odd byte (a[0]=1) -> LOW lane [7:0]. Pattern byte k = k & 0xFF.
        begin
            int mism;
            mism = 0;
            for (i = 0; i < N_WORDS; i++) begin
                logic [7:0] exp_hi, exp_lo;
                exp_hi = (2*i)     & 8'hFF;   // even byte 2i   -> [15:8]
                exp_lo = (2*i + 1) & 8'hFF;   // odd  byte 2i+1 -> [7:0]
                if (got[i][15:8] !== exp_hi || got[i][7:0] !== exp_lo) begin
                    if (mism < 4)
                        $display("  word %0d: got %04h expected hi=%02h lo=%02h",
                                 i, got[i], exp_hi, exp_lo);
                    mism++;
                end
            end
            check("all 256 words byte-exact (even=HIGH lane, odd=LOW lane)",
                  mism == 0);
        end

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
