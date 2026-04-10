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
    output logic [18:0] xram_addr,
    input  logic [7:0]  xram_rdata,
    output logic [7:0]  xram_wdata,
    output logic        xram_we,

    // Memory port C: VGC internal (char/color/gfx/sprite/tile)
    output logic [2:0]  vgc_space,
    output logic [15:0] vgc_addr,
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
        S_READ,
        S_WRITE,
        S_ROWBUF_READ,    // buffered mode: read row into buffer
        S_ROWBUF_WRITE,   // buffered mode: write row from buffer
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
    // =========================================================================
    wire [23:0] src_addr = src_base + {8'b0, row} * {8'b0, src_stride} + {8'b0, col};
    wire [23:0] dst_addr = dst_base + {8'b0, row} * {8'b0, dst_stride} + {8'b0, col};

    // Space size lookup
    function automatic logic [19:0] space_size(input logic [2:0] sp);
        case (sp)
            SPACE_CPU:    return 20'(65536);
            SPACE_CHAR:   return 20'(2000);
            SPACE_COLOR:  return 20'(2000);
            SPACE_GFX:    return 20'(64000);
            SPACE_SPRITE: return 20'(32768);
            SPACE_XRAM:   return 20'(524288);
            SPACE_TILE:   return 20'(32768);
            default:      return 0;
        endcase
    endfunction

    // Range check: does base + (height-1)*stride + width fit in space?
    function automatic logic rect_fits(
        input logic [23:0] base, input logic [15:0] w, input logic [15:0] h,
        input logic [15:0] stride, input logic [19:0] sz
    );
        logic [31:0] last_row_start, last_byte;
        if (w == 0 || h == 0 || sz == 0) return 0;
        last_row_start = {8'b0, base} + 32'(h - 1) * 32'(stride);
        last_byte = last_row_start + 32'(w);
        return (last_byte <= {12'b0, sz}) && ({8'b0, base} < {12'b0, sz});
    endfunction

    // Write-protect check: CPU RAM writes must stay below ROM
    function automatic logic write_protect_ok(
        input logic [2:0] space, input logic [23:0] base,
        input logic [15:0] w, input logic [15:0] h, input logic [15:0] stride
    );
        logic [31:0] last_byte;
        if (space != SPACE_CPU) return 1;
        last_byte = {8'b0, base} + 32'(h - 1) * 32'(stride) + 32'(w);
        return (last_byte <= {8'b0, ROM_BASE});
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
        xram_addr = 0; xram_we = 0; xram_wdata = 0;
        vgc_space = 0; vgc_addr = 0; vgc_we = 0; vgc_wdata = 0; vgc_re = 0;

        // Read address setup (for S_READ and S_ROWBUF_READ)
        if (state == S_READ || state == S_ROWBUF_READ) begin
            if (!fill_mode) begin
                case (src_space)
                    SPACE_CPU:  ram_addr = src_addr[15:0];
                    SPACE_XRAM: xram_addr = src_addr[18:0];
                    default: begin
                        vgc_space = src_space;
                        vgc_addr = src_addr[15:0];
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
                    vgc_addr = dst_addr[15:0];
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
                    read_byte <= mem_read_data;
                    read_valid <= 1;
                    state <= S_WRITE;
                end

                S_WRITE: begin
                    if (read_valid) begin
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
                S_ROWBUF_READ: begin
                    // Read one byte per clock into row buffer
                    row_buf[buf_idx] <= mem_read_data;
                    if (col + 1 >= width) begin
                        // Row fully buffered, switch to writing
                        col <= 0;
                        buf_idx <= 0;
                        state <= S_ROWBUF_WRITE;
                    end else begin
                        col <= col + 1;
                        buf_idx <= buf_idx + 1;
                    end
                end

                S_ROWBUF_WRITE: begin
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
                            state <= S_ROWBUF_READ;
                        end
                    end else begin
                        col <= col + 1;
                        buf_idx <= buf_idx + 1;
                    end
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
