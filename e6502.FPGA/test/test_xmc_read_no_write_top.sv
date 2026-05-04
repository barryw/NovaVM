// XMC read-only regression.
//
// NovaZ's story reader maps XMC window 3, then reads bytes with
// `LDA $BF00,X`. A read through the XMC window must never generate an XRAM
// write, even while the SDRAM-backed XRAM wrapper stalls the CPU.

`timescale 1ns/1ps

module test_xmc_read_no_write_top;

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
    logic       sdram_doneA = 1'b0;

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
        .sdram_clk(clk),
        .sdram_addrA(sdram_addrA), .sdram_dinA(sdram_dinA),
        .sdram_weA(sdram_weA), .sdram_oeA(sdram_oeA),
        .sdram_doutA(sdram_doutA), .sdram_doneA(sdram_doneA),
        .sdram_addrB(), .sdram_dinB(), .sdram_weB(), .sdram_oeB(),
        .sdram_doutB(8'h00),
        .fio_event()
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;
    int xram_write_count = 0;

    byte unsigned sdram_mem [0:524287];
    byte unsigned expected [0:7] = '{8'h03, 8'h00, 8'h00, 8'h58,
                                     8'h4E, 8'h37, 8'h4F, 8'h05};

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
                             input logic [7:0] expected_value);
        test_num++;
        if (actual === expected_value) begin
            $display("  PASS [%0d] %s (=0x%02X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%02X, want 0x%02X)",
                     test_num, name, actual, expected_value);
            fail_count++;
        end
    endtask

    task automatic check_eq_int(input string name, input int actual, input int expected_value);
        test_num++;
        if (actual == expected_value) begin
            $display("  PASS [%0d] %s (=%0d)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got %0d, want %0d)",
                     test_num, name, actual, expected_value);
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

    initial begin
        for (int i = 0; i < 524288; i++)
            sdram_mem[i] = 8'h00;
        for (int i = 0; i < 8; i++)
            sdram_mem[i] = expected[i];
    end

    always_ff @(posedge clk) begin
        sdram_doneA <= 1'b0;
        if (sdram_weA) begin
            sdram_mem[sdram_addrA[18:0]] <= sdram_dinA;
            xram_write_count <= xram_write_count + 1;
        end
        if (sdram_oeA)
            sdram_doutA <= sdram_mem[sdram_addrA[18:0]];
        if (sdram_weA || sdram_oeA)
            sdram_doneA <= 1'b1;
    end

    localparam int PROG_LEN = 62;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h08, 8'h8D, 8'h16, 8'hBA, // enable window 3 only
        8'hA9, 8'h00, 8'h8D, 8'h21, 8'hBA, // W3AL = 0
        8'h8D, 8'h22, 8'hBA,               // W3AM = 0
        8'h8D, 8'h23, 8'hBA,               // W3AH = 0
        8'hA2, 8'h00,                       // X = 0
        8'hBD, 8'h00, 8'hBF, 8'h8D, 8'h00, 8'h02, // read $BF00,X -> $0200
        8'hE8,
        8'hBD, 8'h00, 8'hBF, 8'h8D, 8'h01, 8'h02,
        8'hE8,
        8'hBD, 8'h00, 8'hBF, 8'h8D, 8'h02, 8'h02,
        8'hE8,
        8'hBD, 8'h00, 8'hBF, 8'h8D, 8'h03, 8'h02,
        8'hE8,
        8'hBD, 8'h00, 8'hBF, 8'h8D, 8'h04, 8'h02,
        8'hE8,
        8'hBD, 8'h00, 8'hBF, 8'h8D, 8'h05, 8'h02,
        8'h4C, 8'h3B, 8'hC0                // halt loop
    };

    initial begin
        $display("=== XMC read-only top-level regression ===");

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
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(100000) @(posedge clk);

        $display("Final CPU PC = 0x%04X IR=0x%02X state=%0d addr=0x%04X din=0x%02X dout=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr,
                 dbg_cpu_din, dbg_cpu_dout);

        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC03B) && (dbg_cpu_pc <= 16'hC03E));
        check_eq_int("XMC reads generated no XRAM writes", xram_write_count, 0);

        for (int i = 0; i < 6; i++) begin
            check_eq8($sformatf("read byte %0d reached RAM", i),
                      dut.main_ram.mem[16'h0200 + i], expected[i]);
            check_eq8($sformatf("story byte %0d unchanged", i),
                      sdram_mem[i], expected[i]);
        end

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
