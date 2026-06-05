# Handoff — NovaLogo ROM consolidation + NDK-source-of-truth (2026-06-05)

## TL;DR

NovaLogo is now **one 16 KB base ROM + three shared paged modules (graphics `$01`,
system `$03`, sound `$02`), with NO extension ROM**. The NDK (`runtime/asm/*.s`) is
the **single source of truth** for hardware functions; modules are thin `lib_call`
wrappers. All shipped + pushed to `main`, full suite green (1543/0/12).

**Two things are still open:** (A) **docs-from-NDK** (Part 3 — module docs should be
generated from the NDK, not re-written in the module `.s`), and (B) the **original
goal: repatriate the turtle** from the graphics module into the Logo base ROM
(Phase C). Both described below.

## Shipped this session (all on `main`)

| Commit | What |
|---|---|
| `97ff8a1` | fix(libabi): guard FIO register defs with `.ifndef` (latent collision with nova.inc that broke any clean novalogo/graphics rebuild) |
| `8fd6f2b` | **Phase A** — editor moved from Logo extension ROM → SYSTEM module (`SYS_FN_EDIT`); Logo `TO`/`EDIT` → `lib_call(SYSTEM)` |
| `b4f3332` | **Phase B** — deleted the Logo extension ROM; TONE/NOISE/VOLUME → SOUND module, WAIT/WAITVBL/TIMER → SYSTEM; removed legacy `EXT_TRAMPOLINE`/`ensure_ext_resident` machinery |
| `12014ae` | **NDK-sync Parts 1+2** — sound/system rewritten as thin NDK wrappers; `audio_tone`/`audio_noise` added to `audio.s`, `vgc_wait_frames` to `vgc.s` (reference-only); `ModuleNdkContractTests` drift guard |

Design docs: `docs/plans/2026-06-05-system-module-editor-migration-design.md`,
`2026-06-05-phase-b-delete-logo-extension-rom.md`,
`2026-06-05-ndk-source-of-truth-module-wrappers.md`.

## Current architecture (key facts)

- **Modules** (`modules/{graphics,system,sound}/`): each is a 16 KB ROM with a
  `lib_module_header` (magic `NL`, id, abi, fn_count at `$C007`), an RTS-trick
  jtable on `LIB_FN_ID`, and thin wrappers that marshal `LIB_ARG*` ($0303…) into
  the NDK routine's inputs and `JSR` it. NDK driver bodies are `.include`d at the
  **bottom** (after the wrappers) so ca65 `.referenced()` selective-emit only emits
  the routines actually called → modules stay tiny.
- **NDK = source of truth** ([[feedback-ndk-source-of-truth]]): modules never
  reimplement hardware logic. New primitives added this session are **reference-only**
  guarded (`.if .referenced(x)`, NOT `*_EMIT_ALL`) because the **BASIC ROM is at
  capacity** and links `audio.s`/`vgc.s` wholesale — anything in EMIT_ALL overflows it.
- **Module staging**: `CompositeBusDevice.StageConfiguredModules()` pre-stages
  graphics→slot0, system→slot1, sound→slot2 from `Resources/*.bin` at boot (mirrors
  firmware `boot.json`). Module `.bin`s are committed in `Resources/` + `modules/*/`;
  `.o/.map/.lst/.nmod` are gitignored.
- **Drift guard**: `e6502UnitTests/ModuleNdkContractTests.cs` — header fn_count ==
  jtable entry count, and module-own-code (before NDK `.include`s) has no `SID_BASE+`
  pokes / `CMP VGC_FRAME` busy-wait.
- Base Logo ROM: **~647 bytes free**.

## Build / test gotchas (IMPORTANT)

- `dotnet test -p:SkipRomBuild=true --no-build` uses `Resources/*.bin` AS-IS. After
  changing ROM/module sources you MUST `make -C <dir> install` to refresh
  `e6502.Avalonia/Resources/`, then rebuild the test project (CopyToOutputDirectory
  propagates to test output). `cmp <src>.bin <testout>/Resources/<x>.bin` to confirm
  fresh — a stale ROM gives false-positive passes (cost a false green this session).
- Module build: `make -C modules/<m>` then `make -C modules/<m> install`.
- Deleting a runtime subsystem orphans its dedicated tests — sweep for them (this
  session removed EXT.TEST tests + a legacy-repage test).
- HW deploy still needs `sound.nmod`/`system.nmod` copied to the device SD `/lib/`
  and `novalogo_ext.bin` removed (a deploy step, not code).

## OPEN WORK

### A. docs-from-NDK (Part 3 of NDK-sync) — user-requested
User: *"our module documentation should probably come from the ndk as well.. no
need to document in 2 places"*. Today modules re-document fns as `;@fn/;@brief/;@arg`
(consumed by `tools/nmod_pack.py` → `.nmod`), duplicating the NDK drivers'
`; @symbol/@summary/@in/@out` annotations.
Plan: add a **machine-readable fn-id → NDK-routine mapping** (today only an informal
comment in `libgraphics.inc`, e.g. "GFN_PLOT -> vgc_plot"); then `nmod_pack.py` pulls
each fn's doc from the wrapped NDK routine, modules stop carrying prose, AND the drift
test gains "wrapper actually calls the NDK routine it maps to" (closes the last
hand-maintained gap). Not started.

### B. Phase C — repatriate the turtle (the ORIGINAL goal)
The thing that kicked this all off: user is unhappy the **turtle lives in the graphics
module** (`modules/graphics/graphics.s` `gfn_turtle_*`, ~4.5 KB) — *"that is a _logo_
concern, not a _graphics_ concern"*. Move turtle **semantics** (FD/BK/RT/LT math,
pen/heading/position state, HOME/SETXY, XCOR/YCOR/HEADING reporters, screen-mode setup,
the FD→line orchestration) into the **Logo base ROM**; keep **rendering** (rotate-blit
+ Amiga-BOB save/restore) as a **generic graphics-module primitive** the Logo turtle
calls via `lib_call` (so graphics keeps zero turtle-specific code). Est. ~2–2.5 KB
returns to the base ROM (647 B free now) — measure the fit; reclaim by deleting the
`logo_adapt_turtle` adapter (`eval.s:~1793`) + turtle-specific dispatch. Do Part A
first (clean doc pipeline) per user sequencing, OR confirm with user.

## Memory pointers
`feedback_ndk_source_of_truth.md`, `feedback_rom_domain_placement.md`,
`project_logo_rom_consolidation.md`, `reference_novalogo_build.md`.
