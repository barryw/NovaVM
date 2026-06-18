; NovaVM XMC named-block allocator and command processor declarations.

.ifndef XMC_INC_INCLUDED
XMC_INC_INCLUDED = 1

.include "xram.inc"

.global xmc_process
.global xmc_command_status
.global xmc_select_bank
.global xmc_put_byte
.global xmc_stash_raw
.global xmc_fetch_raw
.global xmc_release
.global xmc_reset_usage
.global xmc_alloc_block
.global xmc_named_stash
.global xmc_named_fetch
.global xmc_named_delete
.global xmc_dir_open
.global xmc_dir_read
.global xmc_window_bit
.global xmc_set_window_addr_current_bank
.global xmc_map_window
.global xmc_unmap_window

.endif
