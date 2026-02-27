---
title: "NCC Language Reference"
type: ncc
category: NCC Compiler
keywords: [ncc, c, syntax, language, reference]
---

## Overview

NCC (Nova C Compiler) supports a subset of the C programming language targeting the 6502 processor. Programs are compiled to native machine code and run directly on the virtual CPU.

## Program Structure

```c
// Global variables
byte counter;
int score;

void main() {
    // Entry point
    counter = 0;
    score = 100;
}
```

## Statements

- Variable declarations: `byte x; int y;`
- Assignment: `x = 42;`
- If/else: `if (x > 0) { ... } else { ... }`
- While loops: `while (x < 10) { x = x + 1; }`
- For loops: `for (byte i = 0; i < 10; i = i + 1) { ... }`
- Return: `return value;`
- Function calls: `plot(10, 20);`

## Operators

| Operator | Description |
|----------|-------------|
| `+` `-` `*` `/` | Arithmetic |
| `==` `!=` `<` `>` `<=` `>=` | Comparison |
| `&&` `\|\|` `!` | Logical |
| `&` `\|` `^` `~` | Bitwise |
| `<<` `>>` | Shift |

## Hardware Access

Use `peek()` and `poke()` to read and write hardware registers:

```c
poke(0xA001, 6);      // Set background color to blue
byte ch = peek(0xA00F); // Read keyboard input
```
