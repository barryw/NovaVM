// xram_sdram wrapper unit test — drives a byte-level write then read
// through xram_sdram.sv into sdram.v against a behavioural MT48LC16M16
// mock. Exit gate for the top.sv integration task: if the wrapper's
// handshake + timing are wrong, the BRAM-stub replacement will fail
// silently on hardware.

`timescale 1ns/1ps

module test_xram_sdram;

    // Pixel clock (25 MHz) — the wrapper's domain
    logic clk_pixel = 0;
    always #20 clk_pixel = ~clk_pixel;

    // SDRAM clock (100 MHz) — controller state machine
    logic clk_sdram = 0;
    always #5 clk_sdram = ~clk_sdram;

    // clkref (6.25 MHz) — 16:1 SDRAM:clkref ratio like hardware
    logic [3:0] clkref_div = 0;
    logic       clkref     = 0;
    always @(posedge clk_sdram) begin
        clkref_div <= clkref_div + 1'b1;
        if (clkref_div == 4'd7) begin
            clkref     <= ~clkref;
            clkref_div <= 0;
        end
    end

    logic rst;

    // -----------------------------------------------------------------
    // DUT: xram_sdram wrapper
    // -----------------------------------------------------------------
    logic [18:0] req_addr;
    logic [7:0]  req_din;
    logic        req_we;
    logic        req_re;
    wire  [7:0]  req_dout;
    wire         busy;

    wire [24:0] sdram_addr_a;
    wire [7:0]  sdram_din_a;
    wire        sdram_we_a;
    wire        sdram_oe_a;
    wire [7:0]  sdram_dout_a;
    wire        sdram_done_a;

    xram_sdram dut (
        .clk(clk_pixel), .sdram_clk(clk_sdram), .rst(rst),
        .req_addr(req_addr), .req_din(req_din),
        .req_we(req_we), .req_re(req_re),
        .req_dout(req_dout), .busy(busy),
        .sdram_addr(sdram_addr_a), .sdram_din(sdram_din_a),
        .sdram_we(sdram_we_a), .sdram_oe(sdram_oe_a),
        .sdram_dout(sdram_dout_a), .sdram_done(sdram_done_a)
    );

    // -----------------------------------------------------------------
    // SDRAM controller (driven at clk_sdram, clkref from divider)
    // -----------------------------------------------------------------
    wire [15:0] sd_data_ctrl_out;
    wire [15:0] sd_data_chip_out;
    wire [12:0] sd_addr;
    wire [1:0]  sd_dqm;
    wire [1:0]  sd_ba;
    wire        sd_cs, sd_we, sd_ras, sd_cas;
    wire        we_out;
    wire [15:0] sd_data_bus = we_out ? sd_data_ctrl_out : sd_data_chip_out;

    sdram sdram_inst (
        .sd_data_in (sd_data_bus),
        .sd_data_out(sd_data_ctrl_out),
        .sd_addr    (sd_addr),
        .sd_dqm     (sd_dqm),
        .sd_ba      (sd_ba),
        .sd_cs      (sd_cs),
        .sd_we      (sd_we),
        .sd_ras     (sd_ras),
        .sd_cas     (sd_cas),
        .init   (rst),
        .clk    (clk_sdram),
        .clkref (clkref),
        .we_out (we_out),
        // Port A driven by xram_sdram wrapper
        .addrA(sdram_addr_a), .weA(sdram_we_a), .dinA(sdram_din_a),
        .oeA(sdram_oe_a), .doutA(sdram_dout_a), .doneA(sdram_done_a),
        // Port B idle
        .addrB(25'd0), .weB(1'b0), .dinB(8'd0), .oeB(1'b0), .doutB(), .doneB()
    );

    // -----------------------------------------------------------------
    // Behavioural MT48LC16M16 model (same pattern as test_sdram_loopback)
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

    logic [15:0] rd_pipe  [0:3];
    logic        rd_valid [0:3];
    logic [15:0] chip_out_r = 16'h0000;

    always @(posedge clk_sdram) begin
        rd_valid[0] <= 1'b0;

        if (cmd == CMD_ACTIVE) begin
            open_row[sd_ba] <= sd_addr;
        end
        if (cmd == CMD_WRITE) begin
            logic [15:0] widx;
            widx = word_index(sd_ba, open_row[sd_ba], sd_addr[8:0]);
            if (!sd_dqm[1]) sdmem[widx][15:8] <= sd_data_ctrl_out[15:8];
            if (!sd_dqm[0]) sdmem[widx][ 7:0] <= sd_data_ctrl_out[ 7:0];
        end
        if (cmd == CMD_READ) begin
            logic [15:0] widx;
            widx = word_index(sd_ba, open_row[sd_ba], sd_addr[8:0]);
            rd_pipe[0]  <= sdmem[widx];
            rd_valid[0] <= 1'b1;
        end

        rd_pipe[1]  <= rd_pipe[0];  rd_valid[1] <= rd_valid[0];
        rd_pipe[2]  <= rd_pipe[1];  rd_valid[2] <= rd_valid[1];
        rd_pipe[3]  <= rd_pipe[2];  rd_valid[3] <= rd_valid[2];

        if (rd_valid[2]) chip_out_r <= rd_pipe[2];
    end

    assign sd_data_chip_out = chip_out_r;

    // -----------------------------------------------------------------
    // Test harness
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

    task automatic wait_idle();
        while (busy) @(posedge clk_pixel);
    endtask

    task automatic xram_write(input logic [18:0] addr, input logic [7:0] data);
        wait_idle();
        @(posedge clk_pixel);
        req_addr <= addr;
        req_din  <= data;
        req_we   <= 1'b1;
        req_re   <= 1'b0;
        @(posedge clk_pixel);
        req_we   <= 1'b0;
        wait_idle();
    endtask

    task automatic xram_read(input logic [18:0] addr, output logic [7:0] data);
        wait_idle();
        @(posedge clk_pixel);
        req_addr <= addr;
        req_re   <= 1'b1;
        req_we   <= 1'b0;
        @(posedge clk_pixel);
        req_re   <= 1'b0;
        wait_idle();
        data = req_dout;
    endtask

    initial begin
        logic [7:0] got;

        $display("=== xram_sdram wrapper test ===");
        $display("");

        rst = 1;
        req_addr = 0; req_din = 0; req_we = 0; req_re = 0;

        // Hold reset long enough for sdram init sequence to finish (~200us
        // in pessimistic sim terms). The controller's reset counter is set
        // to 17'h1f, decrementing on each q-wrap.
        repeat (20) @(posedge clk_pixel);
        rst = 0;
        repeat (500) @(posedge clk_pixel);

        check("wrapper idle after reset", !busy);
        check("controller out of reset", sdram_inst.reset == 0);

        // ----- Byte write/read roundtrip
        $display("Test: write then read byte 0xAA at addr $00000");
        xram_write(19'h00000, 8'hAA);
        xram_read (19'h00000, got);
        check("read-back 0xAA", got == 8'hAA);

        $display("Test: different address");
        xram_write(19'h00001, 8'h55);
        xram_read (19'h00001, got);
        check("read-back 0x55 from addr 1", got == 8'h55);
        xram_read (19'h00000, got);
        check("addr 0 still 0xAA", got == 8'hAA);

        $display("Test: 256-byte burst");
        for (int i = 0; i < 16; i++) xram_write(19'h00100 + i, 8'hC0 + i);
        for (int i = 0; i < 16; i++) begin
            xram_read(19'h00100 + i, got);
            check($sformatf("burst[%0d] = 0x%02X", i, 8'hC0 + i), got == 8'hC0 + i);
        end

        $display("Test: large-address access (64KB+ offset)");
        xram_write(19'h10000, 8'h42);  // 64 KB into XRAM
        xram_read (19'h10000, got);
        check("read-back at 64KB offset", got == 8'h42);
        xram_write(19'h40000, 8'h99);  // 256 KB into XRAM
        xram_read (19'h40000, got);
        check("read-back at 256KB offset", got == 8'h99);

        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
