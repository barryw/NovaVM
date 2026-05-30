# NovaLogo Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a full Logo interpreter as a standalone 16KB ROM for the Nova 6502 platform, with hardware turtle graphics, math coprocessor integration, and Logo-native vocabulary for all Nova hardware.

**Architecture:** The interpreter is a classic REPL with tokenizer → parser → tree-walking evaluator. All dynamic data (lists, words, procedures, scope frames) lives on a managed heap with mark-and-sweep GC. The turtle is a hardware vsprite. NDK routines are callable via a curated binding table, raw CALL, or user NATIVE registration.

**Tech Stack:** ca65/ld65 (6502 assembler/linker), MSTest (C# integration tests), CompositeBusDevice (Avalonia emulator bus)

**Reference:** Design doc at `docs/plans/2026-05-27-novalogo-language-design.md`

---

## Phase 1: Skeleton ROM & REPL

Get a minimal ROM that boots, prints a banner, accepts input, and echoes it back. This proves the ROM slot, build system, and I/O path work end-to-end.

### Task 1.1: Create directory structure and build system

**Files:**
- Create: `novalogo/novalogo.s` (main source)
- Create: `novalogo/novalogo.cfg` (linker script)
- Create: `novalogo/Makefile`

**Step 1: Create the linker config**

Model after `ehbasic/ehbasic.cfg`. NovaLogo occupies the same $C000-$FFFF ROM slot.

```
; novalogo/novalogo.cfg
MEMORY {
    ZP:     file = "",  define = yes, start = $0030, size = $0050;
    BSS:    file = "",  define = yes, start = $0280, size = $7D80;
    ROM:    file = %O,                start = $C000, size = $3FD7, fill = yes;
    MONROM: file = %O,                start = $FFD7, size = $0023, fill = yes;
    VECROM: file = %O,                start = $FFFA, size = $0006;
}
SEGMENTS {
    ZEROPAGE: load = ZP,     type = zp;
    BSS:      load = BSS,    type = bss;
    CODE:     load = ROM,    type = ro,  start = $C000;
    RODATA:   load = ROM,    type = ro;
    MONITOR:  load = MONROM, type = ro,  start = $FFD7;
    VECTORS:  load = VECROM, type = ro,  start = $FFFA;
}
```

**Step 2: Create the Makefile**

```makefile
NOVA_ASM = ../runtime/asm

novalogo.bin: novalogo.o novalogo.cfg
	ld65 -C novalogo.cfg -o $@ novalogo.o

novalogo.o: novalogo.s $(NOVA_ASM)/nova.inc
	ca65 --cpu 65c02 -I $(NOVA_ASM) -o $@ novalogo.s

clean:
	rm -f *.o *.bin

.PHONY: clean
```

**Step 3: Create the minimal ROM source**

Write `novalogo/novalogo.s` with:
- Reset vector handler at $FFD7 (CLD, set stack, JMP to cold start)
- NMI/IRQ vectors (RTI stubs for now)
- Cold start: print banner via VGC_CHAROUT
- `?` prompt
- Character input loop: read VGC_CHARIN, echo VGC_CHAROUT
- Carriage return handling (newline, reprint `?`)

Use the same I/O pattern as EhBASIC's `ACIAin`/`ACIAout` in `min_mon.asm`:
- Output: `STA VGC_CHAROUT` ($A00E)
- Input: `LDA VGC_CHARIN` ($A00F), BEQ (no key), SEC (got key)

**Step 4: Build and verify**

Run: `cd novalogo && make`
Expected: `novalogo.bin` (16384 bytes)

**Step 5: Commit**

```
feat(novalogo): skeleton ROM with banner and input echo
```

---

### Task 1.2: Wire NovaLogo ROM into the Avalonia emulator

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs` (add RomSwapLogo constant)
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (load novalogo.bin, handle swap)
- Create: copy `novalogo/novalogo.bin` to `e6502.Avalonia/Resources/novalogo.bin`

**Step 1: Add ROM swap constant**

In `VgcConstants.cs`, add:
```csharp
public const byte RomSwapLogo = 0x05;
```

And extend the `ActiveRom` enum:
```csharp
public enum ActiveRom { Basic, Ncc, Extension, Logo }
```

**Step 2: Load NovaLogo ROM in CompositeBusDevice constructor**

Follow the `_extRom` pattern — load `novalogo.bin` into a `_logoRom` byte array at construction time. Add a field:
```csharp
private readonly byte[]? _logoRom;
```

Load from `Resources/novalogo.bin` (same pattern as extension.bin loading around line 188).

**Step 3: Handle ROM swap to Logo**

In the `RegRomSwap` write handler (around line 449), add a case:
```csharp
else if (data == VgcConstants.RomSwapLogo && _logoRom != null && CurrentRom != ActiveRom.Logo)
{
    Array.Copy(_logoRom, 0, _ram, VgcConstants.RomBase, 16384);
    CurrentRom = ActiveRom.Logo;
}
```

**Step 4: Add a boot-to-Logo option**

Add a method or constructor parameter to boot directly into Logo ROM instead of BASIC. The simplest approach: a `BootRom` property that controls which ROM is active after `InitVectorTable`. When set to Logo, copy `_logoRom` over `_ram` at RomBase before the CPU starts.

**Step 5: Build and run**

Run: `dotnet build`
Then: `dotnet run --project e6502.Avalonia` (with a way to select Logo ROM)

**Step 6: Commit**

```
feat(avalonia): wire NovaLogo ROM into emulator with boot selection
```

---

### Task 1.3: Write boot integration test

**Files:**
- Create: `e6502UnitTests/NovaLogoTests.cs`

**Step 1: Write the test**

Follow the `BasicRegressionTests` pattern. The test should:
1. Create a `CompositeBusDevice` with Logo ROM active
2. Boot the CPU
3. Run cycles until screen contains `NOVALOGO`
4. Assert the banner text and `?` prompt are visible

```csharp
[TestClass]
public class NovaLogoTests
{
    [TestMethod]
    public void BootShowsBannerAndPrompt()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NOVALOGO"));
        Assert.IsTrue(screen.Contains("?"));
    }
}
```

Reuse/share the `RunUntilScreenContains` and `SnapshotScreen` helpers from `BasicRegressionTests` (extract to a shared base class or static helper if needed).

**Step 2: Run test to verify it passes**

Run: `dotnet test --filter "BootShowsBannerAndPrompt"`
Expected: PASS

**Step 3: Commit**

```
test(novalogo): boot integration test — banner and prompt
```

---

### Task 1.4: Line input buffer

**Files:**
- Modify: `novalogo/novalogo.s`

**Step 1: Write failing test**

Add to `NovaLogoTests.cs`:
```csharp
[TestMethod]
public void TypeCommandAndEnterShowsOutput()
{
    // Boot Logo, type "PRINT 42" + Enter, check screen for "42"
    // (PRINT won't work yet — for now just test that typed text appears and
    // Enter produces a new ? prompt)
}
```

**Step 2: Implement line input**

In `novalogo.s`, add:
- Input buffer at $0280 (128 bytes, same location as EhBASIC's Ibuffs)
- `read_line` routine: accumulate characters into buffer until CR
- Handle backspace (decrement pointer, erase character on screen)
- On CR: null-terminate buffer, print newline
- After processing (initially just reprint `?` prompt), loop back to `read_line`

The input buffer will later feed the tokenizer.

**Step 3: Run tests**

Run: `dotnet test --filter "NovaLogo"`
Expected: PASS

**Step 4: Commit**

```
feat(novalogo): line input buffer with backspace handling
```

---

## Phase 2: Tokenizer

Break input lines into tokens. This is the front-end of the interpreter pipeline.

### Task 2.1: Token types and data structures

**Files:**
- Create: `novalogo/tokenizer.s` (included from novalogo.s)
- Create: `novalogo/heap.s` (basic heap allocator — needed for token storage)

**Step 1: Define token node format**

Each token is a small heap node:
```
Offset 0: tag byte (token type)
Offset 1: GC mark + next pointer (low)
Offset 2: next pointer (high)
Offset 3+: payload (varies by type)
```

Token types:
```
TOK_WORD    = $01  ; payload: length byte + chars
TOK_NUMBER  = $02  ; payload: 3 bytes (16.8 fixed-point)
TOK_LBRACKET = $03 ; no payload
TOK_RBRACKET = $04 ; no payload
TOK_VARREF  = $05  ; payload: length byte + chars (the name after :)
TOK_QUOTE   = $06  ; payload: length byte + chars (the word after ")
TOK_INFIX   = $07  ; payload: 1 byte (operator: +, -, *, /, <, >, =)
TOK_LPAREN  = $08  ; no payload
TOK_RPAREN  = $09  ; no payload
```

**Step 2: Implement basic heap allocator**

In `heap.s`:
- Heap range: $0400-$9FFF
- `heap_init`: set free pointer to $0400
- `heap_alloc(size in A)`: bump-allocate, return pointer in AY. For now, simple bump allocator — GC comes later.
- Track heap top in zero page

**Step 3: Commit**

```
feat(novalogo): token types and bump heap allocator
```

---

### Task 2.2: Tokenizer — word and number scanning

**Files:**
- Modify: `novalogo/tokenizer.s`

**Step 1: Write test**

Add to `NovaLogoTests.cs` — a test that types `PRINT 42` + Enter and verifies the tokenizer produces the right token sequence. Since we can't directly inspect tokens from C#, test indirectly: implement a `TOKENTEST` debug command that tokenizes the input and prints each token type and value.

**Step 2: Implement tokenizer**

`tokenize_line` routine:
- Input: pointer to null-terminated input buffer
- Output: linked list of token nodes on the heap
- Skip whitespace between tokens
- `:` prefix → TOK_VARREF (read name that follows)
- `"` prefix → TOK_QUOTE (read word that follows)
- `[` → TOK_LBRACKET
- `]` → TOK_RBRACKET
- `(` → TOK_LPAREN
- `)` → TOK_RPAREN
- `+`, `-` (when not start of number), `*`, `/`, `<`, `>`, `=` → TOK_INFIX
- Digit or `-` followed by digit → parse number into 16.8 fixed-point → TOK_NUMBER
- Everything else → TOK_WORD (read until whitespace/bracket/paren)

Number parsing (integer to 16.8):
- Parse decimal digits into 16-bit integer
- If `.` encountered, parse fractional digits into 8-bit fraction
- Store as 3 bytes: integer high, integer low, fraction

**Step 3: Commit**

```
feat(novalogo): tokenizer — words, numbers, brackets, operators
```

---

## Phase 3: Core Evaluator

### Task 3.1: Built-in procedure table

**Files:**
- Create: `novalogo/builtins.s`

**Step 1: Define the primitive table**

A table in ROM mapping word names to handler addresses and arities:
```
; Each entry: name-ptr (2), handler-addr (2), arity (1), flags (1)
builtin_table:
    .word str_print, do_print, 1, 0
    .word str_type,  do_type,  1, 0
    .word str_make,  do_make,  2, 0
    .word str_thing, do_thing, 1, 0
    ; ... etc
    .word $0000  ; sentinel
```

`lookup_builtin`: given a word token, scan the table for a match. Return handler address and arity, or carry-set if not found.

**Step 2: Commit**

```
feat(novalogo): built-in procedure lookup table
```

---

### Task 3.2: Expression evaluator — literals and PRINT

**Files:**
- Create: `novalogo/eval.s`
- Modify: `novalogo/novalogo.s` (wire REPL → tokenize → eval)

**Step 1: Write failing test**

```csharp
[TestMethod]
public void PrintNumber()
{
    // Type "PRINT 42" + Enter
    // Assert screen contains "42"
}
```

**Step 2: Implement evaluator core**

`eval_line` routine:
- Takes head of token list
- For each token:
  - TOK_WORD → lookup in builtin table → if found, evaluate N argument expressions, call handler
  - TOK_NUMBER → return the number value
  - TOK_QUOTE → return the quoted word
  - TOK_VARREF → look up variable, return value

`do_print` handler:
- Evaluate one argument expression
- If number: convert 16.8 to decimal ASCII, print via VGC_CHAROUT
- If word: print characters
- Print newline

Wire the REPL: after `read_line`, call `tokenize_line`, then `eval_line`, then reprint `?` prompt.

**Step 3: Run test**

Run: `dotnet test --filter "PrintNumber"`
Expected: PASS

**Step 4: Commit**

```
feat(novalogo): expression evaluator with PRINT
```

---

### Task 3.3: Arithmetic via math coprocessor

**Files:**
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void PrintArithmetic()
{
    // Type "PRINT 3 + 4" → "7"
    // Type "PRINT 10 * 5" → "50"
    // Type "PRINT 100 / 3" → "33"
}
```

**Step 2: Implement infix evaluation**

When the evaluator encounters a TOK_INFIX during argument evaluation:
- The left operand was already evaluated
- Evaluate the right operand
- Dispatch to coprocessor:
  - `+` / `-`: 16.8 add/subtract (software, trivial)
  - `*`: write operands to MATH_MUL16_A/B ($BB20-$BB23), read MATH_RES0-RES1
  - `/`: write to MATH_DIV_N/D ($BB28-$BB2D), read MATH_RES0-RES1

Use the `math_copro.inc` macros (MATHC_MUL16, MATHC_DIV_S32_16).

**Step 3: Run tests, commit**

```
feat(novalogo): arithmetic operators via math coprocessor
```

---

### Task 3.4: Variables — MAKE and :reference

**Files:**
- Create: `novalogo/vars.s`
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void MakeAndPrintVariable()
{
    // Type: MAKE "X 42
    // Type: PRINT :X
    // Assert screen contains "42"
}
```

**Step 2: Implement variable storage**

Variable table: a linked list of (name-ptr, value-ptr) pairs on the heap.
- `var_set(name, value)`: scan list for name match → update value, or append new entry
- `var_get(name)`: scan list for name match → return value, or error "X has no value"

`do_make` handler: evaluate two args (quoted word for name, expression for value), call `var_set`.

TOK_VARREF evaluation: call `var_get` with the variable name.

**Step 3: Run tests, commit**

```
feat(novalogo): MAKE and variable references
```

---

### Task 3.5: REPEAT

**Files:**
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void RepeatPrintsMultipleTimes()
{
    // Type: REPEAT 3 [PRINT "HI]
    // Assert screen contains three lines of "HI"
}
```

**Step 2: Implement REPEAT**

`do_repeat` handler:
- Evaluate first arg (count, integer)
- Second arg must be a list (TOK_LBRACKET ... TOK_RBRACKET)
- Capture the token list between brackets
- Loop: tokenize and evaluate the list body `count` times
- Track REPCOUNT in a zero-page variable (1-based)

**Step 3: Run tests, commit**

```
feat(novalogo): REPEAT with list body execution
```

---

### Task 3.6: IF / IFELSE and comparison operators

**Files:**
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void IfElseCondition()
{
    // Type: MAKE "X 10
    // Type: IFELSE :X > 5 [PRINT "BIG] [PRINT "SMALL]
    // Assert screen contains "BIG"
}
```

**Step 2: Implement**

- Comparison infix operators (`>`, `<`, `=`) return TRUE/FALSE words
- `do_if`: evaluate condition, if TRUE evaluate list body
- `do_ifelse`: evaluate condition, if TRUE evaluate first list, else second list

TRUE and FALSE are special words (Logo convention: `"TRUE` and `"FALSE`).

**Step 3: Run tests, commit**

```
feat(novalogo): IF, IFELSE, and comparison operators
```

---

## Phase 4: Procedures & Recursion

### Task 4.1: TO ... END definition at REPL

**Files:**
- Create: `novalogo/procedures.s`
- Modify: `novalogo/novalogo.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void DefineAndCallProcedure()
{
    // Type: TO SQUARE :SIZE
    // (should switch to multi-line input mode)
    // Type: PRINT :SIZE
    // Type: END
    // Type: SQUARE 42
    // Assert screen contains "42"
}
```

**Step 2: Implement procedure definition**

When the REPL sees `TO` as the first word:
- Parse procedure name and parameter names (`:param`)
- Switch to multi-line input mode (prompt changes to `>`)
- Accumulate lines until `END` is entered
- Store the procedure: name, parameter list, body (as token list or raw text) on the heap
- Add to procedure directory (linked list of defined procedures)

For now, skip EDITUI integration — just use multi-line REPL input. EDITUI editor comes in Phase 8.

**Step 3: Implement procedure call**

When `eval_line` encounters a word that isn't a builtin:
- Search procedure directory
- If found: bind arguments to parameter names in a new scope frame
- Push scope frame onto eval stack (heap-allocated)
- Evaluate procedure body
- Pop scope frame

**Step 4: Run tests, commit**

```
feat(novalogo): procedure definition (TO/END) and invocation
```

---

### Task 4.2: LOCAL, STOP, OUTPUT

**Files:**
- Modify: `novalogo/procedures.s`
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void OutputReturnValue()
{
    // Type: TO DOUBLE :N
    // Type: OUTPUT :N * 2
    // Type: END
    // Type: PRINT DOUBLE 5
    // Assert screen contains "10"
}
```

**Step 2: Implement**

- `LOCAL "name`: create variable in current scope frame (not global)
- `STOP`: set a "stopped" flag, unwind eval to caller
- `OUTPUT expr`: evaluate expression, store return value, set stopped flag

The evaluator checks the stopped flag after each statement. If set, unwind.

**Step 3: Run tests, commit**

```
feat(novalogo): LOCAL, STOP, OUTPUT for procedures
```

---

### Task 4.3: Recursion

**Files:**
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void RecursiveProcedure()
{
    // Type: TO COUNTDOWN :N
    // Type: IF :N = 0 [STOP]
    // Type: PRINT :N
    // Type: COUNTDOWN :N - 1
    // Type: END
    // Type: COUNTDOWN 5
    // Assert screen contains "5", "4", "3", "2", "1" on separate lines
}
```

**Step 2: Verify recursion works**

If the eval stack is heap-based (not 6502 hardware stack), recursion should already work from the procedure call implementation. The key constraint is that each call pushes a new scope frame on the heap, and the 6502 JSR/RTS stack is only used for the interpreter's own call depth (eval calling eval), which must be bounded.

For deep recursion, the evaluator needs to be iterative (loop-based) rather than recursive in its own 6502 code. If the current implementation is recursive in 6502 terms, refactor to use an explicit eval stack.

**Step 3: Run tests, commit**

```
feat(novalogo): verified recursive procedure calls
```

---

## Phase 5: List Operations

### Task 5.1: List literals and FIRST / BUTFIRST / COUNT

**Files:**
- Create: `novalogo/lists.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void ListOperations()
{
    // PRINT FIRST [A B C] → "A"
    // PRINT COUNT [A B C] → "3"
    // PRINT FIRST BUTFIRST [A B C] → "B"
}
```

**Step 2: Implement list representation**

Lists are cons pairs on the heap:
```
Offset 0: tag ($10 = list pair)
Offset 1: GC mark
Offset 2-3: car pointer (first element)
Offset 4-5: cdr pointer (rest of list, or $0000 for empty)
```

When the evaluator encounters `[...]`, build a list from the tokens between brackets. Each element is either a word, number, or nested list.

Implement:
- `FIRST`: return car of list
- `BUTFIRST` / `BF`: return cdr of list
- `LAST`: walk to end, return last car
- `BUTLAST` / `BL`: build new list without last element
- `COUNT`: walk list, count elements
- `EMPTY?`: test if list is nil

**Step 3: Run tests, commit**

```
feat(novalogo): list literals and FIRST/BUTFIRST/LAST/COUNT/EMPTY?
```

---

### Task 5.2: FPUT, LPUT, LIST, SENTENCE, WORD

**Files:**
- Modify: `novalogo/lists.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void ListConstruction()
{
    // PRINT FPUT "X [A B] → "[X A B]"
    // PRINT LIST "A "B → "[A B]"
    // PRINT WORD "HEL "LO → "HELLO"
    // PRINT SENTENCE [A] [B C] → "[A B C]"
}
```

**Step 2: Implement**

- `FPUT`: allocate cons pair, set car=arg1, cdr=arg2
- `LPUT`: walk to end of list, append new cons pair
- `LIST`: create list from arguments
- `SENTENCE`: flatten and concatenate lists
- `WORD`: concatenate two words into one

**Step 3: Run tests, commit**

```
feat(novalogo): FPUT, LPUT, LIST, SENTENCE, WORD
```

---

### Task 5.3: RUN — evaluate list as code

**Files:**
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void RunEvaluatesListAsCode()
{
    // MAKE "CMD [PRINT 42]
    // RUN :CMD
    // Assert screen contains "42"
}
```

**Step 2: Implement**

`do_run`: take a list, convert its elements back to a token stream, pass to `eval_line`. This is the core Lisp power of Logo — code is data.

**Step 3: Run tests, commit**

```
feat(novalogo): RUN evaluates lists as code
```

---

## Phase 6: Garbage Collector

### Task 6.1: Mark-and-sweep GC

**Files:**
- Create: `novalogo/gc.s`
- Modify: `novalogo/heap.s`

**Step 1: Tagged heap nodes**

Every heap allocation already has a tag byte (token type or list pair type). Add a mark bit (bit 7 of tag byte, or a separate mark byte).

**Step 2: Implement mark phase**

`gc_mark`: walk the root set:
- Global variable table
- Current eval stack (all scope frames)
- Current token list being evaluated
For each reachable node, set the mark bit. Follow all pointers recursively (using an explicit mark stack to avoid 6502 stack overflow).

**Step 3: Implement sweep phase**

`gc_sweep`: linear scan of the heap from $0400 to heap top. Unmarked nodes are added to a free list. Marked nodes have their mark bit cleared.

**Step 4: Integrate with allocator**

`heap_alloc`: first try bump allocation. If heap is full, trigger `gc_collect` (mark + sweep), then retry from free list. If still no space, print `Out of memory` error.

**Step 5: Write test**

```csharp
[TestMethod]
public void GarbageCollectionFreesMemory()
{
    // Run a loop that allocates many temporary lists
    // REPEAT 1000 [MAKE "X LIST 1 2 3]
    // Should not run out of memory (GC reclaims old :X values)
    // PRINT "DONE
    // Assert screen contains "DONE" (didn't crash with OOM)
}
```

**Step 6: Run tests, commit**

```
feat(novalogo): mark-and-sweep garbage collector
```

---

## Phase 7: Turtle Graphics

### Task 7.1: Turtle state and VGC setup

**Files:**
- Create: `novalogo/turtle.s`

**Step 1: Define turtle state in zero page**

```asm
turtle_x_frac:  .res 1   ; 16.8 position — fraction
turtle_x_lo:    .res 1   ; 16.8 position — integer low
turtle_x_hi:    .res 1   ; 16.8 position — integer high
turtle_y_frac:  .res 1
turtle_y_lo:    .res 1
turtle_y_hi:    .res 1
turtle_heading: .res 2   ; degrees 0-359 (16-bit for precision)
turtle_pen:     .res 1   ; $00 = down (drawing), $01 = up
turtle_shown:   .res 1   ; $00 = hidden, $01 = shown
turtle_color:   .res 1   ; current pen color
turtle_sprite:  .res 1   ; sprite slot used by turtle (default: 15)
```

**Step 2: Implement turtle init**

`turtle_init`:
- Set position to center of screen (160, 100) as 16.8
- Heading = 0 (north / up)
- Pen down, shown, color = white
- Define turtle shape in sprite slot (arrow/triangle pointing up)
- Enable sprite, position it

Use the vsprite library from `runtime/asm/vsprite.s` for rotation.

**Step 3: Commit**

```
feat(novalogo): turtle state and initialization
```

---

### Task 7.2: FD, BK, RT, LT — core movement

**Files:**
- Modify: `novalogo/turtle.s`
- Modify: `novalogo/builtins.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void ForwardDrawsLine()
{
    // Boot Logo
    // Type: CS
    // Type: FD 50
    // Read graphics buffer — verify a vertical line was drawn
}
```

**Step 2: Implement FD (forward)**

`do_fd`:
1. Evaluate distance argument (16.8 value)
2. Convert heading (degrees) to u8 angle: `angle_u8 = heading * 256 / 360`
3. Call SINCOS via coprocessor: `STA MATH_SINCOS_ANGLE` → read sin (RES0) and cos (RES1)
4. Compute delta: `dx = sin * distance`, `dy = -cos * distance` (Logo: 0° = north/up)
   - Use MATHC_MULFX for the multiply
5. Save old position (integer part) for line drawing
6. Add delta to turtle position (16.8 arithmetic, use FIX16_8 macros)
7. If pen is down: VGC LINE from old position to new position
   - Set VGC_P0-P8 with coordinates and color, write VCMD_LINE to VGC_CMD
8. Update vsprite position on screen

**Step 3: Implement BK (backward)**

`do_bk`: negate distance, call `do_fd` logic.

**Step 4: Implement RT, LT (turn right/left)**

`do_rt`: add angle to heading (mod 360)
`do_lt`: subtract angle from heading (mod 360)

Update vsprite rotation angle after turn.

**Step 5: Run tests, commit**

```
feat(novalogo): FD, BK, RT, LT with hardware turtle
```

---

### Task 7.3: Screen modes — CS, TS, SS, FS

**Files:**
- Modify: `novalogo/turtle.s`
- Modify: `novalogo/builtins.s`

**Step 1: Implement screen mode switching**

- `do_cs` / `do_draw`: clear graphics plane, show turtle, enter splitscreen mode
  - Set VGC_MODE to graphics+sprites (MODE_GFX_SPRITES = 3)
  - Use copper to split: top = graphics, bottom = text
  - Home turtle to center, heading 0
  - Show turtle sprite

- `do_textscreen`: disable graphics mode, full text (MODE_TEXT_ONLY = 0), disable copper
- `do_splitscreen`: copper split active, both graphics and text visible
- `do_fullscreen`: graphics only, text area hidden (copper sets full screen to gfx mode)

Use the copper library from `runtime/asm/copper.s`.

**Step 2: Write test**

```csharp
[TestMethod]
public void ClearScreenShowsTurtle()
{
    // Type: CS
    // Verify sprite 15 is enabled (read sprite registers via peek)
    // Verify VGC mode includes graphics
}
```

**Step 3: Run tests, commit**

```
feat(novalogo): screen modes CS/TS/SS/FS via copper
```

---

### Task 7.4: Remaining turtle commands

**Files:**
- Modify: `novalogo/turtle.s`
- Modify: `novalogo/builtins.s`

**Step 1: Implement**

- `HOME`: position to center, heading 0 (draw line if pen down)
- `PU` (penup): set turtle_pen = 1
- `PD` (pendown): set turtle_pen = 0
- `ST` (showturtle): enable sprite, set turtle_shown = 1
- `HT` (hideturtle): disable sprite, set turtle_shown = 0
- `SETPOS [x y]`: move to position (draw line if pen down)
- `SETXY x y`: move to position
- `SETX x` / `SETY y`: move one axis
- `SETHEADING angle`: set heading directly
- `XCOR` / `YCOR`: output current position (integer part)
- `HEADING`: output current heading
- `TOWARDS x y`: output angle towards point (use ATAN2 coprocessor)
- `PENDOWN?` / `SHOWN?`: output TRUE/FALSE

**Step 2: Write tests for key commands**

```csharp
[TestMethod]
public void SetPosAndQueryPosition()
{
    // CS
    // PU
    // SETXY 100 50
    // PRINT XCOR → "100"
    // PRINT YCOR → "50"
}
```

**Step 3: Run tests, commit**

```
feat(novalogo): full turtle command set
```

---

## Phase 8: EDITUI Integration

### Task 8.1: TO opens editor

**Files:**
- Modify: `novalogo/novalogo.s`
- Modify: `novalogo/procedures.s`

**Step 1: Implement**

When `TO procname :arg1 :arg2` is entered at the REPL:
1. Switch to EDITUI mode (call `editui_init`)
2. Set title to procedure name
3. If procedure already exists, load its body into the editor buffer
4. Accept multi-line text editing (basic line editor within the EDITUI box)
5. On save (e.g., Ctrl-S or menu File→Save): parse body, store procedure, exit editor
6. On cancel (Esc or menu File→Close): discard, exit editor
7. Restore REPL screen state (text mode, `?` prompt)

This requires linking the editui runtime (`editui.s`, which includes `vtext.s` and `blitter.s`).

**Step 2: Implement EDIT "name**

Same as TO but opens an existing procedure for editing.

**Step 3: Write test**

```csharp
[TestMethod]
public void EditProcedureAndCall()
{
    // Test TO → editor → define procedure → call it
    // This may need to simulate specific key sequences
}
```

**Step 4: Commit**

```
feat(novalogo): EDITUI integration for procedure editing
```

---

## Phase 9: Error Handling

### Task 9.1: Error messages

**Files:**
- Create: `novalogo/errors.s`

**Step 1: Implement error reporting**

- `error_unknown_proc`: "I don't know how to %s"
- `error_bad_input`: "%s doesn't like %s as input"
- `error_no_value`: "%s has no value"
- `error_not_enough_args`: "Not enough inputs to %s"
- `error_too_many_args`: "You don't say what to do with %s"

Each error:
1. Prints the message with the relevant word/value substituted
2. Sets an error flag
3. Returns control to the REPL `?` prompt

**Step 2: Commit**

```
feat(novalogo): error messages and error recovery
```

---

### Task 9.2: CATCH / THROW

**Files:**
- Modify: `novalogo/errors.s`
- Modify: `novalogo/eval.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void CatchPreventsCrash()
{
    // CATCH "error [MAKE "X THING "UNDEFINED]
    // PRINT "SURVIVED
    // Assert screen contains "SURVIVED"
}
```

**Step 2: Implement**

- `do_catch`: push a catch frame (tag name + resume point) onto the eval stack, evaluate body list. If error occurs and tag matches, resume at catch frame. If no error, pop frame.
- `do_throw`: scan eval stack for matching catch frame, unwind to it.

**Step 3: Run tests, commit**

```
feat(novalogo): CATCH/THROW error handling
```

---

## Phase 10: NDK Bridge

### Task 10.1: CALL primitive

**Files:**
- Create: `novalogo/ndk.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void CallNdkAddress()
{
    // Poke a small routine at $7000: LDA #42, STA NVR0L, RTS
    // Type: CALL $7000
    // Verify NVR0 contains 42 (via THING "NVR0 or peek)
}
```

**Step 2: Implement**

`do_call`: evaluate address argument, JSR to it. Before the call, marshal any NVR0-NVR7 Logo variables into the zero-page NVR registers. After return, copy NVR registers back to Logo variables.

**Step 3: Commit**

```
feat(novalogo): CALL primitive for raw NDK access
```

---

### Task 10.2: NATIVE registration

**Files:**
- Modify: `novalogo/ndk.s`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void NativeRegistration()
{
    // Poke a routine at $7000 that reads NVR0, adds 1, stores to NVR0, RTS
    // Type: NATIVE "INC1 $7000 1
    // Type: PRINT INC1 5
    // Assert screen contains "6"
}
```

**Step 2: Implement**

`do_native`: parse name (quoted word), address (number), arity (number). Add to a user-native table (heap-allocated, searched before builtins). When called, marshal arguments into NVR0-NVRn, JSR, read NVR0 as return value.

**Step 3: Commit**

```
feat(novalogo): NATIVE word registration for user NDK routines
```

---

### Task 10.3: Curated hardware bindings

**Files:**
- Modify: `novalogo/builtins.s`
- Create: `novalogo/hw_bindings.s`

**Step 1: Implement VGC graphics words**

Each word wraps a VGC command:
- `LINE x1 y1 x2 y2`: set VGC_P0-P8, write VCMD_LINE
- `CIRCLE x y r`: set params, write VCMD_CIRCLE
- `RECT x1 y1 x2 y2`: set params, write VCMD_RECT
- `FILL x1 y1 x2 y2`: set params, write VCMD_FILL
- `PAINT x y`: set params, write VCMD_PAINT
- `PLOT x y`: set params, write VCMD_PLOT
- `SETCOLOR c`: set VGC_FGCOL

**Step 2: Implement sprite words**

- `SPRITE n shapename x y`: define sprite shape, position, enable
- `SPRITEPOS n x y`: move sprite
- `SPRITEON n` / `SPRITEOFF n`: enable/disable
- `SPRITECOLLISION? n`: read collision register

**Step 3: Implement sound words**

- `TONE freq dur`: set SID voice 0 frequency and gate
- `NOISE dur`: SID voice with noise waveform
- `PLAY mml_string`: forward to music engine

**Step 4: Implement remaining hardware words**

- `COPPER [list]`: set copper instructions
- `BLIT / BLITCOPY / BLITFILL`: blitter commands
- `WAIT frames`: wait N vblanks
- `WAITVBL`: wait for next vblank
- `JOY`: read joystick state
- `TIMER`: read timer value

**Step 5: Write tests for key bindings**

```csharp
[TestMethod]
public void PlotPixel()
{
    // CS
    // PLOT 100 50
    // Read graphics and verify pixel is set
}
```

**Step 6: Commit**

```
feat(novalogo): curated hardware bindings — VGC, sprites, SID, blitter
```

---

## Phase 11: File I/O & Standard Library

### Task 11.1: SAVE / LOAD workspaces

**Files:**
- Create: `novalogo/fileio.s`

**Step 1: Implement SAVE**

`do_save`: serialize all defined procedures to a text format (Logo source). Write via FileIoController ($B9A0) to SD card.

Format on disk: plain Logo source that can be re-executed:
```
TO SQUARE :SIZE
REPEAT 4 [FD :SIZE RT 90]
END

TO STAR :SIZE
REPEAT 5 [FD :SIZE RT 144]
END
```

**Step 2: Implement LOAD**

`do_load`: read file from SD, feed each line through the tokenizer/evaluator. `TO ... END` blocks recreate procedures.

**Step 3: Write test, commit**

```
feat(novalogo): SAVE/LOAD workspace to SD
```

---

### Task 11.2: Standard library boot

**Files:**
- Modify: `novalogo/novalogo.s`

**Step 1: Implement startup library load**

At boot, after banner, attempt to load `/lib/startup.logo` from SD. If found, evaluate it silently (no output). This defines the standard library procedures (SQUARE, TRIANGLE, STAR, SPIRAL, TREE, etc.).

If file not found, skip silently — Logo works without it, just fewer pre-defined procedures.

**Step 2: Create startup.logo**

Write the standard library file with the procedures from the design doc.

**Step 3: Commit**

```
feat(novalogo): standard library boot from /lib/startup.logo
```

---

### Task 11.3: HELP system

**Files:**
- Modify: `novalogo/builtins.s`

**Step 1: Implement**

- `HELP` (no args): print categorized word list (Turtle, Graphics, Sound, etc.)
- `HELP "FD`: print usage for a specific word

Help text for builtins stored in ROM as compressed strings. Help for standard library procedures loaded from `/lib/help/` on SD if available.

**Step 2: Commit**

```
feat(novalogo): HELP system with categorized word list
```

---

### Task 11.4: Remaining workspace commands

**Files:**
- Modify: `novalogo/procedures.s`

**Step 1: Implement**

- `PO "name` (printout): print procedure definition at the prompt
- `POTS` (print out titles): list all defined procedure names
- `ERASE "name`: remove procedure from directory

**Step 2: Commit**

```
feat(novalogo): PO, POTS, ERASE workspace commands
```

---

## Phase 12: Polish & Integration

### Task 12.1: Number formatting

Ensure `PRINT` outputs numbers cleanly:
- Integers print without decimal point: `42` not `42.0`
- Fractional numbers print with minimal digits: `3.5` not `3.500`
- Negative numbers: `-7`

### Task 12.2: Additional control structures

- `FOR [i 1 10] [PRINT :i]`
- `WHILE [condition] [body]`
- `UNTIL [condition] [body]`
- `REPCOUNT` inside REPEAT

### Task 12.3: Remaining I/O

- `TYPE` (print without newline)
- `SHOW` (print with brackets for lists)
- `READWORD` / `READLIST` / `READCHAR`

### Task 12.4: Math functions

- `SIN angle` / `COS angle`: degrees → value (use SINCOS coprocessor, scale from 1.7 to user-friendly range)
- `ARCTAN dy dx`: use ATAN2 coprocessor, convert u8 angle back to degrees
- `SQRT n`: use DIST_APPROX or ATAN2 hypot
- `RANDOM n`: use RNG coprocessor, mod by n
- `ABS` / `INT` / `ROUND` / `REMAINDER`

### Task 12.5: ITEM, MEMBER?, WORD?, LIST?, NUMBER? predicates

### Task 12.6: APPLY

`APPLY "procname [arg1 arg2 ...]`: call procedure with args from a list.

### Task 12.7: Final integration test suite

Write a comprehensive test that exercises the full Logo experience:
```logo
TO FLOWER :SIZE :PETALS
  REPEAT :PETALS [
    REPEAT 2 [FD :SIZE RT 360 / :PETALS]
    RT 360 / :PETALS
  ]
END
CS
FLOWER 80 8
```

Verify the graphics output shows a flower pattern.

**Commit:**
```
feat(novalogo): polish — number formatting, control structures, predicates
```

---

## Build Dependencies

The `novalogo/Makefile` needs to produce `novalogo.bin` and copy it to `e6502.Avalonia/Resources/`. The top-level build should include this step. Add a `novalogo` target to the assembly Makefile or create a pre-build step in the Avalonia .csproj.

The novalogo source files should `.include` each other from the main `novalogo.s` file (same pattern as `min_mon.asm` including `basic.asm`). The linked output is a single 16KB binary.

## ROM Space Budget

EhBASIC uses nearly all 16KB. NovaLogo must be more space-efficient:
- Core interpreter (tokenizer, parser, evaluator, GC): ~6KB
- Turtle math + screen modes: ~1KB
- Builtin table + handlers (PRINT, MAKE, REPEAT, IF, list ops): ~3KB
- NDK bridge + hardware bindings: ~2KB
- I/O, errors, strings, HELP index: ~2KB
- Monitor code + vectors: ~0.5KB
- EDITUI glue: ~1KB (EDITUI itself is linked as a separate object)

Total estimate: ~15.5KB. Tight but feasible. If needed, move help text and some hardware binding handlers to an extension ROM (same pattern as EhBASIC extension.s).
