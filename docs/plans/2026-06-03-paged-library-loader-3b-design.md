# Paged Library Loader + Module Format (Phase 3 / step 3b) — Design

**Date:** 2026-06-03
**Status:** LOCKED via brainstorm 2026-06-03. Ready for `writing-plans`.
**Parent design:** `docs/plans/2026-06-02-paged-runtime-libraries-and-xram-design.md` (§5 dispatch
model, §6 rollout step 3, §7 open items). This doc closes §7's "module header bytes" and
"boot staging," and supersedes §5's *flat-command-id* assumption with explicit numeric
`(module-id, fn-id)` (see §1).
**Engine status:** the XRAM→`ext_rom` page-in silicon it depends on is SHIPPED and byte-exact on
HW (`main @ b339d49`). This is the *software* layer on top.

---

## 0. Scope

Turn bank-1 `ext_rom` from a single fixed extension ROM into a **paged shared-library overlay**:
bank 0 = per-runtime foundation + a resident loader (`lib_call`); bank 1 = the one active library
module (16 K), paged in on demand via the working `$BA76` page-in. The same modules
(GRAPHICS / SOUND / SYSTEM) are built **once** and called **identically** from BASIC, Logo,
Pascal, Forth.

The user's framing for this session (verbatim): *"what's our ideal end state? nevermind what we
have now — what would be the ideal for maximum flexibility and reuse of these shared libraries?
each runtime should call these libraries the same way."*

---

## 1. The ideal calling model (LOCKED)

You cannot make a Forth cell and a BASIC float identical, but you **can** make the ABI boundary
identical and push the unavoidable type conversion into a thin per-language shim. The shared
libraries stay 100% runtime-blind.

- **Leaf-only, strictly one-way (runtime → library).** A library never calls back into the calling
  runtime's parser or data structures. This is not just a rule — **the memory map enforces it**:
  a module paged into bank 1 occupies `$C000-$FFFF`, which *unmaps the bank-0 foundation*. A
  running library physically cannot `JSR` into resident runtime code; only low-RAM (`$0000-$BFFF`)
  and MMIO survive the swap. Structured args (a Logo list, a BASIC array) are **flattened to a
  `BYTES` buffer by the per-runtime shim before the call** — the library walks a flat buffer, never
  the runtime's collection. The §7 re-entrant-callback ABI stays deferred (likely never needed).
- **Numeric `(module-id, fn-id)` addressing**, defined once in a shared `.inc` (the successor to
  `novalogo/ext_iface.inc`). Name-binding was considered and **rejected**: it only pays off for
  independent/third-party module evolution, which a single repo does not have — here the `.inc`
  *is* the registry, and numeric is smaller (no name strings), faster (`table[fn-id]` direct jump),
  and needs no load-time string resolution. (A human-readable name is *not* stored in the module
  header — the XRAM directory already names each module; see §5.)
- **Per-runtime marshalling shim.** Each language's foundation evaluates native values, marshals
  them into the canonical mailbox, `JSR lib_call`, then unmarshals the result. "Calls the same way"
  = identical mailbox layout + one `lib_call` entry; the shim is the only per-language code.

This replaces today's split: Logo's ext ABI is already leaf (pre-evaluated ZP mailbox); **BASIC's
is callback** (handlers re-enter BASIC's parser via the `EXT_GTBY/GTWRD/GTSW` bridges). In the
ideal, BASIC's resident foundation parses/evaluates and hands the library pre-evaluated values —
parsing never leaves the runtime. (BASIC's migration onto this model is rollout Phase 5.)

---

## 2. The canonical mailbox (LOCKED)

Value model: neutral boundary types, **32-bit signed cells**.
- `INT` — 32-bit signed LE; also carries **16.16 fixed-point** (the function's signature says which).
- `BYTES` — `[ptr_lo, ptr_hi, len_lo, len_hi]`: a 16-bit main-RAM pointer + 16-bit length, fits one
  4-byte cell (libraries can't address XRAM directly, so pointers are always main-RAM 16-bit).
- `BOOL` — byte 0 = 0/1.

**Types are a compile-time contract in the `.inc`, not transmitted.** Each `(module, fn)` has a
fixed signature, so the shim and module both already know the types — no per-slot type byte on the
wire. Wire layout (**23 bytes**):

```
+0  MOD_ID   (1)   target module
+1  FN_ID    (1)   function within module
+2  STATUS   (1)   return: 0 = OK, nonzero = error code
+3  ARG0     (4)   32-bit cell, interpreted per signature (INT / FIXED / BYTES / BOOL)
+7  ARG1     (4)
+11 ARG2     (4)
+15 ARG3     (4)
+19 RESULT   (4)
```

4 arg slots cover the richest existing commands (`LINE x1 y1 x2 y2`, `EDIT`); more-than-4 or
structured → one `BYTES`. `ARG_MAX` is a one-constant bump. A `BYTES` *result* is returned by the
caller passing a buffer in and the library returning a length as `INT` → one `RESULT` slot suffices.
`ARGC` is dropped (the fn-id implies arg count via signature). Polymorphic slots (lost by dropping
type bytes) — no hardware lib has them; a leading `INT` selector covers it if ever needed.

### 2.1 Placement: **fixed low-RAM**, not ZP (LOCKED — see audit, §3)

The mailbox + module-private scratch live in a **fixed canonical low-RAM block** (page `$02`/`$03`
neighborhood, where the cross-runtime trampoline/vectors already live — exact base TBD by a
low-RAM audit). Only **~2-4 always-free ZP bytes at `$14-$17`** are reserved for the module's
`(zp),Y` `BYTES` dereference. Module arg reads are absolute instead of ZP — ~1 cycle more, irrelevant
for coarse library calls.

---

## 3. Why low-RAM, not ZP — the cross-runtime ZP audit

The instinct was to put the mailbox in ZP (fast, 6502-native) and pay for it with ZP the reorg
frees. A two-runtime ZP audit (verified file:line) plus a correction found this **doesn't hold**:

- **BASIC `$80-$A2` is dense EhBASIC core** (string/array/FP/FOR-NEXT/function vectors) — Logo's
  current mailbox home is unavailable in BASIC, so the canonical mailbox can't live at `$80`.
- The only region free-in-both **today** is `$14-$1F` (12 B) — too small for 23 B.
- A clean `$14-$2F` (28 B) window appears **only in the end state**, after `sprite.s`/`audio.s`
  migrate out of the resident BASIC ROM into the GRAPHICS/SOUND modules. **Correction to the
  initial audit:** the resident BASIC ROM `.include`s `sprite.s` + `audio.s` (`basic.asm:9018,9494`),
  which use `NVR0-3` (`$20-$27`) + `NVR7` (`$2E-$2F`) — so `$20-$2F` is *not* free today; the
  agents missed the `.include` chain. (Resident-BASIC NVR footprint, verified: `audio.s` → NVR0-3;
  `sprite.s` → NVR0,NVR7; `basic.asm` SPRCOLOR → NVR0. Free within `$20-$2F` today: only `$28-$2D`.)
- Even the end-state 28 B can't hold the 23 B mailbox **and** the ~8 B of module scratch the
  migrated GRAPHICS/SOUND handlers need (they used `NVR` as ZP scratch) — scratch spills to low-RAM
  regardless.

**Conclusion:** cross-runtime ZP is scarce, migration-gated, and *fragile* (the audit got it wrong
by one `.include`). Low-RAM is roomy, uniform, migration-independent, and the speed cost is nil for
coarse calls. The audit was still worth it — it's *why* we know to avoid ZP.

---

## 4. Module binary format (LOCKED)

Every module is built `ORG $C000` with a fixed preamble so `lib_call` can validate a page-in and
dispatch uniformly:

```
$C000  JMP dispatch        ; 3B entry — the trampoline's `JSR $C000` is unchanged
$C003  MAGIC   $4E $4C     ; 2B "NL" (Nova Library) — proves a clean page-in
$C005  MODULE_ID  (1)      ; GRAPHICS=1, SOUND=2, SYSTEM=3 …
$C006  VERSION    (1)      ; ABI/module version — loader checks compatibility
$C007  FN_COUNT   (1)      ; jump-table length — bounds-checks FN_ID
$C008  reserved   (2)      ; forward-compat
$C00A  dispatch:  LDA FN_ID(mailbox); CMP FN_COUNT; BCS @bad
                  ASL; TAX; LDA JTABLE+1,X; PHA; LDA JTABLE,X; PHA; RTS   ; cmd*2 RTS-trick
       @bad:      LDA #ERR_NO_FN; STA STATUS; RTS
       JTABLE:    .word handler0-1, handler1-1, …      ; FN_COUNT entries
       handlerN:  read args from mailbox → drive silicon → write RESULT+STATUS → RTS
```

Header = 7 bytes (`$C003-$C009`), fully runtime-agnostic. No name field (the XRAM directory names
modules); the 2 reserved bytes can hold one later for a raw-binary debugger.

**Loader validation** (once per page-in, bank 1 mapped): `MAGIC=="NL"` (catches garbage / un-staged
XRAM — the exact 3a failure mode), `MODULE_ID==intended` (wrong module staged), `VERSION` compatible
→ cache `resident_module`; else set error and refuse to dispatch.

The shared `.inc` defines `MODULE_ID_*`, per-function `FN_ID_*`, and each function's signature — the
single source the module and every runtime's shim compile against.

---

## 5. `lib_call` mechanics (LOCKED)

Every runtime's shim fills the mailbox (`MOD_ID`, `FN_ID`, args) and `JSR lib_call`:

```
lib_call:
  LDA MOD_ID
  CMP resident_module        ; already mapped?
  BEQ call_stub              ; HIT → no page-in
  ; --- page-on-miss ---
  ldx MOD_ID
  copy modtab[X].src   -> $BA78-7A   ; 24-bit XRAM base
  copy modtab[X].words -> $BA7B-7C   ; ≤8192
  LDA #1 : STA $BA76         ; fire page_dma — CPU STALLS until ext_rom loaded (atomic, ~171µs)
  -> validate_stub           ; (low-RAM) swap bank 1, check header, cache resident_module or error
call_stub: (low-RAM)         ; PHP/SEI → swap bank 1 → JSR $C000 → restore bank 0 → restore P
```

Three hard constraints:
1. **The swap-dance must run from low-RAM.** Mapping bank 1 unmaps bank 0 — code doing the swap
   can't live in the foundation ROM. `call_stub` + `validate_stub` are copied to low-RAM at boot
   (the existing `$0270` trampoline, generalized).
2. **Validation needs bank 1 mapped** (it reads `$C003+`), so it's also low-RAM.
3. **IRQ atomicity.** With bank 1 mapped, vectors `$FFFA-$FFFF` come from the *module*, not the
   runtime's handlers (unmapped bank 0). The stub does **`PHP`/`SEI` before the swap, restore `P`
   after** — the call is atomic w.r.t. interrupts (matches the 3a stub's defensive `SEI`; on HW the
   FPGA ties IRQ/NMI off). **Flagged refinement:** a module op running >1 frame drops the 60 Hz
   tick — fine for short leaf ops; revisit if any op can exceed a frame.

**Resident state:** `resident_module` (1B, `$00`=none), `modtab` (module-id → XRAM base + words),
`home_bank` (the runtime's `REG_ROMSWAP` value — or read `$A03F` back if readable, making restore
fully runtime-agnostic).

**Errors** (bad magic / wrong module / version skew / bad fn-id) all land in `STATUS` → the shim
raises a catchable runtime error. Same-module repeat call = `BEQ` hit = zero page-in.

---

## 6. Boot staging (LOCKED — option A bootstrap)

- **SD layout:** `/lib/<name>.mod` — module binaries (the `$C000` image). Coarse, so few:
  `graphics.mod`, `sound.mod`, `system.mod`.
- **Manifest:** a small table (own file or a `boot.json` section) — per module: `name`, `module-id`,
  SD path, byte size, **XRAM slot base**. NovaHost reads it right after `defaultRuntime`.
- **Staging (NovaHost, boot-time):** for each entry, read SD → `write_sdram` (the existing
  `pokeSdramBlock` 256-byte-chunk path — *same machinery as the runtime ROM-load*) into its XRAM
  slot. ~32 ms/16 K today, one-time boot cost. Then release CPU reset; the runtime cold-starts.
- **XRAM shelf (option A — no Phase-1 dependency):** a fixed reserved band, `module i` at
  `SHELF_BASE + i·$4000` (16 K slots). **`SHELF_BASE` is TBD by an XRAM-map audit** — must dodge the
  existing fixed regions (NovaZ `$040000`, EDITUI `$054000`, NVG `$070000`, allocator metadata/dir
  `$07FA00+`); the `$060000` the 3a smoke used collides with NVG, so this needs ZP-audit-level rigor.
- **`modtab` = compile-time constants** in the shared `.inc` (module-id → XRAM base + words). In
  option A there's nothing to populate at runtime — `lib_call` reads static ROM data. The only
  coupling is that NovaHost's manifest slots must match the `.inc` bases → **one source of truth**
  (generate both from one definition). Per-module `words` lets a sub-16 K module page in only what
  it uses.
- **Validation is free:** corrupt/failed staging is caught by the first `lib_call`'s `MAGIC` check.

### 6.1 Migration to System B (Phase 1)
When the 512 K directory allocator lands, the shelf becomes a normal `XALLOC`; the runtime fills
`modtab` by directory lookup at boot; fixed slots become visible reserved-at-address entries.
**`lib_call` never changes** — `modtab` is the abstraction that hides constants-vs-directory.

---

## 7. Open items / TBDs (resolve during `writing-plans`)

- **Low-RAM audit** → fix the canonical mailbox + module-scratch base (page `$02`/`$03`); confirm it
  clears the existing cross-runtime block (`EXT_vec=$0226`, bridges `$0240-$026A`, trampoline
  `$0270`, input buffer `$0272-$02F1`).
- **XRAM-map audit** → fix `SHELF_BASE` clear of NovaZ/EDITUI/NVG/metadata bands.
- **`REG_ROMSWAP` ($A03F) read-back** — verify readable; if not, runtimes set a `home_bank` byte at
  boot. Also confirm how today's `$0270` trampoline / `EXT_CODE` restores the bank.
- **IRQ long-op refinement** — only if a module op can exceed one frame under `SEI`.
- **Module toolchain** — how `.mod` images are assembled (`ORG $C000`, header macro, shared `.inc`),
  and the single-source generator for the `.inc` `modtab` ↔ NovaHost manifest.

---

## 8. Rollout position
This is rollout **step 3** (parent §6). Next: **Phase 4** — move NovaLogo's existing extension
graphics into a paged `GRAPHICS` module, stage it in XRAM at boot, call it from Logo through
`lib_call`. First real end-to-end proof, and the first module the page-in actually delivers.
Then Phase 5 generalizes (SOUND/SYSTEM; BASIC migrates onto leaf-marshalling).
