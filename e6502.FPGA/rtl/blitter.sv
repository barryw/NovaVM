// Blitter — 2D rectangular DMA engine
// Registers at $BA83-$BA9B. Copy, fill, and color-key transparency modes.
// Operates across 7 memory spaces (CPU RAM, char, color, gfx, sprite, XRAM, tile).
// Stalls CPU via rdy_out during operation for maximum throughput.
// Row buffer for safe same-space overlapping copies. Range and write-protect validation.

module blitter (
    input  logic        clk,
    input  logic        rst,

    // CPU bus (snooped for register writes)
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    output logic [7:0]  cpu_rdata,
    input  logic        cpu_re,

    // CPU stall (active low — directly drives CPU RDY)
    output logic        rdy_out,

    // Memory port A: CPU RAM (64KB)
    output logic [15:0] ram_addr,
    input  logic [7:0]  ram_rdata,
    output logic [7:0]  ram_wdata,
    output logic        ram_we,

    // Memory port B: XRAM (512KB)
    // xram_busy signals that the SDRAM-backed XRAM wrapper cannot accept a
    // write this cycle. The blitter must hold xram_we + xram_addr + xram_wdata
    // stable until xram_busy drops; otherwise the byte is silently dropped
    // (top.sv gates `bm_xram_fire = bm_xram_we && !xram_busy`). Manifests as
    // STASH/FETCH round-trip dropping bytes at any length > ~1 byte.
    //
    // xram_re fires a 1-cycle pulse on S_READ when src=XRAM. The wrapper
    // latches in S_IDLE, transitions to S_ACTIVE (busy=1) for HOLD_CYCLES,
    // then S_CAPTURE latches r_dout, then back to S_IDLE (busy=0). The
    // blitter stalls in S_READ_WAIT while xram_busy is high — data on
    // xram_rdata becomes valid the cycle after busy drops.
    output logic [18:0] xram_addr,
    input  logic [7:0]  xram_rdata,
    output logic [7:0]  xram_wdata,
    output logic        xram_we,
    output logic        xram_re,
    input  logic        xram_busy,

    // Memory port C: VGC internal (char/color/gfx/sprite/tile)
    output logic [2:0]  vgc_space,
    output logic [16:0] vgc_addr,     // 17-bit to reach gfx_mem 76800
    input  logic [7:0]  vgc_rdata,
    output logic [7:0]  vgc_wdata,
    output logic        vgc_we,
    output logic        vgc_re
);

    // =========================================================================
    // Constants
    // =========================================================================
    localparam BLT_BASE = 16'hBA83;
    localparam BLT_END  = 16'hBA9B;

    localparam R_CMD       = 0;   localparam R_STATUS    = 1;
    localparam R_ERRCODE   = 2;   localparam R_SRCSPACE  = 3;
    localparam R_DSTSPACE  = 4;   localparam R_SRCL      = 5;
    localparam R_SRCM      = 6;   localparam R_SRCH      = 7;
    localparam R_DSTL      = 8;   localparam R_DSTM      = 9;
    localparam R_DSTH      = 10;  localparam R_WIDTHL    = 11;
    localparam R_WIDTHH    = 12;  localparam R_HEIGHTL   = 13;
    localparam R_HEIGHTH   = 14;  localparam R_SRCSTRIDEL = 15;
    localparam R_SRCSTRIDEH = 16; localparam R_DSTSTRIDEL = 17;
    localparam R_DSTSTRIDEH = 18; localparam R_MODE      = 19;
    localparam R_FILLVAL   = 20;  localparam R_COLORKEY  = 21;
    localparam R_COUNTL    = 22;  localparam R_COUNTM    = 23;
    localparam R_COUNTH    = 24;

    localparam ST_IDLE = 8'h00, ST_BUSY = 8'h01, ST_OK = 8'h02, ST_ERROR = 8'h03;
    localparam ERR_NONE = 8'h00, ERR_BADCMD = 8'h01, ERR_BADSPACE = 8'h02;
    localparam ERR_RANGE = 8'h03, ERR_BADARGS = 8'h04, ERR_WRITEPROT = 8'h05;

    localparam SPACE_CPU = 3'd0, SPACE_CHAR = 3'd1, SPACE_COLOR = 3'd2;
    localparam SPACE_GFX = 3'd3, SPACE_SPRITE = 3'd4, SPACE_XRAM = 3'd5, SPACE_TILE = 3'd6;

    localparam ROM_BASE = 24'hC000;

    localparam MODE_FILL = 0, MODE_COLORKEY = 1;

    // Row buffer: 1024 bytes max width for same-space overlap protection
    localparam ROW_BUF_SIZE = 1024;

    // =========================================================================
    // Registers + state
    // =========================================================================
    logic [7:0] regs [0:24];

    typedef enum logic [3:0] {
        S_IDLE,
        S_VALIDATE,
        S_READ,             // issue read (dpram latches addr at posedge)
        S_READ_WAIT,        // dpram dout settles; capture read_byte
        S_WRITE,
        S_ROWBUF_READ,      // buffered mode: issue read into buffer
        S_ROWBUF_READ_WAIT, // dpram dout settles; capture row_buf[buf_idx]
        S_ROWBUF_WRITE,     // buffered mode: write row from buffer
        S_DONE
    } state_t;

    state_t state;
    logic        fill_mode, colorkey_mode, use_row_buffer;
    logic [2:0]  src_space, dst_space;
    logic [23:0] src_base, dst_base;
    logic [15:0] width, height;
    logic [15:0] src_stride, dst_stride;
    logic [7:0]  fill_value, color_key;
    logic [15:0] row, col;
    logic [23:0] wrote_count;
    logic [7:0]  read_byte;
    logic        read_valid;

    // Row buffer for same-space copies
    logic [7:0]  row_buf [0:ROW_BUF_SIZE-1];
    logic [9:0]  buf_idx;

    initial begin
        for (int i = 0; i < 25; i++) regs[i] = 0;
        regs[R_STATUS] = ST_IDLE;
        state = S_IDLE;
        fill_mode = 0; colorkey_mode = 0; use_row_buffer = 0;
        src_space = 0; dst_space = 0;
        src_base = 0; dst_base = 0;
        width = 0; height = 0;
        src_stride = 0; dst_stride = 0;
        fill_value = 0; color_key = 0;
        row = 0; col = 0; wrote_count = 0;
        read_byte = 0; read_valid = 0; buf_idx = 0;
        src_row_off = 0; dst_row_off = 0;
    end

    // =========================================================================
    // Address decode + CPU read
    // =========================================================================
    wire blt_sel = (cpu_addr >= BLT_BASE && cpu_addr <= BLT_END);
    wire [4:0] reg_off = cpu_addr - BLT_BASE;

    always_comb begin
        cpu_rdata = 8'h00;
        if (blt_sel) cpu_rdata = regs[reg_off];
    end

    // =========================================================================
    // Address computation
    //
    // A straight combinational `base + row*stride + col` synthesized into a
    // MULT18X18D plus a long CCU2 carry chain in the pixel-clock domain —
    // its worst path missed 25 MHz timing by ~0.7 ns. Replacing the runtime
    // multiply with an accumulator (`row_off += stride` on each row step)
    // drops the path to a pure 24-bit add and closes timing.
    //
    // src_row_off / dst_row_off are zeroed at blit start (same instant as
    // `row <= 0`) and incremented by stride whenever `row` advances.
    // =========================================================================
    logic [23:0] src_row_off, dst_row_off;
    wire  [23:0] src_addr = src_base + src_row_off + {8'b0, col};
    wire  [23:0] dst_addr = dst_base + dst_row_off + {8'b0, col};

    // Space size lookup
    function automatic logic [19:0] space_size(input logic [2:0] sp);
        case (sp)
            SPACE_CPU:    space_size = 20'(65536);
            SPACE_CHAR:   space_size = 20'(4800);
            SPACE_COLOR:  space_size = 20'(4800);
            SPACE_GFX:    space_size = 20'(76800);
            SPACE_SPRITE: space_size = 20'(32768);
            SPACE_XRAM:   space_size = 20'(524288);
            SPACE_TILE:   space_size = 20'(32768);
            default:      space_size = 0;
        endcase
    endfunction

    // Range check: does base + (height-1)*stride + width fit in space?
    function automatic logic rect_fits(
        input logic [23:0] base, input logic [15:0] w, input logic [15:0] h,
        input logic [15:0] stride, input logic [19:0] sz
    );
        logic [31:0] last_row_start, last_byte;
        if (w == 0 || h == 0 || sz == 0) begin rect_fits = 0; end
        else begin
            last_row_start = {8'b0, base} + 32'(h - 1) * 32'(stride);
            last_byte = last_row_start + 32'(w);
            rect_fits = (last_byte <= {12'b0, sz}) && ({8'b0, base} < {12'b0, sz});
        end
    endfunction

    // Write-protect check: CPU RAM writes must stay below ROM
    function automatic logic write_protect_ok(
        input logic [2:0] space, input logic [23:0] base,
        input logic [15:0] w, input logic [15:0] h, input logic [15:0] stride
    );
        logic [31:0] last_byte;
        if (space != SPACE_CPU) begin write_protect_ok = 1; end
        else begin
            last_byte = {8'b0, base} + 32'(h - 1) * 32'(stride) + 32'(w);
            write_protect_ok = (last_byte <= {8'b0, ROM_BASE});
        end
    endfunction

    // =========================================================================
    // Memory read data mux
    // =========================================================================
    wire [7:0] mem_read_data =
        (src_space == SPACE_CPU)  ? ram_rdata :
        (src_space == SPACE_XRAM) ? xram_rdata :
                                    vgc_rdata;

    // =========================================================================
    // Memory port outputs (combinational)
    // =========================================================================
    always_comb begin
        ram_addr = 0; ram_we = 0; ram_wdata = 0;
        xram_addr = 0; xram_we = 0; xram_wdata = 0; xram_re = 0;
        vgc_space = 0; vgc_addr = 0; vgc_we = 0; vgc_wdata = 0; vgc_re = 0;

        // Read address setup — hold addr stable across S_READ → S_READ_WAIT
        // and S_ROWBUF_READ → S_ROWBUF_READ_WAIT so dpram port B has time
        // to latch + produce valid dout. One-cycle dpram latency would
        // otherwise capture stale data on the S_*→S_*_WAIT transition.
        if (state == S_READ        || state == S_READ_WAIT ||
            state == S_ROWBUF_READ || state == S_ROWBUF_READ_WAIT) begin
            if (!fill_mode) begin
                case (src_space)
                    SPACE_CPU:  ram_addr = src_addr[15:0];
                    SPACE_XRAM: begin
                        xram_addr = src_addr[18:0];
                        // Pulse read enable only on the first S_READ cycle
                        // (not S_READ_WAIT). The wrapper latches the addr
                        // when in S_IDLE and ignores subsequent asserts
                        // during its internal S_ACTIVE / S_CAPTURE, so the
                        // 1-cycle pulse is sufficient and avoids re-firing.
                        if (state == S_READ || state == S_ROWBUF_READ)
                            xram_re = 1;
                    end
                    default: begin
                        vgc_space = src_space;
                        vgc_addr = src_addr[16:0];
                        vgc_re = 1;
                    end
                endcase
            end
        end

        // Write data setup (for S_WRITE and S_ROWBUF_WRITE)
        if ((state == S_WRITE && read_valid && !(colorkey_mode && read_byte == color_key)) ||
            (state == S_ROWBUF_WRITE && !(colorkey_mode && row_buf[buf_idx] == color_key))) begin
            case (dst_space)
                SPACE_CPU: begin
                    ram_addr = dst_addr[15:0];
                    ram_wdata = (state == S_ROWBUF_WRITE) ? row_buf[buf_idx] : read_byte;
                    ram_we = 1;
                end
                SPACE_XRAM: begin
                    xram_addr = dst_addr[18:0];
                    xram_wdata = (state == S_ROWBUF_WRITE) ? row_buf[buf_idx] : read_byte;
                    xram_we = 1;
                end
                default: begin
                    vgc_space = dst_space;
                    vgc_addr = dst_addr[16:0];
                    vgc_wdata = (state == S_ROWBUF_WRITE) ? row_buf[buf_idx] : read_byte;
                    vgc_we = 1;
                end
            endcase
        end
    end

    // RDY: stall CPU when blitter active
    assign rdy_out = (state == S_IDLE || state == S_DONE);

    // =========================================================================
    // Main state machine
    // =========================================================================
    always_ff @(posedge clk) begin
        if (rst) begin
            state <= S_IDLE;
            regs[R_STATUS] <= ST_IDLE;
            regs[R_ERRCODE] <= ERR_NONE;
            read_valid <= 0;
        end else begin

            case (state)
                S_IDLE: ;

                S_VALIDATE: begin
                    if (width == 0 || height == 0) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADARGS;
                        state <= S_DONE;
                    end else if (!fill_mode && src_space > SPACE_TILE) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADSPACE;
                        state <= S_DONE;
                    end else if (dst_space > SPACE_TILE) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADSPACE;
                        state <= S_DONE;
                    end else if (!fill_mode &&
                                 !rect_fits(src_base, width, height, src_stride, space_size(src_space))) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_RANGE;
                        state <= S_DONE;
                    end else if (!rect_fits(dst_base, width, height, dst_stride, space_size(dst_space))) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_RANGE;
                        state <= S_DONE;
                    end else if (!write_protect_ok(dst_space, dst_base, width, height, dst_stride)) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_WRITEPROT;
                        state <= S_DONE;
                    end else begin
                        if (fill_mode) begin
                            state <= S_WRITE;
                            read_valid <= 1;
                            read_byte <= fill_value;
                        end else if (use_row_buffer) begin
                            state <= S_ROWBUF_READ;
                            col <= 0;
                            buf_idx <= 0;
                        end else begin
                            state <= S_READ;
                            read_valid <= 0;
                        end
                    end
                end

                // --- Direct mode (different spaces) ---
                S_READ: begin
                    // Issue cycle — ram_addr was driven combinationally this
                    // cycle; dpram latched it at the posedge. Next cycle dout
                    // will be valid.
                    state <= S_READ_WAIT;
                end

                S_READ_WAIT: begin
                    // XRAM reads take ~HOLD_CYCLES+2 cycles through xram_sdram.
                    // busy=1 while the wrapper is processing the latched
                    // req_re. When busy drops, r_dout is valid; latch it.
                    if (src_space == SPACE_XRAM && xram_busy) begin
                        // stall — r_dout not yet valid
                    end else begin
                        read_byte <= mem_read_data;
                        read_valid <= 1;
                        state <= S_WRITE;
                    end
                end

                S_WRITE: begin
                    // Back-pressure: if destination is XRAM and the wrapper
                    // is busy, keep xram_we / xram_addr / xram_wdata asserted
                    // (combinationally driven from this state) until the
                    // wrapper can accept the byte. Hold state until ready.
                    if (dst_space == SPACE_XRAM && xram_busy) begin
                        // stall — re-enter S_WRITE implicitly by not advancing
                    end
                    else if (read_valid) begin
                        if (!(colorkey_mode && read_byte == color_key))
                            wrote_count <= wrote_count + 1;

                        if (col + 1 >= width) begin
                            col <= 0;
                            if (row + 1 >= height) begin
                                regs[R_STATUS] <= ST_OK;
                                regs[R_ERRCODE] <= ERR_NONE;
                                {regs[R_COUNTH], regs[R_COUNTM], regs[R_COUNTL]}
                                    <= wrote_count + (!(colorkey_mode && read_byte == color_key) ? 24'd1 : 24'd0);
                                state <= S_DONE;
                            end else begin
                                row <= row + 1;
                                src_row_off <= src_row_off + {8'b0, src_stride};
                                dst_row_off <= dst_row_off + {8'b0, dst_stride};
                                if (fill_mode) begin
                                    state <= S_WRITE;
                                    read_byte <= fill_value;
                                end else
                                    state <= S_READ;
                            end
                        end else begin
                            col <= col + 1;
                            if (fill_mode) begin
                                state <= S_WRITE;
                                read_byte <= fill_value;
                            end else
                                state <= S_READ;
                        end
                    end
                end

                // --- Buffered mode (same-space copies) ---
                // Issue → wait → capture pattern, 2 cycles per byte. Old
                // 1-cycle-per-byte loop captured mem_read_data one cycle
                // early and missed the final byte on dpram-backed memories.
                S_ROWBUF_READ: begin
                    state <= S_ROWBUF_READ_WAIT;
                end

                S_ROWBUF_READ_WAIT: begin
                    // Same multi-cycle read handshake as S_READ_WAIT.
                    // Same-space XRAM→XRAM copies buffer through here.
                    if (src_space == SPACE_XRAM && xram_busy) begin
                        // stall — r_dout not yet valid
                    end else begin
                        row_buf[buf_idx] <= mem_read_data;
                        if (col + 1 >= width) begin
                            col <= 0;
                            buf_idx <= 0;
                            state <= S_ROWBUF_WRITE;
                        end else begin
                            col <= col + 1;
                            buf_idx <= buf_idx + 1;
                            state <= S_ROWBUF_READ;
                        end
                    end
                end

                S_ROWBUF_WRITE: begin
                    // Same back-pressure gating as S_WRITE. Same-space XRAM
                    // copies (src_space == dst_space == XRAM) use this path
                    // via the row-buffer.
                    if (dst_space == SPACE_XRAM && xram_busy) begin
                        // stall — hold the row-buffer write steady until ready
                    end else begin
                    // Write one byte per clock from row buffer
                    if (!(colorkey_mode && row_buf[buf_idx] == color_key))
                        wrote_count <= wrote_count + 1;

                    if (col + 1 >= width) begin
                        col <= 0;
                        buf_idx <= 0;
                        if (row + 1 >= height) begin
                            regs[R_STATUS] <= ST_OK;
                            regs[R_ERRCODE] <= ERR_NONE;
                            {regs[R_COUNTH], regs[R_COUNTM], regs[R_COUNTL]}
                                <= wrote_count + (!(colorkey_mode && row_buf[buf_idx] == color_key) ? 24'd1 : 24'd0);
                            state <= S_DONE;
                        end else begin
                            row <= row + 1;
                            src_row_off <= src_row_off + {8'b0, src_stride};
                            dst_row_off <= dst_row_off + {8'b0, dst_stride};
                            state <= S_ROWBUF_READ;
                        end
                    end else begin
                        col <= col + 1;
                        buf_idx <= buf_idx + 1;
                    end
                    end  // end of (!xram_busy) block
                end

                S_DONE: begin
                    state <= S_IDLE;
                end

                default: state <= S_IDLE;
            endcase

            // CPU register writes
            if (cpu_we && blt_sel) begin
                if (reg_off != R_STATUS && reg_off != R_ERRCODE &&
                    reg_off != R_COUNTL && reg_off != R_COUNTM && reg_off != R_COUNTH)
                    regs[reg_off] <= cpu_wdata;

                if (reg_off == R_CMD) begin
                    if (cpu_wdata == 8'h01) begin
                        if (state == S_IDLE) begin
                            src_space  <= regs[R_SRCSPACE][2:0];
                            dst_space  <= regs[R_DSTSPACE][2:0];
                            src_base   <= {regs[R_SRCH], regs[R_SRCM], regs[R_SRCL]};
                            dst_base   <= {regs[R_DSTH], regs[R_DSTM], regs[R_DSTL]};
                            width      <= {regs[R_WIDTHH], regs[R_WIDTHL]};
                            height     <= {regs[R_HEIGHTH], regs[R_HEIGHTL]};
                            src_stride <= {regs[R_SRCSTRIDEH], regs[R_SRCSTRIDEL]};
                            dst_stride <= {regs[R_DSTSTRIDEH], regs[R_DSTSTRIDEL]};
                            fill_mode     <= regs[R_MODE][MODE_FILL];
                            colorkey_mode <= regs[R_MODE][MODE_COLORKEY];
                            fill_value    <= regs[R_FILLVAL];
                            color_key     <= regs[R_COLORKEY];
                            use_row_buffer <= !regs[R_MODE][MODE_FILL] &&
                                              regs[R_SRCSPACE][2:0] == regs[R_DSTSPACE][2:0];
                            row <= 0; col <= 0;
                            // Reset row-offset accumulators so address math
                            // starts at base. They advance by stride each row.
                            src_row_off <= 0;
                            dst_row_off <= 0;
                            wrote_count <= 0;
                            read_valid <= 0;
                            buf_idx <= 0;
                            regs[R_STATUS]  <= ST_BUSY;
                            regs[R_ERRCODE] <= ERR_NONE;
                            regs[R_COUNTL]  <= 0;
                            regs[R_COUNTM]  <= 0;
                            regs[R_COUNTH]  <= 0;
                            state <= S_VALIDATE;
                        end else begin
                            regs[R_STATUS] <= ST_ERROR;
                            regs[R_ERRCODE] <= ERR_BADCMD;
                        end
                    end else begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADCMD;
                    end
                end
            end
        end
    end

endmodule
