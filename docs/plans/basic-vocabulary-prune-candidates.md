# EhBASIC Vocabulary Prune Candidates — Main ROM Recovery

Date: 2026-06-07
Author: 6510 vocabulary audit (read-only analysis)

## Goal

Free **main ROM** (`ehbasic/basic.bin`, CODE segment) so the extension ROM can be
eliminated by relocating hardware domains into the main ROM as `lib_call`
marshallers. Main ROM is at capacity.

## Measured budget (fresh build, `make basic.bin`)

| Fact | Value |
| --- | --- |
| CODE segment | `$C000`–`$FFAA`, size `$3FAB` (16299 bytes) |
| MONITOR segment | starts `$FFD7` |
| **Free bytes in main ROM** | `$FFD7 - $FFAB` = **44 bytes** |
| Next-domain target (xram + xmc marshallers) | ~350 bytes |

All handler sizes below are measured from `basic.sym` / `min_mon.lst` (block =
start label to next handler label, which captures fall-through tails), not
estimated.

---

## CRITICAL ARCHITECTURAL CONSTRAINT — read first

Removing a keyword does **not** automatically reclaim its dispatch-table slot.

Token dispatch is **index = (token_value − base) × 2** straight into the vector
table (`LAB_CTBL` for commands at basic.asm:1980; `LAB_FTBL`/`LAB_FTPL` for
functions at basic.asm:4218–4232). **Token value === table index.** Primary
tokens are saturated `$80`…`$FF`; functions are indexed `token − TK_SGN`.

Two removal modes follow:

- **TOMBSTONE (safe, no renumber):** keep the `TK_*` value and point its dispatch
  slot at `LAB_15D9-1` (the syntax-error vector — this pattern is already used for
  reserved extended IDs at basic.asm:2042/2048/2074). You reclaim the **handler +
  name-string (TAB_ASCx) + LIST entry (LAB_KEYT)**, but the **2-byte command slot
  / 4-byte function slot stays**. Tokenized `.bas` files keep working.
- **RENUMBER (reclaim slot, high risk):** delete the `TK_*` definition so every
  later token shifts down by one. Reclaims the dispatch slot too, but **rewrites
  the token byte of every keyword after it** — invalidating any saved tokenized
  program and the FPGA/Avalonia token-value tests
  (`EhBasicTokenizationTests.cs` asserts specific token bytes like `01 12`,
  `80 00`). Only viable for a **batch removed from the tail** of a range.

The function candidates worth cutting (SADD, VARPTR, MAX, MIN, TWOPI, USR, POS)
all sit in the **middle** of the function range, so renumber-reclaim of their
4-byte slots is not free — it cascades through SPRITEX/SPRITEY/COLLISION/BUMPED
(the VGC functions at the tail). Realistic savings = TOMBSTONE column unless a
tail batch is chosen.

---

## Per-keyword table costs (the "few bytes each" beyond the handler)

| Table | Location | Cost reclaimed on removal |
| --- | --- | --- |
| Dispatch vector | `LAB_CTBL` ~11481 / `LAB_FTBL`+`LAB_FTPL` ~11594/11550 / `LAB_OPPT` ~11638 | cmd 2 B, func 4 B (2 in FTBL + 2 in FTPL), op 3 B — **TOMBSTONE keeps this** |
| Name string | `TAB_ASCx` ~11764+ | (continuation chars) + 1 token byte. First letter shared in `TAB_1STC`, not freed. |
| LIST decode | `LAB_KEYT` ~12064+ | 4 B per keyword (len, first char, word ptr) |

---

## Candidate table (grouped by classification)

`TOMB` = bytes freed in tombstone mode (handler + name + LIST), slot retained.
`RENUM` = TOMB + dispatch slot, if a tail-batch renumber is done.

### JUNK / legacy teletype (lowest user value)

| Keyword | What it does | Handler B | TOMB | RENUM | Entanglement / risk | Rec |
| --- | --- | --- | --- | --- | --- | --- |
| **WIDTH** | set terminal line width + TAB step (auto-wrap). `LAB_WDTH`→`WExit` computes `Iclim`/`TabSiz` | 82 | **91** | 93 | Writes `TWidth`/`TabSiz`/`Iclim` read by PRINT auto-wrap (lines 2940/2970/3073) and TAB(). `LAB_NOSQ` (RTS) + `TabErr` shared with SQR; `LAB_MMSE` shared with MAX/MIN. Must zero-init `TWidth`/`Iclim` so wrap defaults to infinite line. Has help doc (width.md), no .bas usage; 5 test files mention "width" (substring noise). | **CUT-IF-NEEDED** (biggest single win) |
| **NULL** | `LAB_NULL`: parse+discard a byte then RTS — **already gutted** to a no-op (not classic CR-padding) | 3 | 11 | 13 | RTS at `LAB_167A` shared with CONT — handler is only the `JSR LAB_GTBY` (3 B). Tiny. | **CUT-IF-NEEDED** (cheap, harmless) |
| **WAIT** | spin on a memory location until `(AND mask) EOR mask` is nonzero | 26 | 34 | 36 | `LAB_GADB` (get-two-params) shared with POKE — stays. Handler-specific = 26 B. No .bas usage; help doc exists; tests mention "await"/"wait" (noise). | **CUT-IF-NEEDED** |

### NICHE (rare but valid; mostly clean to excise)

| Keyword | What it does | Handler B | TOMB | RENUM | Entanglement / risk | Rec |
| --- | --- | --- | --- | --- | --- | --- |
| **SADD** | `SADD(s$)` → address of a string's body | 25 | **34** | 38 | Clean (ends `JMP LAB_AYFC`; all inner JSRs are shared lib). FTPL slot is `$0000` (no preprocess). No .bas usage. | **CUT-IF-NEEDED** |
| **VARPTR** | `VARPTR(v)` → address of a variable | 16 | 27 | 31 | Clean (`JMP LAB_AYFC`). FTPL `$0000`. Help doc varptr.md; no .bas usage. | **CUT-IF-NEEDED** |
| **SWAP** | exchange two same-type variables | 40 | 48 | 50 | `SwapErr` (JMP type-mismatch) is local. Clean. Help doc swap.md; no .bas usage. | **CUT-IF-NEEDED** |
| **MAX + MIN** (pair) | variadic max/min of an expr list | 105 | **121** | 129 | Share `LAB_MMPP`, `LAB_PHFA`, `LAB_MMEC`. `LAB_MMSE` also referenced by WIDTH — if WIDTH kept, leave `LAB_MMSE` (−3 B → ~118). Must remove BOTH (each needs the shared body). Help docs exist; no .bas usage. | **CUT-IF-NEEDED** (2nd biggest win) |
| **TWOPI** | constant 2π | 7 | 16 | 20 | Uses shared `LAB_2C7C` (2π) constant — keep constant (PI/ATN use it). | **CUT-IF-NEEDED** |
| **PI** | constant π | 10 | 16 | 20 | Same shared constant. Cheap & iconic — weaker cut. | **KEEP** (cheap, expected) |
| **POS** | `POS(x)` → cursor column | 2 | 6 | 10 | Falls through into `LAB_1FD0` (critical shared "Y→FAC1"). Handler is only `LDY TPos` (2 B). Negligible. | **KEEP** (2 B not worth it) |

### EXPERT / machine-code hooks

| Keyword | What it does | Handler B | TOMB | RENUM | Entanglement / risk | Rec |
| --- | --- | --- | --- | --- | --- | --- |
| **SYS** | `SYS addr[,A[,X[,Y]]]` — call machine code, capture regs | 89 | 96 | 98 | **DO NOT CUT.** This is the launch mechanism for the **entire NDK app ecosystem** — demo, turtle, sprite_editor, editui_shell, vsprite_rotate, keyboard all invoke via `SYS $addr` (assembly/apps/*/*.s, AUTOBOOT.bas). Dedicated test `AvaloniaRomSysCanPassAndCaptureRegisters`. | **KEEP (essential)** |
| **USR** | `USR(x)` — call user vector `Usrjmp` | 6 | 14 | 18 | Only 6 B handler (JSR Usrjmp + JMP). `Usrjmp` ZP vector stays. Low yield. | **KEEP** (tiny; SYS supersedes use) |

### LIKELY KEEPERS (cheap + useful; sized for completeness)

| Keyword | What it does | Handler B | TOMB | Entanglement | Rec |
| --- | --- | --- | --- | --- | --- |
| **DEC + INC** (pair) | decrement / increment variable(s) | 45 | 59 | DEC is 3 B then falls into INC's shared body; remove both or neither. `IncrErr` local. Useful, idiomatic. | **KEEP** |
| **DEEK** | `DEEK(a)` → 16-bit peek | 19 | 28 | Clean; pairs with DOKE. | **KEEP** |
| **DOKE** | `DOKE a,v` → 16-bit poke | 37 | 45 | Clean. Dedicated tests use DOKE (`DOKE 24576,ADDR(...)`). | **KEEP** |
| **HEX$** | number → hex string | 44 | 53 | Shares `LAB_BHSS`/`EndBHS`/`GoPr` tail with BIN$ (counted in BIN$ block). | **KEEP** |
| **BIN$** | number → binary string | 79* | 88* | *includes shared HEX$/BIN$ tail. Both together ≈ 123 B. | **KEEP** |
| **UCASE$** | upper-case a string | 70 | 81 | Sizable, but standard string surface. | **KEEP** |
| **LCASE$** | lower-case a string | 33 | 44 | Standard. | **KEEP** |
| **BITTST** | `BITTST(a,n)` → bit n of byte at a | 40 | 51 | Low-level but tested/useful. | **KEEP** |
| **BITSET** | set a bit | 6 | 16 | Tiny handler. | **KEEP** |
| **BITCLR** | clear a bit | 5 | 14 | Tiny handler. | **KEEP** |

### KEEP (essential — do not touch)

`LOAD` / `SAVE` are **NOT** legacy console/cassette. `TK_LOAD`/`TK_SAVE` dispatch
through `V_LOAD`/`V_SAVE` (`JMP (VEC_LD/VEC_SV)`, 3-byte vectors) to **`LAB_FLOAD`
/ `LAB_FSAVE`** (basic.asm:9945/9893), the **primary Nova file path** that routes
through `lib_call(FILES)` to the FileIoController. They are the working SD
save/load. **KEEP.** (The Nova `XLOAD`/`XSAVE`/`GSAVE`/`GLOAD` are *additional*
file ops, not replacements.)

All core language (END/FOR/NEXT/IF/GOTO/GOSUB/RETURN/PRINT/INPUT/READ/DATA/DIM/
LET/RUN/REM/STOP/ON/DEF/CONT/LIST/CLEAR/NEW/GET/DO/LOOP + operators) and core
functions (SGN/INT/ABS/SQR/RND/LOG/EXP/COS/SIN/TAN/ATN/LEN/STR$/VAL/ASC/CHR$/
LEFT$/RIGHT$/MID$/FRE/PEEK/POKE) are essential and out of scope.

---

## Recommended cut list (ranked by bytes-per-risk, tombstone mode)

Tombstone mode chosen as the realistic baseline (no token renumbering → no broken
saved programs, no FPGA token-test churn). Each cut also requires deleting its
help reference page and any dedicated test.

| # | Cut | TOMB bytes | Risk | Running total |
| --- | --- | --- | --- | --- |
| 1 | **MAX + MIN** | 121 (118 if WIDTH kept) | Low — no .bas use; help-only | 121 |
| 2 | **WIDTH** | 91 | Low-Med — must zero-init TWidth/Iclim; shares TabErr/LAB_NOSQ with SQR | 212 |
| 3 | **SWAP** | 48 | Low | 260 |
| 4 | **SADD** | 34 | Low | 294 |
| 5 | **WAIT** | 34 | Low | 328 |
| 6 | **VARPTR** | 27 | Low | 355 |
| 7 | **TWOPI** | 16 | Low | 371 |
| 8 | **NULL** | 11 | Low (already a no-op) | 382 |

**Total at cuts 1–8: ~382 bytes** (≈379 if WIDTH and MAX/MIN both go and the
shared `LAB_MMSE` is reclaimed). This **clears the ~350 B xram+xmc target** with
cuts 1–6 alone (355 B), with margin from the existing 44 B free → ~399 B
available after cuts 1–6.

### If a larger endgame target is needed (e.g. fold whole extension ROM in)

- Add the EXPERT/keeper tier as CUT-IF-NEEDED: USR (14), BITSET (16), BITCLR (14),
  POS (6) → +50 B.
- A **tail renumber batch** could additionally reclaim dispatch slots, but only if
  the cut keywords are moved to the tail of their range first — not worth the
  saved-program/token-test breakage for the byte counts here.
- The biggest non-vocabulary lever remains relocating large handlers (the
  2026-04-25 COPPER relocation freed 358 B by itself; see
  `project_copper_relocation_2026_04_25`). Vocabulary pruning and handler
  relocation are complementary.

---

## Honest assessment

The vocabulary is **leaner than "tons of junk" suggests.** There is almost no
truly DEAD code: every candidate handler is reachable, and the only near-no-op is
NULL (3 B). The real recoverable bytes come from **NICHE conveniences with
substantial handlers** — MAX/MIN (121), WIDTH (91), SWAP (48), SADD (34) — not
from dead weight.

Key correctness findings that change the naive picture:

1. **LOAD/SAVE are load-bearing** (the Nova file path), not legacy. Not removable.
2. **SYS is load-bearing** (every NDK app launches via SYS). Not removable.
3. **Token value == dispatch index**, so middle-of-range removals only reclaim
   handler+name+LIST in tombstone mode; the dispatch slot stays unless you
   renumber (which breaks saved programs and token tests).

Cutting the NICHE/JUNK tier (cuts 1–8) plausibly frees **~380 B in tombstone
mode**, comfortably covering the ~350 B xram+xmc domain with room to spare. If the
endgame needs substantially more than ~430 B, vocabulary pruning alone will not
get there — pair it with handler relocation (the COPPER-style lib_call pattern).
