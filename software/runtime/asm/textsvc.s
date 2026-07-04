; textsvc.s — shared XRAM-backed text editing services.
;
; This intentionally does not link the general XMC allocator. The editor module
; is a 16K paged library, so text services use fixed NDK-owned XRAM reservations
; documented in textsvc.inc. The API is language-neutral; all editors get the
; same clipboard and undo/redo behavior.

.include "textsvc.inc"

.ifndef TEXTSVC_IMPLEMENTATION_INCLUDED
TEXTSVC_IMPLEMENTATION_INCLUDED = 1

TEXTSVC_STACK_HEADER_BYTES = 6
TEXTSVC_STACK_EMPTY_L      = $FF
TEXTSVC_STACK_EMPTY_H      = $FF
TEXTSVC_WIN3_ENABLE       = $08
ts_raml                  = NVR6L
ts_ramh                  = NVR6H

      .segment "BSS"

TEXTSVC_CLIPLENL:   .res 1
TEXTSVC_CLIPLENH:   .res 1
TEXTSVC_CURL:       .res 1
TEXTSVC_CURH:       .res 1
TEXTSVC_REST_LENL:  .res 1
TEXTSVC_REST_LENH:  .res 1
TEXTSVC_REST_CURL:  .res 1
TEXTSVC_REST_CURH:  .res 1

ts_undo_usedl:      .res 1
ts_undo_usedh:      .res 1
ts_undo_topl:       .res 1
ts_undo_toph:       .res 1
ts_undo_valid:      .res 1
ts_redo_usedl:      .res 1
ts_redo_usedh:      .res 1
ts_redo_topl:       .res 1
ts_redo_toph:       .res 1
ts_redo_valid:      .res 1

ts_base_xm:         .res 1
ts_base_xh:         .res 1
ts_usedl:           .res 1
ts_usedh:           .res 1
ts_topl:            .res 1
ts_toph:            .res 1
ts_valid:           .res 1

ts_savel:           .res 1
ts_saveh:           .res 1
ts_buflenl:         .res 1
ts_buflenh:         .res 1
ts_recordl:         .res 1
ts_recordh:         .res 1
ts_endl:            .res 1
ts_endh:            .res 1
ts_offl:            .res 1
ts_offh:            .res 1
ts_xl:              .res 1
ts_xm:              .res 1
ts_xh:              .res 1
ts_worklenl:        .res 1
ts_worklenh:        .res 1
ts_header:          .res TEXTSVC_STACK_HEADER_BYTES

      .segment "CODE"

      .export textsvc_init
      .export textsvc_clip_release
      .export textsvc_clip_copy_from_ram
      .export textsvc_clip_fetch_to_ram
      .export textsvc_undo_capture
      .export textsvc_undo_apply
      .export textsvc_redo_apply

textsvc_init:
      JSR   textsvc_clip_release
      JSR   ts_reset_undo
      JSR   ts_reset_redo
      LDA   #0
      RTS

; ---------------------------------------------------------------------
; Clipboard
; ---------------------------------------------------------------------

textsvc_clip_release:
      STZ   TEXTSVC_CLIPLENL
      STZ   TEXTSVC_CLIPLENH
      LDA   #0
      RTS

; Copy RAM[XMC_RAML/H, XMC_LENL/H] into the shared XRAM clipboard.
textsvc_clip_copy_from_ram:
      JSR   ts_save_current_args
      LDA   ts_buflenl
      ORA   ts_buflenh
      BNE   :+
      JMP   textsvc_clip_release
:     JSR   ts_clip_len_fits
      BCC   :+
      LDA   #1
      RTS
:     LDA   #TEXTSVC_CLIP_XRAM_L
      STA   ts_xl
      LDA   #TEXTSVC_CLIP_XRAM_M
      STA   ts_xm
      LDA   #TEXTSVC_CLIP_XRAM_H
      STA   ts_xh
      LDA   ts_savel
      STA   ts_raml
      LDA   ts_saveh
      STA   ts_ramh
      LDA   ts_buflenl
      STA   ts_worklenl
      LDA   ts_buflenh
      STA   ts_worklenh
      JSR   ts_copy_ram_to_xram
      LDA   ts_buflenl
      STA   TEXTSVC_CLIPLENL
      LDA   ts_buflenh
      STA   TEXTSVC_CLIPLENH
      LDA   #0
      RTS

; Fetch the shared clipboard into RAM at XMC_RAML/H.
textsvc_clip_fetch_to_ram:
      LDA   TEXTSVC_CLIPLENL
      ORA   TEXTSVC_CLIPLENH
      BNE   :+
      LDA   #1
      RTS
:     LDA   #TEXTSVC_CLIP_XRAM_L
      STA   ts_xl
      LDA   #TEXTSVC_CLIP_XRAM_M
      STA   ts_xm
      LDA   #TEXTSVC_CLIP_XRAM_H
      STA   ts_xh
      LDA   XMC_RAML
      STA   ts_raml
      LDA   XMC_RAMH
      STA   ts_ramh
      LDA   TEXTSVC_CLIPLENL
      STA   ts_worklenl
      LDA   TEXTSVC_CLIPLENH
      STA   ts_worklenh
      JSR   ts_copy_xram_to_ram
      LDA   #0
      RTS

ts_clip_len_fits:
      LDA   ts_buflenh
      CMP   #>TEXTSVC_CLIP_CAP_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   ts_buflenl
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

; ---------------------------------------------------------------------
; Undo / redo
; ---------------------------------------------------------------------

textsvc_undo_capture:
      JSR   ts_save_current_args
      JSR   ts_reset_redo
      JSR   ts_load_undo
      JSR   ts_stack_capture
      PHP
      JSR   ts_store_undo
      PLP
      BCC   @ok
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

textsvc_undo_apply:
      JSR   ts_save_current_args
      JSR   ts_load_undo
      LDA   ts_valid
      BNE   :+
      LDA   #1
      RTS
:
      JSR   ts_load_redo
      JSR   ts_stack_capture
      JSR   ts_store_redo
      JSR   ts_load_undo
      JSR   ts_stack_restore
      PHP
      JSR   ts_store_undo
      PLP
      BCC   @ok
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

textsvc_redo_apply:
      JSR   ts_save_current_args
      JSR   ts_load_redo
      LDA   ts_valid
      BNE   :+
      LDA   #1
      RTS
:
      JSR   ts_load_undo
      JSR   ts_stack_capture
      JSR   ts_store_undo
      JSR   ts_load_redo
      JSR   ts_stack_restore
      PHP
      JSR   ts_store_redo
      PLP
      BCC   @ok
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

ts_save_current_args:
      LDA   XMC_RAML
      STA   ts_savel
      LDA   XMC_RAMH
      STA   ts_saveh
      LDA   XMC_LENL
      STA   ts_buflenl
      LDA   XMC_LENH
      STA   ts_buflenh
      RTS

ts_reset_undo:
      STZ   ts_undo_usedl
      STZ   ts_undo_usedh
      STZ   ts_undo_topl
      STZ   ts_undo_toph
      STZ   ts_undo_valid
      RTS

ts_reset_redo:
      STZ   ts_redo_usedl
      STZ   ts_redo_usedh
      STZ   ts_redo_topl
      STZ   ts_redo_toph
      STZ   ts_redo_valid
      RTS

ts_load_undo:
      LDA   #TEXTSVC_UNDO_XRAM_M
      STA   ts_base_xm
      LDA   #TEXTSVC_UNDO_XRAM_H
      STA   ts_base_xh
      LDA   ts_undo_usedl
      STA   ts_usedl
      LDA   ts_undo_usedh
      STA   ts_usedh
      LDA   ts_undo_topl
      STA   ts_topl
      LDA   ts_undo_toph
      STA   ts_toph
      LDA   ts_undo_valid
      STA   ts_valid
      RTS

ts_store_undo:
      LDA   ts_usedl
      STA   ts_undo_usedl
      LDA   ts_usedh
      STA   ts_undo_usedh
      LDA   ts_topl
      STA   ts_undo_topl
      LDA   ts_toph
      STA   ts_undo_toph
      LDA   ts_valid
      STA   ts_undo_valid
      RTS

ts_load_redo:
      LDA   #TEXTSVC_REDO_XRAM_M
      STA   ts_base_xm
      LDA   #TEXTSVC_REDO_XRAM_H
      STA   ts_base_xh
      LDA   ts_redo_usedl
      STA   ts_usedl
      LDA   ts_redo_usedh
      STA   ts_usedh
      LDA   ts_redo_topl
      STA   ts_topl
      LDA   ts_redo_toph
      STA   ts_toph
      LDA   ts_redo_valid
      STA   ts_valid
      RTS

ts_store_redo:
      LDA   ts_usedl
      STA   ts_redo_usedl
      LDA   ts_usedh
      STA   ts_redo_usedh
      LDA   ts_topl
      STA   ts_redo_topl
      LDA   ts_toph
      STA   ts_redo_toph
      LDA   ts_valid
      STA   ts_redo_valid
      RTS

ts_reset_active:
      STZ   ts_usedl
      STZ   ts_usedh
      STZ   ts_topl
      STZ   ts_toph
      STZ   ts_valid
      RTS

; Push a full-buffer snapshot into the active stack. If the active arena is
; full, older history is discarded and the new snapshot becomes the only entry.
ts_stack_capture:
      CLC
      LDA   ts_buflenl
      ADC   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_recordl
      LDA   ts_buflenh
      ADC   #0
      STA   ts_recordh
      JSR   ts_record_fits_arena
      BCC   :+
      SEC
      RTS
:     JSR   ts_compute_end
      JSR   ts_end_fits_arena
      BCC   @room
      JSR   ts_reset_active
      LDA   ts_recordl
      STA   ts_endl
      LDA   ts_recordh
      STA   ts_endh
@room:
      LDA   ts_valid
      BEQ   @empty_prev
      LDA   ts_topl
      STA   ts_header+0
      LDA   ts_toph
      STA   ts_header+1
      BRA   @have_prev
@empty_prev:
      LDA   #TEXTSVC_STACK_EMPTY_L
      STA   ts_header+0
      LDA   #TEXTSVC_STACK_EMPTY_H
      STA   ts_header+1
@have_prev:
      LDA   ts_buflenl
      STA   ts_header+2
      LDA   ts_buflenh
      STA   ts_header+3
      LDA   TEXTSVC_CURL
      STA   ts_header+4
      LDA   TEXTSVC_CURH
      STA   ts_header+5

      LDA   ts_usedl
      STA   ts_offl
      LDA   ts_usedh
      STA   ts_offh
      JSR   ts_xaddr_from_active_off
      LDA   #<ts_header
      STA   ts_raml
      LDA   #>ts_header
      STA   ts_ramh
      LDA   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_worklenl
      STZ   ts_worklenh
      JSR   ts_copy_ram_to_xram

      CLC
      LDA   ts_usedl
      ADC   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_offl
      LDA   ts_usedh
      ADC   #0
      STA   ts_offh
      JSR   ts_xaddr_from_active_off
      LDA   ts_savel
      STA   ts_raml
      LDA   ts_saveh
      STA   ts_ramh
      LDA   ts_buflenl
      STA   ts_worklenl
      LDA   ts_buflenh
      STA   ts_worklenh
      JSR   ts_copy_ram_to_xram

      LDA   ts_usedl
      STA   ts_topl
      LDA   ts_usedh
      STA   ts_toph
      LDA   ts_endl
      STA   ts_usedl
      LDA   ts_endh
      STA   ts_usedh
      LDA   #1
      STA   ts_valid
      CLC
      RTS

ts_stack_restore:
      LDA   ts_valid
      BNE   :+
      SEC
      RTS
:     LDA   ts_topl
      STA   ts_offl
      LDA   ts_toph
      STA   ts_offh
      JSR   ts_xaddr_from_active_off
      LDA   #<ts_header
      STA   ts_raml
      LDA   #>ts_header
      STA   ts_ramh
      LDA   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_worklenl
      STZ   ts_worklenh
      JSR   ts_copy_xram_to_ram

      CLC
      LDA   ts_topl
      ADC   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_offl
      LDA   ts_toph
      ADC   #0
      STA   ts_offh
      JSR   ts_xaddr_from_active_off
      LDA   ts_savel
      STA   ts_raml
      LDA   ts_saveh
      STA   ts_ramh
      LDA   ts_header+2
      STA   ts_worklenl
      STA   TEXTSVC_REST_LENL
      LDA   ts_header+3
      STA   ts_worklenh
      STA   TEXTSVC_REST_LENH
      LDA   ts_header+4
      STA   TEXTSVC_REST_CURL
      LDA   ts_header+5
      STA   TEXTSVC_REST_CURH
      JSR   ts_copy_xram_to_ram

      LDA   ts_topl
      STA   ts_usedl
      LDA   ts_toph
      STA   ts_usedh
      LDA   ts_header+0
      CMP   #TEXTSVC_STACK_EMPTY_L
      BNE   @has_prev
      LDA   ts_header+1
      CMP   #TEXTSVC_STACK_EMPTY_H
      BNE   @has_prev
      STZ   ts_topl
      STZ   ts_toph
      STZ   ts_valid
      CLC
      RTS
@has_prev:
      LDA   ts_header+0
      STA   ts_topl
      LDA   ts_header+1
      STA   ts_toph
      LDA   #1
      STA   ts_valid
      CLC
      RTS

ts_compute_end:
      CLC
      LDA   ts_usedl
      ADC   ts_recordl
      STA   ts_endl
      LDA   ts_usedh
      ADC   ts_recordh
      STA   ts_endh
      BCC   @done
      LDA   #$FF
      STA   ts_endl
      STA   ts_endh
@done:
      RTS

ts_record_fits_arena:
      LDA   ts_recordh
      CMP   #>TEXTSVC_UNDO_ARENA_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   ts_recordl
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

ts_end_fits_arena:
      LDA   ts_endh
      CMP   #>TEXTSVC_UNDO_ARENA_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   ts_endl
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

ts_xaddr_from_active_off:
      LDA   ts_offl
      STA   ts_xl
      CLC
      LDA   ts_base_xm
      ADC   ts_offh
      STA   ts_xm
      LDA   ts_base_xh
      ADC   #0
      STA   ts_xh
      RTS

; ---------------------------------------------------------------------
; Minimal XRAM window copy helpers
; ---------------------------------------------------------------------

ts_map_xram:
      LDA   XMC_WINCTL
      ORA   #TEXTSVC_WIN3_ENABLE
      STA   XMC_WINCTL
      STZ   WIN3_LO
      LDA   ts_xm
      STA   WIN3_MI
      LDA   ts_xh
      STA   WIN3_HI
      RTS

ts_advance_xram:
      INC   ts_xl
      BNE   @done
      INC   ts_xm
      BNE   @remap
      INC   ts_xh
@remap:
      JSR   ts_map_xram
@done:
      RTS

ts_advance_ram:
      INC   ts_raml
      BNE   @done
      INC   ts_ramh
@done:
      RTS

ts_dec_worklen:
      LDA   ts_worklenl
      BNE   :+
      DEC   ts_worklenh
:     DEC   ts_worklenl
      RTS

ts_copy_ram_to_xram:
      LDA   ts_worklenl
      ORA   ts_worklenh
      BEQ   @done
      JSR   ts_map_xram
@loop:
      LDY   #0
      LDA   (ts_raml),Y
      LDX   ts_xl
      STA   WIN3_BASE,X
      JSR   ts_advance_ram
      JSR   ts_advance_xram
      JSR   ts_dec_worklen
      LDA   ts_worklenl
      ORA   ts_worklenh
      BNE   @loop
@done:
      RTS

ts_copy_xram_to_ram:
      LDA   ts_worklenl
      ORA   ts_worklenh
      BEQ   @done
      JSR   ts_map_xram
@loop:
      LDX   ts_xl
      LDA   WIN3_BASE,X
      LDY   #0
      STA   (ts_raml),Y
      JSR   ts_advance_ram
      JSR   ts_advance_xram
      JSR   ts_dec_worklen
      LDA   ts_worklenl
      ORA   ts_worklenh
      BNE   @loop
@done:
      RTS

.endif
