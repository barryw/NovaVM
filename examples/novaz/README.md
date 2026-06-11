# NovaZ

This directory is the Nova-specific Z-machine runtime tree. It is
separate from `ehbasic/` because it is not part of BASIC: it builds as its own
Nova disk image with an `AUTOBOOT.bin` launcher, a `$C000` runtime ROM, and
story assets.

This is not a Commodore target directory. Nova has its own storage, XRAM, VGC,
input, and future sidecar hardware model, so this tree should stay focused on
how NovaZ works on Nova.

The first target is a Z3 story smoke test using the generic Nova runtime
launcher path:

- build a RAM-resident 65C02 `AUTOBOOT.bin`,
- build a 16K `novaz.bin` runtime ROM linked at `$C000`,
- package it into an `.ndi`,
- optionally import a user-provided story as `story.bin`,
- package story projects such as Zork I/II/III and Deadline from per-project
  directories,
- autoboot on Nova through BASIC,
- have the launcher load `novaz.bin` into the primary runtime ROM bank,
- jump through the NovaZ runtime reset vector,
- allocate owned XRAM buffers for dynamic story memory and the static story
  page cache,
- read and display the story header through flat XRAM addressing,
- provide a Nova `zstory` layer for interpreter byte/word reads from XRAM.

NovaZ began as an Ozmoo-inspired experiment, but it is now a Nova-native
implementation: NDI storage, XRAM story memory, VGC text output, and runtime
replacement at `$C000`.

The upstream Ozmoo source remains a useful reference:
<https://github.com/johanberntsson/ozmoo>.

## Layout

| Path | Purpose |
| --- | --- |
| `src/` | Nova 65C02 runtime source. |
| `experiments/` | Throwaway notes, probes, and small test programs while we figure out the Nova port. |
| `projects/` | Per-story project directories. Story files are local-only and ignored. |
| `tools/Nova.NovaZ.Packer/` | Host-side `.ndi` packer. |
| `build/` | Local object/map/binary output, ignored. |
| `dist/` | Local disk images, ignored. |

Use `experiments/` for work that is allowed to be messy: opcode probes,
XRAM-window timing tests, text decoder sketches, Zork-specific notes, and
anything that helps us answer a question before it becomes part of `src/`.
Once an experiment hardens into real Nova runtime code, move it into `src/`
or a shared assembly library.

## Build

Build the launcher and runtime ROM:

```sh
make -C examples/novaz
```

Build an `.ndi` without a story, useful for checking the runtime handoff:

```sh
make -C examples/novaz ndi
```

The default image is `examples/novaz/dist/fd0.ndi`, matching the NovaHost
auto-mount name for an inserted FD0 floppy.

Build an `.ndi` with a local story file:

```sh
make -C examples/novaz ndi STORY=/path/to/zork1.z3
```

Build one of the initial story projects:

```sh
cp /path/to/your/zork1.z3 examples/novaz/projects/zork-i/STORY.BIN
make -C examples/novaz ndi PROJECT=zork-i
```

The story file is not part of this repository. The packer stores it in the disk
image as `story.bin` and writes a small `STORY.MANIFEST` with parsed header
metadata.

Run the boot/runtime smoke test against a generated image:

```sh
make -C examples/novaz smoke STORY=/path/to/zork1.z3
```

Run the scripted Zork I state/scroll regressions against a generated image:

```sh
make -C examples/novaz test-zork-state STORY=/path/to/zork1.z3
make -C examples/novaz test-zork-house STORY=/path/to/zork1.z3
make -C examples/novaz test-zork-save STORY=/path/to/zork1.z3
```

Run a project-specific regression, such as Deadline's time-status smoke:

```sh
make -C examples/novaz test-project PROJECT=deadline STORY=/path/to/deadline.z3
```

Run the local Infocom archive smokes when `/Volumes/Software/Emulation/Infocom`
is mounted:

```sh
make -C examples/novaz test-infocom-z3-smokes
make -C examples/novaz test-infocom-v45-smokes
make -C examples/novaz test-infocom-smokes
```

Run the longer scripted playthroughs when you want heavier game coverage:

```sh
make -C examples/novaz test-infocom-z3-longplays
make -C examples/novaz test-infocom-v45-longplays
make -C examples/novaz test-infocom-longplays
```

These scripts keep the quick smokes fast, but add representative restart,
save/restore, death, combat, time-status, V4 status-window, V5 save, parser,
inventory, scrolling, and event coverage across real Infocom stories.

Run the generated Z3 compliance smoke:

```sh
make -C examples/novaz test-z3-spec
```

`test-z3-spec` builds a synthetic Version 3 story file and runs it through the
same Nova autoboot/runtime path as the game smokes. The generated story directly
exercises arithmetic, signed comparisons, branches, variable stores,
increment/decrement opcodes, stack operations, memory load/store, routine
calls/returns, literal and address-based printing, abbreviations, ZSCII escape
decoding, signed `print_num`, object tree operations, attributes, properties,
output stream 3, `verify`, and dictionary tokenization including separator
tokens. Keep this target Z3-only; Version 4+ work should get separate generated
stories and targets.

Run the generated Version 4 text style fixture:

```sh
make -C examples/novaz test-z4-styles
```

`test-z4-styles` builds a tiny synthetic Z4 story that prints normal, bold,
reverse, bold-reverse, italic, and fixed-style text. The smoke runner checks
both the visible text and the VGC text color cells: normal/ignored styles use
color `$0C`, bold uses `$0F`, reverse normal uses `$C0`, and bold reverse uses
`$F0`.

Run the generated Version 5 compliance smoke:

```sh
make -C examples/novaz test-z5-spec
```

`test-z5-spec` builds a tiny synthetic Z5 story that exercises V5 text-buffer
layout, selected memory/table operations, output stream handling, and parser
input through the same Nova runtime path.

Run the generated Version 4 compliance smoke:

```sh
make -C examples/novaz test-z4-spec
```

`test-z4-spec` builds a tiny synthetic Z4 story covering V4 capability header
bits and timed input through the same Nova runtime path.

Run the generated Version 6 suite:

```sh
make -C examples/novaz test-z6-suite
```

`test-z6-suite` runs four V6 targets. `test-z6-spec` builds a synthetic V6
story that exercises packed routine/string offsets, the 8-window model, user
stacks, the V6 `pull` form, and cursor/window text placement verified through
VGC text cells. `test-z6-v7-rejected` checks that a Version 7 header is
refused with the unsupported-version message. `test-z6-no-segment` packs a V6
story without `NOVAZ6.BIN` and expects the runtime to abort with the
missing-segment message. `test-z6-main-returns` checks that a V6 main routine
that returns halts the machine cleanly instead of wedging.

Run every generated spec suite in one shot:

```sh
make -C examples/novaz test-spec-suites
```

`test-spec-suites` aggregates `test-z3-spec`, `test-z4-styles`,
`test-z4-spec`, `test-z5-spec`, and `test-z6-suite`. None of these need
external story files, so this is the local regression gate for runtime
changes.

Build and smoke the Zork Zero (V6) project with a local story file:

```sh
cp /path/to/your/zork0.z6 examples/novaz/projects/zork-zero/STORY.BIN
make -C examples/novaz test-project PROJECT=zork-zero
```

See `projects/zork-zero/README.md` for the M1 text-only status and known
layout limitations.

The smoke runner boots the generated `fd0.ndi` in the Avalonia hardware model,
lets BASIC run `AUTOBOOT.bin`, swaps in `novaz.bin` at `$C000`, and verifies the
runtime screen against `STORY.MANIFEST`. Scripted runs use visible command
transcripts so stale text left on screen cannot satisfy a later turn. They also
normalize whitespace in expected phrases, which catches words split across the
right edge while allowing normal word-boundary line wrapping. If NovaZ displays
`[ MORE ]`, the smoke runners press Enter and continue waiting for the prompt.

## Current State

This is still the staging runtime. It now has the Nova platform boundary and
XRAM-backed story memory in place, including fixed dynamic-memory and page-cache
workspaces, version filtering, byte/word story reads and writes,
text decoding, line input, dictionary/tokenizer support, the
core opcode loop, object/property operations, calls/returns, Z3 screen handling
with word-boundary wrapping and `[ MORE ]` pagination, and enough V4/V5 support
to boot and script representative Infocom games. Z3 save/restore uses native
Nova save files through the shared XRAM/FIO path. The generated Z3/Z4/Z5/Z6
compliance smokes and Z4 style fixture guard opcode-focused behavior that game
transcripts do not reliably cover.

The version gate now accepts Versions 1-6. V6 support (M2, text-only) lives in
a RAM-resident segment, `NOVAZ6.BIN`, linked at `$2000` and loaded by the
runtime when a V6 story boots: the 16K `$C000` ROM is full, so V6-only code
(window routing, user stacks, the V6 `pull` form, no-graphics stubs) moves to
RAM and calls back into the ROM through a pinned ABI (`runtime_abi.inc`,
generated from `runtime.sym` at build time). The packer adds `NOVAZ6.BIN` to
every image; a V6 story booted from an image without it aborts with
`NOVAZ6.BIN MISSING OR INVALID`. The segment provides the 8-window model
mapped onto the existing text path, user stacks in dynamic memory, and the
V6 picture pipeline. V6 units are gfx pixels (the advertised header is
320x200 units with a 4x4 font); the window property table drives real vtext
regions by snapping units to 80x50 cells at the text boundary. Windows render
as true rectangles with per-window cursors, margins, in-window scrolling
(vacated rows blanked), and a working `scroll_window`.

M3 added pictures on Avalonia: the packer's `--pictures <file.blb>`
packs the graphics blorb into `PICS.PAK` (4bpp row-packed bitmaps + a flat
index; v2 packs include a generated 16-entry RGB palette) onto the image; the
segment loads the index into XRAM at boot and answers `picture_data` from it
(pixel dimensions, count/release for N=0, Flags1 bit 1 set), and
`draw_picture` streams the bitmap region into the XRAM bounce buffer and uses
the blitter's `BLT_MODE_GFX4_UNPACK` path to unpack row-packed 4bpp bytes into
the gfx plane with per-pixel transparency and edge clipping. V6 boots in
custom-palette mode when a v2 pack is present, otherwise EGA fallback, plus
VGC mode 2 (text over gfx) with
a Z-colour -> VGC-index mapping behind `set_colour`/`set_text_style`. The
carriage-return interrupt (window props 8/9) decrements per newline and fires
the armed routine (Frotz r393 ordering). A pak-less image still runs the
honest text-only path. Zork Zero draws its title art and borders, lays the
banner out from real picture metrics, and its boot newline storm is gone
(morePrompts 1489 -> 0); see `projects/zork-zero/README.md` for what remains
(M4 quirks, M6 hardware notes).
