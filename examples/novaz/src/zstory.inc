; NovaZ Z-story/XRAM interface.
;
; This is the Nova-specific story memory layer the interpreter builds on.
; Dynamic memory and static/high story cache pages live in XRAM blocks
; requested from the shared MEMORY/XMC service. Static/high story bytes are
; demand-paged from story.bin.

.ifndef NOVA_NOVAZ_ZSTORY_INC_INCLUDED
NOVA_NOVAZ_ZSTORY_INC_INCLUDED = 1

ZSTORY_ERR_NONE        = $00
ZSTORY_ERR_IO          = $01
ZSTORY_ERR_UNSUPPORTED = $02
ZSTORY_ERR_READONLY    = $03

; Offsets inside the allocator-owned zstory_cache_base auxiliary arena.
; These are not absolute XRAM addresses: zstory_load_default requests the
; entire arena from MEMORY/XMC before any of these offsets are used.
ZSTORY_AUX_CACHE_SIZE_M       = $40   ; +$0000..+$3FFF: story page cache
ZSTORY_AUX_SAVE_CHAR_OFF_M    = $40   ; +$4000: NUI save-under char plane
ZSTORY_AUX_SAVE_COLOR_OFF_M   = $42   ; +$4200: NUI save-under color plane
ZSTORY_AUX_SAVE_ATTR_OFF_M    = $44   ; +$4400: NUI save-under attr plane
ZSTORY_AUX_SAVE_GFX_OFF_M     = $46   ; +$4600: NUI save-under graphics plane
ZSTORY_AUX_SAVE_TEXT_OFF_M    = $50   ; +$5000: save/load full VTEXT plane scratch
ZSTORY_AUX_PICS_INDEX_OFF_M   = $64   ; +$6400: V6 PICS.PAK index
ZSTORY_AUX_PICS_BOUNCE_OFF_M  = $84   ; +$8400: V6 picture bitmap bounce

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
.global zstory_dynamic_base_l
.global zstory_dynamic_base_m
.global zstory_dynamic_base_h
.global zstory_cache_base_l
.global zstory_cache_base_m
.global zstory_cache_base_h
.global zstory_routine_off_l
.global zstory_routine_off_m
.global zstory_routine_off_h
.global zstory_string_off_l
.global zstory_string_off_m
.global zstory_string_off_h
.global zstory_read_file8
.global zstory_set_dynamic_xram_origin

.endif
