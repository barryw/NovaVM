// 6581 vs 8580 DAC distortion model test. Verifies the `mode` input
// selects between the two behaviors:
//
//   6581: VOICE_DC = 0x340*0xFF (large DC offset), WAVEFORM_DC = 0x380,
//         env_dac = env_6581 (distorted by sid_dac), MIXER_DC_6581 in
//         filter mixer. norm_dac passes through sid_dac distortion.
//   8580: VOICE_DC = 0, WAVEFORM_DC = 0x800, env_dac = raw envelope,
//         no mixer DC. norm_dac bypasses distortion.
//
// We verify:
//   - Same configuration in 6581 vs 8580 produces different DC offset
//     (6581 has much larger absolute DC bias)
//   - Same configuration produces different sample buffers (not
//     bit-identical)
`timescale 1ns/1ps

module test_sid_model;
    `include "sid_tb.svh"

    int unsigned samples_6581[];
    int unsigned samples_8580[];

    function automatic longint mean_signed(input int unsigned a[]);
        int i;
        int signed v;
        longint s, r;
        s = 0;
        r = 0;
        for (i = 0; i < a.size(); i++) begin
            v = $signed(a[i]);
            s += v;
        end
        if (a.size() > 0) r = s / a.size();
        mean_signed = r;
    endfunction

    function automatic longint ac_range(input int unsigned a[]);
        int i;
        int signed v, mn, mx;
        longint r;
        r = 0;
        if (a.size() > 0) begin
            mn = $signed(a[0]); mx = mn;
            for (i = 1; i < a.size(); i++) begin
                v = $signed(a[i]);
                if (v < mn) mn = v;
                if (v > mx) mx = v;
            end
            r = mx - mn;
        end
        ac_range = r;
    endfunction

    function automatic int buffers_identical(input int unsigned a[],
                                               input int unsigned b[]);
        int i, n, same;
        n = (a.size() < b.size()) ? a.size() : b.size();
        same = 1;
        for (i = 0; i < n; i++)
            if (a[i] != b[i]) same = 0;
        buffers_identical = same;
    endfunction

    // Two separate tasks — SV `ref` args can't bind to module-level
    // dynamic arrays cleanly under Verilator. One task per target.
    task automatic play_sawtooth_6581();
        do_reset();
        mode = 1'b0;
        sid_write(5'h00, 8'h00);
        sid_write(5'h01, 8'h20);
        sid_write(5'h05, 8'h00);
        sid_write(5'h06, 8'hF0);
        sid_write(5'h18, 8'h0F);
        sid_write(5'h04, 8'h21);
        wait_1m(2000);
        capture_audio(samples_6581, 1024);
    endtask

    task automatic play_sawtooth_8580();
        do_reset();
        mode = 1'b1;
        sid_write(5'h00, 8'h00);
        sid_write(5'h01, 8'h20);
        sid_write(5'h05, 8'h00);
        sid_write(5'h06, 8'hF0);
        sid_write(5'h18, 8'h0F);
        sid_write(5'h04, 8'h21);
        wait_1m(2000);
        capture_audio(samples_8580, 1024);
    endtask

    initial begin
        longint mean_6581, mean_8580, ac_6581, ac_8580;

        $display("=== SID 6581 vs 8580 DAC delta ===");

        play_sawtooth_6581();
        play_sawtooth_8580();

        mean_6581 = mean_signed(samples_6581);
        mean_8580 = mean_signed(samples_8580);
        ac_6581   = ac_range(samples_6581);
        ac_8580   = ac_range(samples_8580);

        $display("  6581: mean=%0d ac_range=%0d", mean_6581, ac_6581);
        $display("  8580: mean=%0d ac_range=%0d", mean_8580, ac_8580);

        // ── DC offset differs significantly ──
        // 6581 has MIXER_DC_6581 + VOICE_DC_6581 contribution → negative
        // mean (MIXER_DC_6581 is negative).
        // 8580 has no such DC offset → mean near zero.
        begin
            longint abs_mean_6581, abs_mean_8580;
            abs_mean_6581 = (mean_6581 < 0) ? -mean_6581 : mean_6581;
            abs_mean_8580 = (mean_8580 < 0) ? -mean_8580 : mean_8580;
            check("6581 DC offset is large",
                  abs_mean_6581 > 100);
            check("8580 DC offset smaller than 6581",
                  abs_mean_8580 < abs_mean_6581);
        end

        // ── Both modes produce AC content (audio is live) ──
        check("6581 produces AC content", ac_6581 > 500);
        check("8580 produces AC content", ac_8580 > 500);

        // ── Samples are not bit-identical between modes ──
        check("6581 vs 8580 sample buffers differ",
              buffers_identical(samples_6581, samples_8580) == 0);

        // ── Differential energy (sum of abs diffs) is substantial ──
        begin
            longint diff;
            diff = abs_diff_signed(samples_6581, samples_8580);
            $display("  sum |6581 - 8580| = %0d", diff);
            check("6581/8580 differential energy is significant",
                  diff > 1000);
        end

        summary();
        $finish;
    end
endmodule
