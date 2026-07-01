\ NovaForth runner for the upstream Forth 2012 Core test file.
\ The Core test includes one ACCEPT exercise; automated callers must queue a
\ line of input after starting this include.

INCLUDE forth/test/forth2012/src/tester.fr
INCLUDE forth/test/forth2012/src/core.fr

DECIMAL #ERRORS @ . S" F2012-CORE-DONE" TYPE
