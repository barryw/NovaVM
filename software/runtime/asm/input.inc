; =====================================================================
;  Nova NDK — input.inc
;
;  Single source of the $BA9C-$BAA1 board-input controller register map
;  and bit masks (joystick / buttons / DIP switches) for any runtime
;  (BASIC, NovaLogo, NDK apps). Exists in both the emulator
;  (CompositeBusDevice.cs) and the FPGA (top.sv); mirrors VgcConstants.cs.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; HOT PATH: read these MMIO registers DIRECTLY (LDA BOARD_BUTTONS). Do NOT wrap
; in a paged lib_call — input is read per-frame and a page-in per read would
; thrash the resident module bank every frame.

.ifndef INPUT_INC_INCLUDED
INPUT_INC_INCLUDED = 1

; --- controller registers ---
BOARD_BUTTONS     = $BA9C   ; joystick directions + fire buttons (read-only state)
BOARD_SWITCHES    = $BA9D   ; DIP switches, low nibble (read-only state)
BOARD_IRQ_ENABLE  = $BA9E   ; bit0=button-change IRQ, bit1=switch-change IRQ
BOARD_IRQ_STATUS  = $BA9F   ; W1C: bit0=buttons changed, bit1=switches changed
BOARD_BTN_CHANGES = $BAA0   ; W1C mask of changed button bits (edge detect)
BOARD_SW_CHANGES  = $BAA1   ; W1C mask of changed switch bits (edge detect)

; --- BOARD_BUTTONS bit masks ---
JOY_POWER = $01   ; PWR button (reset) — excluded from JOY_MASK
JOY_FIRE1 = $02
JOY_FIRE2 = $04
JOY_UP    = $08
JOY_DOWN  = $10
JOY_LEFT  = $20
JOY_RIGHT = $40
JOY_MASK  = $7E   ; directions + fires (drops PWR bit0)

; --- BOARD_SWITCHES bit masks ---
SW1 = $01
SW2 = $02
SW3 = $04
SW4 = $08
SW_MASK = $0F

; --- IRQ source masks (BOARD_IRQ_ENABLE / BOARD_IRQ_STATUS) ---
BOARD_IRQ_BTN = $01
BOARD_IRQ_SW  = $02

.endif
