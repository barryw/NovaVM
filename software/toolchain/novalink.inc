; Nova-native linker API.

.ifndef NOVALINK_INC
NOVALINK_INC = 1

.include "nobj.inc"
.include "nl_worker.inc"

NLINK_OK             = 0
NLINK_ERR_OBJECT     = 1
NLINK_ERR_OUTPUT     = 2
NLINK_ERR_RELOCATION = 3
NLINK_ERR_CONFIG     = 4
NLINK_OBJECT_CAP     = NLW_OBJECT_CAP
NLINK_REGION_CAP     = 4
NLINK_RULE_CAP       = 8
NLINK_SYMBOL_CAP     = 4
NLINK_NAME_CAP       = 16

.global nlink_link
.global nlink_prepare
.global nlink_export_worker_state
.global nlink_import_worker_objects
.global nlink_object_count
.global nlink_object_ptr_l
.global nlink_object_ptr_h
.global nlink_object_len_l
.global nlink_object_len_h
.global nlink_object_xram
.global nlink_object_xaddr_l
.global nlink_object_xaddr_m
.global nlink_object_xaddr_h
.global nlink_library_ptr
.global nlink_library_len
.global nlink_output_ptr
.global nlink_output_xram
.global nlink_output_xaddr
.global nlink_output_cap
.global nlink_load_base
.global nlink_output_len
.global nlink_memory_len
.global nlink_entry
.global nlink_error
.global nlink_config_enabled
.global nlink_region_count
.global nlink_region_name_len
.global nlink_region_names
.global nlink_region_start_l
.global nlink_region_start_h
.global nlink_region_size_l
.global nlink_region_size_h
.global nlink_rule_count
.global nlink_rule_name_len
.global nlink_rule_names
.global nlink_rule_region
.global nlink_config_symbol_count
.global nlink_config_symbol_name_len
.global nlink_config_symbol_names
.global nlink_config_symbol_value_l
.global nlink_config_symbol_value_h

.endif
