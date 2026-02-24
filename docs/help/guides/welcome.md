---
title: "Welcome to NovaBASIC"
type: guide
category: General
---

> Every great journey starts with a single command. -- *Unknown programmer, circa 1982*

## What NovaBASIC Is

NovaBASIC v1.0 is a modernized 6502 BASIC interpreter for the e6502 virtual
computer. It is derived from Lee Davison's *Enhanced BASIC 2.22p5*
(EhBASIC), one of the cleanest and most complete open-source 6502 BASIC
implementations ever written. NovaBASIC keeps the entire EhBASIC core -- every
numeric function, every string operation, the full `DO`/`LOOP` and
`FOR`/`NEXT` machinery -- and adds a hardware command layer that lets
you drive the e6502's graphics, sound, file system, and expansion memory
directly from BASIC.

The programming model is classic line-numbered BASIC. You type a line with a
number at the front, press Return, and it is stored in the program. You type
`RUN` and it executes from the lowest line number. Simple, immediate, and
still deeply satisfying.

When NovaBASIC boots, the screen clears to a blue background with white text and
you see:

```
NovaBASIC v1.0
Derived from EhBASIC 2.22p5
xxxxx BASIC bytes free
```

The number shown is the amount of free BASIC program memory. The cursor waits
for your first command.

::: note
Boot sets the screen background to color 6 (blue) and the foreground text
color to color 1 (white). You can change these at any time with
`COLOR fg,bg`.
:::

## Runtime Clock Model

Nova VM runs from a single CPU-cycle clock source. By default the target rate is
12 MHz (12,000,000 cycles/second), and video frame timing is derived from that
clock at 60 Hz. This keeps `VSYNC`, raster IRQ behavior, timers, and music
sequencing synchronized to one timing model.

You can override runtime behavior with environment variables:

- `NOVA_CPU_HZ`: target CPU rate in cycles/second (default 12,000,000).
- `NOVA_TURBO`: if set to `1`, `true`, `yes`, or `on`, run unthrottled.
- `NOVA_TIMING_LOG`: if set, emit periodic timing telemetry for diagnostics.

## What You Can Do

Here is what NovaBASIC puts at your fingertips:

- Write and run classic line-numbered BASIC programs in the tradition
of the Commodore 64, Apple II, and BBC Micro.
- Draw directly to a **320x200 pixel bitmap** with
**16 colors** using `PLOT`, `LINE`, `CIRCLE`,
`RECT`, `FILL`, and `PAINT`.
- Animate up to **16 hardware sprites**, each 16x16 pixels
and multicolor, with per-sprite shape, position, and flip control.
- Play **6-voice synthesized sound** (two SID chips) with ADSR
envelopes and four waveforms -- triangle, sawtooth, pulse, and noise.
- Save and load programs and data files with `SAVE`, `LOAD`,
`DIR`, and `DEL`.
- Access **512KB of banked expansion memory** for large data
sets, tile maps, sprite sheets, or music sequences.
- Drop into **6502 assembly language** via `CALL` and
`POKE`/`PEEK` for performance-critical inner loops.

## How to Read This Guide

This manual is organized so each chapter builds on the previous one. You do
not need to read it straight through -- jump to whichever section you need --
but if you are new to NovaBASIC the order makes sense:

- **Chapters 1--3** Getting started, the edit-run workflow, and the core
language: variables, arrays, control flow, operators, and built-in
functions.
- **Chapters 4--5** Graphics and sprites; sound and music. These two
chapters cover everything you need to build a real game or demo.
- **Chapters 6--7** Expansion memory and low-level access. Read these
when your projects outgrow the built-in 64KB address space or when
you want to call hand-written assembly routines.
- **Appendices** Complete command reference, memory map, hardware
register summary, error codes, and system limits.

Cross-references appear as "see Section X.Y" or "see Appendix A." Every
command name is typeset in `this style` throughout the text.

## Your First Program

Let us get something on screen right now. Type each line exactly as shown,
pressing Return after each one, then type `RUN` and press Return.

::: tryit
```basic
10 PRINT "HELLO, NOVABASIC!"
20 FOR I=1 TO 5
30   PRINT "COUNT: ";I
40 NEXT I
RUN
```

**Expected output:**

`HELLO, NOVABASIC!`
`COUNT: 1`
`COUNT: 2`
`COUNT: 3`
`COUNT: 4`
`COUNT: 5`
:::

Six lines of output, then the cursor returns to the `Ready` prompt.
Congratulations -- you have just run your first NovaBASIC program.
