# Nova CLI User Guide

`nova` is the command-line tool for Nova disk images and NovaHost SD-card
management.

NDI is the disk image format. Nova is the CLI.

This guide is written for someone trying to get useful work done without
reverse-engineering the code. It covers the implemented commands, their
parameters, the paths they touch, and examples that are exercised by
`tools/test-nova-cli-doc-examples.py`.

## Quick Start

From the repository, run the managed CLI like this:

```bash
dotnet run --project e6502.Nova --
```

For shorter commands during development:

```bash
alias nova='dotnet run --project /Users/barry/Git/e6502/e6502.Nova --'
```

If you have published the NativeAOT binary, use it directly:

```bash
artifacts/nova-cli/osx-arm64/nova --help
```

Create a floppy-sized image, add a tokenized boot program, inspect it, and
validate it:

```bash
nova create doc-demo.ndi --label DOCDEMO
nova import doc-demo.ndi AUTOBOOT.bas --tokenize
nova dir doc-demo.ndi
nova validate doc-demo.ndi
```

Upload that image to NovaHost as a floppy image:

```bash
nova disk upload doc-demo.ndi --remote 192.168.1.65 --floppy --name doc-demo.ndi
```

Mount it into `fd0`:

```bash
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.65
```

Check the board:

```bash
nova device status --remote 192.168.1.65
```

## Mental Model

Nova has three separate layers:

| Layer | What It Is | Managed By |
| --- | --- | --- |
| `nova` CLI | Host-side tool for images and NovaHost files | Your development machine |
| NDI | Disk image format for floppies and hard drives | `e6502.Storage` and NovaHost |
| NovaHost | ESP-side HTTP server and SD-card/device manager | The board |

Floppy and hard-drive images use the same NDI format. The difference is size,
where the image lives on the SD card, and which NovaHost slot mounts it.

NovaHost drive slots are:

```text
fd0 fd1 fd2 fd3 hd0 hd1
```

Boot selection checks those slots in this order:

```text
fd0, fd1, fd2, fd3, hd0, hd1
```

For each mounted slot, NovaHost looks for `AUTOBOOT.bas` or `AUTOBOOT.bin` in
the image root. If no mounted image has an autoboot file, the first mounted
floppy wins, then the first mounted hard drive.

Drive mount state is stored on the SD card in:

```text
/config/boot.json
```

The CLI uploads images and files, manages mounted drive slots, updates WiFi
settings, stops NovaHost-driven audio, reboots NovaHost, and can forward local
terminal keypresses to the running NovaVM over WiFi. The low-level NovaHost HTTP
API still exists underneath, but normal users should not need to type raw HTTP
requests.

## Build The CLI

Development build:

```bash
dotnet build e6502.Nova/e6502.Nova.csproj -c Release
```

`dotnet build` is for fast local compilation. `dotnet publish` for the Nova CLI
defaults to NativeAOT and produces the binary users should run.

Publish a single NativeAOT binary for the current machine:

```bash
tools/publish-nova-cli.sh
```

Publish for a specific runtime:

```bash
tools/publish-nova-cli.sh osx-arm64
tools/publish-nova-cli.sh linux-x64
tools/publish-nova-cli.sh linux-arm64
tools/publish-nova-cli.sh win-x64
```

NativeAOT output paths:

```text
artifacts/nova-cli/osx-arm64/nova
artifacts/nova-cli/linux-x64/nova
artifacts/nova-cli/linux-arm64/nova
artifacts/nova-cli/win-x64/nova.exe
```

The NativeAOT publish may report trim warnings for DryWetMidi. MIDI import and
upload are still expected to work; verify with a `.mid` to `.nms` import/upload.

## Remote Host Syntax

Remote commands talk to NovaHost over HTTP.

`--remote` may appear before or after the command:

```bash
nova --remote 192.168.1.65 disk list
nova disk list --remote 192.168.1.65
nova disk list --remote=192.168.1.65
```

The host may include a port when testing against a local server:

```bash
nova device status --remote 127.0.0.1:8080
```

Remote paths are relative to NovaHost's `/sd` URL. These are equivalent:

```bash
nova get --remote 192.168.1.65 /config/boot.json
nova get --remote 192.168.1.65 config/boot.json
```

Backslashes are normalized to `/`. Leading slashes are stripped before building
the `/sd/...` URL.

## File Types

The CLI infers local NDI file type from extension:

| Extension | NDI Type | Notes |
| --- | --- | --- |
| `.bas` | `BAS` | Tokenized when imported with `--tokenize` |
| `.sid` | `SID` | SID music file |
| `.bin` | `BIN` | Binary program/data |
| `.mid`, `.midi` | `MID` | Compiled to `.nms` first |
| `.nms` | `MID` | Nova Music Stream |
| `.nvg` | `GFX` | Nova graphics asset |
| unknown | `BIN` | Stored as binary data |

`GFX` is the internal NDI file type name for graphics. User-facing graphics
assets should use `.nvg`.

## Tested Examples

The examples in this guide are intended to be executable. Run the local image
and remote-command documentation smoke test with:

```bash
tools/test-nova-cli-doc-examples.py
```

The test creates temporary NDI images, imports BASIC, binary, MIDI, and NVG
files, validates image state, and runs remote commands against a local
NovaHost-compatible HTTP mock.

Remote examples using `192.168.1.65` are the same command shapes. Replace that
host with your board's IP address.

## Local Image Commands

Local image commands operate on `.ndi` files on the development machine.

### `create`

Create and format an NDI image. Existing files are overwritten.

```bash
nova create <file.ndi> [--size <KB>|--hd] [--label <name>]
```

Options:

| Option | Meaning |
| --- | --- |
| `--size <KB>` | Image size in kilobytes. Default is `800`. |
| `--hd` | Create a 64 MB image, equivalent to `--size 65536`. |
| `--label <name>` | Volume label. Default is `DISK`. |

Tested examples:

```bash
nova create doc-demo.ndi --label DOCDEMO
nova create hd0.ndi --hd --label HOME
nova create scratch.ndi --size 4096 --label SCRATCH
```

### `info`

Print image metadata.

```bash
nova info <file.ndi>
```

Shows format version, volume label, sector size, total sectors, free sectors,
directory location, data start, and directory capacity.

Tested example:

```bash
nova info doc-demo.ndi
```

### `validate`

Check image consistency.

```bash
nova validate <file.ndi>
```

Validation checks that file allocations stay inside the image, data sectors are
not double allocated, and BAM used/free counts match directory usage.

Tested example:

```bash
nova validate doc-demo.ndi
```

### `label`

Update the image volume label in place.

```bash
nova label <file.ndi> <name>
```

The label must be 32 characters or fewer.

Tested example:

```bash
nova label doc-demo.ndi DOCS
```

### `dir`

List a directory inside an image.

```bash
nova dir <file.ndi> [/path]
```

If `/path` is omitted, the root directory is listed.

Tested examples:

```bash
nova dir doc-demo.ndi
nova dir doc-demo.ndi /programs
```

### `mkdir`

Create a directory inside an image.

```bash
nova mkdir <file.ndi> <path>
```

Parent directories must already exist.

Tested example:

```bash
nova mkdir doc-demo.ndi /programs
```

### `rmdir`

Remove an empty directory from an image.

```bash
nova rmdir <file.ndi> <path>
```

Tested example:

```bash
nova mkdir doc-demo.ndi /empty
nova rmdir doc-demo.ndi /empty
```

### `import`

Import a host file into an image.

```bash
nova import <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>]
```

Parameters and options:

| Parameter | Meaning |
| --- | --- |
| `<file.ndi>` | Target image. |
| `<hostfile>` | Host file to import. |
| `[/dest]` | Destination directory inside the image. Defaults to `/`. |
| `--tokenize` | Tokenize ASCII NovaBASIC source before storing it. |
| `--tokens <path>` | Explicit tokenizer JSON file. Defaults to discovered `ehbasic/tokens.json`. |

Behavior:

- Existing files with the same name in the destination directory are replaced.
- `.mid` and `.midi` files are compiled to `.nms` automatically.
- MIDI import cannot be combined with `--tokenize`.
- Tokenized BASIC import writes the normal two-byte BASIC load address prefix.
- Destination is only recognized when it starts with `/`.

Tested examples:

```bash
nova import doc-demo.ndi AUTOBOOT.bas /programs --tokenize
nova import doc-demo.ndi ehbasic/basic.bin
nova import doc-demo.ndi e6502.ESP32/novahost/assets/boot/novavm_logo.nvg /programs
nova import doc-demo.ndi docs/programs/midi/sousa-stars-stripes.mid /programs
```

The MIDI example stores `sousa-stars-stripes.nms` as type `MID`.

### `export`

Export a file from an image.

```bash
nova export <file.ndi> <path> [hostdir-or-file] [--detokenize] [--tokens <path>]
```

Parameters and options:

| Parameter | Meaning |
| --- | --- |
| `<file.ndi>` | Source image. |
| `<path>` | File path inside the image. Must name a file, not a directory. |
| `[hostdir-or-file]` | Existing directory or output filename. Defaults to current directory. |
| `--detokenize` | Convert tokenized BASIC to ASCII text. |
| `--tokens <path>` | Explicit tokenizer JSON file. |

Tested examples:

```bash
nova export doc-demo.ndi /programs/AUTOBOOT.bas ./exported --detokenize
nova export doc-demo.ndi /basic.bin ./basic-export.bin
```

### `delete`

Delete a file from an image.

```bash
nova delete <file.ndi> <path>
```

Tested example:

```bash
nova delete doc-demo.ndi /programs/novavm_logo.nvg
```

## BASIC Token Commands

These commands operate on host files. They do not require an NDI image.

### `tokenize`

Convert ASCII NovaBASIC source to tokenized BASIC with a two-byte load address
prefix.

```bash
nova tokenize <input.txt> <output.bas> [--base <addr>] [--tokens <path>]
```

Options:

| Option | Meaning |
| --- | --- |
| `--base <addr>` | BASIC load address, parsed as hexadecimal. Accepts `$0301`, `0x0301`, or `0301`. Default is `$0301`. |
| `--tokens <path>` | Explicit tokenizer JSON file. |

If `--tokens` is omitted, the CLI searches upward from the executable directory
and current working directory for `ehbasic/tokens.json`.

Tested example:

```bash
nova tokenize AUTOBOOT.bas AUTOBOOT.tokenized.bas --base $0301
```

### `detokenize`

Convert a tokenized BASIC file back to ASCII text.

```bash
nova detokenize <input.bas> [output.txt] [--tokens <path>]
```

If no output path is supplied, text is written to stdout.

Tested examples:

```bash
nova detokenize AUTOBOOT.tokenized.bas
nova detokenize AUTOBOOT.tokenized.bas AUTOBOOT.roundtrip.txt
```

## Remote Device Commands

### `device status`

Read NovaHost health and SD-card status.

```bash
nova device status --remote <host>
```

This performs:

```text
GET /health
GET /sd-status
```

Tested example:

```bash
nova device status --remote 192.168.1.65
```

Use this first when the board is behaving oddly.

### `device reboot`

Reboot NovaHost.

```bash
nova device reboot --remote <host>
```

Tested example:

```bash
nova device reboot --remote 192.168.1.65
```

This reboots the ESP-side NovaHost. It is not the same thing as a 6502 warm
start or cold start.

## Drive Slot Commands

Drive commands manage the six mounted NDI image slots on NovaHost.

```bash
nova drive list --remote <host>
nova drive mount <fd0|fd1|fd2|fd3|hd0|hd1> <sd-path.ndi> --remote <host>
nova drive unmount <fd0|fd1|fd2|fd3|hd0|hd1> --remote <host>
```

`drives` is accepted as an alias for `drive`.

### `drive list`

List mounted and configured drive slots.

```bash
nova drive list --remote <host>
```

Tested example:

```bash
nova drive list --remote 192.168.1.65
```

### `drive mount`

Mount an NDI image from the SD card into a drive slot and persist that mount in
`/config/boot.json`.

```bash
nova drive mount <slot> <sd-path.ndi> --remote <host>
```

The SD path may be written with or without the leading `/`.

Tested examples:

```bash
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.65
nova drive mount hd0 /disks/hard/hd0.ndi --remote 192.168.1.65
```

### `drive unmount`

Unmount a drive slot and clear its persisted mount path.

```bash
nova drive unmount <slot> --remote <host>
```

Tested example:

```bash
nova drive unmount fd0 --remote 192.168.1.65
```

After unmounting, that image will not come back on the next boot unless it is
mounted again.

## WiFi Commands

WiFi commands manage NovaHost WiFi state and configuration.

```bash
nova wifi status --remote <host>
nova wifi scan --remote <host>
nova wifi set --remote <host> --ssid <ssid> [--password <password>] [--dhcp]
nova wifi set --remote <host> --ssid <ssid> [--password <password>] --static --static-ip <ip> --gateway <ip> --subnet <mask> [--dns <ip>]
nova wifi connect|disconnect|reconnect|forget --remote <host>
```

### `wifi status`

Read configured and live WiFi state.

```bash
nova wifi status --remote 192.168.1.65
```

### `wifi scan`

List nearby WiFi networks visible to NovaHost.

```bash
nova wifi scan --remote 192.168.1.65
```

### `wifi set`

Update WiFi configuration.

DHCP example:

```bash
nova wifi set --remote 192.168.1.65 --ssid NovaLab --password secret --dhcp
```

Static IP example:

```bash
nova wifi set --remote 192.168.1.65 --ssid NovaLab --password secret \
  --static --static-ip 192.168.1.65 --gateway 192.168.1.1 \
  --subnet 255.255.255.0 --dns 192.168.1.1
```

Aliases:

| Option | Aliases |
| --- | --- |
| `--password` | `--pass` |
| `--static-ip` | `--ip` |
| `--gateway` | `--gw` |
| `--subnet` | `--netmask` |

### WiFi Actions

```bash
nova wifi connect --remote 192.168.1.65
nova wifi disconnect --remote 192.168.1.65
nova wifi reconnect --remote 192.168.1.65
nova wifi forget --remote 192.168.1.65
```

`forget` clears the stored WiFi configuration.

## Audio Commands

Audio commands manage NovaHost-driven audio playback.

```bash
nova audio status --remote <host>
nova audio stop --remote <host>
```

Tested examples:

```bash
nova audio status --remote 192.168.1.65
nova audio stop --remote 192.168.1.65
```

## Remote Keyboard Command

`nova keyboard` forwards keypresses from the local terminal to the running
NovaVM through NovaHost's debug TCP service. This is a WiFi keyboard stopgap for
hardware sessions where the board-side USB keyboard path is not usable.

```bash
nova keyboard --remote <host> [--port 6503] [--echo] [--ctrl-c-quits]
```

Example:

```bash
nova keyboard --remote 192.168.1.65
```

Control keys:

- `Ctrl-]` exits the local bridge.
- `Ctrl-C` is sent to Nova as byte `$03`; pass `--ctrl-c-quits` to make it
  exit the bridge instead.
- Escape and F1 also send Nova's `$03` break byte.
- Enter, Backspace, Tab, Delete, printable ASCII, and arrow keys are forwarded.
  Arrow keys use the same `$1C` through `$1F` bytes as the desktop emulator;
  the active Nova editor or program decides what those bytes mean.

This command uses TCP port `6503`, not NovaHost's HTTP port. Use `--port` only
when the debug service is exposed on a non-default port.

## Raw Remote SD Commands

Raw remote commands map directly to NovaHost `/sd/<path>`.

```bash
nova ls --remote <host> [path]
nova put --remote <host> <local-path> [remote-path]
nova get --remote <host> <remote-path> [local-path]
nova rm --remote <host> <remote-path>
```

### `ls`

List an SD directory.

```bash
nova ls --remote <host> [path]
```

Tested examples:

```bash
nova ls --remote 192.168.1.65
nova ls --remote 192.168.1.65 disks/floppy
```

### `put`

Upload a local file to an SD path.

```bash
nova put --remote <host> <local-path> [remote-path]
```

If the local file is `.mid` or `.midi`, `put` compiles it to `.nms` before
upload. For all other extensions, bytes are uploaded exactly as supplied.

Tested examples:

```bash
nova put --remote 192.168.1.65 doc-demo.ndi disks/floppy/doc-demo.ndi
nova put --remote 192.168.1.65 docs/programs/midi/sousa-stars-stripes.mid music/STARS.NMS
```

### `get`

Download an SD file.

```bash
nova get --remote <host> <remote-path> [local-path]
```

Tested example:

```bash
nova get --remote 192.168.1.65 config/boot.json ./boot.json
```

### `rm`

Delete an SD file or empty directory.

```bash
nova rm --remote <host> <remote-path>
```

Tested example:

```bash
nova rm --remote 192.168.1.65 tmp/old.bin
```

NovaHost rejects writes/deletes against mounted `.ndi` files. Unmount the slot
first.

## Managed Remote Commands

Managed remote commands place files under standard SD-card directories.

### Disk Images

```bash
nova disk list --remote <host> [--hard|--hd|--floppy|--fd] [--path <path>]
nova disk upload <file.ndi> --remote <host> [--hard|--hd|--floppy|--fd] [--name <name>] [--path <path>]
nova disk download <name-or-path> --remote <host> [local-path] [--hard|--hd|--floppy|--fd] [--path <path>]
nova disk delete <name-or-path> --remote <host> [--hard|--hd|--floppy|--fd] [--path <path>]
```

Standard directories:

| Image Kind | SD Directory |
| --- | --- |
| Floppy | `/disks/floppy` |
| Hard disk | `/disks/hard` |

Placement rules for upload:

- `--floppy` or `--fd` forces `/disks/floppy`.
- `--hard` or `--hd` forces `/disks/hard`.
- `--path <path>` is an explicit SD path and overrides disk type inference.
- Without a flag, filenames beginning with `hd` go to `/disks/hard`.
- Without a flag, images at least 16 MB go to `/disks/hard`.
- Everything else defaults to `/disks/floppy`.

Tested examples:

```bash
nova disk list --remote 192.168.1.65
nova disk list --remote 192.168.1.65 --floppy
nova disk upload doc-demo.ndi --remote 192.168.1.65 --floppy --name doc-demo.ndi
nova disk download doc-demo.ndi --remote 192.168.1.65 --floppy ./doc-demo.downloaded.ndi
nova disk delete doc-demo.ndi --remote 192.168.1.65 --floppy
```

Uploading a disk image only copies it to the SD card. It does not mount the
image into `fd0`, `fd1`, `hd0`, etc.

### ROMs

ROM commands place files under `/roms`.

```bash
nova rom list --remote <host>
nova rom upload <file> --remote <host> [--name <name>] [--path <path>]
nova rom download <name-or-path> --remote <host> [local-path] [--path <path>]
nova rom delete <name-or-path> --remote <host> [--path <path>]
```

If `--name` has no extension, the local file extension is appended.

Tested examples:

```bash
nova rom upload ehbasic/basic.bin --remote 192.168.1.65 --name novabasic.bin
nova rom list --remote 192.168.1.65
nova rom download novabasic.bin --remote 192.168.1.65 ./novabasic.downloaded.bin
```

Uploading ROM files does not automatically reload them into the running FPGA.
Use the existing host-control tooling for runtime reloads.

### Soundfonts

Soundfont commands place files under `/soundfonts`.

```bash
nova soundfont list --remote <host>
nova soundfont upload <file.nsfb> --remote <host> [--name <name>] [--path <path>]
nova soundfont download <name-or-path> --remote <host> [local-path] [--path <path>]
nova soundfont delete <name-or-path> --remote <host> [--path <path>]
```

Current limitation: upload accepts only Nova-native `.nsfb` banks. Uploading an
`.sf2` is rejected because SF2-to-NSFB conversion is not implemented in this
CLI.

Tested examples:

```bash
nova soundfont upload DocBank.nsfb --remote 192.168.1.65
nova soundfont list --remote 192.168.1.65
nova soundfont download DocBank.nsfb --remote 192.168.1.65 ./DocBank.downloaded.nsfb
```

### Music

Music commands place files under `/music`.

```bash
nova music list --remote <host>
nova music upload <file.mid|file.midi|file.nms> --remote <host> [--name <name>] [--path <path>]
nova music download <name-or-path> --remote <host> [local-path] [--path <path>]
nova music delete <name-or-path> --remote <host> [--path <path>]
```

Behavior:

- `.mid` and `.midi` are compiled to `.nms` at upload time.
- `.nms` is uploaded directly.
- The uploaded song is not tied to a soundfont.
- `MIDPLAY` uses the loaded soundfont, or asks NovaHost to load the default
  soundfont if one is available and none is already loaded.

Tested examples:

```bash
nova music upload docs/programs/midi/sousa-stars-stripes.mid --remote 192.168.1.65 --name STARS.NMS
nova music list --remote 192.168.1.65
nova music download STARS.NMS --remote 192.168.1.65 ./STARS.downloaded.NMS
```

MIDI compiler notes:

- SMPTE MIDI timing is not supported.
- The compiler emits Nova Music Stream version 2 (`NMS2`).
- Newly compiled streams preserve MIDI title, author, and copyright metadata
  when those MIDI meta events are present.
- Initial tempo defaults to 120 BPM until tempo events change it.
- Events are routed to up to 8 WTS voices.
- Program, note, velocity, channel, and frame timestamp are preserved.

### Assets

Asset commands place files under `/assets` or a typed asset subdirectory.

```bash
nova asset list --remote <host> [--type|--kind|-t <type>]
nova asset upload <file> --remote <host> --type|--kind|-t <type> [--name <name>] [--path <path>]
nova asset download <name-or-path> --remote <host> [--type|--kind|-t <type>] [local-path] [--path <path>]
nova asset delete <name-or-path> --remote <host> [--type|--kind|-t <type>] [--path <path>]
```

Type mapping:

| Type | SD Directory |
| --- | --- |
| `boot` | `/assets/boot` |
| `font`, `fonts` | `/assets/fonts` |
| `sid`, `sid-assets`, `sid_assets` | `/assets/sid` |
| `music`, `mid`, `midi` | `/music` |
| any other type | `/assets/<type>` |

Tested examples:

```bash
nova asset upload e6502.ESP32/novahost/assets/boot/novavm_logo.nvg \
  --remote 192.168.1.65 --type boot

nova asset list --remote 192.168.1.65 --type boot
nova asset download novavm_logo.nvg --remote 192.168.1.65 --type boot ./novavm_logo.downloaded.nvg
```

## Standard SD Layout

NovaHost currently uses this SD-card layout:

```text
/config/boot.json
/roms/novabasic.bin
/roms/extension.bin
/disks/floppy/*.ndi
/disks/hard/*.ndi
/soundfonts/*.nsfb
/music/*.nms
/assets/boot/*.nvg
/assets/fonts/*
/assets/sid/*
```

## Hardware Runtime Commands

For normal hardware work, use the first-class `nova` commands:

```bash
nova device status --remote 192.168.1.65
nova drive list --remote 192.168.1.65
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.65
nova drive unmount fd0 --remote 192.168.1.65
nova wifi status --remote 192.168.1.65
nova wifi scan --remote 192.168.1.65
nova wifi set --remote 192.168.1.65 --ssid NovaLab --password secret --dhcp
nova audio status --remote 192.168.1.65
nova audio stop --remote 192.168.1.65
nova keyboard --remote 192.168.1.65
nova device reboot --remote 192.168.1.65
```

The raw NovaHost HTTP endpoints are intentionally not the user-facing workflow.
The CLI still exposes raw SD file operations as `nova ls`, `nova put`,
`nova get`, and `nova rm` for compatibility and diagnostics.

## Workflows

### Create And Boot A Floppy

```bash
nova create doc-demo.ndi --label DOCDEMO
nova import doc-demo.ndi AUTOBOOT.bas --tokenize
nova validate doc-demo.ndi
nova disk upload doc-demo.ndi --remote 192.168.1.65 --floppy --name doc-demo.ndi
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.65
```

After a cold start, `fd0` has first boot priority.

### Unmount A Floppy And Fall Back To Default Boot

```bash
nova drive unmount fd0 --remote 192.168.1.65
nova device reboot --remote 192.168.1.65
```

Because unmount clears the persisted path, the image will not come back on the
next boot unless it is mounted again.

### Create A Hard Disk Image

```bash
nova create hd0.ndi --hd --label HOME
nova disk upload hd0.ndi --remote 192.168.1.65 --hard --name hd0.ndi
nova drive mount hd0 /disks/hard/hd0.ndi --remote 192.168.1.65
```

### Round-Trip A BASIC Program

```bash
nova export doc-demo.ndi /AUTOBOOT.bas ./AUTOBOOT.txt --detokenize
# edit AUTOBOOT.txt
nova import doc-demo.ndi AUTOBOOT.txt / --tokenize
nova validate doc-demo.ndi
```

### Upload Music And A Soundfont

```bash
nova soundfont upload DocBank.nsfb --remote 192.168.1.65
nova music upload docs/programs/midi/sousa-stars-stripes.mid \
  --remote 192.168.1.65 --name STARS.NMS
```

On NovaBASIC, use the music commands (`SFLOAD`, `MIDPLAY`, `MIDSTOP`) to load
and play these files.

## Troubleshooting

`tokens.json not found`

: Run from the repository or pass `--tokens ehbasic/tokens.json`.

`Cannot combine --tokenize with MIDI music compilation`

: Import BASIC and MIDI as separate files. MIDI import always compiles to
  `.nms`.

`SF2 upload must convert to Nova-native .nsfb first`

: Upload an existing `.nsfb` bank. The SF2 converter is not implemented in this
  CLI.

`PUT ... 409 Conflict`

: NovaHost rejects writes to mounted `.ndi` files. Unmount the drive first.

`GET ... 404 Not Found`

: Check whether the path is relative to `/sd`, and whether a managed command is
  prepending a base directory.

`503 sd not mounted`

: NovaHost does not see the SD card. Check `/sd-status`, power, card seating,
  and FAT32 formatting.

`Not enough contiguous free sectors`

: NDI images allocate contiguous sectors. Delete/reimport churn can fragment an
  image. Create a fresh image or export/rebuild it.

`Directory is full`

: The image has a fixed directory table. Remove files/directories or extend the
  image format.

## Current Gaps

The implemented CLI does not yet have first-class verbs for:

- reloading ROMs after upload
- converting `.sf2` soundfonts to `.nsfb`

ROM reload is still handled by existing host-control tooling. Soundfont
conversion should become part of the CLI when the `.sf2` to `.nsfb` converter is
implemented.
