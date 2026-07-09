\ =====================================================================
\  NovaForth — vgc.4th
\
\  VGC graphics library (direct register/command interface). Load
\  explicitly with INCLUDE forth/lib/nova/vgc.4th.
\
\  Copyright (C) 2026 Barry Walker
\  SPDX-License-Identifier: MIT
\ =====================================================================

HEX
A000 CONSTANT VGC-MODE
A00A CONSTANT VGC-CURSOR
A010 CONSTANT VGC-CMD
A011 CONSTANT VGC-P0
A012 CONSTANT VGC-P1
A013 CONSTANT VGC-P2
A014 CONSTANT VGC-P3
A015 CONSTANT VGC-P4
A016 CONSTANT VGC-P5
A017 CONSTANT VGC-P6
A018 CONSTANT VGC-P7
A0E9 CONSTANT VGC-PALETTE
A0C0 CONSTANT VGC-TEXT-BG
\ Inline text-style control codes (see nova.inc VGC_CC_*). EMIT them into the
\ text stream and the VGC restyles as it prints. FG/BG/CHARBG take a param byte.
01 CONSTANT CC-RESET
06 CONSTANT CC-BOLD
07 CONSTANT CC-FLASH
0E CONSTANT CC-REVERSE
10 CONSTANT CC-FG
11 CONSTANT CC-BG
14 CONSTANT CC-CHARBG

01 CONSTANT VGC-CMD-PLOT
02 CONSTANT VGC-CMD-UNPLOT
03 CONSTANT VGC-CMD-LINE
04 CONSTANT VGC-CMD-CIRCLE
05 CONSTANT VGC-CMD-RECT
06 CONSTANT VGC-CMD-FILL
07 CONSTANT VGC-CMD-GCLS
08 CONSTANT VGC-CMD-GCOLOR
09 CONSTANT VGC-CMD-PAINT

00 CONSTANT PALETTE-NOVA
01 CONSTANT PALETTE-EGA
DECIMAL

: >LO $FF AND ;
: >HI 8 RSHIFT $FF AND ;

: VGC-P16! ( u addr -- )
  >R DUP >LO R@ C! >HI R> CHAR+ C! ;

: VGC-XY! ( x y -- )
  VGC-P2 VGC-P16! VGC-P0 VGC-P16! ;

: VGC-WAIT ( -- )
  BEGIN VGC-CMD C@ 1 AND 0= UNTIL ;

: VGC-CMD! ( cmd -- ) VGC-WAIT VGC-CMD C! VGC-WAIT ;

\ GRAPHICS = mode 3 (gfx only, text layer hidden). Mode 1 leaves the text layer
\ ON TOP of the gfx, painting the REPL over your drawing; use MIXED-GFX for that
\ on purpose. GRAPHICS is pure graphics.
: GRAPHICS 3 VGC-MODE C! 0 VGC-CURSOR C! ;
: MIXED-GFX 1 VGC-MODE C! 0 VGC-CURSOR C! ;
: TEXT 0 VGC-MODE C! ;

\ Per-character background colour. Each printed character captures the current
\ setting into its own cell, so you can mix freely across the screen:
\   CHARBG-OFF        transparent (default) - the char shows whatever the
\                     screen background is (VGC-BGCOL / a copper gradient).
\   ( color ) CHARBG  opaque background colour 0-15 baked into the char.
: CHARBG ( color -- ) $0F AND VGC-TEXT-BG C! ;
: CHARBG-OFF ( -- ) $10 VGC-TEXT-BG C! ;

\ Inline style helpers — emit a code into the text stream so the styling travels
\ with what you print. INK/PAPER take a colour; REVERSE/BOLD/FLASH toggle (1st
\ use on, next off); PLAIN resets everything to normal (transparent bg, no fx).
: INK ( color -- )   CC-FG EMIT EMIT ;
: PAPER ( color -- ) CC-BG EMIT EMIT ;
: REVERSE ( -- )     CC-REVERSE EMIT ;
: BOLD ( -- )        CC-BOLD EMIT ;
: FLASH ( -- )       CC-FLASH EMIT ;
: PLAIN ( -- )       CC-RESET EMIT ;
: PALETTE ( mode -- ) 1 AND VGC-PALETTE C! ;
: NOVA-PALETTE ( -- ) PALETTE-NOVA PALETTE ;
: C64-PALETTE ( -- ) PALETTE-NOVA PALETTE ;
: EGA-PALETTE ( -- ) PALETTE-EGA PALETTE ;
: GCLS VGC-CMD-GCLS VGC-CMD! ;
: GCOLOR VGC-P0 C! VGC-CMD-GCOLOR VGC-CMD! ;

: PLOT ( x y -- )
  VGC-XY! VGC-CMD-PLOT VGC-CMD! ;

: UNPLOT ( x y -- )
  VGC-XY! VGC-CMD-UNPLOT VGC-CMD! ;

: LINE ( x0 y0 x1 y1 -- )
  VGC-P6 VGC-P16! VGC-P4 VGC-P16!
  VGC-P2 VGC-P16! VGC-P0 VGC-P16!
  VGC-CMD-LINE VGC-CMD! ;

: CIRCLE ( cx cy r -- )
  0 VGC-P6 VGC-P16!
  VGC-P4 VGC-P16! VGC-P2 VGC-P16! VGC-P0 VGC-P16!
  VGC-CMD-CIRCLE VGC-CMD! ;

: ELLIPSE ( cx cy rx ry -- )
  VGC-P6 VGC-P16! VGC-P4 VGC-P16!
  VGC-P2 VGC-P16! VGC-P0 VGC-P16!
  VGC-CMD-CIRCLE VGC-CMD! ;

: RECT ( x0 y0 x1 y1 -- )
  VGC-P6 VGC-P16! VGC-P4 VGC-P16!
  VGC-P2 VGC-P16! VGC-P0 VGC-P16!
  VGC-CMD-RECT VGC-CMD! ;

: GFILL ( x0 y0 x1 y1 -- )
  VGC-P6 VGC-P16! VGC-P4 VGC-P16!
  VGC-P2 VGC-P16! VGC-P0 VGC-P16!
  VGC-CMD-FILL VGC-CMD! ;

: PAINT ( x y -- )
  VGC-XY! VGC-CMD-PAINT VGC-CMD! ;
