; sprite_editor.s -- native Nova 16x16 sprite editor
;
; Load address: $7200   Invoke: SYS $7200
; Fixed v1 data file: FD0:SPRITE.SPR

.include "nova.inc"
.include "vtext.inc"
.include "editui.inc"
.include "xram.inc"

SPRITE_BYTES    = 256
SPRITE_XRAM_L   = $00
SPRITE_XRAM_M   = $FF
SPRITE_XRAM_H   = $06
GRID_X          = 18
GRID_Y          = 10
PREVIEW_X       = 58
PREVIEW_Y       = 10
PALETTE_X       = 3
PALETTE_Y       = 32
STATUS_Y        = 44

HEADER_BOX_X    = 1
HEADER_BOX_Y    = 2
HEADER_BOX_W    = 78
HEADER_BOX_H    = 4
TOOLS_BOX_X     = 1
TOOLS_BOX_Y     = 7
TOOLS_BOX_W     = 14
TOOLS_BOX_H     = 13
INFO_BOX_X      = 1
INFO_BOX_Y      = 21
INFO_BOX_W      = 14
INFO_BOX_H      = 8
PALETTE_BOX_X   = 1
PALETTE_BOX_Y   = 30
PALETTE_BOX_W   = 50
PALETTE_BOX_H   = 7
CANVAS_BOX_X    = 16
CANVAS_BOX_Y    = 7
CANVAS_BOX_W    = 35
CANVAS_BOX_H    = 22
PREVIEW_BOX_X   = 52
PREVIEW_BOX_Y   = 7
PREVIEW_BOX_W   = 27
PREVIEW_BOX_H   = 20
FRAMES_BOX_X    = 52
FRAMES_BOX_Y    = 28
FRAMES_BOX_W    = 27
FRAMES_BOX_H    = 6
LAYER_BOX_X     = 52
LAYER_BOX_Y     = 35
LAYER_BOX_W     = 13
LAYER_BOX_H     = 6
CURSOR_BOX_X    = 66
CURSOR_BOX_Y    = 35
CURSOR_BOX_W    = 13
CURSOR_BOX_H    = 6
STATUS_BOX_X    = 52
STATUS_BOX_Y    = 42
STATUS_BOX_W    = 27
STATUS_BOX_H    = 5

KEY_HELP        = $03
KEY_BACKSPACE   = $08
KEY_DELETE      = $7F
KEY_SPACE       = ' '
KEY_LEFT        = $1C
KEY_RIGHT       = $1D
KEY_UP          = $1E
KEY_DOWN        = $1F
KEY_CLEAR       = 'c'
KEY_CLEAR_UPPER = 'C'
KEY_ERASE       = '.'
KEY_ERASE_X     = 'x'
KEY_ERASE_X_UPPER = 'X'
KEY_EXPORT      = 'e'
KEY_EXPORT_UPPER = 'E'
KEY_LOAD        = 'l'
KEY_LOAD_UPPER  = 'L'
KEY_QUIT        = 'q'
KEY_QUIT_UPPER  = 'Q'
KEY_SAVE        = 's'
KEY_SAVE_UPPER  = 'S'

COL_BLACK       = 0
COL_WHITE       = 1
COL_RED         = 2
COL_CYAN        = 3
COL_GREEN       = 5
COL_BLUE        = 6
COL_YELLOW      = 7
COL_DGRAY       = 11
COL_MGRAY       = 12
COL_LGRAY       = 15

CLR_SHELL       = EDITUI_COLOR_SHELL
CLR_TITLE       = EDITUI_COLOR_TITLE
CLR_FRAME       = EDITUI_COLOR_FRAME
CLR_DIM         = EDITUI_COLOR_DIM
CLR_STATUS      = EDITUI_COLOR_STATUS
CLR_STATUS_HOT  = EDITUI_COLOR_STATUS_HOT
CLR_PANEL       = EDITUI_COLOR_PANEL
CLR_HILITE      = $31
CLR_TRANS_A     = $B1
CLR_TRANS_B     = $C1

MODE_TEXT_ONLY  = 0

.macro SET_COLOR color
      LDA   #color
      STA   VTEXT_COLOR
.endmacro

.macro PRINT_AT col, row, label
      LDA   #col
      STA   VTEXT_CURX
      LDA   #row
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   #<label
      LDY   #>label
      JSR   vtext_puts
.endmacro

.segment "ZEROPAGE"

zp_key:         .res 1
zp_row:         .res 1
zp_col:         .res 1
zp_idx:         .res 1
zp_tmp:         .res 1
zp_msg_l:       .res 1
zp_msg_h:       .res 1

.segment "BSS"

sprite_data:    .res SPRITE_BYTES
cursor_x:       .res 1
cursor_y:       .res 1
old_cursor_x:   .res 1
old_cursor_y:   .res 1
current_color:  .res 1

.segment "HEADER"
      .byte $00, $72

.segment "CODE"

start:
      JSR   editui_init
      JSR   vtext_screen_defaults
      JSR   clear_sprite
      STZ   cursor_x
      STZ   cursor_y
      LDA   #COL_WHITE
      STA   current_color
      LDA   #<msg_ready
      LDY   #>msg_ready
      JSR   set_message
      JSR   draw_all

main_loop:
      JSR   wait_key
      STA   zp_key

      CMP   #KEY_LEFT
      BNE   :+
      JMP   move_left
:
      CMP   #KEY_RIGHT
      BNE   :+
      JMP   move_right
:
      CMP   #KEY_UP
      BNE   :+
      JMP   move_up
:
      CMP   #KEY_DOWN
      BNE   :+
      JMP   move_down
:
      CMP   #KEY_SPACE
      BNE   :+
      JMP   paint_and_redraw
:
      CMP   #KEY_ERASE
      BNE   :+
      JMP   erase_and_redraw
:
      CMP   #KEY_ERASE_X
      BNE   :+
      JMP   erase_and_redraw
:
      CMP   #KEY_ERASE_X_UPPER
      BNE   :+
      JMP   erase_and_redraw
:
      CMP   #KEY_BACKSPACE
      BNE   :+
      JMP   erase_and_redraw
:
      CMP   #KEY_DELETE
      BNE   :+
      JMP   erase_and_redraw
:
      CMP   #KEY_HELP
      BNE   :+
      JMP   show_help
:
      CMP   #KEY_SAVE
      BNE   :+
      JMP   save_and_redraw
:
      CMP   #KEY_SAVE_UPPER
      BNE   :+
      JMP   save_and_redraw
:
      CMP   #KEY_LOAD
      BNE   :+
      JMP   load_and_redraw
:
      CMP   #KEY_LOAD_UPPER
      BNE   :+
      JMP   load_and_redraw
:
      CMP   #KEY_CLEAR
      BNE   :+
      JMP   clear_and_redraw
:
      CMP   #KEY_CLEAR_UPPER
      BNE   :+
      JMP   clear_and_redraw
:
      CMP   #KEY_EXPORT
      BNE   :+
      JMP   show_export
:
      CMP   #KEY_EXPORT_UPPER
      BNE   :+
      JMP   show_export
:
      CMP   #KEY_QUIT
      BNE   :+
      JMP   done
:
      CMP   #KEY_QUIT_UPPER
      BNE   :+
      JMP   done
:

      JSR   try_hex_key
      JMP   main_loop

done:
      LDA   #$0C
      STA   VTEXT_CHAR
      JSR   vtext_put_char
      RTS

move_left:
      JSR   remember_cursor
      LDA   cursor_x
      BEQ   @draw
      DEC   cursor_x
@draw:
      JSR   draw_old_and_current_cells
      JSR   draw_status
      JMP   main_loop

move_right:
      JSR   remember_cursor
      LDA   cursor_x
      CMP   #15
      BEQ   @draw
      INC   cursor_x
@draw:
      JSR   draw_old_and_current_cells
      JSR   draw_status
      JMP   main_loop

move_up:
      JSR   remember_cursor
      LDA   cursor_y
      BEQ   @draw
      DEC   cursor_y
@draw:
      JSR   draw_old_and_current_cells
      JSR   draw_status
      JMP   main_loop

move_down:
      JSR   remember_cursor
      LDA   cursor_y
      CMP   #15
      BEQ   @draw
      INC   cursor_y
@draw:
      JSR   draw_old_and_current_cells
      JSR   draw_status
      JMP   main_loop

paint_and_redraw:
      JSR   paint_current
      JSR   draw_current_cell_pair
      JSR   draw_status
      JMP   main_loop

erase_and_redraw:
      JSR   erase_current
      JSR   draw_current_cell_pair
      JSR   draw_status
      JMP   main_loop

save_and_redraw:
      JSR   save_sprite
      JSR   draw_status
      JMP   main_loop

load_and_redraw:
      JSR   load_sprite
      JSR   draw_grid
      JSR   draw_preview
      JSR   draw_status
      JMP   main_loop

clear_and_redraw:
      JSR   clear_sprite
      LDA   #<msg_cleared
      LDY   #>msg_cleared
      JSR   set_message
      JSR   draw_grid
      JSR   draw_preview
      JSR   draw_status
      JMP   main_loop

try_hex_key:
      LDA   zp_key
      CMP   #'0'
      BCC   @upper
      CMP   #'9' + 1
      BCS   @upper
      SEC
      SBC   #'0'
      BRA   @ok
@upper:
      CMP   #'A'
      BCC   @lower
      CMP   #'F' + 1
      BCS   @lower
      SEC
      SBC   #'A' - 10
      BRA   @ok
@lower:
      CMP   #'a'
      BCC   @bad
      CMP   #'f' + 1
      BCS   @bad
      SEC
      SBC   #'a' - 10
@ok:
      AND   #$0F
      STA   current_color
      JSR   paint_current
      JSR   draw_current_cell_pair
      JSR   draw_palette
      JSR   draw_status
      SEC
      RTS
@bad:
      CLC
      RTS

wait_key:
@wait:
      LDA   VGC_CHARIN
      BEQ   @wait
      RTS

vtext_screen_defaults:
      STZ   VTEXT_LEFT
      STZ   VTEXT_TOP
      LDA   #VTEXT_SCREEN_COLS
      STA   VTEXT_WIDTH
      LDA   #VTEXT_SCREEN_ROWS
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #CLR_SHELL
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      RTS

clear_screen:
      JSR   vtext_screen_defaults
      LDA   #CLR_SHELL
      STA   VTEXT_COLOR
      LDA   #' '
      STA   VTEXT_CHAR
      JMP   vtext_clear_region

draw_all:
      JSR   draw_static
      JSR   draw_boxes
      JSR   draw_grid
      JSR   draw_preview
      JSR   draw_palette
      JMP   draw_status

draw_static:
      JSR   editui_clear_screen
      SET_COLOR CLR_STATUS
      LDA   #0
      JSR   fill_row
      PRINT_AT 2, 0, msg_menu
      SET_COLOR CLR_STATUS
      LDA   #49
      JSR   fill_row
      PRINT_AT 1, 49, msg_keys
      SET_COLOR CLR_TITLE
      PRINT_AT 23, 3, msg_title
      SET_COLOR CLR_DIM
      PRINT_AT 15, 4, msg_file
      RTS

draw_boxes:
      LDA   #HEADER_BOX_X
      STA   EDITUI_BOXX
      LDA   #HEADER_BOX_Y
      STA   EDITUI_BOXY
      LDA   #HEADER_BOX_W
      STA   EDITUI_BOXW
      LDA   #HEADER_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_no_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_no_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #TOOLS_BOX_X
      STA   EDITUI_BOXX
      LDA   #TOOLS_BOX_Y
      STA   EDITUI_BOXY
      LDA   #TOOLS_BOX_W
      STA   EDITUI_BOXW
      LDA   #TOOLS_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_tools_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_tools_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #INFO_BOX_X
      STA   EDITUI_BOXX
      LDA   #INFO_BOX_Y
      STA   EDITUI_BOXY
      LDA   #INFO_BOX_W
      STA   EDITUI_BOXW
      LDA   #INFO_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_info_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_info_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #PALETTE_BOX_X
      STA   EDITUI_BOXX
      LDA   #PALETTE_BOX_Y
      STA   EDITUI_BOXY
      LDA   #PALETTE_BOX_W
      STA   EDITUI_BOXW
      LDA   #PALETTE_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_palette_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_palette_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #CANVAS_BOX_X
      STA   EDITUI_BOXX
      LDA   #CANVAS_BOX_Y
      STA   EDITUI_BOXY
      LDA   #CANVAS_BOX_W
      STA   EDITUI_BOXW
      LDA   #CANVAS_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_canvas_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_canvas_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #PREVIEW_BOX_X
      STA   EDITUI_BOXX
      LDA   #PREVIEW_BOX_Y
      STA   EDITUI_BOXY
      LDA   #PREVIEW_BOX_W
      STA   EDITUI_BOXW
      LDA   #PREVIEW_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_preview_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_preview_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #FRAMES_BOX_X
      STA   EDITUI_BOXX
      LDA   #FRAMES_BOX_Y
      STA   EDITUI_BOXY
      LDA   #FRAMES_BOX_W
      STA   EDITUI_BOXW
      LDA   #FRAMES_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_frames_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_frames_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #LAYER_BOX_X
      STA   EDITUI_BOXX
      LDA   #LAYER_BOX_Y
      STA   EDITUI_BOXY
      LDA   #LAYER_BOX_W
      STA   EDITUI_BOXW
      LDA   #LAYER_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_layer_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_layer_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #CURSOR_BOX_X
      STA   EDITUI_BOXX
      LDA   #CURSOR_BOX_Y
      STA   EDITUI_BOXY
      LDA   #CURSOR_BOX_W
      STA   EDITUI_BOXW
      LDA   #CURSOR_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_cursor_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_cursor_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      LDA   #STATUS_BOX_X
      STA   EDITUI_BOXX
      LDA   #STATUS_BOX_Y
      STA   EDITUI_BOXY
      LDA   #STATUS_BOX_W
      STA   EDITUI_BOXW
      LDA   #STATUS_BOX_H
      STA   EDITUI_BOXH
      LDA   #<msg_status_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>msg_status_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      JSR   draw_tools_panel
      JSR   draw_info_panel
      JSR   draw_canvas_labels
      JSR   draw_preview_labels
      JSR   draw_frames_panel
      JSR   draw_layer_panel
      RTS

fill_row:
      STA   zp_tmp
      LDA   VTEXT_COLOR
      PHA
      JSR   vtext_screen_defaults
      PLA
      STA   VTEXT_COLOR
      LDA   zp_tmp
      STA   VTEXT_TOP
      LDA   #1
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #' '
      STA   VTEXT_CHAR
      JSR   vtext_clear_region
      JMP   vtext_screen_defaults

draw_tools_panel:
      SET_COLOR CLR_HILITE
      PRINT_AT 3, 9, msg_tool_pencil
      SET_COLOR CLR_TITLE
      PRINT_AT 3, 10, msg_tool_fill
      PRINT_AT 3, 11, msg_tool_eraser
      PRINT_AT 3, 12, msg_tool_line
      PRINT_AT 3, 13, msg_tool_rect
      PRINT_AT 3, 14, msg_tool_rect_fill
      PRINT_AT 3, 15, msg_tool_flip_h
      PRINT_AT 3, 16, msg_tool_flip_v
      PRINT_AT 3, 17, msg_tool_rotate_l
      PRINT_AT 3, 18, msg_tool_rotate_r
      RTS

draw_info_panel:
      SET_COLOR CLR_TITLE
      PRINT_AT 3, 23, msg_info_width
      PRINT_AT 3, 24, msg_info_height
      PRINT_AT 3, 25, msg_info_colors
      PRINT_AT 3, 26, msg_info_bpp
      PRINT_AT 3, 27, msg_info_size
      RTS

draw_canvas_labels:
      SET_COLOR CLR_TITLE
      PRINT_AT 18, 9, msg_canvas_cols
      PRINT_AT 17, 10, msg_row_0
      PRINT_AT 17, 11, msg_row_1
      PRINT_AT 17, 12, msg_row_2
      PRINT_AT 17, 13, msg_row_3
      PRINT_AT 17, 14, msg_row_4
      PRINT_AT 17, 15, msg_row_5
      PRINT_AT 17, 16, msg_row_6
      PRINT_AT 17, 17, msg_row_7
      PRINT_AT 17, 18, msg_row_8
      PRINT_AT 17, 19, msg_row_9
      PRINT_AT 17, 20, msg_row_a
      PRINT_AT 17, 21, msg_row_b
      PRINT_AT 17, 22, msg_row_c
      PRINT_AT 17, 23, msg_row_d
      PRINT_AT 17, 24, msg_row_e
      PRINT_AT 17, 25, msg_row_f
      SET_COLOR CLR_DIM
      PRINT_AT 21, 27, msg_canvas_hint
      RTS

draw_preview_labels:
      SET_COLOR CLR_TITLE
      PRINT_AT 55, 9, msg_preview_actual
      RTS

draw_frames_panel:
      SET_COLOR CLR_DIM
      PRINT_AT 55, 30, msg_frames_hint
      SET_COLOR CLR_TITLE
      PRINT_AT 55, 31, msg_frames_list
      RTS

draw_layer_panel:
      SET_COLOR CLR_HILITE
      PRINT_AT 54, 37, msg_layer_zero
      RTS

draw_grid:
      STZ   zp_row
@row:
      STZ   zp_col
@col:
      JSR   draw_editor_cell_at_zp
      INC   zp_col
      LDA   zp_col
      CMP   #16
      BNE   @col
      INC   zp_row
      LDA   zp_row
      CMP   #16
      BEQ   @done
      JMP   @row
@done:
      RTS

draw_preview:
      STZ   zp_row
@row:
      STZ   zp_col
@col:
      JSR   draw_preview_cell_at_zp
      INC   zp_col
      LDA   zp_col
      CMP   #16
      BNE   @col
      INC   zp_row
      LDA   zp_row
      CMP   #16
      BNE   @row
      RTS

draw_editor_cell_at_zp:
      LDA   zp_col
      ASL
      CLC
      ADC   #GRID_X
      STA   VTEXT_CURX
      LDA   zp_row
      CLC
      ADC   #GRID_Y
      STA   VTEXT_CURY
      JSR   vtext_set_cursor

      STZ   VTEXT_ATTR
      LDA   zp_col
      CMP   cursor_x
      BNE   @cell
      LDA   zp_row
      CMP   cursor_y
      BNE   @cell
      LDA   #VTEXT_ATTR_FLASH
      STA   VTEXT_ATTR

@cell:
      JSR   calc_zp_index
      LDA   sprite_data,X
      AND   #$0F
      JSR   set_editor_cell_color
      LDA   #' '
      JSR   vputc
      LDA   #' '
      JSR   vputc
      STZ   VTEXT_ATTR
      RTS

draw_preview_cell_at_zp:
      LDA   zp_col
      CLC
      ADC   #PREVIEW_X
      STA   VTEXT_CURX
      LDA   zp_row
      CLC
      ADC   #PREVIEW_Y
      STA   VTEXT_CURY
      JSR   vtext_set_cursor

      STZ   VTEXT_ATTR
      JSR   calc_zp_index
      LDA   sprite_data,X
      AND   #$0F
      JSR   set_preview_cell_color
      LDA   #' '
      JSR   vputc
      LDA   #' '
      JSR   vputc
      RTS

draw_current_cell_pair:
      LDA   cursor_x
      STA   zp_col
      LDA   cursor_y
      STA   zp_row
      JSR   draw_editor_cell_at_zp
      JMP   draw_preview_cell_at_zp

draw_old_and_current_cells:
      LDA   old_cursor_x
      STA   zp_col
      LDA   old_cursor_y
      STA   zp_row
      JSR   draw_editor_cell_at_zp
      JMP   draw_current_cell_pair

set_editor_cell_color:
      AND   #$0F
      BNE   pack_block_color
      LDA   zp_row
      EOR   zp_col
      AND   #$01
      BEQ   @trans_a
      LDA   #CLR_TRANS_B
      STA   VTEXT_COLOR
      RTS
@trans_a:
      LDA   #CLR_TRANS_A
      STA   VTEXT_COLOR
      RTS

set_preview_cell_color:
      AND   #$0F
      BNE   pack_block_color
      LDA   #CLR_SHELL
      STA   VTEXT_COLOR
      RTS

set_pixel_text_color:
      AND   #$0F
      BNE   @fg
      LDA   #COL_DGRAY
@fg:
      ORA   #$60
      STA   VTEXT_COLOR
      RTS

pack_block_color:
      STA   zp_tmp
      ASL
      ASL
      ASL
      ASL
      ORA   zp_tmp
      STA   VTEXT_COLOR
      RTS

draw_palette:
      LDA   #PALETTE_X
      STA   VTEXT_CURX
      LDA   #PALETTE_Y
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      STZ   zp_col
@digits:
      LDA   zp_col
      ASL
      CLC
      ADC   zp_col
      CLC
      ADC   #PALETTE_X
      STA   VTEXT_CURX
      LDA   #PALETTE_Y
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   zp_col
      JSR   set_pixel_text_color
      LDA   zp_col
      JSR   vtext_put_hex_nibble
      INC   zp_col
      LDA   zp_col
      CMP   #16
      BNE   @digits

      STZ   zp_col
@swatch:
      LDA   zp_col
      ASL
      CLC
      ADC   zp_col
      CLC
      ADC   #PALETTE_X
      STA   VTEXT_CURX
      LDA   #PALETTE_Y + 1
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   zp_col
      JSR   pack_block_color
      LDA   #' '
      JSR   vputc
      LDA   #' '
      JSR   vputc
      INC   zp_col
      LDA   zp_col
      CMP   #16
      BNE   @swatch

      STZ   zp_col
@marker:
      LDA   zp_col
      ASL
      CLC
      ADC   zp_col
      CLC
      ADC   #PALETTE_X
      STA   VTEXT_CURX
      LDA   #PALETTE_Y + 3
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   zp_col
      CMP   current_color
      BNE   @space
      SET_COLOR CLR_STATUS_HOT
      LDA   #'^'
      JSR   vputc
      LDA   #'^'
      JSR   vputc
      BRA   @next_marker
@space:
      SET_COLOR CLR_DIM
      LDA   #' '
      JSR   vputc
      LDA   #' '
      JSR   vputc
@next_marker:
      INC   zp_col
      LDA   zp_col
      CMP   #16
      BNE   @marker
      RTS

draw_status:
      JSR   clear_status_lines
      SET_COLOR CLR_TITLE
      PRINT_AT 68, 37, msg_cursor_x
      LDA   cursor_x
      JSR   vtext_put_hex_nibble
      SET_COLOR CLR_TITLE
      PRINT_AT 74, 37, msg_cursor_y
      LDA   cursor_y
      JSR   vtext_put_hex_nibble
      SET_COLOR CLR_TITLE
      PRINT_AT 68, 38, msg_index
      JSR   calc_index
      TXA
      JSR   vtext_put_hex_byte
      SET_COLOR CLR_TITLE
      PRINT_AT 68, 39, msg_color
      LDA   current_color
      JSR   vtext_put_hex_nibble
      LDA   #75
      STA   VTEXT_CURX
      LDA   #39
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   current_color
      JSR   pack_block_color
      LDA   #' '
      JSR   vputc
      LDA   #' '
      JSR   vputc
      SET_COLOR CLR_TITLE
      PRINT_AT 54, 44, msg_status_file
      SET_COLOR CLR_DIM
      PRINT_AT 54, 45, msg_status_size
      SET_COLOR CLR_TITLE
      PRINT_AT 54, 46, msg_message
      LDA   zp_msg_l
      LDY   zp_msg_h
      JMP   vtext_puts

clear_status_lines:
      SET_COLOR CLR_SHELL
      PRINT_AT 68, 37, msg_blank_10
      PRINT_AT 68, 38, msg_blank_10
      PRINT_AT 68, 39, msg_blank_10
      PRINT_AT 53, 44, msg_blank_25
      PRINT_AT 53, 45, msg_blank_25
      PRINT_AT 53, 46, msg_blank_25
      RTS

set_message:
      STA   zp_msg_l
      STY   zp_msg_h
      RTS

vputc:
      STA   VTEXT_CHAR
      JMP   vtext_put_char

calc_index:
      LDA   cursor_y
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   cursor_x
      TAX
      RTS

calc_zp_index:
      LDA   zp_row
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   zp_col
      TAX
      RTS

remember_cursor:
      LDA   cursor_x
      STA   old_cursor_x
      LDA   cursor_y
      STA   old_cursor_y
      RTS

paint_current:
      JSR   calc_index
      LDA   current_color
      STA   sprite_data,X
      LDA   #<msg_painted
      LDY   #>msg_painted
      JMP   set_message

erase_current:
      JSR   calc_index
      LDA   #0
      STA   sprite_data,X
      LDA   #<msg_erased
      LDY   #>msg_erased
      JMP   set_message

clear_sprite:
      LDX   #0
      LDA   #0
@loop:
      STA   sprite_data,X
      INX
      BNE   @loop
      RTS

set_xram_sprite_args:
      LDA   #SPRITE_XRAM_L
      STA   XRAM_ADDRL
      LDA   #SPRITE_XRAM_M
      STA   XRAM_ADDRM
      LDA   #SPRITE_XRAM_H
      STA   XRAM_ADDRH
      LDA   #<sprite_data
      STA   XRAM_RAML
      LDA   #>sprite_data
      STA   XRAM_RAMH
      LDA   #<SPRITE_BYTES
      STA   XRAM_LENL
      LDA   #>SPRITE_BYTES
      STA   XRAM_LENH
      LDA   #filename_end - filename
      STA   XRAM_NAMELEN
      LDA   #<filename
      STA   XRAM_NAMEPTR_L
      LDA   #>filename
      STA   XRAM_NAMEPTR_H
      RTS

save_sprite:
      JSR   set_xram_sprite_args
      JSR   xram_copy_from_ram
      BNE   @err
      JSR   set_xram_sprite_args
      JSR   xram_xsave
      BNE   @err
      LDA   #<msg_saved
      LDY   #>msg_saved
      JMP   set_message
@err:
      LDA   #<msg_save_error
      LDY   #>msg_save_error
      JMP   set_message

load_sprite:
      JSR   set_xram_sprite_args
      JSR   xram_xload
      BNE   @err
      LDA   XRAM_LENL
      CMP   #<SPRITE_BYTES
      BNE   @err
      LDA   XRAM_LENH
      CMP   #>SPRITE_BYTES
      BNE   @err
      JSR   set_xram_sprite_args
      JSR   xram_copy_to_ram
      BNE   @err
      LDA   #<msg_loaded
      LDY   #>msg_loaded
      JMP   set_message
@err:
      LDA   #<msg_load_error
      LDY   #>msg_load_error
      JMP   set_message

show_help:
      JSR   clear_screen
      SET_COLOR CLR_TITLE
      PRINT_AT 2, 0, help_title
      SET_COLOR CLR_DIM
      PRINT_AT 2, 2, help_1
      PRINT_AT 2, 3, help_2
      PRINT_AT 2, 4, help_3
      PRINT_AT 2, 5, help_4
      PRINT_AT 2, 6, help_5
      PRINT_AT 2, 7, help_6
      PRINT_AT 2, 9, help_7
      PRINT_AT 2, 11, help_footer
      JSR   wait_key
      JSR   draw_all
      JMP   main_loop

show_export:
      JSR   clear_screen
      SET_COLOR CLR_TITLE
      PRINT_AT 2, 0, export_title
      SET_COLOR CLR_DIM
      PRINT_AT 2, 1, export_note
      STZ   zp_row
      STZ   zp_idx
@row:
      LDA   #2
      STA   VTEXT_CURX
      LDA   zp_row
      CLC
      ADC   #4
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      SET_COLOR CLR_DIM
      LDA   #<export_prefix
      LDY   #>export_prefix
      JSR   vtext_puts
      STZ   zp_col
@col:
      SET_COLOR CLR_DIM
      LDA   #'$'
      JSR   vputc
      SET_COLOR CLR_TITLE
      LDX   zp_idx
      LDA   sprite_data,X
      JSR   vtext_put_hex_nibble
      INC   zp_idx
      INC   zp_col
      LDA   zp_col
      CMP   #16
      BEQ   @next_row
      SET_COLOR CLR_DIM
      LDA   #','
      JSR   vputc
      BRA   @col
@next_row:
      INC   zp_row
      LDA   zp_row
      CMP   #16
      BNE   @row
      SET_COLOR CLR_DIM
      PRINT_AT 2, 22, export_footer
      JSR   wait_key
      JSR   draw_all
      JMP   main_loop

.segment "RODATA"

msg_title:
      .byte "NOVA SPRITE EDITOR v1.0", 0
msg_menu:
      .byte "File  Edit  Sprite  View  Tools  Export  Help", 0
msg_keys:
      .byte "Arrows:Move Space:Paint X:Erase F1:Help F2:New F3:Open F4:Save F5:Export", 0
msg_file:
      .byte "Sprite: 16x16  |  File: SPRITE.SPR  |  Palette: DEFAULT.PAL", 0
msg_no_title:
      .byte 0
msg_tools_title:
      .byte " TOOLS ", 0
msg_info_title:
      .byte " SPRITE INFO ", 0
msg_palette_title:
      .byte " PALETTE (16 COLORS) ", 0
msg_canvas_title:
      .byte " SPRITE CANVAS (16X16) ", 0
msg_preview_title:
      .byte " PREVIEW ", 0
msg_frames_title:
      .byte " FRAMES ", 0
msg_layer_title:
      .byte " LAYER ", 0
msg_cursor_title:
      .byte " CURSOR ", 0
msg_status_title:
      .byte " STATUS ", 0
msg_message:
      .byte "STATUS: ", 0
msg_tool_pencil:
      .byte " Pencil", 0
msg_tool_fill:
      .byte " Fill", 0
msg_tool_eraser:
      .byte " Eraser", 0
msg_tool_line:
      .byte " Line", 0
msg_tool_rect:
      .byte " Rect", 0
msg_tool_rect_fill:
      .byte " Rect Fill", 0
msg_tool_flip_h:
      .byte " Flip H", 0
msg_tool_flip_v:
      .byte " Flip V", 0
msg_tool_rotate_l:
      .byte " Rotate L", 0
msg_tool_rotate_r:
      .byte " Rotate R", 0
msg_info_width:
      .byte "Width : 16", 0
msg_info_height:
      .byte "Height: 16", 0
msg_info_colors:
      .byte "Colors: 16", 0
msg_info_bpp:
      .byte "BPP   : 4", 0
msg_info_size:
      .byte "Size  : 256", 0
msg_canvas_cols:
      .byte "0 1 2 3 4 5 6 7 8 9 A B C D E F", 0
msg_canvas_hint:
      .byte "SPACE paints current color", 0
msg_preview_actual:
      .byte "1x actual", 0
msg_preview_note:
      .byte "single-frame preview", 0
msg_frames_hint:
      .byte "00  01  02  03", 0
msg_frames_list:
      .byte "[*] [ ] [ ] [ ]", 0
msg_layer_zero:
      .byte "0: Layer 0", 0
msg_cursor_x:
      .byte "X:", 0
msg_cursor_y:
      .byte "Y:", 0
msg_index:
      .byte "I:$", 0
msg_color:
      .byte "C:$", 0
msg_status_file:
      .byte "File: SPRITE.SPR", 0
msg_status_size:
      .byte "Size: 256 bytes", 0
msg_blank_10:
      .byte "          ", 0
msg_blank_23:
      .byte "                       ", 0
msg_blank_25:
      .byte "                         ", 0
msg_row_0:
      .byte "0", 0
msg_row_1:
      .byte "1", 0
msg_row_2:
      .byte "2", 0
msg_row_3:
      .byte "3", 0
msg_row_4:
      .byte "4", 0
msg_row_5:
      .byte "5", 0
msg_row_6:
      .byte "6", 0
msg_row_7:
      .byte "7", 0
msg_row_8:
      .byte "8", 0
msg_row_9:
      .byte "9", 0
msg_row_a:
      .byte "A", 0
msg_row_b:
      .byte "B", 0
msg_row_c:
      .byte "C", 0
msg_row_d:
      .byte "D", 0
msg_row_e:
      .byte "E", 0
msg_row_f:
      .byte "F", 0
msg_ready:
      .byte "READY", 0
msg_painted:
      .byte "PAINTED", 0
msg_erased:
      .byte "ERASED", 0
msg_cleared:
      .byte "CLEARED", 0
msg_saved:
      .byte "SAVED", 0
msg_save_error:
      .byte "SAVE ERR", 0
msg_loaded:
      .byte "LOADED", 0
msg_load_error:
      .byte "LOAD ERR", 0

help_title:
      .byte "NOVA SPRITE EDITOR HELP", 0
help_1:
      .byte "This first editor works on one 16x16, 4-bit sprite buffer.", 0
help_2:
      .byte "Arrow keys move the cursor. Hex keys 0-F select that color and paint.", 0
help_3:
      .byte "SPACE paints with the current color. X, period, backspace, or delete erases.", 0
help_4:
      .byte "S saves the 256-byte shape buffer. L loads it back from FD0:SPRITE.SPR.", 0
help_5:
      .byte "E shows a ca65 .byte export view for the current buffer.", 0
help_6:
      .byte "Color 0 is transparent/black. The grid displays it as dim gray for editing.", 0
help_7:
      .byte "Next obvious reusable pieces: filename prompt, menus, and editable text fields.", 0
help_footer:
      .byte "PRESS ANY KEY", 0

export_title:
      .byte "CA65 SPRITE DATA EXPORT", 0
export_note:
      .byte "Each line is one sprite row. Values are 4-bit palette indices.", 0
export_prefix:
      .byte ".byte ", 0
export_footer:
      .byte "PRESS ANY KEY", 0

filename:
      .byte "FD0:SPRITE.SPR"
filename_end:
