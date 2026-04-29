// Gtext unit tests — iverilog testbench
// Tests text rendering to graphics framebuffer via font ROM

`timescale 1ns/1ps

module test_gtext;

    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    logic        rst;
    logic        cpu_ce;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    wire  [7:0]  cpu_rdata;
    logic        cpu_we;
    logic        cpu_re;
    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire         vgc_rdy;

    // Tile DMA (unused in gtext tests, but needed for port)
    wire  [15:0] tile_dma_addr;
    logic [7:0]  tile_dma_data;
    wire         tile_dma_active;

    vgc dut (
        .clk(clk), .rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
        .cpu_rdata(cpu_rdata), .cpu_we(cpu_we), .cpu_re(cpu_re),
        .key_valid(1'b0), .key_data(8'h00),
        .tile_dma_addr(tile_dma_addr),
        .tile_dma_data(8'h00),
        .tile_dma_active(tile_dma_active),
        .blt_space(3'd0), .blt_addr(16'd0), .blt_rdata(),
        .blt_wdata(8'd0), .blt_we(1'b0), .blt_re(1'b0),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .irq_out(),
        .rdy_out(vgc_rdy)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    // ---------------------------------------------------------------
    // Helper tasks
    // ---------------------------------------------------------------
    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        cpu_addr <= addr;
        cpu_wdata <= data;
        cpu_we <= 1;
        cpu_ce <= 1;
        @(posedge clk);
        cpu_we <= 0;
        cpu_ce <= 0;
    endtask

    task automatic write_param(input int idx, input logic [7:0] data);
        write_reg(16'hA011 + idx, data);
    endtask

    task automatic write_cmd(input logic [7:0] cmd);
        write_reg(16'hA010, cmd);
    endtask

    task automatic wait_cmd_done();
        int timeout;
        timeout = 0;
        // Wait 2 clocks for NBA to propagate busy flags
        repeat(2) @(posedge clk);
        while ((dut.cmd_busy || dut.artist_inst.busy) && timeout < 500000) begin
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

    // Write a string to the FIO name buffer ($B9B0) and length ($B9A3)
    task automatic set_fio_string(input string s);
        int i;
        // Set length
        write_reg(16'hB9A3, s.len());
        // Set characters
        for (i = 0; i < s.len(); i++)
            write_reg(16'hB9B0 + i, s[i]);
    endtask

    // Read gfx pixel at (x, y)
    function automatic logic [3:0] gfx_pixel(input int x, input int y);
        return dut.gfx_inst.gfx_mem.mem[y * 320 + x];
    endfunction

    // ---------------------------------------------------------------
    // Tests
    // ---------------------------------------------------------------
    initial begin
        $display("=== Gtext Unit Tests ===");
        $display("");

        rst = 1;
        cpu_we = 0; cpu_re = 0; cpu_ce = 0;
        cpu_addr = 0; cpu_wdata = 0;

        repeat(50) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        // ----- Test 1: Empty string does nothing -----
        $display("Test: Empty string");
        write_reg(16'hB9A3, 0); // length = 0
        write_param(0, 0); write_param(1, 0);   // X = 0
        write_param(2, 0); write_param(3, 0);   // Y = 0
        write_param(5, 1);                        // scale = 1
        write_cmd(8'h0A);  // CmdGtext
        repeat(5) @(posedge clk);
        check("empty string: cmd not busy", dut.cmd_busy == 0 && dut.artist_inst.busy == 0);

        // ----- Test 2: Single character 'A' at (0,0), scale=1 -----
        $display("Test: Single char 'A' at (0,0) scale=1");

        // Clear gfx first
        write_cmd(8'h07); // GCLS
        wait_cmd_done();

        // Set draw color to 1 (white)
        write_param(0, 1);
        write_cmd(8'h08); // GCOLOR

        // Set string "A"
        set_fio_string("A");

        // Gtext: P0|P1=x(0), P2|P3=y(0), P4=font(0), P5=scale(1)
        write_param(0, 0); write_param(1, 0); // X = 0
        write_param(2, 0); write_param(3, 0); // Y = 0
        write_param(4, 0);                     // font slot 0
        write_param(5, 1);                     // scale = 1
        write_cmd(8'h0A);
        wait_cmd_done();

        // 'A' = ASCII 65. Check font ROM data for char 65, row 0
        // The font ROM is CP437. 'A' has specific pixel patterns.
        // Check that SOME pixels are set (non-zero) in the 8x8 area
        begin
            int pixel_count;
            pixel_count = 0;
            for (int r = 0; r < 8; r++)
                for (int c = 0; c < 8; c++)
                    if (gfx_pixel(c, r) != 0)
                        pixel_count++;
            check("'A' rendered: has set pixels", pixel_count > 0);
            check("'A' rendered: not all pixels set", pixel_count < 64);
        end

        // Verify pixels match font ROM bits
        begin
            int match;
            match = 1;
            for (int r = 0; r < 8; r++) begin
                logic [7:0] fb;
                fb = dut.text_inst.font_mem.mem[{8'd65, r[2:0]}];
                for (int c = 0; c < 8; c++) begin
                    logic expected;
                    expected = fb[7 - c];
                    if (expected && gfx_pixel(c, r) != 4'd1)
                        match = 0;
                    if (!expected && gfx_pixel(c, r) != 4'd0)
                        match = 0;
                end
            end
            check("'A' pixels match font ROM exactly", match == 1);
        end

        // ----- Test 3: Position offset -----
        $display("Test: Char at offset position");

        write_cmd(8'h07); // GCLS
        wait_cmd_done();

        set_fio_string("X");
        write_param(0, 100); write_param(1, 0); // X = 100
        write_param(2, 50);  write_param(3, 0); // Y = 50
        write_param(4, 0); write_param(5, 1);   // font=0, scale=1
        write_cmd(8'h0A);
        wait_cmd_done();

        // Verify area around origin is still clear
        check("origin pixel clear", gfx_pixel(0, 0) == 0);
        check("pixel at (50,25) clear", gfx_pixel(50, 25) == 0);

        // Verify some pixels in target area (100-107, 50-57) are set
        begin
            int pc;
            pc = 0;
            for (int r = 50; r < 58; r++)
                for (int c = 100; c < 108; c++)
                    if (gfx_pixel(c, r) != 0) pc++;
            check("'X' at (100,50) has pixels", pc > 0);
        end

        // ----- Test 4: Scale factor -----
        $display("Test: Scale factor 2x");

        write_cmd(8'h07); wait_cmd_done();

        set_fio_string("I");
        write_param(0, 0); write_param(1, 0);
        write_param(2, 0); write_param(3, 0);
        write_param(4, 0); write_param(5, 2); // scale=2
        write_cmd(8'h0A);
        wait_cmd_done();

        // At scale=2, char occupies 16x16 pixels
        // Check pixels exist in the 16x16 area
        begin
            int pc;
            pc = 0;
            for (int r = 0; r < 16; r++)
                for (int c = 0; c < 16; c++)
                    if (gfx_pixel(c, r) != 0) pc++;
            check("'I' at scale 2 has pixels in 16x16 area", pc > 0);
        end

        // Each font pixel should produce a 2x2 block
        // Check first font row of 'I'
        begin
            logic [7:0] fb;
            int ok;
            fb = dut.text_inst.font_mem.mem[{8'd73, 3'b000}]; // 'I' row 0
            ok = 1;
            for (int c = 0; c < 8; c++) begin
                if (fb[7-c]) begin
                    // Should have 2×2 block at (c*2, 0), (c*2+1, 0), (c*2, 1), (c*2+1, 1)
                    if (gfx_pixel(c*2, 0) == 0 || gfx_pixel(c*2+1, 0) == 0 ||
                        gfx_pixel(c*2, 1) == 0 || gfx_pixel(c*2+1, 1) == 0)
                        ok = 0;
                end
            end
            check("scale=2 produces 2x2 blocks for row 0", ok == 1);
        end

        // ----- Test 5: Multi-character string -----
        $display("Test: Multi-char string");

        write_cmd(8'h07); wait_cmd_done();

        set_fio_string("AB");
        write_param(0, 0); write_param(1, 0);
        write_param(2, 0); write_param(3, 0);
        write_param(4, 0); write_param(5, 1);
        write_cmd(8'h0A);
        wait_cmd_done();

        // 'A' at (0,0), 'B' at (8,0)
        begin
            int pc_a, pc_b;
            pc_a = 0; pc_b = 0;
            for (int r = 0; r < 8; r++) begin
                for (int c = 0; c < 8; c++)
                    if (gfx_pixel(c, r) != 0) pc_a++;
                for (int c = 8; c < 16; c++)
                    if (gfx_pixel(c, r) != 0) pc_b++;
            end
            check("'A' area has pixels", pc_a > 0);
            check("'B' area has pixels", pc_b > 0);
        end

        // ----- Test 6: Bounds check — off-screen -----
        $display("Test: Off-screen clipping");

        write_cmd(8'h07); wait_cmd_done();

        set_fio_string("X");
        write_param(0, 316 & 8'hFF); write_param(1, (316 >> 8) & 8'hFF); // X = 316
        write_param(2, 196); write_param(3, 0);  // Y = 196
        write_param(4, 0); write_param(5, 1);
        write_cmd(8'h0A);
        wait_cmd_done();

        // Should not crash, pixels within bounds should be set
        check("off-screen render completed", dut.cmd_busy == 0 && dut.artist_inst.busy == 0);

        // ----- Test 7: Draw color -----
        $display("Test: Draw color");

        write_cmd(8'h07); wait_cmd_done();

        // Set draw color to 5 (green)
        write_param(0, 5);
        write_cmd(8'h08);

        set_fio_string("X");
        write_param(0, 0); write_param(1, 0);
        write_param(2, 0); write_param(3, 0);
        write_param(4, 0); write_param(5, 1);
        write_cmd(8'h0A);
        wait_cmd_done();

        // Find a set pixel and verify its color is 5
        begin
            int found;
            found = 0;
            for (int r = 0; r < 8 && !found; r++)
                for (int c = 0; c < 8 && !found; c++)
                    if (gfx_pixel(c, r) != 0) begin
                        check("draw color is 5 (green)", gfx_pixel(c, r) == 4'd5);
                        found = 1;
                    end
            if (!found)
                check("found at least one pixel", 0);
        end

        // ---------------------------------------------------------------
        // Summary
        // ---------------------------------------------------------------
        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
