// VGC POR-determinism regression test.
//
// Verifies that the VGC's HDMI output signals (vid_hsync/vsync/de/r/g/b) and
// the upstream pipeline-delay registers in vgc_timing reach a known idle
// state on rst — even when every register starts with a uniquely-chosen
// (potentially non-zero) X value. This models the ECP5 power-on
// indeterminacy that, until 2026-04-26, produced different visual artifacts
// (50px black bar / 1-char shift / clean) across reflashes of the same
// bitstream because vid_hsync's first cycles after rst-release sometimes
// asserted sync at the wrong h_count, causing the HDMI sink to lock onto
// the wrong horizontal phase.
//
// Build with: verilator ... --x-assign unique

`timescale 1ns/1ps

module test_vgc_por_determinism;
    `include "vgc_tb.svh"

    // ---------------------------------------------------------------------------
    // Test
    // ---------------------------------------------------------------------------
    int passed = 0;
    int failed = 0;

    task report(input string name, input bit cond);
        if (cond) begin
            $display("  PASS [%0d] %s", passed + failed + 1, name);
            passed++;
        end else begin
            $display("  FAIL [%0d] %s", passed + failed + 1, name);
            failed++;
        end
    endtask

    initial begin
        bit hsync_glitch;
        bit vsync_glitch;
        bit any_x;
        bit reset_blank_nonblack;

        // Drive idle inputs.
        cpu_ce    = 0;
        cpu_addr  = 0;
        cpu_wdata = 0;
        cpu_we    = 0;
        cpu_re    = 0;
        key_valid = 0;
        key_data  = 0;
        dbg_addr  = 0;
        hsync_glitch = 0;
        vsync_glitch = 0;
        any_x = 0;
        reset_blank_nonblack = 0;

        // Assert rst. With --x-assign unique, every uninitialized reg in the
        // VGC starts at some non-zero value. The fix at vgc.sv output and
        // vgc_timing pipeline must force vid_* to known idle.
        rst = 1;

        // Sample at cycle 1 of rst — before the visible_d2 pipeline has had
        // time to settle to "1" from h_count's reset value. This is the cycle
        // the bug actually manifests in (random init bit feeds vid_de).
        @(posedge clk);
        $display("DIAG cycle 1 of rst: vid_hsync=%b vid_vsync=%b vid_de=%b vid_r=%h",
                 vid_hsync, vid_vsync, vid_de, vid_r);

        // Wait some cycles and observe vid_* during rst.
        repeat (50) @(posedge clk);
        $display("DIAG cycle 51 of rst: vid_hsync=%b vid_vsync=%b vid_de=%b vid_r=%h",
                 vid_hsync, vid_vsync, vid_de, vid_r);

        // During rst, output must be at idle: HSYNC and VSYNC HIGH (we drive
        // negative-polarity sync), DE LOW (blanking), color = 0.
        report("vid_hsync == 1 during rst", vid_hsync === 1'b1);
        report("vid_vsync == 1 during rst", vid_vsync === 1'b1);
        report("vid_de    == 0 during rst", vid_de    === 1'b0);
        report("vid_r == 0 during rst",     vid_r === 4'h0);
        report("vid_g == 0 during rst",     vid_g === 4'h0);
        report("vid_b == 0 during rst",     vid_b === 4'h0);

        // Release rst. The very next FEW cycles are the danger zone — they
        // are what produced the boot-time artifact pre-fix because the d1/d2
        // pipeline regs were uninitialized.
        @(posedge clk);
        rst = 0;

        // For the first 200 cycles (well below the H_FRONT=656 sync edge),
        // vid_hsync MUST stay HIGH. If it drops LOW anywhere in here, that's
        // a phantom sync pulse — exactly the boot-artifact bug.
        for (int i = 0; i < 200; i++) begin
            @(posedge clk);
            if (dut.reset_display_blank && vid_de &&
                (vid_r !== 4'h0 || vid_g !== 4'h0 || vid_b !== 4'h0))
                reset_blank_nonblack = 1;
            if (vid_hsync === 1'b0) hsync_glitch = 1;
        end
        report("reset scrub displays black pixels only",
               !reset_blank_nonblack);
        report("no phantom hsync pulse in first 200 cycles after rst-release",
               !hsync_glitch);

        // VSYNC: same deal. V_ACTIVE=480 lines and we're nowhere near the
        // bottom of frame yet.
        for (int i = 0; i < 200; i++) begin
            @(posedge clk);
            if (vid_vsync === 1'b0) vsync_glitch = 1;
        end
        report("no phantom vsync pulse in first 200 cycles after rst-release",
               !vsync_glitch);

        // Let the timing/compositor pipeline fill with live, non-idle values,
        // then assert rst again. This models a BASIC/bridge cold_start after
        // graphics tests have touched the VGC while the HDMI sink remains
        // connected. All delayed timing state must clear on the first reset
        // clock, not only on FPGA power-on initialization.
        repeat (257) @(posedge clk);
        rst = 1;
        @(posedge clk);
        report("runtime rst clears h_count_d1",
               dut.timing_inst.h_count_d1 === 10'd0);
        report("runtime rst clears h_count_d2",
               dut.timing_inst.h_count_d2 === 10'd0);
        report("runtime rst clears visible_d1",
               dut.timing_inst.visible_d1 === 1'b0);
        report("runtime rst clears visible_d2",
               dut.timing_inst.visible_d2 === 1'b0);
        report("runtime rst clears in_text_area_d1",
               dut.timing_inst.in_text_area_d1 === 1'b0);
        report("runtime rst clears in_text_area_d2",
               dut.timing_inst.in_text_area_d2 === 1'b0);
        report("runtime rst drives vid_hsync idle",
               vid_hsync === 1'b1);
        report("runtime rst drives vid_vsync idle",
               vid_vsync === 1'b1);
        report("runtime rst drives vid_de blank",
               vid_de === 1'b0);

        repeat (10) @(posedge clk);
        hsync_glitch = 0;
        vsync_glitch = 0;
        reset_blank_nonblack = 0;
        @(posedge clk);
        rst = 0;
        for (int i = 0; i < 200; i++) begin
            @(posedge clk);
            if (dut.reset_display_blank && vid_de &&
                (vid_r !== 4'h0 || vid_g !== 4'h0 || vid_b !== 4'h0))
                reset_blank_nonblack = 1;
            if (vid_hsync === 1'b0) hsync_glitch = 1;
            if (vid_vsync === 1'b0) vsync_glitch = 1;
        end
        report("runtime reset scrub displays black pixels only",
               !reset_blank_nonblack);
        report("no phantom hsync pulse after runtime rst-release",
               !hsync_glitch);
        report("no phantom vsync pulse after runtime rst-release",
               !vsync_glitch);

        // Scrubbers complete at arbitrary pixel positions. The display blank
        // latch must stay asserted until the next frame boundary so hardware
        // never emits a half-reset/half-live frame.
        force dut.reset_clear_phase = 3'd0;
        force dut.tile_reset_busy = 1'b0;
        force dut.h_count = 10'd123;
        force dut.v_count = 10'd45;
        @(posedge clk); #1;
        report("runtime reset blank holds after scrub idle mid-frame",
               dut.reset_display_blank === 1'b1);
        force dut.h_count = 10'd0;
        force dut.v_count = 10'd0;
        @(posedge clk); #1;
        report("runtime reset blank releases at frame start",
               dut.reset_display_blank === 1'b0);
        release dut.h_count;
        release dut.v_count;
        release dut.tile_reset_busy;
        release dut.reset_clear_phase;

        // None of the output signals should ever be X.
        if (^vid_hsync === 1'bx) any_x = 1;
        if (^vid_vsync === 1'bx) any_x = 1;
        if (^vid_de    === 1'bx) any_x = 1;
        if (^vid_r     === 1'bx) any_x = 1;
        if (^vid_g     === 1'bx) any_x = 1;
        if (^vid_b     === 1'bx) any_x = 1;
        report("no X-state on output signals", !any_x);

        $display("");
        $display("=== VGC POR determinism: %0d passed, %0d failed ===", passed, failed);
        if (failed == 0) $display("ALL TESTS PASSED");
        else             $display("FAILURES PRESENT");
        $finish;
    end
endmodule
