; Nova Chess runtime scaffold.
;
; This first checkpoint owns the machine, draws a Nova-native board, and proves
; blitter-backed virtual sprites for chess pieces. The engine will be linked
; behind this runtime boundary once the shared chess engine API stabilizes.

.setcpu "65c02"

.include "vsprite.inc"
.include "vtext.inc"
.include "fio.inc"
.include "nvg.inc"
.include "tween.inc"
.include "nchess_net.inc"
.include "nui.inc"
.include "engine_labels.inc"

BOARD_X              = 10
BOARD_Y              = 6
SQUARE_SIZE          = 22
PIECE_SIZE           = 20
PIECE_OFFSET         = 1
CASTLE_STRIP_X       = BOARD_X + PIECE_OFFSET
CASTLE_STRIP_WIDTH   = SQUARE_SIZE * 7 + PIECE_SIZE
PIECE_ROW_BYTES      = 3
PIECE_TAIL_BITS      = 4
LABEL_WIDTH          = 5
LABEL_HEIGHT         = 7
FILE_LABEL_Y         = 188
RANK_LABEL_X         = 194
.ifndef DEMO_STEP_PAUSE
DEMO_STEP_PAUSE      = 1
.endif
.ifndef CASTLE_STEP_PAUSE
CASTLE_STEP_PAUSE    = 0
.endif
.ifndef DEMO_FINAL_PAUSE
DEMO_FINAL_PAUSE     = 120
.endif
.ifndef DEMO_PIXEL_STEP
DEMO_PIXEL_STEP      = 1
.endif
.ifndef DEMO_REPEAT
DEMO_REPEAT          = 1
.endif
.ifndef CHESS_RENDER_TEST
CHESS_RENDER_TEST    = 0
.endif
.ifndef CHESS_CASTLE_TEST
CHESS_CASTLE_TEST    = 0
.endif
.ifndef CHESS_SELFPLAY_DEMO
CHESS_SELFPLAY_DEMO  = 1
.endif
.ifndef CHESS_AUTOSTART_DEMO
CHESS_AUTOSTART_DEMO = 0
.endif
.ifndef CHESS_SPLASH_FRAMES
CHESS_SPLASH_FRAMES  = 30000
.endif
.ifndef SELFPLAY_MOVE_PAUSE
SELFPLAY_MOVE_PAUSE  = 24
.endif
DEMO_STEPS_22        = 22 / DEMO_PIXEL_STEP
DEMO_STEPS_44        = 44 / DEMO_PIXEL_STEP
DEMO_STEPS_66        = 66 / DEMO_PIXEL_STEP
DEMO_STEPS_88        = 88 / DEMO_PIXEL_STEP
DEMO_MOVE_COUNT      = 12
PANEL_X              = 210
PANEL_Y              = 16
PANEL_W              = 96
PANEL_H              = 168
PANEL_TEXT_X         = 54
PANEL_MOVE_ROWS      = 6
PANEL_LINE_LEN       = 13
PANEL_LAST_MOVE_OFF  = (PANEL_MOVE_ROWS - 1) * PANEL_LINE_LEN
INITIAL_MATERIAL     = 39
FRAMES_PER_SECOND    = 60

COLOR_BLACK          = 0
COLOR_WHITE          = 1
COLOR_RED            = 10
COLOR_TRANSPARENT    = 2
COLOR_CYAN           = 3
COLOR_GREEN          = 5
COLOR_YELLOW         = 7
COLOR_DARK_SQUARE    = 12
COLOR_LIGHT_SQUARE   = 15
COLOR_ACCENT         = 14
COLOR_FRAME          = 15
COLOR_SHADOW         = 11
COLOR_WHITE_PIECE    = 1
COLOR_BLACK_PIECE    = 0
COLOR_WHITE_DETAIL   = 11
COLOR_BLACK_DETAIL   = 15
COLOR_BLACK_REMAP_MAIN = 11
COLOR_BLACK_REMAP_HILITE = 12
COLOR_BLACK_REMAP_SHADOW = 0

PIECE_EMPTY          = 0
PIECE_PAWN           = 1
PIECE_KNIGHT         = 2
PIECE_BISHOP         = 3
PIECE_ROOK           = 4
PIECE_QUEEN          = 5
PIECE_KING           = 6
PIECE_BLACK          = 8

B_PAWN               = PIECE_BLACK | PIECE_PAWN
B_KNIGHT             = PIECE_BLACK | PIECE_KNIGHT
B_BISHOP             = PIECE_BLACK | PIECE_BISHOP
B_ROOK               = PIECE_BLACK | PIECE_ROOK
B_QUEEN              = PIECE_BLACK | PIECE_QUEEN
B_KING               = PIECE_BLACK | PIECE_KING
ENGINE_START_SPRITE_PTR = $30
ENGINE_EMPTY_PIECE   = $30
ENGINE_WHITE_COLOR   = $80
ENGINE_WHITES_TURN   = $01
ENGINE_BLACKS_TURN   = $00
GAME_NORMAL               = $00
GAME_CHECK                = $01
GAME_CHECKMATE            = $02
GAME_STALEMATE            = $03
GAME_DRAW_50_MOVE         = $04
GAME_DRAW_REPETITION      = $05
GAME_DRAW_INSUFFICIENT    = $06
GAME_DRAW_75_MOVE         = $07
GAME_DRAW_REPETITION_AUTO = $08
PROMO_FLAG_KNIGHT         = $80

PLAYMODE_DEMO             = $00
PLAYMODE_HUMAN_VS_NOVA    = $01
PLAYMODE_NOVA_VS_NOVA     = $02
PLAYMODE_HUMAN_VS_HUMAN   = $03
PLAYMODE_NETWORK          = $04

DIFFICULTY_EASY           = $00
DIFFICULTY_MEDIUM         = $01
DIFFICULTY_HARD           = $02

INPUT_BUFFER_LEN          = 5
INPUT_ENTRY_X             = PANEL_TEXT_X + 5
INPUT_ERROR_Y             = 25
INPUT_STATE_NONE          = $00
INPUT_STATE_FLASH         = $01
INPUT_STATE_INVALID       = $02
INPUT_BLINK_SHIFT         = 4
INPUT_BLINK_FRAMES        = 10

.segment "ZEROPAGE"
msg_ptr:             .res 2
mask_ptr:            .res 2
buf_ptr:             .res 2

.segment "BSS"
board_row:           .res 1
board_file:          .res 1
board_index:         .res 1
square_xl:           .res 1
square_xh:           .res 1
square_y:            .res 1
piece_code:          .res 1
piece_type:          .res 1
mask_byte:           .res 1
bit_count:           .res 1
row_count:           .res 1
text_x:              .res 1
text_y:              .res 1
move_index:          .res 1
anim_x:              .res 1
anim_y:              .res 1
anim_dx:             .res 1
anim_dy:             .res 1
anim_path_count:     .res 1
anim_start_x:        .res 1
anim_start_y:        .res 1
anim_piece_code:     .res 1
anim_bg_x:           .res 1
anim_bg_y:           .res 1
castle_rook_x:       .res 1
castle_rook_y:       .res 1
castle_rook_dx:      .res 1
castle_rook_steps:   .res 1
castle_king_steps:   .res 1
castle_tween_duration:.res 1
castle_king_start_x: .res 1
castle_king_end_x:   .res 1
castle_rook_start_x: .res 1
castle_rook_end_x:   .res 1
castle_rook_piece:   .res 1
castle_rook_from:    .res 1
castle_rook_to:      .res 1
engine_index:        .res 1
self_from:           .res 1
self_to:             .res 1
from_board_index:    .res 1
to_board_index:      .res 1
game_state:          .res 1
last_frame:          .res 1
frame_delta:         .res 1
white_clock_frames:  .res 1
black_clock_frames:  .res 1
white_clock_seconds: .res 1
black_clock_seconds: .res 1
white_clock_minutes: .res 1
black_clock_minutes: .res 1
fullmove_number:     .res 1
white_material:      .res 1
black_material:      .res 1
white_capture_score: .res 1
black_capture_score: .res 1
captured_piece:      .res 1
en_passant_capture:  .res 1
digit_tens:          .res 1
digit_ones:          .res 1
from_file:           .res 1
from_row:            .res 1
to_file:             .res 1
to_row:              .res 1
abs_file_delta:      .res 1
abs_row_delta:       .res 1
square_tmp:          .res 1
to_x:                .res 1
to_y:                .res 1
play_mode:           .res 1
human_side:          .res 1
selected_difficulty: .res 1
input_len:           .res 1
input_from:          .res 1
input_to:            .res 1
input_promo:         .res 1
legal_index:         .res 1
menu_key:            .res 1
input_key:           .res 1
input_from_state:    .res 1
input_to_state:      .res 1
input_blink_phase:   .res 1
input_blink_tick:    .res 1
input_blink_frame:   .res 1
input_blink_color:   .res 1
input_display_len:   .res 1
input_error_l:       .res 1
input_error_h:       .res 1
title_wait_l:        .res 1
title_wait_h:        .res 1
network_table_idl:   .res 1
network_table_idh:   .res 1
splash_loaded:       .res 1
piece_buffer:        .res PIECE_SIZE * PIECE_SIZE
anim_bg_buffer:      .res PIECE_SIZE * PIECE_SIZE
castle_king_buffer:  .res PIECE_SIZE * PIECE_SIZE
castle_rook_buffer:  .res PIECE_SIZE * PIECE_SIZE
castle_king_bg:      .res PIECE_SIZE * PIECE_SIZE
castle_rook_bg:      .res PIECE_SIZE * PIECE_SIZE
castle_scene_work:   .res CASTLE_STRIP_WIDTH * PIECE_SIZE
castle_scene:        .res VSPRITE_DESC_SIZE * 2
board_state:         .res 64
panel_line:          .res PANEL_LINE_LEN
move_history:        .res PANEL_MOVE_ROWS * PANEL_LINE_LEN
input_buffer:        .res INPUT_BUFFER_LEN + 1
network_frame:       .res NGS_MAX_FRAME_PAYLOAD

.include "../Pieces.inc"

.segment "CODE"

reset:
        SEI
        CLD
        LDX #$FF
        TXS

        JSR init_video
        JSR clear_text
.if CHESS_CASTLE_TEST
        JSR init_game_defaults
        JSR load_selected_engine_or_halt
        JSR castle_demo_suite
.elseif CHESS_RENDER_TEST
        JSR init_game_defaults
        JSR load_selected_engine_or_halt
        JSR init_engine_game
        JSR start_game_screen
        JSR demo_piece_suite
.else
        JSR init_game_defaults
        JSR title_flow
        JSR load_selected_engine_or_halt
        JSR init_engine_game
        JSR start_game_screen
        JSR game_dispatch
.endif

halt:
        WAI
        BRA halt

init_video:
        LDA #$02
        STA VGC_MODE
        STZ VGC_BGCOL
        STZ VGC_BORDER
        LDA #COLOR_WHITE
        STA VGC_FGCOL
        STZ VGC_CURSEN
        STZ VGC_SCROLLX
        STZ VGC_SCROLLY
        LDA #$0F
        STA VGC_DIMMER
        LDA #COLOR_TRANSPARENT
        STA VGC_GFXTRANS
        RTS

setup_text:
        STZ VTEXT_LEFT
        STZ VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #50
        STA VTEXT_HEIGHT
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        LDA #$01
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        STZ VTEXT_FLAGS
        RTS

clear_text:
        JSR setup_text
        JMP vtext_clear_region

clear_gfx:
        STZ splash_loaded
        STZ VSPRITE_XL
        STZ VSPRITE_XH
        STZ VSPRITE_Y
        LDA #<320
        STA VSPRITE_WIDTHL
        LDA #>320
        STA VSPRITE_WIDTHH
        LDA #200
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA #COLOR_TRANSPARENT
        STA VSPRITE_FILLVALUE
        JMP vsprite_gfx_fill

draw_chrome:
        LDA #(BOARD_X - 4)
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA #(BOARD_Y - 4)
        STA VSPRITE_Y
        LDA #(SQUARE_SIZE * 8 + 8)
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA #COLOR_FRAME
        STA VSPRITE_FILLVALUE
        JSR vsprite_gfx_fill

        LDA #(BOARD_X - 2)
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA #(BOARD_Y - 2)
        STA VSPRITE_Y
        LDA #(SQUARE_SIZE * 8 + 4)
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA #COLOR_SHADOW
        STA VSPRITE_FILLVALUE
        JSR vsprite_gfx_fill

        LDA #PANEL_X
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA #PANEL_Y
        STA VSPRITE_Y
        LDA #PANEL_W
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        LDA #PANEL_H
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA #COLOR_SHADOW
        STA VSPRITE_FILLVALUE
        JSR vsprite_gfx_fill

        LDA #PANEL_X
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA #(PANEL_Y + 1)
        STA VSPRITE_Y
        LDA #PANEL_W
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        LDA #2
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA #COLOR_ACCENT
        STA VSPRITE_FILLVALUE
        JMP vsprite_gfx_fill

draw_board:
        STZ board_row
        LDA #BOARD_Y
        STA square_y
@row_loop:
        STZ board_file
        LDA #BOARD_X
        STA square_xl
        STZ square_xh
@file_loop:
        LDA square_xl
        STA VSPRITE_XL
        LDA square_xh
        STA VSPRITE_XH
        LDA square_y
        STA VSPRITE_Y
        LDA #SQUARE_SIZE
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH

        LDA board_row
        CLC
        ADC board_file
        AND #$01
        BEQ @light
        LDA #COLOR_DARK_SQUARE
        BRA @fill
@light:
        LDA #COLOR_LIGHT_SQUARE
@fill:
        STA VSPRITE_FILLVALUE
        JSR vsprite_gfx_fill

        LDA square_xl
        CLC
        ADC #SQUARE_SIZE
        STA square_xl
        BCC @next_file
        INC square_xh
@next_file:
        INC board_file
        LDA board_file
        CMP #8
        BCC @file_loop

        LDA square_y
        CLC
        ADC #SQUARE_SIZE
        STA square_y
        INC board_row
        LDA board_row
        CMP #8
        BCC @row_loop
        RTS

draw_all_pieces:
        STZ board_index
        STZ board_row
        LDA #BOARD_Y
        STA square_y
@row_loop:
        STZ board_file
        LDA #BOARD_X
        STA square_xl
        STZ square_xh
@file_loop:
        LDY board_index
        LDA board_state,Y
        BEQ @skip_piece
        STA piece_code
        JSR draw_piece_on_square
@skip_piece:
        INC board_index
        LDA square_xl
        CLC
        ADC #SQUARE_SIZE
        STA square_xl
        BCC @next_file
        INC square_xh
@next_file:
        INC board_file
        LDA board_file
        CMP #8
        BCC @file_loop

        LDA square_y
        CLC
        ADC #SQUARE_SIZE
        STA square_y
        INC board_row
        LDA board_row
        CMP #8
        BCC @row_loop
        RTS

draw_piece_on_square:
        LDA square_xl
        CLC
        ADC #PIECE_OFFSET
        STA VSPRITE_XL
        LDA square_xh
        ADC #$00
        STA VSPRITE_XH
        LDA square_y
        CLC
        ADC #PIECE_OFFSET
        STA VSPRITE_Y

draw_piece_at_vsprite:
        LDA piece_code
        AND #$07
        BEQ @done
        STA piece_type

        JSR build_piece_buffer

        LDA #BLT_SPACE_CPU
        STA VSPRITE_SRCSPACE
        LDA #<piece_buffer
        STA VSPRITE_SRCADDRL
        LDA #>piece_buffer
        STA VSPRITE_SRCADDRM
        STZ VSPRITE_SRCADDRH
        LDA #PIECE_SIZE
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        STA VSPRITE_SRCSTRL
        STZ VSPRITE_SRCSTRH
        LDA #COLOR_TRANSPARENT
        STA VSPRITE_COLORKEY
        LDA #VSPRITE_FLAG_COLORKEY
        STA VSPRITE_FLAGS
        JSR vsprite_gfx_blit
@done:
        RTS

set_square_position_from_board_index:
        STA board_index
        AND #$07
        STA board_file
        TAX
        LDA square_piece_x,X
        SEC
        SBC #PIECE_OFFSET
        STA square_xl
        STZ square_xh
        LDA board_index
        LSR
        LSR
        LSR
        STA board_row
        TAX
        LDA square_piece_y,X
        SEC
        SBC #PIECE_OFFSET
        STA square_y
        RTS

erase_piece_footprint_from_index:
        STA board_index

erase_piece_footprint_current:
        LDA board_index
        AND #$07
        STA board_file
        TAX
        LDA square_piece_x,X
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA board_index
        LSR
        LSR
        LSR
        STA board_row
        TAX
        LDA square_piece_y,X
        STA VSPRITE_Y
        LDA #PIECE_SIZE
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA board_row
        CLC
        ADC board_file
        AND #$01
        BEQ @light
        LDA #COLOR_DARK_SQUARE
        BRA @fill
@light:
        LDA #COLOR_LIGHT_SQUARE
@fill:
        STA VSPRITE_FILLVALUE
        JMP vsprite_gfx_fill

draw_piece_from_board_index:
        STA board_index
        TAY
        LDA board_state,Y
        BEQ @done
        STA piece_code
        LDA board_index
        JSR set_square_position_from_board_index
        JMP draw_piece_on_square
@done:
        RTS

repair_piece_footprint_from_state:
        STA board_index
        JSR erase_piece_footprint_current
        LDA board_index
        JMP draw_piece_from_board_index

.if CHESS_RENDER_TEST
demo_piece_suite:
@suite_loop:
        JSR draw_empty_board_surface
        STZ move_index
@move_loop:
        JSR animate_test_move
        INC move_index
        LDA move_index
        CMP #DEMO_MOVE_COUNT
        BCC @move_loop
        LDX #DEMO_FINAL_PAUSE
        JSR wait_demo_frames
.if DEMO_REPEAT
        BRA @suite_loop
.else
        JMP halt
.endif

draw_empty_board_surface:
        JSR draw_board
        JMP draw_board_labels

animate_test_move:
        LDX move_index
        LDA demo_piece_codes,X
        STA piece_code
        STA anim_piece_code
        CPX #1
        BEQ animate_white_knight_test_move
        CPX #7
        BEQ animate_black_knight_test_move
        LDA demo_from_x,X
        STA anim_x
        LDA demo_from_y,X
        STA anim_y
        LDA demo_to_x,X
        STA to_x
        LDA demo_to_y,X
        STA to_y
        LDA demo_path_count,X
        STA anim_path_count
        JMP animate_line

animate_white_knight_test_move:
        LDA #143
        STA anim_x
        STA to_x
        LDA #161
        STA anim_y
        LDA #117
        STA to_y
        LDA #DEMO_STEPS_44
        STA anim_path_count
        JSR animate_line

        LDA #121
        STA to_x
        LDA #117
        STA to_y
        LDA #DEMO_STEPS_22
        STA anim_path_count
        JMP animate_line_continue

animate_black_knight_test_move:
        LDA #143
        STA anim_x
        STA to_x
        LDA #7
        STA anim_y
        LDA #51
        STA to_y
        LDA #DEMO_STEPS_44
        STA anim_path_count
        JSR animate_line

        LDA #121
        STA to_x
        LDA #51
        STA to_y
        LDA #DEMO_STEPS_22
        STA anim_path_count
        JMP animate_line_continue

.endif

.if CHESS_CASTLE_TEST
castle_demo_suite:
@loop:
        JSR setup_castle_kingside_demo
        LDA #$74
        STA self_from
        LDA #$76
        STA self_to
        JSR animate_engine_move
        JSR finish_castle_demo_move
        LDX #DEMO_FINAL_PAUSE
        JSR wait_demo_frames

        JSR setup_castle_queenside_demo
        LDA #$74
        STA self_from
        LDA #$72
        STA self_to
        JSR animate_engine_move
        JSR finish_castle_demo_move
        LDX #DEMO_FINAL_PAUSE
        JSR wait_demo_frames
.if DEMO_REPEAT
        BRA @loop
.else
        JMP halt
.endif

setup_castle_kingside_demo:
        JSR setup_empty_castle_demo
        LDA #PIECE_ROOK
        STA board_state + 63
        JMP start_game_screen

setup_castle_queenside_demo:
        JSR setup_empty_castle_demo
        LDA #PIECE_ROOK
        STA board_state + 56
        JMP start_game_screen

setup_empty_castle_demo:
        JSR clear_board_state
        LDA #PIECE_KING
        STA board_state + 60
        RTS

clear_board_state:
        LDX #$00
        LDA #PIECE_EMPTY
@loop:
        STA board_state,X
        INX
        CPX #64
        BCC @loop
        RTS

finish_castle_demo_move:
        LDY to_board_index
        LDA anim_piece_code
        STA board_state,Y
        LDY castle_rook_to
        LDA castle_rook_piece
        STA board_state,Y
        RTS
.endif

wait_frame:
        LDA VGC_FRAME
@wait:
        CMP VGC_FRAME
        BEQ @wait
        RTS

.if CHESS_RENDER_TEST || CHESS_CASTLE_TEST || CHESS_SELFPLAY_DEMO
animate_line:
        JSR begin_anim_tween
        JSR init_anim_scene
        JSR vsprite_scene_commit_atomic
animate_line_continue:
        JSR begin_anim_tween
@frame:
        LDA anim_path_count
        BEQ @done
        LDX #DEMO_STEP_PAUSE
        JSR wait_demo_frames

        JSR step_anim_tween
        JSR set_anim_scene_position
        JSR set_anim_scene_dirty
        JSR vsprite_scene_commit_atomic
        DEC anim_path_count
        BNE @frame
@done:
        RTS

begin_anim_tween:
        LDA anim_x
        STA anim_start_x
        LDA anim_y
        STA anim_start_y
        LDA anim_path_count
        STA TWEEN_DURATION
        STZ TWEEN_FRAME
        STZ TWEEN_DONE
        RTS

step_anim_tween:
        LDA TWEEN_FRAME
        CMP TWEEN_DURATION
        BCS @eval
        INC TWEEN_FRAME
@eval:
        LDA anim_start_x
        STA TWEEN_STARTL
        STZ TWEEN_STARTH
        LDA to_x
        STA TWEEN_ENDL
        STZ TWEEN_ENDH
        JSR tween_eval_ease_in_out
        LDA TWEEN_VALUEL
        STA anim_x

        LDA anim_start_y
        STA TWEEN_STARTL
        STZ TWEEN_STARTH
        LDA to_y
        STA TWEEN_ENDL
        STZ TWEEN_ENDH
        JSR tween_eval_ease_in_out
        LDA TWEEN_VALUEL
        STA anim_y
        RTS

init_anim_scene:
        JSR build_anim_piece_buffer
        LDA #<castle_scene
        STA VSPRITE_SCENE_ADDRL
        LDA #>castle_scene
        STA VSPRITE_SCENE_ADDRH
        LDA #1
        STA VSPRITE_SCENE_COUNT
        LDA #BLT_SPACE_CPU
        STA VSPRITE_SCENE_WORKSPACE
        LDA #<castle_scene_work
        STA VSPRITE_SCENE_WORKADDRL
        LDA #>castle_scene_work
        STA VSPRITE_SCENE_WORKADDRM
        STZ VSPRITE_SCENE_WORKADDRH

        LDA #(VSPRITE_DESC_VISIBLE | VSPRITE_DESC_FLAG_COLORKEY)
        STA castle_scene + VSPRITE_DESC_FLAGS
        LDA anim_x
        STA castle_scene + VSPRITE_DESC_XL
        STA castle_scene + VSPRITE_DESC_OLD_XL
        STZ castle_scene + VSPRITE_DESC_XH
        STZ castle_scene + VSPRITE_DESC_OLD_XH
        LDA anim_y
        STA castle_scene + VSPRITE_DESC_Y
        STA castle_scene + VSPRITE_DESC_OLD_Y
        LDA #PIECE_SIZE
        STA castle_scene + VSPRITE_DESC_WIDTHL
        STA castle_scene + VSPRITE_DESC_HEIGHTL
        STA castle_scene + VSPRITE_DESC_SRCSTRL
        STA castle_scene + VSPRITE_DESC_BGSTRL
        STZ castle_scene + VSPRITE_DESC_WIDTHH
        STZ castle_scene + VSPRITE_DESC_HEIGHTH
        STZ castle_scene + VSPRITE_DESC_SRCSTRH
        STZ castle_scene + VSPRITE_DESC_BGSTRH
        LDA #BLT_SPACE_CPU
        STA castle_scene + VSPRITE_DESC_SRCSPACE
        STA castle_scene + VSPRITE_DESC_BGSPACE
        LDA #<piece_buffer
        STA castle_scene + VSPRITE_DESC_SRCADDRL
        LDA #>piece_buffer
        STA castle_scene + VSPRITE_DESC_SRCADDRM
        STZ castle_scene + VSPRITE_DESC_SRCADDRH
        LDA #<anim_bg_buffer
        STA castle_scene + VSPRITE_DESC_BGADDRL
        LDA #>anim_bg_buffer
        STA castle_scene + VSPRITE_DESC_BGADDRM
        STZ castle_scene + VSPRITE_DESC_BGADDRH
        LDA #COLOR_TRANSPARENT
        STA castle_scene + VSPRITE_DESC_COLORKEY
        JMP set_anim_scene_dirty

build_anim_piece_buffer:
        LDA anim_piece_code
        STA piece_code
        AND #$07
        STA piece_type
        JMP build_piece_buffer

set_anim_scene_position:
        LDA anim_x
        STA castle_scene + VSPRITE_DESC_XL
        STZ castle_scene + VSPRITE_DESC_XH
        LDA anim_y
        STA castle_scene + VSPRITE_DESC_Y
        RTS

set_anim_scene_dirty:
        LDA castle_scene + VSPRITE_DESC_OLD_XL
        CMP castle_scene + VSPRITE_DESC_XL
        BCC @old_left
        LDA castle_scene + VSPRITE_DESC_XL
        STA VSPRITE_SCENE_XL
        STZ VSPRITE_SCENE_XH
        LDA castle_scene + VSPRITE_DESC_OLD_XL
        SEC
        SBC castle_scene + VSPRITE_DESC_XL
        BRA @width_ready
@old_left:
        LDA castle_scene + VSPRITE_DESC_OLD_XL
        STA VSPRITE_SCENE_XL
        STZ VSPRITE_SCENE_XH
        LDA castle_scene + VSPRITE_DESC_XL
        SEC
        SBC castle_scene + VSPRITE_DESC_OLD_XL
@width_ready:
        CLC
        ADC #PIECE_SIZE
        STA VSPRITE_SCENE_WIDTHL
        STZ VSPRITE_SCENE_WIDTHH
        STA VSPRITE_SCENE_WORKSTRL
        STZ VSPRITE_SCENE_WORKSTRH

        LDA castle_scene + VSPRITE_DESC_OLD_Y
        CMP castle_scene + VSPRITE_DESC_Y
        BCC @old_above
        LDA castle_scene + VSPRITE_DESC_Y
        STA VSPRITE_SCENE_Y
        LDA castle_scene + VSPRITE_DESC_OLD_Y
        SEC
        SBC castle_scene + VSPRITE_DESC_Y
        BRA @height_ready
@old_above:
        LDA castle_scene + VSPRITE_DESC_OLD_Y
        STA VSPRITE_SCENE_Y
        LDA castle_scene + VSPRITE_DESC_Y
        SEC
        SBC castle_scene + VSPRITE_DESC_OLD_Y
@height_ready:
        CLC
        ADC #PIECE_SIZE
        STA VSPRITE_SCENE_HEIGHTL
        STZ VSPRITE_SCENE_HEIGHTH
        RTS

draw_anim_piece:
        LDA anim_piece_code
        STA piece_code
        LDA anim_x
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA anim_y
        STA VSPRITE_Y
        JMP draw_piece_at_vsprite

setup_anim_background:
        LDA #PIECE_SIZE
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        STA VSPRITE_BGSTRL
        STZ VSPRITE_BGSTRH
        LDA #BLT_SPACE_CPU
        STA VSPRITE_BGSPACE
        LDA #<anim_bg_buffer
        STA VSPRITE_BGADDRL
        LDA #>anim_bg_buffer
        STA VSPRITE_BGADDRM
        STZ VSPRITE_BGADDRH
        RTS

save_anim_background:
        JSR setup_anim_background
        LDA anim_x
        STA anim_bg_x
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA anim_y
        STA anim_bg_y
        STA VSPRITE_Y
        JMP vsprite_gfx_save_bg

restore_anim_background:
        JSR setup_anim_background
        LDA anim_bg_x
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA anim_bg_y
        STA VSPRITE_Y
        JMP vsprite_gfx_restore_bg

step_delta:
        BEQ @zero
        BMI @neg
        LDA #DEMO_PIXEL_STEP
        RTS
@neg:
        LDA #($100 - DEMO_PIXEL_STEP)
        RTS
@zero:
        LDA #$00
        RTS

wait_demo_frames:
        TXA
        BEQ @done
@loop:
        JSR wait_frame
        DEX
        BNE @loop
@done:
        RTS
.endif

build_piece_buffer:
        LDA #<piece_buffer
        STA buf_ptr
        LDA #>piece_buffer
        STA buf_ptr + 1
        JMP copy_novadraw_piece_buffer

copy_novadraw_piece_buffer:
        LDX piece_type
        DEX
copy_novadraw_piece_buffer_indexed:
        LDA novadraw_piece_lo,X
        STA mask_ptr
        LDA novadraw_piece_hi,X
        STA mask_ptr + 1
        LDA #PIECE_SIZE
        STA row_count
        LDA piece_code
        AND #PIECE_BLACK
        BNE @black_row
@row:
        LDY #$00
@col:
        LDA (mask_ptr),Y
        STA (buf_ptr),Y
        INY
        CPY #PIECE_SIZE
        BCC @col

        LDA mask_ptr
        CLC
        ADC #PIECE_SIZE
        STA mask_ptr
        BCC @inc_buf
        INC mask_ptr + 1
@inc_buf:
        LDA buf_ptr
        CLC
        ADC #PIECE_SIZE
        STA buf_ptr
        BCC @next_row
        INC buf_ptr + 1
@next_row:
        DEC row_count
        BNE @row
        RTS

@black_row:
        LDY #$00
@black_col:
        LDA (mask_ptr),Y
        CMP #COLOR_WHITE
        BNE @not_main
        LDA #COLOR_BLACK_REMAP_MAIN
        BRA @black_store
@not_main:
        CMP #COLOR_FRAME
        BNE @not_hilite
        LDA #COLOR_BLACK_REMAP_HILITE
        BRA @black_store
@not_hilite:
        CMP #COLOR_DARK_SQUARE
        BNE @black_store
        LDA #COLOR_BLACK_REMAP_SHADOW
@black_store:
        STA (buf_ptr),Y
        INY
        CPY #PIECE_SIZE
        BCC @black_col

        LDA mask_ptr
        CLC
        ADC #PIECE_SIZE
        STA mask_ptr
        BCC @black_inc_buf
        INC mask_ptr + 1
@black_inc_buf:
        LDA buf_ptr
        CLC
        ADC #PIECE_SIZE
        STA buf_ptr
        BCC @black_next_row
        INC buf_ptr + 1
@black_next_row:
        DEC row_count
        BNE @black_row
        RTS

inc_mask_ptr:
        INC mask_ptr
        BNE @done
        INC mask_ptr + 1
@done:
        RTS

draw_panel:
        LDA #$01
        STA VTEXT_COLOR
        STZ VTEXT_ATTR

        LDA #PANEL_TEXT_X
        STA text_x
        LDA #3
        STA text_y
        LDA #<msg_title
        LDY #>msg_title
        JSR print_at

        LDA #PANEL_TEXT_X
        STA text_x
        LDA #10
        STA text_y
        LDA #<msg_status_label
        LDY #>msg_status_label
        JSR print_at

        LDA #PANEL_TEXT_X
        STA text_x
        LDA #16
        STA text_y
        LDA #<msg_moves_label
        LDY #>msg_moves_label
        JMP print_at

init_ui_state:
        STZ white_clock_frames
        STZ black_clock_frames
        STZ white_clock_seconds
        STZ black_clock_seconds
        STZ white_clock_minutes
        STZ black_clock_minutes
        STZ white_capture_score
        STZ black_capture_score
        LDA #$01
        STA fullmove_number
        LDA VGC_FRAME
        STA last_frame
        JSR clear_move_history
        JMP update_material_scores

update_panel_all:
        JSR update_turn_panel
        JSR update_clock_panel
        JSR update_material_scores
        JSR redraw_move_history
        JMP set_status_ready

update_turn_panel:
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #5
        STA text_y
        LDA ENGINE_CURRENTPLAYER
        CMP #ENGINE_WHITES_TURN
        BEQ @white
        LDA #<msg_turn_black
        LDY #>msg_turn_black
        JMP print_at
@white:
        LDA #<msg_turn_white
        LDY #>msg_turn_white
        JMP print_at

update_clock:
        LDA VGC_FRAME
        SEC
        SBC last_frame
        BEQ @done
        STA frame_delta
        LDA VGC_FRAME
        STA last_frame
        LDA ENGINE_CURRENTPLAYER
        CMP #ENGINE_WHITES_TURN
        BEQ @white
        LDA black_clock_frames
        CLC
        ADC frame_delta
        STA black_clock_frames
@black_second_loop:
        CMP #FRAMES_PER_SECOND
        BCC @draw
        SEC
        SBC #FRAMES_PER_SECOND
        STA black_clock_frames
        JSR inc_black_second
        LDA black_clock_frames
        BRA @black_second_loop
@white:
        LDA white_clock_frames
        CLC
        ADC frame_delta
        STA white_clock_frames
@white_second_loop:
        CMP #FRAMES_PER_SECOND
        BCC @draw
        SEC
        SBC #FRAMES_PER_SECOND
        STA white_clock_frames
        JSR inc_white_second
        LDA white_clock_frames
        BRA @white_second_loop
@draw:
        JSR update_clock_panel
@done:
        RTS

inc_white_second:
        INC white_clock_seconds
        LDA white_clock_seconds
        CMP #60
        BCC @done
        STZ white_clock_seconds
        INC white_clock_minutes
@done:
        RTS

inc_black_second:
        INC black_clock_seconds
        LDA black_clock_seconds
        CMP #60
        BCC @done
        STZ black_clock_seconds
        INC black_clock_minutes
@done:
        RTS

update_clock_panel:
        JSR clear_panel_line
        LDA #'W'
        STA panel_line
        LDA #' '
        STA panel_line + 1
        LDY #2
        LDA white_clock_minutes
        JSR write_two_digits_panel
        LDA #':'
        STA panel_line + 4
        LDY #5
        LDA white_clock_seconds
        JSR write_two_digits_panel
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #7
        STA text_y
        LDA #<panel_line
        LDY #>panel_line
        JSR print_at

        JSR clear_panel_line
        LDA #'B'
        STA panel_line
        LDA #' '
        STA panel_line + 1
        LDY #2
        LDA black_clock_minutes
        JSR write_two_digits_panel
        LDA #':'
        STA panel_line + 4
        LDY #5
        LDA black_clock_seconds
        JSR write_two_digits_panel
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #8
        STA text_y
        LDA #<panel_line
        LDY #>panel_line
        JMP print_at

update_status_from_game_state:
        LDA game_state
        CMP #GAME_CHECK
        BEQ set_status_check
        CMP #GAME_CHECKMATE
        BCC @playing
.if CHESS_SELFPLAY_DEMO
        JMP show_game_result
.else
        LDA #<msg_game_over
        LDY #>msg_game_over
        JMP print_status
.endif
@playing:
        JMP set_status_playing

set_status_ready:
        LDA #<msg_status_ready
        LDY #>msg_status_ready
        JMP print_status

set_status_playing:
        LDA #<msg_status_playing
        LDY #>msg_status_playing
        JMP print_status

set_status_thinking:
        LDA #<msg_status_thinking
        LDY #>msg_status_thinking
        JMP print_status

set_status_moving:
        LDA #<msg_status_moving
        LDY #>msg_status_moving
        JMP print_status

set_status_check:
        LDA #<msg_status_check
        LDY #>msg_status_check
        JMP print_status

set_status_invalid:
        LDA #<msg_status_invalid
        LDY #>msg_status_invalid
        JMP print_status

set_status_network:
        LDA #<msg_status_network
        LDY #>msg_status_network
        JMP print_status

set_status_network_error:
        LDA #<msg_status_neterr
        LDY #>msg_status_neterr
        JMP print_status

print_status:
        STA msg_ptr
        STY msg_ptr + 1
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #11
        STA text_y
        LDA msg_ptr
        LDY msg_ptr + 1
        JMP print_at

update_material_scores:
        STZ white_material
        STZ black_material
        LDY #$00
@loop:
        LDA board_state,Y
        BEQ @next
        PHA
        AND #$07
        TAX
        LDA piece_values,X
        STA digit_ones
        PLA
        AND #PIECE_BLACK
        BNE @black_piece
        LDA white_material
        CLC
        ADC digit_ones
        STA white_material
        BRA @next
@black_piece:
        LDA black_material
        CLC
        ADC digit_ones
        STA black_material
@next:
        INY
        CPY #64
        BCC @loop

        LDA #INITIAL_MATERIAL
        SEC
        SBC black_material
        STA white_capture_score
        LDA #INITIAL_MATERIAL
        SEC
        SBC white_material
        STA black_capture_score
        JMP update_capture_panel

update_capture_panel:
        JSR clear_panel_line
        LDA #'C'
        STA panel_line
        LDA #'A'
        STA panel_line + 1
        LDA #'P'
        STA panel_line + 2
        LDA #' '
        STA panel_line + 3
        LDA #'W'
        STA panel_line + 4
        LDA #' '
        STA panel_line + 5
        LDA #'+'
        STA panel_line + 6
        LDY #7
        LDA white_capture_score
        JSR write_two_digits_panel
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #13
        STA text_y
        LDA #<panel_line
        LDY #>panel_line
        JSR print_at

        JSR clear_panel_line
        LDA #'C'
        STA panel_line
        LDA #'A'
        STA panel_line + 1
        LDA #'P'
        STA panel_line + 2
        LDA #' '
        STA panel_line + 3
        LDA #'B'
        STA panel_line + 4
        LDA #' '
        STA panel_line + 5
        LDA #'+'
        STA panel_line + 6
        LDY #7
        LDA black_capture_score
        JSR write_two_digits_panel
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #14
        STA text_y
        LDA #<panel_line
        LDY #>panel_line
        JMP print_at

clear_panel_line:
        LDX #$00
        LDA #' '
@loop:
        STA panel_line,X
        INX
        CPX #12
        BCC @loop
        STZ panel_line + 12
        RTS

write_two_digits_panel:
        STZ digit_tens
@tens:
        CMP #10
        BCC @ones
        SEC
        SBC #10
        INC digit_tens
        BRA @tens
@ones:
        STA digit_ones
        LDA digit_tens
        CLC
        ADC #'0'
        STA panel_line,Y
        INY
        LDA digit_ones
        CLC
        ADC #'0'
        STA panel_line,Y
        RTS

clear_move_history:
        LDX #$00
@line:
        LDY #$00
        LDA #' '
@col:
        STA move_history,X
        INX
        INY
        CPY #12
        BCC @col
        STZ move_history,X
        INX
        CPX #(PANEL_MOVE_ROWS * PANEL_LINE_LEN)
        BCC @line
        RTS

scroll_move_history:
        LDX #$00
@copy:
        LDA move_history + PANEL_LINE_LEN,X
        STA move_history,X
        INX
        CPX #PANEL_LAST_MOVE_OFF
        BCC @copy
        RTS

append_move_history:
        JSR scroll_move_history
        JSR clear_panel_line
        LDY #0
        LDA fullmove_number
        JSR write_two_digits_panel
        LDA #'.'
        STA panel_line + 2
        LDA ENGINE_CURRENTPLAYER
        CMP #ENGINE_BLACKS_TURN
        BEQ @white_moved
        LDA #'B'
        BRA @side
@white_moved:
        LDA #'W'
@side:
        STA panel_line + 3
        LDA #' '
        STA panel_line + 4
        LDY #5
        LDA self_from
        JSR write_square_to_panel
        LDA captured_piece
        BEQ @quiet
        LDA #'x'
        BRA @sep
@quiet:
        LDA #'-'
@sep:
        STA panel_line,Y
        INY
        LDA self_to
        AND #$7F
        JSR write_square_to_panel

        LDX #$00
@copy:
        LDA panel_line,X
        STA move_history + PANEL_LAST_MOVE_OFF,X
        INX
        CPX #PANEL_LINE_LEN
        BCC @copy

        LDA ENGINE_CURRENTPLAYER
        CMP #ENGINE_WHITES_TURN
        BNE @draw
        INC fullmove_number
@draw:
        JMP redraw_move_history

write_square_to_panel:
        STA square_tmp
        AND #$07
        CLC
        ADC #'A'
        STA panel_line,Y
        INY
        LDA square_tmp
        AND #$70
        LSR
        LSR
        LSR
        LSR
        STA digit_ones
        LDA #'8'
        SEC
        SBC digit_ones
        STA panel_line,Y
        INY
        RTS

redraw_move_history:
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #17
        STA text_y
        LDA #<move_history
        LDY #>move_history
        JSR print_at
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #18
        STA text_y
        LDA #<(move_history + PANEL_LINE_LEN)
        LDY #>(move_history + PANEL_LINE_LEN)
        JSR print_at
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #19
        STA text_y
        LDA #<(move_history + PANEL_LINE_LEN * 2)
        LDY #>(move_history + PANEL_LINE_LEN * 2)
        JSR print_at
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #20
        STA text_y
        LDA #<(move_history + PANEL_LINE_LEN * 3)
        LDY #>(move_history + PANEL_LINE_LEN * 3)
        JSR print_at
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #21
        STA text_y
        LDA #<(move_history + PANEL_LINE_LEN * 4)
        LDY #>(move_history + PANEL_LINE_LEN * 4)
        JSR print_at
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #22
        STA text_y
        LDA #<(move_history + PANEL_LINE_LEN * 5)
        LDY #>(move_history + PANEL_LINE_LEN * 5)
        JMP print_at

draw_board_labels:
        LDX #$00
@file_loop:
        LDA file_label_x,X
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA #FILE_LABEL_Y
        STA VSPRITE_Y
        LDA file_glyph_lo,X
        STA mask_ptr
        LDA file_glyph_hi,X
        STA mask_ptr + 1
        PHX
        JSR draw_label_glyph
        PLX
        INX
        CPX #8
        BCC @file_loop

        LDX #$00
@rank_loop:
        LDA #RANK_LABEL_X
        STA VSPRITE_XL
        STZ VSPRITE_XH
        LDA rank_label_y,X
        STA VSPRITE_Y
        LDA rank_glyph_lo,X
        STA mask_ptr
        LDA rank_glyph_hi,X
        STA mask_ptr + 1
        PHX
        JSR draw_label_glyph
        PLX
        INX
        CPX #8
        BCC @rank_loop
        RTS

draw_label_glyph:
        JSR build_label_buffer

        LDA #BLT_SPACE_CPU
        STA VSPRITE_SRCSPACE
        LDA #<piece_buffer
        STA VSPRITE_SRCADDRL
        LDA #>piece_buffer
        STA VSPRITE_SRCADDRM
        STZ VSPRITE_SRCADDRH
        LDA #LABEL_WIDTH
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        LDA #LABEL_HEIGHT
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH
        LDA #LABEL_WIDTH
        STA VSPRITE_SRCSTRL
        STZ VSPRITE_SRCSTRH
        LDA #COLOR_TRANSPARENT
        STA VSPRITE_COLORKEY
        LDA #VSPRITE_FLAG_COLORKEY
        STA VSPRITE_FLAGS
        JMP vsprite_gfx_blit

build_label_buffer:
        LDA #<piece_buffer
        STA buf_ptr
        LDA #>piece_buffer
        STA buf_ptr + 1
        LDA #LABEL_HEIGHT
        STA row_count
@row:
        LDY #$00
        LDA (mask_ptr),Y
        STA mask_byte
        JSR expand_label_row
        JSR inc_mask_ptr
        DEC row_count
        BNE @row
        RTS

expand_label_row:
        LDA #LABEL_WIDTH
        STA bit_count
@bit:
        ASL mask_byte
        LDA #COLOR_TRANSPARENT
        BCC @store
        LDA #COLOR_WHITE
@store:
        LDY #$00
        STA (buf_ptr),Y
        INC buf_ptr
        BNE @next
        INC buf_ptr + 1
@next:
        DEC bit_count
        BNE @bit
        RTS

; A/Y = pointer to null-terminated string. text_x/text_y choose the cursor.
print_at:
        STA msg_ptr
        STY msg_ptr + 1
        LDA text_x
        STA VTEXT_CURX
        LDA text_y
        STA VTEXT_CURY
        JSR vtext_set_cursor
@loop:
        LDY #$00
        LDA (msg_ptr),Y
        BEQ @done
        STA VTEXT_CHAR
        JSR vtext_put_char
        INC msg_ptr
        BNE @loop
        INC msg_ptr + 1
        BRA @loop
@done:
        RTS

load_engine_for_mode:
        LDA play_mode
        CMP #PLAYMODE_HUMAN_VS_HUMAN
        BEQ load_rules_engine
        CMP #PLAYMODE_NETWORK
        BEQ load_rules_engine
        JMP load_full_engine

load_selected_engine_or_halt:
        JSR load_engine_for_mode
        BEQ @ok
        LDA #2
        STA text_x
        LDA #2
        STA text_y
        LDA #<msg_engine_failed
        LDY #>msg_engine_failed
        JSR print_at
        JMP halt
@ok:
        RTS

load_full_engine:
        LDA #<engine_full_name
        STA FIO_ARG_NAMEPTR_L
        LDA #>engine_full_name
        STA FIO_ARG_NAMEPTR_H
        LDA #(engine_full_name_end - engine_full_name)
        BRA load_engine_named

load_rules_engine:
        LDA #<engine_rules_name
        STA FIO_ARG_NAMEPTR_L
        LDA #>engine_rules_name
        STA FIO_ARG_NAMEPTR_H
        LDA #(engine_rules_name_end - engine_rules_name)
load_engine_named:
        STA FIO_ARG_NAMELEN
        JSR fio_copy_name
        BNE @done
        JMP fio_load
@done:
        RTS

load_title_splash:
        LDA #<splash_nvg_name
        STA NVG_NAMEPTR_L
        LDA #>splash_nvg_name
        STA NVG_NAMEPTR_H
        LDA #(splash_nvg_name_end - splash_nvg_name)
        STA NVG_NAMELEN
        JSR nvg_load_named
        BNE @done
        LDA #$01
        STA splash_loaded
        LDA #$00
@done:
        RTS

init_game_defaults:
        LDA #PLAYMODE_DEMO
        STA play_mode
        LDA #ENGINE_WHITES_TURN
        STA human_side
        LDA #DIFFICULTY_EASY
        STA selected_difficulty
        RTS

title_flow:
        JMP setup_new_game

show_title_menu:
        JSR show_menu_backdrop
        JSR nui_dialog_defaults
        LDA #24
        STA NUI_DIALOG_LEFT
        LDA #5
        STA NUI_DIALOG_TOP
        LDA #32
        STA NUI_DIALOG_WIDTH
        LDA #10
        STA NUI_DIALOG_HEIGHT
        LDA #<msg_select_mode
        STA NUI_TITLEL
        LDA #>msg_select_mode
        STA NUI_TITLEH
        LDA #<msg_setup_modes
        STA NUI_MSGL
        LDA #>msg_setup_modes
        STA NUI_MSGH
        STZ NUI_FOOTERL
        STZ NUI_FOOTERH
        JSR nui_show_dialog
        STZ VGC_CURSEN
        RTS

show_menu_backdrop:
        JSR clear_text
        LDA splash_loaded
        BNE @ready
        STZ VGC_DIMMER
        JSR load_title_splash
        BEQ @show
@fallback:
        JSR clear_gfx
@show:
        LDA #$0F
        STA VGC_DIMMER
@ready:
        RTS

wait_splash_or_key:
@drain:
        LDA VGC_CHARIN
        BNE @drain
        LDA #<CHESS_SPLASH_FRAMES
        STA title_wait_l
        LDA #>CHESS_SPLASH_FRAMES
        STA title_wait_h
@loop:
        LDA VGC_CHARIN
        BNE @key
        LDA title_wait_l
        ORA title_wait_h
        BEQ @timeout
        JSR wait_frame
        LDA title_wait_l
        BNE @dec_low
        DEC title_wait_h
@dec_low:
        DEC title_wait_l
        BRA @loop
@timeout:
        LDA #$00
        RTS
@key:
        JSR to_upper
        CMP #'D'
        BEQ @done
        CMP #'N'
        BEQ @done
        CMP #'S'
        BEQ @done
        CMP #$0D
        BEQ @done
        BRA @loop
@done:
        RTS

wait_key:
        STZ VGC_CURSEN
@loop:
        LDA VGC_CHARIN
        BEQ @loop
        PHA
        STZ VGC_CURSEN
        PLA
        JMP to_upper

to_upper:
        CMP #'a'
        BCC @done
        CMP #'z' + 1
        BCS @done
        SEC
        SBC #$20
@done:
        RTS

setup_new_game:
        JSR select_player_mode
        LDA play_mode
        CMP #PLAYMODE_HUMAN_VS_HUMAN
        BEQ @ready
        CMP #PLAYMODE_NETWORK
        BEQ @network_ready
        JSR select_difficulty
        LDA play_mode
        CMP #PLAYMODE_HUMAN_VS_NOVA
        BNE @ready
        JSR select_color
@ready:
        RTS
@network_ready:
        LDA #ENGINE_WHITES_TURN
        STA human_side
        RTS

select_player_mode:
        JSR show_menu_backdrop
        JSR nui_dialog_defaults
        LDA #24
        STA NUI_DIALOG_LEFT
        LDA #5
        STA NUI_DIALOG_TOP
        LDA #32
        STA NUI_DIALOG_WIDTH
        LDA #10
        STA NUI_DIALOG_HEIGHT
        LDA #<msg_select_mode
        STA NUI_TITLEL
        LDA #>msg_select_mode
        STA NUI_TITLEH
        LDA #<msg_setup_modes
        STA NUI_MSGL
        LDA #>msg_setup_modes
        STA NUI_MSGH
        STZ NUI_FOOTERL
        STZ NUI_FOOTERH
        JSR nui_show_dialog
@wait:
        JSR wait_key
        CMP #'1'
        BEQ @nova_nova
        CMP #'2'
        BEQ @human_nova
        CMP #'3'
        BEQ @human_human
        CMP #'4'
        BEQ @network
        CMP #'D'
        BEQ @nova_nova
        BRA @wait
@human_nova:
        LDA #PLAYMODE_HUMAN_VS_NOVA
        STA play_mode
        RTS
@nova_nova:
        LDA #PLAYMODE_NOVA_VS_NOVA
        STA play_mode
        RTS
@human_human:
        LDA #PLAYMODE_HUMAN_VS_HUMAN
        STA play_mode
        RTS
@network:
        LDA #PLAYMODE_NETWORK
        STA play_mode
        LDA #ENGINE_WHITES_TURN
        STA human_side
        RTS
select_difficulty:
        JSR show_menu_backdrop
        JSR nui_dialog_defaults
        LDA #19
        STA NUI_DIALOG_LEFT
        LDA #7
        STA NUI_DIALOG_TOP
        LDA #42
        STA NUI_DIALOG_WIDTH
        LDA #10
        STA NUI_DIALOG_HEIGHT
        LDA #<msg_setup_level
        STA NUI_TITLEL
        LDA #>msg_setup_level
        STA NUI_TITLEH
        LDA #<msg_setup_levels
        STA NUI_MSGL
        LDA #>msg_setup_levels
        STA NUI_MSGH
        STZ NUI_FOOTERL
        STZ NUI_FOOTERH
        JSR nui_show_dialog
@wait:
        JSR wait_key
        CMP #'E'
        BEQ @easy
        CMP #'M'
        BEQ @medium
        CMP #'H'
        BEQ @hard
        CMP #'1'
        BEQ @easy
        CMP #'2'
        BEQ @medium
        CMP #'3'
        BEQ @hard
        BRA @wait
@easy:
        LDA #DIFFICULTY_EASY
        STA selected_difficulty
        RTS
@medium:
        LDA #DIFFICULTY_MEDIUM
        STA selected_difficulty
        RTS
@hard:
        LDA #DIFFICULTY_HARD
        STA selected_difficulty
        RTS

select_color:
        JSR show_menu_backdrop
        JSR nui_dialog_defaults
        LDA #22
        STA NUI_DIALOG_LEFT
        LDA #8
        STA NUI_DIALOG_TOP
        LDA #36
        STA NUI_DIALOG_WIDTH
        LDA #8
        STA NUI_DIALOG_HEIGHT
        LDA #<msg_setup_color
        STA NUI_TITLEL
        LDA #>msg_setup_color
        STA NUI_TITLEH
        LDA #<msg_setup_colors
        STA NUI_MSGL
        LDA #>msg_setup_colors
        STA NUI_MSGH
        STZ NUI_FOOTERL
        STZ NUI_FOOTERH
        JSR nui_show_dialog
@wait:
        JSR wait_key
        CMP #'W'
        BEQ @white
        CMP #'B'
        BEQ @black
        CMP #'1'
        BEQ @white
        CMP #'2'
        BEQ @black
        BRA @wait
@white:
        LDA #ENGINE_WHITES_TURN
        STA human_side
        RTS
@black:
        LDA #ENGINE_BLACKS_TURN
        STA human_side
        RTS

start_game_screen:
        JSR init_ui_state
        JSR clear_text
        JSR clear_gfx
        JSR draw_chrome
        JSR draw_board
        JSR draw_all_pieces
        JSR draw_panel
        JSR update_panel_all
        JMP draw_board_labels

init_engine_game:
        JSR ENGINE_START
        LDA selected_difficulty
        STA ENGINE_DIFFICULTY
        JMP copy_engine_board

copy_engine_board:
        STZ engine_index
        STZ board_index
        LDA #8
        STA row_count
@row:
        LDA #8
        STA bit_count
@file:
        LDX engine_index
        LDA ENGINE_BOARD88,X
        JSR engine_piece_to_ui
        LDY board_index
        STA board_state,Y
        INC board_index
        INC engine_index
        DEC bit_count
        BNE @file

        LDA engine_index
        CLC
        ADC #8
        STA engine_index
        DEC row_count
        BNE @row
        RTS

engine_piece_to_ui:
        CMP #ENGINE_EMPTY_PIECE
        BNE @piece
        LDA #PIECE_EMPTY
        RTS
@piece:
        PHA
        AND #$7F
        SEC
        SBC #ENGINE_START_SPRITE_PTR
        STA piece_type
        PLA
        AND #ENGINE_WHITE_COLOR
        BNE @white
        LDA piece_type
        ORA #PIECE_BLACK
        RTS
@white:
        LDA piece_type
        RTS

.if CHESS_SELFPLAY_DEMO
game_dispatch:
        LDA play_mode
        CMP #PLAYMODE_DEMO
        BEQ @selfplay
        CMP #PLAYMODE_NOVA_VS_NOVA
        BEQ @selfplay
        CMP #PLAYMODE_NETWORK
        BEQ @network
        JMP play_game_loop
@selfplay:
        JMP selfplay_loop
@network:
        JMP network_game_loop

play_game_loop:
@loop:
        JSR update_clock
        JSR ENGINE_CHESS_CHECK_GAME_STATE
        STA game_state
        CMP #GAME_CHECKMATE
        BCS @game_over

        LDA play_mode
        CMP #PLAYMODE_HUMAN_VS_HUMAN
        BEQ @human_turn

        LDA ENGINE_CURRENTPLAYER
        CMP human_side
        BEQ @human_turn

        JSR engine_turn
        BRA @loop

@human_turn:
        JSR human_turn
        BRA @loop

@game_over:
        JSR show_game_result
        JMP post_game_loop

post_game_loop:
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #24
        STA text_y
        LDA #<msg_game_over_prompt
        LDY #>msg_game_over_prompt
        JSR print_at
@wait:
        JSR wait_key
        CMP #'N'
        BEQ @new_game
        CMP #'D'
        BEQ @demo
        BRA @wait
@new_game:
        JSR setup_new_game
        JSR load_selected_engine_or_halt
        JSR init_engine_game
        JSR start_game_screen
        JMP game_dispatch
@demo:
        LDA #PLAYMODE_DEMO
        STA play_mode
        JSR load_selected_engine_or_halt
        JSR init_engine_game
        JSR start_game_screen
        JMP game_dispatch

engine_turn:
        JSR set_status_thinking
        JSR ENGINE_CHESS_FIND_BEST_MOVE
        JSR update_clock
        LDA ENGINE_BEST_MOVE_FROM
        STA self_from
        LDA ENGINE_BEST_MOVE_TO
        STA self_to
        JSR set_status_moving
        JMP commit_current_move

human_turn:
@retry:
        JSR prompt_human_move
        BEQ @move_ready
        JSR set_status_invalid
        LDX #45
        JSR wait_demo_frames
        BRA @retry
@move_ready:
        JSR set_status_moving
        JMP commit_current_move

commit_current_move:
        JSR animate_engine_move
        JSR update_clock

        LDA self_from
        LDX self_to
        JSR ENGINE_CHESS_COMMIT_MOVE
        STA game_state
        JSR append_move_history
        JSR copy_engine_board
        JSR repair_move_after_engine_state
        JSR update_clock
        JSR update_turn_panel
        JSR update_material_scores
        JMP update_status_from_game_state

prompt_human_move:
        JSR print_move_prompt
        JSR read_move_input
        BEQ @parse
        CMP #$02
        BEQ @network_error
        BRA @bad
@parse:
        JSR parse_move_input
        BNE @bad
        JSR validate_human_move
        BNE @bad
        LDA #$00
        RTS
@network_error:
        LDA #$02
        RTS
@bad:
        LDA #$01
        RTS

print_move_prompt:
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #24
        STA text_y
        LDA #<msg_prompt_move
        LDY #>msg_prompt_move
        JSR print_at

        LDA #(PANEL_TEXT_X + 5)
        STA VTEXT_CURX
        LDA #24
        STA VTEXT_CURY
        JMP vtext_set_cursor

read_move_input:
        STZ input_len
        STZ input_from_state
        STZ input_to_state
        STZ input_error_l
        STZ input_error_h
        STZ input_blink_phase
        STZ input_blink_tick
        LDA VGC_FRAME
        STA input_blink_frame
        JSR input_set_blink_color
        JSR input_refresh_visuals
        LDA #$01
        STA VGC_CURSEN
@loop:
        JSR input_update_flash
        LDA play_mode
        CMP #PLAYMODE_NETWORK
        BNE @read_key
        JSR ngs_keepalive_tick
        BEQ @read_key
        STZ VGC_CURSEN
        JSR input_clear_highlights
        LDA #$02
        RTS
@read_key:
        LDA VGC_CHARIN
        BEQ @loop
        CMP #$0D
        BEQ @enter
        CMP #$0A
        BEQ @enter
        CMP #$08
        BEQ @backspace
        CMP #$7F
        BEQ @backspace
        CMP #$20
        BCC @loop
        CMP #$7F
        BCS @loop
        JSR to_upper
        STA input_key
        JSR input_accept_key
        BRA @loop
@backspace:
        LDA input_len
        BEQ @loop
        DEC input_len
        JSR input_refresh_visuals
        BRA @loop
@enter:
        LDA input_to_state
        CMP #INPUT_STATE_FLASH
        BNE @loop
        STZ VGC_CURSEN
        JSR input_clear_highlights
        LDA #$00
        RTS

input_accept_key:
        LDX input_len
        CPX #INPUT_BUFFER_LEN
        BCS @done
        CPX #2
        BCC @store
        LDA input_from_state
        CMP #INPUT_STATE_FLASH
        BNE @done
        CPX #4
        BCC @store
        CPX #4
        BNE @done
        LDA input_to_state
        CMP #INPUT_STATE_FLASH
        BNE @done
        LDA input_key
        CMP #'Q'
        BEQ @store
        CMP #'N'
        BNE @done
@store:
        LDA input_key
        STA input_buffer,X
        INC input_len
        JSR input_refresh_visuals
@done:
        RTS

input_refresh_visuals:
        JSR input_restore_current_outlines
        JSR input_compute_states
        JSR input_render_entry
        JSR input_render_error
        JMP input_draw_state_outlines

input_clear_highlights:
        JSR input_restore_current_outlines
        STZ input_from_state
        STZ input_to_state
        JMP input_clear_error

input_restore_current_outlines:
        LDA input_from_state
        BEQ @to_state
        LDA input_from
        JSR restore_square_from_engine_square
@to_state:
        LDA input_to_state
        BEQ @done
        LDA input_to
        JSR restore_square_from_engine_square
@done:
        RTS

input_compute_states:
        STZ input_from_state
        STZ input_to_state
        STZ input_error_l
        STZ input_error_h
        LDA input_len
        CMP #2
        BCS @have_from
        RTS
@have_from:
        JSR input_parse_from_only
        BEQ @from_coord_ok
        LDA #<msg_input_bad_from
        LDY #>msg_input_bad_from
        JMP input_set_error
@from_coord_ok:
        JSR input_from_has_legal_move
        BEQ @from_ok
        LDA #INPUT_STATE_INVALID
        STA input_from_state
        LDA #<msg_input_bad_from
        LDY #>msg_input_bad_from
        JMP input_set_error
@from_ok:
        LDA #INPUT_STATE_FLASH
        STA input_from_state
        LDA input_len
        CMP #4
        BCS @have_to
        RTS
@have_to:
        JSR parse_move_input
        BEQ @to_coord_ok
        JSR input_parse_to_only
        BNE @bad_to
        LDA #INPUT_STATE_INVALID
        STA input_to_state
@bad_to:
        LDA #<msg_input_bad_to
        LDY #>msg_input_bad_to
        JMP input_set_error
@to_coord_ok:
        JSR validate_human_move
        BEQ @to_ok
        LDA #INPUT_STATE_INVALID
        STA input_to_state
        LDA #<msg_input_illegal
        LDY #>msg_input_illegal
        JMP input_set_error
@to_ok:
        LDA #INPUT_STATE_FLASH
        STA input_to_state
        RTS

input_set_error:
        STA input_error_l
        STY input_error_h
        RTS

input_render_entry:
        LDA #INPUT_ENTRY_X
        STA text_x
        LDA #24
        STA text_y
        LDA #<msg_input_blank
        LDY #>msg_input_blank
        JSR print_at

        LDY #$00
        LDA input_len
        BEQ @done
        LDA input_buffer
        STA panel_line,Y
        INY
        LDA input_len
        CMP #2
        BCC @done
        LDA input_buffer + 1
        STA panel_line,Y
        INY
        LDA input_from_state
        CMP #INPUT_STATE_FLASH
        BNE @done
        LDA #'-'
        STA panel_line,Y
        INY
        LDA input_len
        CMP #3
        BCC @done
        LDA input_buffer + 2
        STA panel_line,Y
        INY
        LDA input_len
        CMP #4
        BCC @done
        LDA input_buffer + 3
        STA panel_line,Y
        INY
        LDA input_len
        CMP #5
        BCC @done
        LDA input_buffer + 4
        STA panel_line,Y
        INY
@done:
        TYA
        STA input_display_len
        LDA #$00
        STA panel_line,Y
        LDA #INPUT_ENTRY_X
        STA text_x
        LDA #24
        STA text_y
        LDA #<panel_line
        LDY #>panel_line
        JSR print_at
        LDA #INPUT_ENTRY_X
        CLC
        ADC input_display_len
        STA VTEXT_CURX
        LDA #24
        STA VTEXT_CURY
        JMP vtext_set_cursor

input_clear_error:
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #INPUT_ERROR_Y
        STA text_y
        LDA #<msg_input_blank
        LDY #>msg_input_blank
        JMP print_at

input_render_error:
        JSR input_clear_error
        LDA input_error_l
        ORA input_error_h
        BEQ @done
        LDA #PANEL_TEXT_X
        STA text_x
        LDA #INPUT_ERROR_Y
        STA text_y
        LDA input_error_l
        LDY input_error_h
        JMP print_at
@done:
        RTS

input_update_flash:
        LDA input_from_state
        CMP #INPUT_STATE_FLASH
        BEQ @maybe_update
        LDA input_to_state
        CMP #INPUT_STATE_FLASH
        BEQ @maybe_update
        RTS
@maybe_update:
        LDA VGC_FRAME
        CMP input_blink_frame
        BEQ @done
        STA input_blink_frame
        INC input_blink_tick
        LDA input_blink_tick
        CMP #INPUT_BLINK_FRAMES
        BCC @done
        STZ input_blink_tick
        INC input_blink_phase
        LDA input_blink_phase
        AND #$07
        STA input_blink_phase
        JSR input_set_blink_color
        JSR input_restore_current_outlines
        JSR input_draw_state_outlines
@done:
        RTS

input_set_blink_color:
        LDX input_blink_phase
        LDA input_blink_colors,X
        STA input_blink_color
        RTS

input_draw_state_outlines:
        JSR input_draw_flash_outlines
        LDA input_from_state
        CMP #INPUT_STATE_INVALID
        BNE @to_invalid
        LDX #COLOR_RED
        LDA input_from
        JSR draw_square_outline
@to_invalid:
        LDA input_to_state
        CMP #INPUT_STATE_INVALID
        BNE @done
        LDX #COLOR_RED
        LDA input_to
        JSR draw_square_outline
@done:
        RTS

input_draw_flash_outlines:
        LDA input_from_state
        CMP #INPUT_STATE_FLASH
        BNE @to_flash
        LDX input_blink_color
        LDA input_from
        JSR draw_square_outline
@to_flash:
        LDA input_to_state
        CMP #INPUT_STATE_FLASH
        BNE @done
        LDX input_blink_color
        LDA input_to
        JSR draw_square_outline
@done:
        RTS

restore_square_from_engine_square:
        JSR engine_square_to_board_index
        JSR set_square_position_from_board_index

        LDA square_xl
        STA VSPRITE_XL
        LDA square_xh
        STA VSPRITE_XH
        LDA square_y
        STA VSPRITE_Y
        LDA #SQUARE_SIZE
        STA VSPRITE_WIDTHL
        STZ VSPRITE_WIDTHH
        STA VSPRITE_HEIGHTL
        STZ VSPRITE_HEIGHTH

        LDA board_row
        CLC
        ADC board_file
        AND #$01
        BEQ @light
        LDA #COLOR_DARK_SQUARE
        BRA @fill
@light:
        LDA #COLOR_LIGHT_SQUARE
@fill:
        STA VSPRITE_FILLVALUE
        JSR vsprite_gfx_fill

        LDY board_index
        LDA board_state,Y
        BEQ @done
        STA piece_code
        JMP draw_piece_on_square
@done:
        RTS

draw_square_outline:
        STA square_tmp
        STX VGC_P0
        LDA #VCMD_GCOLOR
        JSR input_vgc_exec
        LDA square_tmp
        JSR engine_square_to_board_index
        JSR set_square_position_from_board_index

        LDA square_xl
        STA to_x
        CLC
        ADC #(SQUARE_SIZE - 1)
        STA anim_x
        LDA square_y
        STA to_y
        CLC
        ADC #(SQUARE_SIZE - 1)
        STA anim_y

        STZ VGC_P1
        STZ VGC_P3
        STZ VGC_P5
        STZ VGC_P7

        LDA to_x
        STA VGC_P0
        LDA to_y
        STA VGC_P2
        LDA anim_x
        STA VGC_P4
        LDA anim_y
        STA VGC_P6
        LDA #VCMD_RECT
input_vgc_exec:
        STA VGC_CMD
@wait:
        LDA VGC_CMD
        AND #$01
        BNE @wait
        RTS

input_parse_from_only:
        LDA input_len
        CMP #2
        BCC @invalid
        LDA input_buffer
        JSR parse_file
        BCS @invalid
        STA from_file
        LDA input_buffer + 1
        JSR parse_rank_to_row
        BCS @invalid
        STA from_row
        LDA from_row
        ASL
        ASL
        ASL
        ASL
        ORA from_file
        STA input_from
        LDA #$00
        RTS
@invalid:
        LDA #$01
        RTS

input_parse_to_only:
        LDA input_len
        CMP #4
        BCC @invalid
        LDA input_buffer + 2
        JSR parse_file
        BCS @invalid
        STA to_file
        LDA input_buffer + 3
        JSR parse_rank_to_row
        BCS @invalid
        STA to_row
        LDA to_row
        ASL
        ASL
        ASL
        ASL
        ORA to_file
        STA input_to
        LDA #$00
        RTS
@invalid:
        LDA #$01
        RTS

input_from_has_legal_move:
        JSR ENGINE_CHESS_GENERATE_LEGAL_MOVES
        LDX #$00
@loop:
        CPX ENGINE_MOVE_COUNT
        BCS @invalid
        LDA ENGINE_MOVE_LIST_FROM,X
        CMP input_from
        BEQ @valid
        INX
        BRA @loop
@valid:
        LDA #$00
        RTS
@invalid:
        LDA #$01
        RTS

parse_move_input:
        STZ input_promo
        LDA input_len
        CMP #5
        BCC @coords
        LDA input_buffer + 4
        CMP #'N'
        BEQ @knight_promo
        CMP #'Q'
        BEQ @coords
        BRA @invalid
@knight_promo:
        LDA #PROMO_FLAG_KNIGHT
        STA input_promo

@coords:
        JSR input_parse_from_only
        BNE @invalid
        JSR input_parse_to_only
        BNE @invalid
        LDA #$00
        RTS
@invalid:
        LDA #$01
        RTS

parse_file:
        CMP #'A'
        BCC @invalid
        CMP #'I'
        BCS @invalid
        SEC
        SBC #'A'
        CLC
        RTS
@invalid:
        SEC
        RTS

parse_rank_to_row:
        CMP #'1'
        BCC @invalid
        CMP #'9'
        BCS @invalid
        STA square_tmp
        LDA #'8'
        SEC
        SBC square_tmp
        CLC
        RTS
@invalid:
        SEC
        RTS

validate_human_move:
        JSR ENGINE_CHESS_GENERATE_LEGAL_MOVES
        LDX #$00
@loop:
        CPX ENGINE_MOVE_COUNT
        BCS @invalid
        LDA ENGINE_MOVE_LIST_FROM,X
        CMP input_from
        BNE @next
        LDA ENGINE_MOVE_LIST_TO,X
        STA square_tmp
        AND #$7F
        CMP input_to
        BNE @next
        LDA square_tmp
        AND #PROMO_FLAG_KNIGHT
        CMP input_promo
        BNE @next
        LDA input_from
        STA self_from
        LDA square_tmp
        STA self_to
        LDA #$00
        RTS
@next:
        INX
        BRA @loop
@invalid:
        LDA #$01
        RTS

network_game_loop:
        JSR start_game_screen
        JSR network_setup_game
        BEQ @loop
        JSR show_network_error_dialog
        JSR ngs_disconnect
        JMP post_game_loop
@loop:
        JSR update_clock
        JSR ENGINE_CHESS_CHECK_GAME_STATE
        STA game_state
        CMP #GAME_CHECKMATE
        BCS @game_over

        JSR network_human_turn
        BEQ @loop
        JSR show_network_error_dialog
        JSR ngs_disconnect
        JMP post_game_loop
@game_over:
        JSR show_game_result
        JSR ngs_disconnect
        JMP post_game_loop

network_human_turn:
@retry:
        JSR prompt_human_move
        BEQ @move_ready
        CMP #$02
        BEQ @error
        JSR set_status_invalid
        LDX #45
        JSR wait_demo_frames
        BRA @retry
@move_ready:
        JSR set_status_moving
        JSR commit_current_move
        JSR network_send_current_move
        BNE @error
        LDA game_state
        CMP #GAME_CHECKMATE
        BCS @ok
        JSR set_status_thinking
        JSR network_wait_opponent_move
        BNE @error
        JSR set_status_moving
        JSR commit_current_move
@ok:
        LDA #$00
        RTS
@error:
        LDA #$01
        RTS

network_setup_game:
        JSR set_status_network
        JSR nchess_net_init
        LDA #<network_frame
        LDX #>network_frame
        JSR ngs_set_buffer
        LDA NGS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_disconnect

        JSR ngs_connect_default
        LDA NGS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_wait_connected
        BEQ :+
        JMP @error
:

        LDA #<game_server_handle
        LDX #>game_server_handle
        LDY #game_server_handle_end - game_server_handle
        JSR ngs_set_string
        JSR nchess_net_build_hello
        LDA NCHESS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_send_current
        BEQ :+
        JMP @error
:
        LDA #NGS_KIND_WELCOME
        JSR ngs_wait_kind
        BEQ :+
        JMP @error
:

        LDA #<game_server_table_name
        LDX #>game_server_table_name
        LDY #game_server_table_name_end - game_server_table_name
        JSR ngs_set_string
        JSR nchess_net_build_create_table
        LDA NCHESS_RESULT
        BEQ :+
        JMP @error
:
        JSR ngs_send_current
        BEQ :+
        JMP @error
:
        LDA #NGS_KIND_TABLE_CREATED
        JSR ngs_wait_kind
        BEQ :+
        JMP @error
:
        JSR ngs_read_u16
        LDA NGS_RESULT
        BEQ :+
        JMP @error
:
        LDA NGS_VALUE_L
        STA network_table_idl
        STA NGS_TABLE_IDL
        LDA NGS_VALUE_H
        STA network_table_idh
        STA NGS_TABLE_IDH

        LDA #$00
        RTS
@error:
        JSR ngs_disconnect
        LDA #$01
        RTS

network_send_current_move:
        LDA network_table_idl
        STA NGS_TABLE_IDL
        LDA network_table_idh
        STA NGS_TABLE_IDH
        LDA self_from
        STA NCHESS_MOVE_FROM
        LDA self_to
        STA NCHESS_MOVE_TO
        JSR nchess_net_build_move
        LDA NCHESS_RESULT
        BNE @error
        JMP ngs_send_current
@error:
        LDA #$01
        RTS

network_wait_opponent_move:
@wait_event:
        LDA #NGS_KIND_TABLE_EVENT
        JSR ngs_wait_kind
        BNE @error
        JSR nchess_net_parse_table_event
        LDA NCHESS_RESULT
        BNE @wait_event
        LDA NCHESS_EVENT_TABLEL
        CMP network_table_idl
        BNE @wait_event
        LDA NCHESS_EVENT_TABLEH
        CMP network_table_idh
        BNE @wait_event
        LDA NCHESS_EVENT_FROM
        CMP self_from
        BNE @stockfish
        LDA NCHESS_EVENT_TO
        CMP self_to
        BEQ @wait_event
@stockfish:
        LDA NCHESS_EVENT_FROM
        STA self_from
        LDA NCHESS_EVENT_TO
        STA self_to
        LDA #$00
        RTS
@error:
        LDA #$01
        RTS

show_network_error_dialog:
        JSR set_status_network_error
        JSR nui_dialog_defaults
        LDA #<msg_net_error_title
        STA NUI_TITLEL
        LDA #>msg_net_error_title
        STA NUI_TITLEH
        LDA #<msg_net_error_body
        STA NUI_MSGL
        LDA #>msg_net_error_body
        STA NUI_MSGH
        JMP nui_show_error

selfplay_loop:
@loop:
        JSR update_clock
        LDX #SELFPLAY_MOVE_PAUSE
        JSR wait_demo_frames
        JSR update_clock
        JSR ENGINE_CHESS_CHECK_GAME_STATE
        STA game_state
        CMP #GAME_CHECKMATE
        BCS @game_over

        JSR set_status_thinking
        JSR ENGINE_CHESS_FIND_BEST_MOVE
        JSR update_clock
        LDA ENGINE_BEST_MOVE_FROM
        STA self_from
        LDA ENGINE_BEST_MOVE_TO
        STA self_to
        JSR set_status_moving
        JSR commit_current_move
        BRA @loop

@game_over:
        JSR show_game_result
        RTS

show_game_result:
        LDA game_state
        CMP #GAME_CHECKMATE
        BEQ @checkmate
        CMP #GAME_STALEMATE
        BEQ @stalemate
        CMP #GAME_DRAW_50_MOVE
        BEQ @draw_50
        CMP #GAME_DRAW_REPETITION
        BEQ @draw_repeat
        CMP #GAME_DRAW_INSUFFICIENT
        BEQ @draw_insufficient
        CMP #GAME_DRAW_75_MOVE
        BEQ @draw_75
        CMP #GAME_DRAW_REPETITION_AUTO
        BEQ @draw_repeat_auto
        LDA #<msg_game_over
        LDY #>msg_game_over
        JMP print_status
@checkmate:
        LDA #<msg_checkmate
        LDY #>msg_checkmate
        JMP print_status
@stalemate:
        LDA #<msg_stalemate
        LDY #>msg_stalemate
        JMP print_status
@draw_50:
        LDA #<msg_draw_50
        LDY #>msg_draw_50
        JMP print_status
@draw_repeat:
        LDA #<msg_draw_repeat
        LDY #>msg_draw_repeat
        JMP print_status
@draw_insufficient:
        LDA #<msg_draw_insufficient
        LDY #>msg_draw_insufficient
        JMP print_status
@draw_75:
        LDA #<msg_draw_75
        LDY #>msg_draw_75
        JMP print_status
@draw_repeat_auto:
        LDA #<msg_draw_repeat_auto
        LDY #>msg_draw_repeat_auto
        JMP print_status
.endif

.if CHESS_CASTLE_TEST || CHESS_SELFPLAY_DEMO
animate_engine_move:
        JSR prepare_engine_move_geometry
        LDA self_from
        JSR engine_square_to_board_index
        STA from_board_index
        TAY
        LDA board_state,Y
        BEQ @done
        STA anim_piece_code
        LDA #PIECE_EMPTY
        STA board_state,Y
        LDA from_board_index
        JSR erase_piece_footprint_from_index

        LDA self_to
        JSR engine_square_to_board_index
        STA to_board_index
        TAY
        LDA board_state,Y
        STA captured_piece
        STZ en_passant_capture
        BEQ @check_en_passant
        BRA @capture_ready

@check_en_passant:
        JSR prepare_en_passant_capture_visual

@capture_ready:

        LDX from_file
        LDA square_piece_x,X
        STA anim_x
        LDX from_row
        LDA square_piece_y,X
        STA anim_y

        LDA anim_piece_code
        AND #$07
        CMP #PIECE_KING
        BNE @check_knight
        LDA abs_file_delta
        CMP #2
        BNE @check_knight
        JMP animate_castle_move

@check_knight:
        LDA anim_piece_code
        AND #$07
        CMP #PIECE_KNIGHT
        BNE @normal_move
        JMP animate_knight_move

@normal_move:
        JSR setup_normal_move_animation
        JMP animate_line
@done:
        RTS

animate_castle_move:
        JSR prepare_castle_rook_visual
        JSR build_castle_piece_buffers
        JSR init_castle_scene
        JSR begin_castle_tween
        JSR vsprite_scene_commit_atomic

@frame:
        LDA TWEEN_FRAME
        CMP castle_tween_duration
        BEQ @finish
.if CASTLE_STEP_PAUSE
        LDX #CASTLE_STEP_PAUSE
        JSR wait_demo_frames
.endif
        JSR step_castle_tween
        JSR vsprite_scene_commit_atomic
        BRA @frame

@finish:
        RTS

begin_castle_tween:
        LDA castle_tween_duration
        STA TWEEN_DURATION
        STZ TWEEN_FRAME
        STZ TWEEN_DONE
        RTS

step_castle_tween:
        LDA TWEEN_FRAME
        CMP TWEEN_DURATION
        BCS @eval
        INC TWEEN_FRAME
@eval:
        LDA castle_rook_start_x
        STA TWEEN_STARTL
        STZ TWEEN_STARTH
        LDA castle_rook_end_x
        STA TWEEN_ENDL
        STZ TWEEN_ENDH
        JSR tween_eval_ease_in_out
        LDA TWEEN_VALUEL
        STA castle_scene + VSPRITE_DESC_XL

        LDA castle_king_start_x
        STA TWEEN_STARTL
        STZ TWEEN_STARTH
        LDA castle_king_end_x
        STA TWEEN_ENDL
        STZ TWEEN_ENDH
        JSR tween_eval_ease_in_out
        LDA TWEEN_VALUEL
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_XL
        RTS

prepare_castle_rook_visual:
        LDA to_file
        SEC
        SBC from_file
        JSR step_delta
        STA anim_dx
        STZ anim_dy

        LDA from_row
        ASL
        ASL
        ASL
        STA square_tmp
        LDA to_file
        CMP #6
        BEQ @king_side

        LDA square_tmp
        STA castle_rook_from
        CLC
        ADC #3
        STA castle_rook_to
        LDA #DEMO_PIXEL_STEP
        STA castle_rook_dx
        LDX #3
        BRA @rook_ready

@king_side:
        LDA square_tmp
        CLC
        ADC #7
        STA castle_rook_from
        LDA square_tmp
        CLC
        ADC #5
        STA castle_rook_to
        LDA #($100 - DEMO_PIXEL_STEP)
        STA castle_rook_dx
        LDX #2

@rook_ready:
        LDA step_count_by_squares,X
        STA castle_rook_steps
        LDA step_count_by_squares + 2
        STA castle_king_steps
        LDA castle_rook_steps
        CMP castle_king_steps
        BCS @duration_ready
        LDA castle_king_steps
@duration_ready:
        STA castle_tween_duration
        LDY castle_rook_from
        LDA board_state,Y
        STA castle_rook_piece
        LDA #PIECE_EMPTY
        STA board_state,Y
        LDA castle_rook_from
        JMP erase_piece_footprint_from_index

build_castle_piece_buffers:
        LDA anim_piece_code
        STA piece_code
        AND #$07
        STA piece_type
        LDA #<castle_king_buffer
        STA buf_ptr
        LDA #>castle_king_buffer
        STA buf_ptr + 1
        LDX piece_type
        DEX
        JSR copy_novadraw_piece_buffer_indexed

        LDA castle_rook_piece
        STA piece_code
        AND #$07
        STA piece_type
        LDA #<castle_rook_buffer
        STA buf_ptr
        LDA #>castle_rook_buffer
        STA buf_ptr + 1
        LDX piece_type
        DEX
        JMP copy_novadraw_piece_buffer_indexed

init_castle_scene:
        LDA #<castle_scene
        STA VSPRITE_SCENE_ADDRL
        LDA #>castle_scene
        STA VSPRITE_SCENE_ADDRH
        LDA #2
        STA VSPRITE_SCENE_COUNT
        LDA #CASTLE_STRIP_X
        STA VSPRITE_SCENE_XL
        STZ VSPRITE_SCENE_XH
        LDA anim_y
        STA VSPRITE_SCENE_Y
        LDA #<CASTLE_STRIP_WIDTH
        STA VSPRITE_SCENE_WIDTHL
        LDA #>CASTLE_STRIP_WIDTH
        STA VSPRITE_SCENE_WIDTHH
        LDA #PIECE_SIZE
        STA VSPRITE_SCENE_HEIGHTL
        STZ VSPRITE_SCENE_HEIGHTH
        LDA #BLT_SPACE_CPU
        STA VSPRITE_SCENE_WORKSPACE
        LDA #<castle_scene_work
        STA VSPRITE_SCENE_WORKADDRL
        LDA #>castle_scene_work
        STA VSPRITE_SCENE_WORKADDRM
        STZ VSPRITE_SCENE_WORKADDRH
        LDA #<CASTLE_STRIP_WIDTH
        STA VSPRITE_SCENE_WORKSTRL
        LDA #>CASTLE_STRIP_WIDTH
        STA VSPRITE_SCENE_WORKSTRH

        LDA castle_rook_from
        AND #$07
        TAX
        LDA square_piece_x,X
        STA castle_rook_x
        STA castle_rook_start_x
        LDA castle_rook_to
        AND #$07
        TAX
        LDA square_piece_x,X
        STA castle_rook_end_x
        LDA castle_rook_from
        LSR
        LSR
        LSR
        TAX
        LDA square_piece_y,X
        STA castle_rook_y

        LDA #(VSPRITE_DESC_VISIBLE | VSPRITE_DESC_FLAG_COLORKEY)
        STA castle_scene + VSPRITE_DESC_FLAGS
        LDA castle_rook_x
        STA castle_scene + VSPRITE_DESC_XL
        STA castle_scene + VSPRITE_DESC_OLD_XL
        STZ castle_scene + VSPRITE_DESC_XH
        STZ castle_scene + VSPRITE_DESC_OLD_XH
        LDA castle_rook_y
        STA castle_scene + VSPRITE_DESC_Y
        STA castle_scene + VSPRITE_DESC_OLD_Y
        LDA #PIECE_SIZE
        STA castle_scene + VSPRITE_DESC_WIDTHL
        STA castle_scene + VSPRITE_DESC_HEIGHTL
        STA castle_scene + VSPRITE_DESC_SRCSTRL
        STA castle_scene + VSPRITE_DESC_BGSTRL
        STZ castle_scene + VSPRITE_DESC_WIDTHH
        STZ castle_scene + VSPRITE_DESC_HEIGHTH
        STZ castle_scene + VSPRITE_DESC_SRCSTRH
        STZ castle_scene + VSPRITE_DESC_BGSTRH
        LDA #BLT_SPACE_CPU
        STA castle_scene + VSPRITE_DESC_SRCSPACE
        STA castle_scene + VSPRITE_DESC_BGSPACE
        LDA #<castle_rook_buffer
        STA castle_scene + VSPRITE_DESC_SRCADDRL
        LDA #>castle_rook_buffer
        STA castle_scene + VSPRITE_DESC_SRCADDRM
        STZ castle_scene + VSPRITE_DESC_SRCADDRH
        LDA #<castle_rook_bg
        STA castle_scene + VSPRITE_DESC_BGADDRL
        LDA #>castle_rook_bg
        STA castle_scene + VSPRITE_DESC_BGADDRM
        STZ castle_scene + VSPRITE_DESC_BGADDRH
        LDA #COLOR_TRANSPARENT
        STA castle_scene + VSPRITE_DESC_COLORKEY

        LDA #(VSPRITE_DESC_VISIBLE | VSPRITE_DESC_FLAG_COLORKEY)
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_FLAGS
        LDA anim_x
        STA castle_king_start_x
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_XL
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_OLD_XL
        LDX to_file
        LDA square_piece_x,X
        STA castle_king_end_x
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_XH
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_OLD_XH
        LDA anim_y
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_Y
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_OLD_Y
        LDA #PIECE_SIZE
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_WIDTHL
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_HEIGHTL
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_SRCSTRL
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_BGSTRL
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_WIDTHH
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_HEIGHTH
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_SRCSTRH
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_BGSTRH
        LDA #BLT_SPACE_CPU
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_SRCSPACE
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_BGSPACE
        LDA #<castle_king_buffer
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_SRCADDRL
        LDA #>castle_king_buffer
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_SRCADDRM
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_SRCADDRH
        LDA #<castle_king_bg
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_BGADDRL
        LDA #>castle_king_bg
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_BGADDRM
        STZ castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_BGADDRH
        LDA #COLOR_TRANSPARENT
        STA castle_scene + VSPRITE_DESC_SIZE + VSPRITE_DESC_COLORKEY
        RTS

prepare_en_passant_capture_visual:
        LDA anim_piece_code
        AND #$07
        CMP #PIECE_PAWN
        BNE @done
        LDA abs_file_delta
        CMP #1
        BNE @done

        LDA from_row
        ASL
        ASL
        ASL
        CLC
        ADC to_file
        STA square_tmp
        TAY
        LDA board_state,Y
        BEQ @done
        STA captured_piece
        LDA #$01
        STA en_passant_capture
@done:
        RTS

repair_move_after_engine_state:
        JSR repair_landing_square_if_needed
        JMP repair_special_move_squares

repair_landing_square_if_needed:
        LDA captured_piece
        BNE @repair
        LDA self_to
        BMI @repair
@done:
        RTS
@repair:
        LDA to_board_index
        JMP repair_piece_footprint_from_state

repair_special_move_squares:
        LDA anim_piece_code
        AND #$07
        CMP #PIECE_KING
        BNE @check_en_passant
        LDA abs_file_delta
        CMP #2
        BNE @done
        RTS

@check_en_passant:
        LDA en_passant_capture
        BEQ @done
        JMP repair_en_passant_square
@done:
        RTS

repair_en_passant_square:
        LDA from_row
        ASL
        ASL
        ASL
        CLC
        ADC to_file
        JMP repair_piece_footprint_from_state

prepare_engine_move_geometry:
        LDA self_from
        AND #$07
        STA from_file
        LDA self_to
        AND #$7F
        AND #$07
        STA to_file

        LDA self_from
        JSR engine_square_to_row
        STA from_row
        LDA self_to
        AND #$7F
        JSR engine_square_to_row
        STA to_row

        LDA to_file
        SEC
        SBC from_file
        JSR abs_signed_small
        STA abs_file_delta
        LDA to_row
        SEC
        SBC from_row
        JSR abs_signed_small
        STA abs_row_delta
        RTS

setup_normal_move_animation:
        LDX to_file
        LDA square_piece_x,X
        STA to_x
        LDX to_row
        LDA square_piece_y,X
        STA to_y
        LDA to_file
        SEC
        SBC from_file
        JSR step_delta
        STA anim_dx
        LDA to_row
        SEC
        SBC from_row
        JSR step_delta
        STA anim_dy
        LDA abs_file_delta
        BNE @use_file
        LDA abs_row_delta
@use_file:
        TAX
        LDA step_count_by_squares,X
        STA anim_path_count
        RTS

animate_knight_move:
        LDA abs_row_delta
        CMP #2
        BEQ @vertical_first

        JSR setup_horizontal_knight_segment
        JSR animate_line
        JSR setup_vertical_knight_segment
        JMP animate_line_continue

@vertical_first:
        JSR setup_vertical_knight_segment
        JSR animate_line
        JSR setup_horizontal_knight_segment
        JMP animate_line_continue

setup_horizontal_knight_segment:
        LDX to_file
        LDA square_piece_x,X
        STA to_x
        LDA anim_y
        STA to_y
        LDA to_file
        SEC
        SBC from_file
        JSR step_delta
        STA anim_dx
        STZ anim_dy
        LDX abs_file_delta
        LDA step_count_by_squares,X
        STA anim_path_count
        RTS

setup_vertical_knight_segment:
        LDA anim_x
        STA to_x
        LDX to_row
        LDA square_piece_y,X
        STA to_y
        STZ anim_dx
        LDA to_row
        SEC
        SBC from_row
        JSR step_delta
        STA anim_dy
        LDX abs_row_delta
        LDA step_count_by_squares,X
        STA anim_path_count
        RTS

engine_square_to_board_index:
        AND #$7F
        STA square_tmp
        AND #$70
        LSR
        STA mask_byte
        LDA square_tmp
        AND #$07
        ORA mask_byte
        RTS

engine_square_to_row:
        AND #$70
        LSR
        LSR
        LSR
        LSR
        RTS

abs_signed_small:
        BPL @done
        EOR #$FF
        CLC
        ADC #1
@done:
        RTS
.endif

.include "nvg.s"

.segment "RODATA"

splash_nvg_name:
        .byte "SPLASH.NVG"
splash_nvg_name_end:

engine_full_name:
        .byte "CHESSENG"
engine_full_name_end:
engine_rules_name:
        .byte "CHESSRUL"
engine_rules_name_end:

game_server_handle:
        .byte "nova-chess"
game_server_handle_end:
game_server_table_name:
        .byte "Nova Chess"
game_server_table_name_end:

msg_game_over:
        .byte "GAME OVER  ", 0
msg_checkmate:
        .byte "CHECKMATE  ", 0
msg_stalemate:
        .byte "STALEMATE  ", 0
msg_draw_50:
        .byte "DRAW 50MOVE", 0
msg_draw_repeat:
        .byte "DRAW REPEAT", 0
msg_draw_insufficient:
        .byte "DRAW MATRL ", 0
msg_draw_75:
        .byte "DRAW 75MOVE", 0
msg_draw_repeat_auto:
        .byte "DRAW 5FOLD ", 0
piece_values:
        .byte 0, 1, 3, 3, 5, 9, 0

square_piece_x:
        .byte 11, 33, 55, 77, 99, 121, 143, 165
square_piece_y:
        .byte 7, 29, 51, 73, 95, 117, 139, 161
step_count_by_squares:
        .byte 0, 22, 44, 66, 88, 110, 132, 154

.if CHESS_RENDER_TEST
demo_piece_codes:
        .byte PIECE_PAWN, PIECE_KNIGHT, PIECE_BISHOP, PIECE_ROOK, PIECE_QUEEN, PIECE_KING
        .byte B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
demo_from_x:
        .byte 99, 143, 55, 11, 77, 99
        .byte 99, 143, 55, 11, 77, 99
demo_from_y:
        .byte 139, 161, 161, 161, 161, 161
        .byte 29, 7, 7, 7, 7, 7
demo_to_x:
        .byte 99, 121, 143, 11, 165, 99
        .byte 99, 121, 143, 11, 165, 99
demo_to_y:
        .byte 95, 117, 73, 95, 73, 139
        .byte 73, 51, 95, 73, 95, 29
demo_path_count:
        .byte DEMO_STEPS_44, DEMO_STEPS_66, DEMO_STEPS_88, DEMO_STEPS_66, DEMO_STEPS_88, DEMO_STEPS_22
        .byte DEMO_STEPS_44, DEMO_STEPS_66, DEMO_STEPS_88, DEMO_STEPS_66, DEMO_STEPS_88, DEMO_STEPS_22
.endif

novadraw_piece_lo:
        .byte <pieces_pawn, <pieces_knight, <pieces_bishop, <pieces_rook, <pieces_queen, <pieces_king
novadraw_piece_hi:
        .byte >pieces_pawn, >pieces_knight, >pieces_bishop, >pieces_rook, >pieces_queen, >pieces_king

msg_title:
        .byte "NOVA CHESS", 0
msg_select_mode:
        .byte "SELECT A MODE", 0
msg_setup_game:
        .byte "NEW GAME", 0
msg_setup_modes:
        .byte "1  COMPUTER VS COMPUTER", $0D
        .byte "2  HUMAN VS COMPUTER", $0D
        .byte "3  HUMAN VS HUMAN", $0D
        .byte "4  NETWORK GAME", 0
msg_setup_level:
        .byte "DIFFICULTY", 0
msg_setup_levels:
        .byte "E EASY    3 SEC", $0D
        .byte "M MEDIUM 10 SEC", $0D
        .byte "H HARD   25 SEC", 0
msg_setup_color:
        .byte "PLAY AS", 0
msg_setup_colors:
        .byte "W WHITE", $0D
        .byte "B BLACK", 0
msg_turn_white:
        .byte "TURN WHITE ", 0
msg_turn_black:
        .byte "TURN BLACK ", 0
msg_status_label:
        .byte "STATUS", 0
msg_moves_label:
        .byte "MOVES", 0
msg_status_ready:
        .byte "READY       ", 0
msg_status_playing:
        .byte "PLAYING     ", 0
msg_status_thinking:
        .byte "THINKING    ", 0
msg_status_moving:
        .byte "MOVING      ", 0
msg_status_check:
        .byte "CHECK       ", 0
msg_status_invalid:
        .byte "INVALID     ", 0
msg_status_network:
        .byte "NETWORK     ", 0
msg_status_neterr:
        .byte "NET ERROR   ", 0
msg_net_error_title:
        .byte "NETWORK ERROR", 0
msg_net_error_body:
        .byte "network error: game server unavailable.", $0D
        .byte "check novahost network settings", 0
msg_prompt_move:
        .byte "MOVE:       ", 0
msg_input_blank:
        .byte "                    ", 0
msg_input_bad_from:
        .byte "BAD FROM SQUARE", 0
msg_input_bad_to:
        .byte "BAD TO SQUARE", 0
msg_input_illegal:
        .byte "ILLEGAL MOVE", 0
input_blink_colors:
        .byte COLOR_WHITE, COLOR_LIGHT_SQUARE, COLOR_DARK_SQUARE, COLOR_WHITE_DETAIL
        .byte COLOR_BLACK, COLOR_WHITE_DETAIL, COLOR_DARK_SQUARE, COLOR_LIGHT_SQUARE
msg_game_over_prompt:
        .byte "N NEW  D DEMO", 0
msg_engine_failed:
        .byte "CHESS ENGINE LOAD FAILED", 0

file_label_x:
        .byte 18, 40, 62, 84, 106, 128, 150, 172
rank_label_y:
        .byte 13, 35, 57, 79, 101, 123, 145, 167

file_glyph_lo:
        .byte <glyph_a, <glyph_b, <glyph_c, <glyph_d, <glyph_e, <glyph_f, <glyph_g, <glyph_h
file_glyph_hi:
        .byte >glyph_a, >glyph_b, >glyph_c, >glyph_d, >glyph_e, >glyph_f, >glyph_g, >glyph_h
rank_glyph_lo:
        .byte <glyph_8, <glyph_7, <glyph_6, <glyph_5, <glyph_4, <glyph_3, <glyph_2, <glyph_1
rank_glyph_hi:
        .byte >glyph_8, >glyph_7, >glyph_6, >glyph_5, >glyph_4, >glyph_3, >glyph_2, >glyph_1

glyph_a:
        .byte %01110000, %10001000, %10001000, %11111000, %10001000, %10001000, %10001000
glyph_b:
        .byte %11110000, %10001000, %10001000, %11110000, %10001000, %10001000, %11110000
glyph_c:
        .byte %01111000, %10000000, %10000000, %10000000, %10000000, %10000000, %01111000
glyph_d:
        .byte %11110000, %10001000, %10001000, %10001000, %10001000, %10001000, %11110000
glyph_e:
        .byte %11111000, %10000000, %10000000, %11110000, %10000000, %10000000, %11111000
glyph_f:
        .byte %11111000, %10000000, %10000000, %11110000, %10000000, %10000000, %10000000
glyph_g:
        .byte %01111000, %10000000, %10000000, %10011000, %10001000, %10001000, %01110000
glyph_h:
        .byte %10001000, %10001000, %10001000, %11111000, %10001000, %10001000, %10001000

glyph_1:
        .byte %00100000, %01100000, %00100000, %00100000, %00100000, %00100000, %01110000
glyph_2:
        .byte %01110000, %10001000, %00001000, %00010000, %00100000, %01000000, %11111000
glyph_3:
        .byte %11110000, %00001000, %00001000, %01110000, %00001000, %00001000, %11110000
glyph_4:
        .byte %10010000, %10010000, %10010000, %11111000, %00010000, %00010000, %00010000
glyph_5:
        .byte %11111000, %10000000, %10000000, %11110000, %00001000, %00001000, %11110000
glyph_6:
        .byte %01110000, %10000000, %10000000, %11110000, %10001000, %10001000, %01110000
glyph_7:
        .byte %11111000, %00001000, %00010000, %00100000, %01000000, %01000000, %01000000
glyph_8:
        .byte %01110000, %10001000, %10001000, %01110000, %10001000, %10001000, %01110000

.include "vsprite.s"
.include "vtext.s"
.include "tween.s"
.include "nchess_net.s"
.include "nui.s"

.segment "VECTORS"
        .word reset
        .word reset
        .word reset
