; libabi.inc — paged shared-library ABI (mailbox, module header, page-in MMIO).
; Included by every runtime foundation, every module, and the loader.
; Design: docs/plans/2026-06-03-paged-library-loader-3b-design.md

.ifndef LIBABI_INC_INCLUDED
LIBABI_INC_INCLUDED = 1

; --- Page-in (PGD) MMIO — shipped RTL ---
PGD_CMD          = $BA76
PGD_STATUS       = $BA77
PGD_SRCL         = $BA78
PGD_SRCM         = $BA79
PGD_SRCH         = $BA7A
PGD_WORDSL       = $BA7B
PGD_WORDSH       = $BA7C
PGD_START        = $01            ; write to PGD_CMD to begin
PGD_ST_BUSY      = $01            ; PGD_STATUS bit0
PGD_ST_DONE      = $02            ; PGD_STATUS bit1 (done/ok)

; --- Canonical mailbox (fixed low-RAM, carved $0300-$031F in every runtime) ---
LIB_MBOX         = $0300
LIB_MOD_ID       = LIB_MBOX+0     ; $0300  target module id
LIB_FN_ID        = LIB_MBOX+1     ; $0301  function id within module
LIB_STATUS       = LIB_MBOX+2     ; $0302  0=OK else error code
LIB_ARG0         = LIB_MBOX+3     ; $0303  32-bit LE cell
LIB_ARG1         = LIB_MBOX+7     ; $0307
LIB_ARG2         = LIB_MBOX+11    ; $030B
LIB_ARG3         = LIB_MBOX+15    ; $030F
LIB_RESULT       = LIB_MBOX+19    ; $0313  32-bit LE cell
LIB_HOME_BANK    = LIB_MBOX+23    ; $0317  runtime's REG_ROMSWAP home value (set at boot)
LIB_RESIDENT     = LIB_MBOX+24    ; $0318  currently-mapped module id ($00=none)
LIB_SCRATCH      = LIB_MBOX+25    ; $0319-$031F  7 bytes loader/module scratch
LIB_MBOX_END     = LIB_MBOX+32    ; $0320 (exclusive)

; --- Resident loader band (fixed low-RAM, carved $0320-$041F in every runtime) ---
; lib_call (libcall.s, not PIC) lives here. One canonical libcall.bin built at
; ORG $0320 (runtime/asm/libcall_resident.cfg); each runtime reserves the band
; and POKEs/JSRs it. 256 B leaves headroom over the 157 B loader. The $9C00
; build (tests/asm/libcall_blob.cfg) is isolated harness scaffolding, not this.
LIB_LOADER_BAND     = LIB_MBOX_END    ; $0320  resident loader entry (== lib_call)
LIB_LOADER_BAND_END = $0420           ; (exclusive)

; --- Module-BSS band (fixed low-RAM, carved $0420-$08FF in every runtime) ---
; Cross-runtime working storage for a paged-in module (e.g. GRAPHICS MODBSS:
; vsprite/msprite state 320 B + 3x256 B turtle-render buffers added in 4c.2-2).
; Sits between the loader band and runtime user RAM (Logo BSS / EhBASIC Ram_base,
; both moved to $0900). Single source of truth for the band; modules/graphics/
; graphics.cfg MODBSS places the GRAPHICS module's state here. 1248 B.
MODULE_BSS_BAND     = LIB_LOADER_BAND_END  ; $0420  module-private low-RAM working store
MODULE_BSS_BAND_END = $0900                ; (exclusive)

; --- Module-private ZP pointer for (zp),Y BYTES deref (cross-runtime free) ---
LIB_ZP           = $14            ; $14-$17 reserved for module pointer pairs

; --- Value type tags (compile-time, declared in fn signatures; not on the wire) ---
LT_INT           = $00            ; 32-bit signed (also 16.16 fixed)
LT_BYTES         = $01            ; ARG cell = ptr16,len16
LT_BOOL          = $02

; --- Status / error codes ---
LERR_OK          = $00
LERR_BAD_MAGIC   = $80
LERR_BAD_MODULE  = $81
LERR_BAD_VER     = $82
LERR_NO_FN       = $83

; --- Module header (image is ORG $C000) ---
MOD_ENTRY        = $C000          ; JMP dispatch (trampoline JSRs here)
MOD_MAGIC        = $C003          ; 2 bytes
MOD_MOD_ID       = $C005
MOD_VERSION      = $C006
MOD_FNCOUNT      = $C007
; reserved $C008-$C009
MOD_MAGIC0       = $4E            ; 'N'
MOD_MAGIC1       = $4C            ; 'L'
LIB_ABI_VERSION  = $01

; --- Module ids ---
MODULE_ID_NONE     = $00
MODULE_ID_GRAPHICS = $01
MODULE_ID_SOUND    = $02
MODULE_ID_SYSTEM   = $03
MODULE_ID_TEST     = $7F

; --- XRAM library shelf (option A: fixed, unmanaged high 256KB) ---
SHELF_BASE_L     = $00
SHELF_BASE_M     = $00
SHELF_BASE_H     = $06            ; $060000
SHELF_SLOT_WORDS = $2000          ; 8192 words = 16KB per slot

; --- Host-extension re-page (4c.0c: coexistence of legacy ext + paged modules) ---
; LIB_RESIDENT marker meaning "the active runtime's OWN extension is in bank 1"
; (distinct from every module id $01-$7F and from $00=none). A legacy ext command
; checks LIB_RESIDENT; if not $FF it re-pages the host ext from XRAM before calling.
LIB_RESIDENT_HOSTEXT = $FF
; XRAM address $07C000 where the active runtime's extension is staged for re-page.
; NOT a modtab module slot — disjoint from module slots 0-2 ($060000/$064000/$068000)
; and within the 512KB XRAM. NovaHost must stage the active runtime's extensionRom
; here on HW (firmware follow-on); this task does the emulator side.
HOST_EXT_XRAM_L  = $00
HOST_EXT_XRAM_M  = $C0
HOST_EXT_XRAM_H  = $07            ; $07C000

.endif ; LIBABI_INC_INCLUDED
