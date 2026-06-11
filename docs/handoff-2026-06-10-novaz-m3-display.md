# Handoff: NovaZ M3 — Zork Zero display corruption (2026-06-10)

**Worktree:** `/Users/barry/Git/e6502/.worktrees/novaz-z6-m3` (branch `novaz-z6-m3`, 14 commits
`f7b52b0..5c3fbb6`, all suites green, NOT merged to main).

## Where things stand

M3 (pictures on Avalonia) shipped per plan `docs/plans/2026-06-10-novaz-z6-m3.md` — Tasks 0–8
complete plus five GUI-driven fix commits. Zork Zero boots with art: banner, columns,
drop-cap, location thumbnails, picture-metric layout, no boot storm, CR interrupt firing,
art scrolling with text.

**Current resume state (Codex):** the side-column/right-border issue was a real
V6 coordinate-contract bug and is fixed without Zork Zero-specific nudges. NovaZ
now advertises a 320x200-unit V6 screen with a 4x4 font, keeps window/picture
coordinates in pixel units, and converts to 80x50 cells only at the VTEXT
boundary. The live gfx plane byte-matches a host replay of the first-prompt draw
list, and `PICS.PAK` v2 carries a generated 16-entry RGB palette that NovaZ
uploads into a new VGC custom-palette mode on V6 boot.

Still worth checking in the GUI against the MCGA screenshot: final subjective
colour quality of the generated global palette. The implementation is automatic
and game-agnostic; no Zork Zero-specific palette table was added.

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

**Resume note (Codex):** added this harness to `Nova.NovaZ.Smoke` as
`--expect-zork0-boot-gfx-replay` and wired it into
`projects/zork-zero/project.mk`. It reads the generated image's `PICS.PAK`,
replays the deterministic first-prompt draw list into a 320x200 colour-index
buffer (white framebuffer fill, pixel-unit placement, transparent skip), and
byte-compares the live gfx plane. Current result: **0 mismatches** at the first
prompt. Follow-up from the user screenshot found the real geometry issue: the
runtime advertised 80x50 units with a 1x1 font, forcing picture draws through a
coarse cell surrogate. `zvm6.s` now uses pixel-unit placement end to end, and
the title-specific `1F1`/`1F2` nudge was removed. The small right-ribbon patch
is picture `$01E1` drawn by the story at pixel-unit `(y=14,x=192)`; with the
generated custom palette it is a dark patch, not the bright EGA-red square seen
in old false-colour previews.

Build a host-side reference renderer and diff the whole plane:

1. The boot draw list is fully known from `examples/novaz/build/nz6-trace-zork-zero-pixel.log`
   after regeneration (seq 25–93: pics 5, 1F1@35,1, 1F2@35,284, title comps
   at y=1,x=139, 1E1@14,192, 2@5,1, D8@45,1 — window-relative pixel units,
   windows 7/0 origins in the trace).
2. Write a C# test (or python tool) that replays that list against PICS.PAK with the SAME
   rules the segment+host implement (origin+rel−2 pixel placement, transparency skip) into a
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

## Programmable VGC palette

Implemented for Avalonia/NovaZ:

- VGC custom palette mode (`PaletteModeCustom`) plus RGB byte-stream registers
  `RegPaletteIndex`/`RegPaletteData`.
- Renderer uses the uploaded 16-entry BGRA palette when custom mode is active.
- `runtime/asm/vgc_palette.s` adds the reusable NDK helper
  `vgc_upload_palette_rgb_xram`.
- Packer emits `PICS.PAK` v2: 9-byte fixed header, 48-byte RGB palette, then the
  unchanged 15-byte index records and 4bpp bitmap data.
- NovaZ V6 accepts both v1 and v2 packs. v1 stays EGA fallback; v2 uploads the
  palette before declaring pictures available.

Not done here: M6 FPGA palette RAM/RTL parity. The software ABI is now pinned
for that work.

## Session traps already paid for (don't repay)

- python `str.replace` patches that silently no-op: ALWAYS `assert old in s`.
- ca65 cheap-label branch range >127: use early `BRA @begin / @off: CLC RTS` islands.
- `--screen-only` smoke stops at FIRST text match → mid-print screens mislead.
- Spec-story fixtures: window 2 is margin-degenerate (set_margins fixture); first print
  after set_window needs explicit set_cursor; position pins must run at the program TAIL
  (earlier output scrolls them away).
- `dotnet build` rebuilds `novabasic_user_guide.pdf` — discard, never commit.
- novavm.inc is GENERATED from VgcConstants.cs — commit it when constants change.
