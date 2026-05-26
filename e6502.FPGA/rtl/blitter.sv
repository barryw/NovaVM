// Blitter — 2D rectangular DMA engine
// Registers at $BA83-$BA9B plus $BAA2. Copy, fill, color-key, and rotate modes.
// Operates across CPU RAM, char, color, gfx, sprite, XRAM, and text attributes.
// Stalls CPU via rdy_out during operation for maximum throughput.
// Row buffer for safe same-space overlapping copies. Range and write-protect validation.

module blitter (
    input  logic        clk,
    input  logic        rst,

    // CPU bus (snooped for register writes)
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    output logic [7:0]  cpu_rdata,
    input  logic        cpu_re,

    // CPU stall (active low — directly drives CPU RDY)
    output logic        rdy_out,

    // Memory port A: CPU RAM (64KB)
    output logic [15:0] ram_addr,
    input  logic [7:0]  ram_rdata,
    output logic [7:0]  ram_wdata,
    output logic        ram_we,

    // Memory port B: XRAM (512KB)
    // xram_busy signals that the SDRAM-backed XRAM wrapper cannot accept a
    // write this cycle. The blitter must hold xram_we + xram_addr + xram_wdata
    // stable until xram_busy drops; otherwise the byte is silently dropped
    // (top.sv gates `bm_xram_fire = bm_xram_we && !xram_busy`). Manifests as
    // STASH/FETCH round-trip dropping bytes at any length > ~1 byte.
    //
    // xram_re fires a 1-cycle pulse on S_READ when src=XRAM. The wrapper
    // latches in S_IDLE, transitions to S_ACTIVE (busy=1) for HOLD_CYCLES,
    // then S_CAPTURE latches r_dout, then back to S_IDLE (busy=0). The
    // blitter stalls in S_READ_WAIT while xram_busy is high — data on
    // xram_rdata becomes valid the cycle after busy drops.
    output logic [18:0] xram_addr,
    input  logic [7:0]  xram_rdata,
    output logic [7:0]  xram_wdata,
    output logic        xram_we,
    output logic        xram_re,
    input  logic        xram_busy,

    // Memory port C: VGC internal (char/color/gfx/sprite/text attributes)
    output logic [2:0]  vgc_space,
    output logic [16:0] vgc_addr,     // shared VGC-space address bus
    input  logic [7:0]  vgc_rdata,
    output logic [7:0]  vgc_wdata,
    output logic        vgc_we,
    output logic        vgc_re,
    input  logic        video_write_safe
);

    // =========================================================================
    // Constants
    // =========================================================================
    localparam BLT_BASE = 16'hBA83;
    localparam BLT_END  = 16'hBA9B;
    localparam BLT_ROTANGLE_ADDR = 16'hBAA2;

    localparam R_CMD       = 0;   localparam R_STATUS    = 1;
    localparam R_ERRCODE   = 2;   localparam R_SRCSPACE  = 3;
    localparam R_DSTSPACE  = 4;   localparam R_SRCL      = 5;
    localparam R_SRCM      = 6;   localparam R_SRCH      = 7;
    localparam R_DSTL      = 8;   localparam R_DSTM      = 9;
    localparam R_DSTH      = 10;  localparam R_WIDTHL    = 11;
    localparam R_WIDTHH    = 12;  localparam R_HEIGHTL   = 13;
    localparam R_HEIGHTH   = 14;  localparam R_SRCSTRIDEL = 15;
    localparam R_SRCSTRIDEH = 16; localparam R_DSTSTRIDEL = 17;
    localparam R_DSTSTRIDEH = 18; localparam R_MODE      = 19;
    localparam R_FILLVAL   = 20;  localparam R_COLORKEY  = 21;
    localparam R_COUNTL    = 22;  localparam R_COUNTM    = 23;
    localparam R_COUNTH    = 24;
    localparam R_ROTANGLE  = 25;
    localparam REG_COUNT   = 26;

    localparam ST_IDLE = 8'h00, ST_BUSY = 8'h01, ST_OK = 8'h02, ST_ERROR = 8'h03;
    localparam ERR_NONE = 8'h00, ERR_BADCMD = 8'h01, ERR_BADSPACE = 8'h02;
    localparam ERR_RANGE = 8'h03, ERR_BADARGS = 8'h04, ERR_WRITEPROT = 8'h05;

    localparam SPACE_CPU = 3'd0, SPACE_CHAR = 3'd1, SPACE_COLOR = 3'd2;
    localparam SPACE_GFX = 3'd3, SPACE_SPRITE = 3'd4, SPACE_XRAM = 3'd5;
    localparam SPACE_TEXTATTR = 3'd7;

    localparam ROM_BASE = 24'hC000;

    localparam MODE_FILL = 0, MODE_COLORKEY = 1, MODE_ROTATE = 2;

    // Row buffer: 1024 bytes max width for same-space overlap protection
    localparam ROW_BUF_SIZE = 1024;

    // =========================================================================
    // Registers + state
    // =========================================================================
    logic [7:0] regs [0:REG_COUNT-1];

    typedef enum logic [4:0] {
        S_IDLE               = 5'd0,
        S_VALIDATE           = 5'd1,
        S_READ               = 5'd2, // issue read (dpram latches addr at posedge)
        S_READ_WAIT          = 5'd3, // dpram dout settles; capture read_byte
        S_WRITE              = 5'd4,
        S_ROWBUF_READ        = 5'd5, // buffered mode: issue read into buffer
        S_ROWBUF_READ_WAIT   = 5'd6, // dpram dout settles; capture row_buf[buf_idx]
        S_ROWBUF_WRITE       = 5'd7, // buffered mode: write row from buffer
        S_DONE               = 5'd8,
        S_ROWBUF_WRITE_LOAD  = 5'd9, // latch row_buf[buf_idx] before driving memory ports
        S_VALIDATE_SRC       = 5'd10,
        S_VALIDATE_DST       = 5'd11,
        S_VALIDATE_START     = 5'd12,
        S_VALIDATE_SRC_CHECK = 5'd13,
        S_VALIDATE_DST_CHECK = 5'd14,
        S_ROT_SETUP_COS      = 5'd15,
        S_ROT_CAPTURE_COS    = 5'd16,
        S_ROT_CAPTURE_SIN    = 5'd17,
        S_ROT_SAMPLE         = 5'd18,
        S_ROT_ADDR_ROW       = 5'd19,
        S_ROT_ADDR_ADD       = 5'd20
    } state_t;

    state_t state;
    logic        fill_mode, colorkey_mode, rotate_mode, use_row_buffer;
    logic [2:0]  src_space, dst_space;
    logic [23:0] src_base, dst_base;
    logic [15:0] width, height;
    logic [15:0] last_col, last_row;
    logic [15:0] src_stride, dst_stride;
    logic [7:0]  fill_value, color_key;
    logic [7:0]  rotate_angle;
    logic [15:0] row, col;
    logic [23:0] wrote_count;
    logic [7:0]  read_byte;
    logic        read_valid;
    logic [31:0] validate_row_start;
    logic [31:0] validate_limit;
    logic [15:0] validate_remaining;
    logic [15:0] validate_stride;
    logic [15:0] validate_width;
    logic        validate_base_in_range;
    logic [31:0] validate_last_byte;
    logic [31:0] validate_src_start;
    logic [31:0] validate_src_end;

    logic signed [7:0]  rot_sin_reg, rot_cos_reg;
    logic signed [18:0] rot_center_fp_reg;
    logic signed [18:0] rot_row_x_fp, rot_row_y_fp;
    logic signed [18:0] rot_cur_x_fp, rot_cur_y_fp;
    logic signed [18:0] rot_x_step_fp, rot_y_step_fp;
    logic signed [18:0] rot_row_x_step_fp, rot_row_y_step_fp;
    logic signed [9:0]  rot_src_x_int, rot_src_y_int;
    logic [23:0] rot_src_row_off;
    logic [23:0] rot_src_addr;
    logic signed [35:0] rot_center_cos;
    logic signed [18:0] rot_mul_a;
    logic signed [16:0] rot_mul_b;
    wire signed [35:0] rot_mul_product = rot_mul_a * rot_mul_b;

    // Row buffer for same-space copies
    logic [7:0]  row_buf [0:ROW_BUF_SIZE-1];
    logic [9:0]  buf_idx;

    initial begin
        for (int i = 0; i < REG_COUNT; i++) regs[i] = 0;
        regs[R_STATUS] = ST_IDLE;
        state = S_IDLE;
        fill_mode = 0; colorkey_mode = 0; rotate_mode = 0; use_row_buffer = 0;
        src_space = 0; dst_space = 0;
        src_base = 0; dst_base = 0;
        width = 0; height = 0; last_col = 0; last_row = 0;
        src_stride = 0; dst_stride = 0;
        fill_value = 0; color_key = 0; rotate_angle = 0;
        row = 0; col = 0; wrote_count = 0;
        read_byte = 0; read_valid = 0; buf_idx = 0;
        src_row_off = 0; dst_row_off = 0;
        validate_row_start = 0; validate_limit = 0;
        validate_remaining = 0; validate_stride = 0; validate_width = 0;
        validate_last_byte = 0;
        validate_base_in_range = 0;
        validate_src_start = 0;
        validate_src_end = 0;
        rot_sin_reg = 0; rot_cos_reg = 0; rot_center_fp_reg = 0;
        rot_row_x_fp = 0; rot_row_y_fp = 0;
        rot_cur_x_fp = 0; rot_cur_y_fp = 0;
        rot_x_step_fp = 0; rot_y_step_fp = 0;
        rot_row_x_step_fp = 0; rot_row_y_step_fp = 0;
        rot_src_x_int = 0; rot_src_y_int = 0;
        rot_src_row_off = 0; rot_src_addr = 0;
        rot_center_cos = 0; rot_mul_a = 0; rot_mul_b = 0;
    end

    // =========================================================================
    // Address decode + CPU read
    // =========================================================================
    wire blt_core_sel = (cpu_addr >= BLT_BASE && cpu_addr <= BLT_END);
    wire blt_rot_sel = (cpu_addr == BLT_ROTANGLE_ADDR);
    wire blt_sel = blt_core_sel || blt_rot_sel;
    wire [4:0] reg_off = blt_rot_sel ? 5'(R_ROTANGLE) : (cpu_addr - BLT_BASE);

    always_comb begin
        cpu_rdata = 8'h00;
        if (blt_sel) cpu_rdata = regs[reg_off];
    end

    // =========================================================================
    // Address computation
    //
    // A straight combinational `base + row*stride + col` synthesized into a
    // MULT18X18D plus a long CCU2 carry chain in the pixel-clock domain —
    // its worst path missed 25 MHz timing by ~0.7 ns. Replacing the runtime
    // multiply with an accumulator (`row_off += stride` on each row step)
    // drops the path to a pure 24-bit add and closes timing.
    //
    // src_row_off / dst_row_off are zeroed at blit start (same instant as
    // `row <= 0`) and incremented by stride whenever `row` advances.
    // =========================================================================
    logic [23:0] src_row_off, dst_row_off;
    wire  [23:0] src_addr = src_base + src_row_off + {8'b0, col};
    wire  [23:0] dst_addr = dst_base + dst_row_off + {8'b0, col};

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

    wire signed [7:0]  rot_sin_lut = sin_1p7(rotate_angle);
    wire signed [7:0]  rot_cos_lut = sin_1p7(rotate_angle + 8'd64);
    wire signed [18:0] rot_center_fp = $signed({1'b0, last_col[7:0], 7'b0});
    wire signed [18:0] rot_sin_step_fp =
        $signed({{10{rot_sin_reg[7]}}, rot_sin_reg, 1'b0});
    wire signed [18:0] rot_cos_step_fp =
        $signed({{10{rot_cos_reg[7]}}, rot_cos_reg, 1'b0});
    wire signed [9:0]  rot_cur_x_int = (rot_cur_x_fp + 19'sd128) >>> 8;
    wire signed [9:0]  rot_cur_y_int = (rot_cur_y_fp + 19'sd128) >>> 8;
    wire signed [9:0]  rot_last_col = $signed({2'b0, last_col[7:0]});
    wire signed [9:0]  rot_last_row = $signed({2'b0, last_row[7:0]});
    wire rot_cur_oob = (rot_cur_x_int < 0) || (rot_cur_y_int < 0) ||
                       (rot_cur_x_int > rot_last_col) || (rot_cur_y_int > rot_last_row);
    wire [23:0] effective_src_addr = rotate_mode ? rot_src_addr : src_addr;

    // Space size lookup
    function automatic logic [19:0] space_size(input logic [2:0] sp);
        case (sp)
            SPACE_CPU:    space_size = 20'(65536);
            SPACE_CHAR:   space_size = 20'(4000);
            SPACE_COLOR:  space_size = 20'(4000);
            SPACE_GFX:    space_size = 20'(64000);
            SPACE_SPRITE: space_size = 20'(32768);
            SPACE_XRAM:   space_size = 20'(524288);
            SPACE_TEXTATTR: space_size = 20'(4000);
            default:      space_size = 0;
        endcase
    endfunction

    function automatic logic is_video_space(input logic [2:0] sp);
        case (sp)
            SPACE_CHAR, SPACE_COLOR, SPACE_GFX, SPACE_SPRITE, SPACE_TEXTATTR:
                is_video_space = 1'b1;
            default:
                is_video_space = 1'b0;
        endcase
    endfunction

    wire write_skipped_by_colorkey = colorkey_mode && !rotate_mode && read_byte == color_key;
    wire video_write_wait = is_video_space(dst_space) && !video_write_safe &&
                            read_valid && !write_skipped_by_colorkey;

    // =========================================================================
    // Memory read data mux
    // =========================================================================
    wire [7:0] mem_read_data =
        (src_space == SPACE_CPU)  ? ram_rdata :
        (src_space == SPACE_XRAM) ? xram_rdata :
                                    vgc_rdata;

    // =========================================================================
    // Memory port outputs (combinational)
    // =========================================================================
    always_comb begin
        ram_addr = 0; ram_we = 0; ram_wdata = 0;
        xram_addr = 0; xram_we = 0; xram_wdata = 0; xram_re = 0;
        vgc_space = 0; vgc_addr = 0; vgc_we = 0; vgc_wdata = 0; vgc_re = 0;

        // Read address setup — hold addr stable across S_READ → S_READ_WAIT
        // and S_ROWBUF_READ → S_ROWBUF_READ_WAIT so dpram port B has time
        // to latch + produce valid dout. One-cycle dpram latency would
        // otherwise capture stale data on the S_*→S_*_WAIT transition.
        if (state == S_READ        || state == S_READ_WAIT ||
            state == S_ROWBUF_READ || state == S_ROWBUF_READ_WAIT) begin
            if (!fill_mode) begin
                case (src_space)
                    SPACE_CPU:  ram_addr = effective_src_addr[15:0];
                    SPACE_XRAM: begin
                        xram_addr = effective_src_addr[18:0];
                        // Pulse read enable only on the first S_READ cycle
                        // (not S_READ_WAIT). The wrapper latches the addr
                        // when in S_IDLE and ignores subsequent asserts
                        // during its internal S_ACTIVE / S_CAPTURE, so the
                        // 1-cycle pulse is sufficient and avoids re-firing.
                        if (state == S_READ || state == S_ROWBUF_READ)
                            xram_re = 1;
                    end
                    default: begin
                        vgc_space = src_space;
                        vgc_addr = effective_src_addr[16:0];
                        vgc_re = 1;
                    end
                endcase
            end
        end

        // Write data setup (for S_WRITE and S_ROWBUF_WRITE)
        if ((state == S_WRITE && read_valid && !write_skipped_by_colorkey) ||
            (state == S_ROWBUF_WRITE && read_valid && !write_skipped_by_colorkey)) begin
            case (dst_space)
                SPACE_CPU: begin
                    ram_addr = dst_addr[15:0];
                    ram_wdata = read_byte;
                    ram_we = 1;
                end
                SPACE_XRAM: begin
                    xram_addr = dst_addr[18:0];
                    xram_wdata = read_byte;
                    xram_we = 1;
                end
                default: begin
                    vgc_space = dst_space;
                    vgc_addr = dst_addr[16:0];
                    vgc_wdata = read_byte;
                    vgc_we = !video_write_wait;
                end
            endcase
        end
    end

    // RDY: stall CPU when blitter active
    assign rdy_out = (state == S_IDLE || state == S_DONE);

    // =========================================================================
    // Main state machine
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            for (int i = 0; i < REG_COUNT; i++)
                regs[i] <= 8'h00;
            state <= S_IDLE;
            regs[R_STATUS] <= ST_IDLE;
            regs[R_ERRCODE] <= ERR_NONE;
            fill_mode <= 0;
            colorkey_mode <= 0;
            rotate_mode <= 0;
            use_row_buffer <= 0;
            src_space <= 0;
            dst_space <= 0;
            src_base <= 0;
            dst_base <= 0;
            width <= 0;
            height <= 0;
            last_col <= 0;
            last_row <= 0;
            src_stride <= 0;
            dst_stride <= 0;
            fill_value <= 0;
            color_key <= 0;
            rotate_angle <= 0;
            row <= 0;
            col <= 0;
            wrote_count <= 0;
            read_byte <= 0;
            read_valid <= 0;
            buf_idx <= 0;
            src_row_off <= 0;
            dst_row_off <= 0;
            validate_row_start <= 0;
            validate_limit <= 0;
            validate_remaining <= 0;
            validate_stride <= 0;
            validate_width <= 0;
            validate_last_byte <= 0;
            validate_base_in_range <= 0;
            validate_src_start <= 0;
            validate_src_end <= 0;
            rot_sin_reg <= 0;
            rot_cos_reg <= 0;
            rot_center_fp_reg <= 0;
            rot_row_x_fp <= 0;
            rot_row_y_fp <= 0;
            rot_cur_x_fp <= 0;
            rot_cur_y_fp <= 0;
            rot_x_step_fp <= 0;
            rot_y_step_fp <= 0;
            rot_row_x_step_fp <= 0;
            rot_row_y_step_fp <= 0;
            rot_src_x_int <= 0;
            rot_src_y_int <= 0;
            rot_src_row_off <= 0;
            rot_src_addr <= 0;
            rot_center_cos <= 0;
            rot_mul_a <= 0;
            rot_mul_b <= 0;
        end else begin

            case (state)
                S_IDLE: ;

                S_VALIDATE: begin
                    if (width == 0 || height == 0) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADARGS;
                        state <= S_DONE;
                    end else if (rotate_mode && (fill_mode || width != height || width > 16'd256)) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADARGS;
                        state <= S_DONE;
                    end else if (!fill_mode && space_size(src_space) == 0) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADSPACE;
                        state <= S_DONE;
                    end else if (space_size(dst_space) == 0) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADSPACE;
                        state <= S_DONE;
                    end else begin
                        if (!fill_mode) begin
                            validate_row_start <= {8'b0, src_base};
                            validate_limit <= {12'b0, space_size(src_space)};
                            validate_remaining <= last_row;
                            validate_stride <= src_stride;
                            validate_width <= width;
                            validate_base_in_range <= ({8'b0, src_base} < {12'b0, space_size(src_space)});
                            state <= S_VALIDATE_SRC;
                        end else begin
                            validate_row_start <= {8'b0, dst_base};
                            validate_limit <= {12'b0, space_size(dst_space)};
                            validate_remaining <= last_row;
                            validate_stride <= dst_stride;
                            validate_width <= width;
                            validate_base_in_range <= ({8'b0, dst_base} < {12'b0, space_size(dst_space)});
                            state <= S_VALIDATE_DST;
                        end
                    end
                end

                S_VALIDATE_SRC: begin
                    if (validate_remaining != 0) begin
                        validate_row_start <= validate_row_start + {16'd0, validate_stride};
                        validate_remaining <= validate_remaining - 16'd1;
                    end else begin
                        validate_last_byte <= validate_row_start + {16'd0, validate_width};
                        state <= S_VALIDATE_SRC_CHECK;
                    end
                end

                S_VALIDATE_SRC_CHECK: begin
                    if (!validate_base_in_range || validate_last_byte > validate_limit) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_RANGE;
                        state <= S_DONE;
                    end else begin
                        validate_src_start <= {8'b0, src_base};
                        validate_src_end <= validate_last_byte;
                        validate_row_start <= {8'b0, dst_base};
                        validate_limit <= {12'b0, space_size(dst_space)};
                        validate_remaining <= last_row;
                        validate_stride <= dst_stride;
                        validate_width <= width;
                        validate_base_in_range <= ({8'b0, dst_base} < {12'b0, space_size(dst_space)});
                        state <= S_VALIDATE_DST;
                    end
                end

                S_VALIDATE_DST: begin
                    if (validate_remaining != 0) begin
                        validate_row_start <= validate_row_start + {16'd0, validate_stride};
                        validate_remaining <= validate_remaining - 16'd1;
                    end else begin
                        validate_last_byte <= validate_row_start + {16'd0, validate_width};
                        state <= S_VALIDATE_DST_CHECK;
                    end
                end

                S_VALIDATE_DST_CHECK: begin
                    if (!validate_base_in_range || validate_last_byte > validate_limit) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_RANGE;
                        state <= S_DONE;
                    end else if (dst_space == SPACE_CPU && validate_last_byte > {8'b0, ROM_BASE}) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_WRITEPROT;
                        state <= S_DONE;
                    end else if (rotate_mode && src_space == dst_space &&
                                 validate_src_start < validate_last_byte &&
                                 {8'b0, dst_base} < validate_src_end) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADARGS;
                        state <= S_DONE;
                    end else begin
                        state <= S_VALIDATE_START;
                    end
                end

                S_VALIDATE_START: begin
                    if (fill_mode) begin
                        state <= S_WRITE;
                        read_valid <= 1;
                        read_byte <= fill_value;
                    end else if (rotate_mode) begin
                        state <= S_ROT_SETUP_COS;
                        read_valid <= 0;
                    end else if (use_row_buffer) begin
                        state <= S_ROWBUF_READ;
                        col <= 0;
                        buf_idx <= 0;
                    end else begin
                        state <= S_READ;
                        read_valid <= 0;
                    end
                end

                // --- Rotate mode ---
                S_ROT_SETUP_COS: begin
                    rot_sin_reg <= rot_sin_lut;
                    rot_cos_reg <= rot_cos_lut;
                    rot_center_fp_reg <= rot_center_fp;
                    rot_mul_a <= rot_center_fp;
                    rot_mul_b <= $signed({{9{rot_cos_lut[7]}}, rot_cos_lut});
                    state <= S_ROT_CAPTURE_COS;
                end

                S_ROT_CAPTURE_COS: begin
                    rot_center_cos <= rot_mul_product;
                    rot_mul_a <= rot_center_fp_reg;
                    rot_mul_b <= $signed({{9{rot_sin_reg[7]}}, rot_sin_reg});
                    state <= S_ROT_CAPTURE_SIN;
                end

                S_ROT_CAPTURE_SIN: begin
                    rot_row_x_fp <= rot_center_fp_reg -
                        ((rot_center_cos + rot_mul_product) >>> 7);
                    rot_row_y_fp <= rot_center_fp_reg +
                        ((rot_mul_product - rot_center_cos) >>> 7);
                    rot_cur_x_fp <= rot_center_fp_reg -
                        ((rot_center_cos + rot_mul_product) >>> 7);
                    rot_cur_y_fp <= rot_center_fp_reg +
                        ((rot_mul_product - rot_center_cos) >>> 7);
                    rot_x_step_fp <= rot_cos_step_fp;
                    rot_y_step_fp <= -rot_sin_step_fp;
                    rot_row_x_step_fp <= rot_sin_step_fp;
                    rot_row_y_step_fp <= rot_cos_step_fp;
                    state <= S_ROT_SAMPLE;
                end

                S_ROT_SAMPLE: begin
                    rot_src_x_int <= rot_cur_x_int;
                    rot_src_y_int <= rot_cur_y_int;
                    rot_cur_x_fp <= rot_cur_x_fp + rot_x_step_fp;
                    rot_cur_y_fp <= rot_cur_y_fp + rot_y_step_fp;

                    if (rot_cur_oob) begin
                        read_byte <= color_key;
                        read_valid <= 1;
                        state <= S_WRITE;
                    end else begin
                        rot_mul_a <= $signed({9'd0, rot_cur_y_int[9:0]});
                        rot_mul_b <= $signed({1'b0, src_stride});
                        read_valid <= 0;
                        state <= S_ROT_ADDR_ROW;
                    end
                end

                S_ROT_ADDR_ROW: begin
                    rot_src_row_off <= rot_mul_product[23:0];
                    state <= S_ROT_ADDR_ADD;
                end

                S_ROT_ADDR_ADD: begin
                    rot_src_addr <= src_base + rot_src_row_off + {14'd0, rot_src_x_int[9:0]};
                    state <= S_READ;
                end

                // --- Direct mode (different spaces) ---
                S_READ: begin
                    // Issue cycle — ram_addr was driven combinationally this
                    // cycle; dpram latched it at the posedge. Next cycle dout
                    // will be valid.
                    state <= S_READ_WAIT;
                end

                S_READ_WAIT: begin
                    // XRAM reads take ~HOLD_CYCLES+2 cycles through xram_sdram.
                    // busy=1 while the wrapper is processing the latched
                    // req_re. When busy drops, r_dout is valid; latch it.
                    if (src_space == SPACE_XRAM && xram_busy) begin
                        // stall — r_dout not yet valid
                    end else begin
                        read_byte <= mem_read_data;
                        read_valid <= 1;
                        state <= S_WRITE;
                    end
                end

                S_WRITE: begin
                    // Back-pressure: if destination is XRAM and the wrapper
                    // is busy, keep xram_we / xram_addr / xram_wdata asserted
                    // (combinationally driven from this state) until the
                    // wrapper can accept the byte. Hold state until ready.
                    if ((dst_space == SPACE_XRAM && xram_busy) || video_write_wait) begin
                        // stall — re-enter S_WRITE implicitly by not advancing
                    end
                    else if (read_valid) begin
                        if (!write_skipped_by_colorkey)
                            wrote_count <= wrote_count + 1;

                        if (col == last_col) begin
                            col <= 0;
                            if (row == last_row) begin
                                regs[R_STATUS] <= ST_OK;
                                regs[R_ERRCODE] <= ERR_NONE;
                                {regs[R_COUNTH], regs[R_COUNTM], regs[R_COUNTL]}
                                    <= wrote_count + (!write_skipped_by_colorkey ? 24'd1 : 24'd0);
                                state <= S_DONE;
                            end else begin
                                row <= row + 1;
                                src_row_off <= src_row_off + {8'b0, src_stride};
                                dst_row_off <= dst_row_off + {8'b0, dst_stride};
                                if (fill_mode) begin
                                    state <= S_WRITE;
                                    read_byte <= fill_value;
                                end else if (rotate_mode) begin
                                    rot_row_x_fp <= rot_row_x_fp + rot_row_x_step_fp;
                                    rot_row_y_fp <= rot_row_y_fp + rot_row_y_step_fp;
                                    rot_cur_x_fp <= rot_row_x_fp + rot_row_x_step_fp;
                                    rot_cur_y_fp <= rot_row_y_fp + rot_row_y_step_fp;
                                    state <= S_ROT_SAMPLE;
                                end else
                                    state <= S_READ;
                            end
                        end else begin
                            col <= col + 1;
                            if (fill_mode) begin
                                state <= S_WRITE;
                                read_byte <= fill_value;
                            end else if (rotate_mode)
                                state <= S_ROT_SAMPLE;
                            else
                                state <= S_READ;
                        end
                    end
                end

                // --- Buffered mode (same-space copies) ---
                // Issue → wait → capture pattern, 2 cycles per byte. Old
                // 1-cycle-per-byte loop captured mem_read_data one cycle
                // early and missed the final byte on dpram-backed memories.
                S_ROWBUF_READ: begin
                    state <= S_ROWBUF_READ_WAIT;
                end

                S_ROWBUF_READ_WAIT: begin
                    // Same multi-cycle read handshake as S_READ_WAIT.
                    // Same-space XRAM→XRAM copies buffer through here.
                    if (src_space == SPACE_XRAM && xram_busy) begin
                        // stall — r_dout not yet valid
                    end else begin
                        row_buf[buf_idx] <= mem_read_data;
                        if (col == last_col) begin
                            col <= 0;
                            buf_idx <= 0;
                            read_valid <= 0;
                            state <= S_ROWBUF_WRITE_LOAD;
                        end else begin
                            col <= col + 1;
                            buf_idx <= buf_idx + 1;
                            state <= S_ROWBUF_READ;
                        end
                    end
                end

                S_ROWBUF_WRITE_LOAD: begin
                    // Register row-buffer output before it drives the shared
                    // VGC memory port. This breaks the row_buf→VGC mux→sprite
                    // bank critical path; same-space blits pay one cycle/byte.
                    read_byte <= row_buf[buf_idx];
                    read_valid <= 1;
                    state <= S_ROWBUF_WRITE;
                end

                S_ROWBUF_WRITE: begin
                    // Same back-pressure gating as S_WRITE. Same-space XRAM
                    // copies (src_space == dst_space == XRAM) use this path
                    // via the row-buffer.
                    if ((dst_space == SPACE_XRAM && xram_busy) || video_write_wait) begin
                        // stall — hold the row-buffer write steady until ready
                    end else begin
                    // Write one byte per clock from row buffer
                    if (!write_skipped_by_colorkey)
                        wrote_count <= wrote_count + 1;

                    if (col == last_col) begin
                        col <= 0;
                        buf_idx <= 0;
                        if (row == last_row) begin
                            regs[R_STATUS] <= ST_OK;
                            regs[R_ERRCODE] <= ERR_NONE;
                            {regs[R_COUNTH], regs[R_COUNTM], regs[R_COUNTL]}
                                <= wrote_count + (!write_skipped_by_colorkey ? 24'd1 : 24'd0);
                            state <= S_DONE;
                        end else begin
                            row <= row + 1;
                            src_row_off <= src_row_off + {8'b0, src_stride};
                            dst_row_off <= dst_row_off + {8'b0, dst_stride};
                            state <= S_ROWBUF_READ;
                        end
                    end else begin
                        col <= col + 1;
                        buf_idx <= buf_idx + 1;
                        read_valid <= 0;
                        state <= S_ROWBUF_WRITE_LOAD;
                    end
                    end  // end of (!xram_busy) block
                end

                S_DONE: begin
                    state <= S_IDLE;
                end

                default: state <= S_IDLE;
            endcase

            // CPU register writes
            if (cpu_we && blt_sel) begin
                if (reg_off != R_STATUS && reg_off != R_ERRCODE &&
                    reg_off != R_COUNTL && reg_off != R_COUNTM && reg_off != R_COUNTH)
                    regs[reg_off] <= cpu_wdata;

                if (reg_off == R_CMD) begin
                    if (cpu_wdata == 8'h01) begin
                        if (state == S_IDLE) begin
                            src_space  <= regs[R_SRCSPACE][2:0];
                            dst_space  <= regs[R_DSTSPACE][2:0];
                            src_base   <= {regs[R_SRCH], regs[R_SRCM], regs[R_SRCL]};
                            dst_base   <= {regs[R_DSTH], regs[R_DSTM], regs[R_DSTL]};
                            width      <= {regs[R_WIDTHH], regs[R_WIDTHL]};
                            height     <= {regs[R_HEIGHTH], regs[R_HEIGHTL]};
                            last_col   <= {regs[R_WIDTHH], regs[R_WIDTHL]} - 16'd1;
                            last_row   <= {regs[R_HEIGHTH], regs[R_HEIGHTL]} - 16'd1;
                            src_stride <= {regs[R_SRCSTRIDEH], regs[R_SRCSTRIDEL]};
                            dst_stride <= {regs[R_DSTSTRIDEH], regs[R_DSTSTRIDEL]};
                            fill_mode     <= regs[R_MODE][MODE_FILL];
                            colorkey_mode <= regs[R_MODE][MODE_COLORKEY];
                            rotate_mode   <= regs[R_MODE][MODE_ROTATE];
                            fill_value    <= regs[R_FILLVAL];
                            color_key     <= regs[R_COLORKEY];
                            rotate_angle  <= regs[R_ROTANGLE];
                            use_row_buffer <= !regs[R_MODE][MODE_FILL] &&
                                              !regs[R_MODE][MODE_ROTATE] &&
                                              regs[R_SRCSPACE][2:0] == regs[R_DSTSPACE][2:0];
                            row <= 0; col <= 0;
                            // Reset row-offset accumulators so address math
                            // starts at base. They advance by stride each row.
                            src_row_off <= 0;
                            dst_row_off <= 0;
                            wrote_count <= 0;
                            read_valid <= 0;
                            buf_idx <= 0;
                            regs[R_STATUS]  <= ST_BUSY;
                            regs[R_ERRCODE] <= ERR_NONE;
                            regs[R_COUNTL]  <= 0;
                            regs[R_COUNTM]  <= 0;
                            regs[R_COUNTH]  <= 0;
                            state <= S_VALIDATE;
                        end else begin
                            regs[R_STATUS] <= ST_ERROR;
                            regs[R_ERRCODE] <= ERR_BADCMD;
                        end
                    end else begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADCMD;
                    end
                end
            end
        end
    end

endmodule
