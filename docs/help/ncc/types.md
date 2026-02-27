---
title: "NCC Data Types"
type: ncc
category: NCC Compiler
keywords: [ncc, types, byte, int, uint, bool, fixed, void]
---

## Supported Types

| Type | Size | Range | Description |
|------|------|-------|-------------|
| `byte` | 1 byte | 0 to 255 | Unsigned 8-bit integer |
| `int` | 2 bytes | -32768 to 32767 | Signed 16-bit integer |
| `uint` | 2 bytes | 0 to 65535 | Unsigned 16-bit integer |
| `bool` | 1 byte | `true` / `false` | Boolean |
| `fixed` | 2 bytes | -128.0 to 127.996 | 8.8 fixed-point |
| `void` | 0 bytes | n/a | No value (for function return types) |

## Type Rules

- `byte` is the natural type for hardware registers and memory-mapped I/O
- `int` and `uint` use little-endian byte order (low byte first), matching the 6502
- `fixed` uses 8 bits for the integer part and 8 bits for the fractional part
- Implicit narrowing (e.g., `int` to `byte`) generates a warning
- `bool` values: `false` is zero, `true` is non-zero

## Pointers

Pointer types are supported for `byte` and `int`:

```c
byte* ptr = 0xA000;   // Point to VGC base
*ptr = 0;             // Write to address
byte val = *ptr;      // Read from address
```
