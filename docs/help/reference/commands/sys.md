---
title: "SYS"
type: command
category: Assembly
keywords: [sys, assembly, machine code]
syntax: "SYS addr[,a[,x[,y]]]"
see_also: [assembly]
---

Execute a machine-code routine at 6502 address *addr*. Optional byte arguments
load the 6502 `A`, `X`, and `Y` registers before the call. The routine must
return with `RTS` to resume BASIC.

Example:

```
10 SYS 36864
20 SYS 36864,60,10,1
```

See the [Assembly](/docs/help/guides/assembly.md) guide for more details.
