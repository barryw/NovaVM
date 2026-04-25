// ADSR rate-timing test. Exercises the envelope generator via env3_out
// readback at $1C. Verifies:
//   - attack from 0 to 0xFF takes rates[N] * 256 one-MHz ticks (approx)
//   - gate-off triggers release phase
//   - different rates (0, 1, 2) scale envelope time correctly
//
// Rates (from sid_envelope.sv): rate 0=8, 1=31, 2=62, 3=94, ...
// Attack (linear): full ramp ≈ rates[N] × 256 one-MHz cycles.
// We allow ±20% tolerance because envelope counter ordering around ce_1m
// edges costs a handful of ticks.
`timescale 1ns/1ps

module test_sid_adsr;
    `include "sid_tb.svh"

    // Tick-count from gate-on until env3 first reads 8'hFF.
    // Uses voice 3 because env3 is directly readable at $1C.
    task automatic measure_attack(input [3:0] attack_rate,
                                   output int ticks);
        logic [7:0] env;
        int elapsed, max_ticks;
        max_ticks = 200000;  // cap at 200ms of 1MHz ticks
        // Full reset — envelope state persists across tests otherwise
        // (env=0xFF carried over makes attack "complete" at tick 0).
        do_reset();
        // Configure voice 3: freq=0 (doesn't matter), waveform=triangle,
        // attack=attack_rate, decay=0, sustain=F, release=0, master vol=F.
        sid_write(5'h0E, 8'h00);
        sid_write(5'h0F, 8'h00);
        sid_write(5'h13, {attack_rate, 4'h0});  // attack | decay
        sid_write(5'h14, 8'hF0);                // sustain=F, release=0
        sid_write(5'h18, 8'h0F);
        sid_write(5'h12, 8'h00);                // gate off, ensure edge
        wait_1m(4);
        sid_write(5'h12, 8'h11);                // triangle + gate on

        elapsed = 0;
        env = 0;
        while (elapsed < max_ticks) begin
            wait_1m(1);
            env = env3_now();
            if (env == 8'hFF) break;
            elapsed++;
        end
        ticks = elapsed;
    endtask

    // Tick-count from gate-off at full envelope until env3 reads zero.
    // Release rate is the lower nibble of $14 sus_rel.
    task automatic measure_release(input [3:0] release_rate,
                                    output int ticks);
        logic [7:0] env;
        int elapsed, max_ticks;
        max_ticks = 200000;
        // Full reset so envelope starts at 0.
        do_reset();
        // Get envelope to sustain first: attack=0, decay=0, sustain=F,
        // release=release_rate.
        sid_write(5'h13, 8'h00);                        // attack=0, decay=0
        sid_write(5'h14, {4'hF, release_rate});         // sustain=F, release
        sid_write(5'h12, 8'h00);
        wait_1m(4);
        sid_write(5'h12, 8'h11);                        // gate on
        wait_1m(3000);                                   // reach 0xFF
        // Gate off -> release phase
        sid_write(5'h12, 8'h10);

        elapsed = 0;
        env = 8'hFF;
        while (elapsed < max_ticks) begin
            wait_1m(1);
            env = env3_now();
            if (env == 0) break;
            elapsed++;
        end
        ticks = elapsed;
    endtask

    initial begin
        int ticks;
        int expected, lo, hi;

        $display("=== SID ADSR rate timing ===");
        do_reset();

        // ── Attack rate 0 (nominal 2ms, rates[0]=8 per step) ──
        // expected = 8 * 255 = 2040 ticks
        measure_attack(4'd0, ticks);
        expected = 8 * 255;
        lo = expected - (expected / 5);
        hi = expected + (expected / 5);
        check_in_range("attack rate 0 (~2ms)", ticks, lo, hi);

        // ── Attack rate 1 (nominal 8ms, rates[1]=31 per step) ──
        measure_attack(4'd1, ticks);
        expected = 31 * 255;
        lo = expected - (expected / 5);
        hi = expected + (expected / 5);
        check_in_range("attack rate 1 (~8ms)", ticks, lo, hi);

        // ── Attack rate 2 (nominal 16ms, rates[2]=62 per step) ──
        measure_attack(4'd2, ticks);
        expected = 62 * 255;
        lo = expected - (expected / 5);
        hi = expected + (expected / 5);
        check_in_range("attack rate 2 (~16ms)", ticks, lo, hi);

        // ── Release rate 0 drops to zero relatively quickly ──
        // Release is exponential — not a fixed tick count, but within
        // ~3x the attack value is a good sanity bound. Real chip: 6ms.
        measure_release(4'd0, ticks);
        check_in_range("release rate 0 returns to zero", ticks, 500, 60000);

        // ── Release rate 1 takes longer than rate 0 ──
        begin
            int t0, t1;
            measure_release(4'd0, t0);
            measure_release(4'd1, t1);
            check("release rate 1 > rate 0", t1 > t0);
        end

        summary();
        $finish;
    end
endmodule
