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
; @label TILE.COMMAND
; @kind routine
; @symbol tile_cmd
; @summary Issue a raw tile engine command.
; @in A: Tile command byte.
tile_cmd:
      STA   TileCmd
      RTS

; Input: X = tile size. X=16 sets 16x16 mode; all other values select 8x8.
; @label TILE.SET_SIZE
; @kind routine
; @symbol tile_set_size
; @summary Select 8x8 or 16x16 tile size.
; @in X: 16 selects 16x16; any other value selects 8x8.
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
; @label TILE.SET_MIRROR
; @kind routine
; @symbol tile_set_mirror
; @summary Set tile mirror mode from the low two bits of X.
; @in X: Mirror mode.
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
; @label TILE.SET_TRANS_COLOR
; @kind routine
; @symbol tile_set_trans_color
; @summary Set the tile transparent colour.
; @in X: Transparent colour byte.
tile_set_trans_color:
      STX   TileTransColor
      RTS

; Input: NVR0L/H = scroll X, NVR1L/H = scroll Y.
; @label TILE.SET_SCROLL
; @kind routine
; @symbol tile_set_scroll
; @summary Set tile scroll position.
; @in NVR0L: Scroll X low byte.
; @in NVR0H: Scroll X high byte.
; @in NVR1L: Scroll Y low byte.
; @in NVR1H: Scroll Y high byte.
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

; @label TILE.DEF
; @kind routine
; @symbol tile_def
; @summary Define one tile from TileP0 and TileAddrL/H.
tile_def:
      LDA   #TileCmdDef
      JMP   tile_cmd

; @label TILE.DEF_BULK
; @kind routine
; @symbol tile_def_bulk
; @summary Define a run of tiles from TileP0/TileP1 and TileAddrL/H.
tile_def_bulk:
      LDA   #TileCmdDefBulk
      JMP   tile_cmd

; @label TILE.PUT
; @kind routine
; @symbol tile_put
; @summary Put a tile using TileP0..TileP3.
tile_put:
      LDA   #TileCmdPut
      JMP   tile_cmd

; @label TILE.ATTR
; @kind routine
; @symbol tile_attr
; @summary Set a tile attribute using TileP0..TileP3.
tile_attr:
      LDA   #TileCmdAttr
      JMP   tile_cmd

; @label TILE.FILL
; @kind routine
; @symbol tile_fill
; @summary Fill tile memory using TileP0/TileP1.
tile_fill:
      LDA   #TileCmdFill
      JMP   tile_cmd

; @label TILE.ROW
; @kind routine
; @symbol tile_row
; @summary Copy a row of tile data from TileAddrL/H.
tile_row:
      LDA   #TileCmdRow
      JMP   tile_cmd

; @label TILE.COL
; @kind routine
; @symbol tile_col
; @summary Copy a column of tile data from TileAddrL/H.
tile_col:
      LDA   #TileCmdCol
      JMP   tile_cmd

; @label TILE.LOAD
; @kind routine
; @symbol tile_load
; @summary Load tile data from TileAddrL/H.
tile_load:
      LDA   #TileCmdLoad
      JMP   tile_cmd

; @label TILE.CLS
; @kind routine
; @symbol tile_cls
; @summary Clear tile memory.
tile_cls:
      LDA   #TileCmdCls
      JMP   tile_cmd

; @label TILE.BUF_FILL
; @kind routine
; @symbol tile_buf_fill
; @summary Fill the tile command buffer.
tile_buf_fill:
      LDA   #TileCmdBufFill
      JMP   tile_cmd

; @label TILE.BUF_SET
; @kind routine
; @symbol tile_buf_set
; @summary Set a tile command buffer entry.
tile_buf_set:
      LDA   #TileCmdBufSet
      JMP   tile_cmd

; @label TILE.BUF_PUT
; @kind routine
; @symbol tile_buf_put
; @summary Put a tile command buffer entry.
tile_buf_put:
      LDA   #TileCmdBufPut
      JMP   tile_cmd

; @label TILE.BUF_RANGE
; @kind routine
; @symbol tile_buf_range
; @summary Apply a range of tile command buffer entries.
tile_buf_range:
      LDA   #TileCmdBufRange
      JMP   tile_cmd

; @label TILE.PALETTE
; @kind routine
; @symbol tile_pal
; @summary Load tile palette data.
tile_pal:
      LDA   #TileCmdPal
      JMP   tile_cmd

; @label TILE.PALETTE_COLOR
; @kind routine
; @symbol tile_palc
; @summary Set one tile palette colour.
tile_palc:
      LDA   #TileCmdPalC
      JMP   tile_cmd

.endif
