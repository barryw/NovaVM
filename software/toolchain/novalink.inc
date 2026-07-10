; Nova-native linker API.

.ifndef NOVALINK_INC
NOVALINK_INC = 1

.include "nobj.inc"

NLINK_OK             = 0
NLINK_ERR_OBJECT     = 1
NLINK_ERR_OUTPUT     = 2
NLINK_ERR_RELOCATION = 3

.global nlink_link
.global nlink_object_ptr
.global nlink_output_ptr
.global nlink_output_cap
.global nlink_load_base
.global nlink_output_len
.global nlink_entry
.global nlink_error

.endif
