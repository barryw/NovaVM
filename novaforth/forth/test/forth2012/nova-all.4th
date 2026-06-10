\ NovaForth runner for the Forth 2012 suites Nova currently claims.
\ This intentionally stops at Core, Core Extension, File Access, and String.
\ The upstream runtests.fth includes optional word sets that NovaForth does
\ not advertise yet.
\
\ The Core test includes one ACCEPT exercise; automated callers must queue a
\ line of input after starting this include.

INCLUDE forth/test/forth2012/src/prelimtest.fth
INCLUDE forth/test/forth2012/src/tester.fr
INCLUDE forth/test/forth2012/src/core.fr
INCLUDE forth/test/forth2012/src/coreplustest.fth
INCLUDE forth/lib/core-ext.4th
INCLUDE forth/test/forth2012/src/utilities.fth
INCLUDE forth/test/forth2012/src/errorreport.fth
INCLUDE forth/test/forth2012/src/coreexttest.fth
INCLUDE forth/lib/string.4th
INCLUDE forth/test/forth2012/src/stringtest.fth

S" forth/test/forth2012/src" NOVA-CD DROP
INCLUDE filetest.fth

DECIMAL TOTAL-ERRORS @ . S" F2012-CLAIMED-DONE" TYPE
