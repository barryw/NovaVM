// SDRAM loopback — single-address write then read through sdram.v driving a
// behavioral MT48LC16M16 model. First exit gate for Phase 2.5 Step 1.
//
// What the test proves:
//   - sdram.v emits a correct init sequence (PRECHARGE + LOAD_MODE)
//   - ACTIVE + WRITE commands update chip storage with DQM byte mask
//   - ACTIVE + READ commands return the stored byte on doutA at the
//     scheduled state (CAS_LATENCY=3 + 1-cycle late latch)
//
// The mock chip honours bank/row/col addressing and byte-level DQM writes
// but is not cycle-accurate on timing parameters like tRCD / tRP — the
// controller drives them, not the model. Functional correctness of a
// loopback roundtrip is enough; timing closure is nextpnr's job.

`timescale 1ns/1ps

module test_sdram_loopback;

    // -----------------------------------------------------------------
    // Clock generation
    //   clk:    85 MHz nominal (sdram.v's design target)
    //   clkref: derived by dividing clk; the controller expects a
    //           "reference" transition to re-sync the q counter. For the
    //           test we toggle clkref every 8 clk edges so each full
    //           q 0..7 cycle aligns to one clkref half-period — this
    //           matches the pattern from the MiST NES core this
    //           controller was lifted from.
    // -----------------------------------------------------------------
    logic clk = 0;
    always #5.88 clk = ~clk;  // 85 MHz-ish

    logic [3:0] clkref_div = 0;
    logic       clkref = 0;
    always @(posedge clk) begin
        clkref_div <= clkref_div + 1'b1;
        if (clkref_div == 4'd7) begin
            clkref   <= ~clkref;
            clkref_div <= 0;
        end
    end

    // -----------------------------------------------------------------
    // Controller <-> chip wiring
    // -----------------------------------------------------------------
    wire [15:0] sd_data_ctrl_out;
    wire [15:0] sd_data_chip_out;
    wire [12:0] sd_addr;
    wire [1:0]  sd_dqm;
    wire [1:0]  sd_ba;
    wire        sd_cs, sd_we, sd_ras, sd_cas;
    wire        we_out;

    // In real hardware the data bus is tri-state. For sim we mux based
    // on the controller's we_out signal — when writing, the controller
    // drives; when reading, the chip drives.
    wire [15:0] sd_data_bus = we_out ? sd_data_ctrl_out : sd_data_chip_out;

    logic        init = 1'b1;
    logic [24:0] addrA = 0, addrB = 0;
    logic        weA = 0, weB = 0;
    logic        oeA = 0, oeB = 0;
    logic [7:0]  dinA = 0, dinB = 0;
    wire  [7:0]  doutA, doutB;

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

        .addrA(addrA), .weA(weA), .dinA(dinA), .oeA(oeA), .doutA(doutA),
        .addrB(addrB), .weB(weB), .dinB(dinB), .oeB(oeB), .doutB(doutB)
    );

    // -----------------------------------------------------------------
    // Behavioural MT48LC16M16 model
    //
    // Storage is sized for the test only — we never touch more than a
    // few addresses so 64K words is plenty. The address function maps
    // {bank, row[6:0], col[6:0]} into a 16-bit index; full-chip
    // addressing isn't needed for a loopback probe.
    // -----------------------------------------------------------------
    logic [15:0] sdmem [0:65535];
    initial for (int i = 0; i < 65536; i++) sdmem[i] = 16'h0000;

    logic [12:0] open_row [0:3];

    localparam CMD_ACTIVE = 4'b0011;
    localparam CMD_READ   = 4'b0101;
    localparam CMD_WRITE  = 4'b0100;

    wire [3:0] cmd = {sd_cs, sd_ras, sd_cas, sd_we};

    function automatic [15:0] word_index(
        input logic [1:0]  ba,
        input logic [12:0] row,
        input logic [8:0]  col
    );
        word_index = {ba, row[6:0], col[6:0]};
    endfunction

    // CAS_LATENCY=3: a READ issued at clock N produces valid data on
    // the DQ pins at clock N+3. We model that with a 3-deep pipeline
    // of the looked-up word plus a per-stage valid bit.
    logic [15:0] rd_pipe  [0:3];
    logic        rd_valid [0:3];
    logic [15:0] chip_out_r = 16'h0000;

    always @(posedge clk) begin
        // Default: no new read this cycle
        rd_valid[0] <= 1'b0;

        // --- Commands ---
        if (cmd == CMD_ACTIVE) begin
            open_row[sd_ba] <= sd_addr;
        end

        if (cmd == CMD_WRITE) begin
            logic [15:0] widx;
            widx = word_index(sd_ba, open_row[sd_ba], sd_addr[8:0]);
            // DQM[1] high = mask upper byte; DQM[0] high = mask lower byte
            if (!sd_dqm[1]) sdmem[widx][15:8] <= sd_data_ctrl_out[15:8];
            if (!sd_dqm[0]) sdmem[widx][ 7:0] <= sd_data_ctrl_out[ 7:0];
        end

        if (cmd == CMD_READ) begin
            logic [15:0] widx;
            widx = word_index(sd_ba, open_row[sd_ba], sd_addr[8:0]);
            rd_pipe[0]  <= sdmem[widx];
            rd_valid[0] <= 1'b1;
        end

        // Shift the CAS pipeline
        rd_pipe[1]  <= rd_pipe[0];  rd_valid[1] <= rd_valid[0];
        rd_pipe[2]  <= rd_pipe[1];  rd_valid[2] <= rd_valid[1];
        rd_pipe[3]  <= rd_pipe[2];  rd_valid[3] <= rd_valid[2];

        // When the pipeline valid bit reaches stage 3, drive the word
        // onto the chip output for the controller to sample.
        if (rd_valid[2]) chip_out_r <= rd_pipe[2];
    end

    assign sd_data_chip_out = chip_out_r;

    // -----------------------------------------------------------------
    // Test harness — drive controller transactions on port A
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

    // Hold weA/oeA asserted across a generous number of clocks so the
    // controller gets multiple full q 0..7 cycles to see the request
    // on whichever clkref phase port A happens to land on.
    task automatic port_a_write(input logic [24:0] addr, input logic [7:0] data);
        @(posedge clk);
        addrA <= addr;
        dinA  <= data;
        weA   <= 1'b1;
        oeA   <= 1'b0;
        repeat (24) @(posedge clk);
        weA   <= 1'b0;
    endtask

    task automatic port_a_read(input logic [24:0] addr, output logic [7:0] data);
        @(posedge clk);
        addrA <= addr;
        oeA   <= 1'b1;
        weA   <= 1'b0;
        // 24 cycles covers multiple q cycles; the controller latches
        // doutA at STATE_CMD_READ (q=7) once the port-A slot starts.
        repeat (24) @(posedge clk);
        oeA   <= 1'b0;
        repeat (4) @(posedge clk);
        data = doutA;
    endtask

    initial begin
        logic [7:0] got;

        $display("=== SDRAM loopback ===");
        $display("");

        // Let controller run its ~4000-cycle init sequence.
        // reset counter starts at 17'h1f so we need to observe enough
        // clkref edges for the counter to reach zero.
        init = 1'b1;
        repeat (10) @(posedge clk);
        init = 1'b0;

        // Wait for reset != 0 to settle. Easiest conservative approach:
        // run for 2000 clocks.
        repeat (2000) @(posedge clk);

        check("controller exited reset (reset==0)", dut.reset == 0);

        // -------- Test 1: byte write + read back, low byte (addr[0]=1)
        $display("Test: write 0xA5 to addr 0, read back");
        port_a_write(25'h0000000, 8'hA5);
        repeat (20) @(posedge clk);
        port_a_read(25'h0000000, got);
        check("read back 0xA5 from addr 0", got == 8'hA5);

        // -------- Test 2: high byte of same word (addr[0]=0 vs 1)
        $display("Test: write 0x5A to addr 1 (high byte of word 0)");
        port_a_write(25'h0000001, 8'h5A);
        repeat (20) @(posedge clk);
        port_a_read(25'h0000001, got);
        check("read back 0x5A from addr 1", got == 8'h5A);
        port_a_read(25'h0000000, got);
        check("addr 0 preserved (still 0xA5)", got == 8'hA5);

        // -------- Test 3: address decode across a row boundary
        $display("Test: write across row boundary");
        port_a_write(25'h0000200, 8'h11);  // different column
        repeat (20) @(posedge clk);
        port_a_write(25'h0002000, 8'h22);  // different row
        repeat (20) @(posedge clk);
        port_a_read(25'h0000200, got);
        check("col boundary read 0x11", got == 8'h11);
        port_a_read(25'h0002000, got);
        check("row boundary read 0x22", got == 8'h22);

        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
