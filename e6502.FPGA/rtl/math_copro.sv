// Math coprocessor register bank at $BB20-$BB4F.
// v2 implements MUL16, MUL_FX, DIV_S32_16, SINCOS, ATAN2, DIST_APPROX,
// RNG, fixed-point vector helpers, and read-only feature/status ABI.

module math_copro (
    input  logic        clk,
    input  logic        rst,

    input  logic [15:0] cpu_addr,
    input  logic [15:0] cpu_raddr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    input  logic        cpu_re,
    output logic [7:0]  cpu_rdata,
    output logic        rdy_out
);

    localparam MATH_BASE = 16'hBB20;
    localparam MATH_END  = 16'hBB4F;

    localparam R_MUL16_A_LO  = 6'd0;
    localparam R_MUL16_A_HI  = 6'd1;
    localparam R_MUL16_B_LO  = 6'd2;
    localparam R_MUL16_B_HI  = 6'd3;
    localparam R_MULFX_A_LO  = 6'd4;
    localparam R_MULFX_A_HI  = 6'd5;
    localparam R_MULFX_B_LO  = 6'd6;
    localparam R_MULFX_B_HI  = 6'd7;
    localparam R_DIV_N_LO    = 6'd8;
    localparam R_DIV_N_1     = 6'd9;
    localparam R_DIV_N_2     = 6'd10;
    localparam R_DIV_N_HI    = 6'd11;
    localparam R_DIV_D_LO    = 6'd12;
    localparam R_DIV_D_HI    = 6'd13;
    localparam R_SINCOS      = 6'd14;
    localparam R_ATAN_DY_LO  = 6'd15;
    localparam R_ATAN_DY_HI  = 6'd16;
    localparam R_ATAN_DX_LO  = 6'd17;
    localparam R_ATAN_DX_HI  = 6'd18;
    localparam R_DIST_DX_LO  = 6'd19;
    localparam R_DIST_DX_HI  = 6'd20;
    localparam R_DIST_DY_LO  = 6'd21;
    localparam R_DIST_DY_HI  = 6'd22;
    localparam R_RNG         = 6'd23;
    localparam R_RES0        = 6'd24;
    localparam R_RES1        = 6'd25;
    localparam R_RES2        = 6'd26;
    localparam R_RES3        = 6'd27;
    localparam R_CAPS0       = 6'd28;
    localparam R_CAPS1       = 6'd29;
    localparam R_STATUS      = 6'd30;
    localparam R_VERSION     = 6'd31;
    localparam R_VEC_AX_LO   = 6'd32;
    localparam R_VEC_AX_HI   = 6'd33;
    localparam R_VEC_AY_LO   = 6'd34;
    localparam R_VEC_AY_HI   = 6'd35;
    localparam R_VEC_BX_LO   = 6'd36;
    localparam R_VEC_BX_HI   = 6'd37;
    localparam R_VEC_BY_LO   = 6'd38;
    localparam R_VEC_BY_HI   = 6'd39;
    localparam R_VEC_S_LO    = 6'd40;
    localparam R_VEC_S_HI    = 6'd41;
    localparam R_SQRT_LO     = 6'd42;
    localparam R_SQRT_HI     = 6'd43;
    localparam R_VEC_OP      = 6'd46;

    localparam logic [31:0] RNG_DEFAULT = 32'hDEADBEEF;
    localparam logic signed [31:0] FX_MAX = 32'sd32767;
    localparam logic signed [31:0] FX_MIN = -32'sd32768;
    localparam logic [7:0] MATH_VERSION = 8'd2;
    localparam logic [7:0] MATH_CAPS0 =
        8'b1111_1111; // MUL16 | MULFX | SINCOS | DIST | RNG | DIV | ATAN2 | SQRT
    localparam logic [7:0] MATH_CAPS1 =
        8'b0001_1111; // VEC_DOT_S16 | VEC_DOT_FX | VEC_CROSS | VEC_LEN2 | VEC_SCALE_FX

    localparam logic [7:0] VEC_OP_DOT_S16   = 8'h01;
    localparam logic [7:0] VEC_OP_DOT_FX    = 8'h02;
    localparam logic [7:0] VEC_OP_CROSS_S16 = 8'h03;
    localparam logic [7:0] VEC_OP_LEN2      = 8'h04;
    localparam logic [7:0] VEC_OP_SCALE_FX  = 8'h05;

    localparam logic [7:0] STATUS_OK            = 8'h00;
    localparam logic [7:0] STATUS_DIV_ZERO      = 8'h01;
    localparam logic [7:0] STATUS_OVERFLOW      = 8'h02;
    localparam logic [7:0] STATUS_UNIMPLEMENTED = 8'h80;

    typedef enum logic [3:0] {
        OP_NONE,
        OP_MUL16,
        OP_MULFX,
        OP_DIV,
        OP_SINCOS,
        OP_ATAN2,
        OP_DIST,
        OP_SQRT,
        OP_VEC_DOT_S16,
        OP_VEC_DOT_FX,
        OP_VEC_CROSS_S16,
        OP_VEC_LEN2,
        OP_VEC_SCALE_FX,
        OP_UNIMPL
    } op_t;

    logic [7:0] regs [0:63];
    op_t        op;
    logic [5:0] op_stage;
    logic signed [15:0] op_a16;
    logic signed [15:0] op_b16;
    logic [7:0] op_angle;
    logic signed [31:0] product_reg;
    logic signed [31:0] vec_accum;
    logic [15:0] vec_res_x;
    logic [15:0] dist_ax;
    logic [15:0] dist_ay;
    logic [31:0] rng_state;
    logic [1:0]  rng_seed_index;
    logic signed [15:0] vec_ax;
    logic signed [15:0] vec_ay;
    logic signed [15:0] vec_bx;
    logic signed [15:0] vec_by;
    logic signed [15:0] vec_scalar;
    logic        vec_status_overflow;
    logic [31:0] div_dividend_abs;
    logic [15:0] div_divisor_abs;
    logic [31:0] div_quotient;
    logic [32:0] div_remainder;
    logic        div_quotient_neg;
    logic        div_remainder_neg;
    logic signed [23:0] atan_x;
    logic signed [23:0] atan_y;
    logic signed [9:0]  atan_z;
    logic        atan_zero;
    logic [15:0] sqrt_n;
    logic [15:0] sqrt_res;
    logic [15:0] sqrt_bit;

    wire math_sel = (cpu_addr >= MATH_BASE) && (cpu_addr <= MATH_END);
    wire [5:0] reg_off = cpu_addr[5:0] - MATH_BASE[5:0];
    wire math_sel_r = (cpu_raddr >= MATH_BASE) && (cpu_raddr <= MATH_END);
    wire [5:0] reg_off_r = cpu_raddr[5:0] - MATH_BASE[5:0];
    wire readonly_reg = (reg_off == R_CAPS0) ||
                        (reg_off == R_CAPS1) ||
                        (reg_off == R_STATUS) ||
                        (reg_off == R_VERSION);

    function automatic logic [31:0] xorshift32(input logic [31:0] state);
        logic [31:0] y;
        begin
            y = (state == 32'h00000000) ? RNG_DEFAULT : state;
            y = y ^ (y << 13);
            y = y ^ (y >> 17);
            y = y ^ (y << 5);
            xorshift32 = (y == 32'h00000000) ? RNG_DEFAULT : y;
        end
    endfunction

    function automatic logic [15:0] abs_s16(input logic signed [15:0] value);
        begin
            if (value == -16'sd32768)
                abs_s16 = 16'h8000;
            else if (value < 0)
                abs_s16 = (~value[15:0]) + 16'd1;
            else
                abs_s16 = value[15:0];
        end
    endfunction

    function automatic logic [31:0] abs_s32(input logic signed [31:0] value);
        begin
            if (value == -32'sd2147483648)
                abs_s32 = 32'h80000000;
            else if (value < 0)
                abs_s32 = (~value[31:0]) + 32'd1;
            else
                abs_s32 = value[31:0];
        end
    endfunction

    function automatic logic [7:0] atan_step(input logic [2:0] index);
        begin
            unique case (index)
                3'd0: atan_step = 8'd32;
                3'd1: atan_step = 8'd19;
                3'd2: atan_step = 8'd10;
                3'd3: atan_step = 8'd5;
                3'd4: atan_step = 8'd3;
                3'd5: atan_step = 8'd1;
                3'd6: atan_step = 8'd1;
                default: atan_step = 8'd0;
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

    function automatic logic [7:0] sin_1p7(input logic [7:0] angle);
        logic [6:0] index;
        logic [7:0] mag;
        logic       negate;
        begin
            unique case (angle[7:6])
                2'b00: begin
                    index = {1'b0, angle[5:0]};
                    negate = 1'b0;
                end
                2'b01: begin
                    index = 7'd64 - {1'b0, angle[5:0]};
                    negate = 1'b0;
                end
                2'b10: begin
                    index = {1'b0, angle[5:0]};
                    negate = 1'b1;
                end
                default: begin
                    index = 7'd64 - {1'b0, angle[5:0]};
                    negate = 1'b1;
                end
            endcase

            mag = sin_quarter_mag(index);
            if (negate)
                sin_1p7 = (~mag) + 8'd1;
            else
                sin_1p7 = (mag == 8'd128) ? 8'd127 : mag;
        end
    endfunction

    wire [31:0] rng_next = xorshift32(rng_state);

    always_comb begin
        cpu_rdata = 8'h00;
        if (math_sel_r) begin
            unique case (reg_off_r)
                R_RNG:     cpu_rdata = rng_next[7:0];
                R_CAPS0:   cpu_rdata = MATH_CAPS0;
                R_CAPS1:   cpu_rdata = MATH_CAPS1;
                R_STATUS:  cpu_rdata = regs[R_STATUS];
                R_VERSION: cpu_rdata = MATH_VERSION;
                default:   cpu_rdata = regs[reg_off_r];
            endcase
        end
    end

    assign rdy_out = (op == OP_NONE);

    always_ff @(posedge clk) begin
        logic signed [31:0] shifted;
        logic [15:0] fx_result;
        logic [15:0] maxv;
        logic [15:0] minv;
        logic [16:0] dist_approx;
        logic [31:0] seeded_state;
        logic signed [32:0] vec_sum;
        logic [32:0] vec_usum;
        logic [32:0] div_trial;
        logic [32:0] div_overflow_threshold;
        logic        div_set_bit;
        logic signed [15:0] div_q_signed;
        logic signed [15:0] div_r_signed;
        logic signed [31:0] div_numerator_s;
        logic signed [15:0] div_denominator_s;
        logic signed [15:0] atan_dx_s;
        logic signed [15:0] atan_dy_s;
        logic signed [23:0] atan_shift_x;
        logic signed [23:0] atan_shift_y;
        logic signed [23:0] atan_next_x;
        logic signed [23:0] atan_next_y;

        if (rst) begin
            for (int i = 0; i < 64; i++) regs[i] <= 8'h00;
            op <= OP_NONE;
            op_stage <= 6'd0;
            op_a16 <= 16'sd0;
            op_b16 <= 16'sd0;
            op_angle <= 8'd0;
            product_reg <= 32'sd0;
            vec_accum <= 32'sd0;
            vec_res_x <= 16'd0;
            dist_ax <= 16'd0;
            dist_ay <= 16'd0;
            rng_state <= RNG_DEFAULT;
            rng_seed_index <= 2'd0;
            vec_ax <= 16'sd0;
            vec_ay <= 16'sd0;
            vec_bx <= 16'sd0;
            vec_by <= 16'sd0;
            vec_scalar <= 16'sd0;
            vec_status_overflow <= 1'b0;
            div_dividend_abs <= 32'd0;
            div_divisor_abs <= 16'd0;
            div_quotient <= 32'd0;
            div_remainder <= 33'd0;
            div_quotient_neg <= 1'b0;
            div_remainder_neg <= 1'b0;
            atan_x <= 24'sd0;
            atan_y <= 24'sd0;
            atan_z <= 10'sd0;
            atan_zero <= 1'b0;
            sqrt_n <= 16'd0;
            sqrt_res <= 16'd0;
            sqrt_bit <= 16'd0;
        end else begin
            if (op != OP_NONE) begin
                unique case (op)
                    OP_MUL16: begin
                        if (op_stage == 2'd0) begin
                            product_reg <= op_a16 * op_b16;
                            op_stage <= 2'd1;
                        end else begin
                            regs[R_RES0] <= product_reg[7:0];
                            regs[R_RES1] <= product_reg[15:8];
                            regs[R_RES2] <= product_reg[23:16];
                            regs[R_RES3] <= product_reg[31:24];
                            regs[R_STATUS] <= STATUS_OK;
                            op <= OP_NONE;
                        end
                    end

                    OP_MULFX: begin
                        if (op_stage == 2'd0) begin
                            product_reg <= op_a16 * op_b16;
                            op_stage <= 2'd1;
                        end else begin
                            shifted = product_reg >>> 8;
                            if (shifted > FX_MAX) begin
                                fx_result = 16'h7FFF;
                                regs[R_STATUS] <= STATUS_OVERFLOW;
                            end else if (shifted < FX_MIN) begin
                                fx_result = 16'h8000;
                                regs[R_STATUS] <= STATUS_OVERFLOW;
                            end else begin
                                fx_result = shifted[15:0];
                                regs[R_STATUS] <= STATUS_OK;
                            end
                            regs[R_RES0] <= fx_result[7:0];
                            regs[R_RES1] <= fx_result[15:8];
                            regs[R_RES2] <= 8'h00;
                            regs[R_RES3] <= 8'h00;
                            op <= OP_NONE;
                        end
                    end

                    OP_DIV: begin
                        if (div_divisor_abs == 16'd0) begin
                            regs[R_RES0] <= 8'hFF;
                            regs[R_RES1] <= 8'hFF;
                            regs[R_RES2] <= 8'hFF;
                            regs[R_RES3] <= 8'hFF;
                            regs[R_STATUS] <= STATUS_DIV_ZERO;
                            op <= OP_NONE;
                        end else begin
                            div_overflow_threshold =
                                {17'd0, div_divisor_abs} *
                                {16'd0, (div_quotient_neg ? 17'd32769 : 17'd32768)};
                            if (div_dividend_abs >= div_overflow_threshold) begin
                                if (div_quotient_neg) begin
                                    regs[R_RES0] <= 8'h00;
                                    regs[R_RES1] <= 8'h80;
                                end else begin
                                    regs[R_RES0] <= 8'hFF;
                                    regs[R_RES1] <= 8'h7F;
                                end
                                regs[R_RES2] <= 8'h00;
                                regs[R_RES3] <= 8'h00;
                                regs[R_STATUS] <= STATUS_OVERFLOW;
                                op <= OP_NONE;
                            end else begin
                                div_trial = (div_remainder << 1) | {32'd0, div_dividend_abs[op_stage]};
                                div_set_bit = div_trial >= {17'd0, div_divisor_abs};
                                if (div_set_bit) begin
                                    div_trial = div_trial - {17'd0, div_divisor_abs};
                                    div_remainder <= div_trial;
                                    div_quotient[op_stage] <= 1'b1;
                                end else begin
                                    div_remainder <= div_trial;
                                end

                                if (op_stage == 6'd0) begin
                                    div_q_signed = div_quotient_neg
                                        ? -$signed({1'b0, div_quotient[15:0]})
                                        :  $signed({1'b0, div_quotient[15:0]});
                                    div_r_signed = div_remainder_neg
                                        ? -$signed({1'b0, div_trial[15:0]})
                                        :  $signed({1'b0, div_trial[15:0]});
                                    if (div_set_bit) begin
                                        div_q_signed = div_quotient_neg
                                            ? -$signed({1'b0, (div_quotient[15:0] | 16'h0001)})
                                            :  $signed({1'b0, (div_quotient[15:0] | 16'h0001)});
                                        div_r_signed = div_remainder_neg
                                            ? -$signed({1'b0, div_trial[15:0]})
                                            :  $signed({1'b0, div_trial[15:0]});
                                    end
                                    regs[R_RES0] <= div_q_signed[7:0];
                                    regs[R_RES1] <= div_q_signed[15:8];
                                    regs[R_RES2] <= div_r_signed[7:0];
                                    regs[R_RES3] <= div_r_signed[15:8];
                                    regs[R_STATUS] <= STATUS_OK;
                                    op <= OP_NONE;
                                end else begin
                                    op_stage <= op_stage - 6'd1;
                                end
                            end
                        end
                    end

                    OP_SINCOS: begin
                        regs[R_RES0] <= sin_1p7(op_angle);
                        regs[R_RES1] <= sin_1p7(op_angle + 8'd64);
                        regs[R_RES2] <= 8'h00;
                        regs[R_RES3] <= 8'h00;
                        regs[R_STATUS] <= STATUS_OK;
                        op <= OP_NONE;
                    end

                    OP_ATAN2: begin
                        if (atan_zero) begin
                            if (dist_ax >= dist_ay) begin
                                maxv = dist_ax;
                                minv = dist_ay;
                            end else begin
                                maxv = dist_ay;
                                minv = dist_ax;
                            end
                            dist_approx = {1'b0, maxv} + {1'b0, (minv >> 1)} - {1'b0, (minv >> 3)};
                            regs[R_RES0] <= atan_z[7:0];
                            regs[R_RES1] <= dist_approx[16] ? 8'hFF : dist_approx[7:0];
                            regs[R_RES2] <= dist_approx[16] ? 8'hFF : dist_approx[15:8];
                            regs[R_RES3] <= 8'h00;
                            regs[R_STATUS] <= dist_approx[16] ? STATUS_OVERFLOW : STATUS_OK;
                            op <= OP_NONE;
                        end else if (op_stage < 6'd8) begin
                            atan_shift_x = atan_x >>> op_stage[2:0];
                            atan_shift_y = atan_y >>> op_stage[2:0];
                            if (atan_y > 24'sd0) begin
                                atan_next_x = atan_x + atan_shift_y;
                                atan_next_y = atan_y - atan_shift_x;
                                atan_x <= atan_next_x;
                                atan_y <= atan_next_y;
                                atan_z <= atan_z + $signed({2'b00, atan_step(op_stage[2:0])});
                            end else if (atan_y < 24'sd0) begin
                                atan_next_x = atan_x - atan_shift_y;
                                atan_next_y = atan_y + atan_shift_x;
                                atan_x <= atan_next_x;
                                atan_y <= atan_next_y;
                                atan_z <= atan_z - $signed({2'b00, atan_step(op_stage[2:0])});
                            end
                            op_stage <= op_stage + 6'd1;
                        end else begin
                            if (dist_ax >= dist_ay) begin
                                maxv = dist_ax;
                                minv = dist_ay;
                            end else begin
                                maxv = dist_ay;
                                minv = dist_ax;
                            end
                            dist_approx = {1'b0, maxv} + {1'b0, (minv >> 1)} - {1'b0, (minv >> 3)};
                            regs[R_RES0] <= atan_z[7:0];
                            regs[R_RES1] <= dist_approx[16] ? 8'hFF : dist_approx[7:0];
                            regs[R_RES2] <= dist_approx[16] ? 8'hFF : dist_approx[15:8];
                            regs[R_RES3] <= 8'h00;
                            regs[R_STATUS] <= dist_approx[16] ? STATUS_OVERFLOW : STATUS_OK;
                            op <= OP_NONE;
                        end
                    end

                    OP_DIST: begin
                        if (op_stage == 2'd0) begin
                            dist_ax <= abs_s16(op_a16);
                            dist_ay <= abs_s16(op_b16);
                            op_stage <= 2'd1;
                        end else begin
                            if (dist_ax >= dist_ay) begin
                                maxv = dist_ax;
                                minv = dist_ay;
                            end else begin
                                maxv = dist_ay;
                                minv = dist_ax;
                            end
                            dist_approx = {1'b0, maxv} + {1'b0, (minv >> 1)} - {1'b0, (minv >> 3)};
                            regs[R_RES0] <= dist_approx[16] ? 8'hFF : dist_approx[7:0];
                            regs[R_RES1] <= dist_approx[16] ? 8'hFF : dist_approx[15:8];
                            regs[R_RES2] <= 8'h00;
                            regs[R_RES3] <= 8'h00;
                            regs[R_STATUS] <= dist_approx[16] ? STATUS_OVERFLOW : STATUS_OK;
                            op <= OP_NONE;
                        end
                    end

                    OP_SQRT: begin
                        // Unsigned 16-bit floor integer sqrt (non-restoring,
                        // bit-by-bit). MUST stay bit-identical with
                        // MathCoprocessor.Sqrt() in C#.
                        if (op_stage == 6'd0) begin
                            // Align: shrink bit until bit <= n.
                            if (sqrt_bit > sqrt_n)
                                sqrt_bit <= sqrt_bit >> 2;
                            else
                                op_stage <= 6'd1;
                        end else begin
                            if (sqrt_bit == 16'd0) begin
                                regs[R_RES0] <= sqrt_res[7:0];
                                regs[R_RES1] <= sqrt_res[15:8];
                                regs[R_RES2] <= 8'h00;
                                regs[R_RES3] <= 8'h00;
                                regs[R_STATUS] <= STATUS_OK;
                                op <= OP_NONE;
                            end else begin
                                if (sqrt_n >= (sqrt_res + sqrt_bit)) begin
                                    sqrt_n   <= sqrt_n - (sqrt_res + sqrt_bit);
                                    sqrt_res <= (sqrt_res >> 1) + sqrt_bit;
                                end else begin
                                    sqrt_res <= sqrt_res >> 1;
                                end
                                sqrt_bit <= sqrt_bit >> 2;
                            end
                        end
                    end

                    OP_VEC_DOT_S16,
                    OP_VEC_DOT_FX,
                    OP_VEC_CROSS_S16,
                    OP_VEC_LEN2: begin
                        if (op_stage == 6'd0) begin
                            unique case (op)
                                OP_VEC_CROSS_S16: product_reg <= vec_ax * vec_by;
                                OP_VEC_LEN2:      product_reg <= vec_ax * vec_ax;
                                default:          product_reg <= vec_ax * vec_bx;
                            endcase
                            op_stage <= 6'd1;
                        end else if (op_stage == 6'd1) begin
                            vec_accum <= product_reg;
                            unique case (op)
                                OP_VEC_CROSS_S16: product_reg <= vec_ay * vec_bx;
                                OP_VEC_LEN2:      product_reg <= vec_ay * vec_ay;
                                default:          product_reg <= vec_ay * vec_by;
                            endcase
                            op_stage <= 6'd2;
                        end else begin
                            if (op == OP_VEC_CROSS_S16)
                                vec_sum = {vec_accum[31], vec_accum} - {product_reg[31], product_reg};
                            else
                                vec_sum = {vec_accum[31], vec_accum} + {product_reg[31], product_reg};

                            if (op == OP_VEC_DOT_FX) begin
                                shifted = vec_sum >>> 8;
                                if (shifted > FX_MAX) begin
                                    fx_result = 16'h7FFF;
                                    regs[R_STATUS] <= STATUS_OVERFLOW;
                                end else if (shifted < FX_MIN) begin
                                    fx_result = 16'h8000;
                                    regs[R_STATUS] <= STATUS_OVERFLOW;
                                end else begin
                                    fx_result = shifted[15:0];
                                    regs[R_STATUS] <= STATUS_OK;
                                end
                                regs[R_RES0] <= fx_result[7:0];
                                regs[R_RES1] <= fx_result[15:8];
                                regs[R_RES2] <= 8'h00;
                                regs[R_RES3] <= 8'h00;
                            end else if (op == OP_VEC_LEN2) begin
                                vec_usum = {1'b0, vec_accum[31:0]} + {1'b0, product_reg[31:0]};
                                regs[R_RES0] <= vec_usum[7:0];
                                regs[R_RES1] <= vec_usum[15:8];
                                regs[R_RES2] <= vec_usum[23:16];
                                regs[R_RES3] <= vec_usum[31:24];
                                regs[R_STATUS] <= vec_usum[32] ? STATUS_OVERFLOW : STATUS_OK;
                            end else begin
                                if (vec_sum > 33'sd2147483647) begin
                                    regs[R_RES0] <= 8'hFF;
                                    regs[R_RES1] <= 8'hFF;
                                    regs[R_RES2] <= 8'hFF;
                                    regs[R_RES3] <= 8'h7F;
                                    regs[R_STATUS] <= STATUS_OVERFLOW;
                                end else if (vec_sum < -33'sd2147483648) begin
                                    regs[R_RES0] <= 8'h00;
                                    regs[R_RES1] <= 8'h00;
                                    regs[R_RES2] <= 8'h00;
                                    regs[R_RES3] <= 8'h80;
                                    regs[R_STATUS] <= STATUS_OVERFLOW;
                                end else begin
                                    regs[R_RES0] <= vec_sum[7:0];
                                    regs[R_RES1] <= vec_sum[15:8];
                                    regs[R_RES2] <= vec_sum[23:16];
                                    regs[R_RES3] <= vec_sum[31:24];
                                    regs[R_STATUS] <= STATUS_OK;
                                end
                            end
                            op <= OP_NONE;
                        end
                    end

                    OP_VEC_SCALE_FX: begin
                        if (op_stage == 6'd0) begin
                            product_reg <= vec_ax * vec_scalar;
                            vec_status_overflow <= 1'b0;
                            op_stage <= 6'd1;
                        end else if (op_stage == 6'd1) begin
                            shifted = product_reg >>> 8;
                            if (shifted > FX_MAX) begin
                                vec_res_x <= 16'h7FFF;
                                vec_status_overflow <= 1'b1;
                            end else if (shifted < FX_MIN) begin
                                vec_res_x <= 16'h8000;
                                vec_status_overflow <= 1'b1;
                            end else begin
                                vec_res_x <= shifted[15:0];
                            end
                            product_reg <= vec_ay * vec_scalar;
                            op_stage <= 6'd2;
                        end else begin
                            shifted = product_reg >>> 8;
                            if (shifted > FX_MAX) begin
                                fx_result = 16'h7FFF;
                                regs[R_STATUS] <= STATUS_OVERFLOW;
                            end else if (shifted < FX_MIN) begin
                                fx_result = 16'h8000;
                                regs[R_STATUS] <= STATUS_OVERFLOW;
                            end else begin
                                fx_result = shifted[15:0];
                                regs[R_STATUS] <= vec_status_overflow ? STATUS_OVERFLOW : STATUS_OK;
                            end
                            regs[R_RES0] <= vec_res_x[7:0];
                            regs[R_RES1] <= vec_res_x[15:8];
                            regs[R_RES2] <= fx_result[7:0];
                            regs[R_RES3] <= fx_result[15:8];
                            op <= OP_NONE;
                        end
                    end

                    OP_UNIMPL: begin
                        regs[R_RES0] <= 8'hFF;
                        regs[R_RES1] <= 8'hFF;
                        regs[R_RES2] <= 8'hFF;
                        regs[R_RES3] <= 8'hFF;
                        regs[R_STATUS] <= STATUS_UNIMPLEMENTED;
                        op <= OP_NONE;
                    end

                    default: op <= OP_NONE;
                endcase
            end else begin
                if (cpu_re && math_sel && reg_off == R_RNG) begin
                    rng_state <= rng_next;
                    regs[R_STATUS] <= STATUS_OK;
                end

                if (cpu_we && math_sel) begin
                    if (!readonly_reg)
                        regs[reg_off] <= cpu_wdata;

                    unique case (reg_off)
                        R_MUL16_B_HI: begin
                            op_a16 <= $signed({regs[R_MUL16_A_HI], regs[R_MUL16_A_LO]});
                            op_b16 <= $signed({cpu_wdata, regs[R_MUL16_B_LO]});
                            op_stage <= 6'd0;
                            op <= OP_MUL16;
                        end

                        R_MULFX_B_HI: begin
                            op_a16 <= $signed({regs[R_MULFX_A_HI], regs[R_MULFX_A_LO]});
                            op_b16 <= $signed({cpu_wdata, regs[R_MULFX_B_LO]});
                            op_stage <= 6'd0;
                            op <= OP_MULFX;
                        end

                        R_DIV_D_HI: begin
                            div_numerator_s = $signed({regs[R_DIV_N_HI], regs[R_DIV_N_2], regs[R_DIV_N_1], regs[R_DIV_N_LO]});
                            div_denominator_s = $signed({cpu_wdata, regs[R_DIV_D_LO]});
                            div_dividend_abs <= abs_s32(div_numerator_s);
                            div_divisor_abs <= abs_s16(div_denominator_s);
                            div_quotient <= 32'd0;
                            div_remainder <= 33'd0;
                            div_quotient_neg <= div_numerator_s[31] ^ div_denominator_s[15];
                            div_remainder_neg <= div_numerator_s[31];
                            op_stage <= 6'd31;
                            op <= OP_DIV;
                        end

                        R_DIST_DY_HI: begin
                            op_a16 <= $signed({regs[R_DIST_DX_HI], regs[R_DIST_DX_LO]});
                            op_b16 <= $signed({cpu_wdata, regs[R_DIST_DY_LO]});
                            op_stage <= 6'd0;
                            op <= OP_DIST;
                        end

                        R_SINCOS: begin
                            op_angle <= cpu_wdata;
                            op_stage <= 6'd0;
                            op <= OP_SINCOS;
                        end

                        R_SQRT_HI: begin
                            sqrt_n   <= {cpu_wdata, regs[R_SQRT_LO]};
                            sqrt_res <= 16'd0;
                            sqrt_bit <= 16'h4000; // 4^7, highest power of 4 <= 65535
                            op_stage <= 6'd0;
                            op <= OP_SQRT;
                        end

                        R_ATAN_DX_HI: begin
                            atan_dy_s = $signed({regs[R_ATAN_DY_HI], regs[R_ATAN_DY_LO]});
                            atan_dx_s = $signed({cpu_wdata, regs[R_ATAN_DX_LO]});
                            dist_ax <= abs_s16(atan_dx_s);
                            dist_ay <= abs_s16(atan_dy_s);
                            op_stage <= 6'd0;
                            if (atan_dx_s == 16'sd0 && atan_dy_s == 16'sd0) begin
                                atan_zero <= 1'b1;
                                atan_z <= 10'sd0;
                            end else if (atan_dy_s == 16'sd0) begin
                                atan_zero <= 1'b1;
                                atan_z <= atan_dx_s[15] ? 10'sd128 : 10'sd0;
                            end else if (atan_dx_s == 16'sd0) begin
                                atan_zero <= 1'b1;
                                atan_z <= atan_dy_s[15] ? 10'sd192 : 10'sd64;
                            end else begin
                                atan_zero <= 1'b0;
                                if (atan_dx_s[15]) begin
                                    atan_x <= -{{8{atan_dx_s[15]}}, atan_dx_s};
                                    atan_y <= -{{8{atan_dy_s[15]}}, atan_dy_s};
                                    atan_z <= 10'sd128;
                                end else begin
                                    atan_x <= {{8{atan_dx_s[15]}}, atan_dx_s};
                                    atan_y <= {{8{atan_dy_s[15]}}, atan_dy_s};
                                    atan_z <= 10'sd0;
                                end
                            end
                            op <= OP_ATAN2;
                        end

                        R_RNG: begin
                            seeded_state = rng_state;
                            unique case (rng_seed_index)
                                2'd0: seeded_state[7:0]   = cpu_wdata;
                                2'd1: seeded_state[15:8]  = cpu_wdata;
                                2'd2: seeded_state[23:16] = cpu_wdata;
                                2'd3: seeded_state[31:24] = cpu_wdata;
                            endcase
                            rng_state <= (seeded_state == 32'h00000000) ? RNG_DEFAULT : seeded_state;
                            rng_seed_index <= rng_seed_index + 2'd1;
                        end

                        R_VEC_OP: begin
                            vec_ax <= $signed({regs[R_VEC_AX_HI], regs[R_VEC_AX_LO]});
                            vec_ay <= $signed({regs[R_VEC_AY_HI], regs[R_VEC_AY_LO]});
                            vec_bx <= $signed({regs[R_VEC_BX_HI], regs[R_VEC_BX_LO]});
                            vec_by <= $signed({regs[R_VEC_BY_HI], regs[R_VEC_BY_LO]});
                            vec_scalar <= $signed({regs[R_VEC_S_HI], regs[R_VEC_S_LO]});
                            op_stage <= 6'd0;
                            unique case (cpu_wdata)
                                VEC_OP_DOT_S16:   op <= OP_VEC_DOT_S16;
                                VEC_OP_DOT_FX:    op <= OP_VEC_DOT_FX;
                                VEC_OP_CROSS_S16: op <= OP_VEC_CROSS_S16;
                                VEC_OP_LEN2:      op <= OP_VEC_LEN2;
                                VEC_OP_SCALE_FX:  op <= OP_VEC_SCALE_FX;
                                default:          op <= OP_UNIMPL;
                            endcase
                        end

                        default: ;
                    endcase
                end
            end
        end
    end

endmodule
