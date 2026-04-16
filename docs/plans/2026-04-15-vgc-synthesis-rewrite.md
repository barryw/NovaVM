# VGC FPGA Synthesis Rewrite

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Restructure VGC so it synthesizes with Yosys for ECP5 FPGA — all large memories through dpram instances, pipelined rendering, serialized multi-writes.

**Architecture:** Replace all inline `logic [...] name [0:N]` arrays with `dpram` module instances (port A = write from CPU/commands/blitter, port B = read from renderer). Rendering pipeline adds 2-cycle prefetch latency for BRAM reads. Multi-pixel writes (circle ×8, SPRROW ×8) serialized to 1/cycle. Sprite rendering uses scanline buffer filled during hblank. All changes must pass existing Verilator tests.

**Tech Stack:** SystemVerilog, Yosys (ECP5 synth_ecp5), Verilator (simulation), dpram.sv (dual-port BRAM wrapper)

---

## Phase 1: Text Mode (EhBASIC boots, text visible on HDMI)

### Task 1: Verify dpram works in Verilator

**Files:**
- Test: `e6502.FPGA/test/test_dpram.sv` (create)

**Step 1: Write a Verilator testbench for dpram**

```systemverilog
// test_dpram.sv — verify dpram works in both Verilator and Yosys
module test_dpram;
    reg clk = 0;
    always #5 clk = ~clk;
    
    reg [10:0] addr_a, addr_b;
    reg [7:0] din_a;
    reg we_a;
    wire [7:0] dout_a, dout_b;
    
    dpram #(.WIDTH(8), .DEPTH(2048)) uut (
        .clk(clk),
        .addr_a(addr_a), .din_a(din_a), .we_a(we_a), .dout_a(dout_a),
        .addr_b(addr_b), .dout_b(dout_b)
    );
    
    integer pass = 0, fail = 0;
    task check(input [7:0] got, input [7:0] exp, input [255:0] msg);
        if (got === exp) pass++; else begin $display("FAIL: %0s got=%h exp=%h", msg, got, exp); fail++; end
    endtask
    
    initial begin
        we_a = 0; addr_a = 0; addr_b = 0; din_a = 0;
        @(posedge clk); @(posedge clk);
        
        // Write via port A
        addr_a = 11'd42; din_a = 8'hAB; we_a = 1;
        @(posedge clk); we_a = 0;
        
        // Read back via port A (1-cycle latency)
        addr_a = 11'd42;
        @(posedge clk);
        @(posedge clk);
        check(dout_a, 8'hAB, "Port A readback");
        
        // Read via port B (1-cycle latency)
        addr_b = 11'd42;
        @(posedge clk);
        @(posedge clk);
        check(dout_b, 8'hAB, "Port B readback");
        
        $display("dpram: %0d passed, %0d failed", pass, fail);
        if (fail > 0) $fatal;
        $finish;
    end
endmodule
```

**Step 2: Build and run**

```bash
cd e6502.FPGA/test && verilator --cc --exe --build -Wall \
  --top-module test_dpram -Wno-WIDTHTRUNC ../rtl/dpram.sv test_dpram.sv \
  --Mdir build/test_dpram -o test_dpram && build/test_dpram/test_dpram
```

Expected: `dpram: 2 passed, 0 failed`

**Step 3: Verify Yosys synthesizes it to DP16KD**

```bash
cd e6502.FPGA/fpga && yosys -p "read_verilog -sv ../rtl/dpram.sv; synth_ecp5 -top dpram" 2>&1 | grep DP16KD
```

Expected: `2   DP16KD` in cell count

**Step 4: Commit**

```bash
git add e6502.FPGA/rtl/dpram.sv e6502.FPGA/test/test_dpram.sv
git commit -m "feat: add dual-port BRAM wrapper (dpram.sv) for FPGA synthesis"
```

---

### Task 2: Replace char_ram and color_ram with dpram instances

**Files:**
- Modify: `e6502.FPGA/rtl/vgc.sv`

**Context:** char_ram and color_ram each have:
- 1 write port: CPU commands, scrolling, blitter (all from `always_ff @(posedge clk)`)
- 3 combinational read ports: renderer (L1089-1090), CPU readback (L367-368), blitter (L1270-1271)

With dpram: Port A = writes + CPU/blitter reads (muxed), Port B = renderer reads (registered, 1-cycle latency)

**Step 1: Replace char_ram/color_ram declarations with dpram instances**

Remove lines 164-165:
```systemverilog
logic [7:0] char_ram [0:1999];
logic [7:0] color_ram [0:1999];
```

Add dpram instances and mux signals:
```systemverilog
// char_ram dpram — Port A: CPU/cmd/blt write + read, Port B: renderer read
logic [10:0] char_a_addr;
logic [7:0]  char_a_din;
logic        char_a_we;
wire  [7:0]  char_a_dout;
logic [10:0] char_b_addr;
wire  [7:0]  char_b_dout;

dpram #(.WIDTH(8), .DEPTH(2000)) char_ram_inst (
    .clk(clk),
    .addr_a(char_a_addr), .din_a(char_a_din), .we_a(char_a_we), .dout_a(char_a_dout),
    .addr_b(char_b_addr), .dout_b(char_b_dout)
);

// color_ram dpram — same structure
logic [10:0] color_a_addr;
logic [7:0]  color_a_din;
logic        color_a_we;
wire  [7:0]  color_a_dout;
logic [10:0] color_b_addr;
wire  [7:0]  color_b_dout;

dpram #(.WIDTH(8), .DEPTH(2000)) color_ram_inst (
    .clk(clk),
    .addr_a(color_a_addr), .din_a(color_a_din), .we_a(color_a_we), .dout_a(color_a_dout),
    .addr_b(color_b_addr), .dout_b(color_b_dout)
);
```

**Step 2: Route all char_ram writes through port A**

Every `char_ram[addr] <= value` becomes: set `char_a_addr`, `char_a_din`, `char_a_we` in the same always_ff block. Since all writes already happen in a single always_ff, this is a signal rename — add a write-mux at the end of the block:

```systemverilog
// At end of command processor always_ff:
char_a_addr <= char_wr_addr;
char_a_din  <= char_wr_data;
char_a_we   <= char_wr_en;
```

Each existing write site sets `char_wr_addr`, `char_wr_data`, `char_wr_en` instead of directly writing `char_ram[...]`.

**Step 3: Route renderer reads through port B (registered)**

Replace the combinational renderer read:
```systemverilog
// OLD (combinational):
cur_char = char_ram[real_row * COLS + text_col];
cur_fg   = color_ram[real_row * COLS + text_col][3:0];

// NEW (registered, 1-cycle prefetch):
// In always_ff: set read address 1 cycle ahead
char_b_addr  <= next_text_addr;  // real_row * COLS + next_text_col
color_b_addr <= next_text_addr;
// char_b_dout and color_b_dout available next cycle
```

The text rendering pipeline becomes:
- Pixel 0 of char: `char_b_addr` set to current char position (already set last cycle)
- Pixel 1: `char_b_dout` arrives with char code → latch it, set font_rom address
- Pixels 2-7: shift out font pixels from latched font byte
- Pixel 6: set `char_b_addr` for NEXT character (prefetch)

Since characters are 8 pixels wide, we have 8 clocks per character — plenty for the 2-cycle pipeline.

**Step 4: Route CPU/blitter reads through port A**

CPU reads (L367-368) and blitter reads (L1270-1271) currently use combinational reads. Convert to registered reads through port A with a read-request/response protocol:

```systemverilog
// CPU/blitter read: set addr_a, get dout_a next cycle
// Add a 1-cycle read latency register for cpu_rdata and blt_rdata
```

**Step 5: Run Verilator tests**

```bash
cd e6502.FPGA && make clean && make   # rebuild sim
cd test && make                        # run all FPGA tests
```

All existing tests must pass. Text rendering will be offset by 1-2 pixels due to pipeline latency — adjust VGA timing prefetch offset.

**Step 6: Commit**

```bash
git commit -m "feat(vgc): replace char_ram/color_ram with dpram for BRAM synthesis"
```

---

### Task 3: Replace font_rom with dpram instance

**Files:**
- Modify: `e6502.FPGA/rtl/vgc.sv`

**Context:** font_rom is read-only (loaded via $readmemh). Currently read combinationally at L1091 for text rendering and at L649/656/896 for GTEXT command.

**Step 1: Replace font_rom with dpram**

```systemverilog
// font_rom is read-only — use dpram with no writes
logic [10:0] font_a_addr;
wire  [7:0]  font_a_dout;
logic [10:0] font_b_addr;
wire  [7:0]  font_b_dout;

dpram #(.WIDTH(8), .DEPTH(2048), .INIT_FILE("rom/cp437.hex")) font_rom_inst (
    .clk(clk),
    .addr_a(font_a_addr), .din_a(8'h00), .we_a(1'b0), .dout_a(font_a_dout),
    .addr_b(font_b_addr), .dout_b(font_b_dout)
);
```

Port A: GTEXT command reads. Port B: text rendering reads.

**Step 2: Pipeline the text rendering font lookup**

The text pipeline becomes 3 stages:
```
Cycle N:   char_b_addr = text_pos (prefetch char)
Cycle N+1: char_b_dout valid → font_b_addr = {char_b_dout, font_line}
Cycle N+2: font_b_dout valid → shift_reg = font_b_dout, shift out 8 pixels
```

Character is 8 pixels wide, so we start prefetching 2 cycles before the character boundary.

**Step 3: Run tests, commit**

---

### Task 4: Replace gfx_ram with dpram instance

**Files:**
- Modify: `e6502.FPGA/rtl/vgc.sv`

**Context:** gfx_ram is 64000 × 4-bit (32KB). This is the biggest memory and the primary synthesis blocker. Reads: renderer (L1096, combinational), flood fill (L607), CPU register (L959), blitter (L1272). Writes: drawing commands, blitter.

**Step 1: Replace gfx_ram with dpram**

4-bit width means we need 32K × 4-bit. dpram with WIDTH=4, DEPTH=64000.

Port A: all writes (drawing commands, blitter) + command reads (flood fill, register).
Port B: renderer reads (registered).

**Step 2: Serialize circle command (8 writes → 8 cycles)**

Current circle writes 8 octant pixels per cycle. Change to a sub-state machine that does 1 pixel per cycle over 8 cycles. The Bresenham state machine already loops — just add an inner octant counter.

**Step 3: Pipeline flood fill read-modify-write**

Flood fill reads gfx_ram[addr], compares with target, writes if match. With BRAM:
```
Cycle 0: set addr_a = stack_top address
Cycle 1: dout_a valid — compare with fill_target
Cycle 2: if match, write fill_color; push neighbors to stack
```

3 cycles per pixel instead of 1, but flood fill was never fast anyway.

**Step 4: Pipeline graphics rendering prefetch**

GFX pixel = 2 VGA pixels wide (320→640 doubling). Prefetch gfx_ram 2 cycles ahead:
```
Cycle N:   gfx_b_addr = next_gfx_pixel_addr
Cycle N+2: gfx_b_dout valid → palette lookup
```

**Step 5: Run tests, commit**

---

## Phase 2: Sprites (scanline buffer approach)

### Task 5: Implement sprite scanline buffer

**Files:**
- Modify: `e6502.FPGA/rtl/vgc.sv`

**Context:** Current design reads sprite_shapes for ALL 16 sprites combinationally on every pixel (16 parallel reads). Impossible with BRAM. Solution: scanline buffer.

**Architecture:**
1. During horizontal blanking (160 pixel clocks): iterate through 16 sprites, check which intersect current+1 scanline, read their pixel data from sprite_shapes BRAM, write to a 320-entry scanline buffer
2. During active display: read scanline buffer (simple sequential read)

With 160 blanking clocks and 16 sprites × up to 16 pixels each = 256 max reads, this fits if we process ~2 pixels per clock (doable with 1 BRAM read/clock since we prefetch).

**Step 1: Replace sprite_shapes with dpram**

Port A: sprite commands (SPRDEF, SPRROW, SPRCOPY, etc.) + sprite pixel write.
Port B: scanline buffer fill reads.

**Step 2: Create sprite scanline buffer (320 × 5-bit: 4-bit color + 1-bit valid)**

Small BRAM or register array (320 entries = trivial).

**Step 3: Implement hblank sprite evaluator state machine**

During h_count >= H_ACTIVE: for each sprite that intersects next scanline, read its row from sprite_shapes via port B, write decoded pixels to scanline buffer.

**Step 4: Replace combinational sprite rendering with scanline buffer read**

In the rendering always_comb: instead of 16 parallel sprite_shapes reads, just read the scanline buffer (1 entry per pixel, sequential).

**Step 5: Serialize SPRROW (8 writes → 8 cycles)**

Same pattern as circle serialization.

**Step 6: Fix SPRCOPY read-modify-write**

SPRCOPY reads 128 bytes from source shape and writes to dest shape. Pipeline: read via port B, write via port A, 1 byte per cycle over 128 cycles.

**Step 7: Run tests, commit**

---

## Phase 3: Tile Engine

### Task 6: Replace tile_engine memories with dpram

**Files:**
- Modify: `e6502.FPGA/rtl/tile_engine.sv`

**Context:** tile_data (32K), nametable (4K), attr_table (4K), pal_ram (256). Each has 1 write port (DMA/commands) and 1 combinational read port (rendering). Straightforward dpram conversion.

**Step 1: Replace all 4 arrays with dpram instances**

Port A: DMA/command writes + CPU reads.
Port B: rendering reads (registered).

**Step 2: Pipeline tile rendering**

Tile rendering needs 3 sequential reads:
```
Cycle 0: nametable_b_addr = {nt, tile_row, tile_col} → get tile ID
Cycle 1: tile_data_b_addr = {tile_id, pixel_y, pixel_x} → get packed byte  
Cycle 2: pal_ram_b_addr = {sub_pal, color_idx} → get RGB
Cycle 3: output pixel
```

Tiles are 8 pixels wide, giving 8 clocks per tile. 4-stage pipeline fits easily.

**Step 3: Run tests, commit**

---

## Phase 4: Full Integration & Synthesis

### Task 7: Test full Yosys synthesis

**Step 1: Synthesize VGC alone**

```bash
cd e6502.FPGA/fpga && yosys -p "
read_verilog -sv -DSYNTHESIS ../rtl/dpram.sv ../rtl/vgc.sv ../rtl/tile_engine.sv;
hierarchy -top vgc; proc; opt; memory; stat
"
```

Target: completes in <60 seconds, uses <1GB RAM, shows DP16KD cells.

**Step 2: Synthesize full design**

```bash
make clean && make synth
```

Target: completes synthesis in <5 minutes.

**Step 3: Place & route**

```bash
make pnr
```

**Step 4: Generate bitstream and program**

```bash
make prog
```

**Step 5: Verify on real hardware**

Connect HDMI monitor, open serial terminal at 115200, verify EhBASIC boots and text is visible.

**Step 6: Commit everything**

```bash
git commit -m "feat: VGC synthesis-ready — all memories through dpram, pipelined rendering"
```

---

## Key Design Rules

1. **Every memory read must be registered** — `dout <= mem[addr]` in an `always @(posedge clk)` block. No combinational `mem[addr]` anywhere.
2. **One write port per dpram** — all writers mux through port A signals in a single always_ff.
3. **Prefetch 2 cycles ahead** — for characters (every 8 pixels) and graphics (every 2 pixels).
4. **Serialize multi-writes** — circle octants and SPRROW do 1 write/cycle with a sub-counter.
5. **Sprite scanline buffer** — fills during hblank, reads during active display.
6. **All changes must pass Verilator tests** — the pipelined design produces the same pixels, just shifted by the prefetch offset (compensated in timing).
