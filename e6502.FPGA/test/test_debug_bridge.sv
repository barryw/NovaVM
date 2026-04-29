// Unit test for debug_bridge.sv — exercises the binary command protocol
// by driving rx_valid/rx_data directly (no real UART bit-shifting), then
// observes the debug-port outputs and the tx side.
//
// Critical for Phase 2 of the ROM-load rollout: the fpga-side bridge has
// to decode CMD_POKE_ROM and CMD_POKE_ROM_BLK correctly or the 17-min
// hardware build has nothing to show for it. Verilator runs this in ms.

`timescale 1ns/1ps

module test_debug_bridge;

    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic rst;

    // UART-facing signals (we drive rx, ignore tx except for ack observation)
    logic [7:0] rx_data;
    logic       rx_valid;
    wire  [7:0] tx_data;
    wire        tx_start;
    logic       tx_busy;

    // Debug outputs from the bridge
    wire        dbg_peek_en;
    wire [15:0] dbg_peek_addr;
    logic [7:0] dbg_peek_data;
    wire        dbg_poke_en;
    wire [15:0] dbg_poke_addr;
    wire [7:0]  dbg_poke_data;
    wire        dbg_pause;
    wire        dbg_rom_we;
    wire        dbg_rom_idx;
    wire [13:0] dbg_rom_addr;
    wire [7:0]  dbg_rom_data;
    wire        dbg_cpu_reset;

    // CPU state (stubbed)
    logic [15:0] dbg_cpu_pc    = 16'h1234;
    logic [7:0]  dbg_cpu_a     = 8'hAA;
    logic [7:0]  dbg_cpu_x     = 8'hBB;
    logic [7:0]  dbg_cpu_y     = 8'hCC;
    logic [7:0]  dbg_cpu_sp    = 8'hFF;
    logic [7:0]  dbg_cpu_flags = 8'h20;

    wire        key_inject_valid;
    wire [7:0]  key_inject_data;

    wire        sdram_b_we;
    wire [24:0] sdram_b_addr;
    wire [7:0]  sdram_b_din;

    // FIO event input — we pulse this to simulate a CPU write to $B9A0.
    logic       fio_event = 0;

    localparam integer TEST_BOOT_AUTO_RELEASE_CYCLES = 16;

    debug_bridge #(
        .BOOT_AUTO_RELEASE_CYCLES(TEST_BOOT_AUTO_RELEASE_CYCLES)
    ) dut (
        .clk(clk), .rst(rst),
        .rx_data(rx_data), .rx_valid(rx_valid),
        .tx_data(tx_data), .tx_start(tx_start), .tx_busy(tx_busy),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr),
        .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr),
        .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset),
        .dbg_cpu_pc(dbg_cpu_pc),
        .dbg_cpu_a(dbg_cpu_a), .dbg_cpu_x(dbg_cpu_x),
        .dbg_cpu_y(dbg_cpu_y), .dbg_cpu_sp(dbg_cpu_sp),
        .dbg_cpu_flags(dbg_cpu_flags),
        .key_inject_valid(key_inject_valid),
        .key_inject_data(key_inject_data),
        .sdram_b_we(sdram_b_we),
        .sdram_b_addr(sdram_b_addr),
        .sdram_b_din(sdram_b_din),
        .fio_event(fio_event)
    );

    // ------------------------------------------------------------------
    // ROM shadow — captures every byte the bridge writes via dbg_rom_we,
    // so we can verify it later. Two banks mirroring the real system.
    // ------------------------------------------------------------------
    logic [7:0] rom_shadow [0:1] [0:16383];

    initial begin
        for (int b = 0; b < 2; b++)
            for (int i = 0; i < 16384; i++)
                rom_shadow[b][i] = 8'hXX;
    end

    always_ff @(posedge clk) begin
        if (dbg_rom_we)
            rom_shadow[dbg_rom_idx][dbg_rom_addr] <= dbg_rom_data;
    end

    // ------------------------------------------------------------------
    // Accounting
    // ------------------------------------------------------------------
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

    // ------------------------------------------------------------------
    // Inject a single byte into the bridge (rx_valid one-cycle pulse)
    // ------------------------------------------------------------------
    task automatic send_byte(input logic [7:0] b);
        @(posedge clk);
        rx_data  <= b;
        rx_valid <= 1;
        @(posedge clk);
        rx_valid <= 0;
    endtask

    // Collect an outgoing ack. tx_busy is driven by us; the real uart_tx
    // would pull it high while transmitting. Fake minimal behavior: after
    // the bridge asserts tx_start, we accept the byte and pulse busy for
    // a couple of cycles to simulate TX completion.
    task automatic wait_tx(output logic [7:0] b);
        int timeout = 1000;
        @(posedge clk);
        while (!tx_start && timeout > 0) begin
            @(posedge clk);
            timeout--;
        end
        b = tx_data;
        // Pretend the TX completes quickly — the bridge samples tx_busy
        // in S_TX_WAIT. We already set tx_busy=0 as the default.
    endtask

    // ------------------------------------------------------------------
    // Tests
    // ------------------------------------------------------------------
    task automatic test_single_poke_rom();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_POKE_ROM single byte, idx=0, addr=$0100, data=$A5");
        send_byte(8'h0A);   // CMD_POKE_ROM
        send_byte(8'h00);   // idx
        send_byte(8'h01);   // addr_hi
        send_byte(8'h00);   // addr_lo
        send_byte(8'hA5);   // data
        wait_tx(ack);
        check_eq8("ack status", ack, 8'h00);
        check_eq8("rom_shadow[0][0x0100]", rom_shadow[0][14'h0100], 8'hA5);
    endtask

    task automatic test_single_poke_rom_idx1();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_POKE_ROM idx=1 routes to ext_rom");
        send_byte(8'h0A);
        send_byte(8'h01);   // idx=1
        send_byte(8'h3F);   // addr_hi
        send_byte(8'hFC);   // addr_lo → $3FFC
        send_byte(8'h77);
        wait_tx(ack);
        check_eq8("ack status", ack, 8'h00);
        check_eq8("rom_shadow[1][0x3FFC]", rom_shadow[1][14'h3FFC], 8'h77);
    endtask

    task automatic test_bulk_poke_rom();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_POKE_ROM_BLK — 16-byte block into basic_rom @ 0x0040");
        send_byte(8'h0D);   // CMD_POKE_ROM_BLK
        send_byte(8'h00);   // idx
        send_byte(8'h00);   // addr_hi
        send_byte(8'h40);   // addr_lo → $0040
        send_byte(8'h10);   // count = 16
        for (int i = 0; i < 16; i++)
            send_byte(8'hE0 + 8'(i));
        wait_tx(ack);
        check_eq8("ack status", ack, 8'h00);
        for (int i = 0; i < 16; i++)
            check_eq8($sformatf("rom_shadow[0][0x%04X]", 14'h0040 + i),
                      rom_shadow[0][14'h0040 + i], 8'hE0 + 8'(i));
    endtask

    // RAM-shadow capture for CMD_POKE_BLOCK validation. Mirrors the
    // dbg_poke writes that targeted CPU RAM (addr < ROM_BASE).
    logic [7:0] ram_shadow [0:65535];
    initial for (int i = 0; i < 65536; i++) ram_shadow[i] = 8'hFF;
    always_ff @(posedge clk) begin
        if (dbg_poke_en && dbg_poke_addr < 16'hC000)
            ram_shadow[dbg_poke_addr] <= dbg_poke_data;
    end

    task automatic test_poke_block_ram();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_POKE_BLOCK 8 bytes into RAM @ $0500");
        send_byte(8'h0F);   // CMD_POKE_BLOCK
        send_byte(8'h05);   // addr_hi
        send_byte(8'h00);   // addr_lo  → $0500
        send_byte(8'h08);   // count = 8
        send_byte(8'hAA);
        send_byte(8'hBB);
        send_byte(8'hCC);
        send_byte(8'hDD);
        send_byte(8'hEE);
        send_byte(8'hFF);
        send_byte(8'h11);
        send_byte(8'h22);
        wait_tx(ack);
        check_eq8("poke_block ack status", ack, 8'h00);
        check_eq8("ram[0x0500]", ram_shadow[16'h0500], 8'hAA);
        check_eq8("ram[0x0501]", ram_shadow[16'h0501], 8'hBB);
        check_eq8("ram[0x0502]", ram_shadow[16'h0502], 8'hCC);
        check_eq8("ram[0x0503]", ram_shadow[16'h0503], 8'hDD);
        check_eq8("ram[0x0504]", ram_shadow[16'h0504], 8'hEE);
        check_eq8("ram[0x0505]", ram_shadow[16'h0505], 8'hFF);
        check_eq8("ram[0x0506]", ram_shadow[16'h0506], 8'h11);
        check_eq8("ram[0x0507]", ram_shadow[16'h0507], 8'h22);
        check_eq8("ram[0x0508] untouched", ram_shadow[16'h0508], 8'hFF);
    endtask

    task automatic test_bulk_count_zero_means_256();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_POKE_ROM_BLK count=0 → 256 bytes");
        send_byte(8'h0D);
        send_byte(8'h01);   // idx=1
        send_byte(8'h20);   // addr_hi
        send_byte(8'h00);   // addr_lo → $2000
        send_byte(8'h00);   // count = 0 → 256
        for (int i = 0; i < 256; i++)
            send_byte(8'(i));
        wait_tx(ack);
        check_eq8("ack status", ack, 8'h00);
        check_eq8("ext_rom[0x2000]",  rom_shadow[1][14'h2000],       8'h00);
        check_eq8("ext_rom[0x2080]",  rom_shadow[1][14'h2000 + 128], 8'h80);
        check_eq8("ext_rom[0x20FF]",  rom_shadow[1][14'h2000 + 255], 8'hFF);
    endtask

    task automatic test_reset_hold_release();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_RESET_HOLD / CMD_RESET_REL");
        // On reset, bridge boots with dbg_cpu_reset=1 (CPU held)
        check("dbg_cpu_reset asserted at boot", dbg_cpu_reset === 1'b1);

        send_byte(8'h0C);   // CMD_RESET_REL
        wait_tx(ack);
        check_eq8("ack status (release)", ack, 8'h00);
        @(posedge clk);
        check("dbg_cpu_reset deasserted", dbg_cpu_reset === 1'b0);

        send_byte(8'h0B);   // CMD_RESET_HOLD
        wait_tx(ack);
        check_eq8("ack status (hold)", ack, 8'h00);
        @(posedge clk);
        check("dbg_cpu_reset reasserted", dbg_cpu_reset === 1'b1);
    endtask

    task automatic test_boot_auto_release_when_unclaimed();
        $display("");
        $display("Test: unclaimed boot auto-releases CPU reset");

        rst      <= 1;
        rx_data  <= 0;
        rx_valid <= 0;
        tx_busy  <= 0;
        fio_event <= 0;
        repeat(3) @(posedge clk);

        rst <= 0;
        repeat(2) @(posedge clk);
        check("dbg_cpu_reset asserted after reset", dbg_cpu_reset === 1'b1);

        repeat(TEST_BOOT_AUTO_RELEASE_CYCLES + 2) @(posedge clk);
        check("dbg_cpu_reset auto-released", dbg_cpu_reset === 1'b0);
    endtask

    // ------------------------------------------------------------------
    // SDRAM port B shadow — captures bytes the bridge writes via
    // sdram_b_we. Addresses are 25-bit byte addresses.
    // ------------------------------------------------------------------
    logic [7:0] sdram_shadow [0:255];
    logic [7:0] sdram_prev_we;
    logic [24:0] sdram_prev_addr;

    always_ff @(posedge clk) begin
        // Capture on the rising edge of weB (first cycle of the 8-cycle
        // hold) so each byte is stored exactly once.
        if (sdram_b_we && !sdram_prev_we) begin
            if (sdram_b_addr < 25'd256)
                sdram_shadow[sdram_b_addr[7:0]] <= sdram_b_din;
        end
        sdram_prev_we   <= sdram_b_we;
        sdram_prev_addr <= sdram_b_addr;
    end

    task automatic test_bulk_poke_sdram();
        logic [7:0] ack;
        $display("");
        $display("Test: CMD_POKE_SDRAM_BLK — 8 bytes to addr 0x000010");
        for (int i = 0; i < 256; i++) sdram_shadow[i] = 8'hFF;
        send_byte(8'h0E);   // CMD_POKE_SDRAM_BLK
        send_byte(8'h00);   // addr_hi
        send_byte(8'h00);   // addr_mid
        send_byte(8'h10);   // addr_lo → 0x000010
        send_byte(8'h08);   // count = 8
        // Each SDRAM write holds weB for 8 pixel-clocks. UART runs at
        // ~2150 clocks/byte at 115200 baud, so inter-byte gap is never
        // the bottleneck in production. The testbench must emulate that
        // pacing — clock-rate send_byte would overrun the hold window.
        for (int i = 0; i < 8; i++) begin
            send_byte(8'hC0 + 8'(i));
            repeat(12) @(posedge clk);
        end
        wait_tx(ack);
        check_eq8("sdram ack status", ack, 8'h00);
        for (int i = 0; i < 8; i++)
            check_eq8($sformatf("sdram_shadow[0x%04X]", 16 + i),
                      sdram_shadow[16 + i], 8'hC0 + 8'(i));
        // Boundaries untouched.
        check_eq8("sdram_shadow[0x000F] untouched", sdram_shadow[15], 8'hFF);
        check_eq8("sdram_shadow[0x0018] untouched", sdram_shadow[24], 8'hFF);
    endtask

    // ------------------------------------------------------------------
    // FIO event emission tests
    // ------------------------------------------------------------------
    task automatic pulse_fio_event();
        @(posedge clk);
        fio_event <= 1;
        @(posedge clk);
        fio_event <= 0;
    endtask

    task automatic test_fio_event_emission();
        logic [7:0] mark, type_byte;
        $display("");
        $display("Test: fio_event pulse emits EVENT_MARKER + EVENT_TYPE_FIO");
        pulse_fio_event();
        wait_tx(mark);
        check_eq8("event marker byte", mark, 8'hFE);
        wait_tx(type_byte);
        check_eq8("event type byte (FIO)", type_byte, 8'hE0);
    endtask

    // If fio_event fires while a CMD is mid-flight (between RX of first
    // byte and TX of final response byte), the event must NOT interleave
    // with the response stream. It must emit only after the CMD response
    // has fully completed.
    task automatic test_fio_event_queued_during_cmd();
        logic [7:0] ack, mark, type_byte;
        $display("");
        $display("Test: fio_event fires mid-CMD — waits for CMD to finish");
        // Begin a CMD — send all bytes; bridge enters S_RECV, then dispatches.
        send_byte(8'h0A);   // CMD_POKE_ROM
        send_byte(8'h00);   // idx
        send_byte(8'h02);   // addr_hi
        send_byte(8'h00);   // addr_lo
        // Pulse event while CMD is still in S_RECV (before last byte).
        pulse_fio_event();
        send_byte(8'h77);   // data (final param byte → dispatch)

        // CMD ack should TX first — event is queued.
        wait_tx(ack);
        check_eq8("CMD ack before queued event", ack, 8'h00);

        // Queued event drains next.
        wait_tx(mark);
        check_eq8("event marker after CMD", mark, 8'hFE);
        wait_tx(type_byte);
        check_eq8("event type after CMD", type_byte, 8'hE0);
    endtask

    // After an event is emitted, a second pulse must fire a second event.
    // Regression: event_pending flag must clear properly after emission.
    task automatic test_fio_event_no_retrigger_when_cleared();
        logic [7:0] mark, type_byte;
        int timeout, saw_start;
        $display("");
        $display("Test: no spurious event without new fio_event pulse");
        // Drain any residual tx_start pulses from the previous test, then
        // verify no new tx_start fires for a quiet window.
        repeat(20) @(posedge clk);
        saw_start = 0;
        for (timeout = 0; timeout < 50; timeout++) begin
            @(posedge clk);
            if (tx_start) saw_start = 1;
        end
        check("no event TX when not triggered", saw_start == 0);

        // Second pulse should fire another event.
        pulse_fio_event();
        wait_tx(mark);
        check_eq8("second event marker", mark, 8'hFE);
        wait_tx(type_byte);
        check_eq8("second event type", type_byte, 8'hE0);
    endtask

    // ------------------------------------------------------------------
    // Runner
    // ------------------------------------------------------------------
    initial begin
        $display("=== debug_bridge protocol tests ===");
        rst      = 1;
        rx_data  = 0;
        rx_valid = 0;
        tx_busy  = 0;
        dbg_peek_data = 8'h00;
        repeat(10) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);

        test_reset_hold_release();
        test_single_poke_rom();
        test_single_poke_rom_idx1();
        test_bulk_poke_rom();
        test_bulk_count_zero_means_256();
        test_bulk_poke_sdram();
        test_poke_block_ram();
        test_fio_event_emission();
        test_fio_event_queued_during_cmd();
        test_fio_event_no_retrigger_when_cleared();
        test_boot_auto_release_when_unclaimed();

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
