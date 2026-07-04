# Handoff — 2026-06-03: Page-in SHIPPED, Phase 3 (loader/modules) is next

## TL;DR

The **XRAM→ext_rom page-in is byte-exact on real hardware and shipped** (`main @ b339d49`,
pushed). That was the long-pole of the paged-runtime rollout. **What's next is software:
Phase 3 (3b) — the resident loader/dispatch stub + module binary format** that turns bank-1
`ext_rom` into a paged shared-library overlay window. It was the original "NEXT ACTION" before
we found and fixed an HW silicon bug; that bug is now closed.

**Master narrative (read first):** `~/.claude/projects/-Users-barry-Git-e6502/memory/project_xram_streaming_design_2026_06_02.md`
(the ✅ SHIPPED anchor at top + the full 🔴→🟡→✅ arc).

---

## What shipped this session (commit `b339d49`)

The page-in (`page_dma` + `sdram.v` stream FSM) completed but read back **garbage on real
silicon** while Verilator passed byte-exact. Root cause: **two SDRAM stream-FSM timing bugs,
both invisible to behavioral Verilator because `sdram_model.svh` never enforced tRFC**:

1. **tRFC after AUTO_REFRESH** — `S_REFRESH` waited only `STR_TRP`(2cy); tRFC needs ~7cy → rows
   1–31 read floating DQ (`0xAA`). Same hole at stream *entry* (the single-access path's
   opportunistic `AUTO_REFRESH` lands just before `stream_req`) corrupted row 0.
   **Fix:** `STR_TRFC=9` waited in `S_REFRESH` + new `S_ENTER` state waits tRFC at entry.
2. **Back-to-back read miscapture** — 1-word/clk reads drift the DQ eye on real silicon; the
   fixed-offset capture grabbed an adjacent word. **Fix:** `str_pace` issues reads 1 word/2clk
   so each DQ read is isolated like the proven single-access path (matches `page_dma`'s
   2clk/word drain → ~no throughput loss; 16KB page-in ≈ 171µs).

**Method that worked (repeatable):** made `sdram_model.svh` *enforce* tRFC → that **reproduced
the bug in Verilator (RED)** → fixes turned it GREEN. Bug 2 is analog (Verilator can't see it);
fixed by mimicking the working single-access isolation, verified only on HW.

**HW verdict:** `tools/run-page-in-hardware-smoke.py` → checksum **57344 byte-exact,
deterministic ×4**.

---

## Current hardware / board state

- Board: **192.168.1.65 / novahost.local**. `defaultRuntime=novabasic` (changed from `novalogo`
  this session via `nova runtime set novabasic`; the board boots NovaBASIC).
- Flashed bitstream: `e6502.FPGA/fpga/bit_backups/e6502.20260603_153938.pagein-pace-fix-s3.bit`
  (md5 `1a88b22b955da8bead7b82e8fc1cd1dd`). **Built with `NEXTPNR_SEED=3`** — the `str_pace`
  RTL needs seed 3 to close `sdram_clk`@100 (seed 1 misses at 98.59). Record this for re-synth.
- Prior (buggy) backup: `bit_backups/e6502.20260603_011659.xram-stream.bit`.

## Page-in MMIO (works now)
`$BA76` PGD_CMD (write 1 = start) · `$BA77` STATUS (bit0 busy, bit1 done/ok) · `$BA78-7A`
SRC L/M/H (24-bit XRAM byte base) · `$BA7B-7C` WORDS L/H (≤8192 = 16KB). CPU stalls (rdy_out)
during the copy → atomic. `ext_rom` = bank-1 dual-clock `dpram_dc`, mapped at `$C000` when
`REG_ROMSWAP`(`$A03F`)=`ROMSWAP_EXTENSION`(`$04`). `ROMSWAP_BASIC=$02`, `ROMSWAP_LOGO=$05`.

---

## Verify / build commands

- **Verilator gate (fast):** `cd e6502.FPGA && make -C test test_sdram_stream` (+ `test_page_dma`,
  `test_page_in_top`, `test_xram_sdram`, `test_sdram_loopback`). All green at `b339d49`.
- **Synth (17 min):** `make -C e6502.FPGA/boards/ulx3s bitstream` — set
  `NEXTPNR_SEED=3` for the current sdram.v when reproducing this handoff.
  Timing gate is on; never ship a failing one.
- **Flash:** `openFPGALoader --board ulx3s -f --verify <bit>`. If FTDI is busy (`-6`), physically
  replug USB and retry.
- **HW page-in regression:** the old Python smoke wrapper has been retired.
  Use the Nova CLI VM/debug commands and the FPGA test targets above when
  verifying future stream/page_dma changes on hardware.

---

## NEXT: Phase 3 (3b) — loader/dispatch + module format

This is "the software." It turns bank-1 `ext_rom` from a single fixed extension ROM into a
**paged overlay window**: bank 0 = per-runtime foundation + a resident loader stub; bank 1 = the
one active library module (16K), paged in on demand via the now-working `$BA76` page-in.

**Authoritative design:** `docs/plans/2026-06-02-paged-runtime-libraries-and-xram-design.md`
- §5 **dispatch model is LOCKED**: keep the flat `EXT_CMD` id; add a resident `map[EXT_CMD]→module-id`
  table + `lib_call` that page-on-misses (`if resident_module != module: page_dma(xdir_lookup(module)); JSR EXT_TRAMPOLINE`).
- §6 rollout: this is **step 3**. (Step 2 = the engine/page-in, now DONE+HW-verified.)
- §7 **still OPEN (the 3b design work):** (a) **module header bytes** — magic/module-id/version/
  fn-count widths + jump-table entry format; (b) **boot staging** — module manifest on SD, XRAM
  shelf layout, how the loader learns each module's XRAM base.

**Existing code Phase 3 builds on (verified this session):**
- `ehbasic/extension.s` — current bank-1 ROM: 67 cmds (`$01–$42`), `cmd*2` RTS-trick jump table
  at `$C000` (`ExtEntry`), `ExtCmdId=$E4`. Uses RAM bridges `EXT_GTBY/GTWRD/GTSW/SNERR`
  (`$0240/$024E/$025C/$026A`).
- `ehbasic/min_mon.asm` `EXT_CODE` — the BASIC trampoline (sets `ROMSWAP_EXTENSION`, `JSR $C000`,
  restores `ROMSWAP_BASIC`). Built into page 2 at `EXT_vec=$0226`; input buffer at `$0272-$02F1`.
- `novalogo/ext_iface.inc` + `novalogo/extension.s` — Logo's ABI: ZP mailbox `EXT_CMD=$80`
  (args $82-$8D, result $8E-$91), trampoline `EXT_TRAMPOLINE=$0270`. **NOTE: BASIC and Logo have
  DIFFERENT mailboxes** ($E4 vs $80) — the shared module ABI must reconcile or the loader is
  per-runtime.

**⚠️ Dependency gotcha:** §5's `lib_call → xdir_lookup(module)` assumes the **XRAM directory/
allocator (Phase 1 / "System B")**, but Phase 1 is **NOT done** — `runtime/asm/xmc.s` is still
the 256KB-pool allocator (`TOTAL_PAGES=XRAM_USER_HEAP_PAGES`, no 512KB growth, no reserve-at-
address). So before/within 3b, decide module staging: either (a) bootstrap with a **fixed XRAM
shelf base** + NovaHost manifest (simplest, no Phase-1 dep), or (b) do Phase 1 first. Recommend
(a) to keep moving.

**Recommended approach (per the original plan + how Phase 2 went):**
1. `superpowers:brainstorming` the 3b design → lock module header format, command→module map,
   boot staging into the design doc one section at a time.
2. `superpowers:writing-plans` → an impl plan like
   `docs/plans/2026-06-02-xram-streaming-engine-impl.md`.
3. `superpowers:subagent-driven-development` / `executing-plans` with full ceremony (TDD,
   one-change-at-a-time). Phase 4 = move NovaLogo's existing graphics extension into a paged
   `GRAPHICS` module + call it through the loader (the first real end-to-end proof — and the
   first thing that needs a *real* module staged in XRAM, which the page-in now delivers).

---

## Hard-won gotchas this session (don't relearn these)

- **Behavioral SDRAM models must enforce tRFC** (and every datasheet timing the controller can
  violate). Missing checks hide stream bugs that only bite on HW. Add the missing check to
  *reproduce* a Verilator-blind timing bug, then fix RTL. → `feedback_sdram_model_enforce_trfc`.
- **Host debug `poke` derails a RUNNING CPU after ~10 pokes** (PC walks into the stack). Always
  `pause()`/`resume()` around multi-poke batches; `sleep(~0.4s)` after resume before typing.
  → `feedback_debug_bridge_poke_needs_pause`.
- **Host `peek`/`poke` ≠ CPU reads for ROM/MMIO** (different bus path). Trust ONLY CPU-side
  (BASIC/stub) reads for memory content; host peek is fine for plain RAM.
  → `feedback_host_peek_not_cpu_read`.
- **`str_pace` RTL needs `NEXTPNR_SEED=3`** to close `sdram_clk`@100 MHz.
- **Diagnostic pattern that cracked it:** a small RAM stub (POKEd via the host while CPU paused)
  that flips ROMSWAP and copies ext_rom bytes into RAM so BASIC/host can read what the *CPU*
  sees. `/tmp/peek2.s` (parameterizable source page) mapped the corruption precisely.
- NovaHost debug_server exposes `write_sdram`/`read_sdram`/`peek`/`poke`/`type_text`/`send_key`/
  `read_screen` over the network (no firmware change needed to stage XRAM + drive BASIC + observe).
- `cold_start` drops the TCP socket — reconnect + poll `health().bootPhase=='ready'`.
- Verilator-first for FPGA; synth on beast only; one change at a time (each change reshuffles the
  netlist + timing).

## Open low-priority cleanup
- `e6502.FPGA/test/Makefile` still has `-Wno-PINMISSING` (footgun: silently zeroes unconnected
  module inputs; bit us on `stream_ready` earlier).
