; VTEXT shared-runtime integration fixture.
;
; Loaded at $7000 by tests/integration/vtext_asm.6502 and entered through
; BASIC SYS. The fixture includes the same lib/vtext.inc/lib/vtext.s library
; that standalone assembly programs are expected to reuse.

.setcpu "65c02"

.include "vtext.inc"

RESULT_BASE     = $2240
REGION_TABLE    = $2400

.segment "CODE"

; $7000: region clear/write/scroll test.
start:
        jmp asm_vtext_region_entry

asm_vtext_region_entry:
        pha
        phx
        phy
        jsr asm_vtext_region_test
        ply
        plx
        pla
        rts

asm_vtext_region_test:
        jsr clear_results
        jsr set_table

        ; Region 0: fixed Infocom-style status line on row 0.
        stz VTEXT_LEFT
        stz VTEXT_TOP
        lda #80
        sta VTEXT_WIDTH
        lda #1
        sta VTEXT_HEIGHT
        stz VTEXT_CURX
        stz VTEXT_CURY
        lda #$F0                ; bg=15, fg=0
        sta VTEXT_COLOR
        stz VTEXT_ATTR
        stz VTEXT_FLAGS
        stz VTEXT_REGION_ID
        jsr vtext_define_region
        sta RESULT_BASE

        ; Region 1: full story body below the fixed status line.
        stz VTEXT_LEFT
        lda #1
        sta VTEXT_TOP
        lda #80
        sta VTEXT_WIDTH
        lda #49
        sta VTEXT_HEIGHT
        stz VTEXT_CURX
        stz VTEXT_CURY
        lda #$0F                ; bg=0, fg=15
        sta VTEXT_COLOR
        stz VTEXT_ATTR
        lda #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        sta VTEXT_FLAGS
        lda #1
        sta VTEXT_REGION_ID
        jsr vtext_define_region
        sta RESULT_BASE + 1

        jsr set_table
        stz VTEXT_REGION_ID
        jsr vtext_select_region
        sta RESULT_BASE + 2
        jsr vtext_clear_region
        sta RESULT_BASE + 3
        lda #<status_left
        sta VTEXT_TABLEL
        lda #>status_left
        sta VTEXT_TABLEH
        jsr write_string
        lda #55
        sta VTEXT_CURX
        stz VTEXT_CURY
        lda #<status_right
        sta VTEXT_TABLEL
        lda #>status_right
        sta VTEXT_TABLEH
        jsr write_string

        jsr set_table
        lda #1
        sta VTEXT_REGION_ID
        jsr vtext_select_region
        sta RESULT_BASE + 4
        jsr vtext_clear_region
        sta RESULT_BASE + 5
        lda #<story_text
        sta VTEXT_TABLEL
        lda #>story_text
        sta VTEXT_TABLEH
        jsr write_string
        sta RESULT_BASE + 6
        jsr set_table
        jsr vtext_store_region
        sta RESULT_BASE + 7

        lda VTEXT_CURY
        sta RESULT_BASE + 8
        lda VTEXT_CURX
        sta RESULT_BASE + 9
        stz VTEXT_ADDRL
        stz VTEXT_ADDRH
        lda #VTEXT_PLANE_COLOR
        jsr read_vram_byte
        sta RESULT_BASE + $10
        lda #80
        sta VTEXT_ADDRL
        stz VTEXT_ADDRH
        lda #VTEXT_PLANE_COLOR
        jsr read_vram_byte
        sta RESULT_BASE + $11
        stz VTEXT_ADDRL
        stz VTEXT_ADDRH
        lda #VTEXT_PLANE_TEXTATTR
        jsr read_vram_byte
        sta RESULT_BASE + $12
        lda #$AA
        sta RESULT_BASE + $0F
        rts

set_table:
        lda #<REGION_TABLE
        sta VTEXT_TABLEL
        lda #>REGION_TABLE
        sta VTEXT_TABLEH
        rts

clear_results:
        ldx #$00
        lda #$00
@loop:
        sta RESULT_BASE,x
        inx
        cpx #$20
        bne @loop
        rts

write_string:
@loop:
        ldy #0
        lda (VTEXT_TABLEL),y
        beq @done
        sta VTEXT_CHAR
        jsr vtext_put_char
        bne @done
        inc VTEXT_TABLEL
        bne @loop
        inc VTEXT_TABLEH
        bra @loop
@done:
        rts

read_vram_byte:
        sta VGC_VRAM_PLANE
        lda VTEXT_ADDRL
        sta VGC_VRAM_ADDRL
        lda VTEXT_ADDRH
        sta VGC_VRAM_ADDRH
        stz VGC_VRAM_CTRL
        lda VGC_VRAM_DATA       ; first read posts the BRAM-backed value
        lda VGC_VRAM_DATA
        rts

status_left:
        .byte "WEST OF HOUSE",0
status_right:
        .byte "SCORE: 0 MOVES: 1",0
story_text:
        .byte "LINE1",$0A
        .byte "LINE2",$0A
        .byte "LINE3",$0A
        .byte "LINE4",$0A
        .byte "LINE5",$0A
        .byte "LINE6",$0A
        .byte "LINE7",$0A
        .byte "LINE8",$0A
        .byte "LINE9",$0A
        .byte "LINE10",$0A
        .byte "LINE11",$0A
        .byte "LINE12",$0A
        .byte "LINE13",$0A
        .byte "LINE14",$0A
        .byte "LINE15",$0A
        .byte "LINE16",$0A
        .byte "LINE17",$0A
        .byte "LINE18",$0A
        .byte "LINE19",$0A
        .byte "LINE20",$0A
        .byte "LINE21",$0A
        .byte "LINE22",$0A
        .byte "LINE23",$0A
        .byte "LINE24",$0A
        .byte "LINE25",$0A
        .byte "LINE26",$0A
        .byte "LINE27",$0A
        .byte "LINE28",$0A
        .byte "LINE29",$0A
        .byte "LINE30",$0A
        .byte "LINE31",$0A
        .byte "LINE32",$0A
        .byte "LINE33",$0A
        .byte "LINE34",$0A
        .byte "LINE35",$0A
        .byte "LINE36",$0A
        .byte "LINE37",$0A
        .byte "LINE38",$0A
        .byte "LINE39",$0A
        .byte "LINE40",$0A
        .byte "LINE41",$0A
        .byte "LINE42",$0A
        .byte "LINE43",$0A
        .byte "LINE44",$0A
        .byte "LINE45",$0A
        .byte "LINE46",$0A
        .byte "LINE47",$0A
        .byte "LINE48",$0A
        .byte "LINE49",$0A
        .byte "LINE50",$0A
        .byte "LINE51",$0A
        .byte "LINE52",$0A
        .byte "LINE53",$0A
        .byte "LINE54",$0A
        .byte "LINE55",$0A
        .byte "LINE56",$0A
        .byte "LINE57",$0A
        .byte "LINE58",$0A
        .byte "LINE59",$0A
        .byte "LINE60",$0A
        .byte "LINE61",$0A
        .byte "LINE62",$0A
        .byte "LINE63",$0A
        .byte "LINE64",$0A
        .byte "LINE65",$0A
        .byte "LINE66",$0A
        .byte "LINE67",$0A
        .byte "LINE68",$0A
        .byte "LINE69",$0A
        .byte "LINE70",$0A
        .byte "LINE71",$0A
        .byte "LINE72",$0A
        .byte "LINE73",$0A
        .byte "LINE74",$0A
        .byte "LINE75",$0A
        .byte "LINE76",$0A
        .byte "LINE77",$0A
        .byte "LINE78",$0A
        .byte "LINE79",$0A
        .byte "LINE80",$0A
        .byte "LINE81",$0A
        .byte "LINE82",$0A
        .byte "LINE83",$0A
        .byte "LINE84",$0A
        .byte "LINE85",$0A
        .byte "LINE86",$0A
        .byte "LINE87",$0A
        .byte "LINE88",$0A
        .byte "LINE89",$0A
        .byte "LINE90",$0A
        .byte "LINE91",$0A
        .byte "LINE92",$0A
        .byte "LINE93",$0A
        .byte "LINE94",$0A
        .byte "LINE95",$0A
        .byte "LINE96",$0A
        .byte "LINE97",$0A
        .byte "LINE98",$0A
        .byte "LINE99",$0A
        .byte "LINE100",0

.include "vtext.s"
