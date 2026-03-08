# Unified Voice Routing Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Unify SID and WTS into a single coherent music engine with explicit instrument commands (`I`, `@I`, `@D`), MIDPLAY voice mapping syntax, and fixed engine boundaries (voices 1-6 SID, 7-14 WTS).

**Architecture:** Replace `@W` MML command with `@I`/`@D`, add voice=channel mapping to MIDPLAY via C#-side string parsing (no ROM changes), update MusicEngine to guard SID/WTS commands by voice index, update RouteVoices to accept explicit voice-to-channel mappings.

**Tech Stack:** C# (.NET 10), MSTest, 65C02 assembly (ca65)

---

### Task 1: Replace @W with @I and add @D in MML parser

**Files:**
- Modify: `e6502.Avalonia/Hardware/MmlParser.cs:250-256`
- Test: `e6502UnitTests/MmlParserWtsTests.cs`

**Step 1: Update tests to use new syntax**

Replace the existing `MmlParserWtsTests.cs` with tests for the new commands:

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class MmlParserWtsTests
{
    [TestMethod]
    public void AtI_ParsesWtsInstrumentProgram()
    {
        var events = MmlParser.Parse("@I5 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(MmlEventType.SetWtsInstrument, setWts.Type);
        Assert.AreEqual(5, setWts.Param1);
    }

    [TestMethod]
    public void AtI_DefaultsToZero()
    {
        var events = MmlParser.Parse("@I C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(MmlEventType.SetWtsInstrument, setWts.Type);
        Assert.AreEqual(0, setWts.Param1);
    }

    [TestMethod]
    public void AtI_MaxProgram127()
    {
        var events = MmlParser.Parse("@I127 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(127, setWts.Param1);
    }

    [TestMethod]
    public void AtD_ParsesDrumKit()
    {
        var events = MmlParser.Parse("@D0 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(MmlEventType.SetWtsInstrument, setWts.Type);
        // Bank 128 encoded: (128 << 8) | 0 = 32768
        Assert.AreEqual(32768, setWts.Param1);
    }

    [TestMethod]
    public void AtD_ParsesDrumKitWithProgram()
    {
        var events = MmlParser.Parse("@D25 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        // (128 << 8) | 25 = 32793
        Assert.AreEqual(32793, setWts.Param1);
    }

    [TestMethod]
    public void AtW_NoLongerRecognized()
    {
        // @W should be silently ignored (unknown @ command)
        var events = MmlParser.Parse("@W5 C4");
        Assert.IsFalse(events.Any(e => e.Type == MmlEventType.SetWtsInstrument));
    }

    [TestMethod]
    public void AtI_DoesNotInterfereWithAtP()
    {
        var events = MmlParser.Parse("@P2048 @I5 C4");
        Assert.IsTrue(events.Any(e => e.Type == MmlEventType.SetPulseWidth));
        Assert.IsTrue(events.Any(e => e.Type == MmlEventType.SetWtsInstrument));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MmlParserWtsTests" --verbosity normal`
Expected: `AtW_NoLongerRecognized` fails (still recognizes @W), `AtI_*` and `AtD_*` fail (not yet implemented)

**Step 3: Update MmlParser**

In `e6502.Avalonia/Hardware/MmlParser.cs`, replace lines 250-256 (the `@W` handler) with `@I` and `@D`:

```csharp
                if (sub == 'I')
                {
                    pos++;
                    int program = ReadInt(mml, ref pos, 0);
                    events.Add(new MmlEvent(MmlEventType.SetWtsInstrument, program & 0x7F));
                    continue;
                }

                if (sub == 'D')
                {
                    pos++;
                    int program = ReadInt(mml, ref pos, 0);
                    events.Add(new MmlEvent(MmlEventType.SetWtsInstrument, (128 << 8) | (program & 0x7F)));
                    continue;
                }
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MmlParserWtsTests" --verbosity normal`
Expected: All 7 tests PASS

**Step 5: Run all tests**

Run: `dotnet test`
Expected: All pass. Some other tests that used `@W` may break — check `MusicEngineTests.cs` for any `@W` usage and update to `@I`.

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/MmlParser.cs e6502UnitTests/MmlParserWtsTests.cs
git commit -m "feat: replace @W with @I and @D MML commands for WTS instruments"
```

---

### Task 2: Guard instrument events by voice type in MusicEngine

**Files:**
- Modify: `e6502.Avalonia/Hardware/MusicEngine.cs:650-652,732-734`
- Test: `e6502UnitTests/MusicEngineTests.cs`

**Step 1: Write failing tests**

Add to `e6502UnitTests/MusicEngineTests.cs`:

```csharp
[TestMethod]
public void SetInstrument_IgnoredOnWtsVoice()
{
    // Voice 7 (index 6) is WTS — SetInstrument (SID) should be ignored
    // Load MML with I1 into voice 7 and verify it doesn't crash
    // and the WTS instrument ID remains unchanged
    var engine = CreateTestEngine();
    engine.DefineInstrument(1, 0x40, 0, 9, 0, 6);
    engine.SetSequence(6, "I1 C4"); // voice index 6 = voice 7 (WTS)
    engine.MusicPlay();
    engine.Tick(); // should not crash, SID instrument ignored
}

[TestMethod]
public void SetWtsInstrument_IgnoredOnSidVoice()
{
    // Voice 1 (index 0) is SID — SetWtsInstrument should be ignored
    var engine = CreateTestEngine();
    engine.SetSequence(0, "@I5 C4"); // voice index 0 = voice 1 (SID)
    engine.MusicPlay();
    engine.Tick(); // should not crash, WTS instrument ignored
}
```

Note: Check how `CreateTestEngine()` or equivalent test helper is set up in the existing test file. Adapt the helper name to match what exists.

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MusicEngineTests" --verbosity normal`
Expected: Tests may pass or fail depending on current behavior — if they crash, that confirms the issue.

**Step 3: Add guards in ProcessEvent**

In `e6502.Avalonia/Hardware/MusicEngine.cs`:

At line 650 (SetInstrument handler), add voice guard:
```csharp
            case MmlEventType.SetInstrument:
                if (!IsWtsVoice(vi))
                    vs.CurrentInstrument = GetInstrument(ev.Param1);
                break;
```

At line 732 (SetWtsInstrument handler), add voice guard:
```csharp
            case MmlEventType.SetWtsInstrument:
                if (IsWtsVoice(vi))
                    vs.WtsInstrumentId = ev.Param1;
                break;
```

Also guard SID-specific effects. At line 666 (SetPulseWidth):
```csharp
            case MmlEventType.SetPulseWidth:
                if (IsWtsVoice(vi)) break;
                vs.PulseWidth = ev.Param1;
                vs.PwmDir     = 0;
                WritePulse(vi, vs.PulseWidth);
                break;
```

At line 672 (PwmSweep):
```csharp
            case MmlEventType.PwmSweep:
                if (IsWtsVoice(vi)) break;
                vs.PwmDir = ev.Param1;
                break;
```

At line 681 (SetFilterCutoff):
```csharp
            case MmlEventType.SetFilterCutoff:
            {
                if (IsWtsVoice(vi)) break;
                // ... rest unchanged
```

At line 697 (FilterMode):
```csharp
            case MmlEventType.FilterMode:
            {
                if (IsWtsVoice(vi)) break;
                // ... rest unchanged
```

At line 712 (FilterSweep):
```csharp
            case MmlEventType.FilterSweep:
                if (IsWtsVoice(vi)) break;
                vs.FilterSweepDir = ev.Param1;
                break;
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MusicEngineTests" --verbosity normal`
Expected: All PASS

**Step 5: Run all tests**

Run: `dotnet test`
Expected: All pass

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/MusicEngine.cs e6502UnitTests/MusicEngineTests.cs
git commit -m "feat: guard SID/WTS instrument and effect commands by voice type"
```

---

### Task 3: Add MIDPLAY mapping string parser to FileIoController

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs:999-1078`
- Test: `e6502UnitTests/MidPlayMappingTests.cs` (create)

**Step 1: Write failing tests**

Create `e6502UnitTests/MidPlayMappingTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace e6502UnitTests;

[TestClass]
public class MidPlayMappingTests
{
    [TestMethod]
    public void ParseMidPlayString_NoMapping()
    {
        var (filename, mapping) = MidPlayParser.Parse("sousa-stars-stripes");
        Assert.AreEqual("sousa-stars-stripes", filename);
        Assert.IsNull(mapping);
    }

    [TestMethod]
    public void ParseMidPlayString_SingleMapping()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(1, mapping!.Count);
        Assert.AreEqual(0, mapping[6]); // voice 7 (1-based) = index 6 (0-based), channel 0
    }

    [TestMethod]
    public void ParseMidPlayString_MultipleMapping()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0,8=1,1=9");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(3, mapping!.Count);
        Assert.AreEqual(0, mapping[6]);  // voice 7 → ch 0
        Assert.AreEqual(1, mapping[7]);  // voice 8 → ch 1
        Assert.AreEqual(9, mapping[0]);  // voice 1 → ch 9
    }

    [TestMethod]
    public void ParseMidPlayString_InvalidPairIgnored()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0,bad,8=1");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(2, mapping!.Count);
    }

    [TestMethod]
    public void ParseMidPlayString_VoiceOutOfRangeIgnored()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,0=0,15=1,7=2");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        // voice 0 and 15 out of 1-14 range, only voice 7 valid
        Assert.AreEqual(1, mapping!.Count);
        Assert.AreEqual(2, mapping[6]);
    }

    [TestMethod]
    public void ParseMidPlayString_TrailingComma()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0,");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(1, mapping!.Count);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidPlayMappingTests" --verbosity normal`
Expected: FAIL — `MidPlayParser` class doesn't exist yet

**Step 3: Implement MidPlayParser**

Create `e6502.Avalonia/Hardware/MidPlayParser.cs`:

```csharp
using System.Collections.Generic;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Parses MIDPLAY filename strings with optional voice=channel mapping.
/// Format: "filename" or "filename,voice=channel,voice=channel,..."
/// Voice numbers are 1-based (1-14), converted to 0-based internally.
/// </summary>
public static class MidPlayParser
{
    public static (string Filename, Dictionary<int, int>? Mapping) Parse(string input)
    {
        int commaIdx = input.IndexOf(',');
        if (commaIdx < 0)
            return (input, null);

        string filename = input[..commaIdx];
        string rest = input[(commaIdx + 1)..];

        var mapping = new Dictionary<int, int>();
        foreach (var pair in rest.Split(','))
        {
            var parts = pair.Split('=');
            if (parts.Length != 2) continue;
            if (!int.TryParse(parts[0].Trim(), out int voice)) continue;
            if (!int.TryParse(parts[1].Trim(), out int channel)) continue;
            if (voice < 1 || voice > 14) continue;
            if (channel < 0 || channel > 15) continue;
            mapping[voice - 1] = channel; // convert to 0-based
        }

        return (filename, mapping.Count > 0 ? mapping : null);
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidPlayMappingTests" --verbosity normal`
Expected: All 6 tests PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidPlayParser.cs e6502UnitTests/MidPlayMappingTests.cs
git commit -m "feat: add MidPlayParser for MIDPLAY voice=channel mapping syntax"
```

---

### Task 4: Update RouteVoices to accept explicit voice-to-channel mapping

**Files:**
- Modify: `e6502.Avalonia/Hardware/MidiEngine.cs:449-531`
- Test: `e6502UnitTests/MidiEngineRoutingTests.cs` (create)

**Step 1: Write failing tests**

Create `e6502UnitTests/MidiEngineRoutingTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace e6502UnitTests;

[TestClass]
public class MidiEngineRoutingTests
{
    private static MidiEngine.ChannelInfo[] MakeAnalysis(params (int ch, int notes, int program, bool drums)[] channels)
    {
        var analysis = new MidiEngine.ChannelInfo[16];
        for (int i = 0; i < 16; i++)
            analysis[i] = new MidiEngine.ChannelInfo { Channel = i };
        foreach (var (ch, notes, program, drums) in channels)
        {
            analysis[ch].NoteCount = notes;
            analysis[ch].GmProgram = program;
        }
        return analysis;
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_VoiceGetsCorrectChannel()
    {
        var analysis = MakeAnalysis((0, 100, 0, false), (1, 80, 40, false), (9, 50, 0, true));
        var rawChannels = new[] { 0, 1, 9 };
        var explicit_map = new Dictionary<int, int>
        {
            [6] = 0,  // voice 7 (0-based 6) → ch 0
            [7] = 1,  // voice 8 → ch 1
            [0] = 9,  // voice 1 → ch 9 (drums on SID)
        };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false,
            (_, _) => { }, explicit_map);

        Assert.AreEqual(0, result.VoiceToChannel[6]);  // voice 7 → ch 0
        Assert.AreEqual(1, result.VoiceToChannel[7]);  // voice 8 → ch 1
        Assert.AreEqual(9, result.VoiceToChannel[0]);  // voice 1 → ch 9
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_WtsVoiceGetsGmProgram()
    {
        var analysis = MakeAnalysis((0, 100, 42, false));
        var rawChannels = new[] { 0 };
        var explicit_map = new Dictionary<int, int> { [6] = 0 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false,
            (_, _) => { }, explicit_map);

        // WTS voice: instrument slot = (bank << 8) | program = (0 << 8) | 42 = 42
        Assert.AreEqual(42, result.InstrumentSlots[6]);
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_SidVoiceGetsBucket()
    {
        var analysis = MakeAnalysis((0, 100, 0, false));
        var rawChannels = new[] { 0 };
        var explicit_map = new Dictionary<int, int> { [0] = 0 };
        int definedSlot = -1;

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false,
            (slot, _) => { definedSlot = slot; }, explicit_map);

        Assert.AreEqual(0, result.VoiceToChannel[0]);
        Assert.IsTrue(result.InstrumentSlots[0] > 0); // SID bucket assigned
        Assert.IsTrue(definedSlot > 0); // defineInstrument was called
    }

    [TestMethod]
    public void RouteVoices_NoExplicitMapping_AutoRoutesAsDefault()
    {
        var analysis = MakeAnalysis((0, 100, 0, false), (1, 80, 40, false));
        var rawChannels = new[] { 0, 1 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false,
            (_, _) => { }, null);

        // Auto: channels go to WTS voices 6+
        Assert.AreEqual(0, result.VoiceToChannel[6]);
        Assert.AreEqual(1, result.VoiceToChannel[7]);
    }

    [TestMethod]
    public void RouteVoices_ExplicitMapping_DrumOnWtsGetsBankEncoding()
    {
        var analysis = MakeAnalysis((9, 100, 0, true));
        // Channel 9 is drums (IsDrums is determined by channel == 9 in ChannelInfo)
        var rawChannels = new[] { 9 };
        var explicit_map = new Dictionary<int, int> { [6] = 9 };

        var result = MidiEngine.RouteVoices(analysis, rawChannels, false,
            (_, _) => { }, explicit_map);

        // WTS drum: (128 << 8) | 0 = 32768
        Assert.AreEqual(32768, result.InstrumentSlots[6]);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngineRoutingTests" --verbosity normal`
Expected: FAIL — `RouteVoices` doesn't accept a mapping parameter yet

**Step 3: Update RouteVoices signature and implementation**

In `e6502.Avalonia/Hardware/MidiEngine.cs`, update the `RouteVoices` method (line 449):

```csharp
    public static RoutingResult RouteVoices(
        ChannelInfo[] analysis, int[] rawChannels, bool sidOnly,
        Action<int, InstrumentBucket> defineInstrument,
        Dictionary<int, int>? explicitVoiceMapping = null)
    {
        var result = new RoutingResult { SidOnly = sidOnly };

        if (sidOnly)
        {
            // SID-only: if explicit mapping provided, use it; otherwise rawChannels
            result.VoiceToChannel = new int[6];
            result.InstrumentSlots = new int[6];
            for (int i = 0; i < 6; i++)
                result.VoiceToChannel[i] = -1;

            if (explicitVoiceMapping is { Count: > 0 })
            {
                foreach (var (voice, ch) in explicitVoiceMapping)
                {
                    if (voice < 0 || voice >= 6) continue; // SID only
                    result.VoiceToChannel[voice] = ch;
                }
            }
            else
            {
                for (int i = 0; i < rawChannels.Length && i < 6; i++)
                    result.VoiceToChannel[i] = rawChannels[i];
            }

            // Assign SID instrument buckets
            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;
            for (int v = 0; v < 6; v++)
            {
                int ch = result.VoiceToChannel[v];
                if (ch < 0) continue;
                var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                int bucketIdx = Array.IndexOf(Buckets, bucket);
                if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                {
                    slot = nextSlot++;
                    registeredBuckets[bucketIdx] = slot;
                    defineInstrument(slot, bucket);
                }
                result.InstrumentSlots[v] = slot;
            }
        }
        else if (explicitVoiceMapping is { Count: > 0 })
        {
            // Explicit mapping: user specifies voice→channel
            result.VoiceToChannel = new int[14];
            result.InstrumentSlots = new int[14];
            for (int i = 0; i < 14; i++)
                result.VoiceToChannel[i] = -1;

            // Apply explicit assignments
            var usedChannels = new HashSet<int>();
            foreach (var (voice, ch) in explicitVoiceMapping)
            {
                if (voice < 0 || voice >= 14) continue;
                result.VoiceToChannel[voice] = ch;
                usedChannels.Add(ch);
            }

            // Auto-route remaining channels to remaining voices
            var remainingChannels = new List<int>();
            foreach (int ch in rawChannels)
            {
                if (!usedChannels.Contains(ch))
                    remainingChannels.Add(ch);
            }

            // Fill remaining WTS voices first (6-13), then SID (0-5)
            int rcIdx = 0;
            for (int v = 6; v < 14 && rcIdx < remainingChannels.Count; v++)
            {
                if (result.VoiceToChannel[v] >= 0) continue;
                result.VoiceToChannel[v] = remainingChannels[rcIdx++];
            }
            for (int v = 0; v < 6 && rcIdx < remainingChannels.Count; v++)
            {
                if (result.VoiceToChannel[v] >= 0) continue;
                result.VoiceToChannel[v] = remainingChannels[rcIdx++];
            }

            // Assign instruments based on voice type
            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;
            for (int v = 0; v < 14; v++)
            {
                int ch = result.VoiceToChannel[v];
                if (ch < 0) continue;

                if (v >= 6)
                {
                    int bank = analysis[ch].IsDrums ? 128 : 0;
                    result.InstrumentSlots[v] = (bank << 8) | (analysis[ch].GmProgram & 0x7F);
                }
                else
                {
                    var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                    int bucketIdx = Array.IndexOf(Buckets, bucket);
                    if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                    {
                        slot = nextSlot++;
                        registeredBuckets[bucketIdx] = slot;
                        defineInstrument(slot, bucket);
                    }
                    result.InstrumentSlots[v] = slot;
                }
            }
        }
        else
        {
            // Auto mode (existing behavior): WTS first, SID overflow
            int wtsCount = Math.Min(rawChannels.Length, 8);
            int sidCount = rawChannels.Length - wtsCount;
            int totalVoices = 6 + wtsCount;

            result.VoiceToChannel = new int[totalVoices];
            result.InstrumentSlots = new int[totalVoices];

            for (int i = sidCount; i < 6; i++)
                result.VoiceToChannel[i] = -1;

            for (int i = 0; i < sidCount; i++)
                result.VoiceToChannel[i] = rawChannels[wtsCount + i];

            for (int i = 0; i < wtsCount; i++)
                result.VoiceToChannel[6 + i] = rawChannels[i];

            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;

            for (int v = 0; v < totalVoices; v++)
            {
                int ch = result.VoiceToChannel[v];
                if (ch < 0) continue;

                if (v >= 6)
                {
                    int bank = analysis[ch].IsDrums ? 128 : 0;
                    result.InstrumentSlots[v] = (bank << 8) | (analysis[ch].GmProgram & 0x7F);
                }
                else
                {
                    var bucket = GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                    int bucketIdx = Array.IndexOf(Buckets, bucket);
                    if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                    {
                        slot = nextSlot++;
                        registeredBuckets[bucketIdx] = slot;
                        defineInstrument(slot, bucket);
                    }
                    result.InstrumentSlots[v] = slot;
                }
            }
        }

        return result;
    }
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngineRoutingTests" --verbosity normal`
Expected: All 5 tests PASS

**Step 5: Run all tests**

Run: `dotnet test`
Expected: All pass — existing callers pass `null` for the new parameter (default value)

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiEngine.cs e6502UnitTests/MidiEngineRoutingTests.cs
git commit -m "feat: RouteVoices accepts explicit voice-to-channel mapping"
```

---

### Task 5: Wire MidPlayParser into FileIoController and EmulatorTcpServer

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs:1060-1077`
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs:1140-1168`

**Step 1: Update FileIoController.DoMidPlay**

In `e6502.Avalonia/Hardware/FileIoController.cs`, after the MIDI file is read (around line 1060), insert mapping parsing. Replace the block from `var midi = ...` through `_midiPlayback.Play(...)`:

```csharp
            var midi = Melanchall.DryWetMidi.Core.MidiFile.Read(path);
            var analysis = MidiEngine.AnalyzeChannels(midi);
            var mode = _midiPlayback.RoutingMode;

            // Auto-load soundfont if WTS is empty and not in SID-only mode
            if (mode != MidiRoutingMode.SidOnly && _wts is not null && _wts.InstrumentCount == 0)
                MidiAutoSoundfont.TryLoad(this, _wts);

            bool sidOnly = mode == MidiRoutingMode.SidOnly || (_wts is null || _wts.InstrumentCount == 0);
            int maxVoices = sidOnly ? 6 : 14;
            var rawChannels = MidiEngine.SelectChannels(midi, maxVoices);

            var routing = MidiEngine.RouteVoices(analysis, rawChannels, sidOnly,
                (slot, bucket) => _musicEngine.DefineInstrument(slot, bucket.Waveform,
                    bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release),
                mapping);

            _musicEngine.SetVolume(15);
            _midiPlayback.Play(midi, routing.VoiceToChannel, routing.InstrumentSlots);
```

The `mapping` variable comes from parsing the filename. Earlier in `DoMidPlay`, after `ReadFilename()` returns and before resolving the path, add:

```csharp
            string? filename = ReadFilename();
            if (filename is null) { SetError(VgcConstants.FioErrIo); return; }

            // Parse optional voice mapping from filename string
            Dictionary<int, int>? mapping = null;
            var parsed = MidPlayParser.Parse(filename);
            filename = parsed.Filename;
            mapping = parsed.Mapping;
```

**Step 2: Update EmulatorTcpServer.CmdMidPlay**

In `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`, update the voices JSON parsing (lines 1149-1158) to produce a voice-to-channel mapping (0-based voice → channel) and pass it to RouteVoices:

```csharp
        Dictionary<int, int>? voiceMapping = null;
        if (req["voices"] is JsonNode voicesNode)
        {
            voiceMapping = new Dictionary<int, int>();
            foreach (var prop in voicesNode.AsObject())
            {
                // Keys are 1-based voice numbers, values are MIDI channels
                if (int.TryParse(prop.Key, out int voice) && voice >= 1 && voice <= 14)
                    voiceMapping[voice - 1] = prop.Value!.GetValue<int>();
            }
            if (voiceMapping.Count == 0) voiceMapping = null;
        }

        int maxVoices = sidOnly ? 6 : 14;
        var rawChannels = MidiEngine.SelectChannels(midi, maxVoices);

        var routing = MidiEngine.RouteVoices(analysis, rawChannels, sidOnly,
            (slot, bucket) => _bus.Music.DefineInstrument(slot, bucket.Waveform,
                bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release),
            voiceMapping);
```

Remove the old `explicitMapping` parameter from `SelectChannels` — it no longer handles voice mapping.

**Step 3: Build and run all tests**

Run: `dotnet build && dotnet test`
Expected: All pass

**Step 4: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502.Avalonia/Ipc/EmulatorTcpServer.cs
git commit -m "feat: wire MidPlayParser into FileIoController and TCP server"
```

---

### Task 6: Update any remaining @W references

**Files:**
- Search all `.cs` files for `@W` MML usage and `SetWtsInstrument` with old encoding

**Step 1: Search for @W references**

Run: `grep -rn "@W" e6502.Avalonia/ e6502UnitTests/ --include="*.cs"`
Run: `grep -rn "SetWtsInstrument" e6502.Avalonia/ e6502UnitTests/ --include="*.cs"`

**Step 2: Update any found references**

- In MidiEngine/MidiPlayback: if instrument slots are passed to MusicEngine and they set `WtsInstrumentId`, verify the encoding still works with the new `@I`/`@D` scheme.
- In any test files: replace `@W` with `@I` in MML test strings.
- In MidiPlayback.cs: check if it generates MML with `@W` — if so, update to `@I`/`@D`.

**Step 3: Run all tests**

Run: `dotnet test`
Expected: All 1025+ tests pass

**Step 4: Commit**

```bash
git add -u
git commit -m "chore: remove all remaining @W references, replace with @I/@D"
```

---

### Task 7: Manual integration test

**Step 1: Build and start emulator**

Run: `dotnet build && dotnet run --project e6502.Avalonia`

**Step 2: Test auto-route MIDPLAY**

In BASIC:
```
MIDPLAY "sousa-stars-stripes"
```
Expected: Music plays with auto-routing (WTS preferred, SID overflow). Keyboard visualizer shows activity on voices 7-14 primarily.

**Step 3: Test explicit voice mapping**

In BASIC:
```
MIDPLAY "sousa-stars-stripes,1=0,7=1,8=2"
```
Expected: Voice 1 (SID) plays MIDI channel 0, voices 7-8 (WTS) play channels 1-2.

**Step 4: Test MML with @I**

In BASIC:
```
MUSIC 7,"T120@I0O4L4CDEFGAB>C"
MUSIC PLAY
```
Expected: Voice 7 plays piano (GM program 0) through WTS.

**Step 5: Test MML with @D**

In BASIC:
```
MUSIC 7,"T120@D0O4L4CDEFGAB>C"
MUSIC PLAY
```
Expected: Voice 7 plays drum sounds through WTS.

**Step 6: Test SID instrument on SID voice**

In BASIC:
```
INSTRUMENT 1,64,0,9,0,6
MUSIC 1,"T120I1O4L4CDEFGAB>C"
MUSIC PLAY
```
Expected: Voice 1 plays pulse wave through SID.

**Step 7: Test wrong instrument type is ignored**

In BASIC:
```
MUSIC 1,"T120@I0O4L4CDEFGAB>C"
MUSIC PLAY
```
Expected: Voice 1 (SID) ignores `@I0`, plays default SID instrument.
