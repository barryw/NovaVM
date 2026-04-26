// ARTIST — Drawing coprocessor for VGC
// Hardwired state machines for pixel-level graphics operations:
// PLOT, UNPLOT, LINE (Bresenham), CIRCLE (midpoint), RECT (outline),
// FILL (solid), GCLS (clear), PAINT (flood fill), GTEXT (text on gfx)
//
// No instruction memory, no program counter — just command latches
// and state machines that eat commands and write pixels.

module artist (
    input  logic        clk,
    input  logic        rst,

    // Command interface (from vgc.sv NOVA)
    input  logic        cmd_valid,       // pulse: new command
    input  logic [7:0]  cmd_code,        // which command
    input  logic [8:0]  cmd_x0,          // X0 (0-319)
    input  logic [7:0]  cmd_y0,          // Y0 (0-239)
    input  logic [8:0]  cmd_x1,          // X1 (0-319)
    input  logic [7:0]  cmd_y1,          // Y1 (0-239)
    input  logic [3:0]  cmd_color,       // drawing color
    input  logic [7:0]  cmd_radius,      // for CIRCLE (radius = x1 low bits)
    output logic        busy,            // high while processing

    // GFX RAM write port — 17-bit address covers 76800 pixels (320×240)
    output logic [16:0] gfx_addr,        // pixel address
    output logic [3:0]  gfx_wdata,       // pixel color
    output logic        gfx_we,          // write enable

    // GFX RAM read port (for flood fill)
    output logic [16:0] gfx_raddr,       // read address
    input  logic [3:0]  gfx_rdata_raw,   // read data (1-cycle latency from dpram)
    output logic        gfx_re,          // read enable

    // Font ROM read port (for GTEXT)
    output logic [10:0] font_addr,       // font ROM address
    input  logic [7:0]  font_data,       // font data (1-cycle latency)
    output logic        font_re,         // read enable

    // GTEXT parameters
    input  logic [64*8-1:0] gt_char_flat,  // character buffer (packed)
    input  logic [5:0]  gt_char_len,       // string length
    input  logic [7:0]  gt_scale_in        // scale factor
);

    // =========================================================================
    // Parameters
    // =========================================================================
    localparam GFX_W = 320;
    localparam GFX_H = 240;

    // Extract character from packed flat vector
    function automatic logic [7:0] gt_char(input logic [5:0] idx);
        gt_char = gt_char_flat[idx*8 +: 8];
    endfunction

    // Command codes (must match vgc.sv)
    localparam CMD_PLOT   = 8'h01;
    localparam CMD_UNPLOT = 8'h02;
    localparam CMD_LINE   = 8'h03;
    localparam CMD_CIRCLE = 8'h04;
    localparam CMD_RECT   = 8'h05;
    localparam CMD_FILL   = 8'h06;
    localparam CMD_GCLS   = 8'h07;
    localparam CMD_PAINT  = 8'h09;
    localparam CMD_GTEXT  = 8'h0A;

    // =========================================================================
    // Internal state
    // =========================================================================
    logic [7:0]  op;
    logic signed [9:0] x, y, x2, y2;
    logic signed [9:0] dx, dy, err;
    logic signed [9:0] sx, sy;
    logic [8:0]  cx;
    logic [7:0]  cy;
    logic [3:0]  color;

    // Circle serialization
    logic [2:0]  octant_count;

    // Flood fill stack — dpram-backed for synthesis
    localparam FILL_STACK_SIZE = 512;
    logic [8:0]  fs_a_addr;
    logic [16:0] fs_a_din;
    logic        fs_a_we;
    wire  [16:0] fs_a_dout;
    logic [8:0]  fs_b_addr;
    wire  [16:0] fs_b_dout;

    dpram #(.WIDTH(17), .DEPTH(FILL_STACK_SIZE)) fill_stack_mem (
        .clk(clk),
        .addr_a(fs_a_addr), .din_a(fs_a_din), .we_a(fs_a_we), .dout_a(fs_a_dout),
        .addr_b(fs_b_addr), .dout_b(fs_b_dout)
    );

    logic [9:0]  fill_sp;
    logic [3:0]  fill_target;
    logic [2:0]  paint_phase;  // expanded for serialized pushes
    logic [1:0]  paint_init;  // countdown: 2=issued read, 1=dpram done, 0=ready
    logic [1:0]  push_count;   // 0-3: which neighbor to push
    logic [16:0] push_neighbors [0:3];  // latched neighbors to push

    // Pipeline register for gfx_rdata — breaks critical timing path
    // (BRAM output → paint address calc was ~19ns combinational).
    // POR determinism via declaration init (`= 0` encoded in bitstream).
    logic [3:0] gfx_rdata = 0;
    always_ff @(posedge clk) gfx_rdata <= gfx_rdata_raw;

    // Gtext state
    logic [5:0]  gt_char_idx;
    logic [5:0]  gt_len;
    logic [2:0]  gt_font_row;
    logic [2:0]  gt_font_col;
    logic [7:0]  gt_scale;
    logic [7:0]  gt_scale_x;
    logic [7:0]  gt_scale_y;
    logic [8:0]  gt_pen_x;
    logic [7:0]  gt_start_y;
    logic [7:0]  gt_cur_char;
    logic [7:0]  gt_font_byte;
    logic [1:0]  gt_font_pending;

    // =========================================================================
    // Initialization
    // =========================================================================
    initial begin
        busy = 0;
        op = 0;
        fill_sp = 0;
        octant_count = 0;
        paint_phase = 0;
        gt_font_pending = 0;
        paint_init = 0;
    end

    // =========================================================================
    // Main state machine
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            busy <= 0;
            op <= 0;
            fill_sp <= 0;
            octant_count <= 0;
            paint_phase <= 0;
            gt_font_pending <= 0;
            paint_init <= 0;
            gfx_we <= 0;
            gfx_re <= 0;
            font_re <= 0;
            fs_a_we <= 0;
        end else begin
            // Default: no writes this cycle
            gfx_we <= 0;
            gfx_re <= 0;
            font_re <= 0;
            fs_a_we <= 0;

            // Gtext font read return -- 2-cycle pipeline
            if (gt_font_pending == 2'd1) begin
                gt_font_pending <= 2'd2;
            end else if (gt_font_pending == 2'd2) begin
                gt_font_byte <= font_data;
                gt_font_pending <= 2'd0;
            end

            // Drawing command state machine
            if (busy) begin
                case (op)
                    CMD_LINE: begin
                        if (x >= 0 && x < GFX_W && y >= 0 && y < GFX_H) begin
                            gfx_addr <= y * GFX_W + x;
                            gfx_wdata <= color;
                            gfx_we <= 1;
                        end
                        if (x == x2 && y == y2 ||
                            x < -16 || x > GFX_W + 16 ||
                            y < -16 || y > GFX_H + 16) begin
                            busy <= 0;
                        end else begin
                            if (err * 2 >= dy && err * 2 <= dx) begin
                                err <= err + dy + dx;
                                x <= x + sx;
                                y <= y + sy;
                            end else if (err * 2 >= dy) begin
                                err <= err + dy;
                                x <= x + sx;
                            end else if (err * 2 <= dx) begin
                                err <= err + dx;
                                y <= y + sy;
                            end
                        end
                    end

                    CMD_RECT: begin
                        if (cx < GFX_W && cy < GFX_H) begin
                            if (cy == y[7:0] || cy == y2[7:0] ||
                                cx == x[8:0] || cx == x2[8:0]) begin
                                gfx_addr <= {9'b0, cy} * GFX_W + {8'b0, cx};
                                gfx_wdata <= color;
                                gfx_we <= 1;
                            end
                        end
                        if (cx >= x2[8:0]) begin
                            cx <= x[8:0];
                            if (cy >= y2[7:0])
                                busy <= 0;
                            else
                                cy <= cy + 1;
                        end else
                            cx <= cx + 1;
                    end

                    CMD_FILL: begin
                        if (cx < GFX_W && cy < GFX_H) begin
                            gfx_addr <= {9'b0, cy} * GFX_W + {8'b0, cx};
                            gfx_wdata <= color;
                            gfx_we <= 1;
                        end
                        if (cx >= x2[8:0]) begin
                            cx <= x[8:0];
                            if (cy >= y2[7:0])
                                busy <= 0;
                            else
                                cy <= cy + 1;
                        end else
                            cx <= cx + 1;
                    end

                    CMD_GCLS: begin
                        gfx_addr <= {9'b0, cy} * GFX_W + {8'b0, cx};
                        gfx_wdata <= 4'h0;
                        gfx_we <= 1;
                        if (cx == GFX_W - 1) begin
                            cx <= 0;
                            if (cy == GFX_H - 1)
                                busy <= 0;
                            else
                                cy <= cy + 1;
                        end else
                            cx <= cx + 1;
                    end

                    CMD_CIRCLE: begin // midpoint algorithm, serialized 8 octants
                        if (dx <= dy) begin
                            case (octant_count)
                                3'd0: begin
                                    if (x+dx >= 0 && x+dx < GFX_W && y+dy >= 0 && y+dy < GFX_H) begin
                                        gfx_addr <= (y+dy) * GFX_W + (x+dx);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd1;
                                end
                                3'd1: begin
                                    if (x-dx >= 0 && x-dx < GFX_W && y+dy >= 0 && y+dy < GFX_H) begin
                                        gfx_addr <= (y+dy) * GFX_W + (x-dx);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd2;
                                end
                                3'd2: begin
                                    if (x+dx >= 0 && x+dx < GFX_W && y-dy >= 0 && y-dy < GFX_H) begin
                                        gfx_addr <= (y-dy) * GFX_W + (x+dx);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd3;
                                end
                                3'd3: begin
                                    if (x-dx >= 0 && x-dx < GFX_W && y-dy >= 0 && y-dy < GFX_H) begin
                                        gfx_addr <= (y-dy) * GFX_W + (x-dx);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd4;
                                end
                                3'd4: begin
                                    if (x+dy >= 0 && x+dy < GFX_W && y+dx >= 0 && y+dx < GFX_H) begin
                                        gfx_addr <= (y+dx) * GFX_W + (x+dy);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd5;
                                end
                                3'd5: begin
                                    if (x-dy >= 0 && x-dy < GFX_W && y+dx >= 0 && y+dx < GFX_H) begin
                                        gfx_addr <= (y+dx) * GFX_W + (x-dy);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd6;
                                end
                                3'd6: begin
                                    if (x+dy >= 0 && x+dy < GFX_W && y-dx >= 0 && y-dx < GFX_H) begin
                                        gfx_addr <= (y-dx) * GFX_W + (x+dy);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd7;
                                end
                                3'd7: begin
                                    if (x-dy >= 0 && x-dy < GFX_W && y-dx >= 0 && y-dx < GFX_H) begin
                                        gfx_addr <= (y-dx) * GFX_W + (x-dy);
                                        gfx_wdata <= color; gfx_we <= 1;
                                    end
                                    octant_count <= 3'd0;
                                    dx <= dx + 1;
                                    if (err < 0) begin
                                        err <= err + 2 * dx + 3;
                                    end else begin
                                        err <= err + 2 * (dx - dy) + 5;
                                        dy <= dy - 1;
                                    end
                                end
                            endcase
                        end else
                            busy <= 0;
                    end

                    CMD_PAINT: begin
                        if (paint_init != 0) begin
                            // Countdown for first gfx read (dpram + pipeline reg)
                            if (paint_init == 2'd1) begin
                                fill_target <= gfx_rdata;
                                paint_init <= 0;
                                paint_phase <= 3'd0;
                            end else
                                paint_init <= paint_init - 1;
                        end else begin
                            case (paint_phase)
                                3'd0: begin // Pop: request stack read
                                    if (fill_sp > 0) begin
                                        fs_b_addr <= fill_sp[8:0] - 9'd1;
                                        fill_sp <= fill_sp - 1;
                                        paint_phase <= 3'd1;
                                    end else begin
                                        busy <= 0;
                                    end
                                end
                                3'd1: begin // Pop: wait for dpram read
                                    paint_phase <= 3'd2;
                                end
                                3'd2: begin // Pop: latch result, request gfx read
                                    cx <= fs_b_dout[8:0];
                                    cy <= fs_b_dout[16:9];
                                    if (fs_b_dout[8:0] < GFX_W && fs_b_dout[16:9] < GFX_H) begin
                                        gfx_raddr <= {9'b0, fs_b_dout[16:9]} * GFX_W + {8'b0, fs_b_dout[8:0]};
                                        gfx_re <= 1;
                                        paint_phase <= 3'd3;
                                    end else begin
                                        paint_phase <= 3'd0; // out of bounds, skip
                                    end
                                end
                                3'd3: begin // Wait for dpram gfx read
                                    paint_phase <= 3'd7;
                                end
                                3'd7: begin // Wait for gfx_rdata pipeline register
                                    paint_phase <= 3'd4;
                                end
                                3'd4: begin // Check pixel, write + latch neighbors
                                    if (gfx_rdata == fill_target && fill_target != color) begin
                                        gfx_addr <= {9'b0, cy} * GFX_W + {8'b0, cx};
                                        gfx_wdata <= color;
                                        gfx_we <= 1;
                                        // Latch 4 neighbors for serialized push
                                        push_neighbors[0] <= {cy, cx + 9'd1};
                                        push_neighbors[1] <= {cy, cx - 9'd1};
                                        push_neighbors[2] <= {cy + 8'd1, cx};
                                        push_neighbors[3] <= {cy - 8'd1, cx};
                                        push_count <= 2'd0;
                                        paint_phase <= 3'd5;
                                    end else begin
                                        paint_phase <= 3'd0; // no match, next pixel
                                    end
                                end
                                3'd5: begin // Push neighbors one at a time
                                    if (fill_sp < FILL_STACK_SIZE) begin
                                        fs_a_addr <= fill_sp[8:0];
                                        fs_a_din <= push_neighbors[push_count];
                                        fs_a_we <= 1;
                                        fill_sp <= fill_sp + 1;
                                    end
                                    if (push_count == 2'd3) begin
                                        paint_phase <= 3'd0; // all 4 pushed, next pixel
                                    end else begin
                                        push_count <= push_count + 1;
                                    end
                                end
                                default: paint_phase <= 3'd0;
                            endcase
                        end
                    end

                    CMD_GTEXT: begin
                        if (gt_font_pending != 2'd0) begin
                            // Waiting for font ROM read
                        end else if (gt_char_idx < gt_len) begin
                            if (gt_font_byte[3'd7 - gt_font_col]) begin
                                if ((16'(gt_pen_x) + 16'(gt_font_col) * 16'(gt_scale) + 16'(gt_scale_x)) < GFX_W &&
                                    (16'(gt_start_y) + 16'(gt_font_row) * 16'(gt_scale) + 16'(gt_scale_y)) < GFX_H) begin
                                    gfx_addr <= (17'(gt_start_y) + 17'(gt_font_row) * 17'(gt_scale) + 17'(gt_scale_y)) * GFX_W +
                                                (17'(gt_pen_x) + 17'(gt_font_col) * 17'(gt_scale) + 17'(gt_scale_x));
                                    gfx_wdata <= color;
                                    gfx_we <= 1;
                                end
                            end
                            if (gt_scale_x + 8'd1 < gt_scale)
                                gt_scale_x <= gt_scale_x + 1;
                            else begin
                                gt_scale_x <= 0;
                                if (gt_font_col < 7)
                                    gt_font_col <= gt_font_col + 1;
                                else begin
                                    gt_font_col <= 0;
                                    if (gt_scale_y + 8'd1 < gt_scale)
                                        gt_scale_y <= gt_scale_y + 1;
                                    else begin
                                        gt_scale_y <= 0;
                                        if (gt_font_row < 7) begin
                                            gt_font_row <= gt_font_row + 1;
                                            font_addr <= {gt_cur_char, gt_font_row + 3'd1};
                                            font_re <= 1;
                                            gt_font_pending <= 2'd1;
                                        end else begin
                                            gt_font_row <= 0;
                                            gt_char_idx <= gt_char_idx + 1;
                                            gt_pen_x <= gt_pen_x + 16'(8) * 16'(gt_scale);
                                            if (gt_char_idx + 1 < gt_len) begin
                                                gt_cur_char <= gt_char(gt_char_idx + 1);
                                                font_addr <= {gt_char(gt_char_idx + 1), 3'b000};
                                                font_re <= 1;
                                                gt_font_pending <= 2'd1;
                                            end
                                        end
                                    end
                                end
                            end
                        end else
                            busy <= 0;
                    end

                    default: busy <= 0;
                endcase
            end

            // Command dispatch -- accept new commands when idle
            if (cmd_valid && !busy) begin
                color <= cmd_color;
                case (cmd_code)
                    CMD_PLOT: begin
                        if (cmd_x0 < GFX_W && cmd_y0 < GFX_H) begin
                            gfx_addr <= {8'b0, cmd_y0} * GFX_W + {8'b0, cmd_x0};
                            gfx_wdata <= cmd_color;
                            gfx_we <= 1;
                        end
                        // Single-cycle, no busy needed
                    end

                    CMD_UNPLOT: begin
                        if (cmd_x0 < GFX_W && cmd_y0 < GFX_H) begin
                            gfx_addr <= {8'b0, cmd_y0} * GFX_W + {8'b0, cmd_x0};
                            gfx_wdata <= 4'h0;
                            gfx_we <= 1;
                        end
                    end

                    CMD_LINE: begin
                        // Bresenham needs dx = +|x1-x0| and dy = -|y1-y0|,
                        // err = dx + dy. The previous "reversed" branches
                        // forgot to negate |y1-y0|, which broke every line
                        // drawn from higher-y to lower-y: the y step term
                        // pushed err the wrong way and the endpoint fell
                        // off the end of the iteration.
                        x  <= {1'b0, cmd_x0};
                        y  <= {2'b0, cmd_y0};
                        x2 <= {1'b0, cmd_x1};
                        y2 <= {2'b0, cmd_y1};
                        dx <= (cmd_x1 >= cmd_x0)
                              ? 10'(cmd_x1 - cmd_x0)
                              : 10'(cmd_x0 - cmd_x1);
                        dy <= (cmd_y1 >= cmd_y0)
                              ? -(10'(cmd_y1 - cmd_y0))
                              : -(10'(cmd_y0 - cmd_y1));
                        sx <= (cmd_x1 >= cmd_x0) ? 10'd1 : -10'd1;
                        sy <= (cmd_y1 >= cmd_y0) ? 10'd1 : -10'd1;
                        err <= ((cmd_x1 >= cmd_x0)
                               ? 10'(cmd_x1 - cmd_x0)
                               : 10'(cmd_x0 - cmd_x1))
                             + ((cmd_y1 >= cmd_y0)
                               ? -(10'(cmd_y1 - cmd_y0))
                               : -(10'(cmd_y0 - cmd_y1)));
                        busy <= 1; op <= CMD_LINE;
                    end

                    CMD_RECT, CMD_FILL: begin
                        x  <= {1'b0, cmd_x0};
                        y  <= {2'b0, cmd_y0};
                        x2 <= {1'b0, cmd_x1};
                        y2 <= {2'b0, cmd_y1};
                        cx <= cmd_x0;
                        cy <= cmd_y0;
                        busy <= 1;
                        op <= cmd_code;
                    end

                    CMD_GCLS: begin
                        cx <= 0; cy <= 0;
                        busy <= 1; op <= CMD_GCLS;
                    end

                    CMD_CIRCLE: begin
                        x  <= {1'b0, cmd_x0};
                        y  <= {2'b0, cmd_y0};
                        x2 <= {1'b0, cmd_x1};
                        dx <= 0;
                        dy <= {1'b0, cmd_x1};  // radius in x1
                        err <= 1 - {1'b0, cmd_x1};
                        octant_count <= 0;
                        busy <= 1; op <= CMD_CIRCLE;
                    end

                    CMD_PAINT: begin
                        x <= {1'b0, cmd_x0};
                        y <= {2'b0, cmd_y0};
                        if (cmd_x0 < GFX_W && cmd_y0 < GFX_H) begin
                            // Issue first read to get fill_target
                            gfx_raddr <= {8'b0, cmd_y0} * GFX_W + {8'b0, cmd_x0};
                            gfx_re <= 1;
                            fs_a_addr <= 9'd0;
                            fs_a_din <= {cmd_y0, cmd_x0};
                            fs_a_we <= 1;
                            fill_sp <= 1;
                            paint_phase <= 3'd3; // skip to gfx read wait
                            paint_init <= 2'd2;  // 2 cycles: dpram + pipeline reg
                            busy <= 1; op <= CMD_PAINT;
                        end
                    end

                    CMD_GTEXT: begin
                        if (gt_char_len > 0) begin
                            gt_pen_x <= cmd_x0;
                            gt_start_y <= cmd_y0;
                            gt_scale <= (gt_scale_in < 1) ? 8'd1 : gt_scale_in;
                            gt_char_idx <= 0;
                            gt_len <= gt_char_len;
                            gt_font_row <= 0;
                            gt_font_col <= 0;
                            gt_scale_x <= 0;
                            gt_scale_y <= 0;
                            gt_cur_char <= gt_char(0);
                            font_addr <= {gt_char(0), 3'b000};
                            font_re <= 1;
                            gt_font_pending <= 2'd1;
                            busy <= 1; op <= CMD_GTEXT;
                        end
                    end

                    default: ; // Unknown command, ignore
                endcase
            end
        end
    end

endmodule
