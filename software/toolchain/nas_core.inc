; Fixed ABI shared by resident NAS and its disk-loaded assembly core.

.ifndef NAS_CORE_INC
NAS_CORE_INC = 1

.include "nas_backend.inc"

NASCORE_LOAD = NASBE_LOAD
NASCORE_ENTRY = NASCORE_LOAD
NASCORE_MAX = NASBE_MAX

; The resident frontend reserves these 32 zero-page bytes for the core.
NASCORE_ZP_BASE = $72
NASCORE_ZP_SIZE = 32

; Core request/result mailbox. It may overlap the inactive Pascal shell buffer.
nasm_source_ptr = $0900              ; u24 XRAM address
nasm_source_len = $0903              ; u16
nasm_source_name_ptr = $0905         ; u16
nasm_source_name_len = $0907         ; u8
nasm_object_ptr = $0908              ; u16 low/mid; high at nasm_object_ptr_h
nasm_object_cap = $090A              ; u16
nasm_object_len = $090C              ; u16
nasm_error = $090E                   ; u8
nasm_error_line = $090F              ; u16
nasm_error_column = $0911            ; u16
nasm_include_name = $0913            ; 64 bytes
nasm_include_name_len = $0953        ; u8
nasm_include_ptr = $0954             ; u24
nasm_include_len = $0957             ; u16
nasm_include_display_ptr = $0959     ; u16
nasm_error_name_ptr = $095B          ; u16
nasm_error_name_len = $095D          ; u8
NASCORE_INCLUDE_OPEN = $095E          ; u16 callback
NASCORE_INCLUDE_CLOSE = $0960         ; u16 callback
nasm_constant_ptr = $0962             ; u24 XRAM scratch for local constants
nasm_symbol_names_ptr = $0965         ; u16 low/mid XRAM regular-symbol names
nasm_reloc_buffer_ptr = $0967         ; u16 low/mid XRAM relocation buffer
nasm_include_display_len = $0969      ; u8 resolved path length
nasm_symbol_names_ptr_h = $096A       ; u8
nasm_reloc_buffer_ptr_h = $096B       ; u8
nasm_object_ptr_h = $096C             ; u8
NASCORE_INCLUDE_MODE = $096D           ; u8 callback mode
nasm_error_symbol_ptr = $096E          ; u16 optional caller buffer
NASCORE_INCLUDE_STREAM = 0
NASCORE_INCLUDE_BUFFERED = 1
NASCORE_CONSTANT_CAP = $8000
NASCORE_SYMBOL_CAP = 1024
NASCORE_SYMBOL_NAMES_CAP = NASCORE_SYMBOL_CAP * 32
NASCORE_SYMBOL_META_FIELDS = 10
NASCORE_SYMBOL_META_CAP = NASCORE_SYMBOL_CAP * NASCORE_SYMBOL_META_FIELDS
NASCORE_RELOC_CAP = $8000
NASCORE_SYMBOL_WORK_CAP = NASCORE_SYMBOL_NAMES_CAP + NASCORE_SYMBOL_META_CAP
NASCORE_OBJECT_CAP = $FFFF
NASCORE_STREAM_SENTINEL = $8001
NASCORE_INCLUDE_STREAM_CAP = $1000

.endif
