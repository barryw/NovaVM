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
FLOOR   = 141                    ; +20 border-inclusive inset (visual floor unchanged)
XMIN    = 28
XMAX    = 252

; floor "thock": low triangle whose pitch drops as it decays (basketball-ish)
SND_FLOOR_START = $0E            ; voice-1 freq hi at impact (~210 Hz)
SND_FLOOR_STEP  = $01            ; freq hi dropped per frame
SND_FLOOR_LEN   = 7              ; frames of pitch drop
; wall "tok": short, higher pulse tick on voice 2
SND_WALL_FREQ   = $2A            ; voice-2 freq hi (~1 kHz)

.segment "ZEROPAGE"
zp_last_frame:  .res 1
zp_frame:       .res 1
ypos:           .res 2           ; 8.8 fixed point
yvel:           .res 2           ; 8.8 signed
xpos:           .res 2
xvel:           .res 2
snd1_timer:     .res 1           ; frames left in the floor pitch drop
snd1_fh:        .res 1           ; current voice-1 freq hi during the drop

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

      LDA   #$00                 ; ypos = 48.0 apex (28 + 20 border inset)
      STA   ypos
      LDA   #48
      STA   ypos+1
      STZ   yvel
      STZ   yvel+1
      LDA   #$00                 ; xpos = 40.0 (20 + 20 border inset), drifting right 1.0 px/f
      STA   xpos
      LDA   #40
      STA   xpos+1
      STZ   xvel
      LDA   #$01
      STA   xvel+1
      STZ   zp_frame

loop:
      JSR   wait_vsync
      JSR   msprite_commit       ; push the pose during vblank
      JSR   sid_update           ; advance the floor pitch drop

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
      JSR   sid_floor            ; thock on floor contact
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
      JSR   sid_wall
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
      JSR   sid_wall
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
      JSR   msprite_set_pos      ; compute this frame's pose; committed next vblank
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

; --- SID bounce sound ------------------------------------------------------
; INTENTIONAL custom effect, NOT the standard NDK path: a hand-crafted multi-voice
; pitch sweep (floor = voices 1+3 in unison swept downward, wall = a voice-2 tick)
; poked straight to the SID for a specific "basketball" character. For an ordinary
; fire-and-forget sound or note, use the NDK instead -- audio_play_sound_async (SID)
; or wts_note_async (WTS); see `nova new --template sprite`. This demo keeps the
; bespoke effect on purpose.
sid_init:
      LDA   #$0F
      STA   $D418                ; master volume (max)
      LDA   #$0A                 ; voice 1 (floor): attack 0, decay 10 (body)
      STA   $D405
      STZ   $D406
      LDA   #$0A                 ; voice 3 (floor, layered for loudness)
      STA   $D413
      STZ   $D414
      LDA   #$04                 ; voice 2 (wall): attack 0, decay 4 (short)
      STA   $D40C
      STZ   $D40D
      STZ   $D409                ; voice 2 pulse width ~50%
      LDA   #$08
      STA   $D40A
      RTS

; floor "thock": low triangle on voices 1+3 (unison = louder), pitch swept down
sid_floor:
      LDA   #SND_FLOOR_START
      STA   snd1_fh
      STA   $D401                ; voice 1 freq hi
      STA   $D40F                ; voice 3 freq hi
      STZ   $D400
      STZ   $D40E
      LDA   #$10
      STA   $D404                ; voice 1 triangle, gate off
      STA   $D412                ; voice 3 triangle, gate off
      LDA   #$11
      STA   $D404                ; voice 1 gate on -> retrigger
      STA   $D412                ; voice 3 gate on
      LDA   #SND_FLOOR_LEN
      STA   snd1_timer
      RTS

; wall "tok": short higher pulse tick on voice 2
sid_wall:
      LDA   #SND_WALL_FREQ
      STA   $D408                ; voice 2 freq hi
      STZ   $D407
      LDA   #$40
      STA   $D40B                ; pulse, gate off
      LDA   #$41
      STA   $D40B                ; pulse, gate on
      RTS

; per-frame: drop the floor voices' pitch while the timer runs
sid_update:
      LDA   snd1_timer
      BEQ   @done
      DEC   snd1_timer
      LDA   snd1_fh
      SEC
      SBC   #SND_FLOOR_STEP
      STA   snd1_fh
      STA   $D401                ; voice 1 freq hi drops
      STA   $D40F                ; voice 3 tracks it
@done:
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
