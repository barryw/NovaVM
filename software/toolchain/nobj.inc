; Nova native relocatable object format (NOBJ), version 2.
;
; Layout:
;   header
;   section records (each header + name + initialized bytes)
;   symbol records
;   fixed-size relocation records
;
; All offsets are little-endian object-relative offsets. Symbol and section
; indices are zero based. Version 2 carries named initialized and zerofill
; sections with independent alignment and relocation coordinates.

.ifndef NOBJ_INC
NOBJ_INC = 1

NOBJ_MAGIC0          = 'N'
NOBJ_MAGIC1          = 'O'
NOBJ_MAGIC2          = 'B'
NOBJ_MAGIC3          = 'J'
NOBJ_VERSION         = 2

NOBJ_MAGIC           = 0
NOBJ_VERSION_OFF     = 4
NOBJ_FLAGS           = 5
NOBJ_SECTION_COUNT   = 6             ; u8
NOBJ_ENTRY_SECTION   = 7             ; u8, $FF means no entry
NOBJ_ENTRY_OFFSET    = 8             ; u16
NOBJ_SYMBOL_COUNT    = 10            ; u16
NOBJ_SYMBOL_OFFSET   = 12            ; u16
NOBJ_RELOC_COUNT     = 14            ; u16
NOBJ_RELOC_OFFSET    = 16            ; u16
NOBJ_RESERVED        = 18            ; u16, must be zero
NOBJ_HEADER_SIZE     = 20

; Section record: fixed header, name bytes, then initialized data bytes.
NOBJ_SEC_FLAGS       = 0
NOBJ_SEC_ALIGN       = 1             ; log2 alignment, 0 means byte aligned
NOBJ_SEC_NAME_LEN    = 2
NOBJ_SEC_RESERVED    = 3
NOBJ_SEC_SIZE        = 4             ; u16 total in-memory size
NOBJ_SEC_DATA_LEN    = 6             ; u16 initialized byte count
NOBJ_SEC_HEADER_SIZE = 8

NOBJ_SEC_ALLOC       = $01
NOBJ_SEC_EXEC        = $02
NOBJ_SEC_WRITE       = $04
NOBJ_SEC_ZEROFILL    = $08

; Symbol record: fixed header followed by name bytes.
NOBJ_SYM_VALUE       = 0             ; u16 section-relative or absolute value
NOBJ_SYM_SECTION     = 2             ; section index or special value below
NOBJ_SYM_FLAGS       = 3
NOBJ_SYM_NAME_LEN    = 4
NOBJ_SYM_HEADER_SIZE = 5

NOBJ_SYM_UNDEFINED   = $FF
NOBJ_SYM_ABSOLUTE    = $FE
NOBJ_SYM_GLOBAL      = $01

; Relocation record: fixed size, symbol-indexed, signed 16-bit addend.
NOBJ_REL_SECTION     = 0             ; section containing the patch
NOBJ_REL_TYPE        = 1
NOBJ_REL_OFFSET      = 2             ; u16 section-relative patch offset
NOBJ_REL_SYMBOL      = 4             ; u16 symbol index
NOBJ_REL_ADDEND      = 6             ; signed u16
NOBJ_RELOC_SIZE      = 8

NOBJ_RELOC_ABS16     = 1
NOBJ_RELOC_ABS8      = 2
NOBJ_RELOC_LO8       = 3
NOBJ_RELOC_HI8       = 4
NOBJ_RELOC_REL8      = 5

; Nova static-library format, version 2. Each member is a complete NOBJ:
; object-length:u16, object:bytes. The member's NOBJ global symbol table is its
; export index, so one member may export several symbols and import other archive
; members without duplicating symbol names in the container.
NLIB_MAGIC0       = 'N'
NLIB_MAGIC1       = 'L'
NLIB_MAGIC2       = 'I'
NLIB_MAGIC3       = 'B'
NLIB_VERSION      = 2
NLIB_VERSION_OFF  = 4
NLIB_MEMBER_COUNT = 5
NLIB_HEADER_SIZE  = 6

.endif
