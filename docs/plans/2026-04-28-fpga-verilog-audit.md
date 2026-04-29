# FPGA Verilog Audit Backlog

Started: 2026-04-28

Purpose: track hardware design issues, cleanup opportunities, and resource risks found while debugging sprite motion on ULX3S. This is a backlog, not a change log.

## Highest Priority

### HDMI encoder reset/cadence hardening

Files: `e6502.FPGA/rtl/hdmi/vga2dvid.v`, `e6502.FPGA/fpga/fpga_top.sv`

Observed: disabling the HDMI encoder shift-clock synchronizer produced no HDMI signal on the display. Keep the synchronizer enabled.

Observed: hardware-only motion lanes stutter together even though one lane is generated in 320x240 logical sprite space and the other is generated directly in 640x480 physical pixel space. That points at frame cadence / HDMI / display or capture behavior, not sprite register writes, sprite shape RAM, scanline buffering, BASIC timing, or CPU timing.

Concerns:

- `R_sync_fail` is not reset/initialized in the known-good encoder path.
- The `R_shift_clock_synchronizer` reset block is not written as an `else if`, so reset behavior can be partially overwritten by later logic in the same always block.
- `resetn` is generated in the pixel clock domain but consumed in both pixel and shift clock domains.

Likely fix direction:

- Keep dynamic resync enabled.
- Add deterministic reset/init for synchronizer state.
- Use proper reset synchronization for `clk_shift`.
- Test as a narrow HDMI-only cleanup, not mixed with sprite changes.

### Move external video output to a consumer-compatible mode

Files: `e6502.FPGA/fpga/fpga_top.sv`, `e6502.FPGA/rtl/vgc_timing.sv`, `e6502.FPGA/rtl/vgc.sv`, `e6502.FPGA/rtl/hdmi/*`

Observed: a TV shows a scrambled green screen from the current VGA-ish HDMI output, while Cam Link accepts it but may show motion judder. This points at output compatibility, not only internal VGC behavior.

Later isolation result: an external ULX3S checkerboard demo using a separate top, separate timing, separate HDMI example code, and `LVCMOS33 DRIVE=4` full P/N GPDI output still shows green on the Sony TV, while the same flashed bitstream displays the checkerboard correctly through the Cam Link. This is the strongest evidence so far that the FPGA is producing a decodable DVI-over-HDMI stream, but the Sony sink is rejecting or failing to decode the ULX3S GPDI electrical/HDMI presentation.

Preferred target:

- CEA-style `720x480p59.94/60` external timing.
- Keep Nova's logical output at `640x480`, centered horizontally inside the 720-wide active area with 40-pixel side borders.
- Keep graphics/sprites at `320x240` scaled 2x inside the Nova canvas.

Clocking note:

- `ecppll -i 25 -o 135 --clkout1 27` can generate exact `135 MHz` TMDS shift and `27 MHz` pixel clocks from the ULX3S 25 MHz oscillator.
- The existing generic `ecp5pll.sv` wrapper is not the right mechanism for exact CEA video clocks.
- Do not collapse SDRAM clocks into this video PLL; SDRAM still wants its existing `100 MHz` / `6.25 MHz` relationship.

Likely fix direction:

- First build a standalone CEA 480p test-pattern bitstream with no CPU/VGC to verify TV and Cam Link lock cleanly.
- Then integrate VGC by mapping the current 640x480 Nova canvas into centered 720x480 output timing.
- If full-core-at-27 MHz closes timing, use it as the simpler implementation. If it does not, keep the system on its existing clock and add a proper video scanout clock boundary.
- Add hardcoded HDMI AVI InfoFrames later if DVI-style TMDS with CEA timing is still not accepted by some sinks.
- For strict TVs, consider an external HDMI transmitter/retimer or a board/output path with proper HDMI electrical compliance. The Cam Link result proves the current path can be valid enough for tolerant sinks but not necessarily for all TVs.

### BRAM is effectively full

Evidence from recent local `report.json`: `DP16KD used 202 / 208`, leaving 6 blocks free.

Implication: the board is still viable, but feature work must stop treating BRAM as general storage. Use BRAM for latency-critical video/audio state only.

Likely fix direction:

- Reduce main RAM from 64K to 48K if `$C000-$FFFF` is permanently ROM/language space. This should free roughly 16KB worth of BRAM storage.
- Keep bulky language ROMs, disk images, soundfonts, and sample data on ESP/SD/SDRAM.
- Keep only the currently active language/kernel ROM window in FPGA-visible memory.
- Revisit small memories that are forced into full DP16KD blocks.

### Global `dpram` block-RAM policy wastes small memories

File: `e6502.FPGA/rtl/dpram.sv`

Current `dpram` forces `ram_style = "block"` globally. That was justified by the old sprite scanline buffer issue, but the scanline buffer is now explicitly FF-backed in `vgc_sprites.sv`.

Potential waste:

- Small palette RAMs.
- Small stacks.
- Small register-ish memories.
- Any shallow memory that burns a full DP16KD because it uses `dpram`.

Likely fix direction:

- Add a memory-style parameter or separate modules: `dpram_block`, `dpram_lut`, `dpram_regs`.
- Keep large frame/tile/text/ROM memories in DP16KD.
- Move tiny memories to LUTRAM/registers only after each one is tested in synthesis, not by broad replacement.

## Timing / Efficiency Risks

### Tile engine uses modulo in the raster path

File: `e6502.FPGA/rtl/tile_engine.sv`

Concern:

- `world_x = raw_wx % map_w`
- `world_y = raw_wy % map_h`

Modulo by non-powers-of-two is expensive hardware and can create timing pressure. This is in the pixel path.

Likely fix direction:

- Replace modulo with bounded conditional subtract/wrap based on known map sizes.
- Consider constraining scroll registers to map range at write time.

### Tile engine is 320x200 while VGC graphics are 320x240

Files: `e6502.FPGA/rtl/tile_engine.sv`, `e6502.FPGA/rtl/vgc.sv`

Concern:

- VGC defines graphics as `320x240`.
- Tile engine comments and constants use `pixel_y 0-199` and `GFX_H = 200`.
- `vgc.sv` feeds `pre_gfx_y` directly into the tile engine, so the bottom 40 logical rows need an intentional policy.

Likely fix direction:

- Decide whether tile mode is meant to be 320x200 with a border or full 320x240.
- If full-screen tile mode is desired, update map dimensions, scroll wrapping, tests, and documentation together.

### Repeated runtime address multiplies

Files: `e6502.FPGA/rtl/artist.sv`, `e6502.FPGA/rtl/tile_engine.sv`, `e6502.FPGA/rtl/vgc_copper.sv`

Examples:

- Artist uses `y * GFX_W + x` in several drawing states.
- Tile commands use `nt * NT_SIZE + row * NT_COLS + col`.
- Copper computes `beam_pos = gfx_y * 320 + gfx_x`.

Likely fix direction:

- Prefer incremental address counters inside drawing/state machines.
- Replace constant multiplies with shifts/adds where practical.
- For copper, use a running beam-position counter or shift/add expression instead of per-cycle multiply.

### Main CPU read path remains a known timing limiter

File: `e6502.FPGA/rtl/top.sv`

Recent timing reports still show `main_ram`/`cpu_din`/CPU decode paths among critical paths, even after the read mux was improved.

Likely fix direction:

- Keep CPU at divided enable unless the memory/read path is redesigned.
- Avoid adding more MMIO branches to the hot CPU read mux.
- Prefer mirrored/write-only device registers where reads overlap ROM.

## Simulation / Hardware Drift

### `top.sv` has separate simulation and synthesis memory implementations

File: `e6502.FPGA/rtl/top.sv`

This is practical for Verilator speed and BRAM inference, but it means hardware bugs can escape tests unless the `SYNTHESIS` path is exercised.

Likely fix direction:

- Continue adding `-DSYNTHESIS` tests for every FPGA-only bug.
- Prefer shared modules for memories and buses where feasible.
- Keep simulation-only shortcuts explicit and documented.

### SDRAM access is phase-held, not a formal CDC handshake

Files: `e6502.FPGA/rtl/sdram/xram_sdram.sv`, `e6502.FPGA/rtl/sid/sid_curve_reader.sv`, `e6502.FPGA/fpga/fpga_top.sv`

The pixel-clock domain holds SDRAM requests long enough for the 100MHz SDRAM controller to sample them. This is pragmatic and currently works, but it is not as robust as a proper clock-domain request/ack bridge.

Likely fix direction:

- If SDRAM becomes central for language ROMs, wavetable sample data, or larger assets, add a real request/ack CDC wrapper.
- Keep the current hold-window approach only for low-rate/debug-ish traffic unless proven under stress.

## Resource / Feature Planning

### SID waveform tables may be replicated

Files: `e6502.FPGA/rtl/sid/sid_chip.sv`, `e6502.FPGA/rtl/sid/sid_tables.sv`

There are two `sid_chip` instances, each instantiating `sid_tables`. The filter curve was already moved to SDRAM, which was the right direction.

Likely fix direction:

- Confirm actual BRAM breakdown from post-synth hierarchy.
- Consider sharing combined waveform table memory between SIDs if resource pressure becomes severe.
- If the new wavetable synth supersedes SID features, decide whether both should remain in the default bitstream.

### Wavetable synth should not store soundfonts in FPGA BRAM

Planned split:

- ESP/SD card owns `.sf2` upload, conversion, file management, and soundfont storage.
- SDRAM can hold currently loaded sample/wavetable data.
- FPGA owns voice state, phase accumulators, interpolation, envelopes, mixer, and small FIFOs/control registers.

This keeps the ULX3S viable for the synth feature.

## Build / Tooling

### Define-only bitstream builds could reuse stale synthesis netlists

Files: `e6502.FPGA/fpga/Makefile`, `tools/beast-synth.sh`

The Beast wrapper deleted `build/e6502.bit` and `build/e6502.config`, but left `build/e6502.json`. The Makefile also did not track `EXTRA_DEFINES` as an input to synthesis. That meant a build whose only change was a define, such as HDMI phase/test-path selection, could rerun place-and-route against an old netlist and produce a misleading bitstream.

Mitigation added:

- `tools/beast-synth.sh` now removes the full remote `build/` directory before running, saves a timestamped log and manifest, records commit/dirty state, and includes seconds in artifact names.
- `e6502.FPGA/fpga/Makefile` now writes `build/build_args.txt` and makes synthesis/place-and-route depend on it, so local incremental builds notice changes to `EXTRA_DEFINES`, device/package, pin file, and nextpnr flags.

Retest impact:

- Source-change builds were probably valid because source timestamps forced Yosys.
- Define-only variants, especially HDMI phase/test-path experiments, should be considered suspect and rebuilt cleanly before drawing conclusions.

### `--timing-allow-fail` should not hide regressions

File: `e6502.FPGA/fpga/Makefile`

Current nextpnr flags include `--timing-allow-fail`. This is useful while iterating, but dangerous as the default success criterion.

Likely fix direction:

- Keep it for exploratory builds if needed.
- Add a script/CI check that parses `report.json` and fails if any required clock misses target.
- Track fmax for `clk_pixel`, `clk_shift`, and `sdram_clk` per bitstream.

### Debug bridge RX buffering is shallow

File: `e6502.FPGA/rtl/debug_bridge.sv`

The bridge has a one-byte RX buffer. That is probably fine at 115200 for command/response traffic, but async events and bulk transfers are a risk area.

Likely fix direction:

- Stress-test bulk upload/download and async FIO events.
- If drops appear, replace the single-byte buffer with a small FIFO.
