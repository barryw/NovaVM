# BASIC-on-Modules — Design

**Date:** 2026-06-06
**Status:** Design validated; ready to plan Phase 0.
**Goal (user):** "basic should now be using modules like logo does. there should be no
functionality baked into the basic rom (main rom or extended rom) that already lives in
modules." And: "eventually basic should have no extension rom.. it should leverage modules."

## Why

EhBASIC compiles the NDK runtime implementations directly into its ROMs (`vgc.s`,
`sprite.s`, `audio.s`, `nic.s` in main; `blitter.s`, `copper.s`, `audio.s`, `rng.s`,
`xram.s`, `xmc.s` in extension). The shared-library **modules**
(`graphics`/`sound`/`system`/`files`/`memory`/`net`/`turtle`) are thin dispatchers wrapping
those *same* `.s` files. So every hardware keyword in BASIC duplicates code that already
lives in a paged module. BASIC's **main ROM is full** (`CODE = $3FD3` of `$3FD7` — 4 bytes
free); this duplication is the wall the product vision keeps hitting (no room for `WAITVBL`,
`JOY()`, etc.). Routing BASIC through `lib_call` lets those `.s` includes leave the ROM
entirely, freeing thousands of bytes and unifying the platform on one implementation.

## End state

BASIC mirrors NovaLogo: a **single 16KB main ROM, no extension ROM**. Every hardware keyword
routes through `lib_call(MODULE_ID, FN_ID)`. The only baked-in residual is what has **no
module home** — the **math coprocessor** MMIO handlers (`$BB20`), `ADDR` + its
`runtime_labels.inc` table, and `BITSET/BITCLR/BITTGL` — folded into the now-spacious main ROM.

## Decisions (locked)

1. **Incremental by domain.** Build the dispatch infra + boot bridge first, prove with ONE
   domain (graphics — biggest ROM win, best test coverage), full suite green, commit; then one
   domain per phase. Bisectable; measurable bytes-freed per step.
2. **Both targets** (Avalonia + FPGA/NovaHost; modules stream from SD like Logo). FPGA smoke
   **batched** after Avalonia is green, not per-domain.
3. **Eliminate the ext ROM** as the final phase. Fallback if main ROM can't absorb the residual:
   keep a minimal ext ROM via the `ensure_ext_resident` coexistence bridge.
4. **Verify** with the full Avalonia BASIC suite per domain (must stay 100% green) + report ROM
   bytes freed. FPGA hardware smoke batched.

## Architecture facts (verified)

- **`lib_call` ABI:** mailbox at `$0300–$031F` (`LIB_MOD_ID`/`LIB_FN_ID`/`LIB_ARG0–3` 32-bit
  LE/`LIB_RESULT`/`LIB_STATUS`/`LIB_HOME_BANK`/`LIB_RESIDENT`), `JSR LIB_LOADER_BAND` (`$0320`).
  Modules are 16KB, demand-loaded SD→XRAM 4-slot shelf (`SHELF_BASE $060000`), PGD-paged into
  bank 1 (`ROMSWAP_EXTENSION=$04`). Module ids: GRAPHICS `$01`, SOUND `$02`, SYSTEM `$03`,
  FILES `$04`, MEMORY `$05`, NET `$06`, TURTLE `$07`. Coverage is complete for BASIC's hardware
  vocabulary; `VPEEK/VPOKE/MEMREAD/MEMWRITE/GSAVE/GLOAD` = GRAPHICS `$A0–$A9`. **No MATH module.**
- **Infra is already present and unconditional:** `libcall.bin` (the `$0320` loader) is POKEd at
  every boot; GRAPHICS/SYSTEM/SOUND are pre-staged into the shelf and the full module store is
  set regardless of boot ROM. **BASIC can call `lib_call` today.**
- **Bank conflict:** `_extBank` (CompositeBusDevice) is a single 16KB overlay holding *either*
  BASIC's static ext ROM *or* a paged module. A `lib_call` page-in **clobbers** the ext ROM;
  `RomSwapBasic`/`RomSwapLogo` deliberately don't reload it. Logo avoided this by having no ext
  ROM.
- **Coexistence bridge exists in spec only (4c.0c):** `LoadExtBankStatic` stages BASIC's ext ROM
  into XRAM `$07C000` (`HOST_EXT_XRAM`) as a re-page source; `LIB_RESIDENT` markers are `$00`
  none / `$01–7F` module / `$FF` host-ext. But the routine that consumes this —
  `ensure_ext_resident` — is **documented in `libabi.inc:136–147` and NOT implemented** in
  `libcall.s`. BASIC would be the first runtime to combine a static ext ROM with module paging.
- **BASIC boot wires none of this:** `min_mon.asm` sets neither `LIB_HOME_BANK` nor
  `LIB_RESIDENT`, and the `EXT_CODE` trampoline does not re-page.
- **FPGA:** the ROM is streamed from NovaHost (not in the bitstream), so BASIC ROM changes need
  no re-synth. If FPGA is only flashed at the ext-ROM-eliminated end state, NovaHost never needs
  the `$07C000` ext-staging firmware follow-on.

## The migration bridge (the hard part)

Main ROM is full and the ext ROM serves many keywords, so conversion can't be atomic. During the
transition BASIC runs with **both** its ext ROM and `lib_call` active. Any `lib_call` page-in
clobbers `_extBank`, so a later `EXT_CODE` keyword would execute the leftover module. Therefore:

1. **Implement `ensure_ext_resident`** (4c.0c): if `LIB_RESIDENT != $FF`, PGD-re-page the ext ROM
   from XRAM `$07C000` into bank 1, set `LIB_RESIDENT=$FF`, then proceed.
2. **Arm `lib_call` in `min_mon` boot:** `LIB_HOME_BANK=ROMSWAP_BASIC`, `LIB_RESIDENT=$FF` (ext
   ROM resident at boot).
3. **`EXT_CODE` calls `ensure_ext_resident`** before `JSR $C000`.

A green suite with *zero* keywords converted proves the bridge is invisible.

## Phases

- **Phase 0 — infra + bridge.** Implement `ensure_ext_resident`; arm `lib_call` in boot; make
  `EXT_CODE` re-page-safe; add the shared `basic_lib_call` helper. Gate: full suite green with no
  keyword conversions. (Riskiest phase, isolated.)
- **Phases 1–N — one domain per phase**, ordered by payoff/risk: **graphics** (drops `vgc.s` +
  `sprite.s`), then **sound** (`audio.s`), **net** (`nic.s`), then the ext-ROM-resident domains
  (copper/blitter/dma, xram/xmc, `rng.s`). Each: convert handlers to `basic_lib_call`, drop the
  `.s` include, full suite green, report bytes freed, commit.
- **Final phase — eliminate the ext ROM.** Fold the residual (math copro, `ADDR`/labels,
  `BITSET`) into main ROM; delete `extension.s`/`extension.cfg`/the `EXT_CODE` trampoline/
  `ensure_ext_resident`/the `$07C000` staging. BASIC = single 16KB ROM. Run the batched FPGA smoke.

## The dispatch helper

Keep each token's existing **parsing** (small; handles sub-keywords like `SPRITE…COLLISION`,
`MUSIC…TEMPO`). Replace only its **execution**: today a handler ends with `STA VGC_P0 … STA
VGC_CMD` or a `FIO_CMD` poke + `JSR vgc_plot`; after, marshal the parsed args into `LIB_ARG0–3`
(sign-extend s16→32-bit LE) and `JSR basic_lib_call` with the module/fn id; reporters convert
`LIB_RESULT`→FAC. The ROM savings come from dropping the included implementations, not the parsing.

## Reverse direction — harvest BASIC functionality into the NDK (parallel workstream)

User: "find things implemented in basic that should be pulled into the ndk... whatever we can
pull out and reuse we absolutely should."

As we touch each domain, flag any capability BASIC implements that **no module provides yet**, and
evaluate promoting it into the NDK/module so Logo/Forth/Pascal/etc. inherit it. Maintain a running
candidate list. Seed candidates (to confirm during the relevant phase):

- **Sprite / vsprite collision** — hardware-sprite `COLLISION(n)`/`BUMPED(n)` already call
  `sprite.s` (in the NDK). **Verify whether vsprite (software-sprite) collision logic exists in a
  module or only in BASIC**; if BASIC-only, promote it (likely into GRAPHICS vsprite `$60–$71`).
- **MML** — parsing is host-side (C# `MmlParser`/`MusicEngine`) reached via `FIO_CMD`/SOUND; the
  6502 side just forwards strings. Confirm there's nothing BASIC-side worth extracting; ensure the
  SOUND-module path is the single reuse point so Logo can drive MML too.
- **Anything else surfaced per domain:** number formatting, string helpers, geometry, easing, etc.
  — if a module doesn't have it and another runtime would want it, add to the list and extract.

Output: a `docs/plans/ndk-harvest-candidates.md` checklist, updated each phase, with a promote/skip
decision per item.

## Risks

- **`ensure_ext_resident` is unbuilt + untested** (BASIC is the first dual-mode runtime). Phase 0
  must prove it before any conversion. HW side (`$07C000` staging) is emulator-only today; avoid by
  flashing FPGA only at the ext-ROM-eliminated end state.
- **Residual may not fit main ROM** until enough includes are removed → order frees space first;
  ext-ROM elimination is last. Fallback: keep a minimal ext ROM via the bridge.
- **`lib_call` clobbers A/X/Y** and uses the `$0300` mailbox — audit BASIC handlers that assume
  registers/zero-page survive across the old direct-MMIO path.
- **Per-call page-in latency** on FPGA (16KB copy) for keywords in tight loops; acceptable for the
  arcade-vocabulary target, but note hot paths (e.g. `PLOT` in a pixel loop) for possible batching.

## Verification

Full Avalonia BASIC test suite green after every phase; ROM bytes-freed reported per domain;
targeted tests added where a domain is thin on coverage; FPGA hardware smoke batched at the
ext-ROM-eliminated milestone.
