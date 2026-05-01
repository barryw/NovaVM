// XMC mapped-window decode regression.
//
// The XMC window base registers are 24-bit and fully readable/writable, but
// each CPU-visible window is 256-byte page-aligned. WnAL must not be added to
// the CPU address low byte during translation. A BASIC `XMAP 0,$0101` followed
// by `PEEK($BC02)` should touch XRAM address $000102, not $000103.
//
// This also exercises zero-page indexed-indirect addressing, matching the
// failure mode from BASIC PEEK/POKE. The synthesized CPU path runs at a
// divided clock, so XMC requests must only fire on active CPU cycles.

`timescale 1ns/1ps

module test_xmc_window_decode_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;

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

    wire [24:0] sdram_addrA;
    wire [7:0]  sdram_dinA;
    wire        sdram_weA;
    wire        sdram_oeA;
    logic [7:0] sdram_doutA = 8'h00;

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
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0),
        .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
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
        .sdram_addrA(sdram_addrA), .sdram_dinA(sdram_dinA),
        .sdram_weA(sdram_weA), .sdram_oeA(sdram_oeA),
        .sdram_doutA(sdram_doutA),
        .sdram_addrB(), .sdram_dinB(), .sdram_weB(), .sdram_oeB(),
        .sdram_doutB(8'h00),
        .fio_event()
    );

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

    task automatic check_eq19(input string name, input logic [18:0] actual,
                              input logic [18:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%05X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%05X, want 0x%05X)",
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

    int read_count = 0;
    int write_count = 0;
    logic [18:0] first_read_addr = 19'h00000;
    logic [18:0] second_read_addr = 19'h00000;
    logic [18:0] third_read_addr = 19'h00000;
    logic [18:0] first_write_addr = 19'h00000;
    logic [18:0] second_write_addr = 19'h00000;
    logic [18:0] third_write_addr = 19'h00000;
    logic [18:0] fourth_write_addr = 19'h00000;
    logic [7:0] first_write_data = 8'h00;
    logic [7:0] second_write_data = 8'h00;
    logic [7:0] third_write_data = 8'h00;
    logic [7:0] fourth_write_data = 8'h00;

    byte unsigned sdram_mem [0:524287];

    initial begin
        for (int i = 0; i < 524288; i++)
            sdram_mem[i] = 8'h00;
        sdram_mem[19'h00102] = 8'h33;
    end

    always_ff @(posedge clk) begin
        if (sdram_weA)
            sdram_mem[sdram_addrA[18:0]] <= sdram_dinA;
        if (sdram_oeA)
            sdram_doutA <= sdram_mem[sdram_addrA[18:0]];
    end

    always_ff @(posedge clk) begin
        if (dut.xram_a_re) begin
            if (read_count == 0)
                first_read_addr <= dut.xram_a_addr;
            else if (read_count == 1)
                second_read_addr <= dut.xram_a_addr;
            else if (read_count == 2)
                third_read_addr <= dut.xram_a_addr;
            read_count <= read_count + 1;
        end
        if (dut.xram_a_we) begin
            if (write_count == 0) begin
                first_write_addr <= dut.xram_a_addr;
                first_write_data <= dut.xram_a_din;
            end else if (write_count == 1) begin
                second_write_addr <= dut.xram_a_addr;
                second_write_data <= dut.xram_a_din;
            end else if (write_count == 2) begin
                third_write_addr <= dut.xram_a_addr;
                third_write_data <= dut.xram_a_din;
            end else if (write_count == 3) begin
                fourth_write_addr <= dut.xram_a_addr;
                fourth_write_data <= dut.xram_a_din;
            end
            write_count <= write_count + 1;
        end
    end

    localparam int PROG_LEN = 67;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h01, 8'h8D, 8'h18, 8'hBA, // W0AL = $01
        8'hA9, 8'h01, 8'h8D, 8'h19, 8'hBA, // W0AM = $01
        8'hA9, 8'h00, 8'h8D, 8'h1A, 8'hBA, // W0AH = $00
        8'hAD, 8'h02, 8'hBC,               // LDA $BC02
        8'h8D, 8'h00, 8'h02,               // STA $0200
        8'hA9, 8'h77,                       // LDA #$77
        8'h8D, 8'h02, 8'hBC,               // STA $BC02
        8'hA9, 8'h02, 8'h85, 8'h10,         // $10 = $02
        8'hA9, 8'hBC, 8'h85, 8'h11,         // $11 = $BC
        8'hA2, 8'h00,                       // LDX #$00
        8'hA1, 8'h10,                       // LDA ($10,X)
        8'hA9, 8'h55,                       // LDA #$55
        8'h81, 8'h10,                       // STA ($10,X)
        8'hA9, 8'h00, 8'h85, 8'h12,         // $12 = $00
        8'hA9, 8'hBC, 8'h85, 8'h13,         // $13 = $BC
        8'hA0, 8'h02,                       // LDY #$02
        8'hB1, 8'h12,                       // LDA ($12),Y
        8'h8D, 8'h01, 8'h02,               // STA $0201
        8'hA9, 8'h44,                       // LDA #$44
        8'h91, 8'h12,                       // STA ($12),Y
        8'hEE, 8'h02, 8'hBC,               // INC $BC02 (RMW: read then write)
        8'h4C, 8'h40, 8'hC0                // JMP $C040
    };

    initial begin
        $display("=== XMC mapped-window decode top-level test ===");

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
        check_eq8("basic_rom[0x0000] after load", dut.basic_rom_inst.mem[14'h0000], 8'hA9);
        check_eq8("basic_rom[0x3FFC] reset lo", dut.basic_rom_inst.mem[14'h3FFC], 8'h00);
        check_eq8("basic_rom[0x3FFD] reset hi", dut.basic_rom_inst.mem[14'h3FFD], 8'hC0);

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(100000) @(posedge clk);

        $display("Final CPU PC = 0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X dout=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr, dbg_cpu_din, dbg_cpu_dout);

        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC040) && (dbg_cpu_pc <= 16'hC043));
        check_eq8("W0AL remains readable as $01", dut.xmc_regs[6'h18], 8'h01);
        check_eq8("W0AM remains readable as $01", dut.xmc_regs[6'h19], 8'h01);
        check_eq8("W0AH remains readable as $00", dut.xmc_regs[6'h1A], 8'h00);
        check("mapped-window reads fired", read_count >= 4);
        check("mapped-window writes fired", write_count >= 4);
        check_eq19("absolute read $BC02 maps to XRAM $00102", first_read_addr, 19'h00102);
        check_eq19("absolute write $BC02 maps to XRAM $00102", first_write_addr, 19'h00102);
        check_eq19("indirect read ($10,X) maps to XRAM $00102", second_read_addr, 19'h00102);
        check_eq19("indirect write ($10,X) maps to XRAM $00102", second_write_addr, 19'h00102);
        check_eq19("indirect read ($12),Y maps to XRAM $00102", third_read_addr, 19'h00102);
        check_eq19("indirect write ($12),Y maps to XRAM $00102", third_write_addr, 19'h00102);
        check_eq19("RMW write INC $BC02 maps to XRAM $00102", fourth_write_addr, 19'h00102);
        check_eq8("absolute read data reached CPU", dut.main_ram.mem[16'h0200], 8'h33);
        check_eq8("absolute write data is $77", first_write_data, 8'h77);
        check_eq8("indirect write data is $55", second_write_data, 8'h55);
        check_eq8("indirect indexed read data reached CPU", dut.main_ram.mem[16'h0201], 8'h55);
        check_eq8("indirect indexed write data is $44", third_write_data, 8'h44);
        check_eq8("RMW write data increments to $45", fourth_write_data, 8'h45);
        check_eq8("SDRAM contains final RMW write", sdram_mem[19'h00102], 8'h45);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
