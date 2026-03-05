# MusicXML to MML Converter Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build `tools/xml2mml.py` that converts MusicXML files to NovaBASIC MML MUSIC programs using the existing `ly2mml.py` pipeline.

**Architecture:** Parse MusicXML with `xml.etree.ElementTree`, extract parts/notes/expressions into intermediate structures, resolve into `LyNote`/`LyVoice` objects from `ly2mml.py`, then use existing `notes_to_mml()` and `emit_basic()` for output.

**Tech Stack:** Python 3, stdlib only (`xml.etree.ElementTree`, `zipfile`), imports from `tools/ly2mml.py`.

---

### Task 1: Scaffold and MXL Extraction

**Files:**
- Create: `tools/xml2mml.py`
- Create: `tools/test_xml2mml.py`

**Step 1: Write tests for MXL extraction and file loading**

```python
# tools/test_xml2mml.py
"""Tests for xml2mml.py MusicXML to MML converter."""
from __future__ import annotations

import os
import tempfile
import zipfile
from pathlib import Path

import pytest

# Allow running from repo root or tools/
import sys
sys.path.insert(0, str(Path(__file__).parent))

from xml2mml import load_musicxml


SIMPLE_XML = """\
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE score-partwise PUBLIC "-//Recordare//DTD MusicXML 4.0 Partwise//EN"
  "http://www.musicxml.org/dtds/partwise.dtd">
<score-partwise version="4.0">
  <part-list>
    <score-part id="P1">
      <part-name>Piano</part-name>
    </score-part>
  </part-list>
  <part id="P1">
    <measure number="1">
      <attributes>
        <divisions>1</divisions>
        <key><fifths>0</fifths></key>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
      <note>
        <pitch><step>E</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
      <note>
        <pitch><step>F</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>
"""


def _write_xml(content: str, suffix: str = ".musicxml") -> Path:
    """Write XML content to a temp file and return its path."""
    fd, path = tempfile.mkstemp(suffix=suffix)
    os.write(fd, content.encode("utf-8"))
    os.close(fd)
    return Path(path)


def _write_mxl(content: str) -> Path:
    """Write XML content into an .mxl (ZIP) file and return its path."""
    fd, path = tempfile.mkstemp(suffix=".mxl")
    os.close(fd)
    with zipfile.ZipFile(path, "w") as zf:
        zf.writestr("META-INF/container.xml",
            '<?xml version="1.0"?>'
            '<container><rootfiles>'
            '<rootfile full-path="score.musicxml"/>'
            '</rootfiles></container>')
        zf.writestr("score.musicxml", content)
    return Path(path)


class TestLoadMusicXML:
    def test_load_uncompressed_musicxml(self):
        path = _write_xml(SIMPLE_XML, ".musicxml")
        try:
            root = load_musicxml(path)
            assert root.tag == "score-partwise"
            parts = root.findall("part")
            assert len(parts) == 1
        finally:
            path.unlink()

    def test_load_xml_extension(self):
        path = _write_xml(SIMPLE_XML, ".xml")
        try:
            root = load_musicxml(path)
            assert root.tag == "score-partwise"
        finally:
            path.unlink()

    def test_load_mxl_compressed(self):
        path = _write_mxl(SIMPLE_XML)
        try:
            root = load_musicxml(path)
            assert root.tag == "score-partwise"
            parts = root.findall("part")
            assert len(parts) == 1
        finally:
            path.unlink()

    def test_unsupported_extension_raises(self):
        path = _write_xml(SIMPLE_XML, ".txt")
        try:
            with pytest.raises(ValueError, match="Unsupported"):
                load_musicxml(path)
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: FAIL — `xml2mml` module not found.

**Step 3: Write minimal implementation**

```python
# tools/xml2mml.py
#!/usr/bin/env python3
"""Convert MusicXML files to NovaBASIC MML MUSIC programs.

Parses MusicXML (.musicxml, .xml, .mxl) into the same LyNote/LyVoice
pipeline used by ly2mml.py, producing identical BASIC output.
"""
from __future__ import annotations

import argparse
import sys
import xml.etree.ElementTree as ET
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional


def load_musicxml(path: Path) -> ET.Element:
    """Load a MusicXML file (.musicxml, .xml, or .mxl) and return the root Element."""
    suffix = path.suffix.lower()

    if suffix == ".mxl":
        with zipfile.ZipFile(path, "r") as zf:
            # Try META-INF/container.xml first
            root_file = None
            try:
                container = ET.fromstring(zf.read("META-INF/container.xml"))
                for rf in container.iter("rootfile"):
                    fp = rf.get("full-path")
                    if fp:
                        root_file = fp
                        break
            except (KeyError, ET.ParseError):
                pass

            if root_file is None:
                # Fallback: find first .xml or .musicxml in the archive
                for name in zf.namelist():
                    if name.endswith((".musicxml", ".xml")) and not name.startswith("META-INF"):
                        root_file = name
                        break

            if root_file is None:
                raise ValueError("No MusicXML file found inside .mxl archive")

            return ET.fromstring(zf.read(root_file))

    elif suffix in (".musicxml", ".xml"):
        tree = ET.parse(str(path))
        return tree.getroot()

    else:
        raise ValueError(f"Unsupported file extension '{suffix}'. Use .musicxml, .xml, or .mxl")
```

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All 4 tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): scaffold xml2mml.py with MXL/XML file loading"
```

---

### Task 2: Core Note Parsing

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write tests for basic note parsing**

Add to `tools/test_xml2mml.py`:

```python
from xml2mml import load_musicxml, parse_musicxml, MxlPart


class TestParseNotes:
    def test_simple_scale(self):
        """Parse C-D-E-F quarter notes."""
        path = _write_xml(SIMPLE_XML)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            assert len(parts) == 1
            part = parts[0]
            assert part.name == "Piano"

            # Should have one voice stream with 4 notes
            assert len(part.voices) >= 1
            voice = part.voices[0]
            notes = [n for n in voice if not n.bar_marker]
            assert len(notes) == 4
            assert notes[0].letter == "c"
            assert notes[0].octave == 4
            assert notes[0].duration == 4  # quarter
            assert notes[1].letter == "d"
            assert notes[2].letter == "e"
            assert notes[3].letter == "f"
        finally:
            path.unlink()

    def test_rest(self):
        xml = SIMPLE_XML.replace(
            '<note>\n        <pitch><step>D</step><octave>4</octave></pitch>',
            '<note>\n        <rest/>',
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            assert notes[1].is_rest is True
        finally:
            path.unlink()

    def test_accidentals(self):
        xml = SIMPLE_XML.replace(
            "<step>C</step><octave>4</octave>",
            "<step>F</step><alter>1</alter><octave>4</octave>",
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            assert notes[0].letter == "f"
            assert notes[0].accidental == 1
        finally:
            path.unlink()

    def test_dotted_note(self):
        xml = SIMPLE_XML.replace(
            """<note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>""",
            """<note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>2</duration>
        <type>half</type>
        <dot/>
      </note>""",
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            assert notes[0].dotted is True
            assert notes[0].duration == 2  # half
        finally:
            path.unlink()

    def test_tie(self):
        xml = SIMPLE_XML.replace(
            """<note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>""",
            """<note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
        <tie type="start"/>
      </note>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
        <tie type="stop"/>
      </note>""",
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            assert notes[0].tied is True
        finally:
            path.unlink()

    def test_durations(self):
        """Test whole, half, eighth, 16th, 32nd note type mapping."""
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>8</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>32</duration><type>whole</type></note>
    </measure>
    <measure number="2">
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>16</duration><type>half</type></note>
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>4</duration><type>eighth</type></note>
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>2</duration><type>16th</type></note>
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>32nd</type></note>
      <note><rest/><duration>9</duration><type>quarter</type></note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker and not n.is_rest]
            assert notes[0].duration == 1   # whole
            assert notes[1].duration == 2   # half
            assert notes[2].duration == 8   # eighth
            assert notes[3].duration == 16  # 16th
            assert notes[4].duration == 32  # 32nd
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestParseNotes -v`
Expected: FAIL — `parse_musicxml` not defined.

**Step 3: Write the note parser**

Add to `tools/xml2mml.py`:

```python
from ly2mml import LyNote, LyVoice

# MusicXML type name → MML duration number
TYPE_TO_DURATION: dict[str, int] = {
    "whole": 1, "half": 2, "quarter": 4, "eighth": 8,
    "16th": 16, "32nd": 32, "64th": 32,  # clamp 64th to 32nd
}


@dataclass
class MxlPart:
    """A parsed MusicXML part with voice streams."""
    id: str
    name: str
    voices: list[list[LyNote]] = field(default_factory=list)
    midi_program: int = 0
    instrument: str = ""


def _parse_pitch(note_el: ET.Element) -> tuple[str, int, int]:
    """Extract (letter, accidental, octave) from a <note> element's <pitch>."""
    pitch = note_el.find("pitch")
    if pitch is None:
        return ("c", 0, 4)
    step = (pitch.findtext("step") or "C").strip().lower()
    alter = int(float(pitch.findtext("alter") or "0"))
    octave = int(pitch.findtext("octave") or "4")
    return (step, alter, octave)


def _parse_duration_type(note_el: ET.Element) -> int:
    """Get MML duration from <type> element."""
    type_text = note_el.findtext("type") or "quarter"
    return TYPE_TO_DURATION.get(type_text.strip(), 4)


def parse_musicxml(root: ET.Element) -> list[MxlPart]:
    """Parse a MusicXML score-partwise root into MxlPart objects."""
    # Build part name lookup from <part-list>
    part_names: dict[str, str] = {}
    part_midi: dict[str, int] = {}
    for sp in root.iter("score-part"):
        pid = sp.get("id", "")
        part_names[pid] = sp.findtext("part-name") or pid
        # MIDI program
        mi = sp.find("midi-instrument")
        if mi is not None:
            prog = mi.findtext("midi-program")
            if prog:
                part_midi[pid] = int(prog)

    parts: list[MxlPart] = []

    for part_el in root.findall("part"):
        pid = part_el.get("id", "")
        part = MxlPart(
            id=pid,
            name=part_names.get(pid, pid),
            midi_program=part_midi.get(pid, 0),
        )

        # Collect notes per MusicXML voice number
        voice_notes: dict[int, list[LyNote]] = {}

        for measure in part_el.findall("measure"):
            # Track attributes
            attr = measure.find("attributes")
            if attr is not None:
                div = attr.findtext("divisions")
                if div:
                    pass  # divisions tracked but not needed for type-based duration

            # Add bar marker to all active voices
            for vn in voice_notes:
                voice_notes[vn].append(LyNote(
                    letter="c", accidental=0, octave=4,
                    duration=4, dotted=False, tied=False,
                    is_rest=False, bar_marker=True,
                ))

            for note_el in measure.findall("note"):
                # Voice number (MusicXML uses 1-based)
                voice_num = int(note_el.findtext("voice") or "1")

                if voice_num not in voice_notes:
                    voice_notes[voice_num] = []

                # Rest?
                is_rest = note_el.find("rest") is not None

                # Chord? Skip for now — handled in Task 4
                is_chord = note_el.find("chord") is not None

                # Pitch
                if is_rest:
                    letter, accidental, octave = "r", 0, 4
                else:
                    letter, accidental, octave = _parse_pitch(note_el)

                # Duration type
                duration = _parse_duration_type(note_el)

                # Dot
                dotted = note_el.find("dot") is not None

                # Tie
                tied = False
                for tie_el in note_el.findall("tie"):
                    if tie_el.get("type") == "start":
                        tied = True

                ly_note = LyNote(
                    letter=letter,
                    accidental=accidental,
                    octave=octave,
                    duration=duration,
                    dotted=dotted,
                    tied=tied,
                    is_rest=is_rest,
                )

                if not is_chord:
                    voice_notes[voice_num].append(ly_note)
                # Chords stored separately — Task 4

        # Convert voice dict to list, sorted by voice number
        part.voices = [voice_notes[k] for k in sorted(voice_notes.keys())]
        parts.append(part)

    return parts
```

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): add MusicXML note parsing with pitch, duration, dots, ties"
```

---

### Task 3: Expression and Articulation Parsing

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write tests for expressions**

Add to `tools/test_xml2mml.py`:

```python
def _make_note_xml(note_inner: str, attributes: str = "") -> str:
    """Build a minimal MusicXML with one note containing custom inner XML."""
    return f"""\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
        {attributes}
      </attributes>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
        {note_inner}
      </note>
    </measure>
  </part>
</score-partwise>"""


class TestExpressions:
    def test_staccato(self):
        xml = _make_note_xml('<notations><articulations><staccato/></articulations></notations>')
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            notes = [n for n in parts[0].voices[0] if not n.bar_marker]
            assert notes[0].staccato is True
        finally:
            path.unlink()

    def test_trill(self):
        xml = _make_note_xml('<notations><ornaments><trill-mark/></ornaments></notations>')
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            notes = [n for n in parts[0].voices[0] if not n.bar_marker]
            assert notes[0].trill is True
        finally:
            path.unlink()

    def test_slur_start_stop(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
        <notations><slur type="start"/></notations>
      </note>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
        <notations><slur type="stop"/></notations>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            notes = [n for n in parts[0].voices[0] if not n.bar_marker]
            assert notes[0].slur_start is True
            assert notes[1].slur_end is True
        finally:
            path.unlink()

    def test_dynamics(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <direction placement="below">
        <direction-type><dynamics><ff/></dynamics></direction-type>
      </direction>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            notes = [n for n in parts[0].voices[0] if not n.bar_marker]
            assert notes[0].dynamic == "ff"
        finally:
            path.unlink()

    def test_crescendo_wedge(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <direction>
        <direction-type><wedge type="crescendo"/></direction-type>
      </direction>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <direction>
        <direction-type><wedge type="stop"/></direction-type>
      </direction>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            notes = [n for n in parts[0].voices[0] if not n.bar_marker]
            assert notes[0].cresc is True
            assert notes[1].cresc is False
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestExpressions -v`
Expected: FAIL — staccato/trill/slur/dynamic/cresc not parsed yet.

**Step 3: Update the note parser to extract expressions**

In `tools/xml2mml.py`, modify the note-parsing loop inside `parse_musicxml()` to extract articulations, notations, and directions. The key changes:

1. Before iterating notes in a measure, collect `<direction>` elements and associate them with the next note (by tracking pending dynamics, wedges, and tempo).

2. Inside the note loop, parse `<notations>` children:
   - `<articulations>/<staccato>` → `staccato=True`
   - `<articulations>/<accent>` → `accent=True`
   - `<ornaments>/<trill-mark>` → `trill=True`
   - `<slur type="start">` → `slur_start=True`
   - `<slur type="stop">` → `slur_end=True`

Replace the measure-processing loop in `parse_musicxml()` with:

```python
        for measure in part_el.findall("measure"):
            attr = measure.find("attributes")
            if attr is not None:
                div = attr.findtext("divisions")
                if div:
                    pass

            for vn in voice_notes:
                voice_notes[vn].append(LyNote(
                    letter="c", accidental=0, octave=4,
                    duration=4, dotted=False, tied=False,
                    is_rest=False, bar_marker=True,
                ))

            # Pending expression state for next note
            pending_dynamic = ""
            pending_cresc = False
            pending_decresc = False

            for el in measure:
                if el.tag == "direction":
                    for dt in el.findall("direction-type"):
                        # Dynamics
                        dyn = dt.find("dynamics")
                        if dyn is not None:
                            for child in dyn:
                                pending_dynamic = child.tag
                                break
                        # Wedges (crescendo / diminuendo)
                        wedge = dt.find("wedge")
                        if wedge is not None:
                            wtype = wedge.get("type", "")
                            if wtype == "crescendo":
                                pending_cresc = True
                            elif wtype == "diminuendo":
                                pending_decresc = True
                            elif wtype == "stop":
                                pending_cresc = False
                                pending_decresc = False

                elif el.tag == "note":
                    note_el = el
                    voice_num = int(note_el.findtext("voice") or "1")
                    if voice_num not in voice_notes:
                        voice_notes[voice_num] = []

                    is_rest = note_el.find("rest") is not None
                    is_chord = note_el.find("chord") is not None

                    if is_rest:
                        letter, accidental, octave = "r", 0, 4
                    else:
                        letter, accidental, octave = _parse_pitch(note_el)

                    duration = _parse_duration_type(note_el)
                    dotted = note_el.find("dot") is not None

                    tied = False
                    for tie_el in note_el.findall("tie"):
                        if tie_el.get("type") == "start":
                            tied = True

                    # Notations
                    staccato = False
                    accent = False
                    trill = False
                    slur_start = False
                    slur_end = False

                    notations = note_el.find("notations")
                    if notations is not None:
                        artic = notations.find("articulations")
                        if artic is not None:
                            if artic.find("staccato") is not None:
                                staccato = True
                            if artic.find("accent") is not None:
                                accent = True
                        orn = notations.find("ornaments")
                        if orn is not None:
                            if orn.find("trill-mark") is not None:
                                trill = True
                        for slur_el in notations.findall("slur"):
                            stype = slur_el.get("type", "")
                            if stype == "start":
                                slur_start = True
                            elif stype == "stop":
                                slur_end = True

                    ly_note = LyNote(
                        letter=letter,
                        accidental=accidental,
                        octave=octave,
                        duration=duration,
                        dotted=dotted,
                        tied=tied,
                        is_rest=is_rest,
                        staccato=staccato,
                        accent=accent,
                        trill=trill,
                        slur_start=slur_start,
                        slur_end=slur_end,
                        dynamic=pending_dynamic,
                        cresc=pending_cresc,
                        decresc=pending_decresc,
                    )

                    # Clear pending dynamic after attaching to first note
                    if pending_dynamic:
                        pending_dynamic = ""

                    if not is_chord:
                        voice_notes[voice_num].append(ly_note)
```

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): parse MusicXML expressions — dynamics, slurs, staccato, trills, wedges"
```

---

### Task 4: Chord Splitting

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write tests for chord splitting**

Add to `tools/test_xml2mml.py`:

```python
from xml2mml import split_chords


class TestChordSplitting:
    def test_no_chords_single_voice(self):
        """Notes without <chord> stay in one voice."""
        path = _write_xml(SIMPLE_XML)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice_notes = parts[0].voices[0]
            result = split_chords(voice_notes)
            assert len(result) == 1
        finally:
            path.unlink()

    def test_chord_splits_into_two_voices(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Piano</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note>
        <pitch><step>E</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <note>
        <chord/>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice_notes = parts[0].voices[0]
            result = split_chords(voice_notes)
            assert len(result) == 2
            # Voice 0 gets higher note (E), voice 1 gets lower (C)
            v0_notes = [n for n in result[0] if not n.bar_marker and not n.is_rest]
            v1_notes = [n for n in result[1] if not n.bar_marker and not n.is_rest]
            assert v0_notes[0].letter == "e"
            assert v1_notes[0].letter == "c"
            # Non-chord note D appears in voice 0 only
            assert len(v0_notes) == 2
            assert v0_notes[1].letter == "d"
        finally:
            path.unlink()

    def test_three_note_chord(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Piano</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note>
        <pitch><step>G</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <note>
        <chord/>
        <pitch><step>E</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <note>
        <chord/>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice_notes = parts[0].voices[0]
            result = split_chords(voice_notes)
            assert len(result) == 3
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestChordSplitting -v`
Expected: FAIL — `split_chords` not defined.

**Step 3: Implement chord splitting**

Add to `tools/xml2mml.py`:

```python
# Semitone value for sorting
_SEMI = {"c": 0, "d": 2, "e": 4, "f": 5, "g": 7, "a": 9, "b": 11}


def _note_pitch_key(n: LyNote) -> int:
    """Return a sortable pitch value (higher = higher pitch)."""
    if n.is_rest or n.bar_marker:
        return -1
    return n.octave * 12 + _SEMI.get(n.letter, 0) + n.accidental


def split_chords(notes: list[LyNote], max_sub: int = 4) -> list[list[LyNote]]:
    """Split a note stream containing chords into monophonic sub-voices.

    Notes with is_chord=True are grouped with the preceding non-chord note.
    Each chord group is sorted high-to-low and distributed across sub-voices.
    Bar markers are copied to all sub-voices.
    """
    # First, group notes into onset groups.
    # A chord note (has _chord flag) belongs to the same onset as the previous note.
    groups: list[list[LyNote]] = []
    current_group: list[LyNote] = []

    for n in notes:
        if n.bar_marker:
            if current_group:
                groups.append(current_group)
                current_group = []
            groups.append([n])  # bar marker as its own group
            continue
        if hasattr(n, '_chord') and n._chord:
            current_group.append(n)
        else:
            if current_group:
                groups.append(current_group)
            current_group = [n]
    if current_group:
        groups.append(current_group)

    # Check if any group has more than 1 note (actual chords)
    max_chord_size = max((len(g) for g in groups if not g[0].bar_marker), default=1)
    if max_chord_size <= 1:
        return [notes]

    # Number of sub-voices needed
    num_voices = min(max_chord_size, max_sub)
    sub_voices: list[list[LyNote]] = [[] for _ in range(num_voices)]

    for group in groups:
        if group[0].bar_marker:
            for sv in sub_voices:
                sv.append(group[0])
            continue

        # Sort high to low
        sorted_notes = sorted(group, key=_note_pitch_key, reverse=True)

        # Assign: voice 0 = highest, voice 1 = next, etc.
        for i, note in enumerate(sorted_notes):
            if i < num_voices:
                sub_voices[i].append(note)
        # Voices that didn't get a note in this group get a rest
        for i in range(len(sorted_notes), num_voices):
            rest = LyNote(
                letter="r", accidental=0, octave=4,
                duration=sorted_notes[0].duration,
                dotted=sorted_notes[0].dotted,
                tied=False, is_rest=True,
            )
            sub_voices[i].append(rest)

    return sub_voices
```

Also update `parse_musicxml()` to mark chord notes. Where the note is created, add a `_chord` attribute:

```python
                    ly_note._chord = is_chord  # type: ignore[attr-defined]

                    # Now store ALL notes (including chords) for later splitting
                    voice_notes[voice_num].append(ly_note)
```

Remove the `if not is_chord:` guard — all notes get appended, chord splitting happens after.

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): add chord splitting into monophonic sub-voices"
```

---

### Task 5: Ornaments and Grace Notes

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write tests for ornaments and grace notes**

Add to `tools/test_xml2mml.py`:

```python
from xml2mml import expand_ornaments


class TestOrnaments:
    def test_mordent(self):
        xml = _make_note_xml(
            '<notations><ornaments><mordent/></ornaments></notations>',
            '<key><fifths>0</fifths></key>',
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            expanded = expand_ornaments(notes, key_fifths=0)
            real = [n for n in expanded if not n.bar_marker]
            # Mordent: main→lower→main = 3 notes at 32nd
            assert len(real) == 3
            assert real[0].letter == "c" and real[0].duration == 32
            assert real[1].letter == "b" and real[1].duration == 32  # lower neighbor
            assert real[2].letter == "c" and real[2].duration == 32
        finally:
            path.unlink()

    def test_inverted_mordent(self):
        xml = _make_note_xml(
            '<notations><ornaments><inverted-mordent/></ornaments></notations>',
            '<key><fifths>0</fifths></key>',
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            expanded = expand_ornaments(notes, key_fifths=0)
            real = [n for n in expanded if not n.bar_marker]
            # Inverted mordent: main→upper→main
            assert len(real) == 3
            assert real[0].letter == "c" and real[0].duration == 32
            assert real[1].letter == "d" and real[1].duration == 32  # upper neighbor
            assert real[2].letter == "c" and real[2].duration == 32
        finally:
            path.unlink()

    def test_turn(self):
        xml = _make_note_xml(
            '<notations><ornaments><turn/></ornaments></notations>',
            '<key><fifths>0</fifths></key>',
        )
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            expanded = expand_ornaments(notes, key_fifths=0)
            real = [n for n in expanded if not n.bar_marker]
            # Turn: upper→main→lower→main = 4 notes
            assert len(real) == 4
            assert real[0].letter == "d"   # upper
            assert real[1].letter == "c"   # main
            assert real[2].letter == "b"   # lower
            assert real[3].letter == "c"   # main
        finally:
            path.unlink()

    def test_fermata_doubles_duration(self):
        xml = _make_note_xml('<notations><fermata/></notations>')
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            expanded = expand_ornaments(notes, key_fifths=0)
            real = [n for n in expanded if not n.bar_marker]
            # Quarter (4) doubled → half (2)
            assert real[0].duration == 2
        finally:
            path.unlink()

    def test_grace_note(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
        <key><fifths>0</fifths></key>
      </attributes>
      <note>
        <grace slash="yes"/>
        <pitch><step>D</step><octave>4</octave></pitch>
        <type>eighth</type>
      </note>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            expanded = expand_ornaments(notes, key_fifths=0)
            real = [n for n in expanded if not n.bar_marker]
            # Grace note becomes 32nd, followed by principal
            assert len(real) == 2
            assert real[0].letter == "d" and real[0].duration == 32
            assert real[1].letter == "c"
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestOrnaments -v`
Expected: FAIL — `expand_ornaments` not defined.

**Step 3: Implement ornament expansion**

Add to `tools/xml2mml.py`. First, update the parser to tag notes with ornament/grace metadata using `_ornament` and `_grace` attributes. Then add `expand_ornaments()`:

In `parse_musicxml()`, inside the notations block, add:

```python
                        # Ornaments (beyond trill)
                        if orn is not None:
                            for tag in ("mordent", "inverted-mordent", "turn", "inverted-turn"):
                                if orn.find(tag) is not None:
                                    ly_note._ornament = tag  # type: ignore[attr-defined]
                                    break
                        # Fermata
                        if notations.find("fermata") is not None:
                            ly_note._fermata = True  # type: ignore[attr-defined]
```

For grace notes, detect `<grace>` before pitch:

```python
                    grace_el = note_el.find("grace")
                    is_grace = grace_el is not None
                    if is_grace:
                        ly_note._grace = True  # type: ignore[attr-defined]
                        ly_note._grace_slash = grace_el.get("slash") == "yes"  # type: ignore[attr-defined]
```

Then the `expand_ornaments` function:

```python
# Letters in scale order for neighbor-note calculation
_SCALE_LETTERS = ["c", "d", "e", "f", "g", "a", "b"]

# Key signature: fifths → set of sharped/flatted notes
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


def _neighbor_note(letter: str, octave: int, accidental: int, direction: int,
                   key_acc: dict[str, int]) -> tuple[str, int, int]:
    """Get the diatonic neighbor note above (direction=1) or below (direction=-1)."""
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


def _make_ornament_note(letter: str, octave: int, accidental: int, duration: int = 32) -> LyNote:
    return LyNote(
        letter=letter, accidental=accidental, octave=octave,
        duration=duration, dotted=False, tied=False, is_rest=False,
    )


def expand_ornaments(notes: list[LyNote], key_fifths: int = 0) -> list[LyNote]:
    """Expand ornaments (mordent, turn, fermata, grace) into plain note sequences."""
    key_acc = _key_accidentals(key_fifths)
    result: list[LyNote] = []
    i = 0
    while i < len(notes):
        n = notes[i]

        # Grace note: emit as 32nd
        if getattr(n, '_grace', False):
            grace = LyNote(
                letter=n.letter, accidental=n.accidental, octave=n.octave,
                duration=32, dotted=False, tied=False, is_rest=n.is_rest,
            )
            result.append(grace)
            i += 1
            continue

        # Fermata: double duration (halve the duration number)
        if getattr(n, '_fermata', False):
            new_dur = max(1, n.duration // 2)
            result.append(LyNote(
                letter=n.letter, accidental=n.accidental, octave=n.octave,
                duration=new_dur, dotted=n.dotted, tied=n.tied, is_rest=n.is_rest,
                staccato=n.staccato, accent=n.accent, trill=n.trill,
                slur_start=n.slur_start, slur_end=n.slur_end,
                dynamic=n.dynamic, cresc=n.cresc, decresc=n.decresc,
            ))
            i += 1
            continue

        ornament = getattr(n, '_ornament', None)
        if ornament and not n.is_rest and not n.bar_marker:
            upper_l, upper_o, upper_a = _neighbor_note(n.letter, n.octave, n.accidental, 1, key_acc)
            lower_l, lower_o, lower_a = _neighbor_note(n.letter, n.octave, n.accidental, -1, key_acc)

            if ornament == "mordent":
                # main → lower → main
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
                result.append(_make_ornament_note(lower_l, lower_o, lower_a))
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
            elif ornament == "inverted-mordent":
                # main → upper → main
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
                result.append(_make_ornament_note(upper_l, upper_o, upper_a))
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
            elif ornament == "turn":
                # upper → main → lower → main
                result.append(_make_ornament_note(upper_l, upper_o, upper_a))
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
                result.append(_make_ornament_note(lower_l, lower_o, lower_a))
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
            elif ornament == "inverted-turn":
                # lower → main → upper → main
                result.append(_make_ornament_note(lower_l, lower_o, lower_a))
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
                result.append(_make_ornament_note(upper_l, upper_o, upper_a))
                result.append(_make_ornament_note(n.letter, n.octave, n.accidental))
            i += 1
            continue

        result.append(n)
        i += 1

    return result
```

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): add ornament expansion — mordent, turn, fermata, grace notes"
```

---

### Task 6: Repeat Expansion and Transposition

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write tests for repeats and transposition**

Add to `tools/test_xml2mml.py`:

```python
class TestRepeats:
    def test_simple_repeat(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <barline location="left"><repeat direction="forward"/></barline>
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type></note>
      <barline location="right"><repeat direction="backward"/></barline>
    </measure>
    <measure number="2">
      <note><pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type></note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker and not n.is_rest]
            # C repeated once = C, C, then D
            assert len(notes) == 3
            assert notes[0].letter == "c"
            assert notes[1].letter == "c"
            assert notes[2].letter == "d"
        finally:
            path.unlink()

    def test_first_second_ending(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <barline location="left"><repeat direction="forward"/></barline>
      <note><pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type></note>
    </measure>
    <measure number="2">
      <barline location="left"><ending number="1" type="start"/></barline>
      <note><pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type></note>
      <barline location="right"><ending number="1" type="stop"/>
        <repeat direction="backward"/></barline>
    </measure>
    <measure number="3">
      <barline location="left"><ending number="2" type="start"/></barline>
      <note><pitch><step>E</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type></note>
      <barline location="right"><ending number="2" type="stop"/></barline>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker and not n.is_rest]
            # First pass: C, D; second pass: C, E
            assert len(notes) == 4
            assert [n.letter for n in notes] == ["c", "d", "c", "e"]
        finally:
            path.unlink()


class TestTransposition:
    def test_transpose_instrument(self):
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Clarinet</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes>
        <divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
        <transpose><diatonic>-1</diatonic><chromatic>-2</chromatic></transpose>
      </attributes>
      <note><pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type></note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            # D4 transposed down 2 semitones = C4
            assert notes[0].letter == "c"
            assert notes[0].octave == 4
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestRepeats test_xml2mml.py::TestTransposition -v`
Expected: FAIL.

**Step 3: Implement repeat expansion and transposition**

Add repeat handling to `parse_musicxml()`. Before the per-measure loop, collect all measures, then expand repeats into a flat measure list:

```python
def _expand_repeats(measures: list[ET.Element]) -> list[ET.Element]:
    """Expand repeat barlines and endings into a flat measure sequence."""
    result: list[ET.Element] = []
    repeat_start = 0
    ending_measures: dict[str, list[ET.Element]] = {}
    current_ending: str | None = None

    i = 0
    while i < len(measures):
        m = measures[i]

        # Check barlines for repeat/ending markers
        for barline in m.findall("barline"):
            rep = barline.find("repeat")
            ending = barline.find("ending")
            loc = barline.get("location", "right")

            if rep is not None and rep.get("direction") == "forward":
                repeat_start = i

            if ending is not None:
                etype = ending.get("type", "")
                enum = ending.get("number", "1")
                if etype == "start":
                    current_ending = enum
                    ending_measures[enum] = []
                elif etype in ("stop", "discontinue"):
                    current_ending = None

            if rep is not None and rep.get("direction") == "backward":
                if ending_measures:
                    # We have endings — replay from repeat_start,
                    # skip ending-1 measures, add ending-2 measures
                    # First pass already added (with ending 1)
                    # Now replay repeat_start..before-ending-1, then ending 2
                    ending_nums = sorted(ending_measures.keys())
                    if len(ending_nums) >= 2:
                        # Re-add measures from repeat_start to first ending
                        first_ending_idx = None
                        for j in range(repeat_start, i + 1):
                            for bl in measures[j].findall("barline"):
                                e = bl.find("ending")
                                if e is not None and e.get("type") == "start" and e.get("number") == ending_nums[0]:
                                    first_ending_idx = j
                                    break
                            if first_ending_idx is not None:
                                break
                        if first_ending_idx is not None:
                            for j in range(repeat_start, first_ending_idx):
                                result.append(measures[j])
                        # Ending 2+ measures follow after this loop iteration
                else:
                    # Simple repeat: replay from repeat_start
                    for j in range(repeat_start, i + 1):
                        result.append(measures[j])
                ending_measures.clear()
                i += 1
                continue

        if current_ending is not None and current_ending in ending_measures:
            ending_measures[current_ending].append(m)

        result.append(m)
        i += 1

    return result
```

For transposition, add to the note parsing section:

```python
def _apply_transpose(letter: str, octave: int, accidental: int,
                     chromatic: int) -> tuple[str, int, int]:
    """Transpose a note by chromatic semitones."""
    if chromatic == 0:
        return letter, octave, accidental
    semi = _SEMI.get(letter, 0) + accidental + chromatic
    # Normalize to 0-11
    octave_adj = semi // 12
    semi = semi % 12
    new_octave = octave + octave_adj
    # Find best letter match
    best_letter = "c"
    best_acc = 0
    for l, s in _SEMI.items():
        diff = semi - s
        if diff == 0:
            best_letter = l
            best_acc = 0
            break
        elif diff == 1:
            best_letter = l
            best_acc = 1
        elif diff == -1 and best_acc != 0:
            best_letter = l
            best_acc = -1
    return best_letter, new_octave, best_acc
```

Track `transpose_chromatic` in the attributes section and apply it in `_parse_pitch` calls.

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): add repeat expansion and transposing instrument support"
```

---

### Task 7: Voice Selection and Instrument Mapping

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write tests for voice selection and GM mapping**

Add to `tools/test_xml2mml.py`:

```python
from xml2mml import select_voices, gm_to_instrument


class TestVoiceSelection:
    def test_auto_select_by_density(self):
        """Top voices by note count are selected."""
        voices = {
            "V1": [LyNote("c", 0, 4, 4, False, False, False)] * 100,
            "V2": [LyNote("d", 0, 4, 4, False, False, False)] * 50,
            "V3": [LyNote("e", 0, 4, 4, False, False, False)] * 200,
        }
        selected = select_voices(voices, max_voices=2)
        names = [name for name, _ in selected]
        assert names[0] == "V3"
        assert names[1] == "V1"

    def test_max_voices_cap(self):
        voices = {f"V{i}": [LyNote("c", 0, 4, 4, False, False, False)] * 10
                  for i in range(10)}
        selected = select_voices(voices, max_voices=6)
        assert len(selected) == 6


class TestGMMapping:
    def test_piano(self):
        assert gm_to_instrument(1) == "piano"

    def test_harpsichord(self):
        assert gm_to_instrument(7) == "harpsichord"

    def test_organ(self):
        assert gm_to_instrument(20) == "organ"

    def test_strings(self):
        assert gm_to_instrument(49) == "strings"

    def test_flute(self):
        assert gm_to_instrument(74) == "flute"

    def test_unknown_returns_default(self):
        result = gm_to_instrument(128)
        assert result == ""  # empty = use default
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestVoiceSelection test_xml2mml.py::TestGMMapping -v`
Expected: FAIL.

**Step 3: Implement voice selection and GM mapping**

Add to `tools/xml2mml.py`:

```python
def select_voices(
    voice_map: dict[str, list[LyNote]],
    max_voices: int = 6,
) -> list[tuple[str, list[LyNote]]]:
    """Select top voices by note count."""
    counted = []
    for name, notes in voice_map.items():
        note_count = sum(1 for n in notes if not n.bar_marker and not n.is_rest)
        counted.append((note_count, name, notes))
    counted.sort(key=lambda x: (-x[0], x[1]))
    return [(name, notes) for _, name, notes in counted[:max_voices]]


# General MIDI program number → ly2mml instrument name
_GM_MAP: list[tuple[range, str]] = [
    (range(1, 7), "piano"),
    (range(7, 9), "harpsichord"),
    (range(17, 25), "organ"),
    (range(25, 33), "piano"),       # guitar → piano-like
    (range(41, 49), "strings"),
    (range(49, 57), "strings"),     # ensemble strings
    (range(57, 61), "trumpet"),     # brass
    (range(61, 65), "trumpet"),
    (range(65, 73), "oboe"),        # reed
    (range(73, 77), "flute"),       # pipe
    (range(77, 81), "recorder"),
]


def gm_to_instrument(program: int) -> str:
    """Map a General MIDI program number to a ly2mml instrument name."""
    for r, name in _GM_MAP:
        if program in r:
            return name
    return ""
```

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): add voice selection by density and GM instrument mapping"
```

---

### Task 8: Full Pipeline and CLI

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write end-to-end test**

Add to `tools/test_xml2mml.py`:

```python
from xml2mml import main


class TestEndToEnd:
    def test_simple_xml_to_bas(self):
        """Full pipeline: XML → .bas file."""
        xml_path = _write_xml(SIMPLE_XML)
        out_path = xml_path.with_suffix(".bas")
        try:
            rc = main(["--title", "TEST", "--no-viz", str(xml_path), "-o", str(out_path)])
            assert rc == 0
            assert out_path.exists()
            content = out_path.read_text()
            assert "MUSIC" in content
            assert "INSTRUMENT" in content
            assert "TEST" in content
        finally:
            xml_path.unlink()
            if out_path.exists():
                out_path.unlink()

    def test_mml_only_mode(self, capsys):
        xml_path = _write_xml(SIMPLE_XML)
        try:
            rc = main(["--mml-only", str(xml_path)])
            assert rc == 0
            captured = capsys.readouterr()
            assert "Voice" in captured.out
            # MML should contain note letters
            assert "C" in captured.out or "D" in captured.out
        finally:
            xml_path.unlink()

    def test_mxl_to_bas(self):
        mxl_path = _write_mxl(SIMPLE_XML)
        out_path = mxl_path.with_suffix(".bas")
        try:
            rc = main(["--no-viz", str(mxl_path), "-o", str(out_path)])
            assert rc == 0
            assert out_path.exists()
        finally:
            mxl_path.unlink()
            if out_path.exists():
                out_path.unlink()

    def test_max_voices_flag(self):
        xml_path = _write_xml(SIMPLE_XML)
        out_path = xml_path.with_suffix(".bas")
        try:
            rc = main(["--max-voices", "2", "--no-viz", str(xml_path), "-o", str(out_path)])
            assert rc == 0
        finally:
            xml_path.unlink()
            if out_path.exists():
                out_path.unlink()
```

**Step 2: Run tests to verify they fail**

Run: `cd tools && python -m pytest test_xml2mml.py::TestEndToEnd -v`
Expected: FAIL — `main` not defined.

**Step 3: Implement the full pipeline and CLI**

Add to `tools/xml2mml.py`:

```python
from ly2mml import (
    LyNote, LyVoice, notes_to_mml, emit_basic, split_mml_to_lines,
    INSTRUMENT_MAP, INSTRUMENT_VARIANTS, DEFAULT_INSTRUMENTS,
)


def _build_ly_voices(
    parts: list[MxlPart],
    max_voices: int,
    no_split: bool,
    explicit_parts: dict[str, int] | None,
) -> tuple[list[LyVoice], dict[str, int], dict[int, tuple[int, int, int, int, int, str]], int]:
    """Convert MxlParts into LyVoices with voice mapping, instruments, and key signature.

    Returns (voices, voice_map, instruments, key_fifths).
    """
    # Collect all voice streams with names
    all_streams: dict[str, list[LyNote]] = {}
    stream_instruments: dict[str, str] = {}
    key_fifths = 0

    for part in parts:
        # Extract key signature from first measure
        for measure in []:  # key already parsed during parse_musicxml
            pass

        for vi, voice_notes in enumerate(part.voices):
            if no_split:
                name = part.name if len(part.voices) == 1 else f"{part.name}-{vi+1}"
                all_streams[name] = voice_notes
                instr = gm_to_instrument(part.midi_program)
                stream_instruments[name] = instr or part.instrument
            else:
                sub_voices = split_chords(voice_notes)
                for si, sv in enumerate(sub_voices):
                    if len(sub_voices) == 1 and len(part.voices) == 1:
                        name = part.name
                    else:
                        name = f"{part.name}-{vi+1}.{si+1}" if len(part.voices) > 1 else f"{part.name}-{si+1}"
                    all_streams[name] = sv
                    instr = gm_to_instrument(part.midi_program)
                    stream_instruments[name] = instr or part.instrument

    # Select voices
    if explicit_parts:
        selected = [(name, all_streams[name]) for name in explicit_parts if name in all_streams]
    else:
        selected = select_voices(all_streams, max_voices)

    # Build LyVoice objects and mappings
    voices: list[LyVoice] = []
    voice_map: dict[str, int] = {}
    instruments: dict[int, tuple[int, int, int, int, int, str]] = {}

    for i, (name, notes) in enumerate(selected):
        vn = explicit_parts.get(name, i + 1) if explicit_parts else i + 1
        voice_map[name] = vn

        instr_name = stream_instruments.get(name, "")
        voice = LyVoice(name=name, notes=notes, instrument=instr_name or "harpsichord")
        voices.append(voice)

        # Instrument preset
        if instr_name in INSTRUMENT_VARIANTS:
            variants = INSTRUMENT_VARIANTS[instr_name]
            instruments[vn] = variants[min(i, len(variants) - 1)]
        elif instr_name in INSTRUMENT_MAP:
            instruments[vn] = INSTRUMENT_MAP[instr_name]
        elif i < len(DEFAULT_INSTRUMENTS):
            instruments[vn] = DEFAULT_INSTRUMENTS[i]
        else:
            instruments[vn] = DEFAULT_INSTRUMENTS[0]

    return voices, voice_map, instruments, key_fifths


def parse_parts_flag(raw: str | None) -> dict[str, int] | None:
    if not raw:
        return None
    result: dict[str, int] = {}
    for part in raw.split(","):
        name, _, num = part.strip().rpartition(":")
        if name and num:
            result[name.strip()] = int(num.strip())
    return result


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Convert MusicXML files to NovaBASIC MML MUSIC programs",
    )
    parser.add_argument("input", type=Path, help="Input .musicxml, .xml, or .mxl file")
    parser.add_argument("-o", "--output", type=Path, help="Output .bas file")
    parser.add_argument("--title", default="MUSIC", help="Program title")
    parser.add_argument("--subtitle", default="", help="Subtitle")
    parser.add_argument("--tempo", type=int, help="Override BPM")
    parser.add_argument("--parts", help="Map part names to voice numbers: 'Violin I:1,Cello:2'")
    parser.add_argument("--max-voices", type=int, default=6, help="Max voices to select (default 6)")
    parser.add_argument("--max-line-len", type=int, default=200, help="Max BASIC line length")
    parser.add_argument("--mml-only", action="store_true", help="Print raw MML to stdout")
    parser.add_argument("--no-viz", action="store_true", help="Skip piano visualization")
    parser.add_argument("--no-split", action="store_true", help="Disable chord splitting (top note wins)")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    input_path: Path = args.input

    if not input_path.exists():
        print(f"error: input file not found: {input_path}", file=sys.stderr)
        return 1

    try:
        root = load_musicxml(input_path)
    except (ValueError, ET.ParseError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    parts = parse_musicxml(root)
    if not parts:
        print("error: no parts found in MusicXML", file=sys.stderr)
        return 1

    explicit_parts = parse_parts_flag(args.parts)

    voices, voice_map, instruments, key_fifths = _build_ly_voices(
        parts, args.max_voices, args.no_split, explicit_parts,
    )

    if not voices:
        print("error: no voices with notes found", file=sys.stderr)
        return 1

    # Expand ornaments in each voice
    for voice in voices:
        voice.notes = expand_ornaments(voice.notes, key_fifths)

    # Detect tempo from MusicXML
    tempo = args.tempo or 100
    if args.tempo is None:
        for part_el in root.findall("part"):
            for measure in part_el.findall("measure"):
                for direction in measure.findall("direction"):
                    sound = direction.find("sound")
                    if sound is not None and sound.get("tempo"):
                        tempo = int(float(sound.get("tempo", "100")))
                        break

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

    total_notes = sum(
        1 for v in voices for n in v.notes if not n.bar_marker and not n.is_rest
    )
    print(f"Wrote {args.output}")
    print(f"Voices: {len(voices)}, Notes: {total_notes}, Tempo: {tempo} BPM")
    for voice in voices:
        vn = voice_map.get(voice.name, 1)
        note_count = sum(1 for n in voice.notes if not n.bar_marker and not n.is_rest)
        rest_count = sum(1 for n in voice.notes if n.is_rest)
        print(f"  Voice {vn} ({voice.name}): {note_count} notes, {rest_count} rests")

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
```

**Step 4: Run tests to verify they pass**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): complete xml2mml.py pipeline with CLI, voice selection, and BASIC output"
```

---

### Task 9: Backup/Forward Multi-Voice Handling

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write test for backup/forward**

Add to `tools/test_xml2mml.py`:

```python
class TestBackupForward:
    def test_two_voices_via_backup(self):
        """Piano-style: two voices in one part using backup."""
        xml = """\
<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Piano</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note>
        <pitch><step>E</step><octave>5</octave></pitch>
        <duration>2</duration><type>half</type>
        <voice>1</voice>
      </note>
      <note>
        <pitch><step>F</step><octave>5</octave></pitch>
        <duration>2</duration><type>half</type>
        <voice>1</voice>
      </note>
      <backup><duration>4</duration></backup>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>4</duration><type>whole</type>
        <voice>2</voice>
      </note>
    </measure>
  </part>
</score-partwise>"""
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            assert len(parts) == 1
            # Two MusicXML voices → two voice streams
            assert len(parts[0].voices) == 2
            v1 = [n for n in parts[0].voices[0] if not n.bar_marker]
            v2 = [n for n in parts[0].voices[1] if not n.bar_marker]
            assert len(v1) == 2  # E5, F5
            assert len(v2) == 1  # C4
            assert v1[0].letter == "e"
            assert v2[0].letter == "c"
        finally:
            path.unlink()
```

**Step 2: Run tests to verify they fail or pass**

Run: `cd tools && python -m pytest test_xml2mml.py::TestBackupForward -v`

This test should already pass since we group notes by `<voice>` number in Task 2. If it does, confirm and move on. If not, debug the voice grouping.

**Step 3: Verify and commit**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

```bash
git add tools/test_xml2mml.py
git commit -m "test(tools): add backup/forward multi-voice test for xml2mml"
```

---

### Task 10: Key Signature Tracking and Final Polish

**Files:**
- Modify: `tools/xml2mml.py`
- Modify: `tools/test_xml2mml.py`

**Step 1: Write test for key signature affecting ornaments**

Add to `tools/test_xml2mml.py`:

```python
class TestKeySignature:
    def test_mordent_in_g_major(self):
        """Mordent on F in G major should use F# as the key dictates."""
        xml = _make_note_xml(
            '<notations><ornaments><inverted-mordent/></ornaments></notations>',
            '<key><fifths>1</fifths></key>',
        )
        # Replace C with F to test F→F# neighbor
        xml = xml.replace("<step>C</step>", "<step>F</step>")
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            # G major: F is sharped. Inverted mordent on F = F→G→F
            # (upper neighbor of F in G major is G natural)
            expanded = expand_ornaments(notes, key_fifths=1)
            real = [n for n in expanded if not n.bar_marker]
            assert len(real) == 3
            # The F should have accidental from key sig
            assert real[0].letter == "f"
            assert real[1].letter == "g"  # upper neighbor
            assert real[2].letter == "f"
        finally:
            path.unlink()
```

**Step 2: Run test**

Run: `cd tools && python -m pytest test_xml2mml.py::TestKeySignature -v`
Expected: PASS (key_fifths already flows through `expand_ornaments`).

**Step 3: Wire key signature extraction into the full pipeline**

In `parse_musicxml()`, track key fifths from `<attributes>/<key>/<fifths>` and store on the `MxlPart`:

```python
@dataclass
class MxlPart:
    id: str
    name: str
    voices: list[list[LyNote]] = field(default_factory=list)
    midi_program: int = 0
    instrument: str = ""
    key_fifths: int = 0
```

In the attributes parsing:
```python
            if attr is not None:
                key_el = attr.find("key")
                if key_el is not None:
                    fifths = key_el.findtext("fifths")
                    if fifths:
                        current_key_fifths = int(fifths)
```

Set `part.key_fifths = current_key_fifths` and pass it through to `_build_ly_voices` and `expand_ornaments`.

**Step 4: Run all tests**

Run: `cd tools && python -m pytest test_xml2mml.py -v`
Expected: All tests PASS.

**Step 5: Make the script executable and add shebang**

```bash
chmod +x tools/xml2mml.py
```

**Step 6: Commit**

```bash
git add tools/xml2mml.py tools/test_xml2mml.py
git commit -m "feat(tools): complete xml2mml.py with key signature tracking and polish"
```

---

## Summary

| Task | Component | Tests |
|------|-----------|-------|
| 1 | Scaffold + MXL extraction | 4 tests |
| 2 | Core note parsing | 6 tests |
| 3 | Expression/articulation parsing | 5 tests |
| 4 | Chord splitting | 3 tests |
| 5 | Ornaments and grace notes | 5 tests |
| 6 | Repeat expansion + transposition | 3 tests |
| 7 | Voice selection + GM mapping | 8 tests |
| 8 | Full pipeline + CLI | 4 tests |
| 9 | Backup/forward multi-voice | 1 test |
| 10 | Key signature + polish | 1 test |

**Total: 10 tasks, ~40 tests, 10 commits.**
