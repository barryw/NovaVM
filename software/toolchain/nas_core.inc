; Fixed ABI shared by resident NAS and its disk-loaded assembly core.

.ifndef NAS_CORE_INC
NAS_CORE_INC = 1

NASCORE_LOAD = $7000
NASCORE_ENTRY = NASCORE_LOAD
NASCORE_MAX = $3000

; The resident frontend reserves these 32 zero-page bytes for the core.
NASCORE_ZP_BASE = $72
NASCORE_ZP_SIZE = 32

; Core request/result mailbox. It may overlap the inactive Pascal shell buffer.
nasm_source_ptr = $0900              ; u24 XRAM address
nasm_source_len = $0903              ; u16
nasm_source_name_ptr = $0905         ; u16
nasm_source_name_len = $0907         ; u8
nasm_object_ptr = $0908              ; u16
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
nasm_symbol_names_ptr = $0965         ; u16 resident regular-symbol name table
nasm_reloc_buffer_ptr = $0967         ; u16 resident relocation buffer

NASCORE_CONSTANT_CAP = $8000
NASCORE_SYMBOL_NAMES_CAP = 64 * 32
NASCORE_RELOC_CAP = 768

.endif
