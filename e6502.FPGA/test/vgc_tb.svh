// Shared VGC testbench scaffolding — include from inside a `module test_...`.
// Provides: clock/reset, DUT signal decls, VGC instance, helper tasks, pass/fail
// counters, and a summary reporter. Designed for Verilator --timing.

// ---------------------------------------------------------------------------
// VGC address map constants (mirror of vgc.sv localparams)
// ---------------------------------------------------------------------------
localparam logic [15:0] VGC_BASE        = 16'hA000;
localparam logic [15:0] REG_MODE_A      = 16'hA000;
localparam logic [15:0] REG_BGCOL_A     = 16'hA001;
localparam logic [15:0] REG_FGCOL_A     = 16'hA002;
localparam logic [15:0] REG_CURSORX_A   = 16'hA003;
localparam logic [15:0] REG_CURSORY_A   = 16'hA004;
localparam logic [15:0] REG_CHAROUT_A   = 16'hA00E;
localparam logic [15:0] REG_CHARIN_A    = 16'hA00F;
localparam logic [15:0] REG_CMD_A       = 16'hA010;
localparam logic [15:0] REG_PARAM0_A    = 16'hA011;
localparam logic [15:0] CHAR_RAM_BASE_A = 16'hAA00;
localparam logic [15:0] COLOR_RAM_BASE_A= 16'hB1D0;

localparam int COLS_TB = 80;
localparam int ROWS_TB = 60;

// ---------------------------------------------------------------------------
// Clock / reset
// ---------------------------------------------------------------------------
logic clk = 0;
always #20 clk = ~clk;  // ~25 MHz

logic rst;

// ---------------------------------------------------------------------------
// DUT signals
// ---------------------------------------------------------------------------
logic        cpu_ce;
logic [15:0] cpu_addr;
logic [7:0]  cpu_wdata;
wire  [7:0]  cpu_rdata;
logic        cpu_we;
logic        cpu_re;

logic        key_valid;
logic [7:0]  key_data;

wire  [15:0] tile_dma_addr;
wire         tile_dma_active;

// Blitter-style memory interface — exposed as driver signals so tests can
// exercise the VGC's blt_* read/write path directly without instantiating
// the full blitter module. Default to idle (all zero).
logic [2:0]  tb_blt_space  = 3'd0;
logic [15:0] tb_blt_addr   = 16'd0;
wire  [7:0]  tb_blt_rdata;
logic [7:0]  tb_blt_wdata  = 8'd0;
logic        tb_blt_we     = 1'b0;
logic        tb_blt_re     = 1'b0;

logic [15:0] dbg_addr;
wire  [7:0]  dbg_rdata;

wire  [3:0]  vid_r, vid_g, vid_b;
wire         vid_hsync, vid_vsync, vid_de;
wire         irq_out;

vgc dut (
    .clk(clk), .rst(rst),
    .cpu_ce(cpu_ce),
    .cpu_addr(cpu_addr), .cpu_wdata(cpu_wdata),
    .cpu_rdata(cpu_rdata), .cpu_we(cpu_we), .cpu_re(cpu_re),
    .key_valid(key_valid), .key_data(key_data),
    .tile_dma_addr(tile_dma_addr),
    .tile_dma_data(8'h00),
    .tile_dma_active(tile_dma_active),
    .blt_space(tb_blt_space), .blt_addr(tb_blt_addr), .blt_rdata(tb_blt_rdata),
    .blt_wdata(tb_blt_wdata), .blt_we(tb_blt_we), .blt_re(tb_blt_re),
    .dbg_addr(dbg_addr), .dbg_rdata(dbg_rdata),
    .vid_r(vid_r), .vid_g(vid_g), .vid_b(vid_b),
    .vid_hsync(vid_hsync), .vid_vsync(vid_vsync), .vid_de(vid_de),
    .irq_out(irq_out)
);

// ---------------------------------------------------------------------------
// Test accounting
// ---------------------------------------------------------------------------
int pass_count = 0;
int fail_count = 0;
int test_num   = 0;

task automatic check(input string name, input logic condition);
    test_num++;
    if (condition) begin
        $display("  PASS [%0d] %s", test_num, name);
        pass_count++;
    end else begin
        $display("  FAIL [%0d] %s", test_num, name);
        fail_count++;
    end
endtask

task automatic check_eq(input string name, input int actual, input int expected);
    test_num++;
    if (actual == expected) begin
        $display("  PASS [%0d] %s (=%0d)", test_num, name, actual);
        pass_count++;
    end else begin
        $display("  FAIL [%0d] %s (got %0d, want %0d)", test_num, name, actual, expected);
        fail_count++;
    end
endtask

task automatic summary();
    $display("");
    $display("=== Results: %0d passed, %0d failed ===", pass_count, fail_count);
    if (fail_count == 0) $display("ALL TESTS PASSED");
    else                 $display("SOME TESTS FAILED");
endtask

// ---------------------------------------------------------------------------
// Reset / step helpers
// ---------------------------------------------------------------------------
task automatic do_reset();
    rst        = 1;
    cpu_ce     = 0;
    cpu_we     = 0;
    cpu_re     = 0;
    cpu_addr   = 16'h0000;
    cpu_wdata  = 8'h00;
    key_valid  = 0;
    key_data   = 8'h00;
    dbg_addr   = 16'h0000;
    repeat(50) @(posedge clk);
    rst = 0;
    repeat(10) @(posedge clk);
endtask

task automatic step(input int n);
    repeat(n) @(posedge clk);
endtask

// ---------------------------------------------------------------------------
// Bus helpers — single-cycle ce pulses, matches how top.sv drives the VGC.
// VGC write-side has a 1-cycle register-slice (write_active stage) added in
// 2026-04-27 to break the BRAM→CPU→cmd_*_addr critical path. So writes need
// one extra @(posedge clk) before they land in the cmd_* capture FFs, plus
// another for the dpram write to complete. Reads are unchanged.
// ---------------------------------------------------------------------------
task automatic bus_write(input logic [15:0] addr, input logic [7:0] data);
    @(posedge clk);
    cpu_addr  <= addr;
    cpu_wdata <= data;
    cpu_we    <= 1;
    cpu_re    <= 0;
    cpu_ce    <= 1;
    @(posedge clk);
    cpu_we    <= 0;
    cpu_ce    <= 0;
    // Allow VGC's write_active stage to fire and the downstream cmd_*_we
    // pulse to land in the dpram. Without these the test would peek char_mem
    // before the write completes.
    @(posedge clk);
    @(posedge clk);
endtask

// bus_read asserts cpu_re for one cycle, waits the dpram latency and the
// VGC's 1-cycle cpu_rd_latch, then returns cpu_rdata.
task automatic bus_read(input logic [15:0] addr, output logic [7:0] data);
    @(posedge clk);
    cpu_addr <= addr;
    cpu_we   <= 0;
    cpu_re   <= 1;
    cpu_ce   <= 1;
    @(posedge clk);
    cpu_re   <= 0;
    cpu_ce   <= 0;
    @(posedge clk);
    @(posedge clk);
    data = cpu_rdata;
endtask

// Direct dpram peek via Verilator's hierarchical access — bypasses port
// contention entirely. This is what we use for assertions about char RAM.
function automatic logic [7:0] peek_char(input int addr);
    peek_char = dut.text_inst.char_mem.mem[addr];
endfunction

function automatic logic [7:0] peek_color(input int addr);
    peek_color = dut.text_inst.color_mem.mem[addr];
endfunction

function automatic logic [7:0] peek_char_cell(input int col, input int row);
    peek_char_cell = dut.text_inst.char_mem.mem[row * COLS_TB + col];
endfunction

// Graphics pixel peek — 320x200, 4-bit color per pixel
function automatic logic [3:0] peek_gfx(input int x, input int y);
    peek_gfx = dut.gfx_inst.gfx_mem.mem[y * 320 + x];
endfunction

// Count set (non-zero) gfx pixels in a rectangle
function automatic int count_gfx_pixels(input int x0, input int y0,
                                         input int x1, input int y1);
    int n = 0;
    for (int yy = y0; yy <= y1; yy++)
        for (int xx = x0; xx <= x1; xx++)
            if (peek_gfx(xx, yy) != 4'd0) n++;
    return n;
endfunction

// Sprite memory peek — spr_mem is 2048 bytes, addressed as
// {sprite_idx[3:0], row[3:0], col_pair[2:0]}. Each byte packs two 4-bit
// pixels: hi nibble = even column, lo nibble = odd column.
function automatic logic [7:0] peek_spr_byte(input int spr_idx,
                                              input int row,
                                              input int col_pair);
    peek_spr_byte = dut.sprite_inst.spr_mem.mem[
        (spr_idx * 16 + row) * 8 + col_pair];
endfunction

// Read one sprite pixel (0..15 x, 0..15 y) as a 4-bit color.
function automatic logic [3:0] peek_spr_pixel(input int spr_idx,
                                               input int x,
                                               input int y);
    logic [7:0] b;
    b = peek_spr_byte(spr_idx, y, x / 2);
    peek_spr_pixel = (x[0] == 1'b0) ? b[7:4] : b[3:0];
endfunction

// ---------------------------------------------------------------------------
// VGC command / parameter helpers
// ---------------------------------------------------------------------------
task automatic write_param(input int idx, input logic [7:0] data);
    bus_write(REG_PARAM0_A + 16'(idx), data);
endtask

task automatic write_cmd(input logic [7:0] cmd);
    bus_write(REG_CMD_A, cmd);
endtask

task automatic wait_cmd_done();
    int timeout;
    timeout = 0;
    repeat(2) @(posedge clk);
    while ((dut.cmd_busy || dut.artist_inst.busy) && timeout < 500000) begin
        @(posedge clk);
        timeout++;
    end
    // After busy deasserts, give the last pending gfx write two more clocks
    // to propagate through port A → dpram. Without this the assertion can
    // fire before the final pixel commits to memory, masking a real success
    // with a false failure.
    repeat(4) @(posedge clk);
endtask

// ---------------------------------------------------------------------------
// Screen-editor style char-out — write a byte to $A00E, let VGC settle.
// ---------------------------------------------------------------------------
task automatic type_char(input logic [7:0] ch);
    bus_write(REG_CHAROUT_A, ch);
    // Allow 2 cycles for cmd_char_we to fire and the dpram write to commit.
    step(3);
endtask

task automatic type_string(input string s);
    int i;
    for (i = 0; i < s.len(); i++)
        type_char(s[i]);
endtask

// ---------------------------------------------------------------------------
// Screen dump — handy for visual debugging of text-layer bugs
// ---------------------------------------------------------------------------
task automatic dump_screen(input string label);
    int r, c;
    logic [7:0] b;
    $display("--- screen dump: %s (scroll_offset=%0d cursor=%0d,%0d) ---",
             label, dut.scroll_offset, dut.cursor_x, dut.cursor_y);
    for (r = 0; r < ROWS_TB; r++) begin
        string row_str;
        row_str = "";
        for (c = 0; c < COLS_TB; c++) begin
            b = dut.text_inst.char_mem.mem[r * COLS_TB + c];
            if (b >= 8'h20 && b < 8'h7F)
                row_str = {row_str, string'(b)};
            else
                row_str = {row_str, "."};
        end
        $display("  %2d: %s", r, row_str);
    end
endtask
