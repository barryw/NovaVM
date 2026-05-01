// Debug-peek regression test.
//
// Reproduces the hardware bug where peek of VGC-owned register addresses
// falls through to main RAM instead of returning the VGC's state. Text/color
// RAM is no longer directly mapped; $AA00 is intentionally main RAM now.
//
// Scaffolding mirrors test_rom_load.sv — tiny program that prints 'H','I' via
// REG_CHAROUT, then we pause the CPU and peek:
//   - $AA00:      main RAM, not VGC text RAM
//   - VGC reg:    $A003 → cursor_x after 2 printed chars
//   - VGC reg:    $A001 → bg_color reset default
//   - ROM:        $C000 → 0xA9      (LDA immediate opcode — control)
//   - main RAM:   $0100 → 0x00      (untouched — control)
//
// If peek path is correct, the char_mem dpram read-through and VGC register
// combinational path are both exercised in the same `ifdef SYNTHESIS` branch
// that the FPGA runs.

`timescale 1ns/1ps

module test_debug_peek;

    logic clk = 0;
    always #20 clk = ~clk;  // ~25 MHz

    logic rst;

    logic        key_valid;
    logic [7:0]  key_data;
    logic        irq_n = 1;
    logic        nmi_n = 1;

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
        .key_valid(key_valid), .key_data(key_data),
        .irq_n(irq_n), .nmi_n(nmi_n),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(audio_l), .audio_r(audio_r),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_system_reset(1'b0),
        .dbg_cpu_resume(dbg_cpu_resume),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
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
        .dbg_cpu_stopped(dbg_cpu_stopped)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;

    task automatic check_eq8(input string name, input logic [7:0] actual, input logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)", test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic rom_write(input logic idx, input logic [13:0] addr, input logic [7:0] data);
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

    // Issue a debug peek, wait for 2-cycle dpram latency + some settle,
    // return sampled dbg_peek_data. Leaves dbg_peek_en deasserted on exit.
    task automatic do_peek(input logic [15:0] addr, output logic [7:0] result);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en   <= 1;
        // Match debug_bridge S_PEEK_WAIT: 2 cycles of wait, then sample.
        @(posedge clk);
        @(posedge clk);
        result = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en   <= 0;
    endtask

    // Same program as test_rom_load.sv
    localparam int PROG_LEN = 13;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h48, 8'h8D, 8'h0E, 8'hA0,  // LDA #'H'; STA $A00E
        8'hA9, 8'h49, 8'h8D, 8'h0E, 8'hA0,  // LDA #'I'; STA $A00E
        8'h4C, 8'h0A, 8'hC0                  // JMP $C00A (halt)
    };

    logic [7:0] peek_val;

    initial begin
        $display("=== Debug-peek regression test ===");

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

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);

        repeat(4) @(posedge clk);
        wait_vgc_ready();

        // Release CPU, let program run
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;
        repeat(5000) @(posedge clk);

        // Sanity: PC should be in halt loop, char_mem should have 'H','I'
        $display("Final CPU PC = 0x%04X", dbg_cpu_pc);
        check_eq8("char_mem[0] == 'H'", dut.vgc_inst.text_inst.char_mem.mem[0], 8'h48);
        check_eq8("char_mem[1] == 'I'", dut.vgc_inst.text_inst.char_mem.mem[1], 8'h49);

        // Pause CPU for clean peek conditions
        dbg_pause = 1;
        repeat(10) @(posedge clk);

        // $AA00 is not VGC-owned anymore; it should fall through to main RAM.
        do_peek(16'hAA00, peek_val);
        check_eq8("peek \$AA00 returns main RAM", peek_val, 8'h00);

        // Peek VGC register — REG_CURSORX = $A003. After printing 2 chars cursor_x=2.
        do_peek(16'hA003, peek_val);
        check_eq8("peek \$A003 (cursor_x) == 2", peek_val, 8'h02);

        // Peek VGC register — REG_BGCOL = $A001. Default 0 per vgc.sv rst block.
        do_peek(16'hA001, peek_val);
        check_eq8("peek \$A001 (bg_color) == 0", peek_val, 8'h00);

        // Peek VGC register — REG_FGCOL = $A002. Default 15.
        do_peek(16'hA002, peek_val);
        check_eq8("peek \$A002 (fg_color) == 15", peek_val, 8'h0F);

        // Control: peek ROM — should return first opcode byte 0xA9
        do_peek(16'hC000, peek_val);
        check_eq8("peek \$C000 (ROM LDA opcode) == 0xA9", peek_val, 8'hA9);

        // Control: peek untouched main RAM — should be 0
        do_peek(16'h0100, peek_val);
        check_eq8("peek \$0100 (untouched RAM) == 0", peek_val, 8'h00);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #50000000;
        $display("FAIL: global timeout hit, PC=0x%04X", dbg_cpu_pc);
        $finish;
    end

endmodule
