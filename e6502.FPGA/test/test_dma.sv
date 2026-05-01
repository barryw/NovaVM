// DMA unit tests — Verilator testbench
// Tests 1D fill/copy, error cases, RDY stall, write-protect,
// cross-space transfers.

`timescale 1ns/1ps

module test_dma;

    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    logic        rst;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    logic        cpu_we;
    logic        cpu_re;

    // DMA outputs
    wire  [7:0]  dma_cpu_rdata;
    wire         dma_rdy;

    // RAM port
    wire  [15:0] dma_ram_addr;
    wire  [7:0]  dma_ram_wdata;
    wire         dma_ram_we;

    // XRAM port
    wire  [18:0] dma_xram_addr;
    wire  [7:0]  dma_xram_wdata;
    wire         dma_xram_we;

    // VGC memory port
    wire  [2:0]  dma_vgc_space;
    wire  [16:0] dma_vgc_addr;
    wire  [7:0]  dma_vgc_wdata;
    wire         dma_vgc_we;
    wire         dma_vgc_re;

    // Simulated memories
    logic [7:0] sim_ram [0:65535];
    logic [7:0] sim_xram [0:524287];
    logic [7:0] sim_char [0:3999];
    logic [7:0] sim_color [0:3999];
    logic [3:0] sim_gfx [0:63999];
    logic [7:0] sim_sprite [0:32767];

    // Synchronous 1-cycle RAM/XRAM reads — matches real dpram behavior in
    // synthesis (address registered at posedge, data valid next cycle).
    // If test harnesses use combinational reads, sim will pass but real
    // hardware reads stale data. Caught 2026-04-23 on DMACOPY CPU→CPU
    // hardware regression.
    logic [7:0] dma_ram_rdata, dma_xram_rdata;
    always_ff @(posedge clk) begin
        dma_ram_rdata  <= sim_ram[dma_ram_addr];
        dma_xram_rdata <= sim_xram[dma_xram_addr];
    end

    // VGC memory read mux — also registered one cycle to match dpram.
    logic [7:0] dma_vgc_rdata;
    always_ff @(posedge clk) begin
        case (dma_vgc_space)
            3'd1: dma_vgc_rdata <= sim_char[dma_vgc_addr[11:0]];
            3'd2: dma_vgc_rdata <= sim_color[dma_vgc_addr[11:0]];
            3'd3: dma_vgc_rdata <= {4'b0, sim_gfx[dma_vgc_addr[15:0]]};
            3'd4: dma_vgc_rdata <= sim_sprite[dma_vgc_addr[14:0]];
            default: dma_vgc_rdata <= 8'h00;
        endcase
    end

    // VGC memory write handler
    always_ff @(posedge clk) begin
        if (dma_vgc_we) begin
            case (dma_vgc_space)
                3'd1: sim_char[dma_vgc_addr[11:0]]    <= dma_vgc_wdata;
                3'd2: sim_color[dma_vgc_addr[11:0]]   <= dma_vgc_wdata;
                3'd3: sim_gfx[dma_vgc_addr[15:0]]     <= dma_vgc_wdata[3:0];
                3'd4: sim_sprite[dma_vgc_addr[14:0]]   <= dma_vgc_wdata;
                default: ;
            endcase
        end
    end

    always_ff @(posedge clk) begin
        if (dma_ram_we)  sim_ram[dma_ram_addr]   <= dma_ram_wdata;
        if (dma_xram_we) sim_xram[dma_xram_addr] <= dma_xram_wdata;
    end

    dma dut (
        .clk(clk), .rst(rst),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
        .cpu_we(cpu_we), .cpu_rdata(dma_cpu_rdata), .cpu_re(cpu_re),
        .rdy_out(dma_rdy),
        .ram_addr(dma_ram_addr), .ram_rdata(dma_ram_rdata),
        .ram_wdata(dma_ram_wdata), .ram_we(dma_ram_we),
        .xram_addr(dma_xram_addr), .xram_rdata(dma_xram_rdata),
        .xram_wdata(dma_xram_wdata), .xram_we(dma_xram_we),
        .vgc_space(dma_vgc_space), .vgc_addr(dma_vgc_addr),
        .vgc_rdata(dma_vgc_rdata), .vgc_wdata(dma_vgc_wdata),
        .vgc_we(dma_vgc_we), .vgc_re(dma_vgc_re)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    logic all_regs_zero;

    localparam DMA = 16'hBA63;

    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr; cpu_wdata <= data; cpu_we <= 1;
        @(posedge clk);
        cpu_we <= 0;
    endtask

    task automatic dma_reg(input int off, input logic [7:0] data);
        write_reg(DMA + off, data);
    endtask

    task automatic dma_start();
        dma_reg(0, 8'h01);
    endtask

    task automatic wait_dma_done();
        repeat(2) @(posedge clk);
        // S_IDLE=0, S_DONE=5 after S_READ_WAIT insertion (commit 844481c~)
        while (dut.state != 0 && dut.state != 5) @(posedge clk);
        repeat(2) @(posedge clk);
    endtask

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

    task automatic setup_fill(
        input int dst_space, input int dst_addr, input int len, input int fill_val
    );
        dma_reg(4, dst_space);
        dma_reg(8, dst_addr[7:0]);
        dma_reg(9, dst_addr[15:8]);
        dma_reg(10, dst_addr[23:16]);
        dma_reg(11, len[7:0]);
        dma_reg(12, len[15:8]);
        dma_reg(13, len[23:16]);
        dma_reg(14, 8'h01);
        dma_reg(15, fill_val);
    endtask

    task automatic setup_copy(
        input int src_space, input int src_addr,
        input int dst_space, input int dst_addr,
        input int len
    );
        dma_reg(3, src_space);
        dma_reg(4, dst_space);
        dma_reg(5, src_addr[7:0]);
        dma_reg(6, src_addr[15:8]);
        dma_reg(7, src_addr[23:16]);
        dma_reg(8, dst_addr[7:0]);
        dma_reg(9, dst_addr[15:8]);
        dma_reg(10, dst_addr[23:16]);
        dma_reg(11, len[7:0]);
        dma_reg(12, len[15:8]);
        dma_reg(13, len[23:16]);
        dma_reg(14, 8'h00);
    endtask

    initial begin
        $display("=== DMA Unit Tests ===");
        $display("");

        rst = 1; cpu_we = 0; cpu_re = 0;
        cpu_addr = 0; cpu_wdata = 0;

        for (int i = 0; i < 65536; i++) sim_ram[i] = 0;
        for (int i = 0; i < 524288; i++) sim_xram[i] = 0;
        for (int i = 0; i < 4000; i++) begin sim_char[i] = 0; sim_color[i] = 0; end
        for (int i = 0; i < 64000; i++) sim_gfx[i] = 0;
        for (int i = 0; i < 32768; i++) sim_sprite[i] = 0;

        repeat(50) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        // ----- Test: Initial state -----
        $display("Test: Initial state");
        check("status is idle (0)", dut.regs[1] == 0);
        check("RDY is high", dma_rdy == 1);

        // ----- Test: Reset clears programmer-visible state -----
        $display("Test: Reset clears registers");
        dma_reg(3, 8'h05);
        dma_reg(4, 8'h03);
        dma_reg(5, 8'h34);
        dma_reg(6, 8'h12);
        dma_reg(14, 8'h01);
        dma_reg(15, 8'hAA);
        rst = 1;
        repeat(2) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);
        all_regs_zero = 1;
        for (int i = 0; i < 19; i++)
            all_regs_zero &= (dut.regs[i] == 8'h00);
        check("all DMA regs reset to zero/idle", all_regs_zero);
        check("DMA state reset idle", dut.state == 0);
        check("DMA RDY after reset", dma_rdy == 1);

        // ----- Test: DMAFILL CPU RAM 256 bytes -----
        $display("Test: Fill CPU RAM 256 bytes @ 0xAA");
        setup_fill(0, 16'h2000, 256, 8'hAA);
        dma_start();
        wait_dma_done();

        check("status ok", dut.regs[1] == 8'h02);
        check("no error", dut.regs[2] == 0);
        check("count = 256", {dut.regs[18], dut.regs[17], dut.regs[16]} == 24'd256);
        check("ram[$2000] = AA", sim_ram[16'h2000] == 8'hAA);
        check("ram[$2080] = AA", sim_ram[16'h2080] == 8'hAA);
        check("ram[$20FF] = AA", sim_ram[16'h20FF] == 8'hAA);
        check("ram[$2100] = 00 (beyond end)", sim_ram[16'h2100] == 8'h00);

        // ----- Test: DMAFILL then clear with zero -----
        $display("Test: Fill then clear");
        setup_fill(0, 16'h3000, 128, 8'hFF);
        dma_start();
        wait_dma_done();
        setup_fill(0, 16'h3000, 128, 8'h00);
        dma_start();
        wait_dma_done();

        check("cleared byte 0", sim_ram[16'h3000] == 8'h00);
        check("cleared byte 127", sim_ram[16'h307F] == 8'h00);

        // ----- Test: DMACOPY CPU→CPU 8 bytes (the failing case from handoff) -----
        $display("Test: Copy CPU→CPU 8 bytes");
        for (int i = 0; i < 8; i++) sim_ram[16'h4000 + i] = i + 10;
        for (int i = 0; i < 8; i++) sim_ram[16'h5000 + i] = 8'hAA;  // pre-fill dest

        setup_copy(0, 16'h4000, 0, 16'h5000, 8);
        dma_start();
        wait_dma_done();

        check("copy status ok", dut.regs[1] == 8'h02);
        check("count = 8", {dut.regs[18], dut.regs[17], dut.regs[16]} == 24'd8);
        check("dst[0] = 10", sim_ram[16'h5000] == 8'd10);
        check("dst[3] = 13", sim_ram[16'h5003] == 8'd13);
        check("dst[7] = 17", sim_ram[16'h5007] == 8'd17);
        check("src[0] intact", sim_ram[16'h4000] == 8'd10);

        // ----- Test: DMACOPY preserves source -----
        $display("Test: Copy preserves source");
        for (int i = 0; i < 4; i++) sim_ram[16'h6000 + i] = i + 100;
        setup_copy(0, 16'h6000, 0, 16'h7000, 4);
        dma_start();
        wait_dma_done();

        check("source byte 0 intact", sim_ram[16'h6000] == 8'd100);
        check("source byte 3 intact", sim_ram[16'h6003] == 8'd103);
        check("dest byte 0 copied", sim_ram[16'h7000] == 8'd100);

        // ----- Test: DMAFILL then DMACOPY round-trip -----
        $display("Test: Fill then copy round-trip");
        setup_fill(0, 16'h8000, 64, 8'hCC);
        dma_start();
        wait_dma_done();
        setup_copy(0, 16'h8000, 0, 16'h9000, 64);
        dma_start();
        wait_dma_done();

        check("round-trip copy ok", dut.regs[1] == 8'h02);
        check("copied byte 0 = CC", sim_ram[16'h9000] == 8'hCC);
        check("copied byte 63 = CC", sim_ram[16'h903F] == 8'hCC);

        // ----- Test: DMACOPY larger 1KB -----
        $display("Test: Copy 256-byte block");
        for (int i = 0; i < 256; i++) sim_ram[16'h2000 + i] = i[7:0];
        setup_copy(0, 16'h2000, 0, 16'h3000, 256);
        dma_start();
        wait_dma_done();

        check("256 block status ok", dut.regs[1] == 8'h02);
        check("byte 0 = 0", sim_ram[16'h3000] == 8'd0);
        check("byte 255 = 255", sim_ram[16'h30FF] == 8'd255);

        // ----- Test: DMACOPY CPU→XRAM -----
        $display("Test: Copy CPU→XRAM");
        for (int i = 0; i < 16; i++) sim_ram[16'hA000 + i] = i + 50;
        setup_copy(0, 16'hA000, 5, 24'h010000, 16);
        dma_start();
        wait_dma_done();

        check("cpu→xram ok", dut.regs[1] == 8'h02);
        check("xram[$10000] = 50", sim_xram[19'h10000] == 8'd50);
        check("xram[$1000F] = 65", sim_xram[19'h1000F] == 8'd65);

        // ----- Test: DMAFILL VGC char RAM -----
        $display("Test: Fill VGC char RAM");
        setup_fill(1, 100, 20, 8'h41);  // 'A'
        dma_start();
        wait_dma_done();

        check("char fill ok", dut.regs[1] == 8'h02);
        check("char[100] = A", sim_char[100] == 8'h41);
        check("char[119] = A", sim_char[119] == 8'h41);
        check("char[120] = 0 (beyond end)", sim_char[120] == 8'h00);

        // ----- Test: DMACOPY CPU→VGC gfx -----
        $display("Test: Copy CPU→VGC gfx");
        for (int i = 0; i < 10; i++) sim_ram[16'hB000 + i] = 4'(i + 1);
        setup_copy(0, 16'hB000, 3, 500, 10);
        dma_start();
        wait_dma_done();

        check("gfx copy ok", dut.regs[1] == 8'h02);
        check("gfx[500] = 1", sim_gfx[500] == 4'd1);
        check("gfx[509] = 10", sim_gfx[509] == 4'hA);

        // ----- Test: RDY stall during DMA -----
        $display("Test: RDY stall");
        setup_fill(0, 16'h1000, 200, 8'h55);  // sits below ROM
        dma_start();
        repeat(4) @(posedge clk);              // let validate → write begin
        check("RDY low during DMA", dma_rdy == 0);
        wait_dma_done();
        check("RDY restored", dma_rdy == 1);

        // ----- Test: Error — bad command -----
        $display("Test: Error bad command");
        dma_reg(0, 8'hFF);
        repeat(5) @(posedge clk);
        check("status = error", dut.regs[1] == 8'h03);
        check("errcode = badcmd", dut.regs[2] == 8'h01);

        // ----- Test: Error — zero length -----
        $display("Test: Error zero length");
        setup_fill(0, 16'h2000, 0, 8'h00);
        dma_start();
        repeat(10) @(posedge clk);
        check("zero-len status = error", dut.regs[1] == 8'h03);
        check("zero-len errcode = badargs", dut.regs[2] == 8'h04);

        // ----- Test: Error — text attribute range overflow -----
        $display("Test: Error text attribute range overflow");
        setup_fill(7, 16'h2000, 10, 8'h00);  // text attr space is 4000 bytes
        dma_start();
        repeat(10) @(posedge clk);
        check("text-attr-range status = error", dut.regs[1] == 8'h03);
        check("text-attr-range errcode = range", dut.regs[2] == 8'h03);

        // ----- Test: Error — range overflow -----
        $display("Test: Error range overflow");
        setup_fill(0, 16'hFFF0, 32, 8'h00);  // would cross 64KB boundary
        dma_start();
        repeat(10) @(posedge clk);
        check("range-overflow status = error", dut.regs[1] == 8'h03);
        check("range-overflow errcode = range", dut.regs[2] == 8'h03);

        // ----- Test: Error — write protect (ROM) -----
        $display("Test: Error write protect");
        setup_fill(0, 16'hBFF0, 32, 8'h00);  // would overwrite ROM at $C000
        dma_start();
        repeat(10) @(posedge clk);
        check("wp status = error", dut.regs[1] == 8'h03);
        check("wp errcode = writeprot", dut.regs[2] == 8'h05);

        // ----- Summary -----
        $display("");
        $display("=== Results: %0d passed, %0d failed, %0d total ===",
                 pass_count, fail_count, test_num);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                  $display("FAILURES DETECTED");
        $finish;
    end

endmodule
