// Board-input controller regression (top.sv).
//
// Characterizes the joystick/buttons/DIP-switch controller at $BA9C-$BAA1:
//   $BA9C buttons (masked $7F), $BA9D switches (masked $0F),
//   $BA9E/$BA9F IRQ enable/status (W1C), $BAA0/$BAA1 change-masks (W1C).
//
// The controller is independent of the CPU (its FSM is clocked by custom_rst,
// not cpu_reset), so this test holds the CPU in reset and drives the board_*
// inputs directly, reading state back through the debug-peek mux. Mirrors the
// host-side path used by the Avalonia emulator + nova CLI and BASIC JOY/SW.

`timescale 1ns/1ps

module test_board_input_top;

    logic clk = 0;
    always #20 clk = ~clk;  // ~25 MHz

    logic rst;

    logic        key_valid = 1'b0;
    logic [7:0]  key_data  = 8'h00;
    logic [7:0]  board_buttons  = 8'h00;   // driven by the test
    logic [7:0]  board_switches = 8'h00;
    logic [7:0]  usb_hid_status = 8'hA5;
    logic [7:0]  usb_hid_device_type = 8'h01;
    logic [7:0]  usb_hid_last_scan = 8'h04;
    logic [7:0]  usb_hid_last_ascii = 8'h41;
    logic [7:0]  usb_hid_report_count = 8'h11;
    logic [7:0]  usb_hid_key_count = 8'h22;
    logic [7:0]  usb_hid_core_status = 8'hD3;
    logic        irq_n = 1;
    logic        nmi_n = 1;

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

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .board_buttons(board_buttons),
        .board_switches(board_switches),
        .usb_hid_status(usb_hid_status),
        .usb_hid_device_type(usb_hid_device_type),
        .usb_hid_last_scan(usb_hid_last_scan),
        .usb_hid_last_ascii(usb_hid_last_ascii),
        .usb_hid_report_count(usb_hid_report_count),
        .usb_hid_key_count(usb_hid_key_count),
        .usb_hid_core_status(usb_hid_core_status),
        .usb_hid_regs(64'h1122334455667788),
        .irq_n(irq_n), .nmi_n(nmi_n),
        .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
        .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
        .audio_l(audio_l), .audio_r(audio_r),
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
        .brg_sdram_b_we(1'b0), .brg_sdram_b_oe(1'b0), .brg_sdram_b_addr(25'd0), .brg_sdram_b_din(8'd0),
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
        .sdram_stream_req(), .sdram_stream_addr(), .sdram_stream_words(), .sdram_stream_ready(),
        .sdram_stream_dout(16'h0000), .sdram_stream_valid(1'b0), .sdram_stream_busy(1'b0), .sdram_stream_done(1'b0)
    );

    localparam [15:0] BTN         = 16'hBA9C;
    localparam [15:0] SW          = 16'hBA9D;
    localparam [15:0] IRQ_ENABLE  = 16'hBA9E;
    localparam [15:0] IRQ_STATUS  = 16'hBA9F;
    localparam [15:0] BTN_CHANGES = 16'hBAA0;
    localparam [15:0] SW_CHANGES  = 16'hBAA1;

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

    // Issue a debug peek; sample after the 2-cycle settle (matches do_peek in
    // the other top tests). Leaves dbg_peek_en deasserted on exit.
    task automatic do_peek(input logic [15:0] addr, output logic [7:0] result);
        @(posedge clk);
        dbg_peek_addr <= addr;
        dbg_peek_en   <= 1;
        @(posedge clk);
        @(posedge clk);
        result = dbg_peek_data;
        @(posedge clk);
        dbg_peek_en <= 0;
    endtask

    // Issue a debug poke (write-1-to-clear for the change/irq latches).
    task automatic do_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en   <= 1;
        @(posedge clk);
        dbg_poke_en <= 0;
        @(posedge clk);
    endtask

    logic [7:0] v;

    initial begin
        rst           = 1;
        dbg_cpu_reset = 1;   // hold CPU quiet; board input is CPU-independent
        dbg_pause     = 1;
        dbg_peek_en   = 0;
        dbg_poke_en   = 0;
        dbg_peek_addr = 0;
        dbg_poke_addr = 0;
        dbg_poke_data = 0;
        dbg_rom_we    = 0;
        dbg_rom_idx   = 0;
        dbg_rom_addr  = 0;
        dbg_rom_data  = 0;
        repeat (8) @(posedge clk);
        rst = 0;
        repeat (4) @(posedge clk);

        $display("test_board_input_top: board-input controller ($BA9C-$BAA1)");

        // 1. Idle — everything zero after reset.
        do_peek(BTN, v);         check_eq8("idle buttons == 0", v, 8'h00);
        do_peek(SW, v);          check_eq8("idle switches == 0", v, 8'h00);
        do_peek(BTN_CHANGES, v); check_eq8("idle button-changes == 0", v, 8'h00);
        do_peek(IRQ_STATUS, v);  check_eq8("idle irq-status == 0", v, 8'h00);

        // 2. Press UP|FIRE1 ($08|$02 = $0A): register reflects it, edge captured,
        //    button IRQ raised.
        board_buttons = 8'h0A;
        repeat (3) @(posedge clk);
        do_peek(BTN, v);         check_eq8("UP|FIRE1 read back", v, 8'h0A);
        do_peek(BTN_CHANGES, v); check_eq8("button edge captured", v, 8'h0A);
        do_peek(IRQ_STATUS, v);  check_eq8("button irq raised", v, 8'h01);

        // 3. Write-1-to-clear the change latch + irq; held buttons stay set.
        do_poke(BTN_CHANGES, 8'h0A);
        do_poke(IRQ_STATUS, 8'h01);
        repeat (2) @(posedge clk);
        do_peek(BTN_CHANGES, v); check_eq8("button-changes cleared (W1C)", v, 8'h00);
        do_peek(IRQ_STATUS, v);  check_eq8("irq-status cleared (W1C)", v, 8'h00);
        do_peek(BTN, v);         check_eq8("buttons still held after W1C", v, 8'h0A);

        // 4. Full byte masks to $7F (all 7 buttons incl PWR bit0).
        board_buttons = 8'hFF;
        repeat (3) @(posedge clk);
        do_peek(BTN, v);         check_eq8("buttons masked to $7F", v, 8'h7F);

        // 5. Release; clear residual edge/irq from the transitions.
        board_buttons = 8'h00;
        repeat (3) @(posedge clk);
        do_peek(BTN, v);         check_eq8("buttons released == 0", v, 8'h00);
        do_poke(BTN_CHANGES, 8'h7F);
        do_poke(IRQ_STATUS, 8'h03);
        repeat (2) @(posedge clk);

        // 6. Switches SW1|SW3 ($01|$04 = $05): read, edge, switch IRQ.
        board_switches = 8'h05;
        repeat (3) @(posedge clk);
        do_peek(SW, v);          check_eq8("SW1|SW3 read back", v, 8'h05);
        do_peek(SW_CHANGES, v);  check_eq8("switch edge captured", v, 8'h05);
        do_peek(IRQ_STATUS, v);  check_eq8("switch irq raised", v, 8'h02);

        // 7. Switch byte masks to $0F.
        board_switches = 8'hFF;
        repeat (3) @(posedge clk);
        do_peek(SW, v);          check_eq8("switches masked to $0F", v, 8'h0F);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #5000000;
        $display("FAIL: global timeout hit");
        $finish;
    end

endmodule
