# Nova Disk Image Format

This document describes the current Nova Disk Image (`.ndi`) format implemented
by `e6502.Storage`.

NDI is a simple, seekable disk image format used by Nova storage devices. It is
not a FAT filesystem. It is intentionally small enough for NovaHost and the
6502-side file I/O path to reason about directly.

## Current Version

| Field | Value |
| --- | --- |
| Magic | `N`, `D`, `I`, `$1A` |
| Format version | `2` |
| Sector size | 256 bytes |
| Byte order | little-endian |
| Header size | 256 bytes |
| Directory size | 48 sectors |
| Directory entry size | 64 bytes |
| Directory capacity | 192 entries |
| Root parent index | `$FFFF` |

## Image Layout

The image is laid out as:

```text
sector 0                  header
sector 1..dirStart-1      BAM sectors
dirStart..dataStart-1     directory sectors
dataStart..end            file data sectors
```

The data sector numbers stored in directory entries are relative to the data
area, not physical image sectors.

Physical byte offset for a file data sector is:

```text
(header.DataStartSector + entry.StartSector) * header.SectorSize
```

## Header

The header is one 256-byte sector at the start of the image.

| Offset | Size | Description |
| ---: | ---: | --- |
| `$00` | 4 | magic bytes: `4E 44 49 1A` (`NDI` + EOF marker) |
| `$04` | 1 | format version, currently `2` |
| `$05` | 2 | sector size, currently `256` |
| `$07` | 1 | reserved, zero |
| `$08` | 4 | total sectors in the image |
| `$0C` | 32 | volume label, null-padded ASCII |
| `$2C` | 4 | directory start sector |
| `$30` | 4 | directory sector count |
| `$34` | 4 | data start sector |
| `$38` | 4 | free data sector count |
| `$3C` | 196 | reserved, zero |

The format version lives at `$04`. Older notes sometimes assumed it lived at
`$03`, but `$03` is part of the magic sentinel.

## BAM

The block allocation map tracks data sectors only.

Each bit represents one data sector:

| Bit value | Meaning |
| --- | --- |
| `0` | free |
| `1` | allocated |

Bit order within each byte is least-significant bit first:

```text
sector 0 -> byte 0 bit 0
sector 1 -> byte 0 bit 1
...
sector 7 -> byte 0 bit 7
sector 8 -> byte 1 bit 0
```

The BAM byte stream is padded to occupy full 256-byte sectors between the header
and directory.

## Directory Entries

Each directory entry is 64 bytes. Four entries fit in one 256-byte sector.

| Offset | Size | Description |
| ---: | ---: | --- |
| `$00` | 1 | flags |
| `$01` | 1 | file type |
| `$02` | 2 | parent directory entry index, `$FFFF` for root |
| `$04` | 4 | start sector, relative to data area |
| `$08` | 4 | size in bytes |
| `$0C` | 32 | filename, null-padded ASCII |
| `$2C` | 4 | allocated sector count |
| `$30` | 16 | reserved |

### Entry Flags

| Bit | Mask | Meaning |
| ---: | ---: | --- |
| 0 | `$01` | active entry |
| 1 | `$02` | directory |
| 7 | `$80` | locked |

Inactive entries have flags byte `0` and may be reused.

### File Types

| Type | ID | Typical extensions |
| --- | ---: | --- |
| `BAS` | 0 | `.bas` |
| `SID` | 1 | `.sid` |
| `BIN` | 2 | `.bin`, `.xram`, unknown extensions |
| `MID` | 3 | `.mid`, `.midi`, `.nms` |
| `GFX` | 4 | `.nvg` |
| `DIR` | 5 | directories |
| `FTH` | 6 | `.4th`, `.fth`, `.fs` |

`GFX` is the internal NDI file type name for graphics. User-facing graphics
assets should use `.nvg`.

`FTH` is Forth source text. Nova's own Forth libraries use `.4th`; `.fth` and
`.fs` are accepted for compatibility with common host-side Forth conventions.

## Filenames

Filenames are stored as ASCII, null-padded, maximum 32 bytes. The implementation
truncates longer names.

Directory lookup is case-insensitive in current host code.

File names are stored with their extension, for example:

```text
AUTOBOOT.bas
KEYBOARD.BIN
STARS.NMS
SPLASH.NVG
CORE.4TH
```

NovaBASIC display and load paths may strip or infer extensions depending on the
runtime command being used, but the NDI directory stores the full filename.

## NovaForth Library Layout

NovaForth library disks should use this conventional directory tree:

```text
/forth/
  autoexec.4th
  site.4th
  lib/
    core.4th
    core-ext.4th
    tools.4th
    file.4th
    string.4th
    facility.4th
    exception.4th
    nova/
      vgc.4th
      fio.4th
      sprite.4th
      sound.4th
      xram.4th
    compat/
      thinking.4th
```

`core.4th` is the boot-time source library for definitions intentionally kept
out of the NovaForth ROM. `autoexec.4th` is loaded after `core.4th` when present;
use it to include `core-ext.4th`, Nova hardware libraries, or site-local policy.
Other files are loaded explicitly with `INCLUDE`/`INCLUDED` (and later
`REQUIRE`/`REQUIRED` once the full file word set exists).

## Directories

Directories are represented as directory entries with:

- active flag set
- directory flag set
- file type `DIR`
- size `0`
- start sector `0`
- sector count `0`

Children refer to the directory entry index in their parent field. Root entries
use parent `$FFFF`.

Directories do not have separate data sectors.

## File Allocation

Files are allocated as contiguous runs of data sectors.

Current write behavior:

1. If a file with the same name exists in the target directory, it is deleted.
2. The BAM searches for the first contiguous run large enough for the new file.
3. File bytes are written to the data area.
4. The final sector is zero-padded.
5. A directory entry records byte size and allocated sector count.

This is simple and fast, but it means images can become fragmented after
delete/reimport churn. A write can fail even if total free space is large when
no contiguous run is large enough.

## BASIC Files

Tokenized NovaBASIC files stored in NDI images should include a two-byte load
address prefix. The normal BASIC load address is `$0301`.

The Nova CLI's `--tokenize` flow writes this prefix automatically.

```bash
nova import demo.ndi AUTOBOOT.bas --tokenize
```

The standalone token command also writes the prefix:

```bash
nova tokenize AUTOBOOT.txt AUTOBOOT.bas --base $0301
```

## Floppy And Hard Disk Images

Floppy and hard disk images use the same NDI on-disk format. The image size and
NovaHost mount slot determine how the system treats the image.

The Nova CLI defaults to an 800 KB floppy-sized image:

```bash
nova create fd0.ndi --label BOOT
```

The CLI's `--hd` shortcut creates a 64 MB hard-disk-sized image:

```bash
nova create hd0.ndi --hd --label HOME
```

Arbitrary image sizes are also supported:

```bash
nova create scratch.ndi --size 4096 --label SCRATCH
```

Mounting is not stored inside the NDI image. NovaHost stores drive-slot mount
configuration in `/config/boot.json` on the SD card.

NovaHost boot slot order is:

```text
fd0, fd1, fd2, fd3, hd0, hd1
```

For each mounted slot, NovaHost looks for `AUTOBOOT.bas` or `AUTOBOOT.bin` in
the image root. If no autoboot file is found, the first mounted floppy is the
default device, followed by the first mounted hard drive.

## Music Files

MIDI files imported or uploaded through the Nova CLI are compiled to Nova Music
Stream (`.nms`) first. The resulting file is stored as file type `MID`.

The current `.nms` compiler emits `NMS2` streams for the ESP/WTS playback path.
The fixed header is little-endian and starts with:

| Offset | Size | Description |
| --- | ---: | --- |
| `$00` | 4 | Magic `NMS2` |
| `$04` | 2 | Format version (`2`) |
| `$06` | 2 | Header size in bytes |
| `$08` | 2 | Event record size (`10`) |
| `$0A` | 2 | WTS sample rate |
| `$0C` | 4 | Event stream offset |
| `$10` | 4 | Event stream byte count |
| `$14` | 4 | Event record count |
| `$18` | 4 | Total audio frames |

When the header size is at least 128 bytes, offsets `$20`, `$40`, and `$60`
contain 32-byte null-padded ASCII title, author, and copyright strings copied
from MIDI metadata. Older 32-byte headers remain valid; players must use the
event stream offset rather than assuming events begin immediately after the
fixed header.

The `.nms` file is not coupled to a soundfont. Playback uses the currently
loaded soundfont, or asks NovaHost to load a default soundfont if one is
available.

## Validation

`nova validate <file.ndi>` checks:

- file allocations stay within the image
- no data sector is claimed by multiple files
- BAM allocation counts match directory allocation counts

Validation does not currently verify every reserved byte or filename character.

## Known Format Limits

- Fixed 192-entry directory table.
- Contiguous allocation only.
- No journaling.
- No timestamps.
- No long filename extension table.
- No sparse files.
- No per-file checksum.
- Host APIs currently read some file sizes into `int`, although the on-disk
  directory size field is 32-bit.

These limits are acceptable for the current Nova storage layer, but should be
revisited if NDI becomes the long-term hard disk format.
