// VGC Copper (CONDUCTOR) — Copper list execution engine
// Manages beam-synchronized register writes from active copper list
// Command handling (add/clear/enable/disable/list/use/listend) is in vgc.sv top
//
// Active list arrays are passed as flat packed vectors for Yosys compatibility

module vgc_copper #(
    parameter int COPPER_MAX = 32
) (
    input  logic        clk,
    input  logic        rst,

    // --- Timing inputs ---
    input  logic [9:0]  h_count,
    input  logic [9:0]  v_count,
    input  logic        in_text_area,
    input  logic [8:0]  gfx_x,
    input  logic [7:0]  gfx_y,

    // --- Active list arrays ---
    input  logic [COPPER_MAX*17-1:0] copper_pos_flat,
    input  logic [COPPER_MAX*8-1:0]  copper_reg_flat,
    input  logic [COPPER_MAX*8-1:0]  copper_val_flat,
    input  logic [8:0]       copper_count,
    input  logic             copper_enabled,

    // --- Copper index tracking ---
    output logic [8:0]  copper_index,

    // --- Register write output (to top-level register file) ---
    output logic        copper_fire,
    output logic [7:0]  copper_fire_reg,
    output logic [7:0]  copper_fire_val
);

    localparam V_ACTIVE = 480;
    localparam GFX_W    = 320;

    function automatic logic [16:0] gfx_addr_xy(input logic [8:0] x, input logic [7:0] y);
        gfx_addr_xy = {1'b0, y, 8'b0} + {3'b0, y, 6'b0} + {8'b0, x};
    endfunction

    // Unpack flat vectors into local arrays.
    logic [16:0] copper_pos [0:COPPER_MAX-1];
    logic [7:0]  copper_reg [0:COPPER_MAX-1];
    logic [7:0]  copper_val [0:COPPER_MAX-1];

    always_comb begin
        for (int i = 0; i < COPPER_MAX; i++) begin
            copper_pos[i] = copper_pos_flat[i*17 +: 17];
            copper_reg[i] = copper_reg_flat[i*8 +: 8];
            copper_val[i] = copper_val_flat[i*8 +: 8];
        end
    end

    // =========================================================================
    // Copper execution — beam position compare + register writeback
    // =========================================================================
    wire [16:0] beam_pos = gfx_addr_xy(gfx_x, gfx_y);

    // Two coupled left-edge fixes live here:
    //  1) Gate on the whole VISIBLE scanline (v_count < V_ACTIVE), NOT in_text_area
    //     (the canvas region excludes the left border), so a per-scanline write
    //     isn't held back to the active area -- which made the entire left border
    //     scan out with the PREVIOUS line's value (bars lagged one scanline).
    //  2) Fire COMBINATIONALLY the cycle the beam reaches an entry, so the register
    //     write lands one pixel-clock earlier -- in time for COLUMN 0. A registered
    //     copper_fire put the write at h_count=1, leaving the very first pixel one
    //     scanline stale: a 1px-low seam on the screen's left edge.
    // copper_index stays registered so each entry fires exactly once, on
    // consecutive cycles. Both verified pixel-aligned at columns 0..N in the
    // boards/arty_z7/sim/vgc render testbench.
    wire fire_now = copper_enabled && (v_count < V_ACTIVE) &&
                    copper_index < copper_count &&
                    beam_pos >= copper_pos[copper_index];
    assign copper_fire     = fire_now;
    assign copper_fire_reg = copper_reg[copper_index];
    assign copper_fire_val = copper_val[copper_index];

    always_ff @(posedge clk) begin
        if (rst)
            copper_index <= 0;
        else if (h_count == 0 && v_count == V_ACTIVE)   // vblank: reset index
            copper_index <= 0;
        else if (fire_now)
            copper_index <= copper_index + 1;
    end

endmodule
