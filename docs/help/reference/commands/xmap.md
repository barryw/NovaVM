---
title: "XMAP"
type: command
category: Memory
keywords: [xmap, memory]
syntax: "XMAP window,offset"
see_also: [expansion-memory]
---

Map a 256-byte XRAM page in the active `XBANK` to CPU window 0--3
(`$BC00`--`$BFFF`). The low byte of *offset* is ignored because windows are
page-aligned.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
