# MIDI Tools Design

## Overview

Two tools sharing a core MIDI-to-SID engine:

1. **MIDPLAY** -- BASIC command for real-time MIDI playback through the MusicEngine
2. **mid2bas** -- CLI verb in e6502.Tools for offline conversion to .bas files with MML MUSIC statements

Both use DryWetMIDI (`Melanchall.DryWetMidi`) for MIDI parsing.

## Shared Core -- MidiEngine

`MidiEngine` in `e6502.Avalonia/Hardware/` provides shared logic for both tools.

### Pipeline

```
.mid file -> DryWetMIDI parse -> channel analysis -> channel selection -> voice assignment -> event stream
```

**Channel analysis** scans all MIDI tracks and groups note events by channel (0-15). For each channel: note count, GM program number (from Program Change events), and whether it's channel 10 (drums).

**Channel selection** picks up to 6 channels ranked by note count. Explicit mapping overrides auto-selection. Channel 10 (drums) is included in auto-select but mapped to a noise instrument.

### Instrument Bucket Map

Eight SID presets covering the full GM range:

| Bucket | GM Programs | Waveform | ADSR (A,D,S,R) | Character |
|--------|------------|----------|-----------------|-----------|
| Pulse Piano | 1-8 | Pulse ($40) | 0,4,7,6 | Sharp piano |
| Pulse Lead | 81-96 | Pulse ($40) | 0,9,0,6 | Synth lead |
| Saw Strings | 41-56 | Saw ($20) | 4,6,10,8 | Warm strings |
| Saw Brass | 57-72 | Saw ($20) | 0,5,8,4 | Punchy brass |
| Tri Flute | 73-80 | Triangle ($10) | 2,6,12,8 | Soft pipe |
| Tri Bass | 33-40 | Triangle ($10) | 0,5,8,4 | Round bass |
| Noise Drums | Ch.10 | Noise ($80) | 0,3,0,2 | Percussion |
| Saw Default | Everything else | Saw ($20) | 0,5,8,4 | General |

### Velocity Mapping

Per-note. MIDI velocity (0-127) maps to SID per-voice volume (0-15): `velocity * 15 / 127`. Emitted as MML `V` commands for mid2bas, written directly to SID per-voice volume registers for MIDPLAY.

### Polyphony

Top-note-wins. When a channel has simultaneous notes, only the highest pitch is sounded. This gives monophonic reduction per voice.

## MIDPLAY -- Real-Time Playback

BASIC command that loads a MIDI file and plays it through the MusicEngine in real time. Does not convert to MML -- feeds the MusicEngine directly by scheduling note events on a tick-based timeline.

### Syntax

```
MIDPLAY "song"                    -- auto-select 6 busiest channels
MIDPLAY "song",1=3,2=5,3=10      -- explicit SID voice=MIDI channel mapping
MIDSTOP                           -- stop MIDI playback
```

The `.mid` extension is added automatically. Files load from `~/e6502-programs`.

### Implementation

Goes through FileIoController like SIDPLAY. A new FIO command (`FioCmdMidPlay`) triggers host-side logic:

1. Parse the MIDI file with DryWetMIDI
2. Run channel analysis and selection via MidiEngine
3. Convert selected channels into a flat timeline of timed events (note on/off, program change, tempo change)
4. Feed the timeline into MusicEngine via `MidiPlayback`, a 60Hz tick driver

**MidiPlayback** holds a timeline cursor. Each 60Hz tick it advances by the appropriate number of MIDI ticks (based on current tempo and PPQN), fires any events within the frame window. Note-on writes SID frequency + gate-on + per-voice volume. Note-off gates off.

**Stopping:** `MIDSTOP` triggers `FioCmdMidStop`, which stops MidiPlayback and silences all voices.

**Status:** `PLAYING` function returns true while MIDI is active.

## mid2bas -- Offline Converter

CLI verb in e6502.Tools that converts MIDI files to standalone .bas programs with MML MUSIC statements.

### Syntax

```
dotnet run --project e6502.Tools -- mid2bas song.mid -o song.bas
dotnet run --project e6502.Tools -- mid2bas song.mid --mml-only
dotnet run --project e6502.Tools -- mid2bas song.mid --voices 1=3,2=5,3=10
dotnet run --project e6502.Tools -- mid2bas song.mid --title "JOPLIN" --subtitle "THE ENTERTAINER"
```

### MIDI-to-MML Conversion

Walks each selected channel's note events chronologically and emits MML commands:

- Notes: letter + octave tracking (`>`, `<`, `O`) + duration + accidentals
- Rests: `R` with duration
- Velocity changes: `V` command (velocity * 15 / 127)
- Program changes: `I` command (bucket lookup)
- Tempo changes: `T` command
- Chords: top-note-wins (monophonic reduction per voice)
- Durations: quantized to nearest MML value (see Duration Quantization below)

### Output Structure

```basic
10 REM TITLE / SUBTITLE
20 INSTRUMENT 1, ...
30 INSTRUMENT 2, ...
40 MUSIC 1, "T120 V12 I1 L8 O4 ..."
50 MUSIC 1, "..."
60 MUSIC 2, "T120 V10 I2 L4 O3 ..."
70 VOLUME 15
80 MUSIC PLAY
```

Long MML strings split across multiple `MUSIC voice, "..."` lines. `--max-line-len` controls the split point (default 200). `--mml-only` dumps raw MML to stdout for debugging.

## Duration Quantization

MIDI durations are in absolute ticks (PPQN, typically 480). MML uses note-value denominators. Convert via:

```
mml_ticks = midi_duration * 96 / ppqn
```

Valid MML durations:

| MML | Ticks | Dotted | Ticks |
|-----|-------|--------|-------|
| 1 | 384 | 1. | 576 |
| 2 | 192 | 2. | 288 |
| 4 | 96 | 4. | 144 |
| 8 | 48 | 8. | 72 |
| 16 | 24 | 16. | 36 |
| 32 | 12 | 32. | 18 |

**Tie strategy** for non-standard durations: find the largest MML duration that fits within the remaining ticks, subtract, repeat until zero or remainder < 6 ticks (discard). Handles arbitrary MIDI durations cleanly.

This logic lives in MidiEngine. Only mid2bas needs it -- MIDPLAY works directly in MIDI ticks.

## Project Structure

### New NuGet Package

`Melanchall.DryWetMidi` added to `e6502.Avalonia` and `e6502.Tools`.

### New Files

| File | Purpose |
|------|---------|
| `e6502.Avalonia/Hardware/MidiEngine.cs` | Shared core: parsing, channel analysis, selection, instrument buckets, duration quantization, MML generation |
| `e6502.Avalonia/Hardware/MidiPlayback.cs` | Real-time 60Hz tick driver for MIDPLAY |
| `e6502.Tools/MidToBasCommand.cs` | CLI verb: argument parsing, calls MidiEngine, emits .bas output |

### Modified Files

| File | Change |
|------|--------|
| `ehbasic/basic.asm` | MIDPLAY and MIDSTOP tokens + handlers |
| `Hardware/FileIoController.cs` | FioCmdMidPlay, FioCmdMidStop handlers |
| `Hardware/VgcConstants.cs` | New FIO command constants |
| `Hardware/MusicEngine.cs` | Methods for MidiPlayback to drive voices directly |
| `Ipc/EmulatorTcpServer.cs` | midplay/midstop TCP commands |
| `e6502.Tools/Program.cs` | mid2bas verb routing |

## Testing

### Unit Tests

- Channel analysis: note counts, GM program extraction, channel 10 detection
- Auto-selection: top-6-by-note-count, ties broken by channel order
- Explicit mapping: overrides auto-select
- Instrument buckets: GM program ranges to correct presets
- Velocity mapping: MIDI 0->V0, 64->V8, 127->V15
- Duration quantization: exact fits, dotted notes, tie chains
- MML generation: correct notes, rests, octave changes, velocity, tempo

### Integration Tests

- MIDPLAY: load .mid via FileIoController, verify MusicEngine receives correct events
- mid2bas: convert test .mid, verify output .bas has valid MUSIC statements

### Test Data

Small hand-crafted .mid files created programmatically with DryWetMIDI in test setup: single-channel melody, multi-channel piece, drums on channel 10. No external file dependencies.
