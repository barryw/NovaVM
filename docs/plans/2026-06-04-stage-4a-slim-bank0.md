# Stage 4a — Slim NovaLogo Bank 0 Implementation Plan (ruthless 6502)

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans (or superpowers:subagent-driven-development) to implement this plan task-by-task.

**Goal:** Reclaim ~1.3 KB of resident bank-0 ROM in NovaLogo via byte-identical refactors — banking headroom to seat the resident turtle in Stage 4c.

**Architecture:** NovaLogo assembles as ONE translation unit (`ca65 novalogo.s` `.include`s `heap/gc/tokens/tokenizer/eval/builtins/lists/vars/procedures`). All global labels are cross-file callable; `@local` labels scope to the enclosing global label. **Zero behavior change** — the full NovaLogo suite plus new exact-text characterization tests are the safety net.

**Tech Stack:** ca65/ld65 (65C02), MSTest, the Avalonia `CompositeBusDevice` Logo harness.

---

## ✅ RESULT (2026-06-04) — 1071 B reclaimed (free 15 → 1086)

Executed subagent-driven on `main`. Commits dc33115..ce0190e. 102/102 NovaLogo tests green throughout.

| Task | What | free after |
|---|---|---|
| 1 | dead arity-read in `lookup_builtin` | 19 |
| 2 (b–f) | message composer; gut all whole error strings (lists/builtins/procedures/eval) | 961 |
| 3 | PRINT/TYPE newline-flag fall-through merge | 1007 |
| 4 | `proc_next` extract + `do_erase` unlink rewrite | 1086 |

CODE −163 B, RODATA −908 B (the composition of error strings was the dominant win, as predicted).
10 new exact-text characterization tests lock every error family byte-identical; a middle-erase test
covers the `do_erase` unlink. All 29 lists + 8 builtins + 3 procedures name-string mappings verified
byte-identical to the originals offline.

**Deferred to 4c:** Task 5 (1-arg-reporter arity flip, ~110 B) — not a clean drop-in; diverges on the
error path (dispatcher pre-eval skips the handler + infix check vs. handler self-eval `SEC/RTS`),
needs per-reporter characterization, hot-path risk. Revisit only if the measured turtle needs it.
**Rejected:** `proc_name_cmp` unification (3 loops differ in search-name source/offset AND exit
convention; costs more than it saves on the hot path). **Not pursued:** the `"X NEEDS [ BODY ]"` /
FOR / WHILE-UNTIL inline messages in builtins.s remain uncomposed (available if 4c needs more).

---

## The governing principle (user directive, 2026-06-04)

> "think like a 6502 developer writing for a constrained platform… use all the 6502 tricks in the book… we don't have the room to be duplicating strings or code."

Nothing in this ROM may store the same byte sequence twice. Every error message is **composed at print time** from shared fragments + the command's *existing* length-prefixed name string. Code tails are **shared by fall-through**, not duplicated. Sites collapse to a name pointer + a tail-call.

### Why the parent design's §4 numbers were off (both directions)

`docs/plans/2026-06-04-graphics-module-and-bank0-slim-design.md` §4 budgeted ~1.4 KB. Recon found its *per-item framing* wrong:
- §4's `lists.s` "indexed error-printer table (~330 B)" is a **wash** — those sites already tail-call `list_print_err`; an index table saves ~0.
- §4 *under*-counted the real prize: the **error strings themselves** are ~800 B of composable duplication in `lists.s` alone, plus the command-name double-storage.

The corrected, ruthless target stands at **~1.3 KB**, dominated by the message-composition engine (Task 2). The "1.2 KB resident turtle fits" question remains a **Stage 4c** decision with measured numbers — but the budget is no longer in doubt.

---

## The size gate

`novalogo.bin` is `fill=yes` (always 16384 B) — file size is not the metric. Measure ROM-region bytes used from `novalogo/novalogo.map`:

```
ROM region = $C000, size $3FD7 (CODE + RODATA; MONITOR is separate at $FFD7)
free_bytes = 0x3FD7 - (CODE.Size + RODATA.Size)
```
Baseline (`main`): CODE=$3628, RODATA=$09A0 → **free_bytes = 15**.

```bash
cd /Users/barry/Git/e6502/novalogo && make novalogo.bin >/dev/null 2>&1 && \
python3 - <<'EOF'
import re
m=open('novalogo.map').read()
sz=lambda s:int(re.search(rf'^{s}\s+\w+\s+\w+\s+(\w+)',m,re.M).group(1),16)
u=sz('CODE')+sz('RODATA');print(f"CODE={sz('CODE'):#06x} RODATA={sz('RODATA'):#06x} used={u:#06x} free={0x3FD7-u}")
EOF
```
Record `free=` after every task. It must rise (or hold for the dead-code task) and never fall — except the engine-scaffold step (2b), which dips before 2c–2f repay it many-fold.

---

## The test gate

Execution tests load `e6502.Avalonia/Resources/novalogo.bin` at runtime; the csproj copies `Resources/*.bin` into the test output on build. The rebuilt ROM only reaches tests after `make install` copies it into `Resources/`.

**Full verify loop** (end of every code task):
```bash
cd /Users/barry/Git/e6502/novalogo && make >/dev/null && make install >/dev/null && \
cd /Users/barry/Git/e6502 && dotnet test --filter "FullyQualifiedName~NovaLogo" 2>&1 | tail -20
```
All three suites green: `NovaLogoTests`, `NovaLogoHarnessTests`, `NovaLogoMailboxReservationTests`. (`make install` mutates tracked `Resources/novalogo.bin`; expected — it ships per commit.)

---

## Task 0: Baseline (green + sized)

**Step 1:** `git status --short` (clean) and run the NovaLogo suite — record PASS count.
**Step 2:** Run the size gate — expect `free=15`. Record.
**Step 3:** No commit.

---

## Task 1: Delete dead code in `lookup_builtin` (~4 B, zero risk warm-up)

**Files:** Modify `novalogo/builtins.s` (`lookup_builtin`, ~lines 95–96).

Recon: after `STX ptr2_lo` overwrites ptr2 with the handler address, a `LDY #4 / LDA (ptr2_lo),Y` reads from the wrong base and is immediately overwritten by the correct arity read from `num_tmp`. Author comment flags it.

**Step 1:** `sed -n '80,105p' builtins.s` — confirm the `LDA` result is dead (overwritten before use). If consumed, STOP/skip.
**Step 2:** Delete only the `LDY #4` + `LDA (ptr2_lo),Y` + stale comment.
**Step 3:** Size gate → `free` +~4.
**Step 4:** Full verify loop → green.
**Step 5:** Commit:
```bash
git add novalogo/builtins.s e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): drop dead arity-read in lookup_builtin (4a)"
```

---

## Task 2: Message-composition engine (the centerpiece, ~−900 B across files)

No whole error strings. Build a tiny composer; every error = fragments + the existing name string. Sub-steps: 2a tests, 2b engine, 2c–2f migrate per file.

### Task 2a: Characterization tests — exact text for every family (MUST be first)

**Files:** Modify `e6502UnitTests/NovaLogoTests.cs`.

Today the suite only asserts NEI is *absent* on success. We lock the exact output of every error **family** so composition is provably byte-identical (trailing spaces included — e.g. `str_first_type` is literally `"FIRST DOESN'T LIKE "` with a dangling space and no value; reproduce it exactly).

Add tests (use existing `BootShowsBannerAndPrompt` boot pattern + `RunUntilScreenContains`/`SnapshotScreen`/`ScreenEditor.QueueInput`). One per family + edge cases:

```csharp
// NEI family
[TestMethod] public void FirstNoInput_ExactNEI()      // "NOT ENOUGH INPUTS TO FIRST"
[TestMethod] public void EmptypNoInput_ExactNEI()     // "NOT ENOUGH INPUTS TO EMPTY?"  (name w/ '?')
// DOESN'T LIKE family (note dangling trailing space)
[TestMethod] public void FirstWrongType_ExactDoesntLike()  // "FIRST DOESN'T LIKE " (exact, trailing space)
// DOESN'T LIKE [] AS INPUT
[TestMethod] public void FirstEmptyList_ExactEmpty()       // "FIRST DOESN'T LIKE [] AS INPUT"
// NEEDS A LIST
[TestMethod] public void ItemWrongType_ExactNeedsList()    // "ITEM NEEDS A LIST"
// specials
[TestMethod] public void ItemIndexRange_Exact()            // "ITEM INDEX OUT OF RANGE"
[TestMethod] public void WordWrongType_Exact()             // "WORD DOESN'T LIKE THIS INPUT"
// eval.s families
[TestMethod] public void UnknownWord_ExactIDontKnow()      // "I DON'T KNOW HOW TO ABANDON" (exists @ ~line 481 — keep)
[TestMethod] public void UnsetVar_ExactHasNoValue()        // "<NAME> HAS NO VALUE"
// CATCH semantics across an error tail
[TestMethod] public void CatchErrorInterceptsError()       // CATCH "ERROR { <erroring cmd> } still catches
```

**Step 2:** Run them — must PASS now (they characterize current bytes):
```bash
cd /Users/barry/Git/e6502 && dotnet test --filter "FullyQualifiedName~NovaLogo&FullyQualifiedName~Exact" 2>&1 | tail -20
```
Any failure here means our understanding of current output is wrong — fix understanding before touching code.

**Step 3:** Commit:
```bash
git add e6502UnitTests/NovaLogoTests.cs
git commit -m "test(logo): characterize exact text of every error family (4a)"
```

### Task 2b: Build the composer + fragment pool

**Files:** Modify `novalogo/eval.s` (primitives near `print_inl` ~line 663) and split `list_print_err` in `novalogo/lists.s` (~1123) into reusable pieces.

**Step 1 — primitives.** Add to eval.s:

```asm
; print_cstr_xy — print null-terminated string. In: X/Y = lo/hi ptr. Clobbers A,Y,ptr_lo/hi.
print_cstr_xy:
      STX   ptr_lo
      STY   ptr_hi
      LDY   #0
@l:   LDA   (ptr_lo),Y
      BEQ   @d
      STA   VGC_CHAROUT
      INY
      BNE   @l
@d:   RTS

; print_name_xy — print a length-prefixed name ( .byte len,"TEXT" ). In: X/Y = lo/hi ptr. Clobbers A,X,Y,ptr_lo/hi.
print_name_xy:
      STX   ptr_lo
      STY   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y      ; length
      BEQ   @d
      TAX
@l:   INY
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      DEX
      BNE   @l
@d:   RTS
```

**Step 2 — shared tail.** Refactor `list_print_err` so its print/newline/continue tail is reusable by fall-through:

```asm
; list_print_err — print cstring (X/Y), newline, abandon line.  (unchanged entry contract)
list_print_err:
      JSR   print_cstr_xy
err_nl_continue:                 ; <-- shared tail: newline + return to REPL
      JSR   eval_newline
      JMP   eval_continue
```
(If any caller needs the catchable tail, add `err_nl_throw: JSR eval_newline / JSR try_throw_error / RTS`. Determine per Task 2d/2f which sites need which — guarded by the 2a CATCH test.)

**Step 3 — fragment pool (each fragment stored ONCE, in eval.s RODATA):**
```asm
frag_nei:       .byte "NOT ENOUGH INPUTS TO ", 0
frag_dl:        .byte " DOESN'T LIKE ", 0
frag_as_input:  .byte "[] AS INPUT", 0          ; printed AFTER frag_dl
frag_this_input:.byte "THIS INPUT", 0           ; printed AFTER frag_dl
frag_needs_list:.byte " NEEDS A LIST", 0
frag_idk:       .byte "I DON'T KNOW HOW TO ", 0
frag_no_value:  .byte " HAS NO VALUE", 0
; …add fragments only as families demand; never store a composite that contains another fragment.
```

**Step 4 — family helpers (fall-through chained; each takes X/Y = name ptr):**
```asm
; "NOT ENOUGH INPUTS TO <name>"
err_nei:
      STX   num_tmp_lo            ; save name across print_inl (confirm num_tmp free here)
      STY   num_tmp_hi
      LDX   #<frag_nei
      LDY   #>frag_nei
      JSR   print_cstr_xy
      LDX   num_tmp_lo
      LDY   num_tmp_hi
      JSR   print_name_xy
      BRA   err_nl_continue

; "<name> DOESN'T LIKE "
err_doesnt_like:
      JSR   print_name_xy         ; consumes X/Y
      LDX   #<frag_dl
      LDY   #>frag_dl
      JMP   list_print_err        ; print frag + newline + continue

; "<name> NEEDS A LIST"
err_needs_list:
      JSR   print_name_xy
      LDX   #<frag_needs_list
      LDY   #>frag_needs_list
      JMP   list_print_err

; "<name> DOESN'T LIKE [] AS INPUT"
err_dl_empty:
      JSR   print_name_xy
      LDX   #<frag_dl
      LDY   #>frag_dl
      JSR   print_cstr_xy         ; " DOESN'T LIKE "  (no newline)
      LDX   #<frag_as_input
      LDY   #>frag_as_input
      JMP   list_print_err        ; "[] AS INPUT" + newline + continue

; "<name> DOESN'T LIKE THIS INPUT"  (word_type)
err_dl_this:
      JSR   print_name_xy
      LDX   #<frag_dl
      LDY   #>frag_dl
      JSR   print_cstr_xy
      LDX   #<frag_this_input
      LDY   #>frag_this_input
      JMP   list_print_err
```
> Confirm `num_tmp_lo/hi` is free at `err_nei` (general scratch). `print_inl`/`eval_newline`/`eval_continue`/`VGC_CHAROUT` are global. For the one-off `"ITEM INDEX OUT OF RANGE"`: add `frag_index_range: .byte " INDEX OUT OF RANGE",0` and route `item_range` through an `err_name_then` that prints name + that cstring (same shape as `err_needs_list`).

**Step 5:** Build (`make novalogo.bin`). Expect a small `free` **dip** (engine scaffold, fragments, helpers added; no sites migrated yet). Assemble clean. No commit (combine with 2c).

### Task 2c: Migrate `lists.s` — all error families (the bulk, ~−600 B)

**Files:** Modify `novalogo/lists.s`.

**Step 1 — map every site to a family.** From `lists.s` RODATA (~2349–2415):

| Family → helper | Sites (current `str_*` → reuse `*_name`) |
|---|---|
| `err_nei` | first, bf, count, emptyp, last, bl, item, memberp, show, fput, lput, list, sentence, word, run (≈15) |
| `err_doesnt_like` | first_type, bf_type, count_type, last_type, bl_type (5) |
| `err_dl_empty` | first_empty, bf_empty, last_empty, bl_empty (4) |
| `err_needs_list` | item_type, memberp_type, run_type (3) |
| `err_dl_this` | word_type (1) |
| `err_name_then frag_index_range` | item_range (1) |

For each, confirm the matching length-prefixed `str_*_name` exists and its text equals the name baked into the old string (spot-checked: `EMPTY?`, `MEMBER?`, `BUTFIRST` all match). `bf`/`bl` errors say "BUTFIRST"/"BUTLAST" → use `str_butfirst_name`/`str_butlast_name`, NOT `str_bf_name`/`str_bl_name`. Verify each pairing before editing.

**Step 2 — rewrite each call site.** E.g. `do_first`:
```asm
@err_args:   LDX #<str_first_name
             LDY #>str_first_name
             JMP err_nei
@err_type:   LDX #<str_first_name
             LDY #>str_first_name
             JMP err_doesnt_like
@err_empty:  LDX #<str_first_name
             LDY #>str_first_name
             JMP err_dl_empty
```

**Step 3 — delete every now-dead whole error string** (`str_first_err`, `str_first_type`, `str_first_empty`, …). After migrating, verify zero references remain before deleting each label:
```bash
grep -c "str_first_type\b" lists.s   # expect 1 (the def) before deleting the def
```
Keep all `str_*_name` strings (still referenced by `builtin_table` + now the error sites).

**Step 4:** Size gate → `free` jumps hard (this is the ~−600 B).
**Step 5:** Full verify loop → green incl. all 2a tests.
**Step 6:** Commit:
```bash
git add novalogo/eval.s novalogo/lists.s e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): compose error messages from fragments+name; gut lists.s error strings (4a)"
```

### Task 2d: Migrate `builtins.s` error sites

**Files:** Modify `novalogo/builtins.s`.

`grep -n "NOT ENOUGH INPUTS TO\|DOESN'T LIKE\|str_.*_err" builtins.s`. Route NEI sites (CHAR, ASCII, IF, IFELSE, CATCH, THROW, MAKE, REPEAT, …) through `err_nei` with their `str_*_name`; route any type errors through the matching family helper; delete dead whole strings. **CATCH note:** if a site must remain catchable (IF/CATCH/THROW), route it through `err_nl_throw` instead of `err_nl_continue` — the 2a CATCH test is the guard. Size gate → verify loop → commit:
```bash
git add novalogo/builtins.s e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): compose builtins.s error messages (4a)"
```

### Task 2e: Migrate `procedures.s` error sites (PO, ERASE, APPLY, …)

Same procedure with `str_po_name`/`str_erase_name`/`str_apply_name`. Size gate → verify loop → commit:
```bash
git add novalogo/procedures.s e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): compose procedures.s error messages (4a)"
```

### Task 2f: Migrate `eval.s` error sites + fold the token-word path into the engine

**Files:** Modify `novalogo/eval.s`.

Recon: 6 inline error-printer sequences + 2 identical token-word-print blocks (the `"I DON'T KNOW HOW TO " + <word>` path) + the `str_notenough` ("NOT ENOUGH INPUTS") generic + `" HAS NO VALUE"`.
- Replace the 6 inline cstring-print loops with `LDX/LDY` + `JSR print_cstr_xy` + the appropriate tail.
- The 2 `@truly_unknown` paths: print `frag_idk` via `print_cstr_xy`, then print the offending token word. Add an engine entry `err_idk_word` that prints `frag_idk` then the **token word** (length-prefixed at `eval_cur + TOK_PAYLOAD`) — i.e. a tiny variant of `print_name_xy` sourcing from the token payload. Both `@truly_unknown` sites collapse to `JMP err_idk_word`.
- `" HAS NO VALUE"`: `<varname> HAS NO VALUE` → print var name + `frag_no_value` via the same name+cstr shape.

Existing test asserts exact `"I DON'T KNOW HOW TO ABANDON"` (~line 481) — keep byte-identical. Size gate → verify loop → commit:
```bash
git add novalogo/eval.s e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): fold eval.s error/word printers into composer (4a)"
```

---

## Task 3: PRINT/TYPE fall-through merge (~−40 B)

**Files:** Modify `novalogo/builtins.s` (`do_print` ~124, `do_type` ~163).

They share the type-dispatch + word-output loop; the only difference is PRINT emits a trailing newline. Merge via a newline flag + fall-through:
```asm
do_print:  LDA #1            ; want newline
           BRA pt_common
do_type:   LDA #0
pt_common: STA print_nl_flag ; reuse a free zp; confirm unused
           ; ... single shared dispatch (number/list/word) ...
           ; each arm ends: LDA print_nl_flag / BEQ + skip / JSR eval_newline / JMP eval_continue
```
Delete the duplicated word-output loop. Size gate → verify loop (PRINT/TYPE tests) → commit:
```bash
git add novalogo/builtins.s e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): merge PRINT/TYPE via newline-flag fall-through (4a)"
```

---

## Task 4: `procedures.s` structural dedup (~−80 B)

**Files:** Modify `novalogo/procedures.s`.

**4-1 `proc_next`:** 4 byte-identical "follow next-record pointer" sequences (`proc_lookup`, `proc_find_by_name`, `do_erase`, `do_pots`). Extract:
```asm
proc_next:                       ; in/out proc_entry_lo/hi; clobbers A,X,Y,ptr_lo/hi
      LDA proc_entry_lo
      STA ptr_lo
      LDA proc_entry_hi
      STA ptr_hi
      LDY #0
      LDA (ptr_lo),Y
      TAX
      INY
      LDA (ptr_lo),Y
      STA proc_entry_hi
      STX proc_entry_lo
      RTS
```
Replace each inline copy with `JSR proc_next` + its existing `BRA @walk`.

**4-2 `proc_name_cmp`:** the 3 name-compare loops (`proc_names_equal`, `proc_find_by_name`, `do_erase`) differ only in **exit convention** (SEC/RTS vs CLC-loop vs BRA). Unify the *body* into one carry-returning sub (carry set = match), and have each caller branch on carry. The body (~24 B) shared across 3 callers nets ~+35 B even with per-caller branch glue. Verify the source-pointer setup is identical (one uses `eval_cur+TOK_PAYLOAD+1`, two use `var_name+1`) — pass the source base in a register/zp so one body serves all three.

**4-3 `do_erase` unlink:** rewrite `@erase_match` to read `current->next` once (lo→X, hi→stack) then write both into `prev->next`, dropping recon's redundant `proc_entry` reloads.

Add a small ERASE round-trip test if harness coverage is thin (define 2 procs, ERASE one, assert the other survives). Size gate → verify loop → commit:
```bash
git add novalogo/procedures.s e6502UnitTests/NovaLogoTests.cs e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): share proc_next/proc_name_cmp; tighten do_erase unlink (4a)"
```

---

## Task 5: 1-arg reporter arity flip (~−110 B — medium risk, gated, do last)

**Files:** Modify `novalogo/builtins.s` (+`lists.s` reporters).

The dispatcher already evaluates `arity` args before calling a handler. ~22 one-arg reporters declare `arity 0` and hand-roll `JSR eval_expr / BCS @err` (~5 B each). Flip to `arity 1` and drop the prologue **only where observable behavior is byte-identical** (under-supply now routes through the dispatcher's generic path; some reporters currently just `SEC/RTS`, some print specific text).

**Step 1:** Characterize under-supply output for 2–3 representative reporters (SIN, FIRST, NOT) *as-is*, and for an `arity 1` builtin given no arg. Commit those tests.
**Step 2:** Flip only the matches, in batches of ~5; for each: arity `0→1`, delete the `eval_expr/BCS` prologue + dead `@err` stub, leave the body. Leave non-matches at arity 0 (note them). Full verify loop after each batch (bisectable).
**Step 3:** Size gate → commit:
```bash
git add novalogo/builtins.s novalogo/lists.s e6502UnitTests/NovaLogoTests.cs e6502.Avalonia/Resources/novalogo.bin
git commit -m "refactor(logo): 1-arg reporters use dispatcher arity (4a)"
```

---

## Task 6: Final tally + update parent design

**Files:** Modify `docs/plans/2026-06-04-graphics-module-and-bank0-slim-design.md`.

Run the size gate; record total bytes freed vs baseline 15. Update §4/§7 with the measured reclaim and the resident-turtle headroom, so Stage 4c starts from reality. Commit:
```bash
git add docs/plans/2026-06-04-graphics-module-and-bank0-slim-design.md
git commit -m "docs(plan): record measured 4a reclaim for 4c seating (4a)"
```

---

## Considered and left out (with the 6502 reasoning)

- **Tokenizer delimiter-class lookup table:** `is_word_char` is *already* a single shared subroutine called from 3 sites (no code dup). Replacing its 34-byte CMP-chain with a table costs ≥ what it saves (a 256-B table dwarfs 22 B; a 14-entry table + loop is a wash) **and** regresses the per-character hot path (loses early-exit). A bit-class table only pays if many *distinct* class tests share it — there's one. Net loss. Skip. (Free win available: reorder the CMPs most-common-first — speed only, zero bytes.)

---

## Done criteria
1. NovaLogo suite green at every commit.
2. `free_bytes` strictly increased from baseline 15; total reclaim recorded (target ~1.3 KB).
3. No byte sequence stored twice — all error text composed from the fragment pool + existing name strings.
4. New characterization tests prove every error family is byte-identical post-refactor.
5. Parent design updated with the measured number for the 4c seating decision.
