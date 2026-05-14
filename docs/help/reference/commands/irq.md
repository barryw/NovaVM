---
title: "IRQ"
type: command
category: Assembly
keywords: [irq, assembly]
syntax: "IRQ line"
see_also: [assembly, sprite, sprcoll]
---

Redirect the IRQ vector to a BASIC line number handler.

For sprite collision events, use `ON SPRITE COLLISION GOSUB line`. The handler
should read `SPRCOLL` and end with `RETIRQ`.

See the [Assembly](/docs/help/guides/assembly.md) guide for more details.
