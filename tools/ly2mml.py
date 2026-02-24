#!/usr/bin/env python3
"""Convert LilyPond files to NovaBASIC MML MUSIC programs.

Directly parses LilyPond \\relative voice blocks into MML strings —
no intermediate MIDI step. Designed for Bach inventions and similar
two-voice keyboard works.
"""

from __future__ import annotations

import argparse
import re
import sys
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

# LilyPond note letters → index (for interval calculation in relative mode)
LETTER_INDEX = {"c": 0, "d": 1, "e": 2, "f": 3, "g": 4, "a": 5, "b": 6}

# LilyPond letter → semitone offset from C
SEMITONES = {"c": 0, "d": 2, "e": 4, "f": 5, "g": 7, "a": 9, "b": 11}

# MML note letters (same as LilyPond but uppercase)
MML_LETTERS = "CDEFGAB"

# Duration value → MML duration number (LilyPond and MML use the same system)
VALID_DURATIONS = {1, 2, 4, 8, 16, 32}

# MIDI instrument names → SID preset (waveform, A, D, S, R, label)
# Per-voice variants for the same instrument (voice 1, voice 2, ...)
INSTRUMENT_VARIANTS: dict[str, list[tuple[int, int, int, int, int, str]]] = {
    "harpsichord": [
        (64, 0, 6, 5, 4, "PULSE HARPSICHORD"),
        (64, 1, 8, 6, 4, "PULSE BASS"),
    ],
}

INSTRUMENT_MAP: dict[str, tuple[int, int, int, int, int, str]] = {
    "harpsichord": (64, 0, 6, 5, 4, "PULSE HARPSICHORD"),
    "acoustic grand": (64, 0, 4, 8, 4, "PULSE PIANO"),
    "piano": (64, 0, 4, 8, 4, "PULSE PIANO"),
    "church organ": (32, 2, 4, 12, 6, "SAW ORGAN"),
    "organ": (32, 2, 4, 12, 6, "SAW ORGAN"),
    "strings": (32, 4, 6, 10, 8, "SAW STRINGS"),
    "flute": (16, 2, 4, 10, 6, "TRI FLUTE"),
    "recorder": (16, 2, 4, 10, 6, "TRI RECORDER"),
    "oboe": (32, 1, 5, 9, 5, "SAW OBOE"),
    "trumpet": (32, 0, 4, 12, 4, "SAW TRUMPET"),
    "cello": (32, 4, 6, 10, 8, "SAW CELLO"),
}

# Default instruments for voices when none specified
DEFAULT_INSTRUMENTS: list[tuple[int, int, int, int, int, str]] = [
    (64, 0, 6, 5, 4, "PULSE HARPSICHORD"),
    (64, 1, 8, 6, 4, "PULSE BASS"),
]


# ---------------------------------------------------------------------------
# Data classes
# ---------------------------------------------------------------------------

@dataclass
class LyNote:
    """A resolved note/rest from LilyPond."""
    letter: str          # "c"-"b" or "r" for rest
    accidental: int      # -1=flat, 0=natural, +1=sharp
    octave: int          # absolute octave (4 = middle C octave)
    duration: int        # 1,2,4,8,16,32
    dotted: bool         # has dot
    tied: bool           # followed by ~
    is_rest: bool
    bar_marker: bool = False  # True for bar-check-only entries


@dataclass
class LyVoice:
    """A parsed LilyPond voice."""
    name: str
    notes: list[LyNote] = field(default_factory=list)
    instrument: str = "harpsichord"
    start_letter: str = "c"
    start_octave: int = 4


# ---------------------------------------------------------------------------
# LilyPond parser
# ---------------------------------------------------------------------------

def strip_comments(text: str) -> str:
    """Remove LilyPond % comments and %{ %} block comments."""
    # Block comments first
    text = re.sub(r"%\{.*?%\}", "", text, flags=re.DOTALL)
    # Line comments
    text = re.sub(r"%[^\n]*", "", text)
    return text


def extract_relative_pitch(pitch_str: str) -> tuple[str, int]:
    """Parse a LilyPond relative pitch like c' or c, into (letter, octave).

    LilyPond convention: c (no marks) = middle C octave for \\relative c,
    c' = one octave up, c, = one octave down.
    The base octave for unmodified letters:
      c (bass clef middle) = octave 3 in MIDI terms
      c' = octave 4 (middle C)
    """
    letter = pitch_str[0].lower()
    rest = pitch_str[1:]
    # Count octave marks
    ups = rest.count("'")
    downs = rest.count(",")
    # LilyPond: \relative c = starts at C3, \relative c' = C4, etc.
    base = 3  # unmodified letter = octave 3
    return letter, base + ups - downs


def find_brace_block(text: str, start: int) -> str:
    """Extract text between matched braces starting at position start."""
    depth = 0
    i = start
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[start + 1 : i]
        i += 1
    raise ValueError("Unmatched brace")


def parse_lilypond_file(text: str) -> tuple[list[LyVoice], int]:
    """Parse a LilyPond file, extracting voices and tempo.

    Returns (voices, tempo_bpm).
    """
    text = strip_comments(text)

    # Extract tempo
    tempo = 100  # default
    m = re.search(r"tempoWholesPerMinute\s*=\s*#\(ly:make-moment\s+(\d+)\s+(\d+)\)", text)
    if m:
        num, denom = int(m.group(1)), int(m.group(2))
        # Convert to quarter-note BPM: if "100 4" means 100 quarter notes/min
        tempo = num * 4 // denom
    else:
        m = re.search(r"\\tempo\s+(\d+)\s*=\s*(\d+)", text)
        if m:
            tempo = int(m.group(2))

    # Find voice definitions: name = \relative <pitch> { ... }
    voices: list[LyVoice] = []
    pattern = re.compile(
        r"(\w+)\s*=\s*\\relative\s+([a-g][',]*)\s*\{",
        re.IGNORECASE,
    )

    for m in pattern.finditer(text):
        name = m.group(1)
        start_letter, start_octave = extract_relative_pitch(m.group(2))
        brace_pos = text.index("{", m.start())
        body = find_brace_block(text, brace_pos)

        # Extract instrument
        instrument = "harpsichord"
        instr_m = re.search(r'\\set\s+Staff\.midiInstrument\s*=\s*"([^"]+)"', body)
        if instr_m:
            instrument = instr_m.group(1).lower()

        voice = LyVoice(
            name=name,
            instrument=instrument,
            start_letter=start_letter,
            start_octave=start_octave,
        )
        voice.notes = tokenize_voice(body, start_letter, start_octave)
        voices.append(voice)

    return voices, tempo


# ---------------------------------------------------------------------------
# Relative pitch resolver
# ---------------------------------------------------------------------------

def resolve_relative_pitch(
    prev_letter: str,
    prev_octave: int,
    cur_letter: str,
    octave_marks: str,
) -> tuple[str, int]:
    """Resolve a note's absolute octave in LilyPond relative mode.

    The rule: the note is placed in the octave that makes the interval
    from the previous note no larger than a fourth. Then ' and , modify.
    """
    prev_idx = LETTER_INDEX[prev_letter]
    cur_idx = LETTER_INDEX[cur_letter]

    # Calculate ascending interval in scale steps (0-6)
    interval_up = (cur_idx - prev_idx) % 7

    if interval_up <= 3:
        # Place note above or at the same pitch
        octave = prev_octave
        # Check if we cross C boundary going up
        if cur_idx < prev_idx and interval_up > 0:
            octave += 1
    else:
        # interval_up >= 4: place note below (descending interval is 7-interval_up <= 3)
        octave = prev_octave
        # Check if we cross C boundary going down
        if cur_idx > prev_idx:
            octave -= 1

    # Apply explicit octave marks
    ups = octave_marks.count("'")
    downs = octave_marks.count(",")
    octave += ups - downs

    return cur_letter, octave


# ---------------------------------------------------------------------------
# Voice tokenizer
# ---------------------------------------------------------------------------

# Regex for LilyPond tokens
_LY_TOKEN = re.compile(
    r"""
    ([a-g])                     # note letter
    ((?:is(?:is)?|es(?:es)?)?)  # accidental: is, isis, es, eses
    ([',]*)                     # octave marks
    (\d*)                       # duration
    (\.?)                       # dot
    (\s*~)?                     # tie
    |
    (r)(\d*)(\.?)               # rest with optional duration/dot
    |
    (\|)                        # bar check
    |
    \\[a-zA-Z]+                 # commands (skip)
    |
    [\[\](){}^_\\]              # beaming, slurs, markup (skip)
    |
    \#[^\s]+                    # scheme expressions (skip)
    |
    "[^"]*"                     # strings (skip)
    """,
    re.VERBOSE,
)


def parse_accidental(acc_str: str) -> int:
    """Convert LilyPond accidental suffix to semitone offset."""
    if acc_str == "is":
        return 1
    elif acc_str == "isis":
        return 2
    elif acc_str == "es" or acc_str == "eses":
        return -1 if acc_str == "es" else -2
    return 0


def _clean_voice_body(text: str) -> str:
    """Remove non-music LilyPond commands from voice body before tokenizing."""
    # Remove \set ... lines (contain note-letter chars in identifiers)
    text = re.sub(r"\\set\b[^\n]*", "", text)
    # Remove \clef ... argument
    text = re.sub(r'\\clef\s+"?[^"\s]+"?', "", text)
    # Remove \override, \revert, etc. with their arguments
    text = re.sub(r"\\(?:override|revert|once|omit)\b[^\n]*", "", text)
    # Remove ^\fermata and similar articulations
    text = re.sub(r"[\^_]\\[a-zA-Z]+", "", text)
    # Remove \bar "..." directives
    text = re.sub(r'\\bar\s+"[^"]*"', "", text)
    return text


def tokenize_voice(text: str, start_letter: str, start_octave: int) -> list[LyNote]:
    """Tokenize a LilyPond voice body into resolved LyNote objects."""
    text = _clean_voice_body(text)
    notes: list[LyNote] = []
    prev_letter = start_letter
    prev_octave = start_octave
    cur_duration = 8  # default duration if none specified

    for m in _LY_TOKEN.finditer(text):
        # Note
        if m.group(1):
            letter = m.group(1).lower()
            acc_str = m.group(2)
            octave_marks = m.group(3)
            dur_str = m.group(4)
            dot = bool(m.group(5))
            tied = bool(m.group(6))

            accidental = parse_accidental(acc_str)

            # Resolve octave
            letter, octave = resolve_relative_pitch(
                prev_letter, prev_octave, letter, octave_marks
            )
            prev_letter = letter
            prev_octave = octave

            # Update duration if specified
            if dur_str:
                cur_duration = int(dur_str)

            notes.append(LyNote(
                letter=letter,
                accidental=accidental,
                octave=octave,
                duration=cur_duration,
                dotted=dot,
                tied=tied,
                is_rest=False,
            ))

        # Rest
        elif m.group(7):
            dur_str = m.group(8)
            dot = bool(m.group(9))
            if dur_str:
                cur_duration = int(dur_str)
            notes.append(LyNote(
                letter="r",
                accidental=0,
                octave=0,
                duration=cur_duration,
                dotted=dot,
                tied=False,
                is_rest=True,
            ))

        # Bar check
        elif m.group(10):
            notes.append(LyNote(
                letter="",
                accidental=0,
                octave=0,
                duration=0,
                dotted=False,
                tied=False,
                is_rest=False,
                bar_marker=True,
            ))

    return notes


# ---------------------------------------------------------------------------
# MML generator
# ---------------------------------------------------------------------------

def note_to_mml_letter(letter: str, accidental: int) -> str:
    """Convert note letter + accidental to MML note string."""
    mml = letter.upper()
    if accidental == 1:
        mml += "#"
    elif accidental == -1:
        mml += "-"
    elif accidental == 2:
        # Double sharp: respell enharmonically
        idx = LETTER_INDEX[letter]
        semi = SEMITONES[letter] + 2
        # Find the enharmonic note
        for l, s in SEMITONES.items():
            if s == semi % 12:
                return l.upper()
        return mml + "#"  # fallback
    elif accidental == -2:
        idx = LETTER_INDEX[letter]
        semi = SEMITONES[letter] - 2
        for l, s in SEMITONES.items():
            if s == semi % 12:
                return l.upper()
        return mml + "-"  # fallback
    return mml


def notes_to_mml(notes: list[LyNote], tempo: int, instrument_id: int,
                 voice_num: int, is_first_voice: bool) -> str:
    """Convert resolved LyNote list to an MML string."""
    if not notes:
        return ""

    # Find most common duration for default L
    dur_counts: Counter[int] = Counter()
    for n in notes:
        if not n.bar_marker:
            dur_counts[n.duration] += 1
    default_dur = dur_counts.most_common(1)[0][0] if dur_counts else 8

    parts: list[str] = []

    # Prefix
    if is_first_voice:
        parts.append(f"T{tempo}")
    parts.append(f"I{instrument_id}")

    # Find first non-rest note to set initial octave
    initial_octave = 4
    for n in notes:
        if not n.is_rest and not n.bar_marker:
            initial_octave = n.octave
            break
    parts.append(f"O{initial_octave}")
    parts.append(f"L{default_dur}")

    cur_octave = initial_octave

    for n in notes:
        if n.bar_marker:
            parts.append("|")
            continue

        if n.is_rest:
            s = "R"
            if n.duration != default_dur:
                s += str(n.duration)
            if n.dotted:
                s += "."
            parts.append(s)
            continue

        # Octave adjustment
        target_oct = n.octave
        if target_oct != cur_octave:
            diff = target_oct - cur_octave
            if diff == 1:
                parts.append(">")
            elif diff == -1:
                parts.append("<")
            else:
                parts.append(f"O{target_oct}")
            cur_octave = target_oct

        # Note
        s = note_to_mml_letter(n.letter, n.accidental)
        if n.duration != default_dur:
            s += str(n.duration)
        if n.dotted:
            s += "."
        if n.tied:
            s += "&"
        parts.append(s)

    return "".join(parts)


# ---------------------------------------------------------------------------
# Line splitter
# ---------------------------------------------------------------------------

def split_mml_to_lines(
    mml: str,
    voice_num: int,
    start_line: int,
    max_len: int = 200,
    line_step: int = 10,
) -> tuple[list[str], int]:
    """Split MML string into BASIC MUSIC lines.

    Returns (lines, next_line_number).
    """
    prefix_template = f'MUSIC {voice_num},"'
    suffix = '"'
    # Available space for MML content per line
    # Account for line number (up to 5 digits) + space + prefix + suffix
    max_mml = max_len - len(str(start_line + 1000)) - 1 - len(prefix_template) - len(suffix)

    # Split MML at bar markers (|) first, keeping track of octave state
    segments = mml.split("|")

    lines: list[str] = []
    line_no = start_line
    current_chunk = ""

    for i, seg in enumerate(segments):
        seg = seg.strip()
        if not seg:
            continue

        # Would adding this segment exceed the line?
        test = current_chunk + seg if not current_chunk else current_chunk + seg
        if len(test) <= max_mml:
            current_chunk = test
        else:
            # Emit current chunk
            if current_chunk:
                lines.append(f"{line_no} {prefix_template}{current_chunk}{suffix}")
                line_no += line_step
            current_chunk = seg

            # If single segment is too long, split at note boundaries
            while len(current_chunk) > max_mml:
                # Find a split point - look for uppercase note letters
                split_at = max_mml
                for j in range(max_mml - 1, max_mml // 2, -1):
                    if current_chunk[j] in "ABCDEFGRO<>" and (j == 0 or current_chunk[j-1] != "&"):
                        split_at = j
                        break
                lines.append(f"{line_no} {prefix_template}{current_chunk[:split_at]}{suffix}")
                line_no += line_step
                current_chunk = current_chunk[split_at:]

    if current_chunk:
        lines.append(f"{line_no} {prefix_template}{current_chunk}{suffix}")
        line_no += line_step

    return lines, line_no


# ---------------------------------------------------------------------------
# BASIC file generator
# ---------------------------------------------------------------------------

def emit_basic(
    voices: list[LyVoice],
    voice_map: dict[str, int],
    instruments: dict[int, tuple[int, int, int, int, int, str]],
    title: str,
    subtitle: str,
    tempo: int,
    max_line_len: int,
    include_viz: bool,
) -> str:
    """Generate a complete NovaBASIC .bas program."""
    lines: list[str] = []
    ln = 10

    # Header
    lines.append(f"{ln} REM ================================")
    ln += 10
    lines.append(f"{ln} REM  {title}")
    ln += 10
    lines.append(f"{ln} REM  {subtitle} - SID CHIPTUNE")
    ln += 10
    lines.append(f"{ln} REM  NOVABASIC MML MUSIC ENGINE")
    ln += 10
    lines.append(f"{ln} REM ================================")
    ln += 10

    # Screen setup
    lines.append(f"{ln} PRINT CHR$(12)")
    ln += 10
    lines.append(f"{ln} MODE 1:COLOR 1,0:GCLS")
    ln += 10

    # Instruments
    lines.append(f"{ln} REM -- SID INSTRUMENTS --")
    ln += 10
    for voice_num in sorted(instruments.keys()):
        wave, a, d, s, r, label = instruments[voice_num]
        lines.append(f"{ln} INSTRUMENT {voice_num},{wave},{a},{d},{s},{r}:REM {label}")
        ln += 10

    # Music lines for each voice
    is_first = True
    for voice in voices:
        voice_num = voice_map.get(voice.name, 1)
        instrument_id = voice_num
        instr_tuple = instruments.get(voice_num, DEFAULT_INSTRUMENTS[0])

        lines.append(f"{ln} REM -- VOICE {voice_num}: {voice.name.upper()} --")
        ln += 10

        mml = notes_to_mml(
            voice.notes, tempo, instrument_id, voice_num, is_first
        )
        is_first = False

        music_lines, ln = split_mml_to_lines(mml, voice_num, ln, max_line_len)
        lines.extend(music_lines)

    # Volume and play
    lines.append(f"{ln} VOLUME 15")
    ln += 10
    lines.append(f"{ln} MUSIC PLAY")
    ln += 10

    if include_viz:
        ln = _emit_visualization(lines, ln, voices, voice_map, tempo, title, subtitle)

    return "\n".join(lines) + "\n"


def _emit_visualization(
    lines: list[str],
    ln: int,
    voices: list[LyVoice],
    voice_map: dict[str, int],
    tempo: int,
    title: str,
    subtitle: str,
) -> int:
    """Emit dual-row 8-octave piano keyboard with up to 6 voice indicators.

    Layout: two keyboard strips (4 octaves each) covering the full MML
    range O0-O7. Each voice gets a colored indicator strip
    within the white key area below the black keys.

    Uses a two-pass approach: collect all lines with symbolic labels,
    then assign line numbers and resolve cross-references.
    """
    nv = len(voices)  # number of voices (1-6)

    # Colors for each voice: (highlight_white, highlight_black)
    VOICE_COLORS = [
        (10, 2),   # V1: green / dark green
        (12, 4),   # V2: light red / dark red
        (14, 6),   # V3: yellow / brown
        (9, 1),    # V4: light blue / dark blue
        (13, 5),   # V5: light magenta / dark magenta
        (11, 3),   # V6: light cyan / dark cyan
    ]

    # Collect voice numbers
    voice_nums: list[int] = []
    for voice in voices:
        voice_nums.append(voice_map.get(voice.name, 1))

    # Keyboard layout constants
    # 8 octaves = C0-B7 = 96 semitones.  Split: 4 octaves each row.
    # Top row:  C0-B3 = 28 white keys = 308px at 11px each (fits in 320)
    # Bottom row: C4-B7 = 28 white keys = 308px
    kw = 11     # white key width
    bw = 7      # black key width
    strip_h = max(2, 16 // nv)  # per-voice indicator strip height
    indicator_zone = strip_h * nv
    bh = 24     # black key height
    kh = bh + indicator_zone  # total key height
    kx = 6      # left margin
    y1 = 36     # top keyboard Y
    y2 = y1 + kh + 14  # bottom keyboard Y (gap for labels)
    nk1 = 28    # 4 octaves top (C0-B3)
    nk2 = 28    # 4 octaves bottom (C4-B7)
    # MIDI base: C0 = 12 (MML O0 C = (0+1)*12 = 12)
    midi_base = 12

    # Build visualization lines with symbolic labels
    viz: list[str] = []

    # Title
    viz.append("REM -- TITLE --")
    viz.append("GCOLOR 14:LINE 10,6,309,6:LINE 10,7,309,7")
    viz.append(f'COLOR 15,0:PRINT "    {title}"')
    viz.append(f'COLOR 14,0:PRINT "           {subtitle}"')

    # Key lookup tables
    viz.append("REM -- KEY LOOKUP TABLES --")
    viz.append("DIM KT(11),KI(11)")
    viz.append("FOR I=0 TO 11:READ KT(I),KI(I):NEXT")
    viz.append("DATA 0,0,1,0,0,1,1,1,0,2,0,3,1,3,0,4,1,4,0,5,1,5,0,6")

    # Constants
    viz.append("REM -- KEYBOARD CONSTANTS --")
    viz.append(f"KW={kw}:BW={bw}:KH={kh}:BH={bh}:SH={strip_h}")
    viz.append(f"KX={kx}:Y1={y1}:Y2={y2}")
    viz.append(f"MB={midi_base}:REM BASE MIDI C0")
    viz.append(f"N1K={nk1}:N2K={nk2}")

    # Draw both keyboards
    viz.append("REM -- DRAW KEYBOARDS --")
    viz.append("GCOLOR 1")
    viz.append("FOR I=0 TO N1K-1:X=KX+I*KW:FILL X,Y1,X+KW-2,Y1+KH:NEXT I")
    viz.append("FOR I=0 TO N2K-1:X=KX+I*KW:FILL X,Y2,X+KW-2,Y2+KH:NEXT I")
    viz.append("GCOLOR 11")
    viz.append("FOR I=0 TO N1K:X=KX+I*KW-1:LINE X,Y1,X,Y1+KH:NEXT I")
    viz.append("FOR I=0 TO N2K:X=KX+I*KW-1:LINE X,Y2,X,Y2+KH:NEXT I")

    # Black keys
    viz.append("REM -- BLACK KEYS --")
    viz.append("DIM BK(4):BK(0)=0:BK(1)=1:BK(2)=3:BK(3)=4:BK(4)=5")
    viz.append("GCOLOR 0")
    viz.append("FOR O=0 TO 3:FOR I=0 TO 4:X=KX+(O*7+BK(I))*KW+KW-BW/2")
    viz.append("FILL X,Y1,X+BW-1,Y1+BH:NEXT I:NEXT O")
    viz.append("FOR O=0 TO 3:FOR I=0 TO 4:X=KX+(O*7+BK(I))*KW+KW-BW/2")
    viz.append("FILL X,Y2,X+BW-1,Y2+BH:NEXT I:NEXT O")

    # Octave labels
    viz.append("REM -- OCTAVE LABELS --")
    row1 = (y1 + kh + 2) // 8 + 1
    row2 = (y2 + kh + 2) // 8 + 1
    viz.append("COLOR 8,0")
    viz.append(f'FOR O=0 TO 3:LOCATE 1+O*10,{row1}:PRINT "C";:PRINT O;:NEXT O')
    viz.append(f'FOR O=0 TO 3:LOCATE 1+O*10,{row2}:PRINT "C";:PRINT O+4;:NEXT O')

    # Voice legend
    legend_row = row2 + 1
    for i in range(nv):
        vn = voice_nums[i]
        wc, _bc = VOICE_COLORS[i]
        label = voices[i].name.upper()[:8]
        col = 1 + i * 13
        if col + 12 > 40:
            col = 1
            legend_row += 1
        viz.append(f'COLOR {wc},0:LOCATE {col},{legend_row}:PRINT "V{vn} {label}";')

    # Main loop
    viz.append("REM -- MAIN LOOP --")
    init_parts = ":".join(f"P{i+1}=0" for i in range(nv))
    viz.append(init_parts)
    idx_vsync = len(viz)
    viz.append("VSYNC")

    # Read all MNOTE values
    mnote_parts = ":".join(f"N{i+1}=MNOTE({voice_nums[i]})" for i in range(nv))
    viz.append(mnote_parts)

    # Per-voice update blocks
    idx_skips: list[int] = []
    for i in range(nv):
        vi = i + 1
        viz.append(f"REM -- UPDATE V{voice_nums[i]} --")
        viz.append(f"@SKIP{vi}@")
        viz.append(f"@CLR{vi}@")
        viz.append(f"@SET{vi}@")
        viz.append(f"P{vi}=N{vi}")
        idx_skips.append(len(viz))

    viz.append("@GOTO_VSYNC@")
    viz.append("REM -- MUSIC FINISHED --")
    viz.append('VOLUME 0:COLOR 7,0:LOCATE 1,24:PRINT " Finished.       ";')
    viz.append("END")

    # Key highlight subroutine
    # IN: S=MIDI note, KC=highlight color, BC=black highlight,
    #     YO=y-offset within key for this voice's strip
    # Determines which keyboard row (top/bottom) from the semitone value.
    idx_sub = len(viz)
    viz.append("REM == HIGHLIGHT KEY ==")
    viz.append("S=S-MB:IF S<0 OR S>95 THEN RETURN")
    viz.append("OC=INT(S/12):PC=S-OC*12")
    viz.append("T=KT(PC):W=KI(PC)")
    # Top row: octaves 0-3 (relative), bottom row: octaves 4-6
    viz.append("@GOTO_LOWER@")
    viz.append("REM UPPER ROW")
    viz.append("@GOTO_BLACK_U@")
    viz.append("X=KX+(OC*7+W)*KW:GCOLOR KC:FILL X+1,Y1+YO,X+KW-3,Y1+YO+SH-1")
    viz.append("RETURN")
    idx_black_u = len(viz)
    viz.append("X=KX+(OC*7+W)*KW+KW-BW/2:GCOLOR BC:FILL X+1,Y1+YO,X+BW-2,Y1+YO+SH-1")
    viz.append("RETURN")
    idx_lower = len(viz)
    viz.append("REM LOWER ROW")
    viz.append("OC=OC-4")
    viz.append("@GOTO_BLACK_L@")
    viz.append("X=KX+(OC*7+W)*KW:GCOLOR KC:FILL X+1,Y2+YO,X+KW-3,Y2+YO+SH-1")
    viz.append("RETURN")
    idx_black_l = len(viz)
    viz.append("X=KX+(OC*7+W)*KW+KW-BW/2:GCOLOR BC:FILL X+1,Y2+YO,X+BW-2,Y2+YO+SH-1")
    viz.append("RETURN")

    # Assign line numbers
    line_nums = {}
    cur = ln
    for i in range(len(viz)):
        line_nums[i] = cur
        cur += 10

    sub_ln = line_nums[idx_sub]
    vsync_ln = line_nums[idx_vsync]
    lower_ln = line_nums[idx_lower]
    black_u_ln = line_nums[idx_black_u]
    black_l_ln = line_nums[idx_black_l]

    # Build replacements
    replacements: dict[str, str] = {
        "@GOTO_VSYNC@": f"IF PLAYING THEN GOTO {vsync_ln}",
        "@GOTO_LOWER@": f"IF OC>3 THEN GOTO {lower_ln}",
        "@GOTO_BLACK_U@": f"IF T=1 THEN GOTO {black_u_ln}",
        "@GOTO_BLACK_L@": f"IF T=1 THEN GOTO {black_l_ln}",
    }

    for i in range(nv):
        vi = i + 1
        wc, bc = VOICE_COLORS[i]
        y_off = bh + 1 + i * strip_h
        skip_ln = line_nums[idx_skips[i]]

        replacements[f"@SKIP{vi}@"] = f"IF N{vi}=P{vi} THEN GOTO {skip_ln}"
        replacements[f"@CLR{vi}@"] = f"IF P{vi}>0 THEN S=P{vi}:KC=1:BC=0:YO={y_off}:GOSUB {sub_ln}"
        replacements[f"@SET{vi}@"] = f"IF N{vi}>0 THEN S=N{vi}:KC={wc}:BC={bc}:YO={y_off}:GOSUB {sub_ln}"

    for i in range(len(viz)):
        stmt = viz[i]
        if stmt in replacements:
            stmt = replacements[stmt]
        lines.append(f"{line_nums[i]} {stmt}")

    return cur


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def parse_voice_map(raw: str | None) -> dict[str, int]:
    """Parse --voices 'voiceone:1,voicetwo:2' into a dict."""
    if not raw:
        return {}
    result: dict[str, int] = {}
    for part in raw.split(","):
        name, _, num = part.strip().partition(":")
        if name and num:
            result[name.strip()] = int(num.strip())
    return result


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Convert LilyPond files to NovaBASIC MML MUSIC programs",
    )
    parser.add_argument("input", type=Path, help="Input .ly LilyPond file")
    parser.add_argument("-o", "--output", type=Path, help="Output .bas file")
    parser.add_argument("--title", default="MUSIC", help="Program title")
    parser.add_argument("--subtitle", default="", help="Subtitle (e.g. BWV number)")
    parser.add_argument("--tempo", type=int, help="Override BPM (default: from .ly or 100)")
    parser.add_argument(
        "--voices",
        help="Map voice names to numbers: 'voiceone:1,voicetwo:2'",
    )
    parser.add_argument(
        "--max-line-len", type=int, default=200,
        help="Maximum BASIC line length (default: 200)",
    )
    parser.add_argument(
        "--mml-only", action="store_true",
        help="Print raw MML to stdout instead of generating .bas",
    )
    parser.add_argument(
        "--no-viz", action="store_true",
        help="Skip piano keyboard visualization",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    input_path: Path = args.input

    if not input_path.exists():
        print(f"error: input file not found: {input_path}", file=sys.stderr)
        return 1

    text = input_path.read_text(encoding="utf-8", errors="replace")
    voices, file_tempo = parse_lilypond_file(text)

    if not voices:
        print("error: no \\relative voice blocks found", file=sys.stderr)
        return 1

    tempo = args.tempo if args.tempo else file_tempo

    # Voice mapping
    voice_map = parse_voice_map(args.voices)
    if not voice_map:
        for i, v in enumerate(voices):
            voice_map[v.name] = i + 1

    # Instrument mapping — use per-voice variants when available
    instruments: dict[int, tuple[int, int, int, int, int, str]] = {}
    instr_voice_index: dict[str, int] = {}  # track variant index per instrument
    for voice in voices:
        vn = voice_map.get(voice.name, 1)
        instr_name = voice.instrument
        if instr_name in INSTRUMENT_VARIANTS:
            idx = instr_voice_index.get(instr_name, 0)
            variants = INSTRUMENT_VARIANTS[instr_name]
            instruments[vn] = variants[min(idx, len(variants) - 1)]
            instr_voice_index[instr_name] = idx + 1
        elif instr_name in INSTRUMENT_MAP:
            instruments[vn] = INSTRUMENT_MAP[instr_name]
        elif vn - 1 < len(DEFAULT_INSTRUMENTS):
            instruments[vn] = DEFAULT_INSTRUMENTS[vn - 1]
        else:
            instruments[vn] = DEFAULT_INSTRUMENTS[0]

    if args.mml_only:
        is_first = True
        for voice in voices:
            vn = voice_map.get(voice.name, 1)
            mml = notes_to_mml(voice.notes, tempo, vn, vn, is_first)
            is_first = False
            print(f"Voice {vn} ({voice.name}):")
            print(mml)
            print()
        return 0

    if args.output is None:
        args.output = input_path.with_suffix(".bas")

    output_text = emit_basic(
        voices=voices,
        voice_map=voice_map,
        instruments=instruments,
        title=args.title,
        subtitle=args.subtitle,
        tempo=tempo,
        max_line_len=max(80, args.max_line_len),
        include_viz=not args.no_viz,
    )

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(output_text, encoding="ascii", errors="replace")

    # Summary
    total_notes = sum(
        1 for v in voices for n in v.notes if not n.bar_marker and not n.is_rest
    )
    print(f"Wrote {args.output}")
    print(f"Voices: {len(voices)}, Notes: {total_notes}, Tempo: {tempo} BPM")
    for voice in voices:
        vn = voice_map.get(voice.name, 1)
        note_count = sum(1 for n in voice.notes if not n.bar_marker and not n.is_rest)
        rest_count = sum(1 for n in voice.notes if n.is_rest)
        print(f"  Voice {vn} ({voice.name}): {note_count} notes, {rest_count} rests, instrument={voice.instrument}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
