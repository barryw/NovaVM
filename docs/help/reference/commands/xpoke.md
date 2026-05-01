---
title: "XPOKE"
type: command
category: Memory
keywords: [xpoke, memory]
syntax: "XPOKE offset,value"
see_also: [expansion-memory]
---

Write one byte to *offset* in the active `XBANK`. Use `STASH` for block copies;
`XPOKE` is intended for small byte-level updates or diagnostics.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
