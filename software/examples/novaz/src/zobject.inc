; =====================================================================
;  NovaZ — zobject.inc
;
;  Object-table helpers — public interface: error codes and the exported
;  entry points for the Z-machine object tree (parent/sibling/child links,
;  attribute test/set/clear, insert/remove, property access, short-name
;  printing).
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef NOVA_NOVAZ_ZOBJECT_INC_INCLUDED
NOVA_NOVAZ_ZOBJECT_INC_INCLUDED = 1

ZOBJECT_ERR_NONE        = $00
ZOBJECT_ERR_UNSUPPORTED = $02

.global zobject_get_parent
.global zobject_get_sibling
.global zobject_get_child
.global zobject_set_parent
.global zobject_set_sibling
.global zobject_set_child
.global zobject_test_attr
.global zobject_set_attr
.global zobject_clear_attr
.global zobject_remove
.global zobject_insert
.global zobject_get_prop
.global zobject_get_prop_addr
.global zobject_get_next_prop
.global zobject_get_prop_len
.global zobject_print_short_name

.global zobject_num
.global zobject_num_hi
.global zobject_prop_num
.global zobject_attr_num
.global zobject_value_hi
.global zobject_value_lo

.endif
