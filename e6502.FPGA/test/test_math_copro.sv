// Math coprocessor unit tests.
// Covers the hardware math ABI: MUL16, MUL_FX, DIV, SINCOS, ATAN2,
// DIST_APPROX, RNG, vector ops, and RDY stalls.

`timescale 1ns/1ps

module test_math_copro;

    logic clk = 0;
    always #20 clk = ~clk; // 25 MHz

    logic        rst;
    logic [15:0] cpu_addr;
    logic [7:0]  cpu_wdata;
    logic        cpu_we;
    logic        cpu_re;
    wire  [7:0]  cpu_rdata;
    wire         rdy_out;

    localparam MATH = 16'hBB20;
    localparam MUL16_A_LO = MATH + 0;
    localparam MUL16_A_HI = MATH + 1;
    localparam MUL16_B_LO = MATH + 2;
    localparam MUL16_B_HI = MATH + 3;
    localparam MULFX_A_LO = MATH + 4;
    localparam MULFX_A_HI = MATH + 5;
    localparam MULFX_B_LO = MATH + 6;
    localparam MULFX_B_HI = MATH + 7;
    localparam DIV_N_LO   = MATH + 8;
    localparam DIV_D_LO   = MATH + 12;
    localparam DIV_D_HI   = MATH + 13;
    localparam SINCOS     = MATH + 14;
    localparam ATAN_DY_LO = MATH + 15;
    localparam ATAN_DX_LO = MATH + 17;
    localparam ATAN_DX_HI = MATH + 18;
    localparam DIST_DX_LO = MATH + 19;
    localparam DIST_DX_HI = MATH + 20;
    localparam DIST_DY_LO = MATH + 21;
    localparam DIST_DY_HI = MATH + 22;
    localparam RNG        = MATH + 23;
    localparam RES0       = MATH + 24;
    localparam CAPS0      = MATH + 28;
    localparam CAPS1      = MATH + 29;
    localparam STATUS     = MATH + 30;
    localparam VERSION    = MATH + 31;
    localparam VEC_AX_LO  = MATH + 32;
    localparam VEC_AY_LO  = MATH + 34;
    localparam VEC_BX_LO  = MATH + 36;
    localparam VEC_BY_LO  = MATH + 38;
    localparam VEC_S_LO   = MATH + 40;
    localparam SQRT_LO    = MATH + 42;
    localparam SQRT_HI    = MATH + 43;
    localparam VEC_OP     = MATH + 46;

    localparam VEC_OP_DOT_S16   = 8'h01;
    localparam VEC_OP_DOT_FX    = 8'h02;
    localparam VEC_OP_CROSS_S16 = 8'h03;
    localparam VEC_OP_LEN2      = 8'h04;
    localparam VEC_OP_SCALE_FX  = 8'h05;

    localparam STATUS_OK            = 8'h00;
    localparam STATUS_DIV_ZERO      = 8'h01;
    localparam STATUS_OVERFLOW      = 8'h02;
    localparam STATUS_UNIMPLEMENTED = 8'h80;

    math_copro dut (
        .clk(clk),
        .rst(rst),
        .cpu_addr(cpu_addr),
        .cpu_raddr(cpu_addr),
        .cpu_wdata(cpu_wdata),
        .cpu_we(cpu_we),
        .cpu_re(cpu_re),
        .cpu_rdata(cpu_rdata),
        .rdy_out(rdy_out)
    );

    int pass_count = 0;
    int fail_count = 0;

    task automatic check(input string name, input bit ok);
        if (ok) begin
            pass_count++;
            $display("PASS: %s", name);
        end else begin
            fail_count++;
            $display("FAIL: %s", name);
        end
    endtask

    task automatic check_eq16(input string name, input logic [15:0] got, input logic [15:0] exp);
        check($sformatf("%s got=$%04h exp=$%04h", name, got, exp), got === exp);
    endtask

    task automatic check_eq8(input string name, input logic [7:0] got, input logic [7:0] exp);
        check($sformatf("%s got=$%02h exp=$%02h", name, got, exp), got === exp);
    endtask

    task automatic check_eq32(input string name, input logic [31:0] got, input logic [31:0] exp);
        check($sformatf("%s got=$%08h exp=$%08h", name, got, exp), got === exp);
    endtask

    task automatic write_reg(input logic [15:0] addr, input logic [7:0] data);
        @(negedge clk);
        cpu_addr <= addr;
        cpu_wdata <= data;
        cpu_we <= 1'b1;
        cpu_re <= 1'b0;
        @(negedge clk);
        cpu_we <= 1'b0;
    endtask

    task automatic read_reg(input logic [15:0] addr, output logic [7:0] data);
        @(negedge clk);
        cpu_addr <= addr;
        cpu_we <= 1'b0;
        cpu_re <= 1'b1;
        #1 data = cpu_rdata;
        @(posedge clk);
        cpu_re <= 1'b0;
    endtask

    task automatic wait_ready();
        while (rdy_out !== 1'b1) @(posedge clk);
        @(posedge clk);
    endtask

    task automatic read_result32(output logic [31:0] value);
        logic [7:0] b0, b1, b2, b3;
        read_reg(RES0 + 0, b0);
        read_reg(RES0 + 1, b1);
        read_reg(RES0 + 2, b2);
        read_reg(RES0 + 3, b3);
        value = {b3, b2, b1, b0};
    endtask

    task automatic read_result16(output logic [15:0] value);
        logic [7:0] b0, b1;
        read_reg(RES0 + 0, b0);
        read_reg(RES0 + 1, b1);
        value = {b1, b0};
    endtask

    task automatic read_result_hi16(output logic [15:0] value);
        logic [7:0] b0, b1;
        read_reg(RES0 + 2, b0);
        read_reg(RES0 + 3, b1);
        value = {b1, b0};
    endtask

    task automatic read_result_hypot(output logic [15:0] value);
        logic [7:0] b0, b1;
        read_reg(RES0 + 1, b0);
        read_reg(RES0 + 2, b1);
        value = {b1, b0};
    endtask

    task automatic read_status(output logic [7:0] value);
        read_reg(STATUS, value);
    endtask

    task automatic write_word(input logic [15:0] addr, input logic [15:0] value);
        write_reg(addr + 0, value[7:0]);
        write_reg(addr + 1, value[15:8]);
    endtask

    task automatic write_dword(input logic [15:0] addr, input logic [31:0] value);
        write_reg(addr + 0, value[7:0]);
        write_reg(addr + 1, value[15:8]);
        write_reg(addr + 2, value[23:16]);
        write_reg(addr + 3, value[31:24]);
    endtask

    task automatic mul16(input logic [15:0] a, input logic [15:0] b, output logic [31:0] value);
        write_reg(MUL16_A_LO, a[7:0]);
        write_reg(MUL16_A_HI, a[15:8]);
        write_reg(MUL16_B_LO, b[7:0]);
        write_reg(MUL16_B_HI, b[15:8]);
        check("MUL16 trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result32(value);
    endtask

    task automatic mulfx(input logic [15:0] a, input logic [15:0] b, output logic [15:0] value);
        write_reg(MULFX_A_LO, a[7:0]);
        write_reg(MULFX_A_HI, a[15:8]);
        write_reg(MULFX_B_LO, b[7:0]);
        write_reg(MULFX_B_HI, b[15:8]);
        check("MUL_FX trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result16(value);
    endtask

    task automatic run_dist(input logic [15:0] dx, input logic [15:0] dy, output logic [15:0] value);
        write_reg(DIST_DX_LO, dx[7:0]);
        write_reg(DIST_DX_HI, dx[15:8]);
        write_reg(DIST_DY_LO, dy[7:0]);
        write_reg(DIST_DY_HI, dy[15:8]);
        check("DIST_APPROX trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result16(value);
    endtask

    task automatic run_div(input logic [31:0] numerator, input logic [15:0] denominator, output logic [15:0] quot, output logic [15:0] rem);
        write_dword(DIV_N_LO, numerator);
        write_word(DIV_D_LO, denominator);
        check("DIV trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result16(quot);
        read_result_hi16(rem);
    endtask

    task automatic run_atan2(input logic [15:0] dy, input logic [15:0] dx, output logic [7:0] angle, output logic [15:0] hypot);
        write_word(ATAN_DY_LO, dy);
        write_word(ATAN_DX_LO, dx);
        check("ATAN2 trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_reg(RES0, angle);
        read_result_hypot(hypot);
    endtask

    task automatic write_vector(input logic [15:0] ax, input logic [15:0] ay, input logic [15:0] bx, input logic [15:0] by, input logic [15:0] scalar);
        write_word(VEC_AX_LO, ax);
        write_word(VEC_AY_LO, ay);
        write_word(VEC_BX_LO, bx);
        write_word(VEC_BY_LO, by);
        write_word(VEC_S_LO, scalar);
    endtask

    task automatic run_vec32(input logic [7:0] vecop, output logic [31:0] value);
        write_reg(VEC_OP, vecop);
        check("Vector trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result32(value);
    endtask

    task automatic run_vec_words(input logic [7:0] vecop, output logic [15:0] lo_word, output logic [15:0] hi_word);
        write_reg(VEC_OP, vecop);
        check("Vector trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result16(lo_word);
        read_result_hi16(hi_word);
    endtask

    task automatic run_sqrt(input logic [15:0] n, output logic [15:0] value);
        write_reg(SQRT_LO, n[7:0]);
        write_reg(SQRT_HI, n[15:8]);   // high byte triggers the op
        check("SQRT trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result16(value);
    endtask

    task automatic sincos(input logic [7:0] angle, output logic [7:0] sin_value, output logic [7:0] cos_value);
        write_reg(SINCOS, angle);
        check("SINCOS trigger stalls CPU until result is ready", rdy_out === 1'b0);
        wait_ready();
        read_reg(RES0 + 0, sin_value);
        read_reg(RES0 + 1, cos_value);
    endtask

    initial begin
        logic [31:0] r32;
        logic [15:0] r16;
        logic [15:0] r16b;
        logic [7:0]  r8;
        logic [7:0]  sin8;
        logic [7:0]  cos8;

        $display("=== Math Coprocessor Unit Tests ===");

        rst = 1;
        cpu_addr = 16'h0000;
        cpu_wdata = 8'h00;
        cpu_we = 1'b0;
        cpu_re = 1'b0;
        repeat(4) @(posedge clk);
        rst = 0;
        repeat(2) @(posedge clk);

        check("RDY high after reset", rdy_out === 1'b1);
        read_reg(VERSION, r8);
        check_eq8("VERSION exposes math ABI v2", r8, 8'h02);
        read_reg(CAPS0, r8);
        check_eq8("CAPS0 exposes implemented scalar ops", r8, 8'hFF);
        read_reg(CAPS1, r8);
        check_eq8("CAPS1 exposes vector ops", r8, 8'h1F);
        read_status(r8);
        check_eq8("STATUS is clear after reset", r8, STATUS_OK);

        write_reg(VERSION, 8'h99);
        write_reg(CAPS0, 8'h00);
        write_reg(STATUS, 8'hFF);
        read_reg(VERSION, r8);
        check_eq8("VERSION is read-only", r8, 8'h02);
        read_reg(CAPS0, r8);
        check_eq8("CAPS0 is read-only", r8, 8'hFF);
        read_status(r8);
        check_eq8("STATUS is read-only", r8, STATUS_OK);

        mul16(16'd300, 16'hFFF9, r32);      // 300 * -7
        check_eq32("MUL16 signed product", r32, 32'hFFFFF7CC);
        read_status(r8);
        check_eq8("MUL16 leaves STATUS ok", r8, STATUS_OK);

        mul16(16'h8000, 16'h8000, r32);     // -32768 * -32768
        check_eq32("MUL16 signed edge", r32, 32'h40000000);

        mulfx(16'h0180, 16'h0180, r16);     // 1.5 * 1.5 = 2.25
        check_eq16("MUL_FX ordinary Q8.8 product", r16, 16'h0240);
        read_status(r8);
        check_eq8("MUL_FX ordinary product leaves STATUS ok", r8, STATUS_OK);

        mulfx(16'h6400, 16'h0200, r16);     // 100.0 * 2.0 saturates positive
        check_eq16("MUL_FX positive saturation", r16, 16'h7FFF);
        read_status(r8);
        check_eq8("MUL_FX positive saturation sets STATUS overflow", r8, STATUS_OVERFLOW);

        mulfx(16'h9C00, 16'h0200, r16);     // -100.0 * 2.0 saturates negative
        check_eq16("MUL_FX negative saturation", r16, 16'h8000);
        read_status(r8);
        check_eq8("MUL_FX negative saturation sets STATUS overflow", r8, STATUS_OVERFLOW);

        sincos(8'd0, sin8, cos8);
        check_eq8("SINCOS angle 0 sin", sin8, 8'h00);
        check_eq8("SINCOS angle 0 cos", cos8, 8'h7F);

        sincos(8'd32, sin8, cos8);
        check_eq8("SINCOS angle 32 sin", sin8, 8'h5B);
        check_eq8("SINCOS angle 32 cos", cos8, 8'h5B);

        sincos(8'd64, sin8, cos8);
        check_eq8("SINCOS angle 64 sin", sin8, 8'h7F);
        check_eq8("SINCOS angle 64 cos", cos8, 8'h00);

        sincos(8'd128, sin8, cos8);
        check_eq8("SINCOS angle 128 sin", sin8, 8'h00);
        check_eq8("SINCOS angle 128 cos", cos8, 8'h80);

        sincos(8'd192, sin8, cos8);
        check_eq8("SINCOS angle 192 sin", sin8, 8'h80);
        check_eq8("SINCOS angle 192 cos", cos8, 8'h00);

        run_dist(16'd3, 16'd4, r16);
        check_eq16("DIST_APPROX 3/4/5", r16, 16'd5);
        read_status(r8);
        check_eq8("DIST_APPROX leaves STATUS ok", r8, STATUS_OK);

        run_dist(16'hFFF6, 16'd4, r16);     // dx=-10, dy=4 => 10 + 2
        check_eq16("DIST_APPROX handles signed deltas", r16, 16'd12);

        run_div(32'hFFFFFF9C, 16'd7, r16, r16b); // -100 / 7 = -14 rem -2
        check_eq16("DIV signed quotient", r16, 16'hFFF2);
        check_eq16("DIV signed remainder", r16b, 16'hFFFE);
        read_status(r8);
        check_eq8("DIV ordinary quotient leaves STATUS ok", r8, STATUS_OK);

        run_div(32'd42, 16'd0, r16, r16b);
        read_result32(r32);
        check_eq32("DIV zero returns all ones", r32, 32'hFFFFFFFF);
        read_status(r8);
        check_eq8("DIV zero sets STATUS div-zero", r8, STATUS_DIV_ZERO);

        run_div(32'd1000000, 16'd1, r16, r16b);
        check_eq16("DIV overflow saturates positive quotient", r16, 16'h7FFF);
        read_status(r8);
        check_eq8("DIV overflow sets STATUS overflow", r8, STATUS_OVERFLOW);

        run_atan2(16'd0, 16'd100, r8, r16);
        check_eq8("ATAN2 east angle", r8, 8'd0);
        check_eq16("ATAN2 east hypot", r16, 16'd100);
        run_atan2(16'd100, 16'd0, r8, r16);
        check_eq8("ATAN2 north angle", r8, 8'd64);
        check_eq16("ATAN2 north hypot", r16, 16'd100);
        run_atan2(16'hFF9C, 16'd100, r8, r16);
        check_eq8("ATAN2 southeast angle", r8, 8'd224);
        check_eq16("ATAN2 diagonal hypot", r16, 16'd138);

        write_vector(16'h0180, 16'h0100, 16'h0200, 16'hFF00, 16'h0080);
        run_vec_words(VEC_OP_DOT_FX, r16, r16b);
        check_eq16("VEC DOT_FX returns Q8.8 dot", r16, 16'h0200);
        read_status(r8);
        check_eq8("VEC DOT_FX leaves STATUS ok", r8, STATUS_OK);
        run_vec32(VEC_OP_DOT_S16, r32);
        check_eq32("VEC DOT_S16 returns signed 32-bit dot", r32, 32'h00020000);
        run_vec32(VEC_OP_CROSS_S16, r32);
        check_eq32("VEC CROSS_S16 returns signed 32-bit cross", r32, 32'hFFFC8000);
        run_vec32(VEC_OP_LEN2, r32);
        check_eq32("VEC LEN2 returns unsigned length squared", r32, 32'h00034000);
        run_vec_words(VEC_OP_SCALE_FX, r16, r16b);
        check_eq16("VEC SCALE_FX x", r16, 16'h00C0);
        check_eq16("VEC SCALE_FX y", r16b, 16'h0080);

        run_sqrt(16'd0, r16);
        check_eq16("SQRT 0", r16, 16'd0);
        read_status(r8);
        check_eq8("SQRT leaves STATUS ok", r8, STATUS_OK);
        run_sqrt(16'd1, r16);
        check_eq16("SQRT 1", r16, 16'd1);
        run_sqrt(16'd9, r16);
        check_eq16("SQRT 9", r16, 16'd3);
        run_sqrt(16'd15, r16);
        check_eq16("SQRT 15 floor", r16, 16'd3);
        run_sqrt(16'd16, r16);
        check_eq16("SQRT 16", r16, 16'd4);
        run_sqrt(16'd144, r16);
        check_eq16("SQRT 144", r16, 16'd12);
        run_sqrt(16'd65535, r16);
        check_eq16("SQRT 65535", r16, 16'd255);

        write_reg(RNG, 8'h78);
        write_reg(RNG, 8'h56);
        write_reg(RNG, 8'h34);
        write_reg(RNG, 8'h12);
        read_reg(RNG, r8);
        check($sformatf("RNG seeded xorshift byte got=$%02h exp=$a5", r8), r8 === 8'hA5);
        read_status(r8);
        check_eq8("RNG read leaves STATUS ok", r8, STATUS_OK);

        write_reg(VEC_OP, 8'hFF);
        check("Unknown vector trigger stalls until unimplemented result is ready", rdy_out === 1'b0);
        wait_ready();
        read_result32(r32);
        check_eq32("Unknown vector op returns unimplemented result", r32, 32'hFFFFFFFF);
        read_status(r8);
        check_eq8("Unknown vector op sets STATUS unimplemented", r8, STATUS_UNIMPLEMENTED);

        $display("=== Math Coprocessor Tests: %0d passed, %0d failed ===", pass_count, fail_count);
        if (fail_count != 0)
            $fatal(1, "math coprocessor tests failed");
        $finish;
    end

endmodule
