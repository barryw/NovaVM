// Debug bridge — binary protocol decoder/encoder
// Receives commands from UART RX, drives debug ports on top.sv,
// sends responses via UART TX.
//
// Binary protocol (ESP32 → FPGA):
//   Byte 0: Command (0x01-0x09)
//   Byte 1-2: Address big-endian (PEEK/POKE/PEEK_BLOCK)
//   Byte 3: Data (POKE) / Count (PEEK_BLOCK) / Key (SEND_KEY)
//
// Response (FPGA → ESP32):
//   Byte 0: Status (0x00=OK, 0xFF=error)
//   Byte 1+: Payload (variable length per command)

module debug_bridge (
    input  logic        clk,
    input  logic        rst,

    // UART interface
    input  logic [7:0]  rx_data,
    input  logic        rx_valid,
    output logic [7:0]  tx_data,
    output logic        tx_start,
    input  logic        tx_busy,

    // Debug memory interface (active only when bridge drives them)
    output logic        dbg_peek_en,
    output logic [15:0] dbg_peek_addr,
    input  logic [7:0]  dbg_peek_data,
    output logic        dbg_poke_en,
    output logic [15:0] dbg_poke_addr,
    output logic [7:0]  dbg_poke_data,
    output logic        dbg_pause,

    // CPU state (directly wired from top.sv)
    input  logic [15:0] dbg_cpu_pc,
    input  logic [7:0]  dbg_cpu_a,
    input  logic [7:0]  dbg_cpu_x,
    input  logic [7:0]  dbg_cpu_y,
    input  logic [7:0]  dbg_cpu_sp,
    input  logic [7:0]  dbg_cpu_flags,

    // Key injection
    output logic        key_inject_valid,
    output logic [7:0]  key_inject_data
);

    // =========================================================================
    // Command encoding
    // =========================================================================
    localparam CMD_PEEK        = 8'h01;
    localparam CMD_POKE        = 8'h02;
    localparam CMD_SEND_KEY    = 8'h03;
    localparam CMD_READ_SCREEN = 8'h04;
    localparam CMD_READ_COLOR  = 8'h05;
    localparam CMD_CPU_STATE   = 8'h06;
    localparam CMD_PAUSE       = 8'h07;
    localparam CMD_RESUME      = 8'h08;
    localparam CMD_PEEK_BLOCK  = 8'h09;

    localparam SCREEN_BASE = 16'hAA00;
    localparam COLOR_BASE  = 16'hB1D0;
    localparam BULK_SIZE   = 16'd2000;

    // =========================================================================
    // State machine
    // =========================================================================
    typedef enum logic [3:0] {
        S_IDLE,          // 0: wait for command byte
        S_RECV,          // 1: collecting parameter bytes
        S_PEEK_WAIT,     // 2: dpram read latency (2 cycles)
        S_POKE_DO,       // 3: assert poke for 1 cycle
        S_KEY_DO,        // 4: pulse key_valid for 1 cycle
        S_CPU_LATCH,     // 5: latch CPU state registers
        S_TX_BYTE,       // 6: start transmitting one byte
        S_TX_WAIT,       // 7: wait for TX to complete
        S_BULK_PEEK,     // 8: set peek addr for bulk read
        S_BULK_WAIT,     // 9: dpram latency for bulk read
        S_BULK_TX,       // 10: transmit bulk data byte
        S_BULK_TX_WAIT,  // 11: wait for bulk TX to complete
        S_POKE_DONE,     // 12: poke deassert cycle
        S_KEY_DONE,      // 13: key deassert cycle
        S_UNUSED_14,     // 14
        S_UNUSED_15      // 15
    } state_t;

    state_t state;

    // Command and parameters
    logic [7:0]  cmd;
    logic [7:0]  param0, param1, param2;   // received parameter bytes
    logic [1:0]  recv_cnt;                 // bytes received so far
    logic [1:0]  recv_need;                // bytes needed for this command

    // Derived fields
    wire [15:0] cmd_addr = {param0, param1};
    wire [7:0]  cmd_data = param2;
    wire [7:0]  cmd_key  = param0;
    wire [7:0]  cmd_count = param2;

    // Response tracking
    logic [3:0]  resp_idx;     // current byte being sent (0=status)
    logic [3:0]  resp_total;   // total bytes to send

    // Peek data latch
    logic [7:0]  peek_latch;
    logic        wait_done;    // dpram latency counter

    // CPU state latches
    logic [15:0] cpu_pc_l;
    logic [7:0]  cpu_a_l, cpu_x_l, cpu_y_l, cpu_sp_l, cpu_flags_l;

    // Bulk read state
    logic [15:0] bulk_addr;
    logic [10:0] bulk_remaining;   // 0-2000

    // Pause register
    logic paused;
    assign dbg_pause = paused;

    // Error flag
    logic status_err;

    // =========================================================================
    // TX data mux — selects byte to transmit based on command and index
    // =========================================================================
    logic [7:0] tx_byte_mux;

    always_comb begin
        if (status_err)
            tx_byte_mux = 8'hFF;
        else if (resp_idx == 0)
            tx_byte_mux = 8'h00;
        else begin
            case (cmd)
                CMD_PEEK:      tx_byte_mux = peek_latch;
                CMD_CPU_STATE: begin
                    case (resp_idx)
                        4'd1:    tx_byte_mux = cpu_pc_l[15:8];
                        4'd2:    tx_byte_mux = cpu_pc_l[7:0];
                        4'd3:    tx_byte_mux = cpu_a_l;
                        4'd4:    tx_byte_mux = cpu_x_l;
                        4'd5:    tx_byte_mux = cpu_y_l;
                        4'd6:    tx_byte_mux = cpu_sp_l;
                        4'd7:    tx_byte_mux = cpu_flags_l;
                        4'd8:    tx_byte_mux = 8'h00;
                        default: tx_byte_mux = 8'h00;
                    endcase
                end
                default: tx_byte_mux = 8'h00;
            endcase
        end
    end

    // =========================================================================
    // Main state machine
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            state            <= S_IDLE;
            cmd              <= 0;
            param0           <= 0;
            param1           <= 0;
            param2           <= 0;
            recv_cnt         <= 0;
            recv_need        <= 0;
            resp_idx         <= 0;
            resp_total       <= 0;
            peek_latch       <= 0;
            wait_done        <= 0;
            cpu_pc_l         <= 0;
            cpu_a_l          <= 0;
            cpu_x_l          <= 0;
            cpu_y_l          <= 0;
            cpu_sp_l         <= 0;
            cpu_flags_l      <= 0;
            bulk_addr        <= 0;
            bulk_remaining   <= 0;
            paused           <= 0;
            status_err       <= 0;
            dbg_peek_en      <= 0;
            dbg_peek_addr    <= 0;
            dbg_poke_en      <= 0;
            dbg_poke_addr    <= 0;
            dbg_poke_data    <= 0;
            key_inject_valid <= 0;
            key_inject_data  <= 0;
            tx_data          <= 0;
            tx_start         <= 0;
        end else begin
            // Default: clear single-cycle pulses
            tx_start         <= 0;
            dbg_poke_en      <= 0;
            key_inject_valid <= 0;

            case (state)

                // ---------------------------------------------------------
                // IDLE: wait for command byte
                // ---------------------------------------------------------
                S_IDLE: begin
                    dbg_peek_en <= 0;
                    if (rx_valid) begin
                        cmd        <= rx_data;
                        recv_cnt   <= 0;
                        status_err <= 0;
                        case (rx_data)
                            CMD_PEEK:        begin recv_need <= 2; state <= S_RECV; end
                            CMD_POKE:        begin recv_need <= 2'd3; state <= S_RECV; end
                            CMD_SEND_KEY:    begin recv_need <= 1; state <= S_RECV; end
                            CMD_PEEK_BLOCK:  begin recv_need <= 2'd3; state <= S_RECV; end

                            CMD_READ_SCREEN: begin
                                bulk_addr      <= SCREEN_BASE;
                                bulk_remaining <= BULK_SIZE;
                                resp_idx       <= 0;
                                resp_total     <= 1;   // status byte only, then bulk
                                state          <= S_TX_BYTE;
                            end

                            CMD_READ_COLOR: begin
                                bulk_addr      <= COLOR_BASE;
                                bulk_remaining <= BULK_SIZE;
                                resp_idx       <= 0;
                                resp_total     <= 1;
                                state          <= S_TX_BYTE;
                            end

                            CMD_CPU_STATE: begin
                                state <= S_CPU_LATCH;
                            end

                            CMD_PAUSE: begin
                                paused     <= 1;
                                resp_idx   <= 0;
                                resp_total <= 1;
                                state      <= S_TX_BYTE;
                            end

                            CMD_RESUME: begin
                                paused     <= 0;
                                resp_idx   <= 0;
                                resp_total <= 1;
                                state      <= S_TX_BYTE;
                            end

                            default: begin
                                // Unknown command — send error status
                                status_err <= 1;
                                resp_idx   <= 0;
                                resp_total <= 1;
                                bulk_remaining <= 0;
                                state      <= S_TX_BYTE;
                            end
                        endcase
                    end
                end

                // ---------------------------------------------------------
                // RECV: collect parameter bytes
                // ---------------------------------------------------------
                S_RECV: begin
                    if (rx_valid) begin
                        case (recv_cnt)
                            2'd0: param0 <= rx_data;
                            2'd1: param1 <= rx_data;
                            2'd2: param2 <= rx_data;
                            default: ;
                        endcase
                        recv_cnt <= recv_cnt + 1;

                        if (recv_cnt + 1 == recv_need) begin
                            // All parameter bytes received — dispatch
                            case (cmd)
                                CMD_PEEK: begin
                                    // param0=addr_hi, param1 will be set this cycle
                                    // but registered value won't be available until
                                    // next cycle. Use rx_data for addr_lo.
                                    dbg_peek_addr <= {param0, rx_data};
                                    dbg_peek_en   <= 1;
                                    wait_done     <= 0;
                                    state         <= S_PEEK_WAIT;
                                end

                                CMD_POKE: begin
                                    dbg_poke_addr <= {param0, param1};
                                    dbg_poke_data <= rx_data;
                                    dbg_poke_en   <= 1;
                                    state         <= S_POKE_DO;
                                end

                                CMD_SEND_KEY: begin
                                    key_inject_data  <= rx_data;
                                    key_inject_valid <= 1;
                                    state            <= S_KEY_DO;
                                end

                                CMD_PEEK_BLOCK: begin
                                    bulk_addr      <= {param0, param1};
                                    bulk_remaining <= (rx_data == 0) ? 11'd256 : {3'b0, rx_data};
                                    resp_idx       <= 0;
                                    resp_total     <= 1;  // status, then bulk
                                    state          <= S_TX_BYTE;
                                end

                                default: begin
                                    state <= S_IDLE;
                                end
                            endcase
                        end
                    end
                end

                // ---------------------------------------------------------
                // PEEK_WAIT: wait for dpram read latency (2 edges)
                // Edge 0: address registered, dpram captures on next edge
                // Edge 1: dpram output valid, latch it
                // ---------------------------------------------------------
                S_PEEK_WAIT: begin
                    if (wait_done) begin
                        peek_latch  <= dbg_peek_data;
                        dbg_peek_en <= 0;
                        resp_idx    <= 0;
                        resp_total  <= 2;  // status + data byte
                        bulk_remaining <= 0;
                        state       <= S_TX_BYTE;
                    end else begin
                        wait_done <= 1;
                    end
                end

                // ---------------------------------------------------------
                // POKE_DO: poke_en was asserted on entry, hold 1 cycle
                // ---------------------------------------------------------
                S_POKE_DO: begin
                    // poke_en cleared by default pulse clear at top
                    state      <= S_POKE_DONE;
                end

                S_POKE_DONE: begin
                    resp_idx       <= 0;
                    resp_total     <= 1;
                    bulk_remaining <= 0;
                    state          <= S_TX_BYTE;
                end

                // ---------------------------------------------------------
                // KEY_DO: key_valid was pulsed on entry
                // ---------------------------------------------------------
                S_KEY_DO: begin
                    // key_inject_valid cleared by default pulse clear at top
                    state      <= S_KEY_DONE;
                end

                S_KEY_DONE: begin
                    resp_idx       <= 0;
                    resp_total     <= 1;
                    bulk_remaining <= 0;
                    state          <= S_TX_BYTE;
                end

                // ---------------------------------------------------------
                // CPU_LATCH: snapshot all CPU registers
                // ---------------------------------------------------------
                S_CPU_LATCH: begin
                    cpu_pc_l    <= dbg_cpu_pc;
                    cpu_a_l     <= dbg_cpu_a;
                    cpu_x_l     <= dbg_cpu_x;
                    cpu_y_l     <= dbg_cpu_y;
                    cpu_sp_l    <= dbg_cpu_sp;
                    cpu_flags_l <= dbg_cpu_flags;
                    resp_idx    <= 0;
                    resp_total  <= 4'd9;   // status + 8 data bytes
                    bulk_remaining <= 0;
                    state       <= S_TX_BYTE;
                end

                // ---------------------------------------------------------
                // TX_BYTE: transmit one response byte from the mux
                // ---------------------------------------------------------
                S_TX_BYTE: begin
                    if (!tx_busy) begin
                        tx_data  <= tx_byte_mux;
                        tx_start <= 1;
                        state    <= S_TX_WAIT;
                    end
                end

                // ---------------------------------------------------------
                // TX_WAIT: wait for TX to complete, advance or finish
                // ---------------------------------------------------------
                S_TX_WAIT: begin
                    if (!tx_busy) begin
                        if (resp_idx + 1 < resp_total) begin
                            // More response bytes to send
                            resp_idx <= resp_idx + 1;
                            state    <= S_TX_BYTE;
                        end else if (bulk_remaining > 0) begin
                            // Switch to bulk read mode
                            state <= S_BULK_PEEK;
                        end else begin
                            state <= S_IDLE;
                        end
                    end
                end

                // ---------------------------------------------------------
                // BULK_PEEK: set peek address for next bulk byte
                // ---------------------------------------------------------
                S_BULK_PEEK: begin
                    dbg_peek_addr <= bulk_addr;
                    dbg_peek_en   <= 1;
                    wait_done     <= 0;
                    state         <= S_BULK_WAIT;
                end

                // ---------------------------------------------------------
                // BULK_WAIT: dpram latency (same as PEEK_WAIT)
                // ---------------------------------------------------------
                S_BULK_WAIT: begin
                    if (wait_done) begin
                        peek_latch <= dbg_peek_data;
                        dbg_peek_en <= 0;
                        state      <= S_BULK_TX;
                    end else begin
                        wait_done <= 1;
                    end
                end

                // ---------------------------------------------------------
                // BULK_TX: transmit the bulk data byte
                // ---------------------------------------------------------
                S_BULK_TX: begin
                    if (!tx_busy) begin
                        tx_data  <= peek_latch;
                        tx_start <= 1;
                        state    <= S_BULK_TX_WAIT;
                    end
                end

                // ---------------------------------------------------------
                // BULK_TX_WAIT: wait for TX, advance bulk counter
                // ---------------------------------------------------------
                S_BULK_TX_WAIT: begin
                    if (!tx_busy) begin
                        bulk_addr      <= bulk_addr + 1;
                        bulk_remaining <= bulk_remaining - 1;
                        if (bulk_remaining == 1) begin
                            state <= S_IDLE;
                        end else begin
                            state <= S_BULK_PEEK;
                        end
                    end
                end

                default: state <= S_IDLE;

            endcase
        end
    end

endmodule
