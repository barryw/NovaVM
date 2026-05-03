---
title: "ADDR"
type: function
category: Assembly
keywords: [addr, sys, assembly, labels]
syntax: "ADDR(\"label\")"
see_also: [sys, assembly]
---

Returns the 16-bit address for a generated runtime label that has a resolved
ROM, RAM, or MMIO address.

Labels are generated from `@label` comments in the assembly runtime sources and
are documented in `docs/assembly/runtime-labels.md`. Extension-only or
link-time library labels may appear in the generated reference without being
available to `ADDR()`. Unknown or unresolved labels raise `Function Call Error`.
