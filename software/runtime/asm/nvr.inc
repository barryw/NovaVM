; Nova NDK pseudo-register mailbox.
;
; These bytes are ephemeral argument/scratch storage. A routine may clobber
; them unless its contract says otherwise; persistent state belongs in a
; linker-managed segment.

.ifndef NVR_INC_INCLUDED
NVR_INC_INCLUDED = 1

.ifndef NVR0L
NVR0L = $20
.endif
.ifndef NVR0H
NVR0H = $21
.endif
.ifndef NVR1L
NVR1L = $22
.endif
.ifndef NVR1H
NVR1H = $23
.endif
.ifndef NVR2L
NVR2L = $24
.endif
.ifndef NVR2H
NVR2H = $25
.endif
.ifndef NVR3L
NVR3L = $26
.endif
.ifndef NVR3H
NVR3H = $27
.endif
.ifndef NVR4L
NVR4L = $28
.endif
.ifndef NVR4H
NVR4H = $29
.endif
.ifndef NVR5L
NVR5L = $2A
.endif
.ifndef NVR5H
NVR5H = $2B
.endif
.ifndef NVR6L
NVR6L = $2C
.endif
.ifndef NVR6H
NVR6H = $2D
.endif
.ifndef NVR7L
NVR7L = $2E
.endif
.ifndef NVR7H
NVR7H = $2F
.endif

.endif
