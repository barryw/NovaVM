#!/usr/bin/env python3
"""nmod_pack — build a self-documenting .nmod from a module's source + binary.

A .nmod is the 16384-byte module image with a documentation trailer appended:

    [16384 B module image] "NDOC" <uint32 LE len> <UTF-8 JSON>

The JSON (the "NDOC" doc) is generated from `;@` doc-comments in the module's
`.s` source, so the docs are built from the code and cannot drift. See
docs/plans/2026-06-05-nmod-self-documenting-modules-design.md.
"""
import json
import re
import struct


def parse_annotations(text):
    """Parse `;@` doc-comments from module source into a {module, functions} dict.

    Recognised tags: module, version, brief, fn, arg, ret, effect, status.
    `;@brief`/`;@arg`/`;@ret`/`;@effect`/`;@status` attach to the most recent
    block (the module, or the function opened by the last `;@fn`).
    """
    module = {"name": None, "version": None, "brief": None, "id": None}
    functions = []
    current = module
    for raw in text.splitlines():
        line = raw.strip()
        if not line.startswith(";@"):
            continue
        body = line[2:].strip()
        if not body:
            continue
        parts = body.split(None, 1)
        tag = parts[0]
        rest = parts[1].strip() if len(parts) > 1 else ""

        if tag == "module":
            module["name"] = rest
            current = module
        elif tag == "version":
            module["version"] = rest
        elif tag == "brief":
            current["brief"] = rest
        elif tag == "fn":
            fp = rest.split(None, 1)
            name = fp[0]
            idspec = fp[1].strip() if len(fp) > 1 else name
            current = {"name": name, "idspec": idspec, "brief": None,
                       "args": [], "ret": None, "effect": None, "status": []}
            functions.append(current)
        elif tag == "arg":
            ap = rest.split(None, 2)
            current["args"].append({
                "name": ap[0] if ap else "",
                "type": ap[1] if len(ap) > 1 else "",
                "desc": ap[2] if len(ap) > 2 else "",
            })
        elif tag == "ret":
            rp = rest.split(None, 1)
            current["ret"] = {
                "type": rp[0] if rp else "void",
                "desc": rp[1] if len(rp) > 1 else "",
            }
        elif tag == "effect":
            current["effect"] = rest
        elif tag == "status":
            current["status"] = [s.strip() for s in rest.split(",") if s.strip()]

    return {"module": module, "functions": functions}


def resolve_id(idspec, symbols):
    """Resolve an id spec to an int.

    Accepts a literal (`$A4`, `0xB3`, decimal `12`) or a linker symbol name
    looked up in `symbols` (parsed from the build's .lbl). Raises ValueError on
    an unknown symbol so the build fails loudly on drift.
    """
    s = idspec.strip()
    if s.startswith("$"):
        return int(s[1:], 16)
    if s.lower().startswith("0x"):
        return int(s, 16)
    if s.isdigit():
        return int(s, 10)
    if s in symbols:
        return symbols[s]
    raise ValueError(f"unknown id spec / symbol: {idspec!r}")


_SYM_RE = re.compile(
    r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(\$[0-9A-Fa-f]+|0[xX][0-9A-Fa-f]+|\d+)\b")


def load_symbols(text):
    """Parse `NAME = $HH` / `= 0xHH` / `= NN` constant definitions (e.g. the GFN_*
    ids in libgraphics.inc) into {name: int}. Lines whose right-hand side is not a
    plain numeric literal (expressions, other symbols) are skipped."""
    syms = {}
    for line in text.splitlines():
        m = _SYM_RE.match(line)
        if not m:
            continue
        name, val = m.group(1), m.group(2)
        if val.startswith("$"):
            syms[name] = int(val[1:], 16)
        elif val.lower().startswith("0x"):
            syms[name] = int(val, 16)
        else:
            syms[name] = int(val, 10)
    return syms


MODULE_SIZE = 16384
MOD_MAGIC = b"NL"          # libabi.inc MOD_MAGIC0/1 = $4E $4C, at image offset +3


def validate(doc, image):
    """Cross-check the parsed doc against the real module binary header.

    Raises ValueError on any mismatch so the build fails loudly on drift.
    Returns the header fields {id, abiVersion, fnCount} read from the binary.
    """
    if len(image) != MODULE_SIZE:
        raise ValueError(
            f"module image must be {MODULE_SIZE} bytes, got {len(image)}")
    if image[3:5] != MOD_MAGIC:
        raise ValueError(
            f"bad module magic at +3: expected {MOD_MAGIC!r}, got {image[3:5]!r}")
    # Header fn_count is the dispatch-table SPAN (max id + 1), not a dense count:
    # sparse-dispatch modules (graphics) annotate only a subset of in-range ids.
    # Per-function id-range / duplicate checks happen in build_doc once ids are
    # resolved. validate only confirms the image is a structurally valid module.
    return {"id": image[5], "abiVersion": image[6], "fnCount": image[7]}


NDOC_MAGIC = b"NDOC"


def pack(image, doc):
    """Append the NDOC trailer: image + "NDOC" + uint32 LE len + UTF-8 JSON."""
    if len(image) != MODULE_SIZE:
        raise ValueError(
            f"module image must be {MODULE_SIZE} bytes, got {len(image)}")
    payload = json.dumps(doc, separators=(",", ":")).encode("utf-8")
    return bytes(image) + NDOC_MAGIC + struct.pack("<I", len(payload)) + payload


ABI_NOTE = ("All calls clobber A/X/Y; communicate via the mailbox "
            "(LIB_ARG*/LIB_RESULT/LIB_STATUS).")
NDOC_VERSION = 1


def build_doc(parsed, header, symbols):
    """Assemble the NDOC dict: resolve function ids, merge the binary header,
    index args, and drop optional fields that are absent."""
    m = parsed["module"]
    module = {
        "name": m["name"],
        "id": header["id"],
        "abiVersion": header["abiVersion"],
        "version": m.get("version"),
        "brief": m.get("brief"),
        "abiNote": ABI_NOTE,
    }
    functions = []
    seen = set()
    span = header["fnCount"]
    for f in parsed["functions"]:
        fid = resolve_id(f["idspec"], symbols)
        if fid < 0 or fid >= span:
            raise ValueError(
                "function %r id %d ($%02X) out of dispatch range [0, %d)"
                % (f["name"], fid, fid, span))
        if fid in seen:
            raise ValueError(
                "duplicate function id %d ($%02X) at %r" % (fid, fid, f["name"]))
        seen.add(fid)
        entry = {
            "id": fid,
            "idHex": "$%02X" % fid,
            "name": f["name"],
            "brief": f.get("brief"),
            "args": [{"i": i, "name": a["name"], "type": a["type"],
                      "desc": a["desc"]} for i, a in enumerate(f["args"])],
            "ret": f.get("ret") or {"type": "void", "desc": ""},
        }
        if f.get("effect"):
            entry["effect"] = f["effect"]
        if f.get("status"):
            entry["status"] = f["status"]
        functions.append(entry)
    return {"ndocVersion": NDOC_VERSION, "module": module, "functions": functions}


def build_nmod(src_text, image, symbols=None):
    """Full pipeline: parse `;@` doc-comments, cross-check against the binary
    header (raises on drift), build the NDOC doc, and return the .nmod bytes."""
    parsed = parse_annotations(src_text)
    header = validate(parsed, image)
    doc = build_doc(parsed, header, symbols or {})
    return pack(image, doc)


def main(argv=None):
    import argparse
    import sys

    p = argparse.ArgumentParser(
        description="Pack a module .bin + its source doc-comments into a .nmod")
    p.add_argument("--src", required=True,
                   help="module .s source (with ;@ doc-comments)")
    p.add_argument("--bin", required=True, dest="binpath",
                   help="16384-byte module image")
    p.add_argument("--out", required=True, help="output .nmod path")
    p.add_argument("--syms", action="append", default=[],
                   help="symbol source(s) for symbolic function ids "
                        "(e.g. libgraphics.inc); may be repeated")
    args = p.parse_args(argv)

    with open(args.src, "r", encoding="utf-8") as f:
        src_text = f.read()
    with open(args.binpath, "rb") as f:
        image = f.read()

    symbols = {}
    for spath in args.syms:
        with open(spath, "r", encoding="utf-8") as f:
            symbols.update(load_symbols(f.read()))

    try:
        out = build_nmod(src_text, image, symbols)
    except ValueError as e:
        print(f"nmod_pack: {args.src}: {e}", file=sys.stderr)
        return 1

    with open(args.out, "wb") as f:
        f.write(out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
