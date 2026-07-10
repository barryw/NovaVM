; Nova-native assembler API.

.ifndef NOVAASM_INC
NOVAASM_INC = 1

.include "nobj.inc"

NASM_OK            = 0
NASM_ERR_SYNTAX    = 1
NASM_ERR_OUTPUT    = 2

.global nasm_assemble
.global nasm_source_ptr
.global nasm_source_len
.global nasm_object_ptr
.global nasm_object_cap
.global nasm_object_len
.global nasm_error

.endif
