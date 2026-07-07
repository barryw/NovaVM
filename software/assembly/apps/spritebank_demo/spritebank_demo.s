; spritebank_demo.s -- load an NSPR bank, spawn a character, animate it walking.
;
; Proves the on-device NSPR loader end to end on real hardware: parse an embedded
; .nsp bank, stage its shapes to sprite RAM, spawn a character as a metasprite,
; and cycle its animation frames while it scrolls across the screen.
;
; Load address: $7200   Invoke: SYS $7200

.include "nova.inc"
.include "spritebank.inc"
.include "msprite.inc"

MODE_TEXT_GFX = 2
STEP_FRAMES   = 10               ; vblanks between animation-frame advances

.segment "ZEROPAGE"
zp_last_frame:  .res 1
zp_frame:       .res 1           ; current WALK frame (toggles 0/1)
zp_delay:       .res 1
zp_x:           .res 1           ; metasprite X (wraps at 256)

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

      LDA   #1                   ; spawn BOSS (character 1)
      JSR   spritebank_spawn

      STZ   zp_x
      STZ   zp_frame
      LDA   #STEP_FRAMES
      STA   zp_delay

loop:
      JSR   wait_vsync

      LDA   zp_x                 ; scroll right, wrapping at 256
      CLC
      ADC   #2
      STA   zp_x
      STA   NVR0L
      STZ   NVR0H
      LDY   #90
      LDA   spritebank_handle
      JSR   msprite_set_pos

      DEC   zp_delay             ; advance the WALK frame every STEP_FRAMES
      BNE   @commit
      LDA   #STEP_FRAMES
      STA   zp_delay
      LDA   zp_frame
      EOR   #$01
      STA   zp_frame
      JSR   spritebank_set_frame
@commit:
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
bank:
      .incbin "demo.nsp"
