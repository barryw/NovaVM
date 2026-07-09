; =====================================================================
;  Nova NDK — overlay.inc
;
;  Nova fixed-address overlay manager interface. Overlays are files with a
;  32-byte NOVO header plus a payload linked for an exact CPU RAM slot; the
;  manager validates the header, streams the payload in through PAGER.XPAGE,
;  and calls optional entry vectors through a stable ABI.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef OVERLAY_INC_INCLUDED
OVERLAY_INC_INCLUDED = 1

.include "pager.inc"

OVL_OK                 = $00
OVL_ERR                = $01
OVL_ERR_IO             = $02
OVL_ERR_MAGIC          = $03
OVL_ERR_VERSION        = $04
OVL_ERR_LOAD_ADDR      = $05
OVL_ERR_SIZE           = $06
OVL_ERR_NOT_LOADED     = $07
OVL_ERR_NO_ENTRY       = $08
OVL_ERR_ENTRY          = $09

OVL_FORMAT_VERSION     = $01
OVL_ABI_MAJOR          = $01
OVL_ABI_MINOR          = $00
OVL_HEADER_SIZE        = $20

OVL_HDR_MAGIC0         = $00
OVL_HDR_MAGIC1         = $01
OVL_HDR_MAGIC2         = $02
OVL_HDR_MAGIC3         = $03
OVL_HDR_FORMAT         = $04
OVL_HDR_ABI_MAJOR      = $05
OVL_HDR_ABI_MINOR      = $06
OVL_HDR_FLAGS          = $07
OVL_HDR_LOADL          = $08
OVL_HDR_LOADH          = $09
OVL_HDR_SIZEL          = $0A
OVL_HDR_SIZEH          = $0B
OVL_HDR_BSSL           = $0C
OVL_HDR_BSSH           = $0D
OVL_HDR_INITL          = $0E
OVL_HDR_INITH          = $0F
OVL_HDR_MAINL          = $10
OVL_HDR_MAINH          = $11
OVL_HDR_TICKL          = $12
OVL_HDR_TICKH          = $13
OVL_HDR_UNLOADL        = $14
OVL_HDR_UNLOADH        = $15
OVL_HDR_MODULE_IDL     = $16
OVL_HDR_MODULE_IDH     = $17
OVL_HDR_VERSIONL       = $18
OVL_HDR_VERSIONH       = $19
OVL_HDR_RESERVED0      = $1A
OVL_HDR_RESERVED1      = $1B
OVL_HDR_RESERVED2      = $1C
OVL_HDR_RESERVED3      = $1D
OVL_HDR_CHECKL         = $1E
OVL_HDR_CHECKH         = $1F

OVL_MAGIC0             = 'N'
OVL_MAGIC1             = 'O'
OVL_MAGIC2             = 'V'
OVL_MAGIC3             = 'O'

.global OVL_NAMEPTR_L
.global OVL_NAMEPTR_H
.global OVL_NAMELEN
.global OVL_LOADL
.global OVL_LOADH
.global OVL_MAXLENL
.global OVL_MAXLENH
.global OVL_RESULT
.global OVL_LOADED
.global OVL_ACTIVE_LOADL
.global OVL_ACTIVE_LOADH
.global OVL_ACTIVE_SIZEL
.global OVL_ACTIVE_SIZEH
.global OVL_BSSL
.global OVL_BSSH
.global OVL_ENTRY_INITL
.global OVL_ENTRY_INITH
.global OVL_ENTRY_MAINL
.global OVL_ENTRY_MAINH
.global OVL_ENTRY_TICKL
.global OVL_ENTRY_TICKH
.global OVL_ENTRY_UNLOADL
.global OVL_ENTRY_UNLOADH
.global OVL_MODULE_IDL
.global OVL_MODULE_IDH
.global OVL_MODULE_VERSIONL
.global OVL_MODULE_VERSIONH

.global overlay_ok
.global overlay_set_error
.global overlay_clear_active
.global overlay_load_fixed
.global overlay_unload
.global overlay_call_init
.global overlay_call_main
.global overlay_call_tick

.endif
