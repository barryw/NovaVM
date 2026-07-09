; =====================================================================
;  Nova NDK — docbuf.s
;
;  Shared XRAM-backed open-document buffers for native editors. The active
;  document stays in the caller's RAM edit buffer; inactive documents are
;  fixed-size full-buffer snapshots kept in fixed XRAM slots, giving every
;  language shell the same multi-buffer contract.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "docbuf.inc"

.ifndef DOCBUF_IMPLEMENTATION_INCLUDED
DOCBUF_IMPLEMENTATION_INCLUDED = 1

DOCBUF_WIN3_ENABLE = $08

db_raml = NVR6L
db_ramh = NVR6H
db_srcL = NVR4L
db_srcH = NVR4H

      .segment "BSS"

DOCBUF_RAML:       .res 1
DOCBUF_RAMH:       .res 1
DOCBUF_CAPL:       .res 1
DOCBUF_CAPH:       .res 1
DOCBUF_LENL:       .res 1
DOCBUF_LENH:       .res 1
DOCBUF_CURL:       .res 1
DOCBUF_CURH:       .res 1
DOCBUF_DIRTY:      .res 1
DOCBUF_RANGE_OFFL: .res 1
DOCBUF_RANGE_OFFH: .res 1
DOCBUF_DELETE_LENL:.res 1
DOCBUF_DELETE_LENH:.res 1
DOCBUF_INSERT_LENL:.res 1
DOCBUF_INSERT_LENH:.res 1
DOCBUF_WINDOW_OFFL:.res 1
DOCBUF_WINDOW_OFFH:.res 1
DOCBUF_WINDOW_LENL:.res 1
DOCBUF_WINDOW_LENH:.res 1
DOCBUF_TITLEL:     .res 1
DOCBUF_TITLEH:     .res 1
DOCBUF_TYPEL:      .res 1
DOCBUF_TYPEH:      .res 1
DOCBUF_COUNT:      .res 1
DOCBUF_ACTIVE:     .res 1
DOCBUF_SELECTED:   .res 1
DOCBUF_STATUS:     .res 1
DOCBUF_LIST_ROWS:  .res DOCBUF_MAX * DOCBUF_ROW_WIDTH

db_len_l:          .res DOCBUF_MAX
db_len_h:          .res DOCBUF_MAX
db_cur_l:          .res DOCBUF_MAX
db_cur_h:          .res DOCBUF_MAX
db_dirty:          .res DOCBUF_MAX
db_title_l:        .res DOCBUF_MAX
db_title_h:        .res DOCBUF_MAX
db_type_l:         .res DOCBUF_MAX
db_type_h:         .res DOCBUF_MAX

db_slot:           .res 1
db_target:         .res 1
db_index:          .res 1
db_rowoff:         .res 1
db_xl:             .res 1
db_xm:             .res 1
db_xh:             .res 1
db_worklenl:       .res 1
db_worklenh:       .res 1
db_req_curl:       .res 1
db_req_curh:       .res 1
db_req_dirty:      .res 1
db_newlenl:        .res 1
db_newlenh:        .res 1
db_tail_startl:    .res 1
db_tail_starth:    .res 1
db_src_offl:       .res 1
db_src_offh:       .res 1
db_dst_offl:       .res 1
db_dst_offh:       .res 1
db_offl:           .res 1
db_offh:           .res 1
db_byte:           .res 1

      .segment "CODE"

      .export DOCBUF_RAML
      .export DOCBUF_RAMH
      .export DOCBUF_CAPL
      .export DOCBUF_CAPH
      .export DOCBUF_LENL
      .export DOCBUF_LENH
      .export DOCBUF_CURL
      .export DOCBUF_CURH
      .export DOCBUF_DIRTY
      .export DOCBUF_RANGE_OFFL
      .export DOCBUF_RANGE_OFFH
      .export DOCBUF_DELETE_LENL
      .export DOCBUF_DELETE_LENH
      .export DOCBUF_INSERT_LENL
      .export DOCBUF_INSERT_LENH
      .export DOCBUF_WINDOW_OFFL
      .export DOCBUF_WINDOW_OFFH
      .export DOCBUF_WINDOW_LENL
      .export DOCBUF_WINDOW_LENH
      .export DOCBUF_TITLEL
      .export DOCBUF_TITLEH
      .export DOCBUF_TYPEL
      .export DOCBUF_TYPEH
      .export DOCBUF_COUNT
      .export DOCBUF_ACTIVE
      .export DOCBUF_SELECTED
      .export DOCBUF_STATUS
      .export DOCBUF_LIST_ROWS
      .export docbuf_init
      .export docbuf_add_from_ram
      .export docbuf_store_active
      .export docbuf_load_active
      .export docbuf_replace_range
      .export docbuf_load_window
      .export docbuf_next
      .export docbuf_previous
      .export docbuf_select
      .export docbuf_build_list_rows

; @label DOCBUF.INIT
; @kind routine
; @symbol docbuf_init
; @summary Clear the shared open-document list.
docbuf_init:
      STZ   DOCBUF_COUNT
      STZ   DOCBUF_ACTIVE
      STZ   DOCBUF_SELECTED
      STZ   DOCBUF_STATUS
      LDA   #0
      RTS

; @label DOCBUF.ADD_FROM_RAM
; @kind routine
; @symbol docbuf_add_from_ram
; @summary Add a document snapshot from DOCBUF_RAML/H using DOCBUF_LENL/H.
docbuf_add_from_ram:
      LDA   DOCBUF_COUNT
      CMP   #DOCBUF_MAX
      BCC   :+
      JMP   db_error
:     JSR   db_current_len_fits_slot
      BCC   :+
      JMP   db_error
:     LDA   DOCBUF_COUNT
      STA   db_slot
      TAX
      JSR   db_store_meta_x
      LDA   db_slot
      JSR   db_set_xaddr_for_slot
      JSR   db_copy_current_ram_to_xram
      INC   DOCBUF_COUNT
      LDA   DOCBUF_COUNT
      CMP   #1
      BNE   :+
      STZ   DOCBUF_ACTIVE
:     LDA   #0
      RTS

; @label DOCBUF.STORE_ACTIVE
; @kind routine
; @symbol docbuf_store_active
; @summary Store the active document metadata and RAM bytes into its XRAM slot.
docbuf_store_active:
      LDA   DOCBUF_COUNT
      BNE   :+
      JMP   db_error
:     JSR   db_current_len_fits_slot
      BCC   :+
      JMP   db_error
:     LDX   DOCBUF_ACTIVE
      JSR   db_store_meta_x
      LDA   DOCBUF_ACTIVE
      JSR   db_set_xaddr_for_slot
      JSR   db_copy_current_ram_to_xram
      LDA   #0
      RTS

; @label DOCBUF.LOAD_ACTIVE
; @kind routine
; @symbol docbuf_load_active
; @summary Load the active document slot into the configured RAM buffer.
docbuf_load_active:
      LDA   DOCBUF_COUNT
      BNE   :+
      JMP   db_error
:     LDX   DOCBUF_ACTIVE
      JSR   db_load_meta_x
      JSR   db_current_len_fits_ram
      BCC   :+
      JMP   db_error
:     LDA   DOCBUF_ACTIVE
      JSR   db_set_xaddr_for_slot
      JSR   db_copy_xram_to_current_ram
      LDA   #0
      RTS

; @label DOCBUF.REPLACE_RANGE
; @kind routine
; @symbol docbuf_replace_range
; @summary Replace a byte range in the active XRAM document from DOCBUF_RAML/H.
docbuf_replace_range:
      LDA   DOCBUF_COUNT
      BNE   :+
      JMP   db_error
:     LDA   DOCBUF_CURL
      STA   db_req_curl
      LDA   DOCBUF_CURH
      STA   db_req_curh
      LDA   DOCBUF_DIRTY
      STA   db_req_dirty
      LDX   DOCBUF_ACTIVE
      JSR   db_load_meta_x
      LDA   db_req_curl
      STA   DOCBUF_CURL
      LDA   db_req_curh
      STA   DOCBUF_CURH
      LDA   db_req_dirty
      STA   DOCBUF_DIRTY

      CLC
      LDA   DOCBUF_RANGE_OFFL
      ADC   DOCBUF_DELETE_LENL
      STA   db_tail_startl
      LDA   DOCBUF_RANGE_OFFH
      ADC   DOCBUF_DELETE_LENH
      STA   db_tail_starth
      BCC   :+
      JMP   db_error

:     LDA   DOCBUF_LENL
      CMP   db_tail_startl
      LDA   DOCBUF_LENH
      SBC   db_tail_starth
      BCS   :+
      JMP   db_error

:     SEC
      LDA   DOCBUF_LENL
      SBC   db_tail_startl
      STA   db_worklenl
      LDA   DOCBUF_LENH
      SBC   db_tail_starth
      STA   db_worklenh

      SEC
      LDA   DOCBUF_LENL
      SBC   DOCBUF_DELETE_LENL
      STA   db_newlenl
      LDA   DOCBUF_LENH
      SBC   DOCBUF_DELETE_LENH
      STA   db_newlenh
      CLC
      LDA   db_newlenl
      ADC   DOCBUF_INSERT_LENL
      STA   db_newlenl
      LDA   db_newlenh
      ADC   DOCBUF_INSERT_LENH
      STA   db_newlenh
      BCC   :+
      JMP   db_error
:     JSR   db_new_len_fits_slot
      BCC   :+
      JMP   db_error

:     LDA   DOCBUF_INSERT_LENL
      CMP   DOCBUF_DELETE_LENL
      BNE   @size_differs
      LDA   DOCBUF_INSERT_LENH
      CMP   DOCBUF_DELETE_LENH
      BEQ   @copy_insert
@size_differs:
      LDA   DOCBUF_INSERT_LENL
      CMP   DOCBUF_DELETE_LENL
      LDA   DOCBUF_INSERT_LENH
      SBC   DOCBUF_DELETE_LENH
      BCC   @shrink_tail

      JSR   db_shift_tail_backward
      BRA   @copy_insert

@shrink_tail:
      JSR   db_shift_tail_forward

@copy_insert:
      LDA   DOCBUF_INSERT_LENL
      ORA   DOCBUF_INSERT_LENH
      BEQ   @store_meta
      LDA   DOCBUF_RANGE_OFFL
      STA   db_offl
      LDA   DOCBUF_RANGE_OFFH
      STA   db_offh
      JSR   db_set_xaddr_for_active_offset
      LDA   DOCBUF_RAML
      STA   db_raml
      LDA   DOCBUF_RAMH
      STA   db_ramh
      LDA   DOCBUF_INSERT_LENL
      STA   db_worklenl
      LDA   DOCBUF_INSERT_LENH
      STA   db_worklenh
      JSR   db_copy_ram_to_xram

@store_meta:
      LDA   db_newlenl
      STA   DOCBUF_LENL
      LDA   db_newlenh
      STA   DOCBUF_LENH
      LDX   DOCBUF_ACTIVE
      JSR   db_store_meta_x
      LDA   #0
      RTS

; @label DOCBUF.LOAD_WINDOW
; @kind routine
; @symbol docbuf_load_window
; @summary Load a bounded window of the active XRAM document into DOCBUF_RAML/H.
docbuf_load_window:
      LDA   DOCBUF_COUNT
      BNE   :+
      JMP   db_error
:     LDX   DOCBUF_ACTIVE
      JSR   db_load_meta_x
      LDA   DOCBUF_LENL
      CMP   DOCBUF_WINDOW_OFFL
      LDA   DOCBUF_LENH
      SBC   DOCBUF_WINDOW_OFFH
      BCS   :+
      JMP   db_error

:     SEC
      LDA   DOCBUF_LENL
      SBC   DOCBUF_WINDOW_OFFL
      STA   db_worklenl
      LDA   DOCBUF_LENH
      SBC   DOCBUF_WINDOW_OFFH
      STA   db_worklenh

      LDA   db_worklenl
      CMP   DOCBUF_WINDOW_LENL
      LDA   db_worklenh
      SBC   DOCBUF_WINDOW_LENH
      BCS   @requested_fits
      LDA   db_worklenl
      STA   DOCBUF_WINDOW_LENL
      LDA   db_worklenh
      STA   DOCBUF_WINDOW_LENH

@requested_fits:
      LDA   DOCBUF_CAPL
      CMP   DOCBUF_WINDOW_LENL
      LDA   DOCBUF_CAPH
      SBC   DOCBUF_WINDOW_LENH
      BCS   @cap_fits
      LDA   DOCBUF_CAPL
      STA   DOCBUF_WINDOW_LENL
      LDA   DOCBUF_CAPH
      STA   DOCBUF_WINDOW_LENH

@cap_fits:
      LDA   DOCBUF_WINDOW_OFFL
      STA   db_offl
      LDA   DOCBUF_WINDOW_OFFH
      STA   db_offh
      JSR   db_set_xaddr_for_active_offset
      LDA   DOCBUF_RAML
      STA   db_raml
      LDA   DOCBUF_RAMH
      STA   db_ramh
      LDA   DOCBUF_WINDOW_LENL
      STA   db_worklenl
      LDA   DOCBUF_WINDOW_LENH
      STA   db_worklenh
      JSR   db_copy_xram_to_ram
      LDA   #0
      RTS

; @label DOCBUF.NEXT
; @kind routine
; @symbol docbuf_next
; @summary Store the active document, advance with wrap, and load the new document.
docbuf_next:
      LDA   DOCBUF_COUNT
      BNE   :+
      JMP   db_error
:     JSR   docbuf_store_active
      BEQ   :+
      RTS
:     INC   DOCBUF_ACTIVE
      LDA   DOCBUF_ACTIVE
      CMP   DOCBUF_COUNT
      BCC   :+
      STZ   DOCBUF_ACTIVE
:     JMP   docbuf_load_active

; @label DOCBUF.PREVIOUS
; @kind routine
; @symbol docbuf_previous
; @summary Store the active document, move backward with wrap, and load the new document.
docbuf_previous:
      LDA   DOCBUF_COUNT
      BNE   :+
      JMP   db_error
:     JSR   docbuf_store_active
      BEQ   :+
      RTS
:     LDA   DOCBUF_ACTIVE
      BNE   @dec
      LDA   DOCBUF_COUNT
@dec: DEC   A
      STA   DOCBUF_ACTIVE
      JMP   docbuf_load_active

; @label DOCBUF.SELECT
; @kind routine
; @symbol docbuf_select
; @summary Select DOCBUF_SELECTED after storing the current active document.
docbuf_select:
      LDA   DOCBUF_SELECTED
      CMP   DOCBUF_COUNT
      BCC   :+
      JMP   db_error
:     STA   db_target
      JSR   docbuf_store_active
      BEQ   :+
      RTS
:     LDA   db_target
      STA   DOCBUF_ACTIVE
      JMP   docbuf_load_active

; @label DOCBUF.BUILD_LIST_ROWS
; @kind routine
; @symbol docbuf_build_list_rows
; @summary Build fixed-width DOCBUF_LIST_ROWS for the NUI list picker.
docbuf_build_list_rows:
      STZ   db_index
@row:
      LDA   db_index
      CMP   DOCBUF_COUNT
      BCC   :+
      LDA   #0
      RTS
:     ASL
      ASL
      ASL
      ASL
      ASL
      STA   db_rowoff
      LDY   db_rowoff

      LDA   db_index
      CMP   DOCBUF_ACTIVE
      BNE   @not_active
      LDA   #'>'
      BRA   @put_active
@not_active:
      LDA   #' '
@put_active:
      STA   DOCBUF_LIST_ROWS,Y
      INY

      LDX   db_index
      LDA   db_dirty,X
      BEQ   @clean
      LDA   #'*'
      BRA   @put_dirty
@clean:
      LDA   #' '
@put_dirty:
      STA   DOCBUF_LIST_ROWS,Y
      INY
      LDA   #' '
      STA   DOCBUF_LIST_ROWS,Y

      LDX   db_index
      LDA   db_title_l,X
      STA   db_srcL
      LDA   db_title_h,X
      STA   db_srcH
      LDA   db_srcL
      ORA   db_srcH
      BEQ   @pad_name_start

      LDA   db_rowoff
      CLC
      ADC   #3
      TAX
      LDY   #0
@copy_name:
      CPY   #DOCBUF_NAME_BYTES
      BCS   @next_row
      LDA   (db_srcL),Y
      BEQ   @pad_name
      STA   DOCBUF_LIST_ROWS,X
      INX
      INY
      BRA   @copy_name

@pad_name_start:
      LDA   db_rowoff
      CLC
      ADC   #3
      TAX
      LDY   #0
@pad_name:
      LDA   #' '
@pad_loop:
      CPY   #DOCBUF_NAME_BYTES
      BCS   @next_row
      STA   DOCBUF_LIST_ROWS,X
      INX
      INY
      BRA   @pad_loop

@next_row:
      INC   db_index
      JMP   @row

db_store_meta_x:
      LDA   DOCBUF_LENL
      STA   db_len_l,X
      LDA   DOCBUF_LENH
      STA   db_len_h,X
      LDA   DOCBUF_CURL
      STA   db_cur_l,X
      LDA   DOCBUF_CURH
      STA   db_cur_h,X
      LDA   DOCBUF_DIRTY
      STA   db_dirty,X
      LDA   DOCBUF_TITLEL
      STA   db_title_l,X
      LDA   DOCBUF_TITLEH
      STA   db_title_h,X
      LDA   DOCBUF_TYPEL
      STA   db_type_l,X
      LDA   DOCBUF_TYPEH
      STA   db_type_h,X
      RTS

db_load_meta_x:
      LDA   db_len_l,X
      STA   DOCBUF_LENL
      LDA   db_len_h,X
      STA   DOCBUF_LENH
      LDA   db_cur_l,X
      STA   DOCBUF_CURL
      LDA   db_cur_h,X
      STA   DOCBUF_CURH
      LDA   db_dirty,X
      STA   DOCBUF_DIRTY
      LDA   db_title_l,X
      STA   DOCBUF_TITLEL
      LDA   db_title_h,X
      STA   DOCBUF_TITLEH
      LDA   db_type_l,X
      STA   DOCBUF_TYPEL
      LDA   db_type_h,X
      STA   DOCBUF_TYPEH
      RTS

db_current_len_fits_slot:
      LDA   DOCBUF_LENH
      CMP   #>DOCBUF_SLOT_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   DOCBUF_LENL
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

db_new_len_fits_slot:
      LDA   db_newlenh
      CMP   #>DOCBUF_SLOT_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   db_newlenl
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

db_current_len_fits_ram:
      LDA   DOCBUF_CAPL
      CMP   DOCBUF_LENL
      LDA   DOCBUF_CAPH
      SBC   DOCBUF_LENH
      BCS   @fits
      SEC
      RTS
@fits:
      CLC
      RTS

db_set_xaddr_for_slot:
      STZ   db_xl
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #DOCBUF_XRAM_BASE_M
      STA   db_xm
      LDA   #DOCBUF_XRAM_BASE_H
      STA   db_xh
      RTS

db_set_xaddr_for_active_offset:
      LDA   DOCBUF_ACTIVE
      JSR   db_set_xaddr_for_slot
      CLC
      LDA   db_xl
      ADC   db_offl
      STA   db_xl
      LDA   db_xm
      ADC   db_offh
      STA   db_xm
      BCC   :+
      INC   db_xh
:     RTS

db_read_active_offset:
      JSR   db_set_xaddr_for_active_offset
      JSR   db_map_xram
      LDX   db_xl
      LDA   WIN3_BASE,X
      RTS

db_write_active_offset:
      STA   db_byte
      JSR   db_set_xaddr_for_active_offset
      JSR   db_map_xram
      LDX   db_xl
      LDA   db_byte
      STA   WIN3_BASE,X
      RTS

db_shift_tail_forward:
      LDA   db_worklenl
      ORA   db_worklenh
      BEQ   @done
      LDA   db_tail_startl
      STA   db_src_offl
      LDA   db_tail_starth
      STA   db_src_offh
      CLC
      LDA   DOCBUF_RANGE_OFFL
      ADC   DOCBUF_INSERT_LENL
      STA   db_dst_offl
      LDA   DOCBUF_RANGE_OFFH
      ADC   DOCBUF_INSERT_LENH
      STA   db_dst_offh
@loop:
      LDA   db_src_offl
      STA   db_offl
      LDA   db_src_offh
      STA   db_offh
      JSR   db_read_active_offset
      STA   db_byte
      LDA   db_dst_offl
      STA   db_offl
      LDA   db_dst_offh
      STA   db_offh
      LDA   db_byte
      JSR   db_write_active_offset
      JSR   db_inc_src_off
      JSR   db_inc_dst_off
      JSR   db_dec_worklen
      LDA   db_worklenl
      ORA   db_worklenh
      BNE   @loop
@done:
      RTS

db_shift_tail_backward:
      LDA   db_worklenl
      ORA   db_worklenh
      BEQ   @done
      LDA   DOCBUF_LENL
      STA   db_src_offl
      LDA   DOCBUF_LENH
      STA   db_src_offh
      JSR   db_dec_src_off
      LDA   db_newlenl
      STA   db_dst_offl
      LDA   db_newlenh
      STA   db_dst_offh
      JSR   db_dec_dst_off
@loop:
      LDA   db_src_offl
      STA   db_offl
      LDA   db_src_offh
      STA   db_offh
      JSR   db_read_active_offset
      STA   db_byte
      LDA   db_dst_offl
      STA   db_offl
      LDA   db_dst_offh
      STA   db_offh
      LDA   db_byte
      JSR   db_write_active_offset
      JSR   db_dec_src_off
      JSR   db_dec_dst_off
      JSR   db_dec_worklen
      LDA   db_worklenl
      ORA   db_worklenh
      BNE   @loop
@done:
      RTS

db_inc_src_off:
      INC   db_src_offl
      BNE   :+
      INC   db_src_offh
:     RTS

db_inc_dst_off:
      INC   db_dst_offl
      BNE   :+
      INC   db_dst_offh
:     RTS

db_dec_src_off:
      LDA   db_src_offl
      BNE   :+
      DEC   db_src_offh
:     DEC   db_src_offl
      RTS

db_dec_dst_off:
      LDA   db_dst_offl
      BNE   :+
      DEC   db_dst_offh
:     DEC   db_dst_offl
      RTS

db_copy_current_ram_to_xram:
      LDA   DOCBUF_RAML
      STA   db_raml
      LDA   DOCBUF_RAMH
      STA   db_ramh
      LDA   DOCBUF_LENL
      STA   db_worklenl
      LDA   DOCBUF_LENH
      STA   db_worklenh
      JMP   db_copy_ram_to_xram

db_copy_xram_to_current_ram:
      LDA   DOCBUF_RAML
      STA   db_raml
      LDA   DOCBUF_RAMH
      STA   db_ramh
      LDA   DOCBUF_LENL
      STA   db_worklenl
      LDA   DOCBUF_LENH
      STA   db_worklenh
      JMP   db_copy_xram_to_ram

db_map_xram:
      LDA   XMC_WINCTL
      ORA   #DOCBUF_WIN3_ENABLE
      STA   XMC_WINCTL
      STZ   WIN3_LO
      LDA   db_xm
      STA   WIN3_MI
      LDA   db_xh
      STA   WIN3_HI
      RTS

db_advance_xram:
      INC   db_xl
      BNE   @done
      INC   db_xm
      BNE   @remap
      INC   db_xh
@remap:
      JSR   db_map_xram
@done:
      RTS

db_advance_ram:
      INC   db_raml
      BNE   @done
      INC   db_ramh
@done:
      RTS

db_dec_worklen:
      LDA   db_worklenl
      BNE   :+
      DEC   db_worklenh
:     DEC   db_worklenl
      RTS

db_copy_ram_to_xram:
      LDA   db_worklenl
      ORA   db_worklenh
      BEQ   @done
      JSR   db_map_xram
@loop:
      LDY   #0
      LDA   (db_raml),Y
      LDX   db_xl
      STA   WIN3_BASE,X
      JSR   db_advance_ram
      JSR   db_advance_xram
      JSR   db_dec_worklen
      LDA   db_worklenl
      ORA   db_worklenh
      BNE   @loop
@done:
      RTS

db_copy_xram_to_ram:
      LDA   db_worklenl
      ORA   db_worklenh
      BEQ   @done
      JSR   db_map_xram
@loop:
      LDX   db_xl
      LDA   WIN3_BASE,X
      LDY   #0
      STA   (db_raml),Y
      JSR   db_advance_ram
      JSR   db_advance_xram
      JSR   db_dec_worklen
      LDA   db_worklenl
      ORA   db_worklenh
      BNE   @loop
@done:
      RTS

db_error:
      LDA   #1
      STA   DOCBUF_STATUS
      RTS

.endif
