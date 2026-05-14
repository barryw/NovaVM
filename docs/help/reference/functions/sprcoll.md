---
title: "SPRCOLL"
type: function
category: Sprites
keywords: [sprite, collision, irq]
syntax: "SPRCOLL"
see_also: [sprbg, collision, sprite]
---

Return the 16-bit sprite-to-sprite collision bitmask. Bit *n* is set when
sprite *n* participated in a sprite collision. Reading this function clears the
latched mask and acknowledges the sprite collision IRQ source.
