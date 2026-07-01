\ NovaForth boot policy.
\ Edit this file on a NovaForth disk to choose which libraries load at boot.

INCLUDE forth/lib/core-ext.4th

\ Nova disk words: DIR / CATALOG / CD / PWD (+ the FILES-* NDK wrappers).
INCLUDE forth/lib/nova/files.4th
