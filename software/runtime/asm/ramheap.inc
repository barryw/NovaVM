; Dynamic allocator for the unused tail of Nova application RAM.

.ifndef RAMHEAP_INC_INCLUDED
RAMHEAP_INC_INCLUDED = 1

RAMHEAP_OK          = $00
RAMHEAP_ERR_NOMEM   = $01
RAMHEAP_ERR_POINTER = $02
RAMHEAP_ERR_SIZE    = $03

.global ndk_ram_alloc
.global ndk_ram_free
.global ndk_ram_status

; Absolute symbols supplied by NL after final section placement.
.global __NOVA_IMAGE_END
.global __NOVA_RAM_END

.endif
