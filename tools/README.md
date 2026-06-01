# NovaHost Client

Use `novahost_client.py` for Python tools that need NovaHost HTTP/debug access.
Use `novahostctl.py` for shell or manual hardware operations.

Defaults target the static board IP:

```bash
tools/novahostctl.py state
tools/novahostctl.py screen
tools/novahostctl.py basic 'PRINT "HELLO"'
tools/novahostctl.py mute-sid
tools/novahostctl.py cold-start
tools/novahostctl.py vm-reset
e6502.Nova/bin/Release/net10.0/osx-arm64/nova webserver --remote 192.168.1.65
```

Override connection details with `--host`, `--port`, `--http-port`, or the
`NOVAHOST`, `NOVAHOST_PORT`, and `HTTP_PORT` environment variables.

`cold-start` matches a real cold boot through the debug control plane: NovaHost
reloads the default ROMs from SD, then releases the VM so mounted media can
autoboot. `vm-reset` is an HTTP management call to `POST /vm-reset` for the
standalone CLI path; it only resets the currently loaded runtime.

The Nova CLI webserver is the browser control center. It runs on the workstation
and talks to NovaHost through a long-lived TCP management connection on port
6504. Browser requests stay local HTTP/REST, while board traffic uses the
framed `NVH1` protocol with CBOR payloads and chunked raw upload data.

Runtime packages uploaded through the webserver can be `.zip`, `.nvr`, or
`.nrp` files. For two-ROM runtimes, include `nova-runtime.json`:

```json
{"name":"novalogo","rom":"novalogo.bin","extensionRom":"novalogo_ext.bin"}
```

If there is no manifest, one-ROM packages are accepted, and two-ROM packages
are accepted only when the extension ROM filename contains `ext` or
`extension`.

# Drive Mounts

Drive slots are logical pointers stored in `/config/boot.json`, not special
root filenames. Mounting persists the pointer; unmounting clears it. During
boot, NovaHost scans mounted floppies before hard disks (`fd0`..`fd3`, then
`hd0`..`hd1`) and clears stale configured pointers whose images no longer
exist.

```bash
curl -X POST http://192.168.1.65/drives/fd0/mount \
  -H 'Content-Type: application/json' \
  --data '{"path":"/games/novachess.ndi"}'
curl -X POST http://192.168.1.65/drives/fd0/unmount
curl http://192.168.1.65/drives
```

# NIC Hardware Smoke

`check-nic-hardware.py` verifies the live FPGA/NovaHost NIC path. By default it
only touches NIC test registers and restores them:

```bash
tools/check-nic-hardware.py
```

To also prove NovaHost is running the NIC dispatcher, use the Nova Chess-driven
event test. This cold-starts the VM and selects `N NEW GAME`, then `4 NETWORK
GAME`, so it changes the running app state:

```bash
tools/check-nic-hardware.py --exercise-novachess-network
```

To verify the FPGA payload DMA path on hardware, run the BASIC-driven DMA
smoke. It cold-starts BASIC, connects NovaHost to a local TCP peer, sends bytes
from CPU RAM, then receives bytes back into CPU RAM:

```bash
tools/check-nic-hardware.py --exercise-basic-dma
```

# Sprite Multiplex Hardware Smoke

`run-sprite-multiplex-hardware-smoke.py` builds the assembly sprite multiplex
demo, loads it at `$9000` through NovaHost, starts it with `SYS $9000`, and
verifies that all 16 sprite slots are enabled and moving. The demo displays 8
rows of 16 distinct sprites using copper-driven Y multiplexing.

```bash
tools/run-sprite-multiplex-hardware-smoke.py
```

# Overlay Packager

Fixed-address overlays are packaged by `e6502.Tools`:

```bash
dotnet run --project e6502.Tools -- pack-overlay \
  --input build/module.bin \
  --output build/MODULE.OVL \
  --load $7000 \
  --max-size $2000 \
  --sym build/module.sym \
  --main module_main
```

The packager writes the 32-byte `NOVO` header consumed by
`runtime/asm/overlay.s`, resolves entry symbols from ld65 `-Ln` files, and
validates that the payload and entry addresses fit the fixed overlay slot.

# Nova FTDI Keyboard

`nova-ftdi-keyboard.py` turns the host terminal into a keyboard bridge for the
ULX3S FTDI UART input. The current FPGA keyboard UART is `9600 8N1`.

```bash
tools/nova-ftdi-keyboard.py --list
tools/nova-ftdi-keyboard.py --port /dev/cu.usbserial-D01457
```

Type normally in the terminal. `Ctrl-]` exits the bridge; `Ctrl-C` is forwarded
to Nova unless `--ctrl-c-quits` is passed.

# score2bas

`score2bas.py` transpiles a music score into a NovaBASIC v1.0 program that uses `SOUND` and `VSYNC`.

## Supported Input
- `.mid` / `.midi` (direct)
- `.ly` (LilyPond source, requires `lilypond` installed to generate MIDI first)

## Quick Start
```bash
# MIDI -> BASIC (2 voices on MIDI channels 2 and 3)
tools/score2bas.py /path/to/piece.mid \
  -o docs/programs/piece.bas \
  --title "PIECE NAME" \
  --voices 2 \
  --channels 2,3 \
  --tempo-mode fixed \
  --bpm 120 \
  --max-line-len 68
```

## Tempo Modes
- `--tempo-mode fixed`: one BPM for entire piece (`--bpm` override optional).
- `--tempo-mode track`: emit MIDI tempo changes into DATA so BPM can change while playing.

Generated BASIC includes:
- `PP` (ticks per quarter note)
- `SCALE` (global speed percent, default `100`)
- `BPM` (active tempo)

Set `SCALE` lower for faster playback, higher for slower playback.

`--max-line-len` defaults to `68`. NovaBASIC input buffer is 222 bytes.

## LilyPond Input
```bash
tools/score2bas.py /path/to/piece.ly -o docs/programs/piece.bas
```
If `lilypond` is not installed, convert to MIDI externally and pass the `.mid` file.

## Current Output Model
- Up to 4 monophonic voices (`SOUND` channels `0-3`)
- Chords on one selected voice/channel are reduced by policy (`--chord-policy high|low|first`)
- Measure markers are emitted as `REM M<n>` near DATA blocks

This event stream is intentionally close to a future assembly player format.

## Invention 8 Example
```bash
tools/score2bas.py /tmp/bach-invention-08.mid \
  -o docs/programs/invention8.bas \
  --title "BACH INVENTION VIII" \
  --voices 2 \
  --channels 2,3 \
  --tempo-mode fixed \
  --bpm 90
```
