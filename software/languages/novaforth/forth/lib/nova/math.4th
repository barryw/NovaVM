\ =====================================================================
\  NovaForth — math.4th
\
\  Math coprocessor library: wraps the hardware register bank at
\  $BB20-$BB4F. Load explicitly with INCLUDE forth/lib/nova/math.4th.
\
\  Copyright (C) 2026 Barry Walker
\  SPDX-License-Identifier: MIT
\ =====================================================================

HEX
BB4A CONSTANT MATH-SQRT-LO     \ integer sqrt input, low byte
BB4B CONSTANT MATH-SQRT-HI     \ integer sqrt input, high byte (write triggers)
BB38 CONSTANT MATH-RES0        \ result byte 0
BB39 CONSTANT MATH-RES1        \ result byte 1
DECIMAL

\ SQRT ( n -- n' )  exact floor integer square root of unsigned n (0..65535).
\ Drives the coprocessor: store low byte, then high byte to trigger, read result.
: SQRT ( n -- n' )
  DUP 255 AND MATH-SQRT-LO C!    \ low byte (no trigger)
  8 RSHIFT MATH-SQRT-HI C!       \ high byte triggers the operation
  MATH-RES0 C@ MATH-RES1 C@ 8 LSHIFT OR ;
