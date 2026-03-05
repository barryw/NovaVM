"""Tests for xml2mml.py MusicXML to MML converter."""
from __future__ import annotations

import os
import tempfile
import zipfile
from pathlib import Path

import pytest
import sys
sys.path.insert(0, str(Path(__file__).parent))

from xml2mml import load_musicxml, parse_musicxml, MxlPart, split_chords, expand_ornaments, _expand_repeats, _apply_transpose, select_voices, gm_to_instrument
from xml2mml import main


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


class TestOrnaments:
    def test_mordent(self):
        xml = _make_note_xml("<notations><ornaments><mordent/></ornaments></notations>")
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        expanded = expand_ornaments(notes, key_fifths=0)
        assert len(expanded) == 3
        assert all(n.duration == 32 for n in expanded)
        # mordent: main(C) -> lower(B) -> main(C)
        assert expanded[0].letter == "c"
        assert expanded[1].letter == "b"
        assert expanded[2].letter == "c"

    def test_inverted_mordent(self):
        xml = _make_note_xml("<notations><ornaments><inverted-mordent/></ornaments></notations>")
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        expanded = expand_ornaments(notes, key_fifths=0)
        assert len(expanded) == 3
        # inverted-mordent: main(C) -> upper(D) -> main(C)
        assert expanded[0].letter == "c"
        assert expanded[1].letter == "d"
        assert expanded[2].letter == "c"

    def test_turn(self):
        xml = _make_note_xml("<notations><ornaments><turn/></ornaments></notations>")
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        expanded = expand_ornaments(notes, key_fifths=0)
        assert len(expanded) == 4
        # turn: upper(D) -> main(C) -> lower(B) -> main(C)
        assert expanded[0].letter == "d"
        assert expanded[1].letter == "c"
        assert expanded[2].letter == "b"
        assert expanded[3].letter == "c"

    def test_fermata_doubles_duration(self):
        xml = _make_note_xml("<notations><fermata/></notations>")
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].duration == 4  # quarter
        expanded = expand_ornaments(notes, key_fifths=0)
        assert len(expanded) == 1
        assert expanded[0].duration == 2  # half (doubled length)

    def test_grace_note(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
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
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert len(notes) == 2
        # Grace note should have _grace attribute
        assert getattr(notes[0], '_grace', False) is True
        assert getattr(notes[0], '_grace_slash', False) is True
        expanded = expand_ornaments(notes, key_fifths=0)
        assert len(expanded) == 2
        # Grace becomes 32nd
        assert expanded[0].duration == 32
        assert expanded[0].letter == "d"
        # Main note unchanged
        assert expanded[1].duration == 4
        assert expanded[1].letter == "c"


class TestRepeats:
    def test_simple_repeat(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <barline location="left"><repeat direction="forward"/></barline>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <barline location="right"><repeat direction="backward"/></barline>
    </measure>
    <measure number="2">
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert [n.letter for n in notes] == ["c", "c", "d"]

    def test_first_second_ending(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Test</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes><divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
      </attributes>
      <barline location="left"><repeat direction="forward"/></barline>
      <note>
        <pitch><step>C</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
    <measure number="2">
      <barline location="left"><ending number="1" type="start"/></barline>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <barline location="right">
        <ending number="1" type="stop"/>
        <repeat direction="backward"/>
      </barline>
    </measure>
    <measure number="3">
      <barline location="left"><ending number="2" type="start"/></barline>
      <note>
        <pitch><step>E</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
      <barline location="right"><ending number="2" type="stop"/></barline>
    </measure>
  </part>
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert [n.letter for n in notes] == ["c", "d", "c", "e"]


class TestTransposition:
    def test_transpose_instrument(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
<score-partwise version="4.0">
  <part-list><score-part id="P1"><part-name>Clarinet</part-name></score-part></part-list>
  <part id="P1">
    <measure number="1">
      <attributes>
        <divisions>1</divisions>
        <time><beats>4</beats><beat-type>4</beat-type></time>
        <transpose>
          <diatonic>-1</diatonic>
          <chromatic>-2</chromatic>
        </transpose>
      </attributes>
      <note>
        <pitch><step>D</step><octave>4</octave></pitch>
        <duration>1</duration><type>quarter</type>
      </note>
    </measure>
  </part>
</score-partwise>'''
        parts = _parse_simple_xml(xml)
        notes = [n for n in parts[0].voices[0] if not n.bar_marker]
        assert notes[0].letter == "c"
        assert notes[0].octave == 4
        assert notes[0].accidental == 0


from ly2mml import LyNote


class TestVoiceSelection:
    def test_auto_select_by_density(self):
        voices = {
            "V1": [LyNote("c", 0, 4, 4, False, False, False)] * 100,
            "V2": [LyNote("d", 0, 4, 4, False, False, False)] * 50,
            "V3": [LyNote("e", 0, 4, 4, False, False, False)] * 200,
        }
        selected = select_voices(voices, max_voices=2)
        names = [name for name, _ in selected]
        assert names[0] == "V3"  # 200 notes
        assert names[1] == "V1"  # 100 notes

    def test_max_voices_cap(self):
        voices = {f"V{i}": [LyNote("c", 0, 4, 4, False, False, False)] * 10 for i in range(10)}
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
        assert gm_to_instrument(128) == ""


class TestBackupForward:
    def test_two_voices_via_backup(self):
        xml = '''<?xml version="1.0" encoding="UTF-8"?>
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
</score-partwise>'''
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            assert len(parts) == 1
            assert len(parts[0].voices) == 2
            v1 = [n for n in parts[0].voices[0] if not n.bar_marker]
            v2 = [n for n in parts[0].voices[1] if not n.bar_marker]
            assert len(v1) == 2  # E5, F5
            assert len(v2) == 1  # C4
            assert v1[0].letter == "e"
            assert v2[0].letter == "c"
        finally:
            path.unlink()


class TestKeySignature:
    def test_mordent_in_g_major(self):
        xml = _make_note_xml(
            '<notations><ornaments><inverted-mordent/></ornaments></notations>',
            '<key><fifths>1</fifths></key>',
        )
        xml = xml.replace("<step>C</step>", "<step>F</step>")
        path = _write_xml(xml)
        try:
            root = load_musicxml(path)
            parts = parse_musicxml(root)
            voice = parts[0].voices[0]
            notes = [n for n in voice if not n.bar_marker]
            expanded = expand_ornaments(notes, key_fifths=1)
            real = [n for n in expanded if not n.bar_marker]
            assert len(real) == 3
            assert real[0].letter == "f"
            assert real[1].letter == "g"
            assert real[2].letter == "f"
        finally:
            path.unlink()

    def test_key_fifths_stored_on_part(self):
        xml = _make_note_xml("", '<key><fifths>-3</fifths></key>')
        parts = _parse_simple_xml(xml)
        assert parts[0].key_fifths == -3


class TestEndToEnd:
    def test_simple_xml_to_bas(self):
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
