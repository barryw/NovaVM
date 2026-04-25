// Pulse-width / duty-cycle test. Exhaustive sweep across the 12-bit
// PW register. Duty is (4096 - pw) / 4096 * 100% since the pulse
// waveform asserts when osc[23:12] >= pw.
//
// Also verifies:
//   - PW is readable from both $02 (low byte) and $03 (high nibble)
//   - Extreme values: pw=0 → ~100% duty, pw=0xFFF → ~0% duty
//   - Low nibble of $03 only (high 4 bits must read as 0)
`timescale 1ns/1ps

module test_sid_pulse;
    `include "sid_tb.svh"

    logic [7:0] samples[];
    logic [7:0] d;

    task automatic measure_duty_voice3(input [11:0] pw, output int duty);
        do_reset();
        setup_voice3_tone(16'h2000, 4'b0100, pw);
        wait_1m(200);
        capture_osc3(samples, 4096);
        duty = duty_pct_8bit(samples, 8'h80);
    endtask

    initial begin
        int duty;

        $display("=== SID pulse width / duty cycle ===");

        // ── PW register readback ──
        do_reset();
        sid_write(5'h02, 8'hAB);           // voice 1 pw lo
        sid_write(5'h03, 8'h0C);           // voice 1 pw hi nibble
        sid_read(5'h02, d);
        check_eq("voice1 pw lo readback", int'(d), 8'hAB);
        sid_read(5'h03, d);
        check_eq("voice1 pw hi nibble readback (top 4 bits 0)",
                 int'(d), 8'h0C);

        // High nibble of $03 should NOT be stored (register holds 4 bits).
        sid_write(5'h03, 8'hFC);           // write high 4 bits = F
        sid_read(5'h03, d);
        check_eq("voice1 pw hi nibble masks top 4 bits",
                 int'(d), 8'h0C);

        // ── Duty cycle at pw=0x000 (should be ~100%) ──
        measure_duty_voice3(12'h000, duty);
        $display("  pw=0x000 duty=%0d%%", duty);
        check_in_range("pw=0x000 duty ~100%", duty, 95, 100);

        // ── Duty cycle at pw=0xFFF (should be ~0%) ──
        // pulse = osc[23:12] >= pw. osc[23:12] maxes at 0xFFF, so only
        // reaches exactly 0xFFF for ~1 tick per cycle. Duty should be ≤1%.
        measure_duty_voice3(12'hFFF, duty);
        $display("  pw=0xFFF duty=%0d%%", duty);
        check_in_range("pw=0xFFF duty ~0%", duty, 0, 5);

        // ── Sweep across common points ──
        measure_duty_voice3(12'h100, duty);  // 3840/4096 = 93.75%
        $display("  pw=0x100 duty=%0d%%", duty);
        check_in_range("pw=0x100 duty ~94%", duty, 85, 100);

        measure_duty_voice3(12'h200, duty);  // 3584/4096 = 87.5%
        $display("  pw=0x200 duty=%0d%%", duty);
        check_in_range("pw=0x200 duty ~87%", duty, 80, 95);

        measure_duty_voice3(12'h400, duty);  // 3072/4096 = 75%
        $display("  pw=0x400 duty=%0d%%", duty);
        check_in_range("pw=0x400 duty ~75%", duty, 65, 85);

        measure_duty_voice3(12'h800, duty);  // 2048/4096 = 50%
        $display("  pw=0x800 duty=%0d%%", duty);
        check_in_range("pw=0x800 duty ~50%", duty, 40, 60);

        measure_duty_voice3(12'hC00, duty);  // 1024/4096 = 25%
        $display("  pw=0xC00 duty=%0d%%", duty);
        check_in_range("pw=0xC00 duty ~25%", duty, 15, 35);

        measure_duty_voice3(12'hE00, duty);  // 512/4096 = 12.5%
        $display("  pw=0xE00 duty=%0d%%", duty);
        check_in_range("pw=0xE00 duty ~12%", duty, 5, 20);

        // ── Monotonic: higher pw → lower duty ──
        begin
            int d_low, d_mid, d_high;
            measure_duty_voice3(12'h200, d_low);   // high duty
            measure_duty_voice3(12'h800, d_mid);
            measure_duty_voice3(12'hE00, d_high);  // low duty
            check("duty decreases as pw increases",
                  d_low > d_mid && d_mid > d_high);
        end

        summary();
        $finish;
    end
endmodule
