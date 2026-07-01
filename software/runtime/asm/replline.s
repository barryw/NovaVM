; =====================================================================
; replline.s — shared REPL line reader with shell-style up/down history
;
; Provides a generic prompt-line reader: key poll, backspace, printable echo, a
; 6-deep history ring with Up/Down recall, and single-row line repaint.
; Forth, Pascal, and the assembler dev env all inherit this reader. BASIC is
; excluded — it uses EhBASIC's own ROM input routine.
;
; The ONE runtime-specific decision — "is the typed line complete, or keep reading
; for multi-line entry?" — is delegated to a hook the host runtime MUST define:
;
;     repl_line_complete:    ; returns Z=1 when the line is complete (stop reading)
;
;   NovaLogo:           repl_line_complete: JMP line_bracket_depth  ; open [ ( -> keep reading
;   Forth/Pascal/asm:   repl_line_complete: LDA #0  : RTS           ; always complete
;
; Host-runtime contract (must be in scope before this include):
;   - nova.inc symbols: VGC_CHARIN, VGC_CHAROUT, VGC_CURSX, VGC_CURSY, VGC_CURSEN
;   - call  repl_init  once at cold start, then  repl_read_line  per input line.
;
; Shared contract symbols exported here: input_buf (128) + buf_idx (ZP) hold the
; line the reader fills. A single resident runtime owns them at a time.
; =====================================================================

.include "replline.inc"
.include "nova.inc"

.ifndef REPLLINE_IMPLEMENTATION_INCLUDED
REPLLINE_IMPLEMENTATION_INCLUDED = 1

      .import repl_line_complete

; =====================================================================
; ZEROPAGE segment — reader cursor and private history scratch pointer
; =====================================================================
      .segment "ZEROPAGE"

buf_idx:      .res 1          ; current position in input_buf
REPL_PTRL     = buf_idx       ; private history pointer low during copy helpers
REPL_PTRH:    .res 1

; =====================================================================
; Shared line buffer + command-line history — ALL hardcoded into the FREE
; $9800-$9BFF band. A plain BSS .res for input_buf lands in the runtime's
; heap/dictionary growth zone (novaforth's dict grows toward DICT_LIMIT=$9800),
; where defining words overwrite the line buffer mid-read and corrupt EVERY
; parse (the regression this fixes). So input_buf lives in the band too, beside
; the history ring. On NovaLogo: BSS ends $1304, heap ends $9800, turtle is
; $9C00+; nothing else uses this band. Any single resident runtime adopting this
; reader must keep $9800-$9BFF free.
;   header   $9800-$980F
;   input_buf $9810-$988F (128)
;   live-line $9890-$990F (128)
;   5 slots   $9910-$9B8F (5*128)   [one slot traded for input_buf; band is 1KB]
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
input_buf     = HIST_BASE+$10   ; line buffer ($9810-$988F, 128) -- in-band, not BSS
HIST_MAX      = 5               ; one slot traded for input_buf (1KB band)
HIST_LIVE     = HIST_BASE+$90   ; saved in-progress line ($9890-$990F)
HIST_SLOTS    = HIST_BASE+$110  ; slot k at HIST_SLOTS + idx*128 ($9910-$9B8F)

KEY_UP        = $1E
KEY_DOWN      = $1F
KEY_LEFT      = $1C
KEY_RIGHT     = $1D

; =====================================================================
; CODE segment — the reader and history ring
; =====================================================================
      .segment "CODE"

      .export repl_init
      .export repl_read_line
      .export input_buf
      .exportzp buf_idx

; ---------------------------------------------------------------------
; repl_init — clear the history-ring header. Call once at cold start.
; ---------------------------------------------------------------------
repl_init:
      STZ   HIST_COUNT           ; empty command history
      STZ   HIST_HEAD
      STZ   HIST_NAV
      RTS

; ---------------------------------------------------------------------
; repl_read_line — read a line of input into input_buf, null-terminated
;   On return: input_buf contains the line, buf_idx = length
; ---------------------------------------------------------------------
repl_read_line:
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
      ; End of a typed line. The host runtime decides if it's complete; if it asks
      ; to keep reading, a multi-line statement can be entered right at the prompt.
      JSR   repl_line_complete
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
;   hist_slot_ptr    — REPL_PTR = address of the k-th-newest slot (k in HIST_K).
;   hist_copy_in     — copy a NUL-terminated line at REPL_PTR -> input_buf, set buf_idx.
; ---------------------------------------------------------------------
; hist_save: store input_buf (buf_idx bytes) into slot[HEAD], advance the ring.
hist_save:
      LDA   buf_idx
      BEQ   @hs_empty          ; don't record empty lines
      STA   HIST_OLDLEN        ; buf_idx aliases REPL_PTRL during pointer copies
      LDA   HIST_HEAD          ; ptr = HIST_SLOTS + HEAD*128
      STA   HIST_IDX
      JSR   hist_slot_addr
      LDY   #0
@hs_copy:
      CPY   HIST_OLDLEN
      BCS   @hs_term
      LDA   input_buf,Y
      STA   (REPL_PTRL),Y
      INY
      BRA   @hs_copy
@hs_term:
      LDA   #0
      STA   (REPL_PTRL),Y      ; NUL-terminate the stored line
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
      LDA   HIST_OLDLEN
      STA   buf_idx
@hs_empty:
      RTS

; hist_up: recall an older entry (toward the oldest). At the live line, stash it
;   first so hist_down can restore it.
; Every up/down ALWAYS ends in hist_show, even at a boundary (oldest / live line).
; The GUI moves the VGC cursor on the arrow key before the 6502 sees it, so a
; no-op return would leave the cursor displaced — repainting resets it to the line
; origin every time. Up stops at the oldest entry; Down stops at the (possibly
; empty) live line.
hist_up:
      LDA   buf_idx            ; current on-screen length -> blank count for redraw
      STA   HIST_OLDLEN
      LDA   HIST_NAV
      CMP   HIST_COUNT
      BCS   @hu_repaint        ; NAV >= COUNT: at the oldest -> repaint in place
      LDA   HIST_NAV
      BNE   @hu_step           ; leaving the live line? save it once
      ; save the in-progress line into HIST_LIVE
      LDA   #<HIST_LIVE
      STA   REPL_PTRL
      LDA   #>HIST_LIVE
      STA   REPL_PTRH
      JSR   hist_copy_out
@hu_step:
      INC   HIST_NAV
      LDA   HIST_NAV
      STA   HIST_K
      JSR   hist_slot_ptr      ; ptr -> k-th newest entry
      JSR   hist_copy_in
@hu_repaint:
      JMP   hist_show

; hist_down: recall a newer entry, or return to the live in-progress line.
hist_down:
      LDA   buf_idx            ; current on-screen length -> blank count for redraw
      STA   HIST_OLDLEN
      LDA   HIST_NAV
      BEQ   @hd_repaint        ; already at the live line -> repaint in place
      DEC   HIST_NAV
      LDA   HIST_NAV
      BEQ   @hd_live           ; back at the live line -> restore it
      STA   HIST_K
      JSR   hist_slot_ptr
      JSR   hist_copy_in
      JMP   hist_show
@hd_live:
      LDA   #<HIST_LIVE
      STA   REPL_PTRL
      LDA   #>HIST_LIVE
      STA   REPL_PTRH
      JSR   hist_copy_in
@hd_repaint:
      JMP   hist_show

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

; hist_copy_in: copy the NUL-terminated line at REPL_PTR into input_buf; buf_idx=len.
hist_copy_in:
      LDY   #0
@ci_loop:
      LDA   (REPL_PTRL),Y
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

; hist_copy_out: copy input_buf (buf_idx bytes + NUL) to the line at REPL_PTR.
hist_copy_out:
      LDA   buf_idx
      STA   HIST_OLDLEN        ; buf_idx aliases REPL_PTRL during the copy
      LDY   #0
@co_loop:
      CPY   HIST_OLDLEN
      BCS   @co_term
      LDA   input_buf,Y
      STA   (REPL_PTRL),Y
      INY
      BRA   @co_loop
@co_term:
      LDA   #0
      STA   (REPL_PTRL),Y
      LDA   HIST_OLDLEN
      STA   buf_idx
      RTS

; hist_slot_ptr: REPL_PTR = address of the HIST_K-th newest entry (K in 1..COUNT).
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
; hist_slot_addr: REPL_PTR = HIST_SLOTS + HIST_IDX*128.
hist_slot_addr:
      LDA   HIST_IDX
      LSR   A                  ; A = idx>>1, carry = idx&1
      STA   HIST_K             ; reuse K as scratch high part (restored by callers)
      LDA   #0
      ROR   A                  ; carry -> bit7: (idx&1)?$80:0
      CLC
      ADC   #<HIST_SLOTS
      STA   REPL_PTRL
      LDA   HIST_K
      ADC   #>HIST_SLOTS
      STA   REPL_PTRH
      RTS

.endif
