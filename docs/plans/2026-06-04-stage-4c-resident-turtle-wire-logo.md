# Stage 4c — Resident turtle + wire Logo onto lib_call(GRAPHICS) Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to execute this plan task-by-task. Read this whole file first; it carries the recon so the executor has zero-context guidance.

**Goal:** Migrate NovaLogo's graphics off its private extension ROM and onto the shared, HW-proven GRAPHICS paged module (`lib_call`), and make the turtle resident in Logo's foundation with only its *rendering* in GRAPHICS.

**Architecture:** Logo's `ext_cmd_table`/`EXT_TRAMPOLINE` path (eval.s) is rewritten to drive the canonical `lib_call` mailbox instead of Logo's own extension ROM. Direct graphics commands (PLOT/LINE/CIRCLE/…) route table→`lib_call(GRAPHICS, GFN_*)`. The turtle (FD/BK/RT/…) becomes resident builtins doing move-math in the foundation + `lib_call(GRAPHICS, $Bx)` for rendering.

**Tech Stack:** ca65/ld65 (65C02), MSTest (`~NovaLogo`, `~GraphicsModule`), NovaHost TCP CLI (`nova`) for HW. **NovaHost is TCP-ONLY — never use HTTP/REST; rebuild the AOT `nova` binary if the CLI source changed (see [[feedback_novahost_tcp_only]]).**

---

## Recon carried forward (so the executor needn't re-derive)

- **GRAPHICS module = DONE + silicon-proven** (Stage 4b, commits dc33115..ccefa90). Contract: `runtime/asm/libgraphics.inc` — `MODULE_ID_GRAPHICS=$01`, `GFN_*` ids: draw `$00-$09`, text/mode `$10-$1B`, hw-sprite `$20-$3B`, copper `$40-$49`, blit/dma `$50-$5B`, vsprite `$60-$71`, msprite `$80-$8B`, image/mem `$A0-$A9`, anim `$C0-$C7`, tween `$D0-$DA`. Turtle-render `$B0-$BF` is RESERVED (this plan lands it).
- **lib_call ABI** (`runtime/asm/libabi.inc`): write `LIB_MOD_ID=$0300`, `LIB_FN_ID=$0301`, `LIB_ARG0..3=$0303/$0307/$030B/$030F` (32-bit LE), `JSR lib_call` ($9C00 resident loader; or the runtime's resident copy), read `LIB_RESULT=$0313` + `LIB_STATUS=$0302`. The loader's `modtab_lookup` already resolves GRAPHICS→XRAM slot 0. graphics.mod is staged on the board's SD + in boot.json.
- **§5 marshalling path (eval.s, current):** `ext_cmd_table` entries = `{name_ptr(2), EXT_CMD_xxx(1), arity(1)}`. THREE call sites (~lines 151, 300, 463) do `JSR lookup_ext_cmd` (→ A=arity, EXT_CMD set) `→ JSR ext_eval_args` (writes EXT_ARGn, 16.8 fixed) `→ JSR EXT_TRAMPOLINE` `→` read `EXT_RESULT_TYPE/HI/LO/FRAC`. `lookup_ext_cmd` body at eval.s:1321.
- **Turtle (resident target):** currently `ext_fd` etc. in `novalogo/extension.s`; move-math uses MATH copro sincos/mul + 24-bit sub-pixel accumulate into turtle state at `$9F00`. Rendering subs in extension.s: `draw_turtle_sprite`, `erase_turtle_sprite`, `draw_line`, `ensure_gfx_mode` (+ `turtle_init`); icon buffers at `TURTLE_BUF_BASE`, bg-save buffer, `turtle_bg_x/y`. Turtle state $9F00 is shared RAM (bank-independent).
- **Logo memory map (CRITICAL for the band):** `HEAP_START=$0400`, `HEAP_END=$9800` (cons heap $0400-$97FF); `$9800-$9BFF` editor BSS; `$9C00-$9F10` pinned turtle work+state; mailbox `$0300-$031F`; BSS segment (novalogo.cfg) `$0320-$0D23` (incl. `proc_body_buf` .res 2048). **The BSS segment overlaps the heap at `$0400` — an invariant NOT yet understood. Resolve before relocating anything (task 4c.2-0).**
- **Module BSS need (for vsprite/msprite-backed turtle render):** `$0320-$0450` (305 B) + ZP `$14-$1E` (provisional in graphics.cfg). Only vsprite/msprite ops touch it; draw/text/sprite-reg/copper/blit ops do NOT (they use VGC_P MMIO + LIB_SCRATCH).

---

## Stage 4c.1 — Marshalling rewrite + wire non-BSS graphics commands (NO band needed)

Direct graphics commands (PLOT/LINE/CIRCLE/RECT/FILL/PAINT/SETPC→GCOLOR/SETBG→COLOR-bg + the sprite commands) use GFN ids that never touch the module BSS band, so this stage needs no carve.

**4c.1-1: add a `module_id` column to `ext_cmd_table`.** Entry becomes `{name_ptr(2), module_id(1), fn_id(1), arity(1)}` (5 B). For commands routing to GRAPHICS, `module_id=MODULE_ID_GRAPHICS`, `fn_id=GFN_*`. For commands staying resident/Logo-ext (turtle, anything not yet migrated), use a sentinel `module_id=$00` meaning "resident/legacy path". Update the harness test (`NovaLogoHarnessTests` reads `ext_cmd_table`) for the new stride.

**4c.1-2: rewrite the marshalling.** `lookup_ext_cmd` returns module_id + fn_id + arity. `ext_eval_args` writes the 32-bit `LIB_ARG0..3` cells (map Logo's 16.8 fixed `FRAC:LO:HI` → the 16.16 cell; drop the wire TYPE/ARGC). The 3 call sites: if `module_id != $00` → set `LIB_MOD_ID/LIB_FN_ID`, `JSR lib_call`, read `LIB_RESULT` (→ Logo value) + check `LIB_STATUS`; else → the existing resident/legacy path. Set `LIB_HOME_BANK = ROMSWAP_LOGO` at boot.

**4c.1-3: map the direct graphics commands** in `ext_cmd_table` to `(MODULE_ID_GRAPHICS, GFN_*)`: PLOT→`GFN_PLOT`, LINE→`GFN_LINE`, CIRCLE→`GFN_CIRCLE`, RECT→`GFN_RECT`, FILL→`GFN_FILL`, PAINT→`GFN_PAINT`, SETPC→`GFN_GCOLOR`, SETBG→`GFN_COLOR`(bg), the sprite commands→`GFN_SPR_*`, CLS/GCLS/COLOR/etc. Confirm each Logo command's arg order matches the GFN arg-cell layout (libgraphics.inc); add foundation-side aliases where Logo's name differs.

**Test 4c.1:** `dotnet test --filter ~NovaLogo` green; add tests that a Logo program (`PLOT`, `LINE`, `CIRCLE`, sprite ops) produces the right VGC effect *through lib_call(GRAPHICS)* (not the old extension path). `~GraphicsModule` stays green. Commit per logical step.

---

## Stage 4c.2 — Band carve + resident turtle + turtle-render $B0

**4c.2-0 (investigation, FIRST):** Resolve Logo's `$0400` heap / `$0320-$0D23` BSS overlap. Read the actual placement of `proc_body_buf` + how the heap and static buffers coexist (they apparently overlap — understand why before moving anything). Output: a definitive Logo low-RAM map. Without this, do NOT relocate BSS/heap.

**4c.2-1: carve the module-BSS band.** Reserve a band after the mailbox sized to the module need (`$0320-$0450`, 305 B + headroom → e.g. `$0320-$04FF`). Move Logo's BSS start AND `HEAP_START` above it; move EhBASIC `Ram_base` (basic.asm:616) above it for cross-runtime consistency (the mailbox is reserved in both — same philosophy). Extend BOTH sentinel tests (`NovaLogoMailboxReservationTests`, `MailboxReservationTests`) to cover the band. Gate: full `~NovaLogo` + BASIC suites green; `module MODBSS ⊆ reserved band`.

**4c.2-2: turtle-render fns in GRAPHICS ($B0-$BF).** Port `draw_turtle_sprite`/`erase_turtle_sprite`/`draw_line`/`ensure_gfx_mode` (novalogo/extension.s) into the GRAPHICS module as `GFN_TURTLE_*` wrappers over vsprite (rotate-blit icon) + vgc (line/mode). Decide the interface: turtle position/heading/icon passed via the mailbox/BYTES; persistent icon buffers live in the module BSS band. Add to libgraphics.inc + graphics.s + GraphicsModuleTests (assert the rotated-icon pixels). This is the seam — co-design the resident-vs-passed split here.

**4c.2-3: turtle resident in the foundation.** Move the turtle move-math (FD/BK/RT/LT/SETXY/SETH/HOME/XCOR/YCOR/HEADING/pen) into Logo's foundation as resident builtins; each does its math (MATH copro) on turtle state ($9F00), then `lib_call(GRAPHICS, GFN_TURTLE_*)` for rendering. Remove the turtle + graphics drivers from novalogo_ext.bin (Logo's extension shrinks toward just the editor). Verify the reclaimed bank-0 headroom (4a freed 1086 B) seats the resident turtle.

**Test 4c.2:** Logo turtle programs (FD/RT/REPEAT/etc.) draw correctly on the emulator AND on HW (stage updated novalogo.bin/ext via `nova` TCP; run a turtle program; observe). `~NovaLogo` + `~GraphicsModule` green.

---

## Open questions to settle during execution (brainstorm at 4c.2 start)
- Resident-vs-passed turtle-state split (what the turtle-render fns receive vs read from $9F00).
- Band size/placement final number (after 4c.2-0 maps Logo RAM).
- Whether BASIC's `Ram_base` moves now (consistency) or at BASIC-migration (design §6 "Later").

## Done criteria
1. Logo graphics commands run through `lib_call(GRAPHICS)`, not the private extension. `~NovaLogo` + `~GraphicsModule` green throughout.
2. Turtle resident in bank 0; rendering via GRAPHICS `$B0-$BF`; turtle programs work on HW.
3. Module-BSS band reserved cross-runtime; sentinel tests cover it.
4. novalogo_ext.bin reduced (graphics drivers removed; editor + residue only).

## Resume pointer
Memory: [[project_phase4b_graphics_module_2026_06_04]] (4b done), [[project_phase4_graphics_design_2026_06_04]] (parent design §3 turtle-resident, §5 marshalling). Parent design doc: `docs/plans/2026-06-04-graphics-module-and-bank0-slim-design.md`.
