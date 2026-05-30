// Top-level VGC command/readback hazard regression.
//
// NovaLogo's split-screen setup writes VGC_P5 = 3, then immediately polls
// VGC_CMD before issuing CopperAdd. The VGC write register-slice must keep
// the original write address paired with the original write data; otherwise
// the old data byte 3 is seen as a write to VGC_CMD and starts CMD_LINE.

`timescale 1ns/1ps

module test_vgc_cmd_readback_hazard_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic dbg_peek_en;
    logic [15:0] dbg_peek_addr;
    wire  [7:0] dbg_peek_data;
    logic dbg_poke_en;
    logic [15:0] dbg_poke_addr;
    logic [7:0] dbg_poke_data;
    logic dbg_pause;
    logic dbg_rom_we;
    logic dbg_rom_idx;
    logic [13:0] dbg_rom_addr;
    logic [7:0] dbg_rom_data;
    logic dbg_cpu_reset;
    logic dbg_cpu_resume = 1'b0;

    wire [15:0] dbg_cpu_pc;
    wire [7:0] dbg_cpu_a, dbg_cpu_x, dbg_cpu_y, dbg_cpu_sp, dbg_cpu_flags;
    wire [5:0] dbg_cpu_state;
    wire [7:0] dbg_cpu_ir;
    wire [15:0] dbg_cpu_addr;
    wire [7:0] dbg_cpu_din, dbg_cpu_dout;
    wire dbg_cpu_we, dbg_cpu_rdy, dbg_cpu_irq, dbg_cpu_nmi;
    wire dbg_cpu_waiting, dbg_cpu_stopped;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(1'b0), .key_data(8'h00), .key_ready(),
        .board_buttons(8'h00),
        .board_switches(8'h00),
        .usb_hid_status(8'h00),
        .usb_hid_device_type(8'h00),
        .usb_hid_last_scan(8'h00),
        .usb_hid_last_ascii(8'h00),
        .usb_hid_report_count(8'h00),
        .usb_hid_key_count(8'h00),
        .usb_hid_core_status(8'h00),
        .usb_hid_regs(64'h0),
        .irq_n(1'b1), .nmi_n(1'b1),
        .vid_r(), .vid_g(), .vid_b(), .vid_hsync(), .vid_vsync(), .vid_de(),
        .audio_l(), .audio_r(),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_nic_buf_we(1'b0), .dbg_nic_buf_re(1'b0), .dbg_nic_buf_sel(1'b0),
        .dbg_nic_buf_addr(8'd0), .dbg_nic_buf_data(8'd0), .dbg_nic_buf_rdata(),
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
        .fio_event(), .nic_event()
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;
    int artist_count = 0;
    int line_count = 0;
    int nonzero_gfx = 0;

    always_ff @(posedge clk) begin
        if (dut.vgc_inst.artist_cmd_valid) begin
            artist_count <= artist_count + 1;
            if (dut.vgc_inst.artist_cmd_code == 8'h03)
                line_count <= line_count + 1;
        end
    end

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

    task automatic check_eq_int(input string name, input int actual, input int expected);
        test_num++;
        if (actual == expected) begin
            $display("  PASS [%0d] %s (=%0d)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got %0d, want %0d)",
                     test_num, name, actual, expected);
            fail_count++;
        end
    endtask

    task automatic rom_write(input logic idx, input logic [13:0] addr,
                             input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we <= 1;
        @(posedge clk);
        dbg_rom_we <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    localparam int PROG_LEN = 74;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h00, 8'h8D, 8'h11, 8'hA0, // P0 = 0
        8'h8D, 8'h12, 8'hA0,               // P1 = 0
        8'h8D, 8'h13, 8'hA0,               // P2 = 0
        8'h8D, 8'h14, 8'hA0,               // P3 = VGC_MODE register index
        8'h8D, 8'h15, 8'hA0,               // P4 = 0
        8'hA9, 8'h03, 8'h8D, 8'h16, 8'hA0, // P5 = graphics+sprites mode

        8'hAD, 8'h10, 8'hA0,               // poll VGC_CMD immediately
        8'h29, 8'h01,
        8'hD0, 8'hF9,

        8'hA9, 8'h1B, 8'h8D, 8'h10, 8'hA0, // intended command: CopperAdd
        8'hAD, 8'h10, 8'hA0,
        8'h29, 8'h01,
        8'hD0, 8'hF9,

        8'hA9, 8'h00, 8'h8D, 8'h11, 8'hA0, // P0 = 0
        8'h8D, 8'h12, 8'hA0,               // P1 = 0
        8'hA9, 8'hA0, 8'h8D, 8'h13, 8'hA0, // P2 = split line 160
        8'hA9, 8'h00, 8'h8D, 8'h16, 8'hA0, // P5 = text mode

        8'hAD, 8'h10, 8'hA0,               // poll VGC_CMD immediately
        8'h29, 8'h01,
        8'hD0, 8'hF9,

        8'hA9, 8'h1B, 8'h8D, 8'h10, 8'hA0, // intended command: CopperAdd
        8'h4C, 8'h47, 8'hC0                // JMP halt
    };

    initial begin
        $display("=== VGC command/readback hazard top regression ===");

        rst = 1;
        dbg_pause = 1;
        dbg_cpu_reset = 1;
        dbg_peek_en = 0;
        dbg_peek_addr = 0;
        dbg_poke_en = 0;
        dbg_poke_addr = 0;
        dbg_poke_data = 0;
        dbg_rom_we = 0;
        dbg_rom_idx = 0;
        dbg_rom_addr = 0;
        dbg_rom_data = 0;

        repeat(20) @(posedge clk);
        rst = 0;
        repeat(10) @(posedge clk);

        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);
        repeat(4) @(posedge clk);
        wait_vgc_ready();

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(200000) @(posedge clk);

        for (int i = 0; i < 64000; i++) begin
            if (dut.vgc_inst.gfx_inst.gfx_mem.mem[i] != 4'h0)
                nonzero_gfx++;
        end

        $display("Final CPU PC=0x%04X artist_count=%0d line_count=%0d nonzero_gfx=%0d",
                 dbg_cpu_pc, artist_count, line_count, nonzero_gfx);

        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC047) && (dbg_cpu_pc <= 16'hC04A));
        check_eq_int("copper setup does not issue artist commands", artist_count, 0);
        check_eq_int("P5=3 followed by VGC_CMD read does not become CMD_LINE", line_count, 0);
        check_eq_int("copper setup does not draw into graphics plane", nonzero_gfx, 0);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
