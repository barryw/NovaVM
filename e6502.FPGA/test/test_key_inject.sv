// Key-injection FIFO ordering/capacity test.
//
// Verifies vgc.sv's keyboard input replaces the old single-entry char_in_reg
// with a 256-byte ring buffer (ZipCPU sfifo). Rapid injection of N keys
// followed by N CPU reads of $A00F must yield the keys back in FIFO order,
// not just the last one.
//
// Test program stored in ROM @ $C000:
//
//   loop: LDA $A00F       ; read key
//         BEQ loop        ; if empty, spin
//         STA $0200,X     ; store at $0200 + X
//         INX             ; next slot
//         JMP loop        ; forever
//
// The test injects keys one at a time with clock ticks between, then cycles
// the CPU enough to drain them into the $0200+ page. Finally reads RAM via
// peek to verify order.

`timescale 1ns/1ps

module test_key_inject;

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
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0), .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
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

    task automatic inject_key(input logic [7:0] ch);
        @(posedge clk);
        key_data  <= ch;
        key_valid <= 1;
        @(posedge clk);
        key_valid <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    // Drain program: reads $A00F, if non-zero stores at $0200,X then X++
    //   $C000: AD 0F A0       LDA $A00F
    //   $C003: F0 FB          BEQ $C000 (back 5)
    //   $C005: 9D 00 02       STA $0200,X
    //   $C008: E8             INX
    //   $C009: 4C 00 C0       JMP $C000
    //   $FFFC: 00 C0          reset vector
    localparam int PROG_LEN = 12;
    byte unsigned prog [PROG_LEN] = '{
        8'hAD, 8'h0F, 8'hA0,
        8'hF0, 8'hFB,
        8'h9D, 8'h00, 8'h02,
        8'hE8,
        8'h4C, 8'h00, 8'hC0
    };

    initial begin
        $display("=== Key FIFO ordering/capacity test ===");

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

        // Inject BEFORE releasing CPU — FIFO holds them until CPU reads
        $display("-- Injecting 'A','B','C','D','E' while CPU is held in reset --");
        inject_key(8'h41);
        inject_key(8'h42);
        inject_key(8'h43);
        inject_key(8'h44);
        inject_key(8'h45);

        repeat(5) @(posedge clk);
        $display("FIFO fill after 5 injects (expect 5): %0d",
                 dut.vgc_inst.key_fifo_inst.o_fill);

        // Now release CPU and let program drain
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        // Trace the first ~150 cycles so we can see LDA $A00F behavior.
        $display("cyc   | pc    ce re addr  din  vgc_rd r_vgc  fifo_rd head empty fill | a  x  pc_charin_prev");
        for (int c = 0; c < 150; c++) begin
            $display("%04d  | %04X  %0d  %0d  %04X  %02X   %02X    %02X     %0d       %02X   %0d     %0d | %02X %02X",
                     c,
                     dbg_cpu_pc,
                     dut.cpu_ce,
                     dut.cpu_inst.WE == 0 ? 1 : 0,
                     dut.cpu_addr,
                     dut.cpu_din,
                     dut.vgc_cpu_rdata,
                     dut.r_vgc_cpu_rdata,
                     dut.vgc_inst.key_fifo_rd,
                     dut.vgc_inst.key_fifo_head,
                     dut.vgc_inst.key_fifo_empty,
                     dut.vgc_inst.key_fifo_inst.o_fill,
                     dbg_cpu_a,
                     dbg_cpu_x);
            @(posedge clk);
        end

        repeat(30000) @(posedge clk);

        $display("CPU PC after drain cycles: 0x%04X  X=0x%02X  A=0x%02X",
                 dbg_cpu_pc, dbg_cpu_x, dbg_cpu_a);
        $display("FIFO fill after drain (expect 0): %0d",
                 dut.vgc_inst.key_fifo_inst.o_fill);
        $display("-- Reading drain buffer at $0200.. --");
        check_eq8("buf[0] == 'A'", dut.main_ram.mem[16'h0200], 8'h41);
        check_eq8("buf[1] == 'B'", dut.main_ram.mem[16'h0201], 8'h42);
        check_eq8("buf[2] == 'C'", dut.main_ram.mem[16'h0202], 8'h43);
        check_eq8("buf[3] == 'D'", dut.main_ram.mem[16'h0203], 8'h44);
        check_eq8("buf[4] == 'E'", dut.main_ram.mem[16'h0204], 8'h45);

        // Now inject a burst of 50 rapid keys and verify ordering preserved
        $display("-- Injecting 50 more keys mid-run without pacing --");
        dbg_pause = 1;
        repeat(5) @(posedge clk);
        for (int i = 0; i < 50; i++)
            inject_key(8'h30 + 8'(i));  // printable ASCII range
        dbg_pause = 0;

        repeat(100000) @(posedge clk);

        $display("FIFO fill after burst drain (expect 0): %0d",
                 dut.vgc_inst.key_fifo_inst.o_fill);
        $display("CPU state after burst: PC=0x%04X X=0x%02X A=0x%02X",
                 dbg_cpu_pc, dbg_cpu_x, dbg_cpu_a);

        $display("-- Verifying all 50 landed in order --");
        begin
            automatic int first_fail = -1;
            // NOTE: `automatic` is required here. Without it the locals inherit
            // static lifetime from the enclosing initial block, and static-
            // variable declaration initializers only fire once at time 0 —
            // so `got` and `exp` would stay locked at their time-0 values and
            // every iteration would spuriously mismatch.
            // Injected range is 0x30..0x61 (i=0..49), all printable ASCII.
            for (int i = 0; i < 50; i++) begin
                automatic logic [7:0] injected = 8'h30 + 8'(i);
                automatic logic [7:0] exp = injected;
                automatic logic [7:0] got = dut.main_ram.mem[16'(16'h0205 + i)];
                if (got != exp) begin
                    if (first_fail < 0) first_fail = i;
                end
            end
            test_num++;
            if (first_fail < 0) begin
                $display("  PASS [%0d] all 50 bursted keys landed in order (incl DEL->BS xlat)", test_num);
                pass_count++;
            end else begin
                automatic logic [7:0] fail_inj = 8'h30 + 8'(first_fail);
                automatic logic [7:0] fail_exp = fail_inj;
                $display("  FAIL [%0d] first mismatch at i=%0d (got 0x%02X want 0x%02X)",
                    test_num, first_fail,
                    dut.main_ram.mem[16'(16'h0205 + first_fail)],
                    fail_exp);
                fail_count++;
            end
        end

        $display("-- Injecting non-ASCII/NUL noise plus DEL and 'Z' --");
        dbg_pause = 1;
        repeat(5) @(posedge clk);
        inject_key(8'hF0);
        inject_key(8'h80);
        inject_key(8'h00);
        inject_key(8'h7F);
        inject_key(8'h5A);
        dbg_pause = 0;

        repeat(30000) @(posedge clk);

        check_eq8("buf[55] == DEL translated to BS", dut.main_ram.mem[16'h0237], 8'h08);
        check_eq8("buf[56] == 'Z'", dut.main_ram.mem[16'h0238], 8'h5A);
        check_eq8("X advanced only for accepted keys", dbg_cpu_x, 8'h39);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #200000000;
        $display("FAIL: global timeout hit");
        $finish;
    end

endmodule
