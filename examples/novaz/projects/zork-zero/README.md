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
and includes `PICS.PAK` — 503 pictures (396 custom-palette 4bpp bitmaps +
107 Rect placeholders, release 14) pre-converted by the Packer.

## Regression Tests

Quick boot/layout smoke:

```sh
make -C examples/novaz test-project PROJECT=zork-zero \
  STORY="/Volumes/Software/Emulation/Infocom/Zork Zero - The Revenge of Megaboz/Zork0.z6" \
  PICTURES="/Volumes/Software/Emulation/Infocom/Zork Zero - The Revenge of Megaboz/Zork0.blb"
```

Full V6 graphics/save/restore smoke:

```sh
make -C examples/novaz test-project-save-restore PROJECT=zork-zero \
  STORY="/Volumes/Software/Emulation/Infocom/Zork Zero - The Revenge of Megaboz/Zork0.z6" \
  PICTURES="/Volumes/Software/Emulation/Infocom/Zork Zero - The Revenge of Megaboz/Zork0.blb"
```

That script is the important Zork Zero regression. It walks through the prologue,
answers the portcullis riddle, asserts `Entrance Hall` still owns its first text
column after the V6 frame redraw, saves, reboots, restores, and verifies the
outside graphics state is replayed. Run `make -C examples/novaz test-z6-spec`
with it when touching generic V6 window, scroll, erase, picture, or color code.

## Hardware Deploy

Build the current image:

```sh
make -C examples/novaz ndi PROJECT=zork-zero \
  STORY="/Volumes/Software/Emulation/Infocom/Zork Zero - The Revenge of Megaboz/Zork0.z6" \
  PICTURES="/Volumes/Software/Emulation/Infocom/Zork Zero - The Revenge of Megaboz/Zork0.blb"
```

Upload, mount, and reboot Nova hardware:

```sh
e6502.Nova/bin/Release/net10.0/osx-arm64/nova disk upload \
  examples/novaz/dist/zork-zero/fd0.ndi \
  --remote 192.168.1.65 --floppy --name zork0test.ndi
e6502.Nova/bin/Release/net10.0/osx-arm64/nova drive mount \
  fd0 disks/floppy/zork0test.ndi --remote 192.168.1.65
e6502.Nova/bin/Release/net10.0/osx-arm64/nova device reboot \
  --remote 192.168.1.65
```

Use the keyboard bridge for manual play-testing:

```sh
e6502.Nova/bin/Release/net10.0/osx-arm64/nova keyboard \
  --remote 192.168.1.65 --echo --ctrl-c-quits
```

Do not use HDMI capture while judging live display corruption unless that is the
thing being tested; it can disturb the display path. For text-plane checks, use
`tools/snapshot-novavm-state.py --host 192.168.1.65 --no-hdmi --no-pause`.

## Manual `answer time` Route

From the first Banquet Hall prompt, type:

```text
wait
ne
s
w
examine gaunt man
dive under table
get out from under table
get parchment
```

When the story shows `[Hit any key to continue.]`, press Enter. This is not a
normal command prompt and not the same as `[ MORE ]`.

Then type:

```text
n
push doorbell
answer time
```

Expected result: the game reaches the Roof/outside area with the outside header
and border art intact. The transcript must still contain `Entrance Hall`, not
` ntrance Hall` or other first-column corruption.

## V6 graphics status

The game runs with its real art and picture-driven layout:

- **Pictures render.** `draw_picture` streams pak bitmap regions into the
  XRAM bounce buffer, then uses the blitter's `BLT_MODE_GFX4_UNPACK` path to
  expand row-packed 4bpp source into the VGC gfx layer with per-pixel
  transparency;
  the title sequence, the playfield border art, and the per-refresh border
  pic `$D8` all draw. The VGC runs mode 2 (text over gfx); v2 picture packs
  upload a generated 16-entry RGB palette at V6 boot. `--expect-gfx-color`
  probes in `project.mk` pin live picture indices against values derived
  from the blorb PNGs.
- **The banner garble is gone.** `picture_data` answers from the pak index
  (pixel dimensions under the 320x200-unit V6 header), so
  `INIT-STATUS-LINE` lays the banner out as designed:
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
- **Hardware** — the portable path uses only XPAGE-to-XRAM plus the reusable
  blitter unpack mode, so the same draw path runs in Avalonia and on FPGA
  hardware.
