// Shared SID testbench scaffolding — include from inside a `module test_...`.
// Provides: clock/reset, DUT signal decls, sid_chip instance, helper tasks,
// DAC sample capture, pass/fail counters. Designed for Verilator --timing.
//
// Mirrors the structure of vgc_tb.svh but for the sid_chip at 1 MHz ce-rate.

// ---------------------------------------------------------------------------
// Clock / reset
// ---------------------------------------------------------------------------
logic clk = 0;
always #20 clk = ~clk;  // 25 MHz

logic rst;
logic ce_1m;

// 25 MHz → 1 MHz enable (25-count divider)
int ce_cnt = 0;
always_ff @(posedge clk) begin
    ce_1m <= (ce_cnt == 24);
    ce_cnt <= (ce_cnt == 24) ? 0 : ce_cnt + 1;
end

// ---------------------------------------------------------------------------
// DUT signals
// ---------------------------------------------------------------------------
logic mode = 0;
logic cs = 0, we = 0;
logic [4:0] addr = 0;
logic [7:0] din = 0;
wire  [7:0] dout;
wire signed [17:0] audio_out;

// Fixed mid-range filter curve — tests that don't care about filter bypass it
// or set filter_res_filt=0 so F0 has minimal effect on audio path.
logic [15:0] filter_f0_in = 16'h8000;
wire  [10:0] filter_fc_out;

sid_chip dut (
    .clk(clk),
    .rst(rst),
    .ce_1m(ce_1m),
    .mode(mode),
    .cs(cs),
    .we(we),
    .addr(addr),
    .din(din),
    .dout(dout),
    .audio_out(audio_out),
    .filter_fc_out(filter_fc_out),
    .filter_f0_in(filter_f0_in)
);

// ---------------------------------------------------------------------------
// Test accounting
// ---------------------------------------------------------------------------
int pass_count = 0;
int fail_count = 0;
int test_num   = 0;

task automatic check(input string name, input logic condition);
    test_num++;
    if (condition) begin
        $display("  PASS [%0d] %s", test_num, name);
        pass_count++;
    end else begin
        $display("  FAIL [%0d] %s", test_num, name);
        fail_count++;
    end
endtask

task automatic check_eq(input string name, input int actual, input int expected);
    test_num++;
    if (actual == expected) begin
        $display("  PASS [%0d] %s (=%0d)", test_num, name, actual);
        pass_count++;
    end else begin
        $display("  FAIL [%0d] %s (got %0d, want %0d)",
                 test_num, name, actual, expected);
        fail_count++;
    end
endtask

task automatic check_in_range(input string name,
                              input int actual,
                              input int lo,
                              input int hi);
    test_num++;
    if (actual >= lo && actual <= hi) begin
        $display("  PASS [%0d] %s (=%0d in [%0d..%0d])",
                 test_num, name, actual, lo, hi);
        pass_count++;
    end else begin
        $display("  FAIL [%0d] %s (got %0d, want [%0d..%0d])",
                 test_num, name, actual, lo, hi);
        fail_count++;
    end
endtask

task automatic summary();
    $display("");
    $display("=== Results: %0d passed, %0d failed ===",
             pass_count, fail_count);
    if (fail_count == 0) $display("ALL TESTS PASSED");
    else                 $display("SOME TESTS FAILED");
endtask

// ---------------------------------------------------------------------------
// Reset / step helpers
// ---------------------------------------------------------------------------
task automatic do_reset();
    rst  = 1;
    cs   = 0;
    we   = 0;
    addr = 0;
    din  = 0;
    mode = 0;
    repeat(20) @(posedge clk);
    rst = 0;
    repeat(10) @(posedge clk);
endtask

task automatic step(input int n);
    repeat(n) @(posedge clk);
endtask

// Wait N 1-MHz cycles (one ce_1m pulse per cycle).
task automatic wait_1m(input int n);
    int seen;
    seen = 0;
    while (seen < n) begin
        @(posedge clk);
        if (ce_1m) seen++;
    end
endtask

// ---------------------------------------------------------------------------
// Bus helpers
// ---------------------------------------------------------------------------
task automatic sid_write(input [4:0] a, input [7:0] d);
    @(posedge clk);
    cs   <= 1;
    we   <= 1;
    addr <= a;
    din  <= d;
    @(posedge clk);
    cs <= 0;
    we <= 0;
endtask

task automatic sid_read(input [4:0] a, output logic [7:0] d);
    @(posedge clk);
    cs   <= 1;
    we   <= 0;
    addr <= a;
    @(posedge clk);
    d = dout;
    cs <= 0;
endtask

// ---------------------------------------------------------------------------
// Audio capture helpers — sample sid_chip audio_out, which latches at state=6
// (once per ce_1m cycle).
// ---------------------------------------------------------------------------

// Collect N audio samples at 1 MHz tick rate. Buffer must be sized in caller.
task automatic capture_audio(ref int unsigned samples[], input int n);
    int i;
    samples = new[n];
    for (i = 0; i < n; i++) begin
        wait_1m(1);
        // audio_out latches inside sid_chip on tbl_state == 6; give one extra
        // clock edge for the latch to settle before sampling.
        @(posedge clk);
        samples[i] = audio_out;
    end
endtask

// Capture 8-bit OSC3 readback ($1B) over N 1-MHz ticks — avoids signed-math
// ambiguity of filter-stage audio_out for raw waveform tests.
task automatic capture_osc3(ref logic [7:0] samples[], input int n);
    int i;
    logic [7:0] v;
    samples = new[n];
    for (i = 0; i < n; i++) begin
        wait_1m(1);
        sid_read(5'h1B, v);
        samples[i] = v;
    end
endtask

// Capture env3 readback ($1C) over N 1-MHz ticks.
task automatic capture_env3(ref logic [7:0] samples[], input int n);
    int i;
    logic [7:0] v;
    samples = new[n];
    for (i = 0; i < n; i++) begin
        wait_1m(1);
        sid_read(5'h1C, v);
        samples[i] = v;
    end
endtask

// Convenience: read env3 once at current time.
function automatic logic [7:0] env3_now();
    env3_now = dut.env3_out;
endfunction

function automatic logic [7:0] osc3_now();
    osc3_now = dut.osc3_out;
endfunction

// ---------------------------------------------------------------------------
// Signal analysis helpers
// ---------------------------------------------------------------------------

// Count rising zero-crossings for signed int samples (crossing from <=0 to >0).
function automatic int count_zero_xings_signed(input int unsigned samples[]);
    int i, n;
    int signed prev, cur;
    n = 0;
    if (samples.size() < 2) return 0;
    prev = $signed(samples[0]);
    for (i = 1; i < samples.size(); i++) begin
        cur = $signed(samples[i]);
        if (prev <= 0 && cur > 0) n++;
        prev = cur;
    end
    return n;
endfunction

// Count rising-edges above midpoint (0x80) for 8-bit unsigned samples.
function automatic int count_xings_8bit(input logic [7:0] samples[],
                                         input logic [7:0] threshold);
    int i, n;
    logic [7:0] prev, cur;
    n = 0;
    if (samples.size() < 2) return 0;
    prev = samples[0];
    for (i = 1; i < samples.size(); i++) begin
        cur = samples[i];
        if (prev <= threshold && cur > threshold) n++;
        prev = cur;
    end
    return n;
endfunction

// Min / max of 8-bit sample buffer.
function automatic logic [7:0] min_8bit(input logic [7:0] samples[]);
    int i;
    logic [7:0] m;
    m = 8'hFF;
    for (i = 0; i < samples.size(); i++)
        if (samples[i] < m) m = samples[i];
    return m;
endfunction

function automatic logic [7:0] max_8bit(input logic [7:0] samples[]);
    int i;
    logic [7:0] m;
    m = 8'h00;
    for (i = 0; i < samples.size(); i++)
        if (samples[i] > m) m = samples[i];
    return m;
endfunction

// Count of distinct 8-bit values in buffer (rough diversity metric).
function automatic int distinct_count_8bit(input logic [7:0] samples[]);
    int i, j, n;
    logic [7:0] seen [int];
    n = 0;
    for (i = 0; i < samples.size(); i++) begin
        if (!seen.exists(samples[i])) begin
            seen[samples[i]] = 1;
            n++;
        end
    end
    return n;
endfunction

// Fraction (0..100) of samples above threshold — duty-cycle measure.
function automatic int duty_pct_8bit(input logic [7:0] samples[],
                                      input logic [7:0] threshold);
    int i, above, n;
    n = samples.size();
    if (n == 0) return 0;
    above = 0;
    for (i = 0; i < n; i++)
        if (samples[i] > threshold) above++;
    return (above * 100) / n;
endfunction

// Absolute difference between two signed int sample buffers (sum of |a-b|).
function automatic longint abs_diff_signed(input int unsigned a[],
                                            input int unsigned b[]);
    int i, n;
    int signed va, vb, d;
    longint s;
    s = 0;
    n = (a.size() < b.size()) ? a.size() : b.size();
    for (i = 0; i < n; i++) begin
        va = $signed(a[i]);
        vb = $signed(b[i]);
        d  = (va > vb) ? (va - vb) : (vb - va);
        s += d;
    end
    return s;
endfunction

// ---------------------------------------------------------------------------
// Voice-tone convenience — set voice 1 for a steady tone.
//   freq_hi/lo: 16-bit frequency register
//   waveform:   control bits [7:4] (triangle=1, saw=2, pulse=4, noise=8)
//   pulse_width: 12-bit (ignored for non-pulse waveforms)
// Gate is set ON at end.
// ---------------------------------------------------------------------------
task automatic setup_voice1_tone(input [15:0] freq,
                                  input [3:0]  waveform,
                                  input [11:0] pw);
    sid_write(5'h00, freq[7:0]);
    sid_write(5'h01, freq[15:8]);
    sid_write(5'h02, pw[7:0]);
    sid_write(5'h03, {4'h0, pw[11:8]});
    sid_write(5'h05, 8'h00);           // attack=0, decay=0 (instant)
    sid_write(5'h06, 8'hF0);           // sustain=F, release=0
    sid_write(5'h18, 8'h0F);           // master vol = 15
    sid_write(5'h04, {waveform, 3'b000, 1'b1});  // waveform + gate
endtask

task automatic setup_voice3_tone(input [15:0] freq,
                                  input [3:0]  waveform,
                                  input [11:0] pw);
    sid_write(5'h0E, freq[7:0]);
    sid_write(5'h0F, freq[15:8]);
    sid_write(5'h10, pw[7:0]);
    sid_write(5'h11, {4'h0, pw[11:8]});
    sid_write(5'h13, 8'h00);
    sid_write(5'h14, 8'hF0);
    sid_write(5'h18, 8'h0F);
    sid_write(5'h12, {waveform, 3'b000, 1'b1});
endtask

task automatic gate_off_voice1();
    // Clear gate, preserve waveform bits
    logic [7:0] ctrl;
    sid_read(5'h04, ctrl);
    sid_write(5'h04, ctrl & 8'hFE);
endtask

task automatic silence_all();
    sid_write(5'h04, 8'h00);
    sid_write(5'h0B, 8'h00);
    sid_write(5'h12, 8'h00);
endtask
