// Top-level WTS regression.
//
// Runs real 6502 stores through top.sv into the WTS MMIO bank, verifies debug
// reads see the same WTS state, and proves WTS audio reaches top-level HDMI
// audio outputs.

`timescale 1ns/1ps

module test_wts_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic key_valid = 1'b0;
    logic [7:0] key_data = 8'h00;
    logic irq_n = 1'b1;
    logic nmi_n = 1'b1;

    wire [3:0] vid_r;
    wire [3:0] vid_g;
    wire [3:0] vid_b;
    wire vid_hsync;
    wire vid_vsync;
    wire vid_de;
    wire signed [17:0] audio_l;
    wire signed [17:0] audio_r;

    logic dbg_peek_en;
    logic [15:0] dbg_peek_addr;
    wire [7:0] dbg_peek_data;
    logic dbg_poke_en;
    logic [15:0] dbg_poke_addr;
    logic [7:0] dbg_poke_data;
    logic dbg_pause;

    logic dbg_rom_we;
    logic dbg_rom_idx;
    logic [13:0] dbg_rom_addr;
    logic [7:0] dbg_rom_data;
    logic dbg_cpu_reset;
    logic dbg_cpu_resume = 1'b0;

    wire [15:0] dbg_cpu_pc;
    wire [7:0] dbg_cpu_a;
    wire [7:0] dbg_cpu_x;
    wire [7:0] dbg_cpu_y;
    wire [7:0] dbg_cpu_sp;
    wire [7:0] dbg_cpu_flags;
    wire [5:0] dbg_cpu_state;
    wire [7:0] dbg_cpu_ir;
    wire [15:0] dbg_cpu_addr;
    wire [7:0] dbg_cpu_din;
    wire [7:0] dbg_cpu_dout;
    wire dbg_cpu_we;
    wire dbg_cpu_rdy;
    wire dbg_cpu_irq;
    wire dbg_cpu_nmi;
    wire dbg_cpu_waiting;
    wire dbg_cpu_stopped;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .board_buttons(8'h00),
        .board_switches(8'h00),
        .usb_hid_status(8'h00),
        .usb_hid_device_type(8'h00),
        .usb_hid_last_scan(8'h00),
        .usb_hid_last_ascii(8'h00),
        .usb_hid_report_count(8'h00),
        .usb_hid_key_count(8'h00),
        .usb_hid_core_status(8'h00),
        .usb_hid_regs(64'h0),
        .irq_n(irq_n), .nmi_n(nmi_n),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(audio_l), .audio_r(audio_r),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_nic_buf_we(1'b0), .dbg_nic_buf_re(1'b0), .dbg_nic_buf_sel(1'b0),
        .dbg_nic_buf_addr(8'd0), .dbg_nic_buf_data(8'd0), .dbg_nic_buf_rdata(),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_system_reset(1'b0),
        .dbg_cpu_resume(dbg_cpu_resume),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0),
        .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .host_wts_event_we(1'b0), .host_wts_event_data(8'd0),
        .host_wts_event_ready(),
        .dbg_cpu_pc(dbg_cpu_pc),
        .dbg_cpu_a(dbg_cpu_a), .dbg_cpu_x(dbg_cpu_x),
        .dbg_cpu_y(dbg_cpu_y), .dbg_cpu_sp(dbg_cpu_sp),
        .dbg_cpu_flags(dbg_cpu_flags),
        .dbg_cpu_state(dbg_cpu_state),
        .dbg_cpu_ir(dbg_cpu_ir),
        .dbg_cpu_addr(dbg_cpu_addr),
        .dbg_cpu_din(dbg_cpu_din),
        .dbg_cpu_dout(dbg_cpu_dout),
        .dbg_cpu_we(dbg_cpu_we),
        .dbg_cpu_rdy(dbg_cpu_rdy),
        .dbg_cpu_irq(dbg_cpu_irq),
        .dbg_cpu_nmi(dbg_cpu_nmi),
        .dbg_cpu_waiting(dbg_cpu_waiting),
        .dbg_cpu_stopped(dbg_cpu_stopped),
        .sdram_clk(clk),
        .sdram_addrA(), .sdram_dinA(), .sdram_weA(), .sdram_oeA(),
        .sdram_doutA(8'h00), .sdram_doneA(1'b0),
        .sdram_addrB(), .sdram_dinB(), .sdram_weB(), .sdram_oeB(),
        .sdram_doutB(8'h00),
        // SDRAM stream port — page-in engine unused here; tie inputs off.
        .sdram_stream_req(), .sdram_stream_addr(), .sdram_stream_words(), .sdram_stream_ready(),
        .sdram_stream_dout(16'h0000), .sdram_stream_valid(1'b0), .sdram_stream_busy(1'b0), .sdram_stream_done(1'b0),
        .fio_event(),
        .nic_event()
    );

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

    task automatic rom_write(input logic idx, input logic [13:0] addr,
                             input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we <= 1'b1;
        @(posedge clk);
        dbg_rom_we <= 1'b0;
    endtask

    task automatic dbg_peek(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en <= 1'b1;
        repeat(4) @(posedge clk);
        data = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en <= 1'b0;
    endtask

    task automatic dbg_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en <= 1'b1;
        @(posedge clk);
        dbg_poke_en <= 1'b0;
        repeat(4) @(posedge clk);
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    function automatic int abs18(input logic signed [17:0] value);
        begin
            abs18 = value < 0 ? -int'(value) : int'(value);
        end
    endfunction

    localparam int PROG_LEN = 28;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h64, 8'h8D, 8'h41, 8'hA1, // velocity = 100
        8'hA9, 8'h00, 8'h8D, 8'h42, 8'hA1, // instrument = resident sine
        8'hA9, 8'hFF, 8'h8D, 8'h43, 8'hA1, // volume = 255
        8'hA9, 8'h00, 8'h8D, 8'h44, 8'hA1, // pan hard left
        8'hA9, 8'h45, 8'h8D, 8'h40, 8'hA1, // note-on A4
        8'h4C, 8'h19, 8'hC0              // loop forever
    };

    initial begin
        logic [7:0] d;
        int max_l;
        int max_r;

        $display("=== WTS top-level integration test ===");

        rst = 1'b1;
        dbg_pause = 1'b1;
        dbg_cpu_reset = 1'b1;
        dbg_peek_en = 1'b0;
        dbg_poke_en = 1'b0;
        dbg_rom_we = 1'b0;
        dbg_peek_addr = 16'h0000;
        dbg_poke_addr = 16'h0000;
        dbg_poke_data = 8'h00;
        dbg_rom_idx = 1'b0;
        dbg_rom_addr = 14'h0000;
        dbg_rom_data = 8'h00;

        repeat(20) @(posedge clk);
        rst = 1'b0;
        repeat(10) @(posedge clk);

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        repeat(4) @(posedge clk);
        wait_vgc_ready();

        dbg_cpu_reset = 1'b0;
        repeat(4) @(posedge clk);
        dbg_pause = 1'b0;
        repeat(60000) @(posedge clk);

        check("CPU reached WTS program loop",
              dbg_cpu_pc >= 16'hC019 && dbg_cpu_pc <= 16'hC01C);

        dbg_pause = 1'b1;
        repeat(8) @(posedge clk);

        dbg_peek(16'hA143, d);
        check_eq8("debug peek WTS voice volume after CPU store", d, 8'hFF);
        dbg_peek(16'hA144, d);
        check_eq8("debug peek WTS voice pan after CPU store", d, 8'h00);
        dbg_peek(16'hA147, d);
        check("debug peek WTS status sees active voice", d[0] === 1'b1);
        dbg_peek(16'hA187, d);
        check_eq8("debug peek WTS active mask", d, 8'h01);

        max_l = 0;
        max_r = 0;
        repeat(40000) begin
            @(posedge clk);
            if (abs18(audio_l) > max_l) max_l = abs18(audio_l);
            if (abs18(audio_r) > max_r) max_r = abs18(audio_r);
        end
        check("WTS contributes non-zero left audio through top", max_l > 0);
        check("hard-left WTS note keeps right output quiet", max_r == 0);

        dbg_poke(16'hA185, 8'h01);
        dbg_peek(16'hA187, d);
        check_eq8("debug poke all-notes-off clears active mask", d, 8'h00);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #30000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
