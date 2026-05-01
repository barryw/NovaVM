// VGC Graphics (CANVAS) — Graphics framebuffer with gfx_ram dpram
// Owns the gfx_ram BRAM and drives port B for rendering
// Port A is exposed for the top-level command processor, blitter, and reads

module vgc_gfx (
    input  logic        clk,
    input  logic        rst,

    // --- gfx_ram port A (from top-level mux) ---
    // 17-bit address port kept for bus-master compatibility; only 64000
    // entries are used for the 320×200 visible bitmap.
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

    // 320 = 256 + 64. Use shift/add addressing so scanout never infers a
    // multiplier or long generic constant-multiply path.
    function automatic logic [16:0] gfx_addr_xy(input logic [8:0] x, input logic [7:0] y);
        gfx_addr_xy = {1'b0, y, 8'b0} + {3'b0, y, 6'b0} + {8'b0, x};
    endfunction

    // =========================================================================
    // Memory — dpram instance
    // =========================================================================
    logic [16:0] gfx_b_addr = 0;  // POR-init for determinism

    wire [15:0] gfx_a_addr_i = gfx_a_addr[15:0];

    dpram #(.WIDTH(4), .DEPTH(64000)) gfx_mem (
        .clk(clk),
        .addr_a(gfx_a_addr_i), .din_a(gfx_a_din), .we_a(gfx_a_we), .dout_a(gfx_a_dout),
        .addr_b(gfx_b_addr), .dout_b(gfx_b_dout)
    );

    // =========================================================================
    // Port B address generation. POR determinism via declaration init.
    // =========================================================================
    always_ff @(posedge clk) begin
        gfx_b_addr <= gfx_addr_xy(gfx_x, gfx_y);
    end

endmodule
