; Optional trailer used when an executable's final bytes are all BSS.

.ifndef NOVAEXEC_INC
NOVAEXEC_INC = 1

NOVA_EXEC_BSS_MAGIC0 = 'N'
NOVA_EXEC_BSS_MAGIC1 = 'B'
NOVA_EXEC_BSS_MAGIC2 = 'S'
NOVA_EXEC_BSS_MAGIC3 = '1'
NOVA_EXEC_BSS_TRAILER_SIZE = 8

.endif
