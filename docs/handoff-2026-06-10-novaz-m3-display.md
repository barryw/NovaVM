# Handoff: NovaZ M3 — Zork Zero display corruption (2026-06-10)

**Worktree:** `/Users/barry/Git/e6502/.worktrees/novaz-z6-m3` (branch `novaz-z6-m3`, 14 commits
`f7b52b0..5c3fbb6`, all suites green, NOT merged to main).

## Where things stand

M3 (pictures on Avalonia) shipped per plan `docs/plans/2026-06-10-novaz-z6-m3.md` — Tasks 0–8
complete plus five GUI-driven fix commits. Zork Zero boots with art: banner, columns,
drop-cap, location thumbnails, picture-metric layout, no boot storm, CR interrupt firing,
art scrolling with text.

**BUT the display is still wrong** (user evidence, not yet diagnosed):

1. **The compass rose emblem is corrupted** — in the original it sits on a complete WHITE
   disc overlapping the banner; ours renders the disc as a thin top sliver with the rest
   dark/black ring + banner colours showing through. NOT a palette issue — the geometry
   of the rendered emblem is wrong.
2. A black notch right of the centre emblem area and other banner discontinuities remain.
3. The banner/column seam still shows a gap band in places.

The user compared against a real MCGA screenshot (web "zork zero" image) — keep using that
as the reference.

## What's been fixed so far (and how) — context for the next debugger

- `b1f2ed6` draw_picture blanks covered text cells (mode-2: non-bg cells cover gfx).
- `debfa8e` set_colour -1 samples gfx pixel (VGC MEMREAD); underpaint (now retired);
  gfx scrolls with text (`nz_lf_scrolled` + `nz6_gfx_scroll_live`).
- `d7b4ea0` parchment lives in the GFX plane: V6 erase fills gfx with window bg, cells
  clear transparent (bg 0), draw blanks CEIL cell coverage, scroll strips fill bg.
- `5c3fbb6` gfx transparency parked at $FF for V6 (values >15 disable; VGC stores raw
  byte) so black art (index 0) renders; underpaint removed from the draw.

Each fix was verified by screenshot, yet artifacts remain — pixel-poking individual
screenshots is whack-a-mole. **Stop doing that.**

## Recommended next step: a golden-image harness

Build a host-side reference renderer and diff the whole plane:

1. The boot draw list is fully known: `docs/plans/data/2026-06-10-nz6-trace-zork-zero-post-m3.txt`
   (seq 25–67: pics 5, 1F1@10,1, 1F2@10,71, title comps at y=1,x=36, 1E1@5,49, 2@2,1,
   D8@12,1 — window-relative coords, windows 7/0 origins in the trace).
2. Write a C# test (or python tool) that replays that list against PICS.PAK with the SAME
   rules the segment+host implement (cells→px ×4, origin+rel−2, transparency skip) into a
   320×200 buffer.
3. Boot the real image headlessly (smoke harness pattern), snapshot the gfx plane
   (`bus.Vgc.GetGfxPixelColor` / `TryReadMemorySpace(MemSpaceGfx)`), and DIFF byte-exact.
   First mismatch coordinates → the guilty stage (packer nibble order? odd-width rows?
   draw clipping? ceil-blank? scroll? coordinate conversion for specific windows?).

Hypotheses for the emblem corruption, in likelihood order:
- Pic 1E1 (emblem) draw at window 7 rel (5,49): coordinate conversion or clip bug for
  that placement; check what 1E1's dims/flags are in the pak and where its rect lands.
- Transparent-pixel handling for pics whose transparent index collides with art colours
  actually used (packer picks lowest UNUSED index per pic — verify for 1E1/5).
- A LATER draw/blank (banner pic? refresh?) overwriting part of the emblem (check trace
  order vs reference behaviour).
- The Gfx4 unpack odd-width pad bit or row stride off-by-one for specific widths.

## Verification tooling (all working)

- GUI: `cp examples/novaz/dist/zork-zero/fd0.ndi ~/e6502-disks/fd0.ndi` (backup at
  `fd0.ndi.pre-m3.bak`); `dotnet run --project e6502.Avalonia` from the WORKTREE.
- TCP localhost:6502 newline-JSON: `{"command":"screenshot","path":...,"type":"frame"}`,
  `read_screen`, `peek` (e.g. BLT_FILLVALUE $BA97), `type_text`+`send_key ENTER`.
- Headless: `make -C examples/novaz test-project PROJECT=zork-zero`;
  `--expect-gfx-color "x,y=>hex"` probes (gfx INDEX, palette-independent).
- Dispatch trace: `make capture-z6-trace PROJECT=zork-zero` (bit-deterministic).

## Follow-up feature (agreed with user): programmable VGC palette

Colours ARE also off (EGA can't represent the maroon/parchment MCGA tones) and the user
wants this: packer median-cuts the blorb art (main Zork Zero screen = exactly 15 distinct
colours) → palette block in PICS.PAK header (format v2) → VGC custom palette mode +
16-entry write interface (Avalonia renderer + M6 FPGA palette RAM) → runtime uploads at
V6 boot, reset-safe. No per-game config: automatic from art ("that would be sort of
neat!" — user). Do this AFTER the geometry is pixel-correct, or palette changes will
mask geometry diffs in the golden harness.

## Session traps already paid for (don't repay)

- python `str.replace` patches that silently no-op: ALWAYS `assert old in s`.
- ca65 cheap-label branch range >127: use early `BRA @begin / @off: CLC RTS` islands.
- `--screen-only` smoke stops at FIRST text match → mid-print screens mislead.
- Spec-story fixtures: window 2 is margin-degenerate (set_margins fixture); first print
  after set_window needs explicit set_cursor; position pins must run at the program TAIL
  (earlier output scrolls them away).
- `dotnet build` rebuilds `novabasic_user_guide.pdf` — discard, never commit.
- novavm.inc is GENERATED from VgcConstants.cs — commit it when constants change.
