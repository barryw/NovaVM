; Precompiled NovaGraphics unit implementation declarations.

.ifndef NOVA_GRAPHICS_UNIT_INCLUDED
NOVA_GRAPHICS_UNIT_INCLUDED = 1

; Declare this unit's canonical VGC dependencies. The contract is included
; rather than its guard asserted: another unit in the same program (NovaAudio,
; NovaVgc, ...) may have pulled vgc.inc already, and a second assignment of the
; guard is a symbol redefinition.
.include "nova.inc"
.include "vgc.inc"

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
