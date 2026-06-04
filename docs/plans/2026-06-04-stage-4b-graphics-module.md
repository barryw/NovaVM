# Stage 4b — Build the GRAPHICS Module Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to execute this plan task-by-task.

**Goal:** Build `GRAPHICS` (`MODULE_ID_GRAPHICS = $01`) — the canonical paged graphics library = the superset of every NDK graphics driver — as one 16 KB module callable identically by every runtime through the Phase-3 `lib_call` ABI.

**Architecture:** A `.s` module assembled `ORG $C000` with the `lib_module_header` ("NL") macro, a dense RTS-trick dispatch table keyed on `LIB_FN_ID`, and ~110–150 **thin leaf wrappers** — each reads canonical mailbox cells (`$0303`…), calls an existing NDK driver entry, writes `LIB_RESULT`/`LIB_STATUS`. The module `.include`s the driver `.s` bodies (they co-assemble cleanly via `.ifndef` guards — recon-verified). Built to a 16 KB `.bin`, staged to XRAM `$060000` at boot, auto-paged into bank 1 on first `lib_call`.

**Tech Stack:** ca65/ld65 (65C02), the Phase-3 loader (`runtime/asm/libcall.s` + `libabi.inc` + `libmod.inc`), MSTest (`LibLoaderBus` for the loader path, `CompositeBusDevice` for hardware effects), NovaHost boot-staging.

---

## Recon verdict (measured, not assumed)

- **Size CONFIRMED:** the 7-core driver union assembles to **3,497 B** (matches the design exactly); full-11 (+anim/nvg/vtext/tween) = **6,319 B**. After wrappers (~110–150 × ~10–25 B ≈ 1.5–3.7 KB) + header + a dense dispatch table (~512 B), worst case ~8.5–10.5 KB → **~5.5–7.5 KB free** in the 16 KB window. One module holds the superset.
- **ABI (recon-exact):** mailbox `LIB_MOD_ID=$0300`, `LIB_FN_ID=$0301`, `LIB_STATUS=$0302`, `LIB_ARG0/1/2/3=$0303/$0307/$030B/$030F` (32-bit LE each), `LIB_RESULT=$0313`. Caller sets cells, `JSR lib_call` (resident loader at `$9C00`), reads RESULT/STATUS. Module header at `$C000`: `JMP dispatch` + `"NL"` + module_id + ver + fncount (via `lib_module_header` macro); `dispatch` at `$C00A` uses `ASL`/jtable/RTS-trick.
- **Loader hook:** `modtab_lookup` in `runtime/asm/libcall.s` is currently hardcoded for TEST (`$7F`→`$060000`); needs a `MODULE_ID_GRAPHICS` ($01) → XRAM `$060000`, `$2000` words entry.
- **Boot:** `boot.json` `libraries` entry `{name,id,path,base,size}` → firmware `streamSdramAsset`/`pokeSdramStream` into XRAM → demand-paged. `libgraphics.inc` does NOT exist (create it); `MODULE_ID_GRAPHICS=$01` already in `libabi.inc`.

## Scope decisions (user, 2026-06-04)

- **Full superset INCLUDING anim + tween.** §2's fn-id table ($00–$BF) covers the 7 core drivers + nvg + vtext; this plan extends it with **`$C0–$CF` anim** and **`$D0–$DF` tween** (all 11 drivers). Note: anim/nvg pull transitive deps (dma/fio/pager ≈ 512 B) — accepted, still fits.
- **Pipeline-first sequencing.** This session executes **4b.0 + 4b.1 only** (contract + skeleton + loader hook + one fn proven end-to-end on both test axes), then reassess before the bulk wrapper batches (4b.2–4b.11).

## Test architecture (two axes — both required)

`LibLoaderBus` (the Phase-3 loader harness) has **no peripherals** — VGC/sprite/blitter writes land in dead RAM. So every fn is validated on whichever axis proves it:

1. **Loader path** (`LibLoaderBus`, `LibCallTests`-style): stage `graphics.bin` into XRAM, drive the *real* `lib_call`, assert `LIB_STATUS=OK` + correct page-in/resident caching + RESULT for any fn whose result is pure mailbox data (e.g. `getx/gety`, `mem_read`, status/count reads). Proves build→stage→page→dispatch.
2. **Hardware-effect path** (`CompositeBusDevice`, real VGC/sprite/blitter): inject `graphics.bin` as the extension ROM (`ActiveRom.Extension` / romswap bank 1), set the mailbox, `JSR` the module's `dispatch` directly (bypassing the loader — paging is independently Phase-3-proven), assert the peripheral effect (gfx plane cleared, pixel plotted, sprite reg set, etc.). Proves wrapper→driver→hardware.

The full loader+module+hardware chain gets one integration smoke (4b.1) + a HW smoke (4b.final).

---

## Task 4b.0: `libgraphics.inc` — the fn-id contract

**Files:**
- Create: `runtime/asm/libgraphics.inc`

The shared contract every runtime + the module compile against. Defines the fn-id constants (grouped per the design §2 ranges + anim/tween), and documents each fn's mailbox arg layout. No code — constants + doc comments.

**Step 1: Write the constants.** `MODULE_ID_GRAPHICS` already lives in `libabi.inc` — reference it, don't redefine. Define `GFN_*` constants for every op, e.g.:

```asm
; libgraphics.inc — GRAPHICS module (MODULE_ID_GRAPHICS=$01) fn-id contract.
; Args are the canonical 32-bit LE mailbox cells LIB_ARG0..3 ($0303/$0307/$030B/$030F);
; signed-16 values occupy the low word. RESULT in LIB_RESULT ($0313). STATUS in LIB_STATUS.
.ifndef LIBGRAPHICS_INC
LIBGRAPHICS_INC = 1
.include "libabi.inc"

; --- $00-$0F draw ---
GFN_GCLS      = $00   ; () -> clears gfx plane
GFN_GCOLOR    = $01   ; (color:ARG0) -> set gfx pen
GFN_PLOT      = $02   ; (x:ARG0 signed16, y:ARG1 signed16)
GFN_UNPLOT    = $03   ; (x,y)
GFN_LINE      = $04   ; (x0,y0,x1,y1)  -> ARG0..3
GFN_CIRCLE    = $05   ; (cx:ARG0, cy:ARG1, rx:ARG2, ry:ARG3 [ry=rx if 0]) ellipse superset
GFN_RECT      = $06   ; (x0,y0,x1,y1)
GFN_FILL      = $07   ; (x0,y0,x1,y1)
GFN_PAINT     = $08   ; (x,y)
GFN_GTEXT     = $09   ; (x,y, BYTES strptr/len:ARG2)
; ... $10-$1F text/mode, $20-$3F hw-sprite, $40-$4F copper, $50-$5F blit/dma,
;     $60-$7F vsprite, $80-$9F msprite, $A0-$AF image/mem, $B0-$BF turtle-render,
;     $C0-$CF anim, $D0-$DF tween  (full table filled per-domain in 4b.2..4b.11)
.endif
```

Lay out the FULL range bases now (so fn-ids are stable), but only the draw range needs complete constants for 4b.1 — the rest land as each domain batch is implemented. Map each `GFN_*` to its target driver entry point in a comment (recon supplied the 148-entry inventory + `@in`/`@out` conventions in each driver's `.inc`).

**Step 2: Verify it assembles** (a tiny scratch `.s` that `.include`s it). No test commit yet — combine with 4b.1.

---

## Task 4b.1: Module skeleton + loader hook + GCLS proven end-to-end (THE milestone)

**Files:**
- Create: `modules/graphics/graphics.s`, `modules/graphics/graphics.cfg`, `modules/graphics/Makefile`
- Modify: `runtime/asm/libcall.s` (add `MODULE_ID_GRAPHICS` to `modtab_lookup`)
- Create: `e6502UnitTests/GraphicsModuleTests.cs`

### Step 1: Module skeleton + GCLS

`modules/graphics/graphics.s` (copy `tests/asm/testmod.s` structure, scale the table):

```asm
      .include "libabi.inc"
      .include "libmod.inc"
      .include "libgraphics.inc"

      .segment "CODE"
      lib_module_header MODULE_ID_GRAPHICS, LIB_ABI_VERSION, GFX_FN_COUNT

dispatch:
      lda     LIB_FN_ID
      cmp     #GFX_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     gfx_jtable+1,x
      pha
      lda     gfx_jtable,x
      pha
      rts
@bad: lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

gfx_jtable:
      .word   gfn_gcls-1            ; $00
      .word   gfn_unimpl-1         ; $01 (placeholder until 4b.2)
      ; ... grows per domain; unimplemented slots -> gfn_unimpl

gfn_unimpl:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

; --- $00 GCLS: clear the gfx plane ---
gfn_gcls:
      jsr     vgc_gcls             ; or the exact vgc.s entry for gfx-clear (confirm name in vgc.inc)
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

      ; driver bodies (co-assemble cleanly per recon)
      .include "vgc.s"
      ; (copper/sprite/blitter/vsprite/msprite/copper_split/nvg/vtext/anim/tween added as
      ;  their domains are implemented in 4b.2..4b.11 — include only what's referenced to keep
      ;  the build clean; final superset includes all 11)

      .segment "VECTORS"           ; $FFFA
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
```

> Confirm the exact gfx-clear entry in `runtime/asm/vgc.inc` (recon: vgc exposes mode/primitive ops via `VGC_P0..P7`; "GCLS" may be a specific primitive or a `vgc_mode`+clear sequence — read `vgc.inc`/`vgc.s` and use the real entry). `GFX_FN_COUNT` = highest implemented fn-id + 1 (start small; grows per domain). Determine the include scaffolding `vgc.s` needs (zeropage/segment predefs) from how `novalogo/extension.s` includes it.

`graphics.cfg` (copy `tests/asm/testmod.cfg`): `ROM start=$C000 size=$3FFA fill=yes` + `VECROM start=$FFFA size=$0006`. `Makefile`: `ca65 --cpu 65c02 -I ../../runtime/asm graphics.s -o graphics.o` then `ld65 -C graphics.cfg -o graphics.bin graphics.o`. Build → confirm 16384 bytes + header bytes at `$C003`=`"NL"`, `$C005`=$01.

### Step 2: Loader hook — `modtab_lookup` GRAPHICS entry

In `runtime/asm/libcall.s` `modtab_lookup`, add (mirroring the TEST entry):
```asm
      cmp     #MODULE_ID_GRAPHICS      ; $01
      bne     @next
      ; PGD_SRC = $060000, PGD_WORDS = $2000 (16 KB)
      ... set PGD_SRCL/M/H + PGD_WORDSL/H ...
      clc
      rts
@next:
      ... existing TEST entry ...
```
Read the current `modtab_lookup` + the TEST entry and replicate the exact register-set pattern. This must not break the existing `LibCallTests` (TEST module still resolves).

### Step 3: Test axis 1 — loader path (LibLoaderBus)

Add to `e6502UnitTests/GraphicsModuleTests.cs`, copying the `LibCallTests`/`LibLoaderBus` pattern: stage `modules/graphics/graphics.bin` into XRAM `$060000`, drive real `lib_call` for `GFN_GCLS`, assert:
- `LIB_STATUS == LERR_OK`,
- page-in happened once (MISS), `LIB_RESIDENT == $01` cached, home bank restored,
- a second call HITs (no page-in).

This proves build→stage→page→validate-header→dispatch→return for a real GRAPHICS module. (GCLS's VGC writes land in dead RAM here — that's fine; this axis proves the loader/dispatch, not the effect.)

Also re-run the existing `LibCallTests` (TEST module) — they must stay green after the `modtab_lookup` edit.

### Step 4: Test axis 2 — hardware effect (CompositeBusDevice)

In `GraphicsModuleTests.cs`, prove GCLS actually clears the gfx plane on the real VGC:
- Construct a `CompositeBusDevice` with `graphics.bin` injected as the extension ROM (find/extend the ext-ROM injection path — NovaLogo loads `novalogo_ext.bin`; replicate for an arbitrary byte[]; if no API exists, add a minimal test hook).
- Pre-fill a few gfx-plane bytes (via the VGC) to a nonzero value.
- Romswap to bank 1, set `LIB_FN_ID=GFN_GCLS`, set PC to the module `dispatch` entry with a sentinel return, run.
- Assert the gfx plane is cleared (read VGC gfx memory) and `LIB_STATUS=OK`.

If injecting an arbitrary ext ROM into `CompositeBusDevice` proves heavy, the minimal viable proof is: assert GCLS zeroes the VGC gfx region via whatever bus exposes the VGC + a $C000 code window. Resolve the cleanest mechanism during execution; the requirement is *an observed VGC effect from a module fn*.

### Step 5: Build, verify, commit

```bash
make -C modules/graphics            # graphics.bin = 16384 B, header OK
cd /Users/barry/Git/e6502 && dotnet test --filter "FullyQualifiedName~GraphicsModule|FullyQualifiedName~LibCall" 2>&1 | tail
```
Expected: new GRAPHICS tests + existing LibCall tests all green. Commit:
```bash
git add runtime/asm/libgraphics.inc modules/graphics/ runtime/asm/libcall.s e6502UnitTests/GraphicsModuleTests.cs
git commit -m "feat(gfx): GRAPHICS module skeleton + loader hook + GCLS proven end-to-end (4b.1)"
```

**Milestone gate:** the full build→stage→page→dispatch→VGC-effect chain works for a real GRAPHICS fn. STOP and reassess scope/sequencing before the bulk wrapper batches.

---

## Tasks 4b.2 – 4b.11: domain wrapper batches (LATER — after pipeline proof)

Each batch follows the SAME pattern: for each fn in the domain, add a `GFN_*` const (4b.0), a `gfx_jtable` entry, a thin wrapper (read mailbox args per the driver's `@in` convention → `JSR` the driver entry → write `LIB_RESULT`/`LIB_STATUS`), and a representative MSTest (loader-path for mailbox-result fns, hardware-effect for peripheral fns). Bump `GFX_FN_COUNT`. Build → size gate (stays < 16 KB) → tests green → commit per batch.

- **4b.2 draw** `$00–$0F` (vgc): GCLS✓ GCOLOR PLOT UNPLOT LINE CIRCLE(+ellipse) RECT FILL PAINT GTEXT
- **4b.3 text/mode** `$10–$1F` (vgc + vtext): COLOR FONT MODE REVERSE FLASH LOCATE CLS CLSWIN DISPLAY + vtext region/cursor/puts
- **4b.4 hw-sprite** `$20–$3F` (sprite): define/row/clear/copy/pos/enable/disable/flip/priority/shape/setreg/getx/gety + collision API
- **4b.5 copper** `$40–$4F` (copper + copper_split)
- **4b.6 blit/dma** `$50–$5F` (blitter + dma)
- **4b.7 vsprite** `$60–$7F` (vsprite + scene compositor)
- **4b.8 msprite** `$80–$9F` (msprite)
- **4b.9 image/mem** `$A0–$AF` (nvg + vgc mem_read/write + vpoke/vpeek + gsave/gload)
- **4b.10 turtle-render** `$B0–$BF` (the rendering ops 4c's resident turtle calls)
- **4b.11 anim+tween** `$C0–$CF` (anim) + `$D0–$DF` (tween)

Representative test coverage for the ~52 NDK-only ops (no language caller) — direct fn-id tests asserting the hardware-register effect; not all enumerated (design §7).

## Task 4b.final: boot-staging + HW smoke (after wrappers)

- Add the `libraries` entry to `e6502.ESP32/novahost/assets/config/boot.json` (and any mirror): `{ "name": "graphics", "id": 1, "path": "/lib/graphics.mod", "base": 393216, "size": 16384 }`.
- `nova put modules/graphics/graphics.bin /lib/graphics.mod` (HTTP file server).
- HW smoke: on the board, drive a `lib_call(GRAPHICS, GFN_PLOT…)` (BASIC `A=USR(0)` trigger as in the Phase-3 HW smoke) and confirm the pixel/effect on screen.

---

## Done criteria (4b overall)
1. `graphics.bin` builds to 16 KB with a valid "NL"/`$01` header; assembled size leaves headroom in the window (size gate per batch).
2. Every implemented fn green on its proving axis; existing LibCall + NovaLogo + Avalonia suites stay green.
3. `libgraphics.inc` is the single shared contract; runtimes will marshal against it in 4c.
4. Module boots from SD and a real fn produces an on-screen effect on hardware.

## This session
Execute **4b.0 + 4b.1** only. Reassess before 4b.2+.
