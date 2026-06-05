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
                       "ndk": None, "args": [], "ret": None,
                       "effect": None, "status": []}
            functions.append(current)
        elif tag == "ndk":
            # Machine-readable map to the wrapped NDK routine (source of truth for
            # the summary). Only meaningful inside a function block.
            current["ndk"] = rest.split(None, 1)[0] if rest else None
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


def build_doc(parsed, header, symbols, ndk_index=None):
    """Assemble the NDOC dict: resolve function ids, merge the binary header,
    index args, and drop optional fields that are absent.

    `ndk_index` (optional) maps NDK `@symbol` -> the NDK routine's doc entry. When
    a function carries a `;@ndk <symbol>` mapping the summary is single-sourced
    from the NDK (`feedback-ndk-source-of-truth`): if the function has no local
    `;@brief`, its brief is pulled from the NDK routine's `@summary`. An `;@ndk`
    symbol absent from the index raises (build fails loudly on a stale mapping).
    A `None` index embeds the mapping but performs no pull or validation."""
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

        ndk_sym = f.get("ndk")
        brief = f.get("brief")
        if ndk_sym and ndk_index is not None:
            if ndk_sym not in ndk_index:
                raise ValueError(
                    "function %r maps to unknown NDK symbol %r (not found in the "
                    "NDK sources)" % (f["name"], ndk_sym))
            if not brief:
                brief = ndk_index[ndk_sym].get("summary")

        entry = {
            "id": fid,
            "idHex": "$%02X" % fid,
            "name": f["name"],
            "brief": brief,
            "args": [{"i": i, "name": a["name"], "type": a["type"],
                      "desc": a["desc"]} for i, a in enumerate(f["args"])],
            "ret": f.get("ret") or {"type": "void", "desc": ""},
        }
        if ndk_sym:
            entry["ndk"] = ndk_sym
        if f.get("effect"):
            entry["effect"] = f["effect"]
        if f.get("status"):
            entry["status"] = f["status"]
        functions.append(entry)
    return {"ndocVersion": NDOC_VERSION, "module": module, "functions": functions}


_JTABLE_RE = re.compile(r"^\s*\.word\s+([A-Za-z_]\w*)\s*-\s*1\b")
_LABEL_RE = re.compile(r"^([A-Za-z_]\w*):")
_HELPER_JSRS = frozenset({"copy_args_to_p", "vgc_wait_cmd"})


def parse_jtable(src_text):
    """Return the module dispatch jtable's wrapper labels in fn-id order.

    The jtable is the maximal run of consecutive `.word <label>-1` lines (blank
    and comment lines do not break the run). Used to map a function id to the
    label of the wrapper that implements it — the `;@fn` name does NOT reliably
    lowercase to the label (e.g. SYS_FN_EDIT -> sys_edit)."""
    runs = []
    current = []
    for line in src_text.splitlines():
        m = _JTABLE_RE.match(line)
        if m:
            current.append(m.group(1))
            continue
        stripped = line.strip()
        if stripped == "" or stripped.startswith(";"):
            continue
        if current:
            runs.append(current)
            current = []
    if current:
        runs.append(current)
    return max(runs, key=len) if runs else []


def verify_wrapper_calls(src_text, parsed, symbols, jtable_labels):
    """Drift guard: every function with a `;@ndk <symbol>` mapping must have a
    wrapper body that actually `JSR`s that symbol. Raises ValueError otherwise so
    a lying or stale mapping cannot produce a .nmod. Helper JSRs (copy_args_to_p,
    vgc_wait_cmd) are not the mapped routine but are harmless to ignore."""
    lines = src_text.splitlines()
    label_line = {}
    for i, line in enumerate(lines):
        lm = _LABEL_RE.match(line)
        if lm:
            label_line.setdefault(lm.group(1), i)

    for f in parsed["functions"]:
        sym = f.get("ndk")
        if not sym:
            continue
        fid = resolve_id(f["idspec"], symbols)
        if fid >= len(jtable_labels):
            raise ValueError(
                "function %r id %d has no jtable entry (jtable has %d)"
                % (f["name"], fid, len(jtable_labels)))
        label = jtable_labels[fid]
        start = label_line.get(label)
        if start is None:
            raise ValueError(
                "wrapper label %r for function %r not found" % (label, f["name"]))
        end = len(lines)
        for j in range(start + 1, len(lines)):
            if _LABEL_RE.match(lines[j]):
                end = j
                break
        body = "\n".join(lines[start:end])
        jsr_re = re.compile(r"\bjsr\s+" + re.escape(sym) + r"\b", re.IGNORECASE)
        if not jsr_re.search(body):
            raise ValueError(
                "wrapper %r (function %r) does not JSR its mapped NDK routine %r"
                % (label, f["name"], sym))


def load_ndk_index(paths):
    """Build {symbol: entry} from NDK annotation sources, reusing the canonical
    parser. `entry` carries `summary` (and the other @-tags) for that routine."""
    from pathlib import Path
    from gen_runtime_abi_docs import parse_tagged_sources
    entries = parse_tagged_sources([Path(p) for p in paths])
    return {e["symbol"]: e for e in entries if e.get("symbol")}


def build_nmod(src_text, image, symbols=None, ndk_index=None):
    """Full pipeline: parse `;@` doc-comments, cross-check against the binary
    header (raises on drift), build the NDOC doc, and return the .nmod bytes.

    When `ndk_index` is given, also verify each `;@ndk` mapping is real: the
    wrapper must JSR the routine it maps to (raises on drift)."""
    parsed = parse_annotations(src_text)
    header = validate(parsed, image)
    if ndk_index is not None:
        verify_wrapper_calls(src_text, parsed, symbols or {}, parse_jtable(src_text))
    doc = build_doc(parsed, header, symbols or {}, ndk_index)
    return pack(image, doc)


def main(argv=None):
    import argparse
    import os
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
    p.add_argument("--ndk-dir", action="append", default=[], dest="ndk_dir",
                   help="NDK source dir(s) (e.g. runtime/asm); the wrapped "
                        "routine's @summary is single-sourced for each ;@ndk "
                        "mapping, and the wrapper-calls-NDK drift guard is "
                        "enforced. May be repeated.")
    args = p.parse_args(argv)

    with open(args.src, "r", encoding="utf-8") as f:
        src_text = f.read()
    with open(args.binpath, "rb") as f:
        image = f.read()

    symbols = {}
    for spath in args.syms:
        with open(spath, "r", encoding="utf-8") as f:
            symbols.update(load_symbols(f.read()))

    ndk_index = None
    if args.ndk_dir:
        import glob
        ndk_paths = []
        for d in args.ndk_dir:
            ndk_paths.extend(sorted(glob.glob(os.path.join(d, "*.inc"))))
            ndk_paths.extend(sorted(glob.glob(os.path.join(d, "*.s"))))
        ndk_index = load_ndk_index(ndk_paths)

    try:
        out = build_nmod(src_text, image, symbols, ndk_index)
    except ValueError as e:
        print(f"nmod_pack: {args.src}: {e}", file=sys.stderr)
        return 1

    with open(args.out, "wb") as f:
        f.write(out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
