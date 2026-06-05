# Handoff — Stage 4c.2 HW bring-up + self-documenting modules (.nmod)

**Session date:** 2026-06-05. **Branch:** `main`. **Board:** `novahost.local` / `192.168.1.65`,
default runtime = `novalogo` (leave it — user wants Logo default).

Two threads shipped this session, both **complete and hardware-verified**. Everything is committed.
Pick up at "Open / future work" below.

## Thread 1 — Stage 4c.2 hardware bring-up (DONE, shipped `5d20de8`)
The NovaLogo turtle did nothing on HW: it warm-started (banner reprinted, split-screen + turtle intact).
**Root cause (proven by reading board RAM):** the 157-byte resident `lib_call` loader was never installed
at `$0320` on hardware — `$0320` held zeros (`$00` = `BRK`), so every `lib_call` (`JSR $0320`) ran `BRK`.
The emulator pokes it via `CompositeBusDevice.PokeResidentLoader`; nothing did on HW. The `$0300` mailbox was
correctly populated (`[mod=1, fn=$B3, arg=50]`), proving only `$0320` was missing.

**Fix (firmware, `5d20de8`):** `e6502.ESP32/novahost/novahost.ino`
- `stageResidentLoader()` — reads `/roms/libcall.bin` (157 B) and `pokeBlock`s it to RAM `$0320` during
  CPU-only reset-hold (no contention with cold_start's RAM init; `main_ram` is a reset-less `dpram` so it
  survives the system-reset pulse; cold_start never touches the reserved `$0320-$041F` band).
- Also stages the active runtime's ext ROM into XRAM `$07C000` (`HOST_EXT_XRAM`) so `ensure_ext_resident`
  re-pages it into bank 1 after a graphics `lib_call` clobbers the overlay (else EDIT/sound/timing break).
- Makefile reproduces the SD staging (`graphics.mod`/`.nmod` + `libcall.bin`).

**Both handoff verification tests pass on HW:** turtle `CS` + `REPEAT 4 [FD 50 RT 90]` draws the square
(`$9F00` = 160,80 home, `LIB_RESIDENT=$01`); `EDIT SQUARE` → `LIB_RESIDENT=$FF` (ext re-paged from `$07C000`),
editor opens, Ctrl-S/Ctrl-Q → split-screen returns. See memory `project_stage4c2_hw_loader_gap_2026_06_05`.

## Thread 2 — Self-documenting modules (.nmod) — FEATURE COMPLETE
Design: `docs/plans/2026-06-05-nmod-self-documenting-modules-design.md` (commit `2043004`).
A `.nmod` = `[16384 B module image]["NDOC"][uint32 LE len][UTF-8 JSON]`. The JSON is generated from `;@`
doc-comments in the module's source, so docs can't drift. Firmware stages the first 16 KB; the trailer stays
on SD for the CLI/web.

| Commit | What |
|--------|------|
| `59b6749` | Phase 1 — `tools/nmod_pack.py` packer (parse `;@` → validate vs binary header → pack). 23 pytest. |
| `37f2c7c` | Phase 3 — `e6502.Storage/NovaModule.cs` parser + `nova module {info,ls,validate,put,get,rm}`. 7 MSTest. |
| `d1b7655` | Phase 4a — `load_symbols`/`--syms` (resolve `GFN_*` from `libgraphics.inc`); validator → id-in-range/unique (not count==). |
| `4893205` | Phase 4b+2 — `graphics.nmod` + firmware `streamSdramAsset == → >=` (stage first N). **HW verified.** |
| `d48ea1b` | Phase 5 — web `/api/modules` + `/api/modules/{name}` + Modules UI panel + searchable/paged Functions **modal**. |
| `954e32d` | Full annotation — all **132** implemented GFNs (was a 10-fn draw subset). |

`nova module info graphics.nmod` (local or `--remote`) and the web modal both list 132 functions with
signatures/briefs/effects/status. `nova module ls --remote` cross-refs `boot.json` (staged?, id-mismatch).
See memory `project_nmod_self_documenting_modules_2026_06_05`.

## Key facts / gotchas (read before resuming)
- **Build/run:** cc65 on PATH = `export PATH=/Users/barry/Git/cc65/bin:$PATH`. Tests:
  `cd tools && pytest test_nmod_pack.py` (23) and `dotnet test e6502UnitTests --filter ~NovaModuleTests` (7).
- **nova CLI Debug build outputs to a RID subfolder:** run `dotnet e6502.Nova/bin/Debug/net10.0/osx-arm64/nova.dll …`
  (the `bin/Debug/net10.0/nova` apphost is a STALE May-26 leftover). AOT release binary (TCP host ops):
  `e6502.Nova/bin/Release/net10.0/osx-arm64/native/nova`.
- **nova CLI is TCP-ONLY** to the board — never HTTP. The `nova webserver` is a LOCAL web UI that proxies to
  the board over TCP (`nova webserver --remote=novahost.local --no-open --port 8088`).
- **graphics header fn_count = `GFX_FN_COUNT = $DB` (219) is the dispatch-table SPAN**, not a dense count
  (sparse GFN ids, gaps → `gfn_unimpl`). That's why the validator checks `id < fnCount`, not `count == fnCount`.
- **`GFN_*` ids live in `runtime/asm/libgraphics.inc` as `= $XX` constants, NOT in `.lbl`** (ld65 `.lbl` =
  label addresses only). `nmod_pack --syms libgraphics.inc` resolves them.
- **Annotations are ca65 comments** (`;@…`) → `graphics.bin`/`testmod.bin` are byte-identical after annotating.
  `.nmod`/`.o`/`.map`/`.lbl` are build artifacts (untracked); `graphics.bin`/`testmod.bin` ARE tracked.
- **Firmware flash:** OTA via `python3 <Arduino15>/…/espota.py -i 192.168.1.65 -p 3232 -f build/<x>/novahost.ino.bin`
  (compile: `arduino-cli compile --fqbn 'esp32:esp32:lolin32:PartitionScheme=min_spiffs' --build-path build/<x> .`).
  OTA flash does NOT touch the SD card. `main_ram` is reset-less so the loader survives runtime RESET too.
- **Board boot.json `libraries[].path` = `/lib/graphics.nmod`** (size stays 16384). Editing it: `nova get
  /config/boot.json` → edit → `nova put` (preserve `defaultRuntime=novalogo`). The web server's
  `MergeBootConfigDefaults` was fixed to preserve `libraries[]` (it had dropped them → staged=false bug).

## Open / future work (user-flagged, NOT started)
1. **Turtle 26 sub-ops** — `GFN_TURTLE_OP` ($B3) is documented as ONE function; the design's `parent.subop`
   form (`GFN_TURTLE_OP.FD`, etc., from `TOP_*`/`EXT_CMD_*` in libgraphics.inc:496-521) would surface all 26
   turtle commands. Needs: `nmod_pack` sub-op id resolution + the `NovaFn`/JSON to carry a `subOp`.
2. **Web validate-then-stage / staged-toggle endpoints** — upload/delete currently reuse `/api/sd` (no
   server-side validation; no `libraries[]` toggle). Design §5 specced `PUT /api/modules/{name}` (validate
   first, reject invalid) + `POST /api/modules/{name}/staged`. The CLI `nova module put` already validates.
3. Optional: annotate `testmod`'s `.nmod` is already done; other modules (none yet) follow the same pattern.

## Files of record
- `tools/nmod_pack.py` + `tools/test_nmod_pack.py` — packer + 23 tests.
- `e6502.Storage/NovaModule.cs` — shared parser (CLI + web). `e6502UnitTests/NovaModuleTests.cs` — 7 tests.
- `e6502.Nova/Program.cs` — `DoModule` + `nova module` subcommands. `e6502.Nova/NovaWebServer.cs` — `/api/modules*`
  + Modules panel + Functions modal.
- `modules/graphics/graphics.s` — 132 `;@` blocks + Makefile `graphics.nmod` target.
- `runtime/asm/libgraphics.inc` — authoritative GFN signatures (the doc source of truth).
- `e6502.ESP32/novahost/novahost.ino` — `stageResidentLoader` + `$07C000` ext shelf + `streamSdramAsset >=`.

## Lesson
Root-cause by reading the actual hardware state (board RAM peeks) before theorizing — the `$0320`-zeros +
correct-`$0300`-mailbox reads pinned the bug instantly. And: a documented SUBSET is valid by design (the
sparse-dispatch validator), so prove the pipeline end-to-end first, then fill in annotations incrementally.
