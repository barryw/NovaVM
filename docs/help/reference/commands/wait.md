---
title: "WAIT"
type: command
category: Miscellaneous
keywords: [wait, miscellaneous]
syntax: "WAIT addr,mask[,xor]"
---

Busy-wait until `(PEEK(addr) XOR xor) AND mask` is non-zero.
