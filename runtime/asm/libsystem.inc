; libsystem.inc — SYSTEM module (MODULE_ID_SYSTEM = $03) fn-id contract.
;
; The shared system-services module: host-neutral facilities that no single
; runtime should carry in its own 16K ROM. Built once (modules/system/system.s),
; staged to the XRAM shelf, paged into bank 1 on first lib_call.
;
; Phase A ships exactly one fn — the interactive text editor (SYS_FN_EDIT), moved
; verbatim out of the NovaLogo extension ROM. The editor is language-neutral
; (see editbuf.inc): it edits a flat text buffer in low RAM and runs entirely
; within one lib_call (keyboard + screen are MMIO, which survive the bank swap).
; No module->runtime callback is needed; the host builds any document record from
; the final buffer AFTER the editor returns.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_SYSTEM
;   LIB_FN_ID  ($0301) = one of the SYS_FN_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code

.ifndef LIBSYSTEM_INC
LIBSYSTEM_INC = 1

.include "libabi.inc"            ; MODULE_ID_SYSTEM, LIB_* mailbox cells, LERR_*

; =====================================================================
; $00  editor
; =====================================================================
; SYS_FN_EDIT — run the shared EDITUI/EDITBUF modal editor on a RAM text buffer.
;   In:  ARG0 lo word  = buffer ptr        ARG0 hi word = initial cursor byte offset
;        ARG1 lo word  = current length    ARG2 lo word = capacity (bytes)
;        ARG3 lo word  = NUL-terminated title ptr
;   Out: RESULT byte0  = editbuf exit reason (EDITBUF_EXIT_*)
;        RESULT byte1  = nonzero if the user requested a save (Ctrl-S / "Save First")
;        ARG1 lo word  = final buffer length
;        LIB_STATUS    = LERR_OK
SYS_FN_EDIT   = $00

; Highest implemented fn-id + 1. Gaps and ids >= SYS_FN_COUNT resolve to
; LERR_NO_FN via the dispatch bounds-check.
SYS_FN_COUNT  = $01

.endif
