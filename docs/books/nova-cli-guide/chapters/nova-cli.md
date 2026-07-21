# Nova CLI User Guide

`nova` is the command-line tool for Nova disk images and NovaHost SD-card
management.

NDI is the disk image format. Nova is the CLI.

This guide is written for someone trying to get useful work done without
reverse-engineering the code. It covers the implemented commands, their
parameters, the paths they touch, and runnable examples for the supported CLI
surface.

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
nova disk upload doc-demo.ndi --remote 192.168.1.188 --floppy --name doc-demo.ndi
```

Mount it into `fd0`:

```bash
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.188
```

Check the board:

```bash
nova device status --remote 192.168.1.188
```

## Mental Model

Nova has three separate layers:

| Layer | What It Is | Managed By |
| --- | --- | --- |
| `nova` CLI | Host-side tool for images and NovaHost files | Your development machine |
| NDI | Disk image format for floppies and hard drives | `e6502.Storage` and NovaHost |
| NovaHost | ESP-side TCP command server and SD-card/device manager | The board |

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
terminal keypresses to the running NovaVM over WiFi. NovaHost speaks a
newline-delimited JSON command protocol over TCP; the CLI is the supported way
to drive it, and there is no HTTP/REST interface.

## Build The CLI

Development build:

```bash
dotnet build e6502.Nova/e6502.Nova.csproj -c Release
```

`dotnet build` is for fast local compilation. `dotnet publish` for the Nova CLI
defaults to NativeAOT and produces the binary users should run.

Publish a single NativeAOT binary for the current machine:

```bash
nova publish
```

Publish for a specific runtime:

```bash
nova publish osx-arm64
nova publish linux-x64
nova publish linux-arm64
nova publish win-x64
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

Remote commands talk to NovaHost over TCP.

`--remote` may appear before or after the command:

```bash
nova --remote 192.168.1.188 disk list
nova disk list --remote 192.168.1.188
nova disk list --remote=192.168.1.188
```

The host may include a port when testing against a local server:

```bash
nova device status --remote 127.0.0.1:8080
```

Remote paths are relative to the NovaHost SD-card root. These are equivalent:

```bash
nova get --remote 192.168.1.188 /config/boot.json
nova get --remote 192.168.1.188 config/boot.json
```

Backslashes are normalized to `/`. Leading slashes are stripped before building
the SD-card path.

## File Types

The CLI infers local NDI file type from extension:

| Extension | NDI Type | Notes |
| --- | --- | --- |
| `.bas` | `BAS` | Tokenized when imported with `--tokenize` |
| `.pas` | `PASCAL` | NovaPascal source text |
| `.npp` | `PASCAL PROJECT` | NovaPascal project/build manifest |
| `.logo`, `.lgo` | `LOGO` | NovaLogo source text |
| `.s`, `.asm`, `.inc` | `ASM` | Nova assembly source text |
| `.sid` | `SID` | SID music file |
| `.bin` | `BIN` | Binary program/data |
| `.mid`, `.midi` | `MID` | Compiled to `.nms` first |
| `.nms` | `MID` | Nova Music Stream |
| `.nvg` | `GFX` | Nova graphics asset |
| `.4th`, `.fth`, `.fs` | `FTH` | Forth source text |
| unknown | `BIN` | Stored as binary data |

`GFX` is the internal NDI file type name for graphics. User-facing graphics
assets should use `.nvg`.

## NovaPascal Development Disk

Build the language-neutral assembler/linker and the bootable NovaPascal disk,
then inspect it with the normal Nova CLI:

```bash
make -C software/toolchain all
make -C software/languages/novapascal novapascal.ndi
nova validate software/languages/novapascal/novapascal.ndi
nova dir software/languages/novapascal/novapascal.ndi
```

The disk boots to `NovaPascal Shell v1.0`. `NEW HelloWorld` creates a
`HELLOWORLD` project directory containing `MAIN.PAS` and the single end-to-end
`HELLOWORLD.NPP` configuration for NPC, NAS, and NL. `ADDUNIT HELLOWORLD
GREETER` creates `GREETER.PAS` with a Pascal `interface`/`implementation`
skeleton and adds it to the NPP 2 manifest; `DELUNIT HELLOWORLD GREETER`
removes it. `DELPROJECT HELLOWORLD` validates and removes the project, but
refuses directories containing nested directories.

`BUILD HELLOWORLD` enters the project, validates its complete manifest and
linker configuration, compiles `MAIN.PAS` and every repeated `UNIT` source,
then runs disk-loaded NPO2, NAS, and NL. Unit streams are combined in manifest
order with the program entry point, so O2 optimizes and dead-strips across the
whole project before NAS writes one object and NL links `PASCAL.NLIB`. Generated
per-source `.ASM` and combined `.S` assembly, object, map, labels, and
load-address-prefixed binary stay in the project directory. `RUN HELLOWORLD`
executes that result. Direct `BUILD file.pas`, `BUILD file.npp`, and `RUN
file.bin` remain available for compatible flat-file workflows.

Project source uses normal Pascal syntax: the program selects project units
with `uses`, and each unit separates public declarations from matching
implementations. The current source-unit ABI supports public parameterless
procedures and parameterless `Byte`/`Boolean` functions. NPP 2 accepts up to 16
unit files; NPP 1 remains a supported single-main compatibility format. No
source includes are required, and project units do not create a redundant
persistent interface artifact—the compiler emits checked signatures into the
whole-project assembly stream.

Small 65C02 fragments may be written as a line-oriented Pascal `asm` ... `end`
statement using ordinary NAS syntax. NPC copies the block into generated
assembly, and NPO2 treats it as an opaque register-and-memory barrier. O2 does
not inline routines containing assembly, preserves routines referenced by its
case-insensitive NAS symbols, uses stack-backed function results, and rebuilds
cached array addresses afterward. Larger assembly implementations continue to
use the generated unit bundle (`.PAS`, `.NPI`, `.INC`, and `.S`) so Pascal sees
a native contract backed by canonical NDK source.

NPO2 performs six typed streaming passes: local dataflow optimization;
iterative single-call leaf/caller inlining with dead-routine removal; 65C02
instruction selection; and machine peepholes. O2 includes direct comparison
branches, byte/word self-update reduction, effect-safe accumulator and
repeated-load forwarding, call-free function-result folding, inline word/array
operations, and formation of one relocatable base for compatible
constant-offset array accesses. The final `.S` is ordinary readable assembly;
NAS and NL contain no Pascal-specific logic.
NPC, NPO2, NPEDIT, and NAS obtain transient XRAM through the NDK Memory module
and release every allocation on success or failure. NAS and NL load, enter, and
unload their `$7000` workers only through the NDK System overlay API; the tools
do not reserve private XRAM blocks or implement private overlay loaders.
Pascal `uses NovaGraphics;` exposes native Pascal graphics
procedures while its precompiled unit adapter alone handles the canonical VGC
NDK parameter layout and command protocol. `uses NovaInput;` provides
nonblocking `PollKey()` input without exposing MMIO. `uses NovaRandom;` similarly exposes
`RandomByte(): Byte` while its adapter reuses the canonical RNG and FIO NDK
sources. `uses NovaRng, NovaFio;` remains the lower-level binding path for those
canonical declarations and implementations. `Byte` variables live in zero-fill `BSS`,
`status := rng_get8();` receives A, `fio_issue(status);` passes A, and named
pseudo-register assignments remain symbolic through NPC, NAS, and NL.
Generated `.NPI` bindings validate byte call signatures, while
`Byte(FIO_CMD_RNG)` uses a generated canonical byte constant. `EDIT` opens text
files and `Alt-X` or `Ctrl-Q` returns to the shell. The language-neutral Editor
module keeps large documents in XRAM and pages complete-line windows through
lower RAM as navigation crosses them; NPEDIT only supplies disk I/O and the
file-type label.
The disk also includes FizzBuzz and Conway's Game of Life projects. FizzBuzz
uses nested structured statements, byte expressions, `mod`, comparisons,
`while`, `if`/`else`, and numeric `writeln`. Life adds zero-based
Boolean arrays, unsigned 16-bit `Word` values and indices, checked
multi-argument unit procedure calls, parameterless procedures, and
parameterless `Byte`/`Boolean` functions. It seeds a random, roughly half-full
80-by-25 board, applies B3/S23 simultaneously through two arrays, demonstrates
inline NAS in its direct 2,000-byte `Commit` copy, and draws a full-screen,
vsync-paced differential display on Nova's native 320-by-200 graphics plane
until Enter is pressed. Functions return through assignment to their own
name; NPC keeps that result on the 65C02 stack so nested calls do not share a
global result byte. NPC reports syntax failures with the source filename, line,
and column. String `writeln` keeps literals of at most two characters as direct
writes. Longer literals compile to a single `JSR I_P_WRITE_LINE` followed by
zero-terminated inline bytes. Nova reserves the `I_` prefix for routines whose
immutable parameters immediately follow the call; the callee advances the
saved return address over those parameters.
See `software/languages/novapascal/README.md` for the NPP format and complete
native toolchain surface.

## Local Conversion Commands

Build helpers that used to live as standalone scripts are exposed through
`nova convert`.

Convert one 16-bit hex value per line into a little-endian binary file:

```bash
nova convert hex16-to-bin e6502.FPGA/rom/f6581_curve.hex build/f6581_curve.bin
```

Embed a binary file as a C header:

```bash
nova convert bin-header software/languages/ehbasic/basic.bin build/basic_rom.h BASIC_ROM
```

## Code Generation Commands

Generated build artifacts are produced through `nova codegen`.

```bash
nova codegen tokens software/languages/ehbasic/basic.asm -o software/languages/ehbasic/tokens.json
nova codegen novavm-inc e6502.Avalonia/Hardware/VgcConstants.cs software/languages/ehbasic/basic.sym -o software/runtime/asm/novavm.inc
nova codegen runtime-abi <sources...> --sym software/languages/ehbasic/basic.sym --json software/languages/ehbasic/runtime_labels.json --md docs/assembly/runtime-labels.md --asm software/runtime/asm/runtime_labels.inc
nova codegen ndk-reference --runtime-dir software/runtime/asm --tex docs/books/ndk-reference/generated/library-reference.tex --json docs/books/ndk-reference/generated/ndk-api.json [--pascal-dir build/ndk-pascal]
```

Optional `--pascal-dir` writes a complete generated Pascal unit bundle for every
NDK library. Each bundle contains a human-readable `.PAS` contract, guarded
`.NPI` byte-ABI checks, a declaration `.INC`, and an implementation `.S` composed
from every canonical source containing Pascal-callable entries. Underscores are
removed from generated disk stems, so the `vgc_vsync` library becomes the native
Pascal unit `NovaVgcVsync` and files `VGCVSYNC.*`. The generator rewrites only
include filenames; routine bodies and hardware declarations remain the canonical
NDK source. Canonical declaration includes also drive implementation dependencies,
which are appended after callers so source-level dead stripping sees live edges
without a hand-maintained dependency table or order-sensitive `USES` clause.
`.NPI` files encode byte constants, byte storage, and supported
A-register routine signatures directly from annotations. Inline-parameter and
wider register routines remain assembly-callable but are omitted from the typed
Pascal contract and implementation facade until NovaPascal can express their ABI.

Local browser/demo assets are also built through `nova`, not standalone helper
scripts:

```sh
nova build browser-rust-core
nova docs showcase-demo
nova docs fun-n-games
```

`nova build browser-rust-core` builds the browser WASM core and copies the ROM,
font, and module assets into `e6502.Browser/wwwroot/rust`. `nova docs
showcase-demo` rebuilds `docs/programs/demo.ndi` from the SuperNova music browser,
its linked keyboard visualizer, and the curated SID/MIDI sources. The disk has one
`AUTOBOOT.bin`; it does not carry the obsolete standalone `KEYBOARD.bin` payload.
The command also refreshes the checked-in browser copies under
`e6502.Browser/wwwroot/showcase/` and `website/emulator/showcase/`. The curated
catalog includes the fan-ranked SID classics already stored in the project and
the project-owner-supplied orchestral MIDIs of Sousa's *The Washington Post*
and *Semper Fidelis*; their provenance note is packaged as `/wts/CREDITS.txt`.

## CI Maintenance Commands

CI bootstrap behavior is also owned by the Nova CLI:

```sh
nova ci install-linux-deps build
nova ci install-linux-deps release
nova ci install-macos-cc65
nova ci mint-github-token
```

`nova ci install-linux-deps` installs the pinned Linux build or release package
set used by Woodpecker, including a w65c02-capable cc65 when the system `ca65`
is too old. `nova ci install-macos-cc65` self-provisions the same pinned cc65
on the macOS local runner. `nova ci mint-github-token` mints the short-lived
GitHub App installation token used by CI upload and private dependency fetch
steps.

The Makefiles call these commands directly and list the `nova` CLI sources as
prerequisites, so generator changes force regenerated outputs.

The script inventory is intentionally tiny and documented in
`docs/script-inventory.md`; repo operations should be `nova` commands, not
new shell or Python files.

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

If `/path` is omitted, the root directory is listed. Pascal `.pas` files are
shown as `PASCAL SOURCE`; `.npp` project manifests are shown as `PASCAL PROJECT`.

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

This queries NovaHost's health and SD-card mount status over the TCP command
protocol.

Tested example:

```bash
nova device status --remote 192.168.1.188
```

Use this first when the board is behaving oddly.

### `device reboot`

Reboot NovaHost.

```bash
nova device reboot --remote <host>
```

Tested example:

```bash
nova device reboot --remote 192.168.1.188
```

On the Arty Linux host this asks the kernel to sync and reboot. On ESP-side
NovaHost targets it reboots the host controller. It is not the same thing as a
6502 warm start or cold start.

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
nova drive list --remote 192.168.1.188
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
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.188
nova drive mount hd0 /disks/hard/hd0.ndi --remote 192.168.1.188
```

### `drive unmount`

Unmount a drive slot and clear its persisted mount path.

```bash
nova drive unmount <slot> --remote <host>
```

Tested example:

```bash
nova drive unmount fd0 --remote 192.168.1.188
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
nova wifi status --remote 192.168.1.188
```

### `wifi scan`

List nearby WiFi networks visible to NovaHost.

```bash
nova wifi scan --remote 192.168.1.188
```

### `wifi set`

Update WiFi configuration.

DHCP example:

```bash
nova wifi set --remote 192.168.1.188 --ssid NovaLab --password secret --dhcp
```

Static IP example:

```bash
nova wifi set --remote 192.168.1.188 --ssid NovaLab --password secret \
  --static --static-ip 192.168.1.188 --gateway 192.168.1.1 \
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
nova wifi connect --remote 192.168.1.188
nova wifi disconnect --remote 192.168.1.188
nova wifi reconnect --remote 192.168.1.188
nova wifi forget --remote 192.168.1.188
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
nova audio status --remote 192.168.1.188
nova audio stop --remote 192.168.1.188
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
nova keyboard --remote 192.168.1.188
```

Control keys:

- `Ctrl-]` exits the local bridge.
- `Ctrl-C` is sent to Nova as byte `$03`; pass `--ctrl-c-quits` to make it
  exit the bridge instead.
- Escape and F1 also send Nova's `$03` break byte.
- Enter, Backspace, Tab, Delete, printable ASCII, and arrow keys are forwarded.
  Arrow keys use the same `$1C` through `$1F` bytes as the desktop emulator;
  the active Nova editor or program decides what those bytes mean.

This command uses TCP port `6503`, not NovaHost's standard command port. Use
`--port` only when the debug service is exposed on a non-default port.

## Emulator & VM Debug Commands

`nova vm` (alias `nova emulator`) drives a running NovaVM through its debug TCP
service: read/write CPU memory and video RAM, read the text screen, inject keys,
and control execution. It targets the desktop Avalonia emulator by default and a
hardware board with `--remote`.

```bash
nova vm [--remote <host>] [--port <port>] <command> [args] [--json]
```

Connection defaults:

- No `--remote`: `127.0.0.1:6502` (the Avalonia emulator's debug port).
- With `--remote <host>`: debug commands use `<host>:6503`; `reset` and
  `cold-start` go through the NovaHost management service.

Override the debug port with `--port`. Add `--json` to print the raw JSON reply
instead of the formatted result.

### Execution control

```bash
nova vm cold-start [--runtime basic|logo|forth] [--text <text>] [--no-wait]
nova vm reset [--text <text>] [--no-wait]
nova vm wait [text] [--timeout-ms <ms>]
nova vm run-cycles <count>
```

- `cold-start` performs a full cold boot, optionally selecting the language
  runtime, typing `--text` once ready, and (unless `--no-wait`) blocking until
  the `READY.` prompt returns.
- `reset` pulses the CPU reset line and reboots to the current runtime.
- `wait` blocks until the screen shows `text` (or the READY prompt) or the
  timeout elapses.
- `run-cycles` steps the CPU by a fixed cycle count (emulator only).

### Screen & cursor

```bash
nova vm screen [--json]
nova vm line <row>
nova vm cursor
```

- `screen` prints the 80×50 text screen, one line per row.
- `line` prints a single screen row.
- `cursor` prints the current cursor position.

### Keyboard injection

```bash
nova vm type-text <text>
nova vm enter <text>
nova vm key <key>
```

- `type-text` streams a string into the key queue (`\n` becomes carriage
  return).
- `enter` types the text and appends a carriage return.
- `key` sends one named key (`ENTER`, `BACKSPACE`, `TAB`, `ESC`, `SPACE`,
  arrows `LEFT`/`RIGHT`/`UP`/`DOWN`, `HOME`, `END`, `DELETE`,
  `CTRL-A` through `CTRL-Z`, `ALT-A` through `ALT-Z`, or a single character).

### Editor demo smoke test

The live editor smoke test uses the same debug protocol as `nova vm`, so it can
target the Avalonia emulator or the Arty hardware. Start the standalone editor
demo first, then run:

```bash
NOVA_EDITOR_SMOKE_HOST=127.0.0.1 \
  dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --no-build \
  --filter EditorRemoteSmokeTests
```

For Arty, point the test at the board:

```bash
NOVA_EDITOR_SMOKE_HOST=192.168.1.188 \
  dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --no-build \
  --filter EditorRemoteSmokeTests
```

`NOVA_EDITOR_SMOKE_PORT` overrides the debug port. Without it, loopback targets
use `6502` and remote targets use `6503`.

### Memory & video RAM

```bash
nova vm peek <addr>
nova vm peek-block <addr> <count> [--json]
nova vm poke <addr> <value>
nova vm read-vram <space> <addr> <length> [--json]
nova vm fill-vram <space> <addr> <value> <length>
```

Addresses and values accept decimal, `$hex`, or `0xhex`. `peek` prints the byte
as `$XX`. The VGC `space` selects the video-memory plane: `1`=char, `2`=color,
`3`=gfx, `4`=sprite, `7`=text-attr.

### Debugger (emulator only)

```bash
nova vm state|pause|resume|step|break-list|break-clear-all
```

These map to the Avalonia debugger and are not available on hardware.

### Raw passthrough

```bash
nova vm raw '<json>' [--json] [--allow-error]
```

Sends a hand-written debug command (the JSON must include a `command` field).
`--allow-error` keeps a non-OK reply from being treated as a CLI failure.

Example session:

```bash
nova vm --remote 192.168.1.188 cold-start --runtime basic
nova vm --remote 192.168.1.188 enter 'PRINT "HELLO"'
nova vm --remote 192.168.1.188 screen
nova vm --remote 192.168.1.188 peek $A0ED
```

## Raw Remote SD Commands

Raw remote commands map directly to NovaHost SD-card paths.

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
nova ls --remote 192.168.1.188
nova ls --remote 192.168.1.188 disks/floppy
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
nova put --remote 192.168.1.188 doc-demo.ndi disks/floppy/doc-demo.ndi
nova put --remote 192.168.1.188 docs/programs/midi/sousa-stars-stripes.mid music/STARS.NMS
```

### `get`

Download an SD file.

```bash
nova get --remote <host> <remote-path> [local-path]
```

Tested example:

```bash
nova get --remote 192.168.1.188 config/boot.json ./boot.json
```

### `rm`

Delete an SD file or empty directory.

```bash
nova rm --remote <host> <remote-path>
```

Tested example:

```bash
nova rm --remote 192.168.1.188 tmp/old.bin
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
nova disk list --remote 192.168.1.188
nova disk list --remote 192.168.1.188 --floppy
nova disk upload doc-demo.ndi --remote 192.168.1.188 --floppy --name doc-demo.ndi
nova disk download doc-demo.ndi --remote 192.168.1.188 --floppy ./doc-demo.downloaded.ndi
nova disk delete doc-demo.ndi --remote 192.168.1.188 --floppy
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
nova rom upload ehbasic/basic.bin --remote 192.168.1.188 --name novabasic.bin
nova rom list --remote 192.168.1.188
nova rom download novabasic.bin --remote 192.168.1.188 ./novabasic.downloaded.bin
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
nova soundfont upload DocBank.nsfb --remote 192.168.1.188
nova soundfont list --remote 192.168.1.188
nova soundfont download DocBank.nsfb --remote 192.168.1.188 ./DocBank.downloaded.nsfb
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
nova music upload docs/programs/midi/sousa-stars-stripes.mid --remote 192.168.1.188 --name STARS.NMS
nova music list --remote 192.168.1.188
nova music download STARS.NMS --remote 192.168.1.188 ./STARS.downloaded.NMS
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
  --remote 192.168.1.188 --type boot

nova asset list --remote 192.168.1.188 --type boot
nova asset download novavm_logo.nvg --remote 192.168.1.188 --type boot ./novavm_logo.downloaded.nvg
```

### Library Modules

`nova module` manages NovaVM library modules (`.nmod` / `.mod` — the resident
overlay images loaded by `LIB` and the extension commands). On a board they live
under `/lib`.

```bash
nova module info <file|name> [--remote <host>]
nova module validate <file>
nova module ls --remote <host>
nova module put <file> [name] --remote <host>
nova module get <name> [local] --remote <host>
nova module rm <name> --remote <host>
nova module pack --src <module.s> --bin <module.bin> --out <module.nmod> [--syms <file>] [--ndk-dir <dir>]
```

- `info` shows a module's metadata, either from a local file or by reading it
  back from the board.
- `validate` checks a local module image.
- `ls` lists the modules installed on the board's `/lib`.
- `put` uploads a module (the on-board name defaults from the file name).
- `get` downloads a module to a local file.
- `rm` deletes a module from the board.
- `pack` writes a self-documenting `.nmod` from a 16 KB module image and `;@`
  annotations in the module source.

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

## Hardware Command Quick Reference

For normal hardware work, use the first-class `nova` commands:

```bash
nova device status --remote 192.168.1.188
nova drive list --remote 192.168.1.188
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.188
nova drive unmount fd0 --remote 192.168.1.188
nova wifi status --remote 192.168.1.188
nova wifi scan --remote 192.168.1.188
nova wifi set --remote 192.168.1.188 --ssid NovaLab --password secret --dhcp
nova audio status --remote 192.168.1.188
nova audio stop --remote 192.168.1.188
nova keyboard --remote 192.168.1.188
nova device reboot --remote 192.168.1.188
nova capture screen screenshots/hardware/current.png --remote 192.168.1.188
nova capture record /tmp/novavm.mkv --remote 192.168.1.188 --duration 30
nova capture devices --remote 192.168.1.188
nova capture gadget enable --remote 192.168.1.188
nova capture hdmi screenshots/hardware/current.png
nova check spi-bridge --remote 192.168.1.188
nova check vgc-reset-stale --remote 192.168.1.188
nova fpga check-timing e6502.FPGA/boards/ulx3s/build/nextpnr-report.json
nova fpga vivado-utilization e6502.FPGA/boards/arty_z7/build/ps_full/ps_full.runs/synth_1/arty_z7_full.dcp --out /tmp/arty-hier-util.rpt
```

`nova capture screen` uses the NovaVM V4L2 device (`/dev/video0`) and captures
the post-OSD HDMI image. `nova capture record` records V4L2 video plus ALSA
audio (`hw:NovaVM,0`); when `--remote` is used, the recording is copied back to
the requested local output path. `nova capture gadget` configures the standard
UVC/UAC2 gadget endpoints for capture-card mode; verify streaming on hardware
with `nova capture devices` and standard Linux capture tools.

The native V4L2 pixel format is `bgr0`, matching the PL capture buffer byte
order. Use `--format` only when a target tool needs a different ffmpeg input
format.

The raw NovaHost SD commands are intentionally not the primary user-facing
workflow. The CLI still exposes raw SD file operations as `nova ls`, `nova put`,
`nova get`, and `nova rm` for compatibility and diagnostics.

Use `nova fpga vivado-utilization` when an Arty Vivado build fails placement.
It opens the given checkpoint with Vivado, writes a hierarchical utilization
report, and removes its generated Tcl helper before returning.

Arty Z7 board workflows also live under `nova arty`:

```bash
nova arty sync-payloads
nova arty build-linux-host
nova arty build-linux-image
nova arty deploy-boot-image --remote 192.168.1.188
nova arty build-ps-fio
nova arty deploy-editor-demo --remote 192.168.1.188
nova arty upload-infocom --remote 192.168.1.188 --infocom-root /mnt/Software/Emulation/Infocom
nova arty make-boot-bin
```

`nova arty make-boot-bin` packages the FSBL, bitstream, and PS FIO ELF into
`e6502.FPGA/boards/arty_z7/build/BOOT.bin` after refreshing embedded 6502
payloads. It rebuilds the PS FIO app when the ELF is missing or older than the
current hardware handoff, Vitis hook, or PS sources. It refuses to package an
existing `.bit` file when the Vivado implementation run directory contains
failure markers, because that means the file can be stale.

`nova arty build-linux-image` rebuilds the PetaLinux image from the current repo
layer and Arty XSA, then packages the Linux `BOOT.BIN`. This is the path that
installs kernel/device-tree/rootfs features such as the NovaVM V4L2/ALSA capture
device. It uses the same Vivado failure-marker check before `petalinux-package`
so the Linux boot image cannot accidentally embed an older bitstream after a
failed implementation. `nova arty deploy-boot-image --remote 192.168.1.188` copies the rebuilt
`image.ub` and Linux `BOOT.BIN` to `/boot`, stages `rootfs.ext4` under `/data`,
writes it to `/dev/mmcblk0p2`, verifies the rootfs bytes by SHA-256 before
reboot, forces a kernel sync/reboot, verifies boot files, and waits for NovaVM. The
rootfs path requires `/data` to be a separate staging filesystem. Use
`--boot-only` only when the rootfs is already known to match the build; combine
it with `--no-reboot` only when you intentionally want to defer a `/boot`-only
reboot.

`nova arty deploy-editor-demo` packages the editor demo as a bootable
`editor-demo.ndi`, uploads it to `/data/nova/disks/floppy`, clears mounted
drive slots, and mounts it as `fd0` before restarting NovaVM. The deploy path
also passes the current `nova` binary into nested Makefiles as `NOVA_CLI`,
avoiding stale or environment-specific `dotnet run` calls while refreshing
payloads. After installing the Linux host, the command restarts NovaVM and
waits for the management port before returning.

`nova arty deploy-linux-host` places bootable language disks under
`/data/nova/disks/languages`, currently `novalogo.ndi` and `novaforth.ndi`.
`novapascal.ndi` is currently built and validated locally but is not yet added
to this deployment command. The command also rebuilds the
SuperNova showcase, uploads it as `/data/nova/disks/demos/supernova.ndi`, and
verifies that remote image against the local SHA-256 before restarting NovaHost.

`nova arty upload-infocom` builds the known NovaZ Infocom project images with
the existing `software/examples/novaz` Makefile and uploads them under
`/data/nova/disks/infocom` so the Arty OSD disk picker can browse them as
`/disks/infocom/<project>.ndi`.

## Runtime Configuration Commands

`nova runtime` manages the language runtimes (a ROM plus an optional extension
ROM) the board can boot, recorded in `/config/boot.json`.

```bash
nova runtime list --remote <host>
nova runtime status --remote <host>
nova runtime set <name> --remote <host>
nova runtime add <name> --rom <sd-path> [--ext <sd-path>] --remote <host>
nova runtime remove <name> --remote <host>
nova runtime deploy <name> --rom <local-file> [--ext <local-file>] --remote <host>
nova runtime boot-floppy <name> [remote-path] --remote <host>
```

- `list` shows the configured runtimes; `status` shows the active one.
- `set` makes an already-configured runtime the default boot target.
- `add` registers a runtime that points at ROM paths already on the SD card;
  `remove` deletes the entry.
- `deploy` uploads local ROM file(s) and registers/activates the runtime in one
  step — the usual way to install a new language.
- `boot-floppy` configures the named runtime to boot from a floppy image.

## Local Web Admin Server

`nova webserver` (alias `nova web`) runs a local web UI that proxies to a
board's NovaHost management service — the browser-based equivalent of the
`device`, `drive`, `wifi`, and `disk` commands.

```bash
nova webserver --remote <host> [--port 8080] [--bind 127.0.0.1] [--no-open]
```

- `--port` sets the local HTTP port (default `8080`).
- `--bind` sets the local interface (default `127.0.0.1`).
- `--no-open` skips auto-opening the system browser.

## Workflows

### Create And Boot A Floppy

```bash
nova create doc-demo.ndi --label DOCDEMO
nova import doc-demo.ndi AUTOBOOT.bas --tokenize
nova validate doc-demo.ndi
nova disk upload doc-demo.ndi --remote 192.168.1.188 --floppy --name doc-demo.ndi
nova drive mount fd0 /disks/floppy/doc-demo.ndi --remote 192.168.1.188
```

After a cold start, `fd0` has first boot priority.

### Unmount A Floppy And Fall Back To Default Boot

```bash
nova drive unmount fd0 --remote 192.168.1.188
nova device reboot --remote 192.168.1.188
```

Because unmount clears the persisted path, the image will not come back on the
next boot unless it is mounted again.

### Create A Hard Disk Image

```bash
nova create hd0.ndi --hd --label HOME
nova disk upload hd0.ndi --remote 192.168.1.188 --hard --name hd0.ndi
nova drive mount hd0 /disks/hard/hd0.ndi --remote 192.168.1.188
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
nova soundfont upload DocBank.nsfb --remote 192.168.1.188
nova music upload docs/programs/midi/sousa-stars-stripes.mid \
  --remote 192.168.1.188 --name STARS.NMS
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

: Check whether the path is relative to the SD-card root, and whether a managed
  command is prepending a base directory.

`503 sd not mounted`

: NovaHost does not see the SD card. Check `nova device status`, power, card
  seating, and FAT32 formatting.

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
