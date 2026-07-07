; mousedemo.s -- NDK mouse event demo.
;
; Draws three on-screen "buttons", registers each as a mouse area-of-interest,
; and prints a live readout of every click: which button, where (x/y), and which
; area was hit. Shows the whole mouse_events API end to end.
;
;   Load address: $7200   Invoke: SYS $7200 (or autoboot from an NDI floppy)
;   Assembler:    ca65 --cpu w65c02 -I <runtime/asm>
;   Linker:       ld65 -C mousedemo.cfg mousedemo.o nova.lib

.include "novavm.inc"          ; RegCharOut / RegCursorX/Y / RegFgCol / RegStatus
.include "nova.inc"            ; VGC_MODE / VGC_MOUSE_*
.include "mouse.inc"           ; mouse_show
.include "mouse_events.inc"    ; the event dispatch API

; Text cell <-> mouse coordinate scale: 80 text cols over the 320-wide mouse
; plane, 50 rows over 200 -> 4 mouse units per cell. Buttons are placed by cell
; and their areas registered in mouse units (cell * 4).
CELL      = 4
BTN_ROW   = 8                  ; top text row of the button boxes
BTN_H     = 3                  ; box height in rows
STAT_ROW  = 19                 ; readout row

      .segment "HEADER"
      .byte $00, $72           ; 2-byte load-address prefix -> $7200

      .segment "ZEROPAGE"
strptr: .res 2                 ; string pointer for print_str

      .segment "CODE"

start:
      SEI
      CLD
      LDX   #$FF
      TXS

      LDA   #VGC_MODE_TEXT
      STA   VGC_MODE
      LDA   #$00
      STA   RegCursorEnable    ; hide the blinking text caret
      LDA   #$0E               ; light-blue text
      STA   RegFgCol
      JSR   clear_screen
      JSR   draw_ui

      ; --- wire up mouse events ---
      JSR   mouse_events_init
      LDA   #<on_click
      LDX   #>on_click
      JSR   mouse_set_click_handler
      JSR   register_areas
      JSR   mouse_show         ; enable the hardware pointer

main:
      JSR   wait_vsync
      JSR   mouse_poll         ; edge-detect + dispatch on_click
      BRA   main

; ---------------------------------------------------------------------------
; on_click -- the registered handler. Runs on each button press; the event
; context (mouse_evt_*) is already filled in. Print a readout line.
; ---------------------------------------------------------------------------
on_click:
      LDA   #2
      STA   RegCursorX
      LDA   #STAT_ROW
      STA   RegCursorY

      LDA   #<lbl_btn
      STA   strptr
      LDA   #>lbl_btn
      STA   strptr+1
      JSR   print_str          ; "button=$"
      LDA   mouse_evt_button
      JSR   print_hex

      LDA   #<lbl_x
      STA   strptr
      LDA   #>lbl_x
      STA   strptr+1
      JSR   print_str          ; "  x=$"
      LDA   mouse_evt_xh
      AND   #$01
      JSR   print_nib          ; high bit as one hex digit
      LDA   mouse_evt_xl
      JSR   print_hex

      LDA   #<lbl_y
      STA   strptr
      LDA   #>lbl_y
      STA   strptr+1
      JSR   print_str          ; "  y=$"
      LDA   mouse_evt_y
      JSR   print_hex

      LDA   #<lbl_zone
      STA   strptr
      LDA   #>lbl_zone
      STA   strptr+1
      JSR   print_str          ; "  zone="
      JMP   print_zone_name    ; tail-call; its RTS returns to mouse_poll

; ---------------------------------------------------------------------------
; register_areas -- one area per button box (bounds inclusive, in mouse units).
; ---------------------------------------------------------------------------
register_areas:
      LDX   #0                 ; loop over the 3 area rows in area_tbl
@loop:
      CPX   #(AREA_ROWS * 7)
      BCS   @done
      LDY   #0
@copy:
      LDA   area_tbl,X
      STA   mouse_area_desc,Y
      INX
      INY
      CPY   #7
      BNE   @copy
      PHX
      JSR   mouse_add_area
      PLX
      BRA   @loop
@done:
      RTS

; ---------------------------------------------------------------------------
; draw_ui -- title, help line, and the three button boxes.
; ---------------------------------------------------------------------------
draw_ui:
      LDA   #26
      LDX   #2
      LDY   #<title_str
      STY   strptr
      LDY   #>title_str
      STY   strptr+1
      JSR   print_at

      LDA   #18
      LDX   #4
      LDY   #<help_str
      STY   strptr
      LDY   #>help_str
      STY   strptr+1
      JSR   print_at

      LDA   #8
      LDX   #BTN_ROW+1
      LDY   #<btn1_str
      STY   strptr
      LDY   #>btn1_str
      STY   strptr+1
      JSR   print_at

      LDA   #30
      LDX   #BTN_ROW+1
      LDY   #<btn2_str
      STY   strptr
      LDY   #>btn2_str
      STY   strptr+1
      JSR   print_at

      LDA   #52
      LDX   #BTN_ROW+1
      LDY   #<btn3_str
      STY   strptr
      LDY   #>btn3_str
      STY   strptr+1
      JSR   print_at

      LDA   #2
      LDX   #STAT_ROW-1
      LDY   #<wait_str
      STY   strptr
      LDY   #>wait_str
      STY   strptr+1
      JMP   print_at

; ---------------------------------------------------------------------------
; print_zone_name -- print the label for mouse_evt_area, padded to 6 chars.
; ---------------------------------------------------------------------------
print_zone_name:
      LDA   mouse_evt_area
      CMP   #1
      BEQ   @z1
      CMP   #2
      BEQ   @z2
      CMP   #3
      BEQ   @z3
      LDA   #<zone_none
      LDX   #>zone_none
      BRA   @go
@z1:  LDA   #<btn1_name
      LDX   #>btn1_name
      BRA   @go
@z2:  LDA   #<btn2_name
      LDX   #>btn2_name
      BRA   @go
@z3:  LDA   #<btn3_name
      LDX   #>btn3_name
@go:  STA   strptr
      STX   strptr+1
      JMP   print_str

; ---------------------------------------------------------------------------
; print_at -- print the string at strptr starting at column A, row X.
; ---------------------------------------------------------------------------
print_at:
      STA   RegCursorX
      STX   RegCursorY
      ; fall through
print_str:
      LDY   #0
@l:   LDA   (strptr),Y
      BEQ   @d
      STA   RegCharOut
      INY
      BNE   @l
@d:   RTS

; print_hex -- A as two hex digits.
print_hex:
      PHA
      LSR
      LSR
      LSR
      LSR
      JSR   print_nib
      PLA
      AND   #$0F
      ; fall through to print_nib
; print_nib -- low nibble of A as one hex digit.
print_nib:
      AND   #$0F
      CMP   #10
      BCC   @dig
      CLC
      ADC   #('A' - 10)
      STA   RegCharOut
      RTS
@dig: CLC
      ADC   #'0'
      STA   RegCharOut
      RTS

; clear_screen -- fill all 50 text rows with spaces.
clear_screen:
      LDX   #0
@row:
      STX   RegCursorY
      LDA   #0
      STA   RegCursorX
      LDY   #80
@col:
      LDA   #' '
      STA   RegCharOut
      DEY
      BNE   @col
      INX
      CPX   #50
      BNE   @row
      RTS

; wait_vsync -- block until the VGC frame counter advances.
wait_vsync:
      LDA   RegStatus
@w:   CMP   RegStatus
      BEQ   @w
      RTS

      .segment "RODATA"

title_str: .byte "NOVA MOUSE EVENT DEMO", 0
help_str:  .byte "Move the mouse and click a button.", 0
btn1_str:  .byte "[ RESET ]", 0
btn2_str:  .byte "[ PLAY ]", 0
btn3_str:  .byte "[ QUIT ]", 0
wait_str:  .byte "LAST CLICK:", 0

lbl_btn:   .byte "button=$", 0
lbl_x:     .byte "  x=$", 0
lbl_y:     .byte "  y=$", 0
lbl_zone:  .byte "  zone=", 0

btn1_name: .byte "RESET ", 0
btn2_name: .byte "PLAY  ", 0
btn3_name: .byte "QUIT  ", 0
zone_none: .byte "(none)", 0

; Area table: 3 rows of {x0l,x0h,y0,x1l,x1h,y1,tag}, bounds inclusive in mouse
; units. Boxes sit at rows BTN_ROW..BTN_ROW+2 (mouse_y 32..44), columns 8/30/52
; (mouse_x = col*4).
AREA_ROWS = 3
area_tbl:
      .byte  32, 0, 32,  76, 0, 44, 1     ; RESET  (cols 8..19)
      .byte 120, 0, 32, 164, 0, 44, 2     ; PLAY   (cols 30..41)
      .byte 208, 0, 32, 252, 0, 44, 3     ; QUIT   (cols 52..63)
