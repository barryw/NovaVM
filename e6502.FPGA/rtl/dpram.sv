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

    reg [WIDTH-1:0] mem [0:DEPTH-1];

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
