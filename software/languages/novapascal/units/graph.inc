; Turbo-compatible Graph facade over Nova's canonical VGC helpers.
.ifndef NOVA_GRAPH_UNIT_INCLUDED
NOVA_GRAPH_UNIT_INCLUDED = 1
.include "nova.inc"
; Declare the canonical VGC contract instead of asserting its include guard:
; another unit in the same program (NovaAudio, NovaVgc, ...) may have pulled
; vgc.inc already, and a second assignment is a symbol redefinition.
.include "vgc.inc"

.global vgc_wait_cmd
.global vgc_set_mode
.global vgc_mem_read
.global vgc_gcls
.global vgc_gcolor
.global vgc_plot
.global vgc_line
.global vgc_circle
.global vgc_rect
.global vgc_fill
.global vgc_paint

.global detectgraph
.global initgraph
.global closegraph
.global graphresult
.global getmaxx
.global getmaxy
.global getmaxcolor
.global cleardevice
.global setcolor
.global getcolor
.global setbkcolor
.global getbkcolor
.global putpixel
.global getpixel
.global moveto
.global moverel
.global getx
.global gety
.global lineto
.global linerel
.global line
.global rectangle
.global bar
.global circle
.global floodfill
.endif
