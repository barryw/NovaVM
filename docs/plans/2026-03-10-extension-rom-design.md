# Extension ROM Bank Switching Design

## Problem

The BASIC ROM at $C000-$FFFF is full (3 bytes free out of 16,317). New BASIC
commands like SFLOAD cannot be added. Existing verbose command handlers
(DIR listing loop, HELP text, NCC confirmation dialog) waste precious ROM
space that could be reclaimed.

## Solution

Add an **Extension ROM** (bank 4) that holds command implementations.
Bank 0 (BASIC ROM) keeps thin stubs that parse BASIC arguments, then
bank-switch to the Extension ROM for the actual hardware work.

## Architecture

### Memory Layout (unchanged)

```
$C000-$FFFF  ROM window (16KB) — only one bank visible at a time
$FFFA-$FFFF  CPU vectors: NMI=$0217, IRQ=$020D (RAM), RESET=$FFBD
```

### ROM Banks

| Value | Constant          | ROM              | Purpose                    |
|-------|-------------------|------------------|----------------------------|
| 0x01  | RomSwapNcc        | NCC ROM          | Compiled NCC programs      |
| 0x02  | RomSwapBasic      | BASIC ROM        | EhBASIC interpreter        |
| 0x03  | RomSwapNccEdit    | (event only)     | Opens NCC editor UI        |
| 0x04  | RomSwapExtension  | Extension ROM    | Overflow command handlers  |

### Protocol

**Bank 0 stub (in BASIC ROM, ~8 bytes per command):**

```asm
LAB_SFLOAD
    JSR  LAB_FIO_GETNAME  ; parse filename string (uses EhBASIC internals)
    LDA  #EXT_CMD_SFLOAD  ; extension command ID
    JMP  LAB_EXTCALL      ; trampoline to extension ROM
```

**Trampoline (in BASIC ROM, ~20 bytes, shared by all commands):**

```asm
; A = extension command ID
; Arguments already in FIO registers / RAM scratch area
LAB_EXTCALL
    STA  ExtCmdId          ; $02xx RAM — command ID for extension ROM
    LDA  #$04
    STA  $A03F             ; swap to extension ROM
    JSR  $C000             ; call fixed entry point
    PHA                    ; save result
    LDA  #$02
    STA  $A03F             ; swap back to BASIC ROM
    PLA
    RTS
```

**Extension ROM entry ($C000):**

```asm
ExtEntry
    LDA  ExtCmdId          ; read command ID from RAM
    ASL                    ; * 2 for word-sized table
    TAX
    LDA  ExtTable+1,X      ; high byte
    PHA
    LDA  ExtTable,X        ; low byte
    PHA
    RTS                    ; jump to handler (RTS trick)

ExtTable
    .word EXT_SFLOAD-1
    .word EXT_SIDPLAY-1
    ; ... more commands
```

**Extension ROM handlers** access hardware registers directly ($A183, $B9A0,
etc.) and communicate results back through RAM scratch bytes or FIO status
registers. They never call EhBASIC internals.

### Extension ROM Vectors ($FFFA-$FFFF)

Same as BASIC ROM — NMI and IRQ point to RAM handlers, so they work
regardless of active bank:

```
$FFFA: $0217  (NMI — RAM handler)
$FFFC: $C000  (RESET — extension entry, will swap back to BASIC)
$FFFE: $020D  (IRQ — RAM handler)
```

### Argument Passing Convention

Bank 0 stubs use EhBASIC routines to parse arguments and place them in
known locations before calling LAB_EXTCALL:

| Location          | Purpose                              |
|-------------------|--------------------------------------|
| FIO_NAME buffer   | Filename string (via LAB_FIO_GETNAME)|
| FIO_CMD           | FileIO command byte                  |
| FIO_SRCL/H        | Address argument                     |
| FIO_GSPACE        | Graphics space ID                    |
| ExtCmdArg0-3      | General-purpose argument bytes (RAM) |
| ExtCmdId          | Command dispatch ID (RAM)            |

Results returned via:
- FIO_STATUS (ok/error)
- A register (0=ok, nonzero=error code)
- ExtCmdResult (RAM byte for complex returns)

### RAM Locations

Reserve 8 bytes in page 2 for extension ROM communication:

```
ExtCmdId     = $0280  ; command ID (written by bank 0, read by bank 1)
ExtCmdArg0   = $0281  ; argument byte 0
ExtCmdArg1   = $0282  ; argument byte 1
ExtCmdArg2   = $0283  ; argument byte 2
ExtCmdArg3   = $0284  ; argument byte 3
ExtCmdResult = $0285  ; result byte (written by bank 1, read by bank 0)
ExtCmdFlags  = $0286  ; flags (bank 1 -> bank 0)
ExtReserved  = $0287  ; reserved
```

## Implementation Steps

### Phase 1: Infrastructure + SFLOAD (proof of concept)

1. **VgcConstants.cs** — Add `RomSwapExtension = 0x04`
2. **CompositeBusDevice.cs** — Add `_extRom` field, handle bank 4 in
   RegRomSwap write handler, add `ActiveRom.Extension` enum value.
   Load extension ROM from `Resources/extension.bin`.
3. **ehbasic/extension.s** — Extension ROM source with:
   - Entry point at $C000
   - Command dispatch table
   - SFLOAD handler (write FIO_CMD, poll WTS status, return)
   - Proper vectors at $FFFA-$FFFF
4. **ehbasic/extension.cfg** — Linker config ($C000-$FFFF, 16KB)
5. **ehbasic/Makefile** — Build target for extension.bin
6. **ehbasic/basic.asm** — Add:
   - ExtCmdId RAM location
   - LAB_EXTCALL trampoline (~20 bytes)
   - LAB_SFLOAD stub (~8 bytes)
   - XTK_SFLOAD token + dispatch + string
   - FIO_CMD_SFLOAD constant
7. **e6502.Avalonia.csproj** — Embed extension.bin as resource
8. **Revert FileIoController.cs** — Remove the LOAD ".sf2" redirect hack

### Phase 2: Migrate existing commands to free bank 0 space

Move handler bodies for these commands (keep stubs in bank 0):

| Command    | Est. savings | Notes                              |
|------------|-------------|------------------------------------|
| DIR        | ~200 bytes  | Listing loop + size formatting     |
| HELP       | ~100 bytes  | Help panel trigger + text          |
| GSAVE      | ~30 bytes   | Register setup + command           |
| GLOAD      | ~40 bytes   | Register setup + optional length   |
| SIDPLAY    | ~30 bytes   | Filename + optional song number    |
| MIDPLAY    | ~20 bytes   | Filename + command                 |
| DMACOPY    | ~40 bytes   | 5 args → registers                 |
| DMAFILL    | ~30 bytes   | 4 args → registers                 |
| BLITCOPY   | ~50 bytes   | 8 args → registers                 |
| BLITFILL   | ~40 bytes   | 6 args → registers                 |
| NCC        | ~80 bytes   | Confirmation dialog + strings      |
| MUSIC      | ~60 bytes   | Voice + MML string handling        |
| **Total**  | **~720 bytes** |                                 |

### Phase 3: New commands (using freed space)

With ~720 bytes freed in bank 0 (enough for ~90 stub entries), new
commands can be added freely:
- SFLOAD (already done in phase 1)
- Future: SFINFO, WTSVOICE, etc.

## Files Changed

```
e6502.Avalonia/Hardware/VgcConstants.cs        — new constant
e6502.Avalonia/Hardware/CompositeBusDevice.cs  — extension ROM loading + swap
e6502.Avalonia/e6502.Avalonia.csproj           — embed extension.bin
e6502.Avalonia/Hardware/FileIoController.cs    — revert .sf2 redirect
ehbasic/basic.asm                              — trampoline + SFLOAD stub
ehbasic/extension.s                            — new: extension ROM source
ehbasic/extension.cfg                          — new: linker config
ehbasic/Makefile                               — build extension.bin
ehbasic/tokens.json                            — add SFLOAD token
```

## Risks

- **IRQ during bank switch**: Safe — IRQ/NMI vectors point to RAM handlers.
  The ~10 cycles between STA $A03F and JSR/RTS are fine.
- **NMI during bank switch**: Same — NMI handler is in RAM.
- **Stack depth**: Extension calls add one JSR level. EhBASIC uses ~20
  bytes of stack typically, leaving plenty of room.
- **Extension ROM build dependency**: basic.sym provides RAM addresses.
  Extension ROM must be rebuilt when basic.asm changes layout.
