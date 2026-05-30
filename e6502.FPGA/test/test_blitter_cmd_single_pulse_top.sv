// Top-level blitter command pulse regression.
//
// A CPU store to BLT_CMD must reach the blitter exactly once. The blitter then
// stalls the 6502 with RDY while the copy runs; the stopped CPU bus may still
// hold the write address/data, but top.sv must not replay that held write into
// the device on every half-speed cpu_ce tick.

`timescale 1ns/1ps

module test_blitter_cmd_single_pulse_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic dbg_peek_en;
    logic [15:0] dbg_peek_addr;
    wire  [7:0] dbg_peek_data;
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
    wire [7:0] dbg_cpu_a, dbg_cpu_x, dbg_cpu_y, dbg_cpu_sp, dbg_cpu_flags;
    wire [5:0] dbg_cpu_state;
    wire [7:0] dbg_cpu_ir;
    wire [15:0] dbg_cpu_addr;
    wire [7:0] dbg_cpu_din, dbg_cpu_dout;
    wire dbg_cpu_we, dbg_cpu_rdy, dbg_cpu_irq, dbg_cpu_nmi;
    wire dbg_cpu_waiting, dbg_cpu_stopped;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(1'b0), .key_data(8'h00), .key_ready(),
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
        .irq_n(1'b1), .nmi_n(1'b1),
        .vid_r(), .vid_g(), .vid_b(), .vid_hsync(), .vid_vsync(), .vid_de(),
        .audio_l(), .audio_r(),
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
        .fio_event(), .nic_event()
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    int blt_cmd_start_pulses = 0;

    always_ff @(posedge clk) begin
        if (dut.blt_bus_we && dut.blt_bus_addr == 16'hBA83 &&
            dut.blt_bus_wdata == 8'h01 && !dbg_poke_en)
            blt_cmd_start_pulses <= blt_cmd_start_pulses + 1;
    end

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

    task automatic check_eq_int(input string name, input int actual, input int expected);
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

    task automatic rom_write(input logic idx, input logic [13:0] addr,
                             input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we <= 1;
        @(posedge clk);
        dbg_rom_we <= 0;
    endtask

    task automatic dbg_peek(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en <= 1;
        repeat(4) @(posedge clk);
        data = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    localparam int PROG_LEN = 144;
    byte unsigned prog [PROG_LEN] = '{
        8'hA2, 8'h00,                         // LDX #0
        8'h8A,                                 // TXA
        8'h9D, 8'h00, 8'h20,                   // STA $2000,X
        8'hE8,                                 // INX
        8'hE0, 8'h40,                         // CPX #64
        8'hD0, 8'hF7,                         // BNE seed
        8'hA9, 8'h00, 8'h8D, 8'h86, 8'hBA,     // SRCSPACE = CPU
        8'h8D, 8'h87, 8'hBA,                   // DSTSPACE = CPU
        8'h8D, 8'h88, 8'hBA,                   // SRCL = $00
        8'hA9, 8'h20, 8'h8D, 8'h89, 8'hBA,     // SRCM = $20
        8'hA9, 8'h00, 8'h8D, 8'h8A, 8'hBA,     // SRCH = $00
        8'h8D, 8'h8B, 8'hBA,                   // DSTL = $00
        8'hA9, 8'h21, 8'h8D, 8'h8C, 8'hBA,     // DSTM = $21
        8'hA9, 8'h00, 8'h8D, 8'h8D, 8'hBA,     // DSTH = $00
        8'hA9, 8'h40, 8'h8D, 8'h8E, 8'hBA,     // WIDTHL = 64
        8'hA9, 8'h00, 8'h8D, 8'h8F, 8'hBA,     // WIDTHH = 0
        8'hA9, 8'h01, 8'h8D, 8'h90, 8'hBA,     // HEIGHTL = 1
        8'hA9, 8'h00, 8'h8D, 8'h91, 8'hBA,     // HEIGHTH = 0
        8'hA9, 8'h40, 8'h8D, 8'h92, 8'hBA,     // SRCSTRL = 64
        8'hA9, 8'h00, 8'h8D, 8'h93, 8'hBA,     // SRCSTRH = 0
        8'hA9, 8'h40, 8'h8D, 8'h94, 8'hBA,     // DSTSTRL = 64
        8'hA9, 8'h00, 8'h8D, 8'h95, 8'hBA,     // DSTSTRH = 0
        8'hA9, 8'h00, 8'h8D, 8'h96, 8'hBA,     // MODE = copy
        8'h8D, 8'h97, 8'hBA,                   // FILL = 0
        8'h8D, 8'h98, 8'hBA,                   // CKEY = 0
        8'hA9, 8'h01, 8'h8D, 8'h83, 8'hBA,     // BLT_CMD = START
        8'hAD, 8'h84, 8'hBA,                   // wait: LDA STATUS
        8'hC9, 8'h01,                           // CMP BUSY
        8'hF0, 8'hF9,                           // BEQ wait
        8'h8D, 8'h00, 8'h02,                   // STA $0200
        8'hAD, 8'h85, 8'hBA, 8'h8D, 8'h01, 8'h02, // ERR -> $0201
        8'hAD, 8'h00, 8'h21, 8'h8D, 8'h02, 8'h02, // first byte
        8'hAD, 8'h3F, 8'h21, 8'h8D, 8'h03, 8'h02, // last byte
        8'hAD, 8'h99, 8'hBA, 8'h8D, 8'h04, 8'h02, // COUNTL
        8'hAD, 8'h9A, 8'hBA, 8'h8D, 8'h05, 8'h02, // COUNTM
        8'h4C, 8'h8D, 8'hC0                    // JMP halt
    };

    // Logo keeps three 256-byte turtle buffers at $9C00..$9EFF and stores
    // turtle state immediately after them at $9F00. A 16x16 rotate blit must
    // write exactly 256 bytes into the rotated buffer and leave $9F00+ alone.
    localparam int ROT_PROG_LEN = 154;
    byte unsigned prog_rotate_guard [ROT_PROG_LEN] = '{
        8'hA2, 8'h00,                           // LDX #0
        8'h8A,                                   // seed: TXA
        8'h9D, 8'h00, 8'h9C,                     // STA $9C00,X
        8'hE8,                                   // INX
        8'hD0, 8'hF9,                             // BNE seed
        8'hA2, 8'h00,                             // LDX #0
        8'hA9, 8'hA5,                             // LDA #$A5
        8'h9D, 8'h00, 8'h9F,                     // guard: STA $9F00,X
        8'hE8,                                   // INX
        8'hE0, 8'h11,                             // CPX #17
        8'hD0, 8'hF8,                             // BNE guard
        8'hA9, 8'h00, 8'h8D, 8'h86, 8'hBA,       // SRCSPACE = CPU
        8'h8D, 8'h87, 8'hBA,                     // DSTSPACE = CPU
        8'h8D, 8'h88, 8'hBA,                     // SRCL = $00
        8'hA9, 8'h9C, 8'h8D, 8'h89, 8'hBA,       // SRCM = $9C
        8'hA9, 8'h00, 8'h8D, 8'h8A, 8'hBA,       // SRCH = $00
        8'h8D, 8'h8B, 8'hBA,                     // DSTL = $00
        8'hA9, 8'h9D, 8'h8D, 8'h8C, 8'hBA,       // DSTM = $9D
        8'hA9, 8'h00, 8'h8D, 8'h8D, 8'hBA,       // DSTH = $00
        8'hA9, 8'h10, 8'h8D, 8'h8E, 8'hBA,       // WIDTHL = 16
        8'hA9, 8'h00, 8'h8D, 8'h8F, 8'hBA,       // WIDTHH = 0
        8'hA9, 8'h10, 8'h8D, 8'h90, 8'hBA,       // HEIGHTL = 16
        8'hA9, 8'h00, 8'h8D, 8'h91, 8'hBA,       // HEIGHTH = 0
        8'hA9, 8'h10, 8'h8D, 8'h92, 8'hBA,       // SRCSTRL = 16
        8'hA9, 8'h00, 8'h8D, 8'h93, 8'hBA,       // SRCSTRH = 0
        8'hA9, 8'h10, 8'h8D, 8'h94, 8'hBA,       // DSTSTRL = 16
        8'hA9, 8'h00, 8'h8D, 8'h95, 8'hBA,       // DSTSTRH = 0
        8'h8D, 8'h98, 8'hBA,                     // CKEY = 0
        8'h8D, 8'hA2, 8'hBA,                     // ROTANGLE = 0
        8'hA9, 8'h04, 8'h8D, 8'h96, 8'hBA,       // MODE = rotate
        8'hA9, 8'h01, 8'h8D, 8'h83, 8'hBA,       // BLT_CMD = START
        8'hAD, 8'h84, 8'hBA,                     // wait: LDA STATUS
        8'hC9, 8'h01,                             // CMP BUSY
        8'hF0, 8'hF9,                             // BEQ wait
        8'h8D, 8'h10, 8'h02,                     // STA $0210
        8'hAD, 8'h85, 8'hBA, 8'h8D, 8'h11, 8'h02, // ERR -> $0211
        8'hAD, 8'h99, 8'hBA, 8'h8D, 8'h12, 8'h02, // COUNTL -> $0212
        8'hAD, 8'h9A, 8'hBA, 8'h8D, 8'h13, 8'h02, // COUNTM -> $0213
        8'hAD, 8'h00, 8'h9F, 8'h8D, 8'h14, 8'h02, // guard first
        8'hAD, 8'h10, 8'h9F, 8'h8D, 8'h15, 8'h02, // guard last
        8'h4C, 8'h97, 8'hC0                      // JMP halt
    };

    logic [7:0] peek_data;
    int pulse_baseline;

    initial begin
        $display("=== Blitter command single-pulse top regression ===");

        rst = 1;
        dbg_pause = 1;
        dbg_cpu_reset = 1;
        dbg_peek_en = 0;
        dbg_peek_addr = 0;
        dbg_poke_en = 0;
        dbg_poke_addr = 0;
        dbg_poke_data = 0;
        dbg_rom_we = 0;
        dbg_rom_idx = 0;
        dbg_rom_addr = 0;
        dbg_rom_data = 0;

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        wait_vgc_ready();

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(120000) @(posedge clk);

        $display("Final CPU PC = 0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X dout=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr,
                 dbg_cpu_din, dbg_cpu_dout);

        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC08D) && (dbg_cpu_pc <= 16'hC090));
        check_eq_int("BLT_CMD START reached blitter once",
                     blt_cmd_start_pulses, 1);

        dbg_peek(16'h0200, peek_data);
        check_eq8("CPU-observed BLT status is OK", peek_data, 8'h02);
        dbg_peek(16'h0201, peek_data);
        check_eq8("CPU-observed BLT error is NONE", peek_data, 8'h00);
        dbg_peek(16'h0202, peek_data);
        check_eq8("first copied byte landed", peek_data, 8'h00);
        dbg_peek(16'h0203, peek_data);
        check_eq8("last copied byte landed", peek_data, 8'h3F);
        dbg_peek(16'h0204, peek_data);
        check_eq8("count low is 64", peek_data, 8'h40);
        dbg_peek(16'h0205, peek_data);
        check_eq8("count high is 0", peek_data, 8'h00);

        dbg_pause = 1;
        dbg_cpu_reset = 1;
        repeat(8) @(posedge clk);

        for (int i = 0; i < ROT_PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog_rotate_guard[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        wait_vgc_ready();

        pulse_baseline = blt_cmd_start_pulses;
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(300000) @(posedge clk);

        $display("Rotate guard CPU PC = 0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X dout=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr,
                 dbg_cpu_din, dbg_cpu_dout);

        check("CPU reached rotate-guard halt loop",
              (dbg_cpu_pc >= 16'hC097) && (dbg_cpu_pc <= 16'hC09A));
        check_eq_int("rotate BLT_CMD START reached blitter once",
                     blt_cmd_start_pulses - pulse_baseline, 1);

        dbg_peek(16'h0210, peek_data);
        check_eq8("rotate status is OK", peek_data, 8'h02);
        dbg_peek(16'h0211, peek_data);
        check_eq8("rotate error is NONE", peek_data, 8'h00);
        dbg_peek(16'h0212, peek_data);
        check_eq8("rotate count low is 0", peek_data, 8'h00);
        dbg_peek(16'h0213, peek_data);
        check_eq8("rotate count middle is 1", peek_data, 8'h01);
        dbg_peek(16'h0214, peek_data);
        check_eq8("CPU-observed first turtle-state guard survived", peek_data, 8'hA5);
        dbg_peek(16'h0215, peek_data);
        check_eq8("CPU-observed last turtle-state guard survived", peek_data, 8'hA5);
        dbg_peek(16'h9F00, peek_data);
        check_eq8("first turtle-state byte not overwritten by rotate", peek_data, 8'hA5);
        dbg_peek(16'h9F10, peek_data);
        check_eq8("last turtle-state guard byte not overwritten by rotate", peek_data, 8'hA5);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
