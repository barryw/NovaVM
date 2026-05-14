---
title: "MLEN2"
type: function
category: Math
keywords: [mlen2, math, coprocessor, vector, length]
syntax: "MLEN2(x,y)"
see_also: [mdotfx, mscalx, mscaly, mdist]
---

Return the low 16 bits of `x*x + y*y` using the math coprocessor vector unit.

Use small vectors when calling from BASIC if the full squared length matters.
