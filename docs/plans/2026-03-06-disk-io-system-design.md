# Nova Disk & I/O System Design

## Overview

Replace the flat `~/e6502-programs` file system with a device-oriented storage architecture supporting floppy disk images and host directory "hard drives," with subdirectories, autoboot, and a standalone CLI tool.

## Device Model

Two device types, accessed from BASIC via filename prefixes:

- **FD0:--FD3:** Floppy devices backed by `.ndi` (Nova Disk Image) files
- **HD0:--HD1:** Hard drive devices backed by host directories

When no device prefix is given, **HD0:** is the default, preserving backward compatibility with the current `~/e6502-programs` behavior.

Paths support subdirectories with `/` as separator. Full paths always work. Paths starting with `/` are absolute from device root; unqualified names resolve relative to the current directory.

### Device Addressing

Device prefix is parsed from the filename (everything before the first `:`). Examples:

```
SAVE "FD0:MUSIC/BACH"        -- explicit device + subdirectory
LOAD "DEMO"                  -- default device (HD0:), current directory
SAVE "HD1:BACKUPS/MYFILE"    -- second hard drive
```

### Device Defaults

- **HD0:** always points to `~/e6502-programs` (backward compatible)
- **HD1:** points to `~/e6502-data` (created on first use)
- **FD0:--FD3:** mounted from `.ndi` files in `~/e6502-disks/`

### Auto-Mount

On boot, `fd0.ndi`--`fd3.ndi` in `~/e6502-disks/` are mounted if present. HD0 and HD1 are always available.

Future: a settings UI (like VICE) will allow customizing device paths, mount points, and defaults.

## NDI Disk Image Format

A `.ndi` file is a single binary container with a header, block allocation map, directory, and data area.

### Header (256 bytes -- sector 0)

| Offset | Size | Field |
|--------|------|-------|
| $00 | 4 | Magic: `NDI\x1A` |
| $04 | 1 | Format version (2) |
| $05 | 2 | Sector size (256, little-endian) |
| $07 | 1 | Reserved, zero |
| $08 | 4 | Total sectors (little-endian) |
| $0C | 32 | Volume label (padded with $00) |
| $2C | 4 | Directory start sector |
| $30 | 4 | Directory sector count |
| $34 | 4 | Data start sector |
| $38 | 4 | Free sector count |
| $3C | 196 | Reserved (future use, zeroed) |

### Block Allocation Map (sectors 1--N)

One bit per data sector. 0=free, 1=used. For 800KB (3200 sectors), that's 400 bytes = 2 sectors.

### Directory Entries (64 bytes each, 4 per sector)

| Offset | Size | Field |
|--------|------|-------|
| $00 | 1 | Flags: bit 0=active, bit 1=directory, bit 7=locked |
| $01 | 1 | File type (0=BAS, 1=SID, 2=BIN, 3=MID, 4=GFX) |
| $02 | 2 | Parent directory entry index ($FFFF = root) |
| $04 | 4 | Start sector |
| $08 | 4 | Size in bytes (little-endian) |
| $0C | 32 | Filename (null-padded, max 32 chars) |
| $2C | 4 | Sector count (allocated) |
| $30 | 16 | Reserved |

### Storage

Files are stored as chains of 256-byte sectors. The last sector may be partially filled (size field gives exact byte count). The allocator prefers contiguous runs for simplicity.

### Image Sizes

- **Default:** 800KB (3200 sectors, 48 directory sectors = 192 entries)
- **1.44MB:** 5760 sectors
- **Minimum:** 170KB
- **Maximum:** 1TB theoretical with 32-bit sector numbers and 256-byte sectors; current tooling target is 64MB hard-disk images.
- Configurable at creation time via FORMAT command or CLI tool.

## BASIC Commands

### New Commands

| BASIC Syntax | FIO Cmd | Byte | Description |
|---|---|---|---|
| `CD "FD0:MUSIC"` | FIO_CMD_CD | $20 | Change current directory |
| `CD "/"` | FIO_CMD_CD | $20 | Return to root (of default device) |
| `MKDIR "FD0:MUSIC"` | FIO_CMD_MKDIR | $21 | Create subdirectory |
| `RMDIR "FD0:MUSIC"` | FIO_CMD_RMDIR | $22 | Remove empty subdirectory |
| `FORMAT "FD0:",800` | FIO_CMD_FORMAT | $23 | Create/format disk image |
| `MOUNT "FD0:","MYDISK"` | FIO_CMD_MOUNT | $24 | Mount .ndi image |
| `UNMOUNT "FD0:"` | FIO_CMD_UNMOUNT | $25 | Detach disk image |
| `PWD` | FIO_CMD_PWD | $26 | Print current device + directory |

### Device-Aware Existing Commands (no new FIO bytes)

- `SAVE`, `LOAD`, `DELETE`, `DIR` -- device prefix parsed from filename
- `GSAVE`, `GLOAD` -- same treatment
- `SIDPLAY`, `MIDPLAY` -- same treatment

### FIO Register Usage

No new registers needed. The device prefix, path, and filename all fit in the existing 64-byte FIO_NAME buffer ($B9A0--$B9EF). FORMAT size is passed via FIO_SRCL/SRCH (size in KB, little-endian). MOUNT uses FIO_NAME for the device prefix and the image name is appended after a null separator.

FIO_DIRTYPE ($B9AF) extended to include $05=DIR for subdirectory entries in directory listings.

### ROM Cost

New keywords need token table entries and dispatch stubs. Each is ~20 bytes for parse + register setup + STA FIO_CMD. Estimated ~150 bytes for all new commands. Autoboot routine ~70 bytes. Total ~220 bytes, well within the 465 bytes free ($FDAF--$FF80).

## Autoboot

### Boot Priority

On startup, scan mounted devices in order: FD0, FD1, FD2, FD3, HD0, HD1. Check each for `AUTOBOOT.BAS` first, then `AUTOBOOT.BIN`. First match wins.

### ROM Routine (LAB_AUTOBOOT, ~70 bytes at ~$FDB0)

Called during cold start, after memory init, before Ready prompt:

1. Check skip flag -- C# sets a byte if spacebar held or `NOAUTO=1` env var
2. Write `"AUTOBOOT"` into FIO_NAME, set FIO_NAMELEN = 8
3. Set FIO_SRCL/H to Smeml/Smemh (BASIC program start)
4. Write FIO_CMD_LOAD to FIO_CMD
5. Check FIO_STATUS -- if not OK, return (fall through to normal Ready)
6. **BAS:** Update Svarl/Svarh from loaded size, JMP LAB_RUN
7. **BIN:** Read load address from FIO_SRCL/H, JMP to it

The C# side handles device priority scanning transparently -- the ROM issues a single LOAD and DeviceManager checks devices in order.

### Skip Mechanisms

- Hold spacebar during boot
- Environment variable: `NOAUTO=1`

## Device Architecture (C# Side)

### IStorageDevice Interface

```csharp
interface IStorageDevice
{
    string Prefix { get; }              // "FD0", "HD0", etc.
    string CurrentDirectory { get; set; }
    bool IsMounted { get; }
    byte[] Load(string path, string ext);
    void Save(string path, byte[] data, string ext);
    void Delete(string path);
    DirEntry[] ListDirectory(string path);
    void MakeDirectory(string path);
    void Format(string label, int sizeKB);
    void Mount(string imagePath);
    void Unmount();
}
```

### Implementations

- **NdiFloppyDevice** -- reads/writes within an `.ndi` file. Manages BAM, directory entries, sector allocation. Handles subdirectory traversal. Mount loads the image; unmount flushes and closes.
- **HostDirectoryDevice** -- wraps a host directory (like today). CurrentDirectory maps to subdirectories. Load/Save/Delete/Dir are thin wrappers around System.IO. Format/Mount/Unmount are no-ops. Filename sanitization preserved.

### DeviceManager

Owns the 6 device slots (FD0--3, HD0--1). Resolves device prefixes from filenames. Tracks the default device. Handles auto-mount on startup. Provides the entry point that FileIoController calls instead of direct file system access.

### FileIoController Changes

The controller no longer touches System.IO directly. It parses the device prefix from the filename (everything before the first `:`), asks DeviceManager for the device, then delegates. New FIO commands added for CD, FORMAT, MOUNT, UNMOUNT, MKDIR, RMDIR, PWD. Existing command bytes ($01--$14) keep their values; new commands start at $20+.

## Token Table Extraction

### tokens.json

A new build step extracts the complete token mapping from `basic.asm` into a machine-readable JSON file, added to the Makefile alongside `novavm.inc`.

Script: `tools/gen_tokens.py` parses `basic.asm` and emits:

```json
{
  "version": 1,
  "primary": {
    "128": "END",
    "129": "FOR",
    "130": "NEXT"
  },
  "extended": {
    "1": "DIR",
    "2": "DEL"
  }
}
```

The script resolves `TK_END = $80`, `TK_FOR = TK_END+1` chains to compute actual byte values, and extracts keyword spellings from the keyword tables or comments. Extended tokens (XTK_) handled separately.

Makefile integration:

```makefile
tokens.json: basic.asm ../tools/gen_tokens.py
    python3 ../tools/gen_tokens.py basic.asm -o tokens.json
```

Auto-rebuilt during `dotnet build` like `novavm.inc`.

### BasicTokenizer (C#)

- Loads `tokens.json` at startup
- `byte[] Tokenize(string[] asciiLines, ushort baseAddr)` -- ASCII to binary .bas
- `string[] Detokenize(byte[] data)` -- binary .bas to ASCII
- Handles quoted strings (literal), line numbers, both primary and extended tokens
- Longest-match tokenization (e.g., "GOSUB" before "GO")
- Shared by NDI CLI tool and emulator

## NDI CLI Tool

### Project: e6502.NDI

New console project referencing `e6502.Storage`.

```
ndi create <file.ndi> [--size <KB>|--hd] [--label <name>]
ndi dir <file.ndi> [/path]
ndi info <file.ndi>
ndi validate <file.ndi>
ndi label <file.ndi> <name>

ndi import <file.ndi> <hostfile> [/dest] [--tokenize]
ndi export <file.ndi> <path> [hostdir] [--detokenize]
ndi delete <file.ndi> <path>
ndi mkdir <file.ndi> <path>
ndi rmdir <file.ndi> <path>

ndi tokenize <input.txt> <output.bas> [--base <addr>]
ndi detokenize <input.bas> [output.txt]
```

### Key Behaviors

- `import --tokenize` reads ASCII BASIC, tokenizes via BasicTokenizer, writes as .bas entry
- `export --detokenize` reads tokenized .bas, emits readable ASCII
- `tokenize`/`detokenize` are standalone host-file converters (no NDI image involved)
- Wildcard import: `ndi import disk.ndi *.bas /PROGRAMS`
- Whole-directory export: `ndi export disk.ndi / ./dump`
- `dir` output shows filename, type (BAS/SID/BIN/MID/GFX/DIR), size, free space summary
- `validate` checks BAM consistency, directory integrity, orphaned sectors
- `--base` defaults to $0801 (typical EhBASIC program start)
- Future: `ndi shell <file.ndi>` interactive REPL mode

### Shared Library: e6502.Storage

New class library containing:

- `NdiImage` -- open/create/flush/close image files
- `NdiHeader` -- parse/write the 256-byte header
- `NdiBam` -- block allocation map operations
- `NdiDirectory` -- directory entry CRUD, subdirectory traversal
- `BasicTokenizer` -- tokenize/detokenize using tokens.json
- `IStorageDevice` -- interface for FileIoController
- `NdiFloppyDevice` -- IStorageDevice over NdiImage
- `HostDirectoryDevice` -- IStorageDevice over host filesystem
- `DeviceManager` -- device slot management, prefix resolution, auto-mount

## Implementation Phases

### Phase 1: Foundation -- e6502.Storage library + NDI format

- NdiImage, NdiHeader, NdiBam, NdiDirectory classes
- Create, read, write, format disk images
- File CRUD + subdirectory support within images
- Unit tests (round-trip create/write/read, BAM consistency, subdirectories)

### Phase 2: NDI CLI tool + tokenizer

- gen_tokens.py script + tokens.json extraction in Makefile
- BasicTokenizer (tokenize/detokenize)
- e6502.NDI console tool -- all commands
- Integration tests: create image, import ASCII with --tokenize, export with --detokenize, verify round-trip

### Phase 3: Device layer + FileIoController integration

- IStorageDevice, NdiFloppyDevice, HostDirectoryDevice, DeviceManager
- Refactor FileIoController to route through DeviceManager
- Device prefix parsing from filenames
- CD, MKDIR, RMDIR, FORMAT, MOUNT, UNMOUNT FIO commands
- Auto-mount on startup
- HD0 backward compatibility verified

### Phase 4: ROM changes + autoboot

- New BASIC keywords: CD, MKDIR, RMDIR, FORMAT, MOUNT, UNMOUNT, PWD
- Extended token table entries + dispatch stubs
- Autoboot ROM routine (LAB_AUTOBOOT)
- gen_tokens.py updated for new tokens
- End-to-end test: create image, add AUTOBOOT.BAS, boot, verify auto-run
