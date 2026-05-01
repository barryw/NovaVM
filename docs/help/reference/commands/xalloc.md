---
title: "XALLOC"
type: command
category: Memory
keywords: [xalloc, memory]
syntax: "XALLOC length"
see_also: [expansion-memory]
---

Allocate an unnamed XRAM block using the low-level allocator. Prefer named
`STASH`/`FETCH` blocks or raw offsets unless you are writing diagnostics or
tooling that inspects the XMC registers directly.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
