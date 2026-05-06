// HDMI video/audio packet smoke test.
//
// Verifies the vendored hdl-util HDMI core emits true-HDMI data-island packets
// for 720x480p with 48 kHz stereo PCM enabled. This catches accidental
// regressions back to DVI-only output.

`timescale 1ns/1ps

module test_hdmi_audio;

    logic clk_pixel = 1'b0;
    logic clk_pixel_x5 = 1'b0;
    logic clk_audio = 1'b0;
    logic reset = 1'b1;

    always #50 clk_pixel = ~clk_pixel;
    always #10 clk_pixel_x5 = ~clk_pixel_x5;
    always #800 clk_audio = ~clk_audio;

    logic [23:0] rgb = 24'h204080;
    logic [1:0][15:0] audio_sample_word;

    wire [2:0] tmds;
    wire       tmds_clock;
    wire [9:0] cx, cy;
    wire [9:0] frame_width, frame_height;
    wire [9:0] screen_width, screen_height;

    hdmi #(
        .VIDEO_ID_CODE(2),
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
        .AUDIO_RATE(48000),
        .AUDIO_BIT_WIDTH(16),
        .VENDOR_NAME({"Nova", 32'd0}),
        .PRODUCT_DESCRIPTION({"NovaVM", 80'd0}),
        .SOURCE_DEVICE_INFORMATION(8'h09)
    ) dut (
        .clk_pixel_x5(clk_pixel_x5),
        .clk_pixel(clk_pixel),
        .clk_audio(clk_audio),
        .reset(reset),
        .rgb(rgb),
        .audio_sample_word(audio_sample_word),
        .tmds(tmds),
        .tmds_clock(tmds_clock),
        .cx(cx),
        .cy(cy),
        .frame_width(frame_width),
        .frame_height(frame_height),
        .screen_width(screen_width),
        .screen_height(screen_height)
    );

    int pass_count = 0;
    int fail_count = 0;
    int test_num   = 0;
    int data_island_cycles = 0;
    int audio_sample_packets = 0;
    int audio_clock_packets = 0;
    int avi_packets = 0;
    int spd_packets = 0;

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

    always_ff @(posedge clk_audio) begin
        audio_sample_word[0] <= audio_sample_word[0] + 16'd257;
        audio_sample_word[1] <= audio_sample_word[1] - 16'd129;
    end

    always_ff @(posedge clk_pixel) begin
        if (!reset) begin
            if (dut.true_hdmi_output.data_island_period)
                data_island_cycles++;

            if (dut.true_hdmi_output.packet_enable) begin
                case (dut.true_hdmi_output.header[7:0])
                    8'h01: audio_clock_packets++;
                    8'h02: audio_sample_packets++;
                    8'h82: avi_packets++;
                    8'h83: spd_packets++;
                    default: begin end
                endcase
            end
        end
    end

    initial begin
        audio_sample_word[0] = 16'd0;
        audio_sample_word[1] = 16'd0;

        $display("=== HDMI audio packet smoke test ===");

        repeat(20) @(posedge clk_pixel);
        reset = 1'b0;

        repeat(858 * 525 * 2) @(posedge clk_pixel);

        check("720x480 frame width", frame_width == 10'd858);
        check("720x480 frame height", frame_height == 10'd525);
        check("720x480 active width", screen_width == 10'd720);
        check("720x480 active height", screen_height == 10'd480);
        check("data islands emitted", data_island_cycles > 0);
        check("audio clock regeneration packets emitted", audio_clock_packets > 0);
        check("audio sample packets emitted", audio_sample_packets > 0);
        check("AVI InfoFrame packets emitted", avi_packets > 0);
        check("Source Product Description packets emitted", spd_packets > 0);

        $display("");
        $display("Packet counts: clock=%0d sample=%0d avi=%0d spd=%0d data_cycles=%0d",
                 audio_clock_packets, audio_sample_packets, avi_packets,
                 spd_packets, data_island_cycles);
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
