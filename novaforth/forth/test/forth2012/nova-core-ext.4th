\ NovaForth runner for the upstream Forth 2012 Core Extensions test file.
\ Keep this separate from nova-core.4th so Core-Ext can advance without
\ weakening the Core conformance signal.

INCLUDE forth/test/forth2012/src/tester.fr
INCLUDE forth/lib/core-ext.4th
INCLUDE forth/test/forth2012/src/utilities.fth
INCLUDE forth/test/forth2012/src/errorreport.fth
INCLUDE forth/test/forth2012/src/coreexttest.fth

DECIMAL TOTAL-ERRORS @ . S" F2012-CORE-EXT-DONE" TYPE
