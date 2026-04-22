# Geometry Rewrite — 640×480 Native Across the Board

**Status**: Planning handoff, drafted 2026-04-22. Execution starts in next session.

## Objective

Replace the current inconsistent coordinate system (25-row text on a 400-pixel text band, 320×200 pixel-doubled graphics, 80+80 pixel border bars) with a single unified 640×480 native coordinate space.

All of these will share the same coordinate system and maximum extents (`x ∈ [0, 639]`, `y ∈ [0, 479]`):

- Text mode: 80 × 60 rows, 8×8 cells, no pixel doubling
- Graphics mode: 640 × 480 pixels, 4 bits per pixel
- Tile engine: nametables sized to 80×60 (8×8 tiles) or 40×30 (16×16 tiles)
- Sprite positioning: signed 16-bit X and Y, full screen + off-screen margins addressable
- Line/plot/rect/fill/paint/circle/gtext: max x=639, max y=479
- Display: full 640×480 @ 60Hz VGA, no borders at all

The `REG_BORDER` register at `$A00D` is removed; there is no border color because there is no border.

## Why This Now

Quoting the user at the top of the decision:

> "our line drawing commands see a max y of 199 even though the screen is currently 480px.. i think we need to make this consistent.. all of our graphics and text need to have the same understanding of the geometry which right now is inconsistent."

> "i know this is a big fucking change, but i think i really do want a full 640x480 display.. 80x60 text mode.. 640x480 graphics for drawing, tiles, sprites.. everything.. i think this maps cleanly and i get that it breaks a lot of shit and is a lot of work.. lets do this..."

> "lets chat about this because right now what we have is a bit of a mess and we have a chance to fix it before real software gets written for it"

No NovaBASIC programs in the wild yet, so this is the moment to land breaking changes before user code ossifies them.

## Current State (Inventory)

| System | Coordinate Space | Memory | Where |
|---|---|---|---|
| Text | 80×25 chars, 8×16 displayed (8×8 doubled) | char_mem 2000, color_mem 2000 | `rtl/vgc_text.sv`, `rtl/vgc.sv` |
| Graphics | 320×200, 4bpp, pixel-doubled to 640×400 | gfx_mem 32000 (320×200÷2) | `rtl/vgc_gfx.sv`, `rtl/artist.sv` |
| Tiles 8×8 | 40×25 nametable, 200×320 visible area | NT 1000B × 4 = 4000B, tile RAM 8192B | `rtl/tile_engine.sv` |
| Tiles 16×16 | 20×12-ish | tile RAM 32768B | `rtl/tile_engine.sv` |
| Sprites | 9-bit unsigned X (0..511), 8-bit unsigned Y (0..255), 16×16 cells | 16 sprites × shape 128B + 256 shape slots × 128B = 34816B | `rtl/vgc_sprites.sv`, `rtl/vgc.sv` |
| Border | 40 lines top + 40 lines bottom, `REG_BORDER` | none | `rtl/vgc.sv` line 1469-1470 |
| Text firmware | `ROWS=25`, hard-coded in basic.asm | — | `ehbasic/basic.asm` |
| Gfx firmware | `GFX_W=320`, `GFX_H=200`, coordinate parsers | — | `ehbasic/basic.asm`, `ehbasic/extension.s` |

## Target State (The Math)

### Memory growth

| Buffer | Old | New | Delta |
|---|---|---|---|
| char_mem | 2000 | 80 × 60 = **4800** | +2800 |
| color_mem | 2000 | 80 × 60 = **4800** | +2800 |
| gfx_mem | 32000 (320×200÷2) | 640 × 480 ÷ 2 = **153600** | +121600 |
| font_mem | 2048 | unchanged (still 8×8 CP437) | 0 |
| Tile 8×8 NT | 1000B × 4 = 4000 | 80 × 60 × 4 = **19200** | +15200 |
| Tile 16×16 NT | 300B × 4 = 1200 | 40 × 30 × 4 = **4800** | +3600 |
| Tile RAM 8×8 | 8192 | unchanged (256 tiles × 32B) | 0 |
| Tile RAM 16×16 | 32768 | unchanged | 0 |
| Sprite shape RAM | 34816 | unchanged | 0 |

**Total VGC BRAM growth: ~145 KB.** ECP5-85 has 468 KB BRAM total; current allocation (main RAM 64KB dpram + ROMs + sprite RAM + etc.) leaves healthy headroom. **Pre-work #1 below confirms this fits before we commit.**

### Coordinate register widths

| Register | Old | New |
|---|---|---|
| Sprite X (fields 0,1) | 9-bit unsigned (XLo + 1 bit of XHi) | **signed 16-bit** (XLo + XHi both full bytes) |
| Sprite Y (fields 2,3) | 8-bit unsigned (YLo only; YHi dropped) | **signed 16-bit** (YLo + YHi both full bytes) |
| Plot X (cmd params) | 9-bit (0..511) | 10-bit (0..639) or pad to 16-bit for alignment |
| Plot Y (cmd params) | 8-bit (0..255) via reg pair | 9-bit (0..479) or pad to 16-bit |
| Tile scroll X | 10-bit | unchanged (still 0..639) |
| Tile scroll Y | 9-bit | unchanged (still 0..479) |

### VGA timing — no change

640×480 @ 60Hz, 25.175 MHz pixel clock, standard VESA timing. Monitor compatibility unaffected because we're already at 640×480. The PLL, `vgc_timing.sv` H/V counters, and HDMI serializer all stay exactly as they are.

What changes inside `vgc_timing.sv`:
- `V_BORDER` constant → 0 (or delete)
- `TEXT_H` → 480 (was 400)
- `text_row = text_line[7:3]` → `text_line[9:3]` (need 6-bit row index, 0..63)
- `scroll_offset` becomes `[5:0]` (0..59), fixup against `ROWS=60`
- Pipeline delays unchanged

## Affected Components

### RTL (e6502.FPGA/rtl/)

1. `vgc.sv` — ROWS constant, char/color_mem depths, sprite X/Y types, cmd param widths, `REG_BORDER` removal
2. `vgc_timing.sv` — V_BORDER→0, text_row width, real_row arithmetic (already widened by `00a6365`, just re-tune)
3. `vgc_text.sv` — char_mem and color_mem depth parameters
4. `vgc_gfx.sv` — gfx_mem depth 32000→153600, coordinate widths on artist interface
5. `artist.sv` — line/circle/rect/fill/paint/plot coordinate widths and clipping
6. `vgc_sprites.sv` — signed coord storage, scanline eval comparison, screen edge clipping
7. `tile_engine.sv` — nametable depth, scroll wrapping math
8. `blitter.sv` — VGC space size in the gfx branch (may also need stride widening)
9. `rtl/dpram.sv` — no change; depth is a parameter

### Firmware (ehbasic/)

Grep for the constants and update:
- `ROWS`, `LAB_ROWS`, `25`-literal in scroll/cursor/CLS/print/line-editor paths
- `GFX_W`, `GFX_H`, `319`, `199`, `200`, `320` literals in line/plot/rect/circle/fill paint/gtext operand parsing and range checks
- `$A00D` writes (BORDER command) — keep the command for source compat, make it a no-op
- SPRITESET parameter parsing needs to accept signed word for X and now for Y too (current path handles word for field 0, byte for field 2 — needs symmetry)
- Gtext coord range checks

### Tests

| Test | Type | Impact |
|---|---|---|
| `test_vgc_text.sv` | sim | ROWS=60 assertions throughout |
| `test_vgc_gfx.sv` | sim | 640×480 coord space, gfx_mem depth |
| `test_vgc_sprites.sv` | sim | signed coords, off-screen clipping |
| `test_vgc_regs.sv` | sim | REG_BORDER removal, possibly re-route that slot |
| `test_vgc_scroll_wrap.sv` | sim | ROWS=60, scroll_offset width 6-bit |
| `test_vgc_frame_counter.sv` | sim | unchanged |
| `test_copper` / `test_copper_lists` | sim | V coordinate in copper entries |
| `test_tile_engine.sv` | sim | 80×60 nametable |
| `test_artist.sv` if exists | sim | 640×480 bounds |
| Integration `text.6502` | HW | cursor-at-row tests need 60 not 25 |
| Integration `vgc.6502` | HW | coord ranges, mode-register reads |
| Integration `sprites.6502` | HW | sprite reaches all four edges |
| Integration `tiles.6502` | HW | 80×60 or 40×30 depending on tile size |

Avalonia is **already correct**. The only Avalonia work is a sanity pass to confirm it still matches the new firmware (ROWS=60, signed sprite coords, no REG_BORDER). Most of it matches today because Avalonia already renders to fit the text area exactly and already uses signed 16-bit sprite coords.

## Pre-Work (Measurements Before Writing Any Code)

These answer the hard-constraint questions up front so we don't discover a blocker halfway through:

1. **Confirm BRAM fits.** Update `dpram` instances to the new depths in a branch, synthesize with `yosys -p "read_verilog ...; synth_ecp5"`, and check `nextpnr-ecp5` utilization report. Looking for BRAM usage < 95% with everything included. Report as pass/fail before touching firmware.
2. **Confirm rendering pipeline bandwidth.** With `TEXT_H=480`, char_mem reads happen at 8× pixel clock per row (80 cells / 16 px per cell actually — recheck). Make sure dpram read-port hand-off still aligns with the pipeline d2 delay registers.
3. **Confirm 8×8 font readability at 1:1.** No more pixel doubling. On a real monitor this will look half the size. User may want to reconsider if the result is too small to read comfortably — check a screenshot before committing to the scheme.

## Proposed Phasing

Each phase ends with `make` green on Verilator + `dotnet test` green on Avalonia + hardware synth + integration run. No phase merges to `main` until all three are green.

### Phase 1 — Measurements + sanity check
Pre-work items 1 and 2 above. Build a throwaway branch just to run synth with expanded buffer sizes and prove it fits. Screenshot the 8×8 text font on actual hardware to judge legibility.

### Phase 2 — RTL geometry change, firmware untouched
Widen all RTL to 80×60 text / 640×480 gfx / signed-16 sprite coords. Firmware still thinks it's 25-row, so the top 25 rows behave as before and the bottom 35 are unused. Sim tests updated to match. Phase ends with working FPGA display that still runs old firmware correctly on the top portion.

### Phase 3 — Firmware update
Update EhBASIC to `ROWS=60`, signed sprite params, 640×480 coord range in commands. Re-run integration suite; expect all previously-working tests to still pass, plus 3–5 new cases that exercise the new coords.

### Phase 4 — REG_BORDER cleanup
Remove the register from cpu_rdata/dbg_rdata paths. BORDER command in basic.asm becomes a no-op (or gets removed). This is the last step because it's a breaking change for any test that pokes/peeks `$A00D`.

### Phase 5 — Expand integration tests
New tests that could only exist in the 80×60 world (full-screen scroll patterns, sprite off-screen motion, 640×480 full-screen graphics primitives). Close out the audit's remaining "real bug" list (DMA, BLITCOPY, XRAM STASH/FETCH, GSAVE/GLOAD, SID-register-specific reads, math array-operations) on top of the new geometry.

## Open Questions To Resolve In Planning Session

1. **Tile engine 16×16 tile count at new resolution**. 40×30 nametable × 4 tables is 4800 entries. Is 40×30 the right target or should it be scaled differently?
2. **Font legibility at 8×8 1:1 on typical monitors**. If it's unreadable, do we switch to a bigger font (9×16, 8×16 without doubling) or accept it? Pre-work #3 measures this.
3. **REG_BORDER register slot reuse**. Keep it as a no-op for source compat, or reclaim `$A00D` for something new?
4. **SPRITESET symmetric Y parsing**. Today field 2 accepts byte, field 0 accepts word. User-facing BASIC `SPRITE n,x,y` always sends a word for both? Confirm before changing.
5. **Sprite eval when Y is off-screen negative**. Currently `spr_next_scanline >= spr_y` is unsigned; with signed Y = -16, "is scanline 0 inside sprite at y=-16?" means checking `0 >= -16 && 0 < 0`. Need to verify the signed comparison synthesizes correctly on Yosys.
6. **Graphics primitive clipping policy**. Out-of-range coords: clamp, wrap, or error? (Probably clamp — what most games do.)
7. **BASIC line editor behavior on 60-row screen**. Current editor has hard-coded scroll behavior tied to 25 rows. Any hidden dependencies on the specific row count?

## Risk Register

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| gfx_mem 150KB pushes nextpnr over BRAM budget | Medium | High — would force pixel-doubled fallback | Pre-work #1 measures before we commit |
| 8×8 font too small to read on hardware | Medium | High — UX regression | Pre-work #3 screenshots before committing |
| Rendering pipeline bandwidth insufficient at higher row count | Low | High — visible glitches | Pre-work #2 verifies pipeline alignment |
| Firmware coord-range updates miss an edge case | High | Medium | Phase 3 integration tests catch most; manual testing covers rest |
| Sprite signed comparison synthesizes wrong on Yosys | Low | Medium | Write a targeted sim test first |
| Breaks existing integration tests | Certain | — | Tests updated in lockstep; no test skipped |

## Definition of Done

- All phases 1-4 complete, committed, and merged to `main`.
- Verilator suite: 100% green with the new test assertions.
- Avalonia suite: 100% green (no changes expected; confirming no regression).
- Hardware integration suite: all suites that previously passed still pass; sprites that used to only reach 511 now reach 639; sprites at negative X correctly clip at the left edge.
- Live hardware test: type 60 lines at REPL, verify the full 480-line text area is used with no mirror artifacts (7-row mirror is already fixed but this confirms on the new geometry).
- No REG_BORDER accesses remain in firmware or tests.
- User-facing programs can now use `PLOT 639,479` without error.

## Known-Good Starting Bitstream

`e6502.FPGA/fpga/build/e6502.mirror.bit` — has FIFO + cpu_ce gate + $A008 + 7-row-mirror fixes. This is the baseline the rewrite starts from.

## Related Memory

- `project_geometry_rewrite_plan.md` — Decision log, locked decision to go full 640×480
- `project_hw_integration_test_audit.md` — Audit findings, what the test suites surface today
- `feedback_tests_prove_fixes.md` — Every fix needs a bidirectional test (applies here too)
- `feedback_one_change_at_a_time.md` — Hence the phased approach above
- `feedback_correct_fixes_not_workarounds.md` — Why we're not pixel-doubling as a middle-ground
