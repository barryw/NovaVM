`timescale 1ns/1ps

module test_audio_led_meter;
    logic clk = 0;
    logic rst = 1;
    logic sample_en = 0;
    logic [15:0] sample_l = 16'd0;
    logic [15:0] sample_r = 16'd0;
    logic [7:0] leds;
    logic active;

    audio_led_meter #(
        .ACTIVE_HOLD_BITS(12),
        .ACTIVE_THRESHOLD(16'd64)
    ) dut (
        .clk       (clk),
        .rst       (rst),
        .sample_en (sample_en),
        .sample_l  (sample_l),
        .sample_r  (sample_r),
        .leds      (leds),
        .active    (active)
    );

    always #5 clk = ~clk;

    task automatic tick();
        @(posedge clk);
        #1;
    endtask

    task automatic push_sample(input logic signed [15:0] left, input logic signed [15:0] right);
        sample_l = left;
        sample_r = right;
        sample_en = 1'b1;
        tick();
        sample_en = 1'b0;
        sample_l = 16'd0;
        sample_r = 16'd0;
    endtask

    initial begin
        int counts [0:7];

        repeat (3) tick();
        rst = 0;
        tick();

        if (active || leds != 8'h00) begin
            $fatal(1, "meter should be inactive after reset");
        end

        push_sample(16'sd32, 16'sd0);
        tick();
        if (active) begin
            $fatal(1, "quiet sample below threshold should not activate LEDs");
        end

        push_sample(16'sd16384, -16'sd16384);
        if (!active) begin
            $fatal(1, "loud sample should activate LEDs");
        end

        for (int i = 0; i < 8; i++)
            counts[i] = 0;

        repeat (256) begin
            tick();
            for (int i = 0; i < 8; i++) begin
                if (leds[i])
                    counts[i]++;
            end
        end

        if (counts[0] < 240 || counts[1] < 240 || counts[2] < 240 || counts[3] < 240) begin
            $fatal(1, "lower LEDs should be strongly lit for a mid-scale sample: %0d %0d %0d %0d",
                   counts[0], counts[1], counts[2], counts[3]);
        end

        if (counts[6] != 0 || counts[7] != 0) begin
            $fatal(1, "top LEDs should remain off for a mid-scale sample: %0d %0d",
                   counts[6], counts[7]);
        end

        counts[7] = 0;
        push_sample(-16'sd32768, 16'sd0);
        repeat (256) begin
            tick();
            if (leds[7])
                counts[7]++;
        end

        if (counts[7] < 250) begin
            $fatal(1, "full-scale negative PCM should saturate the top LED: %0d",
                   counts[7]);
        end

        repeat (4200) tick();
        if (active) begin
            $fatal(1, "meter should return inactive after hold expires");
        end

        if (leds != 8'h00) begin
            $fatal(1, "inactive meter should gate LEDs off");
        end

        $display("PASS audio_led_meter");
        $finish;
    end
endmodule
