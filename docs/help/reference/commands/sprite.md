---
title: "SPRITE"
type: command
category: Sprites
keywords: [sprite, sprites]
syntax: "SPRITE n,ON | SPRITE n,OFF | SPRITE n,x,y | SPRITE COLLISION ON|OFF|CLEAR"
see_also: [graphics-and-display]
---

Enable (show) sprite *n*, disable it, or set its screen position. Sprite
positions use Nova canvas coordinates: X=0--319 and Y=0--199 are visible. A
16x16 sprite is fully visible at top-left positions X=0--304 and Y=0--184.
Out-of-range positions are clipped at the visible canvas; sprites do not draw
into the hardware border.

`SPRITE COLLISION ON` enables the VGC sprite-collision IRQ source. `SPRITE
COLLISION OFF` disables it, and `SPRITE COLLISION CLEAR` clears the latched
16-bit sprite collision mask and acknowledges the pending IRQ bit.

See the [Graphics And Display](/docs/help/guides/graphics-and-display.md) guide for more details.
