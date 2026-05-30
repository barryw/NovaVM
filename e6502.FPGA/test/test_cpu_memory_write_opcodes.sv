// Directed CPU writeback regression tests.
//
// This catches STZ-class decode bugs where an opcode is recognized but the
// memory write path is not asserted, plus 65C02 memory-modifying opcodes that
// use the read-modify-write path instead of the store path.

`timescale 1ns/1ps

module test_cpu_memory_write_opcodes;

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
    int unsigned emit_pc;

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

    task automatic check_true(input string name, input logic actual);
        test_num++;
        if (actual) begin
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

    task automatic step(input int cycles);
        repeat(cycles) @(posedge clk);
    endtask

    task automatic emit8(input logic [7:0] value);
        mem[emit_pc[15:0]] = value;
        emit_pc++;
    endtask

    task automatic emit16(input logic [15:0] value);
        emit8(value[7:0]);
        emit8(value[15:8]);
    endtask

    task automatic emit_lda_imm(input logic [7:0] value);
        emit8(8'hA9);
        emit8(value);
    endtask

    task automatic emit_ldx_imm(input logic [7:0] value);
        emit8(8'hA2);
        emit8(value);
    endtask

    task automatic emit_ldy_imm(input logic [7:0] value);
        emit8(8'hA0);
        emit8(value);
    endtask

    task automatic emit_zp(input logic [7:0] opcode, input logic [7:0] zp);
        emit8(opcode);
        emit8(zp);
    endtask

    task automatic emit_abs(input logic [7:0] opcode, input logic [15:0] addr);
        emit8(opcode);
        emit16(addr);
    endtask

    task automatic init_mem;
        byte unsigned opcode;

        for (int i = 0; i < 65536; i++)
            mem[i] = 8'hEA;

        mem[16'hFFFC] = 8'h00;
        mem[16'hFFFD] = 8'h04;

        mem[16'h0034] = 8'h04; mem[16'h0035] = 8'h02; // ($30,X), X=4 -> $0204
        mem[16'h0038] = 8'h06; mem[16'h0039] = 8'h02; // ($38),Y, Y=2 -> $0208
        mem[16'h003A] = 8'h09; mem[16'h003B] = 8'h02; // ($3A) -> $0209

        mem[16'h0040] = 8'h0F;
        mem[16'h020F] = 8'h0F;
        mem[16'h0041] = 8'hFF;
        mem[16'h0210] = 8'hFF;

        for (int bit_index = 0; bit_index < 8; bit_index++) begin
            mem[16'h0050 + bit_index] = 8'hFF;
            mem[16'h0058 + bit_index] = 8'h00;
        end

        mem[16'h0070] = 8'h11; mem[16'h0071] = 8'h22;
        mem[16'h0211] = 8'h33; mem[16'h0214] = 8'h44;
        mem[16'h0072] = 8'h40; mem[16'h0073] = 8'h01;
        mem[16'h0215] = 8'h7F; mem[16'h0219] = 8'h80;
        mem[16'h0074] = 8'h80; mem[16'h0075] = 8'h03;
        mem[16'h021A] = 8'h02; mem[16'h021C] = 8'hFE;
        mem[16'h0076] = 8'h02; mem[16'h0077] = 8'h04;
        mem[16'h021D] = 8'h06; mem[16'h021F] = 8'h08;
        mem[16'h0078] = 8'h10; mem[16'h0079] = 8'h00;
        mem[16'h0220] = 8'h80; mem[16'h0222] = 8'h01;
        mem[16'h007A] = 8'h0F; mem[16'h007B] = 8'hFF;
        mem[16'h0223] = 8'h7F; mem[16'h0225] = 8'hFE;

        emit_pc = 16'h0400;

        emit_ldx_imm(8'hF0); emit8(8'h9A);                   // TXS
        emit_lda_imm(8'hA1); emit8(8'h48);                   // PHA
        emit_ldx_imm(8'hB2); emit8(8'hDA);                   // PHX
        emit_ldy_imm(8'hC3); emit8(8'h5A);                   // PHY

        emit_lda_imm(8'h11); emit_zp(8'h85, 8'h20);          // STA zp
        emit_ldx_imm(8'h04); emit_lda_imm(8'h12); emit_zp(8'h95, 8'h20); // STA zp,X
        emit_lda_imm(8'h13); emit_abs(8'h8D, 16'h0200);      // STA abs
        emit_ldx_imm(8'h02); emit_lda_imm(8'h14); emit_abs(8'h9D, 16'h0200); // STA abs,X
        emit_ldy_imm(8'h03); emit_lda_imm(8'h15); emit_abs(8'h99, 16'h0200); // STA abs,Y
        emit_ldx_imm(8'h04); emit_lda_imm(8'h16); emit_zp(8'h81, 8'h30); // STA (zp,X)
        emit_ldy_imm(8'h02); emit_lda_imm(8'h17); emit_zp(8'h91, 8'h38); // STA (zp),Y
        emit_lda_imm(8'h18); emit_zp(8'h92, 8'h3A);          // STA (zp)

        emit_ldx_imm(8'h21); emit_zp(8'h86, 8'h25);          // STX zp
        emit_ldx_imm(8'h22); emit_ldy_imm(8'h03); emit_zp(8'h96, 8'h25); // STX zp,Y
        emit_ldx_imm(8'h23); emit_abs(8'h8E, 16'h020A);      // STX abs

        emit_ldy_imm(8'h31); emit_zp(8'h84, 8'h29);          // STY zp
        emit_ldy_imm(8'h32); emit_ldx_imm(8'h04); emit_zp(8'h94, 8'h29); // STY zp,X
        emit_ldy_imm(8'h33); emit_abs(8'h8C, 16'h020B);      // STY abs

        emit_zp(8'h64, 8'h2E);                               // STZ zp
        emit_ldx_imm(8'h04); emit_zp(8'h74, 8'h2E);          // STZ zp,X
        emit_abs(8'h9C, 16'h020C);                           // STZ abs
        emit_ldx_imm(8'h02); emit_abs(8'h9E, 16'h020C);      // STZ abs,X

        emit_lda_imm(8'hF0); emit_zp(8'h04, 8'h40);          // TSB zp
        emit_lda_imm(8'hF0); emit_abs(8'h0C, 16'h020F);      // TSB abs
        emit_lda_imm(8'h0F); emit_zp(8'h14, 8'h41);          // TRB zp
        emit_lda_imm(8'h0F); emit_abs(8'h1C, 16'h0210);      // TRB abs

        for (int bit_index = 0; bit_index < 8; bit_index++) begin
            opcode = 8'h07 + (bit_index * 16);
            emit_zp(opcode, 8'h50 + bit_index);              // RMBn zp
        end
        for (int bit_index = 0; bit_index < 8; bit_index++) begin
            opcode = 8'h87 + (bit_index * 16);
            emit_zp(opcode, 8'h58 + bit_index);              // SMBn zp
        end

        emit_zp(8'h06, 8'h70);                               // ASL zp
        emit_ldx_imm(8'h01); emit_zp(8'h16, 8'h70);          // ASL zp,X
        emit_abs(8'h0E, 16'h0211);                           // ASL abs
        emit_ldx_imm(8'h02); emit_abs(8'h1E, 16'h0212);      // ASL abs,X

        emit8(8'h38); emit_zp(8'h26, 8'h72);                 // SEC; ROL zp
        emit8(8'h38); emit_ldx_imm(8'h01); emit_zp(8'h36, 8'h72); // SEC; ROL zp,X
        emit8(8'h38); emit_abs(8'h2E, 16'h0215);             // SEC; ROL abs
        emit8(8'h38); emit_ldx_imm(8'h02); emit_abs(8'h3E, 16'h0217); // SEC; ROL abs,X

        emit_zp(8'h46, 8'h74);                               // LSR zp
        emit_ldx_imm(8'h01); emit_zp(8'h56, 8'h74);          // LSR zp,X
        emit_abs(8'h4E, 16'h021A);                           // LSR abs
        emit_ldx_imm(8'h01); emit_abs(8'h5E, 16'h021B);      // LSR abs,X

        emit8(8'h38); emit_zp(8'h66, 8'h76);                 // SEC; ROR zp
        emit8(8'h38); emit_ldx_imm(8'h01); emit_zp(8'h76, 8'h76); // SEC; ROR zp,X
        emit8(8'h38); emit_abs(8'h6E, 16'h021D);             // SEC; ROR abs
        emit8(8'h38); emit_ldx_imm(8'h02); emit_abs(8'h7E, 16'h021D); // SEC; ROR abs,X

        emit_zp(8'hC6, 8'h78);                               // DEC zp
        emit_ldx_imm(8'h01); emit_zp(8'hD6, 8'h78);          // DEC zp,X
        emit_abs(8'hCE, 16'h0220);                           // DEC abs
        emit_ldx_imm(8'h02); emit_abs(8'hDE, 16'h0220);      // DEC abs,X

        emit_zp(8'hE6, 8'h7A);                               // INC zp
        emit_ldx_imm(8'h01); emit_zp(8'hF6, 8'h7A);          // INC zp,X
        emit_abs(8'hEE, 16'h0223);                           // INC abs
        emit_ldx_imm(8'h02); emit_abs(8'hFE, 16'h0223);      // INC abs,X

        emit8(8'hDB);                                        // STP
    endtask

    initial begin
        $display("=== CPU memory write opcode tests ===");
        init_mem();

        step(8);
        reset = 0;

        for (int i = 0; i < 10000 && !dbg_stopped; i++)
            step(1);

        check_true("program reaches STP", dbg_stopped);

        check_eq8("PHA writes stack",       mem[16'h01F0], 8'hA1);
        check_eq8("PHX writes stack",       mem[16'h01EF], 8'hB2);
        check_eq8("PHY writes stack",       mem[16'h01EE], 8'hC3);

        check_eq8("STA zp writes A",       mem[16'h0020], 8'h11);
        check_eq8("STA zp,X writes A",     mem[16'h0024], 8'h12);
        check_eq8("STA abs writes A",      mem[16'h0200], 8'h13);
        check_eq8("STA abs,X writes A",    mem[16'h0202], 8'h14);
        check_eq8("STA abs,Y writes A",    mem[16'h0203], 8'h15);
        check_eq8("STA (zp,X) writes A",   mem[16'h0204], 8'h16);
        check_eq8("STA (zp),Y writes A",   mem[16'h0208], 8'h17);
        check_eq8("STA (zp) writes A",     mem[16'h0209], 8'h18);

        check_eq8("STX zp writes X",       mem[16'h0025], 8'h21);
        check_eq8("STX zp,Y writes X",     mem[16'h0028], 8'h22);
        check_eq8("STX abs writes X",      mem[16'h020A], 8'h23);
        check_eq8("STY zp writes Y",       mem[16'h0029], 8'h31);
        check_eq8("STY zp,X writes Y",     mem[16'h002D], 8'h32);
        check_eq8("STY abs writes Y",      mem[16'h020B], 8'h33);

        check_eq8("STZ zp writes zero",    mem[16'h002E], 8'h00);
        check_eq8("STZ zp,X writes zero",  mem[16'h0032], 8'h00);
        check_eq8("STZ abs writes zero",   mem[16'h020C], 8'h00);
        check_eq8("STZ abs,X writes zero", mem[16'h020E], 8'h00);

        check_eq8("TSB zp sets bits",      mem[16'h0040], 8'hFF);
        check_eq8("TSB abs sets bits",     mem[16'h020F], 8'hFF);
        check_eq8("TRB zp resets bits",    mem[16'h0041], 8'hF0);
        check_eq8("TRB abs resets bits",   mem[16'h0210], 8'hF0);

        for (int bit_index = 0; bit_index < 8; bit_index++) begin
            check_eq8($sformatf("RMB%0d clears its bit", bit_index),
                      mem[16'h0050 + bit_index], 8'hFF & ~(8'h01 << bit_index));
        end
        for (int bit_index = 0; bit_index < 8; bit_index++) begin
            check_eq8($sformatf("SMB%0d sets its bit", bit_index),
                      mem[16'h0058 + bit_index], 8'h01 << bit_index);
        end

        check_eq8("ASL zp writes result",    mem[16'h0070], 8'h22);
        check_eq8("ASL zp,X writes result",  mem[16'h0071], 8'h44);
        check_eq8("ASL abs writes result",   mem[16'h0211], 8'h66);
        check_eq8("ASL abs,X writes result", mem[16'h0214], 8'h88);

        check_eq8("ROL zp writes result",    mem[16'h0072], 8'h81);
        check_eq8("ROL zp,X writes result",  mem[16'h0073], 8'h03);
        check_eq8("ROL abs writes result",   mem[16'h0215], 8'hFF);
        check_eq8("ROL abs,X writes result", mem[16'h0219], 8'h01);

        check_eq8("LSR zp writes result",    mem[16'h0074], 8'h40);
        check_eq8("LSR zp,X writes result",  mem[16'h0075], 8'h01);
        check_eq8("LSR abs writes result",   mem[16'h021A], 8'h01);
        check_eq8("LSR abs,X writes result", mem[16'h021C], 8'h7F);

        check_eq8("ROR zp writes result",    mem[16'h0076], 8'h81);
        check_eq8("ROR zp,X writes result",  mem[16'h0077], 8'h82);
        check_eq8("ROR abs writes result",   mem[16'h021D], 8'h83);
        check_eq8("ROR abs,X writes result", mem[16'h021F], 8'h84);

        check_eq8("DEC zp writes result",    mem[16'h0078], 8'h0F);
        check_eq8("DEC zp,X writes result",  mem[16'h0079], 8'hFF);
        check_eq8("DEC abs writes result",   mem[16'h0220], 8'h7F);
        check_eq8("DEC abs,X writes result", mem[16'h0222], 8'h00);

        check_eq8("INC zp writes result",    mem[16'h007A], 8'h10);
        check_eq8("INC zp,X writes result",  mem[16'h007B], 8'h00);
        check_eq8("INC abs writes result",   mem[16'h0223], 8'h80);
        check_eq8("INC abs,X writes result", mem[16'h0225], 8'hFF);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
