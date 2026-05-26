// Blitter unit tests — iverilog testbench
// Tests fill, copy, color-key, strides, status/error, cross-space, RDY stall

`timescale 1ns/1ps

module test_blitter;

    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    logic        rst;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    logic        cpu_we;
    logic        cpu_re;

    // Blitter outputs
    wire  [7:0]  blt_cpu_rdata;
    wire         blt_rdy;

    // RAM port
    wire  [15:0] blt_ram_addr;
    wire  [7:0]  blt_ram_wdata;
    wire         blt_ram_we;

    // XRAM port
    wire  [18:0] blt_xram_addr;
    wire  [7:0]  blt_xram_wdata;
    wire         blt_xram_we;
    wire         blt_xram_re;
    logic        xram_busy;

    // VGC memory port
    wire  [2:0]  blt_vgc_space;
    wire  [16:0] blt_vgc_addr;
    wire  [7:0]  blt_vgc_wdata;
    wire         blt_vgc_we;
    wire         blt_vgc_re;
    logic        video_write_safe;

    // Simulated memories
    logic [7:0] sim_ram [0:65535];
    logic [7:0] sim_xram [0:524287];
    // VGC memory spaces (simplified for test)
    logic [7:0] sim_char [0:3999];
    logic [7:0] sim_color [0:3999];
    logic [3:0] sim_gfx [0:63999];
    logic [7:0] sim_sprite [0:32767];

    // Synchronous 1-cycle RAM/XRAM reads — matches real dpram behavior in
    // synthesis (address registered at posedge, data valid next cycle).
    // A combinational `wire rdata = sim_mem[addr]` harness produces false
    // green tests that miss stale-data bugs at read→write transitions.
    // Same pattern as test_dma.sv (commit 2f21297).
    logic [7:0] blt_ram_rdata, blt_xram_rdata;
    always_ff @(posedge clk) begin
        blt_ram_rdata  <= sim_ram[blt_ram_addr];
        blt_xram_rdata <= sim_xram[blt_xram_addr];
    end

    // VGC memory read mux — registered one cycle to match dpram.
    logic [7:0] blt_vgc_rdata;
    always_ff @(posedge clk) begin
        case (blt_vgc_space)
            3'd1: blt_vgc_rdata <= sim_char[blt_vgc_addr[11:0]];
            3'd2: blt_vgc_rdata <= sim_color[blt_vgc_addr[11:0]];
            3'd3: blt_vgc_rdata <= {4'b0, sim_gfx[blt_vgc_addr[15:0]]};
            3'd4: blt_vgc_rdata <= sim_sprite[blt_vgc_addr[14:0]];
            default: blt_vgc_rdata <= 8'h00;
        endcase
    end

    // VGC memory write handler
    always_ff @(posedge clk) begin
        if (blt_vgc_we) begin
            case (blt_vgc_space)
                3'd1: sim_char[blt_vgc_addr[11:0]]    <= blt_vgc_wdata;
                3'd2: sim_color[blt_vgc_addr[11:0]]   <= blt_vgc_wdata;
                3'd3: sim_gfx[blt_vgc_addr[15:0]]     <= blt_vgc_wdata[3:0];
                3'd4: sim_sprite[blt_vgc_addr[14:0]]   <= blt_vgc_wdata;
                default: ;
            endcase
        end
    end

    // RAM write handler
    always_ff @(posedge clk) begin
        if (blt_ram_we)
            sim_ram[blt_ram_addr] <= blt_ram_wdata;
    end

    // XRAM write handler
    always_ff @(posedge clk) begin
        if (blt_xram_we)
            sim_xram[blt_xram_addr] <= blt_xram_wdata;
    end

    blitter dut (
        .clk(clk), .rst(rst),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
        .cpu_we(cpu_we), .cpu_rdata(blt_cpu_rdata), .cpu_re(cpu_re),
        .rdy_out(blt_rdy),
        .ram_addr(blt_ram_addr), .ram_rdata(blt_ram_rdata),
        .ram_wdata(blt_ram_wdata), .ram_we(blt_ram_we),
        .xram_addr(blt_xram_addr), .xram_rdata(blt_xram_rdata),
        .xram_wdata(blt_xram_wdata), .xram_we(blt_xram_we),
        .xram_re(blt_xram_re), .xram_busy(xram_busy),
        .vgc_space(blt_vgc_space), .vgc_addr(blt_vgc_addr),
        .vgc_rdata(blt_vgc_rdata), .vgc_wdata(blt_vgc_wdata),
        .vgc_we(blt_vgc_we), .vgc_re(blt_vgc_re),
        .video_write_safe(video_write_safe)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    logic all_regs_zero;

    // ---------------------------------------------------------------
    // Register address helpers
    // ---------------------------------------------------------------
    localparam BLT = 16'hBA83;
    localparam BLT_ROTANGLE = 16'hBAA2;

    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr; cpu_wdata <= data; cpu_we <= 1;
        @(posedge clk);
        cpu_we <= 0;
    endtask

    task automatic blt_reg(input int off, input logic [7:0] data);
        write_reg(BLT + off, data);
    endtask

    task automatic blt_start();
        blt_reg(0, 8'h01); // BltCmd = start
    endtask

    task automatic wait_blt_done();
        int cycles;
        repeat(2) @(posedge clk); // let state machine start
        // Wait for IDLE (0) or DONE (6) — don't match on intermediate states
        // S_IDLE=0, S_DONE=8 after adding S_READ_WAIT / S_ROWBUF_READ_WAIT
        cycles = 0;
        while (dut.state != 0 && dut.state != 8 && cycles < 100000) begin
            cycles++;
            @(posedge clk);
        end
        if (cycles == 100000) begin
            $display("  TIMEOUT waiting for blitter: state=%0d status=%02x err=%02x row=%0d col=%0d read_valid=%0d video_safe=%0d",
                     dut.state, dut.regs[1], dut.regs[2], dut.row, dut.col,
                     dut.read_valid, video_write_safe);
            fail_count++;
        end
        repeat(2) @(posedge clk); // settle
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

    // Set up a fill blit
    task automatic setup_fill(
        input int dst_space, input int dst_addr, input int dst_stride,
        input int width, input int height, input int fill_val
    );
        blt_reg(4, dst_space);              // DstSpace
        blt_reg(8, dst_addr[7:0]);          // DstL
        blt_reg(9, dst_addr[15:8]);         // DstM
        blt_reg(10, dst_addr[23:16]);       // DstH
        blt_reg(11, width[7:0]);            // WidthL
        blt_reg(12, width[15:8]);           // WidthH
        blt_reg(13, height[7:0]);           // HeightL
        blt_reg(14, height[15:8]);          // HeightH
        blt_reg(17, dst_stride[7:0]);       // DstStrideL
        blt_reg(18, dst_stride[15:8]);      // DstStrideH
        blt_reg(19, 8'h01);                 // Mode = fill
        blt_reg(20, fill_val);              // FillValue
    endtask

    // Set up a copy blit
    task automatic setup_copy(
        input int src_space, input int src_addr, input int src_stride,
        input int dst_space, input int dst_addr, input int dst_stride,
        input int width, input int height
    );
        blt_reg(3, src_space);              // SrcSpace
        blt_reg(4, dst_space);              // DstSpace
        blt_reg(5, src_addr[7:0]);          // SrcL
        blt_reg(6, src_addr[15:8]);         // SrcM
        blt_reg(7, src_addr[23:16]);        // SrcH
        blt_reg(8, dst_addr[7:0]);          // DstL
        blt_reg(9, dst_addr[15:8]);         // DstM
        blt_reg(10, dst_addr[23:16]);       // DstH
        blt_reg(11, width[7:0]);            // WidthL
        blt_reg(12, width[15:8]);           // WidthH
        blt_reg(13, height[7:0]);           // HeightL
        blt_reg(14, height[15:8]);          // HeightH
        blt_reg(15, src_stride[7:0]);       // SrcStrideL
        blt_reg(16, src_stride[15:8]);      // SrcStrideH
        blt_reg(17, dst_stride[7:0]);       // DstStrideL
        blt_reg(18, dst_stride[15:8]);      // DstStrideH
        blt_reg(19, 8'h00);                 // Mode = copy
    endtask

    task automatic setup_rotate(
        input int src_space, input int src_addr, input int src_stride,
        input int dst_space, input int dst_addr, input int dst_stride,
        input int size, input int angle, input int background
    );
        setup_copy(src_space, src_addr, src_stride,
                   dst_space, dst_addr, dst_stride,
                   size, size);
        blt_reg(19, 8'h04);                 // Mode = rotate
        blt_reg(21, background);            // out-of-bounds/background byte
        write_reg(BLT_ROTANGLE, angle[7:0]);
    endtask

    // ---------------------------------------------------------------
    // Tests
    // ---------------------------------------------------------------
    initial begin
        $display("=== Blitter Unit Tests ===");
        $display("");

        rst = 1; cpu_we = 0; cpu_re = 0;
        video_write_safe = 1;
        xram_busy = 0;
        cpu_addr = 0; cpu_wdata = 0;

        for (int i = 0; i < 65536; i++) sim_ram[i] = 0;
        for (int i = 0; i < 4000; i++) begin sim_char[i] = 0; sim_color[i] = 0; end
        for (int i = 0; i < 64000; i++) sim_gfx[i] = 0;
        for (int i = 0; i < 32768; i++) sim_sprite[i] = 0;

        repeat(50) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        // ----- Test 1: Initial state -----
        $display("Test: Initial state");
        check("status is idle (0)", dut.regs[1] == 0);
        check("RDY is high", blt_rdy == 1);

        // ----- Test: Reset clears programmer-visible state -----
        $display("Test: Reset clears registers");
        blt_reg(3, 8'h05);
        blt_reg(4, 8'h02);
        blt_reg(5, 8'h34);
        blt_reg(6, 8'h12);
        blt_reg(11, 8'h08);
        blt_reg(13, 8'h04);
        blt_reg(19, 8'h03);
        blt_reg(20, 8'hAA);
        write_reg(BLT_ROTANGLE, 8'h40);
        rst = 1;
        repeat(2) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);
        all_regs_zero = 1;
        for (int i = 0; i < 26; i++)
            all_regs_zero &= (dut.regs[i] == 8'h00);
        check("all blitter regs reset to zero/idle", all_regs_zero);
        check("blitter state reset idle", dut.state == 0);
        check("blitter RDY after reset", blt_rdy == 1);

        // ----- Test 2: Fill — RAM -----
        $display("Test: Fill RAM");
        setup_fill(0, 16'h1000, 10, 5, 3, 8'hAA);
        blt_start();
        wait_blt_done();

        check("status ok", dut.regs[1] == 8'h02);
        check("no error", dut.regs[2] == 0);
        check("count = 15", {dut.regs[24], dut.regs[23], dut.regs[22]} == 24'd15);
        check("RDY restored", blt_rdy == 1);

        // Verify fill pattern with stride
        check("ram[$1000] = AA", sim_ram[16'h1000] == 8'hAA);
        check("ram[$1004] = AA", sim_ram[16'h1004] == 8'hAA);
        check("ram[$1005] = 00 (beyond width)", sim_ram[16'h1005] == 8'h00);
        check("ram[$100A] = AA (row 1)", sim_ram[16'h100A] == 8'hAA);
        check("ram[$100E] = AA (row 1 end)", sim_ram[16'h100E] == 8'hAA);
        check("ram[$1014] = AA (row 2)", sim_ram[16'h1014] == 8'hAA);

        // ----- Test 3: Fill — VGC color RAM -----
        $display("Test: Fill VGC color RAM");
        setup_fill(2, 80, 80, 10, 2, 8'h0E);
        blt_start();
        wait_blt_done();

        check("color status ok", dut.regs[1] == 8'h02);
        check("color[80] = 0E", sim_color[80] == 8'h0E);
        check("color[89] = 0E", sim_color[89] == 8'h0E);
        check("color[160] = 0E (row 1)", sim_color[160] == 8'h0E);
        check("color[79] untouched", sim_color[79] == 8'h00);

        // ----- Test 4: Copy — RAM to RAM -----
        $display("Test: Copy RAM to RAM");
        // Set up source data at $2000 with stride 8
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 4; c++)
                sim_ram[16'h2000 + r*8 + c] = r*10 + c + 1;

        setup_copy(0, 16'h2000, 8, 0, 16'h3000, 6, 4, 3);
        blt_start();
        wait_blt_done();

        check("copy status ok", dut.regs[1] == 8'h02);
        check("count = 12", {dut.regs[24], dut.regs[23], dut.regs[22]} == 24'd12);
        check("dst[0] = src row0 col0", sim_ram[16'h3000] == 8'd1);
        check("dst[3] = src row0 col3", sim_ram[16'h3003] == 8'd4);
        check("dst[6] = src row1 col0", sim_ram[16'h3006] == 8'd11);
        check("dst[12] = src row2 col0", sim_ram[16'h300C] == 8'd21);

        // ----- Test 5: Copy — RAM to VGC gfx -----
        $display("Test: Copy RAM to VGC gfx");
        for (int i = 0; i < 10; i++)
            sim_ram[16'h4000 + i] = i + 1;

        setup_copy(0, 16'h4000, 10, 3, 512, 320, 10, 1);
        blt_start();
        wait_blt_done();

        check("gfx copy ok", dut.regs[1] == 8'h02);
        check("gfx[512] = 1", sim_gfx[512] == 4'd1);
        check("gfx[513] = 2", sim_gfx[513] == 4'd2);
        check("gfx[521] = 10 (0xA)", sim_gfx[521] == 4'hA);

        // ----- Test 6: Color-key copy -----
        $display("Test: Color-key copy");
        // Source: [1, 0, 2, 0, 3] at $5000
        sim_ram[16'h5000] = 1;
        sim_ram[16'h5001] = 0;
        sim_ram[16'h5002] = 2;
        sim_ram[16'h5003] = 0;
        sim_ram[16'h5004] = 3;
        // Pre-fill destination with 9
        for (int i = 0; i < 5; i++)
            sim_ram[16'h5100 + i] = 9;

        setup_copy(0, 16'h5000, 5, 0, 16'h5100, 5, 5, 1);
        blt_reg(19, 8'h02);     // Mode = color-key (not fill)
        blt_reg(21, 8'h00);     // ColorKey = 0
        blt_start();
        wait_blt_done();

        check("colorkey status ok", dut.regs[1] == 8'h02);
        check("count = 3 (skipped 2 zeros)", {dut.regs[24], dut.regs[23], dut.regs[22]} == 24'd3);
        check("dst[0] = 1 (copied)", sim_ram[16'h5100] == 8'd1);
        check("dst[1] = 9 (skipped)", sim_ram[16'h5101] == 8'd9);
        check("dst[2] = 2 (copied)", sim_ram[16'h5102] == 8'd2);
        check("dst[3] = 9 (skipped)", sim_ram[16'h5103] == 8'd9);
        check("dst[4] = 3 (copied)", sim_ram[16'h5104] == 8'd3);

        // ----- Test 7: RDY stall during blit -----
        $display("Test: RDY stall");
        setup_fill(0, 16'h6000, 100, 100, 1, 8'h55);
        blt_start();
        repeat(2) @(posedge clk);
        check("RDY low during blit", blt_rdy == 0);
        wait_blt_done();
        check("RDY restored after blit", blt_rdy == 1);

        // ----- Test 8: Error — bad command -----
        $display("Test: Error bad command");
        blt_reg(0, 8'hFF); // invalid command
        repeat(5) @(posedge clk);
        check("status = error", dut.regs[1] == 8'h03);
        check("errcode = badcmd", dut.regs[2] == 8'h01);

        // ----- Test 9: Error — zero width -----
        $display("Test: Error zero width");
        blt_reg(11, 0); blt_reg(12, 0); // width = 0
        blt_reg(13, 1); blt_reg(14, 0); // height = 1
        blt_reg(19, 8'h01);             // fill mode
        blt_start();
        repeat(5) @(posedge clk);
        check("zero width: status error", dut.regs[1] == 8'h03);
        check("zero width: errcode badargs", dut.regs[2] == 8'h04);

        // ----- Test 10: Error — zero height -----
        $display("Test: Error zero height");
        blt_reg(11, 1); blt_reg(12, 0); // width = 1
        blt_reg(13, 0); blt_reg(14, 0); // height = 0
        blt_start();
        repeat(5) @(posedge clk);
        check("zero height: status error", dut.regs[1] == 8'h03);
        check("zero height: errcode badargs", dut.regs[2] == 8'h04);

        // ----- Test 11: Error — text attribute range overflow -----
        $display("Test: Error text attribute range overflow");
        blt_reg(4, 7);                  // DstSpace = text attribute RAM
        blt_reg(11, 1); blt_reg(12, 0);
        blt_reg(13, 1); blt_reg(14, 0);
        blt_reg(19, 8'h01);             // fill
        blt_start();
        wait_blt_done();
        check("text attr range: status error", dut.regs[1] == 8'h03);
        check("text attr range: errcode", dut.regs[2] == 8'h03);

        // ----- Test 12: Copy with stride — 2D rectangle -----
        $display("Test: 2D stride copy");
        // Source: 3x2 rectangle in a 10-byte-wide field
        for (int i = 0; i < 20; i++) sim_ram[16'h7000 + i] = 0;
        sim_ram[16'h7000] = 8'hA1; sim_ram[16'h7001] = 8'hA2; sim_ram[16'h7002] = 8'hA3;
        sim_ram[16'h700A] = 8'hB1; sim_ram[16'h700B] = 8'hB2; sim_ram[16'h700C] = 8'hB3;

        // Destination: pack into stride=3 (no gaps)
        for (int i = 0; i < 6; i++) sim_ram[16'h7100 + i] = 0;

        setup_copy(0, 16'h7000, 10, 0, 16'h7100, 3, 3, 2);
        blt_start();
        wait_blt_done();

        check("stride dst[0] = A1", sim_ram[16'h7100] == 8'hA1);
        check("stride dst[1] = A2", sim_ram[16'h7101] == 8'hA2);
        check("stride dst[2] = A3", sim_ram[16'h7102] == 8'hA3);
        check("stride dst[3] = B1", sim_ram[16'h7103] == 8'hB1);
        check("stride dst[4] = B2", sim_ram[16'h7104] == 8'hB2);
        check("stride dst[5] = B3", sim_ram[16'h7105] == 8'hB3);

        // ----- Test 13: Fill VGC gfx (4-bit pixels) -----
        $display("Test: Fill VGC gfx");
        setup_fill(3, 0, 320, 320, 1, 8'h05);
        blt_start();
        wait_blt_done();

        check("gfx fill ok", dut.regs[1] == 8'h02);
        check("gfx[0] = 5", sim_gfx[0] == 4'd5);
        check("gfx[319] = 5", sim_gfx[319] == 4'd5);
        check("gfx[320] = 0 (not filled)", sim_gfx[320] == 4'd0);

        // ----- Test 13b: Video destinations wait for vblank-safe writes -----
        $display("Test: VGC video write gating");
        for (int i = 1000; i < 1004; i++) sim_gfx[i] = 4'h0;
        video_write_safe = 0;
        setup_fill(3, 1000, 4, 4, 1, 8'h07);
        blt_start();
        repeat(20) @(posedge clk);
        check("video fill waits while unsafe", blt_rdy == 0 && blt_vgc_we == 0);
        check("video fill wrote nothing while unsafe",
              sim_gfx[1000] == 4'h0 && sim_gfx[1003] == 4'h0);
        video_write_safe = 1;
        wait_blt_done();
        check("video fill resumes when safe", dut.regs[1] == 8'h02);
        check("video fill wrote after safe",
              sim_gfx[1000] == 4'h7 && sim_gfx[1003] == 4'h7);

        // ----- Test 13c: Non-video destinations ignore the video gate -----
        $display("Test: Non-video blits ignore video gate");
        video_write_safe = 0;
        setup_fill(0, 16'h7200, 4, 4, 1, 8'h66);
        blt_start();
        wait_blt_done();
        check("ram fill did not wait for video gate", dut.regs[1] == 8'h02);
        check("ram fill wrote while video gate unsafe", sim_ram[16'h7200] == 8'h66);
        video_write_safe = 1;

        // ----- Test 14: Copy to XRAM -----
        $display("Test: Copy to XRAM");
        for (int i = 0; i < 8; i++)
            sim_ram[16'h8000 + i] = 8'hC0 + i;

        setup_copy(0, 16'h8000, 8, 5, 19'h10000, 8, 8, 1);
        blt_start();
        wait_blt_done();

        check("xram copy ok", dut.regs[1] == 8'h02);
        check("xram[$10000] = C0", sim_xram[19'h10000] == 8'hC0);
        check("xram[$10007] = C7", sim_xram[19'h10007] == 8'hC7);

        // ----- Test 15: Fill sprite RAM -----
        $display("Test: Fill sprite shapes");
        setup_fill(4, 256, 128, 128, 1, 8'hFF);
        blt_start();
        wait_blt_done();

        check("sprite fill ok", dut.regs[1] == 8'h02);
        check("sprite[256] = FF", sim_sprite[256] == 8'hFF);
        check("sprite[383] = FF", sim_sprite[383] == 8'hFF);
        check("sprite[255] untouched", sim_sprite[255] == 8'h00);

        // ----- Test 16: Row buffer — overlapping same-space copy -----
        $display("Test: Row buffer overlapping copy");
        // Source at $9000: [A, B, C, D, E]
        // Destination at $9001 (overlapping, shifted right by 1)
        // Without row buffer: A overwrites $9001 before B is read → B becomes A
        // With row buffer: entire row read first, then written → correct
        sim_ram[16'h9000] = 8'hAA;
        sim_ram[16'h9001] = 8'hBB;
        sim_ram[16'h9002] = 8'hCC;
        sim_ram[16'h9003] = 8'hDD;
        sim_ram[16'h9004] = 8'hEE;

        setup_copy(0, 16'h9000, 5, 0, 16'h9001, 5, 4, 1);
        blt_start();
        wait_blt_done();

        check("overlap: status ok", dut.regs[1] == 8'h02);
        check("overlap: dst[0] = AA", sim_ram[16'h9001] == 8'hAA);
        check("overlap: dst[1] = BB", sim_ram[16'h9002] == 8'hBB);
        check("overlap: dst[2] = CC", sim_ram[16'h9003] == 8'hCC);
        check("overlap: dst[3] = DD", sim_ram[16'h9004] == 8'hDD);

        // ----- Test 17: VGC text-region scroll copy shape -----
        $display("Test: VGC text scroll copy");
        for (int r = 0; r < 50; r++) begin
            for (int c = 0; c < 80; c++) begin
                sim_char[(r * 80) + c] = 8'(((r * 17) + c) & 8'hFF);
            end
        end

        setup_copy(1, 160, 80, 1, 80, 80, 80, 48);
        blt_start();
        wait_blt_done();

        check("vgc scroll copy: status ok", dut.regs[1] == 8'h02);
        check("vgc scroll copy: count = 3840", {dut.regs[24], dut.regs[23], dut.regs[22]} == 24'd3840);
        all_regs_zero = 1;
        for (int r = 1; r < 49; r++) begin
            for (int c = 0; c < 80; c++) begin
                all_regs_zero &= sim_char[(r * 80) + c] == 8'((((r + 1) * 17) + c) & 8'hFF);
            end
        end
        check("vgc scroll copy: rows 2-49 moved to rows 1-48", all_regs_zero);
        check("vgc scroll copy: status row untouched", sim_char[0] == 8'h00 && sim_char[79] == 8'h4F);

        setup_fill(1, 3920, 80, 80, 1, 8'h20);
        blt_start();
        wait_blt_done();

        all_regs_zero = 1;
        for (int c = 0; c < 80; c++)
            all_regs_zero &= sim_char[3920 + c] == 8'h20;
        check("vgc scroll fill: bottom row cleared", all_regs_zero);

        // ----- Test 18: Range validation — src out of bounds -----
        $display("Test: Range validation");
        // Char RAM is 4000 bytes (80×50). Try to read beyond it.
        setup_copy(1, 3990, 2000, 0, 16'hA000, 20, 20, 1);
        blt_start();
        wait_blt_done();
        check("src range: status error", dut.regs[1] == 8'h03);
        check("src range: errcode = range", dut.regs[2] == 8'h03);

        // ----- Test 19: Range validation — dst out of bounds -----
        $display("Test: Dst range validation");
        setup_fill(2, 4790, 2000, 20, 1, 8'h01);
        blt_start();
        wait_blt_done();
        check("dst range: status error", dut.regs[1] == 8'h03);
        check("dst range: errcode = range", dut.regs[2] == 8'h03);

        // ----- Test 20: Write protection — ROM area -----
        $display("Test: Write protection");
        setup_fill(0, 16'hC000, 256, 10, 1, 8'hFF);
        blt_start();
        wait_blt_done();
        check("rom write: status error", dut.regs[1] == 8'h03);
        check("rom write: errcode = writeprot", dut.regs[2] == 8'h05);

        // Write to just below ROM should succeed
        setup_fill(0, 16'hBFF0, 16, 16, 1, 8'h42);
        blt_start();
        wait_blt_done();
        check("below rom: status ok", dut.regs[1] == 8'h02);
        check("below rom: ram[$BFF0] = 42", sim_ram[16'hBFF0] == 8'h42);

        // ----- Test 21: Row buffer with color-key -----
        $display("Test: Row buffer + color-key");
        sim_ram[16'hA000] = 8'h01;
        sim_ram[16'hA001] = 8'h00;  // transparent
        sim_ram[16'hA002] = 8'h02;
        sim_ram[16'hA003] = 8'h00;  // transparent
        for (int i = 0; i < 4; i++)
            sim_ram[16'hA100 + i] = 8'hFF;

        setup_copy(0, 16'hA000, 4, 0, 16'hA100, 4, 4, 1);
        blt_reg(19, 8'h02);     // Mode = color-key
        blt_reg(21, 8'h00);     // ColorKey = 0
        blt_start();
        wait_blt_done();

        check("rowbuf+ck: status ok", dut.regs[1] == 8'h02);
        check("rowbuf+ck: dst[0] = 01", sim_ram[16'hA100] == 8'h01);
        check("rowbuf+ck: dst[1] = FF (skipped)", sim_ram[16'hA101] == 8'hFF);
        check("rowbuf+ck: dst[2] = 02", sim_ram[16'hA102] == 8'h02);
        check("rowbuf+ck: dst[3] = FF (skipped)", sim_ram[16'hA103] == 8'hFF);

        // ----- Test 22: Rotate 90 degrees RAM to RAM -----
        $display("Test: Rotate 90 degrees");
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                sim_ram[16'hA200 + (r * 3) + c] = (r * 3) + c + 1;
        for (int i = 0; i < 9; i++)
            sim_ram[16'hA300 + i] = 8'hEE;

        setup_rotate(0, 16'hA200, 3, 0, 16'hA300, 3, 3, 64, 8'h00);
        blt_start();
        wait_blt_done();

        check("rotate90: status ok", dut.regs[1] == 8'h02);
        check("rotate90: count = 9", {dut.regs[24], dut.regs[23], dut.regs[22]} == 24'd9);
        check("rotate90: row0 = 7,4,1",
              sim_ram[16'hA300] == 8'd7 &&
              sim_ram[16'hA301] == 8'd4 &&
              sim_ram[16'hA302] == 8'd1);
        check("rotate90: row1 = 8,5,2",
              sim_ram[16'hA303] == 8'd8 &&
              sim_ram[16'hA304] == 8'd5 &&
              sim_ram[16'hA305] == 8'd2);
        check("rotate90: row2 = 9,6,3",
              sim_ram[16'hA306] == 8'd9 &&
              sim_ram[16'hA307] == 8'd6 &&
              sim_ram[16'hA308] == 8'd3);

        // ----- Test 23: Rotate fills out-of-bounds samples with BltColorKey -----
        $display("Test: Rotate background fill");
        for (int r = 0; r < 5; r++)
            for (int c = 0; c < 5; c++)
                sim_ram[16'hA400 + (r * 5) + c] = (r * 5) + c + 1;
        for (int i = 0; i < 25; i++)
            sim_ram[16'hA500 + i] = 8'h00;

        setup_rotate(0, 16'hA400, 5, 0, 16'hA500, 5, 5, 32, 8'hFE);
        blt_start();
        wait_blt_done();

        check("rotate45: status ok", dut.regs[1] == 8'h02);
        check("rotate45: corners use background",
              sim_ram[16'hA500] == 8'hFE &&
              sim_ram[16'hA504] == 8'hFE &&
              sim_ram[16'hA514] == 8'hFE &&
              sim_ram[16'hA518] == 8'hFE);
        check("rotate45: center stays centered", sim_ram[16'hA50C] == 8'd13);

        // ----- Test 24: Rotate rejects non-square regions -----
        $display("Test: Rotate rejects non-square regions");
        setup_copy(0, 16'hA200, 3, 0, 16'hA600, 3, 3, 2);
        blt_reg(19, 8'h04);
        write_reg(BLT_ROTANGLE, 8'h00);
        blt_start();
        wait_blt_done();
        check("rotate non-square: status error", dut.regs[1] == 8'h03);
        check("rotate non-square: errcode badargs", dut.regs[2] == 8'h04);

        setup_copy(0, 16'hA200, 257, 0, 16'hA600, 257, 257, 257);
        blt_reg(19, 8'h04);
        write_reg(BLT_ROTANGLE, 8'h00);
        blt_start();
        wait_blt_done();
        check("rotate too large: status error", dut.regs[1] == 8'h03);
        check("rotate too large: errcode badargs", dut.regs[2] == 8'h04);

        // ----- Test 25: Rotate rejects same-space overlap -----
        $display("Test: Rotate rejects overlapping destination");
        setup_rotate(0, 16'hA200, 3, 0, 16'hA201, 3, 3, 64, 8'h00);
        blt_start();
        wait_blt_done();
        check("rotate overlap: status error", dut.regs[1] == 8'h03);
        check("rotate overlap: errcode badargs", dut.regs[2] == 8'h04);

        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
