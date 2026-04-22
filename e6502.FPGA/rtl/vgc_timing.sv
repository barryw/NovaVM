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
    output logic [5:0]  text_row,
    output logic [2:0]  font_line,
    output logic [2:0]  font_pixel,
    output logic [8:0]  gfx_x,
    output logic [7:0]  gfx_y,
    output logic [9:0]  text_line,
    output logic [5:0]  real_row,   // after scroll_offset applied

    // Delayed coordinate signals
    output logic [6:0]  text_col_d1, text_col_d2,
    output logic [5:0]  text_row_d1, text_row_d2,
    output logic [2:0]  font_pixel_d1, font_pixel_d2,
    output logic [8:0]  gfx_x_d1, gfx_x_d2,
    output logic [7:0]  gfx_y_d1, gfx_y_d2,
    output logic [2:0]  font_line_d1,

    // Scroll offset (from register file)
    input  logic [5:0]  scroll_offset,

    // Pre-computed gfx coordinates for tile engine
    output logic [8:0]  pre_gfx_x,
    output logic [7:0]  pre_gfx_y
);

    // =========================================================================
    // Parameters
    // =========================================================================
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 48, H_TOTAL = 800;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 33, V_TOTAL = 525;

    localparam COLS     = 80,  ROWS    = 60;
    localparam CHAR_H   = 8;
    localparam TEXT_H   = ROWS * CHAR_H;               // 480 pixels (1:1, no doubling)
    localparam V_BORDER = (V_ACTIVE - TEXT_H) / 2;     // 0 lines (no border)

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
    // With V_BORDER=0 and TEXT_H=V_ACTIVE, the text area covers the entire
    // visible region. Kept as a conditional so the generalized form still
    // reads correctly if V_BORDER ever becomes non-zero again.
    assign in_text_area = visible && (v_count < V_BORDER + TEXT_H);

    // Pre-compute gfx coordinates for tile engine (combinational)
    assign pre_gfx_x = h_count[9:1];
    assign pre_gfx_y = (v_count >> 1);                 // V_BORDER=0: no subtract

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
    // 7-bit intermediate for real_row arithmetic. ROWS=60, so text_row and
    // scroll_offset are both 6-bit; their unsigned sum can reach 118, which
    // needs 7 bits so the `>= ROWS` mod-fixup can run before wrap. Same class
    // of fix as the prior 7-row mirror bug — the extra carry bit matters.
    logic [6:0] real_row_sum;

    always_comb begin
        text_col   = h_count[9:3];
        font_pixel = h_count[2:0];
        text_line  = v_count - V_BORDER;            // 1:1 text (no pixel doubling)
        text_row   = text_line[8:3];                // 6-bit, 0..63
        font_line  = text_line[2:0];
        real_row_sum = {1'b0, text_row} + {1'b0, scroll_offset};
        real_row   = (real_row_sum >= 7'(ROWS)) ? real_row_sum[5:0] - 6'(ROWS)
                                                : real_row_sum[5:0];

        // Graphics coordinates (320x240, pixel-doubled to 640x480)
        gfx_x = h_count[9:1];
        gfx_y = v_count[8:1];                       // halved v_count (0..239)
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
