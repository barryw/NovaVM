// DMA controller — 1D bulk copy / fill engine at $BA63-$BA75
// Mirrors Avalonia's VirtualDmaController. Separate from the blitter at
// $BA83 (which handles 2D rectangles). Operates across CPU RAM, XRAM,
// and VGC spaces (char/color/gfx/sprite). Stalls CPU via rdy_out.

module dma (
    input  logic        clk,
    input  logic        rst,

    // CPU bus (snooped for register writes, drives rdata on reads)
    input  logic [15:0] cpu_addr,
    input  logic [7:0]  cpu_wdata,
    input  logic        cpu_we,
    output logic [7:0]  cpu_rdata,
    input  logic        cpu_re,

    // CPU stall (active low — drives CPU RDY)
    output logic        rdy_out,

    // Memory port A: CPU RAM (64KB)
    output logic [15:0] ram_addr,
    input  logic [7:0]  ram_rdata,
    output logic [7:0]  ram_wdata,
    output logic        ram_we,

    // Memory port B: XRAM (512KB)
    // See the matching comment in blitter.sv: xram_busy back-pressures the
    // write port so DMA doesn't drop bytes when the SDRAM wrapper is mid-burst.
    // xram_re pulses when DMA reads XRAM — otherwise the wrapper never fires
    // a SDRAM read and DMA samples stale r_dout.
    input  logic        xram_busy,
    output logic        xram_re,
    output logic [18:0] xram_addr,
    input  logic [7:0]  xram_rdata,
    output logic [7:0]  xram_wdata,
    output logic        xram_we,

    // Memory port C: VGC internal (char/color/gfx/sprite)
    output logic [2:0]  vgc_space,
    output logic [16:0] vgc_addr,
    input  logic [7:0]  vgc_rdata,
    output logic [7:0]  vgc_wdata,
    output logic        vgc_we,
    output logic        vgc_re
);

    // =========================================================================
    // Constants
    // =========================================================================
    localparam DMA_BASE = 16'hBA63;
    localparam DMA_END  = 16'hBA75;

    localparam R_CMD       = 0;   localparam R_STATUS    = 1;
    localparam R_ERRCODE   = 2;   localparam R_SRCSPACE  = 3;
    localparam R_DSTSPACE  = 4;   localparam R_SRCL      = 5;
    localparam R_SRCM      = 6;   localparam R_SRCH      = 7;
    localparam R_DSTL      = 8;   localparam R_DSTM      = 9;
    localparam R_DSTH      = 10;  localparam R_LENL      = 11;
    localparam R_LENM      = 12;  localparam R_LENH      = 13;
    localparam R_MODE      = 14;  localparam R_FILLVAL   = 15;
    localparam R_COUNTL    = 16;  localparam R_COUNTM    = 17;
    localparam R_COUNTH    = 18;

    localparam ST_IDLE = 8'h00, ST_BUSY = 8'h01, ST_OK = 8'h02, ST_ERROR = 8'h03;
    localparam ERR_NONE = 8'h00, ERR_BADCMD = 8'h01, ERR_BADSPACE = 8'h02;
    localparam ERR_RANGE = 8'h03, ERR_BADARGS = 8'h04, ERR_WRITEPROT = 8'h05;

    localparam SPACE_CPU = 3'd0, SPACE_CHAR = 3'd1, SPACE_COLOR = 3'd2;
    localparam SPACE_GFX = 3'd3, SPACE_SPRITE = 3'd4, SPACE_XRAM = 3'd5;
    localparam SPACE_MAX = SPACE_XRAM;

    localparam [23:0] ROM_BASE = 24'h00C000;

    localparam MODE_FILL = 0;

    // =========================================================================
    // Registers + state
    // =========================================================================
    logic [7:0] regs [0:18];

    typedef enum logic [2:0] {
        S_IDLE,
        S_VALIDATE,
        S_READ,      // drive ram_addr = src_addr (dpram port B latches this)
        S_READ_WAIT, // hold ram_addr, let dpram output settle one cycle
        S_WRITE,
        S_DONE
    } state_t;

    state_t state;
    logic        fill_mode;
    logic [2:0]  src_space, dst_space;
    logic [23:0] src_base, dst_base;
    logic [23:0] length;
    logic [7:0]  fill_value;
    logic [23:0] idx;
    logic [23:0] moved;
    logic [7:0]  read_byte;
    logic        read_valid;

    initial begin
        for (int i = 0; i < 19; i++) regs[i] = 0;
        regs[R_STATUS] = ST_IDLE;
        state = S_IDLE;
        fill_mode = 0;
        src_space = 0; dst_space = 0;
        src_base = 0; dst_base = 0;
        length = 0;
        fill_value = 0;
        idx = 0; moved = 0;
        read_byte = 0; read_valid = 0;
    end

    // =========================================================================
    // Address decode + CPU read
    // =========================================================================
    wire dma_sel = (cpu_addr >= DMA_BASE && cpu_addr <= DMA_END);
    wire [4:0] reg_off = cpu_addr - DMA_BASE;

    always_comb begin
        cpu_rdata = 8'h00;
        if (dma_sel) cpu_rdata = regs[reg_off];
    end

    // =========================================================================
    // Address computation — single 24-bit add (src/dst base + idx)
    // =========================================================================
    wire [23:0] src_addr = src_base + idx;
    wire [23:0] dst_addr = dst_base + idx;

    // Space size lookup
    function automatic logic [19:0] space_size(input logic [2:0] sp);
        case (sp)
            SPACE_CPU:    space_size = 20'(65536);
            SPACE_CHAR:   space_size = 20'(4000);
            SPACE_COLOR:  space_size = 20'(4000);
            SPACE_GFX:    space_size = 20'(64000);
            SPACE_SPRITE: space_size = 20'(32768);
            SPACE_XRAM:   space_size = 20'(524288);
            default:      space_size = 0;
        endcase
    endfunction

    // Range check: does base + length fit in space?
    function automatic logic range_fits(
        input logic [23:0] base, input logic [23:0] len, input logic [19:0] sz
    );
        logic [31:0] last_byte;
        if (len == 0 || sz == 0) begin range_fits = 0; end
        else begin
            last_byte = {8'b0, base} + {8'b0, len};
            range_fits = (last_byte <= {12'b0, sz}) && ({8'b0, base} < {12'b0, sz});
        end
    endfunction

    // Write-protect check: CPU RAM destination must stay below ROM
    function automatic logic write_protect_ok(
        input logic [2:0] space, input logic [23:0] base, input logic [23:0] len
    );
        logic [31:0] last_byte;
        if (space != SPACE_CPU) begin write_protect_ok = 1; end
        else begin
            last_byte = {8'b0, base} + {8'b0, len};
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

        // Read address setup (for S_READ and S_READ_WAIT). dpram port B
        // has one cycle of read latency in synthesis: address registered
        // at posedge, data valid next cycle. Holding the address for two
        // states gives the dout a full cycle to settle before S_WRITE
        // samples it. Sim harnesses use combinational reads and don't
        // exercise this latency — a subtle source of sim/synth drift.
        if ((state == S_READ || state == S_READ_WAIT) && !fill_mode) begin
            case (src_space)
                SPACE_CPU:  ram_addr = src_addr[15:0];
                SPACE_XRAM: begin
                    xram_addr = src_addr[18:0];
                    if (state == S_READ) xram_re = 1;
                end
                default: begin
                    vgc_space = src_space;
                    vgc_addr = src_addr[16:0];
                    vgc_re = 1;
                end
            endcase
        end

        // Write data setup (for S_WRITE)
        if (state == S_WRITE && read_valid) begin
            case (dst_space)
                SPACE_CPU: begin
                    ram_addr = dst_addr[15:0];
                    ram_wdata = read_byte;
                    ram_we = 1;
                end
                SPACE_XRAM: begin
                    xram_addr = dst_addr[18:0];
                    xram_wdata = read_byte;
                    xram_we = 1;
                end
                default: begin
                    vgc_space = dst_space;
                    vgc_addr = dst_addr[16:0];
                    vgc_wdata = read_byte;
                    vgc_we = 1;
                end
            endcase
        end
    end

    // RDY: stall CPU when DMA active
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
                    if (length == 0) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADARGS;
                        state <= S_DONE;
                    end else if (!fill_mode && src_space > SPACE_MAX) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADSPACE;
                        state <= S_DONE;
                    end else if (dst_space > SPACE_MAX) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_BADSPACE;
                        state <= S_DONE;
                    end else if (!fill_mode &&
                                 !range_fits(src_base, length, space_size(src_space))) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_RANGE;
                        state <= S_DONE;
                    end else if (!range_fits(dst_base, length, space_size(dst_space))) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_RANGE;
                        state <= S_DONE;
                    end else if (!write_protect_ok(dst_space, dst_base, length)) begin
                        regs[R_STATUS] <= ST_ERROR;
                        regs[R_ERRCODE] <= ERR_WRITEPROT;
                        state <= S_DONE;
                    end else begin
                        if (fill_mode) begin
                            state <= S_WRITE;
                            read_valid <= 1;
                            read_byte <= fill_value;
                        end else begin
                            state <= S_READ;
                            read_valid <= 0;
                        end
                    end
                end

                S_READ: begin
                    // Issue cycle — address was driven combinationally this
                    // cycle; dpram captured it at the posedge. Wait one
                    // cycle for dout to become valid before sampling.
                    state <= S_READ_WAIT;
                end

                S_READ_WAIT: begin
                    // For XRAM reads the wrapper takes multi-cycle; stall
                    // while busy, then r_dout is valid when busy drops.
                    if (src_space == SPACE_XRAM && xram_busy) begin
                        // stall
                    end else begin
                        read_byte <= mem_read_data;
                        read_valid <= 1;
                        state <= S_WRITE;
                    end
                end

                S_WRITE: begin
                    if (dst_space == SPACE_XRAM && xram_busy) begin
                        // stall until XRAM wrapper accepts the write
                    end
                    else if (read_valid) begin
                        moved <= moved + 1;
                        if (idx + 1 >= length) begin
                            regs[R_STATUS] <= ST_OK;
                            regs[R_ERRCODE] <= ERR_NONE;
                            {regs[R_COUNTH], regs[R_COUNTM], regs[R_COUNTL]}
                                <= moved + 24'd1;
                            state <= S_DONE;
                        end else begin
                            idx <= idx + 1;
                            if (fill_mode) begin
                                state <= S_WRITE;
                                read_byte <= fill_value;
                            end else begin
                                state <= S_READ;
                            end
                        end
                    end
                end

                S_DONE: begin
                    state <= S_IDLE;
                end

                default: state <= S_IDLE;
            endcase

            // CPU register writes
            if (cpu_we && dma_sel) begin
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
                            length     <= {regs[R_LENH], regs[R_LENM], regs[R_LENL]};
                            fill_mode  <= regs[R_MODE][MODE_FILL];
                            fill_value <= regs[R_FILLVAL];
                            idx        <= 0;
                            moved      <= 0;
                            read_valid <= 0;
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
