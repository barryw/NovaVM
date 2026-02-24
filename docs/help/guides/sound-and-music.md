---
title: "Sound and Music"
type: guide
category: Sound
---

> Music is the arithmetic of sounds as optics is the geometry of light.

-- Claude Debussy

NovaBASIC includes two SID chip emulators -- software recreations of the MOS
6581 Sound Interface Device made famous by the Commodore 64. Six independent
voices with four waveforms, ADSR envelopes, and a programmable filter deliver
authentic chiptune sound. On top of the SID sits a six-voice MML music
sequencer with per-frame effects including vibrato, portamento, arpeggios,
pulse-width modulation, and filter sweeps.

This chapter covers every sound command from simple one-shot notes to
full multi-voice compositions.

## Quick-Start Overview

The sound system has three layers, each building on the one below:

1. **SOUND** -- play a single note on the SID chip. Specify a MIDI
note number, duration in frames, and an optional instrument preset.
2. **INSTRUMENT** -- define a reusable preset that sets the SID
waveform and ADSR envelope. Up to 16 presets (slots 0--15).
3. **MUSIC** -- load MML (Music Macro Language) sequences into up
to six voices and play them back with tempo, looping, and
per-frame effects.

A minimal program that plays a note:

```basic
10 VOLUME 12
20 SOUND 60, 30
```

Line 10 sets master volume. Line 20 plays MIDI note 60 (middle C) for
30 frames (half a second at 60 Hz).

## The SOUND Command

`SOUND note, duration [, instrument]`

- `note` -- MIDI note number (0--127). Middle C is 60; A4
(concert pitch 440 Hz) is 69. See the MIDI note table below.
- `duration` -- length in 1/60-second frames. A value of 60
plays for one second; 30 plays for half a second.
- `instrument` -- optional instrument preset (0--15). If omitted,
instrument 0 is used.

If `note` or `duration` is zero, the sound is stopped immediately.

::: note
`SOUND` triggers a one-shot sound effect through the music engine's
SFX channel. It does not interrupt music playback; the engine allocates a
voice for the effect and restores it when the sound completes.
:::

### Common MIDI note numbers

| **Note** | **MIDI** | **Approx.\ Frequency** |
| --- | --- | --- |
| C3 | 48 | 131 Hz |
| C4 (Middle C) | 60 | 262 Hz |
| D4 | 62 | 294 Hz |
| E4 | 64 | 330 Hz |
| F4 | 65 | 349 Hz |
| G4 | 67 | 392 Hz |
| A4 | 69 | 440 Hz |
| B4 | 71 | 494 Hz |
| C5 | 72 | 523 Hz |
| C6 | 84 | 1047 Hz |

The formula is: frequency = 440 x 2^((midi - 69) / 12).

### A simple melody

```basic
10 VOLUME 12
20 DATA 60, 62, 64, 65, 67, 69, 71, 72
30 FOR N = 1 TO 8
40   READ M
50   SOUND M, 15
60   FOR I = 1 TO 15 : VSYNC : NEXT I
70 NEXT N
```

Each note plays for 15 frames ( 250 ms). The `VSYNC` loop
holds the program for the same duration before the next note fires.

::: note
`SOUND` does not block program execution. Use a `VSYNC` loop after
each `SOUND` call to create the gap between notes.
:::

## The INSTRUMENT Command

`INSTRUMENT id, waveform, attack, decay, sustain, release`

Defines a reusable sound preset in one of 16 instrument slots.

- `id` -- slot number (0--15).
- `waveform` -- SID waveform byte:
`10` = triangle, `20` = sawtooth,
`40` = pulse, `80` = noise.
- `attack` -- attack rate (0--15). 0 is instantaneous; 15 is
slowest.
- `decay` -- decay rate (0--15). How quickly the volume drops
from peak to the sustain level.
- `sustain` -- sustain level (0--15). The steady-state volume
held while the note plays. 15 = full volume; 0 = silent (percussive).
- `release` -- release rate (0--15). How quickly the volume fades
to silence after the note ends.

```basic
10 REM BRIGHT PULSE LEAD
20 INSTRUMENT 0, $40, 0, 9, 0, 6
30 REM WARM SAWTOOTH PAD
40 INSTRUMENT 1, $20, 4, 6, 12, 8
50 REM NOISE DRUM HIT
60 INSTRUMENT 2, $80, 0, 3, 0, 2
```

::: note
Instrument 0 is pre-initialized at boot with: pulse waveform (`$40`),
attack 0, decay 9, sustain 0, release 6, pulse width 2048. All other slots
(1--15) start as copies of slot 0.
:::

### SID waveform reference

| **Value** | **Name** | **Character** |
| --- | --- | --- |
| `$10` | Triangle | Soft and mellow; flute-like. Good for gentle melodies and background pads. |
| `$20` | Sawtooth | Buzzy and harmonically rich. Good for brass-like leads and bass lines. |
| `$40` | Pulse | Bold, hollow, classic chiptune sound. Pulse width can be modulated via MML for evolving timbres. |
| `$80` | Noise | Unpitched random output. Use for drums, hi-hats, explosions, and ambient textures. |

### ADSR envelope overview

The four parameters shape how a note's volume changes over time:

1. **Attack** ramps from silence to full amplitude.
2. **Decay** drops from full amplitude to the sustain level.
3. **Sustain** holds at a constant level while the note plays.
4. **Release** fades from the sustain level to silence after the
note ends.

Sustain is a *level* (0--15); the other three are *rate* values
where 0 is fastest and 15 is slowest. This matches the original SID chip
behavior.

### Instrument recipes

| **Sound** | **Wave** | **A** | **D** | **S** | **R** | **Notes** |
| --- | --- | --- | --- | --- | --- | --- |
| Chiptune lead | $40 | 0 | 9 | 0 | 6 | Sharp attack, no sustain |
| Warm pad | $20 | 8 | 6 | 12 | 10 | Slow fade-in, high sustain |
| Bass | $20 | 0 | 5 | 8 | 4 | Instant attack, medium body |
| Snare drum | $80 | 0 | 3 | 0 | 2 | Short noise burst |
| Hi-hat | $80 | 0 | 1 | 0 | 1 | Very short noise tick |
| Organ | $10 | 0 | 0 | 15 | 4 | Triangle at full sustain |
| Pluck | $40 | 0 | 12 | 0 | 8 | Fast decay, no sustain |

## VOLUME

`VOLUME level`

Sets the SID master volume. `level` is 0--15 (only the low nibble is
used). The default volume at boot is 12.

## The MUSIC Engine

The music engine is a six-voice MML sequencer running on top of two SID
chips. You write melodies and rhythms as text strings using Music Macro
Language, load them into voices, and let the engine handle all the timing,
instrument switching, and per-frame effects automatically.

### Loading and Playing Sequences

`MUSIC voice, "mml-string"`

- `voice` -- voice number 1--6.
- `"mml-string"` -- an MML sequence (see Section \refsec:mml).

Additional subcommands control playback:

| **Command** | **Description** |
| --- | --- |
| MUSIC PLAY | Start playback of all loaded voices. |
| MUSIC STOP | Stop playback and silence all music voices. |
| MUSIC TEMPO bpm | Set tempo in beats per minute. Default is 120. |
| MUSIC LOOP ON | Enable looping; voices restart when all finish. |
| MUSIC LOOP OFF | Disable looping (default). |
| MUSIC PRIORITY v1[,...,v6] | Set voice-stealing priority for sound effects. Lower-numbered voices are stolen first. Default: 6, 5, 4, 3, 2, 1. |

### A Complete Music Example

```basic
10 VOLUME 12
20 REM DEFINE INSTRUMENTS
30 INSTRUMENT 0, $40, 0, 9, 0, 6
40 INSTRUMENT 1, $20, 0, 5, 8, 4
50 REM LOAD VOICES
60 MUSIC 1, "T140 I0 L8 O4 CDEFGAB >C"
70 MUSIC 2, "T140 I1 L4 O3 C G C G"
80 REM START PLAYBACK
90 MUSIC LOOP ON
100 MUSIC PLAY
```

Line 60 loads a melody into voice 1: tempo 140, instrument 0, eighth notes,
octave 4, ascending C major scale. Line 70 loads a bass line into voice 2:
quarter notes, octave 3, alternating C and G. Line 100 starts playback;
`MUSIC LOOP ON` on line 90 means the music repeats indefinitely.

### Querying Music Status

Two functions let you check what the music engine is doing:

| **Function** | **Returns** |
| --- | --- |
| PLAYING | 1 if music is currently playing, 0 if stopped. |
| MNOTE(voice) | The MIDI note number currently sounding on `voice` (1--6), or 0 if that voice is silent. |

```basic
200 IF PLAYING THEN GOTO 200
210 PRINT "MUSIC FINISHED"
```

## MML Reference

Music Macro Language (MML) is a compact text notation for music. Each voice
receives its own MML string. The parser is case-insensitive; all input is
converted to uppercase before processing.

### Notes and Rests

| **Syntax** | **Description** |
| --- | --- |
| C D E F G A B | Play a note. Pitch is determined by the current octave. |
| C# \textnormalor C+ | Sharp (raise one semitone). |
| C- | Flat (lower one semitone). |
| R | Rest (silence for the note duration). |

### Duration

A number following a note or rest sets its length as a note-value denominator:

| **Denominator** | **Ticks** | **Name** |
| --- | --- | --- |
| 1 | 384 | Whole note |
| 2 | 192 | Half note |
| 4 | 96 | Quarter note |
| 8 | 48 | Eighth note |
| 16 | 24 | Sixteenth note |
| 32 | 12 | Thirty-second note |

Internally, one quarter note equals 96 ticks.

A dot (`.`) after the duration extends it by 50\%: `C4.` plays for
96 x 1.5 = 144 ticks (dotted quarter).

If no duration is given, the default length set by `L` is used (initially
a quarter note).

### Ties

The ampersand (`&`) ties two durations together into a single sustained
note:

`C4&8` -> quarter + eighth = 144 ticks

Multiple ties can be chained: `C4&4&4` plays for 96 + 96 + 96 = 288
ticks. A single `NoteOn` event is emitted with the combined duration.

### Octave

| **Cmd** | **Description** |
| --- | --- |
| O4 | Set absolute octave (range 1--7; default 4). |
| > | Octave up (clamped to 7). |
| < | Octave down (clamped to 1). |

MIDI note calculation: `midi = (octave + 1) * 12 + semitone`, where
C=0, D=2, E=4, F=5, G=7, A=9, B=11.

### Default Length

`L8` sets the default note/rest duration to eighth notes. All subsequent
notes and rests that omit an explicit duration will use this value.

### Tempo

`T120` sets the tempo to 120 beats per minute. The default is 120.
Tempo can appear anywhere in the MML string and takes effect immediately.
At 120 BPM, one quarter note lasts exactly 0.5 seconds.

::: note
Tempo is global. If multiple voices contain `T` commands, the last one
processed wins. It is best practice to set tempo in voice 1 only.
:::

### Instrument Selection

`I3` switches the current voice to instrument slot 3 (defined earlier
with the `INSTRUMENT` BASIC command). Instrument changes take effect on
the next note.

### Loops

Square brackets repeat a section:

`[CDEF]3` -> plays C D E F three times

The repeat count follows the closing bracket. If omitted, the default is 1
(no repetition). Loops do not nest.

### Arpeggios

Curly braces define an arpeggio -- a rapid cycling through multiple notes:

`\{CEG\}4` -> cycle C, E, G at 60 Hz for one quarter note

Each frame advances to the next note in the list. Accidentals are supported
inside the braces (`\{C#EG#\}`). The arpeggio duration follows the
closing brace using standard duration syntax.

### Per-Frame Effects

The music engine processes the following effects on every frame (60 Hz).
These effects are set within MML and remain active until changed or a new
note resets them.

#### Vibrato

` 6` sets vibrato depth to 6. Higher values produce wider
pitch oscillation. ` 0` turns vibrato off.

The vibrato oscillates at approximately 2.9 Hz (sine wave). The pitch offset
is proportional to both the depth value and the current note frequency.

#### Portamento (Pitch Slide)

`/` before a note causes the voice to slide from the current pitch to
the target note rather than jumping instantly.

`C4 /E4` -> play C, then glide smoothly to E

The slide rate is approximately 1/8 of the frequency distance per
frame.

#### Pulse Width

`@P2048` sets the SID pulse width to 2048 (range 0--4095). This only
affects the pulse waveform (`$40`). A value of 2048 gives a 50\%
duty cycle (square wave); lower or higher values create thinner, more nasal
timbres.

#### Pulse Width Modulation (PWM)

`@PS+` starts sweeping the pulse width upward; `@PS-` sweeps
downward; `@PS0` stops the sweep. The sweep rate is +/-32 per frame,
clamped to the 0--4095 range. PWM gives the pulse waveform a rich,
evolving character.

```basic
60 MUSIC 1, "@P1024 @PS+ O4 L2 C E G >C"
```

#### Filter Cutoff and Resonance

`@F1024,8` sets the SID filter cutoff to 1024 (range 0--2047) and
resonance to 8 (range 0--15). The resonance parameter is optional; if
omitted, it defaults to 0.

#### Filter Mode

| **Cmd** | **Mode** |
| --- | --- |
| @FL | Low-pass (cuts highs, warm sound) |
| @FB | Band-pass (emphasizes a frequency band) |
| @FH | High-pass (cuts lows, thin sound) |
| @FO | Filter off |

#### Filter Sweep

`@FS+` sweeps the filter cutoff upward; `@FS-` sweeps downward;
`@FS0` stops the sweep. The sweep rate is +/-8 per frame, clamped to
0--2047.

```basic
60 MUSIC 1, "@FL @F200,12 @FS+ L4 O3 [CDEFGAB>C<]2"
```

This creates a classic filter sweep effect: low-pass filter starting at
cutoff 200 with high resonance, sweeping upward through the melody.

### MML Command Summary

| **Command** | **Parameters** | **Description** |
| --- | --- | --- |
| A--G | [#/+/-][dur][.] | Play note |
| R | [dur][.] | Rest |
| O | 1--7 | Set octave |
| > | -- | Octave up |
| < | -- | Octave down |
| L | denominator | Default note length |
| T | bpm | Tempo (default 120) |
| I | 0--15 | Select instrument slot |
| & | [note]dur | Tie (extend note duration) |
| [...]n | repeat count | Loop section n times |
| \{notes\} | [dur][.] | Arpeggio |
|  | depth (0=off) | Vibrato |
| / | -- | Portamento (next note slides) |
| @P | 0--4095 | Set pulse width |
| @PS | +, -, 0 | PWM sweep direction |
| @F | cutoff[,res] | Filter cutoff (0--2047) and resonance (0--15) |
| @FL | -- | Low-pass filter |
| @FB | -- | Band-pass filter |
| @FH | -- | High-pass filter |
| @FO | -- | Filter off |
| @FS | +, -, 0 | Filter sweep direction |

Whitespace, tabs, newlines, and pipe characters (`|`) are ignored and
can be used freely to format MML strings for readability.

## SID File Playback

NovaBASIC can load and play standard `.sid` files -- the native
music format of the Commodore 64 scene:

| **Command** | **Description** |
| --- | --- |
| SIDPLAY "filename" [, song] | Load and play a `.sid` file. The optional `song` parameter selects which sub-tune to play (default 1). |
| SIDSTOP | Stop SID file playback. |

```basic
10 SIDPLAY "commando"
20 FOR I = 1 TO 600 : VSYNC : NEXT I
30 SIDSTOP
```

SID files are loaded from the ` /e6502-programs`
directory. The `.sid` extension is added automatically. The SID
player injects an IRQ trampoline into CPU RAM that calls the file's init
and play routines at 60 Hz.

::: warning
SID playback takes over the SID chip directly. `SOUND` and `MUSIC`
commands will not produce audible output while a SID file is playing. Call
`SIDSTOP` before using other sound commands.
:::

## Graphics File I/O

NovaBASIC can save and load VGC memory spaces to disk:

| **Command** | **Description** |
| --- | --- |
| GSAVE "name", space, offset, len | Save `len` bytes from VGC memory space starting at `offset` to a `.gfx` file. |
| GLOAD "name", space, offset[, len] | Load a `.gfx` file into VGC memory space at `offset`. If `len` is omitted, the entire file is loaded. |

VGC memory spaces:

| **Space** | **Contents** |
| --- | --- |
| 0 | Character RAM (2000 bytes) |
| 1 | Color RAM (2000 bytes) |
| 2 | Graphics bitmap (64000 bytes) |
| 3 | Sprite shape RAM (2048 bytes) |

```basic
10 MODE 1 : GCLS
20 GCOLOR 10 : CIRCLE 160, 100, 80
30 GSAVE "mycircle", 2, 0, 64000
40 REM LATER...
50 GLOAD "mycircle", 2, 0
```

## Music Engine Architecture

For advanced users, understanding the engine internals helps write better
music and avoid common pitfalls.

### Voice allocation

The music engine manages six music voices (voices 1--3 mapped to SID 1
voices 0--2; voices 4--6 mapped to SID 2 voices 0--2) plus one shared SFX
voice. When `SOUND` triggers a sound effect, the engine:

1. Looks for a voice with no music sequence loaded.
2. If all voices have sequences, steals a voice according to the
priority order (default: voice 6 first, then 5, then 4, then 3,
then 2, then 1).
3. Plays the SFX on the stolen voice; when done, restores the music
voice.

### The Second SID Chip

NovaBASIC includes two SID chips for a total of six voices. The first SID
(D400) handles voices 1--3; the second SID (D420) handles voices 4--6.
A legacy mirror at $D500 routes transparently to the second SID.

From BASIC, the second SID is fully transparent -- simply use voice
numbers 4--6 with the `MUSIC` command:

```basic
10 INSTRUMENT 0,64,2,8,12,6
20 MUSIC 1,"T140 I0 O4 C4 E4 G4 C5 2"
30 MUSIC 4,"T140 I0 O3 C2 G2"
40 MUSIC PLAY
```

Voice 4 plays on the second SID chip. All instrument definitions, tempo,
and loop settings apply equally to both chips.

### Timing

The engine ticks at 60 Hz. Tempo is converted to ticks per frame:

ticks per frame = 96 x BPM/3600

At 120 BPM this is 3.2 ticks per frame. A quarter note (96 ticks) takes
exactly 30 frames = 0.5 seconds.

### Effect processing order

Each frame, active effects are processed in this order:

1. Arpeggio (cycle to next note)
2. PWM sweep (+/-32 per frame, clamped 0--4095)
3. Vibrato (sine wave at 2.9 Hz)
4. Portamento (slide 1/8 of remaining distance per frame)
5. Filter sweep (+/-8 per frame, clamped 0--2047)

## Composition Tips

::: tip
- Define all instruments before loading music sequences.
- Use `I` in MML to switch instruments mid-voice for timbral
variety.
- Keep melody, harmony, and bass on separate voices. Each voice has
its own instrument, octave, and effect state.
- Use `|` characters in MML strings as bar-line separators for
readability: `"L8 CDEF | GABC"`.
- At 120 BPM: quarter = 30 frames, eighth = 15, sixteenth = 7.5. Use
`T` to control tempo rather than adjusting note lengths.
- Use `@PS+` and `@PS-` on pulse waveforms for rich, evolving
textures.
- Combine `@FL` with `@FS+` for classic acid-bass filter
sweeps.
- The noise waveform (`$80`) with short ADSR makes convincing
drums. Load a noise instrument and trigger it with `SOUND`
while the music plays.

:::

## Deprecated Commands

::: warning
The following commands from earlier versions have been superseded:

- `WAVE` -- raises a syntax error. Use `INSTRUMENT` instead.
- `ENVELOPE` -- replaced by `INSTRUMENT`. Programs should be
updated to use the new six-parameter syntax.

:::

## Try It Now

::: tryit
Type and run the following program to hear a six-voice arrangement with
instrument presets, MML sequences, filter effects, and looping:

```basic
10 VOLUME 12
20 INSTRUMENT 0, $40, 0, 9, 0, 6
30 INSTRUMENT 1, $20, 0, 5, 8, 4
40 INSTRUMENT 2, $80, 0, 3, 0, 2
50 MUSIC 1, "T120 I0 L8 O4 CDEFGAB >C2"
60 MUSIC 2, "T120 I1 L4 O3 C G C G"
70 MUSIC 3, "T120 I2 L8 O4 R C R C R C R C"
80 MUSIC LOOP ON
90 MUSIC PLAY
```

Expected result: a three-voice loop with a pulse-wave melody, sawtooth bass,
and noise percussion on SID 1. The music repeats until you type `MUSIC STOP` in
direct mode. Try assigning additional voices (4--6) to use the second SID chip.

Experiments:
- Change `T120` to `T180` for a faster tempo.
- Add vibrato to voice 1: change the MML to start with
`"T120 I0 4 L8 O4 ..."`.
- Add a filter sweep to voice 2:
`"T120 I1 @FL @F200,10 @FS+ L4 O3 C G C G"`.

:::
