// VGC Timing (VESTA) — VGA timing generation for 640x480@60Hz
// Generates all timing signals, counters, and derived coordinates
// Pipeline delay registers match BRAM read latency (2 cycles)

module vgc_timing (
    input  logic        clk,
    input  logic        rst,

    // Raw counters
    output logic [9:0]  h_count,
    output logic [9:0]  v_count,

    // Sync and visibility
    output logic        h_sync_area,
    output logic        v_sync_area,
    output logic        h_visible,
    output logic        v_visible,
    output logic        visible,
    output logic        in_text_area,

    // 2-cycle delayed versions (match BRAM pipeline)
    output logic [9:0]  h_count_d1,  h_count_d2,
    output logic [9:0]  v_count_d1,  v_count_d2,
    output logic        visible_d1,  visible_d2,
    output logic        in_text_area_d1, in_text_area_d2,
    output logic        h_sync_area_d1, h_sync_area_d2,
    output logic        v_sync_area_d1, v_sync_area_d2,

    // Derived coordinate signals (current cycle)
    output logic [6:0]  text_col,
    output logic [4:0]  text_row,
    output logic [2:0]  font_line,
    output logic [2:0]  font_pixel,
    output logic [8:0]  gfx_x,
    output logic [7:0]  gfx_y,
    output logic [9:0]  text_line,
    output logic [4:0]  real_row,   // after scroll_offset applied

    // Delayed coordinate signals
    output logic [6:0]  text_col_d1, text_col_d2,
    output logic [4:0]  text_row_d1, text_row_d2,
    output logic [2:0]  font_pixel_d1, font_pixel_d2,
    output logic [8:0]  gfx_x_d1, gfx_x_d2,
    output logic [7:0]  gfx_y_d1, gfx_y_d2,
    output logic [2:0]  font_line_d1,

    // Scroll offset (from register file)
    input  logic [4:0]  scroll_offset,

    // Pre-computed gfx coordinates for tile engine
    output logic [8:0]  pre_gfx_x,
    output logic [7:0]  pre_gfx_y
);

    // =========================================================================
    // Parameters
    // =========================================================================
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 48, H_TOTAL = 800;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 33, V_TOTAL = 525;

    localparam COLS     = 80,  ROWS    = 25;
    localparam CHAR_H   = 8;
    localparam TEXT_H   = ROWS * CHAR_H * 2;           // 400 pixels (doubled)
    localparam V_BORDER = (V_ACTIVE - TEXT_H) / 2;     // 40 lines

    // =========================================================================
    // Video timing counters
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            h_count <= 0; v_count <= 0;
        end else begin
            if (h_count == H_TOTAL - 1) begin
                h_count <= 0;
                v_count <= (v_count == V_TOTAL - 1) ? 10'd0 : v_count + 1;
            end else
                h_count <= h_count + 1;
        end
    end

    // Combinational sync/visibility signals
    assign h_sync_area = (h_count >= H_ACTIVE + H_FRONT) &&
                         (h_count <  H_ACTIVE + H_FRONT + H_SYNC);
    assign v_sync_area = (v_count >= V_ACTIVE + V_FRONT) &&
                         (v_count <  V_ACTIVE + V_FRONT + V_SYNC);
    assign h_visible   = (h_count < H_ACTIVE);
    assign v_visible   = (v_count < V_ACTIVE);
    assign visible     = h_visible && v_visible;
    assign in_text_area = visible && (v_count >= V_BORDER) && (v_count < V_BORDER + TEXT_H);

    // Pre-compute gfx coordinates for tile engine (combinational)
    assign pre_gfx_x = h_count[9:1];
    assign pre_gfx_y = ((v_count - V_BORDER) >> 1);

    // =========================================================================
    // Pipeline delay registers — 2 cycles to match BRAM read latency
    // =========================================================================
    always_ff @(posedge clk) begin
        h_count_d1 <= h_count;       h_count_d2 <= h_count_d1;
        v_count_d1 <= v_count;       v_count_d2 <= v_count_d1;
        visible_d1 <= visible;       visible_d2 <= visible_d1;
        in_text_area_d1 <= in_text_area;  in_text_area_d2 <= in_text_area_d1;
        h_sync_area_d1 <= h_sync_area;   h_sync_area_d2 <= h_sync_area_d1;
        v_sync_area_d1 <= v_sync_area;   v_sync_area_d2 <= v_sync_area_d1;
    end

    // =========================================================================
    // Derived coordinate signals (current cycle)
    // =========================================================================
    // 6-bit intermediate for real_row arithmetic. Previously we had
    //     real_row = text_row + scroll_offset;     // both 5-bit → 5-bit sum
    // which truncated mod 32 BEFORE the `>= ROWS` fixup could run. When
    // text_row + scroll_offset reached 32, the sum wrapped to 0 and
    // bypassed the subtraction, causing rows 18..24 (with scroll_offset
    // from 8 upwards) to collide with rows 0..6 — the 7-row mirror bug
    // visible on hardware once the screen scrolled past the first full
    // page. The widened sum keeps the carry bit so the fixup works.
    logic [5:0] real_row_sum;

    always_comb begin
        text_col   = h_count[9:3];
        font_pixel = h_count[2:0];
        text_line  = (v_count - V_BORDER) >> 1;
        text_row   = text_line[7:3];
        font_line  = text_line[2:0];
        real_row_sum = {1'b0, text_row} + {1'b0, scroll_offset};
        real_row   = (real_row_sum >= 6'(ROWS)) ? real_row_sum[4:0] - 5'(ROWS)
                                                : real_row_sum[4:0];

        // Graphics coordinates (320x200, pixel doubled to 640x400)
        gfx_x = h_count[9:1];
        gfx_y = text_line[7:0];
    end

    // =========================================================================
    // Delayed coordinate signals
    // =========================================================================
    always_ff @(posedge clk) begin
        text_col_d1 <= text_col;      text_col_d2 <= text_col_d1;
        text_row_d1 <= text_row;      text_row_d2 <= text_row_d1;
        font_pixel_d1 <= font_pixel;  font_pixel_d2 <= font_pixel_d1;
        gfx_x_d1 <= gfx_x;           gfx_x_d2 <= gfx_x_d1;
        gfx_y_d1 <= gfx_y;           gfx_y_d2 <= gfx_y_d1;
        font_line_d1 <= font_line;
    end

endmodule
