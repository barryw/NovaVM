# SID Sound & Music BASIC Commands — Design

## Goal

Replace the dead VSC sound commands with a powerful, user-friendly sound and music system that drives the real SID chip. Sound effects are fire-and-forget. Music is background playback with a tracker-quality MML sequence language. Users never manage voice numbers — the engine handles allocation and priority automatically.

## Architecture

All parsing and sequencing happens in C# on the host side. BASIC commands write parameters to FIO registers and trigger FIO commands. The host-side `MusicEngine` class:

- Stores instrument definitions (up to 16)
- Parses MML strings into event lists
- Runs a per-frame sequencer (60 Hz, driven by raster IRQ tick)
- Manages voice allocation for SFX vs music
- Tracks SFX duration and fires completion callbacks

The 6502 never parses note names, calculates frequencies, or manages timing. It just sends commands and data to the host.

### Frame Tick (60 Hz)

On each VGC frame tick (already running in `MainWindow.axaml.cs`):

1. Advance music sequencer — process next events per voice
2. Process per-frame effects (arpeggio cycling, PWM sweep, filter sweep, vibrato, portamento)
3. Write computed SID register values for each active voice
4. Decrement SFX duration counters, release voices when done
5. Set SFX-complete flag if a sound effect ended (for `ON SOUND GOSUB`)

---

## Commands

### INSTRUMENT id, waveform, A, D, S, R

Define a reusable voice preset. Shared between sound effects and music.

- `id`: 0-15 (instrument 0 is a built-in default, overwritable)
- `waveform`: keyword — `TRI`, `SAW`, `PULSE`, `NOISE`
- `A, D, S, R`: ADSR values 0-15 each

```basic
INSTRUMENT 1, PULSE, 0, 6, 0, 0    :REM sharp staccato
INSTRUMENT 2, SAW, 4, 8, 12, 4     :REM warm pad
INSTRUMENT 3, NOISE, 0, 5, 0, 0    :REM hi-hat
```

Built-in default (instrument 0): `PULSE, 0, 9, 0, 6` (short beep).

### SOUND note, duration [, instrument]

Play a one-shot sound effect. Always async. Voice auto-assigned by the engine.

- `note`: MIDI note number 0-127 (60 = middle C)
- `duration`: length in frames (1/60th second each)
- `instrument`: optional instrument id (default: 0)

```basic
SOUND 60, 10           :REM middle C, 10 frames, default beep
SOUND 72, 5, 3         :REM C5, 5 frames, instrument 3 (hi-hat)
```

Voice allocation:
1. Use a free voice (not used by music) — zero disruption
2. If all voices busy, steal lowest-priority music voice
3. When SFX ends, hand voice back to music engine

### VOLUME level

Set SID master volume. Shared by music and SFX.

- `level`: 0-15

```basic
VOLUME 15              :REM full volume
```

### PLAYING

Function. Returns 1 if a sound effect is currently active, 0 if not.

```basic
10 SOUND 60, 30
20 VSYNC
30 IF PLAYING THEN 20
40 PRINT "DONE"
```

### ON SOUND GOSUB line

Register a callback that fires when a sound effect finishes.

```basic
10 ON SOUND GOSUB 500
20 SOUND 60, 30
30 REM ...continues executing...
...
500 PRINT "SOUND DONE"
510 RETURN
```

Implementation: the SFX-complete flag is checked alongside the existing IRQ dispatch in ehbasic. When set, it triggers the GOSUB.

### MUSIC voice, "mml string"

Define a music sequence for a voice. Does not start playback — just loads the sequence.

- `voice`: 1-3
- `mml string`: Music Macro Language sequence (see MML Reference below)

```basic
MUSIC 1, "T140 I1 @P2048 @PS+ O4L8 {CEG}4 {FAC}4 | {GBD}4 {CEG}4"
MUSIC 2, "T140 I2 O2L4 C /E F /A | G /B >C2"
MUSIC 3, "T140 I3 L16 C8 RR C C8 RR C"
```

### MUSIC PLAY

Start playback of all defined music voices simultaneously.

### MUSIC STOP

Stop all music playback. Does not affect active sound effects.

### MUSIC TEMPO bpm

Override the tempo for all voices (can also be set per-voice in MML with `T`).

### MUSIC LOOP ON | OFF

Enable/disable looping. When ON, all voices restart from the beginning when the longest sequence ends.

### MUSIC PRIORITY voice [, voice...]

Define which music voices can be stolen by sound effects, in order.

```basic
MUSIC PRIORITY 3, 2    :REM steal voice 3 first, then 2. Never touch 1.
```

Default priority: 3, 2, 1 (voice 3 stolen first, melody on voice 1 protected).

---

## MML Reference

### Notes & Rests

| Token | Meaning | Example |
|-------|---------|---------|
| `C D E F G A B` | Note names | `C E G` |
| `#` or `+` | Sharp | `C# F+` |
| `-` | Flat | `B- E-` |
| `R` | Rest | `R4` (quarter rest) |

### Duration

| Token | Meaning | Example |
|-------|---------|---------|
| `1` | Whole note | `C1` |
| `2` | Half note | `C2` |
| `4` | Quarter note | `C4` |
| `8` | Eighth note | `C8` |
| `16` | Sixteenth note | `C16` |
| `.` | Dotted (1.5x) | `C4.` |
| `&` | Tie to next note | `C2&C8` |
| `L4` | Set default length | `L8 C D E` (all eighths) |

Number after a note is always **length**, not octave. Octave is set with `O` command.

### Octave & Tempo

| Token | Meaning | Example |
|-------|---------|---------|
| `O4` | Set octave (1-7) | `O4 C` = middle C |
| `>` | Octave up | `O3 C > C` |
| `<` | Octave down | `O5 C < C` |
| `T140` | Tempo in BPM | `T120` |

### Instruments

| Token | Meaning | Example |
|-------|---------|---------|
| `I1` | Switch to instrument 1 | `I1 C D I2 E F` |

### SID Effects — The "Whoa" Features

| Token | Meaning | Example |
|-------|---------|---------|
| `{CEG}` | **Arpeggio** — cycle notes at frame rate | `{CEG}4` = C major chord, quarter note duration |
| `~4` | **Vibrato** depth (0=off) | `~6 C2` |
| `/` | **Portamento** — slide to next note | `C /E` = slide from C to E |
| `@P2048` | Set pulse width (0-4095) | `@P2048` |
| `@PS+` | Auto-sweep pulse width up | `@PS+ C1` |
| `@PS-` | Auto-sweep pulse width down | `@PS-` |
| `@PS0` | Stop pulse width sweep | `@PS0` |
| `@F1024,8` | Filter cutoff (0-2047), resonance (0-15) | `@F1500,12` |
| `@FL` | Filter mode: lowpass | `@FL` |
| `@FB` | Filter mode: bandpass | `@FB` |
| `@FH` | Filter mode: highpass | `@FH` |
| `@FO` | Filter off for this voice | `@FO` |
| `@FS+` | Auto-sweep filter cutoff up | `@FS+` |
| `@FS-` | Auto-sweep filter cutoff down | `@FS-` |
| `@FS0` | Stop filter sweep | `@FS0` |

### Structure

| Token | Meaning | Example |
|-------|---------|---------|
| `[...]4` | Repeat block N times | `[C D E F]4` |
| `\|` | Bar line (ignored, readability) | `C D E F \| G A B >C` |

### Arpeggios In Detail

Arpeggios are the defining SID sound. `{CEG}` on a single voice rapidly cycles C → E → G → C → E → G at 60 Hz, creating the illusion of a chord. The duration suffix applies to the whole arpeggio block:

```
{CEG}4      quarter note arpeggio on C major
{CDE-G}2    half note arpeggio on C minor
{CE}8       octave power chord, eighth note
```

Notes inside braces use the current octave. Use `>` / `<` inside braces for cross-octave arpeggios: `{C E G >C}`.

---

## Example: Full Three-Voice Piece

```basic
10 REM === define instruments ===
20 INSTRUMENT 1, PULSE, 0, 6, 0, 0    :REM sharp lead
30 INSTRUMENT 2, SAW, 2, 6, 10, 4     :REM warm bass
40 INSTRUMENT 3, NOISE, 0, 5, 0, 0    :REM percussion

50 REM === lead: arpeggios with pulse width sweep ===
60 MUSIC 1, "T140 I1 @P2048 @PS+ O4L8 [{CEG}4 {FAC}4 | {GBD}4 {CEG}4]2"

70 REM === bass: slides and long notes ===
80 MUSIC 2, "T140 I2 O2L4 [C /E F /A | G /B >C2 <]2"

90 REM === drums: noise percussion pattern ===
100 MUSIC 3, "T140 I3 O4 [L16 C8 RR C C8 RR C |]4"

110 VOLUME 15
120 MUSIC LOOP ON
130 MUSIC PRIORITY 3, 2
140 MUSIC PLAY

150 REM === game loop with sound effects ===
160 VSYNC
170 IF FIRE THEN SOUND 100, 3, 1 : REM laser pew!
180 GOTO 160
```

---

## FIO Command Interface

All commands flow through the existing FIO register mechanism at `$B9xx`.

| FIO Command | ID | Parameters |
|---|---|---|
| `FIO_CMD_INSTRUMENT` | `$0A` | id, waveform, A, D, S, R via FIO registers |
| `FIO_CMD_SOUND` | `$0B` | note, duration, instrument via FIO registers |
| `FIO_CMD_VOLUME` | `$0C` | level via FIO register |
| `FIO_CMD_MUSIC_SEQ` | `$0D` | voice + MML string via FIO filename buffer |
| `FIO_CMD_MUSIC_PLAY` | `$0E` | (no params) |
| `FIO_CMD_MUSIC_STOP` | `$0F` | (no params) |
| `FIO_CMD_MUSIC_TEMPO` | `$10` | BPM via FIO registers |
| `FIO_CMD_MUSIC_LOOP` | `$11` | on/off via FIO register |
| `FIO_CMD_MUSIC_PRI` | `$12` | voice priority list via FIO registers |

`PLAYING` reads a status register (e.g., `$B9B0`) — returns nonzero if SFX active.

`ON SOUND GOSUB` stores the callback address in zero-page. The IRQ handler checks the SFX-complete flag and triggers GOSUB if set.

---

## Voice Management Detail

### Music Voice Assignment

- `MUSIC 1, "..."` assigns sequence to SID voice 1
- `MUSIC 2, "..."` assigns sequence to SID voice 2
- `MUSIC 3, "..."` assigns sequence to SID voice 3
- Voices without a MUSIC sequence are free for SFX

### SFX Voice Stealing

When SOUND fires:

1. Check for free voices (no music sequence assigned) — use first available
2. If no free voices, consult priority list (default: 3, 2, 1)
3. Steal the highest-priority-to-steal voice: suspend its music sequence, play SFX
4. When SFX duration expires: restore voice to music (resume sequence where it left off)

### Concurrent SFX

If a second SOUND fires while one is already playing:
- If another voice is available (free or stealable), use it
- If not, the new SFX replaces the current one (newest wins)
