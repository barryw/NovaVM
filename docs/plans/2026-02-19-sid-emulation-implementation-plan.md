# SID Emulation Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace the Virtual Sound Controller with a real MOS 6581 SID chip emulation mapped at $D400, add periodic interrupt infrastructure, and build SID file loading via BASIC commands and MCP tools.

**Architecture:** Port cadaver's MIT-licensed SID.cs (~410 lines) from Unity to our codebase, stripping Unity dependencies (Mathf, AudioSettings, RAM64K). Extract OpenAlRenderer from VirtualSoundController into its own file. The SID emulation generates samples via a `Clock(cycles)` / `RenderSamples()` pattern fed into OpenAlRenderer. Bus routing in CompositeBusDevice intercepts writes to $D400-$D41C for the SID while reads still return ROM bytes.

**Tech Stack:** C# / .NET 10, MSTest, OpenAL (existing P/Invoke renderer)

---

### Task 1: Extract OpenAlRenderer to its own file

The OpenAlRenderer is currently a nested class inside VirtualSoundController.cs. Extract it to a standalone file so both the old VSC (during transition) and the new SID chip can use it.

**Files:**
- Create: `e6502.Avalonia/Hardware/OpenAlRenderer.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualSoundController.cs`

**Step 1: Create `OpenAlRenderer.cs`**

Copy the `OpenAlRenderer` class (lines 245-474 of `VirtualSoundController.cs`) into a new file. Change from `private sealed` to `internal sealed`. Add the `System.Runtime.InteropServices` using. Keep the constructor signature: `Func<int, short[]> sampleProvider, int sampleRate`.

```csharp
using System.Runtime.InteropServices;

namespace e6502.Avalonia.Hardware;

internal sealed class OpenAlRenderer : IDisposable
{
    // ... entire OpenAlRenderer class body from VirtualSoundController.cs lines 247-473 ...
    // No changes to any method body — just moved out of the nested class.
}
```

**Step 2: Update `VirtualSoundController.cs`**

Remove the nested `OpenAlRenderer` class (lines 245-474). Change the `_renderer` field type from the nested class to the new standalone class. The field declaration at line 15 (`private readonly OpenAlRenderer? _renderer;`) stays the same since the class name hasn't changed.

**Step 3: Verify build**

Run: `dotnet build e6502.Avalonia`
Expected: Success

**Step 4: Run existing tests**

Run: `dotnet test`
Expected: All tests pass (no behavior changed)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/OpenAlRenderer.cs e6502.Avalonia/Hardware/VirtualSoundController.cs
git commit -m "refactor: extract OpenAlRenderer to standalone file"
```

---

### Task 2: Create SidChip class

Port cadaver's SID.cs, replacing Unity dependencies with System.Math and our OpenAlRenderer. The SID has 3 voice channels, ADSR envelopes with rate tables, LP/BP/HP filter with resonance, oscillator sync, ring modulation, and waveform combinations (triangle, saw, pulse, noise).

**Files:**
- Create: `e6502.Avalonia/Hardware/SidChip.cs`

**Step 1: Write the SID register read/write test**

Create a test that verifies the SidChip accepts writes and reads back register values.

Add to `e6502UnitTests/AvaloniaSidTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaSidTests
{
    private const ushort SidBase = 0xD400;

    [TestMethod]
    public void Sid_WriteRegister_ReadBack()
    {
        var sid = new SidChip(enableAudio: false);
        sid.Write(SidBase + 0x00, 0x37);  // voice 1 freq lo
        sid.Write(SidBase + 0x01, 0x1C);  // voice 1 freq hi
        Assert.AreEqual(0x37, sid.Read(SidBase + 0x00));
        Assert.AreEqual(0x1C, sid.Read(SidBase + 0x01));
    }

    [TestMethod]
    public void Sid_OwnsAddress_InRange()
    {
        var sid = new SidChip(enableAudio: false);
        Assert.IsTrue(sid.OwnsAddress(SidBase));          // $D400
        Assert.IsTrue(sid.OwnsAddress(SidBase + 0x1C));   // $D41C
        Assert.IsFalse(sid.OwnsAddress(SidBase - 1));     // $D3FF
        Assert.IsFalse(sid.OwnsAddress(SidBase + 0x1D));  // $D41D
    }

    [TestMethod]
    public void Sid_VoiceControl_GateOn_StartsEnvelope()
    {
        var sid = new SidChip(enableAudio: false);
        // Set voice 1: frequency, AD, SR, then gate on
        sid.Write(SidBase + 0x00, 0x37);  // freq lo
        sid.Write(SidBase + 0x01, 0x1C);  // freq hi (~440Hz)
        sid.Write(SidBase + 0x05, 0x00);  // AD = 0 (fastest attack/decay)
        sid.Write(SidBase + 0x06, 0xF0);  // SR = sustain max, release 0
        sid.Write(SidBase + 0x04, 0x11);  // gate on + triangle wave

        // Clock the SID forward — should produce non-silent output
        sid.Clock(1000);
        // Verify it's generating something (at minimum, no crash)
    }

    [TestMethod]
    public void Sid_MasterVolume_WrittenToRegister()
    {
        var sid = new SidChip(enableAudio: false);
        sid.Write(SidBase + 0x18, 0x0F);  // max volume
        Assert.AreEqual(0x0F, sid.Read(SidBase + 0x18));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "AvaloniaSidTests"`
Expected: FAIL — SidChip class does not exist

**Step 3: Create `SidChip.cs`**

Port cadaver's `SID` and `SIDChannel` classes. Key changes from the Unity original:
- Replace `using UnityEngine;` and `using EMU6502;` with nothing (no external deps)
- Replace `Mathf.Min` → `Math.Min`, `Mathf.PI` → `Math.PI`, `Mathf.Sin` → `Math.Sin`, `Mathf.Pow` → `Math.Pow`, `Mathf.Ceil` → `Math.Ceiling`
- Remove `RAM64K _ram` dependency — instead, read from an internal `byte[] _registers` array (29 bytes)
- Replace `_ram.ReadIO(addr)` calls in `BufferSamples` with `_registers[addr - 0xD400]`
- Remove `AudioSettings.outputSampleRate` — use constant `SampleRate = 44100`
- Remove Unity's `List<float> samples` / lock pattern — instead, implement `short[] RenderSamples(int count)` that returns PCM16 samples for OpenAlRenderer
- Add `OwnsAddress(ushort)`, `Read(ushort)`, `Write(ushort, byte)` methods matching the bus interface pattern
- Add `Clock(int cycles)` public method that clocks all 3 channels and accumulates samples
- The `Clock` method should be called from `RenderSamples` — when the audio thread requests N samples, clock the SID enough cycles to produce them
- Constructor takes `bool enableAudio` — if true, creates an OpenAlRenderer with RenderSamples as the callback

The register layout (29 bytes at $D400-$D41C):
```
$D400-$D406: Voice 1 (freq lo, freq hi, pw lo, pw hi, control, AD, SR)
$D407-$D40D: Voice 2 (same layout)
$D40E-$D414: Voice 3 (same layout)
$D415: Filter cutoff lo (bits 0-2)
$D416: Filter cutoff hi
$D417: Filter resonance (hi nibble) + voice routing (lo nibble)
$D418: Mode/volume — filter mode (bits 4-6) + master vol (bits 0-3)
$D419-$D41C: Read-only (paddle, osc3, env3) — return 0 for now
```

```csharp
namespace e6502.Avalonia.Hardware;

// Port of cadaver/oldschoolengine2 SID.cs (MIT License, Lasse Oorni 2018-2024)
// Based on jsSID by Hermit (Mihaly Horvath, 2016)

public sealed class SidChip : IDisposable
{
    private const int SidBase = 0xD400;
    private const int SidEnd = 0xD41C;
    private const int RegisterCount = SidEnd - SidBase + 1;
    private const int SampleRate = 44100;
    private const double CpuClockRate = 985248.0;  // PAL C64 clock

    private readonly byte[] _registers = new byte[RegisterCount];
    private readonly SidChannel[] _channels;
    private readonly OpenAlRenderer? _renderer;
    private readonly object _lock = new();

    private double _cyclesPerSample;
    private double _cycleAccumulator;
    private double _prevBandPass;
    private double _prevLowPass;

    public SidChip(bool enableAudio = false)
    {
        _channels = [new SidChannel(), new SidChannel(), new SidChannel()];
        _channels[0].SyncTarget = _channels[1];
        _channels[1].SyncTarget = _channels[2];
        _channels[2].SyncTarget = _channels[0];
        _channels[0].SyncSource = _channels[2];
        _channels[1].SyncSource = _channels[0];
        _channels[2].SyncSource = _channels[1];

        _cyclesPerSample = CpuClockRate / SampleRate;

        if (enableAudio)
        {
            try { _renderer = new OpenAlRenderer(RenderSamples, SampleRate); }
            catch { _renderer = null; }
        }
    }

    public bool OwnsAddress(ushort address) =>
        address >= SidBase && address <= SidEnd;

    public byte Read(ushort address) =>
        _registers[address - SidBase];

    public void Write(ushort address, byte data) =>
        _registers[address - SidBase] = data;

    public void Clock(int cycles) { /* calls channel.Clock for each channel, accumulates samples */ }

    public void Dispose() => _renderer?.Dispose();

    private short[] RenderSamples(int sampleCount)
    {
        // Lock, read registers into channel state, clock channels,
        // generate filtered output, convert float→short, return PCM16 array
    }
}
```

The full implementation ports cadaver's `BufferSamples` logic (reading registers, clocking channels, filter calculation) into `RenderSamples`, and ports `SIDChannel` as an inner class `SidChannel` with all the waveform generators, ADSR logic, and sync/ring mod.

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "AvaloniaSidTests"`
Expected: All 4 tests PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/SidChip.cs e6502UnitTests/AvaloniaSidTests.cs
git commit -m "feat: add SID chip emulation (port of cadaver's SID.cs)"
```

---

### Task 3: Wire SidChip into CompositeBusDevice

Replace VirtualSoundController with SidChip in the bus. Key routing change: writes to $D400-$D41C go to SID, reads from the same range return ROM (since $D400 is inside the ROM region $C000-$FFFF).

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502UnitTests/AvaloniaCompositeBusTests.cs`

**Step 1: Write bus routing tests**

Replace the VSC tests in `AvaloniaCompositeBusTests.cs` (lines 92-167) with SID tests:

```csharp
// -------------------------------------------------------------------------
// SID chip routing ($D400-$D41C)
// -------------------------------------------------------------------------

[TestMethod]
public void Sid_Write_DoesNotCorruptRom()
{
    var bus = MakeBus();
    byte romByte = bus.Read(0xD400);  // read ROM value first
    bus.Write(0xD400, 0xFF);          // write to SID
    Assert.AreEqual(romByte, bus.Read(0xD400));  // read still returns ROM
}

[TestMethod]
public void Sid_WriteRegister_RoutesToSid()
{
    var bus = MakeBus();
    bus.Write(0xD418, 0x0F);  // master volume max
    // The SID received the write (tested via SID-specific method if exposed)
    // At minimum, it didn't crash and ROM is intact
    byte romByte = bus.Read(0xD418);
    // ROM value at $D418 should be whatever ehbasic has there, not $0F
}

[TestMethod]
public void Sid_OutsideRange_NotRouted()
{
    var bus = MakeBus();
    byte romBefore = bus.Read(0xD41D);
    bus.Write(0xD41D, 0xAA);  // outside SID range, should be ROM-protected
    Assert.AreEqual(romBefore, bus.Read(0xD41D));
}
```

**Step 2: Update `VgcConstants.cs`**

Add SID constants, remove old VSC constants:

```csharp
// Remove: VscBase, VscEnd, VscCmd, VscP0-P7, VscActiveMask, VscMasterVol,
//         VscCmdSound, VscCmdVolume, VscCmdEnvelope, VscCmdWave
//         (lines 25-47)

// Add:
public const int SidBase = 0xD400;
public const int SidEnd  = 0xD41C;
```

**Step 3: Update `CompositeBusDevice.cs`**

Replace `_vsc` with `_sid`:

```csharp
// Change field declaration (line 9):
private readonly SidChip _sid;

// Change constructor (line 18):
_sid = new SidChip(enableSound);

// Change Dispose (line 39):
_sid.Dispose();

// Change Read method (lines 58-65):
// SID reads return ROM — do NOT route reads to SID.
// Remove: if (_vsc.OwnsAddress(address)) return _vsc.Read(address);

// Change Write method (lines 69-74):
// Add SID write routing BEFORE the ROM protection check:
if (_sid.OwnsAddress(address)) { _sid.Write(address, data); return; }
// Keep: if (address >= VgcConstants.RomBase) return;  // ROM protection for non-SID
```

The key insight: in `Read()`, we remove the VSC dispatch entirely — reads from $D400-$D41C fall through to `_ram[address]` which contains ROM data. In `Write()`, we intercept SID addresses before the ROM protection line.

**Step 4: Update vector table**

In `InitVectorTable` (line 53), change the VscBase vector to SidBase:

```csharp
WriteWord(VgcConstants.VectorTableBase + 0x08, VgcConstants.SidBase);
```

**Step 5: Update vector table test**

In `AvaloniaCompositeBusTests.cs`, update the vector table test (line 525-526):

```csharp
int sidBase = bus.Read(0x0208) | (bus.Read(0x0209) << 8);
Assert.AreEqual(VgcConstants.SidBase, sidBase);
```

**Step 6: Build and test**

Run: `dotnet build && dotnet test`
Expected: All tests pass

**Step 7: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/Hardware/VgcConstants.cs e6502UnitTests/AvaloniaCompositeBusTests.cs
git commit -m "feat: wire SID chip into bus, replace VSC routing"
```

---

### Task 4: Delete VirtualSoundController

Now that SidChip is wired in, remove the old VSC entirely.

**Files:**
- Delete: `e6502.Avalonia/Hardware/VirtualSoundController.cs`

**Step 1: Delete the file**

```bash
rm e6502.Avalonia/Hardware/VirtualSoundController.cs
```

**Step 2: Search for any remaining references**

Run: `grep -r "VirtualSoundController\|VscCmd\|VscCmdSound\|VscCmdVolume\|VscCmdEnvelope\|VscCmdWave\|VscActiveMask\|VscMasterVol" --include="*.cs" .`

Fix any remaining references. The BASIC assembly (ehbasic) still writes to $A100 for SOUND commands — those writes will now hit RAM (harmless, just no sound). The BASIC SOUND command is a separate concern and can be updated in a future task if desired.

**Step 3: Build and test**

Run: `dotnet build && dotnet test`
Expected: All pass

**Step 4: Commit**

```bash
git add -A
git commit -m "chore: remove VirtualSoundController (replaced by SID chip)"
```

---

### Task 5: Add VGC raster interrupt

Add a register to the VGC that enables a 60Hz interrupt. When enabled, the 60Hz render timer sets `cpu.IrqWaiting = true`.

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualGraphicsController.cs`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`
- Modify: `e6502UnitTests/AvaloniaVgcTests.cs`

**Step 1: Write the test**

```csharp
// In AvaloniaVgcTests.cs:
[TestMethod]
public void Vgc_RasterIrqEnable_DefaultOff()
{
    var vgc = new VirtualGraphicsController();
    Assert.IsFalse(vgc.IsRasterIrqEnabled);
}

[TestMethod]
public void Vgc_RasterIrqEnable_WriteToRegister()
{
    var vgc = new VirtualGraphicsController();
    vgc.Write(VgcConstants.RegIrqCtrl, 0x01);
    Assert.IsTrue(vgc.IsRasterIrqEnabled);
}

[TestMethod]
public void Vgc_RasterIrqEnable_DisableClears()
{
    var vgc = new VirtualGraphicsController();
    vgc.Write(VgcConstants.RegIrqCtrl, 0x01);
    vgc.Write(VgcConstants.RegIrqCtrl, 0x00);
    Assert.IsFalse(vgc.IsRasterIrqEnabled);
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "Vgc_RasterIrq"`
Expected: FAIL

**Step 3: Add the constant**

In `VgcConstants.cs`, find a free register slot. `RegCharOut` is at $A00E, `RegCharIn` at $A00F. The command block starts at $A010. Use $A01F (one past the current RegP13 at $A01E) — but first check if the VGC OwnsAddress range covers it. Currently it's `$A000-$A01E`. Extend to $A01F.

Actually, looking at the VGC register range: `VgcRegsEnd = 0xA01E`. We need to add one more register. Update:

```csharp
// In VgcConstants.cs:
public const int RegIrqCtrl = 0xA01F;  // bit 0 = raster IRQ enable
public const int VgcRegsEnd = 0xA01F;  // was 0xA01E
```

**Step 4: Implement in VirtualGraphicsController**

Add a property:

```csharp
public bool IsRasterIrqEnabled =>
    (_regs[VgcConstants.RegIrqCtrl - VgcConstants.VgcBase] & 0x01) != 0;
```

The Write method already handles all addresses in the VGC range, so writing to $A01F will store into `_regs` automatically.

**Step 5: Wire into MainWindow.axaml.cs**

In the 60Hz DispatcherTimer callback (line 50-55), add the IRQ trigger:

```csharp
DispatcherTimer.Run(() =>
{
    _bus.Vgc.IncrementFrameCounter();
    if (_bus.Vgc.IsRasterIrqEnabled)
        _cpu.IrqWaiting = true;
    _canvas.RequestRedraw();
    return true;
}, TimeSpan.FromMilliseconds(1000.0 / 60.0));
```

**Step 6: Run tests**

Run: `dotnet test`
Expected: All pass

**Step 7: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/VirtualGraphicsController.cs e6502.Avalonia/MainWindow.axaml.cs e6502UnitTests/AvaloniaVgcTests.cs
git commit -m "feat: add VGC raster interrupt (60Hz IRQ)"
```

---

### Task 6: Add programmable timer controller

A small timer controller in the free space at $BA40. Configurable frequency divisor, generates IRQs at the programmed rate.

**Files:**
- Create: `e6502.Avalonia/Hardware/VirtualTimerController.cs`
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Create: `e6502UnitTests/AvaloniaTimerTests.cs`

**Step 1: Write tests**

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaTimerTests
{
    [TestMethod]
    public void Timer_OwnsAddress_InRange()
    {
        var timer = new VirtualTimerController();
        Assert.IsTrue(timer.OwnsAddress((ushort)VgcConstants.TimerBase));
        Assert.IsTrue(timer.OwnsAddress((ushort)VgcConstants.TimerEnd));
        Assert.IsFalse(timer.OwnsAddress((ushort)(VgcConstants.TimerBase - 1)));
    }

    [TestMethod]
    public void Timer_WriteRegister_ReadBack()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x67);
        Assert.AreEqual(0x67, timer.Read((ushort)VgcConstants.TimerDivL));
    }

    [TestMethod]
    public void Timer_Disabled_NeverFires()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x10);
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        // Timer not enabled — Tick should not set IrqPending
        for (int i = 0; i < 100; i++) timer.Tick();
        Assert.IsFalse(timer.IrqPending);
    }

    [TestMethod]
    public void Timer_Enabled_FiresAfterDivisorTicks()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x0A);  // divisor = 10
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01);   // enable
        for (int i = 0; i < 9; i++) timer.Tick();
        Assert.IsFalse(timer.IrqPending);
        timer.Tick();  // 10th tick
        Assert.IsTrue(timer.IrqPending);
    }

    [TestMethod]
    public void Timer_Acknowledge_ClearsPending()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x01);
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01);
        timer.Tick();
        Assert.IsTrue(timer.IrqPending);
        timer.Read((ushort)VgcConstants.TimerStatus);  // acknowledge
        Assert.IsFalse(timer.IrqPending);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "AvaloniaTimerTests"`
Expected: FAIL

**Step 3: Add constants to `VgcConstants.cs`**

```csharp
// Timer controller registers ($BA40-$BA4F)
public const int TimerBase   = 0xBA40;
public const int TimerEnd    = 0xBA4F;
public const int TimerCtrl   = 0xBA40;  // bit 0 = enable
public const int TimerStatus = 0xBA41;  // bit 0 = IRQ pending (read clears)
public const int TimerDivL   = 0xBA42;  // divisor low byte
public const int TimerDivH   = 0xBA43;  // divisor high byte
```

Update `FreeBase` to `0xBA50` (was `0xBA40`).

**Step 4: Create `VirtualTimerController.cs`**

```csharp
namespace e6502.Avalonia.Hardware;

public sealed class VirtualTimerController
{
    private readonly byte[] _regs = new byte[VgcConstants.TimerEnd - VgcConstants.TimerBase + 1];
    private int _counter;
    public bool IrqPending { get; private set; }

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.TimerBase && address <= VgcConstants.TimerEnd;

    public byte Read(ushort address)
    {
        if (address == VgcConstants.TimerStatus)
        {
            byte val = (byte)(IrqPending ? 0x01 : 0x00);
            IrqPending = false;
            return val;
        }
        return _regs[address - VgcConstants.TimerBase];
    }

    public void Write(ushort address, byte data)
    {
        _regs[address - VgcConstants.TimerBase] = data;
        if (address == VgcConstants.TimerCtrl)
        {
            if ((data & 0x01) == 0)
            {
                _counter = 0;
                IrqPending = false;
            }
        }
    }

    /// <summary>Called at a fixed base rate (e.g., ~1MHz from CPU thread or a dedicated thread).</summary>
    public void Tick()
    {
        if ((_regs[0] & 0x01) == 0) return;  // not enabled

        int divisor = _regs[VgcConstants.TimerDivL - VgcConstants.TimerBase]
                    | (_regs[VgcConstants.TimerDivH - VgcConstants.TimerBase] << 8);
        if (divisor == 0) return;

        if (++_counter >= divisor)
        {
            _counter = 0;
            IrqPending = true;
        }
    }
}
```

**Step 5: Wire into CompositeBusDevice**

Add `_timer` field and route in Read/Write. Add to the dispatch chain before RAM:

```csharp
private readonly VirtualTimerController _timer = new();

// In Read:
if (_timer.OwnsAddress(address)) return _timer.Read(address);

// In Write:
if (_timer.OwnsAddress(address)) { _timer.Write(address, data); return; }
```

**Step 6: Wire timer ticking and IRQ into MainWindow**

The timer needs to be ticked at a regular rate. The simplest approach: tick it from the CPU thread. Add a cycle counter in the CPU loop. Every N cycles (matching the timer's base rate), call `_bus.Timer.Tick()`. Or: use a dedicated timer thread.

Simpler approach for now: tick the timer from the 60Hz render loop as well, but with a configurable sub-tick count. Actually, the simplest correct approach is to expose the timer from CompositeBusDevice and tick it from the CPU execution loop:

In `CompositeBusDevice`:
```csharp
public VirtualTimerController Timer => _timer;
```

In `MainWindow.axaml.cs`, modify the CPU loop:
```csharp
var cpuThread = new Thread(() =>
{
    int timerAccum = 0;
    while (_running)
    {
        int cycles = _cpu.ClocksForNext();
        _cpu.ExecuteNext();
        timerAccum += cycles;
        if (timerAccum >= 100)  // tick timer every ~100 cycles
        {
            _bus.Timer.Tick();
            if (_bus.Timer.IrqPending)
                _cpu.IrqWaiting = true;
            timerAccum -= 100;
        }
    }
}) { IsBackground = true, Name = "CpuThread" };
```

Note: this means the timer divisor values are in units of ~100 CPU cycles. For 50Hz at ~1MHz: 1000000/50 = 20000 cycles, divisor = 20000/100 = 200. For 60Hz: divisor ≈ 167.

**Step 7: Add vector table entry**

In `InitVectorTable`:
```csharp
WriteWord(VgcConstants.VectorTableBase + 0x0E, VgcConstants.TimerBase);
```

**Step 8: Run tests**

Run: `dotnet test`
Expected: All pass

**Step 9: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualTimerController.cs e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/MainWindow.axaml.cs e6502UnitTests/AvaloniaTimerTests.cs
git commit -m "feat: add programmable timer controller with IRQ support"
```

---

### Task 7: Create PSID file parser

Parse PSID v1/v2+ file headers and extract the payload.

**Files:**
- Create: `e6502.Avalonia/Hardware/SidFileParser.cs`
- Create: `e6502UnitTests/SidFileParserTests.cs`

**Step 1: Write tests**

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidFileParserTests
{
    // PSID v2 header: 124 bytes, big-endian
    private static byte[] MakeMinimalPsid(ushort loadAddr = 0x1000, ushort initAddr = 0x1000, ushort playAddr = 0x1003)
    {
        var header = new byte[124 + 3];  // header + 3 bytes of payload (JMP init)

        // Magic: "PSID"
        header[0] = (byte)'P'; header[1] = (byte)'S'; header[2] = (byte)'I'; header[3] = (byte)'D';
        // Version: 2 (big-endian)
        header[4] = 0x00; header[5] = 0x02;
        // Data offset: 0x007C (124)
        header[6] = 0x00; header[7] = 0x7C;
        // Load address: 0 means read from first 2 bytes of data
        header[8] = (byte)(loadAddr >> 8); header[9] = (byte)(loadAddr & 0xFF);
        // Init address
        header[10] = (byte)(initAddr >> 8); header[11] = (byte)(initAddr & 0xFF);
        // Play address
        header[12] = (byte)(playAddr >> 8); header[13] = (byte)(playAddr & 0xFF);
        // Songs: 1
        header[14] = 0x00; header[15] = 0x01;
        // Start song: 1
        header[16] = 0x00; header[17] = 0x01;
        // Speed: 0 (VBI)
        header[18] = 0; header[19] = 0; header[20] = 0; header[21] = 0;

        // Title at offset 22 (32 bytes), Author at 54, Copyright at 86
        // Leave as zeros

        // Payload: 3 bytes (NOP NOP NOP)
        header[124] = 0xEA; header[125] = 0xEA; header[126] = 0xEA;

        return header;
    }

    [TestMethod]
    public void Parse_ValidPsid_ReturnsHeader()
    {
        var data = MakeMinimalPsid();
        var result = SidFileParser.Parse(data);
        Assert.IsTrue(result.IsValid);
        Assert.AreEqual("PSID", result.Magic);
        Assert.AreEqual(2, result.Version);
    }

    [TestMethod]
    public void Parse_LoadAddress_Correct()
    {
        var data = MakeMinimalPsid(loadAddr: 0x1000, initAddr: 0x1000, playAddr: 0x1003);
        var result = SidFileParser.Parse(data);
        Assert.AreEqual(0x1000, result.LoadAddress);
        Assert.AreEqual(0x1000, result.InitAddress);
        Assert.AreEqual(0x1003, result.PlayAddress);
    }

    [TestMethod]
    public void Parse_Payload_ExtractedCorrectly()
    {
        var data = MakeMinimalPsid();
        var result = SidFileParser.Parse(data);
        Assert.AreEqual(3, result.Payload.Length);
        Assert.AreEqual(0xEA, result.Payload[0]);  // NOP
    }

    [TestMethod]
    public void Parse_InvalidMagic_ReturnsInvalid()
    {
        var data = MakeMinimalPsid();
        data[0] = (byte)'X';
        var result = SidFileParser.Parse(data);
        Assert.IsFalse(result.IsValid);
    }

    [TestMethod]
    public void Parse_SpeedFlag_Bit0_IsCiaTiming()
    {
        var data = MakeMinimalPsid();
        data[21] = 0x01;  // speed bit 0 = CIA timing for song 1
        var result = SidFileParser.Parse(data);
        Assert.IsTrue(result.UsesCiaTiming);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "SidFileParserTests"`
Expected: FAIL

**Step 3: Implement `SidFileParser.cs`**

```csharp
namespace e6502.Avalonia.Hardware;

public static class SidFileParser
{
    public static SidFileInfo Parse(byte[] data)
    {
        if (data.Length < 124)
            return SidFileInfo.Invalid;

        string magic = $"{(char)data[0]}{(char)data[1]}{(char)data[2]}{(char)data[3]}";
        if (magic != "PSID" && magic != "RSID")
            return SidFileInfo.Invalid;

        int version = ReadBE16(data, 4);
        int dataOffset = ReadBE16(data, 6);
        int loadAddress = ReadBE16(data, 8);
        int initAddress = ReadBE16(data, 10);
        int playAddress = ReadBE16(data, 12);
        int songs = ReadBE16(data, 14);
        int startSong = ReadBE16(data, 16);
        uint speed = (uint)(data[18] << 24 | data[19] << 16 | data[20] << 8 | data[21]);

        string title = ReadString(data, 22, 32);
        string author = ReadString(data, 54, 32);
        string copyright = ReadString(data, 86, 32);

        if (dataOffset > data.Length)
            return SidFileInfo.Invalid;

        byte[] payload = new byte[data.Length - dataOffset];
        Array.Copy(data, dataOffset, payload, 0, payload.Length);

        // If loadAddress is 0, first 2 bytes of payload are the load address (little-endian)
        if (loadAddress == 0 && payload.Length >= 2)
        {
            loadAddress = payload[0] | (payload[1] << 8);
            payload = payload[2..];
        }

        return new SidFileInfo
        {
            IsValid = true,
            Magic = magic,
            Version = version,
            LoadAddress = (ushort)loadAddress,
            InitAddress = (ushort)initAddress,
            PlayAddress = (ushort)playAddress,
            Songs = songs,
            StartSong = startSong,
            Speed = speed,
            UsesCiaTiming = (speed & 1) != 0,
            Title = title,
            Author = author,
            Copyright = copyright,
            Payload = payload
        };
    }

    private static int ReadBE16(byte[] data, int offset) =>
        (data[offset] << 8) | data[offset + 1];

    private static string ReadString(byte[] data, int offset, int maxLen)
    {
        int end = offset;
        while (end < offset + maxLen && end < data.Length && data[end] != 0) end++;
        return System.Text.Encoding.ASCII.GetString(data, offset, end - offset);
    }
}

public record struct SidFileInfo
{
    public bool IsValid;
    public string Magic;
    public int Version;
    public ushort LoadAddress;
    public ushort InitAddress;
    public ushort PlayAddress;
    public int Songs;
    public int StartSong;
    public uint Speed;
    public bool UsesCiaTiming;
    public string Title;
    public string Author;
    public string Copyright;
    public byte[] Payload;

    public static readonly SidFileInfo Invalid = new() { IsValid = false, Payload = [] };
}
```

**Step 4: Run tests**

Run: `dotnet test --filter "SidFileParserTests"`
Expected: All pass

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/SidFileParser.cs e6502UnitTests/SidFileParserTests.cs
git commit -m "feat: add PSID file parser"
```

---

### Task 8: Add SID playback MCP commands

Add `sid_play`, `sid_stop`, and `sid_info` commands to the TCP server and MCP tool definitions.

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`
- Modify: `e6502.MCP/EmulatorTools.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (expose SidChip for direct access)

**Step 1: Expose SidChip from CompositeBusDevice**

```csharp
// Add property:
public SidChip Sid => _sid;
```

**Step 2: Add TCP commands to `EmulatorTcpServer.cs`**

In the `ProcessRequest` switch (line 95), add:

```csharp
"sid_play" => CmdSidPlay(req),
"sid_stop" => CmdSidStop(),
"sid_info" => CmdSidInfo(req),
```

Implement `CmdSidPlay`:
- Read the file path from the request
- Read the .sid file from disk via `File.ReadAllBytes`
- Parse with `SidFileParser.Parse`
- Check that load address fits in RAM ($0280-$9FFF)
- Copy payload into bus RAM at load address
- Inject an IRQ trampoline at a safe location (e.g., $03E0 — inside vector table space that's available)
- Set up IRQ vector ($FFFE-$FFFF) to point to the trampoline
- Call init: set CPU A register to song-1, JSR to init address (by injecting a small bootstrap and executing it)
- Enable the timer at 50Hz or 60Hz based on speed flags

The trampoline (at $03E0):
```
PHA        ; save A
TXA
PHA        ; save X
TYA
PHA        ; save Y
JSR $xxxx  ; call play address
; acknowledge timer IRQ (read timer status register)
LDA $BA41
PLA
TAY        ; restore Y
PLA
TAX        ; restore X
PLA        ; restore A
RTI
```

That's about 20 bytes of 6502 code.

Implement `CmdSidStop`:
- Disable the timer
- Gate off all 3 SID voices (write 0 to control registers $D404, $D40B, $D412)

Implement `CmdSidInfo`:
- Read the file, parse with SidFileParser, return the metadata as JSON (title, author, copyright, songs, load/init/play addresses)

**Step 3: Add MCP tool definitions to `EmulatorTools.cs`**

```csharp
[McpServerTool, Description("Load and play a SID music file. Parses PSID header, loads into memory, sets up IRQ playback.")]
public static async Task<string> SidPlay(
    EmulatorClient client,
    [Description("Path to the .sid file on the host filesystem")] string path,
    [Description("Song number to play (1-based, default 1)")] int song = 1)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "sid_play",
        ["path"] = path,
        ["song"] = song
    });
    return result.ToJsonString();
}

[McpServerTool, Description("Stop SID playback and silence all voices.")]
public static async Task<string> SidStop(EmulatorClient client)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "sid_stop"
    });
    return result.ToJsonString();
}

[McpServerTool, Description("Read metadata from a SID file (title, author, copyright, songs, addresses).")]
public static async Task<string> SidInfo(
    EmulatorClient client,
    [Description("Path to the .sid file")] string path)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "sid_info",
        ["path"] = path
    });
    return result.ToJsonString();
}
```

**Step 4: Build and test manually**

Run: `dotnet build`
Expected: Success

Test manually by running the emulator and using a real .sid file if available, or by using the MCP tools.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Ipc/EmulatorTcpServer.cs e6502.MCP/EmulatorTools.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs
git commit -m "feat: add SID play/stop/info MCP commands and TCP handlers"
```

---

### Task 9: Integration test with a real SID file

Download a simple test SID file and verify end-to-end playback.

**Step 1: Get a test SID file**

Download a simple SID tune from the HVSC (High Voltage SID Collection). A good candidate is a simple one-voice tune that loads at $1000.

**Step 2: Test via MCP tools**

Use `sid_info` to verify the file parses correctly. Use `sid_play` to load and play it. Verify audio output. Use `sid_stop` to stop.

**Step 3: Test via direct register pokes**

Use the `poke` MCP tool to write directly to SID registers and verify sound:
```
poke $D418 $0F    # master volume max
poke $D400 $37    # voice 1 freq lo (440Hz)
poke $D401 $1C    # voice 1 freq hi
poke $D405 $09    # AD: attack=0, decay=9
poke $D406 $00    # SR: sustain=0, release=0
poke $D404 $11    # gate on + triangle wave
```

**Step 4: Commit any fixes**

```bash
git add -A
git commit -m "test: verify SID playback end-to-end"
```

---

## Reference: SID Register Map

```
Offset  C64 Addr  Description
------  --------  -----------
$00     $D400     Voice 1 frequency lo
$01     $D401     Voice 1 frequency hi
$02     $D402     Voice 1 pulse width lo
$03     $D403     Voice 1 pulse width hi (bits 0-3)
$04     $D404     Voice 1 control (gate, sync, ring, test, waveform)
$05     $D405     Voice 1 attack/decay
$06     $D406     Voice 1 sustain/release
$07     $D407     Voice 2 frequency lo
$08     $D408     Voice 2 frequency hi
$09     $D409     Voice 2 pulse width lo
$0A     $D40A     Voice 2 pulse width hi
$0B     $D40B     Voice 2 control
$0C     $D40C     Voice 2 attack/decay
$0D     $D40D     Voice 2 sustain/release
$0E     $D40E     Voice 3 frequency lo
$0F     $D40F     Voice 3 frequency hi
$10     $D410     Voice 3 pulse width lo
$11     $D411     Voice 3 pulse width hi
$12     $D412     Voice 3 control
$13     $D413     Voice 3 attack/decay
$14     $D414     Voice 3 sustain/release
$15     $D415     Filter cutoff lo (bits 0-2)
$16     $D416     Filter cutoff hi
$17     $D417     Filter resonance/routing
$18     $D418     Mode/volume
$19     $D419     Paddle X (read-only)
$1A     $D41A     Paddle Y (read-only)
$1B     $D41B     Oscillator 3 output (read-only)
$1C     $D41C     Envelope 3 output (read-only)
```

## Reference: Voice Control Register ($D404/$D40B/$D412)

```
Bit 0: Gate (1=start/sustain, 0=release)
Bit 1: Sync (hard sync with previous voice)
Bit 2: Ring modulation
Bit 3: Test bit (resets oscillator)
Bit 4: Triangle waveform
Bit 5: Sawtooth waveform
Bit 6: Pulse waveform
Bit 7: Noise waveform
```
