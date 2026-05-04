# NovaZ Engine Design

*Date: 2026-04-30*

## Summary

Build NovaZ, a Nova-native Z-machine runtime, so Nova can boot an `.ndi` image
and play a bundled interactive-fiction story from XRAM. The runtime should be
written in 65C02 assembly, exercise Nova's real storage, XRAM, VGC, and input
hardware, and eventually support Z-machine story versions starting with Z3 and
growing toward Z6+.

This is not an immediate Ozmoo port. Ozmoo is a strong reference point for
Z-machine behavior, test cases, and platform boundaries, but Nova is missing
several platform capabilities that need to exist before a direct engine port is
reasonable. The first goal is to define those capabilities and stage the work
so the eventual interpreter has a stable Nova platform under it.

## Goals

- Autoboot a story disk image: insert or mount an `.ndi`, boot Nova, and land
  directly in the story interpreter.
- Keep the story file resident in XRAM. The runtime should treat XRAM as the
  authoritative backing store for the full story image.
- Write the runtime/interpreter in 65C02 assembly. Host-side build and packing
  tools may be C# or scripts, but the running Z-machine should be Nova-native
  assembly.
- Use Nova's normal hardware surfaces: NDI/FIO for storage, XMC for XRAM, VGC
  for text display, VGC input for keyboard, and later SID/WTS where applicable.
- Make the project a meaningful integration test of Nova, especially large
  file I/O, XRAM addressing, autoboot, screen windows, and save/restore.

## Non-goals

- Do not start by porting all of Ozmoo's Commodore platform layer.
- Do not write a C# Z-machine interpreter for gameplay.
- Do not depend on Commodore KERNAL, D64/D81 layout, REU behavior, PETSCII
  screen RAM, or VERA-style bank windows.
- Do not require arbitrary Z6 graphical-game compatibility for the first
  playable milestone. Z6 should be handled as its own compatibility phase.
- Do not make the first milestone depend on perfect undo, scrollback, custom
  fonts, or sound effects.
- Do not build a full desktop/window-manager API before the interpreter needs
  it. The useful primitive is rectangular text regions with independent cursor
  and scroll behavior.

## Current Nova Constraints

Nova already has most of the right primitives, but not yet in the shape a
large story runtime needs.

- `AUTOBOOT.bin` is a viable launch path. Existing design docs and demo work
  load a binary at `$4000` and jump to it during boot.
- NDI files support 256-byte sectors, 32-bit file sizes, directory entries,
  and contiguous file allocation through the storage layer.
- FIO `LOAD` currently loads `.bas` or `.bin` through a 16-bit CPU-RAM path.
  That works for the engine binary, but not for a 128 KB to 512 KB story file.
- FIO exposes loaded size as a 16-bit field, while Z-machine story files may
  need larger sizes and exact checksum coverage.
- XMC exposes 512 KB of XRAM through 24-bit addresses, raw `STASH`/`FETCH`,
  byte commands, named blocks, and four 256-byte CPU-visible windows at
  `$BC00`--`$BFFF`.
- That 512 KB capacity is enough for standard Z-machine story files through
  Z8. V1-V3 max out at 128 KB, V4-V5 at 256 KB, and V6-V8 at 512 KB.
- XRAM cannot execute code. Interpreter code must remain in CPU RAM; story
  bytes and mutable dynamic memory may live in XRAM.
- VGC text memory is not directly memory-mapped as a flat CPU range. The
  interpreter should write character and color planes through the VDC-style
  VRAM port at `$A0E0`--`$A0E4`, or through narrow helper routines.
- VGC `RegCharOut` is useful for simple output but not sufficient for
  Z-machine windows, status lines, arbitrary cursor placement, and styled text.

## Target Disk Layout

An eventual story image should be generated offline by a Nova Z-machine packer.
The image should include:

```
/AUTOBOOT.bin       65C02 Z-machine runtime, load address in file prefix
/STORY.z3           Original story file, or normalized story payload
/STORY.manifest     Nova-specific metadata, optional but recommended
/SAVES/             Save files, created at runtime
```

For the first milestone, if large file loading is not ready, the packer may
split the story into fixed-size chunk files:

```
/AUTOBOOT.bin
/STORY00.bin
/STORY01.bin
/STORY02.bin
...
```

The engine can load each chunk into a CPU buffer and `STASH` it into XRAM. This
is slower and uglier than a real file-to-XRAM path, but it allows early
interpreter work to proceed without blocking on new FIO hardware.

## Missing Platform Pieces

### 1. Large File To XRAM Loading

This is the first prerequisite. The runtime needs a way to place the entire
story file into XRAM without trying to fit it in CPU RAM.

Required platform path:

- **FIO `XLOAD`:** Add a FIO command that reads an NDI or host-directory file
  directly into XRAM. Inputs: filename, destination XRAM address, maximum
  length. Outputs: exact 24- or 32-bit file size, status, error code.
- **Shared runtime wrapper:** `xram_load_file` calls FIO `XLOAD` so the
  Z-machine, compilers, and standalone assembly programs use the same API.

Temporary bootstrap path:

- **Chunked MVP:** Packer emits `STORYnn.bin` chunks. `AUTOBOOT.bin` loads each
  chunk into a staging buffer below `$A000`, then uses XMC `STASH`.

Chunking is only a way to start interpreter smoke tests before `XLOAD` exists.
It should not become the permanent story-asset model.

### 2. XRAM Save/Restore Path

Z-machine save files need to persist more than a BASIC program range:

- story identity: release, serial, checksum
- current PC
- call stack and evaluation stack
- dynamic memory
- RNG state if deterministic replay matters
- engine save-format version

Nova needs:

- a paired FIO `XSAVE` command for XRAM-to-file writes,
- and a shared `xram_save_file` wrapper that hides the FIO register details.

Undo can be deferred. If implemented, undo should probably store a copy of
dynamic memory plus stack state in XRAM, which competes with large story files.

### 3. 24/32-Bit File Metadata

The runtime needs exact story length and checksum coverage. Current file
metadata paths are partly 16-bit from the 6502 side. The Z-machine packer can
place story length in a manifest, but Nova should eventually expose full file
size for large binary assets.

Useful additions:

- file size low/mid/high/highest bytes in a metadata register block,
- file type for story/data assets,
- optional checksum field written by the packer.

### 4. XRAM Window Mapping Helpers

The interpreter should not call XMC `GETBYTE` for every story byte. That would
make opcode fetch, text decoding, dictionary lookup, and object access too slow.

Build the interpreter on the shared XRAM runtime ABI described in
`docs/plans/2026-04-30-xram-flat-addressing-design.md`, not on
Z-machine-private window routines. The Z-machine can reserve windows by role,
but the byte/word/page-crossing mechanics should live in a reusable `xram.inc`
and `xram.s` pair that other assembly programs and language runtimes can also
link.

Build that common assembly layer around the four XMC windows:

- window 0: instruction stream / PC page
- window 1: general reads
- window 2: dynamic-memory writes and object/property access
- window 3: string, dictionary, or table scanning

Core routines:

- `xram_map_window`
- `xram_read8`
- `xram_read16be`
- `xram_write8`
- `xram_write16be`
- `xram_fetch_pc8`
- `xram_fetch_pc16be`

These routines must handle 256-byte page crossings and 24-bit story offsets
cleanly. This layer is foundational; almost every opcode depends on it, and it
should be useful outside the Z-machine project.

### 5. VGC Text Backend

The Z-machine screen model needs more than sequential character output.
Infocom status lines use reverse video, so the text backend must support
per-cell background/foreground attributes before a faithful Z3 target is
usable.

Required backend routines:

- clear screen and clear window
- cursor positioning
- write character at row/column
- write color/style at row/column
- scroll lower window without destroying the upper/status window
- status line for Z3
- split-window and upper-window behavior for Z4+
- erase line/window
- reverse video, bold/emphasis mapping, and default colors

Implementation should target the VGC VRAM port and maintain a small software
cursor/window state. Direct `RegCharOut` output can remain a diagnostic or
fallback path.

Nova should provide this as a small reusable text-region layer, not as
runtime-private raw VGC code and not as a large C128-style windowing system. The
primitive we need is common across Z-machine output, monitor/debugger panes,
BASIC help/status areas, and terminal-style scroll margins: a rectangular text
area with its own cursor, attributes, clipping, and optional scrolling.

The first assembly API can stay deliberately small:

- `vtext_define_region`: id, left, top, width, height, flags
- `vtext_select_region`: make a region the current output target
- `vtext_set_cursor`: cursor position relative to the selected region
- `vtext_put_char`: clipped/wrapped character output with current attributes
- `vtext_scroll_up`: scroll the selected region or explicit rectangle
- `vtext_clear_region`: fill a region with spaces and current/default attrs
- `vtext_clear_line`: clear one line within a region
- `vtext_set_attrs`: set foreground, background, reverse, emphasis, flash

For Z3, the runtime can define:

```
region 0: status line, row 0, height 1, no scroll
region 1: story output, row 1..bottom, scroll enabled
```

When the lower story region scrolls, the backend copies only rows
`top+1..bottom` to `top..bottom-1` and then clears the bottom row. The same
operation must be applied to the character, color, and text-attribute planes.
This preserves the status line and gives the Z-machine interpreter the same
kind of platform boundary Ozmoo gets from its `screenkernal` layer, where a
scroll-start row protects top/status lines.

For early correctness, the region scroll helper may use the VGC VRAM port in a
tight loop. For performance, it should move to DMA/blitter row copies once that
path is standardized for VGC memory spaces. Scroll-up is overlap-safe with a
forward copy because the destination rows are lower than the source rows.
Scroll-down, if later needed, should use a backward or direction-aware copy.

NovaZ code should map Z-machine windows onto these text regions
rather than manipulating VGC planes directly. For Z4/Z5, `split_window` changes
the upper/lower region geometry; the lower region's scroll top becomes the row
after the split. Z6 graphical windows remain a later compatibility layer on top
of the VGC rather than a requirement for the first text-region API.

The practical VGC contract should be:

```
Color RAM byte: high nibble = background, low nibble = foreground
Normal text:    bg = default background, fg = default foreground
Reverse text:   bg = default foreground, fg = default background
Text Attr byte: bit 0 = flash
```

Avalonia and RTL should keep these semantics aligned. The Z-machine backend can
write packed Color RAM directly for status bars, or use `RegTextFlags` for
sequential output. GTEXT can honor reverse colors when drawing into bitmap
graphics, but flash remains a text-layer attribute rather than a retained
graphics effect.

### 6. Keyboard And Line Input

The engine needs a Z-machine input editor, not just raw key reads.

Required behavior:

- poll VGC character input at `$A00F`
- map Nova key codes to ZSCII where possible
- line editing with backspace, enter, and redraw
- tokenization against the story dictionary
- timeout behavior later for timed input
- single-key reads for menus and prompts

The first milestone can support blocking line input only.

### 7. Z-Machine Core Runtime

Once the platform layer is present, the engine itself needs:

- opcode decoder: long, short, variable, and extended forms
- operand decoding
- 24-bit PC state
- evaluation stack
- call frames and local variables
- globals table access
- variable read/write semantics
- branch, return, and call handling
- packed address conversion by story version
- object tree and property table handling
- dictionary/tokenizer
- Z-character and ZSCII text decoding
- arithmetic, comparisons, jumps, stores, loads
- random number behavior
- save/restore opcodes

This should be built incrementally with a small opcode test harness rather than
as one large drop.

## Version Strategy

The compatibility sequence should avoid letting Z6 dominate the first design.
The flat 24-bit XRAM ABI is sufficient for story-file addressing through Z8;
Z6 is harder because of screen/resource semantics, not because the story address
space is too large.

1. **Z3:** First target. Gives classic stories, status line behavior, basic
   object/property/dictionary coverage, and manageable memory sizes.
2. **Z5:** Adds larger stories, more common modern testing, and more complete
   opcode coverage.
3. **Z4:** Mostly compatibility cleanup once Z5 exists.
4. **Z7/Z8:** Large-story address scaling and packaging. These are closer to
   Z5 than to Z6.
5. **Z6:** Separate milestone. Requires graphics/window/resource decisions and
   should not block the text-engine architecture. Nova's VGC gives us a strong
   target for that work when we choose to tackle it: the hard part is defining
   the Z6 compatibility layer, not inventing display hardware.

If Nova authors generate new story files for Nova, we can define a practical
supported Z6 subset. Arbitrary historical Z6 compatibility is a larger project.

## Relationship To Ozmoo

Ozmoo remains valuable as a reference, especially for:

- interpreter architecture,
- Z-machine version quirks,
- object/property/text/dictionary behavior,
- save/restore shape,
- test story behavior,
- and target abstraction boundaries.

However, Ozmoo's platform layer is tightly bound to Commodore-style concerns:
KERNAL calls, disk layout, REU, C64/C128/Plus4/MEGA65/X16 memory maps, and
screen backends. Nova should not emulate those assumptions just to make the
port easier. The better path is to build the missing Nova primitives first,
then decide whether to adapt Ozmoo's engine code or use it mainly as a
behavioral reference.

## Proposed Milestones

### Milestone 0: Story Disk Packer

- Add host tooling that accepts a `.z3`/`.z5` story and emits an `.ndi`.
- Include `AUTOBOOT.bin` and either one story file or chunked story files.
- Parse and validate the story header.
- Emit a manifest with version, release, serial, checksum, dynamic-memory size,
  static-memory base, globals table, dictionary table, object table, and entry
  PC.

### Milestone 1: XRAM Loader Smoke Test

- Autoboot `AUTOBOOT.bin`.
- Load or stream the whole story into XRAM.
- Verify story header fields from XRAM.
- Compute and display checksum from XRAM.
- Test sizes across 64 KB, 128 KB, 256 KB, and near 512 KB.

This milestone proves Nova can move large story assets through NDI and XRAM.

### Milestone 2: Minimal Z3 Engine

- Implement XRAM read/write helpers.
- Implement opcode fetch/decode for enough opcodes to run a tiny test story.
- Implement text output, blocking line input, dictionary tokenization, stack,
  calls, branches, object/property access, and status line.
- Run known small Z3 story files.

### Milestone 3: Save/Restore

- Define Nova save-file format.
- Implement dynamic-memory and stack serialization.
- Persist saves to NDI.
- Validate story identity before restore.

### Milestone 4: Z5 And Larger Stories

- Fill out missing opcodes and version quirks.
- Exercise large-story address paths.
- Validate performance of XRAM window mapping.
- Add regression stories and integration tests.

### Milestone 5: Z6/Z7/Z8 Expansion

- Add Z7/Z8 address scaling and packaging support.
- Design the Z6 graphics/window/resource subset.
- Decide how Nova VGC features map to Z6 display primitives.

## Test Plan

- Unit-level assembly tests for XRAM helpers, page crossings, and packed
  address conversion.
- Headless integration tests that autoboot an NDI and inspect screen RAM.
- Checksum tests for stories loaded entirely into XRAM.
- Golden-output tests using small generated Z3/Z5 stories.
- Save/restore round-trip tests.
- Hardware tests on FPGA/ESP path once large FIO/XRAM transfer exists.

## Open Questions

- Should the story be stored in NDI as the original `.z3`/`.z5` bytes, or as a
  Nova-normalized payload with a manifest?
- How much XRAM should be reserved for undo, scrollback, and save staging?
- Should dynamic memory stay in XRAM for simplicity, or be mirrored in CPU RAM
  when it fits?
- What is the minimum useful Z6 subset for Nova-authored stories?
- Should NDI gain an explicit story file type, or is `.bin`/generic data enough?

## Recommended First Step

Start with Milestone 1: prove that an autobooting 65C02 binary can load a full
story image into XRAM and verify it from XRAM. Until that works, NovaZ will be
blocked by storage and memory plumbing rather than interpreter logic.
