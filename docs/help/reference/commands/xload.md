---
title: "XLOAD"
type: command
category: File I/O
keywords: [xload, xram, file i/o]
syntax: "XLOAD \"name\",offset[,length]"
see_also: [expansion-memory, xsave]
---

Load a file directly into XRAM at *offset* in the active `XBANK`. If *length* is
omitted or zero, the entire file is loaded. Filenames without an extension use
`.xram`; explicit extensions such as `.z3` are honored.

`XLOAD` streams through the shared XRAM runtime and does not consume a CPU RAM
staging buffer. BASIC addresses XRAM as `XBANK` plus a 16-bit offset, while the
underlying runtime converts that to the same flat 24-bit address used by
assembly callers.

Examples:

```
10 REM LOAD WHOLE FILE INTO XRAM BANK 0 OFFSET 0
20 XBANK 0
30 XLOAD "STORY.Z3",0
```

```
10 REM LOAD ONLY THE FIRST 4096 BYTES
20 XBANK 2
30 XLOAD "LEVEL1",8192,4096
```
