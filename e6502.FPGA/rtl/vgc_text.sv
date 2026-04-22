// VGC Text (SCRIBE) — Text rendering with char_ram, color_ram, font_rom dprams
// Owns the three text-related BRAMs and drives port B for rendering
// Port A is exposed for the top-level command processor, blitter, and CPU access

module vgc_text (
    input  logic        clk,
    input  logic        rst,

    // --- char_ram port A (from top-level mux) ---
    input  logic [12:0] char_a_addr,
    input  logic [7:0]  char_a_din,
    input  logic        char_a_we,
    output logic [7:0]  char_a_dout,

    // --- color_ram port A (from top-level mux) ---
    input  logic [12:0] color_a_addr,
    input  logic [7:0]  color_a_din,
    input  logic        color_a_we,
    output logic [7:0]  color_a_dout,

    // --- font_rom port A (from top-level mux) ---
    input  logic [10:0] font_a_addr,
    input  logic [7:0]  font_a_din,
    input  logic        font_a_we,
    output logic [7:0]  font_a_dout,

    // --- Rendering (port B) inputs from timing ---
    input  logic [5:0]  real_row,
    input  logic [6:0]  text_col,
    input  logic [2:0]  font_line,
    input  logic [2:0]  font_line_d1,

    // --- Rendering outputs ---
    output logic [7:0]  char_b_dout,
    output logic [7:0]  color_b_dout,
    output logic [7:0]  font_b_dout
);

    localparam COLS = 80;

    // =========================================================================
    // Memory — dpram instances
    // =========================================================================

    // --- char_ram (4800 bytes for 80x60 text) ---
    logic [12:0] char_b_addr;

    dpram #(.WIDTH(8), .DEPTH(4800), .INIT_FILE("rom/char_init.hex")) char_mem (
        .clk(clk),
        .addr_a(char_a_addr), .din_a(char_a_din), .we_a(char_a_we), .dout_a(char_a_dout),
        .addr_b(char_b_addr), .dout_b(char_b_dout)
    );

    // --- color_ram (4800 bytes for 80x60 text) ---
    logic [12:0] color_b_addr;

    dpram #(.WIDTH(8), .DEPTH(4800), .INIT_FILE("rom/color_init.hex")) color_mem (
        .clk(clk),
        .addr_a(color_a_addr), .din_a(color_a_din), .we_a(color_a_we), .dout_a(color_a_dout),
        .addr_b(color_b_addr), .dout_b(color_b_dout)
    );

    // --- font_rom (2048 bytes, initialized from file) ---
    logic [10:0] font_b_addr;

    dpram #(.WIDTH(8), .DEPTH(2048), .INIT_FILE("rom/cp437.hex")) font_mem (
        .clk(clk),
        .addr_a(font_a_addr), .din_a(font_a_din), .we_a(font_a_we), .dout_a(font_a_dout),
        .addr_b(font_b_addr), .dout_b(font_b_dout)
    );

    // =========================================================================
    // Port B address generation — combinational to minimize pipeline depth
    // dpram has 1-cycle read latency. With combinational addresses:
    //   Cycle N: char_b_addr = f(text_col(N)) → dpram reads
    //   Cycle N+1: char_b_dout valid → font_b_addr = f(char, font_line) → dpram reads
    //   Cycle N+2: font_b_dout valid → compositing uses font_pixel_d2(N+2) = font_pixel(N)
    // Total 2-cycle pipeline matches the d2 delay on output signals.
    // =========================================================================
    assign char_b_addr  = real_row * COLS + {6'b0, text_col};
    assign color_b_addr = real_row * COLS + {6'b0, text_col};
    assign font_b_addr  = {char_b_dout, font_line};

endmodule
