// File I/O register bank.
//
// 80-byte register file at $B9A0-$B9EF. The CPU pokes these registers to
// request file operations; the ESP32 NovaHost services them over the
// debug bridge.
//
// When the CPU writes a non-zero value to $B9A0 (FioCmd), the module
// asserts `fio_event` for one clock. The debug bridge captures this and
// emits an async EVENT_FIO packet to the ESP. The ESP then reads the
// full register bank via CMD_PEEK, executes the command, writes results
// back via CMD_POKE, and flips FioStatus to 2 (ok) or 3 (err).
//
// Register map (mirrors VgcConstants.cs):
//   +00  FioCmd       (write non-zero → fio_event pulse)
//   +01  FioStatus    (0=idle, 2=ok, 3=err)
//   +02  FioErrCode
//   +03  FioNameLen
//   +04/05 FioSrcL/H
//   +06/07 FioEndL/H
//   +08/09 FioSizeL/H
//   +0A-0E  graphics selector + offsets
//   +0F  FioDirType
//   +10-4F  FioName (64 bytes)
//
// The module does not interpret commands — pure storage + write
// notification.

module fio (
    input  logic        clk,
    input  logic        rst,

    // CPU bus — writes from the 6502 trigger fio_event on the cmd reg.
    input  logic        cpu_ce,
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    output logic [7:0]  cpu_rdata,

    // Debug-bridge write port — ESP writes status/result back through
    // this path. Does NOT trigger fio_event (ESP is the consumer, not
    // the producer).
    input  logic        dbg_we,
    input  logic [6:0]  dbg_addr,    // offset 0..79 within the bank
    input  logic [7:0]  dbg_wdata,
    input  logic [6:0]  dbg_raddr,
    output logic [7:0]  dbg_rdata,

    // One-clock pulse when the CPU writes a non-zero value to FioCmd.
    output logic        fio_event
);

    // Base address: $B9A0. Bank size: 80 bytes.
    localparam logic [15:0] BASE = 16'hB9A0;
    localparam logic [15:0] LAST = 16'hB9EF;
    localparam int          SIZE = 80;

    reg [7:0] bank [0:SIZE-1];

    // Selected when CPU address falls within the bank range.
    wire sel = (cpu_addr >= BASE) && (cpu_addr <= LAST);
    wire [6:0] cpu_off = cpu_addr[6:0] - BASE[6:0];  // 0..79 within bank

    // Combinational read port — in-range addresses return the stored
    // byte; out-of-range returns zero so top.sv can OR into its cpu_din
    // mux without disturbing other peripherals.
    assign cpu_rdata = sel ? bank[cpu_off] : 8'h00;
    assign dbg_rdata = (dbg_raddr < SIZE) ? bank[dbg_raddr] : 8'h00;

    // Event pulse: one clock high when the CPU writes a non-zero value
    // to FioCmd ($B9A0). Debug-bridge writes never pulse.
    wire cpu_cmd_write = cpu_ce && cpu_we && sel && (cpu_off == 7'h00);
    assign fio_event   = cpu_cmd_write && (cpu_wdata != 8'h00);

    // Storage updates — reset clears all bytes; CPU writes (on cpu_ce)
    // and debug writes both land in the same storage. CPU wins if both
    // fire in the same cycle (unlikely in practice but defined).
    integer i;
    always_ff @(posedge clk) begin
        if (rst) begin
            for (i = 0; i < SIZE; i = i + 1)
                bank[i] <= 8'h00;
        end else begin
            if (dbg_we)
                bank[dbg_addr] <= dbg_wdata;
            if (cpu_ce && cpu_we && sel)
                bank[cpu_off] <= cpu_wdata;
        end
    end

endmodule
