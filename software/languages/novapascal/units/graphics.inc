; Precompiled NovaGraphics unit implementation declarations.

.ifndef NOVA_GRAPHICS_UNIT_INCLUDED
NOVA_GRAPHICS_UNIT_INCLUDED = 1

; Declare only this unit's canonical VGC dependencies. This keeps NAS's
; transient symbol table focused while GRAPHICS.S still includes vgc.s itself.
.include "nova.inc"
VGC_INC_INCLUDED = 1

.global vgc_wait_cmd
.global vgc_wait_frames
.global vgc_set_mode
.global vgc_gcls
.global vgc_gcolor
.global vgc_fill

.global graphicsopen
.global graphicsclear
.global graphicscolor
.global graphicsfill
.global graphicstile16
.global graphicstile4x8
.global graphicswait
.global graphicsclose

.endif
