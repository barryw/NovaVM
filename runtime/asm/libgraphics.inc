; libgraphics.inc — GRAPHICS module (MODULE_ID_GRAPHICS = $01) fn-id contract.
;
; The canonical paged graphics library: the superset of every NDK graphics
; driver, callable identically by every runtime through the Phase-3 lib_call ABI.
; Built once (modules/graphics/graphics.s), staged to XRAM $060000, paged into
; bank 1 on first lib_call.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_GRAPHICS
;   LIB_FN_ID  ($0301) = one of the GFN_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;       - signed-16 X/Y values occupy the LOW word of a cell (high word ignored).
;       - BYTES args pack ptr16 in the low word, len16 in the high word.
;   JSR lib_call
;   -> LIB_RESULT ($0313) = 32-bit LE result (for reporter-style fns)
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code
;
; Each fn is a thin leaf wrapper: read the arg cells -> load the driver's
; VGC_P*/register/struct inputs -> JSR the NDK driver entry -> write
; LIB_RESULT/LIB_STATUS. fn-ids are grouped per domain with room to grow.

.ifndef LIBGRAPHICS_INC
LIBGRAPHICS_INC = 1

.include "libabi.inc"            ; MODULE_ID_GRAPHICS, LIB_* mailbox cells, LERR_*

; =====================================================================
; $00-$0F  draw  (driver: vgc.s — vgc_gcls/gcolor/plot/unplot/line/circle/
;                 rect/fill/paint/gtext; primitives issue VCMD_* via vgc_cmd
;                 and the wrapper waits via vgc_wait_cmd)
; =====================================================================
GFN_GCLS      = $00   ; ()                                  -> vgc_gcls
GFN_GCOLOR    = $01   ; (color:ARG0)                        -> VGC_P0; vgc_gcolor
GFN_PLOT      = $02   ; (x:ARG0 s16, y:ARG1 s16)            -> VGC_P0..P3; vgc_plot
GFN_UNPLOT    = $03   ; (x:ARG0, y:ARG1)                    -> VGC_P0..P3; vgc_unplot
GFN_LINE      = $04   ; (x0:ARG0,y0:ARG1,x1:ARG2,y1:ARG3)   -> VGC_P0..P7; vgc_line
GFN_CIRCLE    = $05   ; (cx:ARG0,cy:ARG1,rx:ARG2,ry:ARG3)   -> VGC_P0..P7; vgc_circle (ry=rx if 0 -> ellipse superset)
GFN_RECT      = $06   ; (x0,y0,x1,y1:ARG0..3)               -> VGC_P0..P7; vgc_rect
GFN_FILL      = $07   ; (x0,y0,x1,y1:ARG0..3)               -> VGC_P0..P7; vgc_fill
GFN_PAINT     = $08   ; (x:ARG0, y:ARG1)                    -> VGC_P0..P3; vgc_paint
GFN_GTEXT     = $09   ; (x:ARG0, y:ARG1, BYTES str:ARG2)    -> VGC_P0..P5 + FIO_NAME; vgc_gtext
;             $0A-$0F reserved

; =====================================================================
; $10-$1F  text / mode  (driver: vgc.s text-side)
;   COLOR(fg,bg,border) FONT MODE REVERSE/off FLASH/off LOCATE CLS CLSWIN
;   DISPLAYon/off  -> vgc_set_fg/bg/border, vgc_set_font, vgc_set_mode,
;   vgc_reverse_default/off, vgc_flash_on/off, vgc_locate, vgc_cls,
;   vgc_display_on/off.
;
; Arg-register conventions, verified against runtime/asm/vgc.s:
;   - vgc_set_fg/bg/border take X (STX VGC_FGCOL/BGCOL/BORDER) — pure register
;     stores, no VGC command issued, no wait needed.
;   - vgc_set_font / vgc_set_mode take X (STX VGC_FONT / VGC_MODE) — register
;     stores, no wait.
;   - vgc_locate reads VGC_P0 (col) -> VGC_CURSX and VGC_P1 (row) -> VGC_CURSY,
;     so the wrapper loads ARG0->VGC_P0, ARG1->VGC_P1 — register stores, no wait.
;   - vgc_reverse_default/off, vgc_flash_on/off mutate VGC_TXTFLAGS only — no wait.
;   - vgc_display_on/off store $0F/$00 to VGC_DIMMER — register store, no wait.
;   - vgc_cls issues a char-out ($0C form-feed) command and waits internally.
;
; NOTE: vtext.s (region/cursor/puts, 16 ops) does NOT fit alongside these in
; $10-$1F; it gets its own fn-id range in a later batch. CLSWIN has no driver
; entry in vgc.s, so GFN_CLSWIN is reserved here and stubbed to gfn_unimpl
; (-> LERR_NO_FN) until a window-clear driver routine exists.
GFN_COLOR      = $10   ; (fg:ARG0, bg:ARG1, border:ARG2) -> vgc_set_fg/bg/border (X each)
GFN_FONT       = $11   ; (slot:ARG0)                      -> X; vgc_set_font
GFN_MODE       = $12   ; (mode:ARG0)                      -> X; vgc_set_mode
GFN_REVERSE    = $13   ; ()  reverse-on                   -> vgc_reverse_default
GFN_REVERSEOFF = $14   ; ()                               -> vgc_reverse_off
GFN_FLASH      = $15   ; ()                               -> vgc_flash_on
GFN_FLASHOFF   = $16   ; ()                               -> vgc_flash_off
GFN_LOCATE     = $17   ; (col:ARG0, row:ARG1)             -> VGC_P0/P1; vgc_locate
GFN_CLS        = $18   ; ()                               -> vgc_cls (issues cmd, waits)
GFN_CLSWIN     = $19   ; ()  clear active window          -> STUB (no vgc entry) -> gfn_unimpl
GFN_DISPLAYON  = $1A   ; ()                               -> vgc_display_on  (VGC_DIMMER=$0F)
GFN_DISPLAYOFF = $1B   ; ()                               -> vgc_display_off (VGC_DIMMER=$00)
;             $1C-$1F reserved

; =====================================================================
; $20-$3F  hw-sprite  (driver: runtime/asm/sprite.s)
;   define/row/clear/copy/pos/enable/disable/flip/priority/shape/setreg/
;   getx/gety + collision API (status/mask/clear/read-clear/irq)
;
; Arg-register conventions, VERIFIED against runtime/asm/sprite.s and the VGC
; ExecuteSpriteCommand mapping (VirtualGraphicsController.cs): VGC_P<n> is the
; command register $A011+n; the sprite engine reads P0=sprite-index for every
; command. Sprite X is a full 16-bit (signed) value (low byte VGC_SPR_XL, high
; byte VGC_SPR_XH); sprite Y is a single unsigned byte (VGC_SPR_YL; the YH field
; is reserved/no-op). So only X carries the s16 contract for sprites.
;
; Command-issuing ops (define/row/clear/copy/pos/enable/disable/flip/priority)
; go through sprite_command -> STA VGC_CMD; the wrapper waits via vgc_wait_cmd
; (busy bit reads 0 in the emulator -> no-op wait) and sets STATUS=OK.
; Register-store ops (shape/setreg) write the $A040 sprite attribute block
; directly -> no command, no wait. Reporter ops (getx/gety + every collision
; read) copy the driver's A/Y return into LIB_RESULT (4-byte LE, high bytes
; zeroed) then STATUS=OK.
;
; Layouts (ARGn low word unless noted; index is 0-15):
GFN_SPR_DEFINE   = $20  ; (idx:ARG0, x:ARG1 0-15, y:ARG2 0-15, color:ARG3) -> P0/P1/P2/P3; VCMD_SPRDEF
GFN_SPR_ROW      = $21  ; (idx:ARG0, row:ARG1, bytes0_3:ARG2, bytes4_7:ARG3) -> P0/P1, P2..P9; sprite_row
GFN_SPR_CLEAR    = $22  ; (idx:ARG0)                       -> P0; sprite_clear
GFN_SPR_COPY     = $23  ; (src:ARG0, dst:ARG1)             -> P0/P1; sprite_copy
GFN_SPR_POS      = $24  ; (idx:ARG0, x:ARG1 s16, y:ARG2)   -> P0, P1/P2(=x), P3(=y); sprite_pos
GFN_SPR_ENABLE   = $25  ; (idx:ARG0)                       -> P0; sprite_enable
GFN_SPR_DISABLE  = $26  ; (idx:ARG0)                       -> P0; sprite_disable
GFN_SPR_FLIP     = $27  ; (idx:ARG0, flags:ARG1)           -> P0/P1; sprite_flip
GFN_SPR_PRIORITY = $28  ; (idx:ARG0, pri:ARG1)             -> P0/P1; sprite_priority
GFN_SPR_SHAPE    = $29  ; (idx:ARG0, shape:ARG1)           -> X/A; sprite_set_shape (reg store)
GFN_SPR_SETREG   = $2A  ; (idx:ARG0, field:ARG1, val:ARG2) -> X/Y/A; sprite_set_reg8 (reg store)
GFN_SPR_SETREG16 = $2B  ; (idx:ARG0, field:ARG1, val16:ARG2) -> X/Y/NVR0L,NVR0H; sprite_set_reg16
GFN_SPR_GETX     = $2C  ; (idx:ARG0) reporter -> RESULT = 16-bit X (A=hi,Y=lo); sprite_get_x
GFN_SPR_GETY     = $2D  ; (idx:ARG0) reporter -> RESULT = Y byte (in Y); sprite_get_y
;             $2E-$2F reserved
; --- sprite-sprite collision API ($30-$35) ---
GFN_SPR_COLL_STATUS    = $30  ; () reporter -> RESULT = VGC_COLLST low byte (A); sprite_collision_status
GFN_SPR_COLL_MASK      = $31  ; () reporter -> RESULT = 16-bit mask (A=hi,Y=lo); sprite_collision_mask
GFN_SPR_COLL_CLEAR     = $32  ; () clear mask + ack IRQ; sprite_collision_clear
GFN_SPR_COLL_READCLEAR = $33  ; () reporter -> RESULT = 16-bit mask, then clear; sprite_collision_read_clear
GFN_SPR_COLL_IRQON     = $34  ; () enable sprite-sprite collision IRQ (+CLI); sprite_collision_irq_enable
GFN_SPR_COLL_IRQOFF    = $35  ; () disable sprite-sprite collision IRQ; sprite_collision_irq_disable
; --- sprite-background collision API ($36-$3B) ---
GFN_SPR_BG_STATUS      = $36  ; () reporter -> RESULT = VGC_COLLBG low byte (A); sprite_background_collision_status
GFN_SPR_BG_MASK        = $37  ; () reporter -> RESULT = 16-bit mask (A=hi,Y=lo); sprite_background_collision_mask
GFN_SPR_BG_CLEAR       = $38  ; () clear bg mask + ack IRQ; sprite_background_clear
GFN_SPR_BG_READCLEAR   = $39  ; () reporter -> RESULT = 16-bit mask, then clear; sprite_background_read_clear
GFN_SPR_BG_IRQON       = $3A  ; () enable sprite-bg collision IRQ (+CLI); sprite_background_irq_enable
GFN_SPR_BG_IRQOFF      = $3B  ; () disable sprite-bg collision IRQ; sprite_background_irq_disable
;             $3C-$3F reserved

; =====================================================================
; $40-$4F  copper  (drivers: copper.s + copper_split.s)
;   list/add/clear/on/off/use/end/split/set_reg/set_sprite_reg
;
; Arg-register conventions, VERIFIED against runtime/asm/copper.s,
; copper.inc, copper_split.s, copper_split.inc and the VGC
; ExecuteCopperCommand mapping (VirtualGraphicsController.cs).
;
; The copper is a raster co-processor: it edits up to 128 display "lists" of
; (scanline-position, VGC-register, value) write events, then plays them back
; against the active list every frame. The driver entries issue VCMD_COPPER*
; via copper_command (STA VGC_CMD); the wrapper waits via vgc_wait_cmd and sets
; STATUS=OK. copper_add reads its inputs from VGC_P0..P5:
;   P0/P1 = x (column, 0..GfxWidth-1), P2 = y (scanline, 0..GfxHeight-1),
;   P3/P4 = register specifier, P5 = value. Position = y*GfxWidth + x.
;
; set_reg / set_sprite_reg: copper.s exposes copper_set_reg_index (A=reg index
; -> P3/P4) and copper_set_sprite_reg (X=sprite index, A=field offset -> P3/P4)
; as register-prep helpers — neither issues a command on its own, exactly as
; copper_split_mode composes copper_set_reg_index + copper_add internally. The
; standalone helpers leave no observable bus effect, so GFN_COPPER_SET_REG /
; GFN_COPPER_SET_SPRITE_REG expose the COMPLETE add-a-register-write op: load
; x/y/value into P0/P1/P2/P5, prep the register specifier into P3/P4 via the
; matching helper, then issue copper_add. The result is an observable
; CopperEvent on the active list.
;
; Layouts (ARGn low word unless noted):
GFN_COPPER_LIST       = $40  ; (idx:ARG0)                         -> P0; copper_list (VCMD_COPPERLIST)
GFN_COPPER_ADD        = $41  ; (x:ARG0 s16, y:ARG1, reg:ARG2, val:ARG3) -> P0/P1,P2,P3/P4,P5; copper_add
GFN_COPPER_CLEAR      = $42  ; ()                                 -> copper_clear (VCMD_COPPERCLR)
GFN_COPPER_ON         = $43  ; ()                                 -> copper_on   (VCMD_COPPERENA)
GFN_COPPER_OFF        = $44  ; ()                                 -> copper_off  (VCMD_COPPERDIS)
GFN_COPPER_USE        = $45  ; (idx:ARG0)                         -> P0; copper_use (VCMD_COPPERUSE)
GFN_COPPER_END        = $46  ; ()                                 -> copper_list_end (VCMD_COPPEREND)
GFN_COPPER_SPLIT      = $47  ; (idx:ARG0, splitY:ARG1, mode0:ARG2, mode1:ARG3) -> P0..P3; copper_split_mode
GFN_COPPER_SET_REG    = $48  ; (x:ARG0 s16, y:ARG1, regIndex:ARG2, val:ARG3)
                             ;   -> P0/P1=x, P2=y, copper_set_reg_index(A=regIndex)->P3/P4,
                             ;      P5=val, copper_add. Observable CopperEvent.
GFN_COPPER_SET_SPRITE_REG = $49 ; (x:ARG0 s16, y:ARG1, sprIdx|field:ARG2, val:ARG3)
                             ;   ARG2 low byte = sprite index, byte 1 = VGC_SPR_*_OFF field.
                             ;   -> P0/P1=x, P2=y, copper_set_sprite_reg(X=sprIdx,A=field)->P3/P4,
                             ;      P5=val, copper_add. Observable CopperEvent.
;             $4A-$4F reserved

; =====================================================================
; $50-$5F  blit / dma  (drivers: runtime/asm/blitter.s + runtime/asm/dma.s)
;   blitcopy blitfill blit-start blit-wait dmacopy dmafill
;   + blit status/err/count + dma status/err/count reporters
;
; Arg-register conventions, VERIFIED against runtime/asm/blitter.s+blitter.inc,
; runtime/asm/dma.s+dma.inc, and the BLT_*/DMA_* register map in nova.inc
; (mirrored by VirtualBlitterController / VirtualDmaController on the bus).
;
; Both controllers are 24-bit-address, length/size-bounded engines over a set of
; unified memory "spaces" (CPU=0, VGC_CHAR=1, VGC_COLOR=2, VGC_GFX=3,
; VGC_SPRITE=4, XRAM=5, VGC_TEXTATTR=7 — BLT_SPACE_* alias the DMA_SPACE_*).
; The drivers load the hardware registers, write *_CMD_REG = *_CMD_START, then
; spin in *_wait on *_STATUS_REG until it leaves BUSY; they return A=0 on OK,
; A=1 on error (detail left in *_ERRCODE_REG). The wrappers marshal the ARG
; cells into the hardware registers, JSR the driver, and set STATUS=OK (the
; driver already self-waited; the reporters expose the controller's status/
; errcode/count for callers that want the detail).
;
; DMA is a 1-D bulk move: src/dst are flat 24-bit offsets within their space.
;   ARG0: byte0=srcSpace, byte1=dstSpace, byte2=fillValue (fill only)
;   ARG1: src offset (24-bit, low 3 bytes; ignored by fill)
;   ARG2: dst offset (24-bit, low 3 bytes)
;   ARG3: length    (24-bit, low 3 bytes)
;
; BLIT is a 2-D rectangular move. To keep the op within 4 arg cells the wrapper
; uses a tightly-packed rectangle: srcStride = dstStride = width (no padding
; between rows). Callers needing arbitrary strides drive the BLT_* registers
; directly + GFN_BLIT_START / GFN_BLIT_WAIT.
;   ARG0: byte0=srcSpace, byte1=dstSpace, byte2=fillValue (fill only)
;   ARG1: src offset (24-bit; ignored by fill)
;   ARG2: dst offset (24-bit)
;   ARG3: byte0/1 = width (16-bit), byte2/3 = height (16-bit)
;
; Reporters write LIB_RESULT (4-byte LE, high bytes zeroed) then STATUS=OK:
;   *_STATUS -> the controller status byte (BUSY/OK/ERROR),
;   *_ERR    -> the controller error byte (0 = none),
;   *_COUNT  -> 24-bit bytes-moved counter (3 register bytes -> RESULT low 24 bits).
GFN_BLITCOPY    = $50  ; (ARG0=spaces+fill, ARG1=src, ARG2=dst, ARG3=w|h) -> blitter_copy
GFN_BLITFILL    = $51  ; (ARG0=dstSpace+fill, ARG2=dst, ARG3=w|h)         -> blitter_fill
GFN_BLIT_START  = $52  ; () issue BLT_CMD_START on pre-loaded regs + wait  -> blitter_start_copy
GFN_BLIT_WAIT   = $53  ; () poll BLT_STATUS to completion                  -> blitter_wait
GFN_DMACOPY     = $54  ; (ARG0=spaces+fill, ARG1=src, ARG2=dst, ARG3=len) -> dma_copy
GFN_DMAFILL     = $55  ; (ARG0=dstSpace+fill, ARG2=dst, ARG3=len)         -> dma_fill
GFN_BLIT_STATUS = $56  ; () reporter -> RESULT = BLT_STATUS_REG byte
GFN_BLIT_ERR    = $57  ; () reporter -> RESULT = BLT_ERRCODE_REG byte
GFN_BLIT_COUNT  = $58  ; () reporter -> RESULT = 24-bit BLT bytes-written
GFN_DMA_STATUS  = $59  ; () reporter -> RESULT = DMA_STATUS_REG byte
GFN_DMA_ERR     = $5A  ; () reporter -> RESULT = DMA_ERRCODE_REG byte
GFN_DMA_COUNT   = $5B  ; () reporter -> RESULT = 24-bit DMA bytes-moved
GFN_BLIT_START_FILL = $5C ; () run blitter_fill on caller-preloaded BLT_* regs:
                          ;   zeros SRC/SRCSTRIDE/CKEY, sets MODE_FILL, CMD_START,
                          ;   waits. Mate to GFN_BLIT_START ($52) for the arbitrary-
                          ;   stride BLITFILL path (BASIC loads BLT_DST*/STRIDE/
                          ;   WIDTH/HEIGHT/FILLVALUE, then calls this).
;             $5D-$5F reserved

; =====================================================================
; $60-$7F  vsprite  (driver: runtime/asm/vsprite.s — batch 4b.7)
;   blit/fill/rotate/save-restore-bg/gfx_* + scene compositor
;
; THE MARSHALLING PROBLEM. A virtual sprite needs far more state than the four
; 16-byte arg cells can carry: src/dst/bg/orig/rot 24-bit addresses + 16-bit
; w/h + strides + colorkey + flags + angle + x/y, and the scene ops take a
; descriptor LIST. vsprite.s keeps all that state in a CONTIGUOUS BSS block
; (VSPRITE_SRCSPACE..VSPRITE_SCENE_WORKSTRH, set directly by callers), so the
; ABI is: the caller fills a struct in shared RAM laid out byte-for-byte as that
; block, and passes it as a single BYTES arg. Every vsprite op shares this one
; copy-then-run shape.
;
;   ARG0 = BYTES(ptr16 in the low word, len16 in the high word) of a caller
;          struct that mirrors the VSPRITE config block starting at field
;          offset 0 = VSPRITE_SRCSPACE. The wrapper copies `len` bytes (clamped
;          to GFX_VS_CFG_MAX) into the module's VSPRITE_* BSS via (LIB_ZP),Y,
;          then JSRs the driver entry. Callers only need to fill the prefix of
;          the struct their op @requires; trailing fields may be omitted by
;          passing a shorter len.
;
; Config-struct field offsets (VERIFIED against the BSS order in vsprite.s and
; the VSPRITE_DESC_* layout in vsprite.inc). These are byte offsets from the
; start of the caller struct == offsets into the VSPRITE_* BSS block:
GFX_VS_OFF_SRCSPACE   = 0
GFX_VS_OFF_DSTSPACE   = 1
GFX_VS_OFF_SRCADDRL   = 2
GFX_VS_OFF_SRCADDRM   = 3
GFX_VS_OFF_SRCADDRH   = 4
GFX_VS_OFF_DSTADDRL   = 5
GFX_VS_OFF_DSTADDRM   = 6
GFX_VS_OFF_DSTADDRH   = 7
GFX_VS_OFF_WIDTHL     = 8
GFX_VS_OFF_WIDTHH     = 9
GFX_VS_OFF_HEIGHTL    = 10
GFX_VS_OFF_HEIGHTH    = 11
GFX_VS_OFF_SRCSTRL    = 12
GFX_VS_OFF_SRCSTRH    = 13
GFX_VS_OFF_DSTSTRL    = 14
GFX_VS_OFF_DSTSTRH    = 15
GFX_VS_OFF_BGSPACE    = 16
GFX_VS_OFF_BGADDRL    = 17
GFX_VS_OFF_BGADDRM    = 18
GFX_VS_OFF_BGADDRH    = 19
GFX_VS_OFF_BGSTRL     = 20
GFX_VS_OFF_BGSTRH     = 21
GFX_VS_OFF_COLORKEY   = 22
GFX_VS_OFF_ORIGSPACE  = 23
GFX_VS_OFF_ORIGADDRL  = 24
GFX_VS_OFF_ORIGADDRM  = 25
GFX_VS_OFF_ORIGADDRH  = 26
GFX_VS_OFF_ORIGSTRL   = 27
GFX_VS_OFF_ORIGSTRH   = 28
GFX_VS_OFF_ROTSPACE   = 29
GFX_VS_OFF_ROTADDRL   = 30
GFX_VS_OFF_ROTADDRM   = 31
GFX_VS_OFF_ROTADDRH   = 32
GFX_VS_OFF_ROTSTRL    = 33
GFX_VS_OFF_ROTSTRH    = 34
GFX_VS_OFF_ROTANGLE   = 35
GFX_VS_OFF_FILLVALUE  = 36
GFX_VS_OFF_FLAGS      = 37
GFX_VS_OFF_XL         = 38
GFX_VS_OFF_XH         = 39
GFX_VS_OFF_Y          = 40
; scene block (only the scene ops need these; SCENE_REMAIN at offset 44 is an
; internal counter reset by the driver, so it is harmless to copy over):
GFX_VS_OFF_SCENE_ADDRL    = 41
GFX_VS_OFF_SCENE_ADDRH    = 42
GFX_VS_OFF_SCENE_COUNT    = 43
GFX_VS_OFF_SCENE_XL       = 45
GFX_VS_OFF_SCENE_XH       = 46
GFX_VS_OFF_SCENE_Y        = 47
GFX_VS_OFF_SCENE_WIDTHL   = 48
GFX_VS_OFF_SCENE_WIDTHH   = 49
GFX_VS_OFF_SCENE_HEIGHTL  = 50
GFX_VS_OFF_SCENE_HEIGHTH  = 51
GFX_VS_OFF_SCENE_WORKSPACE= 52
GFX_VS_OFF_SCENE_WORKADDRL= 53
GFX_VS_OFF_SCENE_WORKADDRM= 54
GFX_VS_OFF_SCENE_WORKADDRH= 55
GFX_VS_OFF_SCENE_WORKSTRL = 56
GFX_VS_OFF_SCENE_WORKSTRH = 57
; Maximum config bytes the wrapper will copy (covers the whole caller-facing
; block through SCENE_WORKSTRH; never touches the driver-internal OFFL/OFFH).
GFX_VS_CFG_MAX            = 58
;
; @out mapping: every vsprite op returns A=0 (ok) / A=1 (error) per its @out.
; The wrappers map A!=0 -> LIB_STATUS = LERR_VSPRITE_FAIL and A==0 -> LERR_OK,
; and additionally publish A into LIB_RESULT (byte 0; bytes 1-3 zeroed) so a
; caller can read the raw driver result without inspecting only STATUS. The
; void-return state op (USE_ORIGINAL/USE_ROTATED also return A=0) follow the
; same rule.
;
; WAIT MODEL (VERIFIED against vsprite.s + blitter.s): vsprite_gfx_blit,
; vsprite_gfx_save_bg, vsprite_gfx_restore_bg, vsprite_gfx_fill, vsprite_fill,
; vsprite_rotate and the scene helpers all reach the blitter via
; blitter_start_copy/blitter_fill -> blitter_wait, which self-spins on
; BLT_STATUS until not-BUSY (the emulator completes the blit synchronously on
; the BLT_CMD write). So none of these need a pumped bus. ONLY the two
; frame-pacing rotate variants (vsprite_gfx_rotate_blit / _keyed) additionally
; poll VGC_FRAME via vsprite_wait_frame, which only advances when the bus is
; pumped (AdvanceCycles) -> those two are flagged PUMPED below.
LERR_VSPRITE_FAIL = $84   ; vsprite driver returned A=1 (blitter error / bad geometry)

; Layouts (ARG0 = BYTES(cfg struct) for every op unless noted):
GFN_VS_BLIT              = $60 ; (cfg) -> vsprite_blit (rect copy, self-waits)
GFN_VS_BLIT_START        = $61 ; (cfg) -> vsprite_blit_start (issue, no wait)
GFN_VS_FILL              = $62 ; (cfg) -> vsprite_fill (rect fill, self-waits)
GFN_VS_USE_ORIGINAL      = $63 ; (cfg) -> vsprite_use_original (SRC* := ORIG*)
GFN_VS_USE_ROTATED       = $64 ; (cfg) -> vsprite_use_rotated  (SRC* := ROT*)
GFN_VS_ROTATE            = $65 ; (cfg) -> vsprite_rotate (rotate ORIG->ROT, self-waits)
GFN_VS_GFX_BLIT          = $66 ; (cfg) -> vsprite_gfx_blit (blit at X/Y, self-waits)
GFN_VS_GFX_BLIT_START    = $67 ; (cfg) -> vsprite_gfx_blit_start (issue, no wait)
GFN_VS_GFX_FILL          = $68 ; (cfg) -> vsprite_gfx_fill (fill at X/Y, self-waits)
GFN_VS_GFX_SAVE_BG       = $69 ; (cfg) -> vsprite_gfx_save_bg (gfx->BG, self-waits)
GFN_VS_GFX_RESTORE_BG    = $6A ; (cfg) -> vsprite_gfx_restore_bg (BG->gfx, self-waits)
GFN_VS_GFX_ROTATE_BLIT   = $6B ; (cfg) -> vsprite_gfx_rotate_blit (rotate+frame-wait+blit) PUMPED
GFN_VS_GFX_ROTATE_BLIT_KEYED  = $6C ; (cfg) -> vsprite_gfx_rotate_blit_keyed (colorkey) PUMPED
GFN_VS_GFX_ROTATE_BLIT_NOWAIT = $6D ; (cfg) -> vsprite_gfx_rotate_blit_nowait (no frame wait)
GFN_VS_SCENE_BEGIN       = $6E ; (cfg incl scene table) -> vsprite_scene_begin (restore old bgs)
GFN_VS_SCENE_COMMIT      = $6F ; (cfg incl scene table) -> vsprite_scene_commit (save+draw)
GFN_VS_SCENE_DRAW        = $70 ; (cfg incl scene table) -> vsprite_scene_draw (draw only)
GFN_VS_SCENE_COMMIT_ATOMIC = $71 ; (cfg incl scene+work) -> vsprite_scene_commit_atomic
;             $72-$7F reserved

; =====================================================================
; $80-$9F  msprite / meta-sprite  (driver: runtime/asm/msprite.s — batch 4b.8)
;   spawn/destroy/show/hide/pos/frame/anim/priority/transcolor/tick/commit
;
; A meta-sprite OBJECT owns a contiguous range of hardware sprites and is moved/
; animated as one unit. The driver keeps all object state in a module-owned BSS
; table (MSPRITE_OBJ_* arrays, MSPRITE_HW_OWNER); commit writes the owned hardware
; sprite attribute registers ($A040+) for every dirty visible object, so the
; effect is observable via the real VGC sprite state. NOTE: the driver assumes a
; ZEROED object table (no explicit init fn is in §2 scope); every runtime that
; uses msprite starts from a cold BSS band, which satisfies that.
;
; Arg-register conventions, VERIFIED against runtime/asm/msprite.inc + msprite.s
; (@in/@out/@requires):
;
;   SPAWN reads a VISUAL DESCRIPTOR via the MSPRITE_DESC_L/H pointer cell. The
;   descriptor is a caller-owned record: byte0 = part count (1..MSPRITE_MAX_PARTS),
;   byte1 = flags, then `part count` part records of 4 bytes (dx, dy, shape-base,
;   flags). ARG0 is a BYTES arg whose ptr16 (low word) is written straight into
;   MSPRITE_DESC_L/H (the descriptor stays resident in caller RAM; the len word is
;   ignored — the driver reads part-count from the descriptor itself). SPAWN
;   returns the new object handle in A ($FF on failure) plus a code in
;   MSPRITE_RESULT. The wrapper publishes the handle to LIB_RESULT byte 0 and maps
;   the failure to LIB_STATUS (handle $FF -> LERR_MSPRITE_FAIL, else LERR_OK).
;
;   Per-object ops take the object HANDLE in ARG0 -> A. Extra inputs:
;     POS:   x(s16)=ARG1 -> NVR0L/NVR0H, y=ARG2 -> Y register.
;     FRAME: frame#=ARG1 -> X register.
;     ANIM:  anim descriptor ptr16 = ARG1 BYTES low word -> MSPRITE_ANIM_L/H.
;     PRIORITY / TRANSCOLOR: value=ARG1 -> X register.
;     DESTROY/SHOW/HIDE/COMMIT_ONE: handle only.
;   Each returns a result code in A; the wrapper maps A!=OK -> LERR_MSPRITE_FAIL.
;
;   TICK and COMMIT are engine-wide (no args); both return A=0 (always OK).
;
LERR_MSPRITE_FAIL = $85   ; msprite driver returned a non-OK result / $FF handle

GFN_MS_SPAWN       = $80  ; (BYTES desc:ARG0) -> MSPRITE_DESC_L/H; msprite_spawn
                          ;   RESULT = object handle (A); $FF -> LERR_MSPRITE_FAIL
GFN_MS_DESTROY     = $81  ; (handle:ARG0)            -> A; msprite_destroy
GFN_MS_SHOW        = $82  ; (handle:ARG0)            -> A; msprite_show
GFN_MS_HIDE        = $83  ; (handle:ARG0)            -> A; msprite_hide
GFN_MS_POS         = $84  ; (handle:ARG0, x:ARG1 s16, y:ARG2) -> A, NVR0L/H, Y; msprite_set_pos
GFN_MS_FRAME       = $85  ; (handle:ARG0, frame:ARG1) -> A, X; msprite_set_frame
GFN_MS_ANIM        = $86  ; (handle:ARG0, BYTES anim:ARG1) -> A, MSPRITE_ANIM_L/H; msprite_set_anim
GFN_MS_PRIORITY    = $87  ; (handle:ARG0, pri:ARG1)  -> A, X; msprite_set_priority
GFN_MS_TRANSCOLOR  = $88  ; (handle:ARG0, color:ARG1) -> A, X; msprite_set_transcolor
GFN_MS_TICK        = $89  ; ()                       -> msprite_tick (advance anim timers)
GFN_MS_COMMIT      = $8A  ; ()                       -> msprite_commit (write all dirty -> hw)
GFN_MS_COMMIT_ONE  = $8B  ; (handle:ARG0)            -> A; msprite_commit_one
;             $8C-$9F reserved

; =====================================================================
; $A0-$AF  image / mem  (drivers: vgc.s mem-I/O + fio.s GSAVE/GLOAD + nvg.s)
;   MEMREAD MEMWRITE VPEEK VPOKE GSAVE GLOAD NVGload x4   (batch 4b.9)
;
; THREE driver families, three marshalling shapes, all VERIFIED against the
; sources:
;
;  1. VGC byte memory I/O (vgc.s vgc_mem_read / vgc_mem_write). Both issue a
;     VCMD_MEMREAD/MEMWRITE through VGC_CMD and self-wait via vgc_wait_cmd. The
;     ExecuteMemoryCommand register map (VirtualGraphicsController.cs) is:
;       VGC_P0 = space (a VGC_PLANE_* id), VGC_P1/P2 = 16-bit address,
;       VGC_P3 = value (write input / read OUTPUT), VGC_P4 = autoinc flags
;       (bit0). The wrapper clears VGC_P4 so each op touches exactly one byte.
;     MEMREAD/VPEEK are reporters: after the command, VGC_P3 holds the byte ->
;     LIB_RESULT (4-byte LE, high bytes zeroed) + STATUS=OK.
;     VPEEK/VPOKE are the gfx-plane forms: identical to MEMREAD/MEMWRITE with
;     the space fixed to VGC_PLANE_GFX, so ARG0 carries the gfx offset directly
;     (no space arg).
;
;  2. File save/load of a VGC memory space to/from a .gfx file (fio.s
;     fio_gsave / fio_gload -> FIO_CMD_GSAVE/GLOAD via fio_exec; self-waits on
;     FIO_STATUS, returns A=0 OK / A=1 error). The FileIoController DoGSave/
;     DoGLoad read FIO_GSPACE = space, FIO_GADDRL/H = 16-bit address, FIO_GLENL/H
;     = 16-bit length, and FIO_NAME/FIO_NAMELEN = filename. The wrapper marshals
;     the BYTES filename into FIO_NAME/FIO_NAMELEN exactly as GTEXT does, loads
;     the three graphics registers, and JSRs the driver. Result A!=0 ->
;     LERR_FILE_FAIL, A==0 -> OK; A is also published to LIB_RESULT byte 0.
;
;  3. NVG image load into the gfx plane (nvg.s nvg_load / nvg_load_at /
;     nvg_load_named / nvg_load_named_at). XPAGE reads the packed 4bpp NVG2
;     payload into XRAM, then the blitter unpacks it into the gfx plane. Every
;     NVGload wrapper takes ARG0 = BYTES filename; the _AT/_NAMED_AT variants
;     additionally take a 16-bit destination offset in ARG1. Result A!=0 ->
;     LERR_IMAGE_FAIL, A==0 -> OK; A is also published to LIB_RESULT byte 0.
GFX_MEMSPACE_GFX = $03      ; VGC_PLANE_GFX — VPEEK/VPOKE fixed space
LERR_FILE_FAIL   = $86      ; fio_gsave/gload returned A=1 (FIO error / bad geometry)
LERR_IMAGE_FAIL  = $87      ; nvg_load* returned A=1 (decode / missing file)

; Layouts (ARGn low word unless noted; BYTES args pack ptr16 low / len16 high):
GFN_MEMREAD          = $A0  ; (space:ARG0, addr:ARG1) reporter -> RESULT = byte; vgc_mem_read
GFN_MEMWRITE         = $A1  ; (space:ARG0, addr:ARG1, value:ARG2) -> vgc_mem_write
GFN_VPEEK            = $A2  ; (offset:ARG0) reporter -> RESULT = gfx byte; vgc_mem_read (space=gfx)
GFN_VPOKE            = $A3  ; (offset:ARG0, value:ARG1) -> vgc_mem_write (space=gfx)
GFN_GSAVE            = $A4  ; (space:ARG0, addr:ARG1, len:ARG2, BYTES name:ARG3) -> fio_gsave
GFN_GLOAD            = $A5  ; (space:ARG0, addr:ARG1, len:ARG2, BYTES name:ARG3) -> fio_gload
GFN_NVGLOAD          = $A6  ; (BYTES name:ARG0)                  -> nvg_load (offset 0)
GFN_NVGLOAD_AT       = $A7  ; (BYTES name:ARG0, dest:ARG1)       -> nvg_load_at
GFN_NVGLOAD_NAMED    = $A8  ; (BYTES name:ARG0)                  -> nvg_load_named (named-arg path)
GFN_NVGLOAD_NAMED_AT = $A9  ; (BYTES name:ARG0, dest:ARG1)       -> nvg_load_named_at
;             $AA-$AF reserved

; =====================================================================
; $B0-$BF  turtle — EXTRACTED. The whole turtle (render + command engine) was
;   lifted out of the GRAPHICS module into the dedicated TURTLE module
;   (MODULE_ID_TURTLE = $07). The fn ids (TUR_INIT/DRAW/ERASE/OP) and the TOP_*
;   op-codes now live in runtime/asm/libturtle.inc. This range is reserved here.
; =====================================================================

; =====================================================================
; $C0-$CF  anim  (driver: runtime/asm/anim.s — batch 4b.11) — superset
; extension beyond design §2.
;   init/start/stop/tick/tick_one/set_frame/load_xram_shapes/load_disk_shapes
;
; Track-based sprite animation. The driver keeps an 8-track table + a set of
; descriptor/target/shape-load pseudo-registers in its module-owned BSS block
; (ANIM_DESC_L/H, ANIM_TARGET, ANIM_TARGET_TYPE, ANIM_SHAPE, ANIM_COUNT,
; ANIM_XADDR*, ANIM_FILE*, ANIM_NAME*). The wrappers marshal the ARG cells into
; those BSS cells, JSR the driver entry, then map the result. anim pulls dma/fio/
; pager/msprite — all already co-assembled by earlier batches (dedup'd by the
; .ifndef guards), pager.s is the one new dependency (CODE only, ZP via NVR*
; aliases). The descriptor for ANIM.START stays RESIDENT in caller RAM: ARG0 is a
; BYTES arg whose ptr16 (low word) is written straight into ANIM_DESC_L/H.
;
; Arg-register conventions, VERIFIED against runtime/asm/anim.inc + anim.s
; (@in/@out/@requires):
;
;   START reads ANIM_DESC_L/H + ANIM_TARGET + ANIM_TARGET_TYPE and returns the
;   new track handle in A ($FF on failure). The wrapper publishes the handle to
;   LIB_RESULT byte 0 and maps $FF -> LERR_ANIM_FAIL, else LERR_OK
;   (finish_anim_start).
;
;   Per-track ops take the track HANDLE in A and return a result code in A
;   (0 = OK); the wrapper publishes A to LIB_RESULT byte 0 and maps A!=0 ->
;   LERR_ANIM_FAIL (finish_anim). SET_FRAME additionally takes the frame in X.
;
;   INIT/TICK always return A=0 -> OK (finish_anim too).
;
;   LOAD_XRAM_SHAPES / LOAD_DISK_SHAPES read the shape-load pseudo-registers and
;   return A=0/1; finish_anim maps A!=0 -> LERR_ANIM_FAIL.
;
; None of the anim ops issue a VGC *command* on the wrapper's behalf (the shape
; loaders drive DMA/pager which self-wait inside the driver), so the wrappers
; need no command wait.
LERR_ANIM_FAIL = $88   ; anim driver returned a non-OK result / $FF handle

; Layouts (ARGn low word unless noted; BYTES args pack ptr16 low / len16 high):
GFN_ANIM_INIT        = $C0  ; ()                                  -> anim_init (A=0)
GFN_ANIM_START       = $C1  ; (BYTES desc:ARG0, target:ARG1, type:ARG2)
                            ;   -> ANIM_DESC_L/H, ANIM_TARGET, ANIM_TARGET_TYPE; anim_start
                            ;   RESULT byte 0 = track handle (A); $FF -> LERR_ANIM_FAIL
GFN_ANIM_STOP        = $C2  ; (handle:ARG0)                       -> A; anim_stop
GFN_ANIM_TICK        = $C3  ; ()                                  -> anim_tick (A=0)
GFN_ANIM_TICK_ONE    = $C4  ; (handle:ARG0)                       -> A; anim_tick_one
GFN_ANIM_SET_FRAME   = $C5  ; (handle:ARG0, frame:ARG1)           -> A, X; anim_set_frame
GFN_ANIM_LOAD_XRAM   = $C6  ; (shape:ARG0, count:ARG1, xaddr24:ARG2)
                            ;   -> ANIM_SHAPE, ANIM_COUNT, ANIM_XADDRL/M/H; anim_load_xram_shapes
GFN_ANIM_LOAD_DISK   = $C7  ; (shape:ARG0, count:ARG1, file24:ARG2, BYTES name:ARG3)
                            ;   -> ANIM_SHAPE, ANIM_COUNT, ANIM_FILEL/M/H,
                            ;      ANIM_NAMEPTR_L/H + ANIM_NAMELEN; anim_load_disk_shapes
;             $C8-$CF reserved

; =====================================================================
; $D0-$DF  tween (driver: runtime/asm/tween.s — batch 4b.11) — superset
; extension beyond design §2.
;   begin / eval{,_linear,_ease_in,_ease_out,_ease_in_out} /
;          step{,_linear,_ease_in,_ease_out,_ease_in_out}
;
; Pure 16-bit easing math; no hardware. All state lives in the driver's
; module-owned TWEEN_* BSS cells: software writes START/END/DURATION/FRAME/MODE,
; evaluates, then reads VALUEL/H, DONE, PROGRESS, EASE. This is the register-
; shaped ABI documented in tween.inc.
;
; MARSHALLING (uniform across every tween op, VERIFIED against tween.s
; @requires): the wrapper loads the five input cells from the ARG cells before
; the driver runs:
;   ARG0 low word  -> TWEEN_STARTL/H   (start value, s16)
;   ARG1 low word  -> TWEEN_ENDL/H     (end value, s16)
;   ARG2 byte0     -> TWEEN_DURATION   (frames; 0 snaps to end)
;   ARG2 byte1     -> TWEEN_MODE       (0 linear / 1 in / 2 out / 3 in-out)
;   ARG3 byte0     -> TWEEN_FRAME      (current frame; step ops increment it)
; The fixed-curve eval/step variants force TWEEN_MODE themselves, so ARG2 byte1
; is a don't-care for those — but loading it uniformly is harmless.
;
; RESULT (reporter, VERIFIED against tween.s @out): after the driver runs the
; wrapper packs the readbacks into LIB_RESULT (4-byte LE):
;   byte 0 = TWEEN_VALUEL, byte 1 = TWEEN_VALUEH  (the 16-bit computed value),
;   byte 2 = TWEEN_DONE    (non-zero at end),
;   byte 3 = TWEEN_PROGRESS (normalized 0..255).
; STATUS is always LERR_OK (the math never fails). TWEEN_EASE remains readable
; via a direct cell peek for callers that need it; the 4-byte result carries the
; three fields the contract calls for.
GFN_TWEEN_BEGIN            = $D0  ; (start:ARG0, end:ARG1, duration:ARG2) -> tween_begin
GFN_TWEEN_EVAL             = $D1  ; (start,end,dur,mode:ARG0..2, frame:ARG3) -> tween_eval
GFN_TWEEN_EVAL_LINEAR      = $D2  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_eval_linear
GFN_TWEEN_EVAL_EASE_IN     = $D3  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_eval_ease_in
GFN_TWEEN_EVAL_EASE_OUT    = $D4  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_eval_ease_out
GFN_TWEEN_EVAL_EASE_IN_OUT = $D5  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_eval_ease_in_out
GFN_TWEEN_STEP             = $D6  ; (start,end,dur,mode:ARG0..2, frame:ARG3) -> tween_step
GFN_TWEEN_STEP_LINEAR      = $D7  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_step_linear
GFN_TWEEN_STEP_EASE_IN     = $D8  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_step_ease_in
GFN_TWEEN_STEP_EASE_OUT    = $D9  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_step_ease_out
GFN_TWEEN_STEP_EASE_IN_OUT = $DA  ; (start,end,dur:ARG0..2, frame:ARG3) -> tween_step_ease_in_out
;             $DB-$DF reserved

.endif
