# MusicXML to MML Converter Design

## Overview

New tool `tools/xml2mml.py` that converts MusicXML files (`.musicxml`, `.xml`, `.mxl`) to NovaBASIC MML MUSIC programs. Mirrors the existing `ly2mml.py` workflow and reuses its MML generation pipeline.

## Architecture

**Dependencies:** Python stdlib only (`xml.etree.ElementTree`, `zipfile`). Imports `LyNote`, `LyVoice`, `notes_to_mml`, `emit_basic`, `split_mml_to_lines`, and instrument maps from `ly2mml.py`.

**Pipeline:**

```
.mxl/.musicxml/.xml → parse MusicXML → list[MxlPart] → resolve to list[LyVoice] → notes_to_mml() → emit_basic()
```

Three stages:

1. **Parse** — Walk the MusicXML DOM extracting parts, measures, notes, attributes (key/time/divisions), directions (tempo, dynamics, wedges), and notations (slurs, staccato, trills, ornaments, fermatas, grace notes).

2. **Resolve** — Convert MusicXML's pitch/duration/expression model into `LyNote` objects. Handle chord splitting: if a part has simultaneous notes, split into N monophonic sub-voices using greedy nearest-end allocation.

3. **Select & Emit** — Auto-select up to 6 voices by note density. Pass the `LyNote` lists through the existing `notes_to_mml()` and `emit_basic()` pipeline.

## Input Formats

- `.musicxml` / `.xml` — uncompressed MusicXML, parsed directly with ElementTree
- `.mxl` — compressed MusicXML (ZIP), extracted with `zipfile` module; the `META-INF/container.xml` inside identifies the root `.xml` file

Broad MusicXML standard compatibility: MuseScore, Finale, Sibelius, Dorico exports, IMSLP downloads.

## MusicXML Parsing

### Core Music Data

- `<attributes>` — `<divisions>` (ticks per quarter note, varies per measure), `<key>` (fifths + mode), `<time>` (beats/beat-type), `<clef>`, `<transpose>`
- `<note>` — `<pitch>` (step/alter/octave), `<duration>`, `<type>` (whole/half/quarter/etc.), `<dot>`, `<rest>`, `<chord>` flag, `<tie>`, `<voice>` number, `<staff>` number
- `<forward>` / `<backup>` — time position shifts within a measure (critical for multi-voice parts like piano)

### Expression and Direction

- `<dynamics>` — pp, p, mp, mf, f, ff → filter cutoff via existing `_DYN_FILTER` map
- `<wedge>` — crescendo/diminuendo hairpins → `@FS+` / `@FS-`
- `<direction>` with `<sound tempo="...">` — tempo changes → MML `T` commands
- `<words>` — text expressions like "dolce", "legato" (best-effort mapping)

### Notations — Existing MML Mappings

- `<slur>` start/stop → portamento (`/`)
- `<staccato>` → halved duration + rest
- `<trill-mark>` → rapid alternation (existing `ly2mml.py` behavior)
- Long notes (half note or longer, not staccato) → vibrato (`~4`)

### Notations — New MML Mappings

- **Mordent** (`<mordent>`) → 3-note pattern: `C32D32C` (main→upper→main, all 32nds)
- **Inverted mordent** (`<inverted-mordent>`) → 3-note pattern: `C32B32C` (main→lower→main)
- **Turn** (`<turn>`) → 4-note pattern: `D32C32B32C` (upper→main→lower→main)
- **Inverted turn** (`<inverted-turn>`) → 4-note pattern: `B32C32D32C` (lower→main→upper→main)
- **Fermata** (`<fermata>`) → double the note's written duration
- **Grace notes** (`<grace>`) → short ornamental notes at 32nd duration

Ornament pitches respect the current key signature.

## Chord Splitting

Chord splitting happens per-part, after grouping notes by their MusicXML `<voice>` element.

**Algorithm:** Greedy nearest-end allocation. For each chord onset, sort pitches high-to-low. Assign each pitch to the sub-voice whose previous note ended closest to this onset. Create new sub-voices as needed (up to 4 per part).

**Naming:** Sub-voices get names derived from `<part-name>`: `"Piano-1"`, `"Piano-2"`, etc.

## Voice Selection

Auto-select up to 6 voices (2 SID chips x 3 voices each) ranked by total note count across all parts and sub-voices.

CLI `--parts` flag allows explicit override: `--parts "Violin I:1,Cello:2"`.

## Instrument Mapping

MusicXML's `<midi-instrument>` and `<midi-program>` provide General MIDI program numbers. Map to existing `INSTRUMENT_MAP` / `INSTRUMENT_VARIANTS` in `ly2mml.py`:

- GM 1-8 (piano family) → piano variants
- GM 7-8 (harpsichord/clavinet) → harpsichord variants
- GM 25-32 (guitar) → saw waveform
- GM 41-48 (strings) → saw strings
- GM 57-64 (brass) → saw trumpet
- GM 65-72 (reed) → saw oboe
- GM 73-80 (pipe) → tri flute

Fall back to `DEFAULT_INSTRUMENTS` when no match.

## Grace Notes

MusicXML distinguishes:
- `<grace slash="yes">` — acciaccatura (crushed): steal time from the following note
- `<grace>` without slash — appoggiatura: halve the principal note's duration, give half to the grace

Multiple grace notes preceding a note share the stolen time equally. All emitted as 32nd-note MML events.

## Edge Cases

- **Repeats** (`<repeat>`, `<ending>`) — expanded inline, including first/second endings. MML has no repeat construct.
- **Pickup measures** — handled naturally via absolute time position tracking.
- **Tuplets** (`<time-modification>`) — use MusicXML's actual `<duration>` value. Quantize to nearest valid MML duration (1/2/4/8/16/32, with dotted variants). Round when no clean fit.
- **Transposing instruments** — apply `<transpose>` to produce concert pitch.
- **Multiple `<staff>` elements** — treated same as multiple `<voice>` elements within a part.
- **`<backup>` / `<forward>`** — tracked as time cursor shifts to correctly separate interleaved voices.

## CLI Interface

```
python xml2mml.py score.musicxml -o output.bas \
  --title "SYMPHONY" --subtitle "MOVEMENT 1" \
  --tempo 120 --max-line-len 200 \
  --parts "Violin I:1,Violin II:2,Viola:3" \
  --mml-only --no-viz
```

MusicXML-specific flags:
- `--max-voices 6` — cap on total voices selected (default 6)
- `--no-split` — disable chord splitting, use top-note-wins instead

Shared flags (same as `ly2mml.py`):
- `-o` / `--output` — output `.bas` file path
- `--title` / `--subtitle` — program header text
- `--tempo` — override BPM (default: from MusicXML `<sound tempo>`)
- `--max-line-len` — maximum BASIC line length (default 200)
- `--mml-only` — print raw MML to stdout
- `--no-viz` — skip piano keyboard visualization
