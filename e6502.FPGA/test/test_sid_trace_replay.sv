// Replay a SID register trace captured by e6502.Tools sidtrace into the
// sid_chip RTL and print per-frame raw SID plus HDMI PCM metrics.
//
// Trace format, one write per line:
//   <frame> <chip> <sid-register-hex> <data-hex>
//
// Example:
//   make -C e6502.FPGA/test test_sid_trace_replay \
//     TRACE_FILE=/private/tmp/gryzor-nova-sid.rtltrace SID_MODE=8580

`timescale 1ns/1ps

module test_sid_trace_replay;
    `include "sid_tb.svh"

    string trace_path;
    string metrics_path;
    string mode_arg;
    int frame_ticks;
    int hdmi_sample_div;
    int hdmi_sample_phase;
    int metrics_fd;

    logic sample_en = 1'b0;
    logic [1:0][15:0] audio_sample_word;

    sid_hdmi_audio hdmi_audio (
        .clk              (clk),
        .rst              (rst),
        .sample_en        (sample_en),
        .sid_audio_l      (audio_out),
        .sid_audio_r      (audio_out),
        .audio_sample_word(audio_sample_word)
    );

    function automatic longint abs_long(input longint value);
        abs_long = value < 0 ? -value : value;
    endfunction

    function automatic int pcm16(input logic [15:0] value);
        logic signed [15:0] signed_value;
        begin
            signed_value = value;
            pcm16 = signed_value;
        end
    endfunction

    task automatic maybe_sample_hdmi(output logic did_sample);
        begin
            did_sample = 1'b0;
            if (hdmi_sample_phase == 0) begin
                sample_en = 1'b1;
                @(posedge clk);
                sample_en = 1'b0;
                @(posedge clk);
                did_sample = 1'b1;
            end

            hdmi_sample_phase++;
            if (hdmi_sample_phase >= hdmi_sample_div)
                hdmi_sample_phase = 0;
        end
    endtask

    task automatic apply_sid_write(input int chip, input int regno, input int value);
        begin
            if (chip == 1) begin
                sid_write(regno[4:0], value[7:0]);
            end else begin
                // The current SID test scaffold instantiates one SID. Keep the
                // trace format dual-SID capable, but ignore chip 2 here.
                $display("WARN: ignoring SID%0d write reg=$%02X data=$%02X",
                         chip, regno[7:0], value[7:0]);
            end
        end
    endtask

    task automatic capture_frame_metrics(input int frame_no);
        int i;
        int signed sample;
        int signed pcm_sample;
        int pcm_sample_count;
        int pcm_clip_pos;
        int pcm_clip_neg;
        longint min_sample;
        longint max_sample;
        longint sum_sample;
        longint sum_abs;
        longint peak;
        longint min_pcm_sample;
        longint max_pcm_sample;
        longint sum_pcm_sample;
        longint sum_pcm_abs;
        longint pcm_peak;
        logic did_hdmi_sample;
        begin
            min_sample = 64'sh7fff_ffff_ffff_ffff;
            max_sample = -64'sh7fff_ffff_ffff_ffff;
            sum_sample = 0;
            sum_abs = 0;
            min_pcm_sample = 64'sh7fff_ffff_ffff_ffff;
            max_pcm_sample = -64'sh7fff_ffff_ffff_ffff;
            sum_pcm_sample = 0;
            sum_pcm_abs = 0;
            pcm_sample_count = 0;
            pcm_clip_pos = 0;
            pcm_clip_neg = 0;

            for (i = 0; i < frame_ticks; i++) begin
                wait_1m(1);
                @(posedge clk);
                sample = $signed(audio_out);
                if (sample < min_sample)
                    min_sample = sample;
                if (sample > max_sample)
                    max_sample = sample;
                sum_sample += sample;
                sum_abs += abs_long(sample);

                maybe_sample_hdmi(did_hdmi_sample);
                if (did_hdmi_sample) begin
                    pcm_sample = pcm16(audio_sample_word[0]);
                    if (pcm_sample < min_pcm_sample)
                        min_pcm_sample = pcm_sample;
                    if (pcm_sample > max_pcm_sample)
                        max_pcm_sample = pcm_sample;
                    if (audio_sample_word[0] == 16'h7fff)
                        pcm_clip_pos++;
                    if (audio_sample_word[0] == 16'h8000)
                        pcm_clip_neg++;
                    sum_pcm_sample += pcm_sample;
                    sum_pcm_abs += abs_long(pcm_sample);
                    pcm_sample_count++;
                end
            end

            peak = abs_long(min_sample) > abs_long(max_sample)
                 ? abs_long(min_sample)
                 : abs_long(max_sample);
            if (pcm_sample_count == 0) begin
                min_pcm_sample = 0;
                max_pcm_sample = 0;
            end
            pcm_peak = abs_long(min_pcm_sample) > abs_long(max_pcm_sample)
                     ? abs_long(min_pcm_sample)
                     : abs_long(max_pcm_sample);
            $display("SIDTRACE frame=%0d mode=%s raw_peak=%0d raw_range=%0d raw_mean=%0d raw_mean_abs=%0d pcm_peak=%0d pcm_range=%0d pcm_mean=%0d pcm_mean_abs=%0d pcm_clip=%0d/%0d pcm_samples=%0d",
                     frame_no, mode_arg, peak, max_sample - min_sample,
                     sum_sample / frame_ticks, sum_abs / frame_ticks,
                     pcm_peak, max_pcm_sample - min_pcm_sample,
                     pcm_sample_count == 0 ? 0 : sum_pcm_sample / pcm_sample_count,
                     pcm_sample_count == 0 ? 0 : sum_pcm_abs / pcm_sample_count,
                     pcm_clip_pos, pcm_clip_neg, pcm_sample_count);
            if (metrics_fd != 0) begin
                $fdisplay(metrics_fd, "%0d,%s,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d,%0d",
                          frame_no, mode_arg, peak, max_sample - min_sample,
                          sum_sample / frame_ticks, sum_abs / frame_ticks,
                          pcm_peak, max_pcm_sample - min_pcm_sample,
                          pcm_sample_count == 0 ? 0 : sum_pcm_sample / pcm_sample_count,
                          pcm_sample_count == 0 ? 0 : sum_pcm_abs / pcm_sample_count,
                          pcm_clip_pos, pcm_clip_neg, pcm_sample_count);
            end
        end
    endtask

    initial begin
        int fd;
        int parsed;
        int frame_no;
        int chip;
        int regno;
        int value;
        int current_frame;

        if (!$value$plusargs("TRACE=%s", trace_path))
            $fatal(1, "Missing +TRACE=<sidtrace.rtltrace>");

        mode_arg = "6581";
        void'($value$plusargs("MODE=%s", mode_arg));
        frame_ticks = 20000;
        void'($value$plusargs("FRAME_TICKS=%d", frame_ticks));
        hdmi_sample_div = 21;
        void'($value$plusargs("HDMI_SAMPLE_DIV=%d", hdmi_sample_div));
        if (hdmi_sample_div <= 0)
            $fatal(1, "HDMI_SAMPLE_DIV must be greater than zero");
        hdmi_sample_phase = 0;

        $display("=== SID trace RTL replay ===");
        $display("trace=%s mode=%s frame_ticks=%0d hdmi_sample_div=%0d",
                 trace_path, mode_arg, frame_ticks, hdmi_sample_div);
        metrics_fd = 0;
        if ($value$plusargs("METRICS=%s", metrics_path)) begin
            metrics_fd = $fopen(metrics_path, "w");
            if (metrics_fd == 0)
                $fatal(1, "Could not open metrics file: %s", metrics_path);
            $fdisplay(metrics_fd, "frame,mode,raw_peak,raw_ac_range,raw_mean,raw_mean_abs,pcm_peak,pcm_ac_range,pcm_mean,pcm_mean_abs,pcm_clip_pos,pcm_clip_neg,pcm_samples");
            $display("metrics=%s", metrics_path);
        end

        sample_en = 1'b0;
        do_reset();
        mode = mode_arg == "8580";

        fd = $fopen(trace_path, "r");
        if (fd == 0)
            $fatal(1, "Could not open trace file: %s", trace_path);

        current_frame = -2147483648;
        while (!$feof(fd)) begin
            parsed = $fscanf(fd, "%d %d %h %h\n", frame_no, chip, regno, value);
            if (parsed != 4)
                continue;

            if (current_frame == -2147483648)
                current_frame = frame_no;

            while (frame_no != current_frame) begin
                capture_frame_metrics(current_frame);
                current_frame++;
            end

            apply_sid_write(chip, regno, value);
        end

        if (current_frame != -2147483648)
            capture_frame_metrics(current_frame);

        $fclose(fd);
        if (metrics_fd != 0)
            $fclose(metrics_fd);
        $display("SID trace replay complete.");
        $finish;
    end
endmodule
