// Per-voice volume extension test. Registers $1D/$1E/$1F hold a 4-bit
// volume (0-15) per voice, applied pre-filter in sid_chip.sv:
//   v_scaled = voice_out * vol / 16
//
// Real MOS 6581 lacks this; it's a NovaBASIC extension. Defaults to $0F.
//
// Verify:
//   - Default value is $0F after reset
//   - Writing 0 silences that voice (AC range drops to floor)
//   - Writing F restores full amplitude
//   - Writing intermediate value (e.g. $07) produces roughly half-range
//   - Each $1D/$1E/$1F register targets only its own voice
`timescale 1ns/1ps

module test_sid_pervoice_vol;
    `include "sid_tb.svh"

    int unsigned samples[];

    function automatic longint ac_range(input int unsigned a[]);
        int i;
        int signed v, mn, mx;
        if (a.size() == 0) return 0;
        mn = $signed(a[0]); mx = mn;
        for (i = 1; i < a.size(); i++) begin
            v = $signed(a[i]);
            if (v < mn) mn = v;
            if (v > mx) mx = v;
        end
        return mx - mn;
    endfunction

    // Configure voice N (0..2) with sawtooth + gate + master vol 15.
    task automatic play_voice(input int n);
        logic [4:0] base;
        case (n)
            0: base = 5'h00;
            1: base = 5'h07;
            2: base = 5'h0E;
            default: base = 5'h00;
        endcase
        sid_write(5'(base + 0), 8'h00);
        sid_write(5'(base + 1), 8'h20);    // freq hi = 0x20
        sid_write(5'(base + 5), 8'h00);    // AD
        sid_write(5'(base + 6), 8'hF0);    // SR
        sid_write(5'h18, 8'h0F);           // master vol
        sid_write(5'(base + 4), 8'h21);    // saw + gate
    endtask

    task automatic measure_voice(input int n,
                                   input [3:0] voice_vol,
                                   output longint range);
        logic [4:0] vol_reg;
        case (n)
            0: vol_reg = 5'h1D;
            1: vol_reg = 5'h1E;
            2: vol_reg = 5'h1F;
            default: vol_reg = 5'h1D;
        endcase
        do_reset();
        play_voice(n);
        sid_write(vol_reg, {4'h0, voice_vol});
        wait_1m(2000);
        capture_audio(samples, 512);
        range = ac_range(samples);
    endtask

    initial begin
        logic [7:0] d;
        longint r_full, r_mute, r_half;
        longint r_v1_full, r_v1_mute_only_v1;
        longint r_v2_full, r_v3_full;

        $display("=== SID per-voice volume $1D-$1F ===");

        // ── Defaults after reset ──
        do_reset();
        sid_read(5'h1D, d);
        check_eq("v0 vol default $0F", int'(d), 8'h0F);
        sid_read(5'h1E, d);
        check_eq("v1 vol default $0F", int'(d), 8'h0F);
        sid_read(5'h1F, d);
        check_eq("v2 vol default $0F", int'(d), 8'h0F);

        // ── Voice 0: vol=F full amplitude, vol=0 silences, vol=7 scales ──
        measure_voice(0, 4'hF, r_full);
        $display("  v0 vol=F ac_range=%0d", r_full);
        measure_voice(0, 4'h0, r_mute);
        $display("  v0 vol=0 ac_range=%0d", r_mute);
        measure_voice(0, 4'h7, r_half);
        $display("  v0 vol=7 ac_range=%0d", r_half);

        check("v0 vol=F produces big range", r_full > 500);
        check("v0 vol=0 silences AC to <20% of full",
              r_mute * 5 < r_full);
        check("v0 vol=7 roughly half of full range",
              r_half > r_full / 4 && r_half < r_full);

        // ── Voice 1 (middle) independent ──
        measure_voice(1, 4'hF, r_v2_full);
        $display("  v1 vol=F ac_range=%0d", r_v2_full);
        check("v1 vol=F produces audio", r_v2_full > 500);

        // ── Voice 2 independent ──
        measure_voice(2, 4'hF, r_v3_full);
        $display("  v2 vol=F ac_range=%0d", r_v3_full);
        check("v2 vol=F produces audio", r_v3_full > 500);

        // ── Per-voice isolation: silence v0 only, v1 still playing ──
        do_reset();
        play_voice(0);
        play_voice(1);
        sid_write(5'h1D, 8'h00);        // v0 muted
        sid_write(5'h1E, 8'h0F);        // v1 full
        wait_1m(2000);
        capture_audio(samples, 512);
        r_v1_mute_only_v1 = ac_range(samples);
        $display("  v0 muted, v1 full ac_range=%0d", r_v1_mute_only_v1);
        check("v0 muted + v1 full still has audio",
              r_v1_mute_only_v1 > 500);

        // Confirm per-voice $1D mute alone doesn't change v1's register
        sid_read(5'h1E, d);
        check_eq("v1 vol preserved as F", int'(d), 8'h0F);

        summary();
        $finish;
    end
endmodule
