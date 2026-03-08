# MIDI Tools Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add MIDPLAY BASIC command for real-time MIDI playback and mid2bas CLI converter, both sharing a core MIDI-to-SID engine.

**Architecture:** DryWetMIDI parses .mid files. MidiEngine (shared core) handles channel analysis, instrument mapping, velocity mapping, and MML generation. MidiPlayback drives MusicEngine at 60Hz for real-time playback. MidToBasCommand generates .bas files with MML MUSIC statements.

**Tech Stack:** C#/.NET, Melanchall.DryWetMidi NuGet, MSTest

**Design doc:** `docs/plans/2026-03-05-midi-tools-design.md`

---

### Task 1: Add DryWetMIDI NuGet Package

**Files:**
- Modify: `e6502.Avalonia/e6502.Avalonia.csproj`
- Modify: `e6502.Tools/e6502.Tools.csproj`
- Modify: `e6502UnitTests/e6502UnitTests.csproj`

**Step 1: Add the NuGet package to all three projects**

Run:
```bash
cd /Users/barry/Git/e6502
dotnet add e6502.Avalonia/e6502.Avalonia.csproj package Melanchall.DryWetMidi
dotnet add e6502.Tools/e6502.Tools.csproj package Melanchall.DryWetMidi
dotnet add e6502UnitTests/e6502UnitTests.csproj package Melanchall.DryWetMidi
```

**Step 2: Verify build**

Run: `dotnet build`
Expected: Build succeeded, 0 errors.

**Step 3: Commit**

```bash
git add e6502.Avalonia/e6502.Avalonia.csproj e6502.Tools/e6502.Tools.csproj e6502UnitTests/e6502UnitTests.csproj
git commit -m "chore: add DryWetMIDI NuGet package"
```

---

### Task 2: MidiEngine — Channel Analysis and Selection

**Files:**
- Create: `e6502.Avalonia/Hardware/MidiEngine.cs`
- Create: `e6502UnitTests/MidiEngineTests.cs`

This is the shared core. It parses MIDI files, analyzes channels, selects voices, and maps instruments.

**Step 1: Write the failing tests**

Create `e6502UnitTests/MidiEngineTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502UnitTests;

[TestClass]
public class MidiEngineTests
{
    /// <summary>Build a minimal MIDI file with notes on specific channels.</summary>
    private static MidiFile BuildTestMidi(params (int channel, int noteCount, int program)[] channels)
    {
        var midi = new MidiFile();
        foreach (var (ch, count, prog) in channels)
        {
            var chunk = new TrackChunk();
            using var manager = chunk.ManageTimedEvents();
            manager.Events.Add(new TimedEvent(
                new ProgramChangeEvent((SevenBitNumber)(byte)prog) { Channel = (FourBitNumber)(byte)ch }, 0));
            for (int i = 0; i < count; i++)
            {
                long time = i * 480; // quarter notes
                manager.Events.Add(new TimedEvent(
                    new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100) { Channel = (FourBitNumber)(byte)ch }, time));
                manager.Events.Add(new TimedEvent(
                    new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0) { Channel = (FourBitNumber)(byte)ch }, time + 240));
            }
            midi.Chunks.Add(chunk);
        }
        return midi;
    }

    [TestMethod]
    public void AnalyzeChannels_CountsNotesPerChannel()
    {
        var midi = BuildTestMidi((0, 50, 0), (1, 30, 40), (9, 20, 0));
        var analysis = MidiEngine.AnalyzeChannels(midi);

        Assert.AreEqual(50, analysis[0].NoteCount);
        Assert.AreEqual(30, analysis[1].NoteCount);
        Assert.AreEqual(20, analysis[9].NoteCount);
    }

    [TestMethod]
    public void AnalyzeChannels_ExtractsGmProgram()
    {
        var midi = BuildTestMidi((0, 10, 0), (1, 10, 48));
        var analysis = MidiEngine.AnalyzeChannels(midi);

        Assert.AreEqual(0, analysis[0].GmProgram);
        Assert.AreEqual(48, analysis[1].GmProgram);
    }

    [TestMethod]
    public void AnalyzeChannels_DetectsDrumChannel()
    {
        var midi = BuildTestMidi((9, 10, 0));
        var analysis = MidiEngine.AnalyzeChannels(midi);

        Assert.IsTrue(analysis[9].IsDrums);
        Assert.IsFalse(analysis[0].IsDrums);
    }

    [TestMethod]
    public void SelectChannels_PicksTopByNoteCount()
    {
        var midi = BuildTestMidi(
            (0, 100, 0), (1, 80, 0), (2, 60, 0), (3, 40, 0),
            (4, 20, 0), (5, 10, 0), (6, 5, 0));
        var selected = MidiEngine.SelectChannels(midi, maxVoices: 6);

        Assert.AreEqual(6, selected.Length);
        Assert.AreEqual(0, selected[0]); // most notes first
        Assert.AreEqual(1, selected[1]);
        Assert.AreEqual(2, selected[2]);
        Assert.AreEqual(3, selected[3]);
        Assert.AreEqual(4, selected[4]);
        Assert.AreEqual(5, selected[5]);
    }

    [TestMethod]
    public void SelectChannels_ExplicitMappingOverridesAuto()
    {
        var midi = BuildTestMidi((0, 100, 0), (3, 10, 0), (7, 5, 0));
        var mapping = new Dictionary<int, int> { { 1, 3 }, { 2, 7 } }; // voice=channel
        var selected = MidiEngine.SelectChannels(midi, maxVoices: 6, explicitMapping: mapping);

        // Explicit mapping: voice 1→ch3, voice 2→ch7
        Assert.AreEqual(3, selected[0]); // voice 1 (index 0)
        Assert.AreEqual(7, selected[1]); // voice 2 (index 1)
    }

    [TestMethod]
    public void SelectChannels_SkipsEmptyChannels()
    {
        var midi = BuildTestMidi((0, 10, 0), (1, 5, 0));
        var selected = MidiEngine.SelectChannels(midi, maxVoices: 6);

        Assert.AreEqual(2, selected.Length);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: FAIL — `MidiEngine` class does not exist.

**Step 3: Write minimal implementation**

Create `e6502.Avalonia/Hardware/MidiEngine.cs`:

```csharp
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Shared MIDI-to-SID core: channel analysis, selection, instrument mapping,
/// duration quantization, and MML generation. Used by both MIDPLAY (real-time)
/// and mid2bas (offline converter).
/// </summary>
public static class MidiEngine
{
    public sealed class ChannelInfo
    {
        public int Channel;
        public int NoteCount;
        public int GmProgram;
        public bool IsDrums => Channel == 9;
    }

    /// <summary>Analyze all 16 MIDI channels, counting notes and extracting GM programs.</summary>
    public static ChannelInfo[] AnalyzeChannels(MidiFile midi)
    {
        var channels = new ChannelInfo[16];
        for (int i = 0; i < 16; i++)
            channels[i] = new ChannelInfo { Channel = i };

        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                if (timedEvent.Event is NoteOnEvent noteOn && noteOn.Velocity > 0)
                    channels[noteOn.Channel]!.NoteCount++;
                else if (timedEvent.Event is ProgramChangeEvent pc)
                    channels[pc.Channel]!.GmProgram = pc.ProgramNumber;
            }
        }

        return channels;
    }

    /// <summary>
    /// Select up to maxVoices channels, ranked by note count.
    /// If explicitMapping is provided (voice 1-based → channel 0-based), those are used first.
    /// </summary>
    public static int[] SelectChannels(MidiFile midi, int maxVoices = 6,
        Dictionary<int, int>? explicitMapping = null)
    {
        var analysis = AnalyzeChannels(midi);

        if (explicitMapping is { Count: > 0 })
        {
            // Return channels in voice order (voice 1, 2, 3...)
            return explicitMapping
                .OrderBy(kv => kv.Key)
                .Select(kv => kv.Value)
                .Take(maxVoices)
                .ToArray();
        }

        return analysis
            .Where(c => c.NoteCount > 0)
            .OrderByDescending(c => c.NoteCount)
            .Take(maxVoices)
            .Select(c => c.Channel)
            .ToArray();
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: All 6 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiEngine.cs e6502UnitTests/MidiEngineTests.cs
git commit -m "feat: add MidiEngine with channel analysis and selection"
```

---

### Task 3: MidiEngine — Instrument Bucket Mapping

**Files:**
- Modify: `e6502.Avalonia/Hardware/MidiEngine.cs`
- Modify: `e6502UnitTests/MidiEngineTests.cs`

**Step 1: Write the failing tests**

Add to `MidiEngineTests.cs`:

```csharp
[TestMethod]
public void GetInstrumentBucket_PianoRange()
{
    var bucket = MidiEngine.GetInstrumentBucket(0, isDrums: false);  // Acoustic Grand
    Assert.AreEqual(0x40, bucket.Waveform); // pulse
    bucket = MidiEngine.GetInstrumentBucket(7, isDrums: false);  // Clavinet
    Assert.AreEqual(0x40, bucket.Waveform);
}

[TestMethod]
public void GetInstrumentBucket_StringsRange()
{
    var bucket = MidiEngine.GetInstrumentBucket(48, isDrums: false); // String Ensemble
    Assert.AreEqual(0x20, bucket.Waveform); // saw
}

[TestMethod]
public void GetInstrumentBucket_FluteRange()
{
    var bucket = MidiEngine.GetInstrumentBucket(73, isDrums: false); // Flute
    Assert.AreEqual(0x10, bucket.Waveform); // triangle
}

[TestMethod]
public void GetInstrumentBucket_DrumsAlwaysNoise()
{
    var bucket = MidiEngine.GetInstrumentBucket(0, isDrums: true);
    Assert.AreEqual(0x80, bucket.Waveform); // noise
}

[TestMethod]
public void GetInstrumentBucket_DefaultIsSaw()
{
    var bucket = MidiEngine.GetInstrumentBucket(120, isDrums: false); // Sound Effects GM range
    Assert.AreEqual(0x20, bucket.Waveform); // saw default
}

[TestMethod]
public void VelocityToVolume_MapsRange()
{
    Assert.AreEqual(0, MidiEngine.VelocityToVolume(0));
    Assert.AreEqual(8, MidiEngine.VelocityToVolume(64));
    Assert.AreEqual(15, MidiEngine.VelocityToVolume(127));
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: FAIL — `GetInstrumentBucket` and `VelocityToVolume` do not exist.

**Step 3: Write minimal implementation**

Add to `MidiEngine.cs`:

```csharp
public sealed class InstrumentBucket
{
    public byte Waveform;
    public byte Attack;
    public byte Decay;
    public byte Sustain;
    public byte Release;
    public string Name = "";
}

private static readonly InstrumentBucket[] Buckets =
{
    new() { Waveform = 0x40, Attack = 0, Decay = 4, Sustain = 7, Release = 6, Name = "Pulse Piano" },
    new() { Waveform = 0x40, Attack = 0, Decay = 9, Sustain = 0, Release = 6, Name = "Pulse Lead" },
    new() { Waveform = 0x20, Attack = 4, Decay = 6, Sustain = 10, Release = 8, Name = "Saw Strings" },
    new() { Waveform = 0x20, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Saw Brass" },
    new() { Waveform = 0x10, Attack = 2, Decay = 6, Sustain = 12, Release = 8, Name = "Tri Flute" },
    new() { Waveform = 0x10, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Tri Bass" },
    new() { Waveform = 0x80, Attack = 0, Decay = 3, Sustain = 0, Release = 2, Name = "Noise Drums" },
    new() { Waveform = 0x20, Attack = 0, Decay = 5, Sustain = 8, Release = 4, Name = "Saw Default" },
};

// Indices into Buckets array
private const int BucketPulsePiano = 0;
private const int BucketPulseLead = 1;
private const int BucketSawStrings = 2;
private const int BucketSawBrass = 3;
private const int BucketTriFlute = 4;
private const int BucketTriBass = 5;
private const int BucketNoiseDrums = 6;
private const int BucketSawDefault = 7;

public static InstrumentBucket GetInstrumentBucket(int gmProgram, bool isDrums)
{
    if (isDrums) return Buckets[BucketNoiseDrums];

    return gmProgram switch
    {
        >= 0 and <= 7   => Buckets[BucketPulsePiano],   // Piano family
        >= 8 and <= 15  => Buckets[BucketPulsePiano],   // Chromatic percussion
        >= 16 and <= 23 => Buckets[BucketPulsePiano],   // Organ
        >= 24 and <= 31 => Buckets[BucketPulsePiano],   // Guitar (acoustic)
        >= 32 and <= 39 => Buckets[BucketTriBass],      // Bass
        >= 40 and <= 55 => Buckets[BucketSawStrings],   // Strings + Ensemble
        >= 56 and <= 71 => Buckets[BucketSawBrass],     // Brass + Reed
        >= 72 and <= 79 => Buckets[BucketTriFlute],     // Pipe
        >= 80 and <= 95 => Buckets[BucketPulseLead],    // Synth lead + pad
        _               => Buckets[BucketSawDefault],   // Everything else
    };
}

public static int VelocityToVolume(int velocity) =>
    Math.Clamp(velocity * 15 / 127, 0, 15);
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: All 12 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiEngine.cs e6502UnitTests/MidiEngineTests.cs
git commit -m "feat: add instrument bucket mapping and velocity conversion"
```

---

### Task 4: MidiEngine — Duration Quantization

**Files:**
- Modify: `e6502.Avalonia/Hardware/MidiEngine.cs`
- Modify: `e6502UnitTests/MidiEngineTests.cs`

**Step 1: Write the failing tests**

Add to `MidiEngineTests.cs`:

```csharp
[TestMethod]
public void QuantizeDuration_ExactQuarter()
{
    // 96 MML ticks = quarter note → "4"
    string result = MidiEngine.QuantizeDuration(96);
    Assert.AreEqual("4", result);
}

[TestMethod]
public void QuantizeDuration_ExactEighth()
{
    string result = MidiEngine.QuantizeDuration(48);
    Assert.AreEqual("8", result);
}

[TestMethod]
public void QuantizeDuration_DottedQuarter()
{
    // 144 MML ticks = dotted quarter → "4."
    string result = MidiEngine.QuantizeDuration(144);
    Assert.AreEqual("4.", result);
}

[TestMethod]
public void QuantizeDuration_WholeNote()
{
    string result = MidiEngine.QuantizeDuration(384);
    Assert.AreEqual("1", result);
}

[TestMethod]
public void QuantizeDuration_TieRequired()
{
    // 96 + 48 = 144 ticks = quarter tied to eighth → "4&8" or "4." (dotted)
    // 96 + 24 = 120 ticks = quarter + sixteenth → "4&16"
    string result = MidiEngine.QuantizeDuration(120);
    Assert.AreEqual("4&16", result);
}

[TestMethod]
public void QuantizeDuration_LongTie()
{
    // 384 + 192 = 576 = whole + half → "1&2" or "1." (dotted whole)
    string result = MidiEngine.QuantizeDuration(576);
    Assert.AreEqual("1.", result);
}

[TestMethod]
public void MidiTicksToMmlTicks_StandardPpqn()
{
    // PPQN 480: 480 MIDI ticks = 1 quarter = 96 MML ticks
    Assert.AreEqual(96, MidiEngine.MidiTicksToMmlTicks(480, 480));
    Assert.AreEqual(48, MidiEngine.MidiTicksToMmlTicks(240, 480));
    Assert.AreEqual(192, MidiEngine.MidiTicksToMmlTicks(960, 480));
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: FAIL — `QuantizeDuration` and `MidiTicksToMmlTicks` do not exist.

**Step 3: Write minimal implementation**

Add to `MidiEngine.cs`:

```csharp
private const int MmlTicksPerQuarter = 96;

// Valid MML durations: (denominator, ticks, dotted ticks)
private static readonly (int denom, int ticks, int dottedTicks)[] MmlDurations =
{
    (1, 384, 576),
    (2, 192, 288),
    (4, 96, 144),
    (8, 48, 72),
    (16, 24, 36),
    (32, 12, 18),
};

public static int MidiTicksToMmlTicks(long midiTicks, int ppqn) =>
    (int)Math.Round((double)midiTicks * MmlTicksPerQuarter / ppqn);

/// <summary>
/// Convert MML ticks to a duration string using note values and ties.
/// E.g., 96 → "4", 144 → "4.", 120 → "4&16"
/// </summary>
public static string QuantizeDuration(int mmlTicks)
{
    if (mmlTicks <= 0) return "32";

    var parts = new List<string>();
    int remaining = mmlTicks;

    while (remaining >= 6) // below 6 ticks = discard
    {
        string? best = null;
        int bestTicks = 0;

        foreach (var (denom, ticks, dottedTicks) in MmlDurations)
        {
            if (dottedTicks <= remaining && dottedTicks > bestTicks)
            {
                best = $"{denom}.";
                bestTicks = dottedTicks;
            }
            if (ticks <= remaining && ticks > bestTicks)
            {
                best = $"{denom}";
                bestTicks = ticks;
            }
        }

        if (best is null) break;
        parts.Add(best);
        remaining -= bestTicks;
    }

    return parts.Count > 0 ? string.Join("&", parts) : "32";
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: All 19 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiEngine.cs e6502UnitTests/MidiEngineTests.cs
git commit -m "feat: add MIDI duration quantization with tie support"
```

---

### Task 5: MidiEngine — MML Generation

**Files:**
- Modify: `e6502.Avalonia/Hardware/MidiEngine.cs`
- Modify: `e6502UnitTests/MidiEngineTests.cs`

This converts a single MIDI channel's note events into an MML string.

**Step 1: Write the failing tests**

Add to `MidiEngineTests.cs`:

```csharp
[TestMethod]
public void GenerateMml_SimpleScale()
{
    var midi = new MidiFile();
    var chunk = new TrackChunk();
    using (var manager = chunk.ManageTimedEvents())
    {
        int[] notes = { 60, 62, 64, 65, 67, 69, 71, 72 }; // C D E F G A B C
        for (int i = 0; i < notes.Length; i++)
        {
            long time = i * 480;
            manager.Events.Add(new TimedEvent(
                new NoteOnEvent((SevenBitNumber)(byte)notes[i], (SevenBitNumber)100)
                    { Channel = (FourBitNumber)0 }, time));
            manager.Events.Add(new TimedEvent(
                new NoteOffEvent((SevenBitNumber)(byte)notes[i], (SevenBitNumber)0)
                    { Channel = (FourBitNumber)0 }, time + 480));
        }
    }
    midi.Chunks.Add(chunk);

    string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

    // Should contain the note letters for a C major scale
    Assert.IsTrue(mml.Contains("C"));
    Assert.IsTrue(mml.Contains("D"));
    Assert.IsTrue(mml.Contains("E"));
    Assert.IsTrue(mml.Contains("G"));
    Assert.IsTrue(mml.Contains("A"));
    Assert.IsTrue(mml.Contains("B"));
    // Should contain octave and duration info
    Assert.IsTrue(mml.Contains("O"));
}

[TestMethod]
public void GenerateMml_IncludesVelocity()
{
    var midi = new MidiFile();
    var chunk = new TrackChunk();
    using (var manager = chunk.ManageTimedEvents())
    {
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)50)
                { Channel = (FourBitNumber)0 }, 0));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 480));
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)64, (SevenBitNumber)120)
                { Channel = (FourBitNumber)0 }, 480));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)64, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 960));
    }
    midi.Chunks.Add(chunk);

    string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

    // Should have V commands for velocity changes
    Assert.IsTrue(mml.Contains("V"));
}

[TestMethod]
public void GenerateMml_RestsForGaps()
{
    var midi = new MidiFile();
    var chunk = new TrackChunk();
    using (var manager = chunk.ManageTimedEvents())
    {
        // Note at time 0, then gap, then note at time 960
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100)
                { Channel = (FourBitNumber)0 }, 0));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 480));
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)64, (SevenBitNumber)100)
                { Channel = (FourBitNumber)0 }, 960));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)64, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 1440));
    }
    midi.Chunks.Add(chunk);

    string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

    // Should have a rest between the two notes
    Assert.IsTrue(mml.Contains("R"));
}

[TestMethod]
public void GenerateMml_TopNoteWinsForChords()
{
    var midi = new MidiFile();
    var chunk = new TrackChunk();
    using (var manager = chunk.ManageTimedEvents())
    {
        // Two simultaneous notes: C4 (60) and E4 (64) — top note wins → E
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100)
                { Channel = (FourBitNumber)0 }, 0));
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)64, (SevenBitNumber)100)
                { Channel = (FourBitNumber)0 }, 0));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 480));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)64, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 480));
    }
    midi.Chunks.Add(chunk);

    string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

    // Should contain E (the top note) not both C and E at same position
    Assert.IsTrue(mml.Contains("E"));
}

[TestMethod]
public void GenerateMml_IncludesTempoChange()
{
    var midi = new MidiFile(
        new TrackChunk(
            new SetTempoEvent(500000)),  // 120 BPM
        new TrackChunk());
    var chunk = (TrackChunk)midi.Chunks[1];
    using (var manager = chunk.ManageTimedEvents())
    {
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100)
                { Channel = (FourBitNumber)0 }, 0));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0 }, 480));
    }

    string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

    Assert.IsTrue(mml.Contains("T120"));
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: FAIL — `GenerateMml` does not exist.

**Step 3: Write minimal implementation**

Add to `MidiEngine.cs`:

```csharp
private static readonly string[] NoteNames = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

/// <summary>
/// Generate an MML string from a single MIDI channel's events.
/// Handles notes, rests, velocity (V), octave tracking, tempo, and top-note-wins polyphony.
/// </summary>
public static string GenerateMml(MidiFile midi, int channel, int ppqn)
{
    // Collect all relevant events sorted by time
    var noteEvents = new List<(long time, int midi, int velocity, bool isOn)>();
    long initialTempo = 500000; // default 120 BPM

    foreach (var trackChunk in midi.GetTrackChunks())
    {
        foreach (var timedEvent in trackChunk.GetTimedEvents())
        {
            switch (timedEvent.Event)
            {
                case NoteOnEvent noteOn when noteOn.Channel == channel && noteOn.Velocity > 0:
                    noteEvents.Add((timedEvent.Time, noteOn.NoteNumber, noteOn.Velocity, true));
                    break;
                case NoteOnEvent noteOn when noteOn.Channel == channel && noteOn.Velocity == 0:
                    noteEvents.Add((timedEvent.Time, noteOn.NoteNumber, 0, false));
                    break;
                case NoteOffEvent noteOff when noteOff.Channel == channel:
                    noteEvents.Add((timedEvent.Time, noteOff.NoteNumber, 0, false));
                    break;
                case SetTempoEvent tempo:
                    initialTempo = tempo.MicrosecondsPerQuarterNote;
                    break;
            }
        }
    }

    noteEvents.Sort((a, b) => a.time.CompareTo(b.time));

    // Reduce to monophonic (top-note-wins) timeline
    var notes = ReduceToMonophonic(noteEvents, ppqn);

    // Build MML
    var sb = new System.Text.StringBuilder();
    int bpm = (int)(60_000_000.0 / initialTempo);
    sb.Append($"T{bpm}");

    int currentOctave = -1;
    int currentVelocity = -1;

    foreach (var note in notes)
    {
        if (note.IsRest)
        {
            string dur = QuantizeDuration(note.MmlTicks);
            sb.Append($"R{dur}");
            continue;
        }

        // Velocity → V command
        int vol = VelocityToVolume(note.Velocity);
        if (vol != currentVelocity)
        {
            sb.Append($"V{vol}");
            currentVelocity = vol;
        }

        // Octave tracking
        int noteOctave = (note.Midi / 12) - 1;
        if (noteOctave != currentOctave)
        {
            if (currentOctave >= 0 && noteOctave == currentOctave + 1)
                sb.Append('>');
            else if (currentOctave >= 0 && noteOctave == currentOctave - 1)
                sb.Append('<');
            else
                sb.Append($"O{noteOctave}");
            currentOctave = noteOctave;
        }

        // Note name + duration
        int semitone = note.Midi % 12;
        string name = NoteNames[semitone];
        string dur2 = QuantizeDuration(note.MmlTicks);
        sb.Append($"{name}{dur2}");
    }

    return sb.ToString();
}

private sealed class MonoNote
{
    public bool IsRest;
    public int Midi;
    public int Velocity;
    public int MmlTicks;
}

/// <summary>
/// Reduce polyphonic note events to a monophonic timeline using top-note-wins.
/// </summary>
private static List<MonoNote> ReduceToMonophonic(
    List<(long time, int midi, int velocity, bool isOn)> events, int ppqn)
{
    var result = new List<MonoNote>();
    var activeNotes = new SortedDictionary<int, int>(); // midi → velocity (sorted by pitch)
    long cursor = 0;

    int i = 0;
    while (i < events.Count)
    {
        long eventTime = events[i].time;

        // Process all events at the same time
        while (i < events.Count && events[i].time == eventTime)
        {
            var e = events[i];
            if (e.isOn)
                activeNotes[e.midi] = e.velocity;
            else
                activeNotes.Remove(e.midi);
            i++;
        }

        // Determine what's sounding now (top note wins)
        int topMidi = activeNotes.Count > 0 ? activeNotes.Keys.Last() : -1;
        int topVel = topMidi >= 0 ? activeNotes[topMidi] : 0;

        // Find the next event time
        long nextTime = i < events.Count ? events[i].time : eventTime;
        if (i >= events.Count && activeNotes.Count > 0)
            nextTime = eventTime; // end

        // Look ahead to get duration
        long endTime = i < events.Count ? events[i].time : eventTime;
        int mmlTicks = MidiTicksToMmlTicks(endTime - eventTime, ppqn);

        if (mmlTicks <= 0) continue;

        // Gap from cursor to this event = rest
        if (eventTime > cursor)
        {
            int restTicks = MidiTicksToMmlTicks(eventTime - cursor, ppqn);
            if (restTicks > 0)
                result.Add(new MonoNote { IsRest = true, MmlTicks = restTicks });
        }

        if (topMidi >= 0)
            result.Add(new MonoNote { Midi = topMidi, Velocity = topVel, MmlTicks = mmlTicks });
        else
            result.Add(new MonoNote { IsRest = true, MmlTicks = mmlTicks });

        cursor = endTime;
    }

    return result;
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: All 24 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiEngine.cs e6502UnitTests/MidiEngineTests.cs
git commit -m "feat: add MML generation with monophonic reduction"
```

---

### Task 6: MidiEngine — Full .bas Output Generation

**Files:**
- Modify: `e6502.Avalonia/Hardware/MidiEngine.cs`
- Modify: `e6502UnitTests/MidiEngineTests.cs`

**Step 1: Write the failing test**

Add to `MidiEngineTests.cs`:

```csharp
[TestMethod]
public void GenerateBasProgram_ProducesValidOutput()
{
    var midi = BuildTestMidi((0, 20, 0), (1, 15, 48));
    string bas = MidiEngine.GenerateBasProgram(midi,
        title: "TEST SONG", subtitle: "UNIT TEST",
        maxLineLen: 200);

    // Should have REM header
    Assert.IsTrue(bas.Contains("REM"));
    Assert.IsTrue(bas.Contains("TEST SONG"));
    // Should have INSTRUMENT definitions
    Assert.IsTrue(bas.Contains("INSTRUMENT"));
    // Should have MUSIC statements
    Assert.IsTrue(bas.Contains("MUSIC 1,"));
    Assert.IsTrue(bas.Contains("MUSIC 2,"));
    // Should have VOLUME and MUSIC PLAY
    Assert.IsTrue(bas.Contains("VOLUME 15"));
    Assert.IsTrue(bas.Contains("MUSIC PLAY"));
    // Lines should be numbered
    Assert.IsTrue(bas.StartsWith("10 REM"));
}

[TestMethod]
public void GenerateBasProgram_SplitsLongMml()
{
    // Build a channel with many notes to exceed maxLineLen
    var midi = BuildTestMidi((0, 100, 0));
    string bas = MidiEngine.GenerateBasProgram(midi, maxLineLen: 80);

    // Multiple MUSIC 1,"..." lines expected
    int musicLines = bas.Split('\n').Count(l => l.Contains("MUSIC 1,"));
    Assert.IsTrue(musicLines > 1, $"Expected multiple MUSIC lines, got {musicLines}");
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: FAIL — `GenerateBasProgram` does not exist.

**Step 3: Write minimal implementation**

Add to `MidiEngine.cs`:

```csharp
/// <summary>
/// Generate a complete NovaBASIC .bas program from a MIDI file.
/// </summary>
public static string GenerateBasProgram(MidiFile midi,
    string title = "", string subtitle = "",
    int maxLineLen = 200, int maxVoices = 6,
    Dictionary<int, int>? explicitMapping = null)
{
    int ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
    var analysis = AnalyzeChannels(midi);
    var selectedChannels = SelectChannels(midi, maxVoices, explicitMapping);

    var lines = new List<string>();
    int lineNum = 10;

    // Header
    lines.Add($"{lineNum} REM ================================"); lineNum += 10;
    if (!string.IsNullOrEmpty(title))
    {
        lines.Add($"{lineNum} REM  {title}"); lineNum += 10;
    }
    if (!string.IsNullOrEmpty(subtitle))
    {
        lines.Add($"{lineNum} REM  {subtitle}"); lineNum += 10;
    }
    lines.Add($"{lineNum} REM ================================"); lineNum += 10;

    // Instrument definitions
    var usedBuckets = new Dictionary<int, int>(); // bucket index → instrument slot
    int nextSlot = 1;

    for (int v = 0; v < selectedChannels.Length; v++)
    {
        int ch = selectedChannels[v];
        var info = analysis[ch];
        var bucket = GetInstrumentBucket(info.GmProgram, info.IsDrums);
        int bucketIdx = Array.IndexOf(Buckets, bucket);

        if (!usedBuckets.ContainsKey(bucketIdx))
        {
            usedBuckets[bucketIdx] = nextSlot;
            lines.Add($"{lineNum} INSTRUMENT {nextSlot},{bucket.Waveform},{bucket.Attack},{bucket.Decay},{bucket.Sustain},{bucket.Release}:REM {bucket.Name}");
            lineNum += 10;
            nextSlot++;
        }
    }

    // MML sequences per voice
    for (int v = 0; v < selectedChannels.Length; v++)
    {
        int ch = selectedChannels[v];
        var info = analysis[ch];
        var bucket = GetInstrumentBucket(info.GmProgram, info.IsDrums);
        int bucketIdx = Array.IndexOf(Buckets, bucket);
        int instSlot = usedBuckets[bucketIdx];

        string mml = GenerateMml(midi, ch, ppqn);
        // Prepend instrument selection
        mml = $"I{instSlot}{mml}";

        int voice = v + 1;
        var mmlLines = SplitMml(mml, maxLineLen, voice);
        foreach (var ml in mmlLines)
        {
            lines.Add($"{lineNum} {ml}");
            lineNum += 10;
        }
    }

    // Playback
    lines.Add($"{lineNum} VOLUME 15"); lineNum += 10;
    lines.Add($"{lineNum} MUSIC PLAY"); lineNum += 10;

    return string.Join("\n", lines) + "\n";
}

/// <summary>Split a long MML string into multiple MUSIC voice,"..." lines.</summary>
private static List<string> SplitMml(string mml, int maxLineLen, int voice)
{
    var result = new List<string>();
    string prefix = $"MUSIC {voice},\"";
    int maxMml = maxLineLen - prefix.Length - 1; // -1 for closing quote

    int pos = 0;
    while (pos < mml.Length)
    {
        int len = Math.Min(maxMml, mml.Length - pos);
        result.Add($"{prefix}{mml.Substring(pos, len)}\"");
        pos += len;
    }

    return result;
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~MidiEngine" --verbosity normal`
Expected: All 26 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiEngine.cs e6502UnitTests/MidiEngineTests.cs
git commit -m "feat: add .bas program generation with MML splitting"
```

---

### Task 7: mid2bas CLI Verb

**Files:**
- Modify: `e6502.Tools/Program.cs`

The current Program.cs is flag-based for sidreloc. We'll add verb-based routing: if the first argument is `mid2bas`, handle MIDI conversion; otherwise fall through to sidreloc.

**Step 1: Write the implementation**

Replace `e6502.Tools/Program.cs` with:

```csharp
using e6502.Avalonia.Hardware;
using e6502.Tools;
using Melanchall.DryWetMidi.Core;

if (args.Length < 1)
{
    PrintUsage();
    return 1;
}

// Verb-based routing
if (args[0] == "mid2bas")
    return RunMidToBas(args.Skip(1).ToArray());

// Legacy: sidreloc (no verb prefix)
return RunSidReloc(args);

static int RunMidToBas(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: mid2bas <input.mid> [-o output.bas] [--mml-only] [--title TITLE] [--subtitle SUB] [--voices 1=3,2=5] [--max-line-len 200]");
        return 1;
    }

    string inputPath = args[0];
    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    string? outputPath = null;
    string title = "";
    string subtitle = "";
    bool mmlOnly = false;
    int maxLineLen = 200;
    Dictionary<int, int>? mapping = null;

    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "-o" or "--output" when i + 1 < args.Length:
                outputPath = args[++i];
                break;
            case "--title" when i + 1 < args.Length:
                title = args[++i];
                break;
            case "--subtitle" when i + 1 < args.Length:
                subtitle = args[++i];
                break;
            case "--mml-only":
                mmlOnly = true;
                break;
            case "--max-line-len" when i + 1 < args.Length:
                maxLineLen = int.Parse(args[++i]);
                break;
            case "--voices" when i + 1 < args.Length:
                mapping = ParseVoiceMapping(args[++i]);
                break;
        }
    }

    MidiFile midi;
    try { midi = MidiFile.Read(inputPath); }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Failed to read MIDI file: {ex.Message}");
        return 1;
    }

    if (mmlOnly)
    {
        int ppqn = ((Melanchall.DryWetMidi.Core.TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        var channels = MidiEngine.SelectChannels(midi, explicitMapping: mapping);
        for (int v = 0; v < channels.Length; v++)
        {
            string mml = MidiEngine.GenerateMml(midi, channels[v], ppqn);
            Console.WriteLine($"Voice {v + 1} (ch {channels[v]}): {mml}");
        }
        return 0;
    }

    string bas = MidiEngine.GenerateBasProgram(midi, title, subtitle, maxLineLen,
        explicitMapping: mapping);

    if (outputPath is null)
        outputPath = Path.ChangeExtension(inputPath, ".bas");

    File.WriteAllText(outputPath, bas);
    Console.WriteLine($"Wrote {outputPath}");
    return 0;
}

static Dictionary<int, int> ParseVoiceMapping(string s)
{
    var map = new Dictionary<int, int>();
    foreach (var pair in s.Split(','))
    {
        var parts = pair.Split('=');
        if (parts.Length == 2 && int.TryParse(parts[0], out int voice) && int.TryParse(parts[1], out int channel))
            map[voice] = channel;
    }
    return map;
}

static int RunSidReloc(string[] args)
{
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

    if (args.Contains("--info"))
    {
        Console.WriteLine($"Title:     {info.Title}");
        Console.WriteLine($"Author:    {info.Author}");
        Console.WriteLine($"Copyright: {info.Copyright}");
        Console.WriteLine($"Load:      ${info.LoadAddress:X4}");
        Console.WriteLine($"Init:      ${info.InitAddress:X4}");
        Console.WriteLine($"Play:      ${info.PlayAddress:X4}");
        Console.WriteLine($"Songs:     {info.Songs}");
        Console.WriteLine($"Speed:     {(info.UsesCiaTiming ? "CIA" : "VBI")}");
        Console.WriteLine($"Size:      {info.Payload.Length} bytes");
        return 0;
    }

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

    WriteSidFile(outputPath, relocated);
    Console.WriteLine($"Relocated ${info.LoadAddress:X4} -> ${target:X4}, wrote {outputPath}");
    return 0;
}

static void PrintUsage()
{
    Console.Error.WriteLine("e6502 Tools");
    Console.Error.WriteLine("  mid2bas <input.mid> [-o output.bas] [--mml-only] [--title T] [--subtitle S] [--voices 1=3,2=5]");
    Console.Error.WriteLine("  <input.sid> [output.sid] --target 0x1000   (SID relocator)");
    Console.Error.WriteLine("  <input.sid> --info                         (SID info)");
}

static void WriteSidFile(string path, SidFileInfo info)
{
    byte[] header = new byte[124];
    header[0] = (byte)'P'; header[1] = (byte)'S'; header[2] = (byte)'I'; header[3] = (byte)'D';
    WriteBE16(header, 4, info.Version);
    WriteBE16(header, 6, 124);
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

**Step 2: Verify build**

Run: `dotnet build`
Expected: Build succeeded.

**Step 3: Verify existing tests still pass**

Run: `dotnet test`
Expected: All tests pass (existing sidreloc behavior preserved).

**Step 4: Commit**

```bash
git add e6502.Tools/Program.cs
git commit -m "feat: add mid2bas CLI verb to e6502.Tools"
```

---

### Task 8: MidiPlayback — Real-Time 60Hz Driver

**Files:**
- Create: `e6502.Avalonia/Hardware/MidiPlayback.cs`
- Modify: `e6502.Avalonia/Hardware/MusicEngine.cs`
- Create: `e6502UnitTests/MidiPlaybackTests.cs`

MidiPlayback converts MIDI events into direct MusicEngine calls at 60Hz, similar to how SidPlayer drives the SID chip.

**Step 1: Add direct voice control methods to MusicEngine**

The MusicEngine currently expects MML sequences. MidiPlayback needs to control voices directly: set instrument, gate on/off, set frequency, set volume. Add these methods to `MusicEngine.cs` after the existing `SetVoiceVolume` method (around line 191):

```csharp
// -------------------------------------------------------------------------
// Direct voice control (for MIDI playback, bypasses MML sequencer)
// -------------------------------------------------------------------------

private bool _midiMode;

/// <summary>Enter MIDI mode: stops MML sequencer, allows direct voice control.</summary>
public void EnterMidiMode()
{
    MusicStop();
    MusicReset();
    _midiMode = true;
}

/// <summary>Exit MIDI mode, return to normal MML operation.</summary>
public void ExitMidiMode()
{
    _midiMode = false;
    for (int v = 0; v < VoiceCount; v++)
        DirectNoteOff(v);
}

public void DirectNoteOn(int voiceIndex, int midiNote, int velocity, int instrumentId = 0)
{
    if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
    var inst = GetInstrument(instrumentId);
    var chip = ChipFor(voiceIndex);

    // Set instrument (waveform + ADSR)
    chip.Write(Ad(voiceIndex), inst.Ad);
    chip.Write(Sr(voiceIndex), inst.Sr);
    chip.Write(PwLo(voiceIndex), (byte)(inst.PulseWidth & 0xFF));
    chip.Write(PwHi(voiceIndex), (byte)(inst.PulseWidth >> 8));

    // Set frequency
    int sidFreq = MidiToSid(midiNote);
    chip.Write(FreqLo(voiceIndex), (byte)(sidFreq & 0xFF));
    chip.Write(FreqHi(voiceIndex), (byte)(sidFreq >> 8));

    // Per-voice volume (velocity)
    int vol = Math.Clamp(velocity * 15 / 127, 0, 15);
    int local = LocalVoice(voiceIndex);
    chip.Write((ushort)(chip.BaseAddress + 0x1D + local), (byte)vol);

    // Gate on
    chip.Write(Ctrl(voiceIndex), (byte)(inst.Waveform | 0x01));
}

public void DirectNoteOff(int voiceIndex)
{
    if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;
    var chip = ChipFor(voiceIndex);
    byte ctrl = chip.Read(Ctrl(voiceIndex));
    chip.Write(Ctrl(voiceIndex), (byte)(ctrl & 0xFE)); // gate off
}
```

**Step 2: Write MidiPlayback class**

Create `e6502.Avalonia/Hardware/MidiPlayback.cs`:

```csharp
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Real-time MIDI playback driver. Ticked at 60Hz by the main loop.
/// Converts MIDI events into direct MusicEngine voice commands.
/// </summary>
public sealed class MidiPlayback
{
    private readonly MusicEngine _engine;

    private sealed class TimelineEvent
    {
        public long MidiTick;
        public int Voice;       // 0-5 (-1 for tempo)
        public int MidiNote;    // -1 for note-off
        public int Velocity;
        public int InstrumentId;
        public int Bpm;         // for tempo events
    }

    private List<TimelineEvent>? _timeline;
    private int _timelineIndex;
    private double _midiTickAccum;
    private double _midiTicksPerFrame;
    private int _ppqn;
    private bool _playing;

    public bool IsPlaying => _playing;

    public MidiPlayback(MusicEngine engine)
    {
        _engine = engine;
    }

    /// <summary>
    /// Prepare and start playback of a MIDI file.
    /// voiceToChannel maps voice index (0-5) → MIDI channel.
    /// instrumentSlots maps voice index (0-5) → MusicEngine instrument slot.
    /// </summary>
    public void Play(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
    {
        _ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        _timeline = BuildTimeline(midi, voiceToChannel, instrumentSlots);
        _timelineIndex = 0;
        _midiTickAccum = 0;

        // Default 120 BPM
        SetTempo(120);

        _engine.EnterMidiMode();
        _playing = true;
    }

    public void Stop()
    {
        _playing = false;
        _timeline = null;
        _engine.ExitMidiMode();
    }

    /// <summary>Called at 60Hz. Advance the timeline and fire events.</summary>
    public void Tick()
    {
        if (!_playing || _timeline is null) return;

        _midiTickAccum += _midiTicksPerFrame;

        while (_timelineIndex < _timeline.Count)
        {
            var ev = _timeline[_timelineIndex];
            if (ev.MidiTick > _midiTickAccum) break;

            _timelineIndex++;

            if (ev.Voice == -1)
            {
                // Tempo change
                SetTempo(ev.Bpm);
                continue;
            }

            if (ev.MidiNote < 0)
                _engine.DirectNoteOff(ev.Voice);
            else
                _engine.DirectNoteOn(ev.Voice, ev.MidiNote, ev.Velocity, ev.InstrumentId);
        }

        if (_timelineIndex >= _timeline.Count)
        {
            // All events processed — let notes ring, then stop
            Stop();
        }
    }

    private void SetTempo(int bpm)
    {
        // MIDI ticks per frame at 60Hz
        _midiTicksPerFrame = (double)_ppqn * bpm / (60.0 * 60.0);
    }

    private List<TimelineEvent> BuildTimeline(MidiFile midi, int[] voiceToChannel, int[] instrumentSlots)
    {
        // Build channel → voice reverse lookup
        var channelToVoice = new Dictionary<int, int>();
        for (int v = 0; v < voiceToChannel.Length; v++)
            channelToVoice[voiceToChannel[v]] = v;

        var events = new List<TimelineEvent>();

        // Collect tempo events
        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                if (timedEvent.Event is SetTempoEvent tempo)
                {
                    int bpm = (int)(60_000_000.0 / tempo.MicrosecondsPerQuarterNote);
                    events.Add(new TimelineEvent
                    {
                        MidiTick = timedEvent.Time,
                        Voice = -1,
                        Bpm = bpm
                    });
                }
            }
        }

        // Track active notes per voice for top-note-wins
        var activePerVoice = new Dictionary<int, SortedSet<int>>(); // voice → active MIDI notes
        var noteOnTimes = new Dictionary<(int voice, int midi), long>();

        // Collect all channel note events
        var channelEvents = new List<(long time, int channel, int midi, int velocity, bool isOn)>();

        foreach (var trackChunk in midi.GetTrackChunks())
        {
            foreach (var timedEvent in trackChunk.GetTimedEvents())
            {
                int ch = -1;
                int midiNote = 0;
                int vel = 0;
                bool isOn = false;

                switch (timedEvent.Event)
                {
                    case NoteOnEvent noteOn when noteOn.Velocity > 0:
                        ch = noteOn.Channel;
                        midiNote = noteOn.NoteNumber;
                        vel = noteOn.Velocity;
                        isOn = true;
                        break;
                    case NoteOnEvent noteOn when noteOn.Velocity == 0:
                        ch = noteOn.Channel;
                        midiNote = noteOn.NoteNumber;
                        break;
                    case NoteOffEvent noteOff:
                        ch = noteOff.Channel;
                        midiNote = noteOff.NoteNumber;
                        break;
                }

                if (ch >= 0 && channelToVoice.TryGetValue(ch, out int voice))
                    channelEvents.Add((timedEvent.Time, ch, midiNote, vel, isOn));
            }
        }

        channelEvents.Sort((a, b) => a.time.CompareTo(b.time));

        // Process events into timeline with top-note-wins
        foreach (var (time, channel, midiNote, velocity, isOn) in channelEvents)
        {
            if (!channelToVoice.TryGetValue(channel, out int voice)) continue;

            if (!activePerVoice.ContainsKey(voice))
                activePerVoice[voice] = new SortedSet<int>();

            var active = activePerVoice[voice];
            int prevTop = active.Count > 0 ? active.Max : -1;

            if (isOn)
                active.Add(midiNote);
            else
                active.Remove(midiNote);

            int newTop = active.Count > 0 ? active.Max : -1;

            if (newTop != prevTop)
            {
                if (prevTop >= 0)
                    events.Add(new TimelineEvent { MidiTick = time, Voice = voice, MidiNote = -1 });

                if (newTop >= 0)
                    events.Add(new TimelineEvent
                    {
                        MidiTick = time,
                        Voice = voice,
                        MidiNote = newTop,
                        Velocity = velocity > 0 ? velocity : 100,
                        InstrumentId = instrumentSlots[voice]
                    });
            }
        }

        events.Sort((a, b) => a.MidiTick.CompareTo(b.MidiTick));
        return events;
    }
}
```

**Step 3: Write tests**

Create `e6502UnitTests/MidiPlaybackTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502UnitTests;

[TestClass]
public class MidiPlaybackTests
{
    [TestMethod]
    public void Play_SetsIsPlaying()
    {
        var sid1 = new SidChip(enableAudio: false);
        var sid2 = new SidChip(enableAudio: false, baseAddress: 0xD420);
        var engine = new MusicEngine(sid1, sid2);
        var playback = new MidiPlayback(engine);

        var midi = BuildSimpleMidi();
        playback.Play(midi, voiceToChannel: new[] { 0 }, instrumentSlots: new[] { 0 });

        Assert.IsTrue(playback.IsPlaying);
    }

    [TestMethod]
    public void Stop_ClearsIsPlaying()
    {
        var sid1 = new SidChip(enableAudio: false);
        var sid2 = new SidChip(enableAudio: false, baseAddress: 0xD420);
        var engine = new MusicEngine(sid1, sid2);
        var playback = new MidiPlayback(engine);

        var midi = BuildSimpleMidi();
        playback.Play(midi, voiceToChannel: new[] { 0 }, instrumentSlots: new[] { 0 });
        playback.Stop();

        Assert.IsFalse(playback.IsPlaying);
    }

    [TestMethod]
    public void Tick_AdvancesAndStops()
    {
        var sid1 = new SidChip(enableAudio: false);
        var sid2 = new SidChip(enableAudio: false, baseAddress: 0xD420);
        var engine = new MusicEngine(sid1, sid2);
        var playback = new MidiPlayback(engine);

        var midi = BuildSimpleMidi(); // 1 note, 1 quarter @ 120BPM
        playback.Play(midi, voiceToChannel: new[] { 0 }, instrumentSlots: new[] { 0 });

        // Tick enough frames to exhaust the timeline (120BPM, 1 quarter = 30 frames)
        for (int i = 0; i < 60; i++)
            playback.Tick();

        Assert.IsFalse(playback.IsPlaying);
    }

    private static MidiFile BuildSimpleMidi()
    {
        var midi = new MidiFile();
        var chunk = new TrackChunk();
        using var manager = chunk.ManageTimedEvents();
        manager.Events.Add(new TimedEvent(
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100) { Channel = (FourBitNumber)0 }, 0));
        manager.Events.Add(new TimedEvent(
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0) { Channel = (FourBitNumber)0 }, 480));
        midi.Chunks.Add(chunk);
        return midi;
    }
}
```

**Step 4: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~MidiPlayback" --verbosity normal`
Expected: All 3 tests PASS.

**Step 5: Also run all existing tests**

Run: `dotnet test`
Expected: All tests pass.

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiPlayback.cs e6502.Avalonia/Hardware/MusicEngine.cs e6502UnitTests/MidiPlaybackTests.cs
git commit -m "feat: add MidiPlayback real-time driver with direct voice control"
```

---

### Task 9: MIDPLAY/MIDSTOP BASIC Commands — Assembly

**Files:**
- Modify: `ehbasic/basic.asm`

Add MIDPLAY and MIDSTOP as extended tokens. Follow the SIDPLAY/SIDSTOP pattern exactly.

**Step 1: Add token constants**

After line 517 (`XTK_FONT = $48`), add:

```asm
XTK_MIDPLAY       = $29              ; extended token id: MIDPLAY
XTK_MIDSTOP       = $2A              ; extended token id: MIDSTOP
```

**Step 2: Add FIO command constants**

After line 8835 (`FIO_CMD_MPRI = $12`), add:

```asm
FIO_CMD_MIDPLAY = $13          ; load .mid and start playback
FIO_CMD_MIDSTOP = $14          ; stop MIDI playback
```

**Step 3: Add dispatch table entries**

Replace the first two entries in the `$29-$3F` reserved block (lines 1903-1906). Change:

```asm
      ; $29-$3F reserved (23 entries)
      .repeat $17
      .word LAB_15D9-1
      .endrepeat
```

To:

```asm
      ; $29-$2A: MIDI commands, $2B-$3F reserved (21 entries)
      .word LAB_MIDPLAY-1     ; XTK_MIDPLAY  ($29)
      .word LAB_MIDSTOP-1     ; XTK_MIDSTOP  ($2A)
      .repeat $15
      .word LAB_15D9-1
      .endrepeat
```

**Step 4: Add keyword string table entries**

In the `TAB_XTKSTR` string pointer table, replace the first two reserved entries in the `$29-$3F` block. The reserved entries at line 8647-8650 use `@s_reserved_ext`. Change the `.repeat $17` block to explicitly list MIDPLAY and MIDSTOP first:

After line 8646 (the `@s_dmacount` entry), replace:

```asm
      ; $29-$3F reserved (23 entries)
      .repeat $17
      .word @s_reserved_ext
      .endrepeat
```

With:

```asm
      ; $29-$2A: MIDI commands, $2B-$3F reserved (21 entries)
      .word @s_midplay, @s_midstop
      .repeat $15
      .word @s_reserved_ext
      .endrepeat
```

**Step 5: Add keyword strings**

After `@s_reserved_ext` (line 8697), add:

```asm
@s_midplay: .byte "MIDPLAY",0
@s_midstop: .byte "MIDSTOP",0
```

**Step 6: Add handler code**

After `LAB_SIDSTOP` (around line 9793), add:

```asm
; perform MIDPLAY "filename" [,voice=channel,...]
; MIDPLAY "song"                — auto-select channels
; MIDPLAY "song",1=3,2=5,3=10  — explicit voice=channel mapping

LAB_MIDPLAY
      JSR   LAB_FIO_GETNAME    ; filename → FIO_NAME/FIO_NAMELEN
      ; Parse optional voice=channel mappings into FIO register area
      ; Store mapping count in FIO_SRCL, pairs in FIO_SRCH onwards
      LDA   #$00
      STA   FIO_SRCL            ; mapping count = 0
      LDX   #$00                ; pair index
@mp_check
      JSR   LAB_GBYT            ; peek next char
      CMP   #','
      BNE   @mp_go
      JSR   LAB_IGBY            ; skip comma
      JSR   LAB_GTBY            ; voice number → X
      STX   FIO_SRCH            ; temp: voice
      JSR   LAB_1C01            ; expect '='  — actually we need to check for '='
      ; The '=' is not a comma, so we use LAB_GBYT/CMP
      JSR   LAB_GBYT
      CMP   #TK_EQUAL
      BNE   @mp_go
      JSR   LAB_IGBY            ; skip '='
      JSR   LAB_GTBY            ; channel number → X
      ; Store pair: voice in high nibble, channel in low nibble
      LDA   FIO_SRCH            ; voice
      ASL
      ASL
      ASL
      ASL                        ; voice << 4
      ORA   TMP_REG              ; this won't work cleanly...
      ; Simpler: just store voice,channel as sequential bytes
      ; Use FIO area starting at FIO_ENDL for mapping pairs
      ; FIO_SRCL = count, FIO_ENDL+0 = voice1, FIO_ENDL+1 = ch1, etc.
      LDA   FIO_SRCL             ; current count
      ASL                        ; *2 for byte pairs
      TAY
      LDA   FIO_SRCH             ; voice
      STA   FIO_ENDL,Y
      TXA                        ; channel (still in X from LAB_GTBY)
      INY
      STA   FIO_ENDL,Y
      INC   FIO_SRCL             ; count++
      JMP   @mp_check            ; check for more
@mp_go
      LDA   #FIO_CMD_MIDPLAY
      STA   FIO_CMD
      ; Check for error
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @mp_ok
      JMP   LAB_FCER             ; ?FC ERROR
@mp_ok
      RTS

; perform MIDSTOP

LAB_MIDSTOP
      LDA   #FIO_CMD_MIDSTOP
      STA   FIO_CMD
      RTS
```

Note: The voice=channel parsing is complex in 6502 assembly. A simpler approach for the initial implementation: **skip the mapping syntax in BASIC, only support `MIDPLAY "filename"`**. The explicit mapping can be done via the TCP/MCP interface. Simplify `LAB_MIDPLAY` to:

```asm
LAB_MIDPLAY
      JSR   LAB_FIO_GETNAME    ; filename → FIO_NAME/FIO_NAMELEN
      LDA   #$00
      STA   FIO_SRCL            ; no explicit mapping (auto-select)
      LDA   #FIO_CMD_MIDPLAY
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @mp_ok
      JMP   LAB_FCER             ; ?FC ERROR
@mp_ok
      RTS
```

The voice=channel mapping can be added in a future iteration if needed from BASIC.

**Step 7: Verify the ROM assembles**

Run: `cd /Users/barry/Git/e6502/ehbasic && make` (or however the ROM is assembled — check for a Makefile or build script)

If no assembler is available locally, just verify the syntax looks correct and commit. The CI pipeline will validate.

**Step 8: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: add MIDPLAY/MIDSTOP BASIC command tokens and handlers"
```

---

### Task 10: MIDPLAY/MIDSTOP — Host-Side Integration

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`

**Step 1: Add FIO command constants**

In `VgcConstants.cs`, after `FioCmdMusicPri = 0x12` (line 88), add:

```csharp
public const byte FioCmdMidPlay    = 0x13;
public const byte FioCmdMidStop    = 0x14;
```

**Step 2: Add MidiPlayback to CompositeBusDevice**

In `CompositeBusDevice.cs`, add a field and property for `MidiPlayback`. Find where `_musicEngine` and `_sidPlayer` are created (constructor area) and add:

```csharp
private readonly MidiPlayback _midiPlayback;
public MidiPlayback MidiPlayback => _midiPlayback;
```

Initialize in constructor after `_musicEngine`:

```csharp
_midiPlayback = new MidiPlayback(_musicEngine);
```

Also, in the 60Hz tick method (wherever `_musicEngine.Tick()` is called), add:

```csharp
_midiPlayback.Tick();
```

**Step 3: Add FileIoController handlers**

In `FileIoController.cs`, add cases to the `ExecuteCommand` switch (after `FioCmdMusicPri`):

```csharp
case VgcConstants.FioCmdMidPlay:
    DoMidPlay();
    break;
case VgcConstants.FioCmdMidStop:
    DoMidStop();
    break;
```

Add the handler methods:

```csharp
private void DoMidPlay()
{
    if (_bus.MidiPlayback is null) { SetError(VgcConstants.FioErrIo); return; }

    try
    {
        string? filename = ReadFilename();
        if (filename is null) { SetError(VgcConstants.FioErrIo); return; }

        string path = GetFullPath(filename, ".mid");
        if (!File.Exists(path))
        {
            SetError(VgcConstants.FioErrNotFound);
            return;
        }

        var midi = Melanchall.DryWetMidi.Core.MidiFile.Read(path);
        var analysis = MidiEngine.AnalyzeChannels(midi);
        var selectedChannels = MidiEngine.SelectChannels(midi);

        // Register instrument buckets with MusicEngine
        var instrumentSlots = new int[selectedChannels.Length];
        var registeredBuckets = new Dictionary<int, int>();
        int nextSlot = 1;

        for (int v = 0; v < selectedChannels.Length; v++)
        {
            int ch = selectedChannels[v];
            var bucket = MidiEngine.GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
            int bucketIdx = Array.IndexOf(MidiEngine.GetAllBuckets(), bucket);

            if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
            {
                slot = nextSlot++;
                registeredBuckets[bucketIdx] = slot;
                _bus.Music.DefineInstrument(slot, bucket.Waveform,
                    bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release);
            }
            instrumentSlots[v] = slot;
        }

        _bus.MidiPlayback.Play(midi, selectedChannels, instrumentSlots);
        SetOk();
    }
    catch { SetError(VgcConstants.FioErrIo); }
}

private void DoMidStop()
{
    _bus.MidiPlayback?.Stop();
    SetOk();
}
```

Note: You'll need to expose the `Buckets` array from MidiEngine. Add a public method:

```csharp
// In MidiEngine.cs
public static InstrumentBucket[] GetAllBuckets() => Buckets;
```

**Step 4: Add TCP commands**

In `EmulatorTcpServer.cs`, add to the command dispatch switch:

```csharp
"mid_play" => CmdMidPlay(req),
"mid_stop" => CmdMidStop(),
```

Add handlers:

```csharp
private string CmdMidPlay(JsonNode req)
{
    string? path = req["path"]?.GetValue<string>();
    if (path is null) return Error("Missing 'path'");

    if (!File.Exists(path))
        return Error($"File not found: {path}");

    Melanchall.DryWetMidi.Core.MidiFile midi;
    try { midi = Melanchall.DryWetMidi.Core.MidiFile.Read(path); }
    catch (Exception ex) { return Error($"Failed to read MIDI: {ex.Message}"); }

    var analysis = MidiEngine.AnalyzeChannels(midi);

    // Parse optional explicit mapping
    Dictionary<int, int>? mapping = null;
    if (req["voices"] is JsonNode voicesNode)
    {
        mapping = new Dictionary<int, int>();
        foreach (var prop in voicesNode.AsObject())
        {
            if (int.TryParse(prop.Key, out int voice))
                mapping[voice] = prop.Value!.GetValue<int>();
        }
    }

    var selectedChannels = MidiEngine.SelectChannels(midi, explicitMapping: mapping);

    // Register instruments
    var instrumentSlots = new int[selectedChannels.Length];
    var registeredBuckets = new Dictionary<int, int>();
    int nextSlot = 1;

    for (int v = 0; v < selectedChannels.Length; v++)
    {
        int ch = selectedChannels[v];
        var bucket = MidiEngine.GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
        int bucketIdx = Array.IndexOf(MidiEngine.GetAllBuckets(), bucket);

        if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
        {
            slot = nextSlot++;
            registeredBuckets[bucketIdx] = slot;
            _bus.Music.DefineInstrument(slot, bucket.Waveform,
                bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release);
        }
        instrumentSlots[v] = slot;
    }

    _bus.Music.SetVolume(15);
    _bus.MidiPlayback.Play(midi, selectedChannels, instrumentSlots);

    var result = new JsonObject
    {
        ["ok"] = true,
        ["voices"] = selectedChannels.Length,
        ["channels"] = new JsonArray(selectedChannels.Select(c => (JsonNode)JsonValue.Create(c)).ToArray())
    };
    return result.ToJsonString();
}

private string CmdMidStop()
{
    _bus.MidiPlayback.Stop();
    return Ok();
}
```

**Step 5: Build and test**

Run: `dotnet build && dotnet test`
Expected: Build succeeds, all tests pass.

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/FileIoController.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/Ipc/EmulatorTcpServer.cs e6502.Avalonia/Hardware/MidiEngine.cs
git commit -m "feat: integrate MIDPLAY/MIDSTOP with FileIoController and TCP server"
```

---

### Task 11: Documentation

**Files:**
- Modify: `docs/help/guides/sound-and-music.md`
- Create: `docs/help/reference/commands/midplay.md`
- Create: `docs/help/reference/commands/midstop.md`
- Modify: `CLAUDE.md`

**Step 1: Add MIDPLAY/MIDSTOP command reference pages**

Create `docs/help/reference/commands/midplay.md`:

```markdown
---
title: "MIDPLAY"
type: command
category: Sound
keywords: [midi, music, play]
syntax: "MIDPLAY \"filename\""
see_also: [midstop, sound-and-music]
---

Load and play a standard MIDI file through the SID music engine.

`MIDPLAY "song"` -- load `song.mid` from `~/e6502-programs` and start playback.

The engine auto-selects the 6 busiest MIDI channels, maps GM instruments
to SID waveforms, and converts MIDI velocity to per-voice volume.
Channel 10 (drums) is mapped to the noise waveform.

MIDI playback uses the MusicEngine (voices 1--6). Use `MIDSTOP` to stop.

```basic
10 MIDPLAY "entertainer"
20 REM WAIT FOR FINISH
30 IF PLAYING THEN GOTO 30
40 PRINT "DONE"
```

See the [Sound And Music](/docs/help/guides/sound-and-music.md) guide for more details.
```

Create `docs/help/reference/commands/midstop.md`:

```markdown
---
title: "MIDSTOP"
type: command
category: Sound
keywords: [midi, music, stop]
syntax: "MIDSTOP"
see_also: [midplay, sound-and-music]
---

Stop MIDI file playback.

See the [Sound And Music](/docs/help/guides/sound-and-music.md) guide for more details.
```

**Step 2: Add MIDI section to sound-and-music guide**

After the "SID File Playback" section (around line 456), add:

```markdown
## MIDI File Playback

NovaBASIC can load and play standard MIDI files (`.mid`), automatically
mapping General MIDI instruments and velocity to the SID sound engine:

| **Command** | **Description** |
| --- | --- |
| MIDPLAY "filename" | Load and play a `.mid` file. Auto-selects the 6 busiest channels. |
| MIDSTOP | Stop MIDI playback. |

```basic
10 MIDPLAY "entertainer"
20 FOR I = 1 TO 1800 : VSYNC : NEXT I
30 MIDSTOP
```

MIDI files are loaded from the `~/e6502-programs` directory. The `.mid`
extension is added automatically.

The MIDI engine maps General MIDI program numbers to eight SID instrument
buckets (pulse piano, saw strings, triangle flute, noise drums, etc.) and
converts MIDI note velocity to per-voice volume (0--15).

::: warning
MIDI playback uses the MusicEngine (voices 1--6). `SOUND` and `MUSIC`
commands will conflict while a MIDI file is playing. Call `MIDSTOP` before
using other music commands.
:::
```

**Step 3: Update CLAUDE.md memory map and description**

Add to the appropriate sections of CLAUDE.md mentioning MIDI support.

**Step 4: Commit**

```bash
git add docs/help/guides/sound-and-music.md docs/help/reference/commands/midplay.md docs/help/reference/commands/midstop.md CLAUDE.md
git commit -m "docs: add MIDPLAY/MIDSTOP documentation"
```

---

### Task 12: End-to-End Verification

**Step 1: Run full test suite**

Run: `dotnet test --verbosity normal`
Expected: All tests pass (726 existing + new MIDI tests).

**Step 2: Build release**

Run: `dotnet build -c Release`
Expected: Build succeeds.

**Step 3: Manual smoke test (if emulator available)**

1. Place a `.mid` file in `~/e6502-programs/`
2. Run the emulator: `dotnet run --project e6502.Avalonia`
3. Type: `MIDPLAY "filename"`
4. Verify audio plays
5. Type: `MIDSTOP`
6. Verify silence

**Step 4: Test mid2bas converter**

Run:
```bash
dotnet run --project e6502.Tools -- mid2bas ~/e6502-programs/entertainer.mid -o /tmp/test.bas --title "ENTERTAINER"
cat /tmp/test.bas
```

Verify the output has INSTRUMENT definitions, MUSIC statements, and MUSIC PLAY.

**Step 5: Commit any final fixes**

```bash
git add -A
git commit -m "chore: end-to-end verification complete"
```
