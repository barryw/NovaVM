# Geometry Rewrite — 80×60 Text + 320×240 Gfx (pixel-doubled), Full 480-Line Screen

**Status**: Revised 2026-04-22 PM after Phase 1 measurement.
Original goal of 640×480-native gfx doesn't fit ECP5-85 BRAM; scope revised
to 320×240 gfx (pixel-doubled to 640×480) with 80×60 native text.
Execution begins at Phase 2.

## Objective

Fix the inconsistent coordinate system (25-row text on a 400-pixel band, 320×200
pixel-doubled graphics, 80+80 pixel border bars) and use every scanline of the
480-line display.

**Target coordinate spaces:**

| Layer | Logical space | Display mapping | BASIC coord range |
|---|---|---|---|
| Text | 80 × 60 chars @ 8×8 cells | Native 640 × 480 | `x ∈ [0,79]`, `y ∈ [0,59]` |
| Graphics | 320 × 240 @ 4bpp | Pixel-doubled to 640 × 480 | `x ∈ [0,319]`, `y ∈ [0,239]` |
| Sprites | 16 × 16 cells at gfx resolution | Composited with gfx, doubled | Signed X,Y; visible ≈ `[-16, 319+16]` × `[-16, 239+16]` |
| Tiles 8×8 | 40 × 30 visible at 320 × 240 gfx | Composited with gfx, doubled | unchanged |
| Tiles 16×16 | 20 × 15 visible at 320 × 240 gfx | Composited with gfx, doubled | unchanged |

**Key invariants:**

- Display: full 640 × 480 @ 60 Hz VGA. **No borders.**
- `REG_BORDER` at `$A00D`: removed (reclaimable for future use).
- Text is drawn at native 640 × 480. Graphics and sprites share the 320 × 240
  logical space and composite pixel-doubled onto the same 640 × 480 screen.
- User-facing coordinates in BASIC stay in each layer's native space: text
  commands use 80 × 60, gfx commands use 320 × 240. Cleaner than a fake-640
  scaling layer.

## Why This Scope (Not Full Native 640×480 Gfx)

Earlier drafts of this plan targeted **640 × 480 native graphics at 4bpp**.
Phase 1 measurement on 2026-04-22 PM established that this does not fit
ECP5-85 BRAM, regardless of aspect-ratio tricks:

- Baseline: **194 of 208 DP16KD used (93%)** — only 14 blocks headroom.
- 640 × 480 × 4bpp at WIDTH=4 needs ~300 blocks; at WIDTH=8 packed needs
  ~150 blocks. Both wildly infeasible.
- **Root cause:** ECP5 DP16KD true dual-port mode always uses 1K×18 aspect
  regardless of declared WIDTH. A repack from `WIDTH(4) DEPTH(64000)` to
  `WIDTH(8) DEPTH(32000)` saves **zero** blocks (confirmed by yosys diag).
- Moving tile_data (32), sid_tables (4), and main_ram dual-ref (16) to
  SDRAM frees ~52 blocks — not enough. And moving gfx_mem itself to SDRAM
  works but adds ~1 phase of arbiter/line-buffer RTL work.

Decided: **stay at 320 × 240 gfx** (current storage geometry, just without
the 40-line top/bottom border), pixel-doubled on output. All the text-mode
wins (60-row screen, no border, consistent display usage) still land. Gfx
stays where it is in coordinate space, just without the border crop.

## User Quotes Driving This Decision

> "our line drawing commands see a max y of 199 even though the screen is
> currently 480px.. i think we need to make this consistent.. all of our
> graphics and text need to have the same understanding of the geometry
> which right now is inconsistent."

> "i know this is a big fucking change, but i think i really do want a full
> 640x480 display.. 80x60 text mode.. 640x480 graphics for drawing, tiles,
> sprites.. everything.. i think this maps cleanly and i get that it breaks
> a lot of shit and is a lot of work.. lets do this…"

> "lets do it in avalonia first and should be enough to make a go/no-go
> decision." (on the 8×8 font legibility preview)

> "i like the 80x60.. any chance we can generate images using the other fonts?
> we should have 3 fonts: the cp437 and the 2 c64 fonts (upper + symbols,
> upper + lower). i know these work in avalonia, but they may need to be
> added to verilator/fpga"

> "i want all of the timing critical shit in bram" (on BRAM/SDRAM allocation)

> "ok.. lets do 320x240 @ 4bpp for graphics.. keep 80x60 text" (this scope)

> "a feels misleading.. actually both do lol.. but i think b is less
> misleading and makes for a simpler implementation" (chose BASIC coords
> in 320×240 space over scaled 640×480)

No NovaBASIC programs in the wild yet, so breaking changes are acceptable.

## BRAM Budget (Measured)

Baseline from `make diag` 2026-04-22 PM: 194 of 208 DP16KD blocks (93.3%).
All numbers below use the empirical formula `blocks = ceil(DEPTH / 2048)`
for our dpram dual-port configuration (verified against observed counts).

| Change | Depth before | Depth after | Blocks before | Blocks after | Δ |
|---|---|---|---|---|---|
| `char_mem` | 2000 | **4800** | 2 | 3 | +1 |
| `color_mem` | 2000 | **4800** | 2 | 3 | +1 |
| `gfx_mem` (320×200 → 320×240 @ 4bpp) | 64000 | **76800** | 32 | 38 | +6 |
| `font_mem` (1 font → 3 fonts) | 2048 | **8192** | 2 | 4 | +2 |
| `tile nametable_ram` | 4096 | 4096 | 2 | 2 | 0 |
| `tile attr_table_ram` | 4096 | 4096 | 2 | 2 | 0 |
| Sprite coord widening (registers, no BRAM) | — | — | — | — | 0 |
| `REG_BORDER` removal | — | — | — | — | 0 |
| **Total** |   |   |   |   | **+10** |

Projected total: **204 / 208 blocks (98%)** — fits with 4 blocks spare.

## Affected Components

### RTL (`e6502.FPGA/rtl/`)

1. **`vgc.sv`** — ROWS constant, sprite X/Y register fields (all 4 bytes
   wired, signed-16 internal), cmd param widths for signed sprite ops,
   `REG_BORDER` removal, CPU-side addr propagation for wider char/color mem.
2. **`vgc_timing.sv`** — constants:
   - `ROWS` 25 → 60
   - `TEXT_H` 400 → 480
   - `V_BORDER` 40 → 0
   - `text_row` [4:0] → [5:0] (0..63 valid to cover 0..59)
   - `text_line` no more `>> 1` pixel-doubling
   - `real_row` mod-60 fixup (not mod-25). `scroll_offset` input 5b → 6b.
   - `gfx_x` stays [8:0] (still 0..319), `gfx_y` stays [7:0] (still 0..239).
   - `pre_gfx_x = h_count[9:1]`, `pre_gfx_y = v_count[9:1]` (drop `V_BORDER`
     subtract; it's now 0).
   - All d1/d2 pipeline delay registers track the new `text_row` width.
3. **`vgc_text.sv`** — `char_mem` and `color_mem` DEPTH 2000 → 4800. Local
   `char_b_addr`, `color_b_addr` widen [10:0] → [12:0]. Port inputs
   `char_a_addr`, `color_a_addr` widen the same and propagate up through
   `vgc.sv`. `real_row` input 5b → 6b.
4. **`vgc_gfx.sv`** — `gfx_mem` DEPTH 64000 → 76800. Port A addr stays [15:0]
   (16-bit covers 76800 ≤ 65536 × 2). Port B address math unchanged (x + y×320).
5. **`vgc_sprites.sv`** — sprite X, Y become signed 16-bit. Scanline
   eligibility test uses signed compare. Sprite raster stays at 320×240
   resolution; screen-edge clipping at [0, 319] × [0, 239] internal.
6. **`artist.sv`** — unchanged. Coord widths, clipping, and primitives all
   stay in 320×240 logical space.
7. **`blitter.sv`** — gfx-space size bump (64000 → 76800) in the VGC space
   map. Stride math stays 320. Mostly untouched.
8. **`tile_engine.sv`** — unchanged. 40×30 visible at 320×240 gfx is what it
   already does.
9. **`rtl/dpram.sv`** — no change; depths are parameters.

### Font ROMs (`e6502.FPGA/rom/`)

Add two more font hex files so the FPGA has parity with Avalonia's 3-slot
font support (`cp437`, `petscii_upper`, `petscii_lower`):

- `cp437.hex` (exists) — slot 0
- **new:** `petscii_upper.hex` — slot 1
- **new:** `petscii_lower.hex` — slot 2

Source binaries live in `e6502.Avalonia/Resources/petscii_{upper,lower}.bin`.
Hex generation: same pipeline as `cp437.hex`. `font_mem` DEPTH 2048 → 8192
(4× capacity, room for future slot 3).

### Firmware (`ehbasic/`)

- `ROWS = 25` → `ROWS = 60` pervasive updates in `basic.asm`:
  - cursor position clamp / wrap
  - scroll paths
  - CLS (fill 4800 bytes, not 2000)
  - print / line-editor row math
- `GFX_H = 200` → `GFX_H = 240` in coord range checks
- `GFX_W = 320` unchanged
- `SPRITESET` — accept signed word for X *and* Y (currently X word, Y byte)
- `BORDER` command → no-op (or remove), no BRAM effect

### Avalonia

Mostly already correct. Sanity pass to confirm:
- `VgcConstants.ScreenRows` 25 → 60
- `VgcConstants.GfxHeight` 200 → 240
- Renderer uses full 480 lines (remove text/gfx border math, `NativeHeight`
  640×400 → 640×480)
- Confirm signed-16 sprite coords match new firmware writes
- No `REG_BORDER` access remaining

### Tests

| Test | Type | Change |
|---|---|---|
| `test_vgc_text.sv` | sim | ROWS=60, 80×60 layout, 4800-byte buffers |
| `test_vgc_gfx.sv` | sim | gfx_mem bounds 0..76799 |
| `test_vgc_sprites.sv` | sim | signed coords, off-screen negative-Y clipping |
| `test_vgc_regs.sv` | sim | `REG_BORDER` removal |
| `test_vgc_scroll_wrap.sv` | sim | ROWS=60, scroll_offset 6-bit |
| `test_vgc_frame_counter.sv` | sim | unchanged |
| `test_copper` / `test_copper_lists` | sim | Y coord in copper entries |
| `test_tile_engine.sv` | sim | 40×30 at 320×240 gfx (unchanged) |
| Integration `text.6502` | HW | cursor 60 rows |
| Integration `vgc.6502` | HW | coord ranges, no REG_BORDER |
| Integration `sprites.6502` | HW | sprite reaches all four visible edges of 320×240 gfx |
| Integration `tiles.6502` | HW | tiles still work at 320×240 |

## Phasing

Each phase ends with:
- `make` green on Verilator sim suite
- `dotnet test` green on Avalonia
- yosys diag under BRAM budget (204/208 target)
- hardware integration green

No phase merges to `main` until all four are green.

### Phase 1 — ✅ DONE

Measurements:
1. Baseline BRAM confirmed 194/208 (93%)
2. 8×8 1:1 legibility preview across all 3 fonts — user: *"i like the 80x60"*
3. Repack-width hypothesis falsified (ECP5 dual-port doesn't care about WIDTH)

### Phase 2 — RTL geometry change, firmware untouched

Widen RTL to 80×60 text + 320×240 full-screen gfx + signed-16 sprite coords.
Firmware still thinks it's 25-row, so top 25 rows behave as before and rows
25..59 are unused but addressable (render correctly when written to).

Sim tests updated in lockstep. Phase ends with working FPGA display that
still runs old firmware correctly on the top portion and shows a clean
480-line display (no borders).

**Order within Phase 2:**
1. `vgc_timing.sv` — constants + coord widths. Run sim.
2. `vgc_text.sv` + callers in `vgc.sv` — char/color mem widening. Run sim.
3. `vgc_gfx.sv` + callers in `vgc.sv` — gfx_mem DEPTH 76800. Run sim.
4. `vgc_sprites.sv` + callers — signed-16 X, Y, field 1 (XHi) and 3 (YHi)
   fully wired. Run sim.
5. `vgc.sv` — `REG_BORDER` removal, sprite reg wiring. Run full sim + yosys
   diag under 204 budget.

### Phase 3 — Firmware (ehbasic) update

Update `basic.asm` to `ROWS=60`, `GFX_H=240`, signed-word sprite coord parsing,
`BORDER` no-op. Rerun integration suite — expect all previously passing tests
to still pass, plus 3–5 new cases exercising the new coords.

### Phase 4 — Font parity (3 fonts in FPGA)

- Generate `rom/petscii_upper.hex` and `rom/petscii_lower.hex`
- `font_mem` DEPTH 2048 → 8192
- Font slot select wiring in `vgc.sv` (may already be present; confirm)
- Integration tests covering font switch

### Phase 5 — Integration test expansion

New tests that could only exist in the new geometry:
- 60-row full-screen scroll patterns
- Sprite sliding in from off-screen negative X/Y
- Full-screen 320×240 gfx primitives
- 3-font rendering regression

## Open Questions Still To Resolve

1. **Sprite signed comparison on Yosys** — write targeted sim test first to
   confirm `signed [15:0]` compare synthesizes correctly.
2. **Graphics primitive clipping policy** — out-of-range coords: clamp, wrap,
   or error? Probably clamp (what most games do). Needs explicit decision in
   Phase 3.
3. **BASIC line editor behavior on 60-row screen** — scan `basic.asm` for
   hidden dependencies on row count 25 beyond the obvious cursor/scroll paths.
4. **REG_BORDER slot reuse** — keep as no-op for source compat, or reclaim
   `$A00D` for something new? User hasn't said.

## Risk Register

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| BRAM budget exceeded after all Phase 2 changes | Low | High — rework | Re-run diag after each file change; target ≤205 blocks |
| Signed sprite compare synthesizes wrong on Yosys | Low | Medium — visual bug | Targeted sim test before HW |
| Firmware `ROWS=60` changes miss edge case | Medium | Medium | Phase 3 integration tests; manual exercise of each prompt path |
| Font parity triggers load-time cost | Low | Low | Font ROM is loaded from ESP32 same as BASIC ROM |
| Breaks existing integration tests | Certain | — | Tests updated in lockstep |

## Definition of Done

- All phases 2–5 complete, committed, merged to `main`
- Verilator suite: 100% green
- Avalonia suite: 100% green
- yosys diag: DP16KD ≤ 205/208 (≥3 blocks spare for future work)
- Hardware integration: all previously-passing suites still pass
- Live hardware: type 60 lines at REPL, full 480-line text area used, no
  border artifacts, 3 fonts switchable
- `PLOT 319, 239` works; `PLOT 320, 240` clamps or errors per Q2 above
- Sprite at `x=-16, y=-16` correctly clipped at left/top edges
- Sprite at `x=319+16, y=239+16` correctly clipped at right/bottom edges
- No `$A00D` accesses in firmware or tests

## Known-Good Starting Bitstream

`e6502.FPGA/fpga/build/e6502.mirror.bit` — has FIFO + cpu_ce gate + $A008 +
7-row-mirror fixes. Baseline for Phase 2 work.

## Historical Context

- **Original plan (superseded)**: 640×480-native gfx @ 4bpp. Killed by ECP5-85
  BRAM budget (would have needed ~300 blocks for gfx_mem alone).
- **Middle trade considered**: repack `gfx_mem` WIDTH=4 → WIDTH=8 to save
  blocks. Empirically confirmed useless on ECP5 DP16KD dual-port mode.
- **Final scope**: 80×60 text native + 320×240 gfx pixel-doubled + 3 fonts +
  signed-16 sprite coords. Fits 204/208 BRAM (98%). No SDRAM controller
  needed.

## Related Memory

- `project_geometry_rewrite_plan.md` — session notes + user quotes
- `feedback_ecp5_dualport_bram_aspect.md` — why the repack trick doesn't work
- `feedback_measure_bram_before_planning.md` — always diag first
- `feedback_timing_critical_in_bram.md` — BRAM/SDRAM allocation principle
- `feedback_yosys_diag_for_bram_fit.md` — `make diag` vs full synth
- `feedback_one_change_at_a_time.md` — hence the phased RTL-file-by-file order
- `feedback_tests_prove_fixes.md` — every Phase-2 fix needs a bidirectional test
