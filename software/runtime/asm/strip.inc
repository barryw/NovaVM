; =====================================================================
;  Nova NDK — strip.inc
;
;  Nova runtime size-stripping switches for custom builds. Include before
;  implementation files only when a custom build has enabled full runtime
;  emission and wants to force stripping back on. Stripping uses ca65 .ifref,
;  so implementation files must still appear after their references.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef NOVA_STRIP_INC_INCLUDED
NOVA_STRIP_INC_INCLUDED = 1

NOVA_STRIP_UNUSED = 1
FIO_STRIP_UNUSED = 1
AUDIO_STRIP_UNUSED = 1

.endif
