; NovaZ Z-story/XRAM interface.
;
; This is the Nova-specific story memory layer the interpreter builds on.
; Dynamic memory and static/high story cache pages live in fixed XRAM
; workspaces. Static/high story bytes are demand-paged from story.bin.

.ifndef NOVA_NOVAZ_ZSTORY_INC_INCLUDED
NOVA_NOVAZ_ZSTORY_INC_INCLUDED = 1

; XRAM staging address for the NOVAZ6.BIN segment image (8KB), used by the
; runtime boot loader before DMA-copying it to CPU $2000. Rationale: the
; fixed XRAM layout (runtime/asm/xram.inc) claims $000000-$03FFFF (user
; heap), $040000-$04FFFF (dynamic story), $050000-$053FFF (static page
; cache), $054000+ (save/restore staging: save header + VM state,
; ZVM_SAVE_TOTAL_SIZE — a few KB), and $070000-$07F9FF (NVG stage).
; $060000-$061FFF sits in the documented hole between the save staging
; region and the NVG stage, leaving 48KB of growth room for save staging
; below and touching nothing above.
ZSTORY_XRAM_NOVAZ6_STAGE_L = $00       ; 24-bit XRAM address $060000,
ZSTORY_XRAM_NOVAZ6_STAGE_M = $00       ; decomposed low/mid/high.
ZSTORY_XRAM_NOVAZ6_STAGE_H = $06

; V6 picture regions, also inside the $060000-$06FFFF hole (above the 8KB
; NOVAZ6 stage, below the NVG stage at $070000 — no lifetime handoff needed):
;   $062000-$063FFF  PICS.PAK header + index (9 + count*15 bytes; the 8KB
;                    region caps count at 545 — Zork Zero ships 503, Arthur
;                    171, Journey 134, Shogun 48)
;   $064000-$06BFFF  picture bounce buffer (worst case 320x200 4bpp = 32000
;                    bytes) for draw_picture bitmap region loads
ZSTORY_XRAM_PICS_INDEX_L  = $00        ; $062000
ZSTORY_XRAM_PICS_INDEX_M  = $20
ZSTORY_XRAM_PICS_INDEX_H  = $06
ZSTORY_XRAM_PICS_BOUNCE_L = $00        ; $064000
ZSTORY_XRAM_PICS_BOUNCE_M = $40
ZSTORY_XRAM_PICS_BOUNCE_H = $06

ZSTORY_ERR_NONE        = $00
ZSTORY_ERR_IO          = $01
ZSTORY_ERR_UNSUPPORTED = $02
ZSTORY_ERR_READONLY    = $03

ZHEADER_VERSION        = $00
ZHEADER_FLAGS1         = $01
ZHEADER_RELEASE        = $02
ZHEADER_HIGH_MEMORY    = $04
ZHEADER_INITIAL_PC     = $06
ZHEADER_DICTIONARY     = $08
ZHEADER_OBJECT_TABLE   = $0A
ZHEADER_GLOBALS        = $0C
ZHEADER_STATIC_MEMORY  = $0E
ZHEADER_FLAGS2         = $10
ZHEADER_SERIAL         = $12
ZHEADER_ABBREVIATIONS  = $18
ZHEADER_FILE_LENGTH    = $1A
ZHEADER_CHECKSUM       = $1C
ZHEADER_ROUTINES_OFF   = $28
ZHEADER_STRINGS_OFF    = $2A

.global zstory_addr_l
.global zstory_addr_m
.global zstory_addr_h
.global zstory_word_hi
.global zstory_word_lo
.global zstory_version
.global zstory_flags1
.global zstory_globals_hi
.global zstory_globals_lo
.global zstory_filebytes_h
.global zstory_filebytes_m
.global zstory_filebytes_l
.global zstory_checksum_hi
.global zstory_checksum_lo
.global zstory_routine_off_l
.global zstory_routine_off_m
.global zstory_routine_off_h
.global zstory_string_off_l
.global zstory_string_off_m
.global zstory_string_off_h
.global zstory_read_file8
.global zstory_set_dynamic_xram_origin

.endif
