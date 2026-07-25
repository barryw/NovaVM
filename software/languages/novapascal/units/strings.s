; Turbo Strings facade. PChar values are ordinary typed low-RAM pointers;
; allocation delegates to Nova's canonical low-RAM heap.

.include "ramheap.inc"

.if .referenced(strpas)
      .segment "BSS"
strings_result: .res 256
.endif

.if .referenced(strlen) .OR .referenced(strend) .OR .referenced(strcopy) .OR .referenced(strecopy) .OR .referenced(strpcopy) .OR .referenced(strpas) .OR .referenced(strcat) .OR .referenced(strcomp) .OR .referenced(stricomp) .OR .referenced(strlower) .OR .referenced(strupper) .OR .referenced(strnew)
      .segment "CODE"

strings_load_pair:
      TSX
      LDA   $0105,X
      STA   NVR1L
      LDA   $0106,X
      STA   NVR1H
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      RTS

strings_inc_dest:
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     RTS

strings_inc_source:
      INC   NVR1L
      BNE   :+
      INC   NVR1H
:     RTS

; NVR0=destination, NVR1=source. Return the destination terminator.
strings_copy_z:
      LDA   (NVR1L)
      STA   (NVR0L)
      BEQ   @done
      JSR   strings_inc_source
      JSR   strings_inc_dest
      BRA   strings_copy_z
@done:
      LDA   NVR0L
      LDX   NVR0H
      RTS
.endif

.if .referenced(strlen) .OR .referenced(strnew)
      .export strlen
strlen:
      STA   NVR0L
      STX   NVR0H
      STZ   NVR1L
      STZ   NVR1H
@next:
      LDA   (NVR0L)
      BEQ   @done
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     INC   NVR1L
      BNE   @next
      INC   NVR1H
      BRA   @next
@done:
      LDA   NVR1L
      LDX   NVR1H
      RTS
.endif

.if .referenced(strend)
      .export strend
strend:
      STA   NVR0L
      STX   NVR0H
@next:
      LDA   (NVR0L)
      BEQ   @done
      JSR   strings_inc_dest
      BRA   @next
@done:
      LDA   NVR0L
      LDX   NVR0H
      RTS
.endif

.if .referenced(strcopy)
      .export strcopy
strcopy:
      JSR   strings_load_pair
      LDA   NVR0L
      STA   NVR2L
      LDA   NVR0H
      STA   NVR2H
      JSR   strings_copy_z
      LDA   NVR2L
      LDX   NVR2H
      RTS
.endif

.if .referenced(strecopy)
      .export strecopy
strecopy:
      JSR   strings_load_pair
      JMP   strings_copy_z
.endif

.if .referenced(strpcopy)
      .export strpcopy
strpcopy:
      JSR   strings_load_pair
      LDA   NVR0L
      STA   NVR2L
      LDA   NVR0H
      STA   NVR2H
      LDA   (NVR1L)
      STA   NVR3L
      JSR   strings_inc_source
@copy:
      LDA   NVR3L
      BEQ   @terminate
      LDA   (NVR1L)
      STA   (NVR0L)
      JSR   strings_inc_source
      JSR   strings_inc_dest
      DEC   NVR3L
      BRA   @copy
@terminate:
      LDA   #0
      STA   (NVR0L)
      LDA   NVR2L
      LDX   NVR2H
      RTS
.endif

.if .referenced(strpas)
      .export strpas
strpas:
      STA   NVR0L
      STX   NVR0H
      LDX   #0
@copy:
      CPX   #$FF
      BEQ   @done
      LDA   (NVR0L)
      BEQ   @done
      STA   strings_result+1,X
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     INX
      BRA   @copy
@done:
      STX   strings_result
      LDA   #<strings_result
      LDX   #>strings_result
      RTS
.endif

.if .referenced(strcat)
      .export strcat
strcat:
      JSR   strings_load_pair
      LDA   NVR0L
      STA   NVR2L
      LDA   NVR0H
      STA   NVR2H
@end:
      LDA   (NVR0L)
      BEQ   @append
      JSR   strings_inc_dest
      BRA   @end
@append:
      JSR   strings_copy_z
      LDA   NVR2L
      LDX   NVR2H
      RTS
.endif

.if .referenced(strcomp)
      .export strcomp
strcomp:
      JSR   strings_load_pair
@compare:
      LDA   (NVR0L)
      CMP   (NVR1L)
      BCC   strings_less
      BNE   strings_greater
      CMP   #0
      BEQ   strings_equal
      JSR   strings_inc_dest
      JSR   strings_inc_source
      BRA   @compare
.endif

.if .referenced(stricomp)
      .export stricomp
stricomp:
      JSR   strings_load_pair
@compare:
      LDA   (NVR0L)
      JSR   strings_upper_a
      STA   NVR3L
      LDA   (NVR1L)
      JSR   strings_upper_a
      STA   NVR3H
      LDA   NVR3L
      CMP   NVR3H
      BCC   strings_less
      BNE   strings_greater
      LDA   (NVR0L)
      BEQ   strings_equal
      JSR   strings_inc_dest
      JSR   strings_inc_source
      BRA   @compare

strings_upper_a:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS
.endif

.if .referenced(strcomp) .OR .referenced(stricomp)
strings_less:
      LDA   #$FF
      LDX   #$FF
      RTS
strings_greater:
      LDA   #1
      LDX   #0
      RTS
strings_equal:
      LDA   #0
      TAX
      RTS
.endif

.if .referenced(strlower)
      .export strlower
strlower:
      STA   NVR0L
      STX   NVR0H
      STA   NVR2L
      STX   NVR2H
@next:
      LDA   (NVR0L)
      BEQ   strings_case_done
      CMP   #'A'
      BCC   @advance
      CMP   #'Z'+1
      BCS   @advance
      ORA   #$20
      STA   (NVR0L)
@advance:
      JSR   strings_inc_dest
      BRA   @next
.endif

.if .referenced(strupper)
      .export strupper
strupper:
      STA   NVR0L
      STX   NVR0H
      STA   NVR2L
      STX   NVR2H
@next:
      LDA   (NVR0L)
      BEQ   strings_case_done
      CMP   #'a'
      BCC   @advance
      CMP   #'z'+1
      BCS   @advance
      AND   #$DF
      STA   (NVR0L)
@advance:
      JSR   strings_inc_dest
      BRA   @next
.endif

.if .referenced(strlower) .OR .referenced(strupper)
strings_case_done:
      LDA   NVR2L
      LDX   NVR2H
      RTS
.endif

.if .referenced(strnew)
      .export strnew
strnew:
      PHX
      PHA
      JSR   strlen
      INC   A
      BNE   :+
      INX
:     JSR   ndk_ram_alloc
      STA   NVR2L
      STX   NVR2H
      PLA
      STA   NVR1L
      PLA
      STA   NVR1H
      LDA   NVR2L
      ORA   NVR2H
      BEQ   @failed
      LDA   NVR2L
      STA   NVR0L
      LDA   NVR2H
      STA   NVR0H
      JSR   strings_copy_z
@failed:
      LDA   NVR2L
      LDX   NVR2H
      RTS
.endif

.if .referenced(strdispose)
      .export strdispose
strdispose:
      JMP   ndk_ram_free
.endif

.if .referenced(strnew) .OR .referenced(strdispose)
      .include "ramheap.s"
.endif
