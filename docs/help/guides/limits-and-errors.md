---
title: "Limits, Errors, and Edge Cases"
type: guide
category: Reference
---

This appendix documents all known numeric limits, argument validation rules,
and edge-case behaviors derived from a direct audit of the ROM source and the
Avalonia hardware controller implementations.
Where ROM behavior and host behavior differ, both are noted.

## Numeric Argument Conversion Rules

Most command arguments are passed through one of two shared ROM helpers
before reaching the hardware layer.
Understanding their constraints prevents unexpected function-call errors.

| **Helper** | **Behavior** |
| --- | --- |
| LAB_GTBY | Converts the FAC (floating-point accumulator) to an unsigned byte. Accepts values 0--255. Any value outside this range, or any negative value, raises a function-call error before the command reaches the hardware. |
| LAB_GTWRD | Converts the FAC to an unsigned 16-bit integer. Accepts values 0--65535. Negative values or values above 65535 raise a function-call error. |

::: warning
Commands that accept addresses (`POKE`, `DOKE`, `CALL`, `WAIT`, `STASH`, `FETCH`...)
route through `LAB_GTWRD`.
Passing a value such as `-1` will raise an error rather than
wrapping to `$FFFF`.
:::

## File Command Limits

| **Topic** | **Behavior** |
| --- | --- |
| Filename length | The ROM filename parser accepts 1--63 characters. A length of 0 or greater than 63 causes the FIO controller to return an I/O error (the `ReadFilename` guard in `FileIoController.cs`). |
| Allowed filename characters | The host implementation enforces the pattern `[A-Za-z0-9_.\-]+`. Any character outside this set causes `ReadFilename` to return `null` and the operation to fail with `FioErrIo`. |
| `.bas` extension | If the filename does not already end in `.bas` (case-insensitive), the host appends it automatically before forming the filesystem path. |
| Missing file | `LOAD "name"` and `DEL "name"` on a non-existent file set `FioStatus=03` and `FioErrCode=01` (`FioErrNotFound`). The ROM interprets this as "File not found". |
| I/O fault | Any OS-level exception during `SAVE`/`LOAD`/`DEL` sets `FioStatus=03` and `FioErrCode=02` (`FioErrIo`). The ROM surfaces this as "I/O Error". |
| `SAVE` end address | If `FioEndH:FioEndL` <= `FioSrcH:FioSrcL`, the save is rejected immediately with `FioErrIo` before the file is opened. |
| `DIR` on empty catalog | `DirOpen` sets `FioStatus=03`/`FioErrCode=03` (`FioErrEndOfDir`) when no `.bas` files exist. The ROM `DIR` handler treats this as a silent empty listing. |
| `DIR` after last entry | Each `DirRead` beyond the final file sets `FioErrEndOfDir`; the ROM stops iterating. |

## Graphics and Sprite Edge Cases

| **Command / Feature** | **Limit and edge behavior** |
| --- | --- |
| `GCOLOR c` | Only the low nibble of *c* is used (`c & 0x0F`). Values 0--15 are valid; values above 15 wrap silently into the 0--15 range. |
| Color 0 on the graphics layer | Color index 0 means transparent on the graphics bitmap. `UNPLOT x,y` explicitly sets a pixel to 0 to restore transparency. |
| `PLOT`/`UNPLOT`/`PAINT` bounds | The ROM dispatches coordinates without host-side pre-clipping. The host `BlockGraphics` implementation performs pixel-level bounds checks; out-of-range coordinates are silently ignored. |
| `LINE`/`RECT`/`CIRCLE`/`FILL` bounds | Drawing operations are clipped by the host renderer (`BlockGraphics.cs`). Portions of the shape outside the 320x200 pixel area are dropped; no error is raised. |
| `FILL` rectangle | Coordinates are clamped to the screen boundary before drawing; the swap of x0/x1 or y0/y1 to ensure a positive rectangle is handled by the host. |
| `SPRITE n,...` invalid index | Sprite index *n* must be 0--15. The VGC command handlers check `n >= MaxSprites` and return immediately without error; no ROM-level error is raised. |
| `SPRITEDATA n,row,...` | Row must be 0--15. The `CmdSprRow` handler checks both sprite index and row; an invalid row causes the command to be silently ignored. |
| `SPRITESHAPE` | Tokenised and recognised by the ROM parser; writes the shape slot field in the sprite register block at `$A044 + n*8`. |
| `SPRITESET n,field,value` | Field must be 0--7 and value must be 0--255 (both byte-range). The write is deferred until the next vblank to prevent mid-frame glitches. |
| `SPRITEX(n)`/`SPRITEY(n)` | Combine the XLo/XHi (or YLo/YHi) register bytes of sprite *n* into a signed 16-bit result. Returns the actual current sprite position as set by `SPRITE n,x,y`, `SPRITESET`, or the copper. |
| `COLLISION(n)`/`BUMPED(n)` | The VGC updates `RegColSt` and `RegColBg` each frame; the ROM reads the appropriate register and clears it on read. A given bit is set if sprite *n* participated in a collision that frame. |
| Default sprite priority | On reset all sprites default to priority 2 (in front of everything). This matches the `SpritePriInFront` constant. |
| `CmdSprFlip` flags | Only bits 0--1 of the flags byte are used (`flags & 0x03`): bit 0 = horizontal flip, bit 1 = vertical flip. |
| `CmdSprPri` clamping | Priority values above 2 are clamped to 2 (`Math.Min(value, 2)`). Values 0, 1, and 2 are the only meaningful levels. |

## Sound and Music Limits

| **Command / Feature** | **Limit and edge behavior** |
| --- | --- |
| `SOUND note,dur[,inst]` | *note* is a MIDI note number (0--127 byte range). *dur* is duration in 1/60-second frames (0--255 byte range). *inst* is instrument slot (0--15, default 0). If *note* or *dur* is 0, the sound is stopped. |
| Master volume | Only the low nibble of the volume byte is used (`level & 0x0F`). The default master volume on power-on is 12. |
| `INSTRUMENT` parameters | All six parameters are bytes (0--255). Waveform should be one of 10,20, 40,80. ADSR values 0--15 are meaningful; higher values use the low nibble only for sustain. |
| `INSTRUMENT` slots | 16 slots (0--15). Slot 0 is pre-initialized at boot. All other slots start as copies of slot 0. |
| `MUSIC voice,...` | Voice must be 1--6. The MML string is read from CPU memory via pointer; maximum practical length limited by available RAM. |
| `MUSIC TEMPO` | BPM is a 16-bit value (0--65535). Default is 120. |
| `MUSIC PRIORITY` | 1--6 voice numbers. Controls which voice is stolen first for SFX. |
| MML pulse width | Range 0--4095. Default 2048. PWM sweep step: +/-32 per frame. |
| MML filter cutoff | Range 0--2047. Resonance 0--15. Filter sweep step: +/-8 per frame. |
| MML vibrato | Depth is any positive integer; 0 = off. Oscillates at 2.9 Hz. |
| MML loops | Non-nesting. Maximum practical depth limited by string expansion. |
| Copper events | Maximum 256 events per list; 128 lists available. Events at duplicate position/register replace existing values. |
| `WAVE` | Deprecated. Raises a syntax error. Use `INSTRUMENT` instead. |
| `SIDPLAY` | Loads `.sid` files from ` /e6502-programs`. Song number is 1-based (default 1). |

## XRAM Limits and Failure Modes

| **Command / Feature** | **Limit and edge behavior** |
| --- | --- |
| `XBANK n` | The ROM verifies that *n* is less than the value stored at `XmcBanks` ($BA0D). An out-of-range bank number triggers a function-call error in the ROM before any XMC command is issued. |
| Window number (`XMAP`/`XUNMAP`) | Window must be 0--3. The ROM validates this; an invalid window triggers a function-call error. |
| Window address space | The four CPU-visible windows occupy BC00--BFFF (4 x 256 bytes). Window 0 maps to BC00, window 1 toBD00, window 2 to BE00, window 3 toBF00. |
| Unmapped window reads/writes | If a window is not enabled in `XmcWinCtl`, the XMC does not own that address and the read/write falls through to flat RAM. No error is returned. |
| Named block name length | The ROM enforces a 1--28 byte name (`XmcNameLen` is capped at 28 by the name buffer size: BA24--BA3F = 28 bytes). The host trims whitespace; a blank name after trimming is rejected with `XmcErrName`. |
| Named block name case | Name lookup is case-insensitive in the host (`StringComparer.OrdinalIgnoreCase`). Storing "`SPRITE`" and retrieving "`sprite`" will succeed. |
| `XALLOC len` with `len<=0` | The ROM passes zero through `LAB_GTWRD`, which itself rejects negative values. The XMC command handler rejects `len<=0` with `XmcErrBadArgs`. |
| `XALLOC` with no free space | If no contiguous run of the required pages exists, or the handle pool (1--255) is exhausted, the command fails with `XmcErrNoSpace`. |
| `STASH`/`FETCH` (raw) with `len=0` | The XMC host treats a zero-length raw transfer as a no-op and returns `XmcStatusOk`. No data is moved and no pages are marked. |
| `FETCH "name",ram` (named fetch) | The ROM sends `XmcLenL/H = 0` for the named-fetch command. The host interprets `requested=0` as "fetch the entire stored block": `len = (requested <= 0) ? block.Length : min(requested, block.Length)`. |
| `STASH "name",ram,len` over existing block | If the named block already exists and the new length fits in the allocated pages, only `block.Length` is updated (no reallocation). If it does not fit, the old block is freed and a new allocation is attempted. |
| `XRESET` | Clears the `_usedPages` array, resets `_usedPageCount` to 0, and removes all block and name records. The raw XRAM byte array is *not* zeroed; data remains but is inaccessible through the allocation system. |
| `XFREE off,len` | Frees all usage-tracking pages in the given range and removes any tracked blocks (named or unnamed) whose page range overlaps with the freed region. |
| `XPOKE`/`XPEEK` bank offset | The ROM constructs the XRAM address as `bank * 65536 + offset`. The host validates the resulting 24-bit address against the total XRAM size; out-of-range addresses return `XmcErrRange`. |
| RAM range validation for STASH/FETCH | The host prevents writes to ROM space: any `FETCH` operation whose destination range would extend into $C000 or above is rejected with `XmcErrRange`. Reads (`STASH`) may source from ROM addresses, allowing code capture. |

## DMA and Blitter Limits

| **Command / Feature** | **Limit and edge behavior** |
| --- | --- |
| Memory space IDs | Valid range: 0--5 (CPU RAM, Char, Color, Gfx, Sprite, XRAM). An invalid space ID sets the error code to `BadSpace` ($02). |
| 24-bit addressing | All DMA and blitter addresses are 24-bit (low/mid/high byte). For XRAM (space 5), the current `XBANK` value is used as the high address byte automatically by the BASIC commands. |
| `DMACOPY`/`DMAFILL` | Length is 24-bit; zero-length transfers are rejected with `BadArgs` (04). Out-of-range addresses set `Range` (03). Writes to ROM space set `WriteProt` ($05). |
| `BLITCOPY`/`BLITFILL` | Width and height are 16-bit (1--65535 each). Zero width or height is rejected with `BadArgs` ($04). Stride is the byte offset between row starts; it may be larger than width. |
| Color-key mode (blitter) | When bit 1 of `BltMode` is set, source bytes matching `BltColorKey` are skipped. In fill mode, color-key is ignored. |
| Row buffer | When source and destination overlap in the same space, the blitter uses a temporary row buffer to prevent read-after-write corruption. |
| `DMASTATUS`/`BLITSTATUS` | Returns the raw status register value: 0=idle, 1=busy, 2=ok, 3=error. BASIC commands check status automatically and raise a function-call error if the operation fails. |

## Network Limits

| **Feature** | **Limit and edge behavior** |
| --- | --- |
| Connection slots | 4 slots (0--3). Each slot is independent and may be a client or server. |
| Message size | Maximum 256 bytes per message. Messages are length-prefixed on the wire (1 byte: 00 = 256,01--$FF = 1--255). |
| Receive queue | Up to 16 messages per slot. If the queue overflows, new messages are dropped and the slot error flag is set. |
| Hostname length | Maximum 31 characters (32-byte buffer, null-terminated). |
| Port range | 1--65535 (16-bit). |
| Connect timeout | 10 seconds. If the remote host does not respond, the slot error flag is set. |
| `NRECV$(slot)` | Returns a string of up to 255 characters. If no message is queued, returns an empty string. |
| `NSTATUS(slot)` | Returns the slot status byte. Bit 4 (RemoteClosed) indicates the remote peer has disconnected; the slot should be closed with `NCLOSE`. |
| `NREADY(slot)` | Returns -1 (true) if at least one message is in the receive queue, 0 (false) otherwise. |
| `NLISTEN` binding | TCP server binds to 127.0.0.1 (localhost). External connections require the remote end to connect to the host machine's loopback address. |

## Status and Error Code Quick Reference

### File I/O Controller (FIO) Codes

| **Value** | **Symbol** | **Meaning** |
| --- | --- | --- |
| $00 | FioStatusIdle | No operation has been issued since last reset. |
| $02 | FioStatusOk | Last operation succeeded. |
| $03 | FioStatusError | Last operation failed; check error code. |
| $00 | FioErrNone | No error. |
| $01 | FioErrNotFound | File not found on disk. |
| $02 | FioErrIo | Host I/O error (bad name, OS exception, end address <= start). |
| $03 | FioErrEndOfDir | Directory enumeration exhausted. |

### Expansion Memory Controller (XMC) Codes

| **Value** | **Symbol** | **Meaning** |
| --- | --- | --- |
| $00 | XmcStatusIdle | No operation in progress. |
| $02 | XmcStatusOk | Last operation succeeded. |
| $03 | XmcStatusError | Last operation failed; check error code. |
| $00 | XmcErrNone | No error. |
| $01 | XmcErrRange | XRAM address or transfer endpoint out of XRAM bounds, or FETCH would write into ROM. |
| $02 | XmcErrBadArgs | Invalid argument (`len<=0` for Alloc, unknown command byte). |
| $03 | XmcErrNotFound | Named block not found in directory. |
| $04 | XmcErrNoSpace | No contiguous free pages available, or handle pool (1--255) exhausted. |
| $05 | XmcErrName | Name length 0 or >28, or name is blank after trimming. |
| $06 | XmcErrEndOfDir | No more named-block directory entries. |

### DMA Controller Codes

| **Value** | **Symbol** | **Meaning** |
| --- | --- | --- |
| $00 | DmaStatusIdle | No operation in progress. |
| $01 | DmaStatusBusy | Transfer is executing. |
| $02 | DmaStatusOk | Last transfer succeeded. |
| $03 | DmaStatusError | Last transfer failed; check error code. |
| $00 | DmaErrNone | No error. |
| $01 | DmaErrBadCmd | Invalid command byte. |
| $02 | DmaErrBadSpace | Invalid source or destination space ID. |
| $03 | DmaErrRange | Address out of bounds for the given space. |
| $04 | DmaErrBadArgs | Invalid arguments (e.g., length <= 0). |
| $05 | DmaErrWriteProt | Destination is write-protected (ROM). |

### Blitter Controller Codes

| **Value** | **Symbol** | **Meaning** |
| --- | --- | --- |
| $00 | BltStatusIdle | No operation in progress. |
| $01 | BltStatusBusy | Blit is executing. |
| $02 | BltStatusOk | Last blit succeeded. |
| $03 | BltStatusError | Last blit failed; check error code. |
| $00 | BltErrNone | No error. |
| $01 | BltErrBadCmd | Invalid command byte. |
| $02 | BltErrBadSpace | Invalid source or destination space ID. |
| $03 | BltErrRange | Rectangular region extends out of bounds. |
| $04 | BltErrBadArgs | Invalid arguments (width <= 0, height <= 0). |
| $05 | BltErrWriteProt | Destination is write-protected (ROM). |

::: tip
The BASIC runtime maps FIO, XMC, DMA, and Blitter error returns to one of
three user-visible messages: "`File not found`",
"`I/O Error`", or a function-call error.
For low-level programs that `POKE` the controller registers directly,
use the tables above to interpret the raw status and error bytes.
:::
