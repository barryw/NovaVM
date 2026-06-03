// test_page_in_top.sv — Task 11d integration capstone.
//
// Proves the FULL page-in path end-to-end through the REAL top.sv (-DSYNTHESIS):
//
//   CPU executes a 6502 program in basic_rom that writes the page-in MMIO
//   register block ($BA76-$BA7C): PGD_SRC* = XRAM/SDRAM byte base, PGD_WORDS*
//   = 8192, PGD_CMD = 0x01. The CMD write raises a pixel-domain `busy`, which
//   drives the CPU RDY low (pgd_rdy). A pixel->sdram CDC start pulse kicks the
//   sdram_clk-domain page_dma core, which page-mode-streams 8192 words (16 KB)
//   from the SDRAM through sdram.v's stream port, serializing each word into
//   the two 8-bit byte writes of bank-1 ext_rom (dpram_dc port A, sdram_clk).
//   On completion page_dma's `done` crosses sdram->pixel, clears busy, and the
//   CPU resumes. The next STATUS read shows done/ok.
//
// HARNESS: top.sv (DUT) + a REAL sdram.v stream consumer + the sdram_model
//   timing oracle. top.sv's exposed stream port wires to sdram.v's stream port;
//   sdram.v's port A/B are tied off here (the CPU is stalled during the page-in
//   so XRAM port A is quiescent — N1). A SEPARATE faster sdram_clk genuinely
//   exercises the pixel<->sdram CDC (unlike the same-clock top tests).
//
// PATTERN (unique-per-word so a dropped/wrong-row/skewed word is caught):
//   word(i) = {i[7:0], i[15:8]}  ->  ext_rom[2i] = i[7:0], ext_rom[2i+1] = i[15:8]
//
// VERIFY:
//   - STATUS busy went high during the copy, then cleared with ok=1.
//   - ext_rom_inst.mem[] holds the 16 KB pattern byte-exact + correct order.
//   - CPU reads $C000.. via ROMSWAP_EXT and lands the resident bytes in RAM.
//
// Run: make -C test test_page_in_top

`timescale 1ns/1ps

`include "sdram_model.svh"

module test_page_in_top;

    // -----------------------------------------------------------------------
    // Clocks. Pixel clk ~25 MHz drives the CPU/top.sv. sdram_clk ~85 MHz drives
    // sdram.v + the stream FSM. The two are asynchronous, so the CDC in
    // page_in_ctrl is genuinely exercised. clkref toggled 16:1 off sdram_clk
    // (the ratio sdram.v requires — see [[feedback_sdram_clkref_16_1]]).
    // -----------------------------------------------------------------------
    logic clk = 0;          // pixel
    always #20 clk = ~clk;  // 25 MHz

    logic sdram_clk = 0;
    always #5.88 sdram_clk = ~sdram_clk;   // ~85 MHz

    logic [3:0] clkref_div = 0;
    logic       clkref = 0;
    always @(posedge sdram_clk) begin
        clkref_div <= clkref_div + 1'b1;
        if (clkref_div == 4'd7) begin
            clkref     <= ~clkref;
            clkref_div <= 0;
        end
    end

    logic rst;

    // -----------------------------------------------------------------------
    // DUT debug/control ports
    // -----------------------------------------------------------------------
    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;
    logic        irq_n = 1;
    logic        nmi_n = 1;

    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] audio_l, audio_r;

    logic        dbg_peek_en = 0;
    logic [15:0] dbg_peek_addr = 0;
    wire  [7:0]  dbg_peek_data;
    logic        dbg_poke_en = 0;
    logic [15:0] dbg_poke_addr = 0;
    logic [7:0]  dbg_poke_data = 0;
    logic        dbg_pause;

    logic        dbg_rom_we = 0;
    logic        dbg_rom_idx = 0;
    logic [13:0] dbg_rom_addr = 0;
    logic [7:0]  dbg_rom_data = 0;
    logic        dbg_cpu_reset;
    logic        dbg_cpu_resume = 0;

    wire  [15:0] dbg_cpu_pc;
    wire  [7:0]  dbg_cpu_a, dbg_cpu_x, dbg_cpu_y, dbg_cpu_sp, dbg_cpu_flags;
    wire  [5:0]  dbg_cpu_state;
    wire  [7:0]  dbg_cpu_ir;
    wire  [15:0] dbg_cpu_addr;
    wire  [7:0]  dbg_cpu_din, dbg_cpu_dout;
    wire         dbg_cpu_we, dbg_cpu_rdy, dbg_cpu_irq, dbg_cpu_nmi;
    wire         dbg_cpu_waiting, dbg_cpu_stopped;

    // -----------------------------------------------------------------------
    // top.sv <-> sdram.v stream-port wires
    // -----------------------------------------------------------------------
    wire         stream_req;
    wire [24:0]  stream_addr;
    wire [13:0]  stream_words;
    wire         stream_ready;
    wire [15:0]  stream_dout;
    wire         stream_valid;
    wire         stream_busy;
    wire         stream_done;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
        .board_buttons(8'h00), .board_switches(8'h00),
        .usb_hid_status(8'h00), .usb_hid_device_type(8'h00),
        .usb_hid_last_scan(8'h00), .usb_hid_last_ascii(8'h00),
        .usb_hid_report_count(8'h00), .usb_hid_key_count(8'h00),
        .usb_hid_core_status(8'h00), .usb_hid_regs(64'h0),
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
        // SDRAM port A/B — XRAM/SID-curve paths unused here; the CPU is stalled
        // during the page-in, so port A stays quiescent (N1). Tie off.
        .sdram_clk(sdram_clk),
        .sdram_addrA(), .sdram_dinA(), .sdram_weA(), .sdram_oeA(),
        .sdram_doutA(8'h00), .sdram_doneA(1'b0),
        .sdram_addrB(), .sdram_dinB(), .sdram_weB(), .sdram_oeB(),
        .sdram_doutB(8'h00),
        // SDRAM page-mode STREAM port — wired to the real sdram.v below.
        .sdram_stream_req(stream_req),
        .sdram_stream_addr(stream_addr),
        .sdram_stream_words(stream_words),
        .sdram_stream_ready(stream_ready),
        .sdram_stream_dout(stream_dout),
        .sdram_stream_valid(stream_valid),
        .sdram_stream_busy(stream_busy),
        .sdram_stream_done(stream_done),
        .fio_event(), .nic_event()
    );

    // -----------------------------------------------------------------------
    // Real sdram.v stream consumer + sdram_model oracle (same wiring as
    // test_page_dma / test_sdram_stream). top.sv drives the stream port; we
    // tie off port A/B at the controller.
    // -----------------------------------------------------------------------
    wire [15:0] sd_data_ctrl_out;
    wire [15:0] sd_data_chip_out;
    wire [12:0] sd_addr;
    wire [1:0]  sd_dqm;
    wire [1:0]  sd_ba;
    wire        sd_cs, sd_we, sd_ras, sd_cas;
    wire        we_out;
    wire [15:0] sd_data_bus = we_out ? sd_data_ctrl_out : sd_data_chip_out;

    logic       sdram_init = 1'b1;

    sdram sdram_ctrl (
        .sd_data_in (sd_data_bus),
        .sd_data_out(sd_data_ctrl_out),
        .sd_addr    (sd_addr),
        .sd_dqm     (sd_dqm),
        .sd_ba      (sd_ba),
        .sd_cs      (sd_cs),
        .sd_we      (sd_we),
        .sd_ras     (sd_ras),
        .sd_cas     (sd_cas),

        .init   (sdram_init),
        .clk    (sdram_clk),
        .clkref (clkref),
        .we_out (we_out),

        // Port A/B unused — the page-in uses the stream port exclusively.
        .addrA(25'd0), .weA(1'b0), .dinA(8'd0), .oeA(1'b0), .doutA(), .doneA(),
        .addrB(25'd0), .weB(1'b0), .dinB(8'd0), .oeB(1'b0), .doutB(), .doneB(),

        .stream_req  (stream_req),
        .stream_addr (stream_addr),
        .stream_words(stream_words),
        .stream_ready(stream_ready),
        .stream_dout (stream_dout),
        .stream_valid(stream_valid),
        .stream_busy (stream_busy),
        .stream_done (stream_done)
    );

    sdram_model chip (
        .clk         (sdram_clk),
        .sd_data_out (sd_data_ctrl_out),
        .sd_data_in  (sd_data_chip_out),
        .sd_addr     (sd_addr),
        .sd_dqm      (sd_dqm),
        .sd_ba       (sd_ba),
        .sd_cs       (sd_cs),
        .sd_we       (sd_we),
        .sd_ras      (sd_ras),
        .sd_cas      (sd_cas)
    );

    // -----------------------------------------------------------------------
    // Accounting
    // -----------------------------------------------------------------------
    int pass_count = 0, fail_count = 0, test_num = 0;

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

    // -----------------------------------------------------------------------
    // ROM-load helper (pixel domain, back-to-back like NovaHost).
    // -----------------------------------------------------------------------
    task automatic rom_write(input logic idx, input logic [13:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_rom_idx  <= idx;
        dbg_rom_addr <= addr;
        dbg_rom_data <= data;
        dbg_rom_we   <= 1;
        @(posedge clk);
        dbg_rom_we   <= 0;
    endtask

    // -----------------------------------------------------------------------
    // 6502 driver split across two regions so the CPU never fetches from a ROM
    // bank while that bank is swapped out:
    //
    //   ROM part (basic_rom @ $C000): arm the page-in, fire PGD_CMD (the CPU
    //   stalls here for the whole copy), stash STATUS, then JMP $0200 into RAM.
    //
    //   RAM part ($0200, loaded via dbg_poke): ROMSWAP to ext_rom, read the
    //   first 4 resident bytes into $0300-$0303 (executing from RAM, which is
    //   never swapped), ROMSWAP back to BASIC, then halt-loop in RAM. This is
    //   the CPU-read proof that the paged-in image is live at $C000.
    //
    //   ; --- ROM @ $C000: arm + fire + jump to RAM ---
    //   $C000: A9 00       LDA #$00
    //   $C002: 8D 78 BA    STA $BA78      ; PGD_SRCL = 0
    //   $C005: 8D 79 BA    STA $BA79      ; PGD_SRCM = 0
    //   $C008: 8D 7A BA    STA $BA7A      ; PGD_SRCH = 0
    //   $C00B: 8D 7B BA    STA $BA7B      ; PGD_WORDSL = 0
    //   $C00E: A9 20       LDA #$20
    //   $C010: 8D 7C BA    STA $BA7C      ; PGD_WORDSH = 0x20 -> 0x2000 = 8192
    //   $C013: A9 01       LDA #$01
    //   $C015: 8D 76 BA    STA $BA76      ; PGD_CMD = 1 (start; CPU stalls here)
    //   $C018: AD 77 BA    LDA $BA77      ; read STATUS (after resume)
    //   $C01B: 8D 04 03    STA $0304      ; stash STATUS for the testbench
    //   $C01E: 4C 00 02    JMP $0200      ; into the RAM reader
    // -----------------------------------------------------------------------
    localparam int PROG_LEN = 33;
    byte unsigned prog [PROG_LEN] = '{
        8'hA9, 8'h00,
        8'h8D, 8'h78, 8'hBA,
        8'h8D, 8'h79, 8'hBA,
        8'h8D, 8'h7A, 8'hBA,
        8'h8D, 8'h7B, 8'hBA,
        8'hA9, 8'h20,
        8'h8D, 8'h7C, 8'hBA,
        8'hA9, 8'h01,
        8'h8D, 8'h76, 8'hBA,
        8'hAD, 8'h77, 8'hBA,
        8'h8D, 8'h04, 8'h03,
        8'h4C, 8'h00, 8'h02
    };

    // RAM reader @ $0200 (loaded via dbg_poke). Runs from RAM so the ROM-bank
    // swap below never pulls the executing code out from under the CPU.
    //
    //   $0200: A9 04       LDA #$04
    //   $0202: 8D 3F A0    STA $A03F      ; ROMSWAP_EXT  (ext_rom live at $C000)
    //   $0205: AD 00 C0    LDA $C000      ; ext_rom[0]
    //   $0208: 8D 00 03    STA $0300
    //   $020B: AD 01 C0    LDA $C001
    //   $020E: 8D 01 03    STA $0301
    //   $0211: AD 02 C0    LDA $C002
    //   $0214: 8D 02 03    STA $0302
    //   $0217: AD 03 C0    LDA $C003
    //   $021A: 8D 03 03    STA $0303
    //   $021D: A9 02       LDA #$02
    //   $021F: 8D 3F A0    STA $A03F      ; ROMSWAP_BASIC
    //   $0222: 4C 22 02    JMP $0222      ; halt
    localparam int RAM_PROG_LEN = 37;
    localparam logic [15:0] RAM_PROG_BASE = 16'h0200;
    byte unsigned ram_prog [RAM_PROG_LEN] = '{
        8'hA9, 8'h04,
        8'h8D, 8'h3F, 8'hA0,
        8'hAD, 8'h00, 8'hC0,
        8'h8D, 8'h00, 8'h03,
        8'hAD, 8'h01, 8'hC0,
        8'h8D, 8'h01, 8'h03,
        8'hAD, 8'h02, 8'hC0,
        8'h8D, 8'h02, 8'h03,
        8'hAD, 8'h03, 8'hC0,
        8'h8D, 8'h03, 8'h03,
        8'hA9, 8'h02,
        8'h8D, 8'h3F, 8'hA0,
        8'h4C, 8'h22, 8'h02
    };

    // RAM-load helper (debug poke path writes main RAM).
    task automatic ram_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en   <= 1;
        @(posedge clk);
        dbg_poke_en   <= 0;
    endtask

    // -----------------------------------------------------------------------
    // Preload the unique 16 KB pattern into SDRAM (model backdoor — the same
    // port-B-style preload path test_page_dma uses). word(i) = {i[7:0], i[15:8]}.
    // -----------------------------------------------------------------------
    localparam int N_WORDS = 8192;
    task automatic preload_unique_pattern(input int n_words);
        int i;
        for (i = 0; i < n_words; i++)
            chip.poke_word(25'(2*i), {i[7:0], i[15:8]});
    endtask

    // -----------------------------------------------------------------------
    // Observe pgd_rdy (the CPU stall) and the page-in active window.
    // -----------------------------------------------------------------------
    logic saw_pgd_stall = 0;       // pgd_rdy went low at some point
    logic saw_pgd_active = 0;      // sdram-side copy ran
    always @(posedge clk)
        if (!dut.pgd_rdy) saw_pgd_stall <= 1'b1;
    always @(posedge sdram_clk)
        if (dut.pgd_active) saw_pgd_active <= 1'b1;

    // ext_rom byte-exact verify against the unique pattern.
    task automatic verify_ext_rom(input string name, input int n_words);
        int k, mism;
        logic [7:0] exp_even, exp_odd;
        mism = 0;
        for (k = 0; k < n_words; k++) begin
            exp_even = k[7:0];
            exp_odd  = k[15:8];
            if (dut.ext_rom_inst.mem[14'(2*k)]   !== exp_even ||
                dut.ext_rom_inst.mem[14'(2*k+1)] !== exp_odd) begin
                if (mism < 6)
                    $display("  word %0d: ext_rom[%0d]=%02h (exp %02h) ext_rom[%0d]=%02h (exp %02h)",
                             k, 2*k, dut.ext_rom_inst.mem[14'(2*k)], exp_even,
                             2*k+1, dut.ext_rom_inst.mem[14'(2*k+1)], exp_odd);
                mism++;
            end
        end
        if (mism != 0) $display("  %s: %0d mismatches", name, mism);
        check(name, mism == 0);
    endtask

    initial begin
        $display("=== Task 11d: CPU MMIO -> page-in -> 16 KB resident at $C000 ===");
        $display("");

        // Hold everything in reset; load ROM during the CPU-held gap.
        rst           = 1;
        dbg_pause     = 1;
        dbg_cpu_reset = 1;
        sdram_init    = 1;

        repeat (20) @(posedge clk);
        rst = 0;
        repeat (10) @(posedge clk);

        // Bring the sdram.v controller out of its init sequence (sdram domain).
        repeat (10) @(posedge sdram_clk);
        sdram_init = 0;
        repeat (2000) @(posedge sdram_clk);
        check("sdram controller exited reset", sdram_ctrl.reset == 0);

        // Preload the 16 KB unique pattern into SDRAM.
        $display("Preloading %0d-word unique pattern into SDRAM...", N_WORDS);
        preload_unique_pattern(N_WORDS);

        // Load the ROM driver into basic_rom @ $C000 and the reset vector.
        $display("Loading %0d-byte ROM driver into basic_rom...", PROG_LEN);
        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);   // reset vector low  = $00
        rom_write(1'b0, 14'h3FFD, 8'hC0);   // reset vector high = $C0
        repeat (8) @(posedge clk);

        // Load the RAM reader @ $0200 (executes from RAM, survives ROM swaps).
        $display("Loading %0d-byte RAM reader @ $%04X...", RAM_PROG_LEN, RAM_PROG_BASE);
        for (int i = 0; i < RAM_PROG_LEN; i++)
            ram_poke(RAM_PROG_BASE + 16'(i), ram_prog[i]);
        repeat (4) @(posedge clk);

        check_eq8("basic_rom[0] loaded", dut.basic_rom_inst.mem[14'h0000], 8'hA9);
        check_eq8("reset vector hi", dut.basic_rom_inst.mem[14'h3FFD], 8'hC0);
        check_eq8("ram_prog[0] loaded", dut.main_ram.mem[RAM_PROG_BASE], 8'hA9);

        // Release the CPU. It runs the driver, which triggers the page-in.
        dbg_cpu_reset = 0;
        repeat (4) @(posedge clk);
        dbg_pause = 0;

        // Run long enough for: program prologue + the 16 KB page-in (the CPU is
        // stalled the whole copy) + ROMSWAP + 4 byte copies + halt. The copy is
        // ~2 sdram_clk/word (~18k sdram cycles ~= 5-6k pixel cycles at this
        // ratio); the CPU runs at half pixel rate. 200k pixel cycles is far
        // more than enough and keeps the watchdog (#200ms) comfortably clear.
        repeat (200000) @(posedge clk);

        // -----------------------------------------------------------------
        // Assertions
        // -----------------------------------------------------------------
        $display("");
        $display("Final CPU PC = 0x%04X (expect RAM halt loop $0222)", dbg_cpu_pc);
        check("CPU reached RAM halt loop", (dbg_cpu_pc >= 16'h0222) && (dbg_cpu_pc <= 16'h0225));

        // The CPU was stalled (pgd_rdy low) and the sdram-side copy ran.
        check("CPU was stalled during page-in (pgd_rdy went low)", saw_pgd_stall);
        check("page_dma copy ran (pgd_active went high)",          saw_pgd_active);

        // STATUS read after the stall must show ok=1 (bit1), busy=0 (bit0).
        check_eq8("STATUS after page-in = ok (bit1 set, busy clear)",
                  dut.main_ram.mem[16'h0304], 8'h02);

        // ext_rom resident image — byte-exact + correct byte order, all 16 KB.
        $display("Verifying 16 KB ext_rom resident image...");
        verify_ext_rom("16 KB resident at $C000 byte-exact + order", N_WORDS);

        // CPU-read proof: the program copied ext_rom[0..3] into $0300-$0303
        // through the real CPU read path (ROMSWAP_EXT). Pattern: word0 =
        // {0,0} -> bytes 0,1 = 0,0; word1 = {1,0} -> bytes 2,3 = 1,0.
        check_eq8("CPU read $C000 -> $0300", dut.main_ram.mem[16'h0300], 8'h00);
        check_eq8("CPU read $C001 -> $0301", dut.main_ram.mem[16'h0301], 8'h00);
        check_eq8("CPU read $C002 -> $0302", dut.main_ram.mem[16'h0302], 8'h01);
        check_eq8("CPU read $C003 -> $0303", dut.main_ram.mem[16'h0303], 8'h00);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish;
    end

    // Global watchdog.
    initial begin
        #200_000_000;
        $display("WATCHDOG TIMEOUT");
        $display("=== Results: %0d passed, %0d failed (TIMEOUT) ===", pass_count, fail_count + 1);
        $display("SOME TESTS FAILED");
        $finish;
    end

endmodule
