// Ring modulation test. On a triangle waveform with ringmod bit set
// (ctrl[2]=1), the triangle-fold direction is XOR'd with the source
// voice's MSB — producing a multiplicative waveform between the two
// voices. The sync chain is v3→v1→v2→v3; ringmod uses the same source.
//
// We test via voice 3, reading osc3 at $1B. V3 ringmods from V2.
//
// V2 freq = 0x4000 → period = 1024 one-MHz ticks (MSB flips every 512).
// V3 freq = 0x0400 → natural period = 16384 ticks (1 rising midpoint
//   crossing per period without ringmod).
// Capture 16384 samples.
//   Ringmod OFF → smooth triangle, 1 rising midpoint crossing.
//   Ringmod ON  → every V2 MSB flip creates discontinuity in V3's
//                 triangle fold → many more crossings.
`timescale 1ns/1ps

module test_sid_ringmod;
    `include "sid_tb.svh"

    logic [7:0] samples[];

    task automatic setup_v2(input [15:0] freq);
        sid_write(5'h07, freq[7:0]);
        sid_write(5'h08, freq[15:8]);
        sid_write(5'h0B, 8'h10);    // sawtooth, no gate
    endtask

    // V3 triangle + optional ringmod (ctrl[2]=1).
    //   control[7:4]=0001 (triangle), control[3]=0 (test=0),
    //   control[2]=ring, control[1]=0 (no sync), control[0]=0 (gate off).
    task automatic setup_v3_ring(input [15:0] freq, input logic ring);
        sid_write(5'h0E, freq[7:0]);
        sid_write(5'h0F, freq[15:8]);
        sid_write(5'h12, {4'b0001, 1'b0, ring, 1'b0, 1'b0});
    endtask

    task automatic run_case(input logic ring,
                             output int rising,
                             output int distinct,
                             output logic [7:0] max_v);
        do_reset();
        setup_v2(16'h4000);
        setup_v3_ring(16'h0400, ring);
        wait_1m(500);
        capture_osc3(samples, 16384);
        rising   = count_xings_8bit(samples, 8'h80);
        distinct = distinct_count_8bit(samples);
        max_v    = max_8bit(samples);
    endtask

    initial begin
        int rise_off, rise_on, dist_off, dist_on;
        logic [7:0] max_off, max_on;

        $display("=== SID ring modulation ===");

        run_case(1'b0, rise_off, dist_off, max_off);
        $display("  ring OFF → rising=%0d distinct=%0d max=0x%02x",
                 rise_off, dist_off, max_off);
        run_case(1'b1, rise_on, dist_on, max_on);
        $display("  ring ON  → rising=%0d distinct=%0d max=0x%02x",
                 rise_on, dist_on, max_on);

        // Ring OFF: 1 V3 period → 1 rising midpoint crossing.
        check_in_range("ring-off midpoint crossings ~1", rise_off, 0, 2);
        // Ring ON: many additional crossings from V2 MSB flips.
        check("ring-on produces >5x more crossings than ring-off",
              rise_on > rise_off * 5);

        // Both modes reach near full-scale on triangle peak.
        check("ring-off max near full-scale", max_off > 8'hE0);
        check("ring-on max near full-scale",  max_on  > 8'hE0);

        // Ring ON creates additional distinct values from the fold
        // discontinuities.
        check("ring-on yields ≥ as many distinct samples as ring-off",
              dist_on >= dist_off - 16);

        summary();
        $finish;
    end
endmodule
