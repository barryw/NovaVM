# NDK Harvest Candidates — EhBASIC vs NovaLogo code-reuse audit

**Date:** 2026-06-07
**Scope:** READ-ONLY analysis of `ehbasic/basic.asm` + `ehbasic/extension.s` vs
`novalogo/*.s`. Goal: find NON-hardware language-runtime plumbing that both
runtimes implement independently and could be factored into a shared NDK module
(`modules/{graphics,sound,system,files,memory,net,turtle}`) reached via
`lib_call`.

---

## TL;DR — the value-model wall

The single biggest structural finding: **the two runtimes have incompatible
value models, and almost all "number/value handling" is therefore NOT cleanly
shareable.**

- **EhBASIC** uses a **floating-point accumulator (FAC)**: `FAC1_e/FAC1_1..3/
  FAC1_s` mantissa+exponent+sign. Every numeric routine (parse, format, RND,
  trig, INC/DEC) is written *in terms of FAC* and the BASIC text-cursor ZP
  (`Bpntrl`, `LAB_GBYT`/`LAB_IGBY`).
- **NovaLogo** uses a **24-bit 16.8 signed fixed-point** value
  (`eval_val_hi:eval_val_lo:eval_val_frac`) for numbers and **tagged cons
  cells** on a GC heap for everything else (`eval_type`, ATYPE_* nodes).

These representations do not overlap. A "shared number parser" or "shared number
formatter" would have to pick one representation; whichever side it doesn't match
pays a full conversion on every call — and the *second* structural fact makes
that fatal:

- **Hot-loop `lib_call` page-in cost.** Every `lib_call` pages a 16 KB module
  bank into bank 1 on FPGA (`libabi.inc` PGD MMIO). Number parse/format and
  char-print run in the **innermost** interpreter loops. Routing any of them
  through `lib_call` would page-thrash on every digit / every PRINT. **Per-token
  utilities are categorically bad module candidates even when duplicated.**

The third structural fact removes the math category entirely:

- **All "software math" is actually hardware.** Logo's `*`, `/`, `RANDOM`,
  `SQRT`, `REMAINDER`, `SIN`, `COS` all hit the MATH coprocessor MMIO at
  `$BB20` (`MATH_MUL16_*`, `MATH_DIV_*`, `MATH_RNG`, `MATH_SINCOS_ANGLE`)
  **directly inline** (eval.s:861, 880; builtins.s:1964, 2050, 2067, 2081).
  BASIC's M-functions use the same coprocessor. Both pull the register
  definitions from the already-shared `runtime/asm/nova.inc` /
  `math_copro.inc`. There is **no software-math duplication to factor** — the
  add/sub paths Logo does inline (eval.s:830/845) are 3 instructions and would
  never warrant a call. BASIC's add/sub is FAC float, unrelated.

**Net result: this is a "not much is cleanly shareable" finding, and that is the
correct, valuable answer.** The two genuinely-shareable items are both
*text-I/O / line-editing* plumbing, not value handling. Everything in the
number/string/tokenizer space is FALSE or expensive-ADAPTABLE.

---

## Recommended order (only the REAL/worthwhile items)

1. **REPL line reader for BASIC** (ADAPTABLE-medium) — `replline.s` already
   exists and is shared by every *other* runtime; BASIC is the lone holdout
   still rolling its own `LAB_1357`. Highest user-visible payoff (history recall
   in BASIC) and the shared code already exists. The only real reuse win in the
   whole audit.
2. *(Nothing else clears the bar.)* The small print-string helpers are too
   trivial + hot-loop to justify a module; the number/tokenizer items are
   blocked by the value-model wall. Documented below as FALSE so they are not
   re-proposed next audit.

---

## Candidates by category

Legend: **REAL** = same logic, compatible ABI, shareable as a module fn.
**ADAPTABLE** = shareable only after refactoring one side's value/representation
model (cost noted). **FALSE** = looks common but value models / calling
conventions / hot-loop page-in make it not worth it (why noted).

### 1. Number parsing (ASCII → number)

| Item | Verdict | Evidence (BASIC / Logo) | Why |
|------|---------|-------------------------|-----|
| Integer digit-scan (`*10 + digit`) | **FALSE** | `LAB_GFPN` basic.asm:2706-2744 / `tok_parse_number` tokenizer.s:430-498 | Same *algorithm* (multiply-by-10 accumulate), but (a) BASIC reads via the `LAB_GBYT`/`LAB_IGBY` text cursor and writes `Itempl/Itemph`; Logo reads `input_buf,X` and writes `num_val_lo/hi` + a `.frac` field BASIC has no concept of. (b) Both are inner-loop per-digit code — a `lib_call` per number parse would page-thrash. Net: identical pattern, but glue + hot-loop cost kill it. |
| Float / hex / bin string→FAC | **FALSE** | `LAB_2887`+ basic.asm:7249-7436 (`$`/`%`/decimal→FAC1) / *(none)* | Logo has no float and no hex/bin literal parser. Nothing to share. |

### 2. Number formatting (number → ASCII)

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| Unsigned 16-bit → decimal | **FALSE** | `LAB_295E` basic.asm:7454-7463 / `print_uint16` eval.s:1125-1180 | Closest near-match in the codebase: both emit an unsigned 16-bit decimal with leading-zero suppression. BUT BASIC's path *constructs a FAC* (`STA FAC1_1` …`LDX #$90`…`LAB_STFA`) and runs it through the generic FAC formatter, while Logo does a pure repeated-subtract on `pow10` tables. Different mechanism, different output sink (`LAB_18C3` descriptor-stack print vs raw `STA VGC_CHAROUT`). Hot loop (every PRINT). Not worth it. |
| Signed value → decimal (with `.frac`) | **FALSE** | `LAB_296E`/`LAB_297B` basic.asm:7468-7543 (full FAC float formatter w/ sci-notation, rounding) / `print_number` eval.s:1012-1118 | BASIC is a ~150-line FAC float formatter (scientific notation, 999999.4375 thresholds, round-half). Logo is a 100-line fixed-point formatter (integer part via `print_uint16`, then 2 frac digits via `frac*10>>8`). Entirely different number models. FALSE. |
| HEX$ / BIN$ int→string | **FALSE** | `LAB_HEXS`/`LAB_BINS` basic.asm:8121+ (`nums_1/2/3`) / *(none)* | Logo has no hex/bin output. Nothing to share. |

### 3. Software math

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| mul / div / rng / trig / sqrt | **FALSE (already shared as hardware)** | BASIC M-funcs (extension.s, `MATH_*`) / Logo eval.s:861,880 + builtins.s:1964,1976,1987,2050,2067,2081 | All routed to the MATH coprocessor MMIO `$BB20` *inline* via shared `nova.inc`/`math_copro.inc`. No software implementation exists on either side to factor. The register defs are already the single source of truth. |
| float add/sub | **FALSE** | FAC `LAB_246C` etc / Logo inline 24-bit `@do_add`/`@do_sub` eval.s:830-859 | Logo's is 6 instructions of inline ADC/SBC chain (no call would ever be made). BASIC's is FAC float. No overlap. |

### 4. String operations

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| Uppercase a-z→A-Z (single char) | **FALSE** | `LAB_TOUC` basic.asm:1242-1250 / inline at tokenizer.s:269-274, eval.s:1260-1267, 1271-1277 | Genuinely identical 5-instruction idiom (`CMP #'a' / BCC / CMP #'z'+1 / BCS / SBC #$20` or `AND #$DF`). But it is a *5-byte inline fragment*; making it a module fn costs a 16 KB page-in per character cased — absurd. Even as a shared *macro/local subroutine* it can't cross the ROM boundary cheaply. Keep duplicated. |
| Case-insensitive length-prefixed compare | **FALSE** | *(BASIC has no length-prefixed string model)* / `catch_tags_equal` eval.s:1246-1290, `proc_names_equal` procedures.s:1066, `var_names_equal` vars.s:287 | This is a Logo-internal idiom (length byte + chars, the cons/word model). BASIC strings are descriptor-stack {len, ptr} with separate string space (`Sutill`, `Sstorl`); it has *no* length-prefixed compare to share with. Logo's three internal copies *could* be deduped within Logo, but that is intra-Logo cleanup, not cross-runtime reuse — out of scope here. |
| Print null-terminated string | **FALSE** | `LAB_18C3`→`LAB_20AE`/`LAB_18C6` basic.asm:3022+ / `print_cstr_xy` eval.s:592-606 | Logo's is a clean 8-instruction `(ptr),Y` loop to `VGC_CHAROUT`. BASIC's routes through the descriptor stack (`LAB_22B6`) and `LAB_PRNA` column-tracking output (`TPos`/`TWidth` auto-wrap, `V_OUTP` vector). Different output contracts; hot path. FALSE. |
| Print length-prefixed string | **FALSE** | *(none in BASIC)* / `print_name_xy` eval.s:609-624 | Logo-only idiom (matches its word model). No BASIC counterpart. |

### 5. Tokenizer / lexer

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| Keyword matching / dictionary search | **FALSE** | `LAB_13A6` "crunch" basic.asm:1256-1351 (first-char index `TAB_1STC` → per-keyword byte-string match, emits 1-byte token into the line) / `lookup_*` builtins.s:21-115, eval.s:1296 (linear scan of {name_ptr, handler_ptr, arity} records vs a tokenized TOK_WORD) | Fundamentally different data structures *and* different outputs. BASIC crunches text→compact token bytes ahead of execution; Logo builds a heap-allocated token *node list* (`heap_alloc`, ATYPE_TOKEN) then resolves words against handler tables at eval time. No shareable core. |
| Whitespace skip / line scan | **FALSE** | `LAB_GBYT`/`LAB_IGBY` (skip-space text cursor) / `is_word_char` + `tok_src`/`input_buf` scanning tokenizer.s | Both skip spaces and scan, but bound to incompatible cursor abstractions (ROM ZP text pointer vs `input_buf` index). Trivial code; hot loop. FALSE. |

### 6. Error / message reporting

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| error-code → message + print-and-abort | **FALSE** | BASIC error vector table + `LAB_XERR`-style message print via `LAB_18C3` / Logo composed-fragment errors `list_print_err`, `err_nei`, `err_doesnt_like`, … eval.s:586-712 | BASIC uses a code→offset table into a packed message block; Logo composes messages from reusable fragments (`str_notenough`, `frag_to`, …) and ends in a CATCH/THROW unwind (`try_throw_error`). Different message storage, different control-flow tail (BASIC longjmp-to-error vs Logo catch unwind). The *print primitives* they'd want to share (`print_cstr_xy` / `LAB_18C3`) are already FALSE above. |

### 7. Memory / heap / line management

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| heap / GC vs program-line list + var storage | **FALSE (verified, as predicted)** | Logo `gc.s` (523), `heap.s` (135), cons-cell allocator / BASIC program linked-list (`Baslnl`, `Baslnh`) + variable area + string space (`Sstorl`) | Completely different memory models: Logo is a mark/sweep GC over tagged nodes; BASIC is a static program-line linked list with a separately-managed variable table and top-down string heap. No common allocator. Explicitly NOT shareable. |

### 8. Comparison / sort / list / geometry / easing / random

| Item | Verdict | Evidence | Why |
|------|---------|----------|-----|
| Value comparison (`<`,`>`,`=`) | **FALSE** | BASIC FAC compare `LAB_27F8` / Logo 24-bit signed compare eval.s:936-979 | FAC vs 16.8 fixed. Value-model wall. |
| List utilities / sort | **FALSE** | *(BASIC has no list type; arrays only)* / Logo `lists.s` | No BASIC counterpart. |
| Geometry / easing | **FALSE (already shared)** | turtle math + tween live in `modules/turtle/turtle.s` + `runtime/asm/tween.s`; Logo turtle/SIN/COS go via lib_call(TURTLE) / coprocessor; BASIC has no turtle | Already factored into modules; no duplication. |
| Random helpers | **FALSE (already shared as hardware)** | Logo `do_random` builtins.s:2047 (MATH_RNG) + SYS_RNG8/16/32 fns exist in `libsystem.inc` / BASIC `LAB_RND` basic.asm:7865 (FAC) | Logo uses the coprocessor RNG inline; the SYSTEM module already exposes host RNG (SYS_RNG8/16/32). BASIC RND is FAC software (legacy). If BASIC ever wants host entropy it should call the *existing* `SYS_RNG*` — but that is "BASIC adopts an existing fn", not new factoring. Noted, not a harvest item. |

### Cross-checked memory-flagged items (folded in)

- **MUSIC PRIORITY multi-voice Y-clobber** — this is a **BASIC-internal bug** in
  a BASIC music keyword handler, not a shared-code candidate. Out of scope for
  harvest (fix in place in BASIC). Not a Logo/BASIC commonality.
- **NRECV$ stale `NIC_MSGLEN`** — `NRECV$` (basic.asm:3870) **already routes
  through the NET module** via `lib_call(NET, NET_RECV)` (basic.asm:3871,9780).
  Already shared; any staleness bug lives in the NET module or its wrapper, not
  in duplicated runtime code.
- **vsprite / software-sprite collision** — lives in the **GRAPHICS module**
  (`runtime/asm/vsprite.s`, `msprite.s` under MODULE_BSS_BAND), reached by both
  runtimes via `lib_call(GRAPHICS)`. Already module-resident, not BASIC-only.
  Confirmed: not duplicated.

---

## The one promote-worthy item, in detail

### REPL line reader: bring BASIC onto `replline.s`  — ADAPTABLE (M), risk M

- **Shared code already exists:** `runtime/asm/replline.s` — key poll,
  backspace, printable echo, **6-deep up/down history ring**, single-row
  repaint. Already `.include`d by NovaLogo (novalogo.s:20) and "Forth, Pascal,
  and the assembler dev env all inherit this reader."
- **BASIC still rolls its own:** `LAB_1357` (basic.asm:1195-1240) — a plainer
  loop (no history) filling `Ibuffs` via `V_INPT`, echoing via `LAB_PRNA`.
- **Why ADAPTABLE not REAL:** `replline.s` assumes its own buffer/cursor
  (`input_buf`/`buf_idx`) and a fixed `$9800-$9BFF` history region, and depends
  on `VGC_CHARIN`/`VGC_CHAROUT`/`VGC_CURS*` directly — bypassing BASIC's
  `Ibuffs`, `LAB_PRNA` column tracking, and `V_INPT`/`V_OUTP` indirection
  vectors. The header even states *"BASIC is excluded — it uses EhBASIC's own
  ROM input routine."* Adoption requires either (a) pointing BASIC's input at
  `input_buf` and re-pointing `Bpntrl` at it, or (b) parameterizing
  `replline.s`'s buffer + output hooks. Memory-map check needed:
  `$9800-$9BFF` must be free in the BASIC build (it is the BASIC program space
  top region — **verify before adopting**).
- **Target:** NOT a `lib_call` module — line input is per-line (not per-token),
  but it is also *interactive* (blocks on the user), so page-in cost is
  irrelevant; co-assemble via `.include` exactly as the other runtimes do, OR
  promote to SYSTEM (the editor already lives there as `SYS_FN_EDIT`). Given
  it's interactive and already an `.include` shared file, **`.include` into
  BASIC is the lower-risk path** (no ABI marshalling of the live keystroke
  loop).
- **Effort:** M (buffer/vector reconciliation + a `repl_line_complete` hook that
  returns "always complete" for BASIC, plus the `$9800` free-region check).
- **Risk:** M (touches BASIC's input hot path and memory map; regression surface
  is the entire line editor). Needs the BASIC editor/REPL test suite green.
- **Payoff:** command-line **history recall in BASIC** with zero new code, and
  one fewer line-input implementation to maintain.

---

## Summary counts

- **REAL:** 0
- **ADAPTABLE (worth doing):** 1 — BASIC ← `replline.s`
- **ADAPTABLE (not worth doing):** 0
- **FALSE:** every number-parse, number-format, software-math, string-op,
  tokenizer, error-report, heap, compare, list, geometry, and random item above
  (~18 distinct items), each for one of: value-model wall (FAC vs 16.8/cons),
  hot-loop `lib_call` page-in cost, already-shared-as-hardware (MATH coprocessor
  via `nova.inc`), or already-module-resident.

**Bottom line:** the hardware layer was the reuse; it's already harvested. The
language-runtime layer is split by the FAC-vs-fixed/cons value model and guarded
by the per-`lib_call` page-in cost, so the *only* remaining clean win is moving
BASIC onto the already-shared interactive line reader. There is no case for a new
`math`/`util` module — math is hardware, and util-grade helpers are all
hot-loop, single-digit-instruction fragments that a 16 KB page-in would dwarf.
