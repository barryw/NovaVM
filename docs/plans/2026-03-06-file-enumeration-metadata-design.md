# File Enumeration & Metadata System Design

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Expose filtered directory enumeration and automatic file metadata extraction to both BASIC and assembly programs via the existing FIO register interface.

**Architecture:** Extend FIO_CMD_DIROPEN/DIRREAD to support wildcard filtering and auto-populate a fixed metadata buffer at $BAB0-$BB1F. Add 6 new BASIC tokens as thin wrappers. Assembly programmers use the same registers directly.

**Tech Stack:** C# (FileIoController, CompositeBusDevice), 6502 assembly (EhBASIC ROM), DryWetMidi (MIDI metadata)

---

## 1. Metadata Buffer Layout

112-byte fixed-layout buffer at $BAB0-$BB1F in the free I/O space ($BAA0-$BFFF).

```
Address   Size  Label        Description
-------   ----  -----        -----------
$BAB0     1     META_TYPE    File type (0=BAS, 1=SID, 2=BIN, 3=MID, 4=GFX, 5=DIR)
$BAB1-B2  2     META_SIZE    File size in bytes (16-bit LE)
$BAB3-D2  32    META_TITLE   Title (null-padded ASCII)
$BAD3-F2  32    META_AUTHOR  Author (null-padded ASCII)
$BAF3-12  32    META_CPRT    Copyright (null-padded ASCII)
$BB13-14  2     META_LOAD    Load address (BIN/SID, 16-bit LE)
$BB15-16  2     META_INIT    Init address (SID only, 16-bit LE)
$BB17-18  2     META_PLAY    Play address (SID only, 16-bit LE)
$BB19     1     META_SONGS   Song/track count (SID subtunes / MID tracks)
$BB1A-1B  2     META_DUR     Duration in seconds (MID only, 16-bit LE; 0=unknown)
$BB1C     1     META_GFXSP   GFX space type
$BB1D-1F  3     ---          Reserved
```

Buffer is zeroed before each entry is populated. Fields not applicable to a file type remain zero.

## 2. Wildcard Filter Pattern

### Pattern Syntax

- `*` matches zero or more characters
- `?` matches exactly one character
- Case-insensitive matching
- No regex support

### Pattern Parsing

Format: `[device:][[path/]pattern]`

Examples:
- `*.mid` — all .mid files in current directory of current device
- `BACH*` — all files matching BACH* (any type) in current directory
- `BACH*.mid` — all .mid files matching BACH* in current directory
- `FD0:*.mid` — all .mid files on FD0: current directory
- `FD0:songs/*.sid` — all .sid files in songs/ directory on FD0:

Parsing rules (C# side):
1. If pattern contains `:`, split on first `:` — left = device prefix, right = remainder
2. If remainder contains `/`, last segment = wildcard pattern, prefix = directory path
3. If pattern has an extension (`.mid`, `.sid`, etc.), filter by that extension; wildcard matches against name-without-extension
4. If no extension, match against all supported file types

## 3. FIO Register Protocol Changes

### DIROPEN (FIO_CMD = $03)

**Current behavior:** Enumerate all `.bas` files in current directory. FIO_NAME/FIO_NAMELEN ignored.

**New behavior:**
- If `FIO_NAMELEN == 0`: unchanged — enumerate all files (backward compatible)
- If `FIO_NAMELEN > 0`: FIO_NAME contains a wildcard filter pattern
  - Parse device prefix, path, and wildcard from the pattern
  - Apply wildcard filter during enumeration
  - Populate metadata buffer for first matching entry
  - Set FIO_STATUS = FIO_OK if match found, else FIO_DONE

### DIRREAD (FIO_CMD = $04)

**Current behavior:** Advance to next directory entry, populate FIO registers.

**New behavior (when filtered enumeration active):**
- Advance to next matching entry
- Populate standard FIO registers (FIO_NAME, FIO_NAMELEN, FIO_SIZEL/H, FIO_DIRTYPE)
- Read the file's raw bytes (first N bytes sufficient for header parsing)
- Call type-specific metadata extractor
- Write metadata to $BAB0-$BB1F buffer
- Set FIO_STATUS = FIO_OK if entry available, FIO_DONE if exhausted

**When unfiltered enumeration (FIO_NAMELEN was 0):** no metadata extraction, behaves as today.

## 4. Metadata Extractors

### SID Files
Source: Fixed-offset header fields (no library needed).

| Buffer Field | SID Offset | Bytes |
|-------------|-----------|-------|
| META_TITLE  | 22        | 32    |
| META_AUTHOR | 54        | 32    |
| META_CPRT   | 86        | 32    |
| META_LOAD   | 8         | 2 (big-endian, convert to LE) |
| META_INIT   | 10        | 2 (big-endian, convert to LE) |
| META_PLAY   | 12        | 2 (big-endian, convert to LE) |
| META_SONGS  | 14-15     | 1 (low byte of 16-bit count) |
| META_DUR    | —         | 0 (not available, stays 0) |

### MIDI Files
Source: DryWetMidi library (already a dependency).

| Buffer Field | Source |
|-------------|--------|
| META_TITLE  | First TrackName meta event |
| META_AUTHOR | First Copyright meta event |
| META_CPRT   | Same as author (MIDI has one field) |
| META_SONGS  | Number of track chunks |
| META_DUR    | Computed from timeline (TempoMap + last note-off) |
| META_LOAD   | 0 |

### BIN Files
| Buffer Field | Source |
|-------------|--------|
| META_LOAD   | First 2 bytes of file (load address convention, LE) |
| All others  | 0 |

### BAS / GFX Files
| Buffer Field | Source |
|-------------|--------|
| META_TYPE   | File type enum value |
| META_SIZE   | File size |
| All others  | 0 |

## 5. New BASIC Tokens

Six new extended tokens at $50-$55. `XTK_COUNT` increases from 79 to 85.

### Commands

#### DIROPEN "pattern"
Token: `XTK_DIROPEN = $50`

Parses a string expression, writes it to FIO_NAME/FIO_NAMELEN, issues FIO_CMD_DIROPEN.

```basic
DIROPEN "*.mid"
DIROPEN "FD0:songs/*.sid"
DIROPEN A$
```

### Numeric Functions

#### DIRNEXT
Token: `XTK_DIRNEXT = $51`

Issues FIO_CMD_DIRREAD. Returns -1 (true) if an entry was found (FIO_STATUS == FIO_OK), 0 (false) if enumeration exhausted.

```basic
10 DIROPEN "*.mid"
20 IF NOT DIRNEXT THEN 50
30 PRINT DIRNAM$
40 GOTO 20
50 PRINT "Done"
```

Note: Uses BASIC boolean convention where true = $FFFF (-1) so NOT/AND/OR work correctly.

#### DIRSIZ
Token: `XTK_DIRSIZ = $53`

Returns file size of current entry as a 16-bit integer (reads FIO_SIZEL/FIO_SIZEH).

#### DIRTYP
Token: `XTK_DIRTYP = $54`

Returns file type of current entry as byte (reads FIO_DIRTYPE). Values: 0=BAS, 1=SID, 2=BIN, 3=MID, 4=GFX, 5=DIR.

### String Functions

#### DIRNAM$
Token: `XTK_DIRNAM = $52`

Returns filename of current entry as a BASIC string (reads FIO_NAME, length from FIO_NAMELEN). No arguments.

#### META$(offset)
Token: `XTK_META = $55`

Reads a null-terminated string starting at address $BAB0 + offset from the metadata buffer. The offset is a numeric expression.

```basic
PRINT META$(3)          : REM title  (offset 3 = $BAB3)
PRINT META$(35)         : REM author (offset 35 = $BAD3)
PRINT META$(67)         : REM copyright (offset 67 = $BAF3)
```

Documented offset constants:
- 3 = Title
- 35 = Author
- 67 = Copyright

Numeric metadata fields (load/init/play addresses, duration) are accessed via PEEK:
```basic
LOAD_ADDR = PEEK($BB13) + PEEK($BB14) * 256
DURATION  = PEEK($BB1A) + PEEK($BB1B) * 256
```

## 6. DIR Command Enhancement

The existing `DIR` command (`XTK_DIR = $01`) is extended to accept an optional string argument:

```basic
DIR                       : REM all files, current device/dir (unchanged)
DIR "*.mid"               : REM filtered by pattern
DIR "FD0:songs/*.sid"     : REM filtered, specific device/path
```

Implementation: `LAB_DIR` checks for a following string token. If present, parses it into FIO_NAME/FIO_NAMELEN before issuing DIROPEN. If absent, sets FIO_NAMELEN=0 (existing behavior). Display code unchanged. When filtered, metadata buffer is populated per entry but DIR doesn't display it — it's available for subsequent PEEK/META$ if desired.

## 7. ROM Budget

Available: 242 bytes ($FF08 to $FFFA)

| Item | Estimated Bytes |
|------|----------------|
| DIROPEN command handler | 35 |
| DIRNEXT function handler | 15 |
| DIRNAM$ string function handler | 20 |
| DIRSIZ function handler | 10 |
| DIRTYP function handler | 8 |
| META$ string function handler | 20 |
| DIR command modification | 15 |
| Command dispatch table entries (6 x 2) | 12 |
| Function dispatch table entries (6 x 2) | 12 |
| Keyword strings in token table | 45 |
| **Total** | **~192** |
| **Remaining** | **~50** |

## 8. Assembly Language Access

No ROM helpers needed. Assembly programmers use the same FIO registers and metadata buffer directly:

```asm
META_BASE   = $BAB0
META_TYPE   = $BAB0     ; 1 byte: file type
META_SIZEL  = $BAB1     ; size low byte
META_SIZEH  = $BAB2     ; size high byte
META_TITLE  = $BAB3     ; 32 bytes, null-padded
META_AUTHOR = $BAD3     ; 32 bytes, null-padded
META_CPRT   = $BAF3     ; 32 bytes, null-padded
META_LOADL  = $BB13     ; load address low
META_LOADH  = $BB14     ; load address high
META_INITL  = $BB15     ; init address low (SID)
META_INITH  = $BB16     ; init address high (SID)
META_PLAYL  = $BB17     ; play address low (SID)
META_PLAYH  = $BB18     ; play address high (SID)
META_SONGS  = $BB19     ; song/track count
META_DURL   = $BB1A     ; duration seconds low (MID)
META_DURH   = $BB1B     ; duration seconds high (MID)
META_GFXSP  = $BB1C     ; GFX space type
```

Example enumeration:
```asm
    ; Write "*.mid" to FIO_NAME
    LDX #0
@cp LDA pattern,X
    STA FIO_NAME,X
    INX
    CPX #5
    BNE @cp
    LDA #5
    STA FIO_NAMELEN
    LDA #FIO_CMD_DIROPEN
    STA FIO_CMD
    LDA FIO_STATUS
    CMP #FIO_OK
    BNE @done
@loop:
    ; title at META_TITLE, author at META_AUTHOR, etc.
    ; ... process entry ...
    LDA #FIO_CMD_DIRREAD
    STA FIO_CMD
    LDA FIO_STATUS
    CMP #FIO_OK
    BEQ @loop
@done:
    RTS
pattern: .byte "*.mid"
```

## 9. C# Implementation Scope

### VgcConstants.cs
- Add metadata buffer base address and field offset constants

### FileIoController.cs
- Extend DIROPEN handler: parse filter pattern from FIO_NAME when FIO_NAMELEN > 0
- Extend DIRREAD handler: call metadata extractors, write metadata buffer to bus memory
- Add wildcard matching utility (glob-style `*` and `?`)
- Add metadata extraction methods per file type
- Track whether current enumeration is filtered (controls metadata extraction)

### CompositeBusDevice.cs
- No changes needed — metadata buffer region ($BAB0-$BB1F) is already mapped as regular RAM in the flat byte[] array

### SidFileParser.cs
- Already has `ReadString(data, offset, length)` — reuse for metadata extraction
- Already extracts title/author/copyright/addresses

### MidiPlayback.cs
- Add static method to extract metadata (title, copyright, track count, duration) from raw MIDI bytes using DryWetMidi
- Duration computation already exists for progress bar — factor out

### EhBASIC ROM (basic.asm)
- Add 6 new token definitions ($50-$55)
- Add keyword strings to token table
- Add command handlers: LAB_DIROPEN
- Add function handlers: DIRNEXT, DIRNAM$, DIRSIZ, DIRTYP, META$
- Extend LAB_DIR for optional pattern argument
- Update XTK_COUNT from 79 to 85
- Add dispatch table entries

## 10. Backward Compatibility

- `DIR` with no arguments: unchanged behavior
- `FIO_CMD_DIROPEN` with `FIO_NAMELEN == 0`: unchanged behavior
- `FIO_CMD_DIRREAD` during unfiltered enumeration: no metadata extraction
- Metadata buffer region ($BAB0-$BB1F): previously unused, no conflict
- Existing assembly code using FIO registers: unaffected
