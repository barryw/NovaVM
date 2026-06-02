// test_sdram_model_vs_ctrl.sv — ground-truth latency check for sdram_model.svh.
//
// Wires the REAL, already-proven sdram.v controller to the behavioral
// sdram_model.svh (the timing oracle). The controller captures read data at a
// fixed state (q==STATE_CMD_READ, exactly CAS_LATENCY=3 cycles after it issues
// the READ at q==STATE_CMD_CONT). If the model presents read data on the cycle
// the controller expects, doutA returns the byte that was written; if the
// model's CAS latency is off by even one cycle, the controller latches stale
// (zero) data and the assertions fail.
//
// test_sdram_loopback already proves sdram.v is correct against a hand-rolled
// model with the right latency. This test reuses sdram.v as the fixed reference
// and puts sdram_model.svh on the other end, so a PASS here is direct proof the
// model's read-data timing matches real silicon.
//
// Several distinct byte addresses are exercised — even (a[0]=0 -> HIGH lane)
// and odd (a[0]=1 -> LOW lane) — to re-prove byte-lane decode at the same time.
//
// Run: make -C test test_sdram_model_vs_ctrl

`timescale 1ns/1ps

`include "sdram_model.svh"

module test_sdram_model_vs_ctrl;

    // -----------------------------------------------------------------
    // Clocking — identical scheme to test_sdram_loopback:
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
        .addrB(addrB), .weB(weB), .dinB(dinB), .oeB(oeB), .doutB(doutB), .doneB(doneB)
    );

    // The timing oracle under test. Its sd_data_in is the controller-input
    // port — i.e. the chip's *output* to the controller (read data). The
    // controller drives the model's sd_data_out (write data).
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
    // Harness — drive single-access write/read transactions on port A.
    // The request bridges in sdram.v hold weA/oeA/addr/din until done, so
    // we keep each request asserted across multiple full q 0..7 cycles.
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

    task automatic port_a_read(input logic [24:0] addr, output logic [7:0] data);
        @(posedge clk);
        addrA <= addr;
        oeA   <= 1'b1;
        weA   <= 1'b0;
        repeat (24) @(posedge clk);
        oeA   <= 1'b0;
        repeat (4) @(posedge clk);
        data = doutA;
    endtask

    initial begin
        logic [7:0] got;

        $display("=== SDRAM model vs. real controller (CAS-latency ground truth) ===");
        $display("");

        // Run sdram.v through its init sequence (PRECHARGE + LOAD_MODE).
        init = 1'b1;
        repeat (10) @(posedge clk);
        init = 1'b0;
        repeat (2000) @(posedge clk);
        check("controller exited reset (reset==0)", dut.reset == 0);

        // -------- even byte (a[0]=0 -> HIGH lane) --------
        $display("Test: write 0x3C to even addr 0 (HIGH lane), read back");
        port_a_write(25'h0000000, 8'h3C);
        repeat (20) @(posedge clk);
        port_a_read(25'h0000000, got);
        check("even addr 0 reads back 0x3C (HIGH lane)", got == 8'h3C);

        // -------- odd byte (a[0]=1 -> LOW lane), same word --------
        $display("Test: write 0xD9 to odd addr 1 (LOW lane), read back");
        port_a_write(25'h0000001, 8'hD9);
        repeat (20) @(posedge clk);
        port_a_read(25'h0000001, got);
        check("odd addr 1 reads back 0xD9 (LOW lane)", got == 8'hD9);
        // The even byte of the same word must be untouched by the odd write.
        port_a_read(25'h0000000, got);
        check("even addr 0 still 0x3C after odd-byte write", got == 8'h3C);

        // -------- different column, same row --------
        $display("Test: write 0x7E to addr 0x40 (different column)");
        port_a_write(25'h0000040, 8'h7E);
        repeat (20) @(posedge clk);
        port_a_read(25'h0000040, got);
        check("col-different addr 0x40 reads back 0x7E", got == 8'h7E);

        // -------- different row --------
        $display("Test: write 0xC5 to addr 0x2000 (different row)");
        port_a_write(25'h0002000, 8'hC5);
        repeat (20) @(posedge clk);
        port_a_read(25'h0002000, got);
        check("row-different addr 0x2000 reads back 0xC5", got == 8'hC5);

        // -------- different bank --------
        $display("Test: write 0x18 to addr 0x400000 (different bank)");
        port_a_write(25'h0400000, 8'h18);
        repeat (20) @(posedge clk);
        port_a_read(25'h0400000, got);
        check("bank-different addr 0x400000 reads back 0x18", got == 8'h18);

        // -------- re-read earlier addresses to confirm no corruption --------
        $display("Test: re-read earlier addresses for persistence");
        port_a_read(25'h0000040, got);
        check("addr 0x40 still 0x7E", got == 8'h7E);
        port_a_read(25'h0002000, got);
        check("addr 0x2000 still 0xC5", got == 8'hC5);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
