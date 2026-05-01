// VGC Timing (VESTA) — VGA-like timing for 640x480 at ~60Hz
// Generates all timing signals, counters, and derived coordinates
// Pipeline delay registers match BRAM read latency (2 cycles)

module vgc_timing (
    input  logic        clk,
    input  logic        rst,

    // Raw counters
    output logic [9:0]  h_count = 10'd0,
    output logic [9:0]  v_count = 10'd0,

    // Sync and visibility
    output logic        h_sync_area,
    output logic        v_sync_area,
    output logic        h_visible,
    output logic        v_visible,
    output logic        visible,
    output logic        in_text_area,

    // 2-cycle delayed versions (match BRAM pipeline). POR init via separate
    // initial block below — yosys rejects port-level `= 0` defaults.
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

    // Delayed coordinate signals — POR init via initial block below.
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
    // ULX3S gives us a 25.000 MHz pixel clock. Standard 640x480 VGA timings
    // (800*525) produce 59.52 Hz at that clock, which HDMI sinks/captures may
    // cadence-convert and show as motion judder. Keep the active image and
    // sync pulse standard, but trim back porch to get 25e6/(794*525)=59.97Hz.
`ifdef VIDEO_720X480
    // Full-system CEA-style 720x480p timing at a 27 MHz pixel clock.
    // The Nova canvas is 640x400 centered in 720x480 active video, leaving
    // a 40-pixel border on all sides.
    localparam H_ACTIVE = 720, H_FRONT = 16, H_SYNC = 62, H_BACK = 60, H_TOTAL = 858;
    localparam V_ACTIVE = 480, V_FRONT = 9,  V_SYNC = 6,  V_BACK = 30, V_TOTAL = 525;
    localparam NOVA_X0  = 40,  NOVA_Y0  = 40;
    localparam NOVA_W   = 640, NOVA_H   = 400;
`elsif VGC_TIMING_5994
    // Diagnostic timing profile: 25e6/(796*524)=59.937Hz, much closer to
    // VGA's 59.940Hz vertical cadence while keeping hsync near 31.47kHz.
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 44, H_TOTAL = 796;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 32, V_TOTAL = 524;
    localparam NOVA_X0  = 0,   NOVA_Y0  = 40;
    localparam NOVA_W   = 640, NOVA_H   = 400;
`else
    localparam H_ACTIVE = 640, H_FRONT = 16, H_SYNC = 96, H_BACK = 42, H_TOTAL = 794;
    localparam V_ACTIVE = 480, V_FRONT = 10, V_SYNC = 2,  V_BACK = 33, V_TOTAL = 525;
    localparam NOVA_X0  = 0,   NOVA_Y0  = 40;
    localparam NOVA_W   = 640, NOVA_H   = 400;
`endif

    localparam COLS     = 80,  ROWS    = 50;
    localparam CHAR_H   = 8;
    localparam TEXT_H   = ROWS * CHAR_H;               // 400 pixels (1:1, no doubling)
    localparam V_BORDER = 0;                            // canvas origin already includes border
    localparam [9:0] NOVA_X0_10 = NOVA_X0;
    localparam [9:0] NOVA_Y0_10 = NOVA_Y0;
    localparam [9:0] NOVA_X1_10 = NOVA_X0 + NOVA_W;
    localparam [9:0] NOVA_Y1_10 = NOVA_Y0 + NOVA_H;
    localparam [9:0] NOVA_X_LAST_10 = NOVA_W - 1;
    localparam [9:0] NOVA_Y_LAST_10 = NOVA_H - 1;

    // =========================================================================
    // POR initialization — pipeline regs start at 0. ECP5 trellis encodes
    // these in the bitstream as FF init values. Avoids needing `if (rst) ...`
    // muxes on every pipeline FF (which would cost ~3.5 MHz of clk_pixel).
    // Diagnosed 2026-04-26: same bitstream produced different visual artifacts
    // across reflashes (50px black bar / 1-char shift / clean) because uninit
    // pipeline regs fed garbage to vid_hsync/de during ~10µs rst window,
    // causing HDMI sink to lock onto wrong horizontal phase.
    // =========================================================================
    initial begin
        h_count_d1 = 0; h_count_d2 = 0;
        v_count_d1 = 0; v_count_d2 = 0;
        visible_d1 = 0; visible_d2 = 0;
        in_text_area_d1 = 0; in_text_area_d2 = 0;
        h_sync_area_d1 = 0; h_sync_area_d2 = 0;
        v_sync_area_d1 = 0; v_sync_area_d2 = 0;
        text_col_d1 = 0; text_col_d2 = 0;
        text_row_d1 = 0; text_row_d2 = 0;
        font_pixel_d1 = 0; font_pixel_d2 = 0;
        gfx_x_d1 = 0; gfx_x_d2 = 0;
        gfx_y_d1 = 0; gfx_y_d2 = 0;
        font_line_d1 = 0;
    end

    // =========================================================================
    // Video timing counters — BOTH declaration init (INITVAL=0 for FPGA POR)
    // AND rst clause (forces realignment on every dbg_cpu_reset). Without the
    // rst clause, h_count free-runs through every cold_start while the HDMI
    // encoder resets via ~rst — counter and encoder go out of phase, producing
    // a permanent 1-character LEFT shift in the rendered display. With BOTH,
    // yosys+nextpnr-ecp5 encodes the FF with REGSET="RESET" matching the
    // declaration init AND wires LSR=rst for runtime resync. Diagnosed
    // 2026-04-27 after several iterations — the prior comment claiming INITVAL
    // is dropped when LSR is non-trivial was incorrect; REGSET serves as the
    // POR state on ECP5 when no separate INIT attribute is present, and that
    // matches the declaration init value.
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            h_count <= 0; v_count <= 0;
        end else if (h_count == H_TOTAL - 1) begin
            h_count <= 0;
            v_count <= (v_count == V_TOTAL - 1) ? 10'd0 : v_count + 1;
        end else
            h_count <= h_count + 1;
    end

    // Combinational sync/visibility signals
    assign h_sync_area = (h_count >= H_ACTIVE + H_FRONT) &&
                         (h_count <  H_ACTIVE + H_FRONT + H_SYNC);
    assign v_sync_area = (v_count >= V_ACTIVE + V_FRONT) &&
                         (v_count <  V_ACTIVE + V_FRONT + V_SYNC);
    assign h_visible   = (h_count < H_ACTIVE);
    assign v_visible   = (v_count < V_ACTIVE);
    assign visible     = h_visible && v_visible;
    wire canvas_h_visible = (h_count >= NOVA_X0_10) && (h_count < NOVA_X1_10);
    wire canvas_v_visible = (v_count >= NOVA_Y0_10) && (v_count < NOVA_Y1_10);
    wire canvas_visible   = visible && canvas_h_visible && canvas_v_visible;
    // Clamp fetch coordinates outside the canvas. The compositor is delayed
    // two cycles, so wrapping the fetch address to column 0 immediately after
    // the right edge leaks the first cell's color into the final pixels.
    wire [9:0] canvas_h_count = (h_count < NOVA_X0_10) ? 10'd0 :
                                (h_count < NOVA_X1_10) ? (h_count - NOVA_X0_10) :
                                                          NOVA_X_LAST_10;
    wire [9:0] canvas_v_count = (v_count < NOVA_Y0_10) ? 10'd0 :
                                (v_count < NOVA_Y1_10) ? (v_count - NOVA_Y0_10) :
                                                          NOVA_Y_LAST_10;

    // Text/graphics cover only the centered Nova canvas. Physical visible
    // pixels outside this window are border color in the compositor.
    assign in_text_area = canvas_visible && (canvas_v_count < V_BORDER + TEXT_H);

    // Pre-compute gfx coordinates for tile engine (combinational)
    assign pre_gfx_x = canvas_h_count[9:1];
    assign pre_gfx_y = (canvas_v_count >> 1);

    // =========================================================================
    // Pipeline delay registers — 2 cycles to match BRAM read latency.
    //
    // These registers need both POR init and runtime reset. POR init makes
    // first power-up deterministic; runtime reset keeps the delayed DE/sync
    // pipeline aligned with h_count/v_count whenever the debug bridge or VM
    // issues a system reset after graphics/video activity.
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            h_count_d1 <= 0;          h_count_d2 <= 0;
            v_count_d1 <= 0;          v_count_d2 <= 0;
            visible_d1 <= 0;          visible_d2 <= 0;
            in_text_area_d1 <= 0;     in_text_area_d2 <= 0;
            h_sync_area_d1 <= 0;      h_sync_area_d2 <= 0;
            v_sync_area_d1 <= 0;      v_sync_area_d2 <= 0;
        end else begin
            h_count_d1 <= h_count;       h_count_d2 <= h_count_d1;
            v_count_d1 <= v_count;       v_count_d2 <= v_count_d1;
            visible_d1 <= visible;       visible_d2 <= visible_d1;
            in_text_area_d1 <= in_text_area;  in_text_area_d2 <= in_text_area_d1;
            h_sync_area_d1 <= h_sync_area;   h_sync_area_d2 <= h_sync_area_d1;
            v_sync_area_d1 <= v_sync_area;   v_sync_area_d2 <= v_sync_area_d1;
        end
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
        text_col   = canvas_h_count[9:3];
        font_pixel = canvas_h_count[2:0];
        text_line  = canvas_v_count - V_BORDER;     // 1:1 text (no pixel doubling)
        text_row   = text_line[8:3];                // 6-bit, 0..63
        font_line  = text_line[2:0];
        real_row_sum = {1'b0, text_row} + {1'b0, scroll_offset};
        real_row   = (real_row_sum >= 7'(ROWS)) ? real_row_sum[5:0] - 6'(ROWS)
                                                : real_row_sum[5:0];

        // Graphics coordinates (320x200, pixel-doubled to 640x400)
        gfx_x = canvas_h_count[9:1];
        gfx_y = canvas_v_count[8:1];                // halved canvas y (0..199)
    end

    // =========================================================================
    // Delayed coordinate signals — reset with the timing pipeline so text,
    // graphics, sprites, and tiles all restart from the same pixel phase.
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            text_col_d1 <= 0;         text_col_d2 <= 0;
            text_row_d1 <= 0;         text_row_d2 <= 0;
            font_pixel_d1 <= 0;       font_pixel_d2 <= 0;
            gfx_x_d1 <= 0;            gfx_x_d2 <= 0;
            gfx_y_d1 <= 0;            gfx_y_d2 <= 0;
            font_line_d1 <= 0;
        end else begin
            text_col_d1 <= text_col;      text_col_d2 <= text_col_d1;
            text_row_d1 <= text_row;      text_row_d2 <= text_row_d1;
            font_pixel_d1 <= font_pixel;  font_pixel_d2 <= font_pixel_d1;
            gfx_x_d1 <= gfx_x;           gfx_x_d2 <= gfx_x_d1;
            gfx_y_d1 <= gfx_y;           gfx_y_d2 <= gfx_y_d1;
            font_line_d1 <= font_line;
        end
    end

endmodule
