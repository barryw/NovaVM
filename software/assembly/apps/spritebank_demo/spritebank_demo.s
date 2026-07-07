; spritebank_demo.s -- load an NSPR bank, spawn a character, animate it.
;
; Proves the on-device NSPR loader end to end on real hardware: parse an embedded
; .nsp bank (a shaded ball drawn as a 32x32 metasprite with an 8-frame squash
; cycle), stage its shapes to sprite RAM, spawn it, and bounce it around the
; screen -- Y follows a bounce arc synced to the squash frames, X bounces off
; the walls.
;
; Load address: $7200   Invoke: SYS $7200

.include "nova.inc"
.include "spritebank.inc"
.include "msprite.inc"

MODE_TEXT_GFX = 2
FRAME_TICKS   = 4                ; vblanks between animation-frame advances

.segment "ZEROPAGE"
zp_last_frame:  .res 1
zp_frame:       .res 1           ; current bounce frame 0..7
zp_delay:       .res 1
zp_x:           .res 1           ; metasprite X
zp_xdir:        .res 1           ; +2 / -2 (as $FE)

.segment "HEADER"
      .byte $00, $72

.segment "CODE"
start:
      SEI
      CLD
      LDX   #$FF
      TXS

      JSR   init_display
      JSR   msprite_init

      LDA   #<bank               ; parse the embedded bank
      STA   spritebank_src
      LDA   #>bank
      STA   spritebank_src+1
      JSR   spritebank_open

      STZ   spritebank_xram_base+0   ; shapes -> XRAM $020000 -> sprite RAM
      STZ   spritebank_xram_base+1
      LDA   #$02
      STA   spritebank_xram_base+2
      JSR   spritebank_load_shapes
      JSR   spritebank_load_to_sprites

      LDA   #0                   ; spawn BALL (character 0)
      JSR   spritebank_spawn

      LDA   #120
      STA   zp_x
      LDA   #2
      STA   zp_xdir
      STZ   zp_frame
      LDA   #FRAME_TICKS
      STA   zp_delay

loop:
      JSR   wait_vsync

      LDA   zp_x                 ; horizontal: bounce off the walls
      CLC
      ADC   zp_xdir
      STA   zp_x
      CMP   #232
      BCC   @chk_left
      LDA   #$FE                 ; hit right wall -> move left
      STA   zp_xdir
      BRA   @anim
@chk_left:
      LDA   zp_x
      CMP   #8
      BCS   @anim
      LDA   #2                   ; hit left wall -> move right
      STA   zp_xdir

@anim:
      DEC   zp_delay             ; advance the bounce frame every FRAME_TICKS
      BNE   @place
      LDA   #FRAME_TICKS
      STA   zp_delay
      INC   zp_frame
      LDA   zp_frame
      AND   #$07
      STA   zp_frame
      JSR   spritebank_set_frame

@place:
      LDX   zp_frame             ; Y follows the bounce arc for this frame
      LDA   bounce_y,X
      TAY
      LDA   zp_x
      STA   NVR0L
      STZ   NVR0H
      LDA   spritebank_handle
      JSR   msprite_set_pos
      JSR   msprite_commit
      BRA   loop

init_display:
      STZ   VGC_BGCOL
      LDA   #1
      STA   VGC_FGCOL
      STZ   VGC_BORDER
      LDA   #MODE_TEXT_GFX
      STA   VGC_MODE
      LDA   #$0C                 ; clear screen
      STA   VGC_CHAROUT
      RTS

wait_vsync:
@w:   LDA   VGC_STATUS
      CMP   zp_last_frame
      BEQ   @w
      STA   zp_last_frame
      RTS

.segment "RODATA"
bounce_y:                        ; metasprite Y per frame: high at apex, low (floor) at the squash frames
      .byte 40, 55, 75, 89, 89, 75, 55, 40
bank:
      .incbin "demo.nsp"
