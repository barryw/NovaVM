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
;        ARG1 lo word  = current length    ARG1 hi word = optional redo snapshot buffer
;        ARG2 lo word  = capacity (bytes)  ARG2 hi word = optional undo snapshot buffer
;        ARG3 lo word  = NUL-terminated title ptr
;        ARG3 hi word  = pointer to a 16-byte EDITOR_HOOKS_* table, or 0
;   Out: RESULT byte0  = editbuf exit reason (EDITBUF_EXIT_*)
;        RESULT byte1  = nonzero if the user requested a save (Ctrl-S / "Save First")
;        ARG1 lo word  = final buffer length
;        LIB_STATUS    = LERR_OK
; Hook table entries are copied directly into EDITBUF_TYPEL through
; EDITBUF_CHANGED_VECH before editbuf_run. A zero hook vector is allowed; editbuf
; replaces it with its default no-op hook. A zero TYPE pointer displays "Text".
; Nonzero vectors must be callable while the EDITOR module is paged in; language
; runtimes that live in another bank should point at low-RAM trampolines. The
; module wraps SAVE so RESULT byte1 still records that the user requested save.
EDITOR_FN_EDIT = $00

; EDITOR_FN_EDIT hook table offsets. The layout intentionally mirrors the
; contiguous EDITBUF config bytes so the module can apply it cheaply.
EDITOR_HOOKS_TYPEL        = 0
EDITOR_HOOKS_TYPEH        = 1
EDITOR_HOOKS_STATUSL      = 2
EDITOR_HOOKS_STATUSH      = 3
EDITOR_HOOKS_SAVE_VECL    = 4
EDITOR_HOOKS_SAVE_VECH    = 5
EDITOR_HOOKS_INDENT_VECL  = 6
EDITOR_HOOKS_INDENT_VECH  = 7
EDITOR_HOOKS_HILITE_VECL  = 8
EDITOR_HOOKS_HILITE_VECH  = 9
EDITOR_HOOKS_MENU_VECL    = 10
EDITOR_HOOKS_MENU_VECH    = 11
EDITOR_HOOKS_COMMAND_VECL = 12
EDITOR_HOOKS_COMMAND_VECH = 13
EDITOR_HOOKS_CHANGED_VECL = 14
EDITOR_HOOKS_CHANGED_VECH = 15
EDITOR_HOOKS_SIZE         = 16

; Low-RAM EDITBUF addresses visible to caller-owned hook code while the EDITOR
; module is paged in. Hook code that lives outside the editor module cannot link
; against EDITBUF labels directly, so these fixed addresses are part of the hook
; ABI and are tested against modules/editor/editor.map.
EDITOR_HOOK_ABI_HL_PTRL       = $00AB
EDITOR_HOOK_ABI_HL_PTRH       = $00AC
EDITOR_HOOK_ABI_BUFL          = $0430
EDITOR_HOOK_ABI_LENL          = $0438
EDITOR_HOOK_ABI_TYPEL         = $043C
EDITOR_HOOK_ABI_STATUSL       = $043E
EDITOR_HOOK_ABI_SAVE_VECL     = $0440
EDITOR_HOOK_ABI_INDENT_VECL   = $0442
EDITOR_HOOK_ABI_HILITE_VECL   = $0444
EDITOR_HOOK_ABI_MENU_VECL     = $0446
EDITOR_HOOK_ABI_COMMAND_VECL  = $0448
EDITOR_HOOK_ABI_CHANGED_VECL  = $044A
EDITOR_HOOK_ABI_CURL          = $044D
EDITOR_HOOK_ABI_HL_LEN        = $055C
EDITOR_HOOK_ABI_HL_COLORS     = $055D

EDITOR_FN_COUNT = $01

.endif
