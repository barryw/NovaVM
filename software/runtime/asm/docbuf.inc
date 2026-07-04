; docbuf.inc — shared XRAM-backed open-document buffers for native editors.
;
; DOCBUF stores inactive editor documents in platform-managed XRAM while the
; active document remains in the existing RAM edit buffer. Hosts copy editbuf
; state into DOCBUF_* before storing and copy DOCBUF_* back into editbuf after
; loading. This keeps the text editor language-neutral while giving Pascal,
; Logo, Forth, and future tools the same open-buffer behavior.

.ifndef DOCBUF_INC_INCLUDED
DOCBUF_INC_INCLUDED = 1

.include "nova.inc"

DOCBUF_MAX        = 8
DOCBUF_SLOT_BYTES = $1000
DOCBUF_ROW_WIDTH  = 32
DOCBUF_NAME_BYTES = DOCBUF_ROW_WIDTH - 3

; Fixed platform XRAM reservation for first-pass document snapshots:
;   slot 0: $055000-$055FFF
;   slot 7: $05C000-$05CFFF
; $054000-$054EFF remains editui menu save-under scratch; $060000+ is the
; paged-module shelf.
DOCBUF_XRAM_BASE_L = $00
DOCBUF_XRAM_BASE_M = $50
DOCBUF_XRAM_BASE_H = $05

.global DOCBUF_RAML
.global DOCBUF_RAMH
.global DOCBUF_CAPL
.global DOCBUF_CAPH
.global DOCBUF_LENL
.global DOCBUF_LENH
.global DOCBUF_CURL
.global DOCBUF_CURH
.global DOCBUF_DIRTY
.global DOCBUF_RANGE_OFFL
.global DOCBUF_RANGE_OFFH
.global DOCBUF_DELETE_LENL
.global DOCBUF_DELETE_LENH
.global DOCBUF_INSERT_LENL
.global DOCBUF_INSERT_LENH
.global DOCBUF_WINDOW_OFFL
.global DOCBUF_WINDOW_OFFH
.global DOCBUF_WINDOW_LENL
.global DOCBUF_WINDOW_LENH
.global DOCBUF_TITLEL
.global DOCBUF_TITLEH
.global DOCBUF_TYPEL
.global DOCBUF_TYPEH
.global DOCBUF_COUNT
.global DOCBUF_ACTIVE
.global DOCBUF_SELECTED
.global DOCBUF_STATUS
.global DOCBUF_LIST_ROWS

.global docbuf_init
.global docbuf_add_from_ram
.global docbuf_store_active
.global docbuf_load_active
.global docbuf_replace_range
.global docbuf_load_window
.global docbuf_next
.global docbuf_previous
.global docbuf_select
.global docbuf_build_list_rows

.endif
