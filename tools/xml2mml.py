"""MusicXML to MML converter."""
from __future__ import annotations

import argparse
import sys
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from xml.etree import ElementTree as ET

from ly2mml import (
    LyNote, LyVoice, notes_to_mml, emit_basic,
    INSTRUMENT_MAP, INSTRUMENT_VARIANTS, DEFAULT_INSTRUMENTS,
)

TYPE_TO_DURATION: dict[str, int] = {
    "whole": 1,
    "half": 2,
    "quarter": 4,
    "eighth": 8,
    "16th": 16,
    "32nd": 32,
    "64th": 32,
}


@dataclass
class MxlPart:
    """A parsed MusicXML part."""
    id: str
    name: str
    voices: list[list[LyNote]] = field(default_factory=list)
    midi_program: int = 0
    instrument: str = ""
    key_fifths: int = 0


def _parse_pitch(note_el: ET.Element) -> tuple[str, int, int]:
    """Extract (letter, accidental, octave) from a <note> element's <pitch> child."""
    pitch_el = note_el.find("pitch")
    if pitch_el is None:
        return ("c", 0, 4)
    step = pitch_el.findtext("step", "C").lower()
    alter = int(pitch_el.findtext("alter", "0"))
    octave = int(pitch_el.findtext("octave", "4"))
    return step, alter, octave


def _parse_duration_type(note_el: ET.Element) -> int:
    """Get MML duration number from a <note> element's <type> child."""
    type_text = note_el.findtext("type", "quarter")
    return TYPE_TO_DURATION.get(type_text, 4)


def _expand_repeats(measures: list[ET.Element]) -> list[ET.Element]:
    """Expand repeat barlines into a flat list of measures."""
    result: list[ET.Element] = []
    repeat_start = 0
    ending_1_start: int | None = None
    ending_1_end: int | None = None
    in_ending_1 = False
    in_ending_2 = False

    def _scan_barlines(measure: ET.Element) -> tuple[bool, bool, list[tuple[str, str]]]:
        """Return (has_forward, has_backward, [(ending_number, ending_type), ...])."""
        has_fwd = False
        has_bwd = False
        endings: list[tuple[str, str]] = []
        for barline in measure.findall("barline"):
            rep = barline.find("repeat")
            if rep is not None:
                d = rep.get("direction", "")
                if d == "forward":
                    has_fwd = True
                elif d == "backward":
                    has_bwd = True
            ending = barline.find("ending")
            if ending is not None:
                endings.append((ending.get("number", ""), ending.get("type", "")))
        return has_fwd, has_bwd, endings

    i = 0
    while i < len(measures):
        measure = measures[i]
        has_fwd, has_bwd, endings = _scan_barlines(measure)

        if has_fwd:
            repeat_start = i

        for end_num, end_type in endings:
            if end_type == "start" and "1" in end_num:
                in_ending_1 = True
                ending_1_start = i
            elif end_type == "start" and "2" in end_num:
                in_ending_2 = True

        if has_bwd:
            if in_ending_1:
                in_ending_1 = False
                ending_1_end = i
                result.append(measure)
                # Second pass: replay from repeat_start, skip ending 1 measures
                for j in range(repeat_start, i + 1):
                    if ending_1_start is not None and ending_1_end is not None:
                        if ending_1_start <= j <= ending_1_end:
                            continue
                    result.append(measures[j])
                ending_1_start = None
                ending_1_end = None
                i += 1
                continue
            else:
                # Simple repeat: append current measure, then replay
                result.append(measure)
                for j in range(repeat_start, i + 1):
                    result.append(measures[j])
                i += 1
                continue

        if in_ending_2:
            result.append(measure)
            for end_num, end_type in endings:
                if end_type == "stop" and "2" in end_num:
                    in_ending_2 = False
            i += 1
            continue

        result.append(measure)
        i += 1

    return result


_SEMI = {"c": 0, "d": 2, "e": 4, "f": 5, "g": 7, "a": 9, "b": 11}


def _apply_transpose(letter: str, octave: int, accidental: int, chromatic: int) -> tuple[str, int, int]:
    """Transpose a note by a chromatic interval."""
    if chromatic == 0:
        return letter, octave, accidental
    semi = _SEMI.get(letter, 0) + accidental + chromatic
    octave_adj = semi // 12
    semi = semi % 12
    new_octave = octave + octave_adj
    # Find best letter match
    best_letter, best_acc = "c", 0
    for l, s in _SEMI.items():
        if semi - s == 0:
            best_letter, best_acc = l, 0
            break
        elif semi - s == 1:
            best_letter, best_acc = l, 1
        elif semi - s == -1 and best_acc != 0:
            best_letter, best_acc = l, -1
    return best_letter, new_octave, best_acc


def parse_musicxml(root: ET.Element) -> list[MxlPart]:
    """Parse a MusicXML root element into a list of MxlPart objects."""
    # Build part metadata from <part-list>
    part_meta: dict[str, dict] = {}
    for sp in root.findall("part-list/score-part"):
        pid = sp.get("id", "")
        name = sp.findtext("part-name", "")
        midi_prog = 0
        instrument = ""
        midi_inst = sp.find(".//midi-instrument")
        if midi_inst is not None:
            prog_text = midi_inst.findtext("midi-program")
            if prog_text is not None:
                midi_prog = int(prog_text)
            inst_text = midi_inst.findtext("midi-name", "")
            if inst_text:
                instrument = inst_text
        part_meta[pid] = {"name": name, "midi_program": midi_prog, "instrument": instrument}

    parts: list[MxlPart] = []
    for part_el in root.findall("part"):
        pid = part_el.get("id", "")
        meta = part_meta.get(pid, {"name": "", "midi_program": 0, "instrument": ""})

        # voices_dict maps voice number -> list of LyNote
        voices_dict: dict[int, list[LyNote]] = {}

        # Pending expression state (applies across measures)
        pending_dynamic: str = ""
        pending_cresc: bool = False
        pending_decresc: bool = False

        # Transposition
        transpose_chromatic: int = 0

        # Key signature
        current_key_fifths: int = 0

        measures = _expand_repeats(part_el.findall("measure"))
        for measure_el in measures:
            # Track which voices are active in this measure
            measure_voices: set[int] = set()

            for el in measure_el:
                if el.tag == "direction":
                    # Check for dynamics
                    for dt in el.findall("direction-type"):
                        dyn_el = dt.find("dynamics")
                        if dyn_el is not None and len(dyn_el) > 0:
                            pending_dynamic = dyn_el[0].tag
                        wedge_el = dt.find("wedge")
                        if wedge_el is not None:
                            wtype = wedge_el.get("type", "")
                            if wtype == "crescendo":
                                pending_cresc = True
                            elif wtype == "diminuendo":
                                pending_decresc = True
                            elif wtype == "stop":
                                pending_cresc = False
                                pending_decresc = False
                    continue

                if el.tag == "attributes":
                    transpose_el = el.find("transpose")
                    if transpose_el is not None:
                        chromatic_text = transpose_el.findtext("chromatic", "0")
                        transpose_chromatic = int(chromatic_text)
                    key_el = el.find("key")
                    if key_el is not None:
                        fifths_text = key_el.findtext("fifths")
                        if fifths_text is not None:
                            current_key_fifths = int(fifths_text)
                    continue

                if el.tag != "note":
                    continue

                note_el = el

                is_chord = note_el.find("chord") is not None

                # Detect grace notes (no <duration> element in MusicXML)
                grace_el = note_el.find("grace")
                is_grace = grace_el is not None
                grace_slash = is_grace and grace_el.get("slash") == "yes"

                voice_num = int(note_el.findtext("voice", "1"))
                measure_voices.add(voice_num)

                if voice_num not in voices_dict:
                    voices_dict[voice_num] = []

                is_rest = note_el.find("rest") is not None
                # Notes without pitch or rest (e.g., unpitched percussion) → treat as rest
                if not is_rest and note_el.find("pitch") is None:
                    is_rest = True
                dotted = note_el.find("dot") is not None
                tied = any(
                    t.get("type") == "start"
                    for t in note_el.findall("tie")
                )

                if is_grace:
                    # Grace notes have no <duration>; use <type> or default to 32
                    duration = _parse_duration_type(note_el) if note_el.find("type") is not None else 32
                else:
                    duration = _parse_duration_type(note_el)

                # Parse articulations / expressions from <notations>
                staccato = False
                accent = False
                trill = False
                slur_start = False
                slur_end = False
                ornament_type: str | None = None
                has_fermata = False

                for notations_el in note_el.findall("notations"):
                    for art_el in notations_el.findall("articulations"):
                        if art_el.find("staccato") is not None:
                            staccato = True
                        if art_el.find("accent") is not None:
                            accent = True
                    for orn_el in notations_el.findall("ornaments"):
                        if orn_el.find("trill-mark") is not None:
                            trill = True
                        for orn_name in ("mordent", "inverted-mordent", "turn", "inverted-turn"):
                            if orn_el.find(orn_name) is not None:
                                ornament_type = orn_name
                    for slur_el in notations_el.findall("slur"):
                        stype = slur_el.get("type", "")
                        if stype == "start":
                            slur_start = True
                        elif stype == "stop":
                            slur_end = True
                    if notations_el.find("fermata") is not None:
                        has_fermata = True

                if is_rest:
                    ln = LyNote(
                        letter="r", accidental=0, octave=4,
                        duration=duration, dotted=dotted, tied=False,
                        is_rest=True,
                    )
                else:
                    letter, accidental, octave = _parse_pitch(note_el)
                    if transpose_chromatic != 0:
                        letter, octave, accidental = _apply_transpose(letter, octave, accidental, transpose_chromatic)
                    ln = LyNote(
                        letter=letter, accidental=accidental, octave=octave,
                        duration=duration, dotted=dotted, tied=tied,
                        is_rest=False,
                        staccato=staccato, accent=accent, trill=trill,
                        slur_start=slur_start, slur_end=slur_end,
                        dynamic=pending_dynamic,
                        cresc=pending_cresc, decresc=pending_decresc,
                    )

                ln._chord = is_chord  # type: ignore[attr-defined]
                if ornament_type:
                    ln._ornament = ornament_type  # type: ignore[attr-defined]
                if has_fermata:
                    ln._fermata = True  # type: ignore[attr-defined]
                if is_grace:
                    ln._grace = True  # type: ignore[attr-defined]
                    ln._grace_slash = grace_slash  # type: ignore[attr-defined]

                # Clear pending dynamic after first note receives it
                if pending_dynamic:
                    pending_dynamic = ""

                voices_dict[voice_num].append(ln)

            # Add bar markers after each measure for all active voices
            for v in measure_voices:
                voices_dict[v].append(LyNote(
                    letter="", accidental=0, octave=0,
                    duration=0, dotted=False, tied=False,
                    is_rest=False, bar_marker=True,
                ))

        # Sort voices by number and collect
        sorted_keys = sorted(voices_dict.keys())
        voices_list = [voices_dict[k] for k in sorted_keys]

        parts.append(MxlPart(
            id=pid,
            name=meta["name"],
            voices=voices_list,
            midi_program=meta["midi_program"],
            instrument=meta["instrument"],
            key_fifths=current_key_fifths,
        ))

    return parts


def load_musicxml(path: str | Path) -> ET.Element:
    """Load a MusicXML file (.musicxml, .xml, or .mxl) and return the root Element."""
    path = Path(path)
    suffix = path.suffix.lower()

    if suffix == ".mxl":
        return _load_mxl(path)
    elif suffix in (".musicxml", ".xml"):
        tree = ET.parse(path)
        return tree.getroot()
    else:
        raise ValueError(f"Unsupported file extension: {suffix}")


def _load_mxl(path: Path) -> ET.Element:
    """Extract and parse MusicXML from a compressed .mxl archive."""
    with zipfile.ZipFile(path, "r") as zf:
        # Try META-INF/container.xml first
        try:
            container = ET.fromstring(zf.read("META-INF/container.xml"))
            rootfile_elem = container.find(".//{urn:oasis:names:tc:opendocument:xmlns:container}rootfile")
            if rootfile_elem is None:
                # Try without namespace
                rootfile_elem = container.find(".//rootfile")
            if rootfile_elem is not None:
                root_path = rootfile_elem.get("full-path")
                if root_path:
                    return ET.fromstring(zf.read(root_path))
        except KeyError:
            pass

        # Fallback: find first .musicxml or .xml file
        for name in zf.namelist():
            lower = name.lower()
            if lower.endswith(".musicxml") or (lower.endswith(".xml") and "container" not in lower):
                return ET.fromstring(zf.read(name))

        raise ValueError("No MusicXML file found in .mxl archive")


# ---------------------------------------------------------------------------
# Ornament expansion
# ---------------------------------------------------------------------------

_SCALE_LETTERS = ["c", "d", "e", "f", "g", "a", "b"]


def _key_accidentals(fifths: int) -> dict[str, int]:
    """Return accidental adjustments for each note letter given key signature fifths."""
    sharps_order = ["f", "c", "g", "d", "a", "e", "b"]
    flats_order = ["b", "e", "a", "d", "g", "c", "f"]
    acc: dict[str, int] = {}
    if fifths > 0:
        for i in range(min(fifths, 7)):
            acc[sharps_order[i]] = 1
    elif fifths < 0:
        for i in range(min(-fifths, 7)):
            acc[flats_order[i]] = -1
    return acc


def _neighbor_note(letter, octave, accidental, direction, key_acc):
    """Get diatonic neighbor above (direction=1) or below (-1)."""
    idx = _SCALE_LETTERS.index(letter)
    new_idx = (idx + direction) % 7
    new_letter = _SCALE_LETTERS[new_idx]
    new_octave = octave
    if direction == 1 and new_idx == 0:
        new_octave += 1
    elif direction == -1 and idx == 0:
        new_octave -= 1
    new_acc = key_acc.get(new_letter, 0)
    return new_letter, new_octave, new_acc


def _make_ornament_note(letter, octave, accidental, duration=32):
    return LyNote(letter=letter, accidental=accidental, octave=octave,
                  duration=duration, dotted=False, tied=False, is_rest=False)


def expand_ornaments(notes: list[LyNote], key_fifths: int = 0) -> list[LyNote]:
    """Expand ornaments, fermatas, grace notes into plain note sequences."""
    key_acc = _key_accidentals(key_fifths)
    result = []
    for n in notes:
        # Grace note -> emit as 32nd
        if getattr(n, '_grace', False):
            result.append(LyNote(letter=n.letter, accidental=n.accidental, octave=n.octave,
                                 duration=32, dotted=False, tied=False, is_rest=n.is_rest))
            continue

        # Fermata -> double duration (halve the number)
        if getattr(n, '_fermata', False):
            new_dur = max(1, n.duration // 2)
            result.append(LyNote(letter=n.letter, accidental=n.accidental, octave=n.octave,
                                 duration=new_dur, dotted=n.dotted, tied=n.tied, is_rest=n.is_rest,
                                 staccato=n.staccato, accent=n.accent, trill=n.trill,
                                 slur_start=n.slur_start, slur_end=n.slur_end,
                                 dynamic=n.dynamic, cresc=n.cresc, decresc=n.decresc))
            continue

        ornament = getattr(n, '_ornament', None)
        if ornament and not n.is_rest and not n.bar_marker:
            upper = _neighbor_note(n.letter, n.octave, n.accidental, 1, key_acc)
            lower = _neighbor_note(n.letter, n.octave, n.accidental, -1, key_acc)
            mk = _make_ornament_note
            if ornament == "mordent":       # main->lower->main
                result += [mk(n.letter, n.octave, n.accidental), mk(*lower), mk(n.letter, n.octave, n.accidental)]
            elif ornament == "inverted-mordent":  # main->upper->main
                result += [mk(n.letter, n.octave, n.accidental), mk(*upper), mk(n.letter, n.octave, n.accidental)]
            elif ornament == "turn":        # upper->main->lower->main
                result += [mk(*upper), mk(n.letter, n.octave, n.accidental), mk(*lower), mk(n.letter, n.octave, n.accidental)]
            elif ornament == "inverted-turn":  # lower->main->upper->main
                result += [mk(*lower), mk(n.letter, n.octave, n.accidental), mk(*upper), mk(n.letter, n.octave, n.accidental)]
            continue

        result.append(n)
    return result


# ---------------------------------------------------------------------------
# Chord splitting
# ---------------------------------------------------------------------------


def _note_pitch_key(n: LyNote) -> int:
    """Return a semitone index for sorting; -1 for rests/bar markers."""
    if n.is_rest or n.bar_marker:
        return -1
    return n.octave * 12 + _SEMI.get(n.letter, 0) + n.accidental


def split_chords(notes: list[LyNote], max_sub: int = 4) -> list[list[LyNote]]:
    """Split a note list with chords into monophonic sub-voice lists.

    Returns a list of 1..max_sub note lists.  If there are no chords the
    input is returned unchanged as a single-element list.
    """
    # Build onset groups: each group is a list of notes sounding together.
    groups: list[list[LyNote]] = []
    for n in notes:
        is_chord = getattr(n, "_chord", False)
        if n.bar_marker:
            groups.append([n])
        elif is_chord and groups and not groups[-1][0].bar_marker:
            groups[-1].append(n)
        else:
            groups.append([n])

    # Check whether splitting is needed
    max_chord = max((len(g) for g in groups), default=1)
    if max_chord <= 1:
        return [notes]

    num_voices = min(max_chord, max_sub)

    # Build sub-voice lists
    sub_voices: list[list[LyNote]] = [[] for _ in range(num_voices)]
    for g in groups:
        if g[0].bar_marker:
            for sv in sub_voices:
                sv.append(g[0])
            continue

        # Sort highest pitch first
        sorted_notes = sorted(g, key=_note_pitch_key, reverse=True)

        # Assign to voices; extras dropped if > num_voices
        for i in range(num_voices):
            if i < len(sorted_notes):
                sub_voices[i].append(sorted_notes[i])
            else:
                # Fill with rest of same duration
                rest = LyNote(
                    letter="r", accidental=0, octave=4,
                    duration=sorted_notes[0].duration,
                    dotted=sorted_notes[0].dotted,
                    tied=False, is_rest=True,
                )
                sub_voices[i].append(rest)

    return sub_voices


# ---------------------------------------------------------------------------
# Voice selection
# ---------------------------------------------------------------------------


def select_voices(
    voice_map: dict[str, list[LyNote]],
    max_voices: int = 6,
) -> list[tuple[str, list[LyNote]]]:
    """Select top voices by note count (excluding bar markers and rests)."""
    counted = []
    for name, notes in voice_map.items():
        note_count = sum(1 for n in notes if not n.bar_marker and not n.is_rest)
        counted.append((note_count, name, notes))
    counted.sort(key=lambda x: (-x[0], x[1]))
    return [(name, notes) for _, name, notes in counted[:max_voices]]


# ---------------------------------------------------------------------------
# GM instrument mapping
# ---------------------------------------------------------------------------

_GM_MAP: list[tuple[range, str]] = [
    (range(1, 7), "piano"),
    (range(7, 9), "harpsichord"),
    (range(17, 25), "organ"),
    (range(25, 33), "piano"),
    (range(41, 49), "strings"),
    (range(49, 57), "strings"),
    (range(57, 61), "trumpet"),
    (range(61, 65), "trumpet"),
    (range(65, 73), "oboe"),
    (range(73, 77), "flute"),
    (range(77, 81), "recorder"),
]


def gm_to_instrument(program: int) -> str:
    for r, name in _GM_MAP:
        if program in r:
            return name
    return ""


# ---------------------------------------------------------------------------
# CLI helpers
# ---------------------------------------------------------------------------


def parse_parts_flag(raw: str | None) -> dict[str, int] | None:
    """Parse ``--parts "Violin I:1,Cello:2"`` flag into a dict."""
    if not raw:
        return None
    result: dict[str, int] = {}
    for part in raw.split(","):
        name, _, num = part.strip().rpartition(":")
        if name and num:
            result[name.strip()] = int(num.strip())
    return result


def _build_ly_voices(
    parts: list[MxlPart],
    max_voices: int,
    no_split: bool,
    explicit_parts: dict[str, int] | None,
) -> tuple[list[LyVoice], dict[str, int], dict[int, tuple[int, int, int, int, int, str]], dict[str, int]]:
    """Convert *MxlPart* objects into *LyVoice* objects ready for MML."""
    all_streams: dict[str, list[LyNote]] = {}
    stream_instruments: dict[str, str] = {}
    stream_key_fifths: dict[str, int] = {}

    for part in parts:
        for vi, voice_notes in enumerate(part.voices):
            if no_split:
                name = part.name if len(part.voices) == 1 else f"{part.name}-{vi+1}"
                all_streams[name] = voice_notes
                instr = gm_to_instrument(part.midi_program)
                stream_instruments[name] = instr or part.instrument or ""
                stream_key_fifths[name] = part.key_fifths
            else:
                sub_voices = split_chords(voice_notes)
                for si, sv in enumerate(sub_voices):
                    if len(sub_voices) == 1 and len(part.voices) == 1:
                        name = part.name
                    else:
                        name = (
                            f"{part.name}-{vi+1}.{si+1}"
                            if len(part.voices) > 1
                            else f"{part.name}-{si+1}"
                        )
                    all_streams[name] = sv
                    instr = gm_to_instrument(part.midi_program)
                    stream_instruments[name] = instr or part.instrument or ""
                    stream_key_fifths[name] = part.key_fifths

    if explicit_parts:
        selected = [(n, all_streams[n]) for n in explicit_parts if n in all_streams]
    else:
        selected = select_voices(all_streams, max_voices)

    voices: list[LyVoice] = []
    voice_map: dict[str, int] = {}
    instruments: dict[int, tuple[int, int, int, int, int, str]] = {}

    for i, (name, notes) in enumerate(selected):
        vn = explicit_parts.get(name, i + 1) if explicit_parts else i + 1
        voice_map[name] = vn
        instr_name = stream_instruments.get(name, "")
        voice = LyVoice(name=name, notes=notes, instrument=instr_name or "harpsichord")
        voices.append(voice)

        if instr_name in INSTRUMENT_VARIANTS:
            variants = INSTRUMENT_VARIANTS[instr_name]
            instruments[vn] = variants[min(i, len(variants) - 1)]
        elif instr_name in INSTRUMENT_MAP:
            instruments[vn] = INSTRUMENT_MAP[instr_name]
        elif i < len(DEFAULT_INSTRUMENTS):
            instruments[vn] = DEFAULT_INSTRUMENTS[i]
        else:
            instruments[vn] = DEFAULT_INSTRUMENTS[0]

    # Build key_fifths map for selected voices
    voice_key_fifths: dict[str, int] = {
        name: stream_key_fifths.get(name, 0) for name, _ in selected
    }

    return voices, voice_map, instruments, voice_key_fifths


# ---------------------------------------------------------------------------
# Argparse
# ---------------------------------------------------------------------------


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    """Build and parse CLI arguments."""
    p = argparse.ArgumentParser(description="MusicXML to NovaBASIC MML converter")
    p.add_argument("input", type=Path, help="Input .musicxml / .xml / .mxl file")
    p.add_argument("-o", "--output", type=Path, default=None, help="Output .bas file")
    p.add_argument("--title", default="MUSIC", help="Title for the BASIC program")
    p.add_argument("--subtitle", default="", help="Subtitle for the BASIC program")
    p.add_argument("--tempo", type=int, default=None, help="Override tempo (BPM)")
    p.add_argument("--parts", type=str, default=None,
                   help='Explicit part selection, e.g. "Violin I:1,Cello:2"')
    p.add_argument("--max-voices", type=int, default=6, help="Maximum number of voices")
    p.add_argument("--max-line-len", type=int, default=200, help="Max BASIC line length")
    p.add_argument("--mml-only", action="store_true", help="Print raw MML, no BASIC output")
    p.add_argument("--no-viz", action="store_true", help="Omit visualization code")
    p.add_argument("--no-split", action="store_true", help="Do not split chords into voices")
    return p.parse_args(argv)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main(argv: list[str] | None = None) -> int:
    """Entry point for the MusicXML-to-BASIC pipeline."""
    args = parse_args(argv)

    # Load and parse MusicXML
    root = load_musicxml(args.input)
    parts = parse_musicxml(root)
    if not parts:
        print("No parts found in input file.", file=sys.stderr)
        return 1

    # Detect tempo from MusicXML <sound tempo="..."> if not overridden
    tempo = args.tempo
    if tempo is None:
        for sound_el in root.iter("sound"):
            t = sound_el.get("tempo")
            if t is not None:
                try:
                    tempo = int(float(t))
                except ValueError:
                    pass
                break
    if tempo is None:
        tempo = 120

    # Build LyVoice objects
    explicit_parts = parse_parts_flag(args.parts)
    voices, voice_map, instruments, voice_key_fifths = _build_ly_voices(
        parts, args.max_voices, args.no_split, explicit_parts,
    )

    if not voices:
        print("No voices selected.", file=sys.stderr)
        return 1

    # Expand ornaments
    for v in voices:
        v.notes = expand_ornaments(v.notes, key_fifths=voice_key_fifths.get(v.name, 0))

    if args.mml_only:
        for v in voices:
            vn = voice_map[v.name]
            instr_tuple = instruments.get(vn, DEFAULT_INSTRUMENTS[0])
            instr_id = 0  # slot index
            mml = notes_to_mml(v.notes, tempo, instr_id, vn, vn == 1)
            print(f"Voice {vn} ({v.name}):")
            print(mml)
            print()
        return 0

    # Generate BASIC output
    basic_text = emit_basic(
        voices, voice_map, instruments,
        args.title, args.subtitle, tempo,
        args.max_line_len, not args.no_viz,
    )

    out_path = args.output or args.input.with_suffix(".bas")
    out_path.write_text(basic_text)
    print(f"Wrote {out_path} ({len(basic_text)} bytes, {len(voices)} voices, tempo={tempo})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
