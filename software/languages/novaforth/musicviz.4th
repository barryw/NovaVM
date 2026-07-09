\ =====================================================================
\  NovaForth — musicviz.4th
\
\  Music visualizer: flowing copper bars + radiating rings synced to the
\  live MIDI notes the host publishes into $BA50. Drives the copper via
\  the VGC command interface, so it skips the 16KB graphics.4th.
\
\  Copyright (C) 2026 Barry Walker
\  SPDX-License-Identifier: MIT
\ =====================================================================

INCLUDE forth/lib/nova/vgc.4th
INCLUDE forth/lib/nova/sound.4th

HEX
A008 CONSTANT VGC-FRAME
BA50 CONSTANT MUS-STATUS   BA51 CONSTANT MUS-NOTE0
BA5F CONSTANT MUS-ELAPSED  BA61 CONSTANT MUS-TOTAL
1B CONSTANT VCMD-COPADD   1C CONSTANT VCMD-COPCLR   1D CONSTANT VCMD-COPENA   1E CONSTANT VCMD-COPDIS
DECIMAL


\ copper add via the VGC command interface: P0/P1=x P2=y P3/P4=reg P5=value
: COPPER-ADD ( x y reg value -- )
  VGC-P5 C! VGC-P3 VGC-P16! VGC-P2 C! VGC-P0 VGC-P16! VCMD-COPADD VGC-CMD! ;
: COPPER-CLR VCMD-COPCLR VGC-CMD! ;
: COPPER-ON  VCMD-COPENA VGC-CMD! ;
: COPPER-OFF VCMD-COPDIS VGC-CMD! ;
HEX A001 CONSTANT VGC-BGCOL DECIMAL
: SCREEN-RESET TEXT COPPER-OFF 0 VGC-BGCOL C! 12 EMIT ;

: VSYNC VGC-FRAME C@ BEGIN DUP VGC-FRAME C@ = WHILE REPEAT DROP ;
: NOTE  MUS-NOTE0 + C@ ;
: PLAYING?  MUS-STATUS C@ 2 AND ;
: ELAPSED   MUS-ELAPSED @ ;
: TOTAL     MUS-TOTAL @ 1 MAX ;
: INTENSITY ELAPSED TOTAL 16 / 1 MAX / 15 MIN ;
: VOICES 0 14 0 DO I NOTE IF 1+ THEN LOOP ;
: BASS 127 14 0 DO I NOTE DUP IF MIN ELSE DROP THEN LOOP DUP 127 = IF DROP 0 THEN ;

VARIABLE FLOW
VARIABLE PULSE
VARIABLE PREVV
VARIABLE FLASH
: BEAT? VOICES DUP PREVV @ > SWAP PREVV ! ;
\ full-screen flowing colour gradient; hue base from the bass note, scroll speed
\ from intensity, a brightness kick from the number of voices (the beat).
: BARS
  COPPER-CLR
  BASS 5 / FLOW @ + VOICES + FLASH @ 2* +
  200 0 DO  DUP I 8 / + 15 AND  >R 0 I 1 R> COPPER-ADD  8 +LOOP
  DROP COPPER-ON ;
\ concentric rings radiating from centre; faster as the piece builds.
: RINGS
  PULSE @ INTENSITY 3 + + 64 MOD PULSE !
  INTENSITY 4 / 4 + 0 DO
    FLASH @ IF 15 ELSE I 3 + INTENSITY + 15 AND THEN GCOLOR
    160 100  PULSE @ I 14 * +  6 +  CIRCLE
  LOOP ;

: VIZ
  S" generaluser" SND-SFLOAD-FILE DROP
  GRAPHICS GCLS 0 FLOW ! 0 PULSE ! 0 PREVV ! 0 FLASH !
  S" mountainking" SND-MIDPLAY-FILE DROP
  30 0 DO VSYNC LOOP
  BEGIN
    VSYNC  BEAT? IF 4 FLASH ! THEN
    FLASH @ DUP IF 1- FLASH ! ELSE DROP THEN
    INTENSITY 1+ FLASH @ + FLOW +!
    BARS GCLS RINGS
    PLAYING? 0=
  UNTIL  SCREEN-RESET SND-MIDSTOP DROP ;
