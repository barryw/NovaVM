// ROM-over-SID conflict test — verifies that opcode fetches into the
// $D400-$D43F range return ROM bytes, not SID register values.
//
// EhBASIC's array-handling code (LAB_1F45, LAB_1F48, LAB_1F7B, LAB_1F7C)
// happens to land at $D3FF-$D43F because the assembled ROM at offset
// $1400+ carries live code. The FPGA previously intercepted these
// addresses in the cpu_din mux (r_sid1_reg_sel / r_sid2_reg_sel /
// r_sid_cfg_sel) and returned SID register shadows for every read —
// including opcode fetches. CPU would execute the SID byte as an
// instruction, hit a $00 (BRK) or garbage, and silently drop back to
// the BASIC Ready prompt.
//
// This test writes distinct, non-matching bytes into the SID voice slots
// and the SID config register, then fetches the bytes at $D400, $D420,
// and $D440 via an LDA absolute + STA to $0200-$0202. If the SID-read
// intercept is wrong the stored bytes will NOT match the ROM.
//
// We rom_write the three target bytes so we know the exact expected ROM
// byte values (no reliance on the live EhBASIC layout).

`timescale 1ns/1ps

module test_rom_over_sid;

    logic clk = 0;
    always #20 clk = ~clk;  // ~25 MHz

    logic rst;

    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;
    logic        irq_n     = 1;
    logic        nmi_n     = 1;

    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] audio_l, audio_r;

    logic        dbg_peek_en   = 0;
    logic [15:0] dbg_peek_addr = 0;
    wire  [7:0]  dbg_peek_data;
    logic        dbg_poke_en   = 0;
    logic [15:0] dbg_poke_addr = 0;
    logic [7:0]  dbg_poke_data = 0;
    logic        dbg_pause;

    logic        dbg_rom_we    = 0;
    logic        dbg_rom_idx   = 0;
    logic [13:0] dbg_rom_addr  = 0;
    logic [7:0]  dbg_rom_data  = 0;
    logic        dbg_cpu_reset;

    wire  [15:0] dbg_cpu_pc;
    wire  [7:0]  dbg_cpu_a, dbg_cpu_x, dbg_cpu_y, dbg_cpu_sp, dbg_cpu_flags;

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
        .dbg_vmem_we(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .dbg_cpu_pc(dbg_cpu_pc),
        .dbg_cpu_a(dbg_cpu_a), .dbg_cpu_x(dbg_cpu_x),
        .dbg_cpu_y(dbg_cpu_y), .dbg_cpu_sp(dbg_cpu_sp),
        .dbg_cpu_flags(dbg_cpu_flags)
    );

    int pass = 0, fail = 0, test_num = 0;

    task automatic check8(string name, logic [7:0] actual, logic [7:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)", test_num, name, actual, expected);
            fail++;
        end
    endtask

    task automatic rom_write_byte(logic idx, logic [13:0] addr, logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    // Program at $C000 — write SID bytes, then LDA abs from ROM over SID,
    // STA to $0200-$0202.
    //
    //   C000: A9 11           LDA #$11
    //   C002: 8D 00 D4        STA $D400    ; SID1 voice 1 freq lo = $11
    //   C005: A9 22           LDA #$22
    //   C007: 8D 00 D5        STA $D500    ; SID2 mirror — write routes to SID2 base
    //                                        (voice 1 freq lo) = $22
    //   C00A: A9 33           LDA #$33
    //   C00C: 8D 40 D4        STA $D440    ; SID config reg = $33
    //
    //   C00F: AD 00 D4        LDA $D400    ; fetch ROM byte at $D400
    //   C012: 8D 00 02        STA $0200
    //   C015: AD 20 D4        LDA $D420    ; fetch ROM byte at $D420
    //   C018: 8D 01 02        STA $0201
    //   C01B: AD 40 D4        LDA $D440    ; fetch ROM byte at $D440
    //   C01E: 8D 02 02        STA $0202
    //
    //   C021: 4C 21 C0        JMP $C021   ; halt
    //
    // The test seeds ROM bytes at $D400=$AA, $D420=$BB, $D440=$CC so we
    // can verify the read returned ROM (not SID).
    localparam int PROG_LEN = 36;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h11, 8'h8D, 8'h00, 8'hD4,
        8'hA9, 8'h22, 8'h8D, 8'h00, 8'hD5,
        8'hA9, 8'h33, 8'h8D, 8'h40, 8'hD4,
        8'hAD, 8'h00, 8'hD4, 8'h8D, 8'h00, 8'h02,
        8'hAD, 8'h20, 8'hD4, 8'h8D, 8'h01, 8'h02,
        8'hAD, 8'h40, 8'hD4, 8'h8D, 8'h02, 8'h02,
        8'h4C, 8'h21, 8'hC0
    };

    initial begin
        $display("=== ROM-over-SID readback test ===");

        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        // Install program at $C000 (ROM offset 0 relative to base $C000).
        for (int i = 0; i < PROG_LEN; i++)
            rom_write_byte(1'b0, 14'(i), prog[i]);

        // Seed distinct ROM bytes at the three addresses we will read.
        // $D400 = ROM offset $1400, $D420 = $1420, $D440 = $1440.
        rom_write_byte(1'b0, 14'h1400, 8'hAA);
        rom_write_byte(1'b0, 14'h1420, 8'hBB);
        rom_write_byte(1'b0, 14'h1440, 8'hCC);

        // Reset vector → $C000
        rom_write_byte(1'b0, 14'h3FFC, 8'h00);
        rom_write_byte(1'b0, 14'h3FFD, 8'hC0);

        repeat(4) @(posedge clk);
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(5000) @(posedge clk);

        $display("Final CPU PC = 0x%04X  A=0x%02X", dbg_cpu_pc, dbg_cpu_a);
        test_num++;
        if (dbg_cpu_pc >= 16'hC021 && dbg_cpu_pc <= 16'hC023) begin
            $display("  PASS [%0d] CPU reached halt loop (PC=0x%04X)", test_num, dbg_cpu_pc);
            pass++;
        end else begin
            $display("  FAIL [%0d] CPU not in halt loop (PC=0x%04X, want 0xC021..23)", test_num, dbg_cpu_pc);
            fail++;
        end

        // The key assertions. Before the cpu_din-mux fix, these returned
        // $11 / $22 / $33 (the SID register values we wrote). After the
        // fix, they must return the ROM bytes $AA / $BB / $CC.
        check8("mem[$0200] == ROM[$D400] (=$AA)", dut.main_ram.mem[16'h0200], 8'hAA);
        check8("mem[$0201] == ROM[$D420] (=$BB)", dut.main_ram.mem[16'h0201], 8'hBB);
        check8("mem[$0202] == ROM[$D440] (=$CC)", dut.main_ram.mem[16'h0202], 8'hCC);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass, fail);
        if (fail == 0) $display("ALL TESTS PASSED");
        else           $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #50000000;
        $display("FAIL: global timeout hit, PC=0x%04X", dbg_cpu_pc);
        $finish;
    end

endmodule
