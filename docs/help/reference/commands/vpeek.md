---
title: "VPEEK"
type: command
category: Display
keywords: [vpeek, vram, video, screen, color, memory]
syntax: "VPEEK(plane,address)"
see_also: [vpoke, dmacopy, blitcopy, gsave, gload]
---

Read one byte from VGC video memory.

`plane` selects the video memory plane and uses the same IDs as DMA, blitter,
and `GSAVE`/`GLOAD`: 1=character RAM, 2=color RAM, 3=graphics bitmap,
4=sprite shape RAM, 6=tile data RAM, and 7=text attribute RAM.

Text character, color, and attribute memory use `address = row * 80 + column`.

```basic
10 A=VPEEK(1,0)      : REM character at upper-left
20 C=VPEEK(2,0)      : REM color attribute at upper-left
```
