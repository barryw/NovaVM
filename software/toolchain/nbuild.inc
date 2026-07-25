; Language-neutral incremental-build state ABI.

.ifndef NBUILD_INC
NBUILD_INC = 1

NBUILD_OP_CHECK  = $01
NBUILD_OP_COMMIT = $02
NBUILD_OP_CLEAN  = $03

NBUILD_CLEAN = $00
NBUILD_DIRTY = $01

NBUILD_DETAIL_STATE    = $01
NBUILD_DETAIL_MANIFEST = $02
NBUILD_DETAIL_CAPACITY = $03

; NBUILD mailbox use:
;   ARG0 state file
;   ARG1 output artifact
;   ARG2 stable node name
;   ARG3 stage identity/options
;   ARG4 dependency manifest (COMMIT only; one exact filename per LF line)
;   ARG5 temporary state filename (COMMIT only)
;   FLAGS NBUILD_OP_*

.endif
