// Directed WAI/STP acceptance tests for the FPGA 65C02 core.
//
// WAI is interrupt-wakeable and then follows normal interrupt masking rules.
// STP is not interrupt-wakeable; only reset or the debug resume pulse releases
// it. The bridge uses this to let a debugger recover a stopped CPU.

`timescale 1ns/1ps

module test_cpu_wai_stp;

    logic clk = 0;
    always #20 clk = ~clk;

    logic reset = 1;
    logic rdy = 1;
    logic irq = 0;
    logic nmi = 0;
    logic dbg_resume = 0;

    wire [15:0] ab;
    logic [7:0] di = 8'hEA;
    wire [7:0] dout;
    wire we;

    wire [15:0] dbg_pc;
    wire [7:0]  dbg_a, dbg_x, dbg_y, dbg_s, dbg_flags;
    wire        dbg_waiting, dbg_stopped;

    byte unsigned mem [0:65535];

    cpu dut (
        .clk(clk),
        .reset(reset),
        .AB(ab),
        .DI(di),
        .DO(dout),
        .WE(we),
        .IRQ(irq),
        .NMI(nmi),
        .RDY(rdy),
        .dbg_resume(dbg_resume),
        .dbg_waiting(dbg_waiting),
        .dbg_stopped(dbg_stopped),
        .dbg_pc(dbg_pc),
        .dbg_a(dbg_a),
        .dbg_x(dbg_x),
        .dbg_y(dbg_y),
        .dbg_s(dbg_s),
        .dbg_flags(dbg_flags)
    );

    always_ff @(posedge clk) begin
        if (we)
            mem[ab] <= dout;
        di <= mem[ab];
    end

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

    task automatic step(input int cycles);
        repeat(cycles) begin
            @(posedge clk);
            if ($test$plusargs("TRACE_CPU"))
                $display("  pc=$%04X state=%s ir=$%02X a=$%02X p=$%02X irq=%0d nmi=%0d wai=%0d stp=%0d",
                         dbg_pc, dut.statename, dut.IR, dbg_a, dbg_flags,
                         irq, nmi, dbg_waiting, dbg_stopped);
        end
    endtask

    task automatic init_mem;
        for (int i = 0; i < 65536; i++)
            mem[i] = 8'hEA; // NOP
        mem[16'hFFFA] = 8'h00; mem[16'hFFFB] = 8'h05; // NMI
        mem[16'hFFFC] = 8'h00; mem[16'hFFFD] = 8'h04; // RESET
        mem[16'hFFFE] = 8'h00; mem[16'hFFFF] = 8'h05; // IRQ/BRK
        mem[16'h0200] = 8'h00;
        mem[16'h0201] = 8'h00;
        mem[16'h0202] = 8'h00;
    endtask

    task automatic reset_cpu;
        irq <= 1'b0;
        nmi <= 1'b0;
        dbg_resume <= 1'b0;
        rdy <= 1'b1;
        reset <= 1'b1;
        step(8);
        reset <= 1'b0;
        step(12);
    endtask

    task automatic wait_for_waiting(input string name);
        int timeout = 2000;
        while (!dbg_waiting && timeout > 0) begin
            step(1);
            timeout--;
        end
        check(name, dbg_waiting === 1'b1);
    endtask

    task automatic wait_for_stopped(input string name);
        int timeout = 2000;
        while (!dbg_stopped && timeout > 0) begin
            step(1);
            timeout--;
        end
        check(name, dbg_stopped === 1'b1);
    endtask

    task automatic wait_mem8(input string name, input logic [15:0] addr, input logic [7:0] expected);
        int timeout = 10000;
        while (mem[addr] !== expected && timeout > 0) begin
            step(1);
            timeout--;
        end
        if (timeout == 0)
            $display("  TIMEOUT pc=$%04X state=%s ir=$%02X a=$%02X p=$%02X mem[$%04X]=$%02X",
                     dbg_pc, dut.statename, dut.IR, dbg_a, dbg_flags, addr, mem[addr]);
        check_eq8(name, mem[addr], expected);
    endtask

    task automatic install_irq_marker_handler(input logic [7:0] value);
        mem[16'h0500] = 8'hA9; mem[16'h0501] = value; // LDA #value
        mem[16'h0502] = 8'h8D; mem[16'h0503] = 8'h01; mem[16'h0504] = 8'h02; // STA $0201
        mem[16'h0505] = 8'h40; // RTI
    endtask

    task automatic test_wai_masked_irq_continues_without_vector();
        $display("");
        $display("Test: WAI wakes on masked IRQ and continues after WAI");

        init_mem();
        mem[16'h0400] = 8'h78; // SEI
        mem[16'h0401] = 8'hCB; // WAI
        mem[16'h0402] = 8'hA9; mem[16'h0403] = 8'h42; // LDA #$42
        mem[16'h0404] = 8'h8D; mem[16'h0405] = 8'h00; mem[16'h0406] = 8'h02; // STA $0200
        mem[16'h0407] = 8'h4C; mem[16'h0408] = 8'h07; mem[16'h0409] = 8'h04; // JMP $0407
        install_irq_marker_handler(8'hEE);

        reset_cpu();
        wait_for_waiting("WAI state reached");

        irq <= 1'b1;
        step(8);
        irq <= 1'b0;

        wait_mem8("program continued after WAI", 16'h0200, 8'h42);
        check_eq8("IRQ handler was not taken", mem[16'h0201], 8'h00);
    endtask

    task automatic test_wai_unmasked_irq_vectors_then_returns();
        $display("");
        $display("Test: WAI wakes on unmasked IRQ and vectors through IRQ handler");

        init_mem();
        mem[16'h0400] = 8'h58; // CLI
        mem[16'h0401] = 8'hCB; // WAI
        mem[16'h0402] = 8'hA9; mem[16'h0403] = 8'h55; // LDA #$55
        mem[16'h0404] = 8'h8D; mem[16'h0405] = 8'h00; mem[16'h0406] = 8'h02; // STA $0200
        mem[16'h0407] = 8'h4C; mem[16'h0408] = 8'h07; mem[16'h0409] = 8'h04; // JMP $0407
        install_irq_marker_handler(8'h77);

        reset_cpu();
        wait_for_waiting("WAI state reached");

        irq <= 1'b1;
        wait_mem8("IRQ handler ran", 16'h0201, 8'h77);
        irq <= 1'b0;
        wait_mem8("RTI returned after WAI", 16'h0200, 8'h55);
    endtask

    task automatic test_stp_ignores_interrupts_and_debug_resumes();
        $display("");
        $display("Test: STP ignores IRQ/NMI but resumes from debug bridge pulse");

        init_mem();
        mem[16'h0400] = 8'hDB; // STP
        mem[16'h0401] = 8'hA9; mem[16'h0402] = 8'h33; // LDA #$33
        mem[16'h0403] = 8'h8D; mem[16'h0404] = 8'h02; mem[16'h0405] = 8'h02; // STA $0202
        mem[16'h0406] = 8'h4C; mem[16'h0407] = 8'h06; mem[16'h0408] = 8'h04; // JMP $0406
        install_irq_marker_handler(8'h99);

        reset_cpu();
        wait_for_stopped("STP state reached");

        irq <= 1'b1;
        nmi <= 1'b1;
        step(40);
        irq <= 1'b0;
        nmi <= 1'b0;
        check("STP stayed stopped through IRQ/NMI", dbg_stopped === 1'b1);
        check_eq8("post-STP instruction did not run", mem[16'h0202], 8'h00);
        check_eq8("IRQ/NMI handler did not run", mem[16'h0201], 8'h00);

        dbg_resume <= 1'b1;
        step(1);
        dbg_resume <= 1'b0;
        wait_mem8("debug resume continues after STP", 16'h0202, 8'h33);
        check("STP cleared after debug resume", dbg_stopped === 1'b0);
    endtask

    initial begin
        $display("=== CPU WAI/STP tests ===");

        test_wai_masked_irq_continues_without_vector();
        test_wai_unmasked_irq_vectors_then_returns();
        test_stp_ignores_interrupts_and_debug_resumes();

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #20000000;
        $display("FAIL: global timeout");
        $finish;
    end

endmodule
