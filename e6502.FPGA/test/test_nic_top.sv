// NIC top-level integration test — nic.sv inside top.sv.
//
// Verifies the integration plumbing the unit-level test_nic.sv cannot reach:
//   - CPU writes to $A100-$A13F land in nic.sv through top.sv's MMIO decode
//   - CPU writes to NIC_CMD pulse top.sv's nic_event output
//   - Debug pokes/peeks route to the NIC bank without pulsing nic_event
//   - NIC IRQ status reaches top.sv's CPU IRQ input

`timescale 1ns/1ps

module test_nic_top;

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
    logic        dbg_nic_buf_we = 0;
    logic        dbg_nic_buf_re = 0;
    logic        dbg_nic_buf_sel = 0;
    logic [7:0]  dbg_nic_buf_addr = 0;
    logic [7:0]  dbg_nic_buf_data = 0;
    wire  [7:0]  dbg_nic_buf_rdata;

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

    wire         nic_event;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .irq_n(irq_n), .nmi_n(nmi_n),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(audio_l), .audio_r(audio_r),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_nic_buf_we(dbg_nic_buf_we),
        .dbg_nic_buf_re(dbg_nic_buf_re),
        .dbg_nic_buf_sel(dbg_nic_buf_sel),
        .dbg_nic_buf_addr(dbg_nic_buf_addr),
        .dbg_nic_buf_data(dbg_nic_buf_data),
        .dbg_nic_buf_rdata(dbg_nic_buf_rdata),
        .dbg_vmem_we(1'b0), .dbg_vmem_re(1'b0), .dbg_vmem_space(3'd0),
        .dbg_vmem_addr(17'd0), .dbg_vmem_data(8'd0), .dbg_vmem_rdata(),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_system_reset(1'b0),
        .dbg_cpu_resume(dbg_cpu_resume),
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0),
        .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
        .host_wts_event_we(1'b0), .host_wts_event_data(8'd0),
        .host_wts_event_ready(),
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
        .fio_event(),
        .nic_event(nic_event)
    );

    int event_count = 0;
    always_ff @(posedge clk) begin
        if (nic_event) event_count <= event_count + 1;
    end

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

    task automatic dbg_peek(input logic [15:0] addr, output logic [7:0] data);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en   <= 1;
        repeat(4) @(posedge clk);
        data = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en   <= 0;
    endtask

    task automatic dbg_nic_buf_write(input logic sel, input logic [7:0] addr,
                                      input logic [7:0] data);
        @(posedge clk);
        dbg_nic_buf_sel  <= sel;
        dbg_nic_buf_addr <= addr;
        dbg_nic_buf_data <= data;
        dbg_nic_buf_we   <= 1;
        @(posedge clk);
        dbg_nic_buf_we   <= 0;
    endtask

    task automatic dbg_nic_buf_read(input logic sel, input logic [7:0] addr,
                                     output logic [7:0] data);
        @(posedge clk);
        dbg_nic_buf_sel  <= sel;
        dbg_nic_buf_addr <= addr;
        dbg_nic_buf_re   <= 1;
        repeat(2) @(posedge clk);
        data = dbg_nic_buf_rdata;
        @(posedge clk);
        dbg_nic_buf_re   <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    // $C000:
    //   LDA #$01; STA $A103  ; enable slot0 NIC IRQs
    //   LDA #$34; STA $A108  ; remote port low
    //   LDA #$12; STA $A109  ; remote port high
    //   LDA #$01; STA $A100  ; NIC_CMD=CONNECT -> nic_event
    //   write "OK\n" to $2200
    //   NIC_DMAL/H=$2200, NIC_DMALEN=3
    //   LDA #$03; STA $A100  ; NIC_CMD=SEND -> TX DMA, then nic_event
    //   JMP $C037
    localparam int PROG_LEN = 58;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h01, 8'h8D, 8'h03, 8'hA1,
        8'hA9, 8'h34, 8'h8D, 8'h08, 8'hA1,
        8'hA9, 8'h12, 8'h8D, 8'h09, 8'hA1,
        8'hA9, 8'h01, 8'h8D, 8'h00, 8'hA1,
        8'hA9, 8'h4F, 8'h8D, 8'h00, 8'h22,
        8'hA9, 8'h4B, 8'h8D, 8'h01, 8'h22,
        8'hA9, 8'h0A, 8'h8D, 8'h02, 8'h22,
        8'hA9, 8'h00, 8'h8D, 8'h10, 8'hA1,
        8'hA9, 8'h22, 8'h8D, 8'h11, 8'hA1,
        8'hA9, 8'h03, 8'h8D, 8'h12, 8'hA1,
        8'hA9, 8'h03, 8'h8D, 8'h00, 8'hA1,
        8'h4C, 8'h37, 8'hC0
    };

    initial begin
        logic [7:0] d;
        int events_after_cpu;

        $display("=== NIC top-level integration test ===");

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

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        repeat(4) @(posedge clk);
        wait_vgc_ready();

        check_eq_int("event count before CPU runs", event_count, 0);
        check("CPU IRQ low before NIC status event", dbg_cpu_irq === 1'b0);

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;
        repeat(30000) @(posedge clk);

        check("CPU reached halt loop",
              dbg_cpu_pc >= 16'hC037 && dbg_cpu_pc <= 16'hC03A);
        check_eq_int("CPU CONNECT and SEND fired two nic_events", event_count, 2);

        dbg_pause = 1;
        repeat(8) @(posedge clk);

        dbg_peek(16'hA103, d);
        check_eq8("dbg peek NIC_IRQCTRL", d, 8'h01);
        dbg_peek(16'hA108, d);
        check_eq8("dbg peek NIC_RPORTL", d, 8'h34);
        dbg_peek(16'hA109, d);
        check_eq8("dbg peek NIC_RPORTH", d, 8'h12);
        dbg_peek(16'hA100, d);
        check_eq8("dbg peek NIC_CMD", d, 8'h03);
        dbg_peek(16'hA114, d);
        check_eq8("dbg peek NIC_DMASTATUS after SEND", d, 8'h01);
        dbg_nic_buf_read(1'b0, 8'h00, d);
        check_eq8("NIC TX buffer byte 0 captured by DMA", d, 8'h4F);
        dbg_nic_buf_read(1'b0, 8'h01, d);
        check_eq8("NIC TX buffer byte 1 captured by DMA", d, 8'h4B);
        dbg_nic_buf_read(1'b0, 8'h02, d);
        check_eq8("NIC TX buffer byte 2 captured by DMA", d, 8'h0A);

        events_after_cpu = event_count;
        dbg_poke(16'hA100, 8'h00);
        repeat(8) @(posedge clk);
        check_eq_int("dbg poke NIC_CMD clear does not fire nic_event",
                     event_count, events_after_cpu);

        dbg_poke(16'hA118, 8'h06); // slot0 send-ready + data-ready
        repeat(4) @(posedge clk);
        check("NIC slot data-ready asserts CPU IRQ", dbg_cpu_irq === 1'b1);
        dbg_peek(16'hA104, d);
        check_eq8("debug peek NIC_IRQSTATUS sees slot0 bit", d, 8'h01);

        dbg_poke(16'hA118, 8'h04); // clear data-ready, leave send-ready
        repeat(4) @(posedge clk);
        check("NIC IRQ remains latched after slot status clears", dbg_cpu_irq === 1'b1);

        dbg_poke(16'hA104, 8'h00); // mirror CPU IRQSTATUS read-clear for integration coverage
        repeat(4) @(posedge clk);
        check("NIC IRQ clears after latch clears", dbg_cpu_irq === 1'b0);

        dbg_nic_buf_write(1'b1, 8'h00, 8'hCA);
        dbg_nic_buf_write(1'b1, 8'h01, 8'hFE);
        dbg_nic_buf_write(1'b1, 8'h02, 8'h42);
        dbg_poke(16'hA110, 8'h00);
        dbg_poke(16'hA111, 8'h23);
        dbg_poke(16'hA113, 8'h03);
        dbg_poke(16'hA107, 8'h01); // HOSTCTRL.RX_START
        repeat(64) @(posedge clk);
        dbg_peek(16'hA114, d);
        check_eq8("dbg peek NIC_DMASTATUS after RX", d, 8'h02);
        dbg_peek(16'h2300, d);
        check_eq8("RX DMA wrote RAM byte 0", d, 8'hCA);
        dbg_peek(16'h2301, d);
        check_eq8("RX DMA wrote RAM byte 1", d, 8'hFE);
        dbg_peek(16'h2302, d);
        check_eq8("RX DMA wrote RAM byte 2", d, 8'h42);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===",
                 pass_count, fail_count);
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
