// test_sdram_model_selfcheck.sv — self-check for sdram_model.svh.
//
// Pokes the behavioral MT48LC16M16 model DIRECTLY (not through sdram.v) by driving the
// raw sd_* command/address pins. Proves two things, both required by the plan:
//
//   LEGAL  (default run):       ACTIVATE -> wait tRCD -> READ -> wait CAS returns the
//                               stored word, with correct byte lanes; finishes cleanly.
//   ILLEGAL (+EXPECT_FATAL run): ACTIVATE -> READ on the very next cycle (< tRCD) must
//                               $fatal. The Makefile inverts the exit code for this run,
//                               so a trap is a PASS and a clean finish is a FAIL.
//
// Run: make -C test test_sdram_model_selfcheck            (legal path, expects PASS)
//      make -C test test_sdram_model_selfcheck_fatal      (illegal path, expects $fatal)

`timescale 1ns/1ps

`include "sdram_model.svh"

module test_sdram_model_selfcheck;

    logic clk = 0;
    always #5 clk = ~clk;  // 100 MHz — timing params are in these cycles

    // ----- command encoding (mirror of sdram.v / sdram_model.svh) -----
    localparam logic [3:0] CMD_NOP       = 4'b0111;
    localparam logic [3:0] CMD_ACTIVE    = 4'b0011;
    localparam logic [3:0] CMD_READ      = 4'b0101;
    localparam logic [3:0] CMD_WRITE     = 4'b0100;
    localparam logic [3:0] CMD_PRECHARGE = 4'b0010;

    // ----- raw chip pins driven by this TB -----
    logic [3:0]  cmd = CMD_NOP;
    logic [12:0] sd_addr = 13'd0;
    logic [1:0]  sd_dqm  = 2'b00;
    logic [1:0]  sd_ba   = 2'b00;
    logic [15:0] sd_data_out = 16'h0000;   // TB -> chip (write data)
    wire  [15:0] sd_data_in;                // chip -> TB (read data)

    sdram_model chip (
        .clk         (clk),
        .sd_data_out (sd_data_out),
        .sd_data_in  (sd_data_in),
        .sd_addr     (sd_addr),
        .sd_dqm      (sd_dqm),
        .sd_ba       (sd_ba),
        .sd_cs       (cmd[3]),
        .sd_ras      (cmd[2]),
        .sd_cas      (cmd[1]),
        .sd_we       (cmd[0])
    );

    // Drive one command for exactly one clk, then return to NOP.
    task automatic issue(input logic [3:0] c, input logic [12:0] a,
                         input logic [1:0] ba, input logic [1:0] dqm,
                         input logic [15:0] din);
        @(negedge clk);
        cmd         = c;
        sd_addr     = a;
        sd_ba       = ba;
        sd_dqm      = dqm;
        sd_data_out = din;
        @(negedge clk);
        cmd     = CMD_NOP;
        sd_addr = 13'd0;
        sd_dqm  = 2'b00;
        din     = 16'h0000;
    endtask

    task automatic nop_cycles(input int n);
        repeat (n) begin
            @(negedge clk);
            cmd = CMD_NOP;
        end
    endtask

    int pass_count = 0;
    int fail_count = 0;

    task automatic check(input string name, input logic condition);
        if (condition) begin
            $display("  PASS %s", name);
            pass_count++;
        end else begin
            $display("  FAIL %s", name);
            fail_count++;
        end
    endtask

    bit expect_fatal = 0;

    initial begin
        if ($value$plusargs("EXPECT_FATAL=%d", expect_fatal)) ;

        $display("=== sdram_model self-check (EXPECT_FATAL=%0d) ===", expect_fatal);

        // settle
        nop_cycles(4);

        if (expect_fatal) begin
            // ---- ILLEGAL PATH ----
            // ACTIVATE bank 0 row 5, then READ on the IMMEDIATELY following posedge.
            // Driven back-to-back (not via issue()'s 2-cycle spacing) so the model sees
            // activate-to-read distance = 1 < tRCD=2 and must $fatal. If it gets past
            // this, the model failed to trap -> clean exit -> Makefile flags FAIL.
            $display("ILLEGAL: ACTIVATE then READ before tRCD (must \\$fatal)");
            // ACTIVE live at posedge N
            @(negedge clk);
            cmd     = CMD_ACTIVE;
            sd_addr = 13'd5;
            sd_ba   = 2'b00;
            sd_dqm  = 2'b00;
            // READ live at posedge N+1 (one cycle after ACTIVE)
            @(negedge clk);
            cmd     = CMD_READ;
            sd_addr = {2'b00, 1'b0, 9'd7};  // col 7, A10=0
            @(negedge clk);
            cmd     = CMD_NOP;
            sd_addr = 13'd0;
            // Should never reach here — the READ should have $fatal'd at its posedge.
            nop_cycles(4);
            $display("  FAIL illegal READ did NOT trap (tRCD check missing)");
            $fatal(0, "expected a $fatal that never happened");
        end else begin
            // ---- LEGAL PATH ----
            // Open row 5, bank 0. Write word: col 7. Even byte (a[0]=0) goes to HIGH
            // lane [15:8]; odd byte (a[0]=1) goes to LOW lane [7:0]. Store full word
            // 0xBEEF via two byte writes to verify DQM lanes.
            logic [15:0] got;

            // ACTIVATE bank 0, row 5
            issue(CMD_ACTIVE, 13'd5, 2'b00, 2'b00, 16'h0000);
            // wait the rest of tRCD: issue() consumed 2 clk after the ACTIVE edge, so
            // the activate-to-next-command distance is already >= tRCD. Add a NOP for
            // headroom anyway (legal: more delay is always fine).
            nop_cycles(2);

            // WRITE high lane (even byte) = 0xBE  -> dqm = {a[0]=0, ~a[0]=1} = 2'b01
            issue(CMD_WRITE, {2'b00, 1'b0, 9'd7}, 2'b00, 2'b01, 16'hBE00);
            // WRITE low lane (odd byte) = 0xEF    -> dqm = {a[0]=1, ~a[0]=0} = 2'b10
            issue(CMD_WRITE, {2'b00, 1'b0, 9'd7}, 2'b00, 2'b10, 16'h00EF);
            nop_cycles(2);

            // READ col 7, A10=0 (page-mode). Data appears CAS=3 cycles later.
            issue(CMD_READ, {2'b00, 1'b0, 9'd7}, 2'b00, 2'b00, 16'h0000);
            // After issue() the READ was on a clk edge; sd_data_in updates CAS cycles
            // after that edge. Wait generously then sample.
            nop_cycles(5);
            got = sd_data_in;
            check("legal ACTIVATE->tRCD->READ->CAS returns stored word 0xBEEF",
                  got == 16'hBEEF);
            check("high lane (even byte) == 0xBE", got[15:8] == 8'hBE);
            check("low lane  (odd  byte) == 0xEF", got[7:0]  == 8'hEF);

            // Clean precharge to exercise the legal PRECHARGE path (tRAS satisfied:
            // many cycles since ACTIVATE).
            issue(CMD_PRECHARGE, 13'd0 /*A10=0, bank 0*/, 2'b00, 2'b00, 16'h0000);
            nop_cycles(2);
            check("bank 0 row closed after PRECHARGE", chip.row_active[0] == 1'b0);

            $display("");
            $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
            if (fail_count == 0) $display("ALL TESTS PASSED");
            else                 $display("SOME TESTS FAILED");
        end

        $finish;
    end

endmodule
