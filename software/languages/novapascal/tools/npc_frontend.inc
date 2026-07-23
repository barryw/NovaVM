; Resident/overlay ABI for the Nova Pascal compiler frontend.

.ifndef NPC_FRONTEND_INC
NPC_FRONTEND_INC = 1

NPCFE_LOAD     = $1E00
NPCFE_MAX      = $8200
NPCFE_NAME_LEN = 10
NPC_SOURCE_CAP = $4000
NPC_INCLUDE_CAP = $4000
NPC_SOURCE_PAGE_SIZE = $2000
NPCFE_ERR_INCLUDE = 3

; Entry mailbox:
;   LIB_ARG0 b0..b2 = source XRAM double-buffer address
;   LIB_ARG1 b0..b1 = first source-page byte length
;   LIB_ARG2 b0..b2 = generated-assembly XRAM address
; Return mailbox:
;   LIB_RESULT b0..b1 = generated assembly length
;   LIB_RESULT b2     = PASCAL_* detail

.endif
