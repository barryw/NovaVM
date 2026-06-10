# Handoff — Nova Chess splash intermittent-blank bug (2026-06-08, end of session)

## TL;DR — ROOT CAUSE FOUND (not yet fixed/confirmed)
The chess menu splash is intermittently blank (~15% of cold-starts; gfx(90,80)=`$02` instead of `$0E`).
**The 6502 splash load early-exits at `fio_copy_name` BEFORE it ever issues `CMD_NVGLOAD`, because the shared zero-page scratch register `NVR4L`=`$28` (= `NVG_NAMELEN` = `FIO_ARG_NAMELEN`) is clobbered between the moment `load_title_splash` sets it (=10) and the moment `fio_copy_name` reads it back.** Almost certainly an **interrupt (60 Hz frame/music ISR) clobbers `$28`/`$29` in that window** — a classic ISR-trashes-caller-scratch race. It is NOT the SPI bridge, NOT the SD path, NOT the `splash_loaded` latch, NOT clock/signal-integrity. The whole session chased wrong layers; this is the real one.

## Evidence chain (all confirmed on HW, board 192.168.1.65)
1. Instrumented EVERY `respond_err` exit in ESP firmware `handle_nvgload` (`e6502.ESP32/novahost/fio_dispatcher.cpp`) with unique ERRCODEs (0x10–0x1D, `0xC0|space`, `0x80|cmd`). On a miss, ERRCODE is **always `$02`, never one of ours** → `handle_nvgload` is **never entered**.
2. gfx(90,80)=`$02` on a miss, not `$00`. `handle_nvgload`'s first action is `clear_vgc_gfx` (zeroes gfx to $00). $02 = untouched menu backdrop → confirms handler never ran.
3. `fio_exec` (`runtime/asm/fio.s:160`) busy-waits with NO timeout; the menu text DOES render (no 6502 hang) → if `CMD_NVGLOAD` had been issued and not serviced, the 6502 would hang. It doesn't → **the command was never issued.**
4. Peeked the relocated `splash_loaded` (`$1B25`) on misses: **`$00`** → NOT a skip-on-latch. So `show_menu_backdrop` DID fall through to `load_title_splash`.
5. `load_title_splash` → `nvg_load_named` (`runtime/asm/nvg.s:57`): `JSR fio_copy_name; BNE @done` — its ONLY pre-command exit. `fio_copy_name` (`runtime/asm/fio.s`) `@bad`s if `FIO_ARG_NAMELEN` is `0` or `> FIO_NAME_LIMIT($3F)`.
6. `NVG_NAMELEN = FIO_ARG_NAMELEN = NVR4L = $28`; `NVR4H = $29` (`runtime/asm/nova.inc:38,41`; `nvg.inc`, `fio.inc:53`). `$28/$29` are SHARED scratch (also FIO/pager/audio). `load_title_splash` sets `NVR4L=10` (constant), so an invalid read ⇒ clobbered in-window. Intermittent + clock-independent ⇒ interrupt race.

## NEXT STEPS (do these in order)
### 1. Confirm the ISR-clobber hypothesis (cheap)
- Easiest: in chess `load_title_splash` (`examples/novachess/src/runtime.s:~1835`), wrap the name setup + `JSR nvg_load_named` in `SEI` … `CLI`. Rebuild the mode NDI, re-run the repro loop. If misses vanish → confirmed.
  - Rebuild: `make -C examples/novachess ndi BUILD_DIR=build-mode IMAGE=dist/mode-test.ndi CA65FLAGS="-DCHESS_AUTOSTART_DEMO=0"`
  - Repro/verify: `python3 /tmp/chess_diag.py` (run from repo root; N currently 14; ~5 min; reports HIT/MISS + on miss peeks gfx/NAME($B9B0)/STATUS($B9A1)/ERR($B9A2)/splash_loaded($1B25)). Pre-fix ~2–4 misses / 14–20.
- Alt confirm: peek `$28` on a miss (expect `0` or `>$3F`). Note it's transient.

### 2. Real fix (pick after confirming)
- **Preferred:** audit chess's 60 Hz IRQ/frame/music handler in `examples/novachess/src/` for any use of `$28/$29` (`NVR4*`) or routines that call `fio_copy_name`/audio that use the shared name scratch — make the ISR **save/restore `$28/$29`** (push/pull around the handler), since an ISR must not trash caller scratch.
- **Or** keep the `SEI/CLI` guard from step 1 as the localized fix (only protects this window; other shared-scratch callers stay exposed).
- **Or** NDK-side: have `fio_copy_name` copy its length arg into a non-shared/ISR-safe byte first.

### 3. Cleanup before committing
- **Revert ALL diagnostic instrumentation** in `e6502.ESP32/novahost/fio_dispatcher.cpp`: the unique ERRCODEs (0x10–0x1D, `0xC0|space`, `0x80|cmd`) back to plain `respond_err(ERR_IO)` (handle_nvgload ~line 3894–4041, and handle_event default ~795). These are TEMP debug only.
- `FPGA_SPI_READ_HZ` already reverted to `80000000UL/3UL` (the 5 MHz/13.3 MHz experiments are out).
- Recompile + OTA the clean firmware.

## What's deployed / uncommitted (all uncommitted)
- **FPGA bitstream FLASHED on board:** `e6502.FPGA/fpga/bit_backups/e6502.20260608_180708.rx_resync.bit` — adds the RX-resync watchdog + `CAP_RX_RESYNC` in `rtl/debug_bridge.sv`. Timing-closed (clk_pixel 29.78 MHz). **HARMLESS + a genuine robustness win (prevents a separate cascade/wedge) — KEEP IT.** It just doesn't fix THIS bug.
- **ESP firmware on board:** has CpuPauseGuard (handle_nvgload) + capability-gated retry in `pokeVgcBlock` + `supportsRxResync()` (`fpga_bridge.cpp/.h`) + the TEMP diagnostic ERRCODE instrumentation. Keep CpuPauseGuard/retry/cap; strip the instrumentation.
- **Chess:** `examples/novachess/src/runtime.s` — Session-4 `STZ splash_loaded` in `reset:` AND the new `splash_loaded` relocation `$035C → $1B25` (runtime.s:262). Keep the relocation (good hygiene: a one-shot latch should not sit in the cold_start-preserved `$0320–$041F` resident-loader band). `dist/mode-test.ndi` rebuilt with it.
- New integration test `e6502.FPGA/test/test_debug_bridge_spi_burst.sv` + Makefile wiring (the watchdog regression test). Green. KEEP.

## Wrong turns this session (so the next session doesn't repeat them)
- Trusted Session-4's "★ ROOT CAUSE CONFIRMED: bridge write drop ★" without re-localizing from scratch → built/flashed a watchdog + retry + ran an SPI-clock sweep, none of which touched the real bug. Lesson recorded in [[feedback_localize_before_fixing_hw]].
- The Codex 2nd-opinion fix (relocate `splash_loaded` out of the preserved band) was logical but addressed a DIFFERENT (also-real-but-latent) issue; it did not fix the miss (splash_loaded=$00 on miss proved it).
- SPI clock is NOT the cause (26.7 vs 13.3 MHz identical miss rate). 5 MHz was too slow (boot `degraded`/`storage busy`).

## Tooling / board notes
- Board: 192.168.1.65 (novahost.local). `openFPGALoader --detect` needs `--board ulx3s` (FT231X, not the default FT2232) — bare `--detect` wrongly says "device not found".
- Firmware build: `cd e6502.ESP32/novahost && arduino-cli compile --fqbn esp32:esp32:lolin32:PartitionScheme=min_spiffs --build-path build/ota-direct .`
- OTA: `python3 ~/Library/Arduino15/packages/esp32/hardware/esp32/3.3.8/tools/espota.py -i 192.168.1.65 -p 3232 -f build/ota-direct/novahost.ino.bin` (reboots board → re-streams ROM). After OTA, `nova device status --remote 192.168.1.65` should show `bootPhase:"ready"` (not `degraded`).
- Codex MCP available but ALL `*-codex` models reject on the ChatGPT account ("not supported"); need an allowed model or fall back to `o3`. Briefing for Codex: `docs/chess-splash-codex-briefing.md` (mostly still valid but its conclusion = "6502 skip / splash_loaded persistence" is now SUPERSEDED by the `NVR4L`-clobber finding above).
- Repro harness: `/tmp/chess_diag.py`.

## Key files
- 6502: `examples/novachess/src/runtime.s` (load_title_splash ~1835, show_menu_backdrop, reset:, splash_loaded:262), `runtime/asm/nvg.s` (nvg_load_named:57), `runtime/asm/fio.s` (fio_copy_name, fio_exec:154), `runtime/asm/nova.inc` (NVR4L=$28), `runtime/asm/nvg.inc`/`fio.inc` (alias defs).
- Firmware: `e6502.ESP32/novahost/fio_dispatcher.cpp` (handle_nvgload ~3894, handle_event ~740), `fpga_bridge.cpp/.h`, `debug_server.cpp` (cmdColdStart).
- Memory: `project_chess_splash_nvg_bug_2026_06_08.md` (full session log).
