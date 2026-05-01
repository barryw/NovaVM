---
title: "FETCH"
type: command
category: Memory
keywords: [fetch, memory]
syntax: "FETCH ram,offset,length | FETCH \"name\",ram"
see_also: [expansion-memory]
---

Copy bytes from XRAM back into CPU RAM.

Raw form:

```
FETCH ram,offset,length
```

The raw form reads from *offset* in the active `XBANK`.

Named form:

```
FETCH "name",ram
```

The named form loads the full named block and uses the stored block length.

See the [Expansion Memory](/docs/help/guides/expansion-memory.md) guide for more details.
