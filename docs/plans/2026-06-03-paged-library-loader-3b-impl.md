# Paged Library Loader (3b) — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build and prove a runtime-agnostic paged-library loader: a canonical low-RAM mailbox, a self-describing `$C000` module format, a resident `lib_call` (page-on-miss → validate → dispatch), boot staging from XRAM, and a working test module — verified across three layers (MSTest, Verilator, HW).

**Architecture:** A 23-byte mailbox at fixed low-RAM `$0300` carries `(module-id, fn-id, args, result, status)`; types are a compile-time `.inc` contract. `lib_call` compares the requested module to the resident one, and on a miss programs the `$BA76` page-in (XRAM→bank-1 `ext_rom`, CPU stalls atomically), validates the module header (`"NL"` magic / id / version) with bank 1 mapped from a RAM stub (`SEI`-atomic), then `JSR $C000` to dispatch via the module's jump table. `modtab` (compile-time constants in 3b) maps module-id → XRAM shelf base. Runtime integration into BASIC/Logo command handlers is **Phase 4**, out of scope here.

**Tech Stack:** ca65/ld65 (65C02 asm), C#/.NET MSTest (`e6502UnitTests`), Verilator SystemVerilog (`e6502.FPGA/test`), NovaHost ESP32 firmware, `openFPGALoader`.

**Design doc:** `docs/plans/2026-06-03-paged-library-loader-3b-design.md` (the locked contract).

**Workflow constraints:** work on `main`, no worktrees ([[feedback_no_worktrees_work_on_main]]); Verilator-first before any synth; `dotnet test` for the fast loop. Commit after each green task.

---

## Locked constants (the contract)

| Thing | Value | Source of truth |
|---|---|---|
| Mailbox base | `$0300` | carved this plan (Part B) |
| `MOD_ID / FN_ID / STATUS` | `$0300 / $0301 / $0302` | `libabi.inc` |
| `ARG0..3` | `$0303 / $0307 / $030B / $030F` (4 B each) | `libabi.inc` |
| `RESULT` | `$0313` (4 B) | `libabi.inc` |
| `HOME_BANK / RESIDENT` | `$0317 / $0318` | `libabi.inc` |
| module ZP scratch (`(zp),Y`) | `$14–$17` | cross-runtime free (audited) |
| Page-in MMIO | `PGD_CMD $BA76`, `STATUS $BA77`, `SRC $BA78-7A`, `WORDS $BA7B-7C` | shipped RTL |
| ROMSWAP | `$A03F`; EXT `$04`, BASIC `$02`, LOGO `$05` | `nova.inc` (NOT readable — track in RAM) |
| Module header @ `$C000` | `JMP dispatch` / `"NL"=$4E$4C` @`$C003` / id@`$C005` / ver@`$C006` / fncount@`$C007` / rsvd `$C008-09` | `libabi.inc` |
| `LIB_ABI_VERSION` | `$01` | `libabi.inc` |
| Module ids | NONE `$00`, GRAPHICS `$01`, SOUND `$02`, SYSTEM `$03`, TEST `$7F` | `libabi.inc` |
| Shelf | `SHELF_BASE=$060000`, slot `i` = `+i·$4000`, 4 slots | `libabi.inc` (audited free band) |
| Test module XRAM base | `$060000` (slot 0) | this plan |

---

## Part A — Shared ABI definitions

### Task A1: Create `runtime/asm/libabi.inc`

**Files:**
- Create: `runtime/asm/libabi.inc`
- Test: `e6502.FPGA/test` smoke-assemble (Step 3)

**Step 1: Write the include**

```asm
; libabi.inc — paged shared-library ABI (mailbox, module header, page-in MMIO).
; Included by every runtime foundation, every module, and the loader.
; Design: docs/plans/2026-06-03-paged-library-loader-3b-design.md

; --- Page-in (PGD) MMIO — shipped RTL ---
PGD_CMD          = $BA76
PGD_STATUS       = $BA77
PGD_SRCL         = $BA78
PGD_SRCM         = $BA79
PGD_SRCH         = $BA7A
PGD_WORDSL       = $BA7B
PGD_WORDSH       = $BA7C
PGD_START        = $01            ; write to PGD_CMD to begin
PGD_ST_BUSY      = $01            ; PGD_STATUS bit0
PGD_ST_DONE      = $02            ; PGD_STATUS bit1 (done/ok)

; --- Canonical mailbox (fixed low-RAM, carved $0300-$031F in every runtime) ---
LIB_MBOX         = $0300
LIB_MOD_ID       = LIB_MBOX+0     ; $0300  target module id
LIB_FN_ID        = LIB_MBOX+1     ; $0301  function id within module
LIB_STATUS       = LIB_MBOX+2     ; $0302  0=OK else error code
LIB_ARG0         = LIB_MBOX+3     ; $0303  32-bit LE cell
LIB_ARG1         = LIB_MBOX+7     ; $0307
LIB_ARG2         = LIB_MBOX+11    ; $030B
LIB_ARG3         = LIB_MBOX+15    ; $030F
LIB_RESULT       = LIB_MBOX+19    ; $0313  32-bit LE cell
LIB_HOME_BANK    = LIB_MBOX+23    ; $0317  runtime's REG_ROMSWAP home value (set at boot)
LIB_RESIDENT     = LIB_MBOX+24    ; $0318  currently-mapped module id ($00=none)
LIB_SCRATCH      = LIB_MBOX+25    ; $0319-$031F  7 bytes loader/module scratch
LIB_MBOX_END     = LIB_MBOX+32    ; $0320 (exclusive)

; --- Module-private ZP pointer for (zp),Y BYTES deref (cross-runtime free) ---
LIB_ZP           = $14            ; $14-$17 reserved for module pointer pairs

; --- Value type tags (compile-time, declared in fn signatures; not on the wire) ---
LT_INT           = $00            ; 32-bit signed (also 16.16 fixed)
LT_BYTES         = $01            ; ARG cell = ptr16,len16
LT_BOOL          = $02

; --- Status / error codes ---
LERR_OK          = $00
LERR_BAD_MAGIC   = $80
LERR_BAD_MODULE  = $81
LERR_BAD_VER     = $82
LERR_NO_FN       = $83

; --- Module header (image is ORG $C000) ---
MOD_ENTRY        = $C000          ; JMP dispatch (trampoline JSRs here)
MOD_MAGIC        = $C003          ; 2 bytes
MOD_MOD_ID       = $C005
MOD_VERSION      = $C006
MOD_FNCOUNT      = $C007
; reserved $C008-$C009
MOD_MAGIC0       = $4E            ; 'N'
MOD_MAGIC1       = $4C            ; 'L'
LIB_ABI_VERSION  = $01

; --- Module ids ---
MODULE_ID_NONE     = $00
MODULE_ID_GRAPHICS = $01
MODULE_ID_SOUND    = $02
MODULE_ID_SYSTEM   = $03
MODULE_ID_TEST     = $7F

; --- XRAM library shelf (option A: fixed, unmanaged high 256KB) ---
SHELF_BASE_L     = $00
SHELF_BASE_M     = $00
SHELF_BASE_H     = $06            ; $060000
SHELF_SLOT_WORDS = $2000          ; 8192 words = 16KB per slot
```

**Step 2: Smoke-assemble it**

Create `runtime/asm/_libabi_probe.s`:
```asm
      .include "libabi.inc"
      .word LIB_MOD_ID, MOD_MAGIC, SHELF_BASE_H
```
Run: `ca65 --cpu 65c02 -I runtime/asm runtime/asm/_libabi_probe.s -o /tmp/probe.o`
Expected: exits 0, no errors.

**Step 3: Delete the probe and commit**

```bash
rm runtime/asm/_libabi_probe.s /tmp/probe.o
git add runtime/asm/libabi.inc
git commit -m "feat(lib): shared paged-library ABI constants (libabi.inc)"
```

---

## Part B — Carve the canonical mailbox `$0300–$031F`

The audit proved no cross-runtime-free low-RAM block exists; we make one. Mailbox lives at `$0300`; both runtimes must keep `$0300–$031F` clear.

### Task B1: Bump NovaBASIC `Ram_base` to `$0320`

**Files:**
- Modify: `ehbasic/basic.asm` (the `Ram_base = $0300` definition — grep `Ram_base`)
- Test: existing ehbasic ROM suite + a new sentinel test

**Step 1: Write the failing test** in `e6502UnitTests/MailboxReservationTests.cs`:
```csharp
[TestMethod]
public void Basic_DoesNotClobber_MailboxRegion()
{
    // Boot BASIC, write a sentinel across $0300-$031F, run a small program that
    // creates variables/strings, assert the sentinel survives.
    var h = new BasicHarness();           // see existing BASIC emulator test harness
    h.Boot();
    for (ushort a = 0x0300; a < 0x0320; a++) h.Poke(a, 0x5A);
    h.RunLine("10 A=12345: B$=\"HELLO\": DIM C(20)");
    h.RunLine("RUN");
    for (ushort a = 0x0300; a < 0x0320; a++)
        Assert.AreEqual(0x5A, h.Peek(a), $"clobbered ${a:X4}");
}
```
(Use whatever BASIC emulator harness the repo already has; if none exposes Poke/Peek/RunLine, model it on `PageInChecksumStubTests.cs` driving `CompositeBusDevice`.)

**Step 2: Run — expect FAIL** (variables currently start at `$0300`).
Run: `dotnet test --filter "FullyQualifiedName~MailboxReservation"`

**Step 3: Change `Ram_base`**

In `ehbasic/basic.asm`, change `Ram_base = $0300` → `Ram_base = $0320`.

**Step 4: Rebuild ROM + run test — expect PASS**

Run: `make -C ehbasic && dotnet test --filter "FullyQualifiedName~MailboxReservation"`
Expected: PASS. Also run `dotnet test --filter "FullyQualifiedName~Ehbasic"` (or the BASIC ROM suite) — all green.

**Step 5: Commit**
```bash
git add ehbasic/basic.asm e6502UnitTests/MailboxReservationTests.cs
git commit -m "feat(lib): reserve mailbox $0300-$031F in BASIC (Ram_base->$0320)"
```

### Task B2: Reserve `$0300–$031F` in NovaLogo

**Files:**
- Modify: `novalogo/novalogo.s` (or `novalogo.cfg`) so BSS skips `$0300–$031F`
- Test: link map assertion + Logo suite

**Step 1:** Logo BSS starts at `$0280` (input_buf, 128 B → `$0280–$02FF`); heap starts `$0400`. So `$0300–$031F` is currently the front of post-input_buf BSS. Add an explicit reservation immediately after `input_buf` so the next BSS symbol starts at `$0320`:

In `novalogo.s` where BSS vars are declared (near `input_buf: .res 128`), insert:
```asm
lib_mbox_reserve: .res 32        ; $0300-$031F — canonical library mailbox (libabi.inc)
```
Confirm `input_buf` is the first BSS symbol so this lands at exactly `$0300` (check `novalogo.map`). If ordering differs, place the `.res 32` so the map shows `$0300–$031F` owned by `lib_mbox_reserve` and nothing else.

**Step 2: Build + verify the map**
Run: `make -C novalogo`
Then assert: `grep -E "lib_mbox_reserve|input_buf" novalogo/novalogo.map` shows `input_buf=$0280`, `lib_mbox_reserve=$0300`, and the following symbol ≥ `$0320`.

**Step 3:** Run the Logo suite: `dotnet test --filter "FullyQualifiedName~NovaLogo"` — all green (ROM still links under its size cap; RODATA ends before `$FFD7`).

**Step 4: Commit**
```bash
git add novalogo/novalogo.s
git commit -m "feat(lib): reserve mailbox $0300-$031F in NovaLogo BSS"
```

---

## Part C — Module format macro + test module

### Task C1: Create `runtime/asm/libmod.inc` (header macro)

**Files:** Create `runtime/asm/libmod.inc`

**Step 1:**
```asm
; libmod.inc — emit the standard module preamble at $C000.
; Invoke `lib_module_header <id>,<ver>,<fncount>` as the FIRST thing in the CODE
; segment of a module (cfg ORGs CODE at $C000). `dispatch` must be a label that
; follows (the module's fn-id dispatcher).
      .include "libabi.inc"

.macro lib_module_header mod_id, ver, fncount
      jmp     dispatch                 ; $C000  entry
      .byte   MOD_MAGIC0, MOD_MAGIC1   ; $C003  "NL"
      .byte   mod_id                   ; $C005
      .byte   ver                      ; $C006
      .byte   fncount                  ; $C007
      .byte   $00, $00                 ; $C008  reserved
.endmacro
```

**Step 2: Commit**
```bash
git add runtime/asm/libmod.inc
git commit -m "feat(lib): module header macro (libmod.inc)"
```

### Task C2: Write + build the test module `tests/asm/testmod.s`

**Files:**
- Create: `tests/asm/testmod.s`, `tests/asm/testmod.cfg`, `tests/asm/Makefile`
- Test: header-byte assertion (Step 4)

**Step 1: The module**
```asm
; testmod.s — minimal paged library for loader proofs. id=$7F, 2 functions.
      .include "libabi.inc"
      .include "libmod.inc"

      .segment "CODE"
      lib_module_header MODULE_ID_TEST, LIB_ABI_VERSION, 2

dispatch:
      lda     LIB_FN_ID
      cmp     #2
      bcs     bad_fn
      asl
      tax
      lda     jtable+1,x
      pha
      lda     jtable,x
      pha
      rts
bad_fn:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

jtable:
      .word   fn_echo-1               ; FN 0: ECHO
      .word   fn_add-1                ; FN 1: ADD

; FN 0 ECHO: RESULT = ARG0 (32-bit copy)
fn_echo:
      ldx     #3
@c:   lda     LIB_ARG0,x
      sta     LIB_RESULT,x
      dex
      bpl     @c
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; FN 1 ADD: RESULT = ARG0 + ARG1 (32-bit LE)
fn_add:
      clc
      ldx     #0
@a:   lda     LIB_ARG0,x
      adc     LIB_ARG1,x
      sta     LIB_RESULT,x
      inx
      cpx     #4
      bne     @a
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fill image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY
```

**Step 2: The cfg** (`tests/asm/testmod.cfg`, mirror `ehbasic/extension.cfg`):
```
MEMORY {
  ROM:    start = $C000, size = $3FFA, fill = yes, file = %O;
  VECROM: start = $FFFA, size = $0006, fill = yes, file = %O;
}
SEGMENTS {
  CODE:    load = ROM,    type = ro,  start = $C000;
  VECTORS: load = VECROM, type = ro,  start = $FFFA;
}
```

**Step 3: The Makefile** (`tests/asm/Makefile`):
```make
CA65=ca65 --cpu 65c02 -I ../../runtime/asm
LD65=ld65
all: testmod.bin
testmod.o: testmod.s ../../runtime/asm/libabi.inc ../../runtime/asm/libmod.inc
	$(CA65) testmod.s -o testmod.o
testmod.bin: testmod.o testmod.cfg
	$(LD65) -C testmod.cfg -o testmod.bin testmod.o
clean:
	rm -f testmod.o testmod.bin
```

**Step 4: Write the failing test** in `e6502UnitTests/ModuleFormatTests.cs`:
```csharp
[TestMethod]
public void TestModule_Header_IsWellFormed()
{
    byte[] img = File.ReadAllBytes(TestPaths.TestModBin); // 16384 bytes
    Assert.AreEqual(16384, img.Length);
    Assert.AreEqual(0x4C, img[0]);                 // JMP opcode at $C000
    Assert.AreEqual(0x4E, img[3]);                 // 'N'
    Assert.AreEqual(0x4C, img[4]);                 // 'L'
    Assert.AreEqual(0x7F, img[5]);                 // MODULE_ID_TEST
    Assert.AreEqual(0x01, img[6]);                 // LIB_ABI_VERSION
    Assert.AreEqual(0x02, img[7]);                 // fn count
}
```
(`TestPaths.TestModBin` → resolve to `tests/asm/testmod.bin`; add a build step or copy into test output.)

**Step 5: Build + run — expect PASS**
Run: `make -C tests/asm && dotnet test --filter "FullyQualifiedName~ModuleFormat"`
Expected: PASS.

**Step 6: Commit**
```bash
git add tests/asm/ e6502UnitTests/ModuleFormatTests.cs
git commit -m "feat(lib): test module (id=$7F, ECHO/ADD) + 16KB image build"
```

---

## Part D — `lib_call` loader + behavioral test bus (the fast TDD loop)

### Task D1: `LibLoaderBus` — behavioral model (ROMSWAP overlay + page-in)

**Files:** Create `e6502UnitTests/LibLoaderBus.cs`

**Step 1:** Model the three things the loader touches (mirror `PageInChecksumStubTests.cs`'s `RomSwapBus`, extended with a behavioral page-in). `$A03F` reads return RAM (HW-faithful — the loader never reads it); the **write** flips the overlay; `$BA76=1` copies XRAM→ext.
```csharp
using System;
using KDS.e6502;

internal sealed class LibLoaderBus : IBusDevice
{
    private readonly byte[] _ram  = new byte[0x10000];
    private readonly byte[] _ext  = new byte[0x4000];        // bank-1 ext_rom ($C000-$FFFF)
    private readonly byte[] _xram = new byte[512 * 1024];    // fake SDRAM shelf
    private byte _romswap = 0x02;                            // ROMSWAP_BASIC
    public int PageInCount { get; private set; }

    private const ushort REG_ROMSWAP = 0xA03F;
    private const byte    RS_EXT      = 0x04;
    private const ushort  PGD_CMD=0xBA76, PGD_STATUS=0xBA77,
                          PGD_SRCL=0xBA78, PGD_SRCM=0xBA79, PGD_SRCH=0xBA7A,
                          PGD_WORDSL=0xBA7B, PGD_WORDSH=0xBA7C;

    public byte Read(ushort a)
    {
        if (a == PGD_STATUS) return PGD_DONE;                 // always "done/ok" (sync model)
        if (a >= 0xC000 && _romswap == RS_EXT) return _ext[a - 0xC000];
        return _ram[a];                                      // incl. $A03F (not readable on HW)
    }
    private const byte PGD_DONE = 0x02;

    public void Write(ushort a, byte d)
    {
        if (a == REG_ROMSWAP) { _romswap = d; return; }      // write flips the overlay
        _ram[a] = d;                                         // SRC/WORDS land in RAM for readback
        if (a == PGD_CMD && (d & 1) != 0) DoPageIn();
    }

    private void DoPageIn()
    {
        int src = _ram[PGD_SRCL] | (_ram[PGD_SRCM] << 8) | (_ram[PGD_SRCH] << 16);
        int words = _ram[PGD_WORDSL] | (_ram[PGD_WORDSH] << 8);
        for (int i = 0; i < words; i++)                      // byte order = identity (design §3.3)
        {
            _ext[2*i]   = _xram[src + 2*i];
            _ext[2*i+1] = _xram[src + 2*i + 1];
        }
        PageInCount++;
    }

    public void LoadRam(ushort at, byte[] b)  => Array.Copy(b, 0, _ram,  at, b.Length);
    public void LoadXram(int at, byte[] b)    => Array.Copy(b, 0, _xram, at, b.Length);
    public byte PeekRam(ushort a)             => _ram[a];
    public void PokeRam(ushort a, byte v)     => _ram[a] = v;
    public byte CurrentBank                    => _romswap;
}
```

**Step 2: Commit**
```bash
git add e6502UnitTests/LibLoaderBus.cs
git commit -m "test(lib): behavioral bus — ROMSWAP overlay + $BA76 page-in"
```

### Task D2: `lib_call` + `modtab` (`runtime/asm/libcall.s`) — HIT path first

**Files:**
- Create: `runtime/asm/libcall.s`, `tests/asm/libcall_blob.cfg`, extend `tests/asm/Makefile`
- Test: `e6502UnitTests/LibCallTests.cs`

> **Placement note (3b):** `libcall.s` is assembled as a **relocatable RAM blob** for the proofs (the swap stubs must run from RAM anyway). Production placement (entry in foundation ROM, stubs copied to RAM at boot) is Phase 4. Build it ORG `$9C00` for tests (clear of mailbox `$0300`, clear of BASIC `Ram_base $0320`, below Logo heap/Emem).

**Step 1: Write the failing test** (`LibCallTests.cs`) — HIT path (module pre-resident in `_ext`, `LIB_RESIDENT` preset):
```csharp
[TestClass]
public class LibCallTests
{
    private static (LibLoaderBus bus, ushort entry) Setup()
    {
        var bus = new LibLoaderBus();
        bus.LoadRam(0x9C00, File.ReadAllBytes(TestPaths.LibCallBlob)); // lib_call @ $9C00
        bus.LoadXram(0x060000, File.ReadAllBytes(TestPaths.TestModBin)); // shelf slot 0
        bus.PokeRam(LibAbi.HOME_BANK, 0x02);  // home = BASIC
        bus.PokeRam(LibAbi.RESIDENT, 0x00);   // nothing resident
        return (bus, LibAbi.LibCallEntry);    // $9C00 (or label addr)
    }

    private static byte[] CallLib(LibLoaderBus bus, ushort entry,
                                  byte mod, byte fn, int arg0=0, int arg1=0)
    {
        bus.PokeRam(LibAbi.MOD_ID, mod);
        bus.PokeRam(LibAbi.FN_ID, fn);
        Write32(bus, LibAbi.ARG0, arg0);
        Write32(bus, LibAbi.ARG1, arg1);
        var cpu = new Cpu(bus, E6502Type.Cmos);
        // fake JSR lib_call: push (Sentinel-1), set PC=entry, run to Sentinel
        const ushort Sentinel = 0xFFF9;
        bus.PokeRam(0x01FF, (byte)((Sentinel-1) >> 8));
        bus.PokeRam(0x01FE, (byte)((Sentinel-1) & 0xFF));
        cpu.RestoreState(pc: entry, sp: 0xFD);
        for (int i = 0; i < 2_000_000 && cpu.Pc != Sentinel; i++) cpu.ExecuteNext();
        Assert.AreEqual(Sentinel, cpu.Pc, "lib_call did not return");
        return Read32(bus, LibAbi.RESULT);
    }

    [TestMethod]
    public void Echo_Hit_ReturnsArg0_NoPageIn()
    {
        var (bus, entry) = Setup();
        bus.PokeRam(LibAbi.RESIDENT, 0x7F);                 // pretend TEST already mapped
        bus.LoadExtFromXram(0x060000);                      // helper: copy slot0 -> _ext directly
        var r = CallLib(bus, entry, 0x7F, fn:0, arg0:0x12345678);
        CollectionAssert.AreEqual(BitConverter.GetBytes(0x12345678), r);
        Assert.AreEqual(0, bus.PageInCount);                // HIT = no page-in
        Assert.AreEqual(0x02, bus.CurrentBank);             // restored to home
        Assert.AreEqual(0x00, bus.PeekRam(LibAbi.STATUS));  // OK
    }
}
```
(Add `LibLoaderBus.LoadExtFromXram(int)` test helper; add `LibAbi` constants mirror + `Read32/Write32`. `TestPaths.LibCallBlob` → `tests/asm/libcall.bin`.)

**Step 2: Run — expect FAIL** (`tests/asm/libcall.bin` missing).
Run: `dotnet test --filter "FullyQualifiedName~LibCallTests.Echo_Hit"`

**Step 3: Write `libcall.s`**
```asm
; libcall.s — resident paged-library loader. ORG via cfg (RAM blob for tests).
; Caller fills LIB_MOD_ID/LIB_FN_ID/args, then JSR lib_call.
; LIB_HOME_BANK must hold the caller's REG_ROMSWAP value (set at boot).
      .include "libabi.inc"
      .include "nova.inc"                 ; REG_ROMSWAP, ROMSWAP_EXTENSION

      .segment "CODE"
lib_call:
      lda     LIB_MOD_ID
      cmp     LIB_RESIDENT
      beq     lc_invoke                   ; HIT → dispatch, no page-in
      jsr     modtab_lookup               ; X=id -> PGD_SRC/WORDS; C=1 unknown
      bcs     lc_bad_module
      lda     #PGD_START
      sta     PGD_CMD                     ; page-in; CPU STALLS until ext_rom loaded
      jsr     lc_validate                 ; C=1 on bad header (LIB_STATUS set)
      bcs     lc_done
      ; fall through to invoke
lc_invoke:
      php
      sei                                 ; vectors come from module while bank1 mapped
      lda     #ROMSWAP_EXTENSION
      sta     REG_ROMSWAP
      jsr     MOD_ENTRY                   ; JSR $C000 — module dispatches on LIB_FN_ID
      lda     LIB_HOME_BANK
      sta     REG_ROMSWAP
      plp
lc_done:
      rts
lc_bad_module:
      lda     #LERR_BAD_MODULE
      sta     LIB_STATUS
      rts

; lc_validate — bank1 mapped, check header; cache LIB_RESIDENT or set error.
; C=0 ok, C=1 fail (LIB_STATUS set, home bank restored).
lc_validate:
      php
      sei
      lda     #ROMSWAP_EXTENSION
      sta     REG_ROMSWAP
      lda     MOD_MAGIC
      cmp     #MOD_MAGIC0
      bne     lcv_magic
      lda     MOD_MAGIC+1
      cmp     #MOD_MAGIC1
      bne     lcv_magic
      lda     MOD_MOD_ID
      cmp     LIB_MOD_ID
      bne     lcv_mod
      lda     MOD_VERSION
      cmp     #LIB_ABI_VERSION
      bne     lcv_ver
      lda     LIB_MOD_ID
      sta     LIB_RESIDENT
      lda     LIB_HOME_BANK
      sta     REG_ROMSWAP
      plp
      clc
      rts
lcv_magic: lda #LERR_BAD_MAGIC
      .byte $2C                            ; BIT abs — skip next 2 LDA #imm
lcv_mod:   lda #LERR_BAD_MODULE
      .byte $2C
lcv_ver:   lda #LERR_BAD_VER
      sta     LIB_STATUS
      lda     LIB_HOME_BANK
      sta     REG_ROMSWAP
      plp
      sec
      rts

; modtab_lookup — A/X = module id in; program PGD_SRC*/PGD_WORDS*; C=1 if unknown.
; 3b: single TEST module at shelf slot 0. Phase 1/B replaces with directory lookup.
modtab_lookup:
      cmp     #MODULE_ID_TEST
      bne     mt_unknown
      lda     #SHELF_BASE_L : sta PGD_SRCL
      lda     #SHELF_BASE_M : sta PGD_SRCM
      lda     #SHELF_BASE_H : sta PGD_SRCH
      lda     #<SHELF_SLOT_WORDS : sta PGD_WORDSL
      lda     #>SHELF_SLOT_WORDS : sta PGD_WORDSH
      clc
      rts
mt_unknown:
      sec
      rts
```
Add `tests/asm/libcall_blob.cfg` (ORG `$9C00`, single CODE segment, plain binary) and a `Makefile` rule producing `libcall.bin`. Make `lib_call` the first byte so the entry = blob load addr `$9C00`.

**Step 4: Build + run — expect PASS**
Run: `make -C tests/asm && dotnet test --filter "FullyQualifiedName~LibCallTests.Echo_Hit"`

**Step 5: Commit**
```bash
git add runtime/asm/libcall.s tests/asm/libcall_blob.cfg tests/asm/Makefile e6502UnitTests/LibCallTests.cs e6502UnitTests/LibLoaderBus.cs
git commit -m "feat(lib): lib_call HIT-path dispatch + modtab + validate"
```

### Task D3: Page-on-miss (cold call triggers page-in, then dispatches)

**Step 1: Failing test** (`LibCallTests.cs`):
```csharp
[TestMethod]
public void Add_Miss_PagesIn_ThenDispatches()
{
    var (bus, entry) = Setup();                 // RESIDENT=0, _ext empty
    var r = CallLib(bus, entry, 0x7F, fn:1, arg0:1000, arg1:337);
    CollectionAssert.AreEqual(BitConverter.GetBytes(1337), r);
    Assert.AreEqual(1, bus.PageInCount);        // MISS = exactly one page-in
    Assert.AreEqual(0x7F, bus.PeekRam(LibAbi.RESIDENT)); // cached
    Assert.AreEqual(0x02, bus.CurrentBank);
    Assert.AreEqual(0x00, bus.PeekRam(LibAbi.STATUS));
}

[TestMethod]
public void SecondCall_SameModule_NoSecondPageIn()
{
    var (bus, entry) = Setup();
    CallLib(bus, entry, 0x7F, fn:0, arg0:5);
    CallLib(bus, entry, 0x7F, fn:1, arg0:2, arg1:3);
    Assert.AreEqual(1, bus.PageInCount);        // resident after first → no second copy
}
```
**Step 2: Run — expect PASS** (lib_call already handles miss). If RED, fix `lib_call`. Run: `dotnet test --filter "FullyQualifiedName~LibCallTests"`
**Step 3: Commit**
```bash
git commit -am "test(lib): page-on-miss + resident-cache coverage"
```

### Task D4: Error paths (bad magic, wrong id, bad version, bad fn)

**Step 1: Failing tests:**
```csharp
[TestMethod] public void BadMagic_SetsStatus_NoDispatch() {
    var (bus, entry) = Setup();
    var bad = (byte[])File.ReadAllBytes(TestPaths.TestModBin).Clone();
    bad[3] = 0x00;                                  // corrupt 'N'
    bus.LoadXram(0x060000, bad);
    CallLib(bus, entry, 0x7F, fn:0, arg0:9);
    Assert.AreEqual(0x80, bus.PeekRam(LibAbi.STATUS));  // LERR_BAD_MAGIC
    Assert.AreEqual(0x02, bus.CurrentBank);            // restored
}
[TestMethod] public void WrongModuleId_SetsStatus() {
    var (bus, entry) = Setup();
    var bad = (byte[])File.ReadAllBytes(TestPaths.TestModBin).Clone();
    bad[5] = 0x02;                                  // header says SOUND, we ask TEST
    bus.LoadXram(0x060000, bad);
    CallLib(bus, entry, 0x7F, fn:0, arg0:9);
    Assert.AreEqual(0x81, bus.PeekRam(LibAbi.STATUS));  // LERR_BAD_MODULE
}
[TestMethod] public void BadFnId_SetsStatus() {
    var (bus, entry) = Setup();
    CallLib(bus, entry, 0x7F, fn:9, arg0:0);        // only 0,1 exist
    Assert.AreEqual(0x83, bus.PeekRam(LibAbi.STATUS));  // LERR_NO_FN
}
[TestMethod] public void UnknownModule_SetsStatus() {
    var (bus, entry) = Setup();
    CallLib(bus, entry, 0x55, fn:0);                // not in modtab
    Assert.AreEqual(0x81, bus.PeekRam(LibAbi.STATUS));  // LERR_BAD_MODULE
    Assert.AreEqual(0, bus.PageInCount);
}
```
**Step 2: Run — expect PASS** (`fn_echo`/`bad_fn`/`lc_validate` already cover these). Fix asm if any RED.
**Step 3: Commit** `git commit -am "test(lib): loader error paths (magic/id/version/fn)"`

### Task D5: BYTES round-trip (proves `(zp),Y` deref + flat buffer)

Add a third fn to `testmod.s` — `FN_SUM` (id 2): `ARG0 = BYTES(ptr,len)` of bytes; `RESULT = INT` sum. Bump header fncount to 3, add `jtable` entry. Test loads a buffer into RAM, passes `ptr|len<<16` in ARG0, asserts the sum. This exercises the `LIB_ZP` (`$14`) pointer copy + `(LIB_ZP),Y` walk. Commit.

---

## Part E — Boot staging (modtab source-of-truth + NovaHost)

### Task E1: Document the shelf + modtab contract

`modtab_lookup` (Task D2) already encodes `TEST → $060000`. Add the future ids as commented modtab rows (GRAPHICS slot0… reassign TEST to slot 3 `$06C000` once real modules exist). Add a short `docs/help/guides/library-shelf.md` capturing `SHELF_BASE=$060000`, slot map, and "manifest and modtab MUST agree." Commit.

### Task E2: NovaHost stages the test module at boot

**Files:** `e6502.ESP32/novahost/` (firmware) — the boot ROM-load path; a manifest entry.

**Step 1:** Add a manifest section (own file `/lib/manifest.json` or a `boot.json` key) listing `{name:"testmod", id:0x7F, path:"/lib/testmod.mod", base:0x060000, size:16384}`.
**Step 2:** In the boot sequence, after the runtime ROM is streamed (reuse `pokeSdramBlock` / `write_sdram`, 256-B chunks — same path as ROM-load), read each manifest module from SD and stage it to its `base`. Apply [[feedback_firmware_graceful_degradation]]: if staging fails, log + continue resetRelease (don't wedge the CPU).
**Step 3:** Copy `tests/asm/testmod.bin` → SD `/lib/testmod.mod` (via `nova put` / the HTTP file server).
**Step 4:** No automated test here (firmware); verified by the HW smoke (Part G). Commit firmware + manifest.

---

## Part F — Verilator: real page-in + dispatch of the formatted module

### Task F1: Extend `test_page_in_top.sv` to page in `testmod` and dispatch

**Files:**
- Modify: `e6502.FPGA/test/test_page_in_top.sv`
- Add: a hex of `testmod.bin` (mirror the `rom/*.hex` byte-dumper in `e6502.FPGA/Makefile:51-58`)

**Step 1:** Replace `preload_unique_pattern` with a load of `testmod.bin` words into the SDRAM model at the shelf base (`chip.poke_word`, the existing idiom). Set the MMIO program's `$BA78-7A` to the shelf base, `$BA7B-7C` to `8192`.

**Step 2:** Extend the RAM-resident program (`ram_prog[]` at `$0200`) to perform a real `lib_call`-style dispatch *after* the page-in completes: set `$0300/$0301` (MOD_ID/FN_ID) + `$0303` (ARG0), `STA $A03F`=`$04`, `JSR $C000` (the module dispatches FN_ECHO), `STA $A03F`=`$02`, then stash `$0313` (RESULT) to `$0400` and halt-loop. (This mirrors the existing reader that stashes `$C000` bytes to `$0300`.)

**Step 3:** Assert via `dut.main_ram.mem[]` that `$0400..$0403 == ARG0` (the module ran on real silicon-modeled page-in) and `dut.ext_rom_inst.mem[]` matches `testmod.bin` byte-exact (reuse `verify_ext_rom`).

**Step 4: Run — expect PASS**
Run: `make -C e6502.FPGA/test test_page_in_top`
Expected: `PASS`. Also keep `test_page_dma`, `test_sdram_stream` green.

**Step 5: Commit**
```bash
git add e6502.FPGA/test/test_page_in_top.sv e6502.FPGA/test/rom/testmod.hex
git commit -m "test(fpga): page in formatted module + CPU dispatch into it"
```

---

## Part G — Hardware proof

### Task G1: Extend the HW smoke to stage `testmod` + drive `lib_call`

**Files:** Modify `tools/run-page-in-hardware-smoke.py` (or add `tools/run-libcall-hardware-smoke.py`)

**Step 1:** Stage `tests/asm/testmod.bin` to XRAM `$060000` via `write_sdram` (retarget off the old pattern base). Spot-check with `read_sdram`.
**Step 2:** POKE the `libcall.bin` blob to RAM `$9C00` and a tiny driver stub to `$9E00` that sets the mailbox (`$0300`=`$7F`, `$0301`=`0`, `$0303-06`=`$DEADBEEF`, `$0317`=`$02`, `$0318`=`$00`), `JSR $9C00` (lib_call), then halts. Use `pause()/resume()` around the multi-poke batch ([[feedback_debug_bridge_poke_needs_pause]]); trust only CPU-side reads ([[feedback_host_peek_not_cpu_read]]).
**Step 3:** Point the BASIC `USR` vector at `$9E00`, `USR(0)`, then read back `$0313-16` (RESULT) via a CPU-side path; assert `== $DEADBEEF` and `$0302`(STATUS)`==0`. Print PASS/FAIL; scrape `read_screen`.
**Step 4: Run on HW**
Run: `python3 tools/run-libcall-hardware-smoke.py --host 192.168.1.65`
Expected: `PASS` — module paged in from XRAM and dispatched on real silicon.
**Step 5: Commit.** This closes 3b: a formatted module, staged in XRAM, paged in and called via `lib_call`, proven on hardware.

---

## Definition of done (3b)

- [ ] `libabi.inc` / `libmod.inc` define the contract; both ROMs reserve `$0300–$031F`.
- [ ] Test module (`$7F`, ECHO/ADD/SUM) builds to a 16 KB image with a valid header.
- [ ] `lib_call`: HIT path, page-on-miss + resident cache, header validation, error paths, BYTES — all green in MSTest.
- [ ] Verilator: real page-in of the formatted module + CPU dispatch into it — green.
- [ ] HW smoke: staged → paged → dispatched, `RESULT` byte-exact — PASS on the board.
- [ ] No `dotnet test` / FPGA regressions.

## Out of scope (next phases)
- **Phase 4:** integrate `lib_call` into the NovaLogo foundation (entry in ROM, stubs copied to RAM at boot), build the real `GRAPHICS` module from `novalogo/extension.s`, add Logo's marshalling shim, stage at boot, call turtle/VGC ops through the loader.
- **Phase 5:** SOUND/SYSTEM modules; migrate BASIC's command handlers onto leaf-marshalling (retire the `EXT_GTBY/GTWRD/GTSW` callback bridges).
- **System B (Phase 1):** 512 KB directory allocator → `modtab` filled by directory lookup; shelf becomes a visible `XALLOC` reservation. `lib_call` unchanged.
- **6-slot shelf:** needs a decision to reclaim the `$054000` transient band.
