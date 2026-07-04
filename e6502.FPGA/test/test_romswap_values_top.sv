// Top-level ROMSWAP value regression.
//
// NovaHost loads the selected runtime into the primary ROM bank and the runtime
// extension into the extension bank. The FPGA therefore has only two physical
// banks: ROMSWAP_EXTENSION selects the extension bank, and every runtime value
// selects the primary bank. NovaLogo depends on ROMSWAP_LOGO ($05) returning
// from its extension trampoline; if hardware ignores $05, the CPU keeps
// fetching extension ROM bytes and LOGO DRAW corrupts the display.

`timescale 1ns/1ps

module test_romswap_values_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;
    logic        irq_n = 1, nmi_n = 1;

    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] audio_l, audio_r;

    logic        dbg_peek_en;
    logic [15:0] dbg_peek_addr;
    wire  [7:0]  dbg_peek_data;
    logic        dbg_poke_en;
    logic [15:0] dbg_poke_addr;
    logic [7:0]  dbg_poke_data;
    logic        dbg_pause;

    logic        dbg_rom_we;
    logic        dbg_rom_idx;
    logic [13:0] dbg_rom_addr;
    logic [7:0]  dbg_rom_data;
    logic        dbg_cpu_reset;
    logic        dbg_cpu_resume = 1'b0;

    wire  [15:0] dbg_cpu_pc;
    wire  [7:0]  dbg_cpu_a, dbg_cpu_x, dbg_cpu_y, dbg_cpu_sp, dbg_cpu_flags;
    wire  [5:0]  dbg_cpu_state;
    wire  [7:0]  dbg_cpu_ir;
    wire  [15:0] dbg_cpu_addr;
    wire  [7:0]  dbg_cpu_din, dbg_cpu_dout;
    wire         dbg_cpu_we, dbg_cpu_rdy, dbg_cpu_irq, dbg_cpu_nmi;
    wire         dbg_cpu_waiting, dbg_cpu_stopped;

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
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    task automatic wait_marker(input string name, input logic [15:0] addr,
                               input logic [7:0] expected,
                               input logic expected_ext_active);
        int cycles;
        cycles = 0;
        while (dut.main_ram.mem[addr] !== expected && cycles < 20000) begin
            @(negedge clk);
            cycles++;
        end
        check_eq8(name, dut.main_ram.mem[addr], expected);
        check($sformatf("%s bank state", name), dut.ext_rom_active == expected_ext_active);
    endtask

    localparam int PROG_LEN = 83;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h04, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h10, 8'h8D, 8'h00, 8'h02,
        8'hA9, 8'h01, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h11, 8'h8D, 8'h01, 8'h02,
        8'hA9, 8'h04, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h20, 8'h8D, 8'h02, 8'h02,
        8'hA9, 8'h02, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h21, 8'h8D, 8'h03, 8'h02,
        8'hA9, 8'h04, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h30, 8'h8D, 8'h04, 8'h02,
        8'hA9, 8'h03, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h31, 8'h8D, 8'h05, 8'h02,
        8'hA9, 8'h04, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h40, 8'h8D, 8'h06, 8'h02,
        8'hA9, 8'h05, 8'h8D, 8'h3F, 8'hA0, 8'hA9, 8'h41, 8'h8D, 8'h07, 8'h02,
        8'h4C, 8'h50, 8'hC0
    };

    task automatic load_bank(input logic idx);
        for (int i = 0; i < PROG_LEN; i++)
            rom_write(idx, 14'(i), prog[i]);
        rom_write(idx, 14'h3FFC, 8'h00);
        rom_write(idx, 14'h3FFD, 8'hC0);
    endtask

    initial begin
        $display("=== ROMSWAP value top-level regression ===");

        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;
        key_valid     = 0;
        key_data      = 0;
        dbg_peek_en   = 0;
        dbg_peek_addr = 0;
        dbg_poke_en   = 0;
        dbg_poke_addr = 0;
        dbg_poke_data = 0;
        dbg_rom_we    = 0;
        dbg_rom_idx   = 0;
        dbg_rom_addr  = 0;
        dbg_rom_data  = 0;

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        load_bank(1'b0);
        load_bank(1'b1);

        repeat(4) @(posedge clk);
        wait_vgc_ready();

        check("reset starts on primary ROM", dut.ext_rom_active == 1'b0);
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        wait_marker("ROMSWAP_EXTENSION selects extension", 16'h0200, 8'h10, 1'b1);
        wait_marker("ROMSWAP_EXTENSION selects extension again", 16'h0202, 8'h20, 1'b1);
        wait_marker("ROMSWAP_BASIC returns primary", 16'h0203, 8'h21, 1'b0);
        wait_marker("ROMSWAP_EXTENSION selects extension before LOGO", 16'h0206, 8'h40, 1'b1);
        wait_marker("ROMSWAP_LOGO returns primary", 16'h0207, 8'h41, 1'b0);

        repeat(2000) @(posedge clk);
        check("CPU reached halt loop", dbg_cpu_pc >= 16'hC050 && dbg_cpu_pc <= 16'hC052);
        check("final bank is primary", dut.ext_rom_active == 1'b0);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

    initial begin
        #50000000;
        $display("FAIL: global timeout hit, PC=0x%04X IR=0x%02X state=%0d ext=%0b",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dut.ext_rom_active);
        $finish(1);
    end

endmodule
