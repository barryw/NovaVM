---
title: "STASH"
type: command
category: Memory
keywords: [stash, memory]
syntax: "STASH ram,offset,length | STASH \"name\",ram,length"
see_also: [expansion-memory]
---

Copy *length* bytes from CPU RAM into XRAM.

Raw form:

```
STASH ram,offset,length
```

The raw form writes to *offset* in the active `XBANK`.

Named form:

```
STASH "name",ram,length
```

The named form allocates XRAM automatically and records the block under
*name*, so you can later fetch it without tracking its raw address.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
