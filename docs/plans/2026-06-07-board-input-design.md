# Board Input — `input.inc` NDK + BASIC `JOY`/`SW`

**Date:** 2026-06-07
**Status:** Design approved (brainstorming). Ready for implementation plan.

## Goal

Expose the board-input hardware (joystick directions + 2 fire buttons, 4 DIP
switches) as a **shared, reusable NDK** that any runtime reads directly, and
add BASIC `JOY` / `SW` reporter functions.

This is the genuine arcade-game input gap from the NOVA product vision. It is
**not** a ROM-budget problem — most of the arcade vocab (WAITVBL, integer/
fixed-point math via the copro, RND) already ships. Input was the real hole.

## What already exists (discovered 2026-06-07)

The board-input **hardware controller is already built on both targets**, with
identical register layout and semantics:

| Reg | Addr | Purpose |
|-----|------|---------|
| `BOARD_INPUT_BUTTONS`        | `$BA9C` | bit0=PWR, 1=FIRE1, 2=FIRE2, 3=UP, 4=DOWN, 5=LEFT, 6=RIGHT |
| `BOARD_INPUT_SWITCHES`       | `$BA9D` | bits0–3 = SW1–SW4 |
| `BOARD_INPUT_IRQ_ENABLE`     | `$BA9E` | bit0=button-change IRQ, bit1=switch-change IRQ |
| `BOARD_INPUT_IRQ_STATUS`     | `$BA9F` | W1C: bit0=buttons changed, bit1=switches changed |
| `BOARD_INPUT_BUTTON_CHANGES` | `$BAA0` | W1C mask of changed button bits (edge detect) |
| `BOARD_INPUT_SWITCH_CHANGES` | `$BAA1` | W1C mask of changed switch bits |

- **FPGA** (`e6502.FPGA/rtl/top.sv` + `fpga_top.sv`): `btn[6:0]` and `sw[3:0]`
  pins (already labelled FIRE1/FIRE2/UP/DOWN/LEFT/RIGHT in `ulx3s.lpf`),
  20-bit debounce, 2FF sync, IRQ, edge masks. `dbg_poke_board_input` lets the
  debug bridge inject input for HW tests.
- **Emulator** (`e6502.Avalonia/Hardware/CompositeBusDevice.cs`): the registers,
  IRQ (`BoardInputIrqPending`, wired in `MainWindow.axaml.cs`), W1C edge
  detection, and `BoardButtonState`/`BoardSwitchState` properties. Behaviour is
  already unit-tested in `AvaloniaCompositeBusTests.cs`.
- **Bit constants** live in `e6502.Avalonia/Hardware/VgcConstants.cs`
  (`BoardButtonUp=0x08` … `BoardButtonRight=0x40`, `BoardButtonMask=0x7F`).

## What's missing (this work)

1. **Runtime NDK** — no `runtime/asm/input.inc` exists. This is the shared
   single source of register addresses + bit masks for every runtime.
2. **BASIC bindings** — no `JOY` / `SW`.
3. **Emulator host input** — nothing drives `BoardButtonState`; the GUI
   joystick is dead. No TCP command to inject input for integration tests.

## Architecture decision: shared NDK include, NOT a paged lib_call module

The user's intent ("input belongs in a module, usable by anything, even though
it's MMIO") is realized as a **shared NDK include** (`input.inc`), like
`vgc.inc` / `dma.inc` — every runtime `.include`s it and reads the registers
directly.

It is **not** a `lib_call` paged module. Input is hot-path MMIO read every
frame at 60 Hz; a paged module would DMA 16 KB in + 16 KB back per read and
evict the resident graphics module each frame (constant thrash). Direct MMIO
(`LDA $BA9C` = 4 cycles) is the only sane choice. This is exactly the
thin-hot-MMIO rule in `feedback_relocation_routing_policy` and matches the
`@xtk_playing`/`SPRCOLL` reporter precedent (hot reporters live in **main**,
direct MMIO — not ext ROM, which would page-in on every call).

## Components

### 1. `runtime/asm/input.inc` (the NDK)

Definitions only — no code, no page-in cost:

```
BOARD_BUTTONS    = $BA9C
BOARD_SWITCHES   = $BA9D
BOARD_IRQ_ENABLE = $BA9E
BOARD_IRQ_STATUS = $BA9F
BOARD_BTN_CHANGES = $BAA0
BOARD_SW_CHANGES  = $BAA1

JOY_FIRE1 = $02
JOY_FIRE2 = $04
JOY_UP    = $08
JOY_DOWN  = $10
JOY_LEFT  = $20
JOY_RIGHT = $40
JOY_MASK  = $7E      ; all directions + fires, excludes PWR bit0

SW1 = $01
SW2 = $02
SW3 = $04
SW4 = $08

BOARD_IRQ_BUTTONS  = $01
BOARD_IRQ_SWITCHES = $02
```

Consumed by BASIC now; Logo/NDK later (follow-up). Keep names/values in lock-
step with `VgcConstants.cs` and `rtl/top.sv` — single source of truth.

### 2. BASIC `JOY` / `SW` (main ROM, direct MMIO)

Two XTK **function-only, bare** reporter tokens (no parens — the `PLAYING`
precedent). Decision: bare `JOY` / `SW`.

- `XTK_JOY = $73`, `XTK_SW = $74` (next free ids; current max is `$72`).
- Name strings appended to `TAB_XTKSTR`; command-table slots → `LAB_15D9-1`
  (function-only, like other reporters); `@func_ids` += `XTK_JOY,XTK_SW`;
  `@func_addrs` += `@xtk_joy-1,@xtk_sw-1`.
- Handlers (copy of `@xtk_playing`):

  ```
  @xtk_joy
        JSR  LAB_IGBY          ; consume extension token id
        LDA  BOARD_BUTTONS     ; $BA9C
        AND  #JOY_MASK         ; $7E (drop PWR)
        TAY
        JMP  @ret_0ay          ; A=0,Y=byte -> FAC1

  @xtk_sw
        JSR  LAB_IGBY
        LDA  BOARD_SWITCHES    ; $BA9D
        AND  #$0F
        TAY
        JMP  @ret_0ay
  ```

- **Budget:** ~name(7) + cmd-table(4) + func_ids(2) + func_addrs(4) +
  handlers(~28) ≈ **~45 B**. Fits the **59 free** in main. No Group A reversal
  needed.
- `min_mon.asm` `.include "input.inc"` (or add constants to the existing
  novavm/runtime include path so `BOARD_BUTTONS` resolves).

### 3. Emulator host input (Avalonia)

- **Keyboard map** in `MainWindow.axaml.cs` key handlers: Arrow keys →
  UP/DOWN/LEFT/RIGHT, `Z` → FIRE1, `X` → FIRE2. Set/clear the bit in
  `_bus.BoardButtonState` on KeyDown/KeyUp (the setter already does edge +
  IRQ). Must not interfere with the existing text key queue (these keys still
  feed the screen editor; board input is an additional consumer — confirm no
  conflict, e.g. only mirror arrows to JOY, leave text queue intact).
- **DIP switches:** TCP-only for now (no GUI). Decision: TCP-only.
- **TCP command** in `EmulatorTcpServer.cs`: `{"command":"board_input",
  "buttons":<byte?>, "switches":<byte?>}` → sets `BoardButtonState` /
  `BoardSwitchState`. Enables sim6502 integration tests to "press" inputs.

## Data flow

```
host key / FPGA btn pin -> debounce/2FF -> $BA9C -> (BASIC) LDA $BA9C
  -> AND #$7E -> FAC1 -> JOY value
```

## Error handling / edge cases

- `JOY` / `SW` take no args; trailing tokens are not consumed beyond the token
  id (matches `PLAYING`). `J=JOY` and `IF JOY AND 8` both parse.
- PWR bit (bit0) is masked out of `JOY` (it's a reset button, not a game input).
- Boolean idiom: `JOY AND 8` is nonzero/zero — works with `IF`. (Reporters
  here return the raw mask, not -1/0; that is the chosen minimal vocab.)

## Testing

1. **Avalonia unit** — extend `AvaloniaCompositeBusTests`: set
   `BoardButtonState`, read `$BA9C`, assert mask + edge/IRQ already covered.
2. **BASIC integration** (Sim6502TestRunner): inject buttons (via the new TCP
   command or direct `_bus.BoardButtonState` in-process), run
   `10 PRINT JOY` / `20 PRINT SW`, assert printed values; verify `JOY AND 8`
   branch logic.
3. **FPGA** — new `test_board_input.sv`: drive `btn`/`sw`, step debounce,
   assert `$BA9C`/`$BA9D` + change masks + IRQ. HW path via
   `dbg_poke_board_input` from a sim6502 HW run.
4. **ROM mirror sync** per `reference_rom_mirror_sync` (basic.bin → Avalonia/
   CLI/FPGA mirrors + `make -C e6502.FPGA hex`).

## Out of scope (follow-ups)

- Logo / NDK `JOY` bindings (they `.include input.inc` + read directly).
- Real USB gamepad host input (vs keyboard) in the emulator.
- DIP-switch GUI affordance in Avalonia.
- `JOY`-driven IRQ helpers in BASIC (the IRQ regs exist; no BASIC binding yet).

## Risks

- **Short keyword `SW`** steals a plausible variable name (`SW` tokenizes as
  the keyword, not a variable). `SWAP` was removed (3cd3a85) so no prefix
  clash, but validate tokenizer behaviour; fall back to `DIPSW` if `SW`-as-
  variable breaks existing programs/tests.
- Keyboard arrows are already consumed by the screen editor / cursor movement;
  ensure the JOY mirror is additive and doesn't break editing or the key queue.
