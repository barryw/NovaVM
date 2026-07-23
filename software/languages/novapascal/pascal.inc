; NovaPascal compiler ABI declarations. Implementations are independent
; members of PASCAL.NLIB so NL only extracts helpers a program references.

.ifndef NP_SUPPORT
NP_SUPPORT = 1

.include "nvr.inc"

; The resident Pascal shell leaves $7E-$7F reserved for the active program's
; fast frame pointer. Keeping the address in this canonical ABI include lets
; NAS select compact zero-page indirect opcodes without compiler literals.
.GLOBALZP __NP_FP
__NP_FP = $007E

; Implicit System interface used by classic Pascal programs. __S markers
; encode the compact zero/one-argument ABI; __P markers encode exact arity.
__P4244CF = $02 ; GotoXY
__SB61446 = $00 ; ClrEol
__S79BB5C = $00 ; HighVideo
__SA5788E = $00 ; LowVideo
__SF4A494 = $01 ; Delay
__P8B34C1 = $02 ; Read(Device, Value)
__SF30766 = $03 ; UpCase
__SB64E4E = $00 ; ClrScr
__S8C0573 = $03 ; Succ
__S8A53D0 = $03 ; Pred
__P8B0AEA = $02 ; Assign
__SF219E8 = $01 ; Reset
__S33AE07 = $03 ; Length
__P8947B8 = $03 ; Delete
__S2F9327 = $01 ; Rewrite
__SE671BB = $01 ; Close
__S8B34C1 = $01 ; Read(Value)
__S87E2DF = $03 ; Eof
__P2B731B = $02 ; ReadLn(Device, Value)
__S2B731B = $01 ; ReadLn(Value)
__P882968 = $03 ; Val
__S87D03B = $03 ; Abs
__S8BF66F = $03 ; Sqrt
__S881EBB = $03 ; Sqr
__S881DAF = $03 ; Sin
__S87DA6A = $03 ; Cos
__S705A5E = $03 ; ArcTan
__S59745F = $03 ; Ln
__S87E412 = $03 ; Exp
__S1D7571 = $03 ; Trunc
__PD9C5DA = $03 ; Insert
__S59C618 = $00 ; NormVideo
__S85A96E = $00 ; Halt

.endif
