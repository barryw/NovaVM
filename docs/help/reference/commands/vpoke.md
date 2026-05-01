---
title: "VPOKE"
type: command
category: Display
keywords: [vpoke, vram, video, screen, color, memory]
syntax: "VPOKE plane,address,value"
see_also: [vpeek, dmafill, dmacopy, blitfill, blitcopy]
---

Write one byte to VGC video memory.

`plane` selects the video memory plane and uses the same IDs as DMA, blitter,
and `GSAVE`/`GLOAD`: 1=character RAM, 2=color RAM, 3=graphics bitmap,
4=sprite shape RAM, 6=tile data RAM, and 7=text attribute RAM.

Text character, color, and attribute memory use `address = row * 80 + column`.

```basic
10 VPOKE 1,0,65      : REM "A" at upper-left
20 VPOKE 2,0,$1E    : REM yellow foreground on blue background
```
