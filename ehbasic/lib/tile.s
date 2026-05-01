; Shared Nova tile engine helper routines.
;
; BASIC parses syntax and loads TileP*/TileAddr* registers. Assembly callers can
; load the same registers directly and call the command wrappers.

.include "tile.inc"

.ifndef TILE_IMPLEMENTATION_INCLUDED
TILE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export tile_cmd
      .export tile_set_size
      .export tile_set_mirror
      .export tile_set_trans_color
      .export tile_set_scroll
      .export tile_def
      .export tile_def_bulk
      .export tile_put
      .export tile_attr
      .export tile_fill
      .export tile_row
      .export tile_col
      .export tile_load
      .export tile_cls
      .export tile_buf_fill
      .export tile_buf_set
      .export tile_buf_put
      .export tile_buf_range
      .export tile_pal
      .export tile_palc

; Input: A = tile command. Does not wait for completion.
tile_cmd:
      STA   TileCmd
      RTS

; Input: X = tile size. X=16 sets 16x16 mode; all other values select 8x8.
tile_set_size:
      LDA   TileConfig
      AND   #$FE
      CPX   #16
      BNE   @store
      ORA   #TileCfgSize16
@store:
      STA   TileConfig
      RTS

; Input: X = mirror mode, low two bits.
tile_set_mirror:
      TXA
      AND   #$03
      ASL
      STA   NVR7L
      LDA   TileConfig
      AND   #$F9
      ORA   NVR7L
      STA   TileConfig
      RTS

; Input: X = transparent color.
tile_set_trans_color:
      STX   TileTransColor
      RTS

; Input: NVR0L/H = scroll X, NVR1L/H = scroll Y.
tile_set_scroll:
      LDA   NVR0L
      STA   TileScrollXL
      LDA   NVR0H
      STA   TileScrollXH
      LDA   NVR1L
      STA   TileScrollYL
      LDA   NVR1H
      STA   TileScrollYH
      RTS

tile_def:
      LDA   #TileCmdDef
      JMP   tile_cmd

tile_def_bulk:
      LDA   #TileCmdDefBulk
      JMP   tile_cmd

tile_put:
      LDA   #TileCmdPut
      JMP   tile_cmd

tile_attr:
      LDA   #TileCmdAttr
      JMP   tile_cmd

tile_fill:
      LDA   #TileCmdFill
      JMP   tile_cmd

tile_row:
      LDA   #TileCmdRow
      JMP   tile_cmd

tile_col:
      LDA   #TileCmdCol
      JMP   tile_cmd

tile_load:
      LDA   #TileCmdLoad
      JMP   tile_cmd

tile_cls:
      LDA   #TileCmdCls
      JMP   tile_cmd

tile_buf_fill:
      LDA   #TileCmdBufFill
      JMP   tile_cmd

tile_buf_set:
      LDA   #TileCmdBufSet
      JMP   tile_cmd

tile_buf_put:
      LDA   #TileCmdBufPut
      JMP   tile_cmd

tile_buf_range:
      LDA   #TileCmdBufRange
      JMP   tile_cmd

tile_pal:
      LDA   #TileCmdPal
      JMP   tile_cmd

tile_palc:
      LDA   #TileCmdPalC
      JMP   tile_cmd

.endif
