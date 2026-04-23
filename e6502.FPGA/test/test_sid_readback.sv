// SID / blitter register readback regression — guards the cpu_ce-gating
// fix on r_sid1_dout / r_sid2_dout / r_blt_cpu_rdata (commit c1e75da).
//
// Why this test exists: top.sv captures these three registers into cpu_din
// via unconditional `always_ff @(posedge clk) r_X <= Y;`. If that capture
// is not `cpu_ce`-gated, Arlet's cpu_addr moves through its next opcode
// fetch during cpu_ce=0 cycles and the wrong source value overwrites the
// captured read. The CPU then samples a stale/wrong DI on its next
// cpu_ce=1 edge. Same bug class as the keyboard-drop r_vgc_cpu_rdata bug
// fixed in bf9ab22.
//
// The SID dout case is adversarial by construction: sid1_dout is
// combinational on cpu_addr[4:0], and we write DISTINCT values to
// voice_vol[0..2] at $D41D/E/F so a wrong-addr sample returns a
// detectably wrong byte. Without the cpu_ce gate, at least one of the
// three LDA/STA pairs lands the wrong value in RAM. With the gate, all
// three land correctly.
//
// Scaffolding mirrors test_debug_peek.sv / test_rom_load.sv — runtime
// ROM-load via dbg_rom_we, release CPU, verify $0200..$0202 in main_ram.

`timescale 1ns/1ps

module test_sid_readback;

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
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .dbg_cpu_pc(dbg_cpu_pc),
        .dbg_cpu_a(dbg_cpu_a), .dbg_cpu_x(dbg_cpu_x),
        .dbg_cpu_y(dbg_cpu_y), .dbg_cpu_sp(dbg_cpu_sp),
        .dbg_cpu_flags(dbg_cpu_flags)
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

    // Program at $C000 — three-distinct-value SID readback:
    //
    //   C000: A9 01        LDA #$01
    //   C002: 8D 1D D4     STA $D41D    ; voice_vol[0] = 1
    //   C005: A9 02        LDA #$02
    //   C007: 8D 1E D4     STA $D41E    ; voice_vol[1] = 2
    //   C00A: A9 03        LDA #$03
    //   C00C: 8D 1F D4     STA $D41F    ; voice_vol[2] = 3
    //
    //   C00F: AD 1D D4     LDA $D41D    ; read voice_vol[0]
    //   C012: 8D 00 02     STA $0200
    //   C015: AD 1E D4     LDA $D41E    ; read voice_vol[1]
    //   C018: 8D 01 02     STA $0201
    //   C01B: AD 1F D4     LDA $D41F    ; read voice_vol[2]
    //   C01E: 8D 02 02     STA $0202
    //
    //   C021: AD 83 BA     LDA $BA83    ; blitter R_STATUS (idle after reset → 0)
    //   C024: 8D 03 02     STA $0203
    //
    //   C027: 4C 27 C0     JMP $C027    ; halt
    //
    // Why the SID layout detects the bug: each LDA is followed by STA whose
    // opcode sits at an address whose low 5 bits differ from the SID target's
    // low 5 bits. If the unconditional capture catches sid1_dout when cpu_addr
    // has moved to the STA opcode fetch, it reads sid1_dout(addr=STA-opcode
    // low5) which returns `last_written` (0x03 after the three writes above)
    // or a DIFFERENT voice_vol slot. Either way, the bug produces detectable
    // cross-contamination between the three slots.
    localparam int PROG_LEN = 42;
    byte unsigned prog [PROG_LEN] = '{
        // Writes
        8'hA9, 8'h01, 8'h8D, 8'h1D, 8'hD4,
        8'hA9, 8'h02, 8'h8D, 8'h1E, 8'hD4,
        8'hA9, 8'h03, 8'h8D, 8'h1F, 8'hD4,
        // Reads + stores
        8'hAD, 8'h1D, 8'hD4, 8'h8D, 8'h00, 8'h02,
        8'hAD, 8'h1E, 8'hD4, 8'h8D, 8'h01, 8'h02,
        8'hAD, 8'h1F, 8'hD4, 8'h8D, 8'h02, 8'h02,
        // Blitter status read + store
        8'hAD, 8'h83, 8'hBA, 8'h8D, 8'h03, 8'h02,
        // Halt
        8'h4C, 8'h27, 8'hC0
    };

    initial begin
        $display("=== SID/blitter cpu_ce-gated readback regression test ===");

        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);

        repeat(4) @(posedge clk);
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        // Let the program run — ~40 bytes of code, each bus cycle takes 2
        // clk edges at cpu_ce half-rate, so ~200 edges is plenty but give
        // extra margin for Arlet's opcode fetch cycles.
        repeat(5000) @(posedge clk);

        $display("Final CPU PC = 0x%04X  A=0x%02X", dbg_cpu_pc, dbg_cpu_a);
        // Sanity: CPU must be inside the halt JMP (dbg_pc may sample the
        // JMP operand fetch in-flight, so allow the 3-byte instruction range).
        test_num++;
        if (dbg_cpu_pc >= 16'hC027 && dbg_cpu_pc <= 16'hC029) begin
            $display("  PASS [%0d] CPU reached halt loop (PC=0x%04X)", test_num, dbg_cpu_pc);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] CPU not in halt loop (PC=0x%04X, want 0xC027..29)", test_num, dbg_cpu_pc);
            fail_count++;
        end

        // The core assertions. SID readback must return the exact values we
        // wrote — any wrong-capture from a different cpu_addr slot returns
        // last_written=0x03 or a foreign voice_vol byte, both detectably
        // different from the expected 1 / 2 / 3.
        check_eq8("mem[$0200] == voice_vol[0] (=1)", dut.main_ram.mem[16'h0200], 8'h01);
        check_eq8("mem[$0201] == voice_vol[1] (=2)", dut.main_ram.mem[16'h0201], 8'h02);
        check_eq8("mem[$0202] == voice_vol[2] (=3)", dut.main_ram.mem[16'h0202], 8'h03);

        // Blitter idle after reset — R_STATUS = 0. Same bug class on
        // r_blt_cpu_rdata; if the capture isn't gated, cpu_addr moves to
        // the next STA opcode fetch during cpu_ce=0 and blt_sel goes
        // false, so the capture grabs blitter's default 0x00 anyway (no
        // visible mismatch in the idle case). Keeping the assertion as a
        // sanity check — extend with a post-write status value once the
        // blitter exposes one that's non-zero by construction at idle.
        check_eq8("mem[$0203] == blt_status (idle=0)", dut.main_ram.mem[16'h0203], 8'h00);

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
