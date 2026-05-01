---
title: "SYS"
type: command
category: Assembly
keywords: [sys, assembly, machine code]
syntax: "SYS addr"
see_also: [assembly]
---

Execute a machine-code routine at 6502 address *addr*. The routine must return
with `RTS` to resume BASIC.

Example:

```
10 SYS 36864
```

See the [Assembly](/docs/help/guides/assembly.md) guide for more details.
