"""MusicXML to MML converter."""
from __future__ import annotations

import zipfile
from pathlib import Path
from xml.etree import ElementTree as ET


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
