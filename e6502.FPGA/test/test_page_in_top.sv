// test_page_in_top.sv — Task 11d/T8 paged-library loader capstone.
//
// Proves the FULL paged-library flow end-to-end through the REAL top.sv
// (-DSYNTHESIS) running the REAL `libcall.bin` loader and a REAL formatted
// module (`testmod.bin`) — not a synthetic pattern, not a hand-rolled driver:
//
//   The CPU resets into a tiny basic_rom trampoline at $C000 that JSRs the real
//   resident loader `lib_call` (tests/asm/libcall.bin, ORG $9C00, loaded into
//   main RAM). lib_call reads the mailbox ($0300..), does modtab_lookup
//   (MODULE_ID_TEST $7F -> SHELF_BASE $060000 + 8192 words), fires the page-in
//   MMIO ($BA76), and STALLS (pgd_rdy low) while the sdram_clk-domain page_dma
//   page-mode-streams the 16 KB module image from SDRAM into bank-1 ext_rom.
//   It then SEI-swaps ROMSWAP_EXTENSION ($04) to validate the "NL" header,
//   caches LIB_RESIDENT, JSRs the module at $C000 (which dispatches FN 0 ECHO:
//   RESULT = ARG0), restores ROMSWAP_BASIC ($02), and RTSes. Back in the
//   trampoline the CPU stashes LIB_RESULT ($0313) to $0400 and halts.
//
// HARNESS: top.sv (DUT) + a REAL sdram.v stream consumer + the sdram_model
//   timing oracle. top.sv's exposed stream port wires to sdram.v's stream port;
//   sdram.v's port A/B are tied off here (the CPU is stalled during the page-in
//   so XRAM port A is quiescent — N1). A SEPARATE faster sdram_clk genuinely
//   exercises the pixel<->sdram CDC (unlike the same-clock top tests).
//
// MODULE STAGING (byte-exact at the loader's shelf base $060000):
//   page_dma decodes even byte = stream_dout[15:8], odd byte = stream_dout[7:0]
//   (a[0]?lo:hi). The sdram_model poke_word(byte,{hi,lo}) puts hi at the even
//   byte. So staging word k = {testmod[2k], testmod[2k+1]} at byte $060000+2k
//   yields ext_rom[2k]==testmod[2k], ext_rom[2k+1]==testmod[2k+1] after page-in.
//
// VERIFY:
//   - pgd_rdy went low (CPU stalled INSIDE lib_call) + pgd_active went high.
//   - ext_rom_inst.mem[] holds the 16 KB testmod.bin image byte-exact.
//   - LIB_RESIDENT ($0318) == $7F (loader cached the module id).
//   - LIB_STATUS  ($0302) == $00 (ECHO returned LERR_OK).
//   - The stashed RESULT at $0400 == $DEADBEEF — the module ECHO ran on the
//     paged-in image and the real ROMSWAP_EXT/BASIC dance worked end-to-end.
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
    // basic_rom @ $C000 trampoline. The reset vector points here. It does the
    // minimum: JSR the real resident loader (lib_call @ $9C00 in RAM), then
    // stash the 32-bit LIB_RESULT ($0313) to a scratch RAM address ($0400), then
    // halt. ALL the page-in + ROMSWAP + module-dispatch logic lives in the real
    // libcall.bin running from RAM, so this trampoline never has the ground
    // pulled out from under it by a bank swap.
    //
    //   $C000: 20 00 9C    JSR $9C00      ; lib_call (page-in + validate + dispatch)
    //   $C003: AD 13 03    LDA $0313      ; LIB_RESULT byte0
    //   $C006: 8D 00 04    STA $0400
    //   $C009: AD 14 03    LDA $0314      ; LIB_RESULT byte1
    //   $C00C: 8D 01 04    STA $0401
    //   $C00F: AD 15 03    LDA $0315      ; LIB_RESULT byte2
    //   $C012: 8D 02 04    STA $0402
    //   $C015: AD 16 03    LDA $0316      ; LIB_RESULT byte3
    //   $C018: 8D 03 04    STA $0403
    //   $C01B: 4C 1B C0    JMP $C01B      ; halt
    // -----------------------------------------------------------------------
    localparam int PROG_LEN = 30;
    byte unsigned prog [PROG_LEN] = '{
        8'h20, 8'h00, 8'h9C,
        8'hAD, 8'h13, 8'h03,
        8'h8D, 8'h00, 8'h04,
        8'hAD, 8'h14, 8'h03,
        8'h8D, 8'h01, 8'h04,
        8'hAD, 8'h15, 8'h03,
        8'h8D, 8'h02, 8'h04,
        8'hAD, 8'h16, 8'h03,
        8'h8D, 8'h03, 8'h04,
        8'h4C, 8'h1B, 8'hC0
    };

    // The real resident loader, loaded into main RAM at its ORG.
    localparam logic [15:0] LIBCALL_BASE = 16'h9C00;   // libcall.bin ORG
    localparam int          LIBCALL_MAX  = 512;        // bin is 151 bytes
    byte unsigned libcall_img [LIBCALL_MAX];
    int           libcall_len;

    // The real formatted module image (16 KB), staged into SDRAM at the shelf.
    localparam int          TESTMOD_LEN  = 16384;
    byte unsigned testmod_img [TESTMOD_LEN];
    int           testmod_read;

    // Loader's shelf base (libabi.inc SHELF_BASE = $060000). modtab_lookup in
    // libcall.bin programs PGD_SRC* to exactly this byte base for MODULE_ID_TEST.
    localparam logic [24:0] SHELF_BASE   = 25'h060000;

    // Distinctive 32-bit ECHO argument. ECHO copies ARG0 -> RESULT, so the
    // stashed RESULT at $0400 must equal this after the whole flow.
    localparam logic [31:0] ARG0_VALUE   = 32'hDEADBEEF;

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
    // Stage the 16 KB testmod.bin image into SDRAM at the loader's shelf base
    // (model backdoor — the same path test_page_dma uses). For word k:
    //   word = {testmod[2k], testmod[2k+1]} -> hi lane = byte 2k, lo lane = 2k+1
    // page_dma writes ext_rom[2k]=hi, ext_rom[2k+1]=lo, so the resident image is
    // byte-exact (no swap). Staged at byte base $060000 = SHELF_BASE so the real
    // modtab_lookup's PGD_SRC* program lands on it.
    // -----------------------------------------------------------------------
    localparam int N_WORDS = 8192;
    task automatic stage_module_at_shelf(input int n_words);
        int k;
        logic [7:0] hi, lo;
        for (k = 0; k < n_words; k++) begin
            hi = testmod_img[2*k];
            lo = testmod_img[2*k+1];
            chip.poke_word(SHELF_BASE + 25'(2*k), {hi, lo});
        end
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

    // ext_rom byte-exact verify against the staged testmod.bin image.
    task automatic verify_ext_rom(input string name, input int n_words);
        int k, mism;
        logic [7:0] exp_even, exp_odd;
        mism = 0;
        for (k = 0; k < n_words; k++) begin
            exp_even = testmod_img[2*k];
            exp_odd  = testmod_img[2*k+1];
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

    // File descriptors for the bin loads (run dir is e6502.FPGA/ — the test
    // Makefile cd's to the parent — so the path is ../tests/asm/).
    int fd_lc, fd_tm;

    initial begin
        $display("=== T8: real libcall.bin pages in real testmod.bin, CPU dispatches ECHO ===");
        $display("");

        // Load the real loader + module images from disk via $fopen/$fread
        // (same idiom as test_cpu_dormann). $fread of a packed-byte array fills
        // from index 0 and returns the byte count.
        fd_lc = $fopen("../tests/asm/libcall.bin", "rb");
        if (fd_lc == 0) $fatal(1, "Could not open ../tests/asm/libcall.bin");
        libcall_len = $fread(libcall_img, fd_lc);
        $fclose(fd_lc);

        fd_tm = $fopen("../tests/asm/testmod.bin", "rb");
        if (fd_tm == 0) $fatal(1, "Could not open ../tests/asm/testmod.bin");
        testmod_read = $fread(testmod_img, fd_tm);
        $fclose(fd_tm);

        $display("Loaded libcall.bin = %0d bytes, testmod.bin = %0d bytes",
                 libcall_len, testmod_read);

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

        check("libcall.bin loaded (151 bytes)", libcall_len == 151);
        check("testmod.bin loaded (16384 bytes)", testmod_read == TESTMOD_LEN);
        check_eq8("testmod header = JMP ($4C)", testmod_img[0], 8'h4C);
        check_eq8("testmod magic 'N' @ +3",     testmod_img[3], 8'h4E);
        check_eq8("testmod magic 'L' @ +4",     testmod_img[4], 8'h4C);
        check_eq8("testmod id = $7F @ +5",       testmod_img[5], 8'h7F);

        // Stage the real module image into SDRAM at the loader's shelf base.
        $display("Staging %0d-byte testmod.bin into SDRAM at shelf $%06X...",
                 TESTMOD_LEN, SHELF_BASE);
        stage_module_at_shelf(N_WORDS);

        // Load the $C000 trampoline into basic_rom + the reset vector.
        $display("Loading %0d-byte $C000 trampoline into basic_rom...", PROG_LEN);
        for (int i = 0; i < PROG_LEN; i++)
            rom_write(1'b0, 14'(i), prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);   // reset vector low  = $00
        rom_write(1'b0, 14'h3FFD, 8'hC0);   // reset vector high = $C0
        repeat (8) @(posedge clk);

        // Load the real libcall.bin into main RAM at its ORG ($9C00).
        $display("Loading libcall.bin into RAM @ $%04X...", LIBCALL_BASE);
        for (int i = 0; i < libcall_len; i++)
            ram_poke(LIBCALL_BASE + 16'(i), libcall_img[i]);
        repeat (4) @(posedge clk);

        // Set up the mailbox in low RAM before releasing the CPU.
        //   $0300 LIB_MOD_ID    = $7F  (MODULE_ID_TEST)
        //   $0301 LIB_FN_ID     = $00  (FN 0 ECHO)
        //   $0302 LIB_STATUS    = $FF  (sentinel; ECHO must overwrite with $00)
        //   $0303 LIB_ARG0      = $DEADBEEF (LE)
        //   $0313 LIB_RESULT    = $00000000 (cleared; ECHO must fill it)
        //   $0317 LIB_HOME_BANK = $02  (ROMSWAP_BASIC — caller's home bank)
        //   $0318 LIB_RESIDENT  = $00  (none resident yet)
        ram_poke(16'h0300, 8'h7F);
        ram_poke(16'h0301, 8'h00);
        ram_poke(16'h0302, 8'hFF);
        ram_poke(16'h0303, ARG0_VALUE[7:0]);
        ram_poke(16'h0304, ARG0_VALUE[15:8]);
        ram_poke(16'h0305, ARG0_VALUE[23:16]);
        ram_poke(16'h0306, ARG0_VALUE[31:24]);
        ram_poke(16'h0313, 8'h00);
        ram_poke(16'h0314, 8'h00);
        ram_poke(16'h0315, 8'h00);
        ram_poke(16'h0316, 8'h00);
        ram_poke(16'h0317, 8'h02);
        ram_poke(16'h0318, 8'h00);
        repeat (4) @(posedge clk);

        check_eq8("basic_rom[0] = JSR ($20)", dut.basic_rom_inst.mem[14'h0000], 8'h20);
        check_eq8("reset vector hi", dut.basic_rom_inst.mem[14'h3FFD], 8'hC0);
        check_eq8("libcall[0] loaded (LDA $0300 = $AD)",
                  dut.main_ram.mem[LIBCALL_BASE], 8'hAD);
        check_eq8("mailbox MOD_ID = $7F", dut.main_ram.mem[16'h0300], 8'h7F);
        check_eq8("mailbox ARG0 lo = $EF", dut.main_ram.mem[16'h0303], 8'hEF);

        // Release the CPU. It resets to $C000, JSRs lib_call, which triggers the
        // page-in, validates, dispatches ECHO, restores the bank, and returns.
        dbg_cpu_reset = 0;
        repeat (4) @(posedge clk);
        dbg_pause = 0;

        // Run long enough for: reset prologue + JSR lib_call + the 16 KB page-in
        // (the CPU stalls the whole copy) + validate + ROMSWAP dance + module
        // dispatch + RESULT stash + halt. The copy is ~2 sdram_clk/word (~18k
        // sdram cycles ~= 5-6k pixel cycles at this ratio); the CPU runs at half
        // pixel rate. 300k pixel cycles is far more than enough and keeps the
        // watchdog (#200ms) comfortably clear.
        repeat (300000) @(posedge clk);

        // -----------------------------------------------------------------
        // Assertions
        // -----------------------------------------------------------------
        $display("");
        $display("Final CPU PC = 0x%04X (expect $C000 halt loop $C01B)", dbg_cpu_pc);
        check("CPU reached $C01B halt loop", (dbg_cpu_pc >= 16'hC01B) && (dbg_cpu_pc <= 16'hC01E));

        // The page-in happened INSIDE lib_call: CPU stalled (pgd_rdy low) and the
        // sdram-side page_dma copy ran (pgd_active high).
        check("CPU stalled during page-in inside lib_call (pgd_rdy went low)", saw_pgd_stall);
        check("page_dma copy ran (pgd_active went high)",                      saw_pgd_active);

        // ext_rom resident image — byte-exact vs testmod.bin, all 16 KB.
        $display("Verifying 16 KB ext_rom resident image vs testmod.bin...");
        verify_ext_rom("16 KB ext_rom == testmod.bin byte-exact", N_WORDS);

        // Loader cached the module id and ECHO returned LERR_OK.
        check_eq8("LIB_RESIDENT ($0318) = $7F (loader cached module)",
                  dut.main_ram.mem[16'h0318], 8'h7F);
        check_eq8("LIB_STATUS ($0302) = $00 (ECHO LERR_OK)",
                  dut.main_ram.mem[16'h0302], 8'h00);

        // END-TO-END PROOF: the module ECHO ran on the paged-in image and the
        // real SEI/ROMSWAP_EXT->JSR $C000->ROMSWAP_BASIC dance worked. The
        // stashed RESULT at $0400 must equal the 32-bit ARG0 ($DEADBEEF).
        $display("Stashed RESULT @ $0400 = 0x%02X%02X%02X%02X (expect DEADBEEF)",
                 dut.main_ram.mem[16'h0403], dut.main_ram.mem[16'h0402],
                 dut.main_ram.mem[16'h0401], dut.main_ram.mem[16'h0400]);
        check_eq8("RESULT byte0 @ $0400 = $EF", dut.main_ram.mem[16'h0400], ARG0_VALUE[7:0]);
        check_eq8("RESULT byte1 @ $0401 = $BE", dut.main_ram.mem[16'h0401], ARG0_VALUE[15:8]);
        check_eq8("RESULT byte2 @ $0402 = $AD", dut.main_ram.mem[16'h0402], ARG0_VALUE[23:16]);
        check_eq8("RESULT byte3 @ $0403 = $DE", dut.main_ram.mem[16'h0403], ARG0_VALUE[31:24]);

        // ROMSWAP restored: the CPU is fetching the halt loop from basic_rom at
        // $C01B, which is only possible if lib_call swapped the bank back to
        // BASIC after dispatch (ext_rom_active cleared). Observe the bank flag.
        check("ROMSWAP restored to primary bank (ext_rom_active low)",
              dut.ext_rom_active == 1'b0);

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
