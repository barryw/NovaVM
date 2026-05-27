// HDMI start-offset regression.
//
// Nova's VGC output reaches fpga_top three pixel clocks after the timing core's
// logical visible pixel. For 720x480, fpga_top starts the HDMI core at x=858-3,
// y=524 so the first HDMI active pixel samples VGC active pixel 0 instead of
// the black pre-active pipeline tail.

`timescale 1ns/1ps

module test_hdmi_start_offset;

    localparam int FRAME_W = 858;
    localparam int FRAME_H = 525;
    localparam int VGC_LATENCY_PIXELS = 3;
    localparam int START_X = FRAME_W - VGC_LATENCY_PIXELS;
    localparam int START_Y = FRAME_H - 1;

    logic clk_pixel = 1'b0;
    logic clk_pixel_x5 = 1'b0;
    logic clk_audio = 1'b0;
    logic reset = 1'b1;

    always #50 clk_pixel = ~clk_pixel;
    always #10 clk_pixel_x5 = ~clk_pixel_x5;
    always #800 clk_audio = ~clk_audio;

    logic [23:0] rgb = 24'hFF0000;
    logic [1:0][15:0] audio_sample_word = '{16'd0, 16'd0};

    wire [2:0] tmds;
    wire       tmds_clock;
    wire [9:0] cx, cy;
    wire [9:0] frame_width, frame_height;
    wire [9:0] screen_width, screen_height;

    hdmi #(
        .VIDEO_ID_CODE(2),
        .VIDEO_REFRESH_RATE_MILLIHZ(59940),
        .START_X(START_X),
        .START_Y(START_Y),
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
    int test_num = 0;

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

    initial begin
        $display("=== HDMI start-offset regression ===");

        repeat(4) @(posedge clk_pixel);
        #1;
        check("reset loads x three pixels before 720x480 frame wrap", cx == START_X[9:0]);
        check("reset loads final frame line", cy == START_Y[9:0]);
        check("720x480 frame width", frame_width == 10'd858);
        check("720x480 frame height", frame_height == 10'd525);
        check("720x480 active width", screen_width == 10'd720);
        check("720x480 active height", screen_height == 10'd480);

        reset = 1'b0;
        repeat(VGC_LATENCY_PIXELS) @(posedge clk_pixel);
        #1;
        check("three clocks after reset release wraps to logical active origin",
              cx == 10'd0 && cy == 10'd0);
        check("active data period has not started on pre-active tail",
              dut.video_data_period == 1'b0);

        @(posedge clk_pixel);
        #1;
        check("active data period starts after the compensated origin",
              dut.video_data_period == 1'b1);

        $display("");
        $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count == 0) $display("ALL TESTS PASSED");
        else                 $display("SOME TESTS FAILED");
        $finish(fail_count);
    end

endmodule
