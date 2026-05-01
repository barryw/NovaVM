---
title: "XFREE"
type: command
category: Memory
keywords: [xfree, memory]
syntax: "XFREE offset,length"
see_also: [expansion-memory]
---

Release a raw XRAM range in the active `XBANK` from usage tracking. Any named or
unnamed allocation metadata overlapping that range is removed.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
