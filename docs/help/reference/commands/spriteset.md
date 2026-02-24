---
title: "SPRITESET"
type: command
category: Sprites
keywords: [spriteset, sprites]
syntax: "SPRITESET n,field,value"
see_also: [graphics-and-display]
---

Write *value* (0--255) to register field *field* (0--7) of sprite *n* (0--15), waiting for vblank first. Fields: 0=X low, 1=X high, 2=Y low, 3=Y high, 4=shape, 5=flags, 6=priority, 7=transparent color.

See the [Graphics And Display](/docs/help/guides/graphics-and-display.md) guide for more details.
