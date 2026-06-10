\ NovaForth runner for the upstream Forth 2012 String tests.

INCLUDE forth/test/forth2012/src/tester.fr
INCLUDE forth/lib/string.4th
INCLUDE forth/test/forth2012/src/utilities.fth
INCLUDE forth/test/forth2012/src/errorreport.fth
INCLUDE forth/test/forth2012/src/stringtest.fth

DECIMAL TOTAL-ERRORS @ . S" F2012-STRING-DONE" TYPE
