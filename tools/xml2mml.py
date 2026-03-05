"""MusicXML to MML converter."""
from __future__ import annotations

import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from xml.etree import ElementTree as ET

from ly2mml import LyNote, LyVoice  # noqa: F401 – LyVoice re-exported for convenience

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


def _parse_pitch(note_el: ET.Element) -> tuple[str, int, int]:
    """Extract (letter, accidental, octave) from a <note> element's <pitch> child."""
    pitch_el = note_el.find("pitch")
    if pitch_el is None:
        raise ValueError("No <pitch> element found")
    step = pitch_el.findtext("step", "C").lower()
    alter = int(pitch_el.findtext("alter", "0"))
    octave = int(pitch_el.findtext("octave", "4"))
    return step, alter, octave


def _parse_duration_type(note_el: ET.Element) -> int:
    """Get MML duration number from a <note> element's <type> child."""
    type_text = note_el.findtext("type", "quarter")
    return TYPE_TO_DURATION.get(type_text, 4)


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

        for measure_el in part_el.findall("measure"):
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
                dotted = note_el.find("dot") is not None
                tied = False
                tie_el = note_el.find("tie")
                if tie_el is not None and tie_el.get("type") == "start":
                    tied = True

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

_SEMI = {"c": 0, "d": 2, "e": 4, "f": 5, "g": 7, "a": 9, "b": 11}


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
