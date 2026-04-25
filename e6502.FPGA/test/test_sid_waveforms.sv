// Waveform sample-correctness test. Samples the voice-3 oscillator via
// the $1B OSC3 readback (8-bit top nibbles of the active waveform) and
// verifies:
//   - triangle: ramp up-and-down, one rising midpoint cross per period
//   - sawtooth: ramp up, one rising midpoint cross per period, full range
//   - pulse:    only a few distinct values; duty cycle tracks PW register
//   - noise:    many distinct values across the range
//
// Frequency is chosen so the test runs fast but has clear periodicity:
//   freq = 0x2000 → oscillator period = 2^24 / 0x2000 = 2048 one-MHz ticks.
//   Sampling 4096 ticks ≈ 2 full periods.
`timescale 1ns/1ps

module test_sid_waveforms;
    `include "sid_tb.svh"

    logic [7:0] samples[];

    task automatic run_triangle();
        int rising, min_v, max_v, distinct;
        do_reset();
        setup_voice3_tone(16'h2000, 4'b0001, 12'h000);
        wait_1m(200);                       // skip attack / startup
        capture_osc3(samples, 4096);
        rising = count_xings_8bit(samples, 8'h80);
        min_v  = min_8bit(samples);
        max_v  = max_8bit(samples);
        distinct = distinct_count_8bit(samples);
        $display("  [tri] rising-xings=%0d min=0x%02x max=0x%02x distinct=%0d",
                 rising, min_v, max_v, distinct);
        // 2 full periods → 2 rising crosses of mid-point (±1 for edges).
        check_in_range("triangle rising midpoint crossings", rising, 1, 3);
        // Triangle sweeps a wide range — well above 30% distinct 8-bit vals.
        check("triangle produces many distinct samples", distinct > 50);
        check("triangle max near full-scale", max_v > 8'hE0);
        check("triangle min near zero", min_v < 8'h20);
    endtask

    task automatic run_sawtooth();
        int rising, min_v, max_v, distinct;
        do_reset();
        setup_voice3_tone(16'h2000, 4'b0010, 12'h000);
        wait_1m(200);
        capture_osc3(samples, 4096);
        rising = count_xings_8bit(samples, 8'h80);
        min_v  = min_8bit(samples);
        max_v  = max_8bit(samples);
        distinct = distinct_count_8bit(samples);
        $display("  [saw] rising-xings=%0d min=0x%02x max=0x%02x distinct=%0d",
                 rising, min_v, max_v, distinct);
        check_in_range("sawtooth rising midpoint crossings", rising, 1, 3);
        check("sawtooth max near full-scale", max_v > 8'hE0);
        check("sawtooth min near zero", min_v < 8'h20);
        check("sawtooth produces many distinct samples", distinct > 50);
    endtask

    // Pulse at 50% duty (pw=0x800). osc[23:12] runs 0..0xFFF; pulse asserts
    // when >= pw, so duty = (0x1000 - pw) / 0x1000. pw=0x800 → 50%.
    task automatic run_pulse_50pct();
        int duty, distinct, min_v, max_v;
        do_reset();
        setup_voice3_tone(16'h2000, 4'b0100, 12'h800);
        wait_1m(200);
        capture_osc3(samples, 4096);
        duty     = duty_pct_8bit(samples, 8'h80);
        distinct = distinct_count_8bit(samples);
        min_v    = min_8bit(samples);
        max_v    = max_8bit(samples);
        $display("  [pul 50%%] duty=%0d distinct=%0d min=0x%02x max=0x%02x",
                 duty, distinct, min_v, max_v);
        check_in_range("pulse 50% duty cycle", duty, 40, 60);
        // Pulse waveform is only two levels; ≤4 distinct is tight enough
        // to prove it's not a ramp.
        check("pulse has ≤4 distinct output values", distinct <= 4);
    endtask

    // Pulse at 25% duty (pw=0xC00 → 1024 / 4096 = 25%).
    task automatic run_pulse_25pct();
        int duty;
        do_reset();
        setup_voice3_tone(16'h2000, 4'b0100, 12'hC00);
        wait_1m(200);
        capture_osc3(samples, 4096);
        duty = duty_pct_8bit(samples, 8'h80);
        $display("  [pul 25%%] duty=%0d", duty);
        check_in_range("pulse 25% duty cycle", duty, 15, 35);
    endtask

    // Pulse at 75% duty (pw=0x400 → 3072 / 4096 = 75%).
    task automatic run_pulse_75pct();
        int duty;
        do_reset();
        setup_voice3_tone(16'h2000, 4'b0100, 12'h400);
        wait_1m(200);
        capture_osc3(samples, 4096);
        duty = duty_pct_8bit(samples, 8'h80);
        $display("  [pul 75%%] duty=%0d", duty);
        check_in_range("pulse 75% duty cycle", duty, 65, 85);
    endtask

    // Noise: LFSR drives output. Many distinct values, no fixed period.
    task automatic run_noise();
        int distinct, min_v, max_v;
        do_reset();
        setup_voice3_tone(16'h4000, 4'b1000, 12'h000);
        wait_1m(500);
        capture_osc3(samples, 4096);
        distinct = distinct_count_8bit(samples);
        min_v = min_8bit(samples);
        max_v = max_8bit(samples);
        $display("  [noi] distinct=%0d min=0x%02x max=0x%02x",
                 distinct, min_v, max_v);
        // Noise output is 8-bits drawn from the LFSR mid-bits — expect
        // many unique samples over 4096 reads.
        check("noise produces >30 distinct samples", distinct > 30);
        check("noise max reasonably high", max_v > 8'h80);
    endtask

    initial begin
        $display("=== SID waveform correctness ===");

        run_triangle();
        run_sawtooth();
        run_pulse_50pct();
        run_pulse_25pct();
        run_pulse_75pct();
        run_noise();

        summary();
        $finish;
    end
endmodule
