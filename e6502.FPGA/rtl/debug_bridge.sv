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

module debug_bridge #(
    // If NovaHost never claims the boot by sending CMD_RESET_HOLD, release
    // CPU reset after this many clk cycles. The synthesis ROM BRAMs are now
    // initialized from rom/*.hex, so this gives the board a standalone boot
    // path after FPGA flash/power-cycle even when ESP is stale or absent.
    parameter integer BOOT_AUTO_RELEASE_CYCLES = 50_000_000
) (
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

    // Direct VGC memory write path — used by NovaHost while the CPU is held
    // in reset to stream boot splash assets into the 320x200 graphics plane.
    output logic        dbg_vmem_we,
    output logic [2:0]  dbg_vmem_space,
    output logic [16:0] dbg_vmem_addr,
    output logic [7:0]  dbg_vmem_data,

    // ROM write port — single-cycle pulse writes one byte to a ROM bank.
    // dbg_rom_idx: 0=basic_rom, 1=ext_rom. dbg_rom_addr is the 14-bit offset
    // within the 16KB ROM.
    output logic        dbg_rom_we,
    output logic        dbg_rom_idx,
    output logic [13:0] dbg_rom_addr,
    output logic [7:0]  dbg_rom_data,

    // CPU soft-reset — asserts while high. NovaHost holds this during ROM
    // load so the CPU fetches a clean reset vector from the freshly-loaded
    // ROM when it's released. OR'd with the top-level rst inside top.sv.
    output logic        dbg_cpu_reset,

    // CPU state (directly wired from top.sv)
    input  logic [15:0] dbg_cpu_pc,
    input  logic [7:0]  dbg_cpu_a,
    input  logic [7:0]  dbg_cpu_x,
    input  logic [7:0]  dbg_cpu_y,
    input  logic [7:0]  dbg_cpu_sp,
    input  logic [7:0]  dbg_cpu_flags,

    // Key injection
    output logic        key_inject_valid,
    output logic [7:0]  key_inject_data,

    // SDRAM port B write (used during CPU-reset to preload tables like the
    // 6581 filter curve — see CMD_POKE_SDRAM_BLK below). Only drives weB/dinB
    // while actively writing; top.sv muxes these against sid_curve_reader.
    output logic        sdram_b_we,
    output logic [24:0] sdram_b_addr,
    output logic [7:0]  sdram_b_din,

    // File I/O event — pulses one clock when the CPU writes a non-zero
    // value to FioCmd ($B9A0). Bridge latches the pulse and emits an
    // async 2-byte EVENT_FIO sequence (0xFE 0xE0) to the ESP32 the
    // next time it enters S_IDLE with no RX pending.
    input  logic        fio_event
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
    localparam CMD_POKE_ROM    = 8'h0A;  // [idx, addr_hi, addr_lo, data]
    localparam CMD_RESET_HOLD  = 8'h0B;  // assert cpu reset (level-held)
    localparam CMD_RESET_REL   = 8'h0C;  // release cpu reset
    localparam CMD_POKE_ROM_BLK= 8'h0D;  // [idx, addr_hi, addr_lo, count, ...data]
                                         // count=0 means 256. Single ack at end.
    localparam CMD_POKE_SDRAM_BLK=8'h0E; // [addr_hi, addr_mid, addr_lo, count, ...data]
                                         // 24-bit byte address into SDRAM. count=0
                                         // means 256. Used to preload the 6581
                                         // filter curve at SDRAM byte offset $80_0000
                                         // during CPU reset-hold.
    localparam CMD_POKE_BLOCK  = 8'h0F;  // [addr_hi, addr_lo, count, ...data]
                                         // count=0 means 256. Streams data bytes
                                         // into CPU RAM one byte per dbg_poke pulse.
                                         // Used by FIO LOAD to deliver up to 64KB
                                         // of file payload without per-byte CMD
                                         // overhead.
    localparam CMD_POKE_VGC_BLK=8'h10;   // [space, addr_hi, addr_lo, count, ...data]
                                         // count=0 means 256. Streams directly into
                                         // VGC internal memory spaces.
    localparam CMD_FILL_VGC_BLK=8'h11;   // [space, addr_hi, addr_lo, value]
                                         // Fills exactly 256 bytes in a VGC space.

    // Async FPGA→ESP event marker bytes. Sent as a 2-byte sequence
    // (EVENT_MARKER, EVENT_TYPE_FIO) when fio_event pulses. EVENT_MARKER
    // (0xFE) is unused by the request/response protocol (which uses 0x00
    // for status-OK and 0xFF for status-error), so the ESP's RX handler
    // can dispatch on 0xFE in its idle path.
    localparam EVENT_MARKER    = 8'hFE;
    localparam EVENT_TYPE_FIO  = 8'hE0;

    // SDRAM port B write pulse width — matches sid_curve_reader's HOLD_CYCLES
    // budget for a full clkref 16:1 cycle plus CAS pipeline.
    localparam int SDRAM_HOLD = 8;

    // =========================================================================
    // State machine
    // =========================================================================
    typedef enum logic [4:0] {
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
        S_ROM_BULK_WRITE,// 14: stream incoming bytes into ROM
        S_SDRAM_RECV,    // 15: wait for next SDRAM data byte
        S_SDRAM_HOLD,    // 16: hold SDRAM we for SDRAM_HOLD cycles
        S_EVENT_TX_MARK, // 17: async event — transmit marker byte
        S_EVENT_TX_MARK_WAIT,// 18: wait for marker TX to complete
        S_EVENT_TX_TYPE, // 19: async event — transmit type byte
        S_EVENT_TX_TYPE_WAIT,// 20: wait for type TX to complete
        S_RAM_BULK_WRITE,// 21: stream incoming bytes into CPU RAM
        S_VGC_BULK_WRITE,// 22: stream incoming bytes into VGC memory
        S_VGC_FILL_WRITE // 23: fill 256 bytes in VGC memory
    } state_t;

    state_t state;

    // Command and parameters
    logic [7:0]  cmd;
    logic [7:0]  param0, param1, param2, param3;   // received parameter bytes
    logic [2:0]  recv_cnt;                         // bytes received so far
    logic [2:0]  recv_need;                        // bytes needed for this command

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

    // SDRAM bulk-write state (port B)
    logic [24:0] sdram_bulk_addr;
    logic [3:0]  sdram_hold_cnt;
    logic [2:0]  vgc_bulk_space;
    logic [7:0]  vgc_fill_value;

    // Async FIO event latch — fio_event is a 1-clock pulse; stash it
    // until we can send the 2-byte marker sequence to the ESP.
    logic event_pending;

    // 1-byte RX buffer — the UART RX module has no FIFO, so if an
    // incoming byte arrives while the state machine is in a non-RX
    // state (e.g. emitting an async event or waiting on TX), the pulse
    // would be lost. Buffer any rx_valid pulse and present it to the
    // state machine via rx_buf_valid until consumed.
    logic        rx_buf_valid;
    logic [7:0]  rx_buf_data;

    // Boot/reset ownership. NovaHost claims the boot path with RESET_HOLD and
    // owns reset until RESET_REL. If it never claims reset, the bridge falls
    // back to the bitstream-initialized ROM contents.
    logic [31:0] boot_auto_release_cnt;
    logic        boot_claimed_by_host;

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
            param3           <= 0;
            recv_cnt         <= 3'd0;
            recv_need        <= 3'd0;
            dbg_rom_we       <= 0;
            dbg_rom_idx      <= 0;
            dbg_rom_addr     <= 0;
            dbg_rom_data     <= 0;
            // Hold CPU reset briefly so NovaHost can stream fresh ROM. If it
            // never claims the boot, auto-release and boot embedded ROM.
            dbg_cpu_reset    <= 1'b1;
            boot_auto_release_cnt <= 0;
            boot_claimed_by_host  <= 1'b0;
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
            dbg_vmem_we      <= 0;
            dbg_vmem_space   <= 0;
            dbg_vmem_addr    <= 0;
            dbg_vmem_data    <= 0;
            dbg_poke_addr    <= 0;
            dbg_poke_data    <= 0;
            key_inject_valid <= 0;
            key_inject_data  <= 0;
            tx_data          <= 0;
            tx_start         <= 0;
            sdram_b_we       <= 0;
            sdram_b_addr     <= 0;
            sdram_b_din      <= 0;
            sdram_bulk_addr  <= 0;
            sdram_hold_cnt   <= 0;
            vgc_bulk_space   <= 0;
            vgc_fill_value   <= 0;
            event_pending    <= 0;
            rx_buf_valid     <= 0;
            rx_buf_data      <= 0;
        end else begin
            // Default: clear single-cycle pulses
            tx_start         <= 0;
            dbg_poke_en      <= 0;
            dbg_rom_we       <= 0;
            dbg_vmem_we      <= 0;
            key_inject_valid <= 0;

            if (dbg_cpu_reset && !boot_claimed_by_host) begin
                if (boot_auto_release_cnt >= BOOT_AUTO_RELEASE_CYCLES - 1) begin
                    dbg_cpu_reset <= 1'b0;
                end else begin
                    boot_auto_release_cnt <= boot_auto_release_cnt + 1'b1;
                end
            end

            // Latch any fio_event pulse — sticks until drained by the
            // S_EVENT_TX_MARK path below.
            if (fio_event) event_pending <= 1;

            // RX buffer: latch any incoming byte if buffer is empty.
            // Consumed by state machine via `rx_buf_valid <= 0`.
            if (rx_valid && !rx_buf_valid) begin
                rx_buf_valid <= 1;
                rx_buf_data  <= rx_data;
            end

            case (state)

                // ---------------------------------------------------------
                // IDLE: wait for command byte
                // ---------------------------------------------------------
                S_IDLE: begin
                    dbg_peek_en <= 0;
                    // Priority: service any buffered CMD byte first.
                    // Event emission only runs when the RX buffer is
                    // empty — otherwise a pending CMD would be starved.
                    if (!rx_buf_valid && event_pending && !tx_busy) begin
                        state <= S_EVENT_TX_MARK;
                    end else if (rx_buf_valid) begin
                        cmd          <= rx_buf_data;
                        rx_buf_valid <= 0;           // consume
                        recv_cnt     <= 0;
                        status_err   <= 0;
                        case (rx_buf_data)
                            CMD_PEEK:        begin recv_need <= 3'd2; state <= S_RECV; end
                            CMD_POKE:        begin recv_need <= 3'd3; state <= S_RECV; end
                            CMD_SEND_KEY:    begin recv_need <= 3'd1; state <= S_RECV; end
                            CMD_PEEK_BLOCK:  begin recv_need <= 3'd3; state <= S_RECV; end
                            CMD_POKE_ROM:    begin recv_need <= 3'd4; state <= S_RECV; end
                            CMD_POKE_ROM_BLK:begin recv_need <= 3'd4; state <= S_RECV; end
                            CMD_POKE_SDRAM_BLK:begin recv_need <= 3'd4; state <= S_RECV; end
                            CMD_POKE_BLOCK:  begin recv_need <= 3'd3; state <= S_RECV; end
                            CMD_POKE_VGC_BLK:begin recv_need <= 3'd4; state <= S_RECV; end
                            CMD_FILL_VGC_BLK:begin recv_need <= 3'd4; state <= S_RECV; end

                            CMD_READ_SCREEN: begin
                                // Direct text/color windows were removed. Host-side
                                // screen readers must use the VGC VRAM port protocol.
                                status_err     <= 1;
                                bulk_remaining <= 0;
                                resp_idx       <= 0;
                                resp_total     <= 1;
                                state          <= S_TX_BYTE;
                            end

                            CMD_READ_COLOR: begin
                                // Direct text/color windows were removed. Host-side
                                // screen readers must use the VGC VRAM port protocol.
                                status_err     <= 1;
                                bulk_remaining <= 0;
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

                            CMD_RESET_HOLD: begin
                                dbg_cpu_reset          <= 1'b1;
                                boot_claimed_by_host   <= 1'b1;
                                boot_auto_release_cnt  <= 0;
                                resp_idx               <= 0;
                                resp_total             <= 1;
                                state                  <= S_TX_BYTE;
                            end

                            CMD_RESET_REL: begin
                                dbg_cpu_reset          <= 1'b0;
                                boot_claimed_by_host   <= 1'b0;
                                boot_auto_release_cnt  <= 0;
                                resp_idx               <= 0;
                                resp_total             <= 1;
                                state                  <= S_TX_BYTE;
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
                    if (rx_buf_valid) begin
                        rx_buf_valid <= 0;           // consume
                        case (recv_cnt)
                            3'd0: param0 <= rx_buf_data;
                            3'd1: param1 <= rx_buf_data;
                            3'd2: param2 <= rx_buf_data;
                            3'd3: param3 <= rx_buf_data;
                            default: ;
                        endcase
                        recv_cnt <= recv_cnt + 1;

                        if (recv_cnt + 1 == recv_need) begin
                            // All parameter bytes received — dispatch
                            case (cmd)
                                CMD_PEEK: begin
                                    // param0=addr_hi, param1 will be set this cycle
                                    // but registered value won't be available until
                                    // next cycle. Use rx_buf_data for addr_lo.
                                    dbg_peek_addr <= {param0, rx_buf_data};
                                    dbg_peek_en   <= 1;
                                    wait_done     <= 0;
                                    state         <= S_PEEK_WAIT;
                                end

                                CMD_POKE: begin
                                    dbg_poke_addr <= {param0, param1};
                                    dbg_poke_data <= rx_buf_data;
                                    dbg_poke_en   <= 1;
                                    state         <= S_POKE_DO;
                                end

                                CMD_SEND_KEY: begin
                                    key_inject_data  <= rx_buf_data;
                                    key_inject_valid <= 1;
                                    state            <= S_KEY_DO;
                                end

                                CMD_PEEK_BLOCK: begin
                                    bulk_addr      <= {param0, param1};
                                    bulk_remaining <= (rx_buf_data == 0) ? 11'd256
                                                                         : {3'b0, rx_buf_data};
                                    resp_idx       <= 0;
                                    resp_total     <= 1;  // status, then bulk
                                    state          <= S_TX_BYTE;
                                end

                                CMD_POKE_ROM: begin
                                    dbg_rom_idx  <= param0[0];
                                    dbg_rom_addr <= {param1[5:0], param2};
                                    dbg_rom_data <= rx_buf_data;
                                    dbg_rom_we   <= 1;
                                    state        <= S_POKE_DONE;
                                end

                                CMD_POKE_ROM_BLK: begin
                                    dbg_rom_idx    <= param0[0];
                                    bulk_addr      <= {param1, param2};
                                    bulk_remaining <= (rx_buf_data == 0) ? 11'd256
                                                                         : {3'b0, rx_buf_data};
                                    state          <= S_ROM_BULK_WRITE;
                                end

                                CMD_POKE_SDRAM_BLK: begin
                                    sdram_bulk_addr <= {1'b0, param0, param1, param2};
                                    bulk_remaining  <= (rx_buf_data == 0) ? 11'd256
                                                                          : {3'b0, rx_buf_data};
                                    state           <= S_SDRAM_RECV;
                                end

                                CMD_POKE_BLOCK: begin
                                    // param0=addr_hi, param1=addr_lo,
                                    // rx_buf_data=count (=0 means 256).
                                    // Data bytes stream in via S_RAM_BULK_WRITE.
                                    bulk_addr      <= {param0, param1};
                                    bulk_remaining <= (rx_buf_data == 0) ? 11'd256
                                                                         : {3'b0, rx_buf_data};
                                    state          <= S_RAM_BULK_WRITE;
                                end

                                CMD_POKE_VGC_BLK: begin
                                    vgc_bulk_space <= param0[2:0];
                                    bulk_addr      <= {param1, param2};
                                    bulk_remaining <= (rx_buf_data == 0) ? 11'd256
                                                                         : {3'b0, rx_buf_data};
                                    state          <= S_VGC_BULK_WRITE;
                                end

                                CMD_FILL_VGC_BLK: begin
                                    vgc_bulk_space <= param0[2:0];
                                    bulk_addr      <= {param1, param2};
                                    bulk_remaining <= 11'd256;
                                    vgc_fill_value <= rx_buf_data;
                                    state          <= S_VGC_FILL_WRITE;
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

                // ---------------------------------------------------------
                // ROM_BULK_WRITE: stream incoming bytes into the selected
                // ROM dpram, auto-incrementing the address. When the count
                // reaches zero, send a single status ack and return to IDLE.
                // ---------------------------------------------------------
                S_ROM_BULK_WRITE: begin
                    if (rx_buf_valid) begin
                        rx_buf_valid   <= 0;          // consume
                        dbg_rom_addr   <= bulk_addr[13:0];
                        dbg_rom_data   <= rx_buf_data;
                        dbg_rom_we     <= 1;
                        bulk_addr      <= bulk_addr + 1;
                        bulk_remaining <= bulk_remaining - 1;
                        if (bulk_remaining == 1) begin
                            // Last byte — schedule status ack
                            resp_idx   <= 0;
                            resp_total <= 1;
                            state      <= S_TX_BYTE;
                        end
                    end
                end

                // ---------------------------------------------------------
                // SDRAM_RECV: wait for next data byte. On arrival, drive
                // port B (addr + din + we=1), start hold counter.
                // ---------------------------------------------------------
                S_SDRAM_RECV: begin
                    if (rx_buf_valid) begin
                        rx_buf_valid   <= 0;          // consume
                        sdram_b_addr   <= sdram_bulk_addr;
                        sdram_b_din    <= rx_buf_data;
                        sdram_b_we     <= 1;
                        sdram_hold_cnt <= SDRAM_HOLD - 1;
                        state          <= S_SDRAM_HOLD;
                    end
                end

                // ---------------------------------------------------------
                // RAM_BULK_WRITE: stream incoming bytes into CPU RAM via
                // dbg_poke. One byte per RX → one dbg_poke pulse. Auto-
                // increments the address. Used by FIO LOAD to push file
                // payload from ESP into CPU RAM in 256-byte chunks.
                // ---------------------------------------------------------
                S_RAM_BULK_WRITE: begin
                    if (rx_buf_valid) begin
                        rx_buf_valid   <= 0;          // consume
                        dbg_poke_addr  <= bulk_addr;
                        dbg_poke_data  <= rx_buf_data;
                        dbg_poke_en    <= 1;
                        bulk_addr      <= bulk_addr + 1;
                        bulk_remaining <= bulk_remaining - 1;
                        if (bulk_remaining == 1) begin
                            // Last byte — schedule status ack.
                            resp_idx   <= 0;
                            resp_total <= 1;
                            state      <= S_TX_BYTE;
                        end
                    end
                end

                // ---------------------------------------------------------
                // VGC_BULK_WRITE: stream incoming bytes directly into VGC
                // memory. Used for boot-time graphics assets while CPU reset
                // is held.
                // ---------------------------------------------------------
                S_VGC_BULK_WRITE: begin
                    if (rx_buf_valid) begin
                        rx_buf_valid    <= 0;          // consume
                        dbg_vmem_space  <= vgc_bulk_space;
                        dbg_vmem_addr   <= {1'b0, bulk_addr};
                        dbg_vmem_data   <= rx_buf_data;
                        dbg_vmem_we     <= 1;
                        bulk_addr       <= bulk_addr + 1;
                        bulk_remaining  <= bulk_remaining - 1;
                        if (bulk_remaining == 1) begin
                            resp_idx   <= 0;
                            resp_total <= 1;
                            state      <= S_TX_BYTE;
                        end
                    end
                end

                // ---------------------------------------------------------
                // VGC_FILL_WRITE: fill exactly 256 sequential VGC bytes with
                // one host command. This keeps boot clear-to-black fast even
                // at the reliable 115200-baud debug link.
                // ---------------------------------------------------------
                S_VGC_FILL_WRITE: begin
                    dbg_vmem_space <= vgc_bulk_space;
                    dbg_vmem_addr  <= {1'b0, bulk_addr};
                    dbg_vmem_data  <= vgc_fill_value;
                    dbg_vmem_we    <= 1;
                    bulk_addr      <= bulk_addr + 1;
                    bulk_remaining <= bulk_remaining - 1;
                    if (bulk_remaining == 1) begin
                        resp_idx   <= 0;
                        resp_total <= 1;
                        state      <= S_TX_BYTE;
                    end
                end

                // ---------------------------------------------------------
                // SDRAM_HOLD: keep weB asserted for SDRAM_HOLD pixel-clocks
                // (matches sid_curve_reader's CAS budget). At the end,
                // advance address, decrement count, loop or ack.
                // ---------------------------------------------------------
                S_SDRAM_HOLD: begin
                    if (sdram_hold_cnt != 0) begin
                        sdram_hold_cnt <= sdram_hold_cnt - 1;
                    end else begin
                        sdram_b_we      <= 0;
                        sdram_bulk_addr <= sdram_bulk_addr + 25'd1;
                        bulk_remaining  <= bulk_remaining - 1;
                        if (bulk_remaining == 1) begin
                            resp_idx   <= 0;
                            resp_total <= 1;
                            state      <= S_TX_BYTE;
                        end else begin
                            state <= S_SDRAM_RECV;
                        end
                    end
                end

                // ---------------------------------------------------------
                // EVENT_TX_MARK: TX the event marker byte (0xFE)
                // ---------------------------------------------------------
                S_EVENT_TX_MARK: begin
                    if (!tx_busy) begin
                        tx_data  <= EVENT_MARKER;
                        tx_start <= 1;
                        state    <= S_EVENT_TX_MARK_WAIT;
                    end
                end

                // ---------------------------------------------------------
                // EVENT_TX_MARK_WAIT: wait for marker TX, then send type.
                // ---------------------------------------------------------
                S_EVENT_TX_MARK_WAIT: begin
                    if (!tx_busy) state <= S_EVENT_TX_TYPE;
                end

                // ---------------------------------------------------------
                // EVENT_TX_TYPE: TX the event type byte (0xE0 for FIO).
                // Clearing event_pending here lets a new fio_event arrive
                // during emission and queue up another notification.
                // ---------------------------------------------------------
                S_EVENT_TX_TYPE: begin
                    if (!tx_busy) begin
                        tx_data       <= EVENT_TYPE_FIO;
                        tx_start      <= 1;
                        event_pending <= 0;
                        state         <= S_EVENT_TX_TYPE_WAIT;
                    end
                end

                // ---------------------------------------------------------
                // EVENT_TX_TYPE_WAIT: wait for type TX, back to IDLE.
                // ---------------------------------------------------------
                S_EVENT_TX_TYPE_WAIT: begin
                    if (!tx_busy) state <= S_IDLE;
                end

                default: state <= S_IDLE;

            endcase
        end
    end

endmodule
