# NDK is the source of truth; modules are thin wrappers

**Date:** 2026-06-05
**Status:** Approved, implementing
**Principle (user):** "the ndk should be the source of truth for our functions" /
"everything should just be a thin wrapper around the ndk functions"

## Problem

Paged-library modules (`modules/graphics|system|sound`) deliver NDK functionality
to runtimes via `lib_call`. The GRAPHICS module does this correctly — it `.include`s
the canonical NDK driver bodies (`vgc.s`, `sprite.s`, …) and each wrapper is a thin
shim (`gfn_plot` → `copy_args_to_p` + `JSR vgc_plot`). But the SOUND module
reimplements SID pokes instead of wrapping `audio.s`, and both SOUND and SYSTEM
hand-duplicate a frame-wait that `vgc.s:vgc_vsync` already owns. The lib-call
contract (`lib*.inc` fn-id tables) is hand-maintained with no drift guard.

## Rule

A module NEVER reimplements hardware logic. It `.include`s the NDK driver `.s`,
marshals `LIB_ARG*` → the NDK routine's inputs, `JSR`s the NDK routine, and writes
`LIB_RESULT`/`LIB_STATUS`. If a function isn't in the NDK yet, **add it to the NDK**.
(`audio.s` and other drivers use `.if .referenced(...)` selective emit, so including
a large driver costs only the routines actually called.)

## Part 1 — NDK gains the missing primitives; modules wrap

- `runtime/asm/audio.s`: add `audio_tone` (freq in NVR0L/H, dur in A → sawtooth
  voice 0: master vol, freq, ADSR, gate on, wait, gate off) and `audio_noise`
  (dur in A → noise burst). `audio_volume` already exists (master via AUDIO_VOLUME
  + AUDIO_VOICE).
- `runtime/asm/vgc.s`: add `vgc_wait_frames` (A = N → loop `vgc_vsync`).
  `vgc_vsync` already waits one frame.
- `modules/sound/sound.s`: `.include "audio.s"` (+ vgc for the frame wait, pulled
  transitively by audio_tone). `snd_tone/noise/volume` become thin wrappers around
  `audio_tone/audio_noise/audio_volume`. Delete the SID pokes + local `wait_frames`.
- `modules/system/system.s`: `sys_waitvbl` → `JSR vgc_vsync`; `sys_wait` →
  `JSR vgc_wait_frames`; delete `sys_wait_frames`. (`sys_timer` reads `VGC_FRAME`,
  a one-byte MMIO read — left inline.)

## Part 2 — drift test (`ModuleNdkContractTests`, extends RuntimeLibraryAbiTests)

Per module (graphics/system/sound):
1. **fn-count triple-consistency:** `*_FN_COUNT` in `lib<mod>.inc` == module binary
   header byte `$C007` == count of jtable `.word …-1` entries in `<mod>.s`.
2. **Thin-wrapper enforcement:** the module `.s` must `.include` the NDK driver(s)
   it wraps and must NOT contain NDK-owned hardware idioms directly — no
   `SID_BASE +` writes, no `CMP VGC_FRAME` busy-wait, no `VGC_CMD` issue.
   Marshalling writes (lib mailbox, `VGC_P*` arg registers) remain allowed.

## Verification
- Existing NovaLogo TONE/NOISE/VOLUME/WAIT/WAITVBL/TIMER tests must pass unchanged
  through the rewrapped path (behaviour-preserving).
- New `SoundModule`/`SystemModule` fn-count tests already assert the header; the
  drift test generalizes them. Full suite green.
