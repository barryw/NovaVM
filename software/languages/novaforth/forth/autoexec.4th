\ =====================================================================
\  NovaForth — autoexec.4th
\
\  Boot policy: edit this file on a NovaForth disk to choose which
\  libraries load at boot.
\
\  Copyright (C) 2026 Barry Walker
\  SPDX-License-Identifier: MIT
\ =====================================================================

INCLUDE forth/lib/core-ext.4th

\ Nova disk words: DIR / CATALOG / CD / PWD (+ the FILES-* NDK wrappers).
INCLUDE forth/lib/nova/files.4th
