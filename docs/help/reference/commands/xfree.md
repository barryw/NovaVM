---
title: "XFREE"
type: command
category: Memory
keywords: [xfree, memory]
syntax: "XFREE offset,length"
see_also: [expansion-memory]
---

Release a raw XRAM range from low-heap usage tracking. Any named or unnamed
allocation metadata overlapping that range is removed. The XMC allocator tracks
the low 256 KB BASIC/XMC heap; higher fixed workspaces are not heap-managed.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
