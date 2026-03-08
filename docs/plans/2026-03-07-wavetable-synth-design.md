# Wavetable Synthesizer (WTS) Design

## Overview

An 8-voice memory-mapped wavetable synthesizer that sits alongside the two SID chips. Loads SF2 soundfonts from the host filesystem, exposes instruments to the 6502 through a register interface at $A140–$A1FF. Combined with the 6 SID voices, the system provides 14 total voices with per-voice routing flexibility.

## Architecture

**Audio path:** Each voice reads PCM sample data from host memory, applies pitch interpolation, ADSR envelope, and per-voice volume/panning. The 8 voices mix to stereo, pass through shared reverb and chorus, and output to a dedicated stereo OpenAL source at 44.1kHz independent of the SID chips.

**Integration with MusicEngine:** MusicEngine expands from 6 to 14 voices. Voices 0–5 route to SID (3+3 as today). Voices 6–13 route to the WTS. Each voice's target is fixed by index. MidiPlayback allocates voices to SID or WTS depending on a per-channel routing configuration.

**Soundfont lifecycle:** A FileIoController command loads an SF2 file from `~/e6502-programs/soundfonts/`. The host parses it into an internal sample bank (PCM buffers, loop points, envelope parameters, key ranges). The 6502 enumerates and selects instruments via register reads/writes. Soundfont data never enters 6502 address space.

## Register Map ($A140–$A1FF)

### Per-Voice Registers — 8 voices × 8 bytes ($A140–$A17F)

| Offset | Register | R/W | Description |
|--------|----------|-----|-------------|
| +0 | NoteNum | W | MIDI note (0–127). Write triggers note-on. Write 0 = note-off. |
| +1 | Velocity | W | Note velocity (0–127). |
| +2 | Instrument | W | Instrument/preset index (0–255). |
| +3 | Volume | R/W | Voice volume (0–255). Default 255. |
| +4 | Panning | R/W | Stereo pan (0=left, 128=center, 255=right). Default 128. |
| +5 | PitchBendLo | W | Pitch bend fine (low byte). |
| +6 | PitchBendHi | W | Pitch bend coarse (high byte). $8000 = center/no bend. |
| +7 | Status | R | Bit 0 = voice active, Bit 1 = in release phase. |

Voice 0 at $A140–$A147, voice 1 at $A148–$A14F, etc.

### Global Registers ($A180–$A19F)

| Address | Register | R/W | Description |
|---------|----------|-----|-------------|
| $A180 | ReverbLevel | R/W | Reverb send (0–255). Default 80. |
| $A181 | ChorusLevel | R/W | Chorus send (0–255). Default 40. |
| $A182 | MasterVolume | R/W | Master output (0–255). Default 255. |
| $A183 | SoundfontStatus | R | 0=none, 1=ready, 2=loading, $FF=error. |
| $A184 | InstrumentCount | R | Instrument count in loaded soundfont. |
| $A185 | Command | W | Command register. |
| $A186 | CommandArg | W | Command argument byte. |
| $A187 | ActiveVoices | R | Bitmask of voices currently sounding. |

### Instrument Enumeration Buffer ($A1A0–$A1DF)

| Address | Register | R/W | Description |
|---------|----------|-----|-------------|
| $A1A0 | EnumIndex | W | Write instrument index to populate buffer. |
| $A1A1 | EnumBank | R | MIDI bank number. |
| $A1A2 | EnumProgram | R | MIDI program number. |
| $A1A3–$A1DF | EnumName | R | Instrument name, null-terminated ASCII (28 chars max). |

### Commands (write to $A185)

| Value | Command | Description |
|-------|---------|-------------|
| $01 | AllNotesOff | Silence all voices immediately. |
| $02 | ResetEffects | Reset reverb/chorus to defaults. |

## Internal Sample Engine

### Sample Bank Model

```
SampleBank
  └─ Instrument[] (up to 256)
       ├─ Name (string)
       ├─ MidiBank, MidiProgram
       └─ Region[] (key/velocity zones)
            ├─ KeyRangeLo, KeyRangeHi
            ├─ VelocityRangeLo, VelocityRangeHi
            ├─ SampleData (float[] PCM, mono)
            ├─ SampleRate (original recording rate)
            ├─ LoopStart, LoopEnd
            ├─ LoopEnabled (bool)
            ├─ RootKey (MIDI note the sample was recorded at)
            └─ ADSR (attack, decay, sustain, release in seconds)
```

SF2 files are parsed at load time into this internal representation. The synth engine only works with the internal model — other format loaders can be added without changing the engine.

### Voice Playback

Each voice maintains:

- **Phase accumulator** — fractional sample position (double). Increment: `sampleRate × pitchRatio / 44100` where `pitchRatio = 2^((note - rootKey) / 12)` adjusted by pitch bend.
- **ADSR envelope** — linear attack/decay, sustain hold, linear release. Value 0.0–1.0 multiplied into output. Note-off triggers release; voice freed at zero.
- **Loop handling** — phase wraps from loopEnd to loopStart. Non-looping samples stop at end of data.

### Render Pipeline (per sample, 44.1kHz)

1. For each active voice: fetch interpolated sample at current phase, apply envelope × velocity × volume
2. Apply panning (equal-power: left = cos(pan), right = sin(pan))
3. Sum all voices to stereo L/R
4. Send through chorus (short modulated delay)
5. Send through reverb (Freeverb-style: parallel comb filters + series allpass)
6. Blend dry/wet by ReverbLevel and ChorusLevel
7. Apply MasterVolume, clamp, output as stereo PCM16 to OpenAL

## MusicEngine Integration

### Voice Routing (fixed by index)

- Voices 0–2 → SID1
- Voices 3–5 → SID2
- Voices 6–13 → WTS

Note-on for SID voices writes frequency/waveform/gate registers as today. Note-on for WTS voices calls internal methods directly (no register bus round-trip).

### MIDI Playback Routing

- **Auto mode (default):** Channels allocated to WTS voices first, overflow to SID. Drum channel (ch 10) falls back to SID noise if no drum samples available.
- **Manual mode:** Per-channel routing table pins specific MIDI channels to SID or WTS. Set via FileIoController command or TCP.
- **SID-only mode:** Existing 6-voice SID behavior.

### MML Routing

MML targets voices by number. Voices 0–5 behave as today. Voices 6–13 target WTS. New `@W` MML command selects WTS instrument index (vs existing `@` for SID instrument slots).

### SFX

Voice stealing can draw from either pool. Priority prefers WTS for sample-based SFX, SID for chip-style effects.

## Soundfont Loading

### FileIoController Integration

1. 6502 writes filename to FileIoController filename buffer
2. 6502 writes SfLoad command byte
3. Host reads SF2 from `~/e6502-programs/soundfonts/`, parses to SampleBank
4. SoundfontStatus: 0 → 2 (loading) → 1 (ready) or $FF (error)
5. InstrumentCount populated on success

### SF2 Parsing

SF2 files use RIFF structure with three chunks: INFO (metadata), sdta (raw PCM16 sample data), pdta (presets, instruments, sample headers, key zones). Parsed directly without third-party libraries.

### GM Soundfonts

General MIDI SF2 files provide 128 melodic instruments + percussion. MidiPlayback auto-maps GM program changes to WTS instrument indices for correct playback out of the box.

## TCP/MCP Interface

New TCP commands:

- `wts_load_soundfont` — load SF2 by filename
- `wts_list_instruments` — return instrument names/indices
- `wts_note_on` / `wts_note_off` — direct voice control
- `wts_set_instrument` — set instrument per voice

MCP EmulatorTools exposes matching tool methods.

## Implementation Files

### New Files

| File | Purpose |
|------|---------|
| `Hardware/WavetableSynth.cs` | Core 8-voice sample engine, ADSR, mixing, effects |
| `Hardware/SampleBank.cs` | Internal instrument/region/sample data model |
| `Hardware/Sf2Loader.cs` | SF2 parser → SampleBank converter |
| `Hardware/ReverbEffect.cs` | Freeverb-style reverb |
| `Hardware/ChorusEffect.cs` | Modulated delay chorus |

### Modified Files

| File | Change |
|------|--------|
| `CompositeBusDevice.cs` | Route $A140–$A1DF, instantiate WTS, tick at 60Hz |
| `OpenAlRenderer.cs` | Stereo source support |
| `MusicEngine.cs` | Expand to 14 voices, WTS routing |
| `MidiPlayback.cs` | 14-voice allocation, auto/manual/SID-only modes |
| `MmlParser.cs` | `@W` command for WTS instrument selection |
| `FileIoController.cs` | SfLoad command |
| `VgcConstants.cs` | WTS register address constants |
| `Ipc/EmulatorTcpServer.cs` | WTS TCP commands |
| `e6502.MCP/EmulatorTools.cs` | WTS MCP tools |

### Testing

Unit tests for: SF2 parsing, sample interpolation, ADSR envelope, voice allocation, register read/write, MusicEngine 14-voice routing, MIDI channel-to-voice mapping.
