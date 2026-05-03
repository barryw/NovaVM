# Nova Ozmoo

This directory is the Nova-specific Z-machine/Ozmoo application tree. It is
separate from `ehbasic/` because it is not part of BASIC: it builds as its own
Nova disk image with an `AUTOBOOT.bin` runtime and story assets.

This is not a Commodore target directory. Nova has its own storage, XRAM, VGC,
input, and future sidecar hardware model, so this tree should stay focused on
how Ozmoo/Z-machine support works on Nova.

The first target is a Z3 story smoke test:

- build a 65C02 `AUTOBOOT.bin`,
- package it into an `.ndi`,
- optionally import a user-provided story as `STORY.BIN`,
- package story projects such as Zork I/II/III from per-project directories,
- autoboot on Nova,
- stream `STORY.BIN` into XRAM with the shared `xram` library,
- read and display the story header through flat XRAM addressing.

That gives us the right container and platform boundary before we start pulling
Ozmoo interpreter code across.

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

Build the autoboot binary only:

```sh
make -C ozmoo
```

Build an `.ndi` without a story, useful for checking the autoboot shell:

```sh
make -C ozmoo ndi
```

Build an `.ndi` with a local story file:

```sh
make -C ozmoo ndi STORY=/path/to/zork1.z3
```

Build one of the initial story projects:

```sh
cp /path/to/your/zork1.z3 ozmoo/projects/zork-i/STORY.BIN
make -C ozmoo ndi PROJECT=zork-i
```

The story file is not part of this repository. The packer stores it in the disk
image as `STORY.BIN` and writes a small `STORY.MANIFEST` with parsed header
metadata.

## Current State

This is only the staging shell. The real interpreter still needs the Z-machine
core, line input, dictionary/tokenizer, object/property operations, call stack,
and save/restore support.
