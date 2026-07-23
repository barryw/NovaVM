# Math Coprocessor Design

**Date:** 2026-04-23
**Status:** v2 implemented: scalar ops, DIV/ATAN2, integer square root, RNG, and fixed-point vector helpers
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
- **16.8 fixed-point positions** in software: 16-bit pixel integer plus
  8-bit fraction. Absolute screen positions can exceed signed 8.8 range
  on a 320-pixel-wide display, so positions are not stored as raw 8.8.
- **8.8 signed fixed-point** for velocities, scalars, restitution, and
  other values whose useful range fits -128.0 to +127.996.
- **10.6 signed fixed-point** is the preferred packed 16-bit format for
  fractional screen-space deltas when a coprocessor operation needs both
  sign and sub-pixel precision across the full display.
- **1.7 signed fixed-point** for trig outputs (range −1.0 to +0.992, stored
  as `s8`). Matches the natural information content of `sin(angle)`.
- **8-bit unsigned angles**, 0–255 = full circle. No radians anywhere.

16.16 and float were considered and rejected for the coprocessor ABI — both
add MMIO surface and register-file cost without enabling a 2D game use case
that `16.8` positions plus 16-bit packed operands cannot hit at 320×240.

The coprocessor itself treats most operands as signed 16-bit values. The
caller owns the interpretation: integer, 8.8, 10.6, or another fixed-point
layout. `MUL_FX` is the one format-specific Tier 1 operation; it is explicitly
signed 8.8 × signed 8.8 → signed saturated 8.8.

## Operation set (Tier 1)

| Op | Semantics | Latency | Resources |
|---|---|---|---|
| `MUL16` | signed 16×16 → 32 | 1 fabric cycle | 1 DSP |
| `MUL_FX` | 8.8 × 8.8 → 8.8 saturated | 1 fabric cycle | 1 DSP + post-shift/sat LUTs |
| `DIV_S32_16` | signed 32÷16 → 16 quot + 16 rem | 32 fabric cycles | iterative divider |
| `SINCOS` | u8 angle → (sin_1p7, cos_1p7) | 1 fabric cycle | quarter-wave LUT |
| `ATAN2` | (s16 dy, s16 dx) → (u8 angle, u16 approx hypot) | 8 fabric cycles | CORDIC angle + DIST_APPROX hypot |
| `DIST_APPROX` | (s16 dx, s16 dy) → u16 approx sqrt(dx²+dy²) | 1 fabric cycle | ~30 LUTs |
| `RNG` | xorshift32 on read | 1 fabric cycle | ~30 LUTs |
| `VEC_DOT_S16` | (ax,ay)·(bx,by) with s16 components → s32 | 3 fabric cycles | shared multiplier |
| `VEC_DOT_FX` | Q8.8 vector dot → saturated Q8.8 | 3 fabric cycles | shared multiplier |
| `VEC_CROSS_S16` | ax·by − ay·bx with s16 components → s32 | 3 fabric cycles | shared multiplier |
| `VEC_LEN2` | ax² + ay² with s16 components → u32 | 3 fabric cycles | shared multiplier |
| `VEC_SCALE_FX` | Q8.8 vector × Q8.8 scalar → two saturated Q8.8 results | 3 fabric cycles | shared multiplier |
| `SQRT_U16` | unsigned 16-bit integer → floor square root | 10 fabric cycles | iterative root |

**Explicitly deferred:**
- `DIV_FX` — emulate with MUL_FX + reciprocal LUT if needed.
- Wider or floating-point `SQRT` — compose it in software from `SQRT_U16`
  when the numeric format needs more range or fractional precision.
- Matrix transforms — composable in software from SINCOS + MUL16.
- 2×2 vector rotate — composable in software; 2 MUL_FX + 2 adds.

**Implementation staging:** v1 landed the MMIO block, CPU stall plumbing,
Avalonia parity, constants, and tests for `MUL16`, `MUL_FX`, `SINCOS`,
`DIST_APPROX`, and `RNG`. v2 implements `DIV_S32_16`, `ATAN2`, `SQRT_U16`,
and the fixed-point vector helper bank.

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

The original `$BAA0-$BABF` placement conflicts with the existing file metadata
buffer at `$BAB0-$BB1F`, and Avalonia still had stale blitter comments claiming
ownership through `$BAA2`. Existing metadata is already a software-visible
contract, while the math coprocessor is new, so the coprocessor moves to the
first clean region after metadata: `$BB20-$BB4F`.

```
; Math Coprocessor — $BB20 … $BB4F                            (* = trigger)

$BB20  MUL16_A_LO       ┐
$BB21  MUL16_A_HI       │ signed 16×16 → RES[0..3] (s32)
$BB22  MUL16_B_LO       │
$BB23  MUL16_B_HI  *    ┘

$BB24  MULFX_A_LO       ┐
$BB25  MULFX_A_HI       │ 8.8 × 8.8 → RES[0..1] (sat 8.8)
$BB26  MULFX_B_LO       │
$BB27  MULFX_B_HI  *    ┘

$BB28  DIV_N_LO         ┐
$BB29  DIV_N_1          │ signed s32÷s16 → RES[0..1] quot,
$BB2A  DIV_N_2          │                   RES[2..3] rem
$BB2B  DIV_N_HI         │ div-by-zero → $FFFF, no trap
$BB2C  DIV_D_LO         │
$BB2D  DIV_D_HI   *     ┘

$BB2E  SINCOS_ANGLE *     u8 angle → RES[0]=sin_1p7, RES[1]=cos_1p7

$BB2F  ATAN_DY_LO      ┐
$BB30  ATAN_DY_HI      │ (s16 dy, s16 dx) →
$BB31  ATAN_DX_LO      │     RES[0]=angle_u8, RES[1..2]=hypot_u16
$BB32  ATAN_DX_HI *    ┘

$BB33  DIST_DX_LO      ┐
$BB34  DIST_DX_HI      │ (s16 dx, s16 dy) → RES[0..1]=u16 approx
$BB35  DIST_DY_LO      │
$BB36  DIST_DY_HI *    ┘

$BB37  RNG               read: advances + returns u8
                         write: seed byte (round-robin, 4-byte state)

$BB38  RES_0            ┐
$BB39  RES_1            │ Shared 32-bit result bank — read before next op.
$BB3A  RES_2            │
$BB3B  RES_3            ┘

$BB3C  CAPS_0            bit 0=MUL16, bit 1=MULFX, bit 2=SINCOS,
                         bit 3=DIST_APPROX, bit 4=RNG,
                         bit 5=DIV_S32_16, bit 6=ATAN2,
                         bit 7=SQRT_U16
$BB3D  CAPS_1            vector capability bitmap:
                         bit 0=VEC_DOT_S16, bit 1=VEC_DOT_FX,
                         bit 2=VEC_CROSS_S16, bit 3=VEC_LEN2,
                         bit 4=VEC_SCALE_FX
$BB3E  STATUS            last operation status:
                         bit 0=divide by zero, bit 1=overflow/saturation,
                         bit 7=unimplemented operation
$BB3F  VERSION           math coprocessor ABI version, v2 reads $02

$BB40  VEC_AX_LO         ┐
$BB41  VEC_AX_HI         │ vector operand A.x
$BB42  VEC_AY_LO         │ vector operand A.y
$BB43  VEC_AY_HI         │
$BB44  VEC_BX_LO         │ vector operand B.x
$BB45  VEC_BX_HI         │
$BB46  VEC_BY_LO         │ vector operand B.y
$BB47  VEC_BY_HI         │
$BB48  VEC_SCALAR_LO     │ Q8.8 scalar for VEC_SCALE_FX
$BB49  VEC_SCALAR_HI     ┘
$BB4A  SQRT_LO           │ unsigned 16-bit radicand
$BB4B  SQRT_HI      *    ┘ write high byte to trigger; RES[0..1]=floor root
$BB4E  VEC_OP       *    write: 1=DOT_S16, 2=DOT_FX, 3=CROSS_S16,
                                4=LEN2, 5=SCALE_FX
```

Two notes on the layout:
- **Shared `RES_0..3`.** Simpler mux, smaller LUT count in the read path.
  Game code reads results before issuing the next op. (Given sync-stall,
  this is natural — the result is sitting there the cycle after trigger.)
- **RNG seed via round-robin writes to $BB37.** Four writes load a 32-bit
  seed; internal byte-pointer auto-increments and wraps. Reset default is
  a non-zero magic value (`$DEADBEEF`) because xorshift32 locks at 0.
- **Capability/status registers.** `CAPS_0`, `CAPS_1`, `STATUS`, and
  `VERSION` are read-only from 6502 code. `STATUS` is overwritten by each
  operation; successful operations write `$00`, saturating operations set
  `STATUS_OVERFLOW`, and reserved triggers set `STATUS_UNIMPLEMENTED`.

## Algorithms

### MUL16 and MUL_FX

Both share a single `MULT18X18D` DSP block with different post-processing:
- `MUL16`: full 32-bit product, no shift, no saturation. RES_0..RES_3.
- `MUL_FX`: arithmetically shift the 32-bit product right by 8 (Q8.8 ×
  Q8.8 = Q16.16, keep Q8.8). Saturate the shifted result to signed 16-bit
  range: `$7FFF` for positive overflow, `$8000` for negative overflow.

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
writes to `$BB37` overwrite the four bytes of state in round-robin order.

## RTL file structure

```
e6502.FPGA/rtl/
├── math_copro.sv          ; top wrapper, MMIO decode, stall, result mux
├── math_mul.sv            ; optional later split for shared DSP mult
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
1. Declare `math_reg_sel = (cpu_addr >= 16'hBB20 && cpu_addr <= 16'hBB4F)`.
2. Add `math.cpu_din` inside the existing grouped `$BA/$BB` MMIO mux. Do not
   append another long priority branch to the final CPU read mux.
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
`e6502.Avalonia/Hardware/MathCoprocessor.cs`, intercepting `$BB20-$BB4F` via
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
- `MUL_FX(1.5, 1.5)` = 2.25 exactly (`$0240`), proving ordinary scalar
  values do not saturate accidentally.
- `MUL_FX(100.0, 2.0)` must saturate to `$7FFF`.
- `MUL_FX(-100.0, 2.0)` must saturate to `$8000`.
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
- Free MMIO range revised: `$BB20-$BB4F` is used after the `$BAB0-$BB1F`
  metadata buffer and before the `$BC00-$BFFF` XMC windows.
- ECP5 DSP reference: Lattice TN-02204 (in `~/Downloads/`).
- CORDIC reference: Volder, "The CORDIC Trigonometric Computing Technique"
  (1959). Any modern textbook.
