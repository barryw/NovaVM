; Nova native relocatable object format (NOBJ), version 1.
;
; Relocation records follow the code section:
;   offset:u16, type:u8, name-length:u8, uppercase-name:bytes
; The offset names the first operand byte inside the code section.

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

NOBJ_RELOC_ABS16  = 1

; Nova static-library format. Members are deliberately tiny and independent:
;   name-length:u8, name:bytes, code-length:u16, code:bytes
; Pulling only referenced members is the linker's dead-code elimination.
NLIB_MAGIC0       = 'N'
NLIB_MAGIC1       = 'L'
NLIB_MAGIC2       = 'I'
NLIB_MAGIC3       = 'B'
NLIB_VERSION      = 1
NLIB_VERSION_OFF  = 4
NLIB_MEMBER_COUNT = 5
NLIB_HEADER_SIZE  = 6

.endif
