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
