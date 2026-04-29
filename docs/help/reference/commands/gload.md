---
title: "GLOAD"
type: command
category: File I/O
keywords: [gload, file i/o]
syntax: "GLOAD \"name\",space,offset[,len]"
see_also: [first-session]
---

Load a `.gfx` file into VGC memory. If *len* is omitted, loads the entire file.
*space* uses the VDC-style VRAM/DMA plane IDs: 1=screen, 2=color, 3=gfx bitmap, 4=sprite shapes, 6=tile data.

See the [First Session](/docs/help/guides/first-session.md) guide for more details.
