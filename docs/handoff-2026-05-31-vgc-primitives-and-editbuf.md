# Handoff — VGC text primitives + editor rearchitecture (next session)

Date: 2026-05-31. Resumes from `docs/handoff-2026-05-31-editor-rearchitecture.md`.
Branch: `editor-render-and-vgc-primitives` (2 commits, see below). Memory:
`project_editor_hw_primitives_2026_05_31`.

## TL;DR

The editor smear is **already fixed** (minimal-update render via the VRAM port —
works on the current bitstream). On top of that we added two VGC primitives in
both the emulator AND the FPGA RTL, and a CPU correctness fix. Everything is
committed and verified; a **timing-clean bitstream is built**. What remains is
(1) flash + HW-verify the bitstream and (2) the editbuf rewrite to actually
*use* the new primitives (the DX/perf payoff). No more synth is needed for the
editor — its ROM streams from NovaHost at boot.

## Commits on the branch

- `439241c` feat: editor minimal-update render + VGC text primitives + CPU
  store-prefetch fix. Emulator + runtime. Full .NET suite **1321 green**.
- `7faf397` feat(fpga): mirror screen-base register + direct screen window to
  VGC RTL + nova.inc constants. Verilator green, bitstream timing-clean.

(The branch deliberately excludes ~45 pre-existing uncommitted NovaLogo/docs/FPGA
files that predate this work — leave them alone.)

## What shipped

### 1. Editor minimal-update render (smear fix) — `runtime/asm/editbuf.s`
`render_row` paints char/color/attr via per-cell VRAM-port writes (auto-inc OFF
on purpose — see CPU note below), base offset computed once. insert/delete/
backspace repaint ONLY the changed run `[editcol..VIEW_COLS)` via `EB_PAINTSTART`
/ `EB_EDITDELTA`. Per-keystroke smear gone. Tests: `e6502UnitTests/Editor/
EditBufVmTests.cs` (17). **This works on the current FPGA today** (VRAM port).

### 2. CPU store-prefetch fix — `e6502/CPU.cs`
`ClocksForNext` prefetched store operands → a `STA $A0E3` read the VRAM data port
first, double-incrementing the auto-increment pointer. Fixed via
`WritesWithoutReading` in `GetOperand`: pure stores (STA/STX/STY/STZ) skip the
operand read. Real-6502-correct, ZERO cycle-count change. Unlocks VRAM auto-inc.
Test: `e6502Tests.StoreDoesNotReadItsTarget`. Detail:
`reference_cpu_prefetch_reads_store_operand`.

### 3. Direct char/color window — `$A200`–`$B19F` + plane-select `$B1A0`
Plain `STA $A200+offset` writes the screen; `$B1A0` selects plane (0=char,
1=color, 2=attr). Emulator: `CompositeBusDevice` + `VgcConstants.ScreenWin*`.
FPGA: `vgc.sv` (decode + cmd_* write inject + VRAM-read-pipeline reuse) +
`top.sv` (extend `vgc_read_sel` to `$A200-$B1A0` in BOTH decode paths — one
OR-term routes reads to the VGC AND excludes the range from the main_ram write).
asm: `nova.inc` `VGC_SCREENWIN[_END/_PLANE/_CHAR/_COLOR/_ATTR]`.

### 4. Screen-base register — `$A0ED` (zero-copy ring scroll)
Display row R shows physical row `(text_row + scroll_offset + text_top_row) mod
ROWS`. Emulator: `VirtualGraphicsController` (`RegTextTopRow`, `PhysicalTextRow`)
+ `TextPixelRenderer`. FPGA: folds `(scroll_offset + text_top_row) mod ROWS` into
a REGISTERED `combined_text_scroll` fed to `vgc_timing` — keeps the
combinational `real_row->char-addr` path unchanged (timing-neutral). asm:
`nova.inc` `VGC_TEXT_TOPROW`. Both default to no-op (`topRow=0`) so the new
bitstream is behaviorally identical for existing software.

## The bitstream

`e6502.FPGA/fpga/bit_backups/e6502.20260531_145001.screen-window.bit`
- yosys diag CLEAN; BRAM **176/208 DP16KD** (added zero new BRAM).
- Timing CLOSES: `clk_pixel` 30.06 MHz (req 28), all other domains pass. Built on
  beast; wrapper auto-seed-swept past a first-attempt miss (23.13 MHz).
- Working bitstream was NOT overwritten.

Flash (board must be on USB-JTAG of the machine running this):
```
openFPGALoader --board ulx3s -f <that .bit>     # SPI flash (persistent)
openFPGALoader --board ulx3s    <that .bit>     # SRAM (volatile test)
```

## REMAINING WORK

### A. Flash + HW-verify (do first)
Flash, confirm it **boots + renders normally** (both new features are dormant at
`topRow=0` / window-unused, so existing software must look identical). Then
exercise the new primitives directly (e.g. `STA $B1A0`=0 then `STA $A200+off` →
char appears at cell `off`; `STA $A0ED`=1 → display scrolls one row).
**Watch:** single plain-`LDA $A200+off` READ timing on real HW (BRAM latency vs
the `cpu_ce` window). Writes are solid (the editor's path); reads are verified in
sim with settle cycles but the single-LDA HW timing is unconfirmed. The editor
doesn't read the window (gap buffer is its source of truth), so this isn't a
blocker — but confirm before any app relies on window reads.

### B. editbuf rewrite (the DX payoff — needs a fresh session)
Rewrite `editbuf.s`'s render path to USE the primitives instead of the VRAM port:
- Cell writes: `STA $B1A0` to pick plane, then plain `STA $A200 + physoffset`.
  `physoffset = physrow*80 + screencol`, where the editor must map document rows
  to physical plane rows through the ring (see below).
- Scroll: write `$A0ED` (screen-base) + paint only the one newly-exposed row.
  Zero plane copies. This replaces the full-render-on-scroll path.
- Ring addressing: once `topRow != 0`, EVERY cell write must target
  `physrow = (topRow + visualrow) mod 50`. So the render becomes ring-aware
  throughout (it's a holistic change, not just the scroll path).
- New test infra: `EditBufVmTests` uses a 2-line buffer that never scrolls — add
  a taller-buffer harness to test scroll + the ring mapping.
- Constants are ready in `nova.inc` (VGC_TEXT_TOPROW, VGC_SCREENWIN*).
- MUST land with/after the FPGA bitstream is flashed, or the editor breaks on a
  bitstream that lacks the primitives. (Now satisfied once you flash.)
- Rebuild the editor ROM + push via NovaHost — NO synth needed.

### C. Expose + document the primitives (DX goal)
So *app* developers, not just the editor, get plain-`STA` screen access. That's
the whole "hardware does the work for developers" intent
(`feedback_hardware_does_the_work`).

## Build / test quick reference

- .NET: `dotnet test e6502UnitTests` (full) or `--filter FullyQualifiedName~EditBufVmTests`.
- Rebuild editor harness after editing editbuf.s: `cd assembly && make editbuf-test`.
- FPGA Verilator: `cd e6502.FPGA/test && make test_vgc_top_row` /
  `make test_vgc_screen_window` / `make test_vgc_regs` etc.
- FPGA synth: `make -C e6502.FPGA/boards/ulx3s diag` for diagnostics or
  `make -C e6502.FPGA/boards/ulx3s bitstream` for the full bitstream. Use
  `nova fpga check-timing <nextpnr-report.json>` for the timing gate.

## Gotchas / decisions (from memory)
- VRAM auto-increment is now usable from CPU loops (CPU fix). The editor's
  current render still uses explicit per-cell addressing (ctrl=0) — fine, but the
  rewrite uses the direct window instead anyway.
- FPGA modulo: runtime `%` is NOT proven in this RTL — use conditional subtracts
  (as `real_row` and `combined_text_scroll` do).
- `top.sv` has DUAL decode paths (`ifdef SYNTHESIS` + sim) — edit BOTH.
- Always synth on beast, Verilator-first, never ship a timing-failing bitstream.
