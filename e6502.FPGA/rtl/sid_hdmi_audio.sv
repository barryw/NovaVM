// SID-to-HDMI audio conditioner.
//
// The SID mixer output includes a large DC bias. Analog output paths usually
// remove that downstream, but HDMI PCM carries the sample value directly. This
// block high-passes the SID samples at the HDMI audio clock and emits signed
// 16-bit PCM words for hdl-util/hdmi.

module sid_hdmi_audio #(
    parameter int PCM_SHIFT = 0,
    parameter int PCM_GAIN = 3
) (
    input  logic              clk,
    input  logic              rst,
    input  logic              sample_en,
    input  logic signed [17:0] sid_audio_l,
    input  logic signed [17:0] sid_audio_r,
    output logic [1:0][15:0]  audio_sample_word
);
    logic signed [17:0] sid_audio_l_prev = 18'sd0;
    logic signed [17:0] sid_audio_r_prev = 18'sd0;
    logic signed [25:0] sid_audio_l_hp = 26'sd0;
    logic signed [25:0] sid_audio_r_hp = 26'sd0;

    wire signed [18:0] sid_audio_l_delta = {sid_audio_l[17], sid_audio_l} -
                                           {sid_audio_l_prev[17], sid_audio_l_prev};
    wire signed [18:0] sid_audio_r_delta = {sid_audio_r[17], sid_audio_r} -
                                           {sid_audio_r_prev[17], sid_audio_r_prev};

    function automatic logic signed [25:0] dc_block_next(
        input logic signed [25:0] value,
        input logic signed [18:0] delta
    );
        logic signed [25:0] leak;
        begin
            leak = value >>> 8;
            if (value > 26'sd0 && leak == 26'sd0)
                leak = 26'sd1;
            dc_block_next = value - leak + {{7{delta[18]}}, delta};
        end
    endfunction

    always_ff @(posedge clk) begin
        if (rst) begin
            sid_audio_l_prev <= 18'sd0;
            sid_audio_r_prev <= 18'sd0;
            sid_audio_l_hp <= 26'sd0;
            sid_audio_r_hp <= 26'sd0;
        end else if (sample_en) begin
            sid_audio_l_prev <= sid_audio_l;
            sid_audio_r_prev <= sid_audio_r;
            sid_audio_l_hp <= dc_block_next(sid_audio_l_hp, sid_audio_l_delta);
            sid_audio_r_hp <= dc_block_next(sid_audio_r_hp, sid_audio_r_delta);
        end
    end

    function automatic logic [15:0] to_pcm16(input logic signed [25:0] sample);
        logic signed [31:0] widened;
        logic signed [31:0] scaled;
        begin
            widened = {{6{sample[25]}}, sample};
            scaled = (widened * PCM_GAIN) >>> PCM_SHIFT;
            if (scaled > 32'sd32767)
                to_pcm16 = 16'h7fff;
            else if (scaled < -32'sd32768)
                to_pcm16 = 16'h8000;
            else
                to_pcm16 = scaled[15:0];
        end
    endfunction

    assign audio_sample_word[0] = to_pcm16(sid_audio_l_hp);
    assign audio_sample_word[1] = to_pcm16(sid_audio_r_hp);
endmodule
