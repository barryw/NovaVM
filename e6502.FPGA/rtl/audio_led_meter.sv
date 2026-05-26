// Audio-reactive LED meter for the ULX3S user LEDs.
//
// This intentionally keys off the final PCM stream headed to HDMI, so it
// follows SID, WTS/MIDI, and hosted PCM without any application involvement.
module audio_led_meter #(
    parameter int ACTIVE_HOLD_BITS = 26,
    parameter logic [15:0] ACTIVE_THRESHOLD = 16'd512
) (
    input  logic        clk,
    input  logic        rst,
    input  logic        sample_en,
    input  logic [15:0] sample_l,
    input  logic [15:0] sample_r,
    output logic [7:0]  leds,
    output logic        active
);
    logic [15:0] env = 16'd0;
    logic [15:0] env_next;
    logic [7:0]  pwm_phase = 8'd0;
    logic [7:0]  duty0 = 8'd0;
    logic [7:0]  duty1 = 8'd0;
    logic [7:0]  duty2 = 8'd0;
    logic [7:0]  duty3 = 8'd0;
    logic [7:0]  duty4 = 8'd0;
    logic [7:0]  duty5 = 8'd0;
    logic [7:0]  duty6 = 8'd0;
    logic [7:0]  duty7 = 8'd0;
    logic [ACTIVE_HOLD_BITS-1:0] active_hold = '0;

    wire [15:0] abs_l = pcm_abs(sample_l);
    wire [15:0] abs_r = pcm_abs(sample_r);
    wire [15:0] sample_peak = (abs_l > abs_r) ? abs_l : abs_r;
    wire [15:0] decay_step = (env >> 12) + 16'd2;

    assign active = active_hold != '0;

    always_comb begin
        env_next = env;
        if (sample_en) begin
            if (sample_peak > env) begin
                env_next = sample_peak;
            end else if (env > decay_step) begin
                env_next = env - decay_step;
            end else begin
                env_next = 16'd0;
            end
        end
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            env <= 16'd0;
            pwm_phase <= 8'd0;
            duty0 <= 8'd0;
            duty1 <= 8'd0;
            duty2 <= 8'd0;
            duty3 <= 8'd0;
            duty4 <= 8'd0;
            duty5 <= 8'd0;
            duty6 <= 8'd0;
            duty7 <= 8'd0;
            active_hold <= '0;
        end else begin
            pwm_phase <= pwm_phase + 8'd1;
            env <= env_next;

            if (active_hold != '0)
                active_hold <= active_hold - 1'b1;

            if (sample_en) begin
                if (sample_peak > ACTIVE_THRESHOLD)
                    active_hold <= {ACTIVE_HOLD_BITS{1'b1}};

                duty0 <= led_duty(led_level(env_next), 3'd0);
                duty1 <= led_duty(led_level(env_next), 3'd1);
                duty2 <= led_duty(led_level(env_next), 3'd2);
                duty3 <= led_duty(led_level(env_next), 3'd3);
                duty4 <= led_duty(led_level(env_next), 3'd4);
                duty5 <= led_duty(led_level(env_next), 3'd5);
                duty6 <= led_duty(led_level(env_next), 3'd6);
                duty7 <= led_duty(led_level(env_next), 3'd7);
            end
        end
    end

    assign leds[0] = active && (pwm_phase < duty0);
    assign leds[1] = active && (pwm_phase < duty1);
    assign leds[2] = active && (pwm_phase < duty2);
    assign leds[3] = active && (pwm_phase < duty3);
    assign leds[4] = active && (pwm_phase < duty4);
    assign leds[5] = active && (pwm_phase < duty5);
    assign leds[6] = active && (pwm_phase < duty6);
    assign leds[7] = active && (pwm_phase < duty7);

    function automatic logic [15:0] pcm_abs(input logic [15:0] sample);
        begin
            pcm_abs = sample[15] ? (~sample + 16'd1) : sample;
        end
    endfunction

    function automatic logic [7:0] led_level(input logic [15:0] value);
        begin
            led_level = value[15] ? 8'hff : value[14:7];
        end
    endfunction

    function automatic logic [7:0] led_duty(
        input logic [7:0] level,
        input logic [2:0] index
    );
        logic [7:0] base;
        logic [7:0] partial;
        begin
            base = {index, 5'b00000};

            if (level <= base) begin
                led_duty = 8'd0;
            end else if (index == 3'd7) begin
                partial = level - base;
                led_duty = (level == 8'hff) ? 8'hff : {partial[4:0], 3'b000};
            end else if (level >= (base + 8'd32)) begin
                led_duty = 8'hff;
            end else begin
                partial = level - base;
                led_duty = {partial[4:0], 3'b000};
            end
        end
    endfunction
endmodule
