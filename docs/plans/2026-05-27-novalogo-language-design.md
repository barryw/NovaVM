# NovaLogo Language Design

**Date:** 2026-05-27
**Status:** Design complete, implementation not started

## Overview

NovaLogo is a full Logo interpreter for the Nova platform. It boots from a
dedicated 16KB ROM slot (same architecture as EhBASIC) into an interactive
REPL with access to the complete Nova hardware surface. The language is
classic UCBLogo-style Logo: first-class lists, procedures with `TO ... END`,
dynamic scoping, full recursion, and `CATCH`/`THROW` error handling.

The turtle is a hardware vsprite with real-time rotation. The math
coprocessor provides hardware-accelerated fixed-point arithmetic, trig, and
vector operations.

## Audience

Easy on-ramp for kids (draw a square in 30 seconds), deep ceiling for power
users (sound, sprites, copper, blitter, NDK interop). The full Nova hardware
surface is exposed as Logo-native words from day one.

## Interaction Model

Boot to a text screen:

```
NOVALOGO v1.0
39168 BYTES FREE
?
```

The `?` prompt is the REPL. Type commands, they execute immediately:

- `FD 50` draws a line
- `PRINT "HELLO` prints text
- `CS` clears the screen, shows the turtle in splitscreen mode

When you type `TO FART :SIZE`, the EDITUI editor opens with the procedure
name as the title. Write the body, save and close, return to the `?` prompt.
Now `FART 100` works.

- `EDIT "FART` — reopen a procedure in the editor
- `PO "FART` — print the definition at the prompt
- `POTS` — print all procedure titles
- `ERASE "FART` — delete a procedure

### Screen Modes

- `CS` / `DRAW` — clear graphics, show turtle, enter splitscreen
- `TS` / `TEXTSCREEN` — full text, REPL only
- `SS` / `SPLITSCREEN` — copper split, graphics top, REPL bottom
- `FS` / `FULLSCREEN` — graphics only, commands still accepted blind

The copper splits the display. In splitscreen, the bottom text region scrolls
independently from the graphics viewport above.

## Language

### Data Types

Three types: **words** (strings/numbers), **lists** (ordered sequences of
words and lists), and **procedures** (named, with parameters).

### Numeric Representation

16.8 fixed-point internally. The turtle tracks sub-pixel position for
accurate recursive geometry — `FD 50 RT 120 FD 50 RT 120 FD 50 RT 120`
closes exactly back to the start point with no drift.

Angles at the Logo surface are degrees (0-360). Internally mapped to the
math coprocessor's 0-255 range and back — the user never sees the native
format.

The math coprocessor provides:

| Operation | Logo surface | Coprocessor op |
|-----------|-------------|----------------|
| `*` | integer/fixed multiply | MUL16 / MUL_FX |
| `/` | integer/fixed divide | DIV_S32_16 |
| `SIN` / `COS` | degrees → value | SINCOS (u8 angle) |
| `ARCTAN` | dy dx → degrees | ATAN2 |
| `SQRT` | approximate distance | DIST_APPROX |
| `RANDOM` | random integer | RNG |

### Variables

- `MAKE "x 5` — create/update variable (global unless `LOCAL` declared)
- `:x` — read variable value
- `LOCAL "x` — declare variable in current procedure scope
- `THING "x` — alternative read syntax

Dynamic scoping (Logo tradition). Scope frames live on the heap evaluation
stack, not the 6502 hardware stack.

### Control Structures

```
IF :x > 5 [FD 100]
IFELSE :x > 5 [FD 100] [BK 100]
REPEAT 4 [FD 100 RT 90]
REPCOUNT                          ; current iteration (1-based)
FOR [i 1 10] [PRINT :i]
WHILE [:x > 0] [MAKE "x :x - 1]
UNTIL [:x > 10] [MAKE "x :x + 1]
```

### Procedures

```
TO SQUARE :SIZE
  REPEAT 4 [FD :SIZE RT 90]
END
```

`STOP` exits a procedure. `OUTPUT :value` exits and returns a value.

### List Operations

```
FIRST [A B C]             ; A
BUTFIRST [A B C]          ; [B C]
LAST [A B C]              ; C
BUTLAST [A B C]           ; [A B]
FPUT "X [A B C]           ; [X A B C]
LPUT "X [A B C]           ; [A B C X]
LIST "A "B                ; [A B]
SENTENCE [A] [B C]        ; [A B C]
WORD "HEL "LO             ; HELLO
ITEM 2 [A B C]            ; B
COUNT [A B C]             ; 3
EMPTY? []                 ; TRUE
MEMBER? "B [A B C]        ; TRUE
RUN [FD 50 RT 90]         ; evaluate list as code
APPLY "SQUARE [100]       ; call procedure with args from list
```

`RUN` is the Lisp power — any list can be executed as code.

### Error Handling

Default: stop execution, print a clear message, return to `?` prompt.

```
I don't know how to SPRIAL
FART doesn't like [hello] as input
```

Advanced:

```
CATCH "error [THING "undefined.var]
THROW "error
```

### I/O

```
PRINT "HELLO              ; prints HELLO + newline
TYPE "HELLO                ; prints HELLO, no newline
SHOW [1 2 3]              ; prints [1 2 3] with brackets
READWORD                  ; reads one word from keyboard
READLIST                  ; reads a line, returns as list
READCHAR                  ; reads single keypress
```

## Core Primitives

### Turtle

`FD BK RT LT HOME CS ST HT PU PD SETPOS SETXY SETX SETY SETHEADING
TOWARDS XCOR YCOR HEADING PENDOWN? SHOWN?`

### Arithmetic

`+ - * / REMAINDER SQRT SIN COS ARCTAN RANDOM INT ROUND ABS`

### Predicates

`WORD? LIST? NUMBER? EMPTY? MEMBER? PENDOWN? SHOWN?`

### Workspace

`EDIT PO POTS ERASE SAVE LOAD HELP`

## Nova Hardware Extensions

Logo-native words wrapping NDK routines. Verb-first naming convention.

### Graphics (VGC)

`LINE CIRCLE RECT FILL PAINT PLOT UNPLOT SETCOLOR SETMODE`

### Sprites

`SPRITE SETSHAPE SPRITEPOS SPRITEON SPRITEOFF SPRITEFLIP
SPRITEPRIORITY SPRITECOLLISION?`

### Sound (SID)

`TONE NOISE ENVELOPE VOLUME FILTER WAVE`

### Music Engine

`PLAY TEMPO INSTRUMENT VOICE SFX`

### Copper

`COPPER COPPERON COPPEROFF`

### Blitter

`BLIT BLITCOPY BLITFILL`

### File I/O

`SAVEFILE LOADFILE DIR DELETE`

### XRAM

`XSTORE XFETCH XPAGE`

### Timer

`TIMER WAIT`

### Misc

`JOY WAITVBL`

### Example Usage

```
TO EXPLOSION :X :Y
  SETSHAPE 0 "bang
  SPRITEPOS 0 :X :Y
  SPRITEON 0
  SFX 3
  WAIT 30
  SPRITEOFF 0
END

TO SONG
  TEMPO 120
  INSTRUMENT 0 "piano
  PLAY "CDEFGAB>C
END

TO STARFIELD
  REPEAT 100 [PLOT RANDOM 320 RANDOM 200]
END
```

## NDK Bridge

Three layers for calling 6502 assembly from Logo:

### Curated Bindings (ROM)

NDK routines registered at boot as Logo primitives with defined arity. The
binding table maps Logo names to addresses and argument counts. `TONE 440 30`
just works — Logo marshals args into the right zero-page slots and JSRs.

### CALL (Escape Hatch)

`CALL $7200` — JSR to a raw address. Values passed/returned via NVR0-NVR7
pseudo-registers:

```
MAKE "NVR0 100
CALL $7200
PRINT THING "NVR0
```

### NATIVE (User Registration)

```
NATIVE "vline $7200 5
```

Registers address `$7200` as Logo word `VLINE` with arity 5. After
registration, `VLINE 10 20 50 80 5` works like a built-in.

## Memory Layout

```
$0000-$002F  Zero page: CPU regs, NVR0-NVR7 pseudo-regs
$0030-$00FF  Zero page: Logo interpreter state (turtle pos/heading,
             parse pointers, eval stack pointer, GC state)
$0100-$01FF  6502 hardware stack
$0200-$027F  Nova vector table (device base addresses)
$0280-$03FF  Logo workspace metadata (procedure directory, var table root)
$0400-$9FFF  ~39KB managed heap (lists, words, procedures, scope frames)
$A000-$BFFF  Hardware I/O registers
$C000-$FFFF  Logo ROM (interpreter, tokenizer, built-in primitives,
             turtle math, EDITUI glue, NDK binding table)
```

### Heap

Every heap allocation is a tagged node: word, list pair, scope frame, or
procedure body. Garbage collection is mark-and-sweep, triggered when
allocation fails. Walks the root set (global vars, eval stack, current
expression), marks reachable nodes, sweeps the rest.

Evaluation stack and scope frames live in the heap, not the 256-byte 6502
hardware stack. Recursion depth is limited by heap size, not stack page.

### XRAM (512KB)

- Bank-switched overflow for large workspaces
- Standard library procedure cache (loaded from SD on demand)
- Undo/history buffer for the EDITUI editor

## Interpreter Architecture

```
Input → Tokenizer → Parser → Evaluator → Output
```

**Tokenizer:** Breaks input into words, numbers (parsed to 16.8
fixed-point), list brackets, and special characters. Tokens are small
heap-allocated nodes.

**Parser:** Logo syntax is simple — no operator precedence beyond prefix
application. `FD 50 + 30` means `FD (50 + 30)` because `+` is an infix
operator with known arity. Produces a linked list of expression nodes.

**Evaluator:** Tree-walking. For each node:

- Literal → push value
- Variable reference → look up in scope chain
- Procedure call → evaluate arguments, push scope frame, evaluate body
- Infix op → evaluate both sides, dispatch to coprocessor
- `RUN` → tokenize + parse + evaluate a list as code

**GC:** Mark-and-sweep. Brief pause, acceptable for an interactive language.

## Boot Sequence

1. ROM init — zero page setup, vector table, heap init
2. Banner: `NOVALOGO v1.0` + available memory
3. Load `/lib/startup.logo` from SD (standard library procedures)
4. Register NDK bindings from ROM table
5. Print `?` prompt, enter REPL

## Standard Library

Ships on SD at `/lib/`. Classic patterns as editable Logo source:

```
TO SQUARE :SIZE
  REPEAT 4 [FD :SIZE RT 90]
END

TO TRIANGLE :SIZE
  REPEAT 3 [FD :SIZE RT 120]
END

TO STAR :SIZE
  REPEAT 5 [FD :SIZE RT 144]
END

TO POLY :SIZE :ANGLE
  FD :SIZE RT :ANGLE
  POLY :SIZE :ANGLE
END

TO SPIRAL :SIZE :ANGLE :GROWTH
  IF :SIZE > 200 [STOP]
  FD :SIZE RT :ANGLE
  SPIRAL :SIZE + :GROWTH :ANGLE :GROWTH
END

TO TREE :SIZE
  IF :SIZE < 5 [STOP]
  FD :SIZE
  LT 30 TREE :SIZE * 0.7
  RT 60 TREE :SIZE * 0.7
  LT 30 BK :SIZE
END

TO CIRCLE.RIGHT :R
  REPEAT 360 [FD :R * 3.14159 / 180 RT 1]
END
```

These are real procedures — kids can `EDIT "SPIRAL` to see how they work,
modify them, learn from them.

Help text stored on SD at `/lib/help/` to keep ROM lean. `HELP` prints
categorized word lists. `HELP "FD` prints usage for a specific word.

## Turtle Implementation

The turtle is a hardware vsprite (16x16) with real-time rotation via the
VGC sprite engine. It occupies one sprite slot. Position tracked as 16.8
fixed-point for sub-pixel accuracy.

Turtle math pipeline for `FD :distance`:

1. Read current heading (degrees) → convert to u8 angle (0-255)
2. SINCOS via math coprocessor → sin/cos as 1.7 signed
3. Scale by distance → 16.8 delta
4. Add delta to 16.8 position
5. If pen is down, VGC LINE from old position to new position
6. Update vsprite position and rotation angle
