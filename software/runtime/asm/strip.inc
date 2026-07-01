; Nova runtime size-stripping switches for custom builds.
;
; Runtime files that support stripping do it by default. Include this before
; implementation files only when a custom build has enabled full runtime
; emission and wants to force stripping back on. Stripping uses ca65 .ifref, so
; implementation files must still appear after code that references them.

.ifndef NOVA_STRIP_INC_INCLUDED
NOVA_STRIP_INC_INCLUDED = 1

NOVA_STRIP_UNUSED = 1
FIO_STRIP_UNUSED = 1
AUDIO_STRIP_UNUSED = 1

.endif
