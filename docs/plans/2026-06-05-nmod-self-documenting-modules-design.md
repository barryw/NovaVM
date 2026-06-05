# Self-documenting modules (`.nmod`) + nova module management

**Date:** 2026-06-05
**Status:** Design (validated via brainstorming)
**Author:** Barry Walker (with Claude)

## Goal

A Nova paged-library module should be a **single, self-describing artifact**. Its
documentation — module version, and per-function name, parameters, return value,
side effects, and error codes — is **generated from the module's own source
comments at build time** (so it can never drift), embedded in the module file, and
surfaced by the `nova` CLI and the web UI for validation and browsing.

Today a `.mod` is a raw 16 KB image with a small binary header (`NL` magic, id, ABI
version, function count) but **no function names and no semantic version**. This
design adds a documentation trailer and the tooling to produce and consume it.

## 1. Container format — `.nmod`

The existing 16 KB image with a documentation trailer appended:

```
offset 0       : 16384 B  module image (ORG $C000; "NL" header at +3, unchanged)
offset 16384   : 4 B      trailer magic  "NDOC"  (4E 44 4F 43)
offset 16388   : 4 B      doc length L   (uint32 LE)
offset 16392   : L B      UTF-8 JSON metadata
```

- The first 16384 bytes are **byte-identical** to today's `.mod`, so the image is
  unchanged and still loads at `$C000`. Everything after is invisible to the CPU.
- **Firmware** stages the first `size` (16384) bytes and ignores the trailer.
- **nova CLI / web** read the `NL` header (validate magic/id/ABI/fn-count) *and* the
  `NDOC` JSON. If the trailer is absent (a plain old `.mod`), they degrade to
  header-only metadata.

### Existing binary header (unchanged), per `libabi.inc` / `libmod.inc`

```
$C000  jmp dispatch       (3 bytes)
$C003  MOD_MAGIC          "NL"  ($4E $4C)
$C005  mod_id             (1 byte)
$C006  abi_version        (1 byte, currently $01)
$C007  fn_count           (1 byte)
$C008  reserved           ($00 $00)
```

## 2. Doc-comment convention — `;@`

Doc-comments are ordinary `;`-comments (ca65 ignores them) tagged with `;@`, placed
next to the code they describe. The extractor scans for `;@` lines.

**Module block** (once, near `lib_module_header`):

```asm
;@module GRAPHICS
;@version 1.0           ; semantic module version (NEW — not in the binary)
;@brief  Virtual graphics controller: plot/line/fill, sprites, turtle.
```

**Per-function block** (above each handler or jtable entry):

```asm
;@fn GFN_LINE           ; symbolic id — value resolved from the build's .lbl
;@brief Draw a line in the current colour.
;@arg x0  s16   start x (0..319)
;@arg y0  s16   start y (0..199)
;@arg x1  s16   end x
;@arg y1  s16   end y
;@ret void
;@effect Draws into the gfx plane using the current pen colour.
;@status LERR_OK, LERR_BAD_ARG (coords out of range)
```

**Rules:**

- **Ids are symbolic** (`GFN_LINE`), resolved to the byte value from the linker
  label file (`graphics.lbl`, already emitted). Single source of truth is
  `libgraphics.inc`; a missing symbol **fails the build** (drift caught loudly).
- **Arg names map positionally** to `LIB_ARG0..3` (first `;@arg` = ARG0). The
  `type` comes from a small vocabulary: `u8 s8 u16 s16 u32 s32 ptr16 fix16.8 bytes
  void` — this drives validation and display, since the ABI cells are raw 32-bit.
- **Op-dispatched calls** (the turtle's `GFN_TURTLE_OP $B3` with 26 sub-ops
  FD/RT/…) use `;@fn GFN_TURTLE_OP.FD` — a `parent.subop` id so each turtle command
  is documented individually. The sub-op resolves symbolically (e.g. `EXT_CMD_FD`)
  or may be given as a literal hex in the annotation where cross-include resolution
  is awkward.
- **`;@effect`** (free text: side effects / blocking, e.g. "waits one VGC frame")
  and **`;@status`** (the `LIB_STATUS` codes the fn can return — functions signal
  errors there, not via the return value) are **optional**, included only where they
  add value.
- **Clobbers are NOT documented per-function.** Every `lib_call` clobbers A/X/Y by
  contract (the loader and handler use them freely; the caller reads results from
  the mailbox), and a module may only touch the mailbox cells, the designated
  `LIB_SCRATCH`/`LIB_ZP`, and its own BSS band — anything else is a bug. This is
  stated once per module as an ABI-level note:
  `abiNote: "All calls clobber A/X/Y; communicate via the mailbox."`

## 3. Build pipeline + JSON schema

**One new tool, `tools/nmod_pack.py`** (Python, alongside `tools/hex16_to_bin.py`).
Inputs: the module's source `.s` (annotations), its `.lbl` (symbol→value), and its
`.bin` (16384 B). It:

1. Scans `;@` blocks → module + function records.
2. Resolves symbolic ids from `.lbl` (`GFN_LINE`→`$A4`); a missing symbol **fails
   the build**.
3. **Validates against the binary**: fn-count in JSON must equal header byte
   `$C007`; magic `NL` at +3; module id matches `;@module`; `.bin` is exactly
   16384 bytes.
4. Emits `graphics.nmod` = `graphics.bin` + `"NDOC"` + `uint32 len` + JSON.

### NDOC JSON schema

```json
{ "ndocVersion": 1,
  "module": { "name":"GRAPHICS", "id":1, "abiVersion":1, "version":"1.0",
              "brief":"…",
              "abiNote":"All calls clobber A/X/Y; communicate via the mailbox." },
  "functions": [
    { "id":164, "idHex":"$A4", "name":"GFN_LINE", "brief":"…",
      "args":[ {"i":0,"name":"x0","type":"s16","desc":"start x"}, … ],
      "ret":{"type":"void","desc":""},
      "effect":"…", "status":["LERR_OK","LERR_BAD_ARG"] },
    { "id":179, "idHex":"$B3", "subOp":16, "subOpHex":"$10",
      "name":"GFN_TURTLE_OP.FD", "brief":"…", "args":[…], "ret":{…} }
  ] }
```

`effect`/`status` are omitted when absent.

**Makefile:** the module's `make` gains a `.nmod` step (`.bin` + `.lbl` →
`nmod_pack.py` → `.nmod`). NovaHost `sd-assets` stages `graphics.nmod`, and
`boot.json` `libraries[].path` points at it.

## 4. nova CLI — parser + `nova module` commands

**One shared parser, `NovaModule.cs`** (used by both the CLI and the web API):
`NovaModule.Parse(bytes)`:

- Requires length ≥ 16384; reads the `NL` header (magic@+3, id@+5, abiVersion@+6,
  fnCount@+7).
- If length > 16384, reads the `NDOC` trailer at +16384 (magic, `uint32` len, JSON
  via `System.Text.Json`).
- **Cross-checks**: JSON `module.id` == header id, `functions.length` == header
  `fnCount` → flags drift.
- Returns `{ id, abiVersion, fnCount, valid, hasDoc, doc }`; a plain `.mod` parses
  fine as header-only.

**`nova module` command group:**

- `info <file|name> [--remote]` — parse a local file or a board `/lib/x.nmod`;
  pretty-print module name/version/id/ABI + a function table (id, name, args→types,
  return, effect, status). `--json` for raw.
- `ls [--remote]` — list `/lib/*.{mod,nmod}` (or a local dir), parse each, and
  **cross-reference `boot.json`**: name, id, version, #fns, valid?, *staged?*
  (present in `libraries[]`). Flags the high-value bug **id-in-binary ≠
  id-in-boot.json** (silently breaks `modtab_lookup`).
- `validate <file>` — magic/ABI/size/header-vs-doc consistency; non-zero exit on
  failure (pre-upload + CI gate).
- `put <file> [--remote]` — **validate, then stage** to `/lib/`; refuses to stage an
  invalid/garbage module.
- `get` / `rm` — fetch / delete, thin wrappers over existing SD ops.

Reuses existing remote plumbing (`NovaHostManagementClient`, `PutFile`, boot.json
read/write) — no new transport.

## 5. Web API + UI

**New API endpoints** in `NovaWebServer.cs` (all using the shared `NovaModule`
parser, so CLI and web agree):

- `GET /api/modules` — enumerate `/lib/*.{mod,nmod}`, parse each, cross-reference
  `boot.json`; returns `{name, id, version, abiVersion, fnCount, valid, staged,
  idMismatch}`.
- `GET /api/modules/{name}` — full parsed metadata including the function reference.
- `PUT /api/modules/{name}` — body = `.nmod` bytes → **validate first** (reject
  invalid with a reason), stage to `/lib/`.
- `DELETE /api/modules/{name}` — remove from `/lib/` (and its `libraries[]` entry).
- `POST /api/modules/{name}/staged` `{staged:bool}` — toggle `libraries[]`
  membership without re-uploading.

**UI** — a new **Modules** panel beside the existing inventory/runtime views:

- A table: name, id, version, ABI, #fns, a **valid** badge, a **staged** badge, and
  a ⚠ on `id`-mismatch (binary vs boot.json).
- Click a row → detail drawer rendering the module's **self-generated API
  reference**: brief + version + ABI note, then each function as a signature
  (`GFN_LINE(x0:s16, y0:s16, x1:s16, y1:s16) → void`) with its effect and status
  codes.
- Drag-drop upload that runs validation and shows the result before staging; a
  stage/unstage toggle.
- Reuses the existing `/events` SSE for live refresh.

## 6. Firmware change

`streamSdramAsset` / the library staging currently **errors if the file size is not
exactly `size`** (16384). Change to: require `file_size >= size`, stage the first
`size` bytes (ignore the trailing doc). One small change in
`e6502.ESP32/novahost/novahost.ino`; needs a re-flash. Best-effort/graceful as
today.

## 7. Implementation phases (suggested)

1. **Format + extractor:** `tools/nmod_pack.py`, the `;@` convention, annotate
   `testmod.s` (simple, 3 fns) end-to-end; produce `testmod.nmod`. Unit-test the
   packer (drift detection, symbol resolution, validation).
2. **Firmware stage-first-N:** the `streamSdramAsset`/library size change; flash;
   verify a `.nmod` stages and the module still runs on HW.
3. **CLI parser + commands:** `NovaModule.cs` + `nova module {info,ls,validate,put,
   get,rm}`. Unit-test the parser against `testmod.nmod` and a plain `.mod`.
4. **Annotate GRAPHICS:** add `;@` blocks to `graphics.s` (GFN_* + the 26 turtle
   sub-ops); produce `graphics.nmod`; wire the Makefile + boot.json.
5. **Web API + UI:** `/api/modules*` endpoints + the Modules panel.

Phases 1–3 deliver `nova module` value with no UI; 4 makes the real module
self-documenting; 5 adds the browser surface.

## 8. Notes / non-goals

- **No new transport.** Everything rides the existing nova TCP management path
  ([[feedback_novahost_tcp_only]] — TCP only, never HTTP to the board).
- **YAGNI:** `;@effect`/`;@status` only where they help; no per-function clobber
  lists; no module-format change beyond the appended trailer.
- **Drift safety:** the packer cross-checks the JSON against the real binary header
  and fails the build on any mismatch, so docs can't silently diverge from code.
- **Future:** a richer `ndocVersion` could carry examples, deprecation flags, or
  cross-module dependencies; the parser already keys on `ndocVersion` to allow it.
