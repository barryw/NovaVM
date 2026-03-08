# EhBASIC ROM Refactor Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Recover ~327 bytes from the EhBASIC ROM through dead code removal, code deduplication, and micro-optimizations.

**Architecture:** The ROM is a single 16KB assembly file (`ehbasic/basic.asm`) compiled with ca65/ld65 into `basic.bin` at $C000-$FFFF. Currently 16,253 of 16,254 usable bytes are occupied. We refactor in-place, building and verifying byte counts after each change.

**Tech Stack:** ca65/ld65 assembler, MSTest (.NET), `dotnet test`

---

### Task 1: Establish Baseline (ROM Size + Tests)

**Files:**
- Read: `ehbasic/basic.asm`
- Read: `ehbasic/Makefile`

**Step 1: Build the ROM and record baseline size**

```bash
cd ehbasic && make clean && make
```

Record: `wc -c basic.bin` should report 16384 bytes.

**Step 2: Run the full test suite to confirm green baseline**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

Expected: All tests pass.

**Step 3: Record the CODE segment size from the linker map**

```bash
grep "^CODE" ehbasic/basic.map
```

Expected: `CODE 00C000 00FF7C 003F7D` (16,253 bytes).

---

### Task 2: Remove Dead XMC API Stubs (~57 bytes)

**Files:**
- Modify: `ehbasic/basic.asm` lines 11108-11164

**Step 1: Verify the stubs are unreferenced**

Search `basic.asm` for any reference to `LAB_XM_CMDCHK`, `LAB_XM_SETADDR`, `LAB_XM_STATUS`, `LAB_XM_GETBYTE`, `LAB_XM_PUTBYTE`, `LAB_XM_STASH`, `LAB_XM_FETCH`, `LAB_XM_FILL`, `LAB_XM_ALLOC` outside lines 11108-11164. Confirm all references are internal to the block (the stubs call each other but nothing else calls them).

Expected: Zero external references. The BASIC XMEM commands use `LAB_XMC_CHKOK` instead.

**Step 2: Delete the dead code block**

Remove the entire block from `LAB_XM_CMDCHK` through `LAB_XM_ALLOC` (lines 11108-11164), including the comment header `; --- XMC assembly helper routines ---`.

**Step 3: Build and verify size reduction**

```bash
cd ehbasic && make clean && make && wc -c basic.bin
```

Expected: 16384 (ROM is padded/filled). Check the map instead:
```bash
grep "^CODE" basic.map
```

Expected: CODE size should be ~0x3F44 (16,196 bytes), down ~57 bytes from 0x3F7D.

**Step 4: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

Expected: All tests pass.

**Step 5: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: remove dead XMC API stubs from ROM (~57 bytes)"
```

---

### Task 3: Factor VGC 2-Parameter XY Commands (~46 bytes)

PLOT, UNPLOT, and PAINT share identical parameter-loading code. Extract a shared subroutine.

**Files:**
- Modify: `ehbasic/basic.asm` — PLOT (~line 9112), UNPLOT (~line 9129), PAINT (~line 9253)

**Step 1: Add a shared subroutine**

Place this near the VGC command area (just before LAB_PLOT):

```asm
; Load x,y pair into VGC_P0-P3 (x=16-bit word, y=byte)
; Returns with next BASIC byte in A
LAB_VGC_XY
      JSR   LAB_GTWRD         ; get x (0-319) as 16-bit
      LDA   FAC1_3
      STA   VGC_P0             ; x low
      LDA   FAC1_2
      STA   VGC_P1             ; x high
      JSR   LAB_1C01           ; require comma
      JSR   LAB_GTBY           ; get y (0-199)
      STX   VGC_P2             ; y low
      LDA   #$00
      STA   VGC_P3             ; y high = 0
      RTS
```

This is ~26 bytes.

**Step 2: Replace PLOT, UNPLOT, and PAINT**

Replace each command body with:

```asm
LAB_PLOT
      JSR   LAB_VGC_XY
      LDA   #VCMD_PLOT
      STA   VGC_CMD
      RTS
```

```asm
LAB_UNPLOT
      JSR   LAB_VGC_XY
      LDA   #VCMD_UNPLOT
      STA   VGC_CMD
      RTS
```

```asm
LAB_PAINT
      JSR   LAB_VGC_XY
      LDA   #VCMD_PAINT
      STA   VGC_CMD
      RTS
```

Each caller is 9 bytes. Three callers = 27 bytes.

Before: 3 x 30 bytes = 90 bytes.
After: 26 (subroutine) + 27 (callers) = 53 bytes.
**Savings: ~37 bytes.**

**Step 3: Build and check CODE segment size**

```bash
cd ehbasic && make clean && make && grep "^CODE" basic.map
```

Expected: CODE size reduced by ~37 bytes from previous step.

**Step 4: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

Expected: All tests pass.

**Step 5: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: factor VGC 2-param XY loading into shared subroutine (~37 bytes)"
```

---

### Task 4: Factor VGC 4-Parameter XY Commands (~89 bytes)

LINE, RECT, and FILL share identical x0,y0,x1,y1 parameter loading. Extract a shared subroutine.

**Files:**
- Modify: `ehbasic/basic.asm` — LINE (~line 9146), RECT (~line 9197), FILL/FILLRECT (~line 9225)

**Step 1: Add a shared subroutine**

Place near LAB_VGC_XY:

```asm
; Load x0,y0,x1,y1 into VGC_P0-P7
LAB_VGC_XYXY
      JSR   LAB_VGC_XY         ; load x0,y0 into P0-P3
      JSR   LAB_1C01           ; require comma
      JSR   LAB_GTWRD          ; x1
      LDA   FAC1_3
      STA   VGC_P4
      LDA   FAC1_2
      STA   VGC_P5
      JSR   LAB_1C01
      JSR   LAB_GTBY           ; y1
      STX   VGC_P6
      LDA   #$00
      STA   VGC_P7
      RTS
```

This reuses LAB_VGC_XY from Task 3. ~25 bytes.

**Step 2: Replace LINE, RECT, and FILL**

```asm
LAB_GLINE
      JSR   LAB_VGC_XYXY
      LDA   #VCMD_LINE
      STA   VGC_CMD
      RTS
```

```asm
LAB_RECT
      JSR   LAB_VGC_XYXY
      LDA   #VCMD_RECT
      STA   VGC_CMD
      RTS
```

```asm
LAB_FILLRECT
      JSR   LAB_VGC_XYXY
      LDA   #VCMD_FILL
      STA   VGC_CMD
      RTS
```

Before: 3 x ~50 bytes = ~150 bytes.
After: 25 (subroutine) + 27 (callers) = 52 bytes.
**Savings: ~98 bytes.** (Higher than initial estimate since LAB_VGC_XYXY reuses LAB_VGC_XY.)

**Step 3: Build and check CODE segment size**

```bash
cd ehbasic && make clean && make && grep "^CODE" basic.map
```

**Step 4: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

Expected: All tests pass.

**Step 5: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: factor VGC 4-param XYXY loading into shared subroutine (~98 bytes)"
```

---

### Task 5: Factor XMEM Parameter Loading (~38 bytes)

**Files:**
- Modify: `ehbasic/basic.asm` — XSTASH (~lines 10923-10942), XFETCH (~lines 10967-10991)

**Step 1: Add a shared subroutine**

```asm
; Parse word argument and store in XMC_RAML/H
LAB_XMC_GETRAM
      JSR   LAB_GTWRD
      LDA   FAC1_3
      STA   XMC_RAML
      LDA   FAC1_2
      STA   XMC_RAMH
      RTS
```

~13 bytes.

**Step 2: Replace the 4 occurrences**

Replace each 5-instruction sequence (`JSR LAB_GTWRD / LDA FAC1_3 / STA XMC_RAML / LDA FAC1_2 / STA XMC_RAMH`) with `JSR LAB_XMC_GETRAM` (3 bytes).

Before: 4 x 10 bytes = 40 bytes.
After: 13 (subroutine) + 12 (callers) = 25 bytes.
**Savings: ~15 bytes.**

**Step 3: If the LEN pattern also repeats, add a second helper**

Check if `LDA FAC1_3 / STA XMC_LENL / LDA FAC1_2 / STA XMC_LENH` appears 2+ times after a `JSR LAB_GTWRD`. If so, add:

```asm
LAB_XMC_GETLEN
      JSR   LAB_GTWRD
      LDA   FAC1_3
      STA   XMC_LENL
      LDA   FAC1_2
      STA   XMC_LENH
      RTS
```

Similar savings of ~7 bytes if 2 occurrences.

**Step 4: Build and check**

```bash
cd ehbasic && make clean && make && grep "^CODE" basic.map
```

**Step 5: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

**Step 6: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: factor XMEM parameter loading into shared subroutines (~22 bytes)"
```

---

### Task 6: Add Keyword Skip Helper (~50 bytes)

**Files:**
- Modify: `ehbasic/basic.asm` — MUSIC/COPPER command areas

**Step 1: Add a skip-N-characters subroutine**

```asm
; Skip X characters from BASIC input
; Entry: X = number of characters to skip
LAB_SKIPX
      JSR   LAB_IGBY
      DEX
      BNE   LAB_SKIPX
      RTS
```

~7 bytes.

**Step 2: Replace chains of 3+ consecutive JSR LAB_IGBY**

For each chain of N consecutive `JSR LAB_IGBY` calls, replace with:

```asm
      LDX   #N
      JSR   LAB_SKIPX
```

5 bytes replaces N*3 bytes. Only profitable when N >= 3.

Known targets (from analysis):
| Location | N | Old bytes | New bytes | Savings |
|----------|---|-----------|-----------|---------|
| MUSIC PLAY (~line 9817) | 3 | 9 | 5 | 4 |
| MUSIC TEMPO (~line 9834) | 5 | 15 | 5 | 10 |
| COPPER USE (~line 10014) | 3 | 9 | 5 | 4 |
| COPPER ADD (~line 10025) | 3 | 9 | 5 | 4 |
| COPPER ADD BGCOL (~line 10049) | 5 | 15 | 5 | 10 |
| COPPER ADD SCROLL (~line 10072) | 5 | 15 | 5 | 10 |
| COPPER ADD SPRSHAPE (~line 10146) | 5 | 15 | 5 | 10 |
| COPPER ADD SPRFLAGS (~line 10155) | 5 | 15 | 5 | 10 |
| COPPER ADD SPRPRI (~line 10164) | 3 | 9 | 5 | 4 |

Total: 9 replacements. Old: 111 bytes. New: 45 bytes + 7 (subroutine) = 52 bytes.
**Savings: ~59 bytes.**

**Important:** Verify that X is not live/needed after these skip sequences. The subroutine clobbers X (leaves it at 0). If X is needed, save/restore as needed.

**Step 3: Build and check**

```bash
cd ehbasic && make clean && make && grep "^CODE" basic.map
```

**Step 4: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

**Step 5: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: add LAB_SKIPX helper to replace keyword skip chains (~59 bytes)"
```

---

### Task 7: Convert JSR+RTS Tail Calls to JMP (~2 bytes)

**Files:**
- Modify: `ehbasic/basic.asm` — lines ~8624 and ~10886

**Step 1: Replace both instances**

At line ~8624-8625 (LAB_NCC confirmation handler):
```asm
; Before:
      JSR   LAB_CRLF
      RTS
; After:
      JMP   LAB_CRLF
```

At line ~10886-10887 (LAB_XMEM status display):
```asm
; Before:
      JSR   LAB_CRLF
      RTS
; After:
      JMP   LAB_CRLF
```

**Savings: 2 bytes** (1 per instance).

**Step 2: Build and check**

```bash
cd ehbasic && make clean && make && grep "^CODE" basic.map
```

**Step 3: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

**Step 4: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: convert JSR+RTS tail calls to JMP (2 bytes)"
```

---

### Task 8: MUSIC PRIORITY Comma-Parse Refactor (~45 bytes)

**Files:**
- Modify: `ehbasic/basic.asm` — lines ~9890-9921

**Step 1: Add a table-driven loop**

Replace the 6 repetitive comma-parse blocks with a loop over a target-address table:

```asm
; Table of FIO register targets for MUSIC PRIORITY voices
LAB_MPRI_TBL
      .byte <FIO_SRCL, <FIO_SRCH, <FIO_ENDL
      .byte <FIO_ENDH, <FIO_SIZEL, <FIO_SIZEH

LAB_MPRI_PARSE
      LDX   #$00              ; table index
      JSR   LAB_GTBY           ; first voice (mandatory)
      TXA
      TAY                      ; Y = table index (0)
      LDA   LAB_MPRI_TBL,Y
      TAX
      ; ... store to indexed FIO register
```

**Note:** This optimization depends on the FIO registers being in a contiguous or indexable range. Before implementing, verify the actual addresses of FIO_SRCL, FIO_SRCH, FIO_ENDL, FIO_ENDH, FIO_SIZEL, FIO_SIZEH. If they are not contiguous, use an address table approach instead.

**Alternative simpler approach** if addresses aren't contiguous — use a self-modifying store (not possible in ROM) or an indirect indexed pattern. If this proves too complex, skip this task. The other tasks already provide ~250+ bytes of savings.

**Step 2: Build and check**

```bash
cd ehbasic && make clean && make && grep "^CODE" basic.map
```

**Step 3: Run tests**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

**Step 4: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "refactor: loop-based MUSIC PRIORITY comma parsing (~45 bytes)"
```

---

### Task 9: Final Verification and Copy ROM

**Step 1: Full rebuild**

```bash
cd ehbasic && make clean && make
```

**Step 2: Check final CODE segment size**

```bash
grep "^CODE" basic.map
```

Calculate total bytes saved vs. baseline of 0x3F7D (16,253).

**Step 3: Run full test suite**

```bash
cd /Users/barry/Git/e6502 && dotnet test
```

Expected: All tests pass.

**Step 4: Copy ROM to resources**

```bash
cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin
```

**Step 5: Rebuild and retest with new ROM in place**

```bash
dotnet build && dotnet test
```

**Step 6: Commit the updated ROM binary**

```bash
git add e6502.Avalonia/Resources/ehbasic.bin
git commit -m "chore: update ROM binary with refactored EhBASIC (~300+ bytes recovered)"
```

---

## Expected Total Savings

| Task | Savings |
|------|---------|
| 2: Dead XMC stubs | ~57 bytes |
| 3: VGC 2-param XY | ~37 bytes |
| 4: VGC 4-param XYXY | ~98 bytes |
| 5: XMEM param loading | ~22 bytes |
| 6: Keyword skip helper | ~59 bytes |
| 7: JSR+RTS tail calls | 2 bytes |
| 8: MUSIC PRIORITY loop | ~45 bytes |
| **Total** | **~320 bytes** |

Each task is independent and can be skipped if it proves problematic. Tasks 2-4 and 6 provide the bulk of the savings (~251 bytes).
