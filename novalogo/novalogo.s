; NovaLogo v1.0 — Logo interpreter for the Nova 6502 platform
; Task 1.4: line input buffer with backspace handling

      .include "nova.inc"
      .include "libabi.inc"
      .include "libgraphics.inc"   ; GFN_* ids for lib_call(GRAPHICS) routing (self-guards + re-includes libabi.inc)
      .include "libsystem.inc"     ; SYS_FN_* ids for lib_call(SYSTEM) routing (editor + timing)
      .include "libsound.inc"      ; SND_* ids for lib_call(SOUND) routing (tone/noise/volume)
      .include "libturtle.inc"     ; MODULE_ID_TURTLE ($07) + TUR_OP/TOP_* for the turtle adapter
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

; =====================================================================
; ZEROPAGE segment — interpreter zero-page variables
; =====================================================================
      .segment "ZEROPAGE"

buf_idx:      .res 1          ; current position in input_buf

; =====================================================================
; BSS segment — runtime buffers
; =====================================================================
      .segment "BSS"

input_buf:    .res 128        ; input line buffer

; =====================================================================
; Command-line history (shell-style up/down recall at the "?" prompt).
; Lives in the FREE $9800-$9BFF region (BSS ends $1304, heap ends $9800, turtle
; state is $9C00+; nothing else uses this band). All state is hardcoded here like
; the turtle state at $9F00 — a plain BSS .res would land in the heap-overlap zone.
;   header $9800-$980F, live-line $9810-$988F (128), 6 slots $9890-$9B8F (6*128).
; Arrow key codes (queued by EmulatorCanvas): up=$1E down=$1F left=$1C right=$1D.
; =====================================================================
HIST_BASE     = $9800
HIST_COUNT    = HIST_BASE+0     ; entries stored, 0..HIST_MAX
HIST_HEAD     = HIST_BASE+1     ; ring index where the NEXT entry will be written
HIST_NAV      = HIST_BASE+2     ; 0 = live (in-progress) line, 1..COUNT = recalled
HIST_SX       = HIST_BASE+3     ; input-start cursor X (captured after the prompt)
HIST_SY       = HIST_BASE+4     ; input-start cursor Y
HIST_OLDLEN   = HIST_BASE+5     ; on-screen length to clear on a redraw
HIST_K        = HIST_BASE+6     ; scratch: entry ordinal
HIST_IDX      = HIST_BASE+7     ; scratch: ring slot index
HIST_MAX      = 6
HIST_LIVE     = HIST_BASE+$10   ; saved in-progress line ($9810-$988F)
HIST_SLOTS    = HIST_BASE+$90   ; slot k at HIST_SLOTS + idx*128 ($9890-$9B8F)

KEY_UP        = $1E
KEY_DOWN      = $1F
KEY_LEFT      = $1C
KEY_RIGHT     = $1D

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
      STZ   HIST_COUNT           ; empty command history
      STZ   HIST_HEAD
      STZ   HIST_NAV
      STZ   catch_active
      STZ   VGC_CURSEN
      JSR   print_banner
      JSR   print_prompt

main_loop:
      JSR   read_line
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
; read_line — read a line of input into input_buf, null-terminated
;   On return: input_buf contains the line, buf_idx = length
; ---------------------------------------------------------------------
read_line:
      STZ   buf_idx            ; reset buffer index to 0
      LDA   #$01
      STA   VGC_CURSEN
      ; Remember where the typed line starts (just after the prompt) so history
      ; recall can repaint the line from a known origin, and start at the live line.
      LDA   VGC_CURSX
      STA   HIST_SX
      LDA   VGC_CURSY
      STA   HIST_SY
      STZ   HIST_NAV

@poll:
      LDA   VGC_CHARIN         ; poll keyboard
      BEQ   @poll              ; no key waiting — spin

      ; --- handle CR ($0D) ---
      CMP   #$0D
      BNE   @not_cr
      ; End of a typed line. If brackets/parens are still open, keep reading so
      ; a multi-line statement can be entered right at the prompt.
      JSR   line_bracket_depth
      BEQ   @cr_finish
      LDX   buf_idx
      CPX   #120
      BCS   @cr_finish         ; buffer nearly full — stop accumulating
      LDA   #$0A               ; newline separator (tokenizer treats it as space)
      STA   input_buf,X
      INC   buf_idx
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      LDA   #'~'               ; continuation prompt
      STA   VGC_CHAROUT
      LDA   #' '
      STA   VGC_CHAROUT
      JMP   @poll
@cr_finish:
      LDX   buf_idx
      STZ   input_buf,X        ; null-terminate
      JSR   hist_save          ; record the entered line in the history ring
      LDA   #$0D
      STA   VGC_CHAROUT        ; carriage return
      LDA   #$0A
      STA   VGC_CHAROUT        ; line feed
      RTS

@not_cr:
      ; --- command history (up/down) + cursor keys ---
      CMP   #KEY_UP
      BEQ   @key_up
      CMP   #KEY_DOWN
      BEQ   @key_down
      CMP   #KEY_LEFT          ; left/right: swallow (no inline editing yet) so
      BEQ   @poll              ; they don't corrupt the line or move the cursor
      CMP   #KEY_RIGHT
      BEQ   @poll

      ; --- handle backspace ($08, $14, $7F) ---
      CMP   #$08
      BEQ   @backspace
      CMP   #$14
      BEQ   @backspace
      CMP   #$7F
      BEQ   @backspace

      ; --- buffer full? ---
      LDX   buf_idx
      CPX   #127
      BCS   @poll              ; at capacity — ignore keystroke

      ; --- store and echo printable character ---
      STA   input_buf,X        ; store in buffer
      STA   VGC_CHAROUT        ; echo to screen
      INC   buf_idx
      BRA   @poll

@backspace:
      LDX   buf_idx
      BEQ   @poll              ; nothing to delete — ignore
      DEX
      STX   buf_idx
      LDA   #$08
      STA   VGC_CHAROUT        ; move cursor left and erase
      BRA   @poll

@key_up:
      JSR   hist_up
      JMP   @poll
@key_down:
      JSR   hist_down
      JMP   @poll

; ---------------------------------------------------------------------
; Command-history subroutines. The ring stores up to HIST_MAX entered lines.
;   hist_save        — append input_buf to the ring (called on CR; skips blanks).
;   hist_up/hist_down— recall older/newer (or back to the live in-progress line).
;   hist_show        — repaint the prompt line with input_buf from the saved origin.
;   hist_slot_ptr    — ptr_lo/hi = address of the k-th-newest slot (k in HIST_K).
;   hist_copy_in     — copy a NUL-terminated line at ptr -> input_buf, set buf_idx.
; ---------------------------------------------------------------------
; hist_save: store input_buf (buf_idx bytes) into slot[HEAD], advance the ring.
hist_save:
      LDA   buf_idx
      BEQ   @hs_done           ; don't record empty lines
      LDA   HIST_HEAD          ; ptr = HIST_SLOTS + HEAD*128
      STA   HIST_IDX
      JSR   hist_slot_addr
      LDY   #0
@hs_copy:
      CPY   buf_idx
      BCS   @hs_term
      LDA   input_buf,Y
      STA   (ptr_lo),Y
      INY
      BRA   @hs_copy
@hs_term:
      LDA   #0
      STA   (ptr_lo),Y         ; NUL-terminate the stored line
      ; advance HEAD (mod HIST_MAX) and bump COUNT (saturating at HIST_MAX)
      LDA   HIST_HEAD
      CLC
      ADC   #1
      CMP   #HIST_MAX
      BCC   @hs_sethead
      LDA   #0
@hs_sethead:
      STA   HIST_HEAD
      LDA   HIST_COUNT
      CMP   #HIST_MAX
      BCS   @hs_done           ; already full — COUNT stays at HIST_MAX
      INC   HIST_COUNT
@hs_done:
      RTS

; hist_up: recall an older entry (toward the oldest). At the live line, stash it
;   first so hist_down can restore it.
hist_up:
      LDA   HIST_NAV
      CMP   HIST_COUNT
      BCS   @hu_done           ; NAV >= COUNT: nothing older
      LDA   buf_idx            ; current on-screen length -> blank count for redraw
      STA   HIST_OLDLEN
      LDA   HIST_NAV
      BNE   @hu_step           ; leaving the live line? save it once
      ; save the in-progress line into HIST_LIVE
      LDA   #<HIST_LIVE
      STA   ptr_lo
      LDA   #>HIST_LIVE
      STA   ptr_hi
      JSR   hist_copy_out
@hu_step:
      INC   HIST_NAV
      LDA   HIST_NAV
      STA   HIST_K
      JSR   hist_slot_ptr      ; ptr -> k-th newest entry
      JSR   hist_copy_in
      JMP   hist_show
@hu_done:
      RTS

; hist_down: recall a newer entry, or return to the live in-progress line.
hist_down:
      LDA   HIST_NAV
      BEQ   @hd_done           ; already at the live line
      LDA   buf_idx            ; current on-screen length -> blank count for redraw
      STA   HIST_OLDLEN
      DEC   HIST_NAV
      LDA   HIST_NAV
      BEQ   @hd_live           ; back at the live line -> restore it
      STA   HIST_K
      JSR   hist_slot_ptr
      JSR   hist_copy_in
      JMP   hist_show
@hd_live:
      LDA   #<HIST_LIVE
      STA   ptr_lo
      LDA   #>HIST_LIVE
      STA   ptr_hi
      JSR   hist_copy_in
      JMP   hist_show
@hd_done:
      RTS

; hist_show: repaint the prompt line. Park the cursor at the saved input origin,
;   blank the previously-shown text, then re-echo input_buf; cursor ends at the end.
hist_show:
      ; cursor -> input origin
      LDA   HIST_SX
      STA   VGC_CURSX
      LDA   HIST_SY
      STA   VGC_CURSY
      ; blank HIST_OLDLEN columns (the previously-shown text)
      LDX   HIST_OLDLEN
@show_blank:
      BEQ   @show_reset
      LDA   #' '
      STA   VGC_CHAROUT
      DEX
      BRA   @show_blank
@show_reset:
      LDA   HIST_SX            ; back to origin to print the recalled line
      STA   VGC_CURSX
      LDA   HIST_SY
      STA   VGC_CURSY
      LDX   #0
@show_print:
      CPX   buf_idx
      BCS   @show_done
      LDA   input_buf,X
      STA   VGC_CHAROUT
      INX
      BRA   @show_print
@show_done:
      RTS

; hist_copy_in: copy the NUL-terminated line at (ptr_lo) into input_buf; buf_idx=len.
hist_copy_in:
      LDY   #0
@ci_loop:
      LDA   (ptr_lo),Y
      BEQ   @ci_done
      STA   input_buf,Y
      INY
      CPY   #127
      BCC   @ci_loop
@ci_done:
      STY   buf_idx
      LDA   #0
      STA   input_buf,Y        ; keep input_buf NUL-terminated
      RTS

; hist_copy_out: copy input_buf (buf_idx bytes + NUL) to the line at (ptr_lo).
hist_copy_out:
      LDY   #0
@co_loop:
      CPY   buf_idx
      BCS   @co_term
      LDA   input_buf,Y
      STA   (ptr_lo),Y
      INY
      BRA   @co_loop
@co_term:
      LDA   #0
      STA   (ptr_lo),Y
      RTS

; hist_slot_ptr: ptr_lo/hi = address of the HIST_K-th newest entry (K in 1..COUNT).
;   idx = (HEAD - K) mod HIST_MAX.
hist_slot_ptr:
      SEC
      LDA   HIST_HEAD
      SBC   HIST_K
      BPL   @sp_ok             ; HEAD-K >= 0
      CLC
      ADC   #HIST_MAX          ; wrapped negative -> + HIST_MAX
@sp_ok:
      STA   HIST_IDX
      ; fall through to compute the address from HIST_IDX
; hist_slot_addr: ptr_lo/hi = HIST_SLOTS + HIST_IDX*128.
hist_slot_addr:
      LDA   HIST_IDX
      LSR   A                  ; A = idx>>1, carry = idx&1
      STA   HIST_K             ; reuse K as scratch high part (restored by callers)
      LDA   #0
      ROR   A                  ; carry -> bit7: (idx&1)?$80:0
      CLC
      ADC   #<HIST_SLOTS
      STA   ptr_lo
      LDA   HIST_K
      ADC   #>HIST_SLOTS
      STA   ptr_hi
      RTS

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
      JSR   print_uint16

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
