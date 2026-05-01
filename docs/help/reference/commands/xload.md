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

