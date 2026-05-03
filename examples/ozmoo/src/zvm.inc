; Nova Ozmoo Z-machine interpreter interface.

.ifndef NOVA_OZMOO_ZVM_INC_INCLUDED
NOVA_OZMOO_ZVM_INC_INCLUDED = 1

ZVM_STACK_MAX  = 128
ZVM_FRAME_MAX  = 128
ZVM_LOCAL_MAX  = 16

ZVM_STOP_READ        = $01
ZVM_STOP_UNSUPPORTED = $02

ZVM_STREAM_SCREEN_OFF = $01
ZVM_STREAM_3_ACTIVE   = $02

.global zvm_run_until_read
.global zvm_stop_reason
.global zvm_opcode
.global zvm_pc_l
.global zvm_pc_h
.global zvm_pc_b

.endif
