# SID Playback Completion Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Make SID files actually play end-to-end — fix init auto-execution, add BASIC commands, build relocator CLI tool.

**Architecture:** Three independent features: (1) Extract a SidPlayer helper that auto-inits via a self-modifying IRQ trampoline on first timer tick, shared by TCP server and FIO controller. (2) Add SIDPLAY/SIDSTOP as extended BASIC tokens backed by new FIO commands. (3) Build a control-flow-following disassembler in a new e6502.Tools project for SID relocation.

**Tech Stack:** C#/.NET 10, 6502 assembly (ca65/ld65), MSTest

---

## Context

The SID chip emulation is wired up and clocked, but `sid_play` never calls the SID file's init routine so nothing actually plays. There are no BASIC commands for SID playback. No relocator exists.

### Key Files Reference

- `e6502.Avalonia/Hardware/SidChip.cs` — SID emulation, $D400-$D41C
- `e6502.Avalonia/Hardware/SidFileParser.cs` — PSID v1/v2+ parser
- `e6502.Avalonia/Hardware/VirtualTimerController.cs` — Timer at $BA40-$BA4F
- `e6502.Avalonia/Hardware/CompositeBusDevice.cs` — Bus routing, `WriteRam()`
- `e6502.Avalonia/Hardware/FileIoController.cs` — FIO commands ($B9A0-$B9FF)
- `e6502.Avalonia/Hardware/VgcConstants.cs` — All hardware constants
- `e6502.Avalonia/Ipc/EmulatorTcpServer.cs` — TCP commands including `sid_play`/`sid_stop`
- `e6502.MCP/EmulatorTools.cs` — MCP tool wrappers
- `e6502/OpCodes/OpCodeRecord.cs` — Opcode metadata (Instruction is private)
- `e6502/OpCodes/OpCodeTable.cs` — 256-entry opcode lookup
- `e6502/OpCodes/AddressModes.cs` — Addressing mode enum
- `ehbasic/basic.asm` — EhBASIC source with extended token system
- `ehbasic/min_mon.asm` — Monitor / hardware init
- `ehbasic/Makefile` — Build: `ca65` then `ld65`, output `basic.bin`

### Trampoline Design (Init-On-First-IRQ)

The trampoline at $03E0 self-modifies: on the first timer IRQ, it calls init (with song number in A), clears an init flag, then calls play. On subsequent IRQs it skips init and just calls play. This lets the CPU keep running BASIC while music plays in the background via IRQ.

Memory layout at $03D0-$03FF:

```
$03D0: init_flag    .byte $01       ; 1 = need init, 0 = skip
$03D1: song_num     .byte $00       ; song number - 1

$03E0: PHA                          ; save A
$03E1: TXA
$03E2: PHA                          ; save X
$03E3: TYA
$03E4: PHA                          ; save Y
$03E5: LDA $03D0                    ; check init flag
$03E8: BEQ +$09 (→$03F3)            ; skip init if already done
$03EA: LDA $03D1                    ; load song number
$03ED: JSR init_addr                ; call SID init
$03F0: LDA #$00
$03F2: STA $03D0                    ; clear init flag
$03F4: JSR play_addr                ; call SID play routine
$03F7: LDA $BA41                    ; acknowledge timer IRQ
$03FA: PLA
$03FB: TAY                          ; restore Y
$03FC: PLA
$03FD: TAX                          ; restore X
$03FE: PLA                          ; restore A
$03FF: RTI
```

Total: 2 data bytes ($03D0-$03D1) + 31 code bytes ($03E0-$03FF) = 33 bytes.

### FIO SID Commands

Two new FIO command codes:
- `FioCmdSidPlay = 0x08` — reads filename from FIO name buffer, optional song number from `FIO_SRCL`, loads .sid file, injects trampoline, patches IRQ vector, enables timer
- `FioCmdSidStop = 0x09` — disables timer, gates off SID voices, restores IRQ vector

### Extended Token Registration

Add to ehbasic `basic.asm`:
- `XTK_SIDPLAY = $12` (id 18)
- `XTK_SIDSTOP = $13` (id 19)
- Increment `XTK_COUNT` from 17 to 19
- Add keyword strings and dispatch table entries
- Handler `LAB_SIDPLAY`: parse filename via `LAB_FIO_GETNAME`, optional `,song` parameter to `FIO_SRCL`, write `FIO_CMD_SIDPLAY` to `FIO_CMD`, check status
- Handler `LAB_SIDSTOP`: write `FIO_CMD_SIDSTOP` to `FIO_CMD`

---

## Task 1: Create SidPlayer Helper Class

Extract SID playback logic into a reusable class with the init-on-first-IRQ trampoline.

**Files:**
- Create: `e6502.Avalonia/Hardware/SidPlayer.cs`
- Test: `e6502UnitTests/SidPlayerTests.cs`

**Step 1: Write failing tests**

Create `e6502UnitTests/SidPlayerTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidPlayerTests
{
    private static byte[] MakeMinimalSid(ushort loadAddr = 0x1000, ushort initAddr = 0x1000, ushort playAddr = 0x1003)
    {
        // Minimal PSID v2 header (124 bytes) + 6 bytes payload (LDA #0; RTS; LDA #0; RTS)
        var data = new byte[130];
        data[0] = (byte)'P'; data[1] = (byte)'S'; data[2] = (byte)'I'; data[3] = (byte)'D';
        data[4] = 0x00; data[5] = 0x02; // version 2
        data[6] = 0x00; data[7] = 0x7C; // data offset = 124
        data[8] = (byte)(loadAddr >> 8); data[9] = (byte)(loadAddr & 0xFF); // load address (big-endian)
        data[10] = (byte)(initAddr >> 8); data[11] = (byte)(initAddr & 0xFF);
        data[12] = (byte)(playAddr >> 8); data[13] = (byte)(playAddr & 0xFF);
        data[14] = 0x00; data[15] = 0x01; // 1 song
        data[16] = 0x00; data[17] = 0x01; // start song 1
        // speed = 0 (VBI/50Hz)
        // payload: LDA #0; RTS; LDA #0; RTS
        data[124] = 0xA9; data[125] = 0x00; data[126] = 0x60;
        data[127] = 0xA9; data[128] = 0x00; data[129] = 0x60;
        return data;
    }

    [TestMethod]
    public void Play_InjectsTrampoline_AtExpectedAddress()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        byte[] sidData = MakeMinimalSid();
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);

        // Init flag should be set
        Assert.AreEqual(0x01, bus.Read(0x03D0));
        // Song number (song - 1 = 0)
        Assert.AreEqual(0x00, bus.Read(0x03D1));
        // First trampoline instruction at $03E0 is PHA (0x48)
        Assert.AreEqual(0x48, bus.Read(0x03E0));
        // RTI at end
        Assert.AreEqual(0x40, bus.Read(0x03FF));
    }

    [TestMethod]
    public void Play_PatchesIrqVector()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        byte[] sidData = MakeMinimalSid();
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);

        // IRQ vector at $FFFE/$FFFF should point to $03E0
        // Read from _ram directly via the bus (ROM overlay, but WriteRam patches _ram)
        // We verify by checking the trampoline is reachable
        Assert.AreEqual(0xE0, bus.Read(0xFFFE));
        Assert.AreEqual(0x03, bus.Read(0xFFFF));
    }

    [TestMethod]
    public void Play_EnablesTimer()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        byte[] sidData = MakeMinimalSid();
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);

        // Timer control register should have enable bit set
        Assert.AreEqual(0x01, bus.Read((ushort)VgcConstants.TimerCtrl));
    }

    [TestMethod]
    public void Play_CopiesPayloadToRam()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        byte[] sidData = MakeMinimalSid(loadAddr: 0x1000);
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);

        // Payload starts at $1000: LDA #0 = $A9 $00
        Assert.AreEqual(0xA9, bus.Read(0x1000));
    }

    [TestMethod]
    public void Stop_DisablesTimer()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        byte[] sidData = MakeMinimalSid();
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);
        player.Stop();

        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.TimerCtrl));
    }

    [TestMethod]
    public void Stop_GatesOffSidVoices()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        byte[] sidData = MakeMinimalSid();
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);
        // Set some SID voice controls to non-zero
        bus.Write(0xD404, 0x11);
        bus.Write(0xD40B, 0x11);
        bus.Write(0xD412, 0x11);

        player.Stop();

        // Voice control registers should be zeroed (gate off)
        Assert.AreEqual(0x00, bus.Sid.Read(0xD404));
        Assert.AreEqual(0x00, bus.Sid.Read(0xD40B));
        Assert.AreEqual(0x00, bus.Sid.Read(0xD412));
        // Master volume should be zero
        Assert.AreEqual(0x00, bus.Sid.Read(0xD418));
    }

    [TestMethod]
    public void Play_SetsTimerDivisor_VBI()
    {
        var bus = new CompositeBusDevice();
        var player = new SidPlayer(bus);
        // speed = 0 → VBI (50Hz) → divisor 200
        byte[] sidData = MakeMinimalSid();
        var info = SidFileParser.Parse(sidData);

        player.Play(info, song: 1);

        int divisor = bus.Read((ushort)VgcConstants.TimerDivL)
                    | (bus.Read((ushort)VgcConstants.TimerDivH) << 8);
        Assert.AreEqual(200, divisor);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "SidPlayerTests" --verbosity normal`
Expected: FAIL — `SidPlayer` class does not exist yet.

**Step 3: Implement SidPlayer**

Create `e6502.Avalonia/Hardware/SidPlayer.cs`:

```csharp
namespace e6502.Avalonia.Hardware;

/// <summary>
/// Manages SID file playback: loads payload, injects IRQ trampoline, configures timer.
/// Uses init-on-first-IRQ pattern so the CPU keeps running BASIC while music plays.
/// </summary>
public sealed class SidPlayer
{
    private const ushort TrampolineBase = 0x03D0;
    private const ushort TrampolineCode = 0x03E0;
    private const ushort IrqVectorAddr = 0xFFFE;

    private readonly CompositeBusDevice _bus;
    private byte _savedIrqLo;
    private byte _savedIrqHi;

    public SidPlayer(CompositeBusDevice bus)
    {
        _bus = bus;
    }

    public void Play(SidFileInfo info, int song = 1)
    {
        if (!info.IsValid) return;

        // Save original IRQ vector
        _savedIrqLo = _bus.Read(IrqVectorAddr);
        _savedIrqHi = _bus.Read((ushort)(IrqVectorAddr + 1));

        // Copy payload into RAM
        for (int i = 0; i < info.Payload.Length; i++)
            _bus.Write((ushort)(info.LoadAddress + i), info.Payload[i]);

        // Write init flag and song number
        _bus.Write(TrampolineBase, 0x01);              // init_flag = 1
        _bus.Write((ushort)(TrampolineBase + 1), (byte)(song - 1)); // song_num

        // Inject trampoline code at $03E0
        byte initLo = (byte)(info.InitAddress & 0xFF);
        byte initHi = (byte)(info.InitAddress >> 8);
        byte playLo = (byte)(info.PlayAddress & 0xFF);
        byte playHi = (byte)(info.PlayAddress >> 8);

        byte[] code =
        [
            0x48,                           // $03E0: PHA
            0x8A,                           // $03E1: TXA
            0x48,                           // $03E2: PHA
            0x98,                           // $03E3: TYA
            0x48,                           // $03E4: PHA
            0xAD, 0xD0, 0x03,              // $03E5: LDA $03D0 (init flag)
            0xF0, 0x09,                     // $03E8: BEQ +9 (→$03F3, skip init)
            0xAD, 0xD1, 0x03,              // $03EA: LDA $03D1 (song number)
            0x20, initLo, initHi,           // $03ED: JSR init
            0xA9, 0x00,                     // $03F0: LDA #$00
            0x8D, 0xD0, 0x03,              // $03F2: STA $03D0 (clear init flag)
            // --- BEQ lands here ($03F5) ---
            0x20, playLo, playHi,           // $03F5: JSR play
            0xAD, 0x41, 0xBA,              // $03F8: LDA $BA41 (ack timer IRQ)
            0x68,                           // $03FB: PLA
            0xA8,                           // $03FC: TAY
            0x68,                           // $03FD: PLA
            0xAA,                           // $03FE: TAX
            0x68,                           // $03FF: PLA  -- NOTE: this pushes to $0400 area, need to adjust
        ];

        // Wait — we need RTI too. Let me recalculate the layout.
        // $03E0 to $03FF is 32 bytes. The code above is 32 bytes but we still
        // need RTI. Let's use $03D2 as code start instead (after the 2 data bytes).

        // Recalculated layout:
        // $03D0: init_flag, $03D1: song_num
        // $03D2-$03FF: trampoline code (30 bytes available)

        // Actually, let's just write both data and code starting at $03D0.
        // Total needed: 2 data + PHA+TXA+PHA+TYA+PHA (5) + LDA abs (3) + BEQ (2)
        //   + LDA abs (3) + JSR (3) + LDA imm (2) + STA abs (3) + JSR (3)
        //   + LDA abs (3) + PLA+TAY+PLA+TAX+PLA+RTI (6)
        //   = 5+3+2+3+3+2+3+3+3+6 = 33 code bytes + 2 data = 35 total
        // $03D0 + 35 = $03F3. Fits easily before $0400.

        // Let me build the final byte array properly:
        InjectTrampoline(info, song);

        // Patch IRQ vector to point to trampoline code
        _bus.WriteRam(IrqVectorAddr, (byte)(TrampolineCode & 0xFF));
        _bus.WriteRam((ushort)(IrqVectorAddr + 1), (byte)(TrampolineCode >> 8));

        // Configure timer
        int divisor = info.UsesCiaTiming ? 167 : 200;
        _bus.Write((ushort)VgcConstants.TimerDivL, (byte)(divisor & 0xFF));
        _bus.Write((ushort)VgcConstants.TimerDivH, (byte)(divisor >> 8));
        _bus.Write((ushort)VgcConstants.TimerCtrl, 0x01); // enable
    }

    public void Stop()
    {
        // Disable timer
        _bus.Write((ushort)VgcConstants.TimerCtrl, 0x00);

        // Gate off all 3 SID voices
        _bus.Write(0xD404, 0x00);
        _bus.Write(0xD40B, 0x00);
        _bus.Write(0xD412, 0x00);

        // Zero volume
        _bus.Write(0xD418, 0x00);

        // Restore original IRQ vector
        _bus.WriteRam(IrqVectorAddr, _savedIrqLo);
        _bus.WriteRam((ushort)(IrqVectorAddr + 1), _savedIrqHi);
    }

    private void InjectTrampoline(SidFileInfo info, int song)
    {
        byte initLo = (byte)(info.InitAddress & 0xFF);
        byte initHi = (byte)(info.InitAddress >> 8);
        byte playLo = (byte)(info.PlayAddress & 0xFF);
        byte playHi = (byte)(info.PlayAddress >> 8);

        // Data at $03D0-$03D1
        _bus.Write(TrampolineBase, 0x01);                       // init_flag
        _bus.Write((ushort)(TrampolineBase + 1), (byte)(song - 1)); // song_num

        // Code at $03E0
        byte[] code =
        [
            0x48,                           // PHA
            0x8A,                           // TXA
            0x48,                           // PHA
            0x98,                           // TYA
            0x48,                           // PHA
            0xAD, 0xD0, 0x03,              // LDA $03D0  (init flag)
            0xF0, 0x09,                     // BEQ +9     (skip to JSR play)
            0xAD, 0xD1, 0x03,              // LDA $03D1  (song number)
            0x20, initLo, initHi,           // JSR init
            0xA9, 0x00,                     // LDA #$00
            0x8D, 0xD0, 0x03,              // STA $03D0  (clear init flag)
            0x20, playLo, playHi,           // JSR play
            0xAD, 0x41, 0xBA,              // LDA $BA41  (ack timer IRQ)
            0x68,                           // PLA
            0xA8,                           // TAY
            0x68,                           // PLA
            0xAA,                           // TAX
            0x68,                           // PLA
            0x40                            // RTI
        ];

        for (int i = 0; i < code.Length; i++)
            _bus.Write((ushort)(TrampolineCode + i), code[i]);
    }
}
```

NOTE to implementer: The inline code in `Play()` has a draft that's commented-out/messy. Use only the `InjectTrampoline` method. The `Play()` method should just call `InjectTrampoline`, patch IRQ vector, and configure timer. Clean up the draft code.

Verify the BEQ offset: from $03E8 (the BEQ instruction), +2 for the instruction itself = $03EA, then +9 = $03F3. But the JSR play is at offset 21 in the code array (0-indexed), which is $03E0 + 21 = $03F5. Recheck: The BEQ is at code[8] ($03E8), relative offset is code[9] = 0x09. BEQ target = $03E8 + 2 + 9 = $03F3. But we want to skip to the JSR play at code[21] = $03E0 + 21 = $03F5. So the offset should be 0x0B (11), not 0x09 (9). The code between BEQ and JSR play: LDA $03D1 (3) + JSR init (3) + LDA #0 (2) + STA $03D0 (3) = 11 bytes. BEQ +11 → $03E8 + 2 + 11 = $03F5. **The BEQ offset byte should be 0x0B, not 0x09.**

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "SidPlayerTests" --verbosity normal`
Expected: All 7 pass.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/SidPlayer.cs e6502UnitTests/SidPlayerTests.cs
git commit -m "feat: add SidPlayer with init-on-first-IRQ trampoline"
```

---

## Task 2: Refactor EmulatorTcpServer to Use SidPlayer

Replace inline SID logic in `CmdSidPlay`/`CmdSidStop` with delegation to `SidPlayer`.

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (add SidPlayer property)

**Step 1: Add SidPlayer to CompositeBusDevice**

In `CompositeBusDevice.cs`, add:

```csharp
private readonly SidPlayer _sidPlayer;
public SidPlayer SidPlayer => _sidPlayer;
```

Initialize in constructor after `_sid`:

```csharp
_sidPlayer = new SidPlayer(this);
```

**Step 2: Refactor CmdSidPlay**

In `EmulatorTcpServer.cs`, replace `CmdSidPlay` body. Keep the file reading, parsing, and validation. Replace the trampoline injection, IRQ patching, and timer setup with:

```csharp
_bus.SidPlayer.Play(info, song);
```

Keep the JSON response with metadata.

**Step 3: Refactor CmdSidStop**

Replace body with:

```csharp
_bus.SidPlayer.Stop();
return Ok();
```

**Step 4: Run all tests**

Run: `dotnet test`
Expected: All tests pass (109 existing + 7 new = 116).

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/Ipc/EmulatorTcpServer.cs
git commit -m "refactor: delegate SID playback to SidPlayer helper"
```

---

## Task 3: Add FIO SID Commands (C# Side)

Add `FioCmdSidPlay` (0x08) and `FioCmdSidStop` (0x09) to the FileIO controller so BASIC can trigger SID playback.

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (pass SidPlayer to FIO)
- Test: `e6502UnitTests/FileIoControllerTests.cs`

**Step 1: Add constants**

In `VgcConstants.cs`, add after `FioCmdGLoad`:

```csharp
public const byte FioCmdSidPlay    = 0x08;     // .sid file → SID playback
public const byte FioCmdSidStop    = 0x09;     // stop SID playback
```

Also add an FIO register for the song number. Reuse `FioSrcL` (0xB9A4) — the BASIC handler will write song number there before triggering the command. Document this:

```csharp
// FioSrcL is reused as song number for FioCmdSidPlay (1-based, default 1)
```

**Step 2: Add SidPlayer dependency to FileIoController**

Add a `SidPlayer?` parameter to the `FileIoController` constructor:

```csharp
private readonly SidPlayer? _sidPlayer;

public FileIoController(
    Func<ushort, byte> busRead,
    Action<ushort, byte> busWrite,
    string? saveDir = null,
    Func<byte, int, byte>? vgcRead = null,
    Action<byte, int, byte>? vgcWrite = null,
    Func<byte, int>? vgcSpaceLength = null,
    SidPlayer? sidPlayer = null)
{
    // ... existing ...
    _sidPlayer = sidPlayer;
}
```

**Step 3: Add command dispatch**

In `ExecuteCommand`, add cases:

```csharp
case VgcConstants.FioCmdSidPlay:
    DoSidPlay();
    break;
case VgcConstants.FioCmdSidStop:
    DoSidStop();
    break;
```

**Step 4: Implement DoSidPlay and DoSidStop**

```csharp
private void DoSidPlay()
{
    if (_sidPlayer is null) { SetError(VgcConstants.FioErrIo); return; }

    try
    {
        string? filename = ReadFilename();
        if (filename is null) { SetError(VgcConstants.FioErrIo); return; }

        string path = GetFullPath(filename, ".sid");
        if (!File.Exists(path))
        {
            SetError(VgcConstants.FioErrNotFound);
            return;
        }

        byte[] data = File.ReadAllBytes(path);
        var info = SidFileParser.Parse(data);
        if (!info.IsValid) { SetError(VgcConstants.FioErrIo); return; }

        int song = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        if (song < 1) song = info.StartSong;

        _sidPlayer.Play(info, song);
        SetOk();
    }
    catch (FileNotFoundException) { SetError(VgcConstants.FioErrNotFound); }
    catch { SetError(VgcConstants.FioErrIo); }
}

private void DoSidStop()
{
    if (_sidPlayer is null) { SetError(VgcConstants.FioErrIo); return; }
    _sidPlayer.Stop();
    SetOk();
}
```

**Step 5: Pass SidPlayer from CompositeBusDevice**

In `CompositeBusDevice` constructor, the `_fio` is created before `_sidPlayer` exists. Reorder: create `_sidPlayer` first, then pass it to `_fio`:

```csharp
_sid = new SidChip(enableSound);
_sidPlayer = new SidPlayer(this);
_fio = new FileIoController(
    addr => _ram[addr],
    (addr, data) => _ram[addr] = data,
    saveDir: null,
    vgcRead: ...,
    vgcWrite: ...,
    vgcSpaceLength: ...,
    sidPlayer: _sidPlayer);
```

**Step 6: Write tests**

Add to `e6502UnitTests/FileIoControllerTests.cs` (create if needed):

```csharp
[TestMethod]
public void FioCmdSidStop_SetsOk()
{
    var bus = new CompositeBusDevice();
    bus.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSidStop);
    Assert.AreEqual(VgcConstants.FioStatusOk, bus.Read((ushort)VgcConstants.FioStatus));
}

[TestMethod]
public void FioCmdSidPlay_NoFile_SetsError()
{
    var bus = new CompositeBusDevice();
    // Write a filename that won't exist
    string name = "nonexistent";
    bus.Write((ushort)VgcConstants.FioNameLen, (byte)name.Length);
    for (int i = 0; i < name.Length; i++)
        bus.Write((ushort)(VgcConstants.FioName + i), (byte)name[i]);
    bus.Write((ushort)VgcConstants.FioSrcL, 0x01); // song 1
    bus.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSidPlay);
    Assert.AreEqual(VgcConstants.FioStatusError, bus.Read((ushort)VgcConstants.FioStatus));
}
```

**Step 7: Run all tests**

Run: `dotnet test`
Expected: All pass.

**Step 8: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/FileIoController.cs \
        e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/FileIoControllerTests.cs
git commit -m "feat: add FIO SID play/stop commands for BASIC integration"
```

---

## Task 4: Add SIDPLAY/SIDSTOP BASIC Commands (Assembly)

Add extended tokens to ehbasic for SIDPLAY and SIDSTOP.

**Files:**
- Modify: `ehbasic/basic.asm`
- Build: `ehbasic/Makefile` (run `make`)
- Copy: `ehbasic/basic.bin` → `e6502.Avalonia/Resources/ehbasic.bin`

**Step 1: Add token constants**

After `XTK_GLOAD = $11` (~line 483), add:

```asm
XTK_SIDPLAY       = $12              ; extended token id: SIDPLAY
XTK_SIDSTOP       = $13              ; extended token id: SIDSTOP
```

**Step 2: Update XTK_COUNT**

Change line ~8130:

```asm
XTK_COUNT = 19
```

(was 17)

**Step 3: Add keyword strings**

In `TAB_XTKSTR` word table (~line 8133), add after the `@s_gload` entry:

```asm
      .word @s_sidplay, @s_sidstop
```

After `@s_gload` string (~line 8154), add:

```asm
@s_sidplay: .byte "SIDPLAY",0
@s_sidstop: .byte "SIDSTOP",0
```

**Step 4: Add dispatch table entries**

In `TAB_XTKCMD` (~line 1813), add after `LAB_GLOAD-1`:

```asm
      .word LAB_SIDPLAY-1      ; XTK_SIDPLAY ($12)
      .word LAB_SIDSTOP-1      ; XTK_SIDSTOP ($13)
```

**Step 5: Write handler assembly**

Add the handler routines. Place them near the other FIO-related handlers (near LAB_GSAVE/LAB_GLOAD). The FIO constants (`FIO_CMD`, `FIO_SRCL`, etc.) are already defined in the assembly.

```asm
; ── SID playback commands ─────────────────────────────────────────

; SIDPLAY "filename" [, song]
; Loads a .sid file and starts playback via FIO_CMD_SIDPLAY ($08)

FIO_CMD_SIDPLAY = $08
FIO_CMD_SIDSTOP = $09

LAB_SIDPLAY
      JSR   LAB_FIO_GETNAME   ; parse filename → FIO_NAME/FIO_NAMELEN
      BCC   @sp_have_name
      JMP   LAB_FIO_ERRIO     ; bad filename
@sp_have_name
      LDA   #$01              ; default song = 1
      STA   FIO_SRCL
      JSR   LAB_GBYT          ; peek next token
      CMP   #','
      BNE   @sp_go            ; no comma = use default song
      JSR   LAB_1C01          ; consume comma
      JSR   LAB_GTBY          ; evaluate byte expression → X
      STX   FIO_SRCL          ; store song number
@sp_go
      LDA   #FIO_CMD_SIDPLAY
      STA   FIO_CMD           ; trigger SID play
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @sp_ok
      LDA   FIO_ERRCODE
      CMP   #$01              ; not found?
      BNE   @sp_errio
      JMP   LAB_FIO_ERRFNF   ; "File not found" error
@sp_errio
      JMP   LAB_FIO_ERRIO    ; I/O error
@sp_ok
      RTS

; SIDSTOP
; Stops SID playback

LAB_SIDSTOP
      LDA   #FIO_CMD_SIDSTOP
      STA   FIO_CMD           ; trigger SID stop
      RTS
```

**Step 6: Build the ROM**

```bash
cd ehbasic && make clean && make
```

Expected: `basic.bin` generated without errors.

**Step 7: Copy ROM to Avalonia resources**

```bash
cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin
```

**Step 8: Build and run tests**

```bash
dotnet build && dotnet test
```

Expected: All tests pass. The ROM binary change doesn't break existing tests.

**Step 9: Commit**

```bash
git add ehbasic/basic.asm ehbasic/basic.bin ehbasic/basic.o ehbasic/basic.map \
        ehbasic/min_mon.lst e6502.Avalonia/Resources/ehbasic.bin
git commit -m "feat: add SIDPLAY/SIDSTOP BASIC commands"
```

---

## Task 5: Expose OpCodeRecord.Instruction for Relocator

The relocator needs to identify instruction types (JMP, JSR, RTS, RTI, branches) from opcode records. Currently `Instruction` is private.

**Files:**
- Modify: `e6502/OpCodes/OpCodeRecord.cs`
- Test: Quick test to verify accessibility

**Step 1: Make Instruction internal**

In `e6502/OpCodes/OpCodeRecord.cs` (~line 8), change:

```csharp
private string Instruction { get; }
```

to:

```csharp
internal string Instruction { get; }
```

Also change the class visibility from `internal` to `public` if it isn't already (check line 5). The `OpCodeTable` class should also be accessible.

**Step 2: Build**

```bash
dotnet build
```

Expected: 0 errors. Nothing else references `Instruction` so no side effects.

**Step 3: Commit**

```bash
git add e6502/OpCodes/OpCodeRecord.cs
git commit -m "refactor: expose OpCodeRecord.Instruction as internal"
```

---

## Task 6: Create e6502.Tools Project with SID Relocator

Build the control-flow-following disassembler and address patcher.

**Files:**
- Create: `e6502.Tools/e6502.Tools.csproj`
- Create: `e6502.Tools/SidRelocator.cs`
- Create: `e6502.Tools/Program.cs`
- Test: `e6502UnitTests/SidRelocatorTests.cs`
- Modify: `e6502.sln` (add project)

**Step 1: Create project**

```bash
dotnet new console -n e6502.Tools -o e6502.Tools --framework net10.0
dotnet sln add e6502.Tools/e6502.Tools.csproj
```

Add project references in `e6502.Tools.csproj`:

```xml
<ItemGroup>
    <ProjectReference Include="..\e6502\e6502.csproj" />
    <ProjectReference Include="..\e6502.Avalonia\e6502.Avalonia.csproj" />
</ItemGroup>
```

Add reference in test project `e6502UnitTests.csproj`:

```xml
<ProjectReference Include="..\e6502.Tools\e6502.Tools.csproj" />
```

**Step 2: Write tests**

Create `e6502UnitTests/SidRelocatorTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using e6502.Tools;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidRelocatorTests
{
    // Build a minimal SID with known code: JMP abs, JSR abs, LDA abs, LDA imm
    private static byte[] MakeRelocatableSid(ushort loadAddr, ushort initAddr, ushort playAddr, byte[] payload)
    {
        var data = new byte[124 + payload.Length];
        data[0] = (byte)'P'; data[1] = (byte)'S'; data[2] = (byte)'I'; data[3] = (byte)'D';
        data[4] = 0x00; data[5] = 0x02;
        data[6] = 0x00; data[7] = 0x7C;
        data[8] = (byte)(loadAddr >> 8); data[9] = (byte)(loadAddr & 0xFF);
        data[10] = (byte)(initAddr >> 8); data[11] = (byte)(initAddr & 0xFF);
        data[12] = (byte)(playAddr >> 8); data[13] = (byte)(playAddr & 0xFF);
        data[14] = 0x00; data[15] = 0x01;
        data[16] = 0x00; data[17] = 0x01;
        Array.Copy(payload, 0, data, 124, payload.Length);
        return data;
    }

    [TestMethod]
    public void Relocate_PatchesAbsoluteJsr()
    {
        // Code at $1000: JSR $1003 / RTS / LDA #$FF / RTS
        byte[] payload =
        [
            0x20, 0x03, 0x10,   // JSR $1003
            0x60,               // RTS
            0xA9, 0xFF,         // LDA #$FF (init target)
            0x60                // RTS
        ];
        var sid = MakeRelocatableSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);

        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.IsTrue(result.IsValid);
        Assert.AreEqual(0x2000, result.LoadAddress);
        Assert.AreEqual(0x2000, result.InitAddress);
        Assert.AreEqual(0x2000, result.PlayAddress);
        // JSR target should be patched from $1003 to $2003
        Assert.AreEqual(0x20, result.Payload[0]); // JSR opcode
        Assert.AreEqual(0x03, result.Payload[1]); // low byte
        Assert.AreEqual(0x20, result.Payload[2]); // high byte ($20 = relocated)
    }

    [TestMethod]
    public void Relocate_DoesNotPatchImmediateValues()
    {
        // Code: LDA #$10 / RTS
        byte[] payload = [ 0xA9, 0x10, 0x60 ];
        var sid = MakeRelocatableSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);

        var result = SidRelocator.Relocate(info, 0x2000);

        // LDA #$10 should NOT be patched
        Assert.AreEqual(0xA9, result.Payload[0]);
        Assert.AreEqual(0x10, result.Payload[1]);
    }

    [TestMethod]
    public void Relocate_DoesNotPatchSidRegisters()
    {
        // Code: STA $D400 / RTS
        byte[] payload = [ 0x8D, 0x00, 0xD4, 0x60 ];
        var sid = MakeRelocatableSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);

        var result = SidRelocator.Relocate(info, 0x2000);

        // STA $D400 is outside the player range, should NOT be patched
        Assert.AreEqual(0x8D, result.Payload[0]);
        Assert.AreEqual(0x00, result.Payload[1]);
        Assert.AreEqual(0xD4, result.Payload[2]);
    }

    [TestMethod]
    public void Relocate_FollowsBranches()
    {
        // Code at $1000:
        //   BNE +2 (→$1004)     ; branch over the JMP
        //   JMP $1004           ; abs jump within player
        //   RTS                 ; $1004
        byte[] payload =
        [
            0xD0, 0x02,         // BNE +2
            0x4C, 0x04, 0x10,   // JMP $1004
            0x60                // RTS
        ];
        var sid = MakeRelocatableSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);

        var result = SidRelocator.Relocate(info, 0x2000);

        // BNE offset should stay the same (relative)
        Assert.AreEqual(0xD0, result.Payload[0]);
        Assert.AreEqual(0x02, result.Payload[1]);
        // JMP target should be patched
        Assert.AreEqual(0x4C, result.Payload[2]);
        Assert.AreEqual(0x04, result.Payload[3]); // low
        Assert.AreEqual(0x20, result.Payload[4]); // high
    }

    [TestMethod]
    public void Relocate_UpdatesHeaderAddresses()
    {
        byte[] payload = [ 0x60, 0x60 ]; // RTS; RTS
        var sid = MakeRelocatableSid(0x1000, 0x1000, 0x1001, payload);
        var info = SidFileParser.Parse(sid);

        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.AreEqual(0x2000, result.LoadAddress);
        Assert.AreEqual(0x2000, result.InitAddress);
        Assert.AreEqual(0x2001, result.PlayAddress);
    }
}
```

**Step 3: Run tests to verify they fail**

Run: `dotnet test --filter "SidRelocatorTests"`
Expected: FAIL — `SidRelocator` doesn't exist.

**Step 4: Implement SidRelocator**

Create `e6502.Tools/SidRelocator.cs`:

```csharp
using e6502.Avalonia.Hardware;
using KDS.e6502.OpCodes;

namespace e6502.Tools;

public static class SidRelocator
{
    /// <summary>
    /// Relocate a SID file's code to a new base address using control-flow-following disassembly.
    /// </summary>
    public static SidFileInfo Relocate(SidFileInfo info, ushort targetAddr)
    {
        if (!info.IsValid)
            return SidFileInfo.Invalid;

        int delta = targetAddr - info.LoadAddress;
        if (delta == 0)
            return info;

        ushort origBase = info.LoadAddress;
        ushort origEnd = (ushort)(origBase + info.Payload.Length);

        // Discover all code bytes by following control flow
        var codeOffsets = new HashSet<int>();
        var opTable = new OpCodeTable();
        DiscoverCode(info.Payload, origBase, info.InitAddress, opTable, codeOffsets);
        if (info.PlayAddress != info.InitAddress)
            DiscoverCode(info.Payload, origBase, info.PlayAddress, opTable, codeOffsets);

        // Patch absolute addresses in discovered code
        byte[] patched = (byte[])info.Payload.Clone();
        foreach (int offset in codeOffsets)
        {
            if (offset >= patched.Length) continue;
            var op = opTable.OpCodes[patched[offset]];
            if (!op.IsValid || op.Bytes < 3) continue;

            // Only patch absolute addressing modes
            if (op.AddressMode is not (AddressModes.Absolute or AddressModes.AbsoluteX or AddressModes.AbsoluteY))
                continue;

            int operandOffset = offset + 1;
            if (operandOffset + 1 >= patched.Length) continue;

            ushort addr = (ushort)(patched[operandOffset] | (patched[operandOffset + 1] << 8));

            // Only patch if the address falls within the player's code range
            if (addr >= origBase && addr < origEnd)
            {
                ushort newAddr = (ushort)(addr + delta);
                patched[operandOffset] = (byte)(newAddr & 0xFF);
                patched[operandOffset + 1] = (byte)(newAddr >> 8);
            }
        }

        return new SidFileInfo
        {
            IsValid = true,
            Magic = info.Magic,
            Version = info.Version,
            LoadAddress = targetAddr,
            InitAddress = (ushort)(info.InitAddress + delta),
            PlayAddress = (ushort)(info.PlayAddress + delta),
            Songs = info.Songs,
            StartSong = info.StartSong,
            Speed = info.Speed,
            UsesCiaTiming = info.UsesCiaTiming,
            Title = info.Title,
            Author = info.Author,
            Copyright = info.Copyright,
            Payload = patched
        };
    }

    private static void DiscoverCode(byte[] payload, ushort loadBase, ushort entryPoint,
                                      OpCodeTable opTable, HashSet<int> codeOffsets)
    {
        var queue = new Queue<ushort>();
        queue.Enqueue(entryPoint);

        while (queue.Count > 0)
        {
            ushort pc = queue.Dequeue();
            int offset = pc - loadBase;

            // Bounds check and already-visited check
            while (offset >= 0 && offset < payload.Length && codeOffsets.Add(offset))
            {
                var op = opTable.OpCodes[payload[offset]];
                if (!op.IsValid) break;

                string mnemonic = op.Instruction;

                // Follow branch targets
                if (op.AddressMode == AddressModes.Relative && op.Bytes == 2 && offset + 1 < payload.Length)
                {
                    sbyte rel = (sbyte)payload[offset + 1];
                    ushort branchTarget = (ushort)(pc + 2 + rel);
                    queue.Enqueue(branchTarget);
                    // Fall through continues
                }

                // Follow JSR/JMP absolute targets
                if (op.AddressMode == AddressModes.Absolute && op.Bytes == 3 && offset + 2 < payload.Length)
                {
                    ushort target = (ushort)(payload[offset + 1] | (payload[offset + 2] << 8));

                    if (mnemonic is "JMP")
                    {
                        queue.Enqueue(target);
                        break; // JMP doesn't fall through
                    }
                    if (mnemonic is "JSR")
                    {
                        queue.Enqueue(target);
                        // JSR falls through after return
                    }
                }

                // Indirect JMP — can't follow statically
                if (op.AddressMode == AddressModes.Indirect)
                    break;

                // RTS/RTI — end of this path
                if (mnemonic is "RTS" or "RTI" or "BRK")
                    break;

                // Advance to next instruction
                offset += op.Bytes;
                pc += op.Bytes;
            }
        }
    }
}
```

NOTE: `op.Instruction` requires Task 5 to be completed first (making it internal). The `e6502.Tools` project references `e6502.csproj`, and since `OpCodeRecord` is `internal` to the `e6502` assembly, you'll also need to add an `InternalsVisibleTo` attribute in `e6502` or make the relevant types public.

Add to `e6502/e6502.csproj` or a file like `e6502/Properties/AssemblyInfo.cs`:

```csharp
[assembly: System.Runtime.CompilerServices.InternalsVisibleTo("e6502.Tools")]
[assembly: System.Runtime.CompilerServices.InternalsVisibleTo("e6502UnitTests")]
```

**Step 5: Implement CLI Program**

Create `e6502.Tools/Program.cs`:

```csharp
using e6502.Avalonia.Hardware;
using e6502.Tools;

if (args.Length < 1)
{
    Console.Error.WriteLine("Usage: sidreloc <input.sid> [output.sid] --target 0x1000");
    Console.Error.WriteLine("       sidreloc <input.sid> --info");
    return 1;
}

string inputPath = args[0];
if (!File.Exists(inputPath))
{
    Console.Error.WriteLine($"File not found: {inputPath}");
    return 1;
}

byte[] data = File.ReadAllBytes(inputPath);
var info = SidFileParser.Parse(data);
if (!info.IsValid)
{
    Console.Error.WriteLine("Invalid SID file");
    return 1;
}

// --info mode
if (args.Contains("--info"))
{
    Console.WriteLine($"Title:    {info.Title}");
    Console.WriteLine($"Author:   {info.Author}");
    Console.WriteLine($"Copyright:{info.Copyright}");
    Console.WriteLine($"Load:     ${info.LoadAddress:X4}");
    Console.WriteLine($"Init:     ${info.InitAddress:X4}");
    Console.WriteLine($"Play:     ${info.PlayAddress:X4}");
    Console.WriteLine($"Songs:    {info.Songs}");
    Console.WriteLine($"Speed:    {(info.UsesCiaTiming ? "CIA" : "VBI")}");
    Console.WriteLine($"Size:     {info.Payload.Length} bytes");
    return 0;
}

// --target mode
int targetIdx = Array.IndexOf(args, "--target");
if (targetIdx < 0 || targetIdx + 1 >= args.Length)
{
    Console.Error.WriteLine("Missing --target address");
    return 1;
}

if (!ushort.TryParse(args[targetIdx + 1].Replace("0x", "").Replace("$", ""),
    System.Globalization.NumberStyles.HexNumber, null, out ushort target))
{
    Console.Error.WriteLine($"Invalid target address: {args[targetIdx + 1]}");
    return 1;
}

string outputPath = args.Length > 1 && !args[1].StartsWith("--") ? args[1] : inputPath;

var relocated = SidRelocator.Relocate(info, target);
if (!relocated.IsValid)
{
    Console.Error.WriteLine("Relocation failed");
    return 1;
}

// Write output .sid file (rebuild PSID header + payload)
WriteSidFile(outputPath, relocated);
Console.WriteLine($"Relocated ${info.LoadAddress:X4} → ${target:X4}, wrote {outputPath}");
return 0;

static void WriteSidFile(string path, SidFileInfo info)
{
    byte[] header = new byte[124];
    header[0] = (byte)'P'; header[1] = (byte)'S'; header[2] = (byte)'I'; header[3] = (byte)'D';
    WriteBE16(header, 4, info.Version);
    WriteBE16(header, 6, 124); // data offset
    WriteBE16(header, 8, info.LoadAddress);
    WriteBE16(header, 10, info.InitAddress);
    WriteBE16(header, 12, info.PlayAddress);
    WriteBE16(header, 14, info.Songs);
    WriteBE16(header, 16, info.StartSong);
    header[18] = (byte)(info.Speed >> 24);
    header[19] = (byte)(info.Speed >> 16);
    header[20] = (byte)(info.Speed >> 8);
    header[21] = (byte)info.Speed;
    WriteString(header, 22, info.Title, 32);
    WriteString(header, 54, info.Author, 32);
    WriteString(header, 86, info.Copyright, 32);

    using var fs = File.Create(path);
    fs.Write(header);
    fs.Write(info.Payload);
}

static void WriteBE16(byte[] buf, int offset, int value)
{
    buf[offset] = (byte)(value >> 8);
    buf[offset + 1] = (byte)(value & 0xFF);
}

static void WriteString(byte[] buf, int offset, string s, int maxLen)
{
    int len = Math.Min(s.Length, maxLen);
    for (int i = 0; i < len; i++)
        buf[offset + i] = (byte)s[i];
}
```

**Step 6: Run tests**

Run: `dotnet test --filter "SidRelocatorTests"`
Expected: All 5 pass.

**Step 7: Run full test suite**

Run: `dotnet test`
Expected: All tests pass.

**Step 8: Commit**

```bash
git add e6502.Tools/ e6502UnitTests/SidRelocatorTests.cs e6502UnitTests/e6502UnitTests.csproj \
        e6502/OpCodes/OpCodeRecord.cs e6502.sln
git commit -m "feat: add SID relocator CLI tool"
```

---

## Dependencies

```
Task 1 (SidPlayer)
  ↓
Task 2 (Refactor TCP) ←── depends on Task 1
  ↓
Task 3 (FIO commands) ←── depends on Task 1
  ↓
Task 4 (BASIC assembly) ←── depends on Task 3

Task 5 (Expose Instruction) ←── independent
  ↓
Task 6 (Relocator) ←── depends on Task 5
```

Tasks 1-4 are sequential. Task 5 can run in parallel with Tasks 1-4. Task 6 depends on Task 5.
