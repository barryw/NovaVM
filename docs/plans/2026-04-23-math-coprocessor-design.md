# Math Coprocessor Design

**Date:** 2026-04-23
**Status:** Design locked, implementation pending
**Target:** FPGA (ULX3S-85F) and Avalonia software emulator

## Motivation

Games running on the 12 MHz 6502 spend a large fraction of their cycle budget
on math. Software `MUL16` is ~100 cycles; software trig via polynomial or
table-lookup is multi-hundred cycles; `DIV` on 6502 is agonizing. A
memory-mapped math coprocessor — implemented in the ECP5 fabric and mirrored
in C# for Avalonia — lets game code do signed 16×16 multiply in a single
bus-cycle CPU stall, and combined sin/cos lookup in two cycles.

## Numeric format

- **Signed 16-bit integer** as the native whole-number type.
- **8.8 signed fixed-point** for positions, velocities, scalars.
- **1.7 signed fixed-point** for trig outputs (range −1.0 to +0.992, stored
  as `s8`). Matches the natural information content of `sin(angle)`.
- **8-bit unsigned angles**, 0–255 = full circle. No radians anywhere.

16.16 and float were considered and rejected — both add MMIO surface and
register file cost without enabling any 2D game use case that 8.8 can't
hit at 320×240.

## Operation set (Tier 1)

| Op | Semantics | Latency | Resources |
|---|---|---|---|
| `MUL16` | signed 16×16 → 32 | 1 fabric cycle | 1 DSP |
| `MUL_FX` | 8.8 × 8.8 → 8.8 saturated | 1 fabric cycle | 1 DSP + post-shift/sat LUTs |
| `DIV_S32_16` | signed 32÷16 → 16 quot + 16 rem | ~17 fabric cycles | ~200 LUTs iterative |
| `SINCOS` | u8 angle → (sin_1p7, cos_1p7) | 2 fabric cycles | 1 BRAM (sin LUT) |
| `ATAN2` | (s16 dy, s16 dx) → (u8 angle, u16 hypot) | 8 fabric cycles | CORDIC ~150 LUTs |
| `DIST_APPROX` | (s16 dx, s16 dy) → u16 approx sqrt(dx²+dy²) | 1 fabric cycle | ~30 LUTs |
| `RNG` | xorshift32 on read | 1 fabric cycle | ~30 LUTs |

**Explicitly deferred:**
- `DIV_FX` — emulate with MUL_FX + reciprocal LUT if needed.
- True `SQRT` — CORDIC hypot on ATAN2 covers the real use case;
  DIST_APPROX covers the collision-check case.
- Matrix transforms — composable in software from SINCOS + MUL16.
- 2×2 vector rotate — composable in software; 2 MUL_FX + 2 adds.

## Interaction model

**Synchronous CPU stall.** The coprocessor drives the `arlet_6502` core's
existing `RDY` input low while an operation is in flight, and raises it
when the result is latched. No status register, no IRQ, no polling.

Trade-offs considered:
- Async with IRQ — more 6502 code, hides op latency, but inner-loop
  setup cost (enable IRQ, wait, handle, disable) kills the win for
  sub-20-cycle ops.
- Poll-busy — tempting for pipelining DIV with other work, but in practice
  games do MUL/DIV in a batch then move on; polling overhead is a wash.

Sync stall is the simplest design that still beats software by 5–100×.

## Software ABI

Nova reserves `$20-$2F` in zero page as eight 16-bit pseudo-registers. These
are a system ABI convention used by ROM routines, assembly macros, and future
language runtimes. The math coprocessor does **not** snoop zero page directly;
it remains a normal MMIO device. ROM routines copy operands from the pseudo
registers to the math MMIO block, trigger the operation, and copy results back.

```
NVR0L = $20    NVR0H = $21
NVR1L = $22    NVR1H = $23
NVR2L = $24    NVR2H = $25
NVR3L = $26    NVR3H = $27
NVR4L = $28    NVR4H = $29
NVR5L = $2A    NVR5H = $2B
NVR6L = $2C    NVR6H = $2D
NVR7L = $2E    NVR7H = $2F
```

Initial convention:
- `NVR0` and `NVR1` are primary 16-bit operands.
- `NVR0` receives the primary 16-bit result for simple operations.
- `NVR2:NVR3` receives 32-bit results, low word first.
- `NVR4-NVR6` are argument extension and scratch registers.
- `NVR7` is syscall/coproc scratch or status mirror.

This gives assembly code an "extended register" feel without forking the 6502
ISA or toolchain. ca65 support should start as symbols/macros in `novavm.inc`,
not as custom opcodes.

## MMIO layout

Free address range in `$BA`: `$BA9C-$BAFF` (100 bytes unused after XMC,
Timer, Music, DMA, Blitter). The coprocessor claims `$BAA0-$BABF` (32 bytes).

```
; Math Coprocessor — $BAA0 … $BABF                            (* = trigger)

$BAA0  MUL16_A_LO       ┐
$BAA1  MUL16_A_HI       │ signed 16×16 → RES[0..3] (s32)
$BAA2  MUL16_B_LO       │
$BAA3  MUL16_B_HI  *    ┘

$BAA4  MULFX_A_LO       ┐
$BAA5  MULFX_A_HI       │ 8.8 × 8.8 → RES[0..1] (sat 8.8)
$BAA6  MULFX_B_LO       │
$BAA7  MULFX_B_HI  *    ┘

$BAA8  DIV_N_LO         ┐
$BAA9  DIV_N_1          │ signed s32÷s16 → RES[0..1] quot,
$BAAA  DIV_N_2          │                   RES[2..3] rem
$BAAB  DIV_N_HI         │ div-by-zero → $FFFF, no trap
$BAAC  DIV_D_LO         │
$BAAD  DIV_D_HI   *     ┘

$BAAE  SINCOS_ANGLE *     u8 angle → RES[0]=sin_1p7, RES[1]=cos_1p7

$BAAF  ATAN_DY_LO      ┐
$BAB0  ATAN_DY_HI      │ (s16 dy, s16 dx) →
$BAB1  ATAN_DX_LO      │     RES[0]=angle_u8, RES[1..2]=hypot_u16
$BAB2  ATAN_DX_HI *    ┘

$BAB3  DIST_DX_LO      ┐
$BAB4  DIST_DX_HI      │ (s16 dx, s16 dy) → RES[0..1]=u16 approx
$BAB5  DIST_DY_LO      │
$BAB6  DIST_DY_HI *    ┘

$BAB7  RNG               read: advances + returns u8
                         write: seed byte (round-robin, 4-byte state)

$BAB8  RES_0            ┐
$BAB9  RES_1            │ Shared 32-bit result bank — read before next op.
$BABA  RES_2            │
$BABB  RES_3            ┘

$BABC-BF  reserved
```

Two notes on the layout:
- **Shared `RES_0..3`.** Simpler mux, smaller LUT count in the read path.
  Game code reads results before issuing the next op. (Given sync-stall,
  this is natural — the result is sitting there the cycle after trigger.)
- **RNG seed via round-robin writes to $BAB7.** Four writes load a 32-bit
  seed; internal byte-pointer auto-increments and wraps. Reset default is
  a non-zero magic value (`$DEADBEEF`) because xorshift32 locks at 0.

## Algorithms

### MUL16 and MUL_FX

Both share a single `MULT18X18D` DSP block with different post-processing:
- `MUL16`: full 32-bit product, no shift, no saturation. RES_0..RES_3.
- `MUL_FX`: take bits [23:8] of the 32-bit product (Q8.8 × Q8.8 = Q16.16,
  keep the middle 16 bits for Q8.8). Saturate if top 8 bits disagree with
  the sign bit: `overflow = ~(&top) & ~(~|top)`.

### DIV_S32_16

Iterative restoring divider, one bit per fabric cycle, 16 cycles for the
16-bit quotient + 1 setup cycle = 17 total. Signed via magnitude extraction:
remember dividend and divisor signs, work on absolutes, fix signs of quot
and remainder at the end (C99-style truncated division).

Edge cases:
- `divisor == 0` → fast-path: RES = $FFFFFFFF, 1 cycle stall.
- `INT32_MIN / -1` → saturates to $7FFF (doesn't fit in s16).

### SINCOS

One 256-entry sin LUT stored in a single DP16KD block, 8 bits per entry
(s8 = 1.7 signed). Port A computes `sin(angle)`, port B computes
`cos(angle) = sin(angle + 64)`. Two cycles: register angle, read BRAM.

Quadrant folding considered — halves the table to 128 entries and adds sign
logic. Rejected: one DP16KD block can't be split below 1KB anyway, so the
fold saves no hardware and adds ~20 LUTs + 1 cycle of latency.

### ATAN2 (CORDIC vectoring)

Eight iterations of CORDIC vectoring mode. Each iteration: compare sign of
accumulator-y, shift (x, y) by the iteration index, add/subtract arctan
of 2^-i from the angle accumulator. Initial octant fold (sign of dx, dy,
|dy| vs |dx|) maps all inputs to the first octant and contributes the
top 3 bits of the output angle.

CORDIC vectoring mode yields:
- `angle` = actual atan2(dy, dx) accumulated from the rotations.
- `magnitude` = sqrt(dx² + dy²) × K, where K ≈ 1.647 is the CORDIC gain.

We correct the gain with a single constant multiply by `1/K ≈ 0.6073`
(hard-coded as shift-and-add: 0.6073 ≈ 0.5 + 0.0625 + 0.0312 + 0.0156...).
Hypot output is at RES[1..2] as u16.

Latency: 8 CORDIC cycles + 1 gain-correction cycle = 9 total. Round up
to 10 for octant unfold.

### DIST_APPROX

```
dist ≈ max + (min >> 1) - (min >> 3)          (1 cycle, pure comb logic)
     = max + 0.375 * min
```
where `max = max(|dx|, |dy|)` and `min = min(|dx|, |dy|)`. Worst-case
error < 3.7%. At 240-pixel screen extent this is ±4.4 pixel error at the
corners — acceptable for collision and proximity checks.

### RNG (xorshift32)

```
y = state
y ^= y << 13
y ^= y >> 17
y ^= y << 5
state = y
return low byte of y
```
Full period 2³²−1. Reset default state = `$DEADBEEF`. Seedable: four
writes to `$BAB7` overwrite the four bytes of state in round-robin order.

## RTL file structure

```
e6502.FPGA/rtl/
├── math_copro.sv          ; top wrapper, MMIO decode, stall, result mux
├── math_mul.sv            ; shared DSP mult for MUL16 and MUL_FX
├── math_div.sv            ; iterative signed 32/16 divider
├── math_trig.sv           ; sin LUT (1 BRAM) + combined SINCOS interface
├── math_cordic.sv         ; CORDIC vectoring for ATAN2 + hypot
├── math_rng.sv            ; xorshift32 with round-robin seed write
├── math_dist.sv           ; α·max + β·min combinational
└── math_copro_pkg.sv      ; opcode enum, latency constants

e6502.FPGA/test/
├── test_math_copro.sv     ; positive + negative unit tests
└── gen_math_refs.py       ; generates sin_ref.hex, atan2_ref.hex,
                           ;            xorshift32_ref.hex
```

Integration points in `rtl/top.sv`:
1. Declare `math_reg_sel = (cpu_addr >= 16'hBAA0 && cpu_addr <= 16'hBABF)`.
2. Add `math.cpu_din` to the CPU read-mux else-if chain.
   **Caution:** each `else if` adds a LUT level to the priority mux.
   We've been bitten going from 10→11 branches dropping pixel clock from
   25.83 → 23.45 MHz (see `feedback_priority_mux_timing.md`). Before
   shipping: run PNR and verify we stay ≥ 25 MHz. If we lose margin,
   collapse the chain into a parallel selector.
3. `cpu_rdy_combined = existing_rdy_signal & ~math_busy`.

## Resource budget

From `make diag` at design time (2026-04-23):

| Resource | Pre-copro used | Total | Copro estimate | Post-copro |
|---|---|---|---|---|
| DP16KD (BRAM) | 202 | 208 | +1 | 203 / 208 |
| MULT18X18D (DSP) | 39 | 156 | +2 | 41 / 156 |
| LUT4 | 19,505 | 83,640 | ~+600 | ~20,100 / 83,640 |

Comfortable fit with 5 BRAM blocks still spare post-integration.

## Software parity — Avalonia

The Avalonia emulator gets a C# mirror at
`e6502.Avalonia/Hardware/MathCoprocessor.cs`, intercepting `$BAA0-$BABF` via
`CompositeBusDevice`. Pure-integer C# implementations of every op. Stall is a
no-op (ops complete instantly in software).

The same `gen_math_refs.py` generates:
- `sin_ref.hex` — loaded into the RTL sin LUT via `$readmemh`, also
  compiled into `MathCoprocessor.cs` as a byte array.
- `atan2_ref.hex` — oracle for test assertions on both platforms.
- `xorshift32_ref.hex` — first 64 output bytes from a known seed.

This prevents RTL-vs-software drift: both platforms consume bit-identical
reference tables.

## Test strategy

### Positive tests
- MUL16: signed-edge table (corners of s16×s16 space).
- MUL_FX: fixed-point identities (1.0×1.0, 0.5×0.5, etc).
- DIV_S32_16: textbook (100/3), signs in all four quadrants.
- SINCOS: 64 angles against reference table, ±1 LSB tolerance.
- ATAN2: 32 (dy,dx) pairs covering all 8 octants + axes, ±1 LSB angle,
  ±0.5% hypot.
- DIST_APPROX: 256 random pairs, <4% relative error.
- RNG: 64-byte sequence from known seed, bit-exact match.

### Negative / boundary tests
- `MUL16(-32768, -32768)` = +1073741824 (sign bit of RES_3 must be 0).
- `MUL_FX(1.5, 1.5)` must saturate to +0.999 (not wrap to -0.75).
- `MUL_FX(-2.0, 2.0)` must saturate to -1.0.
- `DIV(x, 0)` → $FFFFFFFF, stall completes in ≤1 cycle.
- `DIV(INT32_MIN, -1)` → saturate to $7FFF quotient.
- Back-to-back DIV with same divisor, different dividends → no stale state.
- `ATAN2(0, 0)` → angle=0, hypot=0 (not NaN, not hang).
- `ATAN2` axis degenerates at all four cardinals return correct multiples of 64.
- `RNG` seeded with all zeros → auto-recovery to non-zero sequence.
- Concurrent op issue — MUL16 followed by SINCOS before RES is read:
  SINCOS stalls, wins cleanly, no glitched RES during transition.
- CPU attempts to read RES during in-flight DIV → stalled (defensive;
  shouldn't happen given sync-stall, but verified).

### Integration tests

`sim6502 --backend novavm --novavm-host novahost.local` runs real 6502
assembly exercising every operation through the ROM ABI routines and, in a
lower-level hardware test, at the real MMIO addresses. BASIC smoke tests should
call BASIC/runtime wrappers once exposed; user-facing BASIC should not require
raw MMIO writes.

## Future work (not in v1)

### BASIC / NovaBASIC intrinsics (future milestone)

Once the coprocessor is stable and in use from assembly, expose its ops as
BASIC-level intrinsics. Sketch:

```basic
10 X = 120 : Y = 80                         ' center of screen
20 FOR A = 0 TO 255 STEP 4
30   SX = X + COSCOP(A) * 60 / 128          ' COSCOP = SINCOS-cos
40   SY = Y + SINCOP(A) * 40 / 128          ' SINCOP = SINCOS-sin
50   PLOT SX, SY
60 NEXT A
70 PRINT "distance: "; DISTCOP(X-10, Y-10)  ' DIST_APPROX
80 PRINT "random: "; RNDCOP                 ' RNG read
```

Candidate intrinsic names (or equivalent — decide when scoping):
- `MULCOP(a, b)`, `MULFXCOP(a, b)`, `DIVCOP(a, b)`
- `SINCOP(angle)`, `COSCOP(angle)`, `ATAN2COP(dy, dx)`
- `DISTCOP(dx, dy)`, `HYPOTCOP(dx, dy)`
- `RNDCOP`, `SEEDCOP(value)`

Implementation: the BASIC interpreter's math dispatch recognizes these as
reserved functions and emits a short inline stub that writes operands to
MMIO and reads RES back. **Not scoped for v1 of the coprocessor.** Game
developers writing assembly get it immediately; the BASIC layer follows
in a later phase.

### Other future additions
- Vector 2×2 rotate as a single op (currently 2 MUL_FX + 2 adds in software).
- Reciprocal LUT + `DIV_FX` if a use case appears.
- Perspective divide / Mode-7 affine setup if 3D experiments start.

## Resources

- arlet_6502 `RDY` input: `rtl/arlet_6502/cpu.v:21`, `cpu.v:32` — existing
  stall mechanism we're hooking into.
- Free MMIO range verified: `$BA9C-$BAFF` unused in `rtl/top.sv` as of
  commit `2f21297` (2026-04-23).
- ECP5 DSP reference: Lattice TN-02204 (in `~/Downloads/`).
- CORDIC reference: Volder, "The CORDIC Trigonometric Computing Technique"
  (1959). Any modern textbook.
