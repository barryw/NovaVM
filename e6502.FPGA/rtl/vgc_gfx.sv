// VGC Graphics (CANVAS) — Graphics framebuffer with gfx_ram dpram
// Owns the gfx_ram BRAM and drives port B for rendering
// Port A is exposed for the top-level command processor, blitter, and reads

module vgc_gfx (
    input  logic        clk,
    input  logic        rst,

    // --- gfx_ram port A (from top-level mux) ---
    // 17-bit addresses needed: 320×240 = 76800 pixels > 65535 (16-bit range).
    input  logic [16:0] gfx_a_addr,
    input  logic [3:0]  gfx_a_din,
    input  logic        gfx_a_we,
    output logic [3:0]  gfx_a_dout,

    // --- Rendering (port B) inputs from timing ---
    input  logic [7:0]  gfx_y,
    input  logic [8:0]  gfx_x,

    // --- Rendering outputs ---
    output logic [3:0]  gfx_b_dout
);

    localparam GFX_W = 320;

    // =========================================================================
    // Memory — dpram instance
    // =========================================================================
    logic [16:0] gfx_b_addr;

    dpram #(.WIDTH(4), .DEPTH(76800)) gfx_mem (
        .clk(clk),
        .addr_a(gfx_a_addr), .din_a(gfx_a_din), .we_a(gfx_a_we), .dout_a(gfx_a_dout),
        .addr_b(gfx_b_addr), .dout_b(gfx_b_dout)
    );

    // =========================================================================
    // Port B address generation
    // =========================================================================
    always_ff @(posedge clk) begin
        gfx_b_addr <= {9'b0, gfx_y} * GFX_W + {8'b0, gfx_x};
    end

endmodule
