---
title: "XSAVE"
type: command
category: File I/O
keywords: [xsave, xram, file i/o]
syntax: "XSAVE \"name\",offset,length"
see_also: [expansion-memory, xload]
---

Save *length* bytes directly from XRAM at *offset* in the active `XBANK` to a
file. Filenames without an extension use `.xram`; explicit extensions are
honored.

`XSAVE` streams through the shared XRAM runtime and does not require a CPU RAM
staging buffer. BASIC addresses XRAM as `XBANK` plus a 16-bit offset, while the
underlying runtime converts that to the same flat 24-bit address used by
assembly callers.

Example:

```
10 REM SAVE 4096 BYTES FROM BANK 1 OFFSET 0
20 XBANK 1
30 XSAVE "SNAPSHOT",0,4096
```
