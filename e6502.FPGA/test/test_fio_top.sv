// FIO integration test — fio.sv inside top.sv.
//
// Verifies the integration plumbing the unit-level test_fio.sv can't
// reach:
//   - CPU writes to $B9A0-$B9EF land in fio.sv (via cpu_dout + cpu_we
//     + the io_group decode)
//   - Reads of $B9A0-$B9EF return the stored value (via r_fio_cpu_rdata
//     in the io_group mux)
//   - Writing non-zero to $B9A0 from the CPU pulses fio_event (top.sv
//     output)
//   - Debug-bridge pokes to the FIO range route to fio.dbg_we and do
//     NOT pulse fio_event (so ESP responses don't loop back as events)
//   - Debug-bridge peeks return the same value as CPU reads
//
// A tiny 6502 program writes 0x42 to $B9A0, then 0x55 to $B9A1
// (FioStatus). The test counts fio_event pulses, peeks the registers,
// and pokes them back via debug-bridge to verify the alternate path.
//
// Mirrors the test_rom_load.sv harness.

`timescale 1ns/1ps

module test_fio_top;

    // -----------------------------------------------------------------------
    // Clock / reset
    // -----------------------------------------------------------------------
    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;

    // -----------------------------------------------------------------------
    // DUT ports
    // -----------------------------------------------------------------------
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

    wire         fio_event;

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
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0), .brg_sdram_b_addr(25'd0),
        .brg_sdram_b_din(8'd0),
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
        .fio_event(fio_event)
    );

    // ---- Event counter ----
    int event_count = 0;
    always_ff @(posedge clk) begin
        if (fio_event) event_count <= event_count + 1;
    end

    // -----------------------------------------------------------------------
    // Test accounting
    // -----------------------------------------------------------------------
    int pass_count = 0, fail_count = 0, test_num = 0;

    task automatic check(input string name, input logic cond);
        test_num++;
        if (cond) begin
            $display("  PASS [%0d] %s", test_num, name);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s", test_num, name);
            fail_count++;
        end
    endtask

    task automatic check_eq8(input string name, input logic [7:0] a, input logic [7:0] e);
        test_num++;
        if (a === e) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, a);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)",
                     test_num, name, a, e);
            fail_count++;
        end
    endtask

    task automatic check_eq_int(input string name, input int a, input int e);
        test_num++;
        if (a == e) begin
            $display("  PASS [%0d] %s (=%0d)", test_num, name, a);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got %0d, want %0d)",
                     test_num, name, a, e);
            fail_count++;
        end
    endtask

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------
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

    task automatic dbg_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en   <= 1;
        @(posedge clk);
        dbg_poke_en   <= 0;
        repeat(4) @(posedge clk);
    endtask

    // dpram peek has 2-cycle latency: assert addr+en, wait for output to
    // settle, then sample. dbg_peek_data is wired through the same mux
    // as cpu_din, so it picks up r_fio_cpu_rdata when addr is in range.
    task automatic dbg_peek(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en   <= 1;
        repeat(4) @(posedge clk);
        data = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en   <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    // -----------------------------------------------------------------------
    // 6502 program — writes 0x42 to $B9A0 and 0x55 to $B9A1, then halts.
    //   $C000:  A9 42       LDA #$42
    //   $C002:  8D A0 B9    STA $B9A0      ; FioCmd ← $42 → fio_event!
    //   $C005:  A9 55       LDA #$55
    //   $C007:  8D A1 B9    STA $B9A1      ; FioStatus ← $55 (no event)
    //   $C00A:  4C 0A C0    JMP $C00A      ; halt
    // -----------------------------------------------------------------------
    localparam int PROG_LEN = 13;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h42, 8'h8D, 8'hA0, 8'hB9,
        8'hA9, 8'h55, 8'h8D, 8'hA1, 8'hB9,
        8'h4C, 8'h0A, 8'hC0
    };

    initial begin
        logic [7:0] d;
        int events_after_cpu, events_after_poke;

        $display("=== FIO top-level integration test ===");

        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;
        dbg_peek_en   = 0;
        dbg_poke_en   = 0;
        dbg_rom_we    = 0;
        dbg_peek_addr = 0;
        dbg_poke_addr = 0;
        dbg_poke_data = 0;
        dbg_rom_idx   = 0;
        dbg_rom_addr  = 0;
        dbg_rom_data  = 0;

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        // Load program into basic_rom + reset vector.
        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        repeat(4) @(posedge clk);
        wait_vgc_ready();

        // ───── Section 1: CPU writes fire fio_event ─────
        check_eq_int("event count before CPU runs", event_count, 0);

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        // Let CPU execute the 13-byte program.
        repeat(20000) @(posedge clk);

        check("CPU reached halt loop",
              dbg_cpu_pc >= 16'hC00A && dbg_cpu_pc <= 16'hC00D);

        events_after_cpu = event_count;
        // Exactly 1 event: STA $B9A0 with non-zero data fires once.
        check_eq_int("event count after CPU writes (1× FioCmd=$42)",
                     events_after_cpu, 1);

        // Pause CPU so subsequent debug pokes/peeks aren't trampled by
        // CPU re-runs of the same code.
        dbg_pause = 1;
        repeat(8) @(posedge clk);

        // ───── Section 2: debug peek reads back CPU's writes ─────
        dbg_peek(16'hB9A0, d);
        check_eq8("dbg peek $B9A0 = $42", d, 8'h42);
        dbg_peek(16'hB9A1, d);
        check_eq8("dbg peek $B9A1 = $55", d, 8'h55);

        // Out-of-range reads still go to RAM/elsewhere — sanity that
        // we didn't accidentally widen the FIO range.
        dbg_peek(16'hB99F, d);
        check_eq8("dbg peek $B99F (one before bank) NOT = $42", (d == 8'h42) ? 8'h00 : d, d);
        dbg_peek(16'hB9F0, d);
        check_eq8("dbg peek $B9F0 (one after bank) NOT = $42", (d == 8'h42) ? 8'h00 : d, d);

        // ───── Section 3: debug poke writes through, NO fio_event ─────
        events_after_cpu = event_count;
        dbg_poke(16'hB9A0, 8'h99);     // would-be FioCmd write
        repeat(8) @(posedge clk);
        events_after_poke = event_count;
        check_eq_int("dbg poke does NOT fire fio_event",
                     events_after_poke, events_after_cpu);

        // Verify the poke landed.
        dbg_peek(16'hB9A0, d);
        check_eq8("dbg peek $B9A0 = $99 after dbg poke", d, 8'h99);

        // Spread out across the bank.
        dbg_poke(16'hB9B0, 8'h41);     // FioName[0] = 'A'
        dbg_poke(16'hB9EF, 8'h7A);     // last byte of bank
        dbg_peek(16'hB9B0, d);
        check_eq8("dbg poke FioName[0]", d, 8'h41);
        dbg_peek(16'hB9EF, d);
        check_eq8("dbg poke FioName[63]", d, 8'h7A);

        // Confirm none of those pokes pulsed an event either.
        check_eq_int("dbg pokes across bank still don't fire events",
                     event_count, events_after_cpu);

        // ───── Section 4: subsequent CPU writes still fire events ─────
        // Reload reset vector to the start of a single-write program at
        // $C100, then bounce the CPU. We patch ROM at $C100..$C103.
        dbg_cpu_reset = 1;
        rom_write(1'b0, 14'h0100, 8'hA9);  // LDA #$AA
        rom_write(1'b0, 14'h0101, 8'hAA);
        rom_write(1'b0, 14'h0102, 8'h8D);  // STA $B9A0
        rom_write(1'b0, 14'h0103, 8'hA0);
        rom_write(1'b0, 14'h0104, 8'hB9);
        rom_write(1'b0, 14'h0105, 8'h4C);  // JMP $C105
        rom_write(1'b0, 14'h0106, 8'h05);
        rom_write(1'b0, 14'h0107, 8'hC1);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC1);  // reset → $C100
        repeat(4) @(posedge clk);

        events_after_poke = event_count;
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;
        repeat(20000) @(posedge clk);

        check_eq_int("second CPU write to $B9A0 fires +1 event",
                     event_count, events_after_poke + 1);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end
endmodule
