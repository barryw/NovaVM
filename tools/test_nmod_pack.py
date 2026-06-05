"""Tests for nmod_pack — the .nmod (self-documenting module) build tool.

Run: pytest tools/test_nmod_pack.py
"""
import nmod_pack


def test_parses_module_and_one_function():
    src = """
; testmod
      lib_module_header MODULE_ID_TEST, LIB_ABI_VERSION, 3
;@module TESTMOD
;@version 1.0
;@brief Minimal test module.

;@fn ECHO 0
;@brief Copy ARG0 to RESULT.
;@arg x s32 the value to echo
;@ret s32 the same value
;@status LERR_OK
fn_echo:
      rts
"""
    doc = nmod_pack.parse_annotations(src)
    assert doc["module"]["name"] == "TESTMOD"
    assert doc["module"]["version"] == "1.0"
    assert doc["module"]["brief"] == "Minimal test module."
    assert len(doc["functions"]) == 1
    fn = doc["functions"][0]
    assert fn["name"] == "ECHO"
    assert fn["idspec"] == "0"
    assert fn["brief"] == "Copy ARG0 to RESULT."
    assert fn["args"] == [{"name": "x", "type": "s32", "desc": "the value to echo"}]
    assert fn["ret"] == {"type": "s32", "desc": "the same value"}
    assert fn["status"] == ["LERR_OK"]


def test_resolve_id_literal_decimal():
    assert nmod_pack.resolve_id("0", {}) == 0
    assert nmod_pack.resolve_id("12", {}) == 12


def test_resolve_id_literal_hex():
    assert nmod_pack.resolve_id("$A4", {}) == 0xA4
    assert nmod_pack.resolve_id("0xB3", {}) == 0xB3


def test_resolve_id_symbol_from_lbl():
    assert nmod_pack.resolve_id("GFN_LINE", {"GFN_LINE": 0xA4}) == 0xA4


def test_resolve_id_unknown_symbol_raises():
    import pytest
    with pytest.raises(ValueError):
        nmod_pack.resolve_id("GFN_MISSING", {"GFN_LINE": 0xA4})


def _make_bin(mod_id, abi, fn_count, size=16384):
    b = bytearray(size)
    b[0:3] = b"\x4c\x0a\xc0"   # JMP dispatch
    b[3:5] = b"NL"             # MOD_MAGIC
    b[5] = mod_id
    b[6] = abi
    b[7] = fn_count
    return bytes(b)


def _doc(n_funcs):
    return {"module": {"name": "T", "version": "1.0", "brief": "b", "id": None},
            "functions": [{"name": f"F{i}", "idspec": str(i), "brief": None,
                           "args": [], "ret": None, "effect": None, "status": []}
                          for i in range(n_funcs)]}


def test_validate_ok_returns_header():
    hdr = nmod_pack.validate(_doc(3), _make_bin(0x7F, 1, 3))
    assert hdr == {"id": 0x7F, "abiVersion": 1, "fnCount": 3}


def test_validate_fn_count_mismatch_raises():
    import pytest
    with pytest.raises(ValueError):
        nmod_pack.validate(_doc(1), _make_bin(0x7F, 1, 3))


def test_validate_bad_magic_raises():
    import pytest
    bad = bytearray(_make_bin(0x7F, 1, 3))
    bad[3] = 0x00
    with pytest.raises(ValueError):
        nmod_pack.validate(_doc(3), bytes(bad))


def test_validate_wrong_size_raises():
    import pytest
    with pytest.raises(ValueError):
        nmod_pack.validate(_doc(3), b"\x00" * 100)


def test_pack_appends_ndoc_trailer():
    import json
    import struct
    image = _make_bin(0x7F, 1, 0)
    doc = {"ndocVersion": 1, "module": {"name": "T"}}
    out = nmod_pack.pack(image, doc)
    assert out[:16384] == image                      # image untouched
    assert out[16384:16388] == b"NDOC"               # trailer magic
    length = struct.unpack("<I", out[16388:16392])[0]
    payload = out[16392:16392 + length]
    assert json.loads(payload.decode("utf-8")) == doc
    assert len(out) == 16392 + length                # nothing trailing


def test_pack_rejects_wrong_size_image():
    import pytest
    with pytest.raises(ValueError):
        nmod_pack.pack(b"\x00" * 100, {"ndocVersion": 1})


def test_build_doc_produces_schema():
    parsed = {
        "module": {"name": "TESTMOD", "version": "1.0", "brief": "Min.", "id": None},
        "functions": [{
            "name": "ECHO", "idspec": "0", "brief": "Echo.",
            "args": [{"name": "x", "type": "s32", "desc": "val"}],
            "ret": {"type": "s32", "desc": "same"},
            "effect": None, "status": ["LERR_OK"]}]}
    header = {"id": 0x7F, "abiVersion": 1, "fnCount": 1}
    doc = nmod_pack.build_doc(parsed, header, symbols={})

    assert doc["ndocVersion"] == 1
    assert doc["module"]["name"] == "TESTMOD"
    assert doc["module"]["id"] == 0x7F
    assert doc["module"]["abiVersion"] == 1
    assert doc["module"]["version"] == "1.0"
    assert "clobber" in doc["module"]["abiNote"].lower()

    fn = doc["functions"][0]
    assert fn["id"] == 0
    assert fn["idHex"] == "$00"
    assert fn["name"] == "ECHO"
    assert fn["brief"] == "Echo."
    assert fn["args"] == [{"i": 0, "name": "x", "type": "s32", "desc": "val"}]
    assert fn["ret"] == {"type": "s32", "desc": "same"}
    assert fn["status"] == ["LERR_OK"]
    assert "effect" not in fn          # omitted when absent


def test_build_doc_omits_empty_optional_fields():
    parsed = {
        "module": {"name": "M", "version": None, "brief": None, "id": None},
        "functions": [{
            "name": "F", "idspec": "$10", "brief": None,
            "args": [], "ret": None, "effect": None, "status": []}]}
    doc = nmod_pack.build_doc(parsed, {"id": 1, "abiVersion": 1, "fnCount": 1}, {})
    fn = doc["functions"][0]
    assert fn["id"] == 0x10
    assert fn["idHex"] == "$10"
    assert fn["ret"] == {"type": "void", "desc": ""}   # defaulted
    assert "effect" not in fn
    assert "status" not in fn


def test_build_nmod_end_to_end():
    import json
    import struct
    src = """
;@module TESTMOD
;@version 1.0
;@brief Minimal test module.
;@fn ECHO 0
;@brief Copy ARG0 to RESULT.
;@arg x s32 the value to echo
;@ret s32 the same value
"""
    image = _make_bin(0x7F, 1, 1)
    out = nmod_pack.build_nmod(src, image, symbols={})

    assert out[:16384] == image
    assert out[16384:16388] == b"NDOC"
    length = struct.unpack("<I", out[16388:16392])[0]
    doc = json.loads(out[16392:16392 + length].decode("utf-8"))
    assert doc["module"]["name"] == "TESTMOD"
    assert doc["module"]["id"] == 0x7F
    assert doc["module"]["abiVersion"] == 1
    assert len(doc["functions"]) == 1
    assert doc["functions"][0]["name"] == "ECHO"
    assert doc["functions"][0]["idHex"] == "$00"


def test_build_nmod_drift_fails():
    # header says 1 function, source annotates 0 → build must fail loudly
    import pytest
    with pytest.raises(ValueError):
        nmod_pack.build_nmod(";@module X\n", _make_bin(0x7F, 1, 1), symbols={})


def test_main_writes_nmod(tmp_path):
    src = tmp_path / "m.s"
    src.write_text(";@module M\n;@version 1.0\n;@fn F 0\n;@brief b\n")
    binf = tmp_path / "m.bin"
    binf.write_bytes(_make_bin(0xAB, 1, 1))
    out = tmp_path / "m.nmod"

    rc = nmod_pack.main(["--src", str(src), "--bin", str(binf), "--out", str(out)])
    assert rc == 0
    data = out.read_bytes()
    assert data[:16384] == _make_bin(0xAB, 1, 1)
    assert data[16384:16388] == b"NDOC"


def test_main_drift_returns_nonzero_and_writes_nothing(tmp_path):
    src = tmp_path / "m.s"
    src.write_text(";@module M\n")               # 0 functions annotated
    binf = tmp_path / "m.bin"
    binf.write_bytes(_make_bin(0xAB, 1, 1))      # header says 1
    out = tmp_path / "m.nmod"

    rc = nmod_pack.main(["--src", str(src), "--bin", str(binf), "--out", str(out)])
    assert rc != 0
    assert not out.exists()
