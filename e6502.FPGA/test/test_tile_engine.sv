// Tile engine unit tests — Verilator testbench
// Tests tile definition, nametable operations, scrolling, mirroring, palette, pixel output

`timescale 1ns/1ps

module test_tile_engine;

    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    logic        rst;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    wire  [7:0]  cpu_rdata;
    logic        cpu_we;
    logic        cpu_re;

    // DMA: tile engine reads from simulated RAM
    wire  [15:0] dma_addr;
    wire  [7:0]  dma_data;
    wire         dma_active;

    // Blitter access port
    logic [14:0] blt_tile_addr;
    logic [7:0]  blt_tile_wdata;
    logic        blt_tile_we;
    logic        blt_tile_re;
    wire  [7:0]  blt_tile_rdata;

    // Pixel interface
    logic [8:0]  pixel_x;
    logic [7:0]  pixel_y;
    logic        pixel_valid;
    wire  [11:0] tile_rgb;
    wire  [1:0]  tile_opaque;

    // Simulated CPU RAM
    logic [7:0] sim_ram [0:65535];

    // Feed DMA reads from simulated RAM (combinational — no pipeline latency)
    assign dma_data = sim_ram[dma_addr];

    tile_engine dut (
        .clk(clk), .rst(rst),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
        .cpu_rdata(cpu_rdata), .cpu_we(cpu_we), .cpu_re(cpu_re),
        .dma_addr(dma_addr), .dma_data(dma_data), .dma_active(dma_active),
        .blt_tile_addr(blt_tile_addr), .blt_tile_wdata(blt_tile_wdata),
        .blt_tile_we(blt_tile_we), .blt_tile_re(blt_tile_re),
        .blt_tile_rdata(blt_tile_rdata),
        .pixel_x(pixel_x), .pixel_y(pixel_y), .pixel_valid(pixel_valid),
        .tile_rgb(tile_rgb), .tile_opaque(tile_opaque)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    // ---------------------------------------------------------------
    // Helper tasks
    // ---------------------------------------------------------------
    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr; cpu_wdata <= data; cpu_we <= 1;
        @(posedge clk);
        cpu_we <= 0;
    endtask

    task automatic write_tile_reg(input int offset, input logic [7:0] data);
        write_reg(16'hA0C0 + offset, data);
    endtask

    task automatic write_tile_cmd(input logic [7:0] cmd);
        write_reg(16'hA0C7, cmd);
    endtask

    task automatic wait_dma_done();
        int timeout;
        // Wait 2 clocks for NBA to propagate dma_state
        repeat(2) @(posedge clk);
        timeout = 0;
        while (dma_active && timeout < 100000) begin
            @(posedge clk);
            timeout++;
        end
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

    // Read a tile register
    task automatic read_tile_reg(input int offset, output logic [7:0] val);
        @(posedge clk);
        cpu_addr <= 16'hA0C0 + offset;
        cpu_re <= 1;
        @(posedge clk);
        val = cpu_rdata;
        cpu_re <= 0;
    endtask

    // Sample tile pixel output at given coordinates
    // Pipeline has 3 stages of latency, so we need extra cycles
    task automatic sample_pixel(input int x, input int y);
        @(posedge clk);
        pixel_x <= x[8:0];
        pixel_y <= y[7:0];
        pixel_valid <= 1;
        // Stage 0: addresses presented to BRAM port B
        @(posedge clk);
        // Stage 1: nt/attr data available, tile_data addr presented
        @(posedge clk);
        // Stage 2: tile_data available, palette addr presented
        @(posedge clk);
        // Stage 3: palette data available, output valid
        @(posedge clk);
        // Let output settle
        @(posedge clk);
    endtask

    // ---------------------------------------------------------------
    // Tests
    // ---------------------------------------------------------------
    initial begin
        $display("=== Tile Engine Unit Tests ===");
        $display("");

        rst = 1;
        cpu_we = 0; cpu_re = 0;
        cpu_addr = 0; cpu_wdata = 0;
        pixel_x = 0; pixel_y = 0; pixel_valid = 0;
        blt_tile_addr = 0; blt_tile_wdata = 0;
        blt_tile_we = 0; blt_tile_re = 0;

        for (int i = 0; i < 65536; i++) sim_ram[i] = 0;

        repeat(50) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        // ----- Test 1: Initial state -----
        $display("Test: Initial state");
        check("tile_size16 starts 0 (8x8)", dut.tile_size16 == 0);
        check("mirror_mode starts 0", dut.mirror_mode == 0);
        check("trans_color starts 0", dut.trans_color == 0);
        check("scroll_x starts 0", dut.scroll_x == 0);
        check("scroll_y starts 0", dut.scroll_y == 0);
        check("DMA idle", dma_active == 0);

        // ----- Test 2: Config register -----
        $display("Test: Config register");
        write_tile_reg(0, 8'b00000101); // size=16x16, mirror=vertical
        repeat(2) @(posedge clk);
        check("tile_size16 set to 1", dut.tile_size16 == 1);
        check("mirror_mode set to 2", dut.mirror_mode == 2);

        // Reset to 8x8 for remaining tests
        write_tile_reg(0, 8'b00000000);
        repeat(2) @(posedge clk);

        // ----- Test 3: TileCmdPut — set nametable entry -----
        $display("Test: TileCmdPut");
        // P0=NT(0), P1=X(5), P2=Y(3), P3=tile#(42)
        write_tile_reg(8, 0);   // P0 = NT 0
        write_tile_reg(9, 5);   // P1 = X = 5
        write_tile_reg(10, 3);  // P2 = Y = 3
        write_tile_reg(11, 42); // P3 = tile# 42
        write_tile_cmd(8'h03);  // TCMD_PUT
        repeat(5) @(posedge clk);

        check("nametable[0][3*40+5] = 42", dut.nametable_ram.mem[3*40+5] == 42);

        // ----- Test 4: TileCmdAttr — set attribute -----
        $display("Test: TileCmdAttr");
        write_tile_reg(8, 0);       // NT 0
        write_tile_reg(9, 5);       // X = 5
        write_tile_reg(10, 3);      // Y = 3
        write_tile_reg(11, 8'hC3);  // attr: VFlip+HFlip + pal 3
        write_tile_cmd(8'h04);      // TCMD_ATTR
        repeat(5) @(posedge clk);

        check("attr_table[0][3*40+5] = 0xC3", dut.attr_table_ram.mem[3*40+5] == 8'hC3);

        // ----- Test 5: TileCmdFill — fill nametable -----
        $display("Test: TileCmdFill");
        write_tile_reg(8, 1);   // NT 1
        write_tile_reg(9, 99);  // tile# 99
        write_tile_cmd(8'h05);  // TCMD_FILL
        wait_dma_done();
        repeat(5) @(posedge clk);

        check("nametable[1][0] filled with 99", dut.nametable_ram.mem[1000] == 99);
        check("nametable[1][999] filled with 99", dut.nametable_ram.mem[1999] == 99);
        check("nametable[0][0] unchanged", dut.nametable_ram.mem[0] == 0); // NT 0 untouched

        // ----- Test 6: TileCmdDef — define tile from RAM via DMA -----
        $display("Test: TileCmdDef (DMA)");
        // Put test tile data in simulated RAM at $2000
        for (int i = 0; i < 32; i++)
            sim_ram[16'h2000 + i] = i + 1; // 1,2,3,...,32

        write_tile_reg(8, 10);          // P0 = tile# 10
        write_tile_reg(12, 8'h00);      // addr low = $00
        write_tile_reg(13, 8'h20);      // addr high = $20 → $2000
        write_tile_cmd(8'h01);          // TCMD_DEF
        wait_dma_done();
        repeat(5) @(posedge clk);

        // Tile 10 starts at offset 10*32=320
        check("tile_data[320] = 1", dut.tile_data_ram.mem[320] == 1);
        check("tile_data[321] = 2", dut.tile_data_ram.mem[321] == 2);
        $display("  DEBUG: tile_data[351]=%0d, dma_state=%0d, dma_count=%0d", dut.tile_data_ram.mem[351], dut.dma_state, dut.dma_count);
        check("tile_data[351] = 32", dut.tile_data_ram.mem[351] == 32);

        // ----- Test 7: TileCmdPeek -----
        $display("Test: TileCmdPeek");
        write_tile_reg(8, 0);   // NT 0
        write_tile_reg(9, 5);   // X = 5
        write_tile_reg(10, 3);  // Y = 3
        write_tile_cmd(8'h0C);  // TCMD_PEEK
        wait_dma_done();
        repeat(5) @(posedge clk);

        $display("  DEBUG: peek_val=%0d peek_attr=%h dma_state=%0d nametable[125]=%0d", dut.peek_val, dut.peek_attr, dut.dma_state, dut.nametable_ram.mem[125]);
        check("peek_val = 42", dut.peek_val == 42);
        check("peek_attr = 0xC3", dut.peek_attr == 8'hC3);

        // ----- Test 8: TileCmdPalC — set single palette color -----
        $display("Test: TileCmdPalC");
        write_tile_reg(14, 2);      // PalP0 = sub-palette 2
        write_tile_reg(15, 5);      // PalP1 = color index 5
        write_tile_reg(8, 8'hA0);   // P0 = R = 0xA0 → 0xA
        write_tile_reg(9, 8'h50);   // P1 = G = 0x50 → 0x5
        write_tile_reg(10, 8'hF0);  // P2 = B = 0xF0 → 0xF
        write_tile_cmd(8'h0B);      // TCMD_PALC
        repeat(5) @(posedge clk);

        $display("  DEBUG: pal_ram[37]=%h dma_state=%0d tregs8=%h tregs9=%h tregs10=%h tregs14=%h tregs15=%h",
            dut.pal_ram_inst.mem[37], dut.dma_state, dut.tregs[8], dut.tregs[9], dut.tregs[10], dut.tregs[14], dut.tregs[15]);
        check("pal_ram[2*16+5] = 0xA5F", dut.pal_ram_inst.mem[2*16+5] == 12'hA5F);

        // ----- Test 9: Scroll registers -----
        $display("Test: Scroll registers");
        write_tile_reg(2, 8'h34);  // ScrollX low
        write_tile_reg(3, 8'h12);  // ScrollX high
        write_tile_reg(4, 8'h78);  // ScrollY low
        write_tile_reg(5, 8'h00);  // ScrollY high
        repeat(2) @(posedge clk);

        check("scroll_x = 0x1234", dut.scroll_x == 16'h1234);
        check("scroll_y = 0x0078", dut.scroll_y == 16'h0078);

        // Reset scroll for pixel tests
        write_tile_reg(2, 0); write_tile_reg(3, 0);
        write_tile_reg(4, 0); write_tile_reg(5, 0);
        repeat(2) @(posedge clk);

        // ----- Test 10: Pixel output — basic tile rendering -----
        $display("Test: Pixel output");

        // Set up: tile 1 with a known pattern, place at NT0 position (0,0)
        // Tile 1 data: 32 bytes for 8x8, packed nibbles
        // Row 0: pixels [F,0,F,0,F,0,F,0] → bytes [F0, F0, F0, F0]
        for (int i = 0; i < 32; i++) sim_ram[16'h3000 + i] = 0;
        sim_ram[16'h3000] = 8'hF0; // row 0: pixel 0=0xF, pixel 1=0x0
        sim_ram[16'h3001] = 8'hF0; // row 0: pixel 2=0xF, pixel 3=0x0
        sim_ram[16'h3002] = 8'hF0; // row 0: pixel 4=0xF, pixel 5=0x0
        sim_ram[16'h3003] = 8'hF0; // row 0: pixel 6=0xF, pixel 7=0x0

        write_tile_reg(8, 1);           // P0 = tile# 1
        write_tile_reg(12, 8'h00);      // addr = $3000
        write_tile_reg(13, 8'h30);
        write_tile_cmd(8'h01);          // TCMD_DEF
        wait_dma_done();
        repeat(5) @(posedge clk);

        // Place tile 1 at NT0 position (0,0)
        write_tile_reg(8, 0);   // NT 0
        write_tile_reg(9, 0);   // X = 0
        write_tile_reg(10, 0);  // Y = 0
        write_tile_reg(11, 1);  // tile# 1
        write_tile_cmd(8'h03);  // PUT
        repeat(5) @(posedge clk);

        // Set transparent color to 0 (default)
        write_tile_reg(1, 0);
        repeat(2) @(posedge clk);

        // Sample pixel (0,0) — should be tile 1, row 0, col 0 = 0xF
        sample_pixel(0, 0);
        check("pixel(0,0) opaque", tile_opaque != 0);
        // Color index 0xF from sub-palette 0 → palette[15] = 12'hBBB (C64 light grey)
        check("pixel(0,0) color = pal[15]", tile_rgb == 12'hBBB);

        // Sample pixel (1,0) — col 1 = 0x0 → transparent
        sample_pixel(1, 0);
        check("pixel(1,0) transparent", tile_opaque == 0);

        // Sample pixel (2,0) — col 2 = 0xF → opaque
        sample_pixel(2, 0);
        check("pixel(2,0) opaque", tile_opaque != 0);

        // ----- Test 11: TileCmdCls — clear all -----
        $display("Test: TileCmdCls");
        write_tile_cmd(8'h0F); // TCMD_CLS
        wait_dma_done();
        repeat(5) @(posedge clk);

        check("nametable[0][0] cleared", dut.nametable_ram.mem[0] == 0);
        check("nametable[1][0] cleared", dut.nametable_ram.mem[1000] == 0);
        check("attr_table[0][3*40+5] cleared", dut.attr_table_ram.mem[3*40+5] == 0);

        // ----- Test 12: Column buffer operations -----
        $display("Test: Column buffer");

        // BUFFILL: fill buffer with tile 7
        write_tile_reg(8, 7);   // P0 = tile# 7
        write_tile_cmd(8'h10);  // TCMD_BUFFILL
        repeat(5) @(posedge clk);
        check("col_buffer[0] = 7", dut.col_buffer[0] == 7);
        check("col_buffer[24] = 7", dut.col_buffer[24] == 7);

        // BUFSET: set row 5 to tile 99
        write_tile_reg(8, 5);   // P0 = row 5
        write_tile_reg(9, 99);  // P1 = tile# 99
        write_tile_cmd(8'h11);  // TCMD_BUFSET
        repeat(5) @(posedge clk);
        check("col_buffer[5] = 99", dut.col_buffer[5] == 99);
        check("col_buffer[4] still 7", dut.col_buffer[4] == 7);

        // BUFRANGE: fill rows 10-15 with tile 55
        write_tile_reg(8, 10);  // P0 = y1
        write_tile_reg(9, 15);  // P1 = y2
        write_tile_reg(10, 55); // P2 = tile#
        write_tile_cmd(8'h12);  // TCMD_BUFRANGE
        repeat(5) @(posedge clk);
        check("col_buffer[10] = 55", dut.col_buffer[10] == 55);
        check("col_buffer[15] = 55", dut.col_buffer[15] == 55);
        check("col_buffer[9] still 7", dut.col_buffer[9] == 7);

        // BUFPUT: write buffer to NT0 column 3
        write_tile_reg(8, 0);   // P0 = NT 0
        write_tile_reg(9, 3);   // P1 = col 3
        write_tile_cmd(8'h13);  // TCMD_BUFPUT
        wait_dma_done();
        repeat(5) @(posedge clk);
        check("NT0 row0 col3 = 7", dut.nametable_ram.mem[0*40+3] == 7);
        check("NT0 row5 col3 = 99", dut.nametable_ram.mem[5*40+3] == 99);
        check("NT0 row10 col3 = 55", dut.nametable_ram.mem[10*40+3] == 55);

        // ----- Test 13: TileCmdLoad — load entire nametable via DMA -----
        $display("Test: TileCmdLoad (DMA)");
        // Fill simulated RAM with pattern
        for (int i = 0; i < 1000; i++)
            sim_ram[16'h4000 + i] = (i & 8'hFF);

        write_tile_reg(8, 2);          // P0 = NT 2
        write_tile_reg(12, 8'h00);     // addr = $4000
        write_tile_reg(13, 8'h40);
        write_tile_cmd(8'h08);         // TCMD_LOAD
        wait_dma_done();
        repeat(5) @(posedge clk);

        check("NT2[0] loaded", dut.nametable_ram.mem[2000] == 0);
        check("NT2[1] loaded", dut.nametable_ram.mem[2001] == 1);
        check("NT2[255] loaded", dut.nametable_ram.mem[2255] == 255);

        // ----- Test 14: TileCmdDefBulk — multi-tile DMA -----
        $display("Test: TileCmdDefBulk");
        for (int i = 0; i < 64; i++)
            sim_ram[16'h5000 + i] = 8'hAA;

        write_tile_reg(8, 20);         // P0 = start tile# 20
        write_tile_reg(9, 2);          // P1 = count = 2 tiles
        write_tile_reg(12, 8'h00);     // addr = $5000
        write_tile_reg(13, 8'h50);
        write_tile_cmd(8'h02);         // TCMD_DEFBULK
        wait_dma_done();
        repeat(5) @(posedge clk);

        // Tile 20 = offset 640, tile 21 = offset 672
        check("tile 20 byte 0 = 0xAA", dut.tile_data_ram.mem[640] == 8'hAA);
        check("tile 21 byte 0 = 0xAA", dut.tile_data_ram.mem[672] == 8'hAA);

        // ----- Test 15: Mirror modes -----
        $display("Test: Mirror resolution");
        // Horizontal mirror: NT0=NT1, NT2=NT3
        check("H mirror: resolve(0,0)=0", dut.resolve_nt(0, 0) == 0);
        check("H mirror: resolve(1,0)=0", dut.resolve_nt(1, 0) == 0);
        check("H mirror: resolve(0,1)=2", dut.resolve_nt(0, 1) == 2);
        check("H mirror: resolve(1,1)=2", dut.resolve_nt(1, 1) == 2);

        // Switch to vertical mirror
        write_tile_reg(0, 8'b00000010); // mirror=1 (vertical)
        repeat(2) @(posedge clk);
        check("V mirror: resolve(0,0)=0", dut.resolve_nt(0, 0) == 0);
        check("V mirror: resolve(1,0)=1", dut.resolve_nt(1, 0) == 1);
        check("V mirror: resolve(0,1)=0", dut.resolve_nt(0, 1) == 0);
        check("V mirror: resolve(1,1)=1", dut.resolve_nt(1, 1) == 1);

        // Single-screen
        write_tile_reg(0, 8'b00000110); // mirror=3 (single)
        repeat(2) @(posedge clk);
        check("single: resolve(0,0)=0", dut.resolve_nt(0, 0) == 0);
        check("single: resolve(1,1)=0", dut.resolve_nt(1, 1) == 0);

        // Reset to horizontal
        write_tile_reg(0, 0);
        repeat(2) @(posedge clk);

        // ----- Test 16: TileCmdRow — load row via DMA -----
        $display("Test: TileCmdRow (DMA)");
        for (int i = 0; i < 40; i++)
            sim_ram[16'h6000 + i] = i + 10;

        write_tile_reg(8, 0);          // NT 0
        write_tile_reg(9, 7);          // row Y = 7
        write_tile_reg(12, 8'h00);     // addr = $6000
        write_tile_reg(13, 8'h60);
        write_tile_cmd(8'h06);         // TCMD_ROW
        wait_dma_done();
        repeat(5) @(posedge clk);

        check("NT0 row7 col0 = 10", dut.nametable_ram.mem[7*40+0] == 10);
        check("NT0 row7 col39 = 49", dut.nametable_ram.mem[7*40+39] == 49);

        // ----- Test 17: Transparent color -----
        $display("Test: Transparent color");
        // Set trans color to 5
        write_tile_reg(1, 5);
        repeat(2) @(posedge clk);
        check("trans_color = 5", dut.trans_color == 5);

        // ----- Test 18: Blitter write + read -----
        $display("Test: Blitter access");
        // Write via blitter port
        @(posedge clk);
        blt_tile_addr <= 15'd100;
        blt_tile_wdata <= 8'hAB;
        blt_tile_we <= 1;
        @(posedge clk);
        blt_tile_we <= 0;
        repeat(2) @(posedge clk);
        check("blitter write stored", dut.tile_data_ram.mem[100] == 8'hAB);

        // Read via blitter port
        @(posedge clk);
        blt_tile_addr <= 15'd100;
        blt_tile_re <= 1;
        @(posedge clk);
        blt_tile_re <= 0;
        @(posedge clk); // wait for BRAM latency
        @(posedge clk);
        check("blitter read returns 0xAB", blt_tile_rdata == 8'hAB);

        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
