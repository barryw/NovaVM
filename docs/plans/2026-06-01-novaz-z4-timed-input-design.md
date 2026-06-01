# NovaZ — Finishing Z4 Conformance

Date: 2026-06-01
Scope: Full V4 conformance. Closes the three gaps found in the Z3/Z4 review.

## Background

NovaZ already covers the full V1–V5 opcode set and the V3/V4 structural
differences (object format, packed addressing, screen model, store-form save).
The Z3/Z4 review left three Z4 gaps:

1. **Timed input** — `zvm_read` and `zvm_read_char` ignore the `time`/`routine`
   operands; both are plain busy-wait loops on `VGC_CHARIN`.
2. **Flags1 capabilities** — `zstory_configure_flags1` never writes header byte
   `$01`, so games cannot detect which style/timing features exist.
3. **`set_text_style`** — overwrites the colour cell from `(operand & 3)`; the
   spec defines styles as cumulative until style `0` resets them.

## 1. Timed input

### Time source

The **Timer Controller** at `$BA40` is the time source. It advances on
`bus.AdvanceCycles`, so it ticks in the headless smoke, on the FPGA, and in the
GUI — unlike the VGC frame counter, which only advances in the GUI render loop.
NovaZ is its first client.

Registers: `TimerCtrl $BA40` (bit 0 = enable), `TimerStatus $BA41` (read returns
and clears the "period elapsed" flag), `TimerDivL/H $BA42/43` (period in
100-cycle ticks). The reference clock is the **12 MHz FPGA CPU** (hardware is the
reference, not the GUI). One tenth of a second = 1,200,000 cycles = **12,000
ticks**, which fits the 16-bit divisor. NovaZ programs a fixed 0.1 s period and
counts periods in software, so the `time` operand (1..255 tenths) never overflows
the divisor.

### Mechanism

When `read`/`read_char` are called with `time > 0` and `routine > 0`:

- Enable the timer for a 0.1 s period; zero a "tenths elapsed" counter.
- In the existing key-poll loop, also read `TimerStatus`. Each elapsed period
  increments the counter; when it reaches `time`, fire the routine.
- **Fire routine:** save the read-loop state that the call could clobber
  (`zvm_store_var`, text/parse pointers, `zvm_input_len`), set
  `zvm_operand_lo/hi` to the routine's packed address, `zvm_operand_count = 1`,
  `zvm_store_var = $FF` (discard), record `zvm_frame_count`, `JSR
  zvm_call_common`, then drive `zvm_step` until `zvm_frame_count` returns to the
  saved depth (or a stop reason fires). The routine's return value is left in
  `zvm_value_lo/hi`.
- Return value ≠ 0 → **abort**: `read_char` returns 0; `read` terminates the
  input as if the player pressed return on what is typed so far. Otherwise reset
  the tenths counter and resume.
- Disable the timer on every exit path.

The timed routine is ordinary Z-code: it may print, switch windows, and update
the status line. NovaZ's window cursor save/restore handles the common
"redraw the upper window" pattern.

## 2. Flags1 capabilities (`zstory_configure_flags1`, V4 path)

Write header byte `$01` honestly for what NovaZ renders:

- bit 2 (bold) — **set** (bold maps to a bright colour cell)
- bit 3 (italic) — **clear** (no italic glyphs)
- bit 4 (fixed-pitch) — **set** (Nova text is inherently fixed-pitch)
- bit 7 (timed input) — **set** (implemented above)

## 3. `set_text_style` cumulative

Track an accumulated style bitset in a new zero-page byte. `set_text_style(0)`
resets it; any other value ORs its bits in. After each call recompute the colour
cell from the accumulated bold/reverse bits (italic and fixed do not change the
colour). Colour map is unchanged: NORMAL `$0C`, REVERSE `$C0`, BOLD `$0F`,
REVERSE+BOLD `$F0`.

The `z4-styles` fixture currently assumes replace semantics (it requests italic
straight after bold+reverse and expects normal). Update it to reset between
independent demos, and add a case that stacks bold then reverse and expects
`$F0`, which proves cumulative behaviour.

## Testing

- **Flags1:** a generated V4 story reads its own header byte `$01` and branches
  per bit, printing pass markers the smoke asserts.
- **Styles:** the updated `test-z4-styles` fixture (incl. the stacked case).
- **Timed input:** a generated V4 story runs `read_char` with `time`/`routine`
  where the routine increments a counter and returns true after N ticks; the
  story prints a marker proving the interrupt fired and aborted input. A second
  case proves a routine returning false lets input continue.
- All 11 Infocom game smokes + longplays and the Z3/Z5 spec smokes must stay
  green (no V4 game uses timed input, so they exercise the `time == 0` path).

## Notes

- No worktree: the working tree carries the (uncommitted) smoke-harness fix that
  the game smokes depend on. Implement in place.
