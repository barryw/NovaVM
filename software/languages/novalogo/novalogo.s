; NovaLogo v1.0 — Logo interpreter for the Nova 6502 platform
; Task 1.4: line input buffer with backspace handling

      .include "nova.inc"
      .include "libabi.inc"
      .include "libgraphics.inc"   ; GFN_* ids for lib_call(GRAPHICS) routing (self-guards + re-includes libabi.inc)
      .include "libsystem.inc"     ; SYS_FN_* ids for lib_call(SYSTEM) routing (timing)
      .include "libeditor.inc"     ; EDITOR_FN_* ids for lib_call(EDITOR) routing
      .include "libsound.inc"      ; SND_* ids for lib_call(SOUND) routing (tone/noise/volume)
      .include "libturtle.inc"     ; MODULE_ID_TURTLE ($07) + TUR_OP/TOP_* for the turtle adapter
      .include "libfiles.inc"      ; MODULE_ID_FILES ($04) + FILE_* ids for SAVE/LOAD/CATALOG
      .include "replline.inc"      ; shared REPL line reader ABI
      .include "ext_iface.inc"
      .include "heap.s"
      .include "tokens.s"
      .include "tokenizer.s"
      .include "eval.s"
      .include "builtins.s"
      .include "lists.s"
      .include "vars.s"
      .include "procedures.s"
      .include "gc.s"
      .export repl_line_complete   ; completion hook consumed by replline.o

; =====================================================================
; CODE segment — cold start and main loop
; =====================================================================
      .segment "CODE"

cold_start:
      ; Tell the resident paged-library loader which ROM bank to restore after
      ; a module call returns (lib_call swaps back to LIB_HOME_BANK).
      LDA   #ROMSWAP_LOGO
      STA   LIB_HOME_BANK

      ; NovaLogo has no extension ROM any more (Phase B): every hardware command
      ; routes through a paged module. Seed LIB_RESIDENT=MODULE_ID_NONE so the
      ; first lib_call to each module MISSES and pages it in; bank 1 holds nothing
      ; runtime-owned at boot.
      LDA   #MODULE_ID_NONE
      STA   LIB_RESIDENT

      LDA   #VCMD_GCLS
      STA   VGC_CMD

      ; Zero NovaLogo turtle/graphics state ($9F00..$9F1F) so each session starts
      ; in a known mode. The extension pins its turtle state here (extension.s
      ; TURTLE_STATE_BASE=$9F00); RAM is stale across soft-reboot on HW, and a
      ; stale graphics-mode/inited flag breaks auto-switch and re-init.
      LDX   #$1F
@zero_turtle:
      STZ   $9F00,X
      DEX
      BPL   @zero_turtle

      JSR   heap_init
      JSR   var_init
      JSR   proc_init
      JSR   repl_init            ; clear the command-history ring
      STZ   catch_active
      STZ   VGC_CURSEN
      JSR   print_banner
      JSR   print_prompt

main_loop:
      JSR   repl_read_line
      STZ   VGC_CURSEN
      JSR   tokenize_line
      JSR   check_to_command
      BCS   @was_to               ; carry set = TO was handled
      JSR   eval_line
@was_to:
      ; Reclaim dead heap objects (tokens, list cells, etc.)
      JSR   gc_collect
      JSR   print_prompt
      BRA   main_loop

; ---------------------------------------------------------------------
; check_to_command — if the first token is TO / EDIT / ED, handle it as a
;   procedure-definition command (TO collects a new proc; EDIT/ED reopen an
;   existing one in the editor).
;   Output: carry set = handled, carry clear = not a definition command.
;   Clobbers: A, Y, ptr_lo/hi, eval_cur
; ---------------------------------------------------------------------
check_to_command:
      LDA   tok_head_lo
      ORA   tok_head_hi
      BEQ   @not_to

      ; First token must be TOK_WORD
      LDA   tok_head_lo
      STA   ptr_lo
      LDA   tok_head_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_WORD
      BNE   @not_to

      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y           ; word length
      CMP   #2
      BEQ   @len2
      CMP   #4
      BEQ   @len4
      BRA   @not_to

@len2:
      LDY   #TOK_PAYLOAD+1
      LDA   (ptr_lo),Y
      CMP   #'T'
      BNE   @not_to
      INY
      LDA   (ptr_lo),Y
      CMP   #'O'
      BNE   @not_to
      ; "TO" — collect a new procedure
      JSR   @set_cur
      JSR   proc_collect
      SEC
      RTS
@len4:
      LDY   #TOK_PAYLOAD+1
      LDA   (ptr_lo),Y
      CMP   #'E'
      BNE   @not_to
      INY
      LDA   (ptr_lo),Y
      CMP   #'D'
      BNE   @not_to
      INY
      LDA   (ptr_lo),Y
      CMP   #'I'
      BNE   @not_to
      INY
      LDA   (ptr_lo),Y
      CMP   #'T'
      BNE   @not_to
      ; "EDIT" — edit
      JSR   @set_cur
      JSR   proc_edit
      SEC
      RTS

@not_to:
      CLC
      RTS

@set_cur:
      ; eval_cur = first token, then advance past it to the name token
      LDA   tok_head_lo
      STA   eval_cur_lo
      LDA   tok_head_hi
      STA   eval_cur_hi
      JMP   eval_advance         ; ends with RTS

; ---------------------------------------------------------------------
; repl_line_complete — NovaLogo's continuation hook for the shared REPL reader.
;   repl_read_line (in replline.s) calls this on CR. Returns Z=1 when the line is
;   complete; NovaLogo keeps reading while '[' / '(' brackets are still open so a
;   multi-line statement can be entered right at the prompt.
; ---------------------------------------------------------------------
repl_line_complete:
      JMP   line_bracket_depth   ; depth 0 (Z set) = complete; line_bracket_depth ends with RTS

; ---------------------------------------------------------------------
; line_bracket_depth — net count of unclosed '[' and '(' in input_buf[0..buf_idx),
;   ignoring text inside ';' comments. Returns depth in A (Z set if balanced).
;   Clobbers: A, X, Y.
; ---------------------------------------------------------------------
line_bracket_depth:
      LDX   #0                 ; scan index
      LDY   #0                 ; depth
@bd_loop:
      CPX   buf_idx
      BCS   @bd_done
      LDA   input_buf,X
      CMP   #';'
      BEQ   @bd_comment
      CMP   #'['
      BEQ   @bd_inc
      CMP   #'('
      BEQ   @bd_inc
      CMP   #']'
      BEQ   @bd_dec
      CMP   #')'
      BEQ   @bd_dec
@bd_next:
      INX
      BRA   @bd_loop
@bd_inc:
      INY
      BRA   @bd_next
@bd_dec:
      CPY   #0
      BEQ   @bd_next           ; floor at 0
      DEY
      BRA   @bd_next
@bd_comment:
      INX                      ; skip ';' and everything up to a newline
      CPX   buf_idx
      BCS   @bd_done
      LDA   input_buf,X
      CMP   #$0A
      BNE   @bd_comment
      BRA   @bd_next           ; resume scanning after the newline
@bd_done:
      TYA                      ; depth -> A, sets Z if zero
      RTS

; ---------------------------------------------------------------------
; print_prompt — prints "? " to the screen
; ---------------------------------------------------------------------
print_prompt:
      JSR   print_inl
      .byte "? ", 0
      LDA   #$01
      STA   VGC_CURSEN
      RTS

; ---------------------------------------------------------------------
; print_banner — prints title and current available heap bytes
; ---------------------------------------------------------------------
print_banner:
      JSR   print_inl
      .byte "Nova LOGO v1.0", $0D, $0A, 0

      SEC
      LDA   #<HEAP_END
      SBC   heap_ptr
      STA   eval_val_lo
      LDA   #>HEAP_END
      SBC   heap_ptr+1
      STA   eval_val_hi
      SEC
      LDA   eval_val_lo
      SBC   #GC_HDR_BYTES
      STA   eval_val_lo
      LDA   eval_val_hi
      SBC   #0
      STA   eval_val_hi
      STZ   eval_val_frac
      JSR   print_uint16          ; unsigned: free bytes can exceed 32767

      JSR   print_inl
      .byte " BYTES FREE", $0D, $0A, $0D, $0A, 0
      RTS

; =====================================================================
; RODATA segment — string constants
; =====================================================================
      .segment "RODATA"

; =====================================================================
; MONITOR segment — reset handler at $FFD7
; =====================================================================
      .segment "MONITOR"
      .org  $FFD7

RES_vec:
      CLD                     ; clear decimal mode
      LDX   #$FF              ; empty stack
      TXS                     ; set the stack
      JMP   cold_start

; IRQ/NMI stubs
NMI_stub:
      RTI

IRQ_stub:
      RTI

; =====================================================================
; VECTORS segment — 6502 hardware vectors at $FFFA
; =====================================================================
      .segment "VECTORS"
      .org  $FFFA

      .word NMI_stub           ; $FFFA — NMI vector
      .word RES_vec            ; $FFFC — RESET vector
      .word IRQ_stub           ; $FFFE — IRQ vector
