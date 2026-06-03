# Paged Runtime Libraries + Authoritative XRAM — Design

**Date:** 2026-06-02
**Status:** Foundational decisions LOCKED. Streaming-engine detail LOCKED (§3.3, brainstorm
2026-06-02). Remaining OPEN: module header bytes, boot staging, Regime-2 FIFO/write-burst,
callback ABI.
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

### 3.3 The page-in path + the general XRAM streaming engine (LOCKED)

Detailed via brainstorm 2026-06-02, grounded in a full read of `sdram.v`,
`xram_sdram.sv`, `dma.sv`, `blitter.sv`, `dpram.sv`, the real clock setup
(`fpga_top.sv`: `clk_sdram`=100 MHz, `clkref`=6.25 MHz, **16:1**), and prior-art recon.

**Prior-art recon — the implementation vehicle (LOCKED).** The entire retro-FPGA SDRAM
family — MiSTer (`SNES`/`Gameboy`/`TurboGrafx` `sdram.sv`), emard's ULX3S ports
(`ulx3s-misc/examples/sdram/*`), pnru — is **burst-length-1 with auto-precharge per
word**; they get throughput from *time-multiplexing ports*, not page-mode streaming. So
"adopt a MiSTer burst core" is a non-option — that core does not exist. LiteDRAM is
ruled out (ECP5 PHY is DDR3-only, Migen-heavy). The one genuine full-page donor is
**OpenCores `sdr_ctrl`** (proper ACTIVATE→READA→BURST_STOP→PRECHARGE→AUTO_REFRESH
sequencing, plain Verilog, no vendor primitives, ships an SDRAM model + TB).
**Decision: EXTEND our MiST `sdram.v` with a new page-mode read branch; borrow only
`sdr_ctrl`'s command-sequencing + row-boundary/refresh-interleave *pattern*; keep our
own proven 16:1 clkref phasing and ECP5 read-capture FF.** Every "swap the core" path
re-solves timing we already closed *and delivers zero burst gain*. (License: borrow the
pattern, not the file; our `sdram.v` is already GPLv3 so even direct borrowing is
compatible.)

**Why the engine is foundational, not premature (LOCKED).** The justification is **bulk
XRAM bandwidth**, not page-in latency. Arcade games — the product target — blit
tiles+sprites from XRAM every frame. At today's ~400 ns/byte XRAM rate, a 16.6 ms frame
moves only **~41 KB**; one 320×240 4bpp screen is **38.4 KB**. So today you can move
*roughly one screenful per frame and nothing else* — the real ceiling on the whole
arcade-game promise. The streaming engine lifts that frame budget from ~41 KB toward
megabytes. It is the bandwidth foundation for both games (Regime 2) and shared paged
libraries (Regime 1) — **not** premature optimization. A per-byte page-in stub (Tier 0
below) would be a throwaway, deleted the moment Regime 2 forces the engine.

**Two regimes — build order (LOCKED).** The 60× win splits by clock domain:
- **Regime 1 — in-domain (page-in: XRAM→`ext_rom` BRAM).** Both ends reachable in
  `sdram_clk`; **no CDC on the data path**. Simplest consumer. Built first as the *proof*.
- **Regime 2 — cross-domain (DMA→VGC/CPU, blitter STASH/FETCH, XMC).** Destinations live
  in the pixel-clock domain → need an async rate-matching FIFO. This is where the *game*
  bandwidth payoff lands — a **committed next phase** ("fast everywhere"), not a vague
  maybe. FIFO depth + the symmetric write-burst engine are designed against measured
  blitter/DMA workloads.

The **engine itself is built general** ("burst N words from addr") from day one so
Regime 2 plugs straight in; only the Regime-2 *consumer wiring* + FIFO are sequenced
after the page-in proof.

**The page-mode read FSM (LOCKED).** `run_addr` (sdram.v:213) fixes the decode every
access uses: `row=a[21:9]`, `col={a[24],a[8:1]}`, `bank=a[23:22]`. For our 512 KB region
(`a[18:0]`, so `a[24:22]=0`): bank 0, `col[8]=0` → columns 0–255 only, **row boundary
every 512 bytes**. So **16 KB = 32 row-opens, not 16** (the "512 words / 1 KB row" is the
*physical* row; this address map only exposes 512 B/row). The streaming read **must
replicate this exact decode** or it reads bytes the writer never stored. Timing is
unaffected — per-row overhead (~7 clk) ≪ 256 reads. A `streaming` flag forces
`run_cmd→INHIBIT` and overrides `sd_cmd/sd_addr/sd_ba/sd_dqm`; the free-running `q` mux
spins harmlessly. Sequence:

```
STREAM_PRECHARGE_ALL  PRECHARGE all (A10=1), tRP        ; guarantee clean entry
STREAM_ACTIVATE       ACTIVATE(bank,row), tRCD(2)
STREAM_READ           READ(col, A10=0), col++, 1/clk × (≤256, row-bounded)
STREAM_CAPTURE        (CAS-3 pipelined) emit dout + valid, 1 word/clk
STREAM_PRECHARGE      PRECHARGE(bank) at row end, tRP
STREAM_REFRESH        AUTO_REFRESH once per row boundary (unconditional), tRC
                      → next row, or STREAM_DONE when stream_words exhausted
```

Refresh: **one AUTO_REFRESH per row boundary, unconditional** — 32 over a 16 K burst =
one per ~2.6 µs (obligation 7.8 µs); over-refresh is always safe and far simpler than a
due-timer. Port B (debug) holds its request and is serviced after the burst — sub-ms of
blocking is harmless for a non-realtime bridge. Atomicity: the CPU is stalled (`rdy_out`)
for the whole copy, so no single-access request races a burst — page-in is atomic, no
queue.

**Stream port — general interface (`sdram_clk` domain):**
```
input         stream_req         output [15:0] stream_dout
input  [24:0] stream_addr        output        stream_valid   ; 1-clk strobe / word
input  [13:0] stream_words       output        stream_busy, stream_done   ; 14-bit: 16K page-in = 8192 words
```
This is the *read* primitive; the symmetric **write-burst** (STASH, faster boot) is the
same shape, built alongside Regime 2.

**The page-in consumer — D2, dedicated engine (LOCKED).** Reusing `dma.sv` +
`SPACE_EXTROM` would drag `dma.sv`'s *pixel-clock* domain into the path → a Regime-2 FIFO
in Phase 2, defeating the in-domain win. Instead a **small dedicated `page_dma` in
`sdram_clk`** consumes `stream_valid`/`stream_dout` and writes straight into `ext_rom`
BRAM; only the trigger + done handshake crosses to the CPU domain. **Byte order:**
single-access decode is `a[0]?data[7:0]:data[15:8]`, so write `ext_rom[2k]=dout[15:8]`,
`ext_rom[2k+1]=dout[7:0]` or the paged ROM byte-swaps. The `ext_rom` write port is muxed
boot-bridge (`dbg_rom_we`) vs `page_dma` — they never overlap.

**Throughput tiers — honest (`dpram` is single-clock 8-bit; `ext_rom` write currently
@ pixel clk, so the 8-bit *write* is the gate, not the SDRAM read):**

| Tier | What it takes | 16 K page-in | New SDRAM RTL |
|---|---|---|---|
| 0 | existing per-byte path + `SPACE_EXTROM` | ~4–6 ms | none — **rejected (throwaway)** |
| 1 | stream read + `ext_rom` write @ pixel clk | ~0.65 ms | engine + small FIFO — **page-in proof** |
| 2 | stream read + `ext_rom` write @ `sdram_clk` (dual-clock dpram) | ~0.16 ms | + dual-clock BRAM — later, if ever |
| 3 | stream read + 16-bit sdram write | ~0.084 ms | + aspect trick (risky) — not pursued |

**REVISED during implementation (2026-06-02) → Tier 2 + engine back-pressure.** The
original "Tier 1 = small FIFO" plan was a mis-call: the engine streams **1 word/clk
free-running** (no back-pressure within a row) while an 8-bit `ext_rom` write is **2
clk/word**, so even a single 16 K burst backlogs ~7 K words (~14 KB) into a "small" FIFO
— Tier 1 actually needs *chunked* streaming, which is *more* complex than the dual-clock
alternative. **Chosen path:** Tier 2 — a **dual-clock `ext_rom`** (write port @
`sdram_clk`, CPU read port @ pixel clk; ECP5 DP16KD natively supports independent port
clocks — the BRAM risk flagged earlier was the *16-bit aspect* of Tier 3, not dual-clock)
+ **stream back-pressure**: a new `stream_ready` input on `sdram.v`'s page-mode FSM holds
READ issuance (NOP, row stays open) when low, so `page_dma` *self-paces* the
free-running stream to its 2-byte-per-word write. Result: **no FIFO, no chunking, no
buffer** — `page_dma` writes `ext_rom` directly in `sdram_clk`, only the trigger/done
handshake (and the slow boot-bridge ROM-load writes) cross clock domains. ~0.16 ms.
Page-in is rare so the speed delta is moot; this is the *lower-risk, simpler* design, and
`stream_ready` is reusable by Regime 2. (The doc's earlier "~84 µs" was the read-stream
throughput, since confirmed at **8676 cyc = 86.76 µs @ 100 MHz**; Tier-3 16-bit write is
not pursued.)

**Discipline — Verilator-first, before any 17-min synth** ([rtl-discipline]: 99.99% sure
first):
1. Drive the new branch against a **proper MT48LC16M16 behavioral model honoring
   tRCD/CAS/tRP/tRC** (from `sdr_ctrl`/MiSTer/emard TBs) — not a hand-waved model, or the
   cycle counts are fiction. Verilator can't see synth cell mapping ([verilator-blind])
   but proves sequencing, refresh, data correctness, cycle count.
2. Test cases: (a) single-access write a pattern → stream-read back → **byte-exact +
   byte order**; (b) multi-row burst (2 KB = 4 rows) → assert PRECHARGE/ACTIVATE/REFRESH
   at each boundary; (c) 16 KB stream → **measured cycle count** replaces the 84 µs guess
   (Verilator-measured: **8676 `sdram_clk` cycles = 86.76 µs @ 100 MHz** — the read-stream
   throughput; the engine reads at the predicted rate, confirming §3.3's ~84 µs estimate);
   (d) AUTO_REFRESH ≥ once / 7.8 µs across the burst; (e) port-B request mid-stream
   defers, then completes.
3. Bench passes *and* cycle count confirmed → only then synth. Respect the existing
   `clkref` ratio / refresh history ([sdram-clkref-16-1], [dont-ship-timing-failing-bitstream]).

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
2. **Streaming SDRAM engine + page-in proof (the long-pole):** page-mode read branch
   in `sdram.v` (EXTEND, borrow `sdr_ctrl` sequencing pattern — §3.3); general
   `stream_*` port; dedicated `sdram_clk` `page_dma` consumer writing `ext_rom`
   (**Tier 1**, ~0.65 ms); `ext_rom` write mux (boot bridge vs `page_dma`).
   **Gate: Verilator bench + measured cycle count before any synth.** Then HW
   measurement of the real page cost.
3. **Loader/dispatch stub + module format:** the `$C000` jump table, the
   command→module map (§5), resident-module tracking, page-on-miss.
4. **First module + proof:** move NovaLogo's existing extension graphics into a
   paged `GRAPHICS` module; stage it in XRAM at boot; call it from Logo through the
   loader. End-to-end validation of the whole pipeline on the smallest slice.
5. **Generalize:** SOUND/SYSTEM modules; wire BASIC to the same modules; per-language
   surfaces.
6. **Regime 2 — bulk XRAM bandwidth ("fast everywhere"):** async rate-matching FIFO +
   the symmetric **write-burst** engine; wire blitter (tile/sprite rows, STASH/FETCH),
   DMA, and the XMC copy path to the streaming engine; FIFO depth designed against
   *measured* per-frame workloads. The arcade-game bandwidth payoff. Depends only on
   the Phase-2 engine (independent of 3–5).

---

## 7. Open sections (to detail next)

- **General XRAM streaming primitive (the long-pole):** approach, FSM, stream-port
  interface, page-in consumer (D2), throughput tiers, and the Verilator gate are now
  **LOCKED in §3.3** (extend `sdram.v` + borrow `sdr_ctrl` pattern; engine built
  general, page-in is the Tier-1 in-domain proof). *Still open:* (a) the **Regime-2
  async rate-matching FIFO** depth + the symmetric **write-burst** engine — both
  designed against *measured* blitter/DMA per-frame workloads (Phase 6); (b) the
  **Tier-2 dual-clock `dpram`** for `ext_rom` (only if page-in latency ever matters —
  it shouldn't). RTL not yet written; Verilator bench is the first artifact.
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
