---
title: "NCC Debugger"
type: ncc
category: NCC Compiler
keywords: [ncc, debug, debugger, breakpoint, step, watch]
---

## Entering Debug Mode

Press **F6** in the editor to compile your program and enter the source-level debugger. The screen splits into three panels:

1. **Source code** (top) — your C source with line numbers and gutter markers
2. **Watch panel** (middle) — CPU registers, flags, and variable values
3. **Key help** (bottom) — available debug commands

## Debug Controls

| Key | Action |
|-----|--------|
| F2 | Toggle breakpoint on current line |
| F5 | Continue running (until next breakpoint or halt) |
| F7 | Step into (same as step line for now) |
| F8 | Step line — execute until the next source line |
| Up/Down | Navigate source code |
| Page Up/Down | Scroll source code |
| Escape | Exit debug mode |

## Breakpoints

Breakpoints are shown as `*` in the gutter. When the program reaches a breakpoint, execution pauses and the current line is highlighted with `>`.

Set breakpoints before starting the debugger, or toggle them while paused.

## Watch Panel

The watch panel shows:

- **CPU registers:** A, X, Y, SP, PC
- **Status flags:** NV-BDIZC
- **Variables:** up to 8 global variables with their current values
- **Source position:** current source line and PC address
