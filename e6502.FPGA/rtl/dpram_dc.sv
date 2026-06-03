// Dual-CLOCK dual-port Block RAM — inferred by Yosys as ECP5 DP16KD with
// independent per-port clocks.
//
// This is the dual-clock sibling of dpram.sv. The single-clock dpram.sv is
// used by the vast majority of the design's memories and MUST NOT change.
// dpram_dc.sv exists only where the two ports legitimately live in different
// clock domains — currently ext_rom_inst, whose port-A write is driven from
// the SDRAM clock domain (page_dma / CDC'd boot bridge) while port B is read
// by the CPU on the pixel clock. The ECP5 DP16KD natively supports a separate
// clock per port, so this maps to a single block RAM with no extra logic.
//
// Port A (clk_a): read/write
// Port B (clk_b): read-only
// Both ports synchronous — reads have 1-cycle latency in their own domain.
//
// NOTE: this primitive provides NO data-path CDC. It is the caller's job to
// only present port-A inputs that are stable/synchronized in the clk_a domain
// and port-B inputs stable in the clk_b domain. (For ext_rom the boot writes
// are quasi-static and synchronized into clk_a by top.sv; the CPU read port is
// a single reader entirely in clk_b.)

module dpram_dc #(
    parameter WIDTH = 8,
    parameter DEPTH = 2048,
    parameter INIT_FILE = ""
)(
    // Port A (read/write) clock
    input  wire                       clk_a,
    input  wire [$clog2(DEPTH)-1:0]   addr_a,
    input  wire [WIDTH-1:0]           din_a,
    input  wire                       we_a,
    output reg  [WIDTH-1:0]           dout_a,

    // Port B (read-only) clock
    input  wire                       clk_b,
    input  wire [$clog2(DEPTH)-1:0]   addr_b,
    output reg  [WIDTH-1:0]           dout_b
);

    // Force BRAM mapping — same rationale as dpram.sv. Without this, yosys's
    // memory_libmap may pick distributed RAM (TRELLIS_DPR16X4) for smaller
    // memories; distributed RAM also cannot honour two independent clocks the
    // way DP16KD does. ext_rom is 16Kbit so it would map to BRAM anyway, but
    // we keep the attribute for parity and to be future-proof for smaller
    // dual-clock memories.
    (* ram_style = "block" *)
    reg [WIDTH-1:0] mem [0:DEPTH-1];

    // POR init — both output regs MUST start at 0. Carried over from dpram.sv:
    // the silicon's output latch comes up indeterminate, and with every BRAM
    // in the design starting from a known 0 the visible pipeline samples a
    // defined value on the first cycle after rst-release instead of garbage.
    // Cannot use port-level `output reg = 0` because yosys rejects port-default
    // initializers.
    initial begin
        dout_a = 0;
        dout_b = 0;
    end

    // Yosys cannot process $readmemh inside a runtime conditional — use a
    // generate block so the condition is resolved at elaboration time. The
    // zero-init loop is kept separate and only used when there is no
    // INIT_FILE, otherwise it would override the $readmemh data during yosys
    // memory-init collection.
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

    // Port A: write + read, clocked by clk_a
    always @(posedge clk_a) begin
        if (we_a)
            mem[addr_a] <= din_a;
        dout_a <= mem[addr_a];
    end

    // Port B: read-only, clocked by clk_b
    always @(posedge clk_b) begin
        dout_b <= mem[addr_b];
    end

endmodule
