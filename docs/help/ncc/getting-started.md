---
title: "NCC Getting Started"
type: ncc
category: NCC Compiler
keywords: [ncc, editor, compile, run, getting started]
---

## Launching NCC

There are two ways to open the Nova C Compiler editor:

1. **From BASIC:** Type `NCC` at the Ready prompt. You will be asked to confirm, since entering the editor clears the BASIC program from memory.
2. **Keyboard shortcut:** Press **Cmd+N** (Mac) or **Ctrl+N** (Windows/Linux).

## Editor Controls

| Key | Action |
|-----|--------|
| Arrow keys | Move cursor |
| Home / End | Start / end of line |
| Ctrl+Home / Ctrl+End | Top / bottom of file |
| Page Up / Page Down | Scroll by page |
| Tab | Insert 2 spaces |
| Ctrl+S | Save file |
| Ctrl+O | Open file by name |
| Ctrl+N | New file |
| F3 | Open file browser |
| F5 | Compile and run |
| F6 | Compile and debug |
| F9 | Compile only (check for errors) |
| Ctrl+Q | Exit to BASIC (with confirmation) |
| Escape | Clear message / cancel sub-mode |

## Compile and Run Workflow

1. Type your C program in the editor
2. Press **F9** to compile — errors appear in the status bar
3. Press **F5** to compile and run
4. Press **Escape** to stop a running program

The compiler generates 6502 machine code starting at address `$0280`. Your program runs directly on the virtual CPU.
