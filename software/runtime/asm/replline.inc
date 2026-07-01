; Shared Nova REPL line reader declarations.
;
; A runtime supplies repl_line_complete, which returns Z=1 when the current
; input_buf contents are complete and repl_read_line should return.

.ifndef REPLLINE_INC_INCLUDED
REPLLINE_INC_INCLUDED = 1

.global repl_init
.global repl_read_line
.global input_buf
.globalzp buf_idx

.endif
