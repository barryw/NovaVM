// Directed 65C02 STZ tests for the FPGA CPU core.
//
// STZ absolute/absolute,X must assert the store path. If decode marks the
// opcode as STZ but not as a store, BSS zeroing silently leaves stale bytes.

`timescale 1ns/1ps

module test_cpu_stz;

    logic clk = 0;
    always #20 clk = ~clk;

    logic reset = 1;
    logic rdy = 1;
    wire [15:0] ab;
    logic [7:0] di = 8'hEA;
    wire [7:0] dout;
    wire we;

    wire [15:0] dbg_pc;
    wire [7:0]  dbg_a, dbg_x, dbg_y, dbg_s, dbg_flags;
    wire [5:0]  dbg_state;
    wire [7:0]  dbg_ir;
    wire        dbg_waiting, dbg_stopped;

    byte unsigned mem [0:65535];

    cpu dut (
        .clk(clk),
        .reset(reset),
        .AB(ab),
        .DI(di),
        .DO(dout),
        .WE(we),
        .IRQ(1'b0),
        .NMI(1'b0),
        .RDY(rdy),
        .dbg_resume(1'b0),
        .dbg_waiting(dbg_waiting),
        .dbg_stopped(dbg_stopped),
        .dbg_pc(dbg_pc),
        .dbg_a(dbg_a),
        .dbg_x(dbg_x),
        .dbg_y(dbg_y),
        .dbg_s(dbg_s),
        .dbg_flags(dbg_flags),
        .dbg_state(dbg_state),
        .dbg_ir(dbg_ir)
    );

    always_ff @(posedge clk) begin
        if (we)
            mem[ab] <= dout;
        di <= mem[ab];
    end

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

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

    task automatic step(input int cycles);
        repeat(cycles) @(posedge clk);
    endtask

    task automatic init_mem;
        for (int i = 0; i < 65536; i++)
            mem[i] = 8'hEA;

        mem[16'hFFFC] = 8'h00;
        mem[16'hFFFD] = 8'h04;

        mem[16'h0200] = 8'hFF;
        mem[16'h0201] = 8'hFF;
        mem[16'h0202] = 8'hFF;
        mem[16'h0023] = 8'hFF;
        mem[16'h0024] = 8'hFF;

        mem[16'h0400] = 8'hA9; mem[16'h0401] = 8'hA0; // LDA #$A0
        mem[16'h0402] = 8'h8D; mem[16'h0403] = 8'h00; mem[16'h0404] = 8'h02; // STA $0200
        mem[16'h0405] = 8'h9C; mem[16'h0406] = 8'h01; mem[16'h0407] = 8'h02; // STZ $0201
        mem[16'h0408] = 8'hA2; mem[16'h0409] = 8'h02; // LDX #$02
        mem[16'h040A] = 8'hA9; mem[16'h040B] = 8'h55; // LDA #$55
        mem[16'h040C] = 8'h9E; mem[16'h040D] = 8'h00; mem[16'h040E] = 8'h02; // STZ $0200,X
        mem[16'h040F] = 8'hA9; mem[16'h0410] = 8'h77; // LDA #$77
        mem[16'h0411] = 8'h64; mem[16'h0412] = 8'h23; // STZ $23
        mem[16'h0413] = 8'hA9; mem[16'h0414] = 8'h99; // LDA #$99
        mem[16'h0415] = 8'h74; mem[16'h0416] = 8'h22; // STZ $22,X -> $24
        mem[16'h0417] = 8'hDB;                         // STP
    endtask

    initial begin
        $display("=== 65C02 STZ CPU tests ===");
        init_mem();

        step(8);
        reset = 0;

        for (int i = 0; i < 2000 && !dbg_stopped; i++)
            step(1);

        check_eq8("STA baseline writes accumulator", mem[16'h0200], 8'hA0);
        check_eq8("STZ abs writes zero", mem[16'h0201], 8'h00);
        check_eq8("STZ abs,X writes zero", mem[16'h0202], 8'h00);
        check_eq8("STZ zp writes zero", mem[16'h0023], 8'h00);
        check_eq8("STZ zp,X writes zero", mem[16'h0024], 8'h00);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
