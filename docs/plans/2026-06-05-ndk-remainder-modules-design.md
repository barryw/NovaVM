# NDK remainder as paged modules — design (2026-06-05)

Expose the rest of the NDK (`runtime/asm/*.s`) as paged-library modules so every
driver is reachable via `lib_call` from any runtime. Continues the consolidation in
`project-logo-rom-consolidation` and the single-source principle in
`feedback-ndk-source-of-truth`. **Modules only** — no runtime (Logo/BASIC) bindings.

## Current coverage

Module-exposed today: GRAPHICS ($01, 118 fns: vgc/sprite/vsprite/msprite/copper/
blitter/dma/nvg/anim/tween), SOUND ($02, 3 of audio's 27: tone/noise/volume),
SYSTEM ($03, editor + wait/timer). Everything else is unreachable via `lib_call`:
audio (24 more), fio (21), xram (17), xmc (18), nic (11), gameserver (44), nui (5),
overlay (8), rng (3), pager (1).

## Roster (domain-grouped — user choice)

| Module | id | NDK drivers | ~fns | action |
|---|---|---|---|---|
| SOUND | $02 | audio (full) | +24 | expand — music engine (MML seq/play/stop/tempo/loop/priority), SID/MIDI playback (sidplay/stop, midplay/stop, *_file), soundfont load, SFX (play_sound/sound/instrument/init/tick), reporters (status/sfx_playing/music_playing/music_note), set_volume |
| FILES | $04 | fio + pager | ~22 | new — save/load, gsave/gload, dir open/read, delete, midplay/midstop, rng, pager page load |
| MEMORY | $05 | xram + xmc | ~35 | new — XRAM copy/fill/peek/poke, XMC window map/config |
| NET | $06 | nic + gameserver | ~55 | new — NIC connect/listen/send/recv/DMA/IRQ, gameserver session/framing |
| SYSTEM | $03 | += nui + rng + overlay | +16 | expand — nui dialogs, rng, overlay load/activate |

Module-ids: $01-$03 taken, $04-$06 new, $7F = TEST. Staging slots are sequential 16 KB
XRAM regions ($60000/$64000/$68000 today → $6C000/$70000/$74000 for $04/$05/$06).

## Per-module pattern (built this session in docs-from-NDK)

1. `runtime/asm/lib<name>.inc` — `MODULE_ID_<NAME>` + `<PFX>_<FN>` fn-ids + `<PFX>_FN_COUNT`.
2. `modules/<name>/<name>.s` — `lib_module_header`, RTS-trick `<pfx>_jtable`, thin wrappers:
   read `LIB_ARG*` mailbox → store to the NDK routine's `@in` registers/pseudo-regs →
   `JSR <ndk_routine>` → write `LIB_RESULT`/`LIB_STATUS`. `;@fn`/`;@ndk`/`;@arg`/`;@ret`
   doc comments. NDK bodies `.include`d AFTER the wrappers (selective `.referenced()` emit
   keeps the image small).
3. `modules/<name>/{Makefile,<name>.cfg}` — assemble + link; `nmod_pack.py --ndk-dir` builds
   the self-documenting `.nmod` (summary single-sourced from the NDK, drift guard verifies
   each wrapper `JSR`s its `;@ndk` routine). `install` copies `.bin` to Avalonia Resources.
4. `e6502.Avalonia/Resources/<name>.bin` committed (Windows CI needs no module rebuild).
5. boot.json + `CompositeBusDevice.StageConfiguredModules()` staging slot.
6. `ModuleNdkContractTests` Modules[] entry → header/jtable + thin-wrapper + `;@ndk` drift checks.

## Verification reality (the tradeoff)

Modules-only (no bindings, no host harness) means each wrapper is verified
**structurally**: it assembles, header fn_count == jtable entries, it pokes no NDK-owned
registers in its own code, and the drift test confirms it `JSR`s the routine its `;@ndk`
maps to. The **arg marshalling is NOT proven end-to-end** until a caller exercises it —
that proof arrives when runtimes bind these fns (or a later host harness). The drift test
is made as strong as the freeform `@in` annotations allow, to catch missed-arg omissions.

## Phasing — one module per phase, each shippable + suite-green

① SOUND (audio full) — the template for expansion · ② FILES · ③ MEMORY · ④ NET ·
⑤ SYSTEM additions. Each phase: spec the fn-ids from the NDK `@symbol`/`@in`/`@out`
surface → 6510 agent implements the wrappers following the pattern → build + `.nmod` +
drift test + full suite green → commit. Boot/staging + Resources committed per phase.

## Out of scope

Runtime command bindings (Logo tokens, BASIC commands), host-side functional harness,
EhBASIC. These come per-runtime as the capabilities are actually used.
