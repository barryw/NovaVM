// SID HDMI audio conditioning regression.
//
// The HDMI audio path must remove the SID's DC bias before PCM packetization.
// A steady biased sample should decay toward silence, while a biased waveform
// should still produce signed PCM movement.

`timescale 1ns/1ps

module test_sid_hdmi_audio;
    logic clk = 1'b0;
    always #10 clk = ~clk;

    logic rst;
    logic sample_en;
    logic signed [17:0] sid_audio_l;
    logic signed [17:0] sid_audio_r;
    logic [1:0][15:0] audio_sample_word;

    sid_hdmi_audio dut (
        .clk(clk),
        .rst(rst),
        .sample_en(sample_en),
        .sid_audio_l(sid_audio_l),
        .sid_audio_r(sid_audio_r),
        .audio_sample_word(audio_sample_word)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num = 0;

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

    function automatic int pcm16(input logic [15:0] v);
        logic signed [15:0] s;
        begin
            s = v;
            pcm16 = s;
        end
    endfunction

    function automatic int abs_int(input int v);
        abs_int = (v < 0) ? -v : v;
    endfunction

    task automatic reset_dut();
        sid_audio_l = 18'sd0;
        sid_audio_r = 18'sd0;
        sample_en = 1'b1;
        rst = 1'b1;
        repeat(4) @(posedge clk);
        rst = 1'b0;
        repeat(2) @(posedge clk);
    endtask

    task automatic test_reset_silence();
        $display("");
        $display("Test: reset outputs silent PCM");
        reset_dut();
        check("left PCM is zero after reset", audio_sample_word[0] == 16'h0000);
        check("right PCM is zero after reset", audio_sample_word[1] == 16'h0000);
    endtask

    task automatic test_constant_dc_decays_to_silence();
        int first_l;
        int first_r;
        int final_l;
        int final_r;

        $display("");
        $display("Test: constant SID DC bias decays toward silence");
        reset_dut();

        sid_audio_l = -18'sd9255;
        sid_audio_r = -18'sd1575;
        repeat(2) @(posedge clk);
        first_l = pcm16(audio_sample_word[0]);
        first_r = pcm16(audio_sample_word[1]);

        repeat(32768) @(posedge clk);
        final_l = pcm16(audio_sample_word[0]);
        final_r = pcm16(audio_sample_word[1]);

        $display("DC response: first L=%0d R=%0d final L=%0d R=%0d",
                 first_l, first_r, final_l, final_r);
        check("left initial DC is visible to filter", abs_int(first_l) > 1000);
        check("right initial DC is visible to filter", abs_int(first_r) > 100);
        check("left sustained DC decays near zero", abs_int(final_l) <= 16);
        check("right sustained DC decays near zero", abs_int(final_r) <= 16);
    endtask

    task automatic test_sample_hold_between_enables();
        logic [15:0] held_l;
        logic [15:0] held_r;

        $display("");
        $display("Test: PCM updates only on sample strobe");
        reset_dut();

        sid_audio_l = -18'sd9255;
        sid_audio_r = -18'sd1575;
        @(posedge clk);
        held_l = audio_sample_word[0];
        held_r = audio_sample_word[1];

        sample_en = 1'b0;
        sid_audio_l = 18'sd131071;
        sid_audio_r = -18'sd131072;
        repeat(8) @(posedge clk);
        check("left PCM holds while sample_en is low", audio_sample_word[0] == held_l);
        check("right PCM holds while sample_en is low", audio_sample_word[1] == held_r);

        sample_en = 1'b1;
        @(posedge clk);
        check("left PCM updates when sample_en returns", audio_sample_word[0] != held_l);
        check("right PCM updates when sample_en returns", audio_sample_word[1] != held_r);
    endtask

    task automatic test_biased_waveform_survives();
        int min_l;
        int max_l;
        int min_r;
        int max_r;
        int cur_l;
        int cur_r;

        $display("");
        $display("Test: biased waveform still produces PCM movement");
        reset_dut();

        min_l = 32767;
        max_l = -32768;
        min_r = 32767;
        max_r = -32768;

        for (int i = 0; i < 8192; i++) begin
            sid_audio_l = (i[0] == 1'b0) ? -18'sd9255 : -18'sd1575;
            sid_audio_r = (i[1] == 1'b0) ? -18'sd9255 : -18'sd1575;
            @(posedge clk);
        end

        for (int i = 0; i < 512; i++) begin
            sid_audio_l = (i[0] == 1'b0) ? -18'sd9255 : -18'sd1575;
            sid_audio_r = (i[1] == 1'b0) ? -18'sd9255 : -18'sd1575;
            @(posedge clk);

            cur_l = pcm16(audio_sample_word[0]);
            cur_r = pcm16(audio_sample_word[1]);
            if (cur_l < min_l) min_l = cur_l;
            if (cur_l > max_l) max_l = cur_l;
            if (cur_r < min_r) min_r = cur_r;
            if (cur_r > max_r) max_r = cur_r;
        end

        $display("PCM ranges: L=%0d..%0d R=%0d..%0d", min_l, max_l, min_r, max_r);
        check("left biased waveform crosses negative", min_l < -1000);
        check("left biased waveform crosses positive", max_l > 500);
        check("right biased waveform crosses negative", min_r < -1000);
        check("right biased waveform crosses positive", max_r > 500);
    endtask

    task automatic test_transients_saturate();
        $display("");
        $display("Test: large transients saturate instead of wrapping");
        reset_dut();

        sid_audio_l = -18'sd131072;
        sid_audio_r = 18'sd131071;
        repeat(2) @(posedge clk);
        check("left negative transient saturates", audio_sample_word[0] == 16'h8000);
        check("right positive transient saturates", audio_sample_word[1] == 16'h7fff);
    endtask

    initial begin
        $display("=== SID HDMI audio conditioning tests ===");
        test_reset_silence();
        test_constant_dc_decays_to_silence();
        test_sample_hold_between_enables();
        test_biased_waveform_survives();
        test_transients_saturate();

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

    initial begin
        #5000000;
        $display("FAIL: global timeout hit");
        $finish(1);
    end
endmodule
