---
title: "XBANK"
type: command
category: Memory
keywords: [xbank, memory]
syntax: "XBANK n"
see_also: [expansion-memory]
---

Select the active 64 KB XRAM bank; *n* must be less than the total bank count.
Raw BASIC XRAM commands combine the active bank with their 16-bit offset. The
equivalent assembly flat address is `n * 65536 + offset`.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
