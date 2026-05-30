// Regression for synthesized top.sv CHARIN timing.
//
// In hardware, VGC read data is driven from cpu_raddr, a held MMIO address.
// live cpu_addr can already have advanced by the time the CHARIN byte is
// presented. CHARIN side effects must therefore key off cpu_raddr/read_active,
// not the live cpu_addr bus.

`timescale 1ns/1ps

module test_vgc_charin_read_timing;

    localparam logic [15:0] REG_CHARIN_A = 16'hA00F;

    logic clk = 0;
    always #20 clk = ~clk;

    logic        rst;
    logic        cpu_ce;
    logic [15:0] cpu_addr;
    logic [15:0] cpu_raddr;
    logic [7:0]  cpu_wdata;
    wire [7:0]   cpu_rdata;
    logic        cpu_we;
    logic        cpu_re;
    logic        key_valid;
    logic [7:0]  key_data;

    wire [7:0] dbg_rdata;
    wire [7:0] dbg_vmem_rdata;
    wire [3:0] vid_r, vid_g, vid_b;
    wire       vid_hsync, vid_vsync, vid_de;
    wire       irq_out;
    wire       vgc_rdy;
    wire       sys_reset_req;

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

    vgc dut (
        .clk(clk), .rst(rst), .video_rst(rst),
        .cpu_ce(cpu_ce),
        .cpu_addr(cpu_addr), .cpu_raddr(cpu_raddr),
        .cpu_wdata(cpu_wdata), .cpu_rdata(cpu_rdata),
        .cpu_we(cpu_we), .cpu_re(cpu_re),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .blt_space(3'd0), .blt_addr(16'd0), .blt_rdata(),
        .blt_wdata(8'd0), .blt_we(1'b0), .blt_re(1'b0),
        .video_blit_safe(),
        .dbg_addr(16'd0), .dbg_rdata(dbg_rdata),
        .dbg_we(1'b0), .dbg_waddr(16'd0), .dbg_wdata(8'd0),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0),
        .dbg_vmem_space(3'd0), .dbg_vmem_addr(17'd0),
        .dbg_vmem_wdata(8'd0), .dbg_vmem_rdata(dbg_vmem_rdata),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .irq_out(irq_out), .rdy_out(vgc_rdy),
        .sys_reset_req(sys_reset_req)
    );

    task automatic check_eq8(input string name, input logic [7:0] actual,
                             input logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)",
                     test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic check_eq_int(input string name, input int actual,
                                input int expected);
        test_num++;
        if (actual == expected) begin
            $display("  PASS [%0d] %s (=%0d)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got %0d, want %0d)",
                     test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic inject_key(input logic [7:0] value);
        @(posedge clk);
        key_data <= value;
        key_valid <= 1'b1;
        @(posedge clk);
        key_valid <= 1'b0;
        repeat (2) @(posedge clk);
    endtask

    task automatic held_charin_read(input logic [15:0] live_addr,
                                    output logic [7:0] value);
        @(posedge clk);
        cpu_addr <= live_addr;
        cpu_raddr <= REG_CHARIN_A;
        cpu_we <= 1'b0;
        cpu_re <= 1'b1;
        cpu_ce <= 1'b1;
        #1 value = cpu_rdata;

        @(posedge clk);
        cpu_ce <= 1'b0;
        cpu_re <= 1'b0;
        cpu_addr <= live_addr + 16'd1;
        cpu_raddr <= REG_CHARIN_A;

        repeat (4) @(posedge clk);
    endtask

    initial begin
        logic [7:0] value;

        rst = 1'b1;
        cpu_ce = 1'b0;
        cpu_addr = 16'h0000;
        cpu_raddr = 16'h0000;
        cpu_wdata = 8'h00;
        cpu_we = 1'b0;
        cpu_re = 1'b0;
        key_valid = 1'b0;
        key_data = 8'h00;

        repeat (50) @(posedge clk);
        rst = 1'b0;
        while (!vgc_rdy) @(posedge clk);
        repeat (10) @(posedge clk);

        inject_key(8'h46); // F
        inject_key(8'h44); // D
        check_eq_int("two injected keys are queued",
                     int'(dut.key_fifo_inst.o_fill), 2);

        held_charin_read(16'hC000, value);
        check_eq8("first held-address CHARIN read returns F", value, 8'h46);
        check_eq_int("first held-address CHARIN read pops one key",
                     int'(dut.key_fifo_inst.o_fill), 1);

        held_charin_read(16'hC010, value);
        check_eq8("second held-address CHARIN read returns D", value, 8'h44);
        check_eq_int("second held-address CHARIN read drains FIFO",
                     int'(dut.key_fifo_inst.o_fill), 0);

        if (fail_count == 0) begin
            $display("All %0d VGC CHARIN timing tests passed", pass_count);
            $finish;
        end else begin
            $display("%0d VGC CHARIN timing tests failed out of %0d",
                     fail_count, pass_count + fail_count);
            $fatal;
        end
    end

endmodule
