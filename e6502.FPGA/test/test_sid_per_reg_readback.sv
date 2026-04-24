// Per-register SID readback regression — verifies that each SID write
// register shadows its own value so PEEK returns what was POKEd.
//
// Before the fix: all unmapped register reads returned `last_written`,
// i.e. the most recent bus write, regardless of addr. So writing
//   $D400 <- $37
//   $D401 <- $1C
// then reading $D400 returned $1C (the last byte written, not voice-1
// freq lo). This broke the sid1-freq-write / sid2-freq-write /
// sid-filter-registers integration tests.
//
// After the fix: each register slot shadows independently; the case
// statement returns voice_freq[v][7:0] for addr $00, etc.

`timescale 1ns/1ps

module test_sid_per_reg_readback;
    logic clk = 0;
    always #20 clk = ~clk;  // 25 MHz

    logic rst;
    logic ce_1m;
    logic mode;
    logic cs, we;
    logic [4:0] addr;
    logic [7:0] din, dout;
    logic signed [17:0] audio_out;

    logic [10:0] filter_fc_out;
    logic [15:0] filter_f0_in = 16'h8000;

    sid_chip uut (.*);

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

    task automatic read_check(input string name, input [4:0] a, input [7:0] expected);
        @(posedge clk);
        cs <= 1; we <= 0; addr <= a;
        @(posedge clk);
        if (dout === expected) begin
            $display("  PASS %s (=0x%02X)", name, dout);
            pass++;
        end else begin
            $display("  FAIL %s (got 0x%02X, want 0x%02X)", name, dout, expected);
            fail++;
        end
        cs <= 0;
    endtask

    initial begin
        $display("=== SID per-register readback test ===");

        rst = 1; cs = 0; we = 0; addr = 0; din = 0; mode = 0;
        repeat(10) @(posedge clk);
        rst = 0;
        repeat(5) @(posedge clk);

        // Drive every write-register with a distinct byte pattern so that
        // a "return last write" bug shows up as every read returning the
        // final value ($ED, the last byte written below).
        write_reg(5'h00, 8'h37);    // voice 1 freq lo
        write_reg(5'h01, 8'h1C);    // voice 1 freq hi
        write_reg(5'h02, 8'h11);    // voice 1 pw lo
        write_reg(5'h03, 8'h0F);    // voice 1 pw hi (low nibble only)
        write_reg(5'h04, 8'h21);    // voice 1 ctrl (saw + gate)
        write_reg(5'h05, 8'h55);    // voice 1 ad
        write_reg(5'h06, 8'h99);    // voice 1 sr
        write_reg(5'h07, 8'hAB);    // voice 2 freq lo
        write_reg(5'h08, 8'hCD);    // voice 2 freq hi
        write_reg(5'h15, 8'h07);    // filter fc lo (3 bits only)
        write_reg(5'h16, 8'h80);    // filter fc hi
        write_reg(5'h17, 8'hF1);    // res + filter route
        write_reg(5'h18, 8'h0F);    // mode + master volume
        write_reg(5'h1D, 8'h07);    // voice 1 vol extension

        // Final extra write so last_written = $ED — a bug that returns
        // last_written instead of per-register value is very visible.
        write_reg(5'h1E, 8'hED);

        // Read back every slot we wrote and verify each returns its own
        // byte, not $ED. The voice-freq/pw/ctrl/ad/sr slots are the ones
        // that used to fall through to `default: last_written`.
        read_check("voice1.freq_lo  ($00)", 5'h00, 8'h37);
        read_check("voice1.freq_hi  ($01)", 5'h01, 8'h1C);
        read_check("voice1.pw_lo    ($02)", 5'h02, 8'h11);
        read_check("voice1.pw_hi    ($03)", 5'h03, 8'h0F);  // low nibble
        read_check("voice1.ctrl     ($04)", 5'h04, 8'h21);
        read_check("voice1.ad       ($05)", 5'h05, 8'h55);
        read_check("voice1.sr       ($06)", 5'h06, 8'h99);
        read_check("voice2.freq_lo  ($07)", 5'h07, 8'hAB);
        read_check("voice2.freq_hi  ($08)", 5'h08, 8'hCD);
        read_check("filter.fc_lo    ($15)", 5'h15, 8'h07);
        read_check("filter.fc_hi    ($16)", 5'h16, 8'h80);
        read_check("filter.res_filt ($17)", 5'h17, 8'hF1);
        read_check("filter.mode_vol ($18)", 5'h18, 8'h0F);
        read_check("voice1.vol_ext  ($1D)", 5'h1D, 8'h07);
        read_check("voice2.vol_ext  ($1E)", 5'h1E, 8'h0D);  // low nibble of $ED

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass, fail);
        if (fail == 0) $display("ALL TESTS PASSED");
        else           $display("SOME TESTS FAILED");
        $finish;
    end

    initial begin
        #1000000;
        $display("FAIL: global timeout hit");
        $finish;
    end
endmodule
