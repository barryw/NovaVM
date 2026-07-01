; libeditor.inc — EDITOR module (MODULE_ID_EDITOR = $08) fn-id contract.
;
; The shared full-screen source editor. Built once (modules/editor/editor.s),
; staged to the XRAM module store, and paged into bank 1 on lib_call. The editor
; runs entirely within one call: keyboard and VGC are MMIO, and the caller builds
; or compiles any language-specific document after the editor returns.

.ifndef LIBEDITOR_INC
LIBEDITOR_INC = 1

.include "libabi.inc"            ; MODULE_ID_EDITOR, LIB_* mailbox cells, LERR_*

; EDITOR_FN_EDIT — run EDITUI/EDITBUF on a RAM text buffer.
;   In:  ARG0 lo word  = buffer ptr        ARG0 hi word = initial cursor byte offset
;        ARG1 lo word  = current length    ARG2 lo word = capacity (bytes)
;        ARG3 lo word  = NUL-terminated title ptr
;        ARG3 byte2    = optional EDITOR_EDIT_PROFILE_* syntax/profile id
;   Out: RESULT byte0  = editbuf exit reason (EDITBUF_EXIT_*)
;        RESULT byte1  = nonzero if the user requested a save (Ctrl+K S / "Save First")
;        ARG1 lo word  = final buffer length
;        LIB_STATUS    = LERR_OK
EDITOR_FN_EDIT = $00

EDITOR_EDIT_PROFILE_NONE  = $00
EDITOR_EDIT_PROFILE_FORTH = $01
EDITOR_EDIT_PROFILE_LOGO  = $02

EDITOR_FN_COUNT = $01

.endif
