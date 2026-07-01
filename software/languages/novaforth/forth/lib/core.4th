\ NovaForth core library.
\ Definitions that are part of the always-available NovaForth environment
\ should live here when they do not need to be ROM primitives.

: CONSTANT CREATE , DOES> @ ;
: VARIABLE CREATE 0 , ;

: ?DUP DUP IF DUP THEN ;
: 2DUP OVER OVER ;
: 2DROP DROP DROP ;

: 1+ 1 + ;
: 1- 1 - ;
: 2* 2 * ;
: / /MOD SWAP DROP ;
: MOD /MOD DROP ;
: */ */MOD SWAP DROP ;
: NEGATE 0 SWAP - ;
: > SWAP < ;
: 0= 0 = ;
: 0< 0 < ;
: ABS DUP 0< IF NEGATE THEN ;
: MIN 2DUP < IF DROP ELSE SWAP DROP THEN ;
: MAX 2DUP < IF SWAP DROP ELSE DROP THEN ;

: CELL+ 2 + ;
: CELLS 2 * ;
: CHAR+ 1+ ;
: CHARS ;
: ALIGN ;
: ALIGNED ;

: 2@ DUP CELL+ @ SWAP @ ;
: 2! DUP >R ! R> CELL+ ! ;
: +! DUP >R @ + R> ! ;
: COUNT DUP CHAR+ SWAP C@ ;

: DECIMAL 10 BASE ! ;
: HEX 16 BASE ! ;

32 CONSTANT BL
: SPACE BL EMIT ;
: CR 13 EMIT 10 EMIT ;

INCLUDE forth/lib/file.4th
