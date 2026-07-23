; NovaMemory Pascal adapter declarations.
.ifndef NOVA_MEMORY_UNIT_INCLUDED
NOVA_MEMORY_UNIT_INCLUDED = 1
.include "libabi.inc"
.include "libmemory.inc"
.include "ramheap.inc"
.global ramalloc
.global ramfree
.global ramread
.global ramwrite
.global ramallocated
.global ramstatus
.global xramalloc
.global xramfree
.global xramread
.global xramwrite
.global xramallocated
.global xramstatus
.endif
