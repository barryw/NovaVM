; Turbo-compatible Dos facade over Nova's canonical FILES/FIO contracts.
.ifndef NOVA_DOS_UNIT_INCLUDED
NOVA_DOS_UNIT_INCLUDED = 1
.include "libfiles.inc"
.include "nova.inc"
.include "nvr.inc"

.global doserror
.global diskfree
.global disksize
.global findfirst
.global findnext
.global getfattr
.global setfattr
.global getftime
.global setftime
.global getdate
.global setdate
.global gettime
.global settime
.global packtime
.global unpacktime
.global fexpand
.global fsplit
.endif
