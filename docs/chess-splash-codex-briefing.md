# Nova Chess splash — intermittent blank menu splash (Codex briefing)

Self-contained problem statement. No repo/context access assumed.

## System
- **NovaHost** = a custom retro computer. A **6502 CPU is synthesized inside an FPGA** (ULX3S/ECP5). An **ESP32 ("NovaHost firmware", C++/Arduino)** is a companion that talks to the FPGA over a **SPI "debug bridge"**, providing file I/O (SD card), boot/ROM loading, and host control.
- The 6502 reaches the ESP through a memory-mapped **FIO controller** ("FIO bank" of registers at `$B9A0+`). The 6502 writes a command byte + params to the FIO bank and busy-waits on `FIO_STATUS`; an FPGA-generated `fio_event` pulse notifies the ESP, which reads the bank over SPI, performs the operation (SD reads, writes into the FPGA's VGC graphics memory, etc.), and writes a status back into the FIO bank to release the 6502.
- **VGC** = the FPGA video chip. Graphics "gfx plane" pixels are 1 byte each (color index). The ESP writes them via SPI `pokeVgcBlock`.
- The app under test is **Nova Chess**, a 6502 program (`NOVACHESS.BIN`) loaded from an SD `.ndi` disk image. Its menu shows a title **splash** (a 320x200 image `SPLASH.NVG`, also on the `.ndi`).

## Symptom
On ~10–15% of cold-starts the chess menu text renders fine but the **splash is blank**. Test reads gfx pixel (90,80): expects `$0E` (yellow), intermittently sees **`$02`**.

## The splash load path
1. 6502 chess `show_menu_backdrop` calls `load_title_splash`, which sets a filename and calls `nvg_load_named` (NDK routine `runtime/asm/nvg.s`).
2. `nvg_load_named` writes `CMD_NVGLOAD` + name to the FIO bank → `fio_event`.
3. ESP `FioDispatcher::handle_event()` does `peekBlock(BANK_BASE=$B9A0, 80, _bank)` (one SPI read of the whole bank), reads `cmd`, dispatches `handle_nvgload()`.
4. `handle_nvgload()`: validates header, calls `clear_vgc_gfx()` (zeroes the whole gfx plane to `$00`), then SD-reads run-length spans and `pokeVgcBlock`s them into VGC gfx. On any failure it `respond_err(ERR_IO)`; on success `respond_ok()`.
5. `fio_exec` (6502, `runtime/asm/fio.s:160`) busy-waits **with NO timeout**: `@wait: LDA FIO_STATUS; BEQ @wait`. So if a command is issued but the ESP never responds, the 6502 **hangs forever**.

## What we did (and ruled out) — a long, mostly-wrong session
Original (prior-session) hypothesis: an **SPI bridge write/command desync** corrupts `pokeVgcBlock`, so the splash writes drop. We:
- Built an integrated Verilator test of the SPI slave + CDC FIFOs + bridge; swept clock phases; it was clean (no logic desync reproduced).
- Shipped an **RX-resync watchdog** (RTL) + a **capability-gated retry** in `pokeVgcBlock` (firmware). Flashed + OTA'd. **Did not fix the miss rate.**
- **Lowered the SPI control clock** 26.67 MHz → 13.3 MHz: **same miss rate** (so NOT clock-rate signal integrity). 5 MHz was too slow and broke boot (`bootPhase:"degraded"`, `storage busy`).

## The decisive instrumentation (this is the key part)
We gave **every** `respond_err` exit inside `handle_nvgload()` a **unique** ERRCODE (0x10–0x1D, plus `0xC0|space` for the space check, and `0x80|cmd` for `handle_event`'s unknown-command default). Then ran 20-cold-start loops, peeking the FIO bank on each miss.

**Result across 8+ captured misses, identical every time:**
- gfx(90,80) = **`$02`**
- FIO `STATUS`($B9A1) = `$03` (ST_ERR=3)
- FIO `ERRCODE`($B9A2) = **`$02`** (ERR_IO=2)
- FIO `NAME`($B9B0) = "SPLASH.NVG"

**ERRCODE is ALWAYS `$02` — NEVER any of our unique codes.** Since every `handle_nvgload` exit is uniquely coded, **`handle_nvgload` is never entered on a miss.**

Two independent confirmations it never ran:
1. **gfx(90,80)=`$02`, not `$00`.** `handle_nvgload`'s first action is `clear_vgc_gfx` (zeroes gfx to `$00`). `$02` is the menu *backdrop* (drawn by chess's `clear_gfx`/dialog), untouched → `clear_vgc_gfx` never ran.
2. The `$02`/ST_ERR/SPLASH.NVG values are **stale**: the **FPGA FIO bank is not reset across cold_start**, so they're leftovers; the `$02` ERR comes from some *other* un-instrumented FIO op (NOT handle_nvgload).

Also: `fio_exec` has no timeout and the menu *renders* (no 6502 hang) ⇒ if the 6502 had *issued* CMD_NVGLOAD and the ESP dropped it, the 6502 would hang. It doesn't ⇒ **the command was never issued ⇒ the 6502 SKIPPED the load.**

## Why the 6502 skips — the gate
```asm
; chess runtime.s
show_menu_backdrop:
        JSR clear_text
        LDA splash_loaded
        BNE @ready            ; splash_loaded != 0  -> SKIP load entirely
        STZ VGC_DIMMER
        JSR load_title_splash
        BEQ @show
@fallback:
        JSR clear_gfx         ; load FAILED -> clear_gfx (also STZ splash_loaded -> retry next time)

load_title_splash:
        ... set name ...
        JSR nvg_load_named
        BNE @done             ; fail: A!=0, splash_loaded left 0
        LDA #$01
        STA splash_loaded     ; success -> latch =1
        LDA #$00
@done:  RTS

reset:                        ; chess entry / CPU reset vector
        SEI / CLD / LDX #$FF / TXS
        STZ splash_loaded     ; <-- the one-shot latch is cleared HERE only
        JSR init_video ...

clear_gfx:
        STZ splash_loaded     ; (also clears it on a failed-load fallback)
        ...
```
- `splash_loaded` is a **BSS** byte (`.res 1`) — **not part of the loaded `NOVACHESS.BIN` image**; load does not initialize it. Only `STZ splash_loaded` in `reset:` (and `clear_gfx`) clears it.
- 6502 RAM is **not** zeroed on reset/cold-start.
- So a successful load sets `splash_loaded=1`; if that survives to the next menu entry, the splash is skipped.

## What cold_start does (ESP firmware `cmdColdStart`)
```cpp
void DebugServer::cmdColdStart(const String& json) {
    if (!loadRomsToFPGA()) { respondError(...); return; }  // reload BASE ROM (NovaBASIC) to FPGA
    _bridge.resume(); _paused = false;
    ... wait for "Ready" text on screen ...
}
```
- cold_start reloads the **base ROM (NovaBASIC)** into the FPGA, then **mounted media autoboots**: NovaBASIC detects the mounted `.ndi`/`/roms` and loads + launches `NOVACHESS.BIN`. Chess `reset:` runs **as part of that autoboot launch**.
- novahost.ino comments note cold_start does some RAM init but **"never touches the reserved `$0320-$041F` band"**. Known chess symbols in that band: `self_from=$0326`, `game_state=$032A`. `splash_loaded` is a nearby BSS byte (declared late in BSS; exact address not in the exported `.sym`; would be in `runtime.map`).

## Current conclusion
The miss is a **6502-side SKIP**: `splash_loaded` is `!=0` at `show_menu_backdrop`, so `load_title_splash` is never called. That requires chess `reset:` (its `STZ splash_loaded`) to **not have run / not have cleared it** on that cold-start. It is **not** a bridge bug, not a clock/SI bug, not an SD-read bug, not `handle_nvgload`. The whole bridge-watchdog/retry effort was the wrong layer (harmless, kept for a separate wedge it does fix).

## Open questions for a second opinion
1. Why would `splash_loaded` persist `!=0` across cold_start **intermittently** (~15%), if chess `reset:` always runs and `STZ`s it? Candidates:
   - The autoboot launch of `NOVACHESS.BIN` doesn't always enter at `reset:` (jumps to a different entry, or a re-launch path skips reset).
   - A race between cold_start's RAM init / autoboot and the `$0320-$041F` reserved band where `splash_loaded` lives.
   - `splash_loaded` shares an address with something the loader *does* write to `1` intermittently.
   - The `.ndi` `NOVACHESS.BIN` BSS layout vs the running image (stale build) — though `.ndi` is freshly built and `STZ splash_loaded` is present in source.
2. Is the right fix (a) make cold_start/autoboot reliably re-enter `reset:` (and/or have the ESP zero the `splash_loaded` RAM byte on cold_start), or (b) drop the `splash_loaded` one-shot gate so the menu **always** reloads the splash (simpler, robust, costs ~11 KB re-load per menu entry)? Any downside to (b) (e.g., flicker if `show_menu_backdrop` is called per-frame vs once per entry)?
3. Anything that would make `handle_nvgload` appear "never entered" *other* than a 6502 skip, given `fio_exec` has no timeout and the menu renders?

## How it's observed (repro)
`/tmp/chess_diag.py`: mounts `examples/novachess/dist/mode-test.ndi`, loops N cold-starts, reads gfx(90,80) via the host debug client; on a miss peeks NAME($B9B0)/STATUS($B9A1)/ERR($B9A2). ~15% of cold-starts miss, sometimes clustered.
