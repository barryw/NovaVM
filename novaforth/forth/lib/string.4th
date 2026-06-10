\ NovaForth Forth-2012 String and String Extensions library.

BASE @
DECIMAL

INCLUDE forth/lib/core-ext.4th

: -TRAILING
  BEGIN
    DUP
  WHILE
    2DUP + 1- C@ BL =
    IF 1- ELSE EXIT THEN
  REPEAT ;

: BLANK BL FILL ;

: CMOVE
  BEGIN
    DUP
  WHILE
    >R OVER C@ OVER C!
    1+ SWAP 1+ SWAP
    R> 1-
  REPEAT
  DROP 2DROP ;

: CMOVE>
  DUP >R + 1- SWAP R@ + 1- SWAP R>
  BEGIN
    DUP
  WHILE
    >R OVER C@ OVER C!
    1- SWAP 1- SWAP
    R> 1-
  REPEAT
  DROP 2DROP ;

VARIABLE (CMP-A1)
VARIABLE (CMP-U1)
VARIABLE (CMP-A2)
VARIABLE (CMP-U2)

: COMPARE
  (CMP-U2) ! (CMP-A2) ! (CMP-U1) ! (CMP-A1) !
  BEGIN
    (CMP-U1) @ 0<> (CMP-U2) @ 0<> AND
  WHILE
    (CMP-A1) @ C@ (CMP-A2) @ C@ 2DUP <>
    IF U< IF -1 ELSE 1 THEN EXIT THEN
    2DROP
    1 (CMP-A1) +! 1 (CMP-A2) +!
    -1 (CMP-U1) +! -1 (CMP-U2) +!
  REPEAT
  (CMP-U1) @ (CMP-U2) @ =
  IF 0 EXIT THEN
  (CMP-U1) @ (CMP-U2) @ U< IF -1 ELSE 1 THEN ;

: (PREFIX?)
  BEGIN
    DUP
  WHILE
    >R OVER C@ OVER C@ <>
    IF R> DROP 2DROP FALSE EXIT THEN
    1+ SWAP 1+ SWAP
    R> 1-
  REPEAT
  DROP 2DROP TRUE ;

VARIABLE (SEARCH-A0)
VARIABLE (SEARCH-U0)
VARIABLE (SEARCH-A)
VARIABLE (SEARCH-U)
VARIABLE (SEARCH-P)
VARIABLE (SEARCH-N)

: SEARCH
  (SEARCH-N) ! (SEARCH-P) ! (SEARCH-U) ! (SEARCH-A) !
  (SEARCH-A) @ (SEARCH-A0) !
  (SEARCH-U) @ (SEARCH-U0) !
  (SEARCH-N) @ 0=
  IF (SEARCH-A0) @ (SEARCH-U0) @ TRUE EXIT THEN
  BEGIN
    (SEARCH-U) @ (SEARCH-N) @ U< 0=
  WHILE
    (SEARCH-A) @ (SEARCH-P) @ (SEARCH-N) @ (PREFIX?)
    IF (SEARCH-A) @ (SEARCH-U) @ TRUE EXIT THEN
    1 (SEARCH-A) +! -1 (SEARCH-U) +!
  REPEAT
  (SEARCH-A0) @ (SEARCH-U0) @ FALSE ;

\ Minimal double-cell storage helpers used by the upstream String tests.
: 2VARIABLE CREATE 0 , 0 , ;

16 CONSTANT (REPL-MAX)
VARIABLE (REPL-COUNT)
CREATE (REPLS) (REPL-MAX) 4 * CELLS ALLOT

: (REPL-REC) 4 * CELLS (REPLS) + ;
: (R-NA) (REPL-REC) ;
: (R-NL) (REPL-REC) CELL+ ;
: (R-VA) (REPL-REC) 2 CELLS + ;
: (R-VL) (REPL-REC) 3 CELLS + ;

VARIABLE (SAVE-LEN)
: (SAVE-STRING)
  DUP (SAVE-LEN) !
  HERE >R
  (SAVE-LEN) @ ALLOT
  DROP
  R@ (SAVE-LEN) @ MOVE
  R> (SAVE-LEN) @ ;

VARIABLE (FIND-A)
VARIABLE (FIND-U)
VARIABLE (FIND-I)
VARIABLE (EQ-A1)
VARIABLE (EQ-U1)
VARIABLE (EQ-A2)
VARIABLE (EQ-U2)

: (UPPER)
  DUP [CHAR] a [CHAR] { WITHIN
  IF 32 - THEN ;

: (REPL-SAME?)
  (EQ-U2) ! (EQ-A2) ! (EQ-U1) ! (EQ-A1) !
  (EQ-U1) @ (EQ-U2) @ <> IF FALSE EXIT THEN
  BEGIN
    (EQ-U1) @
  WHILE
    (EQ-A1) @ C@ (UPPER) (EQ-A2) @ C@ (UPPER) <>
    IF FALSE EXIT THEN
    1 (EQ-A1) +! 1 (EQ-A2) +!
    -1 (EQ-U1) +!
  REPEAT
  TRUE ;

: (REPL-FIND)
  (FIND-U) ! (FIND-A) !
  0 (FIND-I) !
  BEGIN
    (FIND-I) @ (REPL-COUNT) @ U<
  WHILE
    (FIND-A) @ (FIND-U) @
    (FIND-I) @ (R-NA) @ (FIND-I) @ (R-NL) @ (REPL-SAME?)
    IF (FIND-I) @ TRUE EXIT THEN
    1 (FIND-I) +!
  REPEAT
  FALSE ;

VARIABLE (REP-VA)
VARIABLE (REP-VU)
VARIABLE (REP-NA)
VARIABLE (REP-NU)
: REPLACES
  (REP-NU) ! (REP-NA) ! (REP-VU) ! (REP-VA) !
  (REP-NA) @ (REP-NU) @ (REPL-FIND)
  IF
    >R
    (REP-VA) @ (REP-VU) @ (SAVE-STRING)
    R@ (R-VL) ! R> (R-VA) !
    EXIT
  THEN
  (REPL-COUNT) @ DUP (REPL-MAX) U< 0=
  IF DROP EXIT THEN
  DUP >R
  (REP-NA) @ (REP-NU) @ (SAVE-STRING)
  R@ (R-NL) ! R@ (R-NA) !
  (REP-VA) @ (REP-VU) @ (SAVE-STRING)
  R@ (R-VL) ! R> (R-VA) !
  DROP
  1 (REPL-COUNT) +! ;

VARIABLE (US-S)
VARIABLE (US-U)
VARIABLE (US-D)
VARIABLE (US-O)

: (US-EMIT)
  (US-D) @ (US-O) @ + C!
  1 (US-O) +! ;

: UNESCAPE
  (US-D) ! (US-U) ! (US-S) ! 0 (US-O) !
  BEGIN
    (US-U) @
  WHILE
    (US-S) @ C@ [CHAR] % =
    IF
      [CHAR] % (US-EMIT)
      [CHAR] % (US-EMIT)
      1 (US-S) +! -1 (US-U) +!
    ELSE
      (US-S) @ C@ (US-EMIT)
      1 (US-S) +! -1 (US-U) +!
    THEN
  REPEAT
  (US-D) @ (US-O) @ ;

VARIABLE (SUB-S)
VARIABLE (SUB-U)
VARIABLE (SUB-D)
VARIABLE (SUB-MAX)
VARIABLE (SUB-O)
VARIABLE (SUB-N)
VARIABLE (SUB-I)
VARIABLE (SUB-NAME-U)
VARIABLE (SUB-ERR)
VARIABLE (COPY-U)

: (U<=) 2DUP U< >R = R> OR ;
: (SUB-ADV) DUP (SUB-S) +! NEGATE (SUB-U) +! ;
: (SUB-FAIL) TRUE (SUB-ERR) ! ;

: (SUB-ROOM?)
  (SUB-O) @ + (SUB-MAX) @ (U<=) ;

: (SUB-COPY)
  DUP (COPY-U) !
  DUP (SUB-ROOM?) 0=
  IF 2DROP FALSE EXIT THEN
  DROP
  (SUB-D) @ (SUB-O) @ + (COPY-U) @ MOVE
  (COPY-U) @ (SUB-O) +!
  TRUE ;

: (SUB-EMIT)
  1 (SUB-ROOM?) 0=
  IF DROP FALSE EXIT THEN
  (SUB-D) @ (SUB-O) @ + C!
  1 (SUB-O) +!
  TRUE ;

: (SUB-FIND-END)
  1 (SUB-I) !
  BEGIN
    (SUB-I) @ (SUB-U) @ U<
  WHILE
    (SUB-S) @ (SUB-I) @ + C@ [CHAR] % =
    IF (SUB-I) @ 1- TRUE EXIT THEN
    1 (SUB-I) +!
  REPEAT
  FALSE ;

: (SUB-PERCENT)
  (SUB-FIND-END)
  IF
    (SUB-NAME-U) !
    (SUB-S) @ CHAR+ (SUB-NAME-U) @ 2DUP (REPL-FIND)
    IF
      >R 2DROP
      R@ (R-VA) @ R> (R-VL) @ (SUB-COPY)
      IF
        1 (SUB-N) +!
        (SUB-NAME-U) @ 2 + (SUB-ADV)
      ELSE (SUB-FAIL) THEN
    ELSE
      2DROP
      (SUB-S) @ (SUB-NAME-U) @ 2 + (SUB-COPY)
      IF (SUB-NAME-U) @ 2 + (SUB-ADV)
      ELSE (SUB-FAIL) THEN
    THEN
  ELSE
    [CHAR] % (SUB-EMIT)
    IF 1 (SUB-ADV) ELSE (SUB-FAIL) THEN
  THEN ;

: SUBSTITUTE
  (SUB-MAX) ! (SUB-D) ! (SUB-U) ! (SUB-S) !
  (SUB-D) @ (SUB-S) @ (SUB-S) @ (SUB-U) @ + WITHIN
  (SUB-S) @ (SUB-D) @ (SUB-D) @ (SUB-MAX) @ + WITHIN OR
  IF (SUB-D) @ 0 -1 EXIT THEN
  0 (SUB-O) ! 0 (SUB-N) ! FALSE (SUB-ERR) !
  BEGIN
    (SUB-U) @ (SUB-ERR) @ 0= AND
  WHILE
    (SUB-S) @ C@ [CHAR] % <>
    IF
      (SUB-S) @ C@ (SUB-EMIT)
      IF 1 (SUB-ADV) ELSE (SUB-FAIL) THEN
    ELSE
      (SUB-U) @ 1 > (SUB-S) @ CHAR+ C@ [CHAR] % = AND
      IF
        [CHAR] % (SUB-EMIT)
        IF 2 (SUB-ADV) ELSE (SUB-FAIL) THEN
      ELSE
        (SUB-PERCENT)
      THEN
    THEN
  REPEAT
  (SUB-ERR) @
  IF (SUB-D) @ 0 -1
  ELSE (SUB-D) @ (SUB-O) @ (SUB-N) @ THEN ;

BASE !
