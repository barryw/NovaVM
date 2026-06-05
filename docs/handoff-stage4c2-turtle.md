# Handoff — Stage 4c.2 (turtle render + resident turtle)

**Read this first, then `docs/plans/2026-06-04-stage-4c-resident-turtle-wire-logo.md` (§ Stage 4c.2) and memory `project_stage4c_replan_2026_06_04.md`.** Execute with `superpowers:subagent-driven-development` (the plan mandates it). This doc carries all recon so you start with zero re-derivation.

## State at handoff (everything below is SHIPPED + reviewed + green)
- HEAD = `83cab14`. Session built `8285cc9..83cab14` (~17 commits, all on `main`).
- **Stage 4c.1 COMPLETE** (done-criteria #1): Logo graphics run through `lib_call(GRAPHICS)`.
- **Foundation COMPLETE** (4c.0a/0b/0c): a runtime can `lib_call` a paged module on emulator AND HW; the runtime's own extension survives a module page-in via re-page-on-demand.
- **Module-BSS band carved** (4c.2-1): `$0420-$08FF` reserved cross-runtime.
- Tree green: **335** tests across `~NovaLogo|~GraphicsModule|~CompositeLoader|~MailboxReservation|~EhBasic|~Basic`.
- Build artifacts (.o/.bin/.lst) are committed (repo convention). Unrelated pre-existing dirty files exist (extension.bin, novavm.inc, runtime_labels*, boot.json, demo.*, the PDF) — DO NOT stage them.

## Locked low-RAM map (cross-runtime, identical in Logo + BASIC; NCC deferred)
```
$0300-$031F  canonical mailbox (libabi.inc LIB_MBOX). $0318=LIB_RESIDENT, $0317=LIB_HOME_BANK.
$0320-$041F  resident loader band — libcall.bin @ $0320 (libabi.inc LIB_LOADER_BAND..END). JSR $0320 = lib_call.
$0420-$08FF  module-BSS band (1248B) — libabi.inc MODULE_BSS_BAND..END. graphics.cfg MODBSS lives here.
$0900+       runtime RAM: Logo BSS ($0900, novalogo.cfg) + HEAP_START=$09E0 (heap.s); EhBASIC Ram_base=$0900.
```
graphics.cfg MODBSS today: `start=$0420 size=$0140` (320B vsprite/msprite). **4c.2-2 grows `size` to fit the turtle buffers — must keep `start+size ≤ $0900`.**

## lib_call ABI (how the resident turtle will render) — `runtime/asm/libabi.inc`
Fill mailbox, `JSR LIB_LOADER_BAND` ($0320), read result:
- `LIB_MOD_ID=$0300` ← `MODULE_ID_GRAPHICS` ($01); `LIB_FN_ID=$0301` ← a `GFN_*`.
- `LIB_ARG0=$0303, ARG1=$0307, ARG2=$030B, ARG3=$030F` — 32-bit LE cells. **Module reads each cell's LOW WORD as s16** (graphics.s `copy_args_to_p`). Logo 16.8 (HI:LO int) → cell `[LO,HI,sign,sign]`, FRAC dropped.
- `LIB_STATUS=$0302` (0=OK), `LIB_RESULT=$0313` (low word = s16 result).
- `LIB_HOME_BANK=$0317` set to `ROMSWAP_LOGO` at Logo cold_start (done).
- Logo's marshalling helper is `ext_invoke` (novalogo/eval.s, near `ext_eval_args`): branches `ext_mod_id` $00→legacy `ensure_ext_resident`+`EXT_TRAMPOLINE`, `$FE`(MODULE_ID_GFXADAPTER)→adapter dispatch, else→single lib_call. Zeroes all 16 LIB_ARG bytes then fills n<EXT_ARGC.

## Turtle recon — `novalogo/extension.s` (the code to port/migrate)
**Buffers (currently at `$9C00`, TURTLE_SPR_SIZE=16, 256B each):**
- `turtle_source_shape  = $9C00` (icon shape; persistent)
- `turtle_rotated_shape = $9D00` (rotated icon; recomputed per draw)
- `turtle_saved_bg      = $9E00` (bg under turtle; persists draw→erase)
**State (`TURTLE_STATE_BASE=$9F00`, 18 bytes):** X_FRAC/LO/HI ($9F00-02), Y_FRAC/LO/HI ($9F03-05), HEADING_LO/HI ($9F06-07), PEN ($9F08), SHOWN ($9F09), COLOR ($9F0A), SPRITE ($9F0B), INITED ($9F0C), bg_x_lo/hi ($9F0D-0E), bg_y ($9F0F), bg_saved ($9F10), GFX_VISIBLE ($9F11). **Tests read X/Y/HEADING here (NovaLogoHarnessTests).**
**Render subs:** `ensure_gfx_mode`:247, `turtle_init`:562, `draw_line`:627 (uses TURTLE_COLOR + old_x/y + TURTLE_X/Y → vgc_line), `configure_turtle_vsprite`:666 (points vsprite engine at the 3 buffers, `BLT_SPACE_CPU`, 16×16), `erase_turtle_sprite`:788 (restore saved_bg), `draw_turtle_sprite`:806 (save bg + rotate-blit icon).
**Move-math builtins (to make resident):** `ext_fd`:262, `ext_bk`:408, `ext_rt`:425, `ext_lt`:446, `ext_home`:517, `ext_setxy`:1067, `ext_seth`:1242 — use MATH copro (`runtime/asm/math_copro.inc`) sincos/mul + 24-bit sub-pixel accumulate into $9F00 state.
**Turtle-entangled color (migrate here, currently legacy):** `ext_setpc`:1338 (→TURTLE_COLOR), `ext_setbg`:1348 (compound: ensure_gfx_mode + erase_turtle_sprite + VGC_BGCOL + GCLS + turtle_bg_saved).
**GFN reserved range:** `$B0-$BF turtle-render` (libgraphics.inc:467, currently stubbed to gfn_unimpl).

## Locked design decisions (from the 4c.2 brainstorm)
- **Resident-vs-passed split:** foundation OWNS turtle state; **pass position/heading/color to GFN_TURTLE_* via the mailbox** (module stays self-contained; not coupled to Logo's $9F00).
- **Buffer placement:** 3 vsprite buffers (768B) move into the module-BSS band ($0420+). vsprite engine reads them via `BLT_SPACE_CPU`.
  - **OPEN OPTION:** put buffers in XRAM instead → recovers ~768B of Logo heap (band shrinks to ~480B), but requires re-moving runtime BSS (churn) + vsprite space=XRAM. Weigh first thing in 4c.2-2.
- The band is sized (1248B) assuming low-RAM buffers; if you choose XRAM, re-shrink the band (one more cross-runtime move).

## The two remaining tasks
### 4c.2-2 — turtle-render fns in GRAPHICS ($B0-$BF)
Port `draw_turtle_sprite`/`erase_turtle_sprite`/`draw_line`/`ensure_gfx_mode` into `modules/graphics/graphics.s` as `GFN_TURTLE_*` wrappers over the existing vsprite ops (GFN_VS_* already exist) + vgc line/mode. Define ids in `runtime/asm/libgraphics.inc` ($B0-$BF). Add the 3 icon buffers to `graphics.cfg` MODBSS (grow `size`, keep ≤ $0900). Interface receives turtle pos/heading/color via mailbox; persistent buffers in module BSS. Add `GraphicsModuleTests` (Axis-2 dispatch) asserting the rotated-icon pixels + line. Decide the exact GFN set (e.g. TURTLE_DEFINE_ICON(BYTES), TURTLE_DRAW(x,y,heading,color), TURTLE_ERASE(), TURTLE_LINE(x0,y0,x1,y1,color), TURTLE_MODE).

### 4c.2-3 — turtle resident in Logo foundation
Move the move-math builtins (FD/BK/RT/LT/SETXY/SETH/HOME/XCOR/YCOR/HEADING/pen) into Logo's foundation as resident builtins doing MATH-copro math on $9F00 state, then `lib_call(GRAPHICS, GFN_TURTLE_*)` for rendering. Migrate SETPC/SETBG off the legacy path (they're turtle). Remove the turtle + graphics drivers from `novalogo_ext.bin` (extension shrinks toward just the editor). Verify reclaimed bank-0 headroom (4a freed 1086B) seats the resident turtle. **TEST ON HW:** stage updated novalogo.bin/ext via `nova` TCP; run a turtle program (FD/RT/REPEAT); observe.

## Critical gotchas (learned this session)
- **XRAM staging in tests:** any `CompositeBusDevice` test that runs a now-lib_call'd graphics/turtle command MUST `bus.LoadXram(0x060000, File.ReadAllBytes(RepoPath("modules","graphics","graphics.bin")))` first, or the command MISSes. Already staged in NovaLogoHarnessTests/NovaLogoTests/NovaLogoLibCallGraphicsTests — add it to any new test.
- **NovaHost is TCP-ONLY** (no HTTP/REST). Use the `nova` CLI via `dotnet run --project e6502.Nova` (the prebuilt AOT binary may be stale). For HW staging of the runtime extension into XRAM (re-page on HW), NovaHost firmware must stage the active runtime's extensionRom at `HOST_EXT_XRAM=$07C000` — firmware follow-on, not yet done (emulator side is done).
- **SendMessage is unavailable in this harness** — you cannot answer a paused subagent's question; re-dispatch fresh with the decision baked in. FRONT-LOAD all addresses/include-paths/decisions in implementer prompts.
- **Logo GC wedge:** non-compacting high-water-mark GC (gc.s) wedges on many-small-allocations near heap capacity. The 4c.2-1 carve shrank heap 1248B and exposed it (retuned `GarbageCollectionFreesMemory`). If the resident turtle or buffer choice pressures heap, watch for wedges; real fix = compacting GC (separate concern).
- **Build/install loop:** cc65 on PATH at `/Users/barry/Git/cc65/bin`. Logo: `cd novalogo && make && make install` (install copies bins to `e6502.Avalonia/Resources/`). Graphics module: `cd modules/graphics && make` (tests load `modules/graphics/graphics.bin` directly). Tests: `dotnet test e6502UnitTests/e6502UnitTests.csproj --filter "..."` (~4-5 min/run).
- **Two-stage review per task** (spec/correctness + quality), with made-to-pass falsification — that's how every 4c task was verified. Keep it up.

## First actions in the fresh session
1. Read this + the plan §4c.2 + the memory file.
2. Decide the 4c.2-2 buffer placement (low-RAM band vs XRAM) — recon `modules/graphics/graphics.s` vsprite ops + `runtime/asm/vsprite.inc` for the space options; weigh the heap-recovery vs churn.
3. Design the `GFN_TURTLE_*` set + mailbox interface; dispatch 4c.2-2.
4. Then 4c.2-3 (resident turtle + SETPC/SETBG migration + HW verify).
