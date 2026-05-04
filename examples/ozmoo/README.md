# Nova Ozmoo

This directory is the Nova-specific Z-machine/Ozmoo application tree. It is
separate from `ehbasic/` because it is not part of BASIC: it builds as its own
Nova disk image with an `AUTOBOOT.bin` launcher, a `$C000` runtime ROM, and
story assets.

This is not a Commodore target directory. Nova has its own storage, XRAM, VGC,
input, and future sidecar hardware model, so this tree should stay focused on
how Ozmoo/Z-machine support works on Nova.

The first target is a Z3 story smoke test using the generic Nova runtime
launcher path:

- build a RAM-resident 65C02 `AUTOBOOT.bin`,
- build a 16K `ozmoo.bin` runtime ROM linked at `$C000`,
- package it into an `.ndi`,
- optionally import a user-provided story as `story.bin`,
- package story projects such as Zork I/II/III from per-project directories,
- autoboot on Nova through BASIC,
- have the launcher load `ozmoo.bin` into the primary runtime ROM bank,
- jump through the Ozmoo runtime reset vector,
- stream `story.bin` into XRAM with the shared `xram` library,
- read and display the story header through flat XRAM addressing,
- provide a Nova `zstory` layer for interpreter byte/word reads from XRAM.

That gives us the right container and platform boundary before we start pulling
Ozmoo interpreter code across.

The upstream Ozmoo source lives at <https://github.com/johanberntsson/ozmoo>.
Nova should use it as the interpreter reference, but the platform glue here is
Nova-native: NDI storage, XRAM story memory, VGC text output, and runtime
replacement at `$C000`.

## Layout

| Path | Purpose |
| --- | --- |
| `src/` | Nova 65C02 runtime source. |
| `experiments/` | Throwaway notes, probes, and small test programs while we figure out the Nova port. |
| `projects/` | Per-story project directories. Story files are local-only and ignored. |
| `tools/Nova.Ozmoo.Packer/` | Host-side `.ndi` packer. |
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
make -C examples/ozmoo
```

Build an `.ndi` without a story, useful for checking the runtime handoff:

```sh
make -C examples/ozmoo ndi
```

The default image is `examples/ozmoo/dist/fd0.ndi`, matching the NovaHost
auto-mount name for an inserted FD0 floppy.

Build an `.ndi` with a local story file:

```sh
make -C examples/ozmoo ndi STORY=/path/to/zork1.z3
```

Build one of the initial story projects:

```sh
cp /path/to/your/zork1.z3 examples/ozmoo/projects/zork-i/STORY.BIN
make -C examples/ozmoo ndi PROJECT=zork-i
```

The story file is not part of this repository. The packer stores it in the disk
image as `story.bin` and writes a small `STORY.MANIFEST` with parsed header
metadata.

Run the boot/runtime smoke test against a generated image:

```sh
make -C examples/ozmoo smoke STORY=/path/to/zork1.z3
```

Run the scripted Zork I state/scroll regressions against a generated image:

```sh
make -C examples/ozmoo test-zork-state STORY=/path/to/zork1.z3
make -C examples/ozmoo test-zork-house STORY=/path/to/zork1.z3
```

The smoke runner boots the generated `fd0.ndi` in the Avalonia hardware model,
lets BASIC run `AUTOBOOT.bin`, swaps in `ozmoo.bin` at `$C000`, and verifies the
runtime screen against `STORY.MANIFEST`. Scripted runs use visible command
transcripts so stale text left on screen cannot satisfy a later turn. They also
normalize whitespace in expected phrases, which catches words split across the
right edge while allowing normal word-boundary line wrapping. If Ozmoo displays
`[ MORE ]`, the smoke runners press Enter and continue waiting for the prompt.

## Current State

This is still the staging runtime. It now has the Nova platform boundary and
XRAM-backed story memory in place, including version filtering, byte/word story
reads and writes, text decoding, line input, dictionary/tokenizer support, the
core opcode loop, object/property operations, calls/returns, and basic Z3 screen
handling with word-boundary wrapping and `[ MORE ]` pagination. Save/restore
and broader story compatibility are still pending.
