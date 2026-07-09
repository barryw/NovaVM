\ =====================================================================
\  NovaForth — editor.4th
\
\  Wrappers for the NDK EDITOR module.
\
\  Copyright (C) 2026 Barry Walker
\  SPDX-License-Identifier: MIT
\ =====================================================================

INCLUDE forth/lib/nova/ndk.4th

BASE @
DECIMAL

0 CONSTANT EDITOR-FN-EDIT
1 CONSTANT EDITOR-FN-COUNT
0 CONSTANT EDITOR-PROFILE-NONE
1 CONSTANT EDITOR-PROFILE-FORTH
2 CONSTANT EDITOR-PROFILE-LOGO

: EDITOR-CALL MODULE-EDITOR SWAP NOVA-LIBCALL ;

: EDITOR-EDIT ( buffer cursor len capacity title profile -- exit save? len ior )
  LIB-ARG3 2 + C!
  LIB-ARG3 !
  LIB-ARG2 !
  LIB-ARG1 !
  LIB-ARG0 2 + !
  LIB-ARG0 !
  EDITOR-FN-EDIT EDITOR-CALL
  >R NDK-RESULT8 LIB-RESULT 1 + C@ LIB-ARG1 @ R> ;

: NOVA-EDIT-BUFFER ( buffer cursor len capacity title profile -- exit save? len ior )
  EDITOR-EDIT ;

BASE !
