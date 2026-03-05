"""Tests for xml2mml.py MusicXML to MML converter."""
from __future__ import annotations

import os
import tempfile
import zipfile
from pathlib import Path

import pytest
import sys
sys.path.insert(0, str(Path(__file__).parent))

from xml2mml import load_musicxml, parse_musicxml, MxlPart, split_chords


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
    fd, path = tempfile.mkstemp(suffix=suffix)
    os.write(fd, content.encode("utf-8"))
    os.close(fd)
    return Path(path)


def _write_mxl(content: str) -> Path:
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


def _parse_simple_xml(xml_str: str = SIMPLE_XML):
    """Helper to parse an XML string and return the list of MxlPart."""
    from xml.etree import ElementTree as ET
    root = ET.fromstring(xml_str)
    return parse_musicxml(root)


def _replace_note(xml_str: str, old: str, new: str) -> str:
    """Quick helper to swap one <note> block in SIMPLE_XML."""
    return xml_str.replace(old, new)


class TestParseNotes:
    def test_simple_scale(self):
        parts = _parse_simple_xml()
        assert len(parts) == 1
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert len(notes) == 4
        assert [n.letter for n in notes] == ["c", "d", "e", "f"]
        assert all(n.duration == 4 for n in notes)
        assert all(n.octave == 4 for n in notes)

    def test_rest(self):
        xml = SIMPLE_XML.replace(
            "<note>\n"
            "        <pitch><step>C</step><octave>4</octave></pitch>\n"
            "        <duration>1</duration>\n"
            "        <type>quarter</type>\n"
            "      </note>",
            "<note>\n"
            "        <rest/>\n"
            "        <duration>1</duration>\n"
            "        <type>quarter</type>\n"
            "      </note>",
            1,
        )
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].is_rest is True
        assert notes[1].is_rest is False

    def test_accidentals(self):
        xml = SIMPLE_XML.replace(
            "<pitch><step>C</step><octave>4</octave></pitch>",
            "<pitch><step>C</step><alter>1</alter><octave>4</octave></pitch>",
            1,
        )
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].accidental == 1
        assert notes[1].accidental == 0

    def test_dotted_note(self):
        xml = SIMPLE_XML.replace(
            "<note>\n"
            "        <pitch><step>C</step><octave>4</octave></pitch>\n"
            "        <duration>1</duration>\n"
            "        <type>quarter</type>\n"
            "      </note>",
            "<note>\n"
            "        <pitch><step>C</step><octave>4</octave></pitch>\n"
            "        <duration>1</duration>\n"
            "        <type>quarter</type>\n"
            "        <dot/>\n"
            "      </note>",
            1,
        )
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].dotted is True
        assert notes[1].dotted is False

    def test_tie(self):
        xml = SIMPLE_XML.replace(
            "<note>\n"
            "        <pitch><step>C</step><octave>4</octave></pitch>\n"
            "        <duration>1</duration>\n"
            "        <type>quarter</type>\n"
            "      </note>",
            "<note>\n"
            "        <pitch><step>C</step><octave>4</octave></pitch>\n"
            "        <duration>1</duration>\n"
            "        <type>quarter</type>\n"
            "        <tie type=\"start\"/>\n"
            "      </note>",
            1,
        )
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].tied is True
        assert notes[1].tied is False

    def test_durations(self):
        from xml2mml import TYPE_TO_DURATION
        expected = {"whole": 1, "half": 2, "quarter": 4, "eighth": 8, "16th": 16, "32nd": 32, "64th": 32}
        assert TYPE_TO_DURATION == expected

        # Also test actual parsing with different duration types
        for type_name, mml_dur in [("whole", 1), ("half", 2), ("eighth", 8), ("16th", 16), ("32nd", 32)]:
            xml = SIMPLE_XML.replace(
                "<type>quarter</type>",
                f"<type>{type_name}</type>",
            )
            parts = _parse_simple_xml(xml)
            notes = [n for n in parts[0].voices[0] if not n.bar_marker]
            assert all(n.duration == mml_dur for n in notes), f"Failed for {type_name}"


def _make_note_xml(note_inner: str, attributes: str = "") -> str:
    return f'''<?xml version="1.0" encoding="UTF-8"?>
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
</score-partwise>'''


class TestExpressions:
    def test_staccato(self):
        xml = _make_note_xml("<notations><articulations><staccato/></articulations></notations>")
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].staccato is True

    def test_trill(self):
        xml = _make_note_xml("<notations><ornaments><trill-mark/></ornaments></notations>")
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].trill is True

    def test_slur_start_stop(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
        <notations><slur type="start"/></notations>
      </note>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
        <notations><slur type="stop"/></notations>
      </note>
    </measure>
  </part>
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].slur_start is True
        assert notes[0].slur_end is False
        assert notes[1].slur_start is False
        assert notes[1].slur_end is True

    def test_dynamics(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <direction>
        <direction-type><dynamics><ff/></dynamics></direction-type>
      </direction>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].dynamic == "ff"

    def test_crescendo_wedge(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
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
        <duration>1</duration>
        <type>quarter</type>
      </note>
      <direction>
        <direction-type><wedge type="stop"/></direction-type>
      </direction>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration>
        <type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].cresc is True
        assert notes[1].cresc is False


CHORD_2_XML = '''\
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
</score-partwise>'''


CHORD_3_XML = '''\
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
</score-partwise>'''


class TestChordSplitting:
    def test_no_chords_single_voice(self):
        parts = _parse_simple_xml()
        result = split_chords(parts[0].voices[0])
        assert len(result) == 1

    def test_chord_splits_into_two_voices(self):
        parts = _parse_simple_xml(CHORD_2_XML)
        result = split_chords(parts[0].voices[0])
        assert len(result) == 2
        # Voice 0 = highest: E4 then D4
        v0_notes = [n for n in result[0] if not n.bar_marker]
        assert [n.letter for n in v0_notes] == ["e", "d"]
        # Voice 1 = lower: C4 then rest (D4 is solo)
        v1_notes = [n for n in result[1] if not n.bar_marker]
        assert v1_notes[0].letter == "c"
        assert v1_notes[1].is_rest is True

    def test_three_note_chord(self):
        parts = _parse_simple_xml(CHORD_3_XML)
        result = split_chords(parts[0].voices[0])
        assert len(result) == 3
        v0_notes = [n for n in result[0] if not n.bar_marker]
        v1_notes = [n for n in result[1] if not n.bar_marker]
        v2_notes = [n for n in result[2] if not n.bar_marker]
        assert v0_notes[0].letter == "g"
        assert v1_notes[0].letter == "e"
        assert v2_notes[0].letter == "c"
