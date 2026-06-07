# BASIC C64-Style Screen Editor — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Give EhBASIC a Commodore-64-style full-screen editor — arrow the cursor anywhere on screen, edit a line in place, press ENTER and BASIC re-ingests whatever physical row the cursor is on (store it if it starts with a line number, else execute it).

**Architecture:** A new SYSTEM-module fn `SYS_SCREEN_READLINE` ($12) runs the entire edit loop against **VGC MMIO** (so it works on Avalonia *and* FPGA): poll `VGC_CHARIN`, echo printables by writing the char-plane screen window directly, move the cursor on arrow codes, and on ENTER read the physical row under the cursor into BASIC's `Ibuffs` and return its length. BASIC's `LAB_1357` becomes a thin wrapper that calls it; the downstream tokenize + "line# → store / else execute" path is unchanged. The C# host stops moving the cursor itself (the 6502 module owns cursor motion now).

**Tech Stack:** ca65 6502/65C02 (SYSTEM module `modules/system/system.s`, BASIC `ehbasic/basic.asm`), C# (`e6502.Avalonia` host: `EmulatorCanvas`, `ScreenEditor`), MSTest (`e6502UnitTests`).

**Design ref:** `docs/plans/2026-06-07-basic-screen-editor-design.md`. Scope = **Faithful MVP** (single physical row = one logical line ≤80 chars; overwrite typing + destructive backspace; ENTER reads the row; scroll on ENTER at bottom). Deferred: lines >80 chars (multi-row link table), INSERT mode.

---

## Key facts (verified — do not re-derive)

- **Screen is 80×50** (not 25). `NOVA_SCREEN_COLS=80`, `NOVA_SCREEN_ROWS=50` (`runtime/asm/nova.inc`). Clamp cursor X 0–79, Y 0–49. (CLAUDE.md's "80×25" is stale.)
- **Char-plane screen window:** `VGC_SCREENWIN=$A200` (nova.inc:433) maps 4000 cells of the plane selected by `VGC_SCREENWIN_PLANE=$B1A0`; char plane = `VGC_SCREENWIN_CHAR=$00`. Cell (col,row) = `$A200 + row*80 + col`. Set the plane to char once at loop start, then plain LDA/STA.
- **Cursor regs:** `VGC_CURSX=$A003`, `VGC_CURSY=$A004`, `VGC_CURSEN=$A00A` (b0 = visible).
- **Key input:** read `VGC_CHARIN=$A00F` — returns next queued key byte, 0 if empty. Poll until non-zero.
- **Key codes** (host → CHARIN): Left=28, Right=29, Up=30, Down=31, Enter=$0D, Backspace=$08, Ctrl-C=$03, printables=ASCII.
- **Do NOT echo via `VGC_CHAROUT=$A00E`** — it auto-scrolls at the bottom and interprets control codes, which fights the editor. Write the screen window directly.
- **Ring-scroll base:** `VGC_TEXT_TOPROW=$A0ED` (first physical row shown, 0–49). ⚠️ Whether `$A200`-window row R corresponds to visible row `CURSY` directly or to `(CURSY+TOPROW) mod 50` MUST be established empirically in Task 2 before building further.
- **`Ibuffs=$0275`** (basic.asm:622), capacity `Ibuffe-Ibuffs=$7F` (127). It's in low RAM page 2 — outside the module BSS band ($0420–$08FF) and survives the page-in. The module writes the line here (no trailing NUL, matching `LAB_1357`).
- **`LAB_1357`** (basic.asm:~1195, ~45B) is the current line reader; call sites `JSR LAB_1357` (~1021) and `BNE LAB_1357` (~1183). Downstream: `LAB_13A6` crunch (uppercases in-place + tokenizes) then store/execute. Replace LAB_1357's *body* with the lib_call wrapper so both call sites keep working.
- **SYSTEM module:** ids in `runtime/asm/libsystem.inc`; `SYS_FN_COUNT` currently `$12`. Add `SYS_SCREEN_READLINE=$12`, bump count to `$13`. Wire `modules/system/system.s` `sys_jtable` row $12 (mirror how `sys_addr_lookup` / `SYS_ADDR_LOOKUP=$11` was added: jtable `.word sys_screen_readline-1`, a `;@fn` doc block, finish with `LIB_STATUS` set). Module ZP band $A3–$C2 (~8B free); BSS band $0420–$08FF.
- **lib_call ABI:** `LIB_MOD_ID=$0300`, `LIB_FN_ID=$0301`, `LIB_STATUS=$0302`, `LIB_ARG0=$0303` (32-bit LE)…, `LIB_RESULT=$0313`. `JSR LIB_LOADER_BAND` ($0320). Returns A=`LIB_STATUS`. BASIC helper `basic_lib_call` (A=mod, X=fn).
- **Host double-move bug:** `EmulatorCanvas.cs` (~161–178) queues 28–31 AND calls `_editor.CursorLeft/Right/Up/Down()`. The module now owns cursor motion → remove the host cursor-move calls (keep `QueueInput`). Verify nothing else relies on them (replline-based runtimes drive the cursor themselves in 6502).

## Module ABI for `SYS_SCREEN_READLINE`
- **In:** `ARG0` b0/b1 = destination buffer ptr (`Ibuffs`=$0275); b2 = max length ($7F).
- **Out:** `LIB_RESULT` b0 = submitted line length; line bytes written to the buffer (no trailing NUL); `LIB_STATUS=LERR_OK`. (Reserve b1 of RESULT for an exit-reason later; set 0 for ENTER.)

## Build/test commands
- Build module + ROM: `make -C ehbasic && make -C modules/system`
- Stage: copy `ehbasic/basic.bin`→`e6502.Avalonia/Resources/ehbasic.bin`, `ehbasic/extension.bin`→Resources, `modules/system/system.bin`→`e6502.Avalonia/Resources/system.bin`
- Test: `dotnet test --filter "FullyQualifiedName~Basic"` (+ `~SystemModule`); full gate `dotnet test -p:SkipRomBuild=true`
- Mirror sync at the end: CLI Resources + `make -C e6502.FPGA hex` (see memory `reference_rom_mirror_sync`).

---

### Task 1: Wire the fn id + jtable + skeleton

**Files:**
- Modify: `runtime/asm/libsystem.inc` (add `SYS_SCREEN_READLINE=$12`; `SYS_FN_COUNT=$13`)
- Modify: `modules/system/system.s` (jtable row $12 → `sys_screen_readline-1`; `;@fn` doc block; skeleton handler)
- Modify: `e6502UnitTests/SystemModuleTests.cs` (the `SYS_FN_COUNT` header guard `0x12`→`0x13`)

**Step 1 — failing test:** update `SystemModule_Header_IsWellFormed` to expect `img[7]==0x13`. Run `dotnet test --filter "SystemModule_Header_IsWellFormed"` → FAIL (still $12).

**Step 2 — implement skeleton:** add the id + count; add jtable entry; add `sys_screen_readline:` that for now just `STZ LIB_RESULT` (len 0), `LDA #LERR_OK : STA LIB_STATUS : RTS`. Build `make -C modules/system`, copy `system.bin` to Resources.

**Step 3 — pass:** rerun → PASS. **Commit:** `feat(system): reserve SYS_SCREEN_READLINE fn id + skeleton`.

---

### Task 2: Establish the row mapping + read-row-on-ENTER (the core mechanic)

**Goal:** Prove how the `$A200` window row relates to `CURSY`/`TOPROW`, and implement "ENTER reads the physical row under the cursor into the buffer."

**Files:** `modules/system/system.s` (`sys_screen_readline` body); `e6502UnitTests/SystemModuleTests.cs` (new test).

**Step 1 — failing test** (`ScreenReadline_ReadsRowUnderCursorOnEnter`): set up bus+VGC+ScreenEditor; set `VGC_SCREENWIN_PLANE=char`; write "PRINT 7" into the char window at a known row R (cols 0..6) via the bus; set `VGC_CURSY=R`, `VGC_CURSX=0`; queue `$0D` (ENTER); set `ARG0`=Ibuffs ptr, b2=$7F; invoke `lib_call(SYSTEM,$12)`; assert `Ibuffs[0..6]=="PRINT 7"`, `LIB_RESULT b0==7`, trailing spaces trimmed. (Use the existing module-invocation harness pattern + `editor.QueueInput`.)

**Step 2 — implement:** loop body: set plane=char; poll `VGC_CHARIN`; on `$0D`: compute row base = `CURSY*80` (16-bit: `base = (Y<<6)+(Y<<4)` — provide a `screen_row_base` helper), read `$A200+base+col` for col 0..79 into the buffer, track last-non-space to trim, store length to `LIB_RESULT`, `LIB_STATUS=OK`, RTS. **If the read row is wrong, the test reveals the TOPROW interaction — adjust base to `((CURSY+TOPROW) mod 50)*80` and document the finding here.**

**Step 3 — pass + commit:** `feat(system): SYS_SCREEN_READLINE reads the cursor's screen row on ENTER`.

---

### Task 3: Echo printable typing into the screen window

**Files:** `modules/system/system.s`; test in `SystemModuleTests.cs`.

**Step 1 — failing test** (`ScreenReadline_TypedCharsLandOnScreenThenReturn`): start cursor at (0,R) on a blank row; queue `"GOTO 10"` + `$0D`; invoke; assert the char window row R now contains "GOTO 10" AND `Ibuffs=="GOTO 10"`.

**Step 2 — implement:** in the loop, for a byte ≥ $20: write it to `$A200 + CURSY*80 + CURSX`, then advance `CURSX`; if `CURSX==80` wrap `CURSX=0, CURSY++` (clamp `CURSY` ≤49 — MVP: stop at 79 if at last row, defer multi-row). Keep cursor regs in sync (`STA VGC_CURSX/Y`). Enable cursor (`VGC_CURSEN`=1) at entry, leave as-is on exit.

**Step 3 — pass + commit:** `feat(system): echo typed chars to the screen window`.

---

### Task 4: Arrow-key cursor navigation

**Files:** `modules/system/system.s`; test.

**Step 1 — failing test** (`ScreenReadline_ArrowsMoveCursorClamped`): from (5,5), queue Up,Up,Left,Right,Down (30,30,28,29,31) then `$0D`; assert final `VGC_CURSX/Y` == expected (with clamping at 0/79/49). Also a clamp test from (0,0): Up,Left stays (0,0).

**Step 2 — implement:** dispatch 28→`CURSX--` (≥0), 29→`CURSX++` (≤79), 30→`CURSY--` (≥0), 31→`CURSY++` (≤49); write back the cursor regs each move.

**Step 3 — pass + commit:** `feat(system): arrow-key cursor navigation with edge clamping`.

---

### Task 5: Destructive backspace

**Files:** `modules/system/system.s`; test.

**Step 1 — failing test** (`ScreenReadline_BackspaceErasesAndMovesLeft`): type "PRIMT", backspace, "NT" → row shows "PRINT", `Ibuffs=="PRINT"`. (MVP destructive backspace: at CURSX>0, CURSX--, write space at the new cell. Editing mid-line is overwrite-based; full shift-left DELETE is deferred.)

**Step 2 — implement:** byte `$08`: if `CURSX>0`, `CURSX--`, write `$20` to `$A200+CURSY*80+CURSX`, update `VGC_CURSX`.

**Step 3 — pass + commit:** `feat(system): destructive backspace in the screen editor`.

---

### Task 6: Advance + scroll after ENTER

**Files:** `modules/system/system.s`; test.

**Step 1 — failing test** (`ScreenReadline_EnterAdvancesCursorToNextRow` and `..._ScrollsAtBottom`): after ENTER on row R<49, cursor → (0,R+1). On row 49, the screen scrolls up one row (top row drops) and cursor stays at (0,49); assert the previously-row-48 content is now on row 47 (or whatever the ring-scroll semantics from Task 2 dictate — reuse `VGC_TEXT_TOPROW` if that's the scroll mechanism).

**Step 2 — implement:** after reading the row on `$0D`: if `CURSY<49`, `CURSY++, CURSX=0`; else perform a one-row scroll (the mechanism confirmed in Task 2 — likely bump `VGC_TEXT_TOPROW` and clear the newly-exposed row) and set `CURSX=0`. Document the chosen scroll mechanism.

**Step 3 — pass + commit:** `feat(system): cursor advance + scroll on ENTER`.

---

### Task 7: BASIC integration — route input through the module

**Files:** `ehbasic/basic.asm` (`LAB_1357` body → wrapper); test `e6502UnitTests/BasicRegressionTests.cs`.

**Step 1 — failing integration test** (`ScreenEditor_EditListedLineUpdatesProgram`): fresh BASIC; type `10 PRINT 1`+ENTER, `20 PRINT 2`+ENTER, `LIST`+ENTER; then queue Up arrows to land on the listed "10 PRINT 1" row, overwrite the `1` with `9` (position cursor, type `9`), ENTER; then `RUN`+ENTER; assert output shows `9` then `2` (line 10 was updated from the screen). This FAILS on current `LAB_1357` (no navigation; ENTER submits the typed buffer, not the screen row).

**Step 2 — implement:** replace the body of `LAB_1357` with: `JSR LAB_CURS_ON` (or let the module own cursor visibility); marshal `Ibuffs` ptr → `LIB_ARG0` b0/b1, `#$7F`→b2; `LDA #MODULE_ID_SYSTEM : LDX #SYS_SCREEN_READLINE : JSR basic_lib_call`; on return the line is in `Ibuffs`; preserve whatever register/Z state the downstream crunch expects (match old LAB_1357 exit — it fell through to `LAB_CURS_OFF_CR`). Keep the `LAB_1357` label (both call sites jump to it). Retire the old loop body (frees ~30B).

**Step 3 — pass + commit:** `feat(basic): route line input through SYS_SCREEN_READLINE`. Run the full `~Basic` suite — all green.

---

### Task 8: Host — stop double-moving the cursor

**Files:** `e6502.Avalonia/Rendering/EmulatorCanvas.cs` (and `ScreenEditor.cs` if needed); test.

**Step 1 — failing test** (`ArrowKeyMovesCursorExactlyOnce` or assert via the integration test): with the module owning cursor motion, the host must NOT also move it. Currently the host both queues 28–31 and calls `_editor.CursorLeft/...`.

**Step 2 — implement:** remove the `_editor.CursorLeft/Right/Up/Down()` calls from the arrow-key cases (keep `QueueInput(28..31)`). Confirm no other consumer needs host-side cursor motion (replline runtimes move the cursor in 6502). The dead `CursorUp/Down/Left/Right`/`ReadLineFromScreen` methods in `ScreenEditor.cs` can be removed.

**Step 3 — pass + commit:** `fix(host): module owns cursor motion; host only queues arrow codes`.

---

### Task 9: Finalize — mirrors, full suite, docs

**Steps:**
1. Build all; copy `basic.bin`/`extension.bin`/`system.bin` to Avalonia Resources; `cp` to CLI Resources; `make -C e6502.FPGA hex`; verify mirrors byte-match; revert any build-collateral PDF.
2. `dotnet test -p:SkipRomBuild=true` → full suite green.
3. Update `docs/plans/2026-06-07-basic-screen-editor-design.md` status to SHIPPED; fix CLAUDE.md's stale "80×25" → "80×50".
4. Report main-ROM delta (LAB_1357 shrink) + system-module growth (system.bin ≤16384).
5. **Commit + two-stage review (spec + code) + ff-merge to main** per the established per-phase process.

---

## Risks / watch-items
- **Row mapping vs ring-scroll (`TOPROW`)** — Task 2 must pin this empirically; everything downstream depends on reading the *right* row.
- **`row*80` 16-bit math** — provide one tested `screen_row_base` helper; reuse in echo/read/backspace/scroll.
- **Module ZP/BSS budget** — keep editor state minimal (cursor x/y live in VGC regs; only need a few scratch bytes — fit in the ~8B free ZP or the BSS band).
- **FPGA arrow keys** — the module reads CHARIN identically on FPGA, but confirm NovaHost delivers arrow codes 28–31 to the key FIFO (batched HW smoke, not blocking Avalonia).
- **`LAB_CURS_ON/OFF`** — decide once whether BASIC or the module owns cursor visibility; don't leave the cursor stuck on/off after input.
