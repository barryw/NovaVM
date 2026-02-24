---
title: "Assembly and Special Chips"
type: guide
category: Assembly
---

```
"To understand what the machine is actually doing, you have to speak
its language -- and that language is always closer to the metal
than any other abstraction on top of it."
```

NovaBASIC is built on a 6502 core and runs on top of a set of memory-mapped
coprocessors. Most programs never need to touch hardware registers directly, but
knowing where everything lives gives you the full picture: why certain address ranges
are reserved, what BASIC commands actually do at the hardware level, and how to write
assembly routines that cooperate cleanly with the BASIC runtime.

## The Memory Map

The full 64 KB address space is partitioned as follows:

| **Range** | **Size** | **Purpose** | **Access** |
| --- | --- | --- | --- |
| `0000`--`00FF` | 256 B | Zero Page | R/W |
| `0100`--`01FF` | 256 B | Stack | R/W |
| `0200`--`027F` | 128 B | System Vectors | R/W |
| `0280`--`9FFF` | 39 KB | BASIC RAM | R/W |
| `A000`--`A01F` | 32 B | VGC Registers | R/W |
| `A040`--`A0BF` | 128 B | Sprite Registers (16x8) | R/W |
| `A100`--`A13F` | 64 B | Network Interface Controller (NIC) | R/W |
| `AA00`--`B1CF` | 2000 B | Character RAM (80x25) | R/W |
| `B1D0`--`B99F` | 2000 B | Color RAM (80x25) | R/W |
| `B9A0`--`B9EF` | 80 B | File I/O Controller (FIO) | R/W |
| `BA00`--`BA3F` | 64 B | Expansion Memory Controller (XMC) | R/W |
| `BA40`--`BA4F` | 16 B | Timer Controller | R/W |
| `BA50`--`BA56` | 7 B | Music Status (6 voices) | RO |
| `BA60`--`BA7F` | 32 B | DMA Controller | R/W |
| `BA80`--`BA9F` | 32 B | Blitter Controller | R/W |
| `BC00`--`BFFF` | 1024 B | XRAM Windows (when mapped) | R/W |
| `C000`--`FFFF` | 16 KB | ROM (NovaBASIC) | R only |
| `D400`--`D41C` | 29 B | SID chip 1 (write-intercepted) | W only |
| `D420`--`D43C` | 29 B | SID chip 2 (write-intercepted) | W only |

Everything from `A000` upward through `BFFF` is hardware I/O or
managed window space. Writing to ROM (`$C000`+) has no effect.

### System Vectors at `$0200`

At boot, NovaBASIC's ROM initializes page $02 with EhBASIC internal vectors
and the BASIC IRQ/NMI handler pointers. The layout is inherited from
EhBASIC 2.22p5:

| **Address** | **Purpose** |
| --- | --- |
| `0200`--`0202` | Ctrl-C flag, byte, and timeout |
| `0203`--`0204` | VEC_CC -- Ctrl-C check vector |
| `0205`--`0206` | VEC_IN -- input vector |
| `0207`--`0208` | VEC_OUT -- output vector |
| `0209`--`020A` | VEC_LD -- load vector |
| `020B`--`020C` | VEC_SV -- save vector |
| `020D`--`0216` | IRQ handler code area (set by `IRQ`) |
| `0217`--`0220` | NMI handler code area (set by `NMI`) |

Hardware controller base addresses are fixed constants; use the values from
Appendix \refchap:memmap directly in your assembly code.

## Talking to Hardware from BASIC

Because the hardware controllers are memory-mapped, you can read and write their
registers with ordinary `PEEK` and `POKE` calls from BASIC. This is the
simplest way to experiment with hardware state or build lightweight diagnostic tools.

### Reading the frame counter

The VGC increments a frame counter register at `$A008` on every display
frame. Reading it gives you a running frame tick useful for timing and animation:

```
PRINT PEEK(40968)
```

`$A008` = 40968 decimal. The counter wraps from 255 to 0 (it is an 8-bit
register).

### Checking the active sprite count

```
PRINT "SPRITES: ";PEEK(40969)
```

`$A009` = 40969 decimal. This read-only register holds the number of
currently enabled sprites.

### Reading keyboard input

```
K = PEEK(40975)
```

`$A00F` = 40975 decimal. This is the VGC character input register. Reading
it returns the last character received, or 0 if none. BASIC's own `GET`
command uses the same register.

### Character output

Writing a character code to `$A00E` (40974 decimal) emits that character to
the current cursor position, exactly as BASIC's `PRINT` does internally:

```
POKE 40974,65
REM prints the letter A
```

## The CALL and USR Interface

NovaBASIC provides two ways to execute machine code from within a BASIC program.

### `CALL addr`

Performs a `JSR` to *addr*. Execution resumes in BASIC after the
machine-code routine executes an `RTS`. There is no parameter passing;
`CALL` is a simple subroutine jump. You are responsible for preserving
CPU registers if the routine will return to BASIC in a clean state.

```
10 CALL 49152
REM jumps to machine code at $C000
```

### `USR(x)`

Calls a user-defined machine-code routine, passing the numeric value *x*
through the 6502 floating-point accumulator (FAC). The routine can read, modify,
and return a value through the same register. `USR(x)` is a numeric function
and its result can be used in an expression:

```
10 V = USR(42)
20 PRINT "RESULT: ";V
```

The address of the USR routine is set by storing a 16-bit pointer in the
appropriate zero-page location before calling. Consult the EhBASIC 2.22
documentation for the exact zero-page addresses used by the USR vector.

## Interrupts

The 6502 supports two interrupt lines: the maskable IRQ and the non-maskable NMI.
NovaBASIC lets you handle both from within a BASIC program, which is useful for
writing interrupt-driven input handlers, timing routines, and co-operative
multitasking sketches.

### Setting up a handler: `IRQ linenumber` and `NMI linenumber`

```
10 IRQ 1000
20 NMI 2000
```

When an IRQ fires, execution branches to line 1000. When an NMI fires, execution
branches to line 2000. The handlers are ordinary BASIC subroutines.

### Returning from a handler: `RETIRQ` and `RETNMI`

The last statement in an IRQ handler must be `RETIRQ`; the last statement in
an NMI handler must be `RETNMI`. These are not interchangeable with
`RETURN` -- they restore the correct CPU state and re-enable the interrupt
flag.

```
1000 REM IRQ handler
1010 PRINT "IRQ FIRED"
1020 RETIRQ
2000 REM NMI handler
2010 PRINT "NMI FIRED"
2020 RETNMI
```

::: warning
Interrupt handlers run in the context of the BASIC interpreter. Keep them short.
Avoid file I/O, heavy computation, or anything that re-enters the interpreter in
an unexpected state. Long handlers can cause instability.
:::

## XMC Assembly Helpers

The NovaBASIC ROM exports a set of labelled helper routines for accessing the XMC
from assembly code. Using these helpers instead of writing to XMC registers directly
keeps your code clean and gives you error detection for free.

All helpers follow the same convention: on return, **carry clear** means
success and **carry set** means an error occurred, with the XMC error code
in the accumulator.

| **Label** | **Purpose** |
| --- | --- |
| `LAB_XM_SETADDR` | Set the 24-bit XRAM address: A = low byte, X = mid byte, Y = high byte. |
| `LAB_XM_STATUS` | Read status snapshot: A = status register, X = error code. |
| `LAB_XM_GETBYTE` | Read byte at current XADDR: A = value on success. |
| `LAB_XM_PUTBYTE` | Write byte at current XADDR: A = value to write. |
| `LAB_XM_STASH` | Bulk copy RAM to XRAM (preload XMC_RAML/H, XMC_LENL/H). |
| `LAB_XM_FETCH` | Bulk copy XRAM to RAM (preload XMC_RAML/H, XMC_LENL/H). |
| `LAB_XM_FILL` | Fill XRAM range with a byte value (preload XMC_DATA, XMC_LENL/H). |
| `LAB_XM_ALLOC` | Allocate a block: preload XMC_LENL/H; XADDR and handle returned in registers. |

### Example: reading one byte from XRAM in assembly

```
; Set 24-bit address 0x010000 (bank 1, offset 0)
LDA \#$00
LDX \#$00
LDY \#$01
JSR LAB_XM_SETADDR
JSR LAB_XM_GETBYTE
BCS error
; A now holds the byte value
```

::: note
The ROM helper labels are defined in the NovaBASIC assembly source
(`ehbasic/basic.asm`). If you assemble custom ROM extensions or overlays,
link against the same symbol file to pick up these addresses.
:::

## VGC Register-Level Programming

The VGC command pipeline works by writing parameters to registers
`A011`--`A01E`, then writing the command byte to `$A010`.
The VGC executes the command synchronously. This is exactly what every graphics
BASIC command does under the hood.

From assembly you can issue any VGC command directly:

```
; Issue GCLS command (clear graphics layer)
LDA \#$07
STA $A010
```

The full VGC command code reference, including copper commands
(1B--1E) and memory I/O commands (19--1A), is documented in
Appendix \refchap:memmap.

## SID Chip Access

The SID chip registers at D400--D41C are write-intercepted within the ROM
address range. Assembly code can write to them directly:

```
; Set voice 0 to sawtooth waveform, gate on
LDA \#$21
STA $D404
```

The SID register layout matches the MOS 6581. Per-voice registers occupy 7
bytes each (voice 0 at D400, voice 1 atD407, voice 2 at $D40E). Filter
and volume registers are at D415--D418. See Appendix \refchap:memmap
for the full register map.

::: warning
The BASIC `INSTRUMENT`, `SOUND`, and `MUSIC` commands manage
SID registers automatically. Direct SID register writes from assembly will
conflict with the music engine unless you stop all music and SFX first.
:::

## Copper Programming from Assembly

From BASIC, the `COPPER` keyword provides high-level access
(see Chapter \refchap:graphics). From assembly, write parameters to
A011--A016 and the command code to $A010:

```
; Add copper event: at Y=50, X=0, set BgCol to color 5
LDA \#00 : STAA011
LDA \#00 : STAA012
LDA \#50 : STA $A013
LDA \#01 : STAA014
LDA \#00 : STAA015
LDA \#05 : STAA016
LDA \#1B : STAA010
; Enable copper
LDA \#1D : STAA010
```

The copper also supports sprite registers at A040--A0BF. Pass the absolute
address as P3/P4 (16-bit little-endian). For example, to change sprite 0's
Y position at scanline 80:

```
; Copper event: at Y=80, X=0, set sprite 0 YLo to 150
LDA \#00 : STAA011
LDA \#00 : STAA012
LDA \#80 : STA $A013
LDA \#42 : STAA014
LDA \#A0 : STAA015
LDA \#150 : STA $A016
LDA \#1B : STAA010
```

Here P3/P4 = $A042 (sprite 0's YLo register). See
Chapter \refchap:graphics for the COPPER keyword syntax and sprite
multiplexing patterns.

## Sprite Registers from Assembly

The sprite register block at A040--A0BF provides direct memory-mapped
access to all 16 sprites. Each sprite occupies 8 bytes with a stride of 8:

| **Offset** | **Field** | **Size** |
| --- | --- | --- |
| +0 | X position, low byte | 1 byte |
| +1 | X position, high byte | 1 byte |
| +2 | Y position, low byte | 1 byte |
| +3 | Y position, high byte | 1 byte |
| +4 | Shape slot index (0--255) | 1 byte |
| +5 | Flags (bit 0=hflip, bit 1=vflip, bit 2=enable) | 1 byte |
| +6 | Priority (0=behind, 1=between, 2=front) | 1 byte |
| +7 | TransColor (per-sprite transparent color index, 0--15; >15 = all opaque) | 1 byte |

Sprite *n* starts at `$A040 + n*8`. For example, sprite 3
starts at $A058. To position sprite 3 at X=200, Y=100 and enable it:

```
; Sprite 3 base = $A058
LDA \#200 : STA $A058
LDA \#00 : STAA059
LDA \#100 : STA $A05A
LDA \#00 : STAA05B
; Enable (bit 2 of flags)
LDA \#04 : STAA05D
```

Writing directly to sprite registers is faster than going through the VGC
command interface and is the preferred approach from assembly code.

## DMA Controller from Assembly

The DMA controller at $BA60 transfers data between six unified memory
spaces. The pattern is: load parameters into registers, then write $01
to the command register to start.

```basic
10 REM -- DMA copy 2000 bytes from CPU $6000 to char RAM --
20 POKE $BA63, 0   : REM source space = CPU RAM
30 POKE $BA64, 1   : REM dest space = Char RAM
40 POKE $BA65, 0   : POKE $BA66, $60 : POKE $BA67, 0
50 REM source addr = $6000 (low, mid, high)
60 POKE $BA68, 0   : POKE $BA69, 0  : POKE $BA6A, 0
70 REM dest addr = 0 (start of char RAM)
80 POKE $BA6B, $D0 : POKE $BA6C, $07 : POKE $BA6D, 0
90 REM length = 2000 ($07D0)
100 POKE $BA6E, 0  : REM mode = copy (not fill)
110 POKE $BA60, 1  : REM start!
120 IF PEEK($BA61) = 1 THEN 120 : REM poll until not busy
130 IF PEEK($BA61) <> 2 THEN PRINT "Error:"; PEEK($BA62)
```

For fill mode, set bit 0 of `DmaMode` ($BA6E) and load the fill byte
into `DmaFillValue` ($BA6F).

## Blitter from Assembly

The blitter at $BA80 performs 2D rectangular copies and fills with row
stride. Set up source and destination addresses, width, height, and stride,
then write $01 to `BltCmd`.

```basic
10 REM -- Scroll color RAM up by 1 row using blitter --
20 REM Source: row 1 (offset 80), Dest: row 0 (offset 0)
30 REM Width: 80, Height: 24, Stride: 80
40 POKE $BA83, 2  : POKE $BA84, 2   : REM src/dst = color RAM
50 POKE $BA85, 80 : POKE $BA86, 0   : POKE $BA87, 0
60 REM source offset = 80 (row 1)
70 POKE $BA88, 0  : POKE $BA89, 0   : POKE $BA8A, 0
80 REM dest offset = 0 (row 0)
90 POKE $BA8B, 80 : POKE $BA8C, 0   : REM width = 80
100 POKE $BA8D, 24 : POKE $BA8E, 0  : REM height = 24
110 POKE $BA8F, 80 : POKE $BA90, 0  : REM src stride = 80
120 POKE $BA91, 80 : POKE $BA92, 0  : REM dst stride = 80
130 POKE $BA93, 0  : REM mode = copy
140 POKE $BA80, 1  : REM start!
150 IF PEEK($BA81) = 1 THEN 150
```

Color-key mode: set bit 1 of `BltMode` ($BA93) and load the
transparent color into `BltColorKey` ($BA95). Source pixels
matching the color key are skipped.

## Network Controller from Assembly

The NIC at $A100 provides TCP networking. To connect as a client from
register-level code:

```basic
10 REM -- Connect to 127.0.0.1 port 8080 --
20 POKE $A102, 0          : REM slot 0
30 REM Write hostname to name buffer ($A120+)
40 H$ = "127.0.0.1"
50 FOR I = 1 TO LEN(H$)
60   POKE $A11F + I, ASC(MID$(H$, I, 1))
70 NEXT I
80 POKE $A11F + LEN(H$) + 1, 0 : REM null terminate
90 POKE $A108, $90 : POKE $A109, $1F : REM port 8080
100 POKE $A100, 1  : REM connect command
110 REM Poll slot status for connected bit
120 IF (PEEK($A118) AND 1) = 0 THEN 120
```

To send data, copy the message into CPU RAM, then set `NicDmaAddrL/H`
and `NicDmaLen`, and write $03 to `NicCmd`. To receive, write
$04 and read `NicMsgLen` for the actual length.

## The Timer Controller

The timer controller at $BA40 generates periodic IRQ interrupts, firing once
every *N* video frames (at 60 Hz). It has no dedicated BASIC keyword --
you configure it with `POKE` and handle its interrupts with `IRQ`.

### Registers

| **Address** | **Name** | **Purpose** |
| --- | --- | --- |
| `$BA40` | TimerCtrl | Bit 0 = enable |
| `$BA41` | TimerStatus | Bit 0 = IRQ pending (reading clears) |
| `$BA42` | TimerDivL | Divisor low byte |
| `$BA43` | TimerDivH | Divisor high byte |

The timer fires an IRQ every *divisor* video frames. A divisor of 60
fires once per second; a divisor of 1 fires every frame. The pending flag is
cleared automatically when you read `TimerStatus`.

### Example: one-second heartbeat

```basic
10 T = 0
20 IRQ 1000
30 POKE $BA42, 60 : POKE $BA43, 0
40 REM divisor = 60 frames = 1 second
50 POKE $BA40, 1 : REM enable
60 DO : LOOP UNTIL T >= 5
70 POKE $BA40, 0 : REM disable
80 PRINT "DONE"
90 END
1000 T = T + 1
1010 PRINT "TICK "; T
1020 RETIRQ
```

::: note
Disable the timer (`POKE $BA40, 0`) before changing the divisor.
The `SIDPLAY` command uses the timer internally; starting SID playback
will reconfigure the timer divisor.
:::

## Timer from Assembly

The timer at $BA40 fires an IRQ every *N* video frames.
To set up a 1-second timer (60 frames):

```basic
10 IRQ 1000
20 POKE $BA42, 60 : POKE $BA43, 0  : REM divisor = 60
30 POKE $BA40, 1                    : REM enable timer
40 GOTO 40
1000 PRINT "TICK ";
1010 RETIRQ
```

Reading `TimerStatus` ($BA41) clears the pending IRQ flag. The
timer must be disabled with `POKE $BA40, 0` before changing the divisor.

## Try It Now

```
Read the frame counter via PEEK
10 REM Read frame counter via PEEK
20 FOR I=1 TO 60:VSYNC:NEXT I
30 PRINT "FRAMES: ";PEEK(40968)
```

`$A008` = 40968 decimal. After 60 `VSYNC` waits (approximately one
second at 60 Hz), the frame counter register reflects the elapsed frame ticks.
The value will be somewhere in the range 0--255 because the counter wraps after
256 frames. Run the program several times and observe how the value changes.
