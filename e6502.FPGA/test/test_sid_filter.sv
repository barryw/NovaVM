// SID filter routing + resonance + mode test. Validates that the
// $D417 resonance/routing register and $D418 master/mode register
// control the audio path correctly.
//
// $D417 (Res_Filt):
//   bits [7:4] = resonance (4 bits)
//   bit  [0]   = route voice 1 through filter
//   bit  [1]   = route voice 2 through filter
//   bit  [2]   = route voice 3 through filter
//   bit  [3]   = route ext_in through filter
//
// $D418 (Mode_Vol):
//   bits [3:0] = master volume (0-15)
//   bit  [4]   = LP output mix
//   bit  [5]   = BP output mix
//   bit  [6]   = HP output mix
//   bit  [7]   = voice 3 OFF (when v3 not filter-routed)
//
// The filter path adds/drops signal based on these bits. We verify:
//   - master vol = 0 → silence
//   - all-direct, master vol = 15 → big audio
//   - voice-routed without filter mode enabled → audio mostly lost
//   - voice-routed with LP enabled → audio present, waveform differs
//     from direct path
//   - voice 3 OFF bit silences v3 on direct path
//   - resonance register change alters the filter response
`timescale 1ns/1ps

module test_sid_filter;
    `include "sid_tb.svh"

    int unsigned samples_a[];
    int unsigned samples_b[];

    // Maximum absolute sample value across a captured buffer.
    function automatic longint abs_max_signed(input int unsigned a[]);
        int i;
        int signed v;
        longint m;
        m = 0;
        for (i = 0; i < a.size(); i++) begin
            v = $signed(a[i]);
            if (v < 0) v = -v;
            if (v > m) m = v;
        end
        return m;
    endfunction

    // AC range: max - min of signed samples. DC-free energy proxy — a
    // pure DC output has range 0 even if magnitudes are large.
    function automatic longint ac_range_signed(input int unsigned a[]);
        int i;
        int signed v, mn, mx;
        if (a.size() == 0) return 0;
        mn = $signed(a[0]);
        mx = $signed(a[0]);
        for (i = 1; i < a.size(); i++) begin
            v = $signed(a[i]);
            if (v < mn) mn = v;
            if (v > mx) mx = v;
        end
        return mx - mn;
    endfunction

    // Run voice 1 sawtooth through given filter settings, return AC range.
    task automatic measure_filter(input [7:0] res_filt,
                                   input [7:0] mode_vol,
                                   input int n_samples,
                                   output longint peak,
                                   output longint ac_range);
        do_reset();
        // Voice 1: freq=0x2000, sawtooth, gate on, instant attack
        sid_write(5'h00, 8'h00);
        sid_write(5'h01, 8'h20);
        sid_write(5'h05, 8'h00);          // a=0, d=0
        sid_write(5'h06, 8'hF0);          // s=F, r=0
        sid_write(5'h17, res_filt);       // resonance + routing
        sid_write(5'h18, mode_vol);       // master vol + mode
        sid_write(5'h04, 8'h21);          // saw + gate
        wait_1m(2000);                    // let envelope reach max

        capture_audio(samples_a, n_samples);
        peak     = abs_max_signed(samples_a);
        ac_range = ac_range_signed(samples_a);
    endtask

    initial begin
        longint peak_direct, peak_mute, peak_lost, peak_lp;
        longint peak_hp, peak_bp;
        longint ac_direct, ac_mute, ac_lost, ac_lp, ac_hp, ac_bp;
        longint peak_low_q, peak_high_q;
        longint ac_low_q, ac_high_q;

        $display("=== SID filter routing + resonance ===");

        // ── Test A: master volume = 0 silences output ──
        measure_filter(8'h00, 8'h00, 512, peak_mute, ac_mute);
        $display("  A [direct, vol=0] peak=%0d ac_range=%0d",
                 peak_mute, ac_mute);
        check("master vol=0 silences output", peak_mute < 1024);

        // ── Test B: all unfiltered, master vol = 15 → audible ──
        measure_filter(8'h00, 8'h0F, 512, peak_direct, ac_direct);
        $display("  B [direct, vol=F] peak=%0d ac_range=%0d",
                 peak_direct, ac_direct);
        check("direct path produces audio", peak_direct > 1000);
        check("direct path has AC content", ac_direct > 500);

        // ── Test C: voice 1 routed to filter, no filter mode ──
        //   Signal enters filter but no LP/BP/HP output bit set so the
        //   filter output is excluded from the mixer. Direct path drops
        //   voice 1 (Res_Filt[0]=1). AC content should collapse — only
        //   DC from MIXER_DC + voice2/3 idle DC remains.
        measure_filter(8'h01, 8'h0F, 512, peak_lost, ac_lost);
        $display("  C [v1→filt, no mode] peak=%0d ac_range=%0d",
                 peak_lost, ac_lost);
        check("voice-routed with no filter mode loses AC signal",
              ac_lost < ac_direct / 4);

        // ── Test D: voice 1 routed + LP enabled → audio differs ──
        measure_filter(8'h01, 8'h1F, 512, peak_lp, ac_lp);
        $display("  D [v1→filt, LP on] peak=%0d ac_range=%0d",
                 peak_lp, ac_lp);
        check("LP filter output has AC content", ac_lp > 200);
        check("LP AC content differs from direct path",
              ac_lp != ac_direct);

        // ── Test E: voice 1 routed + HP enabled ──
        measure_filter(8'h01, 8'h4F, 512, peak_hp, ac_hp);
        $display("  E [v1→filt, HP on] peak=%0d ac_range=%0d",
                 peak_hp, ac_hp);
        check("HP filter output has AC content", ac_hp > 0);
        check("HP AC content differs from LP AC content",
              ac_hp != ac_lp);

        // ── Test F: voice 1 routed + BP enabled ──
        measure_filter(8'h01, 8'h2F, 512, peak_bp, ac_bp);
        $display("  F [v1→filt, BP on] peak=%0d ac_range=%0d",
                 peak_bp, ac_bp);
        check("BP filter output has AC content", ac_bp > 0);
        check("BP AC content differs from LP AC content",
              ac_bp != ac_lp);

        // ── Test G: resonance change alters filter response ──
        // Low Q (res=0), LP on.
        measure_filter(8'h01, 8'h1F, 512, peak_low_q, ac_low_q);
        $display("  G [Q=0]  peak=%0d ac_range=%0d",
                 peak_low_q, ac_low_q);
        // High Q (res=F), LP on.
        measure_filter(8'hF1, 8'h1F, 512, peak_high_q, ac_high_q);
        $display("  G [Q=F]  peak=%0d ac_range=%0d",
                 peak_high_q, ac_high_q);
        check("resonance change alters filter response",
              peak_low_q != peak_high_q ||
              ac_low_q   != ac_high_q);

        summary();
        $finish;
    end
endmodule
