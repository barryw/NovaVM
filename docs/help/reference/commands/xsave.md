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

