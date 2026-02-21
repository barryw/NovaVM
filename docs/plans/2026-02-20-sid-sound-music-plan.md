# SID Sound & Music Commands — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace dead VSC sound commands with INSTRUMENT/SOUND/VOLUME/MUSIC commands driving the real SID chip, with an MML music string parser and automatic voice allocation.

**Architecture:** BASIC commands write parameters to FIO registers and trigger FIO commands. A C#-side `MusicEngine` manages instruments, parses MML, runs a 60Hz sequencer, and handles SFX voice allocation. The SidChip registers are written directly by the engine each frame.

**Tech Stack:** C#/.NET 10, MSTest, 6502 assembly (vasm), ehbasic

**Design doc:** `docs/plans/2026-02-20-sid-sound-music-design.md`

---

## Task 1: MML Token Types and Instrument Model

**Files:**
- Create: `e6502.Avalonia/Hardware/MmlTypes.cs`
- Test: `e6502UnitTests/MmlParserTests.cs`

**Step 1: Create MmlTypes.cs with event types and instrument struct**

```csharp
// e6502.Avalonia/Hardware/MmlTypes.cs
namespace e6502.Avalonia.Hardware;

public enum MmlEventType
{
    NoteOn,         // Param1=MIDI note, Param2=duration in ticks
    Rest,           // Param2=duration in ticks
    SetTempo,       // Param1=BPM
    SetInstrument,  // Param1=instrument id
    SetOctave,      // Param1=octave (1-7)
    OctaveUp,
    OctaveDown,
    SetDefaultLen,  // Param1=length denominator (4=quarter)
    ArpStart,       // Param1=count, ArpNotes has MIDI notes, Param2=duration
    SetPulseWidth,  // Param1=pulse width (0-4095)
    PwmSweep,       // Param1=direction (-1, 0, +1)
    SetFilterCutoff,// Param1=cutoff (0-2047), Param2=resonance (0-15)
    FilterMode,     // Param1=mode (0=off, 1=LP, 2=BP, 4=HP, combos ok)
    FilterSweep,    // Param1=direction (-1, 0, +1)
    SetVibrato,     // Param1=depth (0=off)
    Portamento,     // flags next note as portamento target
    LoopStart,
    LoopEnd,        // Param1=repeat count
}

public readonly record struct MmlEvent(MmlEventType Type, int Param1 = 0, int Param2 = 0, int[]? ArpNotes = null);

public sealed class SidInstrument
{
    public byte Waveform;  // 0x10=tri, 0x20=saw, 0x40=pulse, 0x80=noise
    public byte Attack;    // 0-15
    public byte Decay;     // 0-15
    public byte Sustain;   // 0-15
    public byte Release;   // 0-15
    public ushort PulseWidth = 2048;  // default center

    public byte Ad => (byte)((Attack << 4) | Decay);
    public byte Sr => (byte)((Sustain << 4) | Release);
}
```

**Step 2: Write initial test scaffold**

Create `e6502UnitTests/MmlParserTests.cs`:

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MmlParserTests
{
    [TestMethod]
    public void Parse_SingleNote_ProducesNoteOn()
    {
        var events = MmlParser.Parse("C");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(48, events[0].Param1); // C in default octave 4 = MIDI 48+12=60...
        // Default octave 4, C4 = MIDI 60
        Assert.AreEqual(60, events[0].Param1);
    }
}
```

**Step 3: Run test — should fail (MmlParser doesn't exist yet)**

```bash
dotnet test --filter "Parse_SingleNote_ProducesNoteOn" --verbosity normal
```
Expected: FAIL — `MmlParser` type does not exist.

**Step 4: Commit types file**

```bash
git add e6502.Avalonia/Hardware/MmlTypes.cs e6502UnitTests/MmlParserTests.cs
git commit -m "feat: add MML event types and instrument model"
```

---

## Task 2: MML Parser — Core Note Parsing

**Files:**
- Create: `e6502.Avalonia/Hardware/MmlParser.cs`
- Modify: `e6502UnitTests/MmlParserTests.cs`

The MML parser converts a string like `"T140 O4L8 C D# E R4 G"` into a `List<MmlEvent>`.

**Duration system:** 96 ticks per quarter note (MIDI standard).
- Whole=384, Half=192, Quarter=96, Eighth=48, Sixteenth=24.
- Dotted = duration * 1.5.
- Default length set by `L` command (default: L4 = quarter).

**Note mapping:** Octave 4 C = MIDI 60. Each octave is 12 semitones.
- C=0, C#/D-=1, D=2, D#/E-=3, E=4, F=5, F#/G-=6, G=7, G#/A-=8, A=9, A#/B-=10, B=11
- MIDI note = (octave + 1) * 12 + semitone

**Step 1: Write core parser tests**

Add to `MmlParserTests.cs`:

```csharp
[TestMethod]
public void Parse_SingleNote_DefaultQuarter()
{
    var events = MmlParser.Parse("C");
    Assert.AreEqual(1, events.Count);
    Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
    Assert.AreEqual(60, events[0].Param1);  // C4 = MIDI 60
    Assert.AreEqual(96, events[0].Param2);  // quarter note = 96 ticks
}

[TestMethod]
public void Parse_NoteWithSharp()
{
    var events = MmlParser.Parse("C#");
    Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
    Assert.AreEqual(61, events[0].Param1);  // C#4
}

[TestMethod]
public void Parse_NoteWithFlat()
{
    var events = MmlParser.Parse("B-");
    Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
    Assert.AreEqual(70, events[0].Param1);  // Bb4 = MIDI 70
}

[TestMethod]
public void Parse_NoteWithExplicitLength()
{
    var events = MmlParser.Parse("C8");
    Assert.AreEqual(48, events[0].Param2);  // eighth = 48 ticks
}

[TestMethod]
public void Parse_DottedNote()
{
    var events = MmlParser.Parse("C4.");
    Assert.AreEqual(144, events[0].Param2);  // dotted quarter = 96*1.5
}

[TestMethod]
public void Parse_Rest()
{
    var events = MmlParser.Parse("R4");
    Assert.AreEqual(MmlEventType.Rest, events[0].Type);
    Assert.AreEqual(96, events[0].Param2);
}

[TestMethod]
public void Parse_MultipleNotes()
{
    var events = MmlParser.Parse("C D E");
    Assert.AreEqual(3, events.Count);
    Assert.AreEqual(60, events[0].Param1);  // C4
    Assert.AreEqual(62, events[1].Param1);  // D4
    Assert.AreEqual(64, events[2].Param1);  // E4
}

[TestMethod]
public void Parse_OctaveCommand()
{
    var events = MmlParser.Parse("O5 C");
    Assert.AreEqual(72, events[0].Param1);  // C5 = MIDI 72
}

[TestMethod]
public void Parse_OctaveUpDown()
{
    var events = MmlParser.Parse("C > C < C");
    Assert.AreEqual(60, events[0].Param1);  // C4
    Assert.AreEqual(72, events[1].Param1);  // C5
    Assert.AreEqual(60, events[2].Param1);  // C4
}

[TestMethod]
public void Parse_DefaultLength()
{
    var events = MmlParser.Parse("L8 C D");
    Assert.AreEqual(48, events[0].Param2);  // eighth
    Assert.AreEqual(48, events[1].Param2);
}

[TestMethod]
public void Parse_Tempo()
{
    var events = MmlParser.Parse("T120 C");
    Assert.AreEqual(2, events.Count);
    Assert.AreEqual(MmlEventType.SetTempo, events[0].Type);
    Assert.AreEqual(120, events[0].Param1);
}

[TestMethod]
public void Parse_InstrumentSwitch()
{
    var events = MmlParser.Parse("I3 C");
    Assert.AreEqual(MmlEventType.SetInstrument, events[0].Type);
    Assert.AreEqual(3, events[0].Param1);
}

[TestMethod]
public void Parse_BarLine_Ignored()
{
    var events = MmlParser.Parse("C D | E F");
    Assert.AreEqual(4, events.Count);  // bar line is whitespace
}

[TestMethod]
public void Parse_Tie()
{
    // C4 tied to C8 = one note with duration 96+48=144
    var events = MmlParser.Parse("C4&C8");
    Assert.AreEqual(1, events.Count);
    Assert.AreEqual(144, events[0].Param2);
}
```

**Step 2: Implement MmlParser.Parse()**

Create `e6502.Avalonia/Hardware/MmlParser.cs`:

```csharp
namespace e6502.Avalonia.Hardware;

public static class MmlParser
{
    private const int TicksPerQuarter = 96;

    private static readonly int[] NoteOffsets = { 9, 11, 0, 2, 4, 5, 7 }; // A B C D E F G

    public static List<MmlEvent> Parse(string mml)
    {
        var events = new List<MmlEvent>();
        int pos = 0;
        int octave = 4;
        int defaultLen = 4; // quarter note

        while (pos < mml.Length)
        {
            char c = char.ToUpper(mml[pos]);

            if (c == ' ' || c == '|') { pos++; continue; }

            if (c >= 'A' && c <= 'G')
            {
                int semitone = NoteOffsets[c - 'A'];
                pos++;
                // sharp/flat
                if (pos < mml.Length && (mml[pos] == '#' || mml[pos] == '+'))
                    { semitone++; pos++; }
                else if (pos < mml.Length && mml[pos] == '-')
                    { semitone--; pos++; }

                int midi = (octave + 1) * 12 + semitone;
                int duration = ReadDuration(mml, ref pos, defaultLen);

                // tie
                while (pos < mml.Length && mml[pos] == '&')
                {
                    pos++; // skip &
                    // expect another note — skip note name and accidental
                    if (pos < mml.Length && char.ToUpper(mml[pos]) >= 'A' && char.ToUpper(mml[pos]) <= 'G')
                    {
                        pos++; // skip note name
                        if (pos < mml.Length && (mml[pos] == '#' || mml[pos] == '+' || mml[pos] == '-'))
                            pos++;
                        duration += ReadDuration(mml, ref pos, defaultLen);
                    }
                }

                events.Add(new MmlEvent(MmlEventType.NoteOn, midi, duration));
            }
            else if (c == 'R')
            {
                pos++;
                int duration = ReadDuration(mml, ref pos, defaultLen);
                events.Add(new MmlEvent(MmlEventType.Rest, 0, duration));
            }
            else if (c == 'O')
            {
                pos++;
                octave = ReadInt(mml, ref pos, octave);
            }
            else if (c == '>')
            {
                pos++;
                octave = Math.Min(7, octave + 1);
            }
            else if (c == '<')
            {
                pos++;
                octave = Math.Max(1, octave - 1);
            }
            else if (c == 'L')
            {
                pos++;
                defaultLen = ReadInt(mml, ref pos, defaultLen);
            }
            else if (c == 'T')
            {
                pos++;
                int bpm = ReadInt(mml, ref pos, 120);
                events.Add(new MmlEvent(MmlEventType.SetTempo, bpm));
            }
            else if (c == 'I')
            {
                pos++;
                int id = ReadInt(mml, ref pos, 0);
                events.Add(new MmlEvent(MmlEventType.SetInstrument, id));
            }
            else if (c == '~')
            {
                pos++;
                int depth = ReadInt(mml, ref pos, 4);
                events.Add(new MmlEvent(MmlEventType.SetVibrato, depth));
            }
            else if (c == '/')
            {
                pos++;
                events.Add(new MmlEvent(MmlEventType.Portamento));
            }
            else if (c == '{')
            {
                pos++;
                var arpNotes = new List<int>();
                int arpOctave = octave;
                while (pos < mml.Length && mml[pos] != '}')
                {
                    char ac = char.ToUpper(mml[pos]);
                    if (ac >= 'A' && ac <= 'G')
                    {
                        int s = NoteOffsets[ac - 'A'];
                        pos++;
                        if (pos < mml.Length && (mml[pos] == '#' || mml[pos] == '+'))
                            { s++; pos++; }
                        else if (pos < mml.Length && mml[pos] == '-')
                            { s--; pos++; }
                        arpNotes.Add((arpOctave + 1) * 12 + s);
                    }
                    else if (ac == '>') { arpOctave++; pos++; }
                    else if (ac == '<') { arpOctave--; pos++; }
                    else { pos++; } // skip whitespace etc.
                }
                if (pos < mml.Length) pos++; // skip }
                int arpDuration = ReadDuration(mml, ref pos, defaultLen);
                events.Add(new MmlEvent(MmlEventType.ArpStart, arpNotes.Count, arpDuration, arpNotes.ToArray()));
            }
            else if (c == '@')
            {
                pos++;
                if (pos < mml.Length)
                {
                    char sub = char.ToUpper(mml[pos]);
                    if (sub == 'P')
                    {
                        pos++;
                        if (pos < mml.Length && char.ToUpper(mml[pos]) == 'S')
                        {
                            // PWM sweep: @PS+ @PS- @PS0
                            pos++;
                            int dir = 0;
                            if (pos < mml.Length)
                            {
                                if (mml[pos] == '+') { dir = 1; pos++; }
                                else if (mml[pos] == '-') { dir = -1; pos++; }
                                else if (mml[pos] == '0') { dir = 0; pos++; }
                            }
                            events.Add(new MmlEvent(MmlEventType.PwmSweep, dir));
                        }
                        else
                        {
                            // Pulse width: @P2048
                            int pw = ReadInt(mml, ref pos, 2048);
                            events.Add(new MmlEvent(MmlEventType.SetPulseWidth, pw));
                        }
                    }
                    else if (sub == 'F')
                    {
                        pos++;
                        if (pos < mml.Length)
                        {
                            char fc = char.ToUpper(mml[pos]);
                            if (fc == 'L') { pos++; events.Add(new MmlEvent(MmlEventType.FilterMode, 1)); }
                            else if (fc == 'B') { pos++; events.Add(new MmlEvent(MmlEventType.FilterMode, 2)); }
                            else if (fc == 'H') { pos++; events.Add(new MmlEvent(MmlEventType.FilterMode, 4)); }
                            else if (fc == 'O') { pos++; events.Add(new MmlEvent(MmlEventType.FilterMode, 0)); }
                            else if (fc == 'S')
                            {
                                pos++;
                                int dir = 0;
                                if (pos < mml.Length)
                                {
                                    if (mml[pos] == '+') { dir = 1; pos++; }
                                    else if (mml[pos] == '-') { dir = -1; pos++; }
                                    else if (mml[pos] == '0') { dir = 0; pos++; }
                                }
                                events.Add(new MmlEvent(MmlEventType.FilterSweep, dir));
                            }
                            else
                            {
                                // @F1024,8 — cutoff,resonance
                                int cutoff = ReadInt(mml, ref pos, 1024);
                                int res = 0;
                                if (pos < mml.Length && mml[pos] == ',')
                                {
                                    pos++;
                                    res = ReadInt(mml, ref pos, 0);
                                }
                                events.Add(new MmlEvent(MmlEventType.SetFilterCutoff, cutoff, res));
                            }
                        }
                    }
                    else if (sub == 'W')
                    {
                        // @W1 — raw waveform (0=tri,1=saw,2=pulse,3=noise)
                        pos++;
                        int w = ReadInt(mml, ref pos, 2);
                        // Convert to SID waveform byte
                        byte sidWave = w switch
                        {
                            0 => 0x10, 1 => 0x20, 2 => 0x40, 3 => 0x80,
                            _ => 0x40
                        };
                        // Store as SetPulseWidth is wrong — we need a waveform event
                        // For now, use Param1 to carry raw SID byte
                        // Actually, instrument switch handles this better
                        pos = pos; // skip for now — waveform via instrument
                    }
                }
            }
            else if (c == '[')
            {
                pos++;
                events.Add(new MmlEvent(MmlEventType.LoopStart));
            }
            else if (c == ']')
            {
                pos++;
                int count = ReadInt(mml, ref pos, 2);
                events.Add(new MmlEvent(MmlEventType.LoopEnd, count));
            }
            else
            {
                pos++; // skip unknown
            }
        }

        return events;
    }

    private static int ReadDuration(string mml, ref int pos, int defaultLen)
    {
        int len = ReadInt(mml, ref pos, 0);
        if (len == 0) len = defaultLen;

        int ticks = len > 0 ? TicksPerQuarter * 4 / len : TicksPerQuarter;

        // dotted
        if (pos < mml.Length && mml[pos] == '.')
        {
            ticks = ticks * 3 / 2;
            pos++;
        }

        return ticks;
    }

    private static int ReadInt(string mml, ref int pos, int defaultVal)
    {
        if (pos >= mml.Length || !char.IsDigit(mml[pos]))
            return defaultVal;

        int val = 0;
        while (pos < mml.Length && char.IsDigit(mml[pos]))
        {
            val = val * 10 + (mml[pos] - '0');
            pos++;
        }
        return val;
    }
}
```

**Step 3: Run tests**

```bash
dotnet test --filter "MmlParserTests" --verbosity normal
```
Expected: ALL PASS

**Step 4: Commit**

```bash
git add e6502.Avalonia/Hardware/MmlParser.cs e6502.Avalonia/Hardware/MmlTypes.cs e6502UnitTests/MmlParserTests.cs
git commit -m "feat: MML parser with note, octave, tempo, arpeggio, effects parsing"
```

---

## Task 3: MusicEngine — Instrument Table and SFX Playback

**Files:**
- Create: `e6502.Avalonia/Hardware/MusicEngine.cs`
- Create: `e6502UnitTests/MusicEngineTests.cs`

The MusicEngine manages instruments, SFX voice allocation, and music sequencing. It writes SID registers through CompositeBusDevice each frame.

**Step 1: Write MusicEngine tests for instruments and SFX**

```csharp
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MusicEngineTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    [TestMethod]
    public void DefineInstrument_StoresInTable()
    {
        using var bus = MakeBus();
        var engine = new MusicEngine(bus);
        engine.DefineInstrument(1, 0x40, 0, 9, 0, 0); // pulse, A=0, D=9, S=0, R=0
        var inst = engine.GetInstrument(1);
        Assert.AreEqual((byte)0x40, inst.Waveform);
        Assert.AreEqual((byte)9, inst.Decay);
    }

    [TestMethod]
    public void PlaySound_WritesNoteToSid()
    {
        using var bus = MakeBus();
        var engine = new MusicEngine(bus);
        engine.DefineInstrument(0, 0x40, 0, 9, 0, 6); // default beep
        engine.PlaySound(60, 10); // C4, 10 frames

        // After PlaySound, SID voice should have gate on + waveform
        // Voice 3 (index 2) used first when no music playing
        // Voice 3 control register = $D412
        byte ctrl = bus.Sid.Read(0xD412);
        Assert.AreEqual(0x41, ctrl, "Gate on + pulse waveform");
    }

    [TestMethod]
    public void PlaySound_FrequencyCorrect()
    {
        using var bus = MakeBus();
        var engine = new MusicEngine(bus);
        engine.DefineInstrument(0, 0x40, 0, 9, 0, 6);

        // MIDI 69 = A4 = 440 Hz
        // SID frequency for 440Hz (PAL): 440 * 16777216 / 985248 ≈ 7382
        engine.PlaySound(69, 10);
        int freqLo = bus.Sid.Read(0xD40E); // voice 3 freq low
        int freqHi = bus.Sid.Read(0xD40F); // voice 3 freq high
        int freq = freqLo | (freqHi << 8);
        Assert.IsTrue(Math.Abs(freq - 7382) < 2, $"Expected ~7382, got {freq}");
    }

    [TestMethod]
    public void PlaySound_GatesOffAfterDuration()
    {
        using var bus = MakeBus();
        var engine = new MusicEngine(bus);
        engine.DefineInstrument(0, 0x40, 0, 9, 0, 6);
        engine.PlaySound(60, 3); // 3 frames

        engine.Tick(); // frame 1
        engine.Tick(); // frame 2
        engine.Tick(); // frame 3 — should gate off

        byte ctrl = bus.Sid.Read(0xD412);
        Assert.AreEqual(0x40, ctrl, "Gate off after duration (waveform stays, gate bit 0 cleared)");
    }

    [TestMethod]
    public void IsPlaying_TrueWhileSfxActive()
    {
        using var bus = MakeBus();
        var engine = new MusicEngine(bus);
        engine.DefineInstrument(0, 0x40, 0, 9, 0, 6);
        engine.PlaySound(60, 5);

        Assert.IsTrue(engine.IsPlaying);
        for (int i = 0; i < 5; i++) engine.Tick();
        Assert.IsFalse(engine.IsPlaying);
    }

    [TestMethod]
    public void SetVolume_WritesMasterVolume()
    {
        using var bus = MakeBus();
        var engine = new MusicEngine(bus);
        engine.SetVolume(12);

        Assert.AreEqual(12, bus.Sid.Read(0xD418) & 0x0F);
    }
}
```

**Step 2: Implement MusicEngine (SFX portion)**

```csharp
// e6502.Avalonia/Hardware/MusicEngine.cs
namespace e6502.Avalonia.Hardware;

public sealed class MusicEngine
{
    private const double CpuClockRate = 985248.0;
    private const int InstrumentCount = 16;
    private const int VoiceCount = 3;

    private readonly CompositeBusDevice _bus;
    private readonly SidInstrument[] _instruments = new SidInstrument[InstrumentCount];
    private readonly int[] _sfxVoice = { -1 }; // voice index currently used by SFX (-1 = none)
    private int _sfxFramesLeft;
    private int _sfxVoiceIndex = -1;
    private bool _sfxCompleted;

    // Music sequencer state (per voice)
    private readonly List<MmlEvent>?[] _sequences = new List<MmlEvent>?[VoiceCount];
    private readonly int[] _seqPos = new int[VoiceCount];
    private readonly double[] _seqTickAccum = new double[VoiceCount];
    private readonly int[] _seqWaitTicks = new int[VoiceCount];
    private readonly int[] _voiceOctave = new int[VoiceCount];
    private readonly int[] _voiceInstrument = new int[VoiceCount];
    private readonly int[] _voiceDefaultLen = new int[VoiceCount];
    // Arpeggio state
    private readonly int[]?[] _voiceArpNotes = new int[]?[VoiceCount];
    private readonly int[] _voiceArpIndex = new int[VoiceCount];
    private readonly int[] _voiceArpTicks = new int[VoiceCount];
    // Effect state
    private readonly int[] _voiceVibrato = new int[VoiceCount];
    private readonly int[] _voiceVibratoPhase = new int[VoiceCount];
    private readonly bool[] _voicePortamento = new bool[VoiceCount];
    private readonly int[] _voiceTargetFreq = new int[VoiceCount];
    private readonly int[] _voiceCurrentFreq = new int[VoiceCount];
    private readonly int[] _voicePwmDir = new int[VoiceCount];
    private readonly int[] _voicePulseWidth = new int[VoiceCount];
    private readonly int[] _filterSweepDir = { 0 };
    private int _filterCutoff;
    private int _filterResonance;
    private int _filterMode;

    // Loop stack per voice
    private readonly Stack<(int pos, int count)>[] _loopStack = new Stack<(int, int)>[VoiceCount];

    private int _bpm = 120;
    private bool _musicPlaying;
    private bool _musicLoop;
    private readonly int[] _stealPriority = { 2, 1, 0 }; // voice 3 first, then 2, then 1

    public bool IsPlaying => _sfxVoiceIndex >= 0 && _sfxFramesLeft > 0;
    public bool IsMusicPlaying => _musicPlaying;
    public bool SfxCompleted { get { var v = _sfxCompleted; _sfxCompleted = false; return v; } }

    public MusicEngine(CompositeBusDevice bus)
    {
        _bus = bus;
        for (int i = 0; i < InstrumentCount; i++)
            _instruments[i] = new SidInstrument { Waveform = 0x40, Attack = 0, Decay = 9, Sustain = 0, Release = 6 };
        for (int i = 0; i < VoiceCount; i++)
            _loopStack[i] = new Stack<(int, int)>();
    }

    public void DefineInstrument(int id, byte waveform, byte a, byte d, byte s, byte r)
    {
        if (id < 0 || id >= InstrumentCount) return;
        _instruments[id] = new SidInstrument
        {
            Waveform = waveform, Attack = a, Decay = d, Sustain = s, Release = r
        };
    }

    public SidInstrument GetInstrument(int id) =>
        id >= 0 && id < InstrumentCount ? _instruments[id] : _instruments[0];

    public void SetVolume(int level)
    {
        byte vol = (byte)(level & 0x0F);
        byte current = _bus.Sid.Read(0xD418);
        _bus.Sid.Write(0xD418, (byte)((current & 0xF0) | vol));
    }

    public void PlaySound(int midiNote, int durationFrames, int instrumentId = 0)
    {
        int voiceIdx = AllocateSfxVoice();
        if (voiceIdx < 0) return;

        _sfxVoiceIndex = voiceIdx;
        _sfxFramesLeft = durationFrames;

        var inst = GetInstrument(instrumentId);
        int freq = MidiToSidFreq(midiNote);
        int baseReg = voiceIdx * 7;

        _bus.Sid.Write((ushort)(0xD400 + baseReg + 0), (byte)(freq & 0xFF));        // freq lo
        _bus.Sid.Write((ushort)(0xD400 + baseReg + 1), (byte)((freq >> 8) & 0xFF)); // freq hi
        _bus.Sid.Write((ushort)(0xD400 + baseReg + 2), (byte)(inst.PulseWidth & 0xFF));
        _bus.Sid.Write((ushort)(0xD400 + baseReg + 3), (byte)((inst.PulseWidth >> 8) & 0x0F));
        _bus.Sid.Write((ushort)(0xD400 + baseReg + 5), inst.Ad);
        _bus.Sid.Write((ushort)(0xD400 + baseReg + 6), inst.Sr);
        _bus.Sid.Write((ushort)(0xD400 + baseReg + 4), (byte)(inst.Waveform | 0x01)); // gate on
    }

    public void Tick()
    {
        // SFX countdown
        if (_sfxVoiceIndex >= 0 && _sfxFramesLeft > 0)
        {
            _sfxFramesLeft--;
            if (_sfxFramesLeft <= 0)
            {
                // Gate off
                int baseReg = _sfxVoiceIndex * 7;
                byte ctrl = _bus.Sid.Read((ushort)(0xD400 + baseReg + 4));
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 4), (byte)(ctrl & 0xFE)); // clear gate
                _sfxCompleted = true;
                _sfxVoiceIndex = -1;
            }
        }

        // Music sequencer
        if (_musicPlaying)
            TickMusic();
    }

    // --- Music ---

    public void SetSequence(int voice, string mml)
    {
        if (voice < 1 || voice > 3) return;
        _sequences[voice - 1] = MmlParser.Parse(mml);
    }

    public void MusicPlay()
    {
        _musicPlaying = true;
        double ticksPerFrame = 96.0 * _bpm / 3600.0;
        for (int i = 0; i < VoiceCount; i++)
        {
            _seqPos[i] = 0;
            _seqTickAccum[i] = 0;
            _seqWaitTicks[i] = 0;
            _voiceOctave[i] = 4;
            _voiceInstrument[i] = 0;
            _voiceDefaultLen[i] = 4;
            _voiceArpNotes[i] = null;
            _voiceVibrato[i] = 0;
            _voicePortamento[i] = false;
            _voicePwmDir[i] = 0;
            _loopStack[i].Clear();
        }
    }

    public void MusicStop()
    {
        _musicPlaying = false;
        for (int i = 0; i < VoiceCount; i++)
        {
            if (i == _sfxVoiceIndex) continue; // don't touch SFX voice
            int baseReg = i * 7;
            byte ctrl = _bus.Sid.Read((ushort)(0xD400 + baseReg + 4));
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 4), (byte)(ctrl & 0xFE));
        }
    }

    public void SetTempo(int bpm) { if (bpm > 0) _bpm = bpm; }
    public void SetLoop(bool enabled) => _musicLoop = enabled;
    public void SetPriority(int[] voices)
    {
        for (int i = 0; i < Math.Min(voices.Length, _stealPriority.Length); i++)
            _stealPriority[i] = voices[i] - 1; // convert 1-based to 0-based
    }

    // --- Private helpers ---

    private int AllocateSfxVoice()
    {
        // Try to find a free voice (not used by music)
        for (int i = VoiceCount - 1; i >= 0; i--)
        {
            if (_sequences[i] == null && i != _sfxVoiceIndex)
                return i;
        }
        // Steal by priority
        foreach (int vi in _stealPriority)
        {
            if (vi != _sfxVoiceIndex)
                return vi;
        }
        return _stealPriority.Length > 0 ? _stealPriority[0] : VoiceCount - 1;
    }

    private void TickMusic()
    {
        double ticksPerFrame = 96.0 * _bpm / 3600.0;

        for (int v = 0; v < VoiceCount; v++)
        {
            if (_sequences[v] == null) continue;
            if (v == _sfxVoiceIndex) continue; // SFX owns this voice

            _seqTickAccum[v] += ticksPerFrame;

            while (_seqTickAccum[v] >= 1.0)
            {
                if (_seqWaitTicks[v] > 0)
                {
                    _seqWaitTicks[v]--;
                    _seqTickAccum[v] -= 1.0;
                    break;
                }

                if (_seqPos[v] >= _sequences[v]!.Count)
                {
                    if (_musicLoop)
                    {
                        _seqPos[v] = 0;
                        _loopStack[v].Clear();
                        continue;
                    }
                    break;
                }

                var evt = _sequences[v]![_seqPos[v]];
                _seqPos[v]++;

                ProcessEvent(v, evt);
            }

            // Per-frame effects
            ProcessEffects(v);
        }
    }

    private void ProcessEvent(int v, MmlEvent evt)
    {
        int baseReg = v * 7;
        switch (evt.Type)
        {
            case MmlEventType.NoteOn:
            {
                var inst = GetInstrument(_voiceInstrument[v]);
                int freq = MidiToSidFreq(evt.Param1);

                if (_voicePortamento[v])
                {
                    _voiceTargetFreq[v] = freq;
                    _voicePortamento[v] = false;
                }
                else
                {
                    _voiceCurrentFreq[v] = freq;
                    _voiceTargetFreq[v] = freq;
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 0), (byte)(freq & 0xFF));
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 1), (byte)((freq >> 8) & 0xFF));
                }

                int pw = _voicePulseWidth[v] > 0 ? _voicePulseWidth[v] : inst.PulseWidth;
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 2), (byte)(pw & 0xFF));
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 3), (byte)((pw >> 8) & 0x0F));
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 5), inst.Ad);
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 6), inst.Sr);
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 4), (byte)(inst.Waveform | 0x01));

                _voiceArpNotes[v] = null;
                _seqWaitTicks[v] = evt.Param2;
                break;
            }
            case MmlEventType.Rest:
            {
                // Gate off
                byte ctrl = _bus.Sid.Read((ushort)(0xD400 + baseReg + 4));
                _bus.Sid.Write((ushort)(0xD400 + baseReg + 4), (byte)(ctrl & 0xFE));
                _seqWaitTicks[v] = evt.Param2;
                break;
            }
            case MmlEventType.SetTempo:
                _bpm = evt.Param1;
                break;
            case MmlEventType.SetInstrument:
                _voiceInstrument[v] = evt.Param1;
                break;
            case MmlEventType.SetOctave:
                _voiceOctave[v] = evt.Param1;
                break;
            case MmlEventType.OctaveUp:
                _voiceOctave[v] = Math.Min(7, _voiceOctave[v] + 1);
                break;
            case MmlEventType.OctaveDown:
                _voiceOctave[v] = Math.Max(1, _voiceOctave[v] - 1);
                break;
            case MmlEventType.SetDefaultLen:
                _voiceDefaultLen[v] = evt.Param1;
                break;
            case MmlEventType.ArpStart:
            {
                _voiceArpNotes[v] = evt.ArpNotes;
                _voiceArpIndex[v] = 0;
                _voiceArpTicks[v] = 0;
                // Set initial note + gate
                if (evt.ArpNotes is { Length: > 0 })
                {
                    var inst = GetInstrument(_voiceInstrument[v]);
                    int freq = MidiToSidFreq(evt.ArpNotes[0]);
                    _voiceCurrentFreq[v] = freq;
                    int pw = _voicePulseWidth[v] > 0 ? _voicePulseWidth[v] : inst.PulseWidth;
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 0), (byte)(freq & 0xFF));
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 1), (byte)((freq >> 8) & 0xFF));
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 2), (byte)(pw & 0xFF));
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 3), (byte)((pw >> 8) & 0x0F));
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 5), inst.Ad);
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 6), inst.Sr);
                    _bus.Sid.Write((ushort)(0xD400 + baseReg + 4), (byte)(inst.Waveform | 0x01));
                }
                _seqWaitTicks[v] = evt.Param2;
                break;
            }
            case MmlEventType.SetPulseWidth:
                _voicePulseWidth[v] = evt.Param1;
                break;
            case MmlEventType.PwmSweep:
                _voicePwmDir[v] = evt.Param1;
                break;
            case MmlEventType.SetFilterCutoff:
                _filterCutoff = evt.Param1;
                _filterResonance = evt.Param2;
                WriteFilter();
                break;
            case MmlEventType.FilterMode:
                _filterMode = evt.Param1;
                WriteFilter();
                break;
            case MmlEventType.FilterSweep:
                _filterSweepDir[0] = evt.Param1;
                break;
            case MmlEventType.SetVibrato:
                _voiceVibrato[v] = evt.Param1;
                _voiceVibratoPhase[v] = 0;
                break;
            case MmlEventType.Portamento:
                _voicePortamento[v] = true;
                break;
            case MmlEventType.LoopStart:
                _loopStack[v].Push((_seqPos[v], 0));
                break;
            case MmlEventType.LoopEnd:
            {
                if (_loopStack[v].Count > 0)
                {
                    var (startPos, count) = _loopStack[v].Pop();
                    count++;
                    if (count < evt.Param1)
                    {
                        _loopStack[v].Push((startPos, count));
                        _seqPos[v] = startPos;
                    }
                }
                break;
            }
        }
    }

    private void ProcessEffects(int v)
    {
        int baseReg = v * 7;

        // Arpeggio cycling
        if (_voiceArpNotes[v] is { Length: > 1 })
        {
            _voiceArpIndex[v] = (_voiceArpIndex[v] + 1) % _voiceArpNotes[v].Length;
            int freq = MidiToSidFreq(_voiceArpNotes[v][_voiceArpIndex[v]]);
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 0), (byte)(freq & 0xFF));
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 1), (byte)((freq >> 8) & 0xFF));
        }

        // Portamento slide
        if (_voiceCurrentFreq[v] != _voiceTargetFreq[v])
        {
            int diff = _voiceTargetFreq[v] - _voiceCurrentFreq[v];
            int step = Math.Max(1, Math.Abs(diff) / 8);
            _voiceCurrentFreq[v] += diff > 0 ? step : -step;
            if (Math.Abs(_voiceCurrentFreq[v] - _voiceTargetFreq[v]) < step)
                _voiceCurrentFreq[v] = _voiceTargetFreq[v];
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 0), (byte)(_voiceCurrentFreq[v] & 0xFF));
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 1), (byte)((_voiceCurrentFreq[v] >> 8) & 0xFF));
        }

        // Vibrato
        if (_voiceVibrato[v] > 0)
        {
            _voiceVibratoPhase[v] = (_voiceVibratoPhase[v] + 8) & 0xFF;
            int vibOffset = (int)(Math.Sin(_voiceVibratoPhase[v] * Math.PI / 128.0) * _voiceVibrato[v]);
            int freq = _voiceCurrentFreq[v] + vibOffset;
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 0), (byte)(freq & 0xFF));
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 1), (byte)((freq >> 8) & 0xFF));
        }

        // PWM sweep
        if (_voicePwmDir[v] != 0)
        {
            _voicePulseWidth[v] = Math.Clamp(_voicePulseWidth[v] + _voicePwmDir[v] * 32, 0, 4095);
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 2), (byte)(_voicePulseWidth[v] & 0xFF));
            _bus.Sid.Write((ushort)(0xD400 + baseReg + 3), (byte)((_voicePulseWidth[v] >> 8) & 0x0F));
        }
    }

    private void WriteFilter()
    {
        // SID filter registers:
        // $D415/$D416 = cutoff (11 bits: lo 3 bits at D415, hi 8 bits at D416)
        // $D417 = resonance (hi nybble) + filter voice routing (lo 3 bits)
        // $D418 = filter mode (bits 4-6) + volume (bits 0-3)
        int cutoff = Math.Clamp(_filterCutoff, 0, 2047);
        _bus.Sid.Write(0xD415, (byte)(cutoff & 0x07));
        _bus.Sid.Write(0xD416, (byte)(cutoff >> 3));

        byte resonance = (byte)((_filterResonance & 0x0F) << 4);
        // Route all music voices through filter
        byte routing = 0;
        for (int i = 0; i < VoiceCount; i++)
            if (_sequences[i] != null) routing |= (byte)(1 << i);
        _bus.Sid.Write(0xD417, (byte)(resonance | routing));

        byte modeVol = _bus.Sid.Read(0xD418);
        byte vol = (byte)(modeVol & 0x0F);
        byte mode = (byte)((_filterMode & 0x07) << 4);
        _bus.Sid.Write(0xD418, (byte)(mode | vol));
    }

    internal static int MidiToSidFreq(int midiNote)
    {
        double hz = 440.0 * Math.Pow(2.0, (midiNote - 69) / 12.0);
        return (int)(hz * 16777216.0 / CpuClockRate);
    }
}
```

**Step 3: Run tests**

```bash
dotnet test --filter "MusicEngineTests" --verbosity normal
```
Expected: ALL PASS

**Step 4: Commit**

```bash
git add e6502.Avalonia/Hardware/MusicEngine.cs e6502UnitTests/MusicEngineTests.cs
git commit -m "feat: MusicEngine with instrument table, SFX playback, music sequencer"
```

---

## Task 4: Music Sequencer Tests

**Files:**
- Modify: `e6502UnitTests/MusicEngineTests.cs`

**Step 1: Add music sequencer tests**

```csharp
[TestMethod]
public void MusicPlay_WritesFirstNoteToSid()
{
    using var bus = MakeBus();
    var engine = new MusicEngine(bus);
    engine.DefineInstrument(1, 0x20, 2, 6, 10, 4); // saw
    engine.SetVolume(15);
    engine.SetSequence(1, "T120 I1 O4 C4");
    engine.MusicPlay();
    engine.Tick(); // first frame processes first event

    byte ctrl = bus.Sid.Read(0xD404); // voice 1 control
    Assert.AreEqual(0x21, ctrl, "Gate on + saw waveform on voice 1");
}

[TestMethod]
public void MusicStop_GatesOff()
{
    using var bus = MakeBus();
    var engine = new MusicEngine(bus);
    engine.DefineInstrument(1, 0x20, 2, 6, 10, 4);
    engine.SetVolume(15);
    engine.SetSequence(1, "T120 I1 C1");
    engine.MusicPlay();
    engine.Tick();

    engine.MusicStop();
    byte ctrl = bus.Sid.Read(0xD404);
    Assert.AreEqual(0x20, ctrl, "Gate off after MusicStop");
}

[TestMethod]
public void Arpeggio_CyclesNotes()
{
    using var bus = MakeBus();
    var engine = new MusicEngine(bus);
    engine.DefineInstrument(1, 0x40, 0, 9, 0, 0);
    engine.SetSequence(1, "I1 {CEG}1"); // C4=60, E4=64, G4=67
    engine.MusicPlay();
    engine.Tick(); // plays first note (C)

    int freq1Lo = bus.Sid.Read(0xD400);
    int freq1Hi = bus.Sid.Read(0xD401);
    int freqC = freq1Lo | (freq1Hi << 8);

    engine.Tick(); // arp cycles to E
    freq1Lo = bus.Sid.Read(0xD400);
    freq1Hi = bus.Sid.Read(0xD401);
    int freqE = freq1Lo | (freq1Hi << 8);

    Assert.AreNotEqual(freqC, freqE, "Arpeggio should cycle to different frequency");
    Assert.IsTrue(freqE > freqC, "E should be higher than C");
}

[TestMethod]
public void SfxStealsVoice_MusicContinuesOnOthers()
{
    using var bus = MakeBus();
    var engine = new MusicEngine(bus);
    engine.DefineInstrument(0, 0x40, 0, 9, 0, 0);
    engine.DefineInstrument(1, 0x20, 2, 6, 10, 4);

    // Music on voices 1 and 2
    engine.SetSequence(1, "I1 C1");
    engine.SetSequence(2, "I1 E1");
    engine.MusicPlay();
    engine.Tick();

    // SFX should steal voice 3 (free)
    engine.PlaySound(80, 5);
    byte ctrl3 = bus.Sid.Read(0xD412); // voice 3
    Assert.AreEqual(0x41, ctrl3, "SFX on voice 3 (free voice)");

    // Music voices still active
    byte ctrl1 = bus.Sid.Read(0xD404);
    Assert.AreEqual(0x21, ctrl1 & 0x21, "Voice 1 music still playing");
}

[TestMethod]
public void Loop_RepeatsSequence()
{
    using var bus = MakeBus();
    var engine = new MusicEngine(bus);
    engine.DefineInstrument(1, 0x40, 0, 9, 0, 0);
    // Very fast tempo, short notes — should loop twice
    engine.SetSequence(1, "T240 I1 [C16]2");
    engine.MusicPlay();

    // Run many frames to get through the loop
    int noteOnCount = 0;
    for (int i = 0; i < 120; i++)
    {
        byte ctrlBefore = bus.Sid.Read(0xD404);
        engine.Tick();
        byte ctrlAfter = bus.Sid.Read(0xD404);
        if ((ctrlAfter & 0x01) != 0 && (ctrlBefore & 0x01) == 0)
            noteOnCount++;
    }

    Assert.AreEqual(2, noteOnCount, "Loop should repeat note twice");
}
```

**Step 2: Run tests**

```bash
dotnet test --filter "MusicEngineTests" --verbosity normal
```
Expected: ALL PASS

**Step 3: Commit**

```bash
git add e6502UnitTests/MusicEngineTests.cs
git commit -m "test: music sequencer, arpeggio, voice stealing, loop tests"
```

---

## Task 5: FIO Integration — Wire MusicEngine Into Bus

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`

**Step 1: Add FIO command constants to VgcConstants.cs**

Add after `FioCmdSidStop`:

```csharp
public const byte FioCmdInstrument = 0x0A;  // define instrument
public const byte FioCmdSound      = 0x0B;  // play SFX
public const byte FioCmdVolume     = 0x0C;  // set master volume
public const byte FioCmdMusicSeq   = 0x0D;  // set voice sequence (MML string)
public const byte FioCmdMusicPlay  = 0x0E;  // start music
public const byte FioCmdMusicStop  = 0x0F;  // stop music
public const byte FioCmdMusicTempo = 0x10;  // set tempo
public const byte FioCmdMusicLoop  = 0x11;  // set loop on/off
public const byte FioCmdMusicPri   = 0x12;  // set SFX steal priority

// Music/SFX status register (read-only)
public const int  MusicStatus      = 0xBA50; // bit 0 = SFX playing, bit 1 = music playing
```

**Step 2: Add MusicEngine to CompositeBusDevice**

In `CompositeBusDevice.cs`:

- Add field: `private readonly MusicEngine _musicEngine;`
- Add property: `public MusicEngine Music => _musicEngine;`
- In constructor after `_sidPlayer = new SidPlayer(this);`: `_musicEngine = new MusicEngine(this);`
- Pass `_musicEngine` to FileIoController constructor
- In `Read()`, add: `if (address == VgcConstants.MusicStatus) return GetMusicStatus();`
- Add helper: `private byte GetMusicStatus() => (byte)((_musicEngine.IsPlaying ? 1 : 0) | (_musicEngine.IsMusicPlaying ? 2 : 0));`

**Step 3: Add MusicEngine to FileIoController**

Add `MusicEngine? _musicEngine` parameter to constructor. Add FIO command handlers:

```csharp
case VgcConstants.FioCmdInstrument:
    DoInstrument();
    break;
case VgcConstants.FioCmdSound:
    DoSound();
    break;
case VgcConstants.FioCmdVolume:
    DoVolume();
    break;
case VgcConstants.FioCmdMusicSeq:
    DoMusicSeq();
    break;
case VgcConstants.FioCmdMusicPlay:
    DoMusicPlay();
    break;
case VgcConstants.FioCmdMusicStop:
    DoMusicStop();
    break;
case VgcConstants.FioCmdMusicTempo:
    DoMusicTempo();
    break;
case VgcConstants.FioCmdMusicLoop:
    DoMusicLoop();
    break;
case VgcConstants.FioCmdMusicPri:
    DoMusicPri();
    break;
```

FIO handler implementations — reuse existing FIO register slots for parameter passing:

```csharp
private void DoInstrument()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    // Params: SrcL=id, SrcH=waveform, EndL=attack, EndH=decay, SizeL=sustain, SizeH=release
    int id = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
    byte waveform = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
    byte a = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];
    byte d = _regs[VgcConstants.FioEndH - VgcConstants.FioBase];
    byte s = _regs[VgcConstants.FioSizeL - VgcConstants.FioBase];
    byte r = _regs[VgcConstants.FioSizeH - VgcConstants.FioBase];
    _musicEngine.DefineInstrument(id, waveform, a, d, s, r);
    SetOk();
}

private void DoSound()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    // Params: SrcL=midi note, SrcH=duration, EndL=instrument id
    int note = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
    int duration = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
    int instId = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];
    _musicEngine.PlaySound(note, duration, instId);
    SetOk();
}

private void DoVolume()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    int level = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
    _musicEngine.SetVolume(level);
    SetOk();
}

private void DoMusicSeq()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    // SrcL = voice number (1-3), filename buffer = MML string
    int voice = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
    string? mml = ReadFilename(); // reuse filename buffer for MML string
    if (mml is null) { SetError(VgcConstants.FioErrIo); return; }
    _musicEngine.SetSequence(voice, mml);
    SetOk();
}

private void DoMusicPlay()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    _musicEngine.MusicPlay();
    SetOk();
}

private void DoMusicStop()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    _musicEngine.MusicStop();
    SetOk();
}

private void DoMusicTempo()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    int bpm = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
            | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
    _musicEngine.SetTempo(bpm);
    SetOk();
}

private void DoMusicLoop()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    bool on = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] != 0;
    _musicEngine.SetLoop(on);
    SetOk();
}

private void DoMusicPri()
{
    if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
    // SrcL=first priority, SrcH=second, EndL=third
    var pri = new List<int>();
    byte v1 = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
    byte v2 = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
    byte v3 = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];
    if (v1 > 0) pri.Add(v1);
    if (v2 > 0) pri.Add(v2);
    if (v3 > 0) pri.Add(v3);
    _musicEngine.SetPriority(pri.ToArray());
    SetOk();
}
```

**Step 4: Hook MusicEngine.Tick() into 60Hz timer in MainWindow.axaml.cs**

In the DispatcherTimer.Run callback (around line 64), add `_bus.Music.Tick();`:

```csharp
DispatcherTimer.Run(() =>
{
    _bus.Vgc.IncrementFrameCounter();
    if (_bus.Vgc.IsRasterIrqEnabled)
        _cpu.IrqWaiting = true;
    _bus.Music.Tick();  // ← ADD THIS
    _canvas.RequestRedraw();
    return true;
}, TimeSpan.FromMilliseconds(1000.0 / 60.0));
```

**Step 5: Build and run all tests**

```bash
dotnet build && dotnet test --verbosity normal
```
Expected: ALL PASS (124+ existing + new tests)

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/Hardware/FileIoController.cs e6502.Avalonia/MainWindow.axaml.cs
git commit -m "feat: wire MusicEngine into FIO commands and 60Hz frame tick"
```

---

## Task 6: BASIC Assembly — INSTRUMENT, SOUND, VOLUME Commands

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Update VSC register area to point to FIO**

Replace the VSC constants and SOUND/VOLUME/ENVELOPE/WAVE handlers with new implementations that write to FIO registers.

Replace `LAB_SOUND` (line ~8697):

```asm
; perform SOUND note, duration [, instrument]
; SOUND 60, 10       — MIDI note 60, 10 frames, default instrument
; SOUND 60, 10, 3    — MIDI note 60, 10 frames, instrument 3

LAB_SOUND
      JSR   LAB_GTBY          ; midi note → X
      STX   FIO_SRCL          ; note
      JSR   LAB_1C01          ; comma
      JSR   LAB_GTBY          ; duration → X
      STX   FIO_SRCH          ; duration
      LDA   #$00
      STA   FIO_ENDL          ; default instrument = 0
      JSR   LAB_GBYT          ; peek next token
      CMP   #','
      BNE   @snd_go
      JSR   LAB_IGBY          ; skip comma
      JSR   LAB_GTBY          ; instrument id → X
      STX   FIO_ENDL
@snd_go
      LDA   #FIO_CMD_SOUND
      STA   FIO_CMD
      RTS
```

Replace `LAB_VOLUME` (line ~8718):

```asm
; perform VOLUME level (0-15)

LAB_VOLUME
      JSR   LAB_GTBY          ; level → X
      STX   FIO_SRCL
      LDA   #FIO_CMD_VOLUME
      STA   FIO_CMD
      RTS
```

Replace `LAB_ENVELOPE` (line ~8727):

```asm
; perform INSTRUMENT id, waveform, a, d, s, r
; waveform: 1=TRI($10), 2=SAW($20), 4=PULSE($40), 8=NOISE($80)
; Actually pass raw SID waveform byte for simplicity.
; BASIC syntax: INSTRUMENT 1, $40, 0, 9, 0, 0

LAB_ENVELOPE
      JSR   LAB_GTBY          ; id → X
      STX   FIO_SRCL
      JSR   LAB_1C01
      JSR   LAB_GTBY          ; waveform → X
      STX   FIO_SRCH
      JSR   LAB_1C01
      JSR   LAB_GTBY          ; attack → X
      STX   FIO_ENDL
      JSR   LAB_1C01
      JSR   LAB_GTBY          ; decay → X
      STX   FIO_ENDH
      JSR   LAB_1C01
      JSR   LAB_GTBY          ; sustain → X
      STX   FIO_SIZEL
      JSR   LAB_1C01
      JSR   LAB_GTBY          ; release → X
      STX   FIO_SIZEH
      LDA   #FIO_CMD_INSTRUMENT
      STA   FIO_CMD
      RTS
```

Replace `LAB_WAVE` (line ~8748) — repurpose as NOP or remove (INSTRUMENT replaces WAVE+ENVELOPE):

```asm
; WAVE is deprecated — use INSTRUMENT instead
; Keep the handler to avoid breaking tokenization, but make it a no-op with error

LAB_WAVE
      JMP   LAB_15D9          ; syntax error — use INSTRUMENT instead
```

**Step 2: Add FIO command constants to assembly**

Add near the existing FIO constants:

```asm
FIO_CMD_INSTRUMENT = $0A    ; define instrument preset
FIO_CMD_SOUND  = $0B        ; play SFX (note, duration, instrument)
FIO_CMD_VOLUME = $0C        ; set master volume
FIO_CMD_MSEQ   = $0D        ; set voice MML sequence
FIO_CMD_MPLAY  = $0E        ; start music playback
FIO_CMD_MSTOP  = $0F        ; stop music playback
FIO_CMD_MTEMPO = $10        ; set music tempo (BPM)
FIO_CMD_MLOOP  = $11        ; set music loop on/off
FIO_CMD_MPRI   = $12        ; set SFX voice steal priority
MUSIC_STATUS   = $BA50      ; bit 0=SFX playing, bit 1=music playing
```

Also add the FIO register aliases used by SOUND if not already present:

```asm
FIO_ENDL      = $B9A6
FIO_ENDH      = $B9A7
FIO_SIZEL     = $B9A8
FIO_SIZEH     = $B9A9
```

**Step 3: Rename the keyword — ENVELOPE → INSTRUMENT**

In the keyword string table, change "ENVELOPE" to "INSTRUMENT" (the token `TK_ENVELOPE` stays the same internally but the keyword changes). Find the line in the keyword table that has `"OPE",TK_ENVELOPE` and update it. Also update `LBB_SOUND` label references.

Note: Because ehbasic tokenizes keywords by matching from a string table, changing "ENVELOPE" to "INSTRUMENT" requires updating the keyword match entries in the character-indexed tables. The token ID stays the same.

**Step 4: Build ROM and run tests**

```bash
cd ehbasic && make && cd .. && cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && dotnet build && dotnet test
```
Expected: ALL PASS

**Step 5: Commit**

```bash
git add ehbasic/basic.asm ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin
git commit -m "feat: rewire SOUND/VOLUME/INSTRUMENT to MusicEngine via FIO"
```

---

## Task 7: BASIC Assembly — MUSIC Command (Extended Token)

**Files:**
- Modify: `ehbasic/basic.asm`

The MUSIC command uses the extended token system (2-byte prefix). Add a new extended token `XTK_MUSIC`.

**Step 1: Add extended token**

After `XTK_SIDSTOP = $13`:

```asm
XTK_MUSIC         = $14              ; extended token id: MUSIC
```

Update `XTK_COUNT = 20` (was 19).

Add to `TAB_XTKSTR`:
```asm
      .word @s_music
...
@s_music:  .byte "MUSIC",0
```

Add to `TAB_XTKCMD`:
```asm
      .word LAB_MUSIC-1      ; XTK_MUSIC ($14)
```

**Step 2: Implement MUSIC handler**

MUSIC has subcommands:
- `MUSIC voice, "mml"` — set sequence (voice is 1-3)
- `MUSIC PLAY` — start
- `MUSIC STOP` — stop
- `MUSIC TEMPO bpm` — set tempo
- `MUSIC LOOP ON|OFF` — set loop
- `MUSIC PRIORITY v1 [,v2 [,v3]]` — set steal priority

The handler peeks at the next token to determine which subcommand:

```asm
; perform MUSIC subcommand
; MUSIC voice, "mml"
; MUSIC PLAY | STOP | TEMPO bpm | LOOP ON/OFF | PRIORITY v1[,v2[,v3]]

LAB_MUSIC
      JSR   LAB_GBYT          ; peek at next byte
      ; Check for subcommand keywords by comparing first char
      CMP   #'P'
      BEQ   @m_chk_play
      CMP   #'S'
      BEQ   @m_stop
      CMP   #'T'
      BEQ   @m_tempo
      CMP   #'L'
      BEQ   @m_loop
      ; If none matched, must be "MUSIC voice, string"
      JMP   @m_seq

@m_chk_play
      ; Could be PLAY or PRIORITY — check second char
      INY                     ; peek +1 (basic pointer offset)
      LDA   (Bpntrl),Y
      DEY
      CMP   #'R'
      BEQ   @m_priority
      ; It's PLAY — skip 4 chars "PLAY"
      LDA   #$04
      JMP   @m_skip_go_play

@m_skip_go_play
      TAX
@m_skip_loop
      JSR   LAB_IGBY
      DEX
      BNE   @m_skip_loop
      LDA   #FIO_CMD_MPLAY
      STA   FIO_CMD
      CLI                     ; enable interrupts for music
      RTS

@m_stop
      ; skip "STOP" (4 chars)
      JSR   LAB_IGBY
      JSR   LAB_IGBY
      JSR   LAB_IGBY
      JSR   LAB_IGBY
      LDA   #FIO_CMD_MSTOP
      STA   FIO_CMD
      RTS

@m_tempo
      ; skip "TEMPO" then parse number
      JSR   LAB_IGBY          ; T
      JSR   LAB_IGBY          ; E
      JSR   LAB_IGBY          ; M
      JSR   LAB_IGBY          ; P
      JSR   LAB_IGBY          ; O
      JSR   LAB_GTWRD         ; bpm → FAC1_3(lo), FAC1_2(hi)
      LDA   FAC1_3
      STA   FIO_SRCL
      LDA   FAC1_2
      STA   FIO_SRCH
      LDA   #FIO_CMD_MTEMPO
      STA   FIO_CMD
      RTS

@m_loop
      ; skip "LOOP" then parse ON/OFF
      JSR   LAB_IGBY          ; L
      JSR   LAB_IGBY          ; O
      JSR   LAB_IGBY          ; O
      JSR   LAB_IGBY          ; P
      JSR   LAB_IGBY          ; skip space
      ; Check for ON or OFF
      CMP   #TK_ON            ; ON token
      BEQ   @m_loop_on
      ; Must be OFF
      LDA   #$00
      STA   FIO_SRCL
      JMP   @m_loop_go
@m_loop_on
      LDA   #$01
      STA   FIO_SRCL
@m_loop_go
      JSR   LAB_IGBY          ; consume remaining chars
      LDA   #FIO_CMD_MLOOP
      STA   FIO_CMD
      RTS

@m_priority
      ; skip "PRIORITY" then parse voice numbers
      LDX   #$08              ; 8 chars in PRIORITY
@m_pri_skip
      JSR   LAB_IGBY
      DEX
      BNE   @m_pri_skip
      JSR   LAB_GTBY          ; first voice → X
      STX   FIO_SRCL
      LDA   #$00
      STA   FIO_SRCH
      STA   FIO_ENDL
      JSR   LAB_GBYT
      CMP   #','
      BNE   @m_pri_go
      JSR   LAB_IGBY
      JSR   LAB_GTBY          ; second voice → X
      STX   FIO_SRCH
      JSR   LAB_GBYT
      CMP   #','
      BNE   @m_pri_go
      JSR   LAB_IGBY
      JSR   LAB_GTBY          ; third voice → X
      STX   FIO_ENDL
@m_pri_go
      LDA   #FIO_CMD_MPRI
      STA   FIO_CMD
      RTS

@m_seq
      ; MUSIC voice, "mml string"
      JSR   LAB_GTBY          ; voice (1-3) → X
      STX   FIO_SRCL
      JSR   LAB_1C01          ; comma
      JSR   LAB_FIO_GETNAME   ; parse string → FIO_NAME/FIO_NAMELEN
      BCC   @m_seq_go
      JMP   LAB_FIO_ERRIO
@m_seq_go
      LDA   #FIO_CMD_MSEQ
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @m_seq_err
      RTS
@m_seq_err
      JMP   LAB_FIO_ERRIO
```

**Note:** The subcommand keyword parsing above is simplified. A more robust approach would tokenize PLAY/STOP/TEMPO/LOOP/PRIORITY as secondary tokens. However, since these are only valid after MUSIC, direct character matching works and avoids adding more tokens. The implementer should verify the exact byte-by-byte scanning against ehbasic's parsing conventions.

**Step 3: Build ROM and run tests**

```bash
cd ehbasic && make && cd .. && cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && dotnet build && dotnet test
```

**Step 4: Commit**

```bash
git add ehbasic/basic.asm ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin
git commit -m "feat: MUSIC extended token with PLAY/STOP/TEMPO/LOOP/PRIORITY/SEQ subcommands"
```

---

## Task 8: BASIC Assembly — PLAYING Function and ON SOUND GOSUB

**Files:**
- Modify: `ehbasic/basic.asm`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (ensure MusicStatus register readable)

**Step 1: Add PLAYING as a function token**

After `TK_BUMPED`:

```asm
TK_PLAYING        = TK_BUMPED+1      ; PLAYING function
```

Add to function table and keyword table. The implementation reads `MUSIC_STATUS` ($BA50) and returns bit 0:

```asm
; perform PLAYING — returns 1 if SFX active, 0 otherwise
LAB_PLAYING
      LDA   MUSIC_STATUS      ; read music status register
      AND   #$01              ; isolate SFX playing bit
      TAY
      LDA   #$00
      JMP   LAB_AYFC          ; return integer AY to FAC1
```

**Step 2: Add ON SOUND GOSUB**

This follows the same pattern as `ON IRQ GOSUB`. Add a new token `TK_SOUNDEVT` (or reuse a secondary position in the ON handler). Store callback address in a zero-page pair.

In the `LAB_ON` handler, add a check for the SOUND token alongside IRQ/NMI. When the SFX-complete flag is set (checked in the main loop or IRQ handler), trigger the stored GOSUB.

**Note:** ON SOUND GOSUB is the most complex assembly piece. If it causes delays, it can be deferred to a follow-up task — PLAYING covers the polling use case.

**Step 3: Build ROM and run tests**

```bash
cd ehbasic && make && cd .. && cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && dotnet build && dotnet test
```

**Step 4: Commit**

```bash
git add ehbasic/basic.asm ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin
git commit -m "feat: PLAYING function and ON SOUND GOSUB callback"
```

---

## Task 9: Remove Dead VSC Code and Cleanup

**Files:**
- Modify: `ehbasic/basic.asm` — remove VSC_CMD, VSC_P0-P4 constants, VSCMD_* constants
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs` — remove any VSC references if present

**Step 1: Clean up assembly**

Remove the VSC register block:

```asm
; --- VSC sound controller registers --- (REMOVE THIS BLOCK)
VSC_CMD       = $A100
VSC_P0        = $A101
...
VSCMD_WAVE    = $04
```

These addresses are no longer used — all sound/music goes through FIO.

**Step 2: Build and test**

```bash
cd ehbasic && make && cd .. && cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && dotnet build && dotnet test
```

**Step 3: Commit**

```bash
git add ehbasic/basic.asm ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin
git commit -m "chore: remove dead VSC sound controller registers"
```

---

## Task 10: Integration Test and TCP/MCP Support

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs` — add TCP commands for INSTRUMENT/SOUND/MUSIC
- Modify: `e6502.MCP/EmulatorTools.cs` — add MCP tools if appropriate

**Step 1: Add TCP commands for testing**

Add handlers in EmulatorTcpServer for:
- `instrument id waveform a d s r`
- `sound note duration [instrument]`
- `volume level`
- `music voice "mml"`
- `music play` / `music stop` / `music loop on|off` / `music tempo bpm`

This allows testing from the MCP tools without needing BASIC.

**Step 2: Test end-to-end via MCP**

Use the emulator MCP tools to:
1. Define instruments
2. Set a music sequence
3. Start playback
4. Play a sound effect while music runs
5. Stop music

**Step 3: Commit**

```bash
git add e6502.Avalonia/Ipc/EmulatorTcpServer.cs
git commit -m "feat: TCP commands for MusicEngine (instrument, sound, music)"
```

---

## Summary

| Task | Component | Key Deliverable |
|------|-----------|-----------------|
| 1 | MmlTypes | Event types, instrument struct |
| 2 | MmlParser | Full MML string parser with tests |
| 3 | MusicEngine | Instrument table, SFX playback, voice allocation |
| 4 | MusicEngine Tests | Sequencer, arpeggio, voice stealing, loop tests |
| 5 | FIO Integration | Wire into bus, FIO commands, 60Hz tick |
| 6 | BASIC Assembly | INSTRUMENT, SOUND, VOLUME commands |
| 7 | BASIC Assembly | MUSIC extended token with subcommands |
| 8 | BASIC Assembly | PLAYING function, ON SOUND GOSUB |
| 9 | Cleanup | Remove dead VSC code |
| 10 | Integration | TCP/MCP commands, end-to-end test |
