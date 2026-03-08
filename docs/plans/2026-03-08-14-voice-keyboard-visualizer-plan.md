# 14-Voice Keyboard Visualizer Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Extend the 6-voice keyboard visualizer to 14 voices (6 SID + 8 WTS) with unique colors, two-row legend, and auto-soundfont loading for MIDI playback.

**Architecture:** Add 8 contiguous note registers in the music status block, extend the 65C02 assembly visualizer to poll and render all 14 voices, and auto-load a default soundfont in the MIDI play command when WTS has no instruments.

**Tech Stack:** C# (.NET 10, MSTest), 65C02 assembly (ca65/ld65), Avalonia

**Design doc:** `docs/plans/2026-03-08-14-voice-keyboard-visualizer-design.md`

---

### Task 1: Extend VgcConstants with 14 voice note registers

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs:110-121`

**Step 1: Update the register constants**

Replace lines 110-121 with:

```csharp
    // Music/SFX status register
    public const int  MusicStatus      = 0xBA50; // bit 0 = SFX playing, bit 1 = music playing
    public const int  MusicNote1       = 0xBA51; // voice 1 current MIDI note (0=silent)
    public const int  MusicNote2       = 0xBA52; // voice 2 current MIDI note
    public const int  MusicNote3       = 0xBA53; // voice 3 current MIDI note
    public const int  MusicNote4       = 0xBA54; // voice 4 current MIDI note
    public const int  MusicNote5       = 0xBA55; // voice 5 current MIDI note
    public const int  MusicNote6       = 0xBA56; // voice 6 current MIDI note
    public const int  MusicNote7       = 0xBA57; // voice 7 current MIDI note (WTS 0)
    public const int  MusicNote8       = 0xBA58; // voice 8 current MIDI note (WTS 1)
    public const int  MusicNote9       = 0xBA59; // voice 9 current MIDI note (WTS 2)
    public const int  MusicNote10      = 0xBA5A; // voice 10 current MIDI note (WTS 3)
    public const int  MusicNote11      = 0xBA5B; // voice 11 current MIDI note (WTS 4)
    public const int  MusicNote12      = 0xBA5C; // voice 12 current MIDI note (WTS 5)
    public const int  MusicNote13      = 0xBA5D; // voice 13 current MIDI note (WTS 6)
    public const int  MusicNote14      = 0xBA5E; // voice 14 current MIDI note (WTS 7)
    public const int  MusicElapsedL    = 0xBA5F; // elapsed frames low byte (60Hz)
    public const int  MusicElapsedH    = 0xBA60; // elapsed frames high byte
    public const int  MusicTotalL      = 0xBA61; // total frames low byte
    public const int  MusicTotalH      = 0xBA62; // total frames high byte
```

**Step 2: Build to verify no compile errors**

Run: `dotnet build`
Expected: Build succeeded

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat: add MusicNote7-14 registers, move elapsed/total to $BA5F-$BA62"
```

---

### Task 2: Update CompositeBusDevice to serve 14 voice notes

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs:176-189`

**Step 1: Write the failing test**

Add to `e6502UnitTests/AvaloniaCompositeBusTests.cs`:

```csharp
    [TestMethod]
    public void MusicNoteRegisters_ReadAllFourteenVoices()
    {
        var bus = MakeBus();

        // Set notes on all 14 voices via MusicEngine
        for (int v = 0; v < 14; v++)
            bus.Music.DirectNoteOn(v, 60 + v, 100, 0);

        // Read back from contiguous registers $BA51-$BA5E
        for (int v = 0; v < 14; v++)
        {
            byte note = bus.Read((ushort)(VgcConstants.MusicNote1 + v));
            Assert.AreEqual((byte)(60 + v), note, $"Voice {v} note mismatch");
        }
    }

    [TestMethod]
    public void MusicElapsedRegisters_AtNewAddresses()
    {
        var bus = MakeBus();
        // Elapsed/Total registers should be readable at new addresses
        // Default is 0 when nothing is playing
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.MusicElapsedL));
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.MusicElapsedH));
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.MusicTotalL));
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.MusicTotalH));
    }
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "MusicNoteRegisters_ReadAllFourteenVoices|MusicElapsedRegisters_AtNewAddresses"`
Expected: FAIL — voice 6+ returns 0 (old range was MusicNote1-6 only), elapsed reads from wrong address

**Step 3: Update CompositeBusDevice**

In `e6502.Avalonia/Hardware/CompositeBusDevice.cs`, replace lines 176-189:

```csharp
        if (address >= VgcConstants.MusicNote1 && address <= VgcConstants.MusicNote14)
            return _musicEngine.GetVoiceNote(address - VgcConstants.MusicNote1);
        if (address >= VgcConstants.MusicElapsedL && address <= VgcConstants.MusicTotalH)
        {
            int elapsed = _sidPlayer.IsPlaying ? _sidPlayer.ElapsedFrames : _musicEngine.ElapsedFrames;
            int total = _musicEngine.TotalFrames;
            return address switch
            {
                VgcConstants.MusicElapsedL => (byte)(elapsed & 0xFF),
                VgcConstants.MusicElapsedH => (byte)((elapsed >> 8) & 0xFF),
                VgcConstants.MusicTotalL   => (byte)(total & 0xFF),
                VgcConstants.MusicTotalH   => (byte)((total >> 8) & 0xFF),
                _ => 0
            };
        }
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "MusicNoteRegisters_ReadAllFourteenVoices|MusicElapsedRegisters_AtNewAddresses"`
Expected: PASS

**Step 5: Run all tests**

Run: `dotnet test`
Expected: All 1020+ tests pass

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/AvaloniaCompositeBusTests.cs
git commit -m "feat: CompositeBusDevice serves 14 voice note registers"
```

---

### Task 3: Auto-load soundfont in CmdMidPlay

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs:1140-1142`

**Step 1: Write the failing test**

Add a new test file `e6502UnitTests/MidiAutoSoundfontTests.cs`:

```csharp
using System;
using System.IO;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MidiAutoSoundfontTests
{
    [TestMethod]
    public void AutoLoadSoundfont_LoadsWhenSfExists()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-autosf-" + Guid.NewGuid().ToString("N"));
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "GeneralUser_GS.sf2"), sf2);

            var wts = new WavetableSynth();
            Assert.AreEqual(0, wts.InstrumentCount);

            var memory = new byte[65536];
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                dir,
                wts: wts);

            // Simulate auto-load logic
            MidiAutoSoundfont.TryLoad(fio, wts);

            Assert.IsTrue(wts.InstrumentCount > 0);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void AutoLoadSoundfont_NoOpWhenAlreadyLoaded()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-autosf-" + Guid.NewGuid().ToString("N"));
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            File.WriteAllBytes(Path.Combine(sfDir, "GeneralUser_GS.sf2"), sf2);

            var wts = new WavetableSynth();
            var memory = new byte[65536];
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                dir,
                wts: wts);

            // Load once
            MidiAutoSoundfont.TryLoad(fio, wts);
            int count1 = wts.InstrumentCount;

            // Load again — should not reload
            MidiAutoSoundfont.TryLoad(fio, wts);
            Assert.AreEqual(count1, wts.InstrumentCount);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void AutoLoadSoundfont_NoOpWhenFileNotFound()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-autosf-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(dir);

        try
        {
            var wts = new WavetableSynth();
            var memory = new byte[65536];
            var fio = new FileIoController(
                address => memory[address],
                (address, data) => memory[address] = data,
                dir,
                wts: wts);

            // No soundfont file exists — should silently do nothing
            MidiAutoSoundfont.TryLoad(fio, wts);
            Assert.AreEqual(0, wts.InstrumentCount);
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "MidiAutoSoundfontTests"`
Expected: FAIL — `MidiAutoSoundfont` class does not exist

**Step 3: Create MidiAutoSoundfont helper**

Create `e6502.Avalonia/Hardware/MidiAutoSoundfont.cs`:

```csharp
namespace e6502.Avalonia.Hardware;

/// <summary>
/// Auto-loads a default soundfont when WTS has no instruments.
/// </summary>
public static class MidiAutoSoundfont
{
    private const string DefaultSoundfont = "GeneralUser_GS";

    public static void TryLoad(FileIoController fio, WavetableSynth wts)
    {
        if (wts.InstrumentCount > 0) return;

        string sfPath = Path.Combine(fio.SaveDirectory, "soundfonts", DefaultSoundfont + ".sf2");
        if (!File.Exists(sfPath)) return;

        try
        {
            using var fs = File.OpenRead(sfPath);
            var bank = Sf2Loader.Load(fs);
            wts.LoadBank(bank);
        }
        catch
        {
            // Silently fail — fall back to SID-only
        }
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "MidiAutoSoundfontTests"`
Expected: PASS

**Step 5: Wire into CmdMidPlay**

In `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`, replace line 1142:

```csharp
        bool sidOnly = mode == MidiRoutingMode.SidOnly || _bus.Wts.InstrumentCount == 0;
```

with:

```csharp
        // Auto-load soundfont if WTS is empty and not in SID-only mode
        if (mode != MidiRoutingMode.SidOnly && _bus.Wts.InstrumentCount == 0)
            MidiAutoSoundfont.TryLoad(_bus.Fio, _bus.Wts);

        bool sidOnly = mode == MidiRoutingMode.SidOnly || _bus.Wts.InstrumentCount == 0;
```

**Step 6: Run all tests**

Run: `dotnet test`
Expected: All tests pass

**Step 7: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiAutoSoundfont.cs e6502.Avalonia/Ipc/EmulatorTcpServer.cs e6502UnitTests/MidiAutoSoundfontTests.cs
git commit -m "feat: auto-load GeneralUser_GS soundfont when MIDI needs WTS voices"
```

---

### Task 4: Regenerate novavm.inc

**Files:**
- Regenerate: `ehbasic/novavm.inc` (via `tools/gen_novavm_inc.py`)

**Step 1: Regenerate**

Run: `cd ehbasic && make novavm.inc`
Expected: `novavm.inc` updated with new `MusicNote7-14` equates and moved `MusicElapsedL/H`, `MusicTotalL/H`

**Step 2: Verify new equates exist**

Run: `grep -E "MusicNote(7|8|14)|MusicElapsedL|MusicTotalH" ehbasic/novavm.inc`
Expected output:
```
MusicNote7              = $BA57
MusicNote8              = $BA58
MusicNote14             = $BA5E
MusicElapsedL           = $BA5F
MusicTotalH             = $BA62
```

**Step 3: Commit**

```bash
git add ehbasic/novavm.inc
git commit -m "chore: regenerate novavm.inc with 14 voice note registers"
```

---

### Task 5: Update keyboard.s for 14 voices

**Files:**
- Modify: `ehbasic/keyboard.s`

This task has several sub-changes. Apply them all, then build and test.

**Step 1: Update NUM_VOICES constant**

Line 25, change:
```asm
NUM_VOICES  = 6
```
to:
```asm
NUM_VOICES  = 14
```

**Step 2: Update voice_colors table**

Line 1268, replace:
```asm
voice_colors: .byte COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN, COL_CYAN, COL_BLUE
```
with:
```asm
; SID voices 0-5, then WTS voices 6-13
voice_colors:
    .byte COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN, COL_CYAN, COL_BLUE
    .byte 9, 10, 11, 12, 13, 14, 15, 4
```

Palette indices: 9=brown, 10=light red, 11=dark gray, 12=medium gray, 13=light green, 14=light blue, 15=light gray, 4=purple.

**Step 3: Rewrite draw_chrome legend to two rows**

Replace the entire `draw_chrome` subroutine (lines 610-709) with:

```asm
; =====================================================================
; Draw player chrome (progress bar, legend, instructions)
; =====================================================================
draw_chrome:
    ; Progress bar outline
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    lda #<BAR_X
    sta RegP0
    lda #>BAR_X
    sta RegP1
    lda #BAR_Y
    sta RegP2
    stz RegP3
    lda #<(BAR_X + BAR_W)
    sta RegP4
    lda #>(BAR_X + BAR_W)
    sta RegP5
    lda #BAR_Y + BAR_H
    sta RegP6
    stz RegP7
    lda #CmdRect
    sta RegCmd

    ; ---- SID voice legend: row 20 ----
    lda #20
    sta RegCursorY
    ldx #0                          ; voice index 0-5
@leg_sid:
    cpx #6
    bcs @leg_wts_init
    phx

    ; Column = 2 + voice * 5
    txa
    asl                             ; *2
    clc
    adc zp_tmp                      ; oops, need clean multiply
    ; Redo: A = x * 5
    plx
    phx
    txa
    sta zp_tmp
    asl                             ; *2
    asl                             ; *4
    clc
    adc zp_tmp                      ; *5
    clc
    adc #2                          ; +2 offset
    sta RegCursorX

    plx
    phx
    lda voice_colors,x
    sta RegFgCol

    ; Solid block
    lda #$DB
    sta RegCharOut

    lda #COL_WHITE
    sta RegFgCol
    lda #'S'
    sta RegCharOut

    plx
    phx
    txa
    clc
    adc #'1'
    sta RegCharOut

    plx
    inx
    bra @leg_sid

@leg_wts_init:
    ; ---- WTS voice legend: row 21 ----
    lda #21
    sta RegCursorY
    ldx #6                          ; voice index 6-13
@leg_wts:
    cpx #NUM_VOICES
    bcs @leg_done
    phx

    ; Column = 2 + (voice - 6) * 5
    txa
    sec
    sbc #6
    sta zp_tmp
    asl                             ; *2
    asl                             ; *4
    clc
    adc zp_tmp                      ; *5
    clc
    adc #2
    sta RegCursorX

    plx
    phx
    lda voice_colors,x
    sta RegFgCol

    ; Solid block
    lda #$DB
    sta RegCharOut

    lda #COL_WHITE
    sta RegFgCol
    lda #'W'
    sta RegCharOut

    plx
    phx
    txa
    sec
    sbc #6
    clc
    adc #'1'
    sta RegCharOut

    plx
    inx
    bra @leg_wts

@leg_done:
    ; "PRESS ANY KEY TO EXIT" at row 24, column 29 (centered)
    lda #24
    sta RegCursorY
    lda #29
    sta RegCursorX
    lda #COL_LGRAY
    sta RegFgCol
    lda #<str_exit
    sta zp_str_ptr
    lda #>str_exit
    sta zp_str_ptr+1
    jsr print_str

    ; Time label at row 19
    lda #COL_WHITE
    sta RegFgCol
    lda #19
    sta RegCursorY
    lda #2
    sta RegCursorX
    lda #<str_time
    sta zp_str_ptr
    lda #>str_time
    sta zp_str_ptr+1
    jsr print_str

    rts
```

**Note:** The SID legend multiply-by-5 has a bug in the above — the first `adc zp_tmp` references `zp_tmp` before storing. Here is the corrected SID legend loop body (replace the `@leg_sid` block):

```asm
@leg_sid:
    cpx #6
    bcs @leg_wts_init
    phx

    ; Column = 2 + voice * 5
    txa
    sta zp_tmp                      ; save voice index
    asl                             ; *2
    asl                             ; *4
    clc
    adc zp_tmp                      ; *5
    clc
    adc #2
    sta RegCursorX

    plx
    phx
    lda voice_colors,x
    sta RegFgCol

    lda #$DB
    sta RegCharOut

    lda #COL_WHITE
    sta RegFgCol
    lda #'S'
    sta RegCharOut

    plx
    phx
    txa
    clc
    adc #'1'
    sta RegCharOut

    plx
    inx
    bra @leg_sid
```

**Step 4: Build the keyboard binary**

Run: `cd ehbasic && make keyboard.bin`
Expected: keyboard.bin built successfully

**Step 5: Install keyboard binary**

Run: `cp ehbasic/keyboard.bin ~/e6502-programs/keyboard.bin`

**Step 6: Commit**

```bash
git add ehbasic/keyboard.s ehbasic/keyboard.bin
git commit -m "feat: keyboard visualizer shows 14 voices with SID/WTS legend"
```

---

### Task 6: Manual integration test

**No code changes — verification only.**

**Step 1: Start the emulator**

Run: `dotnet run --project e6502.Avalonia &`

**Step 2: Play a 14-channel MIDI with soundfont**

```bash
echo '{"command":"mid_play","path":"/Users/barry/e6502-programs/Sousa_StarsAndStripesForever.mid"}' | nc -w 2 localhost 6502
```

Expected: `"routing":"auto"` with 14 voices. Soundfont should auto-load (no manual `wts_load_soundfont` needed).

**Step 3: Launch keyboard visualizer**

In the emulator BASIC prompt, type:
```
LOAD "KEYBOARD", $9000
SYS $9000
```

Expected:
- 8-octave piano keyboard displayed
- All 14 voices light up with distinct colors as notes play
- Row 20 shows: `█ S1  █ S2  █ S3  █ S4  █ S5  █ S6`
- Row 21 shows: `█ W1  █ W2  █ W3  █ W4  █ W5  █ W6  █ W7  █ W8`
- Row 24 shows "PRESS ANY KEY TO EXIT" centered at column 29
- Progress bar and time display work correctly
- Press any key to exit cleanly

**Step 4: Verify SID-only fallback**

Stop the emulator, delete the soundfont, restart, and play the same MIDI:
Expected: Falls back to 6-voice SID routing, keyboard visualizer shows only 6 voices.
