// Oscillator sync test. The SID sync chain is v3→v1→v2→v3: when voice N's
// sync bit (ctrl[1]) is set, voice N's accumulator resets whenever the
// source voice's MSB falls (goes 1→0).
//
// We test via voice 3 (the only voice with direct osc_out readback at
// $1B). V3 syncs from V2. Set V2 to a higher frequency and V3 to a lower
// one. With sync OFF, V3 period is 2^24/freq_v3. With sync ON, V3's
// accumulator resets on every V2 MSB fall — so V3 never reaches full
// scale, and its waveform has many more rising crossings at a low
// threshold.
//
// V2 freq = 0x2000 → period = 2048 ticks (1MHz / (2^24/0x2000)).
// V3 freq = 0x0800 → natural period = 8192 ticks.
// Capture window = 32768 ticks.
// Sync OFF: full-scale V3 saw, ~4 rising crossings at midpoint 0x80,
//           max reaches near 0xFF.
// Sync ON:  V3 clamped by V2's MSB-fall resets, max stays ≤ 0x40,
//           never crosses 0x80. Crossings at low threshold 0x10 match
//           V2's ~16 sync events.
`timescale 1ns/1ps

module test_sid_sync;
    `include "sid_tb.svh"

    logic [7:0] samples[];

    // Set V2: high freq, sawtooth, gate on (to enable envelope → output
    // but we read osc3 which doesn't need envelope).
    task automatic setup_v2(input [15:0] freq);
        sid_write(5'h07, freq[7:0]);
        sid_write(5'h08, freq[15:8]);
        sid_write(5'h0B, 8'h10);    // sawtooth, no gate
    endtask

    // Set V3: freq + sawtooth + optional sync. No gate needed — osc3
    // readback doesn't depend on envelope.
    task automatic setup_v3_with_sync(input [15:0] freq,
                                       input logic sync_enable);
        sid_write(5'h0E, freq[7:0]);
        sid_write(5'h0F, freq[15:8]);
        sid_write(5'h12, {4'b0010, 2'b00, sync_enable, 1'b0});
        // bits: waveform=saw (0010 in [7:4]), test=0, ring=0, sync, gate=0
    endtask

    task automatic run_case(input logic sync,
                             output int rising_mid,
                             output int rising_low,
                             output logic [7:0] max_v);
        do_reset();
        setup_v2(16'h2000);
        setup_v3_with_sync(16'h0800, sync);
        wait_1m(500);                       // let oscillators align
        capture_osc3(samples, 32768);
        rising_mid = count_xings_8bit(samples, 8'h80);
        rising_low = count_xings_8bit(samples, 8'h10);
        max_v      = max_8bit(samples);
    endtask

    initial begin
        int mid_off, low_off, mid_on, low_on;
        logic [7:0] max_off, max_on;

        $display("=== SID oscillator sync ===");

        run_case(1'b0, mid_off, low_off, max_off);
        $display("  sync OFF → mid-xings=%0d low-xings=%0d max=0x%02x",
                 mid_off, low_off, max_off);
        run_case(1'b1, mid_on, low_on, max_on);
        $display("  sync ON  → mid-xings=%0d low-xings=%0d max=0x%02x",
                 mid_on, low_on, max_on);

        // Sync OFF: V3 saw runs full-scale. Max near 0xFF.
        check("sync-off max near full-scale", max_off > 8'hE0);
        // Sync ON: V3 saw clamped by V2's MSB-fall resets. Max stays small.
        check("sync-on max clipped below 0x80", max_on < 8'h80);

        // Sync OFF: ~4 rising crossings of midpoint (one per natural V3
        // period over the 32768-sample window).
        check_in_range("sync-off midpoint crossings ~4", mid_off, 3, 5);
        // Sync ON: never crosses midpoint because max is clipped.
        check_in_range("sync-on midpoint crossings =0", mid_on, 0, 0);

        // Low-threshold (0x10) crossings count sync events (sync ON) or
        // natural periods (sync OFF).
        check_in_range("sync-off low crossings ~4", low_off, 3, 5);
        // Sync ON should produce many more low crossings — one per V2
        // MSB-fall. V2 period = 2048 → 32768/2048 = 16 events.
        check_in_range("sync-on low crossings ~16", low_on, 13, 20);

        // Strict relationship: sync ON must cross 0x10 more often.
        check("sync-on >> sync-off at low threshold", low_on > low_off);

        summary();
        $finish;
    end
endmodule
