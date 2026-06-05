# Dynamic Module Shelf — Phase A Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace the static, hand-mapped `modtab_lookup` (module id → fixed XRAM slot) with a runtime RAM directory so a module's slot is assigned at load time, killing the TEST/GRAPHICS slot-0 collision and removing `base` from `boot.json` and the web staging surface.

**Architecture:** The XRAM shelf becomes an N=4 cache. An 8-byte directory in the resident loader band (`shelf_tag[4]` + `shelf_lru[4]`) records which module id is in which slot; `base` is computed from slot index (`SHELF_BASE + i*$4000`). The 6502 loader scans the directory on each `lib_call`; the firmware (HW) and the test harness (emulator) seed it. **Phase A is zero-RTL** — boot-time allocation only; the miss/eviction handshake is Phase B (separate plan). See `docs/plans/2026-06-05-dynamic-module-shelf-design.md`.

**Tech Stack:** ca65/ld65 6502 assembly (`runtime/asm`), C#/.NET emulator + MSTest (`e6502.Avalonia`, `e6502UnitTests`), C#/.NET web+CLI (`e6502.Nova`, `e6502.Storage`), Arduino C++ firmware (`e6502.ESP32/novahost`).

**Key addresses (lock these):**
- `SHELF_N = 4`; slots `$060000 $064000 $068000 $06C000` (base = `$060000 + slot*$4000`).
- `SHELF_TAG = $0418` (4 bytes, slot→id, `$00`=empty); `SHELF_LRU = $041C` (4 bytes, slot indices MRU→LRU). Both in the loader band `$0320–$041F`, packed against the top edge → loader may grow to **≤ 248 bytes** (`$0320–$0417`) before colliding.
- Identity = `(MOD_MOD_ID $C005, MOD_VERSION $C006)`; `NovaModule.Id`/`NovaModule.AbiVersion`.

**Pre-flight:** `export PATH=/Users/barry/Git/cc65/bin:$PATH` (ca65/ld65 on PATH).

---

### Task 1: Shelf directory symbols in `libabi.inc`

**Files:**
- Modify: `runtime/asm/libabi.inc` (after the shelf block, ~line 89)

**Step 1: Add the symbols.** After the `SHELF_SLOT_WORDS` line, insert:

```asm
SHELF_N          = 4              ; physical shelf slots (cache lines)
; Dynamic shelf directory (RAM, in the resident loader band $0320-$041F, packed
; against the top edge so the loader can grow to $0417). slot i base is computed:
; SHELF_BASE + i*$4000. shelf_tag[i] = resident module id ($00 = empty).
; shelf_lru[i] = slot indices, MRU(front) -> LRU(back). Seeded by the host
; (firmware) at boot / by the test harness in the emulator.
SHELF_TAG        = $0418          ; SHELF_N bytes: $0418-$041B
SHELF_LRU        = $041C          ; SHELF_N bytes: $041C-$041F
```

**Step 2: Verify it still assembles** (no behavior yet — Task 3 consumes these):

Run: `cd runtime/asm && PATH=/Users/barry/Git/cc65/bin:$PATH make`
Expected: builds `libcall.bin` with no errors (loader unchanged this task).

**Step 3: Commit**

```bash
git add runtime/asm/libabi.inc
git commit -m "feat(shelf): reserve shelf_tag/shelf_lru directory in loader band"
```

---

### Task 2: Emulator shelf-staging test helper

Mirror the firmware's "stage module to slot + seed directory" in `CompositeBusDevice` so emulator tests exercise the directory exactly as hardware will. The existing test-only `LoadXram` stages bytes but seeds nothing.

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (near `LoadXram`, ~line 330)

**Step 1: Add constants + helper.** After `LoadXram` (line 330) add:

```csharp
// Dynamic module shelf (libabi.inc): N=4 cache slots at SHELF_BASE + i*$4000;
// the directory (slot->id tags + LRU order) lives in the loader band.
internal const int ShelfBaseAddr = 0x060000;
internal const int ShelfSlotBytes = 0x4000;
internal const int ShelfN = 4;
internal const ushort ShelfTag = 0x0418;   // SHELF_N bytes
internal const ushort ShelfLru = 0x041C;   // SHELF_N bytes

/// <summary>
/// Test hook mirroring the firmware boot path: stage a 16K module image into
/// XRAM shelf slot <paramref name="slot"/> and seed the directory so the resident
/// loader's scan finds it. Initializes shelf_lru to identity [0,1,2,3] on slot 0.
/// </summary>
internal void StageShelfModule(int slot, byte[] image, byte id)
{
    ArgumentNullException.ThrowIfNull(image);
    LoadXram(ShelfBaseAddr + slot * ShelfSlotBytes, image);
    _ram[ShelfTag + slot] = id;
    if (slot == 0)
        for (int i = 0; i < ShelfN; i++) _ram[ShelfLru + i] = (byte)i;
}
```

**Step 2: Build the emulator project to confirm it compiles**

Run: `dotnet build e6502.Avalonia 2>&1 | grep -E 'error|Build succeeded'`
Expected: `Build succeeded.`

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs
git commit -m "test(shelf): add StageShelfModule emulator helper (stage + seed directory)"
```

---

### Task 3: Directory-scan loader (`libcall.s : modtab_lookup`)

The core change. TDD through the emulator: a module staged at a **non-zero** slot must dispatch (proving slot is decoupled from id), and an unknown id must fail.

**Files:**
- Modify: `runtime/asm/libcall.s:89-110` (replace `modtab_lookup`)
- Modify: `e6502UnitTests/CompositeLoaderTests.cs` (seed the directory; add new tests)
- Rebuild: `runtime/asm/libcall.bin` → `e6502.Avalonia/Resources/libcall.bin`

**Step 1: Update the existing tests to seed the directory (contract change).** In `e6502UnitTests/CompositeLoaderTests.cs`, in `SetupLoaderBus()`, replace the `LoadXram(...graphics.bin)` line with the seeding helper:

```csharp
// was: bus.LoadXram(ShelfBase, File.ReadAllBytes(RepoPath("modules","graphics","graphics.bin")));
bus.StageShelfModule(0, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                     MODULE_ID_GRAPHICS);
```

**Step 2: Write the failing test — module at a NON-zero slot.** Add to `CompositeLoaderTests.cs`:

```csharp
[TestMethod]
public void Gcls_ModuleAtNonZeroSlot_DispatchesViaDirectory()
{
    var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
    var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
    // Stage GRAPHICS into slot 2 ($068000) only — slot 0 is empty.
    bus.StageShelfModule(2, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                         MODULE_ID_GRAPHICS);
    bus.WriteRam(RESIDENT, 0x00);
    bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

    CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

    Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "must dispatch from slot 2 via the directory");
    Assert.AreEqual(MODULE_ID_GRAPHICS, bus.ReadRam(RESIDENT));
    Assert.AreEqual(1, bus.PageInCount);
}
```

**Step 3: Run it — expect FAIL.** The old `modtab_lookup` hardwires GRAPHICS→slot 0 ($060000), which is empty, so the header validate fails.

Run: `dotnet test e6502UnitTests --filter "FullyQualifiedName~CompositeLoaderTests" 2>&1 | tail -15`
Expected: `Gcls_ModuleAtNonZeroSlot_DispatchesViaDirectory` FAILS (STATUS != LERR_OK). Existing tests may also fail until Step 4 (acceptable RED).

**Step 4: Replace `modtab_lookup` in `runtime/asm/libcall.s`** (lines 89-110) with the directory scan + LRU touch:

```asm
; modtab_lookup — A = module id. Scan shelf_tag[] for a resident slot.
; HIT : program PGD_SRC = SHELF_BASE + slot*$4000, PGD_WORDS = SHELF_SLOT_WORDS,
;       bump the slot to MRU in shelf_lru[], return C=0.
; MISS: return C=1 (caller -> LERR_BAD_MODULE). Phase B adds the SD load handshake.
; The compile-time map is gone: slot is assigned by the host (firmware/test harness),
; which seeds shelf_tag[]/shelf_lru[]. See docs/plans/2026-06-05-dynamic-module-shelf-design.md.
modtab_lookup:
      ldx     #0
mt_scan:
      cmp     SHELF_TAG,x
      beq     mt_hit
      inx
      cpx     #SHELF_N
      bne     mt_scan
      sec                              ; not resident
      rts
mt_hit:
      stx     LIB_SCRATCH              ; save slot index for the LRU touch
      lda     #SHELF_BASE_L
      sta     PGD_SRCL
      txa                              ; slot index -> mid byte = slot*$40 (slot<<6)
      asl
      asl
      asl
      asl
      asl
      asl
      sta     PGD_SRCM
      lda     #SHELF_BASE_H
      sta     PGD_SRCH
      lda     #<SHELF_SLOT_WORDS
      sta     PGD_WORDSL
      lda     #>SHELF_SLOT_WORDS
      sta     PGD_WORDSH
      jsr     shelf_touch
      clc
      rts

; shelf_touch — make slot (in LIB_SCRATCH) the MRU entry of shelf_lru[].
; Finds it, shifts the preceding entries down one, writes it at [0].
shelf_touch:
      ldx     #0
st_find:
      lda     SHELF_LRU,x
      cmp     LIB_SCRATCH
      beq     st_found
      inx
      cpx     #SHELF_N
      bne     st_find
      rts                              ; not present (shouldn't happen) — leave as-is
st_found:
      txa
      beq     st_done                  ; already MRU (position 0)
st_shift:
      lda     SHELF_LRU-1,x
      sta     SHELF_LRU,x
      dex
      bne     st_shift
      lda     LIB_SCRATCH
      sta     SHELF_LRU                ; [0] = slot
st_done:
      rts
```

**Step 5: Rebuild and install the loader.**

Run:
```bash
cd runtime/asm && PATH=/Users/barry/Git/cc65/bin:$PATH make && \
  printf 'libcall.bin size: ' && wc -c < libcall.bin
```
Expected: builds clean; **size ≤ 248** (must stay below `$0418`). Then install to the emulator:
```bash
cd runtime/asm && PATH=/Users/barry/Git/cc65/bin:$PATH make install
```
(Confirm `make install` target copies to `e6502.Avalonia/Resources/libcall.bin`; if `RESOURCES` is unset, copy manually: `cp libcall.bin ../../e6502.Avalonia/Resources/libcall.bin`.)

**Step 6: Run the tests — expect PASS.**

Run: `dotnet test e6502UnitTests --filter "FullyQualifiedName~CompositeLoaderTests" 2>&1 | tail -8`
Expected: all CompositeLoaderTests PASS (existing + the new non-zero-slot test). Output pristine.

**Step 7: Add LRU + unknown-id tests.** Append:

```csharp
[TestMethod]
public void LibCall_UnknownId_ReturnsBadModule()
{
    using var bus = SetupLoaderBus();       // only GRAPHICS (id 1) staged at slot 0
    CallLib(bus, 0x42, 0x00);               // id $42 is not in the directory
    Assert.AreEqual(LERR_BAD_MODULE, bus.ReadRam(STATUS));
}

[TestMethod]
public void LibCall_Hit_MovesSlotToFrontOfLru()
{
    var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
    var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
    bus.StageShelfModule(2, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                         MODULE_ID_GRAPHICS);
    // Force a known LRU order where slot 2 is at the back.
    bus.WriteRam(0x041C, 0); bus.WriteRam(0x041D, 1); bus.WriteRam(0x041E, 3); bus.WriteRam(0x041F, 2);
    bus.WriteRam(RESIDENT, 0x00);
    bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

    CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

    Assert.AreEqual(2, bus.ReadRam(0x041C), "hit slot must move to LRU front");
    Assert.AreEqual(0, bus.ReadRam(0x041D));
    Assert.AreEqual(1, bus.ReadRam(0x041E));
    Assert.AreEqual(3, bus.ReadRam(0x041F));
}
```

**Step 8: Run — expect PASS.**

Run: `dotnet test e6502UnitTests --filter "FullyQualifiedName~CompositeLoaderTests" 2>&1 | tail -8`
Expected: all PASS.

**Step 9: Commit**

```bash
git add runtime/asm/libcall.s runtime/asm/libcall.bin runtime/asm/libcall.map \
        e6502.Avalonia/Resources/libcall.bin e6502UnitTests/CompositeLoaderTests.cs
git commit -m "feat(shelf): directory-scan loader replaces static modtab_lookup"
```

---

### Task 4: Firmware — slot = array index, compute base, seed directory

Make the slot-assignment logic a pure, host-testable function; then wire it into `stageConfiguredLibraries` and poke the directory.

**Files:**
- Create: `e6502.ESP32/novahost/shelf_alloc.h` (pure logic, no Arduino deps)
- Test: `e6502.ESP32/novahost/test/test_shelf_alloc.cpp`
- Modify: `e6502.ESP32/novahost/test/Makefile` (add the test)
- Modify: `e6502.ESP32/novahost/novahost.ino` (constants + `stageConfiguredLibraries` + seed poke)

**Step 1: Write the failing host test.** Create `test/test_shelf_alloc.cpp`:

```cpp
#include "../shelf_alloc.h"
#include <cassert>
#include <cstdio>

int main() {
    // slot = index; base = SHELF_BASE + index*SLOT; tag table tracks id per slot.
    ShelfPlan p;
    shelf_plan_reset(&p);
    assert(shelf_plan_add(&p, /*id*/1) == 0);   // slot 0
    assert(shelf_plan_add(&p, /*id*/2) == 1);   // slot 1
    assert(p.base[0] == 0x060000u);
    assert(p.base[1] == 0x064000u);
    assert(p.tag[0] == 1 && p.tag[1] == 2);
    assert(p.tag[2] == 0 && p.tag[3] == 0);     // empty slots
    // LRU identity order.
    assert(p.lru[0] == 0 && p.lru[1] == 1 && p.lru[2] == 2 && p.lru[3] == 3);
    // Overflow past SHELF_N is rejected.
    shelf_plan_add(&p, 3); shelf_plan_add(&p, 4);
    assert(shelf_plan_add(&p, 5) < 0);
    printf("shelf_alloc OK\n");
    return 0;
}
```

**Step 2: Run it — expect FAIL (header missing).**

Run: `cd e6502.ESP32/novahost/test && make test_shelf_alloc 2>&1 | tail`
Expected: compile error (`shelf_alloc.h` not found). If the Makefile has no such target yet, this fails to build — that's RED.

**Step 3: Create `e6502.ESP32/novahost/shelf_alloc.h`:**

```cpp
#pragma once
#include <stdint.h>
#include <string.h>

// Dynamic module shelf — pure boot-time slot allocator (host-testable, no Arduino).
// slot = sequential index; base = SHELF_BASE + slot*SHELF_SLOT_SIZE. Mirrors
// runtime/asm/libabi.inc (SHELF_BASE=$060000, SHELF_SLOT_WORDS=$2000=16KB, N=4).
#define SHELF_BASE       0x060000u
#define SHELF_SLOT_SIZE  0x4000u
#define SHELF_N          4

typedef struct {
    int      count;
    uint32_t base[SHELF_N];   // XRAM byte address per used slot
    uint8_t  tag[SHELF_N];    // module id per slot (0 = empty)
    uint8_t  lru[SHELF_N];    // slot indices, MRU front -> LRU back
} ShelfPlan;

static inline void shelf_plan_reset(ShelfPlan* p) {
    memset(p, 0, sizeof(*p));
    for (int i = 0; i < SHELF_N; i++) p->lru[i] = (uint8_t)i;
}

// Returns the assigned slot index, or -1 if the shelf is full.
static inline int shelf_plan_add(ShelfPlan* p, uint8_t id) {
    if (p->count >= SHELF_N) return -1;
    int slot = p->count++;
    p->base[slot] = SHELF_BASE + (uint32_t)slot * SHELF_SLOT_SIZE;
    p->tag[slot]  = id;
    return slot;
}

// Serialize the 6502 directory image: tag[N] then lru[N] (SHELF_N*2 bytes),
// matching SHELF_TAG ($0418) immediately followed by SHELF_LRU ($041C).
static inline void shelf_plan_dir(const ShelfPlan* p, uint8_t* out /*[SHELF_N*2]*/) {
    for (int i = 0; i < SHELF_N; i++) out[i] = p->tag[i];
    for (int i = 0; i < SHELF_N; i++) out[SHELF_N + i] = p->lru[i];
}
```

**Step 4: Add the test target to `test/Makefile`** (follow the existing pattern, e.g. how `test_boot_config_parser` is built) and run — expect PASS.

Run: `cd e6502.ESP32/novahost/test && make test 2>&1 | tail`
Expected: `shelf_alloc OK` and the rest of the suite still green.

**Step 5: Wire into `novahost.ino`.** Add `#include "shelf_alloc.h"` near the top includes, add the directory RAM address constant near line 499:

```cpp
static const uint16_t SHELF_DIR_BASE = 0x0418;   // SHELF_TAG; SHELF_LRU follows at $041C
```

Then rewrite the body of `stageConfiguredLibraries()` (lines 887-916) to assign slots and seed the directory. Replace the loop with:

```cpp
    ShelfPlan plan;
    shelf_plan_reset(&plan);
    int staged = 0;
    for (JsonObject lib : libs) {
        const char* name = lib["name"] | "";
        const char* path = lib["path"] | "";
        int      id   = lib["id"] | -1;
        uint32_t size = lib["size"] | (uint32_t)BOOT_ROM_LEN;   // .nmod image is 16K
        if (path[0] == '\0' || id < 0) {
            logLn("WARN: boot library '%s' missing path/id; skipped", name[0] ? name : "?");
            continue;
        }
        int slot = shelf_plan_add(&plan, (uint8_t)id);
        if (slot < 0) { logLn("WARN: shelf full (%d slots); '%s' skipped", SHELF_N, name); continue; }
        uint32_t base = plan.base[slot];
        if (!streamSdramAsset(base, name[0] ? name : path, &path, 1, (size_t)size)) {
            logLn("WARN: boot library '%s' (%s) not staged; continuing", name[0] ? name : "?", path);
            plan.count--; plan.tag[slot] = 0;   // roll back the failed slot
            continue;
        }
        logLn("Boot library staged: %s id=%d slot=%d (%u bytes @ XRAM $%06X)",
              name[0] ? name : path, id, slot, (unsigned)size, (unsigned)base);
        staged++;
    }

    uint8_t dir[SHELF_N * 2];
    shelf_plan_dir(&plan, dir);
    if (!fpgaBridge.pokeBlock(SHELF_DIR_BASE, dir, sizeof(dir)))
        logLn("WARN: shelf directory poke failed @ $%04X", (unsigned)SHELF_DIR_BASE);
    else
        logLn("Shelf directory seeded: %d module(s) @ RAM $%04X", staged, (unsigned)SHELF_DIR_BASE);

    logLn("Boot libraries staged: %d", staged);
    return staged;
```

(`base` is no longer read from JSON; a legacy `base` field is simply ignored.)

**Step 6: Compile the firmware (host-side compile check only here; flashing is Task 7).**

Run:
```bash
cd e6502.ESP32/novahost && arduino-cli compile \
  --fqbn 'esp32:esp32:lolin32:PartitionScheme=min_spiffs' --build-path build/shelf . 2>&1 | tail -5
```
Expected: compiles without error.

**Step 7: Commit**

```bash
git add e6502.ESP32/novahost/shelf_alloc.h e6502.ESP32/novahost/test/test_shelf_alloc.cpp \
        e6502.ESP32/novahost/test/Makefile e6502.ESP32/novahost/novahost.ino
git commit -m "feat(shelf): firmware slot=index allocation + directory seeding"
```

---

### Task 5: Web — `boot.json libraries[]` carries `{name,id,version,path,size}` (no base)

Move the libraries[] mutation into a shared, testable helper and update the staged-toggle/delete (built this session) to the new schema.

**Files:**
- Create: `e6502.Storage/BootLibraries.cs`
- Test: `e6502UnitTests/BootLibrariesTests.cs`
- Modify: `e6502.Nova/NovaWebServer.cs` (`SetLibraryEntry`/`RemoveLibraryEntry` → delegate; `ModuleSetStagedAsync` passes id+version+size)

**Step 1: Write the failing test.** Create `e6502UnitTests/BootLibrariesTests.cs`:

```csharp
using System.Text.Json.Nodes;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class BootLibrariesTests
{
    [TestMethod]
    public void SetEntry_NewModule_WritesFullSchemaNoBase()
    {
        var cfg = new JsonObject();
        BootLibraries.SetEntry(cfg, "graphics.nmod", id: 1, version: 1, size: 16384);

        JsonObject e = cfg["libraries"]!.AsArray()[0]!.AsObject();
        Assert.AreEqual("graphics", (string?)e["name"]);
        Assert.AreEqual(1, (int)e["id"]!);
        Assert.AreEqual(1, (int)e["version"]!);
        Assert.AreEqual("/lib/graphics.nmod", (string?)e["path"]);
        Assert.AreEqual(16384, (int)e["size"]!);
        Assert.IsNull(e["base"], "base must not be written (firmware assigns the slot)");
    }

    [TestMethod]
    public void SetEntry_ExistingModule_UpdatesInPlace()
    {
        var cfg = new JsonObject();
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 1, 16384);
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 2, 16384);   // version bump
        Assert.AreEqual(1, cfg["libraries"]!.AsArray().Count, "no duplicate entry");
        Assert.AreEqual(2, (int)cfg["libraries"]!.AsArray()[0]!["version"]!);
    }

    [TestMethod]
    public void RemoveEntry_ByBasename_RemovesAndReportsTrue()
    {
        var cfg = new JsonObject();
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 1, 16384);
        Assert.IsTrue(BootLibraries.RemoveEntry(cfg, "graphics.nmod"));
        Assert.AreEqual(0, cfg["libraries"]!.AsArray().Count);
        Assert.IsFalse(BootLibraries.RemoveEntry(cfg, "graphics.nmod"), "second remove is a no-op");
    }
}
```

**Step 2: Run — expect FAIL** (class missing).

Run: `dotnet test e6502UnitTests --filter "FullyQualifiedName~BootLibrariesTests" 2>&1 | tail`
Expected: compile error / `BootLibraries` not found.

**Step 3: Create `e6502.Storage/BootLibraries.cs`:**

```csharp
using System.IO;
using System.Text.Json.Nodes;

namespace e6502.Storage;

/// <summary>
/// Boot-config libraries[] membership helpers, shared by the nova CLI and web.
/// A libraries[] entry names a paged-library module for the firmware to stage at
/// boot. Phase A: no `base` — the firmware assigns slot = array index and computes
/// the XRAM address. See docs/plans/2026-06-05-dynamic-module-shelf-design.md.
/// </summary>
public static class BootLibraries
{
    /// <summary>Add or update (by filename) a libraries[] entry. Writes
    /// {name,id,version,path,size}; never writes base.</summary>
    public static void SetEntry(JsonObject config, string fileName, int id, int version, int size)
    {
        if (config["libraries"] is not JsonArray libs)
        {
            libs = new JsonArray();
            config["libraries"] = libs;
        }
        string name = Path.GetFileNameWithoutExtension(fileName);
        string path = $"/lib/{fileName}";
        foreach (JsonNode? lib in libs)
        {
            if (lib is JsonObject o && Matches(o, fileName))
            {
                o["name"] = name; o["id"] = id; o["version"] = version;
                o["path"] = path; o["size"] = size;
                o.Remove("base");
                return;
            }
        }
        libs.Add(new JsonObject
        {
            ["name"] = name, ["id"] = id, ["version"] = version,
            ["path"] = path, ["size"] = size,
        });
    }

    /// <summary>Remove all libraries[] entries whose path basename matches
    /// <paramref name="fileName"/>. Returns true if anything was removed.</summary>
    public static bool RemoveEntry(JsonObject config, string fileName)
    {
        if (config["libraries"] is not JsonArray libs) return false;
        bool removed = false;
        for (int i = libs.Count - 1; i >= 0; i--)
        {
            if (libs[i] is JsonObject o && Matches(o, fileName)) { libs.RemoveAt(i); removed = true; }
        }
        return removed;
    }

    private static bool Matches(JsonObject entry, string fileName) =>
        string.Equals(Path.GetFileName(entry["path"]?.GetValue<string>() ?? ""),
                      fileName, System.StringComparison.OrdinalIgnoreCase);
}
```

**Step 4: Run — expect PASS.**

Run: `dotnet test e6502UnitTests --filter "FullyQualifiedName~BootLibrariesTests" 2>&1 | tail`
Expected: 3 PASS.

**Step 5: Delegate from `NovaWebServer.cs`.** Replace the bodies of the private `SetLibraryEntry`/`RemoveLibraryEntry` (added this session) — or delete them and call `BootLibraries` directly. In `ModuleSetStagedAsync`, change the staging branch from `SetLibraryEntry(config, name, mod.Id)` to:

```csharp
BootLibraries.SetEntry(config, name, mod.Id, mod.AbiVersion, NovaModule.ImageSize);
```

and the unstage branch to `BootLibraries.RemoveEntry(config, name)`. In `ModuleDeleteAsync`, change to `BootLibraries.RemoveEntry(config, name)`. Remove the now-dead private helpers. Add `using e6502.Storage;` if not present.

**Step 6: Build + full Nova/web test pass.**

Run:
```bash
dotnet build e6502.Nova 2>&1 | grep -E 'error|Build succeeded'
dotnet test e6502UnitTests --filter "FullyQualifiedName~NovaModuleTests|FullyQualifiedName~BootLibrariesTests" 2>&1 | tail -5
```
Expected: `Build succeeded.`; all module + bootlib tests PASS.

**Step 7: Commit**

```bash
git add e6502.Storage/BootLibraries.cs e6502UnitTests/BootLibrariesTests.cs e6502.Nova/NovaWebServer.cs
git commit -m "feat(shelf): web libraries[] toggle writes {name,id,version,path,size}, no base"
```

---

### Task 6: Sync `libcall.bin` to the firmware SD staging + boot.json schema

The new loader must reach the board's SD card; the firmware Makefile reproduces SD staging.

**Files:**
- Modify (regenerated): `e6502.ESP32/novahost/build/sd/roms/libcall.bin`
- Reference: `e6502.ESP32/novahost/Makefile`

**Step 1: Regenerate the firmware SD staging** (it pulls `libcall.bin` from `runtime/asm`):

Run: `cd e6502.ESP32/novahost && grep -n libcall Makefile` to find the staging target, then run it (e.g. `make sd-assets`). Verify the staged copy matches:
```bash
cmp runtime/asm/libcall.bin e6502.ESP32/novahost/build/sd/roms/libcall.bin && echo "libcall.bin in sync"
```
Expected: `libcall.bin in sync`.

**Step 2: Confirm no other live mirror drifted.** The `.bin` copies under `*/bin/*/Resources/` are dotnet build outputs (regenerated on build) — ignore. The canonical pair is `runtime/asm/libcall.bin` + `e6502.Avalonia/Resources/libcall.bin` (Task 3) + the firmware SD copy (this task). `tests/asm/libcall.bin` is a DIFFERENT ORG ($9C00) build — do **not** touch.

**Step 3: Commit**

```bash
git add e6502.ESP32/novahost/build/sd/roms/libcall.bin
git commit -m "chore(rom): sync directory-scan libcall.bin to firmware SD staging"
```

---

### Task 7: Hardware verification (flash + round-trip)

**No code — verification only. Mutates the board; snapshot and restore.** Board: `novahost.local` / `192.168.1.65`, default runtime `novalogo` (leave it).

**Step 1: Snapshot the board boot.json** (read-only):

Run: `dotnet e6502.Nova/bin/Debug/net10.0/osx-arm64/nova.dll get /config/boot.json /tmp/boot.pre.json --remote=novahost.local`

**Step 2: Update the board boot.json `libraries[]` to the new schema** (drop `base`; the firmware now computes it). Use the web staged-toggle or hand-edit + `nova put`, preserving `defaultRuntime=novalogo`. Target entry:
```json
{ "name": "graphics", "id": 1, "version": 1, "path": "/lib/graphics.nmod", "size": 16384 }
```

**Step 3: Push the new loader to the SD card** (the board's `/roms/libcall.bin`):

Run: `dotnet e6502.Nova/bin/Debug/net10.0/osx-arm64/nova.dll put runtime/asm/libcall.bin /roms/libcall.bin --remote=novahost.local`

**Step 4: Flash the firmware (OTA)** and reboot to re-stage:

Run:
```bash
cd e6502.ESP32/novahost && arduino-cli compile --fqbn 'esp32:esp32:lolin32:PartitionScheme=min_spiffs' --build-path build/shelf .
python3 "$(find ~/Library/Arduino15 -name espota.py | head -1)" -i 192.168.1.65 -p 3232 -f build/shelf/novahost.ino.bin
```
Expected: OTA upload succeeds; board reboots and re-stages (the serial/log shows `Shelf directory seeded: 1 module(s) @ RAM $0418`).

**Step 5: Verify graphics still works** (the handoff's smoke test). On the board (via `nova` type_text + ENTER), run NovaLogo: `CS` then `REPEAT 4 [FD 50 RT 90]` — the square must draw (proves `lib_call(GRAPHICS)` resolves the slot via the directory). Confirm `LIB_RESIDENT=$01` by peeking RAM if needed.

**Step 6: Web toggle round-trip** (uses the local web server proxying to the board):

Run `nova webserver --remote=novahost.local --no-open --port 8088` (background), then:
- `GET /api/modules` → graphics `staged:true`.
- `POST /api/modules/graphics.nmod/staged {"staged":false}` → entry removed.
- `POST /api/modules/graphics.nmod/staged {"staged":true}` → entry re-added with `{name,id,version,path,size}`, no `base`.
- `GET /api/modules` → graphics `staged:true` again.

**Step 7: Restore exact original boot.json** to leave zero trace:

Run: `dotnet e6502.Nova/bin/Debug/net10.0/osx-arm64/nova.dll put /tmp/boot.pre.json /config/boot.json --remote=novahost.local`
…unless the new-schema boot.json is the desired end state (it is — Step 2's schema is correct going forward). If keeping the new schema, skip restore; just confirm `defaultRuntime` is still `novalogo`.

**Step 8: Commit any board-config artifacts captured in-repo** (e.g. `e6502.ESP32/novahost/build/sd/config/boot.json` if it should reflect the new schema):

```bash
git add -A e6502.ESP32/novahost/build/sd/config/boot.json
git commit -m "chore(boot): board libraries[] schema drops base (firmware assigns slot)"
```

---

## Done criteria (Phase A)

- TEST/GRAPHICS slot-0 collision is gone: a module dispatches from any slot the host assigns (emulator test `Gcls_ModuleAtNonZeroSlot_DispatchesViaDirectory`).
- `boot.json libraries[]` carries no `base`; firmware assigns slot = array index and seeds the 6502 directory; graphics verified on hardware.
- Web `stage:true` writes `{name,id,version,path,size}` (no base); round-trip verified on the board.
- All emulator + storage tests green; `libcall.bin` ≤ 248 bytes and in sync across the canonical, emulator, and firmware-SD mirrors.
- Phase B (runtime miss/eviction via FIO `LOAD_MODULE`) remains a clean additive follow-on — the directory it needs is now live.
```
