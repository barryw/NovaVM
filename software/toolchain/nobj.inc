; Nova native relocatable object format (NOBJ), version 1.
;
; The first vertical slice carries one code section and one entry offset. The
; relocation count is reserved in the on-disk contract so the same format can
; grow local/external symbol relocation without replacing the assembler IDE's
; files.

.ifndef NOBJ_INC
NOBJ_INC = 1

NOBJ_MAGIC0       = 'N'
NOBJ_MAGIC1       = 'O'
NOBJ_MAGIC2       = 'B'
NOBJ_MAGIC3       = 'J'
NOBJ_VERSION      = 1

NOBJ_MAGIC        = 0
NOBJ_VERSION_OFF  = 4
NOBJ_FLAGS        = 5
NOBJ_CODE_LEN     = 6             ; u16
NOBJ_ENTRY        = 8             ; u16 section-relative entry offset
NOBJ_RELOC_COUNT  = 10            ; u16
NOBJ_HEADER_SIZE  = 12

.endif
