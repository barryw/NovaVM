; Nova fixed-address overlay manager implementation.

.include "overlay.inc"

.ifndef OVERLAY_IMPLEMENTATION_INCLUDED
OVERLAY_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

OVL_NAMEPTR_L:         .res 1
OVL_NAMEPTR_H:         .res 1
OVL_NAMELEN:           .res 1
OVL_LOADL:             .res 1
OVL_LOADH:             .res 1
OVL_MAXLENL:           .res 1
OVL_MAXLENH:           .res 1
OVL_RESULT:            .res 1
OVL_LOADED:            .res 1
OVL_ACTIVE_LOADL:      .res 1
OVL_ACTIVE_LOADH:      .res 1
OVL_ACTIVE_SIZEL:      .res 1
OVL_ACTIVE_SIZEH:      .res 1
OVL_BSSL:              .res 1
OVL_BSSH:              .res 1
OVL_ENTRY_INITL:       .res 1
OVL_ENTRY_INITH:       .res 1
OVL_ENTRY_MAINL:       .res 1
OVL_ENTRY_MAINH:       .res 1
OVL_ENTRY_TICKL:       .res 1
OVL_ENTRY_TICKH:       .res 1
OVL_ENTRY_UNLOADL:     .res 1
OVL_ENTRY_UNLOADH:     .res 1
OVL_MODULE_IDL:        .res 1
OVL_MODULE_IDH:        .res 1
OVL_MODULE_VERSIONL:   .res 1
OVL_MODULE_VERSIONH:   .res 1
overlay_call_ptr:      .res 2
overlay_header:        .res OVL_HEADER_SIZE

      .segment "CODE"

      .export overlay_ok
      .export overlay_set_error
      .export overlay_clear_active
      .export overlay_load_fixed
      .export overlay_unload
      .export overlay_call_init
      .export overlay_call_main
      .export overlay_call_tick

; @label OVERLAY.OK
; @kind routine
; @symbol overlay_ok
; @summary Store OVL.OK and return A=0.
overlay_ok:
      STZ   OVL_RESULT
      LDA   #OVL_OK
      RTS

; @label OVERLAY.SET_ERROR
; @kind routine
; @symbol overlay_set_error
; @summary Store a detailed overlay error code.
; @in A: OVL_ERR_* code.
; @out A: 1.
overlay_set_error:
      STA   OVL_RESULT
      LDA   #OVL_ERR
      RTS

; @label OVERLAY.CLEAR_ACTIVE
; @kind routine
; @symbol overlay_clear_active
; @summary Forget the active overlay metadata without touching the RAM slot.
overlay_clear_active:
      STZ   OVL_LOADED
      STZ   OVL_ACTIVE_LOADL
      STZ   OVL_ACTIVE_LOADH
      STZ   OVL_ACTIVE_SIZEL
      STZ   OVL_ACTIVE_SIZEH
      STZ   OVL_BSSL
      STZ   OVL_BSSH
      STZ   OVL_ENTRY_INITL
      STZ   OVL_ENTRY_INITH
      STZ   OVL_ENTRY_MAINL
      STZ   OVL_ENTRY_MAINH
      STZ   OVL_ENTRY_TICKL
      STZ   OVL_ENTRY_TICKH
      STZ   OVL_ENTRY_UNLOADL
      STZ   OVL_ENTRY_UNLOADH
      STZ   OVL_MODULE_IDL
      STZ   OVL_MODULE_IDH
      STZ   OVL_MODULE_VERSIONL
      STZ   OVL_MODULE_VERSIONH
      JMP   overlay_ok

; @label OVERLAY.LOAD_FIXED
; @kind routine
; @symbol overlay_load_fixed
; @summary Load and activate a NOVO fixed-address overlay into CPU RAM.
; @requires OVL_NAMEPTR_L OVL_NAMEPTR_H OVL_NAMELEN OVL_LOADL OVL_LOADH OVL_MAXLENL OVL_MAXLENH
; @out A: 0 on success, 1 on error. OVL_RESULT holds OVL_ERR_* detail.
overlay_load_fixed:
      JSR   overlay_unload
      BEQ   :+
      RTS
:
      JSR   overlay_load_header
      BEQ   :+
      RTS
:
      JSR   overlay_validate_header
      BEQ   :+
      RTS
:
      JSR   overlay_load_payload
      BEQ   :+
      RTS
:
      JSR   overlay_clear_bss
      BEQ   :+
      RTS
:
      JSR   overlay_activate_header
      JMP   overlay_ok

overlay_load_header:
      JSR   overlay_prepare_name
      STZ   PAGER_FILEL
      STZ   PAGER_FILEM
      STZ   PAGER_FILEH
      LDA   #<overlay_header
      STA   PAGER_ADDRL
      LDA   #>overlay_header
      STA   PAGER_ADDRM
      STZ   PAGER_ADDRH
      LDA   #OVL_HEADER_SIZE
      STA   PAGER_LENL
      STZ   PAGER_LENH
      LDA   #PAGER_TARGET_RAM
      STA   PAGER_TARGET
      JSR   pager_load_file_page
      BEQ   @ok
      LDA   #OVL_ERR_IO
      JMP   overlay_set_error
@ok:
      JMP   overlay_ok

overlay_load_payload:
      JSR   overlay_prepare_name
      LDA   #OVL_HEADER_SIZE
      STA   PAGER_FILEL
      STZ   PAGER_FILEM
      STZ   PAGER_FILEH
      LDA   OVL_LOADL
      STA   PAGER_ADDRL
      LDA   OVL_LOADH
      STA   PAGER_ADDRM
      STZ   PAGER_ADDRH
      LDA   overlay_header + OVL_HDR_SIZEL
      STA   PAGER_LENL
      LDA   overlay_header + OVL_HDR_SIZEH
      STA   PAGER_LENH
      LDA   #PAGER_TARGET_RAM
      STA   PAGER_TARGET
      JSR   pager_load_file_page
      BEQ   @ok
      LDA   #OVL_ERR_IO
      JMP   overlay_set_error
@ok:
      JMP   overlay_ok

overlay_prepare_name:
      LDA   OVL_NAMELEN
      STA   PAGER_NAMELEN
      LDA   OVL_NAMEPTR_L
      STA   PAGER_NAMEPTR_L
      LDA   OVL_NAMEPTR_H
      STA   PAGER_NAMEPTR_H
      RTS

overlay_validate_header:
      LDA   overlay_header + OVL_HDR_MAGIC0
      CMP   #OVL_MAGIC0
      BNE   @bad_magic
      LDA   overlay_header + OVL_HDR_MAGIC1
      CMP   #OVL_MAGIC1
      BNE   @bad_magic
      LDA   overlay_header + OVL_HDR_MAGIC2
      CMP   #OVL_MAGIC2
      BNE   @bad_magic
      LDA   overlay_header + OVL_HDR_MAGIC3
      CMP   #OVL_MAGIC3
      BNE   @bad_magic

      LDA   overlay_header + OVL_HDR_FORMAT
      CMP   #OVL_FORMAT_VERSION
      BNE   @bad_version
      LDA   overlay_header + OVL_HDR_ABI_MAJOR
      CMP   #OVL_ABI_MAJOR
      BNE   @bad_version

      LDA   overlay_header + OVL_HDR_LOADL
      CMP   OVL_LOADL
      BNE   @bad_load
      LDA   overlay_header + OVL_HDR_LOADH
      CMP   OVL_LOADH
      BNE   @bad_load

      LDA   overlay_header + OVL_HDR_SIZEL
      ORA   overlay_header + OVL_HDR_SIZEH
      BEQ   @bad_size
      JSR   overlay_total_fits_slot
      BNE   @bad_size
      JSR   overlay_total_fits_ram
      BNE   @bad_size
      JMP   overlay_ok

@bad_magic:
      LDA   #OVL_ERR_MAGIC
      JMP   overlay_set_error
@bad_version:
      LDA   #OVL_ERR_VERSION
      JMP   overlay_set_error
@bad_load:
      LDA   #OVL_ERR_LOAD_ADDR
      JMP   overlay_set_error
@bad_size:
      LDA   #OVL_ERR_SIZE
      JMP   overlay_set_error

overlay_compute_total_size:
      LDA   overlay_header + OVL_HDR_SIZEL
      CLC
      ADC   overlay_header + OVL_HDR_BSSL
      STA   NVR6L
      LDA   overlay_header + OVL_HDR_SIZEH
      ADC   overlay_header + OVL_HDR_BSSH
      STA   NVR6H
      BCS   @bad
      LDA   #$00
      RTS
@bad:
      LDA   #$01
      RTS

; Return A=0 if payload+BSS size <= OVL_MAXLEN, else A=1.
overlay_total_fits_slot:
      JSR   overlay_compute_total_size
      BNE   @bad
      LDA   NVR6H
      CMP   OVL_MAXLENH
      BCC   @ok
      BNE   @bad
      LDA   NVR6L
      CMP   OVL_MAXLENL
      BCC   @ok
      BEQ   @ok
@bad:
      LDA   #$01
      RTS
@ok:
      LDA   #$00
      RTS

; Return A=0 if OVL_LOAD + payload+BSS size does not wrap 64K, else A=1.
overlay_total_fits_ram:
      JSR   overlay_compute_total_size
      BNE   @bad
      LDA   OVL_LOADL
      CLC
      ADC   NVR6L
      LDA   OVL_LOADH
      ADC   NVR6H
      BCS   @bad
      LDA   #$00
      RTS
@bad:
      LDA   #$01
      RTS

overlay_clear_bss:
      LDA   overlay_header + OVL_HDR_BSSL
      ORA   overlay_header + OVL_HDR_BSSH
      BNE   :+
      JMP   overlay_ok
:
      LDA   OVL_LOADL
      CLC
      ADC   overlay_header + OVL_HDR_SIZEL
      STA   NVR5L
      LDA   OVL_LOADH
      ADC   overlay_header + OVL_HDR_SIZEH
      STA   NVR5H
      LDA   overlay_header + OVL_HDR_BSSL
      STA   NVR6L
      LDA   overlay_header + OVL_HDR_BSSH
      STA   NVR6H
@loop:
      LDA   NVR6L
      ORA   NVR6H
      BEQ   @done
      LDA   #$00
      STA   (NVR5L)
      INC   NVR5L
      BNE   :+
      INC   NVR5H
:
      LDA   NVR6L
      BNE   :+
      DEC   NVR6H
:
      DEC   NVR6L
      BRA   @loop
@done:
      JMP   overlay_ok

overlay_activate_header:
      LDA   overlay_header + OVL_HDR_LOADL
      STA   OVL_ACTIVE_LOADL
      LDA   overlay_header + OVL_HDR_LOADH
      STA   OVL_ACTIVE_LOADH
      LDA   overlay_header + OVL_HDR_SIZEL
      STA   OVL_ACTIVE_SIZEL
      LDA   overlay_header + OVL_HDR_SIZEH
      STA   OVL_ACTIVE_SIZEH
      LDA   overlay_header + OVL_HDR_BSSL
      STA   OVL_BSSL
      LDA   overlay_header + OVL_HDR_BSSH
      STA   OVL_BSSH
      LDA   overlay_header + OVL_HDR_INITL
      STA   OVL_ENTRY_INITL
      LDA   overlay_header + OVL_HDR_INITH
      STA   OVL_ENTRY_INITH
      LDA   overlay_header + OVL_HDR_MAINL
      STA   OVL_ENTRY_MAINL
      LDA   overlay_header + OVL_HDR_MAINH
      STA   OVL_ENTRY_MAINH
      LDA   overlay_header + OVL_HDR_TICKL
      STA   OVL_ENTRY_TICKL
      LDA   overlay_header + OVL_HDR_TICKH
      STA   OVL_ENTRY_TICKH
      LDA   overlay_header + OVL_HDR_UNLOADL
      STA   OVL_ENTRY_UNLOADL
      LDA   overlay_header + OVL_HDR_UNLOADH
      STA   OVL_ENTRY_UNLOADH
      LDA   overlay_header + OVL_HDR_MODULE_IDL
      STA   OVL_MODULE_IDL
      LDA   overlay_header + OVL_HDR_MODULE_IDH
      STA   OVL_MODULE_IDH
      LDA   overlay_header + OVL_HDR_VERSIONL
      STA   OVL_MODULE_VERSIONL
      LDA   overlay_header + OVL_HDR_VERSIONH
      STA   OVL_MODULE_VERSIONH
      LDA   #$01
      STA   OVL_LOADED
      RTS

; @label OVERLAY.UNLOAD
; @kind routine
; @symbol overlay_unload
; @summary Call the active overlay unload entry if present, then clear metadata.
; @out A: 0 on success, 1 on unload-entry error.
overlay_unload:
      LDA   OVL_LOADED
      BEQ   @clear
      JSR   overlay_call_unload_optional
      PHA
      JSR   overlay_clear_active
      PLA
      CMP   #OVL_OK
      BNE   @entry_error
      JMP   overlay_ok
@entry_error:
      LDA   #OVL_ERR_ENTRY
      JMP   overlay_set_error
@clear:
      JMP   overlay_clear_active

; @label OVERLAY.CALL_INIT
; @kind routine
; @symbol overlay_call_init
; @summary Call the active overlay init entry if one is declared.
overlay_call_init:
      LDA   OVL_ENTRY_INITL
      LDX   OVL_ENTRY_INITH
      LDY   #$00
      JMP   overlay_call_optional

; @label OVERLAY.CALL_MAIN
; @kind routine
; @symbol overlay_call_main
; @summary Call the active overlay main entry. Missing main is an error.
overlay_call_main:
      LDA   OVL_ENTRY_MAINL
      LDX   OVL_ENTRY_MAINH
      LDY   #$01
      JMP   overlay_call_entry

; @label OVERLAY.CALL_TICK
; @kind routine
; @symbol overlay_call_tick
; @summary Call the active overlay tick entry if one is declared.
overlay_call_tick:
      LDA   OVL_ENTRY_TICKL
      LDX   OVL_ENTRY_TICKH
      LDY   #$00
      JMP   overlay_call_optional

overlay_call_unload_optional:
      LDA   OVL_ENTRY_UNLOADL
      LDX   OVL_ENTRY_UNLOADH
      LDY   #$00
      JMP   overlay_call_optional

overlay_call_optional:
      JMP   overlay_call_entry

; Input: A/X = entry pointer, Y = 1 if pointer is required.
overlay_call_entry:
      STA   overlay_call_ptr
      STX   overlay_call_ptr + 1
      LDA   OVL_LOADED
      BNE   @loaded
      LDA   #OVL_ERR_NOT_LOADED
      JMP   overlay_set_error
@loaded:
      LDA   overlay_call_ptr
      ORA   overlay_call_ptr + 1
      BNE   @call
      CPY   #$00
      BNE   @missing_required
      JMP   overlay_ok
@missing_required:
      LDA   #OVL_ERR_NO_ENTRY
      JMP   overlay_set_error
@call:
      LDA   #>(@returned - 1)
      PHA
      LDA   #<(@returned - 1)
      PHA
      JMP   (overlay_call_ptr)
@returned:
      CMP   #OVL_OK
      BNE   @entry_error
      JMP   overlay_ok
@entry_error:
      LDA   #OVL_ERR_ENTRY
      JMP   overlay_set_error

.endif
