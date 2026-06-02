# XRAM Streaming Engine + Tier-1 Page-In — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a page-mode burst read path to `sdram.v` and use it to page a 16 KB
library module from XRAM into the `ext_rom` (bank-1) BRAM (~0.65 ms, Tier 1), proven
Verilator-first before any synth.

**Architecture:** Extend the existing MiST `sdram.v` (do NOT replace it) with a new
`streaming` mode that monopolises the SDRAM bus for a burst: ACTIVATE a row → back-to-back
READs (A10=0, no auto-precharge) → PRECHARGE → AUTO_REFRESH per row boundary. A dedicated
`page_dma` module in the `sdram_clk` domain consumes the word stream and writes it straight
into `ext_rom` BRAM (no data-path CDC). CPU triggers it via a small MMIO register block and
is stalled (`rdy_out`) for the whole copy, making the page-in atomic.

**Tech Stack:** SystemVerilog, Verilator (`--timing`), the `e6502.FPGA/test/` harness
(`make -C test test_<name>`). Synthesis (yosys/nextpnr-ecp5) runs **on beast only**, and
**only after the Verilator bench is green** ([[feedback_always_synth_on_beast]],
[[feedback_verilator_first_for_fpga]]).

**Full design spec:** `docs/plans/2026-06-02-paged-runtime-libraries-and-xram-design.md` §3.3
(committed `fbdc2a7`). This plan implements **Phase 2** of that doc's rollout. Phases 3–6
(loader, modules, Regime-2 FIFO/write-burst) are separate future plans.

---

## Ground truth (verified, do not re-derive)

- **Clocks** (`fpga/fpga_top.sv`): `clk_sdram` = 100 MHz, `clkref` = 6.25 MHz → **16:1**.
- **Address decode** every access uses (`sdram.v:213` `run_addr`, `:220` `sd_ba`):
  `bank = a[23:22]`, `row = a[21:9]`, `col = {a[24], a[8:1]}`, where `a` is the 25-bit
  **byte** address. For our 512 KB XRAM (`a[18:0]`, so `a[24:22]=0`): bank 0, `col[8]=0`,
  so **columns 0–255 only → a row is 512 bytes (256 words). 16 KB = 32 row-opens.**
  The streaming read MUST replicate this decode exactly or it reads the wrong bytes.
- **Byte order** (`sdram.v:158`): even byte (`a[0]=0`) = `sd_data_in[15:8]`, odd = `[7:0]`.
  So when writing a streamed word to `ext_rom`: `ext_rom[2k] = dout[15:8]`, `ext_rom[2k+1]
  = dout[7:0]`, or the paged ROM byte-swaps.
- **`ext_rom`** (`top.sv:463`): `dpram WIDTH(8) DEPTH(16384)`. Port A = write
  (`dbg_rom_addr/data`, `we = erom_we = dbg_rom_we && dbg_rom_idx==1`), port B = CPU read
  (`erom_b_addr → erom_b_dout`). `dpram` is **single-clock** (`dpram.sv` — both ports on
  one `clk`); `ext_rom_inst.clk` is the pixel `clk`. ⇒ Tier-1 writes `ext_rom` at pixel
  clock; `page_dma` crosses the word stream from `sdram_clk` to pixel clock through a small
  async FIFO (the only data-path CDC in Phase 2).
- **CPU stall precedent**: `dma.sv` / `blitter.sv` drive `rdy_out` low while busy
  (`top.sv` ANDs them into CPU RDY). `page_dma` does the same.
- **MMIO free slot**: `$BA76`–`$BA82` is unclaimed (DMA ends `$BA75`, blitter starts
  `$BA83`). Use `$BA76`+ for the `page_dma` registers. **Verify unclaimed** before writing
  (grep `top.sv` for `BA76`..`BA82`).
- **Test harness**: `cd e6502.FPGA && make -C test test_<name>` builds + runs one test.
  Recipe pattern (`test/Makefile:372`):
  `verilator $(VFLAGS) --top-module test_x --Mdir build/test_x -o test_x <rtl deps> test_x.sv && (cd .. && test/build/test_x/test_x)`.
  `VFLAGS` includes `--timing`. A stand-alone sdram test lists only `$(RTL)/sdram/sdram.v`
  as its dep. A top-level test lists `ROM_LOAD_RTL` (see `test/Makefile:293`).
- **Cycle counts are the truth, not nanoseconds.** The bench clock need not be 100 MHz;
  assert on **`sdram_clk` cycle count** (clock-rate independent). Expected for 16 KB:
  32 rows × ~270 clk + overhead ≈ **~8,400–8,700 cycles** (≈84–87 µs @ 100 MHz). The bench
  *measures and prints* the real number; assert a generous window and record the actual.

---

## Task 1: Page-mode-aware MT48LC16M16 behavioral model

The loopback model (`test_sdram_loopback.sv`) is single-access only and not timing-checking.
Build a reusable page-mode model with timing assertions, as an include file.

**Files:**
- Create: `e6502.FPGA/test/sdram_model.svh`

**Step 1: Write the model** — a behavioral chip that tracks per-bank open row, services
back-to-back column reads with A10=0, requires PRECHARGE before re-ACTIVATE, and **asserts
timing**: a READ only legal ≥ tRCD after ACTIVATE; data appears CAS cycles after READ;
PRECHARGE only after tRAS; re-ACTIVATE only after tRP; AUTO_REFRESH only with all banks
precharged. On any violation, `$error` + `$fatal`.

```systemverilog
// sdram_model.svh — behavioral MT48LC16M16 with page-mode + timing assertions.
// Include into a testbench; instantiate `sdram_model chip(...)` wired to the sd_* pins.
// Storage is 16-bit words keyed by {bank,row,col}; honours DQM byte masks on write.
// Timing params in sdram_clk cycles @100MHz: tRCD=2, CAS=3, tRP=2, tRAS=5, tRC=7.
// NOTE: returns correct DATA for page-mode (A10=0) reads; the controller owns exact
// timing — but this model *checks* the controller obeys tRCD/CAS/tRP/tRAS/tRC and
// $fatal's otherwise, which is the whole point of the bench.
```
(Full module body: decode the 4-bit command from {cs,ras,cas,we}; on ACTIVE latch
`open_row[bank]` + stamp activate-time; on READ/WRITE check `A10` for auto-precharge,
check tRCD since activate, pipeline read data by CAS into a small shift register driving
`sd_data_in`; on PRECHARGE clear open row(s) honouring A10 (all-banks vs one); on
AUTO_REFRESH assert all banks precharged. Use an associative array
`logic [15:0] mem [bit [24:0]]` keyed by word address for sparse storage.)

**Step 2: Self-check the model** — add a tiny `test_sdram_model_selfcheck.sv` that pokes
the model directly (not through `sdram.v`): ACTIVATE→READ before tRCD must `$fatal`; a
legal ACTIVATE→(wait tRCD)→READ→(wait CAS) returns the stored word.

Run: `make -C test test_sdram_model_selfcheck`
Expected: PASS (legal path) and a separate `EXPECT_FATAL=1` variant that confirms the
violation path traps. (Add target to `test/Makefile` `TESTS` + a recipe line listing no
RTL deps — model is self-contained.)

**Step 3: Commit**
```bash
git add e6502.FPGA/test/sdram_model.svh e6502.FPGA/test/test_sdram_model_selfcheck.sv e6502.FPGA/test/Makefile
git commit -m "test(sdram): page-mode MT48LC16M16 behavioral model with timing assertions"
```

---

## Task 2: `stream_*` port stubs on `sdram.v`

Add the general stream port so the bench compiles. Stub only — `streaming` always 0.

**Files:**
- Modify: `e6502.FPGA/rtl/sdram/sdram.v` (port list + a stubbed `streaming` reg)

**Step 1: Add ports** (after the port-B block, before the closing `);`):
```verilog
	// Page-mode burst read port (sdram_clk domain). Mutually exclusive with
	// ports A/B via the `streaming` flag.
	input            stream_req,    // pulse to start a burst
	input     [24:0] stream_addr,   // start BYTE address
	input     [12:0] stream_words,  // # 16-bit words to read
	output reg[15:0] stream_dout,
	output reg       stream_valid,  // 1-clk strobe per word
	output reg       stream_busy,
	output reg       stream_done    // 1-clk pulse at completion
```

**Step 2: Stub bodies** near the top of the always block:
```verilog
// STUB (Task 4 replaces): no streaming yet.
reg streaming;
always @(posedge clk) begin
	streaming    <= 1'b0;
	stream_valid <= 1'b0;
	stream_busy  <= 1'b0;
	stream_done  <= 1'b0;
	stream_dout  <= 16'd0;
end
```

**Step 3: Verify existing sdram tests still pass** (ports added, behaviour unchanged):
```bash
make -C test test_sdram_loopback && make -C test test_xram_sdram
```
Expected: both PASS (the new ports are unconnected in those harnesses).

**Step 4: Commit**
```bash
git add e6502.FPGA/rtl/sdram/sdram.v
git commit -m "feat(sdram): add stream_* port stubs for page-mode burst read"
```

---

## Task 3: Bench case (a) — byte-exact stream readback (RED)

**Files:**
- Create: `e6502.FPGA/test/test_sdram_stream.sv`
- Modify: `e6502.FPGA/test/Makefile` (add `test_sdram_stream` to `TESTS` + recipe listing
  `$(RTL)/sdram/sdram.v` as the only RTL dep, mirroring `test_sdram_loopback`)

**Step 1: Write the failing test.** Drive `sdram.v` with the 16:1 clkref pattern (copy from
`test_sdram_loopback.sv`), wire `sdram_model` to the pins. Test body:
1. Single-access **write** a known pattern (e.g. `mem[byte k] = k & 0xFF`) for 512 bytes via
   port A (`addrA/weA/dinA/oeA`, wait `doneA`) — exercises the existing path.
2. Pulse `stream_req` with `stream_addr=0`, `stream_words=256`.
3. Capture every `stream_valid` word into `got[i]`.
4. After `stream_done`: assert `got[i][15:8] == expected even byte (2i)` and
   `got[i][7:0] == expected odd byte (2i+1)` for all 256 words (**byte order**).

**Step 2: Run — expect FAIL** (stub never asserts `stream_done`/`stream_valid`; test
times out or asserts mismatch):
```bash
make -C test test_sdram_stream
```
Expected: FAIL (no valid words / done never fires).

**Step 3: Commit the RED test**
```bash
git add e6502.FPGA/test/test_sdram_stream.sv e6502.FPGA/test/Makefile
git commit -m "test(sdram): failing byte-exact stream readback case"
```

---

## Task 4: Implement the page-mode FSM in `sdram.v` (GREEN)

Replace the Task-2 stub with the real branch. Spec: design doc §3.3 "page-mode read FSM".

**Files:**
- Modify: `e6502.FPGA/rtl/sdram/sdram.v`

**Step 1: Implement.** Key requirements (full code authored here during execution):
- `streaming` flag: while set, force `run_cmd → CMD_INHIBIT` (gate the existing
  `assign sd_cmd`) and override `sd_addr/sd_ba/sd_dqm` from the stream FSM. The free-running
  `q` counter keeps spinning — only the *outputs* are overridden.
- Stream FSM states: `PRECHARGE_ALL → ACTIVATE → READ (col++, A10=0, ≤256/row) → CAPTURE
  (CAS-3 pipelined, emit dout+valid 1/clk) → PRECHARGE(bank) → REFRESH (AUTO_REFRESH,
  unconditional, 1/row) → next row or DONE`.
- Address gen: `cur_addr` walks the byte address from `stream_addr`; per access derive
  `row=cur[21:9]`, `col={cur[24],cur[8:1]}`, `bank=cur[23:22]`; advance by 2 bytes/word;
  detect row boundary when `cur[9]` (row LSB) changes (every 256 words).
- Own dedicated counters for tRCD(2)/CAS(3)/tRP(2)/tRC(7) — do **not** reuse `q`.
- `stream_busy` high from `stream_req` accept to `stream_done`; `stream_done` 1-clk pulse.
- Entry: latch `stream_req` only when `reset==0`; issue PRECHARGE_ALL first.

**Step 2: Run case (a) — expect PASS:**
```bash
make -C test test_sdram_stream
```
Expected: PASS (256 words, byte-exact, correct order).

**Step 3: Regression — existing sdram paths still pass:**
```bash
make -C test test_sdram_loopback && make -C test test_xram_sdram
```
Expected: both PASS (single-access path untouched; `streaming` defaults low).

**Step 4: Commit**
```bash
git add e6502.FPGA/rtl/sdram/sdram.v
git commit -m "feat(sdram): page-mode streaming read branch (ACTIVATE/READ/PRECHARGE/REFRESH)"
```

---

## Task 5: Bench case (b) — multi-row boundary sequence

**Files:** Modify `e6502.FPGA/test/test_sdram_stream.sv`

**Step 1: Add test.** Write 2 KB pattern (= 4 rows × 512 B), stream `stream_words=1024`.
Have `sdram_model` count commands; assert exactly **4 ACTIVATEs, 4 PRECHARGEs (bank), ≥3
AUTO_REFRESHes** and that every byte read back is exact across all 4 rows. The model's
timing assertions ($fatal on tRCD/tRP/tRC violation) do the sequence-legality checking.

**Step 2: Run — fix FSM if boundary handling is off, then PASS:**
```bash
make -C test test_sdram_stream
```
Expected: PASS.

**Step 3: Commit**
```bash
git add e6502.FPGA/test/test_sdram_stream.sv
git commit -m "test(sdram): multi-row burst crosses row boundaries correctly"
```

---

## Task 6: Bench case (c) — 16 KB cycle-count measurement

**Files:** Modify `e6502.FPGA/test/test_sdram_stream.sv`

**Step 1: Add test.** Write 16 KB pattern, stream `stream_words=8192`. Count `sdram_clk`
cycles from `stream_req` to `stream_done`. **`$display` the measured count** and assert a
generous window: `7000 < cycles < 11000` (records the real number; replaces the 84 µs
guess — design doc §3.3). Assert all 8192 words byte-exact.

**Step 2: Run — PASS, capture the printed cycle count:**
```bash
make -C test test_sdram_stream 2>&1 | grep -i "cycles\|PASS\|FAIL"
```
Expected: PASS; note the real cycle count in the commit message.

**Step 3: Commit**
```bash
git add e6502.FPGA/test/test_sdram_stream.sv
git commit -m "test(sdram): 16K burst byte-exact; measured <N> sdram cycles (~<T>us @100MHz)"
```

---

## Task 7: Bench case (d) — refresh obligation

**Files:** Modify `e6502.FPGA/test/test_sdram_stream.sv`

**Step 1: Add test.** During the 16 KB burst, have `sdram_model` record the cycle gap
between consecutive AUTO_REFRESHes. Assert **no gap exceeds 780 `sdram_clk` cycles**
(7.8 µs @ 100 MHz). (Per-row unconditional refresh gives ~one per ~270 cycles, well inside.)

**Step 2: Run — PASS:**
```bash
make -C test test_sdram_stream
```
Expected: PASS.

**Step 3: Commit**
```bash
git add e6502.FPGA/test/test_sdram_stream.sv
git commit -m "test(sdram): AUTO_REFRESH issued within tREF across the whole burst"
```

---

## Task 8: Bench case (e) — port-B defers during stream

**Files:** Modify `e6502.FPGA/test/test_sdram_stream.sv`

**Step 1: Add test.** Mid-burst, assert a port-B request (`addrB/oeB`). Assert `doneB` does
**not** fire until after `stream_done`, then the port-B read returns the correct byte.
Confirms the `streaming` flag correctly defers port B (it holds its request).

**Step 2: Run — PASS** (may require ensuring `run_cmd` gating also blocks port-B servicing
while `streaming`):
```bash
make -C test test_sdram_stream
```
Expected: PASS.

**Step 3: Commit**
```bash
git add e6502.FPGA/test/test_sdram_stream.sv e6502.FPGA/rtl/sdram/sdram.v
git commit -m "test(sdram): port-B request defers until burst completes"
```

> **GATE:** Tasks 1–8 green = the streaming engine is proven in Verilator. Only now does
> the consumer-side work (Tasks 9–12) and, later, synth-on-beast make sense.

---

## Task 9: `ext_rom` write-port mux (boot bridge vs `page_dma`)

**Files:** Modify `e6502.FPGA/rtl/top.sv` (around `:463` `ext_rom_inst`)

**Step 1: Add a write-source mux** in front of `ext_rom_inst` port A. New inputs from
`page_dma` (declared as wires now, driven in Task 10): `pgd_erom_we`, `pgd_erom_addr[13:0]`,
`pgd_erom_data[7:0]`. Mux:
```verilog
wire        erom_we_muxed   = pgd_active ? pgd_erom_we   : erom_we;
wire [13:0] erom_addr_muxed = pgd_active ? pgd_erom_addr : dbg_rom_addr;
wire [7:0]  erom_data_muxed = pgd_active ? pgd_erom_data : dbg_rom_data;
```
Wire those into `ext_rom_inst.addr_a/din_a/we_a`. (`pgd_active` from Task 11; tie to 0 for
now so behaviour is unchanged.) Boot bridge and page_dma never overlap.

**Step 2: Regression** — boot ROM-load path unchanged with `pgd_active=0`:
```bash
make -C test test_rom_load
```
Expected: PASS.

**Step 3: Commit**
```bash
git add e6502.FPGA/rtl/top.sv
git commit -m "feat(top): ext_rom write-port mux (boot bridge vs page_dma)"
```

---

## Task 10: `page_dma` module — stream → `ext_rom`

**Files:**
- Create: `e6502.FPGA/rtl/page_dma.sv`
- Create: `e6502.FPGA/test/test_page_dma.sv` + Makefile target (deps: `sdram.v`,
  `page_dma.sv`, `dpram.sv`, plus an instance of `sdram_model`)

**Step 1: Write failing test.** Harness wires `sdram_model` ↔ `sdram.v` (stream port) ↔
`page_dma` ↔ a `dpram` standing in for `ext_rom`. Pre-load 16 KB pattern into the model.
Trigger `page_dma` (start, `src_base=0`, `words=8192`). After `page_dma` done, read the
`dpram` (port B) for all 16384 bytes and assert `ext_rom[2k]=pattern_even`,
`ext_rom[2k+1]=pattern_odd` (**byte order**). RED first (no `page_dma.sv` yet → won't link;
write a minimal empty module so it links and the assertion fails).

**Step 2: Implement `page_dma.sv`.** `sdram_clk`-domain FSM: on `start`, assert
`stream_req` to `sdram.v` with `stream_addr=src_base`, `stream_words=words`; for each
`stream_valid`, write two bytes into the small async FIFO toward the pixel domain (or, for
this unit test where everything is one clock, write directly to the `dpram` at
`addr=2k`/`2k+1`). Maintain `pgd_active` high during the copy; pulse `done` at
`stream_done`. (The async FIFO + pixel-domain drain is added in Task 11 where the real
`ext_rom` lives on the pixel clock.)

**Step 3: Run — PASS:**
```bash
make -C test test_page_dma
```
Expected: PASS (16 KB byte-exact, correct order).

**Step 4: Commit**
```bash
git add e6502.FPGA/rtl/page_dma.sv e6502.FPGA/test/test_page_dma.sv e6502.FPGA/test/Makefile
git commit -m "feat(page_dma): sdram_clk consumer streams XRAM -> ext_rom (byte-ordered)"
```

---

## Task 11: CPU trigger registers + CDC + `rdy_out` stall

**Files:**
- Modify: `e6502.FPGA/rtl/page_dma.sv` (add CPU-domain register block + async FIFO to pixel
  clock for the real `ext_rom` write + `rdy_out`)
- Modify: `e6502.FPGA/rtl/top.sv` (instantiate `page_dma`, wire MMIO + `pgd_active` + the
  `ext_rom` write mux from Task 9 + AND `pgd_rdy` into CPU RDY)

**Step 1: Define registers** at `$BA76`+ (verify unclaimed first):
`PGD_CMD ($BA76, write 1=start)`, `PGD_STATUS ($BA77: busy/done/err)`,
`PGD_SRCL/M/H ($BA78–7A, 24-bit XRAM byte base)`, `PGD_WORDSL/H ($BA7B–7C, default 8192)`.
CPU writes are pixel-domain; CDC `start` (toggle) into `sdram_clk`; CDC `done` back. While
busy, drive `rdy_out=0` (CPU stalled → page-in atomic, no race).

**Step 2: Write failing top-level test** `test_page_in_top.sv` (deps: `ROM_LOAD_RTL` +
`page_dma.sv`, like `test_rom_load`): preload a 16 KB pattern into XRAM (via the existing
port-B poke path the rom_load/novaz tests use), then CPU writes `PGD_SRC*`/`PGD_WORDS*` and
`PGD_CMD=1`; after the stall releases, CPU reads `$C000`+ (with `ext_rom_active=1`) and
asserts the bytes match the pattern. RED before wiring.

**Step 3: Implement** the register block + FIFO + `top.sv` wiring; run:
```bash
make -C test test_page_in_top
```
Expected: PASS.

**Step 4: Regression sweep** (engine + consumers + boot intact):
```bash
for t in test_sdram_stream test_page_dma test_rom_load test_xram_sdram test_sdram_loopback test_page_in_top; do make -C test $t || echo "FAIL $t"; done
```
Expected: all PASS.

**Step 5: Commit**
```bash
git add e6502.FPGA/rtl/page_dma.sv e6502.FPGA/rtl/top.sv e6502.FPGA/test/test_page_in_top.sv e6502.FPGA/test/Makefile
git commit -m "feat(page_dma): CPU MMIO trigger ($BA76) + CDC + rdy_out stall; end-to-end page-in"
```

---

## Task 12: Diag + synth gate (beast only)

**Files:** none (build verification)

**Step 1: BRAM/timing diag on beast** ([[feedback_yosys_diag_for_bram_fit]],
[[reference_beast_synth_wrapper]]):
```bash
tools/beast-synth.sh diag xram-stream
```
Expected: DP16KD count not materially worse than baseline (202/208-ish); no new timing
blowup from the stream mux on `sdram_clk`.

**Step 2: Full bitstream on beast only if diag is clean** ([[feedback_dont_ship_timing_failing_bitstream]],
[[feedback_nextpnr_seed_sweep]] if timing misses by <1 MHz):
```bash
tools/beast-synth.sh bitstream xram-stream
```
Expected: timing met; back up the prior working `.bit` first
([[feedback_backup_bitstreams]]).

**Step 3: Hardware measurement** — flash, page a real 16 KB blob, confirm the measured
page time matches the Verilator cycle count (Task 6) and the paged bytes read correctly at
`$C000`. Record the real HW figure.

**Step 4: Commit any synth-config changes** (Makefile seeds, etc.) separately.

---

## Notes for the executor

- **TDD is non-negotiable here** ([[feedback_tests_prove_fixes]]): every RTL change lands
  with a Verilator test that fails without it. The `sdram_model` timing assertions are the
  safety net for the synth-only bugs Verilator usually misses ([[feedback_verilator_blind_to_synth_mapping]]).
- **One change at a time** ([[feedback_one_change_at_a_time]]): never combine the FSM, the
  consumer, and the mux in one commit.
- **Do not synth locally** ([[feedback_always_synth_on_beast]]); do not pivot a long synth
  ([[feedback_dont_pivot_late]]).
- **Sim/synth read-latency drift** ([[feedback_dpram_read_latency_harness]]): the bench
  `dpram` reads are posedge-clocked (1-cycle latency) — match that in the harness, don't use
  combinational reads.
- Out of scope (future plans): the loader/dispatch stub + module format (Phase 3), first
  paged GRAPHICS module (Phase 4), and Regime-2 FIFO + symmetric write-burst wiring of
  blitter/DMA/XMC (Phase 6).
