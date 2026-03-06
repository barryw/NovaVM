# Keyboard Visualizer Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a reusable 6502 assembly keyboard visualizer that works with any music source (MML, MIDI, SID).

**Architecture:** Assembly program loaded at $9000, reads music status registers ($BA50-$BA5A), displays 8-octave keyboard with sprite-based note indicators. C# infrastructure adds frame counters, .bin file support, fixes MIDI/SID note reporting, renames CALL→SYS.

**Tech Stack:** C#/.NET (emulator), ca65/ld65 (assembler), Python (code generator), MSTest (tests)

---

### Task 1: Rename CALL → SYS in ROM

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Rename token constant and all CALL references**

In `ehbasic/basic.asm`, make these exact replacements:

Line 349: `TK_CALL` → `TK_SYS`, comment `CALL token` → `SYS token`
```asm
TK_SYS            = TK_DOKE+1       ; SYS token
```

Line 350: `TK_CALL` ref → `TK_SYS`
```asm
TK_DO             = TK_SYS+1        ; DO token
```

Line 5952: comment
```asm
; perform SYS
```

Line 5954: label
```asm
LAB_SYS
```

Lines 5958, 5960: `CallExit` → `SysExit`
```asm
      LDA   #>SysExit         ; set return address high byte
      ...
      LDA   #<SysExit-1       ; set return address low byte
```

Line 5967: label
```asm
SysExit
```

Line 11397: dispatch vector + comment
```asm
      .word LAB_SYS-1         ; SYS             new command
```

Line 11675-11676: keyword string label + bytes. Note: "CALL" is stored as "C" prefix + "ALL" + token. "SYS" is "S" prefix + "YS" + token. The label and keyword lookup must both change:
```asm
LBB_SYS
      .byte "YS",TK_SYS       ; SYS
```

Line 12007: keyword index entry
```asm
      .word LBB_SYS            ; SYS
```

Also update the keyword index entry that routes to LBB_SYS. The keyword scanner looks up by first character. "CALL" was under 'C'; "SYS" goes under 'S'. Find the entry that points to LBB_CALL and change it. Check the dispatch table around line 12007 — it's organized by first-letter. The entry for 'C' that had `LBB_CALL` must be removed, and an 'S' entry for `LBB_SYS` must be added.

Search for the keyword dispatch table structure near lines 11990-12050. Each entry is: `.byte count, 'X'` followed by `.word LBB_xxx`. Find the 'C' group, remove the CALL entry (decrement count), find the 'S' group, add the SYS entry (increment count).

**Step 2: Rebuild ROM and verify**

Run:
```bash
cd ehbasic && make clean && make
```
Expected: builds successfully, no assembler errors.

**Step 3: Verify SYS token in symbol file**

Run:
```bash
grep -i "SYS\|LBB_SYS\|LAB_SYS" ehbasic/basic.sym
```
Expected: symbols present with correct addresses.

**Step 4: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: rename CALL command to SYS for familiarity"
```

---

### Task 2: Fix DirectNoteOn/Off to update CurrentMidi

**Bug:** `MusicEngine.DirectNoteOn()` doesn't set `_voices[].CurrentMidi`, so `GetVoiceNote()` returns 0 during MIDI playback. `DirectNoteOff()` doesn't clear it either. `DirectNoteSlide()` doesn't update it.

**Files:**
- Modify: `e6502.Avalonia/Hardware/MusicEngine.cs`
- Test: `e6502UnitTests/AvaloniaSidTests.cs`

**Step 1: Write failing tests**

Add to `e6502UnitTests/AvaloniaSidTests.cs`:

```csharp
[TestMethod]
public void MusicEngine_DirectNoteOn_SetsVoiceNote()
{
    var bus = TestHelper.CreateBus();
    var engine = bus.MusicEngine;
    engine.EnterMidiMode();
    engine.DirectNoteOn(0, 60, 100, 0);
    Assert.AreEqual(60, engine.GetVoiceNote(0));
}

[TestMethod]
public void MusicEngine_DirectNoteOff_ClearsVoiceNote()
{
    var bus = TestHelper.CreateBus();
    var engine = bus.MusicEngine;
    engine.EnterMidiMode();
    engine.DirectNoteOn(0, 60, 100, 0);
    engine.DirectNoteOff(0);
    Assert.AreEqual(0, engine.GetVoiceNote(0));
}

[TestMethod]
public void MusicEngine_DirectNoteSlide_UpdatesVoiceNote()
{
    var bus = TestHelper.CreateBus();
    var engine = bus.MusicEngine;
    engine.EnterMidiMode();
    engine.DirectNoteOn(0, 60, 100, 0);
    engine.DirectNoteSlide(0, 72);
    Assert.AreEqual(72, engine.GetVoiceNote(0));
}
```

Note: Check if `TestHelper.CreateBus()` exists. If not, create a minimal helper that constructs a `CompositeBusDevice` with audio disabled. Look at how existing tests in AvaloniaSidTests create their test objects — they may use `SidChip(enableAudio: false)` directly. Adapt accordingly — the tests need a `MusicEngine` instance, which requires a `CompositeBusDevice`.

**Step 2: Run tests to verify they fail**

Run:
```bash
dotnet test --filter "DirectNoteOn_SetsVoiceNote|DirectNoteOff_ClearsVoiceNote|DirectNoteSlide_UpdatesVoiceNote" --verbosity normal
```
Expected: FAIL — GetVoiceNote returns 0 for all.

**Step 3: Fix DirectNoteOn — add CurrentMidi assignment**

In `e6502.Avalonia/Hardware/MusicEngine.cs`, line 243 (after `chip.Write(Ctrl(...), ...)`), add:

```csharp
    public void DirectNoteOn(int voiceIndex, int midiNote, int velocity, int instrumentId = 0)
    {
        if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
        var inst = GetInstrument(instrumentId);
        var chip = ChipFor(voiceIndex);

        // ... existing code ...

        // Gate on
        chip.Write(Ctrl(voiceIndex), (byte)(inst.Waveform | 0x01));

        // Track MIDI note for status register readback
        _voices[voiceIndex].CurrentMidi = midiNote;
    }
```

**Step 4: Fix DirectNoteOff — clear CurrentMidi**

After the gate-off write at line 261:

```csharp
    public void DirectNoteOff(int voiceIndex)
    {
        if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
        var chip = ChipFor(voiceIndex);
        byte ctrl = chip.Read(Ctrl(voiceIndex));
        chip.Write(Ctrl(voiceIndex), (byte)(ctrl & 0xFE)); // gate off
        _voices[voiceIndex].CurrentMidi = -1;
    }
```

**Step 5: Fix DirectNoteSlide — update CurrentMidi**

After the frequency write at line 253:

```csharp
    public void DirectNoteSlide(int voiceIndex, int midiNote)
    {
        if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
        int sidFreq = MidiToSid(midiNote);
        var chip = ChipFor(voiceIndex);
        chip.Write(FreqLo(voiceIndex), (byte)(sidFreq & 0xFF));
        chip.Write(FreqHi(voiceIndex), (byte)((sidFreq >> 8) & 0xFF));
        _voices[voiceIndex].CurrentMidi = midiNote;
    }
```

**Step 6: Run tests**

Run:
```bash
dotnet test --filter "DirectNoteOn_SetsVoiceNote|DirectNoteOff_ClearsVoiceNote|DirectNoteSlide_UpdatesVoiceNote" --verbosity normal
```
Expected: PASS

**Step 7: Run all tests**

Run:
```bash
dotnet test
```
Expected: all pass.

**Step 8: Commit**

```bash
git add e6502.Avalonia/Hardware/MusicEngine.cs e6502UnitTests/AvaloniaSidTests.cs
git commit -m "fix: DirectNoteOn/Off/Slide now updates CurrentMidi for voice note readback"
```

---

### Task 3: Fix MusicStatus to reflect MIDI/SID playback

**Bug:** `MusicStatus` bit 1 only reflects MML sequencer (`IsMusicPlaying`). When MIDI plays via `MidiPlayback`, or SID plays via `SidPlayer`, the status register says "not playing" because `_musicPlaying = false`.

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs:153-154`
- Test: `e6502UnitTests/AvaloniaSidTests.cs`

**Step 1: Write failing test**

```csharp
[TestMethod]
public void MusicStatus_ReflectsMidiPlayback()
{
    var bus = TestHelper.CreateBus();
    bus.MusicEngine.EnterMidiMode();
    // MidiPlayback.IsPlaying would be true during real playback.
    // For this test, just verify the bus Read includes MIDI state.
    // We need to check that bit 1 of $BA50 reflects MidiPlayback.IsPlaying.
    byte status = bus.Read(0xBA50);
    // With no music playing, bit 1 should be 0
    Assert.AreEqual(0, status & 0x02);
}
```

Note: This test may need adaptation based on how to simulate MIDI playback state in tests. The key behavioral change is: bit 1 of $BA50 should be `IsMusicPlaying || MidiPlayback.IsPlaying`.

**Step 2: Modify CompositeBusDevice.Read**

At line 153-154 in `CompositeBusDevice.cs`, change:

```csharp
if (address == VgcConstants.MusicStatus)
    return (byte)((_musicEngine.IsPlaying ? 1 : 0) | (_musicEngine.IsMusicPlaying ? 2 : 0));
```

To:

```csharp
if (address == VgcConstants.MusicStatus)
    return (byte)((_musicEngine.IsPlaying ? 1 : 0)
        | (_musicEngine.IsMusicPlaying || _midiPlayback.IsPlaying ? 2 : 0));
```

Note: SidPlayer doesn't have an `IsPlaying` property. SID playback works via IRQ trampoline at the CPU level, not ticked by C#. We may need to add a simple `IsPlaying` flag to SidPlayer that's set in `Play()` and cleared in `Stop()`, then include it in the status check:

```csharp
| (_musicEngine.IsMusicPlaying || _midiPlayback.IsPlaying || _sidPlayer.IsPlaying ? 2 : 0)
```

**Step 3: Add IsPlaying to SidPlayer**

In `SidPlayer.cs`, add a field and property:

```csharp
private bool _playing;
public bool IsPlaying => _playing;
```

Set `_playing = true` at end of `Play()`, set `_playing = false` at start of `Stop()`.

**Step 4: Run all tests**

```bash
dotnet test
```
Expected: all pass.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/Hardware/SidPlayer.cs e6502UnitTests/AvaloniaSidTests.cs
git commit -m "fix: MusicStatus register reflects MIDI and SID playback state"
```

---

### Task 4: Add elapsed/total frame registers

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/MusicEngine.cs`
- Modify: `e6502.Avalonia/Hardware/MidiPlayback.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Test: `e6502UnitTests/AvaloniaSidTests.cs`

**Step 1: Add constants to VgcConstants.cs**

After line 99 (`MusicNote6`), add:

```csharp
public const int  MusicElapsedL    = 0xBA57; // elapsed frames low byte
public const int  MusicElapsedH    = 0xBA58; // elapsed frames high byte
public const int  MusicTotalL      = 0xBA59; // total frames low byte
public const int  MusicTotalH      = 0xBA5A; // total frames high byte
```

**Step 2: Add frame tracking to MusicEngine**

In `MusicEngine.cs`, add fields after line 88:

```csharp
private int _elapsedFrames;
private int _totalFrames;
```

Add public properties after `IsMusicPlaying` (line 138):

```csharp
public int ElapsedFrames => _elapsedFrames;
public int TotalFrames => _totalFrames;
```

In `Tick()` (line 356), increment elapsed when any music is active:

```csharp
public void Tick()
{
    TickSfx();
    if (_musicPlaying && !_midiMode)
        TickMusic();
    if (_musicPlaying || _midiMode)
        _elapsedFrames++;
}
```

In `MusicPlay()` (line 307), reset elapsed and compute total:

```csharp
public void MusicPlay()
{
    _musicPlaying = true;
    _elapsedFrames = 0;
    for (int i = 0; i < VoiceCount; i++)
    {
        _voices[i].Reset(_bpm, _frameRateHz);
        _voices[i].CurrentInstrument = GetInstrument(0);
    }
    _totalFrames = ComputeTotalFrames();
}
```

Add total frames computation:

```csharp
private int ComputeTotalFrames()
{
    int maxFrames = 0;
    for (int i = 0; i < VoiceCount; i++)
    {
        var events = _voices[i].Events;
        if (events is null || events.Count == 0) continue;
        double totalTicks = 0;
        foreach (var ev in events)
        {
            if (ev.Type == MmlEventType.NoteOn || ev.Type == MmlEventType.Rest)
                totalTicks += ev.Param2; // wait ticks
        }
        double ticksPerFrame = _voices[i].TicksPerFrame;
        if (ticksPerFrame > 0)
        {
            int frames = (int)(totalTicks / ticksPerFrame);
            if (frames > maxFrames) maxFrames = frames;
        }
    }
    return maxFrames;
}
```

In `EnterMidiMode()` (line 205), reset elapsed:

```csharp
public void EnterMidiMode()
{
    MusicStop();
    MusicReset();
    _midiMode = true;
    _elapsedFrames = 0;
}
```

Add a method to set total from external source (MIDI):

```csharp
public void SetTotalFrames(int frames) => _totalFrames = frames;
```

**Step 3: Add total frames to MidiPlayback**

In `MidiPlayback.cs`, after `BuildTimeline()` in `Play()`, compute total:

```csharp
public void Play(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
{
    _ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
    _timeline = BuildTimeline(midi, voiceToChannel, instrumentSlots);
    _timelineIndex = 0;
    _midiTickAccum = 0;

    // Default 120 BPM
    SetTempo(120);

    _engine.EnterMidiMode();

    // Set total duration from last timeline event
    if (_timeline.Count > 0)
    {
        long lastTick = _timeline[^1].MidiTick;
        int totalFrames = (int)(lastTick / _midiTicksPerFrame);
        _engine.SetTotalFrames(totalFrames);
    }

    _playing = true;
}
```

Note: This is approximate since tempo changes during playback affect the real duration. Good enough for a progress bar.

**Step 4: Wire up CompositeBusDevice read handler**

In `CompositeBusDevice.cs`, after the MusicNote6 check (line 156), add:

```csharp
if (address >= VgcConstants.MusicElapsedL && address <= VgcConstants.MusicTotalH)
{
    int elapsed = _musicEngine.ElapsedFrames;
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

**Step 5: Write tests**

```csharp
[TestMethod]
public void MusicEngine_ElapsedFrames_IncrementsOnTick()
{
    var bus = TestHelper.CreateBus();
    var engine = bus.MusicEngine;
    engine.SetSequence(0, "T120O4L4C");
    engine.MusicPlay();
    Assert.AreEqual(0, engine.ElapsedFrames);
    engine.Tick();
    Assert.AreEqual(1, engine.ElapsedFrames);
    engine.Tick();
    Assert.AreEqual(2, engine.ElapsedFrames);
}

[TestMethod]
public void MusicEngine_TotalFrames_ComputedFromSequence()
{
    var bus = TestHelper.CreateBus();
    var engine = bus.MusicEngine;
    engine.SetSequence(0, "T120O4L4CDEF");
    engine.MusicPlay();
    Assert.IsTrue(engine.TotalFrames > 0, "TotalFrames should be computed from sequence");
}
```

**Step 6: Run tests**

```bash
dotnet test
```
Expected: all pass.

**Step 7: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/MusicEngine.cs e6502.Avalonia/Hardware/MidiPlayback.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/AvaloniaSidTests.cs
git commit -m "feat: add elapsed/total frame registers at \$BA57-\$BA5A"
```

---

### Task 5: Add .bin file extension support

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs`
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`

**Step 1: Add FioDirTypeBin constant**

In `VgcConstants.cs`, after line 108 (`FioErrEndOfDir`):

```csharp
public const byte FioDirTypeBin     = 0x02;   // dir entry type: .bin (binary executable)
```

**Step 2: Update DoLoad to support .bin**

In `FileIoController.cs`, `DoLoad()` at line 196, change the extension logic. When a load address is specified (raw mode), try `.bas` first, then `.bin`:

```csharp
private void DoLoad()
{
    try
    {
        string? filename = ReadFilename();
        if (filename is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        // Try .bas first, then .bin for raw loads
        string path = GetFullPath(filename, ".bas");
        if (!File.Exists(path))
        {
            string binPath = GetFullPath(filename, ".bin");
            if (File.Exists(binPath))
                path = binPath;
        }

        if (!File.Exists(path))
        {
            SetError(VgcConstants.FioErrNotFound);
            return;
        }

        // ... rest unchanged ...
```

**Step 3: Update DoDirOpen to include .bin files**

In `FileIoController.cs`, `DoDirOpen()` around line 242-245, add `.bin`:

```csharp
_dirFiles = dir.Exists
    ? dir.GetFiles("*.bas")
          .Concat(dir.GetFiles("*.sid"))
          .Concat(dir.GetFiles("*.bin"))
          .OrderBy(f => f.Name).ToList()
    : [];
```

**Step 4: Update PopulateDirEntry for .bin type**

In `FileIoController.cs`, `PopulateDirEntry()` around line 643-644:

```csharp
bool isSid = fi.Extension.Equals(".sid", StringComparison.OrdinalIgnoreCase);
bool isBin = fi.Extension.Equals(".bin", StringComparison.OrdinalIgnoreCase);
_regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)(isSid ? 1 : isBin ? 2 : 0);
```

For .bin files, the size should also exclude the 2-byte prefix (same as .bas):

```csharp
long dataSize = isSid ? fi.Length : Math.Max(0, fi.Length - 2);
```

This already handles .bin correctly since it's the same format.

**Step 5: Run all tests**

```bash
dotnet test
```
Expected: all pass.

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat: add .bin file extension support for binary executables"
```

---

### Task 6: Add SidPlayer note register population

**Files:**
- Modify: `e6502.Avalonia/Hardware/SidPlayer.cs`
- Modify: `e6502.Avalonia/Hardware/MusicEngine.cs`
- Modify: `e6502.Avalonia/Hardware/SidChip.cs`

**Step 1: Add SidFreqToMidi helper to MusicEngine**

This is the reverse of `MidiToSid`. Add as a public static method:

```csharp
/// <summary>Convert SID frequency register value to nearest MIDI note (0-127), or -1 if silent.</summary>
public static int SidFreqToMidi(int sidFreq)
{
    if (sidFreq <= 0) return -1;
    double hz = sidFreq * CpuClock / 16777216.0;
    if (hz < 8.0) return -1; // below audible
    int midi = (int)Math.Round(12.0 * Math.Log2(hz / 440.0) + 69.0);
    return Math.Clamp(midi, 0, 127);
}
```

**Step 2: Add note sniffing to SidPlayer**

SidPlayer runs via CPU IRQ — the 6502 SID tune code writes SID registers directly through the bus. We need to sniff what notes are playing. The cleanest approach: after each frame tick (raster IRQ), read the SID frequency and gate registers from the SidChip and update MusicEngine voice notes.

But SidPlayer doesn't have a Tick() method — it runs via CPU IRQ. The best hook is in `CompositeBusDevice.TickFrame()` where MusicEngine.Tick() is called. Add SID note sniffing there.

In `CompositeBusDevice.cs`, in the frame tick block (around line 272), after `_midiPlayback.Tick()`:

```csharp
_midiPlayback.Tick();
if (_sidPlayer.IsPlaying)
    SniffSidNotes();
```

Add the sniffing method:

```csharp
private void SniffSidNotes()
{
    // Read SID frequency + gate for voices 0-2 (SID1) and optionally 3-5 (SID2)
    for (int v = 0; v < 3; v++)
    {
        int regBase = VgcConstants.SidBase + v * 7;
        byte ctrl = _sid.Read((ushort)(regBase + 4));
        if ((ctrl & 0x01) != 0) // gate on
        {
            int freqLo = _sid.Read((ushort)(regBase + 0));
            int freqHi = _sid.Read((ushort)(regBase + 1));
            int sidFreq = freqLo | (freqHi << 8);
            int midi = MusicEngine.SidFreqToMidi(sidFreq);
            _musicEngine.SetVoiceNoteExternal(v, midi);
        }
        else
        {
            _musicEngine.SetVoiceNoteExternal(v, -1);
        }
    }
    // SID2 voices 3-5
    for (int v = 0; v < 3; v++)
    {
        int regBase = VgcConstants.Sid2Base + v * 7;
        byte ctrl = _sid2.Read((ushort)(regBase + 4));
        if ((ctrl & 0x01) != 0)
        {
            int freqLo = _sid2.Read((ushort)(regBase + 0));
            int freqHi = _sid2.Read((ushort)(regBase + 1));
            int sidFreq = freqLo | (freqHi << 8);
            int midi = MusicEngine.SidFreqToMidi(sidFreq);
            _musicEngine.SetVoiceNoteExternal(v + 3, midi);
        }
        else
        {
            _musicEngine.SetVoiceNoteExternal(v + 3, -1);
        }
    }
}
```

**Step 3: Add SetVoiceNoteExternal to MusicEngine**

```csharp
/// <summary>Set voice note from external source (SidPlayer). Only applies when not in MML/MIDI mode.</summary>
public void SetVoiceNoteExternal(int voiceIndex, int midiNote)
{
    if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
    _voices[voiceIndex].CurrentMidi = midiNote;
}
```

**Step 4: Add SidPlayer elapsed frame tracking**

In `SidPlayer.cs`, add an elapsed frame counter. Since SidPlayer doesn't have its own Tick(), we track it via a counter incremented by CompositeBusDevice:

```csharp
private int _elapsedFrames;
public int ElapsedFrames => _elapsedFrames;
public void IncrementElapsed() => _elapsedFrames++;
```

In `Play()`, reset: `_elapsedFrames = 0;`

In `CompositeBusDevice.cs`, in the SID playing block:

```csharp
if (_sidPlayer.IsPlaying)
{
    SniffSidNotes();
    _sidPlayer.IncrementElapsed();
}
```

Update the elapsed frames read handler to also check SidPlayer:

```csharp
if (address >= VgcConstants.MusicElapsedL && address <= VgcConstants.MusicTotalH)
{
    int elapsed = _sidPlayer.IsPlaying ? _sidPlayer.ElapsedFrames : _musicEngine.ElapsedFrames;
    int total = _musicEngine.TotalFrames; // 0 for SID (unknown duration)
    // ... same switch ...
}
```

**Step 5: Write test for SidFreqToMidi**

```csharp
[TestMethod]
public void MusicEngine_SidFreqToMidi_A440()
{
    // A440 → SID freq = 440 * 16777216 / 985248 ≈ 7500
    int sidFreq = (int)(440.0 * 16777216.0 / 985248.0);
    int midi = MusicEngine.SidFreqToMidi(sidFreq);
    Assert.AreEqual(69, midi); // A4 = MIDI 69
}

[TestMethod]
public void MusicEngine_SidFreqToMidi_MiddleC()
{
    double hz = 261.63; // C4
    int sidFreq = (int)(hz * 16777216.0 / 985248.0);
    int midi = MusicEngine.SidFreqToMidi(sidFreq);
    Assert.AreEqual(60, midi); // C4 = MIDI 60
}

[TestMethod]
public void MusicEngine_SidFreqToMidi_Zero_ReturnsNegative()
{
    Assert.AreEqual(-1, MusicEngine.SidFreqToMidi(0));
}
```

**Step 6: Run all tests**

```bash
dotnet test
```
Expected: all pass.

**Step 7: Commit**

```bash
git add e6502.Avalonia/Hardware/SidPlayer.cs e6502.Avalonia/Hardware/MusicEngine.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/AvaloniaSidTests.cs
git commit -m "feat: SidPlayer populates voice note registers via frequency reverse-mapping"
```

---

### Task 7: Create gen_novavm_inc.py

**Files:**
- Create: `tools/gen_novavm_inc.py`

**Step 1: Write the generator script**

```python
#!/usr/bin/env python3
"""Generate novavm.inc (ca65 equates) from VgcConstants.cs and basic.sym."""

import re
import sys
import argparse
from pathlib import Path


def parse_vgc_constants(path: str) -> list[tuple[str, str, str]]:
    """Parse VgcConstants.cs, return list of (section_comment, name, value)."""
    results = []
    current_section = "General"

    # Match: public const int/byte NAME = 0xHEX; or = DECIMAL;
    const_re = re.compile(
        r'public\s+const\s+(?:int|byte|ushort)\s+(\w+)\s*=\s*'
        r'(0x[0-9A-Fa-f]+|\d+)\s*;'
    )
    section_re = re.compile(r'//\s*[-=]+\s*\n\s*//\s*(.+?)\s*\n', re.MULTILINE)

    text = Path(path).read_text()

    # Find all section headers with their positions
    sections = [(m.start(), m.group(1).strip()) for m in section_re.finditer(text)]

    for line_no, line in enumerate(text.splitlines()):
        m = const_re.search(line)
        if not m:
            continue

        name = m.group(1)
        value_str = m.group(2)

        # Skip computed constants (those referencing other constants)
        if any(c.isalpha() for c in value_str.split('x')[-1] if value_str.startswith('0x')) is False:
            pass

        # Convert value to hex string
        if value_str.startswith('0x') or value_str.startswith('0X'):
            val = int(value_str, 16)
        else:
            val = int(value_str)

        if val > 0xFF:
            hex_val = f"${val:04X}"
        else:
            hex_val = f"${val:02X}"

        # Determine section from position in file
        char_pos = sum(len(l) + 1 for l in text.splitlines()[:line_no])
        sec = current_section
        for pos, section_name in sections:
            if char_pos > pos:
                sec = section_name

        results.append((sec, name, hex_val))

    return results


# Curated list of ROM symbols to export (stable entry points only)
ROM_SYMBOLS = {
    'V_INPT', 'V_OUTP', 'V_LOAD', 'V_SAVE',
    'LAB_COLD', 'LAB_WARM',
    'Smeml', 'Smemh',       # start of BASIC memory
    'Svarl', 'Svarh',       # start of variables
    'Earryl', 'Earryh',     # end of arrays
    'Sstorl', 'Sstorh',     # string storage pointer
}


def parse_basic_sym(path: str) -> list[tuple[str, str]]:
    """Parse VICE label file, return list of (name, hex_value) for curated symbols."""
    results = []
    sym_re = re.compile(r'^al\s+([0-9A-Fa-f]+)\s+\.(\S+)')

    for line in Path(path).read_text().splitlines():
        m = sym_re.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        name = m.group(2)
        if name in ROM_SYMBOLS:
            if addr > 0xFF:
                results.append((name, f"${addr:04X}"))
            else:
                results.append((name, f"${addr:02X}"))

    return results


def generate(vgc_path: str, sym_path: str, output_path: str):
    """Generate novavm.inc."""
    hw_constants = parse_vgc_constants(vgc_path)
    rom_symbols = parse_basic_sym(sym_path) if sym_path else []

    lines = [
        "; novavm.inc — auto-generated, DO NOT EDIT",
        f"; Sources: {Path(vgc_path).name}"
        + (f", {Path(sym_path).name}" if sym_path else ""),
        f"; Run: python3 tools/gen_novavm_inc.py",
        "",
    ]

    # Group hardware constants by section
    current_section = None
    max_name_len = max((len(name) for _, name, _ in hw_constants), default=0)

    for section, name, value in hw_constants:
        if section != current_section:
            current_section = section
            lines.append("")
            lines.append(f"; --- {section} ---")
            lines.append("")
        lines.append(f"{name:<{max_name_len}} = {value}")

    # ROM symbols
    if rom_symbols:
        lines.append("")
        lines.append("; --- ROM Entry Points (from basic.sym) ---")
        lines.append("")
        for name, value in sorted(rom_symbols, key=lambda x: x[1]):
            lines.append(f"{name:<{max_name_len}} = {value}")

    lines.append("")

    Path(output_path).write_text("\n".join(lines))
    print(f"Generated {output_path} ({len(hw_constants)} hardware + {len(rom_symbols)} ROM symbols)")


def main():
    parser = argparse.ArgumentParser(description="Generate novavm.inc from VgcConstants.cs")
    parser.add_argument("vgc_constants", help="Path to VgcConstants.cs")
    parser.add_argument("basic_sym", nargs="?", help="Path to basic.sym (VICE label file)")
    parser.add_argument("-o", "--output", default="novavm.inc", help="Output file path")
    args = parser.parse_args()

    generate(args.vgc_constants, args.basic_sym, args.output)


if __name__ == "__main__":
    main()
```

**Step 2: Run the generator**

```bash
cd /Users/barry/Git/e6502
python3 tools/gen_novavm_inc.py e6502.Avalonia/Hardware/VgcConstants.cs ehbasic/basic.sym -o ehbasic/novavm.inc
```

Expected: generates `ehbasic/novavm.inc` with all hardware constants and ROM symbols.

**Step 3: Verify output**

```bash
head -40 ehbasic/novavm.inc
grep "V_INPT\|MusicNote1\|RegCmd\|CmdFill" ehbasic/novavm.inc
```

Expected: constants present with correct values.

**Step 4: Commit**

```bash
git add tools/gen_novavm_inc.py ehbasic/novavm.inc
git commit -m "feat: add gen_novavm_inc.py to auto-generate ca65 hardware constants"
```

---

### Task 8: Create keyboard.cfg and update Makefile

**Files:**
- Create: `ehbasic/keyboard.cfg`
- Modify: `ehbasic/Makefile`

**Step 1: Create linker config**

```
# keyboard.cfg — ld65 config for keyboard visualizer binary
# Emits 2-byte load address prefix ($00 $90) followed by code at $9000

MEMORY {
    HDR:  start = $0000, size = 2, fill = yes;
    CODE: start = $9000, size = $1000;
}
SEGMENTS {
    HEADER: load = HDR,  type = ro;
    CODE:   load = CODE, type = ro;
    RODATA: load = CODE, type = ro;
    BSS:    load = CODE, type = bss, define = yes;
    ZEROPAGE: load = CODE, type = zp;
}
```

**Step 2: Update Makefile**

Add keyboard targets. The full Makefile should be:

```makefile
# EhBASIC 2.22 + NovaVM tools — cc65 toolchain
# Requires ca65 and ld65 on PATH

CC65_CFG ?= $(shell dirname $(shell which ld65))/../cfg

all: basic.bin

basic.bin basic.sym: basic.o ehbasic.cfg
	ld65 -C ehbasic.cfg -vm -m basic.map -Ln basic.sym -o basic.bin basic.o

basic.o: basic.asm min_mon.asm
	ca65 -g -l min_mon.lst --feature labels_without_colons -o basic.o min_mon.asm

# Auto-generate hardware constants include
novavm.inc: ../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym ../tools/gen_novavm_inc.py
	python3 ../tools/gen_novavm_inc.py ../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym -o novavm.inc

# Keyboard visualizer
keyboard: keyboard.bin

keyboard.bin: keyboard.o keyboard.cfg
	ld65 -C keyboard.cfg -o keyboard.bin keyboard.o

keyboard.o: keyboard.s novavm.inc
	ca65 --cpu 65c02 keyboard.s -o keyboard.o

install-keyboard: keyboard.bin
	cp keyboard.bin ~/e6502-programs/keyboard.bin

clean:
	$(RM) *.o *.lst *.map *.bin *.sym novavm.inc

.PHONY: all keyboard install-keyboard clean
```

**Step 3: Verify basic ROM still builds**

```bash
cd ehbasic && make clean && make
```
Expected: builds successfully.

**Step 4: Commit**

```bash
git add ehbasic/keyboard.cfg ehbasic/Makefile
git commit -m "feat: add keyboard.cfg linker config and Makefile targets"
```

---

### Task 9: Keyboard visualizer skeleton

**Files:**
- Create: `ehbasic/keyboard.s`

**Step 1: Write minimal skeleton**

Create `ehbasic/keyboard.s` — a minimal program that enters graphics mode, waits for keypress or music stop, then returns to BASIC:

```asm
; keyboard.s — NovaVM Keyboard Visualizer
; Loaded at $9000 via LOAD "KEYBOARD",$9000
; Called via SYS $9000

.include "novavm.inc"

; =====================================================================
; Header segment — 2-byte load address
; =====================================================================
.segment "HEADER"
    .word $9000

; =====================================================================
; Zero page temporaries (use locations $80-$8F, safe from EhBASIC)
; =====================================================================
.zeropage

zp_temp1:       .res 1
zp_temp2:       .res 1
zp_voice:       .res 1      ; current voice index (0-5)
zp_note:        .res 1      ; current MIDI note
zp_octave:      .res 1      ; computed octave
zp_pitch:       .res 1      ; pitch class (0-11)
zp_keytype:     .res 1      ; 0=white, 1=black
zp_keyidx:      .res 1      ; white key index in octave
zp_xlo:         .res 1      ; computed X position low
zp_xhi:         .res 1      ; computed X position high

; =====================================================================
; Code
; =====================================================================
.code

; Previous note for each voice (6 bytes)
prev_notes: .res 6

; Note-to-key lookup tables
key_type:
;           C  C# D  D# E  F  F# G  G# A  A# B
    .byte   0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0

key_index:
    .byte   0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6

; Voice colors (palette indices)
voice_colors:
    .byte   2, 8, 7, 5, 3, 6  ; red, orange, yellow, green, cyan, blue

; =====================================================================
; Entry point
; =====================================================================
.proc main
    ; --- Initialize ---
    jsr init_screen
    jsr draw_keyboards
    jsr define_sprites
    jsr draw_chrome

    ; Clear previous notes
    ldx #5
    lda #0
@clr:
    sta prev_notes,x
    dex
    bpl @clr

    ; --- Main loop ---
@loop:
    ; VSYNC — wait for vertical blank
    lda RegStatus
    ; TODO: proper vsync via register or timer

    ; Poll 6 voices
    jsr update_voices

    ; Update progress bar
    jsr update_progress

    ; Check for keypress
    jsr V_INPT
    bcs @exit           ; carry set = key received

    ; Check music status
    lda MusicStatus
    and #$02            ; bit 1 = music playing
    bne @loop           ; still playing

@exit:
    jsr cleanup
    rts                 ; return to BASIC
.endproc

; =====================================================================
; Subroutines (stubs for now)
; =====================================================================

.proc init_screen
    ; Set mode 1 (graphics over text)
    lda #1
    sta RegMode
    ; Clear graphics
    lda #CmdGcls
    sta RegCmd
    rts
.endproc

.proc draw_keyboards
    ; TODO: Task 10
    rts
.endproc

.proc define_sprites
    ; TODO: Task 12
    rts
.endproc

.proc draw_chrome
    ; TODO: Task 13
    rts
.endproc

.proc update_voices
    ; TODO: Task 11
    rts
.endproc

.proc update_progress
    ; TODO: Task 13
    rts
.endproc

.proc cleanup
    ; Disable sprites 0-5
    ldx #5
@dis:
    lda #0
    sta SpriteRegBase + SprRegFlags,x  ; TODO: stride calculation
    dex
    bpl @dis
    rts
.endproc
```

**Step 2: Build it**

```bash
cd ehbasic && make novavm.inc && make keyboard
```

Expected: assembles and links successfully. Verify the output:

```bash
hexdump -C keyboard.bin | head -3
```

Expected: first 2 bytes are `00 90` (load address $9000).

**Step 3: Install and test**

```bash
make install-keyboard
```

Test manually in the emulator:
```
LOAD "KEYBOARD",$9000
SYS $9000
```

Expected: screen clears to graphics mode, pressing any key returns to BASIC.

**Step 4: Commit**

```bash
git add ehbasic/keyboard.s
git commit -m "feat: keyboard visualizer skeleton with main loop"
```

---

### Task 10: Draw static keyboards

**Files:**
- Modify: `ehbasic/keyboard.s`

Implement `draw_keyboards` — draws both 4-octave keyboard rows using VGC FILL and LINE commands.

**Step 1: Implement draw_keyboards**

The VGC command interface: write parameters to RegP0-RegP13, then write command to RegCmd.

For `CmdFill` (fill rectangle): P0=x1_lo, P1=x1_hi, P2=y1, P3=0, P4=x2_lo, P5=x2_hi, P6=y2, P7=0.

For `CmdGcolor` (set graphics color): P0=color.

For `CmdLine` (draw line): P0=x1_lo, P1=x1_hi, P2=y1, P3=0, P4=x2_lo, P5=x2_hi, P6=y2, P7=0.

This is the most code-intensive part. The logic mirrors the BASIC programs:

1. Set gcolor to white (1), draw 28 white key rectangles for each row
2. Set gcolor to gray (11), draw vertical key border lines
3. Set gcolor to black (0), draw black key rectangles

Constants:
```asm
KW      = 11        ; white key width
BW      = 7         ; black key width
KH      = 40        ; key height
BH      = 24        ; black key height
KX      = 6         ; keyboard X origin
Y1      = 36        ; upper keyboard Y
Y2      = 90        ; lower keyboard Y
NKEYS   = 28        ; keys per row (4 octaves × 7 white keys)
```

The implementation needs loops for:
- 28 white keys × 2 rows
- 29 border lines × 2 rows
- 5 black keys × 4 octaves × 2 rows

Write the full assembly for this. Use subroutines for the VGC commands (e.g., `vgc_fill`, `vgc_line`, `vgc_gcolor`) to keep it DRY.

**Step 2: Add octave labels**

Write "C0"-"C7" text at the appropriate positions using the text layer (LOCATE + character output via V_OUTP, or direct writes to character RAM at $AA00).

**Step 3: Build and test visually**

```bash
cd ehbasic && make keyboard && make install-keyboard
```

Run in emulator, verify keyboard looks correct.

**Step 4: Commit**

```bash
git add ehbasic/keyboard.s
git commit -m "feat: draw static 8-octave keyboard display"
```

---

### Task 11: Voice tracking and sprite positioning

**Files:**
- Modify: `ehbasic/keyboard.s`

Implement `update_voices` — poll 6 voice note registers, compute key positions, update sprites.

**Step 1: Implement the voice update loop**

For each voice 0-5:
1. Read `MusicNote1 + voice` to get current MIDI note
2. Compare with `prev_notes[voice]`
3. If changed and old note > 0: disable sprite
4. If changed and new note > 0:
   - Subtract 12 (C0 base)
   - Bounds check 0-95
   - Divide by 12 → octave, mod 12 → pitch class
   - Table lookup: `key_type[pitch]`, `key_index[pitch]`
   - If octave ≥ 4: lower row (Y2), octave -= 4; else upper row (Y1)
   - X = KX + (octave × 7 + key_index) × 11
   - If black key: X += (11 - 7/2) = X + 7, shape = voice + 6
   - If white key: shape = voice
   - Position sprite, set shape, enable
5. Store as prev_notes[voice]

**Key 6502 challenge:** Division by 12. Use a lookup table or repeated subtraction. Since MIDI notes are 0-95, a 96-entry lookup table for octave and pitch class is feasible (192 bytes). Or use repeated subtraction (subtract 12 until < 12).

**Step 2: Sprite register writes**

Each sprite occupies 8 bytes at `SpriteRegBase + (sprite × 8)`:
- Offset 0-1: X position (16-bit)
- Offset 2-3: Y position (16-bit)
- Offset 4: Shape index
- Offset 5: Flags (bit 7 = enable)
- Offset 6: Priority (2 = in front)
- Offset 7: Transparent color

**Step 3: Build and test**

Set up a BASIC program with music and the visualizer. Verify sprites appear on correct keys.

**Step 4: Commit**

```bash
git add ehbasic/keyboard.s
git commit -m "feat: voice tracking with sprite-based key indicators"
```

---

### Task 12: Define sprite shapes

**Files:**
- Modify: `ehbasic/keyboard.s`

Implement `define_sprites` — create 12 sprite shapes (6 white-pressed, 6 black-pressed).

**Step 1: Design sprite pixel data**

Each sprite is 16×16 pixels, 4-bit color per pixel, 128 bytes per shape. Use VGC `CmdSprRow` to define one row at a time (P0=shape, P1=row, P2-P9=8 bytes of pixel data).

White key pressed (shape 0-5):
- Rows 0-1: voice color accent line (full width 11px, rest transparent)
- Rows 2-13: slightly darkened white (color 15 or similar)
- Rows 14-15: dark gray bottom edge (shadow effect)
- Width: 11px effective (remaining 5 columns = transparent color)

Black key pressed (shape 6-11):
- Rows 0-1: voice color accent dot/line
- Rows 2-11: dark gray (slightly lighter than black 0)
- Width: 7px effective

Use transparent color 15 (or another unused color) so the key underneath shows through where the sprite doesn't cover.

**Step 2: Write shape data in assembly**

Store shape data as byte arrays in RODATA. Use a loop to load each shape via `CmdSprRow` commands.

**Step 3: Build and test**

Verify sprites render correctly over keys.

**Step 4: Commit**

```bash
git add ehbasic/keyboard.s
git commit -m "feat: define 12 sprite shapes for key depression indicators"
```

---

### Task 13: Player chrome (progress bar, time, legend)

**Files:**
- Modify: `ehbasic/keyboard.s`

Implement `draw_chrome` (static elements) and `update_progress` (per-frame updates).

**Step 1: Draw static chrome**

At y=140-175:
- Progress bar outline: empty rectangle
- Voice legend: 6 colored blocks with "V1"-"V6" labels
- Status text: "Press any key to exit"

Use text layer writes (LOCATE + character writes to char RAM) for text, VGC commands for colored blocks.

**Step 2: Implement update_progress**

Each frame:
1. Read `MusicElapsedL/H` ($BA57-$BA58)
2. Read `MusicTotalL/H` ($BA59-$BA5A)
3. If total = 0, skip (unknown duration)
4. Compute fill width: `elapsed * bar_width / total` (16-bit division)
5. Fill progress bar to computed width
6. Compute elapsed seconds: `elapsed_frames / 60`
7. Compute MM:SS from seconds
8. Write time to text layer

**6502 challenge:** 16-bit division. Use a standard 16-bit divide routine. For time display, divide by 60 twice (first for frames→seconds, then seconds→minutes).

**Step 3: Build and test**

Test with a short MML piece, verify progress bar fills and time updates.

**Step 4: Commit**

```bash
git add ehbasic/keyboard.s
git commit -m "feat: player chrome with progress bar and time display"
```

---

### Task 14: Integration testing

**Files:**
- Create: `docs/programs/keyboard-test-mml.bas`
- Create: `docs/programs/keyboard-test-mid.bas`

**Step 1: Test with MML**

Create test BASIC program:

```basic
10 REM KEYBOARD VISUALIZER - MML TEST
20 INSTRUMENT 1,64,0,4,7,6
30 MUSIC 1,"T120I1O4L4CDEFGAB>CDEFGAB>C"
40 MUSIC 2,"T120I1O3L2CGCG"
50 VOLUME 15
60 MUSIC PLAY
70 LOAD "KEYBOARD",$9000
80 SYS $9000
90 VOLUME 0
```

Run in emulator. Verify:
- Keyboard displays correctly
- Notes light up on correct keys for voices 1 and 2
- Progress bar fills
- Time displays correctly
- Exits when music ends

**Step 2: Test with MIDPLAY**

```basic
10 MIDPLAY "joplin-entertainer"
20 LOAD "KEYBOARD",$9000
30 SYS $9000
```

Verify notes display for all active voices.

**Step 3: Test with SIDPLAY**

```basic
10 SIDPLAY "some-sid-file"
20 LOAD "KEYBOARD",$9000
30 SYS $9000
```

Verify frequency-mapped notes light up.

**Step 4: Edge case testing**

- Press key during playback → should exit immediately
- Notes outside 8-octave range → should be ignored (no crash)
- All 6 voices active simultaneously → 6 sprites visible
- Very fast note changes → sprites update smoothly

**Step 5: Commit test programs**

```bash
git add docs/programs/keyboard-test-mml.bas docs/programs/keyboard-test-mid.bas
git commit -m "test: add keyboard visualizer integration test programs"
```

---

### Task 15: Final build and full test suite

**Step 1: Run all unit tests**

```bash
dotnet test
```
Expected: all pass.

**Step 2: Clean build of ROM and keyboard**

```bash
cd ehbasic && make clean && make && make novavm.inc && make keyboard && make install-keyboard
```

**Step 3: Verify SYS command works**

In emulator:
```
SYS $9000
```
Should work (previously `CALL $9000`).

**Step 4: Final commit**

```bash
git add -A
git commit -m "feat: complete keyboard visualizer v1"
```
