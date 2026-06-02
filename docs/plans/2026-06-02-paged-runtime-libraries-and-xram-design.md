# Paged Runtime Libraries + Authoritative XRAM — Design

**Date:** 2026-06-02
**Status:** Foundational decisions LOCKED (via brainstorm). Detail sections OPEN.
**Scope:** Cross-runtime architecture for BASIC / Logo / Pascal / Forth (and future
language runtimes). Does not change machine-owning runtimes (NovaZ) except to make
their XRAM use *visible*.

---

## 1. Problem

A language runtime executes from a single **16K window** at `$C000–$FFFF`. `$A03F`
(`REG_ROMSWAP`) flips which BRAM bank maps there. On hardware (`top.sv`) there are
only **two physical 16K BRAMs**: `basic_rom` (idx 0) and `ext_rom` (idx 1).
`ROMSWAP_LOGO/BASIC/NCC` all map bank 0; the `ext_rom_active` bit maps bank 1.
NovaHost streams the selected runtime into bank 0 at boot.

Consequence: a runtime's **language core is hard-capped at 16K**, and the legacy
extension model is a *one-way leaf callout* (pre-evaluated args, no callback) that
can only hold self-contained hardware ops. NovaLogo hit this wall (41 bytes free
after the 2026-06 lexer/EDIT work + a 250-byte reclaim).

"32K total" is a mirage: only one 16K bank executes at a time, and the second is a
leaf-only callout. The real budget for a *language core* is 16K.

### The vision (user, verbatim)

> "i dont want to even be limited to 32k.. we need just enough in the 16 k for
> basic, logo, pascal, forth, etc.. to handle the foundations and then swap shit
> in and out as needed. we can either use xram or the sd card."

> "keep the base portions of the languages in memory.. and page the special
> features in and out using libraries that can be shared across languages? that
> way we build those hardware libs once and use them from basic, logo, pascal,
> forth, etc."

> "all language runtimes need to be able to see what is using room in xram… they
> should be able to say, 'store these x bytes somewhere' without stomping on
> something."

> "every consumer of xram memory _needs_ to allocate it and that needs to show up
> when we ask for what's allocated and what's free. even novaz. this will help
> every runtime to not have to care about where shit is stored.. it can just say,
> 'store this giant bag of shit somewhere' and if there's space, it happens.. if
> not, it gets an allocation failure.. just like a real language/runtime."

---

## 2. Architecture overview

Two intertwined systems:

- **A. Paged shared-library overlays** — each runtime keeps only its *foundation*
  resident; hardware features become shared **library modules** paged into the
  second bank on demand.
- **B. Authoritative XRAM directory** — one allocator/directory is the single
  source of truth for all 512KB, so the library shelf and developer data coexist
  without collision, and everything is *visible*.

```
 RESIDENT (always mapped)                      PAGED / SHELF
 ┌─────────────────────────────┐    ┌──────────────────────────────┐
 │ $0000-$01FF ZP + stack      │    │  XRAM 512KB (authoritative     │
 │   (EXT mailbox in ZP)       │    │  directory): library modules   │
 │ $0270 RAM trampoline        │    │  + dev allocations + reserved  │
 │ $0280-$9FFF program + heap  │    │  regions. The "shelf."         │
 │   (runtime data, shared)    │    └──────────────────────────────┘
 │ $C000-$FFFF bank 0:         │              │ page-DMA on miss (new)
 │   LANGUAGE FOUNDATION       │              ▼
 │   (core + loader/dispatch)  │    ┌──────────────────────────────┐
 └─────────────────────────────┘    │ $C000-$FFFF bank 1: the ONE   │
                                     │ active library module (16K)   │
                                     └──────────────────────────────┘
```

---

## 3. System A — Paged shared-library overlays

### 3.1 Locked decisions

- **Bank 0 = per-runtime foundation:** interpreter core + a small resident
  loader/dispatch stub. ≤16K.
- **Bank 1 = single overlay window:** exactly one module resident at a time. Every
  module is built to **ORG `$C000`** and begins with a **jump table**.
- **Coarse modules**, grouped by usage locality so module *switches* (the only thing
  that pays a page-in) are rare: e.g. `GRAPHICS` (VGC + sprites + copper + blitter),
  `SOUND` (SID + WTS), `SYSTEM` (file + net). One resident at a time.
- **XRAM is the shelf:** at boot, NovaHost streams every module from SD into XRAM
  (same machinery as today's ROM-load). Runtime paging is **XRAM → bank-1 BRAM**
  via a new on-FPGA **page-DMA**, **page-on-miss** (same module already resident →
  no page; only a *switch* pays the copy).
- **SD and the ESP32 are never in the runtime page path.** They only touch the
  shelf at boot.
- **Leaf-call ABI** = the existing `EXT_CMD` ZP mailbox + RAM trampoline at `$0270`.
  Libraries take pre-evaluated values and drive silicon; they do **not** call back
  into the runtime's data structures. (Re-entrant callback is a deferred phase; the
  ABI reserves room for it.)
- **Modules are runtime-agnostic** — built once, called identically from every
  language. The shared ABI is the contract.

### 3.2 Feasibility (verified in source)

- `top.sv:458`: *"Extension ROM: 16KB. Bitstream initialized, runtime-overwritable."*
  The `ext_rom` BRAM already has a write port (`dbg_rom_we`, idx 1). Today it's
  driven by NovaHost over the bridge — fine for boot, too slow to page at runtime.
- The bridge is **SPI at 40 MHz** (`FPGA_SPI_HZ`), not the 115200 debug UART (which
  carries control/events only). SD card SPI is currently throttled to **4 MHz**
  (`SD.begin(..., 4000000)`).
- **SD → XRAM, 16K ≈ 32 ms** today (SD-read bound). Bumping SD SPI 4→20 MHz → ≈ 8 ms.
  This is a **boot** cost (stage the shelf once), not a per-page cost.
- **Runtime cold page = XRAM → bank-1**, entirely on-FPGA. See §3.3.

### 3.3 The page-in path (LOCKED: page-mode streaming engine)

**Destination wiring (small):** paging is a new DMA *destination space*, not a new
engine. `dma.sv` (`$BA63`: `CMD`/`STATUS`/`SRC*`/`DST*`/`LEN*`) already copies
XRAM→{CPU,VGC} with busy/done and a CPU stall (`rdy_out`); it just lacks an
`ext_rom` destination. Add `SPACE_EXTROM` and mux the DMA write into the `ext_rom`
port that boot's `dbg_rom_we`/`erom_we` already drives. "Page module M" = an
ordinary DMA command `SRC=XRAM@base, DST=EXTROM, LEN=16384`.

**Why the read path needs work:** `sdram.v` (MiST, MT48LC16M16, clk ≤128 MHz) is
hardwired `BURST_LENGTH=3'b000` (single word), `NO_WRITE_BURST=1`, and
time-multiplexes two *random-access* ports by `clkref` — full ACTIVATE→READ→precharge
per word, wrapped by a 3-stage CDC each way (`xram_sdram.sv`). Byte-at-a-time that is
**~4–6 ms / 16K** — *not* acceptable.

**Decision: build a page-mode streaming read engine (~80 µs).** Open a row once
(512 words = 1KB on this part), stream its columns at ~1 word/clk (CAS-pipelined),
write straight into `ext_rom` BRAM (1 word → 2 byte-writes). 16K = 16 rows ≈
`16 × (512 + ~7)` ≈ 8,300 `sdram_clk` ≈ **65–85 µs @ 100–128 MHz** (~60×). The engine
lives in the `sdram_clk` domain (no per-byte CDC) and **owns the SDRAM for the page**
— legitimate because the CPU is already stalled (`rdy_out`) for the whole copy, which
also makes a page-in **atomic** (resolves the "call during in-flight page" race —
no queue needed). Refresh is honored between rows.

**Scope: this is the GENERAL XRAM bulk path, not a one-off page engine.** The
streaming read (+ write-burst) lives once in `xram_sdram.sv` + `sdram.v` as a
*"burst N words from addr"* request mode, and every run-mover reaches it through the
existing port-A / `bm_xram_*` arbitration. Beneficiaries (all move contiguous runs):

| Consumer | XRAM use | Benefit |
|---|---|---|
| DMA (`dma.sv` `$BA63`) | bulk XRAM↔{CPU,VGC} | read+write runs |
| Blitter (`blitter.sv` `$BA83`) | 2D rect *rows* (tiles/sprites, STASH/FETCH) | prime case — each row is a run |
| XMC bulk (`xram_copy_*`/`fill`, `STASH`/`FETCH`) | CPU↔XRAM runs | read+write |
| Library page-in (new) | XRAM→`ext_rom` 16K | client #1 |
| NovaHost boot (port B `pokeSdramStream`) | SD→XRAM block writes | write-burst speeds boot |

Untouched: CPU single-byte `xram_read8/write8` (no run to amortize — already fine);
video/pixel fetch (doesn't use XRAM — timing-critical lives in BRAM).

Consumer-side cost: each run-mover's FSM changes from *per-byte fire/wait* to
*issue-burst / consume-stream*. They already carry the run length (`DMA LEN`, blitter
`WIDTH`/stride, XMC copy len), so it's a natural fit — but it does mean touching
`dma.sv`, `blitter.sv`, and the XMC copy path, plus the SDRAM core.

**Streaming FSM design (research confirmed — `sdram.v` read in full):** the existing
controller runs an 8-state `q` cycle resynced to `clkref`: `ACTIVE`@q1 →
`READ`/`WRITE`@q3 (+tRCD=2) → capture@q6 (+CAS=3), **single access with auto-precharge
(A10=1)**, two ports time-shared by clkref phase (16:1). Streaming adds a **separate
mode** that, on a burst request (port A), *suspends* the port-mux q-cycle and runs:
`ACTIVATE row` → **back-to-back `READ`s to consecutive columns with A10=0** (no
precharge), capturing **1 word/clk** after the CAS-3 fill → `PRECHARGE` at row end →
`AUTO_REFRESH` when the 7.8 µs timer is due (between rows; a row is 5.2 µs < 7.8 µs so
at most one refresh per 1–2 rows). Confirmed cost: **~83 µs / 16K @ 100 MHz.** Single-
word random access (CPU bytes) keeps the existing path untouched; the wrapper picks
streaming only when a run is long enough to amortize the row-open.

Request interface (wrapper): burst addr + length + a per-word `valid` strobe stream in
the `sdram_clk` domain (no per-byte CDC). Latent `sdram.v` primitives already present:
`BURST_LENGTH`/`ACCESS_TYPE` params, `CMD_BURST_TERMINATE`, `CMD_PRECHARGE`,
`CMD_AUTO_REFRESH` — page-mode is an FSM addition, not new command decode.

**Discipline (the SDRAM core is the timing-sensitive block):**
1. **Prior-art first** ([prefer-prior-art]): evaluate a burst/streaming SDRAM
   controller (emard ULX3S refs, MiSTer) vs. extending `sdram.v`'s port A with a
   page-mode path, before hand-rolling an FSM.
2. **Verilator-first** ([verilator-first-for-fpga]): cycle-accurate bench proving the
   stream + refresh + the ~80 µs figure before any 17-min synth. Harness already
   exists (`test_xram_sdram.sv`, `test_sdram_loopback.sv`).
3. Respect the existing `clkref` ratio / refresh timing history ([sdram-clkref-16-1],
   [dont-ship-timing-failing-bitstream]).

---

## 4. System B — Authoritative XRAM directory

### 4.1 What already exists (`runtime/asm/xmc.s`, tested)

A real **named-block page allocator**, more complete than remembered:

- **Managed pool:** low 256KB (`TOTAL_PAGES=1024`, 256-byte pages), 1024-bit bitmap.
- **Persistent metadata in SDRAM:** control block at `$07FA00`, magic sentinel
  `$A5`. `xmc_init_check` initializes once and only re-inits if the magic is gone —
  so allocations + directory **survive runtime swaps** while SDRAM holds. This is a
  **persistent shared RAM-disk**: `STASH "FOO"` in BASIC, swap to Logo, `XDIR` still
  sees it.
- **Directory:** 32 entries × 32 bytes (`$07FB00–$07FEFF`): name[24], handle,
  24-bit addr, 16-bit len, 16-bit pages. Live used/free at `$BA0E/$BA0F`.
- Exported as `xmc_alloc_block`, commented *"available to other runtimes."*
- Tests: `Xmc_AllocCommand_ReturnsHandleAndMarksPages`,
  `Xmc_ReleaseCommand_UnmarksUsedPages`, `Xmc_UsageStats_TrackWrittenPages`,
  `Xmc_NamedStashFetch_RoundTrips`, `Xmc_NamedDir_EnumeratesBlocks`,
  `XramXfreeReusesReleasedAllocationPages`.

### 4.2 Locked decision: malloc for the whole machine — every consumer allocates

Today XRAM is **two worlds** — a tracked 256KB pool + an *invisible* 256KB high band
of fixed regions (NovaZ story `$040000`, NVG stage `$070000`, EDITUI `$054000`).
Invisible regions violate the requirement that runtimes can *see* all XRAM use.

**Decision:** **every consumer of XRAM allocates it — no exceptions, including
NovaZ.** Every byte in use is a **directory entry**, so "what's allocated / what's
free" is always the complete truth. The default developer model is
**placement-agnostic**, exactly like a real runtime's allocator:

```
handle/addr = XALLOC(len)     ; "store this giant bag of shit somewhere"
                              ;   → succeeds with an address, OR allocation failure
              XREL(handle)    ; give it back
```

A consumer that doesn't care where (the common case) never names an address — it
gets one back. **The only intrinsically-fixed thing is the allocator's own metadata
band** (`$07FA00+`) — it can't allocate its own bootstrap — and even that is a
**visible reserved entry** counted in the stats. There are no other holes.

### 4.3 New primitives this requires

1. **Grow the managed map to 512KB** — `TOTAL_PAGES 1024→2048`, bitmap `128→256`
   bytes; represent the metadata band (`$07FA00+`) as its own reserved entry so it
   is counted, not hidden. (`XALLOC` first-fit already exists and is the primary
   API.)
2. **Reserve-at-address alloc** — an *escape hatch* (not the default) to claim a
   *specific* page range as a named entry, for migration of code that still has a
   hardcoded XRAM base, or genuine hardware-fixed needs. Still a visible entry.
3. **Runtime-neutral surface** — the service is already exported; Logo/Pascal/Forth
   each get a thin verb over `xmc_alloc_block` + directory enumeration (BASIC's are
   `XALLOC`/`XREL`/`XHANDLE`/`STASH`/`XDIR`). Allocation-failure is a first-class,
   catchable result in each language.

### 4.4 Migration

- **NovaZ and every fixed-region user move onto the allocator.** Ideally they
  `XALLOC` and use the returned address (fully dynamic). Where assembly still
  hardcodes a base, the reserve-at-address escape hatch claims that exact range as a
  named entry during migration — same bytes, now *visible* and *counted* — and the
  hardcode is retired opportunistically.
- The **library shelf** is a normal `XALLOC` by the loader at boot; the loader
  records each module's base from the returned handle (directory lookup by name), so
  nothing downstream needs a hardcoded shelf address.

---

## 5. Shared library ABI (LOCKED — reuse + extend)

The existing `EXT_CMD` mailbox is already a runtime-agnostic library call interface
(`ext_iface.inc`): a flat ZP block `$80–$91` — `EXT_CMD` (1-byte command id),
`EXT_ARGC`, three typed args (TYPE/HI/LO/FRAC each), a typed RESULT — plus the
14-byte RAM trampoline at `$0270` that swaps bank 1 in, `JSR $C000`, swaps back.
Today **one** extension owns the whole flat id space (`$01–$4B`), dispatched by a
`cmd*2` jump table at `$C000`.

**Decision: keep the flat command id; add a resident command→module map.** No
explicit (module-id, fn-id). Every existing `EXT_CMD_*` value is unchanged and no
call site moves.

- **`map[EXT_CMD] → module-id`**: a small resident table in the foundation says
  which coarse module owns each id range.
- **`lib_call`** replaces the bare trampoline call:
  ```
  lib_call:                          ; EXT_CMD + args already set
    module = map[EXT_CMD]
    if resident_module != module:
        base = xdir_lookup(module)   ; XRAM shelf addr from the directory (by name)
        page_dma(base -> bank1)
        resident_module = module
    JSR EXT_TRAMPOLINE               ; swap bank1, JSR $C000, swap back
  ```
  Same module as the previous call → no page-in → identical cost to today.
- **Module binary layout** (ORG `$C000`): header (magic, module-id, version,
  fn-count) + jump table + code. After a page-in the loader validates the header
  (right module / compatible version) before the first call. Each module's `$C000`
  dispatcher is today's `cmd*2` table over its own id range.

Backward-compatible; the only new runtime cost is a map lookup + page-on-miss.
A reserved `EXT_MODULE` byte is **not** added now (flat space + coarse modules ≪ 256
commands); revisit only if a 2D space is ever needed.

---

## 6. Phased rollout (proposed — OPEN)

1. **XRAM authority foundation:** add reserve-at-address alloc; grow pool to 512KB;
   make metadata a reserved entry; migrate fixed regions to reserved entries; expose
   a runtime-neutral allocator API. (Pure XRAM/firmware/asm; no new RTL.)
2. **Page-DMA silicon:** XRAM → bank-1 (`ext_rom`) DMA with trigger/busy/done
   registers; Verilator + hardware measurement of the real cold-page cost.
3. **Loader/dispatch stub + module format:** the `$C000` jump table, the
   (module-id, fn-id) mailbox dispatch, resident-module tracking, page-on-miss.
4. **First module + proof:** move NovaLogo's existing extension graphics into a
   paged `GRAPHICS` module; stage it in XRAM at boot; call it from Logo through the
   loader. End-to-end validation of the whole pipeline on the smallest slice.
5. **Generalize:** SOUND/SYSTEM modules; wire BASIC to the same modules; per-language
   surfaces.

---

## 7. Open sections (to detail next)

- **General XRAM streaming primitive (the long-pole):** the burst read + write-burst
  in `sdram.v`/`xram_sdram.sv`, refresh interleave, and the consumer-side FSM updates
  to *issue-burst / consume-stream* — `dma.sv`, `blitter.sv`, the XMC `xram_copy_*`
  path — plus `ext_rom` write mux + `SPACE_EXTROM` for the page-in. Prior-art recon
  (emard/MiSTer vs. extend `sdram.v` port A); Verilator bench (harness exists)
  confirming the ~80 µs and per-consumer throughput. (Scope/approach locked §3.3; RTL
  design open.)
- **Module header bytes:** exact magic/version/fn-count field widths and the
  jump-table entry format (dispatch model locked in §5; only byte layout remains).
- **Boot staging:** module manifest on SD, sizes, the XRAM shelf layout, and how the
  loader learns each module's XRAM base (directory lookup by name).
- **Deferred:** re-entrant callback ABI (paged code calling back into the
  foundation) for non-leaf libraries.

*Resolved:* the loader's "call during an in-flight page" race — the DMA holds the
CPU stalled for the whole copy, so a page-in is atomic (§3.3). No queue.

---

## 8. Risks / notes

- The streaming engine touches `sdram.v` (timing-sensitive, careful clkref/refresh
  history). It is the highest-risk block: prior-art first, Verilator-first, measure
  the ~80 µs before synth (§3.3). Until measured, ~80 µs is an estimate.
- Coarse-module granularity assumes usage locality; if real programs interleave
  subsystems tightly, revisit sub-slots in bank 1 (deferred; measure first).
- Directory is 32 entries / 24-char names — generous now, expandable (more directory
  pages) later.
- Single-tasking assumption underpins the whole model; do not break it.
- Keep the allocator service identity *runtime-neutral* (not "BASIC's XMC"); it is
  the foundation of the multi-runtime vision.
