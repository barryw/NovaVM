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


def test_validate_allows_fewer_functions_than_fncount():
    # Sparse-dispatch modules (e.g. graphics: fnCount = jump-table span) annotate
    # only a subset of ids; validate must NOT require count == fnCount.
    hdr = nmod_pack.validate(_doc(2), _make_bin(0x01, 1, 219))
    assert hdr["fnCount"] == 219


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
    doc = nmod_pack.build_doc(parsed, {"id": 1, "abiVersion": 1, "fnCount": 0x20}, {})
    fn = doc["functions"][0]
    assert fn["id"] == 0x10
    assert fn["idHex"] == "$10"
    assert fn["ret"] == {"type": "void", "desc": ""}   # defaulted
    assert "effect" not in fn
    assert "status" not in fn


def _fn(name, idspec):
    return {"name": name, "idspec": idspec, "brief": None,
            "args": [], "ret": None, "effect": None, "status": []}


def test_build_doc_id_out_of_range_raises():
    import pytest
    parsed = {"module": {"name": "M", "version": None, "brief": None, "id": None},
              "functions": [_fn("F", "5")]}
    with pytest.raises(ValueError):                       # id 5 >= fnCount 3
        nmod_pack.build_doc(parsed, {"id": 1, "abiVersion": 1, "fnCount": 3}, {})


def test_build_doc_duplicate_id_raises():
    import pytest
    parsed = {"module": {"name": "M", "version": None, "brief": None, "id": None},
              "functions": [_fn("A", "1"), _fn("B", "1")]}
    with pytest.raises(ValueError):
        nmod_pack.build_doc(parsed, {"id": 1, "abiVersion": 1, "fnCount": 10}, {})


def test_build_doc_subset_ok():
    # 2 functions with in-range ids against a 219-span header is fine.
    parsed = {"module": {"name": "M", "version": "1.0", "brief": None, "id": None},
              "functions": [_fn("A", "$04"), _fn("B", "$50")]}
    doc = nmod_pack.build_doc(parsed, {"id": 1, "abiVersion": 1, "fnCount": 219}, {})
    assert [f["id"] for f in doc["functions"]] == [0x04, 0x50]


def test_load_symbols_parses_inc_constants():
    inc = ("GFN_GCLS      = $00   ; ()\n"
           "GFN_LINE      = $04\n"
           "MODULE_ID_GRAPHICS = $01\n"
           "EXPR = GFN_LINE+1\n")
    syms = nmod_pack.load_symbols(inc)
    assert syms["GFN_GCLS"] == 0x00
    assert syms["GFN_LINE"] == 0x04
    assert syms["MODULE_ID_GRAPHICS"] == 0x01
    assert "EXPR" not in syms          # non-numeric RHS skipped


def test_build_nmod_resolves_symbolic_ids():
    import json
    import struct
    src = (";@module GFX\n;@version 2.0\n"
           ";@fn GFN_LINE\n;@brief draw a line\n;@arg x0 s16 sx\n;@ret void\n")
    out = nmod_pack.build_nmod(src, _make_bin(0x01, 1, 219), {"GFN_LINE": 0x04})
    length = struct.unpack("<I", out[16388:16392])[0]
    doc = json.loads(out[16392:16392 + length].decode("utf-8"))
    assert doc["functions"][0]["id"] == 0x04
    assert doc["functions"][0]["idHex"] == "$04"
    assert doc["functions"][0]["name"] == "GFN_LINE"


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


def test_build_nmod_id_out_of_range_fails():
    # an annotated id outside the dispatch-table span must fail loudly
    import pytest
    src = ";@module X\n;@fn F 5\n;@brief b\n"
    with pytest.raises(ValueError):
        nmod_pack.build_nmod(src, _make_bin(0x7F, 1, 3), symbols={})  # id 5 >= 3


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
    src.write_text(";@module M\n;@fn F 5\n;@brief b\n")   # id 5
    binf = tmp_path / "m.bin"
    binf.write_bytes(_make_bin(0xAB, 1, 3))              # span 3 → 5 out of range
    out = tmp_path / "m.nmod"

    rc = nmod_pack.main(["--src", str(src), "--bin", str(binf), "--out", str(out)])
    assert rc != 0
    assert not out.exists()


def test_main_with_syms_resolves_symbolic_ids(tmp_path):
    import json
    import struct
    src = tmp_path / "g.s"
    src.write_text(";@module GFX\n;@version 1.0\n;@fn GFN_LINE\n;@brief draw\n;@ret void\n")
    inc = tmp_path / "g.inc"
    inc.write_text("GFN_GCLS = $00\nGFN_LINE = $04\n")
    binf = tmp_path / "g.bin"
    binf.write_bytes(_make_bin(0x01, 1, 219))
    out = tmp_path / "g.nmod"

    rc = nmod_pack.main(["--src", str(src), "--bin", str(binf),
                         "--out", str(out), "--syms", str(inc)])
    assert rc == 0
    data = out.read_bytes()
    length = struct.unpack("<I", data[16388:16392])[0]
    doc = json.loads(data[16392:16392 + length].decode("utf-8"))
    assert doc["functions"][0]["id"] == 0x04
    assert doc["functions"][0]["name"] == "GFN_LINE"


# ---------------------------------------------------------------------------
# docs-from-NDK: ;@ndk mapping, summary pull, and wrapper-calls-NDK drift guard.
# ---------------------------------------------------------------------------

def test_parse_annotations_reads_ndk_tag():
    src = (";@module M\n;@fn SND_TONE\n;@ndk audio_tone\n"
           ";@arg freq u16 freq word (ARG0)\nsnd_tone:\n      rts\n")
    doc = nmod_pack.parse_annotations(src)
    fn = doc["functions"][0]
    assert fn["ndk"] == "audio_tone"
    # mailbox arg bindings stay in the module — not pulled from the NDK
    assert fn["args"] == [{"name": "freq", "type": "u16", "desc": "freq word (ARG0)"}]


def test_build_doc_pulls_brief_from_ndk_summary_when_absent():
    parsed = {"module": {"name": "S", "version": "1.0", "brief": None, "id": None},
              "functions": [{"name": "SND_TONE", "idspec": "0", "brief": None,
                             "ndk": "audio_tone", "args": [], "ret": None,
                             "effect": None, "status": []}]}
    ndk = {"audio_tone": {"summary": "Play a sawtooth tone for a number of frames."}}
    doc = nmod_pack.build_doc(parsed, {"id": 2, "abiVersion": 1, "fnCount": 3},
                              symbols={}, ndk_index=ndk)
    fn = doc["functions"][0]
    assert fn["brief"] == "Play a sawtooth tone for a number of frames."
    assert fn["ndk"] == "audio_tone"          # provenance embedded


def test_build_doc_local_brief_overrides_ndk_summary():
    parsed = {"module": {"name": "S", "version": "1.0", "brief": None, "id": None},
              "functions": [{"name": "F", "idspec": "0", "brief": "Local override.",
                             "ndk": "audio_tone", "args": [], "ret": None,
                             "effect": None, "status": []}]}
    ndk = {"audio_tone": {"summary": "NDK summary."}}
    doc = nmod_pack.build_doc(parsed, {"id": 2, "abiVersion": 1, "fnCount": 3},
                              symbols={}, ndk_index=ndk)
    fn = doc["functions"][0]
    assert fn["brief"] == "Local override."
    assert fn["ndk"] == "audio_tone"


def test_build_doc_unknown_ndk_symbol_raises():
    import pytest
    parsed = {"module": {"name": "S", "version": None, "brief": None, "id": None},
              "functions": [{"name": "F", "idspec": "0", "brief": None,
                             "ndk": "audio_missing", "args": [], "ret": None,
                             "effect": None, "status": []}]}
    with pytest.raises(ValueError):
        nmod_pack.build_doc(parsed, {"id": 2, "abiVersion": 1, "fnCount": 3},
                            symbols={}, ndk_index={"audio_tone": {"summary": "x"}})


def test_build_doc_without_ndk_index_embeds_mapping_but_does_not_pull():
    parsed = {"module": {"name": "S", "version": None, "brief": None, "id": None},
              "functions": [{"name": "F", "idspec": "0", "brief": None,
                             "ndk": "audio_tone", "args": [], "ret": None,
                             "effect": None, "status": []}]}
    doc = nmod_pack.build_doc(parsed, {"id": 2, "abiVersion": 1, "fnCount": 3},
                              symbols={})                      # ndk_index defaults None
    fn = doc["functions"][0]
    assert fn["ndk"] == "audio_tone"
    assert fn["brief"] is None                                 # no NDK to pull from


def test_parse_jtable_returns_labels_in_fnid_order():
    src = ("dispatch:\n      rts\n"
           "snd_jtable:\n"
           "      .word   snd_tone-1      ; $00\n"
           "      .word   snd_noise-1     ; $01\n"
           "      .word   snd_volume-1    ; $02\n"
           "snd_tone:\n      rts\n")
    assert nmod_pack.parse_jtable(src) == ["snd_tone", "snd_noise", "snd_volume"]


def test_verify_wrapper_calls_passes_when_wrapper_jsrs_symbol():
    src = ("snd_jtable:\n      .word snd_tone-1\n"
           "snd_tone:\n      LDA LIB_ARG0\n      JSR audio_tone\n      RTS\n")
    parsed = nmod_pack.parse_annotations(";@fn SND_TONE 0\n;@ndk audio_tone\n")
    jtable = nmod_pack.parse_jtable(src)
    nmod_pack.verify_wrapper_calls(src, parsed, symbols={}, jtable_labels=jtable)


def test_verify_wrapper_calls_raises_when_wrapper_missing_jsr():
    import pytest
    # wrapper JSRs a DIFFERENT routine than the one it claims to map to
    src = ("snd_jtable:\n      .word snd_tone-1\n"
           "snd_tone:\n      JSR audio_noise\n      RTS\n")
    parsed = nmod_pack.parse_annotations(";@fn SND_TONE 0\n;@ndk audio_tone\n")
    jtable = nmod_pack.parse_jtable(src)
    with pytest.raises(ValueError):
        nmod_pack.verify_wrapper_calls(src, parsed, symbols={}, jtable_labels=jtable)


def test_build_nmod_with_ndk_index_pulls_summary_and_verifies_jsr():
    import json
    import struct
    src = (";@module SND\n;@version 1.0\n"
           ";@fn SND_TONE 0\n;@ndk audio_tone\n;@arg freq u16 f (ARG0)\n"
           "snd_jtable:\n      .word snd_tone-1\n"
           "snd_tone:\n      JSR audio_tone\n      RTS\n")
    ndk = {"audio_tone": {"summary": "Play a tone."}}
    out = nmod_pack.build_nmod(src, _make_bin(0x02, 1, 1), symbols={}, ndk_index=ndk)
    length = struct.unpack("<I", out[16388:16392])[0]
    doc = json.loads(out[16392:16392 + length].decode("utf-8"))
    assert doc["functions"][0]["brief"] == "Play a tone."
    assert doc["functions"][0]["ndk"] == "audio_tone"


def test_build_nmod_with_ndk_index_raises_on_lying_mapping():
    import pytest
    src = (";@module SND\n;@version 1.0\n"
           ";@fn SND_TONE 0\n;@ndk audio_tone\n"
           "snd_jtable:\n      .word snd_tone-1\n"
           "snd_tone:\n      JSR audio_noise\n      RTS\n")   # body calls audio_noise
    ndk = {"audio_tone": {"summary": "Play a tone."},
           "audio_noise": {"summary": "Noise."}}
    with pytest.raises(ValueError):
        nmod_pack.build_nmod(src, _make_bin(0x02, 1, 1), symbols={}, ndk_index=ndk)
