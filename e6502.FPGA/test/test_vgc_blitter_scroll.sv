// VGC + blitter integration regression for vtext/NovaZ scrolling.
// Reproduces the shared-library scroll shape: copy rows 2..49 to 1..48
// within VGC char/color/text-attribute RAM, then clear the bottom story row.

`timescale 1ns/1ps

module test_vgc_blitter_scroll;

    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic rst;

    logic        vgc_cpu_ce;
    logic [15:0] vgc_cpu_addr;
    logic [7:0]  vgc_cpu_wdata;
    wire  [7:0]  vgc_cpu_rdata;
    logic        vgc_cpu_we;
    logic        vgc_cpu_re;
    logic        key_valid;
    logic [7:0]  key_data;
    wire  [15:0] tile_dma_addr;
    wire         tile_dma_active;
    logic [15:0] dbg_addr;
    wire  [7:0]  dbg_rdata;
    logic        dbg_we;
    logic [15:0] dbg_waddr;
    logic [7:0]  dbg_wdata;
    logic        dbg_vmem_we;
    logic        dbg_vmem_re;
    logic [2:0]  dbg_vmem_space;
    logic [16:0] dbg_vmem_addr;
    logic [7:0]  dbg_vmem_wdata;
    wire  [7:0]  dbg_vmem_rdata;
    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire         irq_out;
    wire         vgc_rdy;
    wire         vgc_sys_reset_req;

    logic [15:0] blt_cpu_addr;
    logic [7:0]  blt_cpu_wdata;
    logic        blt_cpu_we;
    logic        blt_cpu_re;
    wire  [7:0]  blt_cpu_rdata;
    wire         blt_rdy;
    wire  [15:0] blt_ram_addr;
    wire  [7:0]  blt_ram_wdata;
    wire         blt_ram_we;
    wire  [18:0] blt_xram_addr;
    wire  [7:0]  blt_xram_wdata;
    wire         blt_xram_we;
    wire         blt_xram_re;
    wire  [2:0]  blt_vgc_space;
    wire  [16:0] blt_vgc_addr;
    wire  [7:0]  blt_vgc_rdata;
    wire  [7:0]  blt_vgc_wdata;
    wire         blt_vgc_we;
    wire         blt_vgc_re;

    logic [7:0] sim_ram [0:65535];
    logic [7:0] sim_xram [0:524287];
    logic [7:0] blt_ram_rdata;
    logic [7:0] blt_xram_rdata;

    always_ff @(posedge clk) begin
        blt_ram_rdata <= sim_ram[blt_ram_addr];
        blt_xram_rdata <= sim_xram[blt_xram_addr];
        if (blt_ram_we)
            sim_ram[blt_ram_addr] <= blt_ram_wdata;
        if (blt_xram_we)
            sim_xram[blt_xram_addr] <= blt_xram_wdata;
    end

    vgc vgc_inst (
        .clk(clk), .rst(rst), .video_rst(rst),
        .cpu_ce(vgc_cpu_ce),
        .cpu_addr(vgc_cpu_addr), .cpu_wdata(vgc_cpu_wdata),
        .cpu_rdata(vgc_cpu_rdata), .cpu_we(vgc_cpu_we), .cpu_re(vgc_cpu_re),
        .key_valid(key_valid), .key_data(key_data),
        .tile_dma_addr(tile_dma_addr),
        .tile_dma_data(8'h00),
        .tile_dma_active(tile_dma_active),
        .blt_space(blt_vgc_space), .blt_addr(blt_vgc_addr),
        .blt_rdata(blt_vgc_rdata), .blt_wdata(blt_vgc_wdata),
        .blt_we(blt_vgc_we), .blt_re(blt_vgc_re),
        .dbg_addr(dbg_addr), .dbg_rdata(dbg_rdata),
        .dbg_we(dbg_we), .dbg_waddr(dbg_waddr), .dbg_wdata(dbg_wdata),
        .dbg_vmem_we(dbg_vmem_we), .dbg_vmem_re(dbg_vmem_re),
        .dbg_vmem_space(dbg_vmem_space),
        .dbg_vmem_addr(dbg_vmem_addr), .dbg_vmem_wdata(dbg_vmem_wdata),
        .dbg_vmem_rdata(dbg_vmem_rdata),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .irq_out(irq_out),
        .rdy_out(vgc_rdy),
        .sys_reset_req(vgc_sys_reset_req)
    );

    blitter blt_inst (
        .clk(clk), .rst(rst),
        .cpu_addr(blt_cpu_addr), .cpu_wdata(blt_cpu_wdata),
        .cpu_we(blt_cpu_we), .cpu_rdata(blt_cpu_rdata), .cpu_re(blt_cpu_re),
        .rdy_out(blt_rdy),
        .ram_addr(blt_ram_addr), .ram_rdata(blt_ram_rdata),
        .ram_wdata(blt_ram_wdata), .ram_we(blt_ram_we),
        .xram_addr(blt_xram_addr), .xram_rdata(blt_xram_rdata),
        .xram_wdata(blt_xram_wdata), .xram_we(blt_xram_we),
        .xram_re(blt_xram_re), .xram_busy(1'b0),
        .vgc_space(blt_vgc_space), .vgc_addr(blt_vgc_addr),
        .vgc_rdata(blt_vgc_rdata), .vgc_wdata(blt_vgc_wdata),
        .vgc_we(blt_vgc_we), .vgc_re(blt_vgc_re)
    );

    localparam BLT = 16'hBA83;

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    logic ok;

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

    task automatic do_reset();
        rst = 1;
        vgc_cpu_ce = 0;
        vgc_cpu_addr = 16'h0000;
        vgc_cpu_wdata = 8'h00;
        vgc_cpu_we = 0;
        vgc_cpu_re = 0;
        key_valid = 0;
        key_data = 8'h00;
        dbg_addr = 16'h0000;
        dbg_we = 0;
        dbg_waddr = 16'h0000;
        dbg_wdata = 8'h00;
        dbg_vmem_we = 0;
        dbg_vmem_re = 0;
        dbg_vmem_space = 3'd0;
        dbg_vmem_addr = 17'd0;
        dbg_vmem_wdata = 8'h00;
        blt_cpu_addr = 16'h0000;
        blt_cpu_wdata = 8'h00;
        blt_cpu_we = 0;
        blt_cpu_re = 0;
        repeat(50) @(posedge clk);
        rst = 0;
        begin
            int timeout = 0;
            while (!vgc_rdy && timeout < 100000) begin
                @(posedge clk);
                timeout++;
            end
        end
        repeat(10) @(posedge clk);
    endtask

    task automatic blt_reg(input int off, input logic [7:0] data);
        @(posedge clk);
        blt_cpu_addr <= BLT + off;
        blt_cpu_wdata <= data;
        blt_cpu_we <= 1;
        @(posedge clk);
        blt_cpu_we <= 0;
    endtask

    task automatic setup_copy(
        input int src_space, input int src_addr, input int src_stride,
        input int dst_space, input int dst_addr, input int dst_stride,
        input int width, input int height
    );
        blt_reg(3, src_space[7:0]);
        blt_reg(4, dst_space[7:0]);
        blt_reg(5, src_addr[7:0]);
        blt_reg(6, src_addr[15:8]);
        blt_reg(7, src_addr[23:16]);
        blt_reg(8, dst_addr[7:0]);
        blt_reg(9, dst_addr[15:8]);
        blt_reg(10, dst_addr[23:16]);
        blt_reg(11, width[7:0]);
        blt_reg(12, width[15:8]);
        blt_reg(13, height[7:0]);
        blt_reg(14, height[15:8]);
        blt_reg(15, src_stride[7:0]);
        blt_reg(16, src_stride[15:8]);
        blt_reg(17, dst_stride[7:0]);
        blt_reg(18, dst_stride[15:8]);
        blt_reg(19, 8'h00);
    endtask

    task automatic setup_fill(
        input int dst_space, input int dst_addr, input int dst_stride,
        input int width, input int height, input logic [7:0] fill_value
    );
        blt_reg(4, dst_space[7:0]);
        blt_reg(8, dst_addr[7:0]);
        blt_reg(9, dst_addr[15:8]);
        blt_reg(10, dst_addr[23:16]);
        blt_reg(11, width[7:0]);
        blt_reg(12, width[15:8]);
        blt_reg(13, height[7:0]);
        blt_reg(14, height[15:8]);
        blt_reg(17, dst_stride[7:0]);
        blt_reg(18, dst_stride[15:8]);
        blt_reg(19, 8'h01);
        blt_reg(20, fill_value);
    endtask

    task automatic scroll_plane(input int space, input logic [7:0] fill_value);
        setup_copy(space, 160, 80, space, 80, 80, 80, 48);
        blt_start_and_wait(space);

        check($sformatf("space %0d copy status ok", space), blt_inst.regs[1] == 8'h02);
        check($sformatf("space %0d copy count = 3840", space),
              {blt_inst.regs[24], blt_inst.regs[23], blt_inst.regs[22]} == 24'd3840);

        setup_fill(space, 3920, 80, 80, 1, fill_value);
        blt_start_and_wait(space);

        check($sformatf("space %0d fill status ok", space), blt_inst.regs[1] == 8'h02);
        check($sformatf("space %0d fill count = 80", space),
              {blt_inst.regs[24], blt_inst.regs[23], blt_inst.regs[22]} == 24'd80);
    endtask

    task automatic blt_start_and_wait(input int read_noise_space);
        int noise_addr;
        noise_addr = 0;
        dbg_vmem_space <= read_noise_space[2:0];
        dbg_vmem_addr  <= 17'(noise_addr);
        dbg_vmem_re    <= 1;
        blt_reg(0, 8'h01);
        repeat(2) @(posedge clk);
        while (!blt_rdy) begin
            @(posedge clk);
            noise_addr = (noise_addr + 37) % 4000;
            dbg_vmem_space <= read_noise_space[2:0];
            dbg_vmem_addr  <= 17'(noise_addr);
            dbg_vmem_re    <= 1;
        end
        @(posedge clk);
        dbg_vmem_re <= 0;
        repeat(2) @(posedge clk);
    endtask

    initial begin
        $display("=== VGC + Blitter Scroll Integration Test ===");
        do_reset();

        for (int r = 0; r < 50; r++) begin
            for (int c = 0; c < 80; c++) begin
                vgc_inst.text_inst.char_mem.mem[(r * 80) + c] = 8'(((r * 17) + c) & 8'hFF);
                vgc_inst.text_inst.color_mem.mem[(r * 80) + c] = 8'(((r * 13) + c + 8'h40) & 8'hFF);
                vgc_inst.text_inst.attr_mem.mem[(r * 80) + c] = 8'(((r * 7) + c + 8'h80) & 8'hFF);
            end
        end
        repeat(4) @(posedge clk);

        scroll_plane(1, 8'h20);
        scroll_plane(2, 8'h0F);
        scroll_plane(7, 8'h00);

        ok = 1;
        for (int r = 1; r < 49; r++) begin
            for (int c = 0; c < 80; c++) begin
                ok &= vgc_inst.text_inst.char_mem.mem[(r * 80) + c] ==
                      8'((((r + 1) * 17) + c) & 8'hFF);
            end
        end
        check("char rows 2-49 copied to rows 1-48 through real port", ok);
        check("char status row remains untouched",
              vgc_inst.text_inst.char_mem.mem[0] == 8'h00 &&
              vgc_inst.text_inst.char_mem.mem[79] == 8'h4F);
        ok = 1;
        for (int c = 0; c < 80; c++)
            ok &= vgc_inst.text_inst.char_mem.mem[3920 + c] == 8'h20;
        check("char bottom story row cleared through real port", ok);

        ok = 1;
        for (int r = 1; r < 49; r++) begin
            for (int c = 0; c < 80; c++) begin
                ok &= vgc_inst.text_inst.color_mem.mem[(r * 80) + c] ==
                      8'((((r + 1) * 13) + c + 8'h40) & 8'hFF);
            end
        end
        check("color rows 2-49 copied to rows 1-48 through real port", ok);
        ok = 1;
        for (int c = 0; c < 80; c++)
            ok &= vgc_inst.text_inst.color_mem.mem[3920 + c] == 8'h0F;
        check("color bottom story row cleared through real port", ok);

        ok = 1;
        for (int r = 1; r < 49; r++) begin
            for (int c = 0; c < 80; c++) begin
                ok &= vgc_inst.text_inst.attr_mem.mem[(r * 80) + c] ==
                      8'((((r + 1) * 7) + c + 8'h80) & 8'hFF);
            end
        end
        check("attribute rows 2-49 copied to rows 1-48 through real port", ok);
        ok = 1;
        for (int c = 0; c < 80; c++)
            ok &= vgc_inst.text_inst.attr_mem.mem[3920 + c] == 8'h00;
        check("attribute bottom story row cleared through real port", ok);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #2000000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
