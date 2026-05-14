---
title: "SPRBG"
type: function
category: Sprites
keywords: [sprite, collision, background, irq]
syntax: "SPRBG"
see_also: [sprcoll, bumped, sprite]
---

Return the 16-bit sprite-to-background collision bitmask. Bit *n* is set when
sprite *n* touched a non-transparent graphics pixel. Reading this function
clears the latched mask and acknowledges the sprite-background IRQ source.
