// VGC Copper (CONDUCTOR) — Copper list execution engine
// Manages beam-synchronized register writes from active copper list
// Command handling (add/clear/enable/disable/list/use/listend) is in vgc.sv top
//
// Active list arrays are passed as flat packed vectors for Yosys compatibility

module vgc_copper (
    input  logic        clk,
    input  logic        rst,

    // --- Timing inputs ---
    input  logic [9:0]  h_count,
    input  logic [9:0]  v_count,
    input  logic        in_text_area,
    input  logic [8:0]  gfx_x,
    input  logic [7:0]  gfx_y,

    // --- Active list arrays (packed: 32 entries per vector) ---
    input  logic [32*17-1:0] copper_pos_flat,
    input  logic [32*8-1:0]  copper_reg_flat,
    input  logic [32*8-1:0]  copper_val_flat,
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

    // Unpack flat vectors into local arrays
    logic [16:0] copper_pos [0:31];
    logic [7:0]  copper_reg [0:31];
    logic [7:0]  copper_val [0:31];

    always_comb begin
        for (int i = 0; i < 32; i++) begin
            copper_pos[i] = copper_pos_flat[i*17 +: 17];
            copper_reg[i] = copper_reg_flat[i*8 +: 8];
            copper_val[i] = copper_val_flat[i*8 +: 8];
        end
    end

    // =========================================================================
    // Copper execution — beam position compare + register writeback
    // =========================================================================
    wire [16:0] beam_pos = gfx_addr_xy(gfx_x, gfx_y);

    always_ff @(posedge clk) begin
        if (rst) begin
            copper_index <= 0;
            copper_fire <= 0;
        end else begin
            copper_fire <= 0;

            // Vblank: reset index
            if (h_count == 0 && v_count == V_ACTIVE) begin
                copper_index <= 0;
            end

            // Fire copper entries when beam reaches their position
            if (copper_enabled && in_text_area &&
                copper_index < copper_count &&
                beam_pos >= copper_pos[copper_index]) begin
                copper_fire <= 1;
                copper_fire_reg <= copper_reg[copper_index];
                copper_fire_val <= copper_val[copper_index];
                copper_index <= copper_index + 1;
            end
        end
    end

endmodule
