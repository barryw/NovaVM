---
title: "Memory Map"
type: guide
category: Reference
---

## Address Space Overview

The e6502 virtual computer presents a flat 64 KB address space to the 6502 CPU.
Coprocessor regions (VGC, NIC, FIO, XMC, DMA, Blitter) respond to reads and writes within
their assigned windows; all remaining space is RAM except the upper 16 KB
(C000--FFFF) which is write-protected ROM.

| **Address Range** | **Size** | **Region** |
| --- | --- | --- |
| 0000--00FF | 256 B | Zero Page |
| 0100--01FF | 256 B | CPU Stack |
| 0200--027F | 128 B | System Vectors (IRQ/NMI handlers, BASIC vectors) |
| 0280--9FFF | 39,680 B | BASIC Program RAM |
| A000--A01F | 32 B | Virtual Graphics Controller (VGC) registers and command interface |
| A040--A0BF | 128 B | Sprite Registers (16 sprites x 8 bytes) |
| A100--A13F | 64 B | Network Interface Controller (NIC) registers |
| AA00--B1CF | 2,000 B | Character RAM (80x25 text cells) |
| B1D0--B99F | 2,000 B | Color RAM (80x25 text cells) |
| B9A0--B9EF | 80 B | File I/O Controller (FIO) registers |
| BA00--BA3F | 64 B | Expansion Memory Controller (XMC) registers |
| BA40--BA4F | 16 B | Timer Controller registers |
| BA50--BA56 | 7 B | Music Status and Voice Note Readback (6 voices) |
| BA60--BA7F | 32 B | DMA Controller registers |
| BA80--BA9F | 32 B | Blitter Controller registers |
| BC00--BFFF | 1,024 B | XMC Memory Windows (4 x 256-byte mapped pages) |
| C000--FFFF | 16,384 B | ROM (NovaBASIC interpreter) |
| D400--D41C | 29 B | SID chip registers (inside ROM range; writes intercepted) |
| D420--D43C | 29 B | SID chip 2 registers (inside ROM range; writes intercepted) |
| D500--D51C | 29 B | SID chip 2 mirror (transparently routes to $D420) |

::: note
The address range A020--A03F and A0C0--A0FF and A140--A9FF are not claimed by any
coprocessor and fall through to the underlying flat RAM.
The range BAA0--BBFF is similarly unallocated RAM.
SID registers at D400--D43C occupy space within the ROM address range
but are intercepted on write by the SID chip emulators.
:::

## VGC Register Map

The Virtual Graphics Controller occupies A000--A01F.
Registers A000--A00F are the core status and display registers.
Registers A010--A01E are the command register and its 14 parameter slots;
writing to $A010 both stores the command byte and triggers immediate execution.
Register $A01F is the IRQ control register.

### Core Registers (A000--A00F)

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $A000 | RegMode | R/W | Display mode: 0=text only, 1=graphics over text, 2=text over graphics, 3=graphics and sprites only (no text). |
| $A001 | RegBgCol | R/W | Background color index (0--15). |
| $A002 | RegFgCol | R/W | Default foreground color index (0--15); reset value is 1 (white). |
| $A003 | RegCursorX | R/W | Text cursor column (0--79). |
| $A004 | RegCursorY | R/W | Text cursor row (0--24). |
| $A005 | RegScrollX | R/W | Horizontal scroll offset (used by copper raster effects). |
| $A006 | RegScrollY | R/W | Vertical scroll offset (used by copper raster effects). |
| $A007 | RegBank | R/W | Reserved. |
| $A008 | RegStatus | RO | Frame counter; incremented each video frame. Writes are ignored. |
| $A009 | RegSpriteCount | RO | Count of currently enabled sprites (0--16). Writes are ignored. |
| $A00A | RegCursorEnable | R/W | Non-zero enables the cursor blink. |
| $A00B | RegColSt | RO | Sprite-to-sprite collision bitmask; reading clears the register. |
| $A00C | RegColBg | RO | Sprite-to-background collision bitmask; reading clears the register. |
| $A00D | RegBorder | R/W | Border color index (0--15). |
| $A00E | RegCharOut | R/W | Character output port; writing outputs a character to the text screen. |
| $A00F | RegCharIn | R/W | Character input port; reading dequeues the next keypress byte. |

### IRQ Control ($A01F)

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $A01F | RegIrqCtrl | R/W | Bit 0: raster IRQ enable. When set, the VGC fires an IRQ once per video frame. |

### Command Register and Parameters (A010--A01E)

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $A010 | RegCmd | R/W | Command byte; writing triggers immediate command execution. |
| $A011 | RegP0 | R/W | Parameter 0. |
| $A012 | RegP1 | R/W | Parameter 1. |
| $A013 | RegP2 | R/W | Parameter 2. |
| $A014 | RegP3 | R/W | Parameter 3. |
| $A015 | RegP4 | R/W | Parameter 4. |
| $A016 | RegP5 | R/W | Parameter 5. |
| $A017 | RegP6 | R/W | Parameter 6. |
| $A018 | RegP7 | R/W | Parameter 7. |
| $A019 | RegP8 | R/W | Parameter 8. |
| $A01A | RegP9 | R/W | Parameter 9. |
| $A01B | RegP10 | R/W | Parameter 10. |
| $A01C | RegP11 | R/W | Parameter 11. |
| $A01D | RegP12 | R/W | Parameter 12. |
| $A01E | RegP13 | R/W | Parameter 13. |

Multi-byte parameters (coordinates, sprite positions) are packed
little-endian across consecutive parameter registers.
For example, a 16-bit x-coordinate uses P0 (low byte) and P1 (high byte).

## VGC Command Codes

All commands are invoked by writing the command byte to `RegCmd` ($A010).
Parameters must be loaded into `RegP0`--`RegP13` before the write.

### Graphics Commands (01--09)

| **Code** | **Name** | **Parameters and behavior** |
| --- | --- | --- |
| $01 | CmdPlot | P0/P1 = x (16-bit), P2/P3 = y (16-bit). Set pixel to current draw color. |
| $02 | CmdUnplot | P0/P1 = x (16-bit), P2/P3 = y (16-bit). Clear pixel to 0 (transparent). |
| $03 | CmdLine | P0/P1 = x0, P2/P3 = y0, P4/P5 = x1, P6/P7 = y1. Draw Bresenham line. |
| $04 | CmdCircle | P0/P1 = cx, P2/P3 = cy, P4/P5 = radius. Draw circle outline. |
| $05 | CmdRect | P0/P1 = x0, P2/P3 = y0, P4/P5 = x1, P6/P7 = y1. Draw rectangle outline. |
| $06 | CmdFill | P0/P1 = x0, P2/P3 = y0, P4/P5 = x1, P6/P7 = y1. Draw filled rectangle. |
| $07 | CmdGcls | No parameters. Clear entire graphics bitmap to 0. |
| $08 | CmdGcolor | P0 low nibble = color index (0--15). Set current draw color. |
| $09 | CmdPaint | P0/P1 = x (16-bit), P2/P3 = y (16-bit). Flood-fill from seed point. |

### Sprite Commands (10--18)

| **Code** | **Name** | **Parameters and behavior** |
| --- | --- | --- |
| $10 | CmdSprDef | P0 = sprite (0--15), P1 = x pixel (0--15), P2 = y pixel (0--15), P3 = color nibble. Set one pixel in sprite shape. |
| $11 | CmdSprRow | P0 = sprite (0--15), P1 = row (0--15), P2--P9 = 8 data bytes (two 4-bit pixels per byte). Define one sprite row. |
| $12 | CmdSprClr | P0 = sprite (0--15). Clear all 128 bytes of sprite shape data to 0. |
| $13 | CmdSprCopy | P0 = source sprite (0--15), P1 = destination sprite (0--15). Copy shape data. |
| $14 | CmdSprPos | P0 = sprite (0--15), P1/P2 = x (16-bit), P3/P4 = y (16-bit). Set screen position. |
| $15 | CmdSprEna | P0 = sprite (0--15). Enable sprite; increments `RegSpriteCount`. |
| $16 | CmdSprDis | P0 = sprite (0--15). Disable sprite; decrements `RegSpriteCount`. |
| $17 | CmdSprFlip | P0 = sprite (0--15), P1 = flags (0=none, 1=horizontal, 2=vertical, 3=both). |
| $18 | CmdSprPri | P0 = sprite (0--15), P1 = priority (0=behind all, 1=between text/gfx, 2=in front). |

::: tip
Sprite shape data is stored host-side and is not 6502-addressable.
All sprite shape manipulation must go through the command register interface.
:::

### Memory I/O Commands (19--1A)

| **Code** | **Name** | **Parameters and behavior** |
| --- | --- | --- |
| $19 | CmdMemRead | P0 = memory space (0--3), P1/P2 = address (16-bit), P4 bit 0 = auto-increment. Read byte from VGC memory; result in P3. |
| $1A | CmdMemWrite | P0 = memory space (0--3), P1/P2 = address (16-bit), P3 = data byte, P4 bit 0 = auto-increment. Write byte to VGC memory. |

Memory spaces: 0=character RAM (2000 B), 1=color RAM (2000 B),
2=graphics bitmap (64000 B), 3=sprite shape RAM (32768 B; 256 slots).
Auto-increment advances the address after each read or write.

### Copper Commands (1B--1E, 20--22)

The copper triggers register writes at specific raster positions each frame.
The VGC stores 128 independent copper lists (0--127), each holding up to 256
events. All copper state changes take effect at vblank.

| **Code** | **Name** | **Parameters and behavior** |
| --- | --- | --- |
| $1B | CmdCopperAdd | P0/P1 = X (16-bit), P2 = Y, P3/P4 = register (0--15, A000--A00F, or A040--A0BF for sprite registers), P5 = value. Adds to the target list. Replaces existing event at same position/register. Max 256 events per list. |
| $1C | CmdCopperClear | No parameters. Remove all events from the target list. |
| $1D | CmdCopperEnable | No parameters. Start executing the active copper list each frame. |
| $1E | CmdCopperDisable | No parameters. Stop executing copper. |
| $20 | CmdCopperList | P0 = list index (0--127). Set target list for ADD/CLEAR. |
| $21 | CmdCopperUse | P0 = list index (0--127). Set pending active list (swaps at next vblank). |
| $22 | CmdCopperListEnd | No parameters. Reset target list to the currently active list. |

### System Reset Command ($1F)

| **Code** | **Name** | **Parameters and behavior** |
| --- | --- | --- |
| $1F | CmdSysReset | No parameters. Resets the VGC, stops both SID chips, stops the music engine, and resets the NIC. This is the command invoked by the BASIC `RESET` statement. |

Copper-writable registers: RegMode (A000), RegBgCol (A001),
RegScrollX (A005), RegScrollY (A006), and sprite registers
A040--A0BF (all eight fields, including TransColor at offset +7).

## SID Chip Registers

The SID chip occupies D400--D41C within the ROM address range. Writes
to these addresses are intercepted by the SID emulator; reads return the
underlying ROM byte. The register layout matches the original MOS 6581.

### Per-Voice Registers (3 voices, 7 bytes each)

| **Offset** | **Description** |
| --- | --- |
| +0 | Frequency low byte. |
| +1 | Frequency high byte (16-bit SID frequency units). |
| +2 | Pulse width low byte. |
| +3 | Pulse width high byte (12-bit, bits 0--11 only). |
| +4 | Control register: bit 0=gate, bit 4=triangle, bit 5=sawtooth, bit 6=pulse, bit 7=noise. |
| +5 | Attack (bits 7--4) / Decay (bits 3--0). |
| +6 | Sustain (bits 7--4) / Release (bits 3--0). |

Voice 0: D400--D406. Voice 1: D407--D40D. Voice 2: D40E--D414.

### Filter and Volume Registers

| **Address** | **Description** |
| --- | --- |
| $D415 | Filter cutoff low (bits 0--2). |
| $D416 | Filter cutoff high (bits 0--7). |
| $D417 | Resonance (bits 7--4) / Filter route (bits 3--0, one bit per voice + external). |
| $D418 | Volume (bits 3--0) / Filter mode (bit 4=LP, bit 5=BP, bit 6=HP). |

### Second SID Chip (D420--D43C)

A second SID chip is mapped at D420--D43C with an identical register
layout offset by 20 from the primary. A legacy mirror atD500--$D51C
routes transparently to $D420.

Voice mapping: voices 1--3 use SID 1 ($D400), voices 4--6 use SID 2
($D420). The music engine addresses all six voices via
`MUSIC voice,"mml"` where *voice* is 1--6.

::: note
The BASIC commands `INSTRUMENT`, `SOUND`, and `MUSIC` manage
SID registers automatically. Direct writes to D400+ orD420+ are for
advanced use only and may conflict with the music engine.
:::

## Timer Controller and Music Status

### Timer Controller (BA40--BA4F)

The timer controller provides periodic interrupt generation. It is used
internally by `SIDPLAY` but can also be configured from assembly for
custom timing.

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $BA40 | TimerCtrl | R/W | Control register: bit 0 = enable. |
| $BA41 | TimerStatus | RO | Status: bit 0 = IRQ pending (reading clears). |
| $BA42 | TimerDivL | R/W | Divisor low byte. |
| $BA43 | TimerDivH | R/W | Divisor high byte. |

The timer fires an IRQ every *divisor* video frames (at 60 Hz).
A divisor of 1 fires every frame; a divisor of 60 fires once per second.
Reading `TimerStatus` clears the pending IRQ flag.

### Music Status (BA50--BA56)

| **Address** | **Access** | **Description** |
| --- | --- | --- |
| $BA50 | RO | Status flags: bit 0 = SFX playing, bit 1 = music playing. |
| $BA51 | RO | Voice 1 current MIDI note (0 = silent). SID 1, voice 1. |
| $BA52 | RO | Voice 2 current MIDI note (0 = silent). SID 1, voice 2. |
| $BA53 | RO | Voice 3 current MIDI note (0 = silent). SID 1, voice 3. |
| $BA54 | RO | Voice 4 current MIDI note (0 = silent). SID 2, voice 1. |
| $BA55 | RO | Voice 5 current MIDI note (0 = silent). SID 2, voice 2. |
| $BA56 | RO | Voice 6 current MIDI note (0 = silent). SID 2, voice 3. |

These registers are read by the `PLAYING` and `MNOTE()` functions.

## DMA Controller Register Map

The DMA Controller occupies BA60--BA7F and provides bulk memory transfers
across six unified memory spaces. Writing $01 to `DmaCmd` triggers
the configured operation; poll `DmaStatus` for completion.

### DMA Registers

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $BA60 | DmaCmd | W | Command byte; write $01 to start transfer. |
| $BA61 | DmaStatus | RO | Status: 00=idle,01=busy, 02=ok,03=error. |
| $BA62 | DmaErrCode | RO | Error detail code (see Appendix \refchap:limits). |
| $BA63 | DmaSrcSpace | R/W | Source memory space (0--5). |
| $BA64 | DmaDstSpace | R/W | Destination memory space (0--5). |
| $BA65 | DmaSrcL | R/W | Source address low byte. |
| $BA66 | DmaSrcM | R/W | Source address mid byte. |
| $BA67 | DmaSrcH | R/W | Source address high byte. |
| $BA68 | DmaDstL | R/W | Destination address low byte. |
| $BA69 | DmaDstM | R/W | Destination address mid byte. |
| $BA6A | DmaDstH | R/W | Destination address high byte. |
| $BA6B | DmaLenL | R/W | Transfer length low byte. |
| $BA6C | DmaLenM | R/W | Transfer length mid byte. |
| $BA6D | DmaLenH | R/W | Transfer length high byte. |
| $BA6E | DmaMode | R/W | Mode flags: bit 0 = fill mode. |
| $BA6F | DmaFillValue | R/W | Fill byte (used when bit 0 of DmaMode is set). |
| $BA70 | DmaCountL | RO | Bytes transferred, low byte. |
| $BA71 | DmaCountM | RO | Bytes transferred, mid byte. |
| $BA72 | DmaCountH | RO | Bytes transferred, high byte. |

### DMA Memory Spaces

| **ID** | **Space** |
| --- | --- |
| 0 | CPU RAM (64 KB) |
| 1 | VGC Character RAM (2,000 bytes) |
| 2 | VGC Color RAM (2,000 bytes) |
| 3 | VGC Graphics Bitmap (64,000 bytes) |
| 4 | VGC Sprite Shapes (32,768 bytes; 256 slots) |
| 5 | Expansion RAM (uses current XBANK) |

## Blitter Controller Register Map

The Blitter Controller occupies BA80--BA9F and provides 2D rectangular
copy and fill operations with configurable row stride. It shares the same
six memory spaces as the DMA controller. Writing $01 to `BltCmd`
triggers the configured operation.

### Blitter Registers

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $BA80 | BltCmd | W | Command byte; write $01 to start blit. |
| $BA81 | BltStatus | RO | Status: 00=idle,01=busy, 02=ok,03=error. |
| $BA82 | BltErrCode | RO | Error detail code (see Appendix \refchap:limits). |
| $BA83 | BltSrcSpace | R/W | Source memory space (0--5). |
| $BA84 | BltDstSpace | R/W | Destination memory space (0--5). |
| $BA85 | BltSrcL | R/W | Source base address low byte. |
| $BA86 | BltSrcM | R/W | Source base address mid byte. |
| $BA87 | BltSrcH | R/W | Source base address high byte. |
| $BA88 | BltDstL | R/W | Destination base address low byte. |
| $BA89 | BltDstM | R/W | Destination base address mid byte. |
| $BA8A | BltDstH | R/W | Destination base address high byte. |
| $BA8B | BltWidthL | R/W | Rectangle width low byte. |
| $BA8C | BltWidthH | R/W | Rectangle width high byte. |
| $BA8D | BltHeightL | R/W | Rectangle height low byte. |
| $BA8E | BltHeightH | R/W | Rectangle height high byte. |
| $BA8F | BltSrcStrideL | R/W | Source row stride low byte. |
| $BA90 | BltSrcStrideH | R/W | Source row stride high byte. |
| $BA91 | BltDstStrideL | R/W | Destination row stride low byte. |
| $BA92 | BltDstStrideH | R/W | Destination row stride high byte. |
| $BA93 | BltMode | R/W | Mode flags: bit 0 = fill, bit 1 = color-key. |
| $BA94 | BltFillValue | R/W | Fill byte (when bit 0 of BltMode is set). |
| $BA95 | BltColorKey | R/W | Transparent color (when bit 1 of BltMode is set). |
| $BA96 | BltCountL | RO | Bytes written, low byte. |
| $BA97 | BltCountM | RO | Bytes written, mid byte. |
| $BA98 | BltCountH | RO | Bytes written, high byte. |

::: note
When color-key mode is active (bit 1 of `BltMode`), source pixels
matching `BltColorKey` are skipped and the destination byte is left
unchanged. This enables transparent sprite and tile overlays.
:::

## NIC Register Map

The Network Interface Controller occupies A100--A13F and provides
message-oriented TCP networking over four connection slots. Writing a
command byte to `NicCmd` triggers the operation on the slot
selected by `NicSlot`.

### NIC Registers

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $A100 | NicCmd | W | Command byte; writing triggers execution. |
| $A101 | NicStatus | RO | Global status flags. |
| $A102 | NicSlot | R/W | Active slot ID (0--3). |
| $A103 | NicIrqCtrl | R/W | IRQ enable mask (one bit per slot). |
| $A104 | NicIrqStatus | RO | IRQ pending flags (reading clears). |
| $A108 | NicRemotePortL | R/W | Remote port low byte (for connect). |
| $A109 | NicRemotePortH | R/W | Remote port high byte. |
| $A10A | NicLocalPortL | R/W | Local port low byte (for listen). |
| $A10B | NicLocalPortH | R/W | Local port high byte. |
| $A110 | NicDmaAddrL | R/W | DMA RAM address low byte. |
| $A111 | NicDmaAddrH | R/W | DMA RAM address high byte. |
| $A112 | NicDmaLen | R/W | DMA length (1--255; $00 = 256). |
| $A113 | NicMsgLen | RO | Last received message length. |
| $A118 | NicSlotStatus0 | RO | Slot 0 status flags. |
| $A119 | NicSlotStatus1 | RO | Slot 1 status flags. |
| $A11A | NicSlotStatus2 | RO | Slot 2 status flags. |
| $A11B | NicSlotStatus3 | RO | Slot 3 status flags. |
| A120--A13F | NicNameBuf | R/W | Hostname buffer (32 bytes ASCII). |

### NIC Command Codes

| **Code** | **Name** | **Behavior** |
| --- | --- | --- |
| $01 | NicCmdConnect | Connect slot to remote host (hostname in NicNameBuf, port in NicRemotePortL/H). |
| $02 | NicCmdDisconnect | Close active slot connection. |
| $03 | NicCmdSend | Send message from CPU RAM (NicDmaAddrL/H, NicDmaLen). |
| $04 | NicCmdRecv | Receive next queued message into CPU RAM. |
| $05 | NicCmdListen | Start TCP server on local port (NicLocalPortL/H). |
| $06 | NicCmdAccept | Accept pending connection on listening slot. |

### NIC Slot Status Bits

| **Bit** | **Name** | **Meaning** |
| --- | --- | --- |
| 0 | Connected | Slot has an active TCP connection. |
| 1 | DataReady | Receive queue has at least one message (or pending accept). |
| 2 | SendReady | Slot is ready to accept a send operation. |
| 3 | Error | Connection failed, I/O error, or queue overflow. |
| 4 | RemoteClosed | Remote peer closed the connection. |

### NIC Global Status Bits ($A101)

| **Bit** | **Name** | **Meaning** |
| --- | --- | --- |
| 0 | Ready | NIC is initialized and ready. |
| 1 | AnyData | At least one slot has queued data. |
| 7 | AnyError | At least one slot has an error flag set. |

## FIO Register Map

The File I/O Controller occupies B9A0--B9EF.
Writing to $B9A0 (`FioCmd`) triggers the operation.
The caller polls $B9A1 (`FioStatus`) for completion.

### FIO Registers

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $B9A0 | FioCmd | R/W | Command byte; writing triggers the operation. |
| $B9A1 | FioStatus | RO | Result status: 0=idle, 2=ok, 3=error. |
| $B9A2 | FioErrCode | RO | Error detail code (see below). |
| $B9A3 | FioNameLen | R/W | Filename length in bytes (1--63). |
| $B9A4 | FioSrcL | R/W | Source/destination address, low byte. |
| $B9A5 | FioSrcH | R/W | Source/destination address, high byte. |
| $B9A6 | FioEndL | R/W | End address, low byte (used by `SAVE` to determine program extent). |
| $B9A7 | FioEndH | R/W | End address, high byte. |
| $B9A8 | FioSizeL | RO | Loaded data size, low byte (written by host after `LOAD` or `DIR` read). |
| $B9A9 | FioSizeH | RO | Loaded data size, high byte. |
| $B9AA | FioGSpace | R/W | Graphics memory space for `GSAVE`/`GLOAD` (0=screen, 1=color, 2=gfx, 3=sprite). |
| $B9AB | FioGAddrL | R/W | Graphics offset, low byte. |
| $B9AC | FioGAddrH | R/W | Graphics offset, high byte. |
| $B9AD | FioGLenL | R/W | Graphics transfer length, low byte. |
| $B9AE | FioGLenH | R/W | Graphics transfer length, high byte. |
| $B9AF | FioDirType | RO | Directory entry type: 0=PRG, 1=SID. |
| B9B0--B9EF | FioName | R/W | Filename buffer (64 bytes ASCII, not null-terminated). |

### FIO Command Codes

| **Code** | **Name** | **Behavior** |
| --- | --- | --- |
| $01 | FioCmdSave | Save bytes from `FioSrcL/H` to `FioEndL/H` (exclusive) to disk; prepends a 2-byte load-address. |
| $02 | FioCmdLoad | Load file into RAM at `FioSrcL/H`; skips the 2-byte load-address prefix; sets `FioSizeL/H`. |
| $03 | FioCmdDirOpen | Open the program directory; populates `FioName` and `FioSizeL/H` with the first entry. |
| $04 | FioCmdDirRead | Advance to the next directory entry; populates `FioName` and `FioSizeL/H`. |
| $05 | FioCmdDelete | Delete the named program from disk. |
| $06 | FioCmdGSave | Save VGC memory space to a `.gfx` file. FioGSpace=space, FioGAddrL/H=offset, FioGLenL/H=length. |
| $07 | FioCmdGLoad | Load `.gfx` file into VGC memory space. FioGSpace=space, FioGAddrL/H=offset, FioGLenL/H=max length. |
| $08 | FioCmdSidPlay | Load and play a `.sid` file. FioSrcL=song number (1-based). |
| $09 | FioCmdSidStop | Stop SID file playback. |
| $0A | FioCmdInstrument | Define instrument preset. FioSrcL=id, FioSrcH=waveform, FioEndL=A, FioEndH=D, FioSizeL=S, FioSizeH=R. |
| $0B | FioCmdSound | Play SFX. FioSrcL=MIDI note, FioSrcH=duration (frames), FioEndL=instrument ID. |
| $0C | FioCmdVolume | Set SID master volume. FioSrcL=level (0--15). |
| $0D | FioCmdMSeq | Load MML sequence. FioSrcL=voice (1--6), FioEndL/H=string pointer, FioNameLen=string length. |
| $0E | FioCmdMPlay | Start music playback. |
| $0F | FioCmdMStop | Stop music playback. |
| $10 | FioCmdMTempo | Set tempo. FioSrcL/H=BPM (16-bit). |
| $11 | FioCmdMLoop | Set loop. FioSrcL=0 (off) or 1 (on). |

### FIO Status Codes

| **Value** | **Name** | **Meaning** |
| --- | --- | --- |
| $00 | FioStatusIdle | No operation in progress. |
| $02 | FioStatusOk | Operation completed successfully. |
| $03 | FioStatusError | Operation failed; see `FioErrCode`. |

### FIO Error Codes

| **Value** | **Name** | **Meaning** |
| --- | --- | --- |
| $00 | FioErrNone | No error. |
| $01 | FioErrNotFound | File not found on disk. |
| $02 | FioErrIo | Host I/O error (invalid name, end address <= start, OS exception). |
| $03 | FioErrEndOfDir | No more directory entries (returned for `DirOpen` on empty dir or after last entry). |

## XMC Register Map

The Expansion Memory Controller occupies BA00--BA3F.
Writing to $BA00 (`XmcCmd`) triggers the operation.
Memory windows (BC00--BFFF) provide direct CPU-bus access to mapped XRAM pages.

### XMC Registers

| **Address** | **Name** | **Access** | **Description** |
| --- | --- | --- | --- |
| $BA00 | XmcCmd | R/W | Command byte; writing triggers execution. |
| $BA01 | XmcStatus | RO | Result status: 0=idle, 2=ok, 3=error. |
| $BA02 | XmcErrCode | RO | Error detail code (see below). |
| $BA03 | XmcCfg | R/W | Reserved. |
| $BA04 | XmcAddrL | R/W | XRAM address, low byte. |
| $BA05 | XmcAddrM | R/W | XRAM address, middle byte. |
| $BA06 | XmcAddrH | R/W | XRAM address, high byte. |
| $BA07 | XmcRamL | R/W | CPU RAM address, low byte. |
| $BA08 | XmcRamH | R/W | CPU RAM address, high byte. |
| $BA09 | XmcLenL | R/W | Transfer length, low byte. |
| $BA0A | XmcLenH | R/W | Transfer length, high byte. |
| $BA0B | XmcData | R/W | Byte data port (used by `GetByte`/`PutByte`). |
| $BA0C | XmcBank | R/W | Default 64 KB bank selector. |
| $BA0D | XmcBanks | RO | Total number of 64 KB banks available (read-only). |
| $BA0E | XmcPagesUsedL | RO | Used 256-byte pages, low byte. |
| $BA0F | XmcPagesUsedH | RO | Used 256-byte pages, high byte. |
| $BA10 | XmcPagesFreeL | RO | Free 256-byte pages, low byte. |
| $BA11 | XmcPagesFreeH | RO | Free 256-byte pages, high byte. |
| $BA12 | XmcNameLen | R/W | Name length for named block operations (1--28). |
| $BA13 | XmcHandle | RO | Block handle returned by `Alloc`/`NStash`/`DirRead`. |
| $BA14 | XmcDirCountL | RO | Count of named blocks, low byte. |
| $BA15 | XmcDirCountH | RO | Count of named blocks, high byte. |
| $BA16 | XmcWinCtl | R/W | Window enable bitmask (bit 0=window 0, bit 1=window 1, etc.). |
| $BA18 | XmcWin0AL | R/W | Window 0 mapped XRAM base address, low byte. |
| $BA19 | XmcWin0AM | R/W | Window 0 mapped XRAM base address, middle byte. |
| $BA1A | XmcWin0AH | R/W | Window 0 mapped XRAM base address, high byte. |
| $BA1B | XmcWin1AL | R/W | Window 1 mapped XRAM base address, low byte. |
| $BA1C | XmcWin1AM | R/W | Window 1 mapped XRAM base address, middle byte. |
| $BA1D | XmcWin1AH | R/W | Window 1 mapped XRAM base address, high byte. |
| $BA1E | XmcWin2AL | R/W | Window 2 mapped XRAM base address, low byte. |
| $BA1F | XmcWin2AM | R/W | Window 2 mapped XRAM base address, middle byte. |
| $BA20 | XmcWin2AH | R/W | Window 2 mapped XRAM base address, high byte. |
| $BA21 | XmcWin3AL | R/W | Window 3 mapped XRAM base address, low byte. |
| $BA22 | XmcWin3AM | R/W | Window 3 mapped XRAM base address, middle byte. |
| $BA23 | XmcWin3AH | R/W | Window 3 mapped XRAM base address, high byte. |
| BA24--BA3F | XmcName | R/W | ASCII name buffer (28 bytes, not null-terminated). |

### XMC Command Codes

| **Code** | **Name** | **Behavior** |
| --- | --- | --- |
| $01 | XmcCmdGetByte | Read byte at `XmcAddrL/M/H` into `XmcData`. |
| $02 | XmcCmdPutByte | Write `XmcData` to `XmcAddrL/M/H`; marks page used. |
| $03 | XmcCmdStash | Copy `XmcLenL/H` bytes from CPU RAM at `XmcRamL/H` to XRAM at `XmcAddrL/M/H`. `len=0` is a no-op success. |
| $04 | XmcCmdFetch | Copy `XmcLenL/H` bytes from XRAM at `XmcAddrL/M/H` to CPU RAM at `XmcRamL/H`. `len=0` is a no-op success. |
| $05 | XmcCmdFill | Fill `XmcLenL/H` bytes in XRAM starting at `XmcAddrL/M/H` with `XmcData`. |
| $07 | XmcCmdStats | Refresh the `PagesUsed`/`PagesFree`/`DirCount` read-only registers. |
| $08 | XmcCmdResetUsage | Clear all usage tracking, block records, and named-block metadata (destructive). |
| $09 | XmcCmdRelease | Mark XRAM range (`XmcAddrL/M/H`, `XmcLenL/H`) as free; removes any overlapping block records. |
| $0A | XmcCmdAlloc | Allocate `XmcLenL/H` bytes; sets `XmcAddrL/M/H`, `XmcHandle`, and `XmcBank`. |
| $0B | XmcCmdNStash | Named stash: create or update named block from CPU RAM; name read from `XmcName`/`XmcNameLen`. |
| $0C | XmcCmdNFetch | Named fetch: copy named block to CPU RAM at `XmcRamL/H`; `len=0` fetches full block. |
| $0D | XmcCmdNDelete | Delete named block by name. |
| $0E | XmcCmdNDirOpen | Open named-block directory; emits first entry to registers. |
| $0F | XmcCmdNDirRead | Advance to the next named-block directory entry. |

### XMC Status Codes

| **Value** | **Name** | **Meaning** |
| --- | --- | --- |
| $00 | XmcStatusIdle | No operation in progress. |
| $02 | XmcStatusOk | Operation completed successfully. |
| $03 | XmcStatusError | Operation failed; see `XmcErrCode`. |

### XMC Error Codes

| **Value** | **Name** | **Meaning** |
| --- | --- | --- |
| $00 | XmcErrNone | No error. |
| $01 | XmcErrRange | XRAM address or length out of bounds. |
| $02 | XmcErrBadArgs | Invalid arguments (e.g., `len<=0` for `Alloc`, unknown command). |
| $03 | XmcErrNotFound | Named block not found. |
| $04 | XmcErrNoSpace | No contiguous free pages of the required size, or handle pool exhausted. |
| $05 | XmcErrName | Name length is 0 or exceeds 28, or name is blank after trimming. |
| $06 | XmcErrEndOfDir | No more named-block directory entries. |

## System Vectors

The address range 0200--027F is the system vector table.
Each entry is a 16-bit little-endian pointer initialized from ROM at cold start.
BASIC uses the lower portion; the upper portion is reserved for future use.

| **Address** | **Purpose** |
| --- | --- |
| 0200--0202 | Ctrl-C flag, byte, and timeout. |
| 0203--0204 | VEC_CC -- Ctrl-C check vector. |
| 0205--0206 | VEC_IN -- input device vector. |
| 0207--0208 | VEC_OUT -- output device vector. |
| 0209--020A | VEC_LD -- load program vector. |
| 020B--020C | VEC_SV -- save program vector. |
| 020D--0216 | IRQ handler code area. Set by the `IRQ` statement. |
| 0217--0220 | NMI handler code area. Set by the `NMI` statement. |
| 0221--027F | Reserved for future system use. |

::: note
The vector layout at 0200--020C is inherited from EhBASIC 2.22p5.
Hardware controller base addresses are fixed constants; see the Address Space
Overview table at the start of this appendix.
:::
