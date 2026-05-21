// Nova wavetable synthesizer chip.
//
// Hardware-facing implementation of the Avalonia WTS register ABI:
//   $A140-$A17F  8 voices x 8 registers
//   $A180-$A187  global/effects/status registers
//   $A1A0-$A1DF  instrument enumeration buffer
//
// This block is deliberately self-contained: register writes drive the same
// note/velocity/instrument/volume/pan/pitch-bend controls as Avalonia's
// WavetableSynth. The current FPGA slice provides a resident 8-instrument
// wavetable bank so the audio path is testable before the ESP-side SF2 sample
// loader starts feeding external sample regions.

module wts_chip (
    input  logic              clk,
    input  logic              rst,

    input  logic              cpu_ce,
    input  logic [15:0]       cpu_addr,
    input  logic [7:0]        cpu_wdata,
    input  logic              cpu_we,
    output logic [7:0]        cpu_rdata,

    input  logic              dbg_we,
    input  logic [7:0]        dbg_addr,
    input  logic [7:0]        dbg_wdata,
    input  logic [7:0]        dbg_raddr,
    output logic [7:0]        dbg_rdata,

    input  logic              host_event_we,
    input  logic [7:0]        host_event_data,
    output logic              host_event_ready,

    output logic [24:0]       sdram_addrB,
    output logic [7:0]        sdram_dinB,
    output logic              sdram_weB,
    output logic              sdram_oeB,
    input  logic [7:0]        sdram_doutB,

    output logic signed [17:0] audio_l,
    output logic signed [17:0] audio_r
);

    localparam logic [15:0] WTS_BASE = 16'hA140;
    localparam logic [15:0] WTS_END  = 16'hA1FF;

    localparam logic [7:0] CMD_ALL_NOTES_OFF  = 8'h01;
    localparam logic [7:0] CMD_RESET_EFFECTS  = 8'h02;
    localparam logic [7:0] CMD_EVENT_RESET    = 8'h03;
    localparam logic [7:0] CMD_EVENT_START    = 8'h04;
    localparam logic [7:0] CMD_EVENT_STOP     = 8'h05;
    localparam logic [7:0] DEFAULT_REVERB     = 8'd80;
    localparam logic [7:0] DEFAULT_CHORUS     = 8'd40;
    localparam logic [7:0] DEFAULT_MASTER     = 8'd255;
    localparam logic [7:0] RESIDENT_INST_COUNT = 8'd8;
    localparam logic [15:0] ENV_ATTACK_STEP   = 16'h0300;
    localparam logic [15:0] ENV_DECAY_STEP    = 16'h0020;
    localparam logic [15:0] ENV_RELEASE_STEP  = 16'h0004;
    localparam logic [15:0] ENV_SUSTAIN_LEVEL = 16'hA000;
    localparam int EVENT_FIFO_DEPTH = 1024;
    localparam int EVENT_FIFO_INDEX_BITS = 10;
    localparam int EVENT_FIFO_COUNT_BITS = 11;
    localparam int EVENT_RECORD_BYTES = 6;
    localparam logic [EVENT_FIFO_COUNT_BITS-1:0] EVENT_FIFO_DEPTH_COUNT =
        EVENT_FIFO_DEPTH;

    typedef enum logic [2:0] {
        ENV_OFF,
        ENV_ATTACK,
        ENV_DECAY,
        ENV_SUSTAIN,
        ENV_RELEASE
    } env_stage_t;

    typedef enum logic [4:0] {
        MIX_IDLE,
        MIX_VOICE,
        MIX_SAMPLE_READ,
        MIX_SAMPLE_CAPTURE,
        MIX_SAMPLE_READ_NEXT,
        MIX_SAMPLE_DELTA,
        MIX_SAMPLE_INTERPOLATE,
        MIX_SAMPLE_PHASE,
        MIX_GAIN_ENV,
        MIX_GAIN_VELOCITY,
        MIX_GAIN_VOLUME,
        MIX_GAIN_PAN_L,
        MIX_GAIN_PAN_R,
        MIX_LATCH_MASTER_L,
        MIX_LATCH_MASTER_R,
        MIX_NOTE_PREP,
        MIX_NOTE_APPLY,
        MIX_BEND_PREP,
        MIX_BEND_APPLY
    } mix_state_t;

    logic [7:0]  note [0:7];
    logic [7:0]  velocity [0:7];
    logic [7:0]  instrument [0:7];
    logic [7:0]  volume [0:7];
    logic [7:0]  pan [0:7];
    logic [15:0] pitch_bend [0:7];
    logic [31:0] phase [0:7];
    logic [31:0] phase_step [0:7];
    logic [24:0] sample_start [0:7];
    logic [24:0] sample_end [0:7];
    logic [24:0] sample_loop_start [0:7];
    logic [24:0] sample_loop_end [0:7];
    logic [31:0] sample_phase_step [0:7];
    logic        sample_mode [0:7];
    logic        sample_loop [0:7];
    logic [15:0] env_level [0:7];
    logic [15:0] env_attack_step [0:7];
    logic [15:0] env_decay_step [0:7];
    logic [15:0] env_sustain_level [0:7];
    logic [15:0] env_release_step [0:7];
    env_stage_t  env_stage [0:7];
    logic        active [0:7];
    logic        releasing [0:7];
    logic [7:0]  note_on_pending;
    logic [7:0]  note_off_pending;
    logic [7:0]  bend_pending;
    logic [7:0]  pending_note [0:7];
    logic [15:0] pending_bend [0:7];

    logic [7:0] reverb_level;
    logic [7:0] chorus_level;
    logic [7:0] master_volume;
    logic [7:0] enum_index;

    logic [31:0] sample_acc;
    logic        sample_en;
    mix_state_t  mix_state;
    logic [2:0]  mix_voice;
    logic signed [31:0] mix_acc_l;
    logic signed [31:0] mix_acc_r;
    logic [2:0]  param_voice;
    logic [2:0]  env_param_index;
    logic [24:0] sample_read_addr;
    logic [3:0]  sample_hold_cnt;
    logic signed [8:0] sample_interp_base;
    logic signed [8:0] sample_interp_next;
    logic signed [8:0] sample_interp_value;
    logic [7:0]  sample_interp_frac;
    logic [31:0] sample_next_phase;
    logic [31:0] sample_loop_start_phase;
    logic [31:0] sample_loop_end_phase;
    logic [31:0] sample_end_phase;
    logic signed [31:0] mix_amp;
    logic signed [31:0] mix_left;
    logic signed [31:0] latch_scaled_l;
    logic [2:0]  service_voice;
    logic [7:0]  service_note;
    logic [15:0] service_bend;
    logic [31:0] service_base_step;
    logic signed [17:0] mul_a;
    logic signed [17:0] mul_b;
    logic        cpu_write_pending;
    logic [7:0]  cpu_write_off;
    logic [7:0]  cpu_write_data;
    logic        dbg_write_pending;
    logic [7:0]  dbg_write_off;
    logic [7:0]  dbg_write_data;

    (* ram_style = "block" *) logic [47:0] event_fifo [0:EVENT_FIFO_DEPTH-1];
    logic [EVENT_FIFO_INDEX_BITS-1:0] event_wr_ptr;
    logic [EVENT_FIFO_INDEX_BITS-1:0] event_rd_ptr;
    logic [EVENT_FIFO_COUNT_BITS-1:0] event_wr_count;
    logic [EVENT_FIFO_COUNT_BITS-1:0] event_rd_count;
    wire [EVENT_FIFO_COUNT_BITS-1:0] event_fifo_count =
        event_wr_count - event_rd_count;
    wire [EVENT_FIFO_COUNT_BITS-1:0] event_fifo_space =
        EVENT_FIFO_DEPTH_COUNT - event_fifo_count;
    wire event_fifo_empty = (event_fifo_count == 0);
    wire event_fifo_full =
        (event_fifo_count == EVENT_FIFO_DEPTH_COUNT);
    logic [2:0]  event_byte_index;
    logic [31:0] event_build_time;
    logic [7:0]  event_build_off;
    logic [47:0] event_head;
    logic        event_head_valid;
    logic        event_read_pending;
    logic [EVENT_FIFO_INDEX_BITS-1:0] event_read_addr;
    logic        event_running;
    logic        event_parser_error;
    logic [31:0] event_frame;
    wire [31:0] event_head_time = event_head[47:16];
    wire [7:0]  event_head_off  = event_head[15:8];
    wire [7:0]  event_head_data = event_head[7:0];
    wire        event_due =
        event_running && event_head_valid && (event_head_time <= event_frame);
    wire        command_write_pending =
        (dbg_write_pending && dbg_write_off == 8'h45) ||
        (cpu_write_pending && cpu_write_off == 8'h45);

    wire cpu_sel = (cpu_addr >= WTS_BASE) && (cpu_addr <= WTS_END);
    wire [7:0] cpu_off = cpu_addr[7:0] - WTS_BASE[7:0];
    wire [7:0] dbg_off = dbg_addr;

    // 48 kHz sample cadence from the 27 MHz pixel clock domain used by HDMI.
    localparam [31:0] SAMPLE_STEP = 32'd48000;
    localparam [31:0] SAMPLE_RATE = 32'd27000000;
    wire [32:0] sample_next = {1'b0, sample_acc} + {1'b0, SAMPLE_STEP};
    wire signed [35:0] mul_product = mul_a * mul_b;

    assign sdram_addrB = sample_read_addr;
    assign sdram_dinB  = 8'd0;
    assign sdram_weB   = 1'b0;
    assign sdram_oeB   = (mix_state == MIX_SAMPLE_READ) ||
                          (mix_state == MIX_SAMPLE_READ_NEXT);
    assign host_event_ready = !event_fifo_full && !event_parser_error;

    function automatic logic [31:0] midi_phase_step(input logic [6:0] midi_note);
        begin
            unique case (midi_note)
                7'd000: midi_phase_step = 32'd0;
                7'd001: midi_phase_step = 32'd775059;
                7'd002: midi_phase_step = 32'd821146;
                7'd003: midi_phase_step = 32'd869974;
                7'd004: midi_phase_step = 32'd921705;
                7'd005: midi_phase_step = 32'd976513;
                7'd006: midi_phase_step = 32'd1034579;
                7'd007: midi_phase_step = 32'd1096099;
                7'd008: midi_phase_step = 32'd1161276;
                7'd009: midi_phase_step = 32'd1230329;
                7'd010: midi_phase_step = 32'd1303488;
                7'd011: midi_phase_step = 32'd1380998;
                7'd012: midi_phase_step = 32'd1463116;
                7'd013: midi_phase_step = 32'd1550118;
                7'd014: midi_phase_step = 32'd1642292;
                7'd015: midi_phase_step = 32'd1739948;
                7'd016: midi_phase_step = 32'd1843411;
                7'd017: midi_phase_step = 32'd1953026;
                7'd018: midi_phase_step = 32'd2069159;
                7'd019: midi_phase_step = 32'd2192197;
                7'd020: midi_phase_step = 32'd2322552;
                7'd021: midi_phase_step = 32'd2460658;
                7'd022: midi_phase_step = 32'd2606977;
                7'd023: midi_phase_step = 32'd2761996;
                7'd024: midi_phase_step = 32'd2926232;
                7'd025: midi_phase_step = 32'd3100235;
                7'd026: midi_phase_step = 32'd3284585;
                7'd027: midi_phase_step = 32'd3479896;
                7'd028: midi_phase_step = 32'd3686822;
                7'd029: midi_phase_step = 32'd3906052;
                7'd030: midi_phase_step = 32'd4138318;
                7'd031: midi_phase_step = 32'd4384395;
                7'd032: midi_phase_step = 32'd4645104;
                7'd033: midi_phase_step = 32'd4921317;
                7'd034: midi_phase_step = 32'd5213953;
                7'd035: midi_phase_step = 32'd5523991;
                7'd036: midi_phase_step = 32'd5852465;
                7'd037: midi_phase_step = 32'd6200470;
                7'd038: midi_phase_step = 32'd6569170;
                7'd039: midi_phase_step = 32'd6959793;
                7'd040: midi_phase_step = 32'd7373644;
                7'd041: midi_phase_step = 32'd7812103;
                7'd042: midi_phase_step = 32'd8276635;
                7'd043: midi_phase_step = 32'd8768789;
                7'd044: midi_phase_step = 32'd9290209;
                7'd045: midi_phase_step = 32'd9842633;
                7'd046: midi_phase_step = 32'd10427907;
                7'd047: midi_phase_step = 32'd11047982;
                7'd048: midi_phase_step = 32'd11704930;
                7'd049: midi_phase_step = 32'd12400941;
                7'd050: midi_phase_step = 32'd13138339;
                7'd051: midi_phase_step = 32'd13919586;
                7'd052: midi_phase_step = 32'd14747287;
                7'd053: midi_phase_step = 32'd15624207;
                7'd054: midi_phase_step = 32'd16553270;
                7'd055: midi_phase_step = 32'd17537579;
                7'd056: midi_phase_step = 32'd18580418;
                7'd057: midi_phase_step = 32'd19685267;
                7'd058: midi_phase_step = 32'd20855814;
                7'd059: midi_phase_step = 32'd22095965;
                7'd060: midi_phase_step = 32'd23409859;
                7'd061: midi_phase_step = 32'd24801882;
                7'd062: midi_phase_step = 32'd26276679;
                7'd063: midi_phase_step = 32'd27839171;
                7'd064: midi_phase_step = 32'd29494575;
                7'd065: midi_phase_step = 32'd31248413;
                7'd066: midi_phase_step = 32'd33106541;
                7'd067: midi_phase_step = 32'd35075158;
                7'd068: midi_phase_step = 32'd37160835;
                7'd069: midi_phase_step = 32'd39370534;
                7'd070: midi_phase_step = 32'd41711627;
                7'd071: midi_phase_step = 32'd44191930;
                7'd072: midi_phase_step = 32'd46819719;
                7'd073: midi_phase_step = 32'd49603764;
                7'd074: midi_phase_step = 32'd52553357;
                7'd075: midi_phase_step = 32'd55678342;
                7'd076: midi_phase_step = 32'd58989149;
                7'd077: midi_phase_step = 32'd62496826;
                7'd078: midi_phase_step = 32'd66213081;
                7'd079: midi_phase_step = 32'd70150316;
                7'd080: midi_phase_step = 32'd74321671;
                7'd081: midi_phase_step = 32'd78741067;
                7'd082: midi_phase_step = 32'd83423255;
                7'd083: midi_phase_step = 32'd88383859;
                7'd084: midi_phase_step = 32'd93639437;
                7'd085: midi_phase_step = 32'd99207528;
                7'd086: midi_phase_step = 32'd105106715;
                7'd087: midi_phase_step = 32'd111356685;
                7'd088: midi_phase_step = 32'd117978298;
                7'd089: midi_phase_step = 32'd124993653;
                7'd090: midi_phase_step = 32'd132426162;
                7'd091: midi_phase_step = 32'd140300631;
                7'd092: midi_phase_step = 32'd148643341;
                7'd093: midi_phase_step = 32'd157482134;
                7'd094: midi_phase_step = 32'd166846509;
                7'd095: midi_phase_step = 32'd176767719;
                7'd096: midi_phase_step = 32'd187278874;
                7'd097: midi_phase_step = 32'd198415056;
                7'd098: midi_phase_step = 32'd210213429;
                7'd099: midi_phase_step = 32'd222713370;
                7'd100: midi_phase_step = 32'd235956596;
                7'd101: midi_phase_step = 32'd249987305;
                7'd102: midi_phase_step = 32'd264852324;
                7'd103: midi_phase_step = 32'd280601263;
                7'd104: midi_phase_step = 32'd297286682;
                7'd105: midi_phase_step = 32'd314964268;
                7'd106: midi_phase_step = 32'd333693018;
                7'd107: midi_phase_step = 32'd353535438;
                7'd108: midi_phase_step = 32'd374557749;
                7'd109: midi_phase_step = 32'd396830112;
                7'd110: midi_phase_step = 32'd420426858;
                7'd111: midi_phase_step = 32'd445426740;
                7'd112: midi_phase_step = 32'd471913192;
                7'd113: midi_phase_step = 32'd499974611;
                7'd114: midi_phase_step = 32'd529704648;
                7'd115: midi_phase_step = 32'd561202526;
                7'd116: midi_phase_step = 32'd594573365;
                7'd117: midi_phase_step = 32'd629928537;
                7'd118: midi_phase_step = 32'd667386037;
                7'd119: midi_phase_step = 32'd707070876;
                7'd120: midi_phase_step = 32'd749115498;
                7'd121: midi_phase_step = 32'd793660223;
                7'd122: midi_phase_step = 32'd840853716;
                7'd123: midi_phase_step = 32'd890853480;
                7'd124: midi_phase_step = 32'd943826385;
                7'd125: midi_phase_step = 32'd999949222;
                7'd126: midi_phase_step = 32'd1059409297;
                7'd127: midi_phase_step = 32'd1122405052;
                default: midi_phase_step = 32'd0;
            endcase
        end
    endfunction

    function automatic logic [7:0] sin_quarter_mag(input logic [6:0] index);
        begin
            unique case (index)
                7'd00: sin_quarter_mag = 8'd0;
                7'd01: sin_quarter_mag = 8'd3;
                7'd02: sin_quarter_mag = 8'd6;
                7'd03: sin_quarter_mag = 8'd9;
                7'd04: sin_quarter_mag = 8'd13;
                7'd05: sin_quarter_mag = 8'd16;
                7'd06: sin_quarter_mag = 8'd19;
                7'd07: sin_quarter_mag = 8'd22;
                7'd08: sin_quarter_mag = 8'd25;
                7'd09: sin_quarter_mag = 8'd28;
                7'd10: sin_quarter_mag = 8'd31;
                7'd11: sin_quarter_mag = 8'd34;
                7'd12: sin_quarter_mag = 8'd37;
                7'd13: sin_quarter_mag = 8'd40;
                7'd14: sin_quarter_mag = 8'd43;
                7'd15: sin_quarter_mag = 8'd46;
                7'd16: sin_quarter_mag = 8'd49;
                7'd17: sin_quarter_mag = 8'd52;
                7'd18: sin_quarter_mag = 8'd55;
                7'd19: sin_quarter_mag = 8'd58;
                7'd20: sin_quarter_mag = 8'd60;
                7'd21: sin_quarter_mag = 8'd63;
                7'd22: sin_quarter_mag = 8'd66;
                7'd23: sin_quarter_mag = 8'd68;
                7'd24: sin_quarter_mag = 8'd71;
                7'd25: sin_quarter_mag = 8'd74;
                7'd26: sin_quarter_mag = 8'd76;
                7'd27: sin_quarter_mag = 8'd79;
                7'd28: sin_quarter_mag = 8'd81;
                7'd29: sin_quarter_mag = 8'd84;
                7'd30: sin_quarter_mag = 8'd86;
                7'd31: sin_quarter_mag = 8'd88;
                7'd32: sin_quarter_mag = 8'd91;
                7'd33: sin_quarter_mag = 8'd93;
                7'd34: sin_quarter_mag = 8'd95;
                7'd35: sin_quarter_mag = 8'd97;
                7'd36: sin_quarter_mag = 8'd99;
                7'd37: sin_quarter_mag = 8'd101;
                7'd38: sin_quarter_mag = 8'd103;
                7'd39: sin_quarter_mag = 8'd105;
                7'd40: sin_quarter_mag = 8'd106;
                7'd41: sin_quarter_mag = 8'd108;
                7'd42: sin_quarter_mag = 8'd110;
                7'd43: sin_quarter_mag = 8'd111;
                7'd44: sin_quarter_mag = 8'd113;
                7'd45: sin_quarter_mag = 8'd114;
                7'd46: sin_quarter_mag = 8'd116;
                7'd47: sin_quarter_mag = 8'd117;
                7'd48: sin_quarter_mag = 8'd118;
                7'd49: sin_quarter_mag = 8'd119;
                7'd50: sin_quarter_mag = 8'd121;
                7'd51: sin_quarter_mag = 8'd122;
                7'd52: sin_quarter_mag = 8'd122;
                7'd53: sin_quarter_mag = 8'd123;
                7'd54: sin_quarter_mag = 8'd124;
                7'd55: sin_quarter_mag = 8'd125;
                7'd56: sin_quarter_mag = 8'd126;
                7'd57: sin_quarter_mag = 8'd126;
                7'd58: sin_quarter_mag = 8'd127;
                7'd59: sin_quarter_mag = 8'd127;
                7'd60: sin_quarter_mag = 8'd127;
                7'd61: sin_quarter_mag = 8'd128;
                7'd62: sin_quarter_mag = 8'd128;
                7'd63: sin_quarter_mag = 8'd128;
                7'd64: sin_quarter_mag = 8'd128;
                default: sin_quarter_mag = 8'd0;
            endcase
        end
    endfunction

    function automatic logic signed [8:0] sine_sample(input logic [7:0] angle);
        logic [6:0] index;
        logic [7:0] mag;
        begin
            unique case (angle[7:6])
                2'b00: begin index = {1'b0, angle[5:0]}; mag = sin_quarter_mag(index); end
                2'b01: begin index = 7'd64 - angle[5:0]; mag = sin_quarter_mag(index); end
                2'b10: begin index = {1'b0, angle[5:0]}; mag = sin_quarter_mag(index); end
                default: begin index = 7'd64 - angle[5:0]; mag = sin_quarter_mag(index); end
            endcase
            sine_sample = angle[7] ? -$signed({1'b0, mag}) : $signed({1'b0, mag});
        end
    endfunction

    function automatic logic signed [8:0] triangle_sample(input logic [7:0] angle);
        logic [7:0] mag;
        begin
            mag = angle[6] ? (8'd255 - {angle[5:0], 2'b00}) : {angle[5:0], 2'b00};
            triangle_sample = angle[7] ? -$signed({1'b0, mag[7:0]}) : $signed({1'b0, mag[7:0]});
        end
    endfunction

    function automatic logic signed [8:0] wave_sample(
        input logic [2:0] instrument_id,
        input logic [7:0] phase_angle
    );
        logic [7:0] angle;
        begin
            angle = phase_angle;
            unique case (instrument_id)
                3'd0: wave_sample = sine_sample(angle);
                3'd1: wave_sample = $signed({1'b0, angle}) - 9'sd128;          // saw
                3'd2: wave_sample = phase_angle[7] ? -9'sd128 : 9'sd127;       // square
                3'd3: wave_sample = triangle_sample(angle);
                3'd4: wave_sample = (sine_sample(angle) + triangle_sample(angle)) >>> 1;
                3'd5: wave_sample = (phase_angle[7] ? -9'sd96 : 9'sd95) +
                                     ((sine_sample(angle) >>> 2));
                3'd6: wave_sample = ($signed({1'b0, angle}) - 9'sd128) +
                                     (sine_sample(angle) >>> 2);
                default: wave_sample = sine_sample(angle) + (triangle_sample(angle) >>> 2);
            endcase
        end
    endfunction

    function automatic logic [31:0] bend_phase_step(
        input logic [31:0] base_step,
        input logic [15:0] bend
    );
        logic signed [16:0] delta;
        logic [31:0] coarse;
        begin
            delta = $signed({1'b0, bend}) - 17'sd32768;
            unique case (delta[16:13])
                4'b1111: coarse = base_step >> 5;
                4'b1110: coarse = base_step >> 4;
                4'b1101: coarse = (base_step >> 4) + (base_step >> 5);
                4'b1100: coarse = base_step >> 3;
                4'b0000: coarse = base_step >> 5;
                4'b0001: coarse = base_step >> 4;
                4'b0010: coarse = (base_step >> 4) + (base_step >> 5);
                4'b0011: coarse = base_step >> 3;
                default: coarse = 32'd0;
            endcase
            bend_phase_step = delta[16] ? base_step - coarse : base_step + coarse;
        end
    endfunction

    function automatic logic [2:0] first_pending_voice(input logic [7:0] mask);
        begin
            unique casez (mask)
                8'b???????1: first_pending_voice = 3'd0;
                8'b??????10: first_pending_voice = 3'd1;
                8'b?????100: first_pending_voice = 3'd2;
                8'b????1000: first_pending_voice = 3'd3;
                8'b???10000: first_pending_voice = 3'd4;
                8'b??100000: first_pending_voice = 3'd5;
                8'b?1000000: first_pending_voice = 3'd6;
                default:     first_pending_voice = 3'd7;
            endcase
        end
    endfunction

    function automatic logic [7:0] pan_gain_left(input logic [7:0] pan_value);
        begin
            unique case (pan_value[7:4])
                4'd0: pan_gain_left = 8'd255;
                4'd1: pan_gain_left = 8'd254;
                4'd2: pan_gain_left = 8'd249;
                4'd3: pan_gain_left = 8'd242;
                4'd4: pan_gain_left = 8'd233;
                4'd5: pan_gain_left = 8'd221;
                4'd6: pan_gain_left = 8'd206;
                4'd7: pan_gain_left = 8'd190;
                4'd8: pan_gain_left = 8'd172;
                4'd9: pan_gain_left = 8'd150;
                4'd10: pan_gain_left = 8'd128;
                4'd11: pan_gain_left = 8'd104;
                4'd12: pan_gain_left = 8'd79;
                4'd13: pan_gain_left = 8'd53;
                4'd14: pan_gain_left = 8'd27;
                default: pan_gain_left = 8'd0;
            endcase
        end
    endfunction

    function automatic logic [7:0] pan_gain_right(input logic [7:0] pan_value);
        begin
            unique case (pan_value[7:4])
                4'd0: pan_gain_right = 8'd0;
                4'd1: pan_gain_right = 8'd27;
                4'd2: pan_gain_right = 8'd53;
                4'd3: pan_gain_right = 8'd79;
                4'd4: pan_gain_right = 8'd104;
                4'd5: pan_gain_right = 8'd128;
                4'd6: pan_gain_right = 8'd150;
                4'd7: pan_gain_right = 8'd172;
                4'd8: pan_gain_right = 8'd190;
                4'd9: pan_gain_right = 8'd206;
                4'd10: pan_gain_right = 8'd221;
                4'd11: pan_gain_right = 8'd233;
                4'd12: pan_gain_right = 8'd242;
                4'd13: pan_gain_right = 8'd249;
                4'd14: pan_gain_right = 8'd254;
                default: pan_gain_right = 8'd255;
            endcase
        end
    endfunction

    function automatic logic signed [17:0] gain_mul18(input logic [7:0] gain);
        begin
            gain_mul18 = $signed({10'd0, gain});
        end
    endfunction

    function automatic logic signed [17:0] velocity_mul18(input logic [6:0] gain);
        begin
            velocity_mul18 = $signed({11'd0, gain});
        end
    endfunction

    function automatic logic signed [17:0] sample_mul18(input logic signed [8:0] sample);
        begin
            sample_mul18 = {{9{sample[8]}}, sample};
        end
    endfunction

    function automatic logic signed [17:0] delta_mul18(input logic signed [9:0] delta);
        begin
            delta_mul18 = {{8{delta[9]}}, delta};
        end
    endfunction

    function automatic logic signed [8:0] clip_sample9(input logic signed [31:0] value);
        begin
            if (value > 32'sd127)
                clip_sample9 = 9'sd127;
            else if (value < -32'sd128)
                clip_sample9 = -9'sd128;
            else
                clip_sample9 = value[8:0];
        end
    endfunction

    function automatic logic signed [17:0] clip_mul18(input logic signed [31:0] value);
        begin
            if (value > 32'sd131071)
                clip_mul18 = 18'sd131071;
            else if (value < -32'sd131072)
                clip_mul18 = -18'sd131072;
            else
                clip_mul18 = value[17:0];
        end
    endfunction

    function automatic logic [15:0] next_env_level(
        input logic [15:0] current,
        input env_stage_t stage,
        input logic [15:0] attack_step,
        input logic [15:0] decay_step,
        input logic [15:0] sustain_level,
        input logic [15:0] release_step
    );
        logic [16:0] decay_threshold;
        begin
            decay_threshold = {1'b0, sustain_level} + {1'b0, decay_step};
            unique case (stage)
                ENV_ATTACK:
                    next_env_level = (attack_step == 16'd0 || current >= (16'hFFFF - attack_step))
                        ? 16'hFFFF
                        : current + attack_step;
                ENV_DECAY:
                    next_env_level = (decay_step == 16'd0 || current <= sustain_level ||
                                      decay_threshold >= 17'h10000 || current <= decay_threshold[15:0])
                        ? sustain_level
                        : current - decay_step;
                ENV_RELEASE:
                    next_env_level = (release_step == 16'd0 || current <= release_step)
                        ? 16'h0000
                        : current - release_step;
                ENV_SUSTAIN:
                    next_env_level = current;
                default:
                    next_env_level = 16'h0000;
            endcase
        end
    endfunction

    function automatic env_stage_t next_env_stage(
        input logic [15:0] current,
        input env_stage_t stage,
        input logic [15:0] attack_step,
        input logic [15:0] decay_step,
        input logic [15:0] sustain_level,
        input logic [15:0] release_step
    );
        logic [16:0] decay_threshold;
        begin
            decay_threshold = {1'b0, sustain_level} + {1'b0, decay_step};
            unique case (stage)
                ENV_ATTACK:
                    next_env_stage = (attack_step == 16'd0 || current >= (16'hFFFF - attack_step))
                        ? ENV_DECAY
                        : ENV_ATTACK;
                ENV_DECAY:
                    next_env_stage = (decay_step == 16'd0 || current <= sustain_level ||
                                      decay_threshold >= 17'h10000 || current <= decay_threshold[15:0])
                        ? ENV_SUSTAIN
                        : ENV_DECAY;
                ENV_RELEASE:
                    next_env_stage = (release_step == 16'd0 || current <= release_step)
                        ? ENV_OFF
                        : ENV_RELEASE;
                default:
                    next_env_stage = stage;
            endcase
        end
    endfunction

    function automatic logic signed [17:0] clip18(input logic signed [31:0] value);
        begin
            if (value > 32'sd131071)
                clip18 = 18'sd131071;
            else if (value < -32'sd131072)
                clip18 = -18'sd131072;
            else
                clip18 = value[17:0];
        end
    endfunction

    function automatic logic [7:0] enum_name_byte(input logic [2:0] inst, input logic [5:0] index);
        begin
            enum_name_byte = 8'h00;
            unique case (inst)
                3'd0: unique case (index)
                    6'd0: enum_name_byte = "S"; 6'd1: enum_name_byte = "I"; 6'd2: enum_name_byte = "N"; 6'd3: enum_name_byte = "E";
                    default: enum_name_byte = 8'h00;
                endcase
                3'd1: unique case (index)
                    6'd0: enum_name_byte = "S"; 6'd1: enum_name_byte = "A"; 6'd2: enum_name_byte = "W";
                    default: enum_name_byte = 8'h00;
                endcase
                3'd2: unique case (index)
                    6'd0: enum_name_byte = "S"; 6'd1: enum_name_byte = "Q"; 6'd2: enum_name_byte = "U"; 6'd3: enum_name_byte = "A"; 6'd4: enum_name_byte = "R"; 6'd5: enum_name_byte = "E";
                    default: enum_name_byte = 8'h00;
                endcase
                3'd3: unique case (index)
                    6'd0: enum_name_byte = "T"; 6'd1: enum_name_byte = "R"; 6'd2: enum_name_byte = "I"; 6'd3: enum_name_byte = "A"; 6'd4: enum_name_byte = "N"; 6'd5: enum_name_byte = "G"; 6'd6: enum_name_byte = "L"; 6'd7: enum_name_byte = "E";
                    default: enum_name_byte = 8'h00;
                endcase
                3'd4: unique case (index)
                    6'd0: enum_name_byte = "S"; 6'd1: enum_name_byte = "I"; 6'd2: enum_name_byte = "N"; 6'd3: enum_name_byte = "E"; 6'd4: enum_name_byte = "+"; 6'd5: enum_name_byte = "T"; 6'd6: enum_name_byte = "R"; 6'd7: enum_name_byte = "I";
                    default: enum_name_byte = 8'h00;
                endcase
                3'd5: unique case (index)
                    6'd0: enum_name_byte = "S"; 6'd1: enum_name_byte = "Q"; 6'd2: enum_name_byte = "+"; 6'd3: enum_name_byte = "S"; 6'd4: enum_name_byte = "I"; 6'd5: enum_name_byte = "N";
                    default: enum_name_byte = 8'h00;
                endcase
                3'd6: unique case (index)
                    6'd0: enum_name_byte = "S"; 6'd1: enum_name_byte = "A"; 6'd2: enum_name_byte = "W"; 6'd3: enum_name_byte = "+"; 6'd4: enum_name_byte = "S"; 6'd5: enum_name_byte = "I"; 6'd6: enum_name_byte = "N";
                    default: enum_name_byte = 8'h00;
                endcase
                default: unique case (index)
                    6'd0: enum_name_byte = "B"; 6'd1: enum_name_byte = "R"; 6'd2: enum_name_byte = "I"; 6'd3: enum_name_byte = "G"; 6'd4: enum_name_byte = "H"; 6'd5: enum_name_byte = "T";
                    default: enum_name_byte = 8'h00;
                endcase
            endcase
        end
    endfunction

    task automatic note_on(input logic [2:0] voice, input logic [7:0] midi_note);
        logic [31:0] base_step;
        begin
            base_step = midi_phase_step(midi_note[6:0]);
            note[voice] <= midi_note;
            phase[voice] <= 32'd0;
            phase_step[voice] <= sample_mode[voice]
                ? sample_phase_step[voice]
                : bend_phase_step(base_step, pitch_bend[voice]);
            env_level[voice] <= 16'd0;
            env_stage[voice] <= ENV_ATTACK;
            active[voice] <= 1'b1;
            releasing[voice] <= 1'b0;
        end
    endtask

    task automatic note_off(input logic [2:0] voice);
        begin
            if (active[voice]) begin
                env_stage[voice] <= ENV_RELEASE;
                releasing[voice] <= 1'b1;
            end
        end
    endtask

    task automatic reset_event_fifo();
        begin
            event_wr_ptr <= '0;
            event_rd_ptr <= '0;
            event_wr_count <= '0;
            event_rd_count <= '0;
            event_byte_index <= 3'd0;
            event_build_time <= 32'd0;
            event_build_off <= 8'd0;
            event_head <= 48'd0;
            event_head_valid <= 1'b0;
            event_read_pending <= 1'b0;
            event_read_addr <= '0;
            event_running <= 1'b0;
            event_parser_error <= 1'b0;
            event_frame <= 32'd0;
        end
    endtask

    task automatic all_notes_off();
        begin
            reset_event_fifo();
            note_on_pending <= 8'd0;
            note_off_pending <= 8'd0;
            bend_pending <= 8'd0;
            for (int i = 0; i < 8; i++) begin
                active[i] <= 1'b0;
                releasing[i] <= 1'b0;
                env_stage[i] <= ENV_OFF;
                env_level[i] <= 16'd0;
                note[i] <= 8'd0;
                phase[i] <= 32'd0;
                phase_step[i] <= 32'd0;
            end
        end
    endtask

    task automatic start_mix_sample(input logic signed [8:0] sample);
        begin
            mul_a <= sample_mul18(sample);
            mul_b <= gain_mul18(env_level[mix_voice][15:8]);
            mix_state <= MIX_GAIN_ENV;
        end
    endtask

    task automatic update_current_env();
        logic [15:0] env_next;
        env_stage_t stage_next;
        begin
            env_next = next_env_level(env_level[mix_voice],
                                      env_stage[mix_voice],
                                      env_attack_step[mix_voice],
                                      env_decay_step[mix_voice],
                                      env_sustain_level[mix_voice],
                                      env_release_step[mix_voice]);
            stage_next = next_env_stage(env_level[mix_voice],
                                        env_stage[mix_voice],
                                        env_attack_step[mix_voice],
                                        env_decay_step[mix_voice],
                                        env_sustain_level[mix_voice],
                                        env_release_step[mix_voice]);
            env_level[mix_voice] <= env_next;
            env_stage[mix_voice] <= stage_next;
            if (stage_next == ENV_OFF) begin
                active[mix_voice] <= 1'b0;
                releasing[mix_voice] <= 1'b0;
            end
        end
    endtask

    task automatic advance_mix_voice();
        begin
            if (mix_voice == 3'd7) begin
                mul_a <= clip_mul18(mix_acc_l);
                mul_b <= gain_mul18(master_volume);
                mix_state <= MIX_LATCH_MASTER_L;
            end else begin
                mix_voice <= mix_voice + 3'd1;
                mix_state <= MIX_VOICE;
            end
        end
    endtask

    function automatic logic [7:0] env_param_byte(
        input logic [2:0] voice,
        input logic [2:0] index
    );
        begin
            unique case (index)
                3'd0: env_param_byte = env_attack_step[voice][7:0];
                3'd1: env_param_byte = env_attack_step[voice][15:8];
                3'd2: env_param_byte = env_decay_step[voice][7:0];
                3'd3: env_param_byte = env_decay_step[voice][15:8];
                3'd4: env_param_byte = env_sustain_level[voice][7:0];
                3'd5: env_param_byte = env_sustain_level[voice][15:8];
                3'd6: env_param_byte = env_release_step[voice][7:0];
                default: env_param_byte = env_release_step[voice][15:8];
            endcase
        end
    endfunction

    task automatic write_env_param_byte(
        input logic [2:0] voice,
        input logic [2:0] index,
        input logic [7:0] data
    );
        begin
            unique case (index)
                3'd0: env_attack_step[voice][7:0] <= data;
                3'd1: env_attack_step[voice][15:8] <= data;
                3'd2: env_decay_step[voice][7:0] <= data;
                3'd3: env_decay_step[voice][15:8] <= data;
                3'd4: env_sustain_level[voice][7:0] <= data;
                3'd5: env_sustain_level[voice][15:8] <= data;
                3'd6: env_release_step[voice][7:0] <= data;
                default: env_release_step[voice][15:8] <= data;
            endcase
        end
    endtask

    function automatic logic [7:0] read_offset(input logic [7:0] off);
        logic [2:0] voice;
        logic [2:0] voice_reg;
        logic [5:0] enum_name_index;
        begin
            read_offset = 8'h00;
            if (off <= 8'h3F) begin
                voice = off[5:3];
                voice_reg = off[2:0];
                unique case (voice_reg)
                    3'd3: read_offset = volume[voice];
                    3'd4: read_offset = pan[voice];
                    3'd7: read_offset = {6'b000000, releasing[voice], active[voice]};
                    default: read_offset = 8'h00;
                endcase
            end else begin
                unique case (off)
                    8'h40: read_offset = reverb_level;
                    8'h41: read_offset = chorus_level;
                    8'h42: read_offset = master_volume;
                    8'h43: read_offset = 8'h01; // resident bank ready
                    8'h44: read_offset = RESIDENT_INST_COUNT;
                    8'h46: read_offset = 8'h57; // hardware WTS signature ("W")
                    8'h47: begin
                        read_offset = 8'h00;
                        for (int i = 0; i < 8; i++)
                            if (active[i])
                                read_offset[i] = 1'b1;
                    end
                    8'h48: read_offset = {5'b00000, param_voice};
                    8'h49: read_offset = sample_start[param_voice][7:0];
                    8'h4A: read_offset = sample_start[param_voice][15:8];
                    8'h4B: read_offset = sample_start[param_voice][23:16];
                    8'h4C: read_offset = {7'b0000000, sample_start[param_voice][24]};
                    8'h4D: read_offset = sample_end[param_voice][7:0];
                    8'h4E: read_offset = sample_end[param_voice][15:8];
                    8'h4F: read_offset = sample_end[param_voice][23:16];
                    8'h50: read_offset = {7'b0000000, sample_end[param_voice][24]};
                    8'h51: read_offset = sample_loop_start[param_voice][7:0];
                    8'h52: read_offset = sample_loop_start[param_voice][15:8];
                    8'h53: read_offset = sample_loop_start[param_voice][23:16];
                    8'h54: read_offset = {7'b0000000, sample_loop_start[param_voice][24]};
                    8'h55: read_offset = sample_loop_end[param_voice][7:0];
                    8'h56: read_offset = sample_loop_end[param_voice][15:8];
                    8'h57: read_offset = sample_loop_end[param_voice][23:16];
                    8'h58: read_offset = {7'b0000000, sample_loop_end[param_voice][24]};
                    8'h59: read_offset = sample_phase_step[param_voice][7:0];
                    8'h5A: read_offset = sample_phase_step[param_voice][15:8];
                    8'h5B: read_offset = sample_phase_step[param_voice][23:16];
                    8'h5C: read_offset = sample_phase_step[param_voice][31:24];
                    8'h5D: read_offset = {6'b000000, sample_loop[param_voice], sample_mode[param_voice]};
                    8'h5E: read_offset = {5'b00000, env_param_index};
                    8'h5F: read_offset = env_param_byte(param_voice, env_param_index);
                    8'h61: read_offset = 8'h00; // resident bank 0
                    8'h62: read_offset = {1'b0, enum_index[6:0]};
                    8'hA0: read_offset = {
                        event_parser_error,
                        3'b000,
                        event_running,
                        event_fifo_empty,
                        event_fifo_full,
                        host_event_ready
                    };
                    8'hA1: read_offset = event_fifo_count[7:0];
                    8'hA2: read_offset = {5'b00000, event_fifo_count[10:8]};
                    8'hA3: read_offset = event_fifo_space[7:0];
                    8'hA4: read_offset = {5'b00000, event_fifo_space[10:8]};
                    8'hA5: read_offset = event_frame[7:0];
                    8'hA6: read_offset = event_frame[15:8];
                    8'hA7: read_offset = event_frame[23:16];
                    8'hA8: read_offset = event_frame[31:24];
                    default: begin
                        if (off >= 8'h63 && off <= 8'h9F) begin
                            enum_name_index = off[5:0] - 6'h23;
                            read_offset = enum_name_byte(enum_index[2:0], enum_name_index);
                        end
                    end
                endcase
            end
        end
    endfunction

    assign cpu_rdata = cpu_sel ? read_offset(cpu_off) : 8'h00;
    assign dbg_rdata = (dbg_raddr <= 8'hBF) ? read_offset(dbg_raddr) : 8'h00;

    task automatic write_offset(input logic [7:0] off, input logic [7:0] data);
        logic [2:0] voice;
        logic [2:0] voice_reg;
        begin
            if (off <= 8'h3F) begin
                voice = off[5:3];
                voice_reg = off[2:0];
                unique case (voice_reg)
                    3'd0: begin
                        if (data == 8'd0) begin
                            note_on_pending[voice] <= 1'b0;
                            note_off_pending[voice] <= 1'b1;
                        end else begin
                            pending_note[voice] <= data;
                            note_on_pending[voice] <= 1'b1;
                            note_off_pending[voice] <= 1'b0;
                        end
                    end
                    3'd1: velocity[voice] <= {1'b0, data[6:0]};
                    3'd2: instrument[voice] <= data;
                    3'd3: volume[voice] <= data;
                    3'd4: pan[voice] <= data;
                    3'd5: begin
                        pitch_bend[voice][7:0] <= data;
                        pending_bend[voice] <= {pitch_bend[voice][15:8], data};
                        bend_pending[voice] <= 1'b1;
                    end
                    3'd6: begin
                        pitch_bend[voice][15:8] <= data;
                        pending_bend[voice] <= {data, pitch_bend[voice][7:0]};
                        bend_pending[voice] <= 1'b1;
                    end
                    default: begin end
                endcase
            end else begin
                unique case (off)
                    8'h40: reverb_level <= data;
                    8'h41: chorus_level <= data;
                    8'h42: master_volume <= data;
                    8'h45: begin
                        unique case (data)
                            CMD_ALL_NOTES_OFF: all_notes_off();
                            CMD_RESET_EFFECTS: begin
                                reverb_level <= DEFAULT_REVERB;
                                chorus_level <= DEFAULT_CHORUS;
                                master_volume <= DEFAULT_MASTER;
                            end
                            CMD_EVENT_RESET: reset_event_fifo();
                            CMD_EVENT_START: begin
                                event_frame <= 32'd0;
                                event_running <= 1'b1;
                            end
                            CMD_EVENT_STOP: event_running <= 1'b0;
                            default: begin end
                        endcase
                    end
                    8'h46: begin end
                    8'h48: param_voice <= data[2:0];
                    8'h49: sample_start[param_voice][7:0] <= data;
                    8'h4A: sample_start[param_voice][15:8] <= data;
                    8'h4B: sample_start[param_voice][23:16] <= data;
                    8'h4C: sample_start[param_voice][24] <= data[0];
                    8'h4D: sample_end[param_voice][7:0] <= data;
                    8'h4E: sample_end[param_voice][15:8] <= data;
                    8'h4F: sample_end[param_voice][23:16] <= data;
                    8'h50: sample_end[param_voice][24] <= data[0];
                    8'h51: sample_loop_start[param_voice][7:0] <= data;
                    8'h52: sample_loop_start[param_voice][15:8] <= data;
                    8'h53: sample_loop_start[param_voice][23:16] <= data;
                    8'h54: sample_loop_start[param_voice][24] <= data[0];
                    8'h55: sample_loop_end[param_voice][7:0] <= data;
                    8'h56: sample_loop_end[param_voice][15:8] <= data;
                    8'h57: sample_loop_end[param_voice][23:16] <= data;
                    8'h58: sample_loop_end[param_voice][24] <= data[0];
                    8'h59: sample_phase_step[param_voice][7:0] <= data;
                    8'h5A: sample_phase_step[param_voice][15:8] <= data;
                    8'h5B: sample_phase_step[param_voice][23:16] <= data;
                    8'h5C: sample_phase_step[param_voice][31:24] <= data;
                    8'h5D: begin
                        sample_mode[param_voice] <= data[0];
                        sample_loop[param_voice] <= data[1];
                    end
                    8'h5E: env_param_index <= data[2:0];
                    8'h5F: write_env_param_byte(param_voice, env_param_index, data);
                    8'h60: enum_index <= data;
                    default: begin end
                endcase
            end
        end
    endtask

    always_ff @(posedge clk) begin
        if (rst) begin
            sample_acc <= 32'd0;
            sample_en <= 1'b0;
        end else if (sample_next >= {1'b0, SAMPLE_RATE}) begin
            sample_acc <= sample_next[31:0] - SAMPLE_RATE;
            sample_en <= 1'b1;
        end else begin
            sample_acc <= sample_next[31:0];
            sample_en <= 1'b0;
        end
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            reverb_level <= DEFAULT_REVERB;
            chorus_level <= DEFAULT_CHORUS;
            master_volume <= DEFAULT_MASTER;
            enum_index <= 8'd0;
            audio_l <= 18'sd0;
            audio_r <= 18'sd0;
            mix_state <= MIX_IDLE;
            mix_voice <= 3'd0;
            mix_acc_l <= 32'sd0;
            mix_acc_r <= 32'sd0;
            param_voice <= 3'd0;
            env_param_index <= 3'd0;
            sample_read_addr <= 25'd0;
            sample_hold_cnt <= 4'd0;
            sample_interp_base <= 9'sd0;
            sample_interp_next <= 9'sd0;
            sample_interp_value <= 9'sd0;
            sample_interp_frac <= 8'd0;
            sample_next_phase <= 32'd0;
            sample_loop_start_phase <= 32'd0;
            sample_loop_end_phase <= 32'd0;
            sample_end_phase <= 32'd0;
            mix_amp <= 32'sd0;
            mix_left <= 32'sd0;
            latch_scaled_l <= 32'sd0;
            note_on_pending <= 8'd0;
            note_off_pending <= 8'd0;
            bend_pending <= 8'd0;
            service_voice <= 3'd0;
            service_note <= 8'd0;
            service_bend <= 16'h8000;
            service_base_step <= 32'd0;
            mul_a <= 18'sd0;
            mul_b <= 18'sd0;
            cpu_write_pending <= 1'b0;
            cpu_write_off <= 8'd0;
            cpu_write_data <= 8'd0;
            dbg_write_pending <= 1'b0;
            dbg_write_off <= 8'd0;
            dbg_write_data <= 8'd0;
            event_wr_ptr <= '0;
            event_rd_ptr <= '0;
            event_wr_count <= '0;
            event_rd_count <= '0;
            event_byte_index <= 3'd0;
            event_build_time <= 32'd0;
            event_build_off <= 8'd0;
            event_head <= 48'd0;
            event_head_valid <= 1'b0;
            event_read_pending <= 1'b0;
            event_read_addr <= '0;
            event_running <= 1'b0;
            event_parser_error <= 1'b0;
            event_frame <= 32'd0;
            for (int i = 0; i < 8; i++) begin
                note[i] <= 8'd0;
                velocity[i] <= 8'd100;
                instrument[i] <= i[7:0];
                volume[i] <= 8'hFF;
                pan[i] <= 8'h80;
                pitch_bend[i] <= 16'h8000;
                phase[i] <= 32'd0;
                phase_step[i] <= 32'd0;
                sample_start[i] <= 25'd0;
                sample_end[i] <= 25'd0;
                sample_loop_start[i] <= 25'd0;
                sample_loop_end[i] <= 25'd0;
                sample_phase_step[i] <= 32'd0;
                sample_mode[i] <= 1'b0;
                sample_loop[i] <= 1'b0;
                env_level[i] <= 16'd0;
                env_attack_step[i] <= ENV_ATTACK_STEP;
                env_decay_step[i] <= ENV_DECAY_STEP;
                env_sustain_level[i] <= ENV_SUSTAIN_LEVEL;
                env_release_step[i] <= ENV_RELEASE_STEP;
                env_stage[i] <= ENV_OFF;
                active[i] <= 1'b0;
                releasing[i] <= 1'b0;
                pending_note[i] <= 8'd0;
                pending_bend[i] <= 16'h8000;
            end
        end else begin
            dbg_write_pending <= dbg_we && dbg_off <= 8'hBF;
            dbg_write_off <= dbg_off;
            dbg_write_data <= dbg_wdata;

            cpu_write_pending <= cpu_ce && cpu_we && cpu_sel;
            cpu_write_off <= cpu_off;
            cpu_write_data <= cpu_wdata;

            if (sample_en && event_running)
                event_frame <= event_frame + 32'd1;

            if (host_event_we) begin
                if (!host_event_ready) begin
                    event_parser_error <= 1'b1;
                end else begin
                    unique case (event_byte_index)
                        3'd0: begin
                            event_build_time[7:0] <= host_event_data;
                            event_byte_index <= 3'd1;
                        end
                        3'd1: begin
                            event_build_time[15:8] <= host_event_data;
                            event_byte_index <= 3'd2;
                        end
                        3'd2: begin
                            event_build_time[23:16] <= host_event_data;
                            event_byte_index <= 3'd3;
                        end
                        3'd3: begin
                            event_build_time[31:24] <= host_event_data;
                            event_byte_index <= 3'd4;
                        end
                        3'd4: begin
                            event_build_off <= host_event_data;
                            event_byte_index <= 3'd5;
                        end
                        default: begin
                            event_fifo[event_wr_ptr] <= {
                                event_build_time,
                                event_build_off,
                                host_event_data
                            };
                            event_wr_ptr <= event_wr_ptr + 1'b1;
                            event_wr_count <= event_wr_count + 1'b1;
                            event_byte_index <= 3'd0;
                        end
                    endcase
                end
            end

            if (event_read_pending) begin
                event_head <= event_fifo[event_read_addr];
                event_head_valid <= 1'b1;
                event_read_pending <= 1'b0;
            end else if (!event_head_valid && !event_fifo_empty) begin
                event_read_addr <= event_rd_ptr;
                event_read_pending <= 1'b1;
            end

            if (dbg_write_pending) begin
                write_offset(dbg_write_off, dbg_write_data);
            end
            if (cpu_write_pending) begin
                write_offset(cpu_write_off, cpu_write_data);
            end

            unique case (mix_state)
                MIX_IDLE: begin
                    if (event_due && !command_write_pending) begin
                        write_offset(event_head_off, event_head_data);
                        event_head_valid <= 1'b0;
                        event_rd_ptr <= event_rd_ptr + 1'b1;
                        event_rd_count <= event_rd_count + 1'b1;
                    end else if (note_on_pending != 8'd0) begin
                        logic [2:0] voice;
                        voice = first_pending_voice(note_on_pending);
                        service_voice <= voice;
                        service_note <= pending_note[voice];
                        note_on_pending[voice] <= 1'b0;
                        mix_state <= MIX_NOTE_PREP;
                    end else if (note_off_pending != 8'd0) begin
                        logic [2:0] voice;
                        voice = first_pending_voice(note_off_pending);
                        note_off_pending[voice] <= 1'b0;
                        note_off(voice);
                    end else if (bend_pending != 8'd0) begin
                        logic [2:0] voice;
                        voice = first_pending_voice(bend_pending);
                        service_voice <= voice;
                        service_bend <= pending_bend[voice];
                        bend_pending[voice] <= 1'b0;
                        mix_state <= MIX_BEND_PREP;
                    end else if (sample_en) begin
                        mix_acc_l <= 32'sd0;
                        mix_acc_r <= 32'sd0;
                        mix_voice <= 3'd0;
                        mix_state <= MIX_VOICE;
                    end
                end

                MIX_NOTE_PREP: begin
                    service_base_step <= midi_phase_step(service_note[6:0]);
                    mix_state <= MIX_NOTE_APPLY;
                end

                MIX_NOTE_APPLY: begin
                    note[service_voice] <= service_note;
                    phase[service_voice] <= 32'd0;
                    phase_step[service_voice] <= sample_mode[service_voice]
                        ? sample_phase_step[service_voice]
                        : bend_phase_step(service_base_step, pitch_bend[service_voice]);
                    env_level[service_voice] <= 16'd0;
                    env_stage[service_voice] <= ENV_ATTACK;
                    active[service_voice] <= 1'b1;
                    releasing[service_voice] <= 1'b0;
                    mix_state <= MIX_IDLE;
                end

                MIX_BEND_PREP: begin
                    service_base_step <= midi_phase_step(note[service_voice][6:0]);
                    mix_state <= MIX_BEND_APPLY;
                end

                MIX_BEND_APPLY: begin
                    phase_step[service_voice] <= sample_mode[service_voice]
                        ? sample_phase_step[service_voice]
                        : bend_phase_step(service_base_step, service_bend);
                    mix_state <= MIX_IDLE;
                end

                MIX_VOICE: begin
                    logic signed [8:0] sample;

                    if (active[mix_voice]) begin
                        if (sample_mode[mix_voice]) begin
                            logic [24:0] current_addr;
                            current_addr = sample_start[mix_voice] + phase[mix_voice][31:8];
                            if (sample_end[mix_voice] <= sample_start[mix_voice] ||
                                current_addr >= sample_end[mix_voice]) begin
                                active[mix_voice] <= 1'b0;
                                releasing[mix_voice] <= 1'b0;
                                advance_mix_voice();
                            end else begin
                                sample_read_addr <= current_addr;
                                sample_interp_frac <= phase[mix_voice][7:0];
                                sample_hold_cnt <= 4'd7;
                                mix_state <= MIX_SAMPLE_READ;
                            end
                        end else begin
                            sample = wave_sample(instrument[mix_voice][2:0], phase[mix_voice][31:24]);
                            phase[mix_voice] <= phase[mix_voice] + phase_step[mix_voice];
                            start_mix_sample(sample);
                        end
                    end else begin
                        advance_mix_voice();
                    end
                end

                MIX_SAMPLE_READ: begin
                    if (sample_hold_cnt == 0)
                        mix_state <= MIX_SAMPLE_CAPTURE;
                    else
                        sample_hold_cnt <= sample_hold_cnt - 1'd1;
                end

                MIX_SAMPLE_CAPTURE: begin
                    logic signed [8:0] sample;
                    logic [24:0] next_addr;

                    sample = {sdram_doutB[7], sdram_doutB};
                    sample_interp_base <= sample;

                    next_addr = sample_read_addr + 25'd1;
                    if (sample_loop[mix_voice] &&
                        sample_loop_end[mix_voice] > sample_loop_start[mix_voice] &&
                        next_addr >= sample_loop_end[mix_voice]) begin
                        next_addr = sample_loop_start[mix_voice];
                    end else if (next_addr >= sample_end[mix_voice]) begin
                        next_addr = sample_read_addr;
                    end

                    sample_read_addr <= next_addr;
                    sample_hold_cnt <= 4'd7;
                    mix_state <= MIX_SAMPLE_READ_NEXT;
                end

                MIX_SAMPLE_READ_NEXT: begin
                    if (sample_hold_cnt == 0) begin
                        sample_interp_next <= {sdram_doutB[7], sdram_doutB};
                        mix_state <= MIX_SAMPLE_DELTA;
                    end else begin
                        sample_hold_cnt <= sample_hold_cnt - 1'd1;
                    end
                end

                MIX_SAMPLE_DELTA: begin
                    logic signed [9:0] sample_delta;

                    sample_delta = {sample_interp_next[8], sample_interp_next} -
                                   {sample_interp_base[8], sample_interp_base};
                    mul_a <= delta_mul18(sample_delta);
                    mul_b <= gain_mul18(sample_interp_frac);
                    mix_state <= MIX_SAMPLE_INTERPOLATE;
                end

                MIX_SAMPLE_INTERPOLATE: begin
                    logic signed [31:0] interpolated;
                    logic [24:0] loop_start_diff;
                    logic [24:0] loop_end_diff;
                    logic [24:0] end_diff;

                    interpolated = $signed(sample_interp_base) +
                                   $signed(mul_product >>> 8);
                    sample_interp_value <= clip_sample9(interpolated);

                    sample_next_phase <= phase[mix_voice] + phase_step[mix_voice];
                    loop_start_diff = sample_loop_start[mix_voice] - sample_start[mix_voice];
                    loop_end_diff = sample_loop_end[mix_voice] - sample_start[mix_voice];
                    end_diff = sample_end[mix_voice] - sample_start[mix_voice];
                    sample_loop_start_phase <= {loop_start_diff[23:0], 8'd0};
                    sample_loop_end_phase <= {loop_end_diff[23:0], 8'd0};
                    sample_end_phase <= {end_diff[23:0], 8'd0};
                    mix_state <= MIX_SAMPLE_PHASE;
                end

                MIX_SAMPLE_PHASE: begin
                    if (sample_loop[mix_voice] &&
                        sample_loop_end[mix_voice] > sample_loop_start[mix_voice] &&
                        sample_next_phase >= sample_loop_end_phase) begin
                        phase[mix_voice] <= sample_loop_start_phase +
                                            (sample_next_phase - sample_loop_end_phase);
                    end else begin
                        phase[mix_voice] <= sample_next_phase;
                        if (sample_next_phase >= sample_end_phase) begin
                            active[mix_voice] <= 1'b0;
                            releasing[mix_voice] <= 1'b0;
                        end
                    end
                    start_mix_sample(sample_interp_value);
                end

                MIX_GAIN_ENV: begin
                    logic signed [31:0] amp_next;
                    amp_next = $signed(mul_product >>> 8);
                    mix_amp <= amp_next;
                    mul_a <= clip_mul18(amp_next);
                    mul_b <= velocity_mul18(velocity[mix_voice][6:0]);
                    mix_state <= MIX_GAIN_VELOCITY;
                end

                MIX_GAIN_VELOCITY: begin
                    logic signed [31:0] amp_next;
                    amp_next = $signed(mul_product >>> 7);
                    mix_amp <= amp_next;
                    mul_a <= clip_mul18(amp_next);
                    mul_b <= gain_mul18(volume[mix_voice]);
                    mix_state <= MIX_GAIN_VOLUME;
                end

                MIX_GAIN_VOLUME: begin
                    logic signed [31:0] amp_next;
                    amp_next = $signed(mul_product >>> 8) <<< 5;
                    mix_amp <= amp_next;
                    mul_a <= clip_mul18(amp_next);
                    mul_b <= gain_mul18(pan_gain_left(pan[mix_voice]));
                    mix_state <= MIX_GAIN_PAN_L;
                end

                MIX_GAIN_PAN_L: begin
                    mix_left <= $signed(mul_product >>> 8);
                    mul_a <= clip_mul18(mix_amp);
                    mul_b <= gain_mul18(pan_gain_right(pan[mix_voice]));
                    mix_state <= MIX_GAIN_PAN_R;
                end

                MIX_GAIN_PAN_R: begin
                    logic signed [31:0] right;
                    logic signed [31:0] next_l;
                    logic signed [31:0] next_r;
                    right = $signed(mul_product >>> 8);
                    next_l = mix_acc_l + mix_left;
                    next_r = mix_acc_r + right;
                    mix_acc_l <= next_l;
                    mix_acc_r <= next_r;
                    update_current_env();
                    if (mix_voice == 3'd7) begin
                        mul_a <= clip_mul18(next_l);
                        mul_b <= gain_mul18(master_volume);
                        mix_state <= MIX_LATCH_MASTER_L;
                    end else begin
                        mix_voice <= mix_voice + 3'd1;
                        mix_state <= MIX_VOICE;
                    end
                end

                MIX_LATCH_MASTER_L: begin
                    latch_scaled_l <= $signed(mul_product >>> 8);
                    mul_a <= clip_mul18(mix_acc_r);
                    mul_b <= gain_mul18(master_volume);
                    mix_state <= MIX_LATCH_MASTER_R;
                end

                MIX_LATCH_MASTER_R: begin
                    logic signed [31:0] scaled_r;
                    scaled_r = $signed(mul_product >>> 8);
                    audio_l <= clip18(latch_scaled_l);
                    audio_r <= clip18(scaled_r);
                    mix_state <= MIX_IDLE;
                end
            endcase
        end
    end

endmodule
