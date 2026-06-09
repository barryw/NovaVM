// Integrated reproduction test: debug_spi_slave + CDC FIFOs + debug_bridge.
//
// Why this exists
// ---------------
// On hardware, ~7% of Nova Chess splash loads come up blank because a
// `pokeVgcBlock` (CMD_POKE_VGC_BLK) returns false: the ESP issues the write,
// then `recvStatus()` never sees the bridge's 0x00 status byte and times out.
// The HW classifier proved the CPU pause works and that completed writes are
// always correct — the sole failure is a lost/mis-framed status byte during a
// long (~555-poke) VGC write burst, worsening under concurrent bus traffic.
//
// The existing unit tests never exercise the path where the bug lives:
//   * test_debug_spi_slave.sv tests the slave transport WITHOUT the bridge.
//   * test_debug_bridge.sv drives the bridge rx/tx directly, bypassing the
//     SPI slave and both CDC FIFOs.
//   * Both phase-lock spi_sck to clk, so the clk_pixel<->spi_sck async FIFO
//     boundary (where a queued status byte must cross) is never stressed.
//
// This testbench wires the slave to the bridge exactly as fpga_top.sv does,
// drives a faithful ESP SPI master on a GENUINELY INDEPENDENT spi_sck clock,
// and runs the real failing operation: a burst of CMD_POKE_VGC_BLK(256) each
// followed by recvStatus(), with async fio/nic event injection. Every status
// must be 0x00 and every written byte must land in the VGC shadow.

`timescale 1ns/1ps

module test_debug_bridge_spi_burst;

    // ---- Protocol constants (mirror fpga_bridge.h / debug_bridge.sv) ----
    localparam logic [7:0] SPI_WRITE_OP    = 8'h57;  // 'W'
    localparam logic [7:0] SPI_READ_OP     = 8'h52;  // 'R'
    localparam logic [7:0] SPI_TOKEN_EMPTY = 8'h00;
    localparam logic [7:0] SPI_TOKEN_DATA  = 8'h01;
    localparam logic [7:0] CMD_POKE_VGC_BLK = 8'h10;
    localparam logic [7:0] CMD_PEEK_BLOCK   = 8'h09;
    localparam logic [7:0] CMD_PAUSE        = 8'h07;
    localparam logic [7:0] CMD_RESUME       = 8'h08;
    localparam logic [7:0] EVENT_MARKER    = 8'hFE;

    // ESP master constants (fpga_bridge.cpp): each recvSpiByte READ transaction
    // polls up to SPI_READ_POLL_BYTES tokens; recvByte retries transactions
    // until BYTE_TIMEOUT_MS. We model the timeout as a finite transaction
    // budget so a genuine drop manifests as a bounded-time FAIL, not a hang.
    localparam int SPI_READ_POLL_BYTES = 8;
    localparam int READ_TX_BUDGET      = 400;   // ~ "200 ms" of polling
    int recv_budget = READ_TX_BUDGET;           // overridable per-call budget

    localparam int BURST_POKES = 150;           // span/clear pokes per nvgload
    localparam int VGC_GFX_SPACE = 3;

    // ---- Bridge/pixel clock: free-running, independent of spi_sck.
    // Half-period is a runtime variable so the test can sweep the
    // clk_pixel<->spi_sck frequency ratio (incl. the near-1:1 slow beat). ----
    int   clk_half = 20;          // 25 MHz default
    logic clk = 0;
    always #(clk_half) clk = ~clk;

    logic rst;

    // ---- SPI pins (driven by the ESP master process) ----
    logic spi_sck  = 1'b0;
    logic spi_cs_n = 1'b1;
    logic spi_mosi = 1'b0;
    wire  spi_miso;
    wire  spi_miso_oe;

    // SPI half-bit period in ns. Deliberately incommensurate with the 20 ns
    // clk half-period so the clk_pixel<->spi_sck phase relationship sweeps
    // across the burst (this is what exposes CDC/transaction-boundary races).
    int sck_half = 13;            // ~25.6 MHz bit rate at the 3-phase cadence

    // ---- slave <-> bridge nets ----
    wire [7:0] s_rx_data;
    wire       s_rx_valid;
    wire       b_rx_ready;
    // fpga_top.sv gates the slave's rx_ready with its own rx_valid.
    wire       s_rx_ready = b_rx_ready && s_rx_valid;

    wire [7:0] b_tx_data;
    wire       b_tx_start;
    wire       s_tx_busy;

    wire rx_overflow, tx_overflow, tx_underflow, selected_seen;

    debug_spi_slave #(
        .RX_ADDR_WIDTH(12),
        .TX_ADDR_WIDTH(9),
        .IDLE_BYTE(8'hA5)
    ) slave (
        .clk(clk), .rst(rst),
        .spi_sck(spi_sck), .spi_cs_n(spi_cs_n), .spi_mosi(spi_mosi),
        .spi_miso(spi_miso), .spi_miso_oe(spi_miso_oe),
        .rx_data(s_rx_data), .rx_valid(s_rx_valid), .rx_ready(s_rx_ready),
        .tx_data(b_tx_data), .tx_start(b_tx_start), .tx_busy(s_tx_busy),
        .rx_overflow(rx_overflow), .tx_overflow(tx_overflow),
        .tx_underflow(tx_underflow), .selected_seen(selected_seen)
    );

    // ---- bridge debug-port stubs + VGC write shadow ----
    wire        dbg_peek_en;  wire [15:0] dbg_peek_addr;  logic [7:0] dbg_peek_data = 8'h00;
    wire        dbg_poke_en;  wire [15:0] dbg_poke_addr;  wire [7:0]  dbg_poke_data;
    wire        dbg_pause;
    wire        dbg_nic_buf_we, dbg_nic_buf_re, dbg_nic_buf_sel;
    wire [7:0]  dbg_nic_buf_addr, dbg_nic_buf_data;  logic [7:0] dbg_nic_buf_rdata = 8'h00;
    wire        dbg_vmem_we, dbg_vmem_re;
    wire [2:0]  dbg_vmem_space;  wire [16:0] dbg_vmem_addr;  wire [7:0] dbg_vmem_data;
    logic [7:0] dbg_vmem_rdata = 8'h00;
    wire        dbg_rom_we, dbg_rom_idx;  wire [13:0] dbg_rom_addr;  wire [7:0] dbg_rom_data;
    wire        dbg_cpu_reset, dbg_system_reset, dbg_cpu_resume;
    wire        key_inject_valid;  wire [7:0] key_inject_data;
    wire        sdram_b_we, sdram_b_oe;  wire [24:0] sdram_b_addr;  wire [7:0] sdram_b_din;
    wire [7:0]  host_status;
    wire        audio_pcm_we;  wire [7:0] audio_pcm_data;
    wire        wts_event_we;  wire [7:0] wts_event_data;

    // VGC gfx shadow — captures every byte the bridge streams via dbg_vmem_we.
    // Sized for the gfx space (76800 bytes); other spaces ignored here.
    logic [7:0] vgc_shadow [0:131071];
    logic       vgc_written [0:131071];

    always_ff @(posedge clk) begin
        if (dbg_vmem_we) begin
            vgc_shadow[dbg_vmem_addr]  <= dbg_vmem_data;
            vgc_written[dbg_vmem_addr] <= 1'b1;
        end
    end

    // ---- async event injection ----
    logic fio_event = 1'b0;
    logic nic_event = 1'b0;

    debug_bridge #(
        .BOOT_AUTO_RELEASE_CYCLES(1_000_000),
        .RX_RESYNC_STALL_CYCLES(600)     // small so recovery is fast in sim
    ) bridge (
        .clk(clk), .rst(rst),
        .rx_data(s_rx_data), .rx_valid(s_rx_valid), .rx_ready(b_rx_ready),
        .tx_data(b_tx_data), .tx_start(b_tx_start), .tx_busy(s_tx_busy),
        .dbg_peek_en(dbg_peek_en), .dbg_peek_addr(dbg_peek_addr), .dbg_peek_data(dbg_peek_data),
        .dbg_poke_en(dbg_poke_en), .dbg_poke_addr(dbg_poke_addr), .dbg_poke_data(dbg_poke_data),
        .dbg_pause(dbg_pause),
        .dbg_nic_buf_we(dbg_nic_buf_we), .dbg_nic_buf_re(dbg_nic_buf_re),
        .dbg_nic_buf_sel(dbg_nic_buf_sel), .dbg_nic_buf_addr(dbg_nic_buf_addr),
        .dbg_nic_buf_data(dbg_nic_buf_data), .dbg_nic_buf_rdata(dbg_nic_buf_rdata),
        .dbg_vmem_we(dbg_vmem_we), .dbg_vmem_re(dbg_vmem_re),
        .dbg_vmem_space(dbg_vmem_space), .dbg_vmem_addr(dbg_vmem_addr),
        .dbg_vmem_data(dbg_vmem_data), .dbg_vmem_rdata(dbg_vmem_rdata),
        .dbg_rom_we(dbg_rom_we), .dbg_rom_idx(dbg_rom_idx),
        .dbg_rom_addr(dbg_rom_addr), .dbg_rom_data(dbg_rom_data),
        .dbg_cpu_reset(dbg_cpu_reset), .dbg_system_reset(dbg_system_reset),
        .dbg_cpu_resume(dbg_cpu_resume),
        .dbg_cpu_pc(16'h1234), .dbg_cpu_a(8'hAA), .dbg_cpu_x(8'hBB), .dbg_cpu_y(8'hCC),
        .dbg_cpu_sp(8'hFF), .dbg_cpu_flags(8'h20), .dbg_cpu_state(6'd12), .dbg_cpu_ir(8'hEA),
        .dbg_cpu_addr(16'h1234), .dbg_cpu_din(8'hEA), .dbg_cpu_dout(8'h00),
        .dbg_cpu_we(1'b0), .dbg_cpu_rdy(1'b0), .dbg_cpu_irq(1'b0), .dbg_cpu_nmi(1'b0),
        .dbg_cpu_waiting(1'b0), .dbg_cpu_stopped(1'b0),
        .key_inject_valid(key_inject_valid), .key_inject_data(key_inject_data),
        .key_inject_ready(1'b1),
        .sdram_b_we(sdram_b_we), .sdram_b_oe(sdram_b_oe), .sdram_b_addr(sdram_b_addr),
        .sdram_b_din(sdram_b_din), .sdram_b_dout(8'h00), .sdram_b_done_toggle(1'b0),
        .fio_event(fio_event), .nic_event(nic_event), .host_status(host_status),
        .audio_pcm_we(audio_pcm_we), .audio_pcm_data(audio_pcm_data),
        .audio_pcm_ready(1'b1), .audio_pcm_space(16'd4096), .audio_pcm_underruns(16'd0),
        .wts_event_we(wts_event_we), .wts_event_data(wts_event_data), .wts_event_ready(1'b1)
    );

    // ---- accounting ----
    int pass_count = 0;
    int fail_count = 0;

    task automatic check(input string name, input logic cond);
        if (cond) pass_count++;
        else begin
            $display("  FAIL %s", name);
            fail_count++;
        end
    endtask

    // =====================================================================
    // ESP SPI master model (mode-0, MSB first) — mirrors fpga_bridge.cpp.
    // spi_sck is driven with #ns delays, independent of clk.
    // =====================================================================
    task automatic spi_pulse_bit(input logic mosi_bit, output logic miso_bit);
        spi_mosi <= mosi_bit;
        #(sck_half);                 // low phase: data set up, sample MISO
        miso_bit = spi_miso;
        spi_sck <= 1'b1;
        #(sck_half);                 // high phase
        spi_sck <= 1'b0;
        #(sck_half);                 // low tail
    endtask

    task automatic spi_transfer(input logic [7:0] din, output logic [7:0] dout);
        logic mb;
        dout = 8'h00;
        for (int i = 7; i >= 0; i--) begin
            spi_pulse_bit(din[i], mb);
            dout = {dout[6:0], mb};
        end
    endtask

    task automatic spi_select();
        spi_sck <= 1'b0; spi_cs_n <= 1'b0; #(sck_half * 2);
    endtask

    task automatic spi_deselect();
        spi_sck <= 1'b0; spi_cs_n <= 1'b1; #(sck_half * 2);
    endtask

    // One logical inbound byte: poll READ transactions until a DATA token,
    // up to READ_TX_BUDGET transactions. Returns -1 on timeout.
    task automatic esp_recv_byte(output int value);
        logic [7:0] dummy, token, payload;
        for (int t = 0; t < recv_budget; t++) begin
            spi_select();
            spi_transfer(SPI_READ_OP, dummy);
            for (int p = 0; p < SPI_READ_POLL_BYTES; p++) begin
                spi_transfer(8'h00, token);
                if (token == SPI_TOKEN_DATA) begin
                    spi_transfer(8'h00, payload);
                    spi_deselect();
                    value = int'(payload);
                    return;
                end
            end
            spi_deselect();
        end
        value = -1;
    endtask

    // recvStatus(): drain 0xFE event pairs inline, return the real status.
    task automatic esp_recv_status(output bit ok, output int status);
        int b, ev;
        forever begin
            esp_recv_byte(b);
            if (b < 0) begin ok = 1'b0; status = -1; return; end
            if (b == int'(EVENT_MARKER)) begin
                esp_recv_byte(ev);             // event type, discard
                if (ev < 0) begin ok = 1'b0; status = -1; return; end
                continue;
            end
            status = b;
            ok = (b == 0);
            return;
        end
    endtask

    // CMD_POKE_VGC_BLK write transaction: W + [cmd,space,hi,lo,count] + data.
    task automatic esp_write_poke_vgc(input int unsigned addr, input int unsigned count,
                                      input logic [7:0] seed);
        logic [7:0] dummy;
        spi_select();
        spi_transfer(SPI_WRITE_OP, dummy);
        spi_transfer(CMD_POKE_VGC_BLK, dummy);
        spi_transfer(VGC_GFX_SPACE[7:0], dummy);
        spi_transfer(addr[15:8], dummy);
        spi_transfer(addr[7:0], dummy);
        spi_transfer((count == 256) ? 8'h00 : count[7:0], dummy);
        for (int i = 0; i < count; i++)
            spi_transfer(seed ^ i[7:0], dummy);
        spi_deselect();
    endtask

    // recvBytes(): bulk read WITHOUT 0xFE event filtering — faithful to the
    // real firmware recvBytes() used by peekBlock/readVgcBlock. ok=0 on timeout.
    task automatic esp_recv_bytes(input int n, output bit ok);
        int b;
        for (int i = 0; i < n; i++) begin
            esp_recv_byte(b);
            if (b < 0) begin ok = 1'b0; return; end
        end
        ok = 1'b1;
    endtask

    // CMD_PAUSE / CMD_RESUME: single opcode byte, single status ack.
    task automatic esp_cmd_byte(input logic [7:0] op, output bit ok, output int status);
        logic [7:0] dummy;
        spi_select();
        spi_transfer(SPI_WRITE_OP, dummy);
        spi_transfer(op, dummy);
        spi_deselect();
        esp_recv_status(ok, status);
    endtask

    // CMD_PEEK_BLOCK addr,count: status + count bulk bytes (count=0 => 256).
    task automatic esp_peek_block(input int unsigned addr, input int count,
                                  output bit ok, output int status);
        logic [7:0] dummy;
        bit bok;
        spi_select();
        spi_transfer(SPI_WRITE_OP, dummy);
        spi_transfer(CMD_PEEK_BLOCK, dummy);
        spi_transfer(addr[15:8], dummy);
        spi_transfer(addr[7:0], dummy);
        spi_transfer((count == 256) ? 8'h00 : count[7:0], dummy);
        spi_deselect();
        esp_recv_status(ok, status);
        if (ok) begin
            esp_recv_bytes((count == 0) ? 256 : count, bok);
            ok = bok;
        end
    endtask

    // A glitch on spi_sck/mosi can drop bytes, so the slave registers FEWER
    // payload bytes than the ESP clocked out: a truncated command. The bridge
    // then waits forever for the missing bytes while the ESP moves on to
    // read-only recvStatus polling -> the exact 200 ms timeout / blank splash.
    // This task reproduces that by claiming `claimed` bytes but sending `sent`.
    task automatic esp_write_truncated_poke(input int unsigned addr,
                                            input int claimed, input int sent);
        logic [7:0] dummy;
        spi_select();
        spi_transfer(SPI_WRITE_OP, dummy);
        spi_transfer(CMD_POKE_VGC_BLK, dummy);
        spi_transfer(VGC_GFX_SPACE[7:0], dummy);
        spi_transfer(addr[15:8], dummy);
        spi_transfer(addr[7:0], dummy);
        spi_transfer((claimed == 256) ? 8'h00 : claimed[7:0], dummy);
        for (int i = 0; i < sent; i++)
            spi_transfer(8'hC3 ^ i[7:0], dummy);
        spi_deselect();                       // CS high with sent < claimed
    endtask

    // =====================================================================
    // Background process: pulse async host events while the burst runs, the
    // way the CPU/NIC do on hardware. 1-clk pulses at clk-domain phases that
    // are uncorrelated with the SPI transaction boundaries.
    // =====================================================================
    logic events_enabled = 1'b0;
    initial begin
        forever begin
            @(posedge clk);
            if (events_enabled) begin
                repeat (137) @(posedge clk);
                fio_event <= 1'b1; @(posedge clk); fio_event <= 1'b0;
                repeat (91) @(posedge clk);
                nic_event <= 1'b1; @(posedge clk); nic_event <= 1'b0;
            end
        end
    end

    // =====================================================================
    // Main stimulus
    //
    // Sweeps the SCK half-bit period (hence the clk_pixel<->spi_sck phase
    // relationship) across fast-drift and slow-beat-near-clk values, and for
    // each runs the real handle_nvgload command sequence with async events:
    //   fio_event -> CMD_PEEK_BLOCK(80) -> CMD_PAUSE -> N x POKE_VGC_BLK(256)
    //   -> CMD_RESUME.
    // Every status must be 0x00; a dropped/mis-framed status surfaces as an
    // esp_recv_status timeout (ok=0) — exactly the HW $02/blank-splash failure.
    // =====================================================================
    integer i, off;
    bit     ok;
    int     status;
    int     total_fail;
    int     poke_seq;

    // Cross-sweep clk and sck half-periods. Near-equal pairs hold a phase
    // alignment for many bytes (worst-case sustained CDC near-miss); coprime
    // pairs drift quickly through all alignments.
    localparam int N_CLK = 4;
    localparam int N_SCK = 6;
    int clk_sweep [0:N_CLK-1] = '{20, 19, 18, 21};
    int sck_sweep [0:N_SCK-1] = '{19, 20, 21, 18, 11, 39};

    task automatic run_nvgload(input int unsigned base, input logic [7:0] seed);
        // Trigger event (as the CPU write of CMD_NVGLOAD does on HW).
        @(posedge clk); fio_event <= 1'b1; @(posedge clk); fio_event <= 1'b0;
        // handle_event reads the 80-byte register bank first.
        esp_peek_block(16'hB9A0, 80, ok, status);
        if (!ok) begin
            $display("  FAIL peek_block(80) seed=$%02x sck=%0d ok=%0b status=%0d",
                     seed, sck_half, ok, status);
            total_fail++;
        end
        // CpuPauseGuard.pause()
        esp_cmd_byte(CMD_PAUSE, ok, status);
        if (!ok) begin
            $display("  FAIL pause seed=$%02x sck=%0d status=%0d", seed, sck_half, status);
            total_fail++;
        end
        // clear_vgc_gfx + span writes: the VGC write burst (CPU paused).
        for (i = 0; i < BURST_POKES; i++) begin
            off = (base + i * 256) & 16'hFFFF;
            esp_write_poke_vgc(off, 256, seed ^ i[7:0]);
            esp_recv_status(ok, status);
            poke_seq++;
            if (!ok) begin
                $display("  FAIL poke[%0d] seq=%0d addr=$%04x sck=%0d ok=%0b status=%0d",
                         i, poke_seq, off, sck_half, ok, status);
                total_fail++;
            end else pass_count++;
        end
        // CpuPauseGuard.resume()
        esp_cmd_byte(CMD_RESUME, ok, status);
        if (!ok) begin
            $display("  FAIL resume seed=$%02x sck=%0d status=%0d", seed, sck_half, status);
            total_fail++;
        end
    endtask

    initial begin
        rst = 1'b1;
        #200;
        @(posedge clk); rst = 1'b0;
        repeat (10) @(posedge clk);

        total_fail = 0;
        poke_seq   = 0;
        events_enabled = 1'b1;   // host/NIC/CPU async events throughout

        for (int c = 0; c < N_CLK; c++) begin
            clk_half = clk_sweep[c];
            for (int s = 0; s < N_SCK; s++) begin
                sck_half = sck_sweep[s];
                $display("Sweep clk=%0d sck=%0d ns: nvgload(%0d pokes) + async events",
                         clk_half, sck_half, BURST_POKES);
                run_nvgload(16'h0000, 8'h5A + (c * N_SCK + s));
            end
        end

        events_enabled = 1'b0;

        // ---- Corruption recovery: a glitch-truncated command must not wedge
        // the bridge forever. After a truncated poke (bridge left waiting for
        // missing bytes), a fresh poke + recvStatus MUST succeed. Without a
        // recovery watchdog the bridge stays stuck and this times out. ----
        $display("Phase R: glitch-truncated command, then recovery");
        clk_half = 20; sck_half = 19;
        recv_budget = 40;                      // short "timeout" to keep sim fast
        esp_write_truncated_poke(16'hD000, 256, 250);  // 6 bytes short (glitch)
        esp_recv_status(ok, status);           // ESP polls (READ only) — its poke failed
        check("truncated poke yields no valid status (wedge symptom)", !ok);
        // The next legitimate command (fresh WRITE transaction) must be parsed
        // cleanly and land its DATA at the right addresses. A persistent
        // byte-desync silently routes this poke's bytes wrong even though a
        // stale 0x00 status may come back — so verify the VGC shadow, not ok.
        esp_write_poke_vgc(16'hE000, 256, 8'h77);
        esp_recv_status(ok, status);
        recv_budget = READ_TX_BUDGET;
        begin
            automatic int bad = 0;
            for (int k = 0; k < 256; k++)
                if (!(vgc_written[16'hE000 + k] &&
                      (vgc_shadow[16'hE000 + k] === (8'h77 ^ k[7:0]))))
                    bad++;
            if (bad != 0) begin
                $display("  FAIL recovery: bridge desynced by truncated command (%0d/256 fresh bytes wrong/missing)", bad);
                fail_count++;
            end else begin
                $display("  PASS recovery: fresh poke landed correctly after corruption");
                pass_count++;
            end
        end

        fail_count += total_fail;

        // Verify the final block's bytes actually landed in the VGC shadow.
        off = ((BURST_POKES - 1) * 256) & 16'hFFFF;
        for (int k = 0; k < 256; k++)
            check($sformatf("final-block byte %0d landed", k), vgc_written[off + k]);

        check("no RX FIFO overflow", !rx_overflow);
        check("no TX FIFO overflow", !tx_overflow);

        $display("");
        if (fail_count == 0) begin
            $display("All debug bridge SPI burst checks passed (%0d pokes, %0d checks)",
                     poke_seq, pass_count);
            $finish;
        end else begin
            $display("%0d debug bridge SPI burst checks FAILED (%0d pokes issued)",
                     fail_count, poke_seq);
            $fatal;
        end
    end

    // Global watchdog so a true wedge fails instead of hanging CI.
    initial begin
        #2_000_000_000;
        $display("WATCHDOG TIMEOUT — bridge wedged (this reproduces the HW hang)");
        $fatal;
    end

endmodule
