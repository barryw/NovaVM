---
title: "SPRITE"
type: command
category: Sprites
keywords: [sprite, sprites]
syntax: "SPRITE n,ON | SPRITE n,OFF | SPRITE n,x,y | SPRITE COLLISION ON|OFF|CLEAR"
see_also: [graphics-and-display]
---

Enable (show) sprite *n*, disable it, or set its screen position. X is an unsigned 16-bit sprite-plane coordinate. Y is an unsigned 8-bit sprite-plane coordinate.

`SPRITE COLLISION ON` enables the VGC sprite-collision IRQ source. `SPRITE
COLLISION OFF` disables it, and `SPRITE COLLISION CLEAR` clears the latched
16-bit sprite collision mask and acknowledges the pending IRQ bit.

See the [Graphics And Display](/docs/help/guides/graphics-and-display.md) guide for more details.
