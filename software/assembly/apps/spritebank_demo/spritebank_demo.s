; spritebank_demo.s -- load an NSPR bank, spawn a character, animate it.
;
; Proves the on-device NSPR loader end to end on real hardware: parse an embedded
; .nsp bank (a shaded ball drawn as a 32x32 metasprite with round + squash
; frames), stage its shapes to sprite RAM, spawn it, and bounce it around the
; screen with real physics -- 8.8 fixed-point position + velocity, constant
; gravity, elastic floor + wall bounces. The squash frame is chosen by how close
; the ball is to the floor, so it flattens on impact.
;
; Load address: $7200   Invoke: SYS $7200

.include "nova.inc"
.include "spritebank.inc"
.include "msprite.inc"

MODE_TEXT_GFX = 2
GRAVITY = $0016                  ; 8.8 accel added to yvel each frame (~0.09 px/f^2)
FLOOR   = 121                    ; max ypos (ball bottom sits ~29px below -> screen ~150)
XMIN    = 8
XMAX    = 232

.segment "ZEROPAGE"
zp_last_frame:  .res 1
zp_frame:       .res 1
ypos:           .res 2           ; 8.8 fixed point
yvel:           .res 2           ; 8.8 signed
xpos:           .res 2
xvel:           .res 2

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
      JSR   sid_init

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

      LDA   #$00                 ; ypos = 28.0 (the apex the constant bounce reaches)
      STA   ypos
      LDA   #28
      STA   ypos+1
      STZ   yvel
      STZ   yvel+1
      LDA   #$00                 ; xpos = 20.0, drifting right at 1.0 px/frame
      STA   xpos
      LDA   #20
      STA   xpos+1
      STZ   xvel
      LDA   #$01
      STA   xvel+1
      STZ   zp_frame

loop:
      JSR   wait_vsync

      ; --- vertical: gravity, integrate, elastic floor ---
      CLC                        ; yvel += GRAVITY
      LDA   yvel
      ADC   #<GRAVITY
      STA   yvel
      LDA   yvel+1
      ADC   #>GRAVITY
      STA   yvel+1
      CLC                        ; ypos += yvel
      LDA   ypos
      ADC   yvel
      STA   ypos
      LDA   ypos+1
      ADC   yvel+1
      STA   ypos+1
      LDA   ypos+1
      CMP   #FLOOR
      BCC   @yok
      LDA   #FLOOR               ; clamp to floor
      STA   ypos+1
      STZ   ypos
      LDA   yvel+1
      BMI   @yok                 ; already moving up -> just clamp
      ; ponytail: relaunch at a constant speed instead of reflecting the impact
      ; velocity -- perpetual identical bounces, immune to fixed-point clamp loss.
      STZ   yvel                 ; yvel = -4.0 (8.8) upward
      LDA   #$FC
      STA   yvel+1
      JSR   sid_bounce           ; blip on floor contact
@yok:

      ; --- horizontal: integrate, bounce off the walls ---
      CLC
      LDA   xpos
      ADC   xvel
      STA   xpos
      LDA   xpos+1
      ADC   xvel+1
      STA   xpos+1
      LDA   xpos+1
      CMP   #XMAX
      BCC   @chk_left
      LDA   #XMAX
      STA   xpos+1
      STZ   xpos
      LDA   xvel+1
      BMI   @xok                 ; already moving left -> just clamp
      JSR   neg_xvel
      BRA   @xok
@chk_left:
      LDA   xpos+1
      CMP   #XMIN
      BCS   @xok
      LDA   #XMIN
      STA   xpos+1
      STZ   xpos
      LDA   xvel+1
      BPL   @xok                 ; already moving right -> just clamp
      JSR   neg_xvel
@xok:

      ; --- squash frame from distance to the floor ---
      LDA   #FLOOR
      SEC
      SBC   ypos+1               ; A = floor - ypos = distance
      CMP   #2
      BCC   @full
      CMP   #7
      BCC   @light
      LDA   #0                   ; round
      BRA   @have
@full:
      LDA   #2                   ; full squash
      BRA   @have
@light:
      LDA   #1                   ; light squash
@have:
      CMP   zp_frame
      BEQ   @place
      STA   zp_frame
      JSR   spritebank_set_frame

@place:
      LDA   xpos+1
      STA   NVR0L
      STZ   NVR0H
      LDY   ypos+1
      LDA   spritebank_handle
      JSR   msprite_set_pos
      JSR   msprite_commit
      JMP   loop

; xvel = -xvel (16-bit negate)
neg_xvel:
      SEC
      LDA   #0
      SBC   xvel
      STA   xvel
      LDA   #0
      SBC   xvel+1
      STA   xvel+1
      RTS

sid_init:
      LDA   #$0F
      STA   $D418                ; master volume
      LDA   #$08
      STA   $D405                ; voice 1: attack 0, decay 8
      STZ   $D406                ; sustain 0, release 0 (percussive)
      STZ   $D400
      LDA   #$10
      STA   $D401                ; freq ~240 Hz "bonk"
      RTS

; retrigger the voice-1 envelope -> a short bounce blip
sid_bounce:
      LDA   #$10
      STA   $D404                ; triangle, gate off
      LDA   #$11
      STA   $D404                ; triangle, gate on
      RTS

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
bank:
      .incbin "demo.nsp"
