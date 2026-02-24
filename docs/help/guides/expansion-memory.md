---
title: "Expansion Memory"
type: guide
category: Memory
---

```
"The problem with a 64-kilobyte address space is not that it is small.
It is that everything you want to do next is just barely larger than it."
```

The 6502 CPU can directly address 64 KB. That is enough for code, stack, screen RAM,
and a modest program, but it leaves little room for data-heavy work: large sprite
sheets, level maps, sample tables, or pre-rendered buffers quickly overflow the
available RAM. The NovaBASIC expansion memory system solves this by providing an
additional 512 KB of RAM that lives outside the CPU address space, accessible through
a set of commands and a window-mapping mechanism.

## Why Expansion Memory?

The CPU address bus is 16 bits wide. After subtracting ROM, the VGC, the sound
controller, the file I/O coprocessor, the XMC registers, and screen RAM, your usable
BASIC RAM runs from `0280` to `9FFF` -- approximately 39 KB. That
is enough for programs and variables, but becomes tight the moment you try to cache a
full screen buffer, store multiple music tracks, or keep several sprite-sheet layers
in memory simultaneously.

Expansion memory (**XRAM**) gives you up to 512 KB of additional storage with no
impact on the CPU address space. You cannot execute code from XRAM directly, but you
can:

- Store and retrieve arbitrary data blocks by raw offset.
- Name blocks and recall them without tracking raw addresses.
- Map 256-byte XRAM pages directly into CPU address windows for transparent
byte-level access via `PEEK` and `POKE`.
- Allocate and free regions using a lightweight page-tracking allocator.

The XRAM hardware is controlled by the Expansion Memory Controller (XMC), mapped at
`$BA00`. You normally never touch those registers directly -- the BASIC
commands handle all of it for you.

## Banks and Status

XRAM is divided into **banks**, each 64 KB in size. With the default 512 KB
configuration there are **8 banks** (numbered 0 through 7). One bank is always
*active*; raw-offset commands operate within that bank.

### Checking memory status: `XMEM`

Type `XMEM` at the prompt to print a status summary:

```
XMEM
8 BANKS, 512 KB XRAM, BANK 0, USED 0, FREE 2048 PAGES
```

The output shows total bank count, total XRAM capacity, the currently active bank,
and the number of 256-byte pages that are in use versus free across the entire
512 KB space. Two thousand and forty-eight free pages means 512 KB is available.

### Selecting a bank: `XBANK n`

```
XBANK 3
```

Selects bank 3 as the active bank. Subsequent `XPOKE`, `XPEEK`, and raw
`STASH`/`FETCH` operations use offsets within that bank. Valid range is
0 to 7 for the default 512 KB configuration.

::: note
Named blocks (`STASH "name"` / `FETCH "name"`) are allocated automatically
across the full XRAM space and are not limited to the active bank. The active bank
setting only affects raw-offset operations.
:::

## Single-Byte Access

For simple tasks -- writing a flag, reading a configuration byte, probing a value
-- the single-byte commands are the most direct interface.

### `XPOKE offset,value`

Writes *value* (0--255) to *offset* within the currently active bank.
Offsets run from 0 to 65535.

```
XBANK 0
XPOKE 0,42
XPOKE 1,255
```

### `XPEEK(offset)`

Reads a single byte from *offset* in the active bank and returns it as a
numeric value. You can use it anywhere a number is valid:

```
PRINT XPEEK(0)
V = XPEEK(1) + XPEEK(2)
```

::: tip
`XPOKE` and `XPEEK` are convenient for a handful of bytes or for
inspecting specific locations. For moving blocks of data use `STASH` and
`FETCH`, which issue a single hardware transfer rather than looping byte
by byte.
:::

## Bulk Transfers

Moving data in and out of XRAM is the core workflow for anything non-trivial. The
raw bulk-transfer commands operate on explicit offsets within the active bank.

### Raw `STASH` -- CPU RAM to XRAM

```
STASH ramaddr,xramoffset,length
```

Copies *length* bytes starting at CPU address *ramaddr* into XRAM
starting at *xramoffset* within the active bank.

### Raw `FETCH` -- XRAM to CPU RAM

```
FETCH ramaddr,xramoffset,length
```

Copies *length* bytes from XRAM at *xramoffset* back to CPU RAM starting
at *ramaddr*. The destination must be within writable RAM
(`0000`--`BFFF`).

### Example: saving and restoring a character screen buffer

The character screen RAM occupies `AA00`--`B1CF` (2000 bytes).
You can snapshot it to XRAM bank 0 at offset 0 and restore it later:

```
10 REM Save character screen to XRAM
20 XBANK 0
30 STASH 43520,0,2000
40 PRINT "SCREEN STASHED."
50 REM ... do other things ...
60 REM Restore character screen from XRAM
70 FETCH 43520,0,2000
80 PRINT "SCREEN RESTORED."
```

::: note
`AA00` = 43520 decimal. The color RAM at `B1D0` is an additional
2000 bytes; stash it at offset 2000 in the same bank to capture the full display
state including colors.
:::

## Named Blocks

Raw offsets require you to track where each block lives. The named-block interface
lets you store data under a string name and retrieve it without knowing its physical
XRAM address.

### Storing a named block: `STASH "name",ramaddr,length`

```
STASH "MYDATA",2048,16
```

Allocates a region in XRAM, copies 16 bytes from CPU address 2048 into it, and
registers the block under the name `MYDATA`. Names are 1 to 28 characters
and are case-insensitive. If a block with that name already exists and is large
enough for the new data, it is overwritten in place. If the existing block is too
small, it is freed and a new one is allocated automatically.

### Loading a named block: `FETCH "name",ramaddr`

```
FETCH "MYDATA",2048
```

Finds the named block and copies its full contents back to CPU RAM at
*ramaddr*. No length argument is required; the XMC remembers the exact size.

### Listing named blocks: `XDIR`

```
XDIR
```

Prints all named blocks sorted alphabetically (case-insensitive), showing each
name and its stored size in bytes.

### Deleting a named block: `XDEL "name"`

```
XDEL "MYDATA"
```

Removes the named block and releases its XRAM pages back to the free pool.

### Complete example: named-block round-trip

```
10 FOR I=0 TO 15:POKE 2048+I,I+10:NEXT I
20 STASH "MYDATA",2048,16
30 FOR I=0 TO 15:POKE 2048+I,0:NEXT I
40 FETCH "MYDATA",2048
50 FOR I=0 TO 15:PRINT PEEK(2048+I);:NEXT I
60 XDEL "MYDATA"
```

Line 10 writes values 10 through 25 into RAM at address 2048. Line 20 stashes them
to XRAM under the name `MYDATA`. Line 30 zeroes the same RAM region to
confirm the data is no longer in CPU RAM. Line 40 fetches the block back. Line 50
prints the restored values -- you should see
`10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25`. Line 60 cleans up.

## Low-Level Allocation

For programs that need to manage XRAM regions dynamically without names, two
lower-level commands are available.

### `XALLOC length`

Allocates *length* bytes in XRAM and returns a numeric handle (1--255). The
handle identifies the allocated region. You are responsible for recording it; the
XMC does not associate a name.

```
H = XALLOC(4096)
PRINT "HANDLE: ";H
```

### `XFREE offset,length`

Releases the XRAM range starting at *offset* with the given *length*.
Any named or unnamed blocks that overlap that range are removed from the allocation
table.

::: warning
**XRESET** clears all allocation tracking in one step -- named blocks,
unnamed allocations, and page usage records are all discarded and cannot be
recovered. The raw bytes in XRAM are not erased, but every name and handle is gone.
Use `XRESET` only when you want a clean slate, such as at program startup.
:::

## Memory Windows

For the highest-performance XRAM access -- or for assembly code that needs to use
ordinary load and store instructions against XRAM -- you can map an XRAM page
directly into the CPU address space using a **window**.

There are four windows, each exactly 256 bytes wide, at fixed CPU addresses:

| **Window** | **CPU Address Range** | **Size** |
| --- | --- | --- |
| 0 | `BC00`--`BCFF` | 256 bytes |
| 1 | `BD00`--`BDFF` | 256 bytes |
| 2 | `BE00`--`BEFF` | 256 bytes |
| 3 | `BF00`--`BFFF` | 256 bytes |

When a window is mapped, any `PEEK` or `POKE` to its CPU address range reads
or writes directly into XRAM. No transfer command is needed.

### `XMAP window,offset`

Maps the 256-byte XRAM page that contains *offset* into the specified window
(0--3). The offset is rounded down to a 256-byte page boundary automatically.

```
10 REM Map XRAM page 0 into window 0 at $BC00
20 XMAP 0,0
30 FOR I=0 TO 255
40 POKE 48128+I,I
50 NEXT I
60 PRINT PEEK(48128);" ";PEEK(48255)
```

`$BC00` = 48128 decimal. After `XMAP 0,0`, writes to
`BC00`--`BCFF` go directly into XRAM. The `PRINT` on
line 60 should show `0 255`.

### `XUNMAP window`

Unmaps the specified window. Reads and writes to that CPU address range no longer
reach XRAM.

```
XUNMAP 0
```

::: warning
Mapped windows are shared address space visible to both BASIC and any running
assembly code. If two windows are mapped to the same XRAM page, or if BASIC and
assembly both access the same window, coordinate ownership carefully to avoid
corruption. Unmap windows you are finished with.
:::

## Error Codes

When an XRAM command fails, NovaBASIC reports an error derived from the XMC error
register. The complete set:

| **Code** | **Meaning** |
| --- | --- |
| 0 | No error |
| 1 | Address out of range |
| 2 | Bad arguments |
| 3 | Named block not found |
| 4 | No space available |
| 5 | Invalid name |
| 6 | End of directory |

## Try It Now

```
Named block round-trip
10 FOR I=0 TO 15:POKE 2048+I,I+10:NEXT I
20 STASH "MYDATA",2048,16
30 FOR I=0 TO 15:POKE 2048+I,0:NEXT I
40 FETCH "MYDATA",2048
50 FOR I=0 TO 15:PRINT PEEK(2048+I);:NEXT I
60 XDEL "MYDATA"
```

Expected result: line 50 prints `10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25`,
confirming the data survived the round-trip through XRAM and back.
