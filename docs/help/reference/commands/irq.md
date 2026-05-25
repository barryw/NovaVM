---
title: "IRQ"
type: command
category: Assembly
keywords: [irq, assembly]
syntax: "IRQ line"
see_also: [assembly, sprite, sprcoll, memory-map]
---

Redirect the IRQ vector to a BASIC line number handler.

For sprite collision events, use `ON SPRITE COLLISION GOSUB line`. For board
button/DIP-switch change events, enable `$BA9E`, handle status at `$BA9F`, and
acknowledge the W1C latches before `RETIRQ`.

See the [Assembly](/docs/help/guides/assembly.md) guide for more details.
