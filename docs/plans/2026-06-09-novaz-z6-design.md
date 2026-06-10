# NovaZ — Z-machine Version 6 (graphical Infocom games)

Date: 2026-06-09
Goal: run the four Infocom V6 games — **Zork Zero** (first target), Arthur,
Shogun, Journey — as mount-and-run `.ndi` images with pictures on the VGC
gfx layer. Avalonia POC first, hardware after, same as the sound work.

## Background

V6 is the graphical Z-machine. NovaZ today is a solid V5-era interpreter:
`zstory_validate_version` rejects version ≥ 6, and the V6 graphics EXT
opcodes (5–8) plus the V6 window EXT block (16–29) are absent or
`zvm_unsupported`. V6 changes three big things:

1. **Packed addresses**: routine addr = `4P + 8×R_O` (header `$28`), string
   addr = `4P + 8×S_O` (header `$2A`). Initial PC (`$06`) becomes a packed
   *routine* called at startup; `quit` when it returns. Stories grow to
   269–341 KB — all four fit the 512 KB XRAM page cache.
2. **Screen model**: the two-window model is replaced by **8 windows in
   units (= text cells, see Coordinate mapping)**, each with 16 properties
   (position, size, cursor, margins,
   newline-interrupt routine, style, colour, font, attributes, line count).
   No interpreter status line — games draw their own chrome.
3. **Pictures**: `draw_picture` / `picture_data` / `erase_picture` /
   `picture_table`, with art shipped outside the story file. Our source is
   the Blorb files already next to each `.z6` in the archive (PNG, MCGA-era
   320×200 art).

Only these four games exist; every mainstream interpreter carries per-game
quirks for them. That is expected here too and is what the per-project
longplay harness is for.

## Coordinate mapping — units are cells

*(Amended for M2: an earlier revision declared 1 unit = 1 gfx pixel with a
4×4 font. The Zork Zero capture — finding 1 in
`2026-06-10-zork-zero-window-ops.md` — proved games trust the header
metrics and send cell coordinates, so units are cells.)*

**1 V6 unit = 1 text cell.** NovaZ declares:

- screen size `80×50` units (header `$22/$24`), 80×50 chars (`$20/$21`),
- font width = font height = **1 unit** (`$26/$27` — V5/V6 swap these two
  bytes, but at 1×1 the swap is harmless).

The game computes window layout from these metrics and sends arbitrary cell
values (`set_cursor 2,9`, `window_size 0,45,70`); the segment does direct
cell math everywhere — window-relative 1-based unit ↔ absolute 0-based cell
is `origin + rel − 2` — and coordinates, sizes, margins (`set_margins`,
window props 6/7) and scroll amounts are exact cell counts with no
rounding. Prop values round-trip exactly through `put_wind_prop` /
`get_wind_prop` / `get_cursor`.

Pictures stay in native gfx pixels: the VGC gfx layer is 320×200 and one
text cell = exactly 4×4 gfx pixels, so M3 converts cells → gfx pixels (×4,
exact) at the picture-blit boundary only.

Header capability bits: interpreter number 6 (IBM PC, matches the MCGA art),
Flags1 advertises pictures/bold/fixed/timed-input, Flags2 mouse and menu
bits are **cleared** — all four games shipped keyboard-playable, and
`make_menu`/`read_mouse` simply fail cleanly. Mouse support is a possible
later addition (Avalonia has a pointer; hardware does not).

## ROM budget — V6 lives in RAM

The 16 K runtime ROM had ~1.7 KB free at branch time (`runtime.map`:
code+rodata ended at `$F924`; after M1's ROM-side additions the end is
`$FB1A`, ~1.2 KB free). The V6 window model + 18 EXT opcodes + picture path
will not fit.

Decision: a **RAM-resident V6 segment**. BSS ends at `$1866`; the launcher
loads `NOVAZ6.BIN` at `$2000` (≈40 KB free to `$BFFF`, of which V6 needs
single-digit KB). The ROM keeps the dispatch tables and a version check:
on a V6 story it requires the segment's magic word at `$2000`, else aborts
with a clear message. ROM-side EXT table entries for V6 opcodes become
trampolines into the segment. Non-V6 stories never touch it.

## Packaging — pictures pre-converted in the packer

Same philosophy as `SOUND.PAK`: the 6502 never decodes anything.

`Nova.NovaZ.Packer` grows `--pictures <file.blb>` (and `PROJECT_PICTURES`
in `project.mk`): reuse `BlorbReader` to walk `RIdx` for `Pict` resources,
decode each PNG host-side, quantize to the VGC **EGA palette** (the VGC
already has `RegPaletteMode` / `PaletteModeEga` — V6 is exactly what it's
for), and emit a flat `PICS.PAK` on the `.ndi`:

```
PICS.PAK
  "NZPK", version(1), count(2, LE), release(2, LE)
  index[count]: z_number(2)  width(2)  height(2)  flags(1)  offset(4)  len(4)   (LE)
  data: concatenated 4bpp row-packed bitmaps, blitter-ready
```

`flags` bit 0 = has transparency; transparent pixels use a reserved index
and are drawn via the blitter's transparent-skip mode (fallback: pre-mask
in the packer if a picture's background is known). The index (count ≈ 400
for Zork Zero, ~5 KB) is loaded into XRAM at boot so `picture_data` answers
without touching the floppy.

## Components

### 1. `zstory.s` — version gate + addressing (small)
- Accept version 6; read `$28`/`$2A` offsets into BSS.
- `zstory` packed-address helpers gain the `+8×offset` term (24-bit math
  already exists for flat XRAM addressing).
- Startup: call the main routine instead of jumping (reuses the normal
  `call` frame path with 0 args).
- V6 Flags1/Flags2 configuration as above.

### 2. V6 window model (`zvm6.s`, RAM segment — the big one)
- Window table: 8 × 16-word property records in BSS, `get_wind_prop` /
  `put_wind_prop` are straight table reads/writes with validation.
- Reworked-for-V6 semantics behind version branches in the existing
  handlers: `erase_window`, `split_window`, `set_window`, `set_cursor`
  (window arg, cell coords), `get_cursor`, `set_colour` (window arg).
- New EXT: `move_window` (16), `window_size` (17), `window_style` (18),
  `get_wind_prop` (19), `scroll_window` (20), `pop_stack` (21),
  `mouse_window` (23, no-op), `push_stack` (24), `put_wind_prop` (25),
  `print_form` (26), `make_menu` (27, branch false), `picture_table` (28,
  no-op), `buffer_screen` (29, no-op), `read_mouse` (22, zeros).
- Text output routes through the current window's cursor/margins/attributes
  (wrap + scroll bits); `[ MORE ]` pagination only in windows with the
  scrolling attribute.
- User stacks (`push_stack`/`pop_stack`) are plain Z-memory word arrays —
  small.

### 3. Pictures (runtime + host)
- New FIO command `FIO_CMD_ZPICT`: host locates picture *N* in `PICS.PAK`
  on the mounted floppy and DMAs the bitmap into a caller-supplied XRAM
  region (Avalonia: `FileIoController`; hardware: `fio.sv` + ESP32
  dispatcher, identical to story paging).
- `draw_picture`: FIO load → blitter XRAM→gfx blit at (x,y) gfx pixels.
- `erase_picture`: gfx-layer rect fill with the window background colour.
- `picture_data`: answered from the XRAM-resident index (word 0 = height,
  word 1 = width, in units = cells (pixel dimensions ÷4)); `N=0` returns
  count + release.

### 4. XRAM map (512 KB)
Story page cache (≤ 341 KB) + dynamic-memory workspace + save staging as
today, plus: `PICS.PAK` index (~8 KB) and one picture bounce buffer
(worst case 320×200×4bpp = 32 KB). Fits with margin; the build asserts the
layout.

## Per-game quirks (expected, budgeted)

Zork Zero draws its parchment border and status from pictures and reads
window properties back; Arthur keeps a banner/map window; Shogun uses
margins around inline art; Journey is an almost parser-less menu UI.
Quirks get fixed against scripted longplays per project — no global hacks.

## Testing

- **`Nova.NovaZ.Z6SpecStory`**: generated synthetic V6 story (sibling of
  the Z3/Z4/Z5 generators) covering packed-offset calls/strings, the window
  property set, user stacks, cursor/window text placement (verified via VGC
  text cells like the Z4 style fixture), and picture opcodes against a tiny
  generated `PICS.PAK`. `make test-z6-spec`.
- **Picture fixture**: packer round-trip unit tests in C# (PNG → quantize →
  pack → reference bitmap compare).
- **Zork Zero project**: `projects/zork-zero/` smoke + longplay script
  (intro, status border render asserted via gfx-layer readback, Peggleboz
  entry, save/restore, death/restart).
- Arthur, Shogun, Journey projects follow once Zork Zero passes; then the
  hardware smoke runner (`run-novaz-hardware-smokes.py`) gains the V6 set.

## Milestones

1. **M1 — V6 boots, text only**: version gate, packed offsets, main-routine
   start, minimal 8-window model mapped to existing text path. Zork Zero
   reaches its first prompt (ugly is fine).
   **Shipped 2026-06-10** on branch `novaz-z6` (see git log for the commit
   trail): NOVAZ6.BIN RAM segment at `$2000`, user stacks + V6 `pull`,
   no-graphics stubs, `test-z6-suite` green, Zork Zero text-only prompt
   round-trips.
2. **M2 — window model complete**: full EXT block + property semantics;
   `test-z6-spec` green.
   **Shipped 2026-06-10** on branch `novaz-z6` (see git log): units = cells
   (design amended per the capture), windows drive vtext regions, in-window
   scroll/wrap with blanking, real `scroll_window`; Zork Zero renders its
   banner/playfield layout. Capture-driven plan:
   `docs/plans/2026-06-10-novaz-z6-m2.md`.
3. **M3 — pictures on Avalonia**: packer `PICS.PAK`, `FIO_CMD_ZPICT`,
   draw/erase/data; Zork Zero border + minigames render.
4. **M4 — Zork Zero playable**: longplay green, save/restore, quirks fixed.
5. **M5 — remaining three games** with their own projects/longplays.
6. **M6 — hardware**: ESP32 `ZPICT` dispatch, EGA palette mode on FPGA
   verified, hardware smokes.

Out of scope for now: mouse input, `make_menu` UI, sound in V6 games
(Journey/Shogun barely use it; the existing `sound_effect` path covers the
bleeps), proportional fonts.
