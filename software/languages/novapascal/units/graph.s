; Turbo-compatible Graph unit over Nova's canonical VGC implementation.

.include "graph.inc"

.ifndef NOVA_GRAPH_UNIT_IMPLEMENTATION_INCLUDED
NOVA_GRAPH_UNIT_IMPLEMENTATION_INCLUDED = 1

.if .referenced(detectgraph) .OR .referenced(initgraph) .OR .referenced(closegraph) .OR .referenced(graphresult) .OR .referenced(getmaxx) .OR .referenced(getmaxy) .OR .referenced(getmaxcolor) .OR .referenced(cleardevice) .OR .referenced(setcolor) .OR .referenced(getcolor) .OR .referenced(setbkcolor) .OR .referenced(getbkcolor) .OR .referenced(putpixel) .OR .referenced(getpixel) .OR .referenced(moveto) .OR .referenced(moverel) .OR .referenced(getx) .OR .referenced(gety) .OR .referenced(lineto) .OR .referenced(linerel) .OR .referenced(line) .OR .referenced(rectangle) .OR .referenced(bar) .OR .referenced(circle) .OR .referenced(floodfill)
GRAPH_UNIT_REFERENCED = 1
.endif

.ifdef GRAPH_UNIT_REFERENCED
      .segment "BSS"
graph_error:     .res 2
graph_color:     .res 1
graph_background:.res 1
graph_current_x: .res 2
graph_current_y: .res 2
graph_arg_x:     .res 2
graph_arg_y:     .res 2
graph_arg_color: .res 1
graph_tmp:       .res 2

      .segment "CODE"

graph_set_vgc_color:
      AND   #$0F
      STA   VGC_P0
      JSR   vgc_gcolor
      JMP   vgc_wait_cmd

graph_draw_to:
      LDA   graph_current_x
      STA   VGC_P0
      LDA   graph_current_x+1
      STA   VGC_P1
      LDA   graph_current_y
      STA   VGC_P2
      LDA   graph_current_y+1
      STA   VGC_P3
      LDA   graph_arg_x
      STA   VGC_P4
      LDA   graph_arg_x+1
      STA   VGC_P5
      LDA   graph_arg_y
      STA   VGC_P6
      LDA   graph_arg_y+1
      STA   VGC_P7
      JSR   vgc_line
      JSR   vgc_wait_cmd
      LDA   graph_arg_x
      STA   graph_current_x
      LDA   graph_arg_x+1
      STA   graph_current_x+1
      LDA   graph_arg_y
      STA   graph_current_y
      LDA   graph_arg_y+1
      STA   graph_current_y+1
      RTS

.if .referenced(detectgraph)
      .export detectgraph
detectgraph:
      TSX
      LDA   $0103,X
      STA   NVR0L
      LDA   $0104,X
      STA   NVR0H
      LDA   $0105,X
      STA   NVR1L
      LDA   $0106,X
      STA   NVR1H
      LDY   #0
      LDA   #0
      STA   (NVR0L),Y
      INY
      STA   (NVR0L),Y
      LDY   #0
      LDA   #9
      STA   (NVR1L),Y
      INY
      LDA   #0
      STA   (NVR1L),Y
      STZ   graph_error
      STZ   graph_error+1
      RTS
.endif

.if .referenced(initgraph)
      .export initgraph
initgraph:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0107,X
      STA   NVR1L
      LDA   $0108,X
      STA   NVR1H
      LDY   #0
      LDA   #0
      STA   (NVR0L),Y
      INY
      STA   (NVR0L),Y
      LDY   #0
      LDA   #9
      STA   (NVR1L),Y
      INY
      LDA   #0
      STA   (NVR1L),Y
      STZ   graph_error
      STZ   graph_error+1
      STZ   graph_background
      STZ   graph_current_x
      STZ   graph_current_x+1
      STZ   graph_current_y
      STZ   graph_current_y+1
      LDA   #15
      STA   graph_color
      LDX   #VGC_MODE_GFX_ONLY
      JSR   vgc_set_mode
      LDA   graph_color
      JSR   graph_set_vgc_color
      JSR   vgc_gcls
      JMP   vgc_wait_cmd
.endif

.if .referenced(closegraph)
      .export closegraph
closegraph:
      JSR   vgc_wait_cmd
      LDX   #VGC_MODE_TEXT
      JSR   vgc_set_mode
      STZ   graph_error
      STZ   graph_error+1
      RTS
.endif

.if .referenced(graphresult)
      .export graphresult
graphresult:
      LDA   graph_error
      LDX   graph_error+1
      STZ   graph_error
      STZ   graph_error+1
      RTS
.endif

.if .referenced(getmaxx)
      .export getmaxx
getmaxx:
      LDA   #<$013F
      LDX   #>$013F
      RTS
.endif

.if .referenced(getmaxy)
      .export getmaxy
getmaxy:
      LDA   #<$00C7
      LDX   #>$00C7
      RTS
.endif

.if .referenced(getmaxcolor)
      .export getmaxcolor
getmaxcolor:
      LDA   #15
      LDX   #0
      RTS
.endif

.if .referenced(cleardevice)
      .export cleardevice
cleardevice:
      STZ   graph_current_x
      STZ   graph_current_x+1
      STZ   graph_current_y
      STZ   graph_current_y+1
      LDA   graph_background
      JSR   graph_set_vgc_color
      STZ   VGC_P0
      STZ   VGC_P1
      STZ   VGC_P2
      STZ   VGC_P3
      LDA   #<$013F
      STA   VGC_P4
      LDA   #>$013F
      STA   VGC_P5
      LDA   #$C7
      STA   VGC_P6
      STZ   VGC_P7
      JSR   vgc_fill
      JSR   vgc_wait_cmd
      LDA   graph_color
      JMP   graph_set_vgc_color
.endif

.if .referenced(setcolor)
      .export setcolor
setcolor:
      AND   #$0F
      STA   graph_color
      JMP   graph_set_vgc_color
.endif

.if .referenced(getcolor)
      .export getcolor
getcolor:
      LDA   graph_color
      LDX   #0
      RTS
.endif

.if .referenced(setbkcolor)
      .export setbkcolor
setbkcolor:
      AND   #$0F
      STA   graph_background
      RTS
.endif

.if .referenced(getbkcolor)
      .export getbkcolor
getbkcolor:
      LDA   graph_background
      LDX   #0
      RTS
.endif

.if .referenced(putpixel)
      .export putpixel
putpixel:
      TSX
      LDA   $0103,X
      AND   #$0F
      STA   graph_arg_color
      JSR   graph_set_vgc_color
      TSX
      LDA   $0105,X
      STA   VGC_P2
      LDA   $0106,X
      STA   VGC_P3
      LDA   $0107,X
      STA   VGC_P0
      LDA   $0108,X
      STA   VGC_P1
      JSR   vgc_plot
      JSR   vgc_wait_cmd
      LDA   graph_color
      JMP   graph_set_vgc_color
.endif

.if .referenced(getpixel)
      .export getpixel
getpixel:
      TSX
      LDA   $0103,X
      STA   graph_arg_y
      LDA   $0104,X
      STA   graph_arg_y+1
      LDA   $0105,X
      STA   graph_arg_x
      LDA   $0106,X
      STA   graph_arg_x+1
      LDA   graph_arg_y
      STA   graph_tmp
      LDA   graph_arg_y+1
      STA   graph_tmp+1
      LDY   #6
@scale:
      ASL   graph_tmp
      ROL   graph_tmp+1
      DEY
      BNE   @scale
      CLC
      LDA   graph_tmp
      ADC   graph_arg_x
      STA   VGC_P1
      LDA   graph_tmp+1
      ADC   graph_arg_x+1
      STA   VGC_P2
      CLC
      ADC   graph_arg_y
      STA   VGC_P2
      LDA   #DMA_SPACE_VGC_GFX
      STA   VGC_P0
      STZ   VGC_P4
      JSR   vgc_mem_read
      LDA   VGC_P3
      LDX   #0
      RTS
.endif

.if .referenced(moveto)
      .export moveto
moveto:
      TSX
      LDA   $0103,X
      STA   graph_current_y
      LDA   $0104,X
      STA   graph_current_y+1
      LDA   $0105,X
      STA   graph_current_x
      LDA   $0106,X
      STA   graph_current_x+1
      RTS
.endif

.if .referenced(moverel)
      .export moverel
moverel:
      TSX
      CLC
      LDA   graph_current_y
      ADC   $0103,X
      STA   graph_current_y
      LDA   graph_current_y+1
      ADC   $0104,X
      STA   graph_current_y+1
      CLC
      LDA   graph_current_x
      ADC   $0105,X
      STA   graph_current_x
      LDA   graph_current_x+1
      ADC   $0106,X
      STA   graph_current_x+1
      RTS
.endif

.if .referenced(getx)
      .export getx
getx:
      LDA   graph_current_x
      LDX   graph_current_x+1
      RTS
.endif

.if .referenced(gety)
      .export gety
gety:
      LDA   graph_current_y
      LDX   graph_current_y+1
      RTS
.endif

.if .referenced(lineto)
      .export lineto
lineto:
      TSX
      LDA   $0103,X
      STA   graph_arg_y
      LDA   $0104,X
      STA   graph_arg_y+1
      LDA   $0105,X
      STA   graph_arg_x
      LDA   $0106,X
      STA   graph_arg_x+1
      JMP   graph_draw_to
.endif

.if .referenced(linerel)
      .export linerel
linerel:
      TSX
      CLC
      LDA   graph_current_y
      ADC   $0103,X
      STA   graph_arg_y
      LDA   graph_current_y+1
      ADC   $0104,X
      STA   graph_arg_y+1
      CLC
      LDA   graph_current_x
      ADC   $0105,X
      STA   graph_arg_x
      LDA   graph_current_x+1
      ADC   $0106,X
      STA   graph_arg_x+1
      JMP   graph_draw_to
.endif

.if .referenced(line)
      .export line
line:
      TSX
      LDA   $0109,X
      STA   VGC_P0
      LDA   $010A,X
      STA   VGC_P1
      LDA   $0107,X
      STA   VGC_P2
      LDA   $0108,X
      STA   VGC_P3
      LDA   $0105,X
      STA   VGC_P4
      LDA   $0106,X
      STA   VGC_P5
      LDA   $0103,X
      STA   VGC_P6
      LDA   $0104,X
      STA   VGC_P7
      JSR   vgc_line
      JMP   vgc_wait_cmd
.endif

.if .referenced(rectangle)
      .export rectangle
rectangle:
      TSX
      LDA   $0109,X
      STA   VGC_P0
      LDA   $010A,X
      STA   VGC_P1
      LDA   $0107,X
      STA   VGC_P2
      LDA   $0108,X
      STA   VGC_P3
      LDA   $0105,X
      STA   VGC_P4
      LDA   $0106,X
      STA   VGC_P5
      LDA   $0103,X
      STA   VGC_P6
      LDA   $0104,X
      STA   VGC_P7
      JSR   vgc_rect
      JMP   vgc_wait_cmd
.endif

.if .referenced(bar)
      .export bar
bar:
      TSX
      LDA   $0109,X
      STA   VGC_P0
      LDA   $010A,X
      STA   VGC_P1
      LDA   $0107,X
      STA   VGC_P2
      LDA   $0108,X
      STA   VGC_P3
      LDA   $0105,X
      STA   VGC_P4
      LDA   $0106,X
      STA   VGC_P5
      LDA   $0103,X
      STA   VGC_P6
      LDA   $0104,X
      STA   VGC_P7
      JSR   vgc_fill
      JMP   vgc_wait_cmd
.endif

.if .referenced(circle)
      .export circle
circle:
      TSX
      LDA   $0107,X
      STA   VGC_P0
      LDA   $0108,X
      STA   VGC_P1
      LDA   $0105,X
      STA   VGC_P2
      LDA   $0106,X
      STA   VGC_P3
      LDA   $0103,X
      STA   VGC_P4
      LDA   $0104,X
      STA   VGC_P5
      STZ   VGC_P6
      STZ   VGC_P7
      JSR   vgc_circle
      JMP   vgc_wait_cmd
.endif

; Nova's VGC fills the contiguous source-colour region, so Border is accepted
; for Turbo source compatibility but does not need a second software flooder.
.if .referenced(floodfill)
      .export floodfill
floodfill:
      TSX
      LDA   $0105,X
      STA   VGC_P2
      LDA   $0106,X
      STA   VGC_P3
      LDA   $0107,X
      STA   VGC_P0
      LDA   $0108,X
      STA   VGC_P1
      JSR   vgc_paint
      JMP   vgc_wait_cmd
.endif

.include "vgc.s"
.endif

.endif
