# Turtle module: Logo-faithful coordinate system + WRAP

**Date:** 2026-06-06
**Branch:** `feat/turtle-logo-coordinates`
**Status:** validated design, implementing

## Problem

The TURTLE module (`modules/turtle/turtle.s`, module `$07`) reports the turtle's
position in **raw screen pixels**: home is `(160, 80)` and `XCOR`/`YCOR` return
those pixels directly. Every Logo dialect (UCBLogo, FMSLogo, Terrapin, Apple,
MIT, Python `turtle` in `logo` mode) instead puts the origin `(0,0)` at the
**center of the screen** with **+Y up**. So `HOME` should report `(0,0)`, `FD 50`
should report `YCOR 50`, and `SETXY -50 50` should go left-and-up. Today it
reports `(160, 80)`, which is wrong and surprises every Logo user.

## Research (what "people expect")

Verified against UCBLogo 6.x, FMSLogo, Terrapin, MIT Logo, Python `turtle`:

| Aspect | Convention | Already correct in module? |
|---|---|---|
| Home position | center of screen, reports `(0,0)` | ❌ reports `(160,80)` |
| +X | right | ✅ |
| +Y | **up** (math convention, opposite of framebuffer) | ✅ (FD negates cos; TOWARDS inverts) |
| Heading 0 | north / up | ✅ |
| Angle direction | clockwise | ✅ |
| `SETXY` | two numbers; `SETPOS [x y]` is a list | ✅ (list unpack is in the Logo adapter) |
| Edge default | **WRAP** (reappear opposite edge) | ❌ currently unbounded/WINDOW-ish |

Only the **reported/accepted coordinate frame** and **edge behavior** are wrong.
The heading math, FD/BK move-math, and TOWARDS are already Logo-correct.

The Logo adapter (`novalogo/eval.s logo_adapt_turtle`) passes evaluated numbers
straight through with no coordinate translation, so **the whole convention lives
in the module** — Logo needs no coordinate change.

## Decisions (confirmed with user)

1. **Center at full-plane `(160, 100)`** (not the split-viewport center 80) — a
   symmetric coordinate system over the full 320×200 plane. Logo X ∈ [−160, 159],
   Y ∈ [−99, 100]. The split-screen text band just hides the bottom of the plane;
   the coordinate system is mode-independent.
2. **WRAP is the default edge behavior**, with **full line wrapping** (a move
   that crosses an edge draws the wrapped segments on the opposite side).
3. **Fix the VGC line command** to read signed-16 endpoints and clip to the
   plane — the correct layer. This also fixes a latent bug (negative line
   endpoints today read as ~65000 → garbage + ~65000-iteration Bresenham) for
   every caller (GRAPHICS, BASIC).

## Approach: boundary translation (keep the tested render/move core)

Internal turtle state stays **screen-pixel** (home `160,100`). Only translate at
the op boundaries, so FD/BK move-math, the pen-line VGC draw, sprite clamping,
and their tests stay byte-for-byte identical.

```
Logo -> screen:  sx = lx + 160        sy = 100 - ly
screen -> Logo:  lx = sx - 160        ly = 100 - sy
```

- **In** (`SETXY`/`SETX`/`SETY`; `TOWARDS` target): convert Logo→screen.
- **Out** (`XCOR`/`YCOR`): convert screen→Logo.
- `HOME` already lands at screen center → reports `(0,0)` for free.
- `TOWARDS`: convert the target Logo→screen in the mailbox, then the existing
  ATAN2 body (which already does the screen-Y inversion) runs unchanged.

## WRAP

The turtle's stored screen position is always reduced into `[0,320) × [0,200)`
after every move, so reporters always return in-range values and the icon never
needs the off-plane clamp.

**Position:** after a move, `wrap`: `sx mod 320`, `sy mod 200` (signed; frac kept).

**Pen lines (full wrapping) — tile-shift:** a move goes from `old` (in-bounds)
to `new_unwrapped` (possibly off-plane). On a torus this is the unwrapped line
drawn shifted by every plane-tile it crosses, clipped to the viewport. So:

```
fx = floor(new_x / 320),  fy = floor(new_y / 200)      ; signed tile counts
for kx in min(0,fx)..max(0,fx):
  for ky in min(0,fy)..max(0,fy):
    vgc_line(ox - kx*320, oy - ky*200, nx - kx*320, ny - ky*200)
```

The VGC clips each shifted copy. Common case `fx=fy=0` → a single line =
today's behavior. One edge crossing → two tiles. The shifted endpoints can be
negative, which is exactly why the VGC line must accept signed-16 + clip.

**Order in `turtle_render`:** erase old turtle → draw tile-shifted pen line using
the *unwrapped* new position → wrap the stored position into bounds → redraw the
turtle at the wrapped position.

## VGC change (C#)

`VirtualGraphicsController.ExecuteGfxCommand` `CmdLine`: sign-extend the four
endpoint words (`(short)`). `BlockGraphics.Line`: Cohen–Sutherland clip to
`[0,Width) × [0,Height)` before Bresenham (per-pixel `Plot` already rejects
out-of-range, so this is a correctness/perf guard, not a visual change for
existing in-bounds callers). FPGA line parity is a noted follow-up (turtle
module is exercised on the Avalonia VGC today).

## Test plan (TDD)

**VGC (`AvaloniaVgcTests`):** negative endpoint clips not garbles; line fully
left of plane draws nothing; line crossing left edge draws the in-bounds part;
no hang.

**Module (`TurtleModuleTests`):** update `TURTLE_CENTER_Y` 80→100 and the
center-peek constants. New convention tests via the **reporters** (public
contract): `HOME`→XCOR 0/YCOR 0; `FD 50`→YCOR 50, XCOR 0; `RT 90 FD 50`→XCOR 50,
YCOR 0; `SETXY -50 50`→XCOR −50/YCOR 50 and internal screen `(110, 50)`;
`SETX`/`SETY` centered; `TOWARDS 100 0`→~east, `TOWARDS 0 80`→~north. WRAP:
move past top edge wraps Y into range; past right edge wraps X; wrapped pen line
draws pixels on both edges.

## TOWARDS returns a Logo heading

Discovered during research: `TOWARDS` returned a **math angle** (east=0°, CCW),
not a **Logo heading** (north=0°, east=90°, clockwise), so `SETHEADING TOWARDS
[x y]` pointed ~90° wrong. Fixed in the reporter: `heading = (90 − mathdeg) mod
360`. Now east→90, north→0, south→180, west→270, matching every Logo.

## Out of scope (follow-ups)

- FENCE / WINDOW mode switching (`UCBLogo` `FENCE`/`WINDOW`).
- FPGA VGC line signed-clip parity.
- Sub-pixel (`SETXY 10.5`) — `SETXY` stays integer, matching current behavior.
