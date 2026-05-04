; NovaZ Z-story/XRAM interface.
;
; This is the Nova-specific story memory layer the interpreter will build on.
; The whole story file is loaded into flat XRAM at address $000000, and these
; routines provide byte/word reads plus decoded header state.

.ifndef NOVA_NOVAZ_ZSTORY_INC_INCLUDED
NOVA_NOVAZ_ZSTORY_INC_INCLUDED = 1

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

.global zstory_addr_l
.global zstory_addr_m
.global zstory_addr_h
.global zstory_word_hi
.global zstory_word_lo
.global zstory_version
.global zstory_flags1
.global zstory_globals_hi
.global zstory_globals_lo

.endif
