---
title: "Language Fundamentals"
type: guide
category: Language
---

> Simplicity is the ultimate sophistication. -- *Leonardo da Vinci*

NovaBASIC inherits a rich language from EhBASIC 2.22p5 and extends it with
hardware-specific commands. This chapter covers the core language features
you will use in almost every program: variables, arrays, control flow,
operators, built-in functions, and direct memory access.

## Variables

NovaBASIC has two kinds of variable: **numeric** and **string**.

- **Numeric variables** hold floating-point numbers. Names begin with a
letter and may contain letters and digits (e.g.\ `A`,
`SCORE`, `X1`, `HITCOUNT`).
- **String variables** hold text. Names end with a dollar sign
(e.g.\ `N`, `NAME`, `MSG$`).

Variable names are **case-insensitive**: `SCORE` and
`score` refer to the same variable. Numeric variables default to
`0`; string variables default to the empty string. You do not need to
declare a variable before using it.

```basic
10 SCORE = 0
20 NAME$ = "PLAYER ONE"
30 LIVES = 3
40 PRINT NAME$; " HAS "; LIVES; " LIVES"
50 SCORE = SCORE + 100
60 PRINT "SCORE: "; SCORE
```

## Arrays

Use `DIM` to declare an array before using it. The default lower bound
is 0, so `DIM A(10)` creates 11 elements: `A(0)` through
`A(10)`.

```basic
10 DIM A(10)
20 FOR I=0 TO 10
30   A(I) = I * I
40 NEXT I
50 FOR I=0 TO 10
60   PRINT "A(";I;") = ";A(I)
70 NEXT I
```

Two-dimensional arrays work the same way:

```basic
10 DIM GRID(7,7)
20 FOR R=0 TO 7
30   FOR C=0 TO 7
40     GRID(R,C) = R*8 + C
50   NEXT C
60 NEXT R
```

::: note
String arrays are also supported: `DIM LABEL$(9)` creates ten
string slots.
:::

## Control Flow

### FOR / NEXT Loops

The workhorse of NovaBASIC iteration. The optional `STEP` clause sets
the increment; if omitted, it defaults to 1.

```basic
10 FOR I = 1 TO 10
20   PRINT I; " ";
30 NEXT I
40 PRINT
50 REM count down with STEP
60 FOR N = 10 TO 1 STEP -1
70   PRINT N; " ";
80 NEXT N
```

### IF / THEN / ELSE

```basic
10 INPUT "ENTER A NUMBER: "; N
20 IF N > 100 THEN PRINT "BIG" ELSE PRINT "SMALL"
30 IF N = 42 THEN PRINT "THE ANSWER"
```

`THEN` may be followed by a statement or a line number
(as a `GOTO` shorthand). `ELSE` is optional.

### DO / LOOP

`DO`/`LOOP` supports four variants for flexible looping:

```basic
10 X = 1
20 DO
30   PRINT X
40   X = X + 1
50 LOOP WHILE X <= 5

100 Y = 10
110 DO UNTIL Y = 0
120   PRINT Y
130   Y = Y - 1
140 LOOP
```

### GOSUB / RETURN

Use subroutines to avoid repeating code. `GOSUB` branches to a line
number and `RETURN` comes back:

```basic
10 GOSUB 1000
20 GOSUB 1000
30 END
1000 REM draw border subroutine
1010 PRINT "----------"
1020 RETURN
```

### GOTO

`GOTO` unconditionally jumps to a line number. Use it sparingly;
`GOSUB`/`RETURN` and structured loops are usually cleaner.

## Shorthand Commands

### DEC and INC

`INC` and `DEC` increment or decrement a variable by 1 without
a full assignment statement. They are faster and more readable than
`X = X + 1`.

```basic
10 SCORE = 0
20 INC SCORE : INC SCORE : INC SCORE
30 PRINT "SCORE: "; SCORE
40 DEC SCORE
50 PRINT "AFTER DEC: "; SCORE
```

### WAIT

`WAIT addr, mask [, xor]` busy-waits (polls in a tight loop) until a
memory location satisfies a bit condition:

`(PEEK(addr) XOR xor) AND mask <> 0`

The *xor* parameter defaults to 0 if omitted. `WAIT` is useful for
polling hardware status registers:

```basic
10 REM Wait for timer IRQ pending (bit 0 of $BA41)
20 WAIT $BA41, 1
```

```basic
10 REM Wait for a key press (non-zero at $A00F)
20 WAIT $A00F, $FF
```

::: warning
`WAIT` blocks the BASIC interpreter completely until the condition is met.
If the condition is never satisfied, the program hangs. Use `STOP` (Ctrl+C)
to break out.
:::

### RESET

`RESET` performs a full hardware reset: it stops both SID chips, halts the
music engine, resets the NIC, clears the VGC, and restarts the BASIC interpreter
from cold start. All variables and the current program are lost.

```basic
10 PRINT "RESETTING IN 3..."
20 FOR I=3 TO 1 STEP -1 : PRINT I : NEXT I
30 RESET
```

## Operators

| **Operator** | **Type** | **Description** |
| --- | --- | --- |
| + | Arithmetic | Addition |
| - | Arithmetic | Subtraction or unary negation |
| * | Arithmetic | Multiplication |
| / | Arithmetic | Division |
| ^ | Arithmetic | Exponentiation (`2^8` = 256) |
| = | Comparison | Equal to |
| < | Comparison | Less than |
| > | Comparison | Greater than |
| <= | Comparison | Less than or equal to |
| >= | Comparison | Greater than or equal to |
| <> | Comparison | Not equal to |
| AND | Logical | Logical (bitwise) AND |
| OR | Logical | Logical (bitwise) OR |
| NOT | Logical | Logical (bitwise) NOT |
| EOR | Logical | Exclusive OR |
| << | Bit | Left shift (`LSHIFT`) |
| >> | Bit | Right shift (`RSHIFT`) |
| BITSET | Bit | Set bits by mask at an address |
| BITCLR | Bit | Clear bits by mask at an address |
| BITTGL | Bit | Toggle bits by mask at an address |
| BITTST(a,m) | Bit | Test bits by mask at an address |

## Built-in Functions

### Numeric Functions

| **Function** | **Description** |
| --- | --- |
| INT(x) | Truncate to integer (towards zero) |
| ABS(x) | Absolute value |
| SGN(x) | Sign: -1, 0, or 1 |
| SQR(x) | Square root |
| RND(1) | Pseudo-random number in [0, 1) |
| LOG(x) | Natural logarithm |
| EXP(x) | e^x |
| SIN(x) | Sine (radians) |
| COS(x) | Cosine (radians) |
| TAN(x) | Tangent (radians) |
| ATN(x) | Arctangent (radians) |
| PI | The constant pi 3.14159 |
| TWOPI | The constant 2pi 6.28318 |
| MAX(a,b) | Larger of two values |
| MIN(a,b) | Smaller of two values |
| PEEK(addr) | Read a byte from memory address *addr* |
| DEEK(addr) | Read a 16-bit word from address *addr* |
| FRE(0) | Free BASIC program memory in bytes |
| POS(0) | Current cursor column position |

### String Functions

| **Function** | **Description** |
| --- | --- |
| LEN(s$) | Number of characters in the string |
| CHR$(n) | Character whose ASCII code is *n* |
| ASC(s$) | ASCII code of the first character |
| STR$(n) | Convert number to string |
| VAL(s$) | Convert string to number |
| LEFT(s,n) | Left *n* characters |
| RIGHT(s,n) | Right *n* characters |
| MID(s,p,n) | *n* characters starting at position *p* |
| UCASE(s) | Convert to uppercase |
| LCASE(s) | Convert to lowercase |
| HEX$(n) | Hexadecimal string representation of *n* |
| BIN$(n) | Binary string representation of *n* |

## Memory Access

NovaBASIC gives you direct read and write access to the 6502 address space.
This is useful for reading hardware registers, patching values, and interfacing
with assembly routines.

- **`PEEK(addr)`** Read one byte (0--255) from address *addr*.
- **`POKE addr, val`** Write one byte to address *addr*.
- **`DEEK(addr)`** Read a 16-bit little-endian word from
*addr* and *addr*+1.
- **`DOKE addr, val`** Write a 16-bit little-endian word.
- **`VARPTR(v)`** Return the memory address of variable *v*.
Useful when passing variable addresses to assembly routines.

```basic
10 REM read and print two bytes at $0300
20 PRINT PEEK(768); PEEK(769)
30 REM write a counter value
40 POKE 768, 42
50 PRINT PEEK(768)
```

::: warning
Writing to the wrong address can crash the virtual machine or corrupt the
BASIC interpreter. Know what you are writing to before you `POKE`
into system areas. See the memory map in Appendix \refchap:memmap for safe zones.
:::

### Cursor Control

The text cursor position is set with `LOCATE x, y` where *x* is
the column (0--79) and *y* is the row (0--24).

Cursor **visibility** is controlled via the VGC register at $A00A:

```basic
10 POKE $A00A, 0  : REM hide cursor
20 LOCATE 10, 5   : PRINT "SCORE: 0"
30 POKE $A00A, 1  : REM show cursor
```

Any non-zero value enables the blinking cursor; zero hides it. This is
useful in games where you want a clean display without a flashing block.

## Style Tips

::: tip
- **One idea per line.** Statements can be chained with colons
(`:`), but a single clear statement per line is easier to read
and debug.
- **Group by function.** Put constants and configuration near
the top (lines 10--90), input routines in the 100s, game logic in the
200s--500s, display routines in the 600s, cleanup in the 900s.
- **Name things meaningfully.** `LIVES` is clearer than
`L`; `SCORE` is clearer than `S1`. NovaBASIC
accepts long names.
- **Define constants up front.**
`10 MAXLIVES=5 : STARTSPEED=2` at the top of the program means
you tune the game by changing one line, not hunting through hundreds.

:::

## Putting It Together

::: tryit
Enter and run this program. It builds an array of random values and prints
them in indexed form.
```basic
10 DIM A(5)
20 FOR I=1 TO 5
30   A(I) = INT(RND(1) * 100)
40 NEXT I
50 FOR I=1 TO 5
60   PRINT "A(";I;") = ";A(I)
70 NEXT I
```

**Expected:** Five lines printed, each showing an index and a random

integer between 0 and 99. The values differ every time you `RUN`.
:::

Try modifying the program: change `100` to `10` to get
single-digit values, or change `5` to `20` to see a longer
array. The structure stays the same; only the constants change.
