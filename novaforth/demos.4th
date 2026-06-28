\ Nova hardware show-off demos for NovaForth.
\ Run from the disk you booted:   INCLUDE "demos.4th"
\ then type a demo name:          TUNNEL   STARBURST   MOIRE   ALL
\ Each ends back at the text prompt. The VGC's artist (line/circle) draws in
\ hardware, so these vector effects run fast; VSYNC keeps them tear-free.

INCLUDE forth/lib/nova/vgc.4th

HEX A008 CONSTANT VGC-FRAME DECIMAL
: VSYNC ( -- )  VGC-FRAME C@ BEGIN DUP VGC-FRAME C@ = WHILE REPEAT DROP ;

\ TUNNEL — concentric rings, colour pulsing inward. No clear -> no flicker:
\ each frame just redraws the rings over themselves in shifted colours.
: TUNNEL
  GRAPHICS NOVA-PALETTE GCLS
  150 0 DO
    100 4 DO  J I 4 / + 7 AND 1+ GCOLOR  160 100 I CIRCLE  4 +LOOP
    VSYNC
  LOOP TEXT ;

\ STARBURST — a fan of lines from a focal point sweeping across, colour cycling.
: ~FAN ( fx fy -- )
  320 0 DO 2DUP I   0 LINE 14 +LOOP
  320 0 DO 2DUP I 199 LINE 14 +LOOP
  2DROP ;
: STARBURST
  GRAPHICS NOVA-PALETTE
  320 0 DO
    GCLS  I 16 / 7 AND 1+ GCOLOR  I 100 ~FAN  VSYNC
  8 +LOOP TEXT ;

\ MOIRE — two sets of concentric rings drifting through each other.
: ~RINGS ( cx cy -- )  90 8 DO 2DUP I CIRCLE 7 +LOOP 2DROP ;
: MOIRE
  GRAPHICS NOVA-PALETTE
  200 0 DO
    GCLS
    14 GCOLOR  90 I +  320 MOD  100 ~RINGS
    9  GCOLOR  230 I -          100 ~RINGS
    VSYNC
  6 +LOOP TEXT ;

: ALL  TUNNEL STARBURST MOIRE ;

: DEMOS  TEXT CR
  ." Nova demos loaded.  Type:  TUNNEL  STARBURST  MOIRE  ALL" CR ;
DEMOS
