\ NovaForth runner for the upstream Forth 2012 File Access test file.
\ The upstream file test expects its helper files in the current directory,
\ so this runner changes Nova's FIO current directory before including it.

INCLUDE forth/test/forth2012/src/tester.fr
INCLUDE forth/lib/core-ext.4th
INCLUDE forth/test/forth2012/src/utilities.fth
INCLUDE forth/test/forth2012/src/errorreport.fth
INCLUDE forth/test/forth2012/src/coreexttest.fth

S" forth/test/forth2012/src" NOVA-CD DROP
INCLUDE filetest.fth

DECIMAL TOTAL-ERRORS @ . S" F2012-FILE-DONE" TYPE
