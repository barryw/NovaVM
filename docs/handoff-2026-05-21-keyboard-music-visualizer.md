# Session handoff - 2026-05-21 keyboard music visualizer

## Current goal

Build a reusable Nova music keyboard visualizer that can run on real hardware
while SID or MIDI/WTS music is playing.

The intended user flow is:

1. Start playback with `MIDPLAY` or `SIDPLAY`.
2. Load `KEYBOARD.BIN` at `$9000`.
3. Run `SYS $9000`.
4. The visualizer owns the screen until the song ends or the user presses
   `ESC`/`Q`.
5. It displays song metadata, source type, soundfont/SID details, a progress
   bar, and live note highlights.
6. It exits cleanly back to BASIC without stuck notes or stale text.

The hardware target used during this session was NovaHost at `192.168.1.65`.
At the user's request, audio was stopped and a cold start was issued before
ending the session:

```bash
curl -fsS -X POST http://192.168.1.65/audio-stop
tools/novahostctl.py --host 192.168.1.65 --timeout 20 cold-start --no-wait
```

## Why the NCC prompt appeared

The prompt:

```text
THIS WILL CLEAR THE BASIC PROGRAM
ARE YOU SURE? (Y/N)
```

was not produced by the keyboard visualizer.

It came from the old NCC handler in the BASIC extension ROM. Extension command
slot `0` still dispatched to `EXT_NCC`, which printed that prompt and waited
for input. When the CPU escaped the visualizer path and fell back into ROM/BASIC
code, the text plane was written over whatever graphics were still visible from
the visualizer. That is why the prompt appeared on top of the keyboard screen.

The prompt was a symptom. The deeper bug is that the combined
`MIDPLAY` plus `SYS $9000` visualizer flow is not reliably keeping control in
the visualizer loop.

## Changes made

### NCC BASIC hook removal

The NCC BASIC command path was removed/reserved. NCC should load as a separate
runtime, not as a BASIC command or BASIC-screen prompt.

Files changed:

- `ehbasic/extension.s`
  - Extension command slot `0` now maps to `EXT_UNSUPPORTED`.
  - Removed `EXT_NCC`.
  - Removed the `THIS WILL CLEAR...` / `ARE YOU SURE...` prompt strings.
- `ehbasic/basic.asm`
  - Renamed `XTK_NCC` to `XTK_RESERVED47`.
  - Dispatch comment now marks `$47` as reserved.
- `runtime/asm/nova.inc`
  - Replaced exposed `EXT_CMD_NCC` with `EXT_CMD_RESERVED0`.
- `e6502.Avalonia/MainWindow.axaml.cs`
  - Removed the desktop NCC confirmation prompt that wrote to the BASIC screen.
  - Ctrl/Command+N now activates the NCC editor directly.
- `e6502.Avalonia/Hardware/VgcConstants.cs`
  - Removed stale "from BASIC NCC command" wording from `RomSwapNccEdit`.

Build/upload steps already run:

```bash
make -C ehbasic
make -C e6502.FPGA hex
make -C e6502.ESP32/novahost sd-assets
curl -fsS -X PUT --data-binary @e6502.ESP32/novahost/build/sd/roms/novabasic.bin http://192.168.1.65/sd/roms/novabasic.bin
curl -fsS -X PUT --data-binary @e6502.ESP32/novahost/build/sd/roms/extension.bin http://192.168.1.65/sd/roms/extension.bin
tools/novahostctl.py --host 192.168.1.65 --timeout 30 reload-rom
tools/novahostctl.py --host 192.168.1.65 --timeout 20 cold-start --no-wait
```

Verification already done:

```bash
rg -n "THIS WILL CLEAR|ARE YOU SURE|EXT_CMD_NCC|XTK_NCC|NCC confirmation|BASIC NCC command|EXT_NCC" ehbasic runtime e6502.Avalonia assembly ndk docs
```

After rebuild, only historical planning/handoff docs referenced old NCC text or
token names. Active source did not.

### SPI/WTS transfer tuning

`e6502.ESP32/novahost/fpga_bridge.h` was changed earlier in this work to keep
unpaced SPI writes below the FPGA SPI slave FIFO depth:

- `SDRAM_STREAM_MAX_BYTES = 256`
- `WTS_EVENT_STREAM_MAX_BYTES = 60`

Reason: the FPGA SPI slave cannot backpressure MOSI. Large write transactions
can overrun/desynchronize the bridge or starve WTS/event delivery. The smaller
WTS batch is not the final "ludicrous speed" design, but it made the current
MIDI path less likely to drop out while debugging.

## What currently works

- The keyboard test drawing path had reached a good visual state:
  - clean dark-gray key boundaries
  - black keys visible
  - white keys paint/highlight correctly
  - colored voice blocks render without paint leaks
  - centered title looked correct in the latest accepted screenshot
- `make -C ehbasic` passes.
- Updated runtime ROMs can be uploaded to hardware and reloaded from SD.
- The old NCC prompt source has been removed from active runtime source.
- `KEYBOARD.BIN` loads at `$9000`; `peek 0x9000` returned `$20`, matching the
  `JSR` opcode at the start of the binary after its two-byte load header.

## What is broken

The combined playback plus visualizer flow is broken.

Observed behavior:

- After cold start/autoboot, the machine ended up at the BASIC `Ready` prompt.
- The visualizer did not remain in control.
- `audio-status` reported MIDI state, but the user heard a single stuck tone,
  not real music playback.
- Manual `SYS $9000` after the failure did not recover the visualizer flow.
- The last CPU state seen after manual `SYS $9000` was not in the `$9000`
  visualizer code path; it was in low memory/BASIC state.

Do not treat `midiPlaying:true` alone as proof of success. For music playback,
verify all of the following:

- the user hears real music, not a stuck tone
- `musicEventRead` advances over time
- no note stays stuck after stop
- CPU remains in the intended app loop while the visualizer is active

## Important pieces

### `AUTOBOOT.bas`

Current intended launcher:

```basic
10 LOAD "KEYBOARD.BIN"
20 MIDPLAY "STARS.NMS"
30 SYS $9000
40 END
```

This should be stored in the NDI as a tokenized BASIC file with a `$0301` load
address. ASCII source is fine on the host side, but the disk entry must be
tokenized.

### `KEYBOARD.BIN`

Assembly app:

- source: `assembly/apps/keyboard/keyboard.s`
- linker config: `assembly/apps/keyboard/keyboard.cfg`
- load address: `$9000`
- entry: `$9000`
- invocation: `SYS $9000`

It saves VGC state, clears graphics/text, draws the keyboard, draws metadata,
then loops on frame updates until explicit exit or debounced end-of-music.

### Music status block

The visualizer reads hosted music state from the `$BA50` area:

- `MusicStatus` at `$BA50`
- note registers starting at `$BA51`
- metadata/progress registers in the same status region

For MIDI/WTS playback, it should show 8 WTS voices. For SID playback, it should
show 6 SID voices. The source should be inferred from the shared music metadata
ABI rather than separate SID/MIDI-specific ABIs.

### ESP-hosted playback

The 6502 side should remain thin. The ESP side owns the heavy work:

- parse/load soundfonts
- parse MIDI or pre-parsed Nova music streams
- execute SID files with the ESP-side 6502 SID runner
- stream register/event writes to the FPGA
- publish metadata/status to the Nova memory/register ABI

## Likely failure area

The next session should not start by changing drawing code.

The likely problem is at the control-flow/runtime boundary:

- tokenized `AUTOBOOT.bas`
- `LOAD "KEYBOARD.BIN"` load semantics
- `MIDPLAY` interaction with BASIC execution
- `SYS $9000` entry and return behavior
- zero-page/stack preservation in the visualizer
- visualizer calls into BASIC ROM helpers such as `V_INPT`
- hosted music status transitions causing premature visualizer exit

The bug needs to be isolated with small harnesses before more feature work.

## Recommended next steps

1. Verify the machine is at a clean BASIC prompt and audio is stopped.
2. Build a minimal `$9000` app that:
   - clears the screen
   - draws a visible counter or color toggle
   - never returns
   - exits only on explicit `ESC`/`Q`
3. Load it with `LOAD "TEST.BIN"` and run `SYS $9000`.
   Prove `LOAD` and `SYS` are reliable independent of music.
4. Test `MIDPLAY "STARS.NMS"` alone.
   Verify audible music, advancing `musicEventRead`, and clean `audio-stop`.
5. Test visualizer alone in synthetic mode.
6. Test `MIDPLAY` followed by the minimal `$9000` app.
7. Only then reintroduce the full keyboard visualizer.
8. If `SYS $9000` returns unexpectedly, pause and inspect:
   - PC
   - SP
   - stack bytes
   - `$0014-$005A` scratch range
   - BASIC execute pointer
   - `MusicStatus`
9. Add a hardware smoke test once the flow is stable.

## Acceptance criteria

### MIDI/WTS visualizer

1. Cold start with the MIDI test disk mounted.
2. Run the launcher.
3. `STARS.NMS` starts real music playback, not a stuck tone.
4. The keyboard visualizer stays on screen.
5. BASIC `Ready` does not appear while the visualizer is active.
6. The old NCC prompt never appears.
7. Song title and soundfont name display correctly.
8. Keys highlight as notes play.
9. Progress bar advances.
10. `ESC` or `Q` exits cleanly.
11. BASIC returns to a usable `Ready` prompt after exit.
12. No stuck notes remain after exit or `audio-stop`.
13. Repeat from cold start at least three times without reset, I/O error,
    stuck tone, or stale text.

### SID visualizer

1. Start a SID from the HVSC disk.
2. Visualizer detects SID mode.
3. It shows 6 SID voices, not 8 WTS voices.
4. It displays song/file metadata.
5. It highlights SID notes/voices.
6. Progress display behaves sensibly.
7. Exit is clean and leaves no stuck SID voice.

## Useful commands

Stop audio:

```bash
curl -fsS -X POST http://192.168.1.65/audio-stop
```

Read audio status:

```bash
curl -fsS http://192.168.1.65/audio-status
```

Read CPU state:

```bash
tools/novahostctl.py --host 192.168.1.65 --timeout 5 state
```

Cold start without waiting for `Ready`:

```bash
tools/novahostctl.py --host 192.168.1.65 --timeout 20 cold-start --no-wait
```

Reload ROMs from SD:

```bash
tools/novahostctl.py --host 192.168.1.65 --timeout 30 reload-rom
```

Upload updated BASIC and extension ROMs:

```bash
curl -fsS -X PUT --data-binary @e6502.ESP32/novahost/build/sd/roms/novabasic.bin http://192.168.1.65/sd/roms/novabasic.bin
curl -fsS -X PUT --data-binary @e6502.ESP32/novahost/build/sd/roms/extension.bin http://192.168.1.65/sd/roms/extension.bin
```

Run a BASIC line:

```bash
tools/novahostctl.py --host 192.168.1.65 --timeout 5 basic 'SYS $9000'
```

Check the loaded binary at `$9000`:

```bash
tools/novahostctl.py --host 192.168.1.65 --timeout 5 peek 0x9000
```

## Rules for continuing

- No workarounds or fallback paths to hide broken behavior.
- Do not reintroduce BASIC `NCC`.
- Do not trust screen remnants; confirm CPU PC/state.
- Do not trust `midiPlaying:true` alone; confirm event progress and actual audio.
- Keep the 6502 side thin. Heavy MIDI/SID parsing stays on ESP/NovaHost.
- Isolate the `LOAD`/`SYS`/loop lifetime issue before touching rendering.
