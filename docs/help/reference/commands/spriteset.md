---
title: "SPRITESET"
type: command
category: Sprites
keywords: [spriteset, sprites]
syntax: "SPRITESET n,field,value"
see_also: [graphics-and-display]
---

Write *value* to register field *field* (0--7) of sprite *n* (0--15). Field 0 accepts an unsigned 16-bit X value and writes X low/high. Other fields are bytes: 2=Y, 3=reserved, 4=shape, 5=flags, 6=priority, 7=transparent color. Sprite attribute writes are hardware-buffered and published at the frame boundary.

See the [Graphics And Display](/docs/help/guides/graphics-and-display.md) guide for more details.
