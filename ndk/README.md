# Nova Developer Kit

The Nova Developer Kit (NDK) is the packaged developer-facing surface for
writing NovaVM software outside the BASIC ROM tree. It collects the shared
6502 assembly libraries, generated hardware constants, assembly documentation,
and standalone example applications into one distributable directory.

The canonical source remains in the repository:

| NDK package path | Repository source |
| --- | --- |
| `asm/` | `runtime/asm/` |
| `docs/assembly/` | `docs/assembly/` |
| `docs/help/guides/` | selected guides from `docs/help/guides/` |
| `examples/programs/` | assembly examples from `docs/programs/` |
| `apps/` | `assembly/apps/` |

## Build The Kit

From the repository root:

```sh
make -C ndk package
```

The package is written to `dist/nova-ndk/`.

To also create a compressed archive:

```sh
make -C ndk archive
```

The archive is written to `dist/nova-ndk.tar.gz`.

## Use The Kit

Assemble a standalone program by pointing ca65 at the packaged `asm/`
directory:

```sh
ca65 --cpu 65c02 -I asm -o myprog.o myprog.s
ld65 -C myprog.cfg -o myprog.bin myprog.o
```

## Keep Binaries Small

Nova runtime implementation files are intended to be included after your
application code. For routines that support it, the NDK can use ca65 reference
tracking to assemble only the helper routines your program actually calls.
This is handled by the NDK runtime files; application code does not need an
extra opt-in include.

```asm
.setcpu "65c02"

.include "fio.inc"

    jsr fio_load
    rts

.include "fio.s"        ; emits fio_load, fio_exec, and fio_check
```

Runtime files that support stripping emit referenced routines and their
required dependencies. In the example above, the binary gets `fio_load`,
`fio_exec`, and `fio_check`, not the full file I/O helper surface.

The important rule is ordering: reference library routines first, then include
the implementation. If an implementation is included before the calls, ca65 has
not seen the references yet and stripped routines will not be emitted.
This is the same include-at-the-end pattern used by the NDK examples.

For unusual ROM/debug builds that intentionally include implementation files
before all call sites, define `NOVA_EMIT_ALL_RUNTIME = 1` before those includes
to force full runtime emission. `strip.inc` is still shipped as an explicit
force-strip switch for custom builds, but normal applications should not need
it.

For a small program, include a library interface near the top and include the
implementation once near the end:

```asm
.setcpu "65c02"

.include "vgc.inc"
.include "sprite.inc"

; application code

.include "vgc.s"
.include "sprite.s"
```

Higher-level implementation files include their dependencies. For example,
`anim.s` includes the lower-level animation stack it needs, so applications
should include the highest-level implementation once rather than including
every dependency manually.

File-backed audio helpers use the same filename ABI as FIO. Point
`FIO_ARG_NAMEPTR_L/H` and `FIO_ARG_NAMELEN` at a filename, then call
`audio_midplay_file`, `audio_sidplay_file`, or `audio_sfload_file`:

```asm
.include "audio.inc"

    lda #<song_name
    sta FIO_ARG_NAMEPTR_L
    lda #>song_name
    sta FIO_ARG_NAMEPTR_H
    lda #(song_name_end - song_name)
    sta FIO_ARG_NAMELEN
    jsr audio_midplay_file

song_name:
    .byte "stars-stripes"
song_name_end:

.include "audio.s"
```

For assembly UIs, poll `MUSIC_STATUS`/`AUDIO_STATUS` and test
`AUDIO_STATUS_LOADING` (`$10`) while hosted SID/MIDI assets or soundfonts are
loading. `AUDIO_STATUS_MUSIC`, `AUDIO_STATUS_SID`, and `AUDIO_STATUS_WTS`
identify active playback after the load phase finishes.
Active song metadata is published at `AUDIO_META_*`; SID files use their
embedded title/author/copyright header fields, and newly compiled `.nms` MIDI
streams preserve MIDI title/author/copyright metadata.

## App Layout

The NDK includes standalone apps under `apps/`:

| App | Purpose |
| --- | --- |
| `apps/keyboard` | Piano keyboard music visualizer. |
| `apps/demo` | Demo disk music browser/player. |

These are examples and usable binaries. They are not part of the BASIC ROM.
