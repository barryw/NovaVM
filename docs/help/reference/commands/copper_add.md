---
title: "COPPER_ADD"
type: command
category: Graphics
keywords: [copper_add, graphics]
syntax: "COPPER ADD x,y,reg,val"
see_also: [graphics-and-display]
---

Add copper event: set *reg* to *val* at position (*x*,*y*). Registers: BGCOL, MODE, SCROLLX, SCROLLY, SPRX(*n*), SPRXH(*n*), SPRY(*n*), SPRSHAPE(*n*), SPRFLAGS(*n*), SPRPRI(*n*). Sprite Y high is reserved.

Advanced use: numeric register `$FE` does not write a display register. It raises VGC IRQ pending bits from *val*, so the copper can request an interrupt at an arbitrary beam position.

See the [Graphics And Display](/docs/help/guides/graphics-and-display.md) guide for more details.
