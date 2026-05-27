// Top-level SID audio regression.
//
// Runs real 6502 code through the synthesized top.sv path and verifies that
// CPU writes to the SID MMIO range update the SID chip and produce non-zero
// signed audio samples. This covers the path the BASIC POKE program uses on
// hardware: CPU bus -> SID registers -> SID mixer output.

`timescale 1ns/1ps

module test_sid_cpu_audio_top;

    logic clk = 0;
    always #20 clk = ~clk;

    logic rst;
    logic        key_valid = 0;
    logic [7:0]  key_data  = 0;
    logic        irq_n = 1, nmi_n = 1;

    wire  [3:0]  vid_r, vid_g, vid_b;
    wire         vid_hsync, vid_vsync, vid_de;
    wire signed [17:0] audio_l, audio_r;
    logic       clk_audio = 1'b0;
    logic [1:0][15:0] hdmi_audio_sample_word;

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

    wire fio_event;

    always #11240 clk_audio = ~clk_audio;

    top dut (
        .clk(clk), .rst(rst),
        .key_valid(key_valid), .key_data(key_data), .key_ready(),
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
        .fio_event(fio_event),
        .nic_event()
    );

    sid_hdmi_audio sid_hdmi_audio_inst (
        .clk              (clk_audio),
        .rst              (rst),
        .sample_en        (1'b1),
        .sid_audio_l      (audio_l),
        .sid_audio_r      (audio_r),
        .audio_sample_word(hdmi_audio_sample_word)
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

    task automatic check_eq16(input string name, input logic [15:0] actual,
                              input logic [15:0] expected);
        test_num++;
        if (actual === expected) begin
            $display("  PASS [%0d] %s (=0x%04X)", test_num, name, actual);
            pass_count++;
        end else begin
            $display("  FAIL [%0d] %s (got 0x%04X, want 0x%04X)",
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

    task automatic debug_poke(input logic [15:0] addr, input logic [7:0] data);
        @(posedge clk);
        dbg_poke_addr <= addr;
        dbg_poke_data <= data;
        dbg_poke_en   <= 1;
        @(posedge clk);
        dbg_poke_en   <= 0;
    endtask

    task automatic wait_vgc_ready();
        while (dut.vgc_rdy !== 1'b1) @(posedge clk);
        repeat(4) @(posedge clk);
    endtask

    function automatic int abs18(input logic signed [17:0] v);
        abs18 = (v < 0) ? -v : v;
    endfunction

    function automatic int pcm16(input logic [15:0] v);
        logic signed [15:0] s;
        begin
            s = v;
            pcm16 = s;
        end
    endfunction

    task automatic count_sid_ticks(input int cycles, output int ticks);
        ticks = 0;
        repeat(cycles) begin
            @(posedge clk);
            if (dut.sid_ce_1m)
                ticks++;
        end
    endtask

    task automatic rom_emit(inout int pc, input logic [7:0] data);
        rom_write(1'b0, 14'(pc), data);
        pc++;
    endtask

    task automatic emit_lda_sta(inout int pc, input logic [7:0] data,
                                input logic [15:0] addr);
        rom_emit(pc, 8'hA9);
        rom_emit(pc, data);
        rom_emit(pc, 8'h8D);
        rom_emit(pc, addr[7:0]);
        rom_emit(pc, addr[15:8]);
    endtask

    task automatic emit_jmp_self(inout int pc, input logic [15:0] base);
        logic [15:0] target;
        target = base + pc[15:0];
        rom_emit(pc, 8'h4C);
        rom_emit(pc, target[7:0]);
        rom_emit(pc, target[15:8]);
    endtask

    // Program at $C000:
    //   configure all three SID1 voices at $D400
    //   configure all three SID2 voices at $D420
    //   write SID2 voice 1 frequency high through its legacy $D500 mirror
    //   gate every oscillator on with distinct waveforms
    //   halt in a tight loop
    task automatic load_sid_register_program(output int program_len);
        int pc;
        pc = 0;

        // SID1 voice 1
        emit_lda_sta(pc, 8'h0F, 16'hD418);
        emit_lda_sta(pc, 8'h00, 16'hD405);
        emit_lda_sta(pc, 8'hF0, 16'hD406);
        emit_lda_sta(pc, 8'h00, 16'hD400);
        emit_lda_sta(pc, 8'h10, 16'hD401);
        emit_lda_sta(pc, 8'h21, 16'hD404);

        // SID1 voice 2
        emit_lda_sta(pc, 8'h34, 16'hD407);
        emit_lda_sta(pc, 8'h12, 16'hD408);
        emit_lda_sta(pc, 8'h23, 16'hD40C);
        emit_lda_sta(pc, 8'hE2, 16'hD40D);
        emit_lda_sta(pc, 8'h41, 16'hD40B);

        // SID1 voice 3
        emit_lda_sta(pc, 8'h78, 16'hD40E);
        emit_lda_sta(pc, 8'h56, 16'hD40F);
        emit_lda_sta(pc, 8'h45, 16'hD413);
        emit_lda_sta(pc, 8'hD3, 16'hD414);
        emit_lda_sta(pc, 8'h81, 16'hD412);

        // SID2 voice 1, with high byte overwritten through the legacy mirror.
        emit_lda_sta(pc, 8'h0F, 16'hD438);
        emit_lda_sta(pc, 8'h00, 16'hD425);
        emit_lda_sta(pc, 8'hF0, 16'hD426);
        emit_lda_sta(pc, 8'h00, 16'hD420);
        emit_lda_sta(pc, 8'h18, 16'hD421);
        emit_lda_sta(pc, 8'h20, 16'hD501);
        emit_lda_sta(pc, 8'h21, 16'hD424);

        // SID2 voice 2
        emit_lda_sta(pc, 8'hCD, 16'hD427);
        emit_lda_sta(pc, 8'hAB, 16'hD428);
        emit_lda_sta(pc, 8'h67, 16'hD42C);
        emit_lda_sta(pc, 8'hC4, 16'hD42D);
        emit_lda_sta(pc, 8'h11, 16'hD42B);

        // SID2 voice 3
        emit_lda_sta(pc, 8'hEF, 16'hD42E);
        emit_lda_sta(pc, 8'hBE, 16'hD42F);
        emit_lda_sta(pc, 8'h89, 16'hD433);
        emit_lda_sta(pc, 8'hB5, 16'hD434);
        emit_lda_sta(pc, 8'h21, 16'hD432);

        emit_jmp_self(pc, 16'hC000);
        program_len = pc;
    endtask

    localparam int TONE_PROG_LEN = 8;
    byte unsigned tone_prog [TONE_PROG_LEN] = '{
        8'hA9, 8'h80, 8'h8D, 8'h40, 8'hD4,
        8'h4C, 8'h05, 8'hC0
    };

    initial begin
        int max_abs_l;
        int max_abs_r;
        int min_l;
        int max_l;
        int min_r;
        int max_r;
        int min_pcm_l;
        int max_pcm_l;
        int min_pcm_r;
        int max_pcm_r;
        int cur_pcm_l;
        int cur_pcm_r;
        int pal_ticks;
        int ntsc_ticks;
        int program_len;

        $display("=== SID CPU-to-audio top-level regression ===");

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

        debug_poke(16'hD440, 8'h00);
        repeat(4) @(posedge clk);
        count_sid_ticks(27000, pal_ticks);
        $display("SID PAL ticks per 27000 pixel clocks: %0d", pal_ticks);
        check("SID PAL clock produces 985248 Hz cadence",
              pal_ticks >= 984 && pal_ticks <= 986);
        check("SID clock defaults to PAL", !dut.sid_clock_ntsc);

        debug_poke(16'hD440, 8'h02);
        repeat(4) @(posedge clk);
        count_sid_ticks(27000, ntsc_ticks);
        $display("SID NTSC ticks per 27000 pixel clocks: %0d", ntsc_ticks);
        check("SID NTSC clock produces 1022727 Hz cadence",
              ntsc_ticks >= 1022 && ntsc_ticks <= 1024);
        check("SID clock config selects NTSC", dut.sid_clock_ntsc);

        debug_poke(16'hD440, 8'h00);
        repeat(4) @(posedge clk);

        load_sid_register_program(program_len);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);

        repeat(4) @(posedge clk);
        wait_vgc_ready();

        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;

        repeat(100000) @(posedge clk);

        $display("Final CPU PC = 0x%04X IR=0x%02X state=%0d addr=0x%04X dout=0x%02X",
                 dbg_cpu_pc, dbg_cpu_ir, dbg_cpu_state, dbg_cpu_addr, dbg_cpu_dout);

        check("CPU reached halt loop",
              (dbg_cpu_pc >= 16'hC000 + program_len - 3) &&
              (dbg_cpu_pc <= 16'hC000 + program_len));

        check_eq16("SID voice 1 frequency", dut.sid_inst.voice_freq[0], 16'h1000);
        check_eq8("SID voice 1 AD", dut.sid_inst.voice_ad[0], 8'h00);
        check_eq8("SID voice 1 SR", dut.sid_inst.voice_sr[0], 8'hF0);
        check_eq8("SID voice 1 control", dut.sid_inst.voice_ctrl[0], 8'h21);
        check_eq16("SID voice 2 frequency", dut.sid_inst.voice_freq[1], 16'h1234);
        check_eq8("SID voice 2 AD", dut.sid_inst.voice_ad[1], 8'h23);
        check_eq8("SID voice 2 SR", dut.sid_inst.voice_sr[1], 8'hE2);
        check_eq8("SID voice 2 control", dut.sid_inst.voice_ctrl[1], 8'h41);
        check_eq16("SID voice 3 frequency", dut.sid_inst.voice_freq[2], 16'h5678);
        check_eq8("SID voice 3 AD", dut.sid_inst.voice_ad[2], 8'h45);
        check_eq8("SID voice 3 SR", dut.sid_inst.voice_sr[2], 8'hD3);
        check_eq8("SID voice 3 control", dut.sid_inst.voice_ctrl[2], 8'h81);
        check_eq8("SID master volume", dut.sid_inst.filter_mode_vol, 8'h0F);
        check_eq16("SID2 voice 1 frequency via mirror", dut.sid2_inst.voice_freq[0], 16'h2000);
        check_eq8("SID2 voice 1 AD", dut.sid2_inst.voice_ad[0], 8'h00);
        check_eq8("SID2 voice 1 SR", dut.sid2_inst.voice_sr[0], 8'hF0);
        check_eq8("SID2 voice 1 control", dut.sid2_inst.voice_ctrl[0], 8'h21);
        check_eq16("SID2 voice 2 frequency", dut.sid2_inst.voice_freq[1], 16'hABCD);
        check_eq8("SID2 voice 2 AD", dut.sid2_inst.voice_ad[1], 8'h67);
        check_eq8("SID2 voice 2 SR", dut.sid2_inst.voice_sr[1], 8'hC4);
        check_eq8("SID2 voice 2 control", dut.sid2_inst.voice_ctrl[1], 8'h11);
        check_eq16("SID2 voice 3 frequency", dut.sid2_inst.voice_freq[2], 16'hBEEF);
        check_eq8("SID2 voice 3 AD", dut.sid2_inst.voice_ad[2], 8'h89);
        check_eq8("SID2 voice 3 SR", dut.sid2_inst.voice_sr[2], 8'hB5);
        check_eq8("SID2 voice 3 control", dut.sid2_inst.voice_ctrl[2], 8'h21);
        check_eq8("SID2 master volume", dut.sid2_inst.filter_mode_vol, 8'h0F);

        max_abs_l = 0;
        max_abs_r = 0;
        min_l = 262143;
        max_l = -262144;
        min_r = 262143;
        max_r = -262144;
        repeat(200000) begin
            @(posedge clk);
            if (abs18(audio_l) > max_abs_l)
                max_abs_l = abs18(audio_l);
            if (abs18(audio_r) > max_abs_r)
                max_abs_r = abs18(audio_r);
            if (audio_l < min_l)
                min_l = audio_l;
            if (audio_l > max_l)
                max_l = audio_l;
            if (audio_r < min_r)
                min_r = audio_r;
            if (audio_r > max_r)
                max_r = audio_r;
        end

        $display("Audio peaks: L=%0d R=%0d ranges: L=%0d..%0d R=%0d..%0d",
                 max_abs_l, max_abs_r, min_l, max_l, min_r, max_r);
        check("left audio becomes non-zero", max_abs_l > 0);
        check("right audio becomes non-zero", max_abs_r > 0);
        check("left audio varies over time", max_l > min_l);
        check("right audio varies over time", max_r > min_r);
        check("mixed SID audio routes equally to left/right",
              max_abs_l == max_abs_r && min_l == min_r && max_l == max_r);

        min_pcm_l = 32767;
        max_pcm_l = -32768;
        min_pcm_r = 32767;
        max_pcm_r = -32768;
        repeat(1024) begin
            @(posedge clk_audio);
            cur_pcm_l = pcm16(hdmi_audio_sample_word[0]);
            cur_pcm_r = pcm16(hdmi_audio_sample_word[1]);
            if (cur_pcm_l < min_pcm_l)
                min_pcm_l = cur_pcm_l;
            if (cur_pcm_l > max_pcm_l)
                max_pcm_l = cur_pcm_l;
            if (cur_pcm_r < min_pcm_r)
                min_pcm_r = cur_pcm_r;
            if (cur_pcm_r > max_pcm_r)
                max_pcm_r = cur_pcm_r;
        end

        $display("Conditioned HDMI PCM ranges: L=%0d..%0d R=%0d..%0d",
                 min_pcm_l, max_pcm_l, min_pcm_r, max_pcm_r);
        check("left conditioned PCM has audible movement", max_pcm_l - min_pcm_l > 4000);
        check("right conditioned PCM has audible movement", max_pcm_r - min_pcm_r > 4000);

        dbg_pause = 1;
        dbg_cpu_reset = 1;
        repeat(16) @(posedge clk);
        for (int i = 0; i < TONE_PROG_LEN; i++)
            rom_write(1'b0, 14'(i), tone_prog[i]);
        rom_write(1'b0, 14'h3FFC, 8'h00);
        rom_write(1'b0, 14'h3FFD, 8'hC0);

        repeat(4) @(posedge clk);
        dbg_cpu_reset = 0;
        repeat(4) @(posedge clk);
        dbg_pause = 0;
        repeat(200000) @(posedge clk);

        min_l = 262143;
        max_l = -262144;
        repeat(140000) begin
            @(posedge clk);
            if (audio_l < min_l)
                min_l = audio_l;
            if (audio_l > max_l)
                max_l = audio_l;
        end
        $display("HDMI diagnostic tone range: L=%0d..%0d", min_l, max_l);
        check("HDMI audio test tone register was enabled", dut.hdmi_audio_test_enable);
        check("HDMI audio test tone drives negative samples", min_l < -60000);
        check("HDMI audio test tone drives positive samples", max_l > 60000);

        dbg_pause = 1;
        debug_poke(16'hD440, 8'h00);
        repeat(4) @(posedge clk);
        check("debug poke disables HDMI audio test tone", !dut.hdmi_audio_test_enable);

        debug_poke(16'hD440, 8'h80);
        repeat(4) @(posedge clk);
        check("debug poke enables HDMI audio test tone", dut.hdmi_audio_test_enable);

        min_l = 262143;
        max_l = -262144;
        repeat(80000) begin
            @(posedge clk);
            if (audio_l < min_l)
                min_l = audio_l;
            if (audio_l > max_l)
                max_l = audio_l;
        end
        $display("Debug-poked HDMI diagnostic tone range: L=%0d..%0d", min_l, max_l);
        check("debug-poked HDMI tone drives negative samples", min_l < -60000);
        check("debug-poked HDMI tone drives positive samples", max_l > 60000);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

    initial begin
        #120000000;
        $display("FAIL: global timeout hit, PC=0x%04X", dbg_cpu_pc);
        $finish(1);
    end

endmodule
