# Zork Zero

NovaZ project for a local, user-supplied Zork Zero (V6) story file.

Expected local files (both gitignored):

```text
examples/novaz/projects/zork-zero/STORY.BIN   # Zork0.z6
examples/novaz/projects/zork-zero/PICS.BLB    # Zork0.blb (graphics blorb)
```

Build:

```sh
make -C examples/novaz ndi PROJECT=zork-zero
```

The generated image is written to `examples/novaz/dist/zork-zero/fd0.ndi`
and includes `PICS.PAK` — 503 pictures (396 EGA-quantized 4bpp bitmaps +
107 Rect placeholders, release 14) pre-converted by the Packer.

## M3 status (pictures on Avalonia)

The game runs with its real art and picture-driven layout:

- **Pictures render.** `draw_picture` streams pak bitmap regions into the
  VGC gfx layer (host-assisted 4bpp unpack with per-pixel transparency);
  the title sequence, the playfield border art, and the per-refresh border
  pic `$D8` all draw. The VGC runs mode 2 (text over gfx) in the EGA
  palette; `--expect-gfx-color` probes in `project.mk` pin live picture
  pixels against values derived from the blorb PNGs.
- **The banner garble is gone.** `picture_data` answers from the pak index
  (dims in cells), so `INIT-STATUS-LINE` lays the banner out as designed:
  "Banquet Hall … Flatheadia" / "Moves: 0 … Score: 0".
- **The boot newline storm is gone.** With real picture metrics the
  CR-interrupt countdown arms with a real height instead of -1, so
  `CLEAR-CRCNT` prints a handful of newlines instead of ~65.5K —
  morePrompts collapsed 1489 → 0 and `NOVAZ_SMOKE_MAX_STEPS` dropped
  480M → 80M.
- **The CR interrupt fires.** Each newline decrements window prop 9 and
  calls the prop-8 routine at 0 (Frotz r393 ordering): `RESET-MARGIN`
  releases the drop-cap margins, and the prologue flows at the real border
  margin with the prompt inside the picture-derived 40x58 playfield
  (`move_window 0,11,12` in the post-M3 capture).

Remaining limitations:

- **Stale title art under the playfield** — the game never erases the gfx
  layer after the title sequence (no `erase_window`/`erase_picture` calls);
  text cells with the parchment background cover it, but gutter pixels
  outside text regions can show title remnants. Verify in a GUI session;
  per-window gfx erase semantics are a quirk candidate for M4.
- **Premature ready-detection** — the game's event pump polls `read_char`
  between output paragraphs, so the smoke runner may see "ready" before a
  response finishes; `smoke.txt` pins response text and stable banner
  rows, not exact response rows.
- **Hardware (M6)** — the FPGA gfx plane is 4bpp packed, so the Avalonia
  host-assisted unpack (`FioPageTargetGfx4`) does not apply there; the RTL
  blitter grows the nibble-granular color key (`BltModeColorKey4`, already
  pinned by Avalonia tests) and NovaHost streams pak bytes as-is.
