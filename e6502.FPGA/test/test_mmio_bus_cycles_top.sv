// Top-level MMIO bus-cycle regression.
//
// Runs real 6502 code against the synthesized top.sv path and writes several
// memory-mapped peripherals through the addressing modes most likely to expose
// repeated-address or dummy-cycle bugs:
//   - STA ($zp),Y, including a page-crossed pointer
//   - STA ($zp,X)
//   - STA abs,X
//   - INC abs read-modify-write
//
// The checks look at the peripheral side effects, not just the CPU bus, so this
// catches decode/latch mistakes in the device integration.

`timescale 1ns/1ps

module test_mmio_bus_cycles_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;
    logic [7:0]  board_buttons = 8'h55;
    logic [7:0]  board_switches = 8'h0A;
    logic [7:0]  usb_hid_status = 8'hA5;
    logic [7:0]  usb_hid_device_type = 8'h01;
    logic [7:0]  usb_hid_last_scan = 8'h04;
    logic [7:0]  usb_hid_last_ascii = 8'h41;
    logic [7:0]  usb_hid_report_count = 8'h11;
    logic [7:0]  usb_hid_key_count = 8'h5A;
    logic [7:0]  usb_hid_core_status = 8'hD3;
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

    wire fio_event;
    logic [7:0] peek_data;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .board_buttons(board_buttons),
        .board_switches(board_switches),
        .usb_hid_status(usb_hid_status),
        .usb_hid_device_type(usb_hid_device_type),
        .usb_hid_last_scan(usb_hid_last_scan),
        .usb_hid_last_ascii(usb_hid_last_ascii),
        .usb_hid_report_count(usb_hid_report_count),
        .usb_hid_key_count(usb_hid_key_count),
        .usb_hid_core_status(usb_hid_core_status),
        .usb_hid_regs(64'h1122334455667788),
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
        .fio_event(fio_event),
        .nic_event()
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;
    int event_count = 0;

    always_ff @(posedge clk) begin
        if (fio_event)
            event_count <= event_count + 1;
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
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    task automatic dbg_peek(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en   <= 1;
        repeat(4) @(posedge clk);
        data = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en   <= 0;
    endtask

    task automatic dbg_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en   <= 1;
        @(posedge clk);
        dbg_poke_en   <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    localparam int PROG_LEN = 126;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h9E, 8'h85, 8'h10,         // $10/$11 = $B99E
        8'hA9, 8'hB9, 8'h85, 8'h11,
        8'hA0, 8'h02,                       // Y=2 -> $B9A0
        8'hA9, 8'h21, 8'h91, 8'h10,         // STA ($10),Y -> FioCmd
        8'hA9, 8'hA1, 8'h85, 8'h12,         // $12/$13 = $B9A1
        8'hA9, 8'hB9, 8'h85, 8'h13,
        8'hA2, 8'h00, 8'hA9, 8'h34,
        8'h81, 8'h12,                       // STA ($12,X) -> FioStatus
        8'hEE, 8'hA1, 8'hB9,               // INC $B9A1 -> FioStatus
        8'hA9, 8'h00, 8'h85, 8'h14,         // $14/$15 = $A000
        8'hA9, 8'hA0, 8'h85, 8'h15,
        8'hA0, 8'h01, 8'hA9, 8'h05,
        8'h91, 8'h14,                       // STA ($14),Y -> VGC bg
        8'hA2, 8'h02, 8'hA9, 8'h06,
        8'h9D, 8'h00, 8'hA0,               // STA $A000,X -> VGC fg
        8'hEE, 8'h01, 8'hA0,               // INC $A001 -> VGC bg
        8'hA9, 8'hFE, 8'h85, 8'h16,         // $16/$17 = $B9FE
        8'hA9, 8'hB9, 8'h85, 8'h17,
        8'hA0, 8'h02, 8'hA9, 8'h22,
        8'h91, 8'h16,                       // STA ($16),Y -> $BA00
        8'hEE, 8'h00, 8'hBA,               // INC $BA00
        8'hA9, 8'h64, 8'h85, 8'h18,         // $18/$19 = $BA64
        8'hA9, 8'hBA, 8'h85, 8'h19,
        8'hA0, 8'h02, 8'hA9, 8'h05,
        8'h91, 8'h18,                       // STA ($18),Y -> DMA srcspace
        8'hEE, 8'h66, 8'hBA,               // INC $BA66
        8'hA2, 8'h03, 8'hA9, 8'h06,
        8'h9D, 8'h83, 8'hBA,               // STA $BA83,X -> blitter srcspace
        8'hEE, 8'h86, 8'hBA,               // INC $BA86
        8'hAD, 8'h9C, 8'hBA,               // LDA $BA9C -> board buttons
        8'h8D, 8'h00, 8'h04,               // STA $0400
        8'hAD, 8'h9D, 8'hBA,               // LDA $BA9D -> board switches
        8'h8D, 8'h01, 8'h04,               // STA $0401
        8'hAD, 8'hA3, 8'hBA,               // LDA $BAA3 -> USB HID status
        8'h8D, 8'h02, 8'h04,               // STA $0402
        8'hAD, 8'hA8, 8'hBA,               // LDA $BAA8 -> USB HID key count
        8'h8D, 8'h03, 8'h04,               // STA $0403
        8'h4C, 8'h7B, 8'hC0                // JMP $C07B
    };

    initial begin
        $display("=== MMIO bus-cycle top-level regression ===");

        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;
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

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        repeat(4) @(posedge clk);
        wait_vgc_ready();

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(100000) @(posedge clk);

        $display("Final CPU PC = 0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X dout=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr, dbg_cpu_din, dbg_cpu_dout);

        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC07B) && (dbg_cpu_pc <= 16'hC07E));

        check_eq_int("FioCmd write fired exactly one event", event_count, 1);
        check_eq8("FioCmd via STA ($zp),Y", dut.fio_inst.bank[7'h00], 8'h21);
        check_eq8("FioStatus via STA ($zp,X) then INC", dut.fio_inst.bank[7'h01], 8'h35);

        check_eq8("VGC bg via STA ($zp),Y then INC",
                  {4'h0, dut.vgc_inst.bg_color}, 8'h06);
        check_eq8("VGC fg via STA abs,X",
                  {4'h0, dut.vgc_inst.fg_color}, 8'h06);

        check_eq8("XMC reg via page-crossed STA ($zp),Y then INC",
                  dut.xmc_regs[6'h00], 8'h23);
        check_eq8("DMA srcspace via STA ($zp),Y then INC",
                  dut.dma_inst.regs[3], 8'h06);
        check_eq8("Blitter srcspace via STA abs,X then INC",
                  dut.blt_inst.regs[3], 8'h07);
        dbg_peek(16'h0400, peek_data);
        check_eq8("Board buttons are CPU-visible", peek_data, 8'h55);
        dbg_peek(16'h0401, peek_data);
        check_eq8("Board switches are CPU-visible", peek_data, 8'h0A);
        dbg_peek(16'h0402, peek_data);
        check_eq8("USB HID status is CPU-visible", peek_data, 8'hA5);
        dbg_peek(16'h0403, peek_data);
        check_eq8("USB HID key count is CPU-visible", peek_data, 8'h5A);
        dbg_peek(16'hBAA9, peek_data);
        check_eq8("USB HID core status is debug-visible", peek_data, 8'hD3);
        dbg_peek(16'hBAAA, peek_data);
        check_eq8("USB HID VID low is debug-visible", peek_data, 8'h88);
        dbg_peek(16'hBAAF, peek_data);
        check_eq8("USB HID protocol is debug-visible", peek_data, 8'h22);

        dbg_pause = 1;
        board_buttons = 8'h54;
        repeat(4) @(posedge clk);
        dbg_peek(16'hBA9F, peek_data);
        check_eq8("Board button change latches IRQ status", peek_data, 8'h01);
        dbg_peek(16'hBAA0, peek_data);
        check_eq8("Board button change mask latches changed bit", peek_data, 8'h01);
        dbg_poke(16'hBA9E, 8'h01);
        repeat(4) @(posedge clk);
        check("Board input IRQ asserts when enabled", dbg_cpu_irq === 1'b1);
        dbg_poke(16'hBAA0, 8'h01);
        dbg_poke(16'hBA9F, 8'h01);
        repeat(4) @(posedge clk);
        dbg_peek(16'hBA9F, peek_data);
        check_eq8("Board input IRQ status is W1C", peek_data, 8'h00);
        check("Board input IRQ deasserts after ack", dbg_cpu_irq === 1'b0);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
