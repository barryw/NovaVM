; Turbo-style Overlay facade over Nova's canonical System module.

.include "libsystem.inc"
.include "nova.inc"

.if .referenced(ovrresult) .OR .referenced(ovrinitat) .OR .referenced(ovrcallinit) .OR .referenced(ovrrun) .OR .referenced(ovrtick) .OR .referenced(ovrdone)
      .segment "BSS"
      .export ovrresult
ovrresult: .res 2

      .segment "CODE"

overlayface_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BEQ   @ok
      LDA   LIB_RESULT+1
      BNE   @store
      LDA   LIB_STATUS
      BRA   @store
@ok:
      LDA   #0
@store:
      STA   ovrresult
      STZ   ovrresult+1
      RTS
.endif

.if .referenced(ovrinitat)
      .export ovrinitat
ovrinitat:
      TSX
      LDA   $0103,X
      STA   LIB_ARG3
      LDA   $0104,X
      STA   LIB_ARG3+1
      STZ   LIB_ARG3+2
      STZ   LIB_ARG3+3
      LDA   $0105,X
      STA   LIB_ARG2
      LDA   $0106,X
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      CLC
      LDA   NVR0L
      ADC   #1
      STA   LIB_ARG0
      LDA   NVR0H
      ADC   #0
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   #SYS_OVL_LOAD
      JMP   overlayface_call
.endif

.if .referenced(ovrcallinit)
      .export ovrcallinit
ovrcallinit:
      LDA   #SYS_OVL_INIT
      JMP   overlayface_call
.endif

.if .referenced(ovrrun)
      .export ovrrun
ovrrun:
      LDA   #SYS_OVL_MAIN
      JMP   overlayface_call
.endif

.if .referenced(ovrtick)
      .export ovrtick
ovrtick:
      LDA   #SYS_OVL_TICK
      JMP   overlayface_call
.endif

.if .referenced(ovrdone)
      .export ovrdone
ovrdone:
      LDA   #SYS_OVL_UNLOAD
      JMP   overlayface_call
.endif
