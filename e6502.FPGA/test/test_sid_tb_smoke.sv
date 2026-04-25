// Smoke test for sid_tb.svh — exercises the helper itself.
// Sanity-checks reset, bus write/read, and a short audio capture.
// Gets removed or folded into test_sid.sv once the 10 real tests are in.
`timescale 1ns/1ps

module test_sid_tb_smoke;
    `include "sid_tb.svh"

    int unsigned samples[];

    initial begin
        $display("=== sid_tb.svh smoke ===");
        do_reset();

        // Register write/read round-trip.
        sid_write(5'h00, 8'hA5);
        begin
            logic [7:0] d;
            sid_read(5'h00, d);
            check_eq("freq lo read-back", int'(d), 8'hA5);
        end

        // Default per-voice volume should be $0F.
        begin
            logic [7:0] d;
            sid_read(5'h1D, d);
            check_eq("voice1 vol default", int'(d), 8'h0F);
        end

        // Fire up a saw tone and capture 100 samples — just verify non-zero.
        setup_voice1_tone(16'h1000, 4'b0010, 12'h800);
        wait_1m(1000);
        capture_audio(samples, 100);
        begin
            int i, nonzero;
            nonzero = 0;
            for (i = 0; i < samples.size(); i++)
                if ($signed(samples[i]) != 0) nonzero++;
            check("audio non-zero after 1ms attack", nonzero > 10);
        end

        summary();
        $finish;
    end
endmodule
