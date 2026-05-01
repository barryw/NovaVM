// ROM-load-via-debug-bridge end-to-end test.
//
// Proves that a runtime-loaded ROM works the same as the compile-time-baked
// ROM today: pump bytes via `dbg_rom_we`, release reset, CPU fetches from the
// loaded BRAM and executes.
//
// Success = tiny 6502 program written at $C000 + reset vector at $FFFC causes
// 'H' and 'I' to appear at char_mem[0] and char_mem[1].
//
// This validates the exact path NovaHost will use to stream ehbasic.bin +
// extension.bin at boot. If this test passes in Verilator, we do ONE 17-min
// hardware build and from then on ROM iteration is seconds.

`timescale 1ns/1ps

module test_rom_load;

    // -----------------------------------------------------------------------
    // Clock / reset
    // -----------------------------------------------------------------------
    logic clk = 0;
    always #20 clk = ~clk;  // ~25 MHz

    logic rst;

    // -----------------------------------------------------------------------
    // DUT ports
    // -----------------------------------------------------------------------
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

    // -----------------------------------------------------------------------
    // Test accounting
    // -----------------------------------------------------------------------
    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;

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

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------
    task automatic rom_write(input logic idx, input logic [13:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    // -----------------------------------------------------------------------
    // Tiny 6502 program to load into basic_rom:
    //   $C000: A9 48       LDA #'H'
    //   $C002: 8D 0E A0    STA $A00E      ; VGC RegCharOut
    //   $C005: A9 49       LDA #'I'
    //   $C007: 8D 0E A0    STA $A00E
    //   $C00A: 4C 0A C0    JMP $C00A      ; halt loop
    //   $FFFC: 00 C0       ; reset vector
    // -----------------------------------------------------------------------
    localparam int PROG_LEN = 13;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h48, 8'h8D, 8'h0E, 8'hA0,
        8'hA9, 8'h49, 8'h8D, 8'h0E, 8'hA0,
        8'h4C, 8'h0A, 8'hC0
    };

    initial begin
        $display("=== ROM load end-to-end test ===");

        // Initial state — hold everything in reset. dbg_cpu_reset keeps the
        // CPU held even after top-level rst releases; that gap is where we
        // load the ROM bytes.
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
        rst = 0;                 // top-level reset released; dbg_cpu_reset still holding CPU
        repeat(10) @(posedge clk);

        // Load program into basic_rom starting at offset 0x0000 (CPU $C000)
        $display("Loading %0d bytes into basic_rom @ offset 0x0000", PROG_LEN);
        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);

        // Reset vector at $FFFC (offset 0x3FFC/3FFD): low=0x00, high=0xC0
        $display("Writing reset vector @ 0x3FFC = $C000");
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);

        repeat(4) @(posedge clk);

        // Sanity — verify bytes landed in the dpram
        check_eq8("basic_rom[0x0000] after load",
                  dut.basic_rom_inst.mem[14'h0000], 8'hA9);
        check_eq8("basic_rom[0x000C] after load",
                  dut.basic_rom_inst.mem[14'h000C], 8'hC0);
        check_eq8("basic_rom[0x3FFC] reset lo",
                  dut.basic_rom_inst.mem[14'h3FFC], 8'h00);
        check_eq8("basic_rom[0x3FFD] reset hi",
                  dut.basic_rom_inst.mem[14'h3FFD], 8'hC0);

        // Release CPU reset → CPU fetches the (now populated) reset vector.
        // Then release pause so clock-enable advances.
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        // Let it execute. The CPU under -DSYNTHESIS runs at half clock via
        // cpu_ce; under the sim branch it runs every edge. Either way 100k
        // cycles is hilariously long for 13 bytes of program.
        repeat(100000) @(posedge clk);

        // -----------------------------------------------------------------------
        // Assertions
        // -----------------------------------------------------------------------
        $display("Final CPU PC = 0x%04X (expect in $C00A-$C00C loop)", dbg_cpu_pc);
        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC00A) && (dbg_cpu_pc <= 16'hC00D));

        check_eq8("char_mem[0] == 'H'",
                  dut.vgc_inst.text_inst.char_mem.mem[0], 8'h48);
        check_eq8("char_mem[1] == 'I'",
                  dut.vgc_inst.text_inst.char_mem.mem[1], 8'h49);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    // Global safety timeout
    initial begin
        #50000000;  // 50 ms sim time
        $display("FAIL: global timeout hit, PC=0x%04X", dbg_cpu_pc);
        $finish;
    end

endmodule
