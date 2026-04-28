// Dual-port Block RAM — inferred by Yosys as ECP5 DP16KD
// Port A: read/write (CPU, commands, blitter)
// Port B: read-only (renderer, readback)
// Both ports synchronous — reads have 1-cycle latency

module dpram #(
    parameter WIDTH = 8,
    parameter DEPTH = 2048,
    parameter INIT_FILE = ""
)(
    input  wire                       clk,

    // Port A (read/write)
    input  wire [$clog2(DEPTH)-1:0]   addr_a,
    input  wire [WIDTH-1:0]           din_a,
    input  wire                       we_a,
    output reg  [WIDTH-1:0]           dout_a,

    // Port B (read-only)
    input  wire [$clog2(DEPTH)-1:0]   addr_b,
    output reg  [WIDTH-1:0]           dout_b
);

    // Force BRAM mapping. Without this, yosys's `memory_libmap` may
    // pick distributed RAM (TRELLIS_DPR16X4) for small memories — for the
    // SLB (320×7=2240 bits) yosys did exactly that, and the cascade-write
    // logic for cell-banked distributed dual-port RAM dropped writes,
    // making sprite scanline buffer effectively non-clearable. Symptom
    // 2026-04-28: vertical sprite-bar persisted across cold_starts and
    // accumulated old sprite-X positions. (* ram_style = "block" *) tells
    // yosys to force DP16KD even for small memories. Cost: one DP16KD per
    // small dpram instance (16Kbit each, mostly unused for SLB) — well
    // worth correctness.
    (* ram_style = "block" *)
    reg [WIDTH-1:0] mem [0:DEPTH-1];

    // POR init — output regs MUST start at 0. Without this, dout_a/dout_b
    // hold whatever bits the silicon's output latch came up with for the
    // first cycle after rst-release. With every BRAM in the design (char
    // RAM, font ROM, color RAM, gfx RAM, sprite RAM, palette, ROM banks,
    // etc.) doing the same, the visible pipeline samples 1 cycle of
    // garbage from EACH and renders intermittent wrong glyphs across boots.
    // Cannot use port-level `output reg = 0` because yosys rejects port-
    // default initializers. Diagnosed 2026-04-26 with iteration sweep
    // (1/5 boots showed garbage chars on rows 0-1 with char RAM verified
    // correct via peek — i.e., RAM had spaces but render-side output was
    // showing previous-state $AA).
    initial begin
        dout_a = 0;
        dout_b = 0;
    end

    // Yosys cannot process $readmemh inside a runtime conditional.
    // Use a generate block so the condition is resolved at elaboration time.
    // The zero-init loop is also kept in a separate initial block and only
    // used when there is no INIT_FILE — otherwise it would override the
    // $readmemh data during Yosys memory-init collection.
    generate
        if (INIT_FILE != "") begin : gen_init_file
            initial $readmemh(INIT_FILE, mem);
        end else begin : gen_init_zero
            integer i;
            initial begin
                for (i = 0; i < DEPTH; i = i + 1)
                    mem[i] = 0;
            end
        end
    endgenerate

    // Port A: write + read
    always @(posedge clk) begin
        if (we_a)
            mem[addr_a] <= din_a;
        dout_a <= mem[addr_a];
    end

    // Port B: read-only
    always @(posedge clk) begin
        dout_b <= mem[addr_b];
    end

endmodule
