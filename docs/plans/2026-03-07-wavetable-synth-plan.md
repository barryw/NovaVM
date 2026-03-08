# Wavetable Synthesizer (WTS) Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add an 8-voice sample-based wavetable synthesizer that loads SF2 soundfonts from the host filesystem and exposes them via memory-mapped registers at $A140–$A1FF, integrated with the existing MusicEngine for 14-voice mixed SID+WTS playback.

**Architecture:** The WTS is a standalone audio engine (`WavetableSynth`) with its own stereo OpenAL output. Soundfonts are parsed into an internal `SampleBank` model at load time. `MusicEngine` expands from 6 to 14 voices — voices 0–5 route to SID, voices 6–13 route to WTS. `MidiPlayback` gains auto/manual/SID-only routing modes. Effects: reverb (Freeverb) and chorus (modulated delay), plus per-voice volume and panning.

**Tech Stack:** C#/.NET 10, MSTest, OpenAL (via existing P/Invoke in OpenAlRenderer)

**Design Doc:** `docs/plans/2026-03-07-wavetable-synth-design.md`

---

### Task 1: SampleBank Data Model

**Files:**
- Create: `e6502.Avalonia/Hardware/SampleBank.cs`
- Test: `e6502UnitTests/SampleBankTests.cs`

**Step 1: Write the failing test**

```csharp
// e6502UnitTests/SampleBankTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SampleBankTests
{
    [TestMethod]
    public void SampleRegion_StoresKeyRange()
    {
        var region = new SampleRegion
        {
            KeyRangeLo = 36,
            KeyRangeHi = 72,
            VelocityRangeLo = 0,
            VelocityRangeHi = 127,
            SampleData = new float[] { 0.5f, -0.5f },
            SampleRate = 44100,
            RootKey = 60,
            LoopStart = 0,
            LoopEnd = 1,
            LoopEnabled = false,
            AttackTime = 0.01f,
            DecayTime = 0.1f,
            SustainLevel = 0.7f,
            ReleaseTime = 0.3f
        };

        Assert.AreEqual(36, region.KeyRangeLo);
        Assert.AreEqual(72, region.KeyRangeHi);
        Assert.AreEqual(60, region.RootKey);
        Assert.AreEqual(44100, region.SampleRate);
    }

    [TestMethod]
    public void Instrument_FindsRegionByNoteAndVelocity()
    {
        var low = new SampleRegion
        {
            KeyRangeLo = 0, KeyRangeHi = 59,
            VelocityRangeLo = 0, VelocityRangeHi = 127,
            SampleData = new float[] { 0.1f },
            SampleRate = 44100, RootKey = 36
        };
        var high = new SampleRegion
        {
            KeyRangeLo = 60, KeyRangeHi = 127,
            VelocityRangeLo = 0, VelocityRangeHi = 127,
            SampleData = new float[] { 0.9f },
            SampleRate = 44100, RootKey = 72
        };

        var inst = new SampleInstrument
        {
            Name = "Piano",
            MidiBank = 0,
            MidiProgram = 0,
            Regions = new List<SampleRegion> { low, high }
        };

        var found = inst.FindRegion(48, 100);
        Assert.AreSame(low, found);

        found = inst.FindRegion(72, 64);
        Assert.AreSame(high, found);
    }

    [TestMethod]
    public void Instrument_FindRegion_ReturnsNullWhenNoMatch()
    {
        var region = new SampleRegion
        {
            KeyRangeLo = 60, KeyRangeHi = 72,
            VelocityRangeLo = 80, VelocityRangeHi = 127,
            SampleData = new float[] { 0.5f },
            SampleRate = 44100, RootKey = 66
        };

        var inst = new SampleInstrument
        {
            Name = "Test",
            Regions = new List<SampleRegion> { region }
        };

        Assert.IsNull(inst.FindRegion(48, 100));  // note out of range
        Assert.IsNull(inst.FindRegion(66, 40));    // velocity out of range
    }

    [TestMethod]
    public void SampleBank_StoresAndRetrievesInstruments()
    {
        var bank = new SampleBank();
        var inst = new SampleInstrument { Name = "Grand Piano", MidiProgram = 0 };
        bank.Instruments.Add(inst);

        Assert.AreEqual(1, bank.Instruments.Count);
        Assert.AreEqual("Grand Piano", bank.Instruments[0].Name);
    }
}
```

**Step 2: Run test to verify it fails**

Run: `dotnet test --filter "FullyQualifiedName~SampleBankTests" --verbosity normal`
Expected: FAIL — types do not exist

**Step 3: Write minimal implementation**

```csharp
// e6502.Avalonia/Hardware/SampleBank.cs
namespace e6502.Avalonia.Hardware;

/// <summary>
/// A single sample region — one key/velocity zone within an instrument.
/// </summary>
public sealed class SampleRegion
{
    public int KeyRangeLo { get; set; }
    public int KeyRangeHi { get; set; } = 127;
    public int VelocityRangeLo { get; set; }
    public int VelocityRangeHi { get; set; } = 127;
    public float[] SampleData { get; set; } = Array.Empty<float>();
    public int SampleRate { get; set; } = 44100;
    public int RootKey { get; set; } = 60;
    public int LoopStart { get; set; }
    public int LoopEnd { get; set; }
    public bool LoopEnabled { get; set; }
    public float AttackTime { get; set; } = 0.01f;
    public float DecayTime { get; set; } = 0.1f;
    public float SustainLevel { get; set; } = 0.7f;
    public float ReleaseTime { get; set; } = 0.3f;

    public bool Matches(int note, int velocity) =>
        note >= KeyRangeLo && note <= KeyRangeHi &&
        velocity >= VelocityRangeLo && velocity <= VelocityRangeHi;
}

/// <summary>
/// A single instrument composed of one or more sample regions.
/// </summary>
public sealed class SampleInstrument
{
    public string Name { get; set; } = "";
    public int MidiBank { get; set; }
    public int MidiProgram { get; set; }
    public List<SampleRegion> Regions { get; set; } = new();

    public SampleRegion? FindRegion(int note, int velocity)
    {
        foreach (var r in Regions)
            if (r.Matches(note, velocity))
                return r;
        return null;
    }
}

/// <summary>
/// A collection of instruments parsed from a soundfont file.
/// </summary>
public sealed class SampleBank
{
    public List<SampleInstrument> Instruments { get; } = new();
}
```

**Step 4: Run test to verify it passes**

Run: `dotnet test --filter "FullyQualifiedName~SampleBankTests" --verbosity normal`
Expected: PASS (all 4 tests)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/SampleBank.cs e6502UnitTests/SampleBankTests.cs
git commit -m "feat(wts): add SampleBank data model with instrument/region/key zone lookup"
```

---

### Task 2: SF2 Loader

**Files:**
- Create: `e6502.Avalonia/Hardware/Sf2Loader.cs`
- Test: `e6502UnitTests/Sf2LoaderTests.cs`
- Test data: need a small SF2 file or synthetic test data

SF2 is a RIFF container with three chunks: INFO, sdta (PCM16 samples), pdta (presets/instruments/zones/sample headers). The loader reads these and builds a `SampleBank`.

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/Sf2LoaderTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

namespace e6502UnitTests;

[TestClass]
public class Sf2LoaderTests
{
    // Helper: build a minimal valid SF2 in memory
    private static byte[] BuildMinimalSf2(
        string presetName = "Piano",
        int midiBank = 0,
        int midiProgram = 0,
        int rootKey = 60,
        int keyLo = 0,
        int keyHi = 127,
        short[] sampleData = null)
    {
        sampleData ??= new short[] { 0, 16383, 32767, 16383, 0, -16384, -32768, -16384,
                                      0, 16383, 32767, 16383, 0, -16384, -32768, -16384,
                                      // 46 zero pad samples (SF2 requires 46 trailing zeros)
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
        // Build using Sf2Builder helper (tested separately)
        return Sf2Builder.Build(presetName, midiBank, midiProgram, rootKey, keyLo, keyHi, sampleData);
    }

    [TestMethod]
    public void Load_MinimalSf2_ReturnsOneInstrument()
    {
        byte[] sf2 = BuildMinimalSf2();
        var bank = Sf2Loader.Load(new MemoryStream(sf2));

        Assert.AreEqual(1, bank.Instruments.Count);
        Assert.AreEqual("Piano", bank.Instruments[0].Name);
    }

    [TestMethod]
    public void Load_SetsCorrectMidiProgram()
    {
        byte[] sf2 = BuildMinimalSf2(midiProgram: 42);
        var bank = Sf2Loader.Load(new MemoryStream(sf2));

        Assert.AreEqual(42, bank.Instruments[0].MidiProgram);
    }

    [TestMethod]
    public void Load_SetsRootKeyAndKeyRange()
    {
        byte[] sf2 = BuildMinimalSf2(rootKey: 48, keyLo: 36, keyHi: 72);
        var bank = Sf2Loader.Load(new MemoryStream(sf2));

        var region = bank.Instruments[0].Regions[0];
        Assert.AreEqual(48, region.RootKey);
        Assert.AreEqual(36, region.KeyRangeLo);
        Assert.AreEqual(72, region.KeyRangeHi);
    }

    [TestMethod]
    public void Load_ConvertsSampleDataToFloat()
    {
        var samples = new short[] { 0, 16384, 32767, -32768,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
        byte[] sf2 = BuildMinimalSf2(sampleData: samples);
        var bank = Sf2Loader.Load(new MemoryStream(sf2));

        var data = bank.Instruments[0].Regions[0].SampleData;
        Assert.AreEqual(0f, data[0], 0.001f);
        Assert.IsTrue(data[1] > 0.49f);  // 16384/32768
        Assert.IsTrue(data[2] > 0.99f);  // 32767/32768
        Assert.IsTrue(data[3] < -0.99f); // -32768/32768
    }

    [TestMethod]
    public void Load_InvalidData_ThrowsInvalidDataException()
    {
        Assert.ThrowsException<InvalidDataException>(() =>
            Sf2Loader.Load(new MemoryStream(new byte[] { 0, 1, 2, 3 })));
    }
}
```

**Step 2: Run test to verify it fails**

Run: `dotnet test --filter "FullyQualifiedName~Sf2LoaderTests" --verbosity normal`
Expected: FAIL — Sf2Loader does not exist

**Step 3: Write implementation**

Create `e6502.Avalonia/Hardware/Sf2Loader.cs`. The SF2 format is RIFF-based:

```
RIFF "sfbk"
  LIST "INFO" — metadata
  LIST "sdta"
    "smpl" — raw PCM16 sample pool
  LIST "pdta"
    "phdr" — preset headers (38 bytes each)
    "pbag" — preset zone bags (4 bytes each)
    "pmod" — preset modulators
    "pgen" — preset generators (4 bytes each)
    "inst" — instrument headers (22 bytes each)
    "ibag" — instrument zone bags (4 bytes each)
    "imod" — instrument modulators
    "igen" — instrument generators (4 bytes each)
    "shdr" — sample headers (46 bytes each)
```

Implementation approach:
1. Parse RIFF container to find chunks
2. Extract `smpl` data (PCM16 → float[])
3. Parse `shdr` for sample offsets/loop points/root key/sample rate
4. Parse `inst` + `ibag` + `igen` to build instruments with zone key ranges
5. Parse `phdr` + `pbag` + `pgen` to build presets mapping to instruments
6. Assemble into `SampleBank`

Also create `Sf2Builder` (internal test helper) to generate minimal valid SF2 bytes for testing.

The implementer should reference the SF2 spec for exact chunk/record layouts. Key generator IDs:
- 43 = keyRange (lo/hi bytes)
- 44 = velRange (lo/hi bytes)
- 53 = sampleID (index into shdr)
- 54 = sampleModes (1=loop)
- 58 = overridingRootKey

**Step 4: Run test to verify it passes**

Run: `dotnet test --filter "FullyQualifiedName~Sf2LoaderTests" --verbosity normal`
Expected: PASS (all 5 tests)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/Sf2Loader.cs e6502UnitTests/Sf2LoaderTests.cs
git commit -m "feat(wts): add SF2 soundfont loader with RIFF parser"
```

---

### Task 3: WavetableSynth Core — Voice Playback

**Files:**
- Create: `e6502.Avalonia/Hardware/WavetableSynth.cs`
- Test: `e6502UnitTests/WavetableSynthTests.cs`

The synth renders 8 voices at 44.1kHz into stereo PCM16. Each voice has: phase accumulator, ADSR envelope, volume, panning.

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/WavetableSynthTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class WavetableSynthTests
{
    private static SampleBank MakeSineBank()
    {
        // Single instrument, single region, one cycle of sine at root key 60
        int len = 512;
        var data = new float[len];
        for (int i = 0; i < len; i++)
            data[i] = MathF.Sin(2f * MathF.PI * i / len);

        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Sine",
            MidiProgram = 0,
            Regions = { new SampleRegion
            {
                KeyRangeLo = 0, KeyRangeHi = 127,
                VelocityRangeLo = 0, VelocityRangeHi = 127,
                SampleData = data,
                SampleRate = 44100,
                RootKey = 60,
                LoopStart = 0, LoopEnd = len - 1, LoopEnabled = true,
                AttackTime = 0f, DecayTime = 0f,
                SustainLevel = 1f, ReleaseTime = 0.01f
            }}
        });
        return bank;
    }

    [TestMethod]
    public void NoteOn_ProducesNonSilentOutput()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.NoteOn(0, 60, 127, 0);  // voice 0, middle C, full velocity, instrument 0
        short[] samples = synth.RenderSamples(1024);

        // Stereo: 1024 frames = 2048 samples (L,R,L,R,...)
        Assert.AreEqual(2048, samples.Length);
        Assert.IsTrue(samples.Any(s => Math.Abs(s) > 100), "Expected non-silent output");
    }

    [TestMethod]
    public void NoteOff_EventuallyProducesSilence()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.NoteOn(0, 60, 127, 0);
        synth.RenderSamples(512);  // let attack settle
        synth.NoteOff(0);

        // Render enough samples for release to complete (0.01s = ~441 samples)
        short[] tail = synth.RenderSamples(2048);
        // Last samples should be near-silent
        bool silent = tail.Skip(tail.Length - 200).All(s => Math.Abs(s) < 50);
        Assert.IsTrue(silent, "Expected silence after release");
    }

    [TestMethod]
    public void Volume_ScalesOutput()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.SetVolume(0, 255);
        synth.NoteOn(0, 60, 127, 0);
        short[] loud = synth.RenderSamples(512);
        synth.NoteOff(0);
        synth.RenderSamples(4096); // drain

        synth.SetVolume(0, 64);
        synth.NoteOn(0, 60, 127, 0);
        short[] quiet = synth.RenderSamples(512);

        double peakLoud = loud.Max(s => Math.Abs((int)s));
        double peakQuiet = quiet.Max(s => Math.Abs((int)s));
        Assert.IsTrue(peakQuiet < peakLoud * 0.5, "Volume 64 should be quieter than 255");
    }

    [TestMethod]
    public void Panning_AffectsStereoBalance()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.SetPanning(0, 0);  // hard left
        synth.NoteOn(0, 60, 127, 0);
        short[] panned = synth.RenderSamples(512);

        // Stereo interleaved: even indices = left, odd = right
        double leftPeak = 0, rightPeak = 0;
        for (int i = 0; i < panned.Length; i += 2)
        {
            leftPeak = Math.Max(leftPeak, Math.Abs(panned[i]));
            rightPeak = Math.Max(rightPeak, Math.Abs(panned[i + 1]));
        }

        Assert.IsTrue(leftPeak > rightPeak * 3, "Hard-left pan should have much louder left channel");
    }

    [TestMethod]
    public void NoBank_NoteOn_ProducesSilence()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.NoteOn(0, 60, 127, 0);
        short[] samples = synth.RenderSamples(512);
        Assert.IsTrue(samples.All(s => s == 0));
    }

    [TestMethod]
    public void ActiveVoiceMask_ReflectsPlayingVoices()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        Assert.AreEqual(0, synth.ActiveVoiceMask);
        synth.NoteOn(0, 60, 127, 0);
        synth.NoteOn(3, 72, 100, 0);
        Assert.AreEqual(0b00001001, synth.ActiveVoiceMask);
    }

    [TestMethod]
    public void PitchBend_ChangesFrequency()
    {
        var synth = new WavetableSynth(enableAudio: false);
        synth.LoadBank(MakeSineBank());

        synth.NoteOn(0, 60, 127, 0);
        short[] normal = synth.RenderSamples(512);

        synth.NoteOff(0);
        synth.RenderSamples(4096); // drain

        synth.SetPitchBend(0, 0xC000); // bend up
        synth.NoteOn(0, 60, 127, 0);
        short[] bent = synth.RenderSamples(512);

        // Count zero crossings — bent up should have more (higher frequency)
        int crossNormal = CountZeroCrossings(normal);
        int crossBent = CountZeroCrossings(bent);
        Assert.IsTrue(crossBent > crossNormal, "Pitch bend up should increase frequency");
    }

    private static int CountZeroCrossings(short[] stereo)
    {
        int count = 0;
        for (int i = 2; i < stereo.Length; i += 2) // left channel only
            if ((stereo[i - 2] >= 0) != (stereo[i] >= 0))
                count++;
        return count;
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~WavetableSynthTests" --verbosity normal`
Expected: FAIL — WavetableSynth does not exist

**Step 3: Write implementation**

Create `e6502.Avalonia/Hardware/WavetableSynth.cs`:

```csharp
namespace e6502.Avalonia.Hardware;

public sealed class WavetableSynth : IDisposable
{
    public const int VoiceCount = 8;
    private const int SampleRate = 44100;

    private readonly WtsVoice[] _voices = new WtsVoice[VoiceCount];
    private SampleBank? _bank;
    private readonly OpenAlRenderer? _renderer;
    private readonly object _lock = new();

    // Global state
    private byte _reverbLevel = 80;
    private byte _chorusLevel = 40;
    private byte _masterVolume = 255;

    public WavetableSynth(bool enableAudio = false) { ... }

    public void LoadBank(SampleBank bank) { ... }
    public void NoteOn(int voice, int note, int velocity, int instrument) { ... }
    public void NoteOff(int voice) { ... }
    public void SetVolume(int voice, byte volume) { ... }
    public void SetPanning(int voice, byte pan) { ... }
    public void SetPitchBend(int voice, ushort bend) { ... }
    public byte ActiveVoiceMask { get; }
    public short[] RenderSamples(int frameCount) { ... }
    public void Dispose() { ... }
}
```

Key implementation details:
- `WtsVoice` inner class: phase (double), envelope state, velocity, volume (byte, 0-255), pan (byte, 0-255), pitchBend (ushort), active region, active flag
- `RenderSamples` returns interleaved stereo: `short[frameCount * 2]`
- Phase increment: `region.SampleRate * pitchRatio / 44100.0` where `pitchRatio = Math.Pow(2, (note - rootKey + bendSemitones) / 12.0)`
- Pitch bend: `bendSemitones = ((pitchBend - 0x8000) / 8192.0) * 2.0` (±2 semitones)
- ADSR: linear segments, rates derived from time parameters and sample rate
- Panning: equal-power `left = cos(pan * PI/2 / 255)`, `right = sin(pan * PI/2 / 255)`
- Volume: `voice.Volume / 255f`
- When `enableAudio` is true, create `OpenAlRenderer` with stereo format (see Task 5)
- When false, `RenderSamples` is still callable for testing

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~WavetableSynthTests" --verbosity normal`
Expected: PASS (all 7 tests)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/WavetableSynth.cs e6502UnitTests/WavetableSynthTests.cs
git commit -m "feat(wts): add 8-voice wavetable synth core with ADSR, volume, panning, pitch bend"
```

---

### Task 4: Reverb and Chorus Effects

**Files:**
- Create: `e6502.Avalonia/Hardware/ReverbEffect.cs`
- Create: `e6502.Avalonia/Hardware/ChorusEffect.cs`
- Test: `e6502UnitTests/AudioEffectsTests.cs`
- Modify: `e6502.Avalonia/Hardware/WavetableSynth.cs` — integrate effects into render pipeline

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/AudioEffectsTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class AudioEffectsTests
{
    [TestMethod]
    public void Reverb_ImpulseProducesTail()
    {
        var reverb = new ReverbEffect(44100);
        // Feed an impulse (one loud sample) then silence
        float[] left = new float[8820];   // 200ms
        float[] right = new float[8820];
        left[0] = 1.0f;
        right[0] = 1.0f;

        reverb.Process(left, right);

        // Reverb tail: samples after the impulse should have non-zero energy
        double tailEnergy = 0;
        for (int i = 4410; i < 8820; i++)
            tailEnergy += left[i] * left[i] + right[i] * right[i];

        Assert.IsTrue(tailEnergy > 0.001, "Reverb should produce a decay tail");
    }

    [TestMethod]
    public void Reverb_SilenceInSilenceOut()
    {
        var reverb = new ReverbEffect(44100);
        float[] left = new float[1024];
        float[] right = new float[1024];

        reverb.Process(left, right);

        Assert.IsTrue(left.All(s => s == 0f));
        Assert.IsTrue(right.All(s => s == 0f));
    }

    [TestMethod]
    public void Chorus_ModulatesSignal()
    {
        var chorus = new ChorusEffect(44100);
        // Feed a constant tone
        float[] left = new float[4410];
        float[] right = new float[4410];
        for (int i = 0; i < left.Length; i++)
        {
            float v = MathF.Sin(2f * MathF.PI * 440f * i / 44100f);
            left[i] = v;
            right[i] = v;
        }

        float[] origLeft = (float[])left.Clone();
        chorus.Process(left, right);

        // Chorus should modify the signal (add modulated delay)
        double diff = 0;
        for (int i = 0; i < left.Length; i++)
            diff += Math.Abs(left[i] - origLeft[i]);

        Assert.IsTrue(diff > 1.0, "Chorus should modify the signal");
    }

    [TestMethod]
    public void Chorus_SilenceInSilenceOut()
    {
        var chorus = new ChorusEffect(44100);
        float[] left = new float[1024];
        float[] right = new float[1024];

        chorus.Process(left, right);

        Assert.IsTrue(left.All(s => s == 0f));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~AudioEffectsTests" --verbosity normal`
Expected: FAIL — types do not exist

**Step 3: Write implementation**

`ReverbEffect.cs` — Freeverb algorithm:
- 8 parallel comb filters (stereo, different tunings L/R)
- 4 series allpass filters
- Parameters: room size, damping, wet/dry mix
- Process in-place on float[] left/right arrays

`ChorusEffect.cs` — Modulated delay:
- Circular delay buffer (~30ms)
- LFO (sine, ~1Hz) modulates read position ±2ms
- Mix: dry + wet (delayed) signal
- Process in-place on float[] left/right arrays

Then integrate into `WavetableSynth.RenderSamples()`:
1. Mix all voices into float[] stereoLeft, stereoRight
2. `_chorus.Process(stereoLeft, stereoRight)` scaled by `_chorusLevel / 255f`
3. `_reverb.Process(stereoLeft, stereoRight)` scaled by `_reverbLevel / 255f`
4. Apply `_masterVolume / 255f`, clamp, convert to PCM16

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~AudioEffectsTests" --verbosity normal`
Expected: PASS (all 4 tests)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/ReverbEffect.cs e6502.Avalonia/Hardware/ChorusEffect.cs e6502UnitTests/AudioEffectsTests.cs e6502.Avalonia/Hardware/WavetableSynth.cs
git commit -m "feat(wts): add reverb and chorus effects, integrate into render pipeline"
```

---

### Task 5: OpenAlRenderer Stereo Support

**Files:**
- Modify: `e6502.Avalonia/Hardware/OpenAlRenderer.cs`
- Modify: `e6502.Avalonia/Hardware/WavetableSynth.cs` — wire up audio output

The existing `OpenAlRenderer` is mono-only (`AlFormatMono16`). The WTS needs stereo output. Rather than changing the existing renderer (which would break SID), add a stereo mode flag.

**Step 1: Write the failing test**

```csharp
// Add to e6502UnitTests/WavetableSynthTests.cs
[TestMethod]
public void Synth_WithAudioDisabled_StillRendersStereo()
{
    var synth = new WavetableSynth(enableAudio: false);
    short[] samples = synth.RenderSamples(100);
    Assert.AreEqual(200, samples.Length); // 100 frames × 2 channels
}
```

**Step 2: Run to verify it passes** (should already pass from Task 3)

Run: `dotnet test --filter "Synth_WithAudioDisabled_StillRendersStereo" --verbosity normal`
Expected: PASS

**Step 3: Modify OpenAlRenderer**

In `e6502.Avalonia/Hardware/OpenAlRenderer.cs`:
- Add constructor parameter: `bool stereo = false`
- Add constant: `const int AlFormatStereo16 = 0x1102`
- Store `_stereo` flag
- In buffer data call: use `AlFormatStereo16` when stereo, double the byte count
- Sample provider callback returns `short[count * 2]` for stereo (interleaved L,R,L,R)
- `SamplesPerBuffer` stays 1024 (frames), but byte size doubles for stereo

In `WavetableSynth` constructor when `enableAudio` is true:
```csharp
_renderer = new OpenAlRenderer(count => RenderSamples(count), SampleRate, stereo: true);
```

**Step 4: Verify existing SID tests still pass**

Run: `dotnet test --filter "FullyQualifiedName~AvaloniaSidTests" --verbosity normal`
Expected: PASS (no regression)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/OpenAlRenderer.cs e6502.Avalonia/Hardware/WavetableSynth.cs
git commit -m "feat(wts): add stereo mode to OpenAlRenderer, wire up WTS audio output"
```

---

### Task 6: VgcConstants — WTS Register Addresses

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs` — add WTS register constants

**Step 1: No test needed — constants only**

**Step 2: Add constants**

Add after the Music status registers section (after line 121) in `e6502.Avalonia/Hardware/VgcConstants.cs`:

```csharp
    // -------------------------------------------------------------------------
    // Wavetable Synthesizer registers ($A140-$A1DF)
    // -------------------------------------------------------------------------

    public const int WtsBase             = 0xA140;

    // Per-voice registers: 8 voices × 8 bytes ($A140-$A17F)
    public const int WtsVoiceBase        = 0xA140; // voice 0 starts here
    public const int WtsVoiceStride      = 8;      // 8 bytes per voice
    public const int WtsVoiceEnd         = 0xA17F; // voice 7 ends here

    // Voice register offsets (add to WtsVoiceBase + voice * WtsVoiceStride)
    public const int WtsVoiceNote        = 0;  // W: MIDI note (1-127), write 0 = note off
    public const int WtsVoiceVelocity    = 1;  // W: velocity (0-127)
    public const int WtsVoiceInstrument  = 2;  // W: instrument index (0-255)
    public const int WtsVoiceVolume      = 3;  // R/W: volume (0-255, default 255)
    public const int WtsVoicePanning     = 4;  // R/W: pan (0=left, 128=center, 255=right)
    public const int WtsVoicePitchBendLo = 5;  // W: pitch bend low byte
    public const int WtsVoicePitchBendHi = 6;  // W: pitch bend high byte ($80=center)
    public const int WtsVoiceStatus      = 7;  // R: bit 0=active, bit 1=releasing

    // Global registers ($A180-$A19F)
    public const int WtsGlobalBase       = 0xA180;
    public const int WtsReverbLevel      = 0xA180; // R/W: 0-255 (default 80)
    public const int WtsChorusLevel      = 0xA181; // R/W: 0-255 (default 40)
    public const int WtsMasterVolume     = 0xA182; // R/W: 0-255 (default 255)
    public const int WtsSoundfontStatus  = 0xA183; // R: 0=none, 1=ready, 2=loading, $FF=error
    public const int WtsInstrumentCount  = 0xA184; // R: instrument count in loaded soundfont
    public const int WtsCommand          = 0xA185; // W: command byte
    public const int WtsCommandArg       = 0xA186; // W: command argument
    public const int WtsActiveVoices     = 0xA187; // R: bitmask of active voices

    // Instrument enumeration ($A1A0-$A1DF)
    public const int WtsEnumBase         = 0xA1A0;
    public const int WtsEnumIndex        = 0xA1A0; // W: write index to populate buffer
    public const int WtsEnumBank         = 0xA1A1; // R: MIDI bank
    public const int WtsEnumProgram      = 0xA1A2; // R: MIDI program
    public const int WtsEnumName         = 0xA1A3; // R: null-terminated name (28 chars)
    public const int WtsEnumEnd          = 0xA1DF;

    public const int WtsEnd              = 0xA1DF;

    // WTS commands (write to WtsCommand)
    public const byte WtsCmdAllNotesOff  = 0x01;
    public const byte WtsCmdResetEffects = 0x02;

    // FileIO command for soundfont loading
    public const byte FioCmdSfLoad       = 0x15;
```

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat(wts): add WTS register address constants to VgcConstants"
```

---

### Task 7: CompositeBusDevice — WTS Register Routing

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` — instantiate WTS, route registers, add tick
- Test: `e6502UnitTests/WtsRegisterTests.cs`

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/WtsRegisterTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class WtsRegisterTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    [TestMethod]
    public void WtsProperty_IsExposed()
    {
        var bus = MakeBus();
        Assert.IsNotNull(bus.Wts);
    }

    [TestMethod]
    public void WriteVoiceVolume_ReadBack()
    {
        var bus = MakeBus();
        int addr = VgcConstants.WtsVoiceBase + 0 * VgcConstants.WtsVoiceStride + VgcConstants.WtsVoiceVolume;
        bus.Write((ushort)addr, 200);
        Assert.AreEqual(200, bus.Read((ushort)addr));
    }

    [TestMethod]
    public void WriteGlobalReverb_ReadBack()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.WtsReverbLevel, 120);
        Assert.AreEqual(120, bus.Read((ushort)VgcConstants.WtsReverbLevel));
    }

    [TestMethod]
    public void WriteNoteOn_ActivatesVoice()
    {
        var bus = MakeBus();
        // Load a bank first
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Test",
            Regions = { new SampleRegion
            {
                SampleData = new float[512],
                SampleRate = 44100,
                RootKey = 60,
                LoopEnabled = true, LoopEnd = 511
            }}
        });
        bus.Wts.LoadBank(bank);

        int voiceBase = VgcConstants.WtsVoiceBase;
        bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceInstrument), 0);
        bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceVelocity), 100);
        bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceNote), 60); // triggers note-on

        Assert.AreEqual(1, bus.Read((ushort)VgcConstants.WtsActiveVoices) & 0x01);
    }

    [TestMethod]
    public void ReadSoundfontStatus_NoBank_ReturnsZero()
    {
        var bus = MakeBus();
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsSoundfontStatus));
    }

    [TestMethod]
    public void ReadInstrumentCount_WithBank()
    {
        var bus = MakeBus();
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument { Name = "A" });
        bank.Instruments.Add(new SampleInstrument { Name = "B" });
        bus.Wts.LoadBank(bank);

        Assert.AreEqual(2, bus.Read((ushort)VgcConstants.WtsInstrumentCount));
    }

    [TestMethod]
    public void EnumInstrument_ReadsNameBack()
    {
        var bus = MakeBus();
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument { Name = "Grand Piano", MidiBank = 0, MidiProgram = 0 });
        bus.Wts.LoadBank(bank);

        bus.Write((ushort)VgcConstants.WtsEnumIndex, 0); // select instrument 0
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsEnumBank));
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsEnumProgram));
        Assert.AreEqual((byte)'G', bus.Read((ushort)VgcConstants.WtsEnumName));
        Assert.AreEqual((byte)'r', bus.Read((ushort)(VgcConstants.WtsEnumName + 1)));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~WtsRegisterTests" --verbosity normal`
Expected: FAIL

**Step 3: Modify CompositeBusDevice**

1. Add field: `private readonly WavetableSynth _wts;`
2. Add property: `public WavetableSynth Wts => _wts;`
3. In constructor: `_wts = new WavetableSynth(enableSound);`
4. Add `Read` routing for $A140–$A1DF → delegate to `_wts.ReadRegister(address)`
5. Add `Write` routing for $A140–$A1DF → delegate to `_wts.WriteRegister(address, value)`
6. In `Dispose()`: `_wts.Dispose()`

The `WavetableSynth` needs `ReadRegister(ushort)` and `WriteRegister(ushort, byte)` methods that handle:
- Per-voice registers: note-on trigger on NoteNum write, volume/pan read-back
- Global registers: reverb/chorus/master volume read-back, command dispatch
- Enum registers: instrument name/bank/program read-back
- Status registers: soundfont status, instrument count, active voice mask

Note-on is triggered by writing to the NoteNum register. The synth must first latch velocity and instrument from prior writes, then fire note-on when NoteNum is written with a non-zero value. Writing 0 to NoteNum fires note-off.

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~WtsRegisterTests" --verbosity normal`
Expected: PASS (all 6 tests)

Also run: `dotnet test --filter "FullyQualifiedName~AvaloniaCompositeBusTests" --verbosity normal`
Expected: PASS (no regression)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/Hardware/WavetableSynth.cs e6502UnitTests/WtsRegisterTests.cs
git commit -m "feat(wts): wire WTS registers into CompositeBusDevice memory map"
```

---

### Task 8: MusicEngine — Expand to 14 Voices

**Files:**
- Modify: `e6502.Avalonia/Hardware/MusicEngine.cs`
- Test: `e6502UnitTests/MusicEngineWtsTests.cs`

This is the most complex integration task. MusicEngine currently has `VoiceCount = 6` with all voices routing to SID. We expand to 14 voices where voices 6–13 route to WTS.

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/MusicEngineWtsTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MusicEngineWtsTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    [TestMethod]
    public void VoiceCount_Is14()
    {
        var bus = MakeBus();
        // MusicEngine should now have 14 voices
        Assert.AreEqual(14, bus.Music.TotalVoiceCount);
    }

    [TestMethod]
    public void DirectNoteOn_SidVoice_WritesSidRegisters()
    {
        var bus = MakeBus();
        var engine = bus.Music;

        engine.DirectNoteOn(0, 60, 100, 0); // voice 0 → SID1
        // SID1 voice 0 control register should have gate bit set
        byte ctrl = bus.Sid.Read(0xD404);
        Assert.IsTrue((ctrl & 0x01) != 0, "SID voice should be gated");
    }

    [TestMethod]
    public void DirectNoteOn_WtsVoice_ActivatesWts()
    {
        var bus = MakeBus();
        // Load a sample bank into WTS
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Test",
            Regions = { new SampleRegion
            {
                SampleData = new float[512],
                SampleRate = 44100, RootKey = 60,
                LoopEnabled = true, LoopEnd = 511
            }}
        });
        bus.Wts.LoadBank(bank);

        bus.Music.DirectNoteOn(6, 60, 100, 0); // voice 6 → WTS voice 0
        Assert.IsTrue((bus.Wts.ActiveVoiceMask & 0x01) != 0, "WTS voice 0 should be active");
    }

    [TestMethod]
    public void DirectNoteOff_WtsVoice_ReleasesWts()
    {
        var bus = MakeBus();
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Test",
            Regions = { new SampleRegion
            {
                SampleData = new float[512],
                SampleRate = 44100, RootKey = 60,
                LoopEnabled = true, LoopEnd = 511,
                ReleaseTime = 0f // instant release for test
            }}
        });
        bus.Wts.LoadBank(bank);

        bus.Music.DirectNoteOn(6, 60, 100, 0);
        bus.Music.DirectNoteOff(6);
        // Render a batch to let release complete
        bus.Wts.RenderSamples(4096);
        Assert.AreEqual(0, bus.Wts.ActiveVoiceMask & 0x01);
    }

    [TestMethod]
    public void GetCurrentMidi_WorksForBothSidAndWts()
    {
        var bus = MakeBus();
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Test",
            Regions = { new SampleRegion
            {
                SampleData = new float[512],
                SampleRate = 44100, RootKey = 60,
                LoopEnabled = true, LoopEnd = 511
            }}
        });
        bus.Wts.LoadBank(bank);

        bus.Music.DirectNoteOn(2, 48, 100, 0);  // SID voice 2
        bus.Music.DirectNoteOn(8, 72, 100, 0);  // WTS voice 2

        Assert.AreEqual(48, bus.Music.GetVoiceMidi(2));
        Assert.AreEqual(72, bus.Music.GetVoiceMidi(8));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MusicEngineWtsTests" --verbosity normal`
Expected: FAIL

**Step 3: Modify MusicEngine**

Key changes to `e6502.Avalonia/Hardware/MusicEngine.cs`:

1. Change `VoiceCount` from 6 to 14
2. Add `private const int SidVoiceCount = 6;`
3. Add field: `private readonly WavetableSynth? _wts;`
4. Update constructor to accept WTS: get it from `bus.Wts`
5. Add `public int TotalVoiceCount => VoiceCount;`
6. Add `public int GetVoiceMidi(int voice)` to return `_voices[voice].CurrentMidi`

7. Modify `ChipFor(int v)` — only valid for voices 0–5
8. Add routing check: `private bool IsWtsVoice(int v) => v >= SidVoiceCount;`
9. Add `private int WtsLocalVoice(int v) => v - SidVoiceCount;`

10. Modify `DirectNoteOn`:
```csharp
public void DirectNoteOn(int voiceIndex, int midiNote, int velocity, int instrumentId = 0)
{
    if (voiceIndex < 0 || voiceIndex >= VoiceCount) return;

    if (IsWtsVoice(voiceIndex))
    {
        _wts?.NoteOn(WtsLocalVoice(voiceIndex), midiNote, velocity, instrumentId);
        _voices[voiceIndex].CurrentMidi = midiNote;
        return;
    }

    // ... existing SID logic unchanged ...
}
```

11. Similarly modify `DirectNoteOff` and `DirectNoteSlide` to route WTS voices
12. Update `_stealPriority` default to include voices 6–13
13. Expand `_voices` array to 14 entries

**Important:** All existing SID voice behavior (voices 0–5) must remain exactly the same. Only add the WTS routing branch for voices 6–13.

**Step 4: Run ALL music engine tests**

Run: `dotnet test --filter "FullyQualifiedName~MusicEngine" --verbosity normal`
Expected: PASS (both old and new tests)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MusicEngine.cs e6502UnitTests/MusicEngineWtsTests.cs
git commit -m "feat(wts): expand MusicEngine to 14 voices with SID/WTS routing"
```

---

### Task 9: MidiPlayback — Routing Modes

**Files:**
- Modify: `e6502.Avalonia/Hardware/MidiPlayback.cs`
- Test: `e6502UnitTests/MidiPlaybackWtsTests.cs`

Add three routing modes: Auto (WTS preferred), Manual (per-channel table), SID-only (existing behavior).

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/MidiPlaybackWtsTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MidiPlaybackWtsTests
{
    [TestMethod]
    public void RoutingMode_DefaultIsAuto()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        Assert.AreEqual(MidiRoutingMode.Auto, bus.MidiPlayback.RoutingMode);
    }

    [TestMethod]
    public void RoutingMode_SidOnly_Uses6Voices()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.MidiPlayback.RoutingMode = MidiRoutingMode.SidOnly;
        Assert.AreEqual(6, bus.MidiPlayback.AvailableVoiceCount);
    }

    [TestMethod]
    public void RoutingMode_Auto_Uses14Voices()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.MidiPlayback.RoutingMode = MidiRoutingMode.Auto;
        Assert.AreEqual(14, bus.MidiPlayback.AvailableVoiceCount);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MidiPlaybackWtsTests" --verbosity normal`
Expected: FAIL

**Step 3: Modify MidiPlayback**

Add to `e6502.Avalonia/Hardware/MidiPlayback.cs`:

```csharp
public enum MidiRoutingMode { Auto, Manual, SidOnly }
```

Key changes:
1. Add `public MidiRoutingMode RoutingMode { get; set; } = MidiRoutingMode.Auto;`
2. Add `public int AvailableVoiceCount` — returns 14 for Auto/Manual, 6 for SidOnly
3. In `BuildTimeline()`: allocate across 14 voices (Auto) or 6 (SidOnly)
4. Auto mode: assign channels to WTS voices (6–13) first, overflow to SID (0–5)
5. Manual mode: use a `int[16]` channel-to-voicePool mapping table
6. SidOnly: existing 6-voice behavior exactly as-is

The voice allocation in `BuildTimeline` currently selects the 6 busiest channels and maps them to voices 0–5. For Auto mode:
- Select up to 14 busiest channels
- Assign first 8 to WTS voices (6–13)
- Assign remaining 6 to SID voices (0–5)
- Channel 10 (drums) → SID voice if no drum samples, WTS if samples available

**Step 4: Run all MIDI tests**

Run: `dotnet test --filter "FullyQualifiedName~MidiPlayback" --verbosity normal`
Expected: PASS (both old and new)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MidiPlayback.cs e6502UnitTests/MidiPlaybackWtsTests.cs
git commit -m "feat(wts): add auto/manual/SID-only MIDI routing modes for 14 voices"
```

---

### Task 10: FileIoController — SfLoad Command

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs`
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs` (already done in Task 6)
- Test: `e6502UnitTests/FileIoSfLoadTests.cs`

Follow the existing MidPlay pattern: read filename from buffer, load file, parse, configure engine.

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/FileIoSfLoadTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.IO;

namespace e6502UnitTests;

[TestClass]
public class FileIoSfLoadTests
{
    [TestMethod]
    public void SfLoad_ValidFile_SetsSoundfontStatusReady()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        string dir = Path.Combine(Path.GetTempPath(), "e6502-test-sfload-" + Guid.NewGuid());
        string sfDir = Path.Combine(dir, "soundfonts");
        Directory.CreateDirectory(sfDir);

        try
        {
            // Write a minimal SF2 file
            byte[] sf2 = Sf2Builder.Build("TestPiano", 0, 0, 60, 0, 127);
            string path = Path.Combine(sfDir, "test.sf2");
            File.WriteAllBytes(path, sf2);

            // Point FileIO at our temp dir
            bus.Fio.SetSaveDirectory(dir);

            // Write filename to FIO buffer
            WriteFilename(bus, "test.sf2");

            // Execute SfLoad command
            bus.Write(VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

            Assert.AreEqual(VgcConstants.FioStatusOk, bus.Read(VgcConstants.FioStatus));
            Assert.AreEqual(1, bus.Read((ushort)VgcConstants.WtsSoundfontStatus));
            Assert.AreEqual(1, bus.Read((ushort)VgcConstants.WtsInstrumentCount));
        }
        finally
        {
            Directory.Delete(dir, true);
        }
    }

    [TestMethod]
    public void SfLoad_FileNotFound_SetsError()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        WriteFilename(bus, "nonexistent.sf2");
        bus.Write(VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

        Assert.AreEqual(VgcConstants.FioStatusError, bus.Read(VgcConstants.FioStatus));
    }

    private static void WriteFilename(CompositeBusDevice bus, string name)
    {
        bus.Write(VgcConstants.FioNameLen, (byte)name.Length);
        for (int i = 0; i < name.Length; i++)
            bus.Write((ushort)(VgcConstants.FioName + i), (byte)name[i]);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~FileIoSfLoadTests" --verbosity normal`
Expected: FAIL

**Step 3: Modify FileIoController**

1. Add `WavetableSynth? _wts` parameter to constructor
2. In `CompositeBusDevice` constructor, pass `_wts` to `FileIoController`
3. Add case in command dispatch: `VgcConstants.FioCmdSfLoad => DoSfLoad()`
4. Implement `DoSfLoad()`:

```csharp
private void DoSfLoad()
{
    if (_wts is null) { SetError(FioErrIo); return; }

    string? filename = ReadFilename();
    if (filename is null) { SetError(FioErrIo); return; }

    // Look in soundfonts/ subdirectory
    string path = ResolvePath(Path.Combine("soundfonts", filename));
    if (!File.Exists(path)) { SetError(FioErrNotFound); return; }

    try
    {
        using var stream = File.OpenRead(path);
        var bank = Sf2Loader.Load(stream);
        _wts.LoadBank(bank);
        SetOk();
    }
    catch
    {
        SetError(FioErrIo);
    }
}
```

**Step 4: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~FileIoSfLoadTests" --verbosity normal`
Expected: PASS

Also: `dotnet test --filter "FullyQualifiedName~FileIoController" --verbosity normal`
Expected: PASS (no regression)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502UnitTests/FileIoSfLoadTests.cs
git commit -m "feat(wts): add SfLoad command to FileIoController for soundfont loading"
```

---

### Task 11: MmlParser — @W Command

**Files:**
- Modify: `e6502.Avalonia/Hardware/MmlParser.cs`
- Test: `e6502UnitTests/MmlParserWtsTests.cs`

Add `@W<n>` command to select WTS instrument index for a voice.

**Step 1: Write the failing tests**

```csharp
// e6502UnitTests/MmlParserWtsTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class MmlParserWtsTests
{
    [TestMethod]
    public void AtW_ParsesWtsInstrumentIndex()
    {
        var events = MmlParser.Parse("@W5 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.IsNotNull(setWts);
        Assert.AreEqual(5, setWts.Param1);
    }

    [TestMethod]
    public void AtW_DefaultsToZero()
    {
        var events = MmlParser.Parse("@W C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.IsNotNull(setWts);
        Assert.AreEqual(0, setWts.Param1);
    }

    [TestMethod]
    public void AtW_DoesNotInterfereWithExistingAt()
    {
        // Existing @0 should still work (SID instrument)
        var events = MmlParser.Parse("@0 C4");
        Assert.IsFalse(events.Any(e => e.Type == MmlEventType.SetWtsInstrument));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~MmlParserWtsTests" --verbosity normal`
Expected: FAIL

**Step 3: Modify MmlParser**

1. Add `SetWtsInstrument` to `MmlEventType` enum
2. In the `@` command handler in `MmlParser.Parse()`, add a case for `'W'`:

```csharp
case 'W':
    pos++;
    int wtsInst = ReadInt(mml, ref pos, 0);
    events.Add(new MmlEvent(MmlEventType.SetWtsInstrument, wtsInst, 0));
    continue;
```

3. In `MusicEngine`, handle the new event type during MML sequencer tick:

```csharp
case MmlEventType.SetWtsInstrument:
    voice.WtsInstrumentId = ev.Param1;
    break;
```

**Step 4: Run all MML tests**

Run: `dotnet test --filter "FullyQualifiedName~MmlParser" --verbosity normal`
Expected: PASS (both old and new)

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/MmlParser.cs e6502.Avalonia/Hardware/MusicEngine.cs e6502UnitTests/MmlParserWtsTests.cs
git commit -m "feat(wts): add @W MML command for WTS instrument selection"
```

---

### Task 12: TCP Server — WTS Commands

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`
- Test: Integration test via TCP or direct method call

Add TCP commands: `wts_load_soundfont`, `wts_list_instruments`, `wts_note_on`, `wts_note_off`, `wts_set_instrument`.

**Step 1: Write the failing test**

Since TCP tests may require a running server, test via the command handler methods directly. Add tests alongside existing TCP test patterns in the project.

```csharp
// Add to an appropriate test file or create e6502UnitTests/WtsTcpTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class WtsTcpTests
{
    // These test the command dispatch logic, not actual TCP transport
    [TestMethod]
    public void WtsListInstruments_EmptyBank_ReturnsEmptyArray()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        // Verify WTS has no instruments
        Assert.AreEqual(0, bus.Wts.InstrumentCount);
    }

    [TestMethod]
    public void WtsListInstruments_WithBank_ReturnsList()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument { Name = "Piano" });
        bank.Instruments.Add(new SampleInstrument { Name = "Strings" });
        bus.Wts.LoadBank(bank);

        Assert.AreEqual(2, bus.Wts.InstrumentCount);
        Assert.AreEqual("Piano", bus.Wts.GetInstrumentName(0));
        Assert.AreEqual("Strings", bus.Wts.GetInstrumentName(1));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~WtsTcpTests" --verbosity normal`
Expected: FAIL

**Step 3: Add TCP commands**

In `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`, add to the command switch:

```csharp
"wts_load_soundfont" => CmdWtsLoadSoundfont(req),
"wts_list_instruments" => CmdWtsListInstruments(),
"wts_note_on" => CmdWtsNoteOn(req),
"wts_note_off" => CmdWtsNoteOff(req),
"wts_set_instrument" => CmdWtsSetInstrument(req),
"wts_set_volume" => CmdWtsSetVolume(req),
"wts_set_panning" => CmdWtsSetPanning(req),
"wts_set_reverb" => CmdWtsSetReverb(req),
"wts_set_chorus" => CmdWtsSetChorus(req),
```

Each follows the existing pattern: extract JSON params, call `_bus.Wts.Method()` or write FileIO registers, return `Success(...)`.

Also add helper properties to `WavetableSynth`:
- `public int InstrumentCount` — returns bank instrument count
- `public string? GetInstrumentName(int index)` — returns name or null

**Step 4: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~WtsTcpTests" --verbosity normal`
Expected: PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Ipc/EmulatorTcpServer.cs e6502.Avalonia/Hardware/WavetableSynth.cs e6502UnitTests/WtsTcpTests.cs
git commit -m "feat(wts): add WTS TCP commands for soundfont/voice control"
```

---

### Task 13: MCP Tools — WTS Exposure

**Files:**
- Modify: `e6502.MCP/EmulatorTools.cs`

Add MCP tool methods that map to the TCP commands from Task 12.

**Step 1: No separate test needed** — MCP tools are thin wrappers over TCP commands.

**Step 2: Add MCP tools**

Follow the existing pattern in `e6502.MCP/EmulatorTools.cs`:

```csharp
[McpServerTool, Description("Load an SF2 soundfont file for wavetable playback")]
public static async Task<string> WtsLoadSoundfont(
    EmulatorClient client,
    [Description("SF2 filename in ~/e6502-programs/soundfonts/")] string filename)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "wts_load_soundfont",
        ["filename"] = filename
    });
    return result.ToJsonString();
}

[McpServerTool, Description("List instruments in the loaded soundfont")]
public static async Task<string> WtsListInstruments(EmulatorClient client)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "wts_list_instruments"
    });
    return result.ToJsonString();
}

[McpServerTool, Description("Play a note on a wavetable voice")]
public static async Task<string> WtsNoteOn(
    EmulatorClient client,
    [Description("Voice index (0-7)")] int voice,
    [Description("MIDI note number (0-127)")] int note,
    [Description("Velocity (0-127)")] int velocity = 100,
    [Description("Instrument index")] int instrument = 0)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "wts_note_on",
        ["voice"] = voice,
        ["note"] = note,
        ["velocity"] = velocity,
        ["instrument"] = instrument
    });
    return result.ToJsonString();
}

[McpServerTool, Description("Stop a note on a wavetable voice")]
public static async Task<string> WtsNoteOff(
    EmulatorClient client,
    [Description("Voice index (0-7)")] int voice)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "wts_note_off",
        ["voice"] = voice
    });
    return result.ToJsonString();
}

[McpServerTool, Description("Set reverb level for wavetable synth")]
public static async Task<string> WtsSetReverb(
    EmulatorClient client,
    [Description("Reverb level (0-255)")] int level)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "wts_set_reverb",
        ["level"] = level
    });
    return result.ToJsonString();
}

[McpServerTool, Description("Set chorus level for wavetable synth")]
public static async Task<string> WtsSetChorus(
    EmulatorClient client,
    [Description("Chorus level (0-255)")] int level)
{
    var result = await client.SendAsync(new JsonObject
    {
        ["command"] = "wts_set_chorus",
        ["level"] = level
    });
    return result.ToJsonString();
}
```

**Step 3: Build to verify compilation**

Run: `dotnet build`
Expected: Build succeeded

**Step 4: Commit**

```bash
git add e6502.MCP/EmulatorTools.cs
git commit -m "feat(wts): add MCP tool methods for wavetable synth control"
```

---

### Task 14: Integration Test — End-to-End WTS Playback

**Files:**
- Test: `e6502UnitTests/WtsIntegrationTests.cs`

Full round-trip: load bank → set instrument → note on → render → verify audio output.

**Step 1: Write the integration test**

```csharp
// e6502UnitTests/WtsIntegrationTests.cs
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class WtsIntegrationTests
{
    private static SampleBank MakeTestBank()
    {
        int len = 1024;
        var data = new float[len];
        for (int i = 0; i < len; i++)
            data[i] = MathF.Sin(2f * MathF.PI * i / len);

        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Sine",
            MidiProgram = 0,
            Regions = { new SampleRegion
            {
                SampleData = data, SampleRate = 44100, RootKey = 60,
                LoopEnabled = true, LoopStart = 0, LoopEnd = len - 1,
                AttackTime = 0.001f, SustainLevel = 1f, ReleaseTime = 0.01f
            }}
        });
        bank.Instruments.Add(new SampleInstrument
        {
            Name = "Square",
            MidiProgram = 80,
            Regions = { new SampleRegion
            {
                SampleData = Enumerable.Range(0, len)
                    .Select(i => i < len / 2 ? 1f : -1f).ToArray(),
                SampleRate = 44100, RootKey = 60,
                LoopEnabled = true, LoopStart = 0, LoopEnd = len - 1,
                AttackTime = 0f, SustainLevel = 1f, ReleaseTime = 0f
            }}
        });
        return bank;
    }

    [TestMethod]
    public void FullRoundTrip_ViaRegisters()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Wts.LoadBank(MakeTestBank());

        // Set instrument 0, velocity 127, then trigger note 60 on voice 0
        int v0 = VgcConstants.WtsVoiceBase;
        bus.Write((ushort)(v0 + VgcConstants.WtsVoiceInstrument), 0);
        bus.Write((ushort)(v0 + VgcConstants.WtsVoiceVelocity), 127);
        bus.Write((ushort)(v0 + VgcConstants.WtsVoiceNote), 60);

        // Render and verify non-silent
        short[] audio = bus.Wts.RenderSamples(1024);
        Assert.IsTrue(audio.Any(s => Math.Abs(s) > 100));

        // Verify status register shows active
        byte status = bus.Read((ushort)(v0 + VgcConstants.WtsVoiceStatus));
        Assert.IsTrue((status & 0x01) != 0);
    }

    [TestMethod]
    public void MusicEngine_MixedSidAndWts()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Wts.LoadBank(MakeTestBank());

        // SID voice
        bus.Music.DirectNoteOn(0, 48, 100, 0);
        // WTS voice
        bus.Music.DirectNoteOn(6, 72, 100, 0);

        // Both should show active notes
        Assert.AreEqual(48, bus.Music.GetVoiceMidi(0));
        Assert.AreEqual(72, bus.Music.GetVoiceMidi(6));

        // SID should have gate set
        byte sidCtrl = bus.Sid.Read(0xD404);
        Assert.IsTrue((sidCtrl & 0x01) != 0);

        // WTS should have voice active
        Assert.IsTrue((bus.Wts.ActiveVoiceMask & 0x01) != 0);
    }

    [TestMethod]
    public void EnumerateInstruments_ViaRegisters()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Wts.LoadBank(MakeTestBank());

        Assert.AreEqual(2, bus.Read((ushort)VgcConstants.WtsInstrumentCount));

        // Select instrument 1
        bus.Write((ushort)VgcConstants.WtsEnumIndex, 1);
        Assert.AreEqual(80, bus.Read((ushort)VgcConstants.WtsEnumProgram));

        // Read name "Square"
        byte s = bus.Read((ushort)VgcConstants.WtsEnumName);
        Assert.AreEqual((byte)'S', s);
    }

    [TestMethod]
    public void AllNotesOff_Command()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Wts.LoadBank(MakeTestBank());

        bus.Wts.NoteOn(0, 60, 127, 0);
        bus.Wts.NoteOn(3, 72, 100, 0);
        Assert.IsTrue(bus.Wts.ActiveVoiceMask != 0);

        bus.Write((ushort)VgcConstants.WtsCommand, VgcConstants.WtsCmdAllNotesOff);
        bus.Wts.RenderSamples(4096); // drain releases
        Assert.AreEqual(0, bus.Wts.ActiveVoiceMask);
    }
}
```

**Step 2: Run tests**

Run: `dotnet test --filter "FullyQualifiedName~WtsIntegrationTests" --verbosity normal`
Expected: PASS (all 4 tests)

**Step 3: Commit**

```bash
git add e6502UnitTests/WtsIntegrationTests.cs
git commit -m "test(wts): add end-to-end integration tests for wavetable synth"
```

---

### Task 15: Final Verification

**Step 1: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: ALL PASS, zero failures

**Step 2: Build release**

Run: `dotnet build -c Release`
Expected: Build succeeded with no warnings in WTS files

**Step 3: Update CLAUDE.md memory map**

Update the memory map section in `/Users/barry/Git/e6502/CLAUDE.md` to include:
```
$A140-$A1DF  Wavetable Synthesizer (WTS) registers
```

And add a brief WTS section to the Architecture docs.

**Step 4: Final commit**

```bash
git add CLAUDE.md
git commit -m "docs: update memory map and architecture notes for WTS"
```
