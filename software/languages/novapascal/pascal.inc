; NovaPascal compiler ABI declarations. Implementations are independent
; members of PASCAL.NLIB so NL only extracts helpers a program references.

.ifndef NP_SUPPORT
NP_SUPPORT = 1

.include "nvr.inc"

.global I_P_AGET
.global I_P_ASETB
.global I_P_ASETW
.global P_ADDW
.global P_SUBW
.global P_CMPW
.global P_EQ
.global P_NE
.global P_LT
.global P_LE
.global P_GT
.global P_GE

.endif
