# GRAPHICS Module (superset) + Logo Bank-0 Slim — Design

**Date:** 2026-06-04
**Status:** LOCKED via brainstorm 2026-06-04. **Stage 4a DONE 2026-06-04 (1071 B reclaimed, see §6).** Next: 4b (build GRAPHICS module).
**Parent:** `docs/plans/2026-06-03-paged-library-loader-3b-design.md` (the loader ABI, shipped + HW-proven).
This is rollout **Phase 4** ("first paged module + proof") generalized: build GRAPHICS as the canonical
graphics library, then SOUND, then SYSTEM (Phase 5).

---

## 0. Goal (user directive, verbatim)

> "we want to make sure the graphics module contains all functions from the ndk.. we want a superset
> of whatever basic, logo and the ndk has for graphics functions"

So `GRAPHICS` (`MODULE_ID_GRAPHICS = $01`) is **the canonical graphics library = the union of every
graphics op across BASIC, NovaLogo, and the NDK drivers** — built once, called identically by every
runtime through `lib_call`.

---

## 1. The superset fits in one 16 KB module (measured)

The deduped union of all 7 NDK graphics drivers
(`vgc + copper + copper_split + vsprite + blitter + sprite + msprite`) assembles to **3,497 B**; with
`anim/nvg/vtext/tween` and ~110 thin dispatch wrappers it lands at **~8.5–9.5 KB of the 16 KB
`$C000–$FFFF` window — ~6.5 KB headroom.** Coarse-module-per-domain holds for graphics. (Logo's
extension is 12.6 KB only because the text **editor** is 7.4 KB of it — not graphics, and it stays out.)

**~110 distinct ops** span: turtle-rendering · draw-primitives · color/palette · screen-modes ·
hardware sprites · meta-sprites (msprite) · virtual-sprites + scene compositor (vsprite) · copper ·
blitter · DMA-gfx · text-on-gfx · image I/O. About **52 are NDK-only today** (surfaced by no language):
the whole msprite object system, the vsprite scene API, sprite copy/clear/flip/priority, blitter
low-level, anim/nvg. The superset exposes all of them — the point is "build the hardware lib once."

---

## 2. The GRAPHICS contract (`libgraphics.inc`)

Shared `.inc` every runtime + the module compile against. `MODULE_ID_GRAPHICS = $01`; numeric `fn-id`
in grouped ranges (room to grow each area):

```
$00–$0F  draw       GCLS GCOLOR PLOT UNPLOT LINE CIRCLE RECT FILL PAINT GTEXT
$10–$1F  text/mode  COLOR FONT MODE REVERSE/off FLASH/off LOCATE CLS CLSWIN DISPLAYon/off
$20–$3F  hw-sprite  define row clear copy pos enable disable flip priority shape
                    setreg getx gety + collision API (status/mask/clear/read-clear/irq)
$40–$4F  copper     list add clear on off use end split set_reg set_sprite_reg
$50–$5F  blit/dma   blitcopy blitfill blit-start/wait dmacopy dmafill +status/err/count
$60–$7F  vsprite    blit fill rotate save/restore-bg gfx_* + scene compositor
$80–$9F  msprite    spawn destroy show hide pos frame anim priority transcolor tick commit
$A0–$AF  image/mem  GSAVE GLOAD NVGload×4 MEMREAD MEMWRITE VPOKE VPEEK
$B0–$BF  turtle-render  draw-line, draw/rotate turtle-icon sprite, erase-icon, ensure-gfx-mode
```

Each fn = a **thin leaf wrapper**: read canonical mailbox cells (`$0303`…) → call the existing NDK
driver → write `RESULT`/`STATUS`. Pure leaf (recon confirmed: no callbacks into any runtime). Built
`ORG $C000` with the `"NL"` header (`libmod.inc`), `MODULE_ID_GRAPHICS`, dispatched by `FN_ID`.

### 2.1 Canonical signatures (resolving BASIC/Logo conflicts — superset form)
- **All X/Y = 16-bit signed** cells (BASIC's byte-Y → word-Y; matches signed-sprite HW).
- **CIRCLE cx,cy,rx,[ry]** — optional `ry` → ellipse (BASIC's superset form).
- **Sprites:** signed-16 X/Y, index 0–15; flip/priority/copy/clear are first-class fns.
- **Three clears:** `CLS` (full text), `CLSWIN` (active window), `GCLS` (gfx).
- **Color:** `COLOR fg,bg,border` (text) + `GCOLOR c` (gfx pen) distinct; Logo `SETPC`→`GCOLOR`,
  `SETBG`→`COLOR`-bg are foundation-side aliases.
- **Raster origin** for primitives is top-left/+Y-down; the turtle's center-origin Cartesian transform
  stays in the (resident) turtle layer.

---

## 3. The turtle: RESIDENT in Logo's foundation

**Decision (user):** the turtle (FD/BK/RT/LT/SETXY/SETH/HOME/XCOR/YCOR/HEADING/pen/screen-modes) is a
Logo **language abstraction** and stays **resident in bank 0**, NOT in the shared GRAPHICS module.

Why resident (not paged): a turtle that calls GRAPHICS must be resident — it pages GRAPHICS into
bank 1 via `lib_call`. A *paged* turtle can't call a *paged* GRAPHICS (one bank-1 slot). So turtle is
resident; only its **rendering** (line draw, rotate-blit of the turtle icon) lives in GRAPHICS
(fn-ids `$B0–$BF`), invoked via `lib_call(GRAPHICS,…)`.

**Resident turtle ≈ 1.2 KB** — measured from `ext_fd`: its bulk (lines 275–393 of `extension.s`) is
move-math (heading→angle, MATH-copro sincos/mul, sub-pixel 24-bit accumulate into state at `$9F00`);
that's resident language logic. The four `JSR`s to graphics subroutines become `lib_call(GRAPHICS,…)`.
Turtle state stays at `$9F00` (shared RAM, bank-independent). Turtle commands become resident builtins.

---

## 4. Bank-0 slim (the prerequisite) — ~1.4 KB, no callback ABI

Logo's base ROM is **full**: `CODE $C000–$F627`, `RODATA $F628–$FFC7`, `MONITOR $FFD7` → **15 bytes free**.
Seating the ~1.2 KB resident turtle requires reclaiming ~1.4 KB. Four parallel size-reviews found it as
**pure dedup/cleanup (zero behavior change)** — *not* a callback ABI, *not* relocating core:

| Source | Reclaim | How |
|---|---|---|
| `lists.s` | ~500 B | parameterized error printer (~330); shared `fill_cons_from_eval` + `next_cdr` subs (~150); `DOESN'T LIKE` suffix dedup (~45) |
| `builtins.s` | ~270 B | 1-arg reporters set table arity=1 and drop hand-rolled eval/error prologues (~110); parameterized "NOT ENOUGH INPUTS TO " (~115); print/type tail merge; dead `LDY#4/LDA` in lookup_builtin |
| `procedures.s` | ~300 B | dedup the 3 name-compare loops → `proc_name_cmp` (~140); `proc_next` follow-pointer helper; `do_erase` unlink rewrite; share `do_apply`/`do_run` run-input-buf |
| `eval.s`/`tokenizer.s` | ~100 B | factor 6× inline error-printers → `print_err_cstr`; shared token-word print tail; delimiter-class lookup table |

**≈ 1.15 KB clean** + ~0.3 KB from a couple medium-risk shared-loop refactors (cdr-walk, bracket-scan)
→ **~1.4 KB**, covering the 1.2 KB turtle + its dispatch entries.

**Do NOT count** the `ext_cmd_table` + command-name vocabulary (~720 B `eval.s` RODATA): it's the
keyword→`(module,fn)` map the **resident** foundation needs on every command lookup, so it can't move
to a paged module. (The ~1.5 KB of cold builtins — EDIT/PO/POTS/ERASE — *is* relocatable, but only with
the re-entrant callback ABI; not needed for the turtle, deferred.)

Each slim item: zero behavior change, mostly cold paths; the only hot-path touches (cdr-walk,
1-arg-reporter arity flip) are leaf-safe and add no work the dispatcher didn't already do. Gate: the
full NovaLogo suite stays green and `novalogo.bin` shrinks by the reclaimed amount.

---

## 5. Foundation marshalling rewrite (Logo → lib_call)

Localized to `eval.s` (recon-verified): `ext_cmd_table` gains a `module_id` column
(`name_ptr, module_id, fn_id, arity`); `lookup_ext_cmd` writes `LIB_MOD_ID $0300` + `LIB_FN_ID $0301`;
`ext_eval_args` writes four **32-bit cells** at `LIB_ARG0/1/2/3` (`$0303/$0307/$030B/$030F`), mapping
Logo's 16.8 fixed (`FRAC:LO:HI`) into the 16.16 cell (drop the wire `TYPE`/`ARGC`); the 3 call sites
`JSR EXT_TRAMPOLINE` → set mailbox + `JSR lib_call` (`LIB_HOME_BANK` = `ROMSWAP_LOGO`, set at boot);
read back `LIB_RESULT $0313` + check `LIB_STATUS $0302`. Graphics-primitive commands (PLOT/LINE/CIRCLE/
RECT/FILL/PAINT/SETCOLOR + sprites) route to GRAPHICS; turtle commands become resident builtins that
orchestrate `lib_call(GRAPHICS,…)`.

---

## 6. Staged plan

- **4a — Slim bank 0 (~1.4 KB). ✅ DONE 2026-06-04 — reclaimed 1071 B (free 15 → 1086).**
  Plan: `docs/plans/2026-06-04-stage-4a-slim-bank0.md`. The win was NOT §4's per-item estimates (the
  "indexed error-printer table" was a wash) but a **message-composition engine**: no error string stored
  whole — every message composed at print time from shared fragments + the command's existing
  length-prefixed `*_name` string (RODATA −908 B). Plus PRINT/TYPE fall-through merge and `proc_next`/
  `do_erase` dedup (CODE −163 B). Full NovaLogo suite green (102 tests, incl. 10 new exact-text
  characterization tests locking every error family + a middle-erase test). Commits dc33115..ce0190e.
  **Deferred to 4c:** the 1-arg-reporter arity flip (~110 B) — not a clean drop-in (diverges on the
  error path, needs per-reporter characterization, hot-path risk). Revisit only if the measured turtle
  needs the headroom. The name-compare-loop unification was assessed and rejected (3 loops differ in
  source/offset/exit; costs more than it saves on the hot path).
- **4b — Build GRAPHICS module.** `libgraphics.inc` contract (§2) + the module (`ORG $C000`, NDK driver
  bodies + fn-id dispatch + thin leaf wrappers). Stage at boot (`boot.json` `libraries` entry, base
  `$060000` slot 0; reuse the proven page-in + the `nova put`/boot-staging path from Phase 3 §T9).
  Test: MSTest direct-fn-id calls (assert the VGC/blitter/sprite register effects) + a HW smoke.
- **4c — Turtle resident + wire Logo.** Move turtle handlers into the foundation (calling GRAPHICS for
  rendering); move PLOT/LINE/CIRCLE/SPRITE/etc onto `lib_call(GRAPHICS)`; the §5 marshalling rewrite.
  Test: Logo programs (draw, turtle, sprites) on HW through the paged module.
- **Phase 5 — SOUND** (`MODULE_ID_SOUND $02`: TONE/NOISE/VOLUME — trivial, SID MMIO), then **SYSTEM**
  (`$03`: WAIT/WAITVBL/TIMER + the editor/EDIT — the editor is ~7.4 KB, needs BYTES marshalling +
  arg-writeback, likely its own module or resident; revisit).
- Later: migrate **BASIC** onto the same GRAPHICS/SOUND modules (retire its callback-style extension).

---

## 7. Open items / risks
- **EDIT/editor** (~7.4 KB): the outlier — needs `BYTES` marshalling + final-length writeback (no other
  command writes an arg back). Own module or resident; design in Phase 5.
- **`SEI` across `lib_call`:** TONE/NOISE/WAIT busy-wait polling `VGC_FRAME` run with IRQs masked.
  `VGC_FRAME` is free-running MMIO so polling still advances — verify no foundation IRQ work starves
  during long TONE/WAIT.
- **SETPOS** (list arg) + the cons-cell layout coupling: marshal as `BYTES` (ptr into shared heap RAM);
  keep the cons-cell offsets in a shared header so module + foundation agree.
- **Per-fn test coverage** for the ~52 NDK-only ops with no language caller: direct fn-id unit tests
  asserting the hardware-register effect (representative coverage; not all 110 enumerated in 4b).
- **Verify resident-turtle final size** against the reclaimed budget. **4a has landed: 1086 B now free
  in bank 0** (was 15). The resident turtle is estimated ~1.2 KB (~1228 B) — i.e. ~140 B over the
  current headroom if the estimate holds. **4c gating step:** measure the actual resident-turtle
  footprint first. If it exceeds 1086 B, close the gap via the deferred 1-arg-reporter arity flip
  (~110 B, see 4a plan Task 5) and/or further composition (the `"X NEEDS [ BODY ]"` / FOR/WHILE inline
  messages in builtins.s are not yet composed). The turtle estimate is soft and may come in under 1086.

---

## 8. Resume pointer
Full session narrative + verbatim user quotes + the recon evidence are in memory
`project_phase4_graphics_design_2026_06_04.md`. Workflow: work on `main`, no worktrees; ca65/ld65;
`make -C novalogo`; `dotnet test --filter ~NovaLogo`; Verilator + HW smoke as in Phase 3.
Next concrete action: `writing-plans` for **Stage 4a (slim bank 0)**, then subagent-driven execution.
