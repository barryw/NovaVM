\ NovaForth automated run skips the upstream manual OUTPUT/ACCEPT block.
\ ------------------------------------------------------------------------
TESTING DICTIONARY SEARCH RULES

T{ : GDX   123 ; : GDX   GDX 234 ; -> }T

T{ GDX -> 123 234 }T

CR .( End of Core word set tests) CR
