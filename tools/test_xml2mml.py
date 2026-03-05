"""Tests for xml2mml.py MusicXML to MML converter."""
from __future__ import annotations

import os
import tempfile
import zipfile
from pathlib import Path

import pytest
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
