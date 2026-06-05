# Phase B: evict the rest of the Logo extension ROM into modules, then delete it

**Date:** 2026-06-05
**Status:** Approved (B1 + B2), implementing
**Part of:** Logo ROM consolidation. Follows Phase A (editor → SYSTEM module).

## Goal

Move the last six extension handlers into shared modules, remove the obsolete
`EXT.TEST`, then **delete the NovaLogo extension ROM entirely** along with its
foundation/emulator/firmware machinery. End state: NovaLogo is one 16 KB ROM +
shared modules, with no extension ROM and no `REG_ROMSWAP=$04`-to-host-ext path.

## What's left in `novalogo/extension.s` (322 B)

| Handler | Cmd | New home |
|---|---|---|
| `ext_tone`   | $45 | sound module ($02) `SND_TONE` |
| `ext_noise`  | $46 | sound module `SND_NOISE` |
| `ext_volume` | $47 | sound module `SND_VOLUME` |
| `ext_wait`   | $48 | system module ($03) `SYS_FN_WAIT` |
| `ext_waitvbl`| $49 | system module `SYS_FN_WAITVBL` |
| `ext_timer`  | $4A | system module `SYS_FN_TIMER` |
| `ext_test`   | $01 | **deleted** (proved the legacy round-trip; obsolete) |

All are SID/VGC MMIO pokes (survive banking) + a shared `wait_frames` helper.

## B1 — evict handlers
- `modules/sound/` (new): `sound.s` (ports tone/noise/volume + `wait_frames`,
  reads `LIB_ARG*`), `sound.cfg`, `Makefile`; `runtime/asm/libsound.inc`
  (`SND_TONE=$00`, `SND_NOISE=$01`, `SND_VOLUME=$02`, `SND_FN_COUNT=$03`).
- `modules/system/`: add `SYS_FN_WAIT=$01`, `SYS_FN_WAITVBL=$02`, `SYS_FN_TIMER=$03`
  (`SYS_FN_COUNT=$04`); port the three timing handlers + `wait_frames`.
- `novalogo/eval.s`: retarget the six `ext_cmd_table` entries
  (`MODULE_ID_NONE`→`SOUND`/`SYSTEM`, fn-ids); include `libsound.inc`. `@lib_call`
  already marshals args and reads `LIB_RESULT` (TIMER's reporter result).
- Remove `EXT.TEST`: its `ext_cmd_table` entry + the `NovaLogoTests`/
  `NovaLogoHarnessTests` cases that exercise it.

## B2 — delete the extension ROM
- After B1 nothing maps `MODULE_ID_NONE`, so the foundation legacy path is dead:
  remove the `@legacy` branch, `EXT_TRAMPOLINE`, `ensure_ext_resident` from
  `eval.s` (Logo only — BASIC keeps its own separate extension).
- Delete `novalogo/extension.s`, `novalogo/extension.cfg`, `novalogo_ext.bin`,
  `Resources/novalogo_ext.bin`; drop the `novalogo install` ext copy.
- Avalonia `CompositeBusDevice`: remove `_logoExtRom` + the Logo branch of
  `LoadExtBankStatic` / HOST_EXT XRAM staging; csproj `novalogo_ext.bin` wiring.
- Auto-stage the sound module (slot 2, id $02); add it to `boot.json` `libraries[]`.

## Tests
- Sound module header test; Logo `lib_call` round-trips: `TONE`/`VOLUME` (audible
  effect or SID-register assert), `WAIT`/`WAITVBL` (frame advance), `TIMER`
  (reporter value). Full suite green.
