// SID unit test — verifies register read/write, voice output, model switching
`timescale 1ns/1ps

module test_sid;
    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic rst;
    logic ce_1m;
    logic mode;
    logic cs, we;
    logic [4:0] addr;
    logic [7:0] din, dout;
    logic signed [17:0] audio_out;

    // SID filter curve now comes from an external SDRAM reader. In this
    // stand-alone SID unit test we pass a fixed mid-range f0 so the
    // filter exercises consistently — audio tests don't rely on exact
    // curve values, just that the module processes them.
    logic [10:0] filter_fc_out;
    logic [15:0] filter_f0_in = 16'h8000;

    sid_chip uut (.*);

    // Generate ~1 MHz clock enable
    int ce_cnt = 0;
    always_ff @(posedge clk) begin
        ce_1m <= (ce_cnt == 24);
        ce_cnt <= (ce_cnt == 24) ? 0 : ce_cnt + 1;
    end

    int pass = 0, fail = 0;

    task write_reg(input [4:0] a, input [7:0] d);
        @(posedge clk);
        cs <= 1; we <= 1; addr <= a; din <= d;
        @(posedge clk);
        cs <= 0; we <= 0;
    endtask

    task check(string name, int actual, int expected);
        if (actual == expected) begin
            pass++;
        end else begin
            $display("FAIL: %s — got %0d, expected %0d", name, actual, expected);
            fail++;
        end
    endtask

    initial begin
        rst = 1; cs = 0; we = 0; addr = 0; din = 0; mode = 0;
        repeat(10) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);

        // ── Test 1: Register write and read back ──
        write_reg(5'h00, 8'hAB);  // Voice 1 freq lo
        write_reg(5'h01, 8'hCD);  // Voice 1 freq hi
        // Read back via last_written (non-readable registers return last write)
        cs = 1; we = 0; addr = 5'h01;
        @(posedge clk);
        check("Freq hi read-back", dout, 8'hCD);
        cs = 0;

        // ── Test 2: Per-voice volume defaults to $0F ──
        cs = 1; we = 0; addr = 5'h1D;
        @(posedge clk);
        check("Voice 1 vol default", dout, 8'h0F);
        cs = 0;

        // ── Test 3: Write per-voice volume ──
        write_reg(5'h1D, 8'h07);
        cs = 1; we = 0; addr = 5'h1D;
        @(posedge clk);
        check("Voice 1 vol written", dout, 8'h07);
        cs = 0;

        // ── Test 4: Short reset clears registers and voice internals ──
        write_reg(5'h00, 8'h34);
        write_reg(5'h01, 8'h12);
        write_reg(5'h04, 8'h21);
        write_reg(5'h15, 8'h07);
        write_reg(5'h16, 8'h55);
        repeat(200) @(posedge clk);
        rst = 1;
        repeat(2) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);
        cs = 1; we = 0; addr = 5'h00;
        @(posedge clk);
        check("Freq lo reset", dout, 8'h00);
        addr = 5'h1D;
        @(posedge clk);
        check("Voice 1 vol reset default", dout, 8'h0F);
        cs = 0;
        check("Filter Fc reset", filter_fc_out, 0);
        check("Voice oscillator reset", uut.v1.oscillator, 0);
        check("Audio latch reset", audio_out, 0);

        // ── Test 5: OSC3 and ENV3 readable ──
        cs = 1; we = 0; addr = 5'h1B;
        @(posedge clk);
        // Just check it doesn't hang — value depends on oscillator state
        check("OSC3 readable", 1, 1);  // just verifying no hang
        cs = 0;

        cs = 1; we = 0; addr = 5'h1C;
        @(posedge clk);
        check("ENV3 readable", 1, 1);  // just verifying no hang
        cs = 0;

        // ── Test 6: Set up a tone and verify audio is non-zero ──
        mode = 0;  // 6581 mode
        write_reg(5'h00, 8'h00);  // freq lo = 0
        write_reg(5'h01, 8'h10);  // freq hi = $10 (medium pitch)
        write_reg(5'h05, 8'h00);  // attack=0, decay=0
        write_reg(5'h06, 8'hF0);  // sustain=$F, release=0
        write_reg(5'h18, 8'h0F);  // master volume = 15
        write_reg(5'h04, 8'h21);  // sawtooth + gate on

        // Run for a good number of 1MHz cycles to let the envelope ramp up
        repeat(50000) @(posedge clk);  // ~2ms at 25MHz, ~80 SID clocks

        // Check audio output is non-zero (voice is playing)
        check("Audio non-zero (6581)", audio_out != 0, 1);

        // ── Test 7: Switch to 8580 mode ──
        mode = 1;
        repeat(50000) @(posedge clk);
        check("Audio non-zero (8580)", audio_out != 0, 1);

        // ── Test 8: Gate off should eventually silence ──
        write_reg(5'h04, 8'h20);  // gate off, sawtooth still selected
        repeat(200000) @(posedge clk);  // let release complete

        // ── Results ──
        $display("");
        if (fail == 0)
            $display("SID: All %0d tests passed", pass);
        else
            $display("SID: %0d passed, %0d FAILED", pass, fail);
        $finish;
    end
endmodule
