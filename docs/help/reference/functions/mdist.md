---
title: "MDIST"
type: function
category: Math
keywords: [mdist, math, coprocessor, distance]
syntax: "MDIST(dx,dy)"
see_also: [msin, mcos, mmulfx, mrnd]
---

Return the math coprocessor's approximate distance for signed 16-bit deltas.

The approximation is `max(abs(dx), abs(dy)) + 3/8 * min(abs(dx), abs(dy))`.
