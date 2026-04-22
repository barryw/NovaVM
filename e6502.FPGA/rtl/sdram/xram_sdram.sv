// XRAM-over-SDRAM wrapper — replaces the 4KB BRAM xram stub with a
// pixel-clock-facing handshake to the 100 MHz sdram.v controller port A.
//
// Interface matches the prior dpram stub from the outside (req_addr /
// req_din / req_we / req_re / req_dout) with two additions:
//   busy — assert while a transaction is in flight; back-pressure for CPU
//   req_re — read strobe (the BRAM stub read unconditionally; SDRAM
//            transactions are explicit)
//
// Timing: the sdram.v state machine completes one port-A transaction per
// clkref HIGH half-period (4 pixel clocks at 16:1 SDRAM:clkref ratio with
// 100 MHz SDRAM + 6.25 MHz clkref + 25 MHz pixel). We conservatively hold
// the SDRAM request asserted for 8 pixel clocks (~2 clkref periods) —
// guarantees the controller's clkref-HIGH window covers our request
// regardless of which phase the request arrives in.

module xram_sdram (
    input  logic        clk,         // pixel clock
    input  logic        rst,

    // CPU/XMC-facing request port
    input  logic [18:0] req_addr,    // 19-bit byte address (512 KB)
    input  logic [7:0]  req_din,
    input  logic        req_we,
    input  logic        req_re,
    output logic [7:0]  req_dout,
    output logic        busy,

    // SDRAM port A connection (driven from pixel-clock domain — sdram.v
    // samples on clk_sdram; the conservative hold window makes signals
    // stable across at least one full clkref HIGH period)
    output logic [24:0] sdram_addr,
    output logic [7:0]  sdram_din,
    output logic        sdram_we,
    output logic        sdram_oe,
    input  logic [7:0]  sdram_dout
);

    // Hold an SDRAM request asserted for this many pixel clocks to cover
    // one full clkref period worst-case plus read-data latch margin.
    localparam int HOLD_CYCLES = 8;

    typedef enum logic [1:0] {
        S_IDLE,
        S_ACTIVE,
        S_CAPTURE
    } state_t;

    state_t      state;
    logic [3:0]  hold_cnt;
    logic [18:0] lat_addr;
    logic [7:0]  lat_din;
    logic        lat_is_write;
    logic [7:0]  r_dout;

    assign busy       = (state != S_IDLE);
    assign sdram_addr = {6'b0, lat_addr};
    assign sdram_din  = lat_din;
    assign sdram_we   = (state == S_ACTIVE) &&  lat_is_write;
    assign sdram_oe   = (state == S_ACTIVE) && !lat_is_write;
    assign req_dout   = r_dout;

    always_ff @(posedge clk) begin
        if (rst) begin
            state        <= S_IDLE;
            hold_cnt     <= 0;
            lat_addr     <= 0;
            lat_din      <= 0;
            lat_is_write <= 0;
            r_dout       <= 0;
        end else begin
            case (state)
                S_IDLE: begin
                    if (req_we) begin
                        lat_addr     <= req_addr;
                        lat_din      <= req_din;
                        lat_is_write <= 1'b1;
                        hold_cnt     <= HOLD_CYCLES - 1;
                        state        <= S_ACTIVE;
                    end else if (req_re) begin
                        lat_addr     <= req_addr;
                        lat_is_write <= 1'b0;
                        hold_cnt     <= HOLD_CYCLES - 1;
                        state        <= S_ACTIVE;
                    end
                end
                S_ACTIVE: begin
                    if (hold_cnt == 0) begin
                        // For reads, capture the controller's doutA one
                        // pixel clock after deasserting oeA so the value
                        // settles even under pessimistic CDC.
                        if (!lat_is_write) begin
                            state <= S_CAPTURE;
                        end else begin
                            state <= S_IDLE;
                        end
                    end else begin
                        hold_cnt <= hold_cnt - 1;
                    end
                end
                S_CAPTURE: begin
                    r_dout <= sdram_dout;
                    state  <= S_IDLE;
                end
                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
