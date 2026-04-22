# Phase 2.5 — SDRAM Integration

**Status**: Step 1 + Step 2 shipped 2026-04-22. Step 3 + Step 4 deferred — see
"Closure" section at the bottom.

## Why Now

Phase 2 Batch A (80×60 text + 320×240 gfx geometry, commit `c25e02a`) landed
green: Verilator 4,212 assertions pass, yosys diag 202 of 208 DP16KD (97%).
Phase 2 still has ~+8 blocks of planned growth ahead (gfx_mem 64000→76800 for
320×240 full-screen, font_mem 2048→8192 for 3-slot support). That would push
us to ~210 blocks — over the ECP5-85 budget by 2.

User-selected path:

> "B.. pause phase 2, do phase 2.5 to give us room in bram.. then come back
> to phase 2 to finish up without moving shit around"

And the framing:

> "we'll need to start pushing shit to sdram sooner or later, so we may as
> well rip that bandaid off now. can we get the sid tables, dead code and
> fonts into sdram? those dont feel performance critical to me"

Principle from earlier in the session:

> "i want all of the timing critical shit in bram"

→ SDRAM gets things that aren't pixel-clock-rate accessed. SID tables (audio,
44.1 kHz), XRAM (CPU demand-paged via XMC), optionally fonts (pixel-rate but
scanline-cacheable).

## Starting State

- `rtl/sdram/sdram.v` exists (MiST-adapted Luddes NES core). 2-port, 25-bit
  byte-addressed, 16-bit data. **Not instantiated anywhere.**
- `top.sv:152` has `xram_inst` as a 4KB BRAM stub (XRAM at $BC00-$BFFF via
  XMC paged window). Stub is explicit TODO pointing to SDRAM.
- Board: ULX3S with MT48LC16M16 SDRAM chip, 32MB. Pins defined in stock
  ULX3S LPF templates (not yet in our `ulx3s.lpf`).
- NovaHost (ESP32) already streams `ehbasic.bin` + `extension.bin` to FPGA
  BRAM at boot via UART protocol. Same pipeline extensible to SID tables
  and fonts.

## Goals

1. Instantiate `sdram.v`, get a loopback (write → read) working through real
   SDRAM hardware.
2. Migrate XRAM from 4KB BRAM stub → 512KB real SDRAM. Side benefit: matches
   the multi-env vision (SD-card ROM library + large XRAM).
3. Migrate SID tables (`f6581_curve`, `wave6581/8580_*`, preserve unused
   `_unused_f6581_adj` for future Fc_offset work) to SDRAM. Free ~4 BRAM
   blocks.
4. Re-run yosys diag after each migration; stop early when Phase 2 budget
   fits with comfortable headroom.
5. (Optional) Migrate `font_mem` to SDRAM + scanline prefetch cache. Only
   pursued if Steps 1-3 don't clear enough headroom.

## Non-Goals

- CPU-direct SDRAM execution (ROM from SDRAM). EhBASIC ROM stays in BRAM —
  CPU every-cycle access requires single-cycle read.
- Video framebuffer (gfx_mem) to SDRAM. Scanline-rate reads are already fine
  in BRAM at our scope (320×240 @ 4bpp = 32 blocks). Keep simple.
- Tile data RAM to SDRAM. Same reason — already fits current scope.

## Phasing

Four steps, each with its own verification gate. After each step:
- Verilator sim for that component passes
- yosys diag completes (< budget)
- Real hardware smoke test via NovaHost if possible

### Step 1 — SDRAM controller instantiation

**Scope**: bring `sdram.v` alive on the ULX3S board.

- Instantiate in `fpga_top.sv`; wire clock, reset, init
- Add ULX3S SDRAM pin constraints in `ulx3s.lpf` (13-bit addr, 16-bit data,
  2 DQM, 2 bank, CS, WE, RAS, CAS)
- Derive SDRAM clock from PLL (100 MHz typical)
- Refresh logic: auto-refresh every 7.8 µs
- 200 µs power-on init sequence: NOP → precharge-all → auto-refresh × 2 →
  mode register set

**Testbench**: new `test_sdram_loopback.sv` — toy write/read at a single
address, verify read data matches write. Verilator model for SDRAM chip
exists in MiST community.

**Hardware smoke test**: XMC-routed write to a known XRAM address, read
back, verify via NovaHost PEEK command.

**Exit criteria**: SDRAM responds correctly to single-address write/read
cycles. Refresh timing meets spec. No setup/hold warnings from nextpnr.

### Step 2 — XRAM migration (BRAM stub → SDRAM)

**Scope**: replace 4KB `xram_inst` dpram with SDRAM-backed XRAM, keeping
the XMC (Expansion Memory Controller) API unchanged at the CPU boundary.

- Route `xram_a_addr / xram_a_din / xram_a_we / xram_a_dout` through an
  SDRAM arbiter port instead of the BRAM
- XMC address already 24-bit → SDRAM 25-bit direct mapping, first 512KB
- Dpram stub removed entirely (free 2 blocks)
- Add "SDRAM busy" back-pressure to XMC: XMC issues request → waits for
  SDRAM ack before advancing state machine

**Testbench**: extend `test_blitter.sv` or new `test_xram_sdram.sv` — XMC
fill, XMC read, CMD_STASH + CMD_FETCH end-to-end in sim.

**Hardware smoke test**: `POKE 0xBA00,1` (XMC cmd GET_BYTE), verify read at
`POKE 0xBA0B`. Then exercise STASH of 1KB CPU→XRAM and FETCH back;
byte-compare.

**Exit criteria**: Existing XMC integration tests pass without modification.
512KB XRAM addressable. No regression in XMC latency-sensitive tests.

### Step 3 — SID tables migration

**Scope**: move `rtl/sid/sid_tables.sv` memory declarations out of synthesis
(no more $readmemh at BRAM level) and into SDRAM-backed storage with a
cache at the audio clock boundary.

SID table inventory:

| Table | Size | Access | Notes |
|---|---|---|---|
| `f6581_curve` | 4K × 16-bit = 8KB | per-sample lookup (44.1 kHz) | currently in BRAM, 4 blocks |
| `wave6581_p_t` | 2K × 8-bit = 2KB | per-sample lookup | absorbed into LUT today per stat |
| `wave6581_ps_` | 2K × 8-bit = 2KB | per-sample lookup | ditto |
| `wave8580_p_t` | 2K × 8-bit = 2KB | per-sample lookup | ditto |
| `wave8580_ps_` | 4K × 8-bit = 4KB | per-sample lookup | ditto |
| `_unused_f6581_adj` | 1K × 15-bit ≈ 2KB | unread — future Fc_offset feature | preserved in SDRAM, free to keep |

Approach:
- All tables share one SDRAM arbiter port
- Access is per-sample (22 µs @ 44.1 kHz); SDRAM random-read latency ~150 ns
  — trivially within budget
- SID sample pipeline issues prefetch at sample start; by the time the
  filter/wave math needs the value, SDRAM has returned it

**NovaHost protocol extension**: add a "SDRAM region stream" message like
ehbasic.bin boot stream, with a region selector byte. Targets:
`SID_6581_CURVE`, `SID_WAVE_*`, `FONT_SLOT_N`, etc. ESP32 side reads source
hex files from SD card or embedded flash.

**Testbench**: new `test_sid_sdram.sv` — load curve values into mock SDRAM,
drive SID voice at known frequency, capture audio samples, verify filter
produces expected output. May share scaffolding with existing `test_sid.sv`.

**Hardware smoke test**: boot with NovaHost streaming tables; play a known
SID tune and A/B against the Avalonia reference.

**Exit criteria**: SID audio passes existing `test_sid.sv` + `test_sid_readback.sv`
suites unchanged. Frees ~4 DP16KD blocks (diag report).

### Step 4 — (Optional) Font migration with scanline cache

**Skip this step if Steps 1-3 already give ≥4 blocks of Phase 2 headroom.**

**Scope**: move `font_mem` to SDRAM, add BRAM scanline cache.

- `font_mem` sized to 8192 bytes (4 font slots × 2048 each) in SDRAM
- 80-byte BRAM "line buffer" caches the current scanline's pre-fetched
  font bytes — one byte per text cell
- Prefetch state machine: during h-blank, walk 80 text columns, read
  char_b_dout[col] to pick glyph, read font[glyph_code][font_line] from
  SDRAM into line_buffer[col]
- At visible pixel time, text render path reads line_buffer[text_col]
  instead of hitting SDRAM directly — maintains 1-cycle-per-pixel access

**Complexity call**: hiding SDRAM latency across 80 prefetches per scanline
requires careful h-blank timing. 640 pixel clock periods of h-blank @
25 MHz = 25.6 µs. 80 reads × 150 ns per SDRAM random access = 12 µs. Fits
with margin, but edge cases (refresh collisions) need explicit handling.

Only do this if Phase 2 can't finish without it.

## Risk Register

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| SDRAM timing closure fails at 100 MHz | Low | High — requires PLL rework | Start at 50 MHz, ramp up after Step 1 works |
| ULX3S SDRAM pin mismatch (v3.0.8 vs v3.1.x labeling) | Medium | Medium — silent read failures | BOARD.md already warns; test with known-good probe pattern |
| Refresh collisions cause sample dropouts | Medium | Medium — audible artifacts | Arbiter prioritizes refresh during h-blank; confirm no refresh during visible scanline in the font path |
| XMC latency regression breaks existing STASH/FETCH tests | Low | Medium | Run blitter + XMC tests after Step 2 |
| NovaHost streaming bottleneck at boot | Low | Low — slower boot | Streams run at UART speed today (~3 sec for ehbasic), adding ~20KB tables = +1 sec |
| SDRAM controller has latent bugs from MiST adaptation | Medium | High — intermittent failures | Start with simple tests; known-good NES projects depend on this core, but read every comment carefully |

## Definition of Done

- Step 1 + Step 2 + Step 3 complete, committed to `main`
- `rtl/sdram/sdram.v` instantiated, passing loopback tests
- XRAM expanded from 4KB → 512KB, XMC tests unchanged
- SID tables in SDRAM, `test_sid.sv` + `test_sid_readback.sv` pass
- yosys diag: DP16KD ≤ 200 (comfortable margin for Phase 2 return)
- Hardware: full system boots via NovaHost stream, BASIC REPL works,
  SID tune plays identically to Avalonia reference
- No regressions in any existing sim suite (Verilator 17 suites green)

Then: resume Phase 2 from commit `c25e02a` for Batches B–E (gfx_mem grow,
fonts, signed sprite coords, REG_BORDER removal).

## Related Memory

- `project_geometry_rewrite_plan.md` — Phase 2 state + pivot decision
- `project_multi_env_vision.md` — 512KB XRAM unlock lines up with this
- `project_ulx3s_board.md` — board version/pinout caveats
- `project_sid_fpga_implementation.md` — SID adaptation notes
- `feedback_timing_critical_in_bram.md` — allocation principle guiding moves
- `feedback_ecp5_dualport_bram_aspect.md` — why repacking didn't help
- `feedback_one_change_at_a_time.md` — hence the 4-step phasing
- `feedback_rtl_discipline.md` — 99.99% sure before 17-min synth; diag between steps

## Closure — 2026-04-22 PM

Phase 2.5 closed after Steps 1 + 2. Summary:

| Step | Status | Commit | DP16KD delta | Notes |
|---|---|---|---|---|
| 1 | SHIPPED | `e6c176d` + `f891dc7` | +0 | sdram.v instantiated idle; clkref 16:1 fix landed as follow-up |
| 2 | SHIPPED | `f28df2b` | −4 | xram 4KB BRAM → 512KB SDRAM via `xram_sdram.sv` |
| 3 | DEFERRED | — | would be −4 | Full migration needs NovaHost firmware extension (ESP32 streams `f6581_curve.hex` to SDRAM at boot via new `POKE_SDRAM` debug_bridge command). Right sequencing is to do the FPGA + ESP32 halves together, not bolted onto an RTL-only session. |
| 4 | SKIPPED | — | — | Plan's own criterion: skip if headroom ≥ 4. We're at 10 blocks after Step 2 (198/208). |

Current budget:

- DP16KD: **198 / 208** — 10 blocks free
- Phase 2 Batch B–E planned growth: **+8 blocks** (gfx_mem for 320×240 full-screen, font_mem for 3-slot)
- Expected post-Phase 2 state: **206 / 208** = 2 blocks margin

2-block margin is tight but not broken. If Phase 2 ultimately overruns, Step 3 (SID tables → SDRAM) is the escape hatch — 4 more blocks available when NovaHost firmware is extended.

Known follow-ups from Step 2 (track outside this plan):
- `blitter.sv` doesn't yet respect `xram_busy` — blitter XRAM reads sample stale data on hardware. Writes are fire-and-forget and work today.
- Step 3 full migration: ESP32 firmware + FPGA `POKE_SDRAM` path. Own phase when tackled.

## Historical Context

Earlier in this same session, the "repack gfx_mem WIDTH=4 → WIDTH=8" trick
was proposed and empirically falsified (DP16KD dual-port always uses 1K×18
aspect regardless of WIDTH). SDRAM was then raised as a fallback but
deferred. When Phase 2 Batch A confirmed budget still tight after the
tested RTL changes, user chose to pivot rather than patch with piecemeal
tricks. This plan is the result.
