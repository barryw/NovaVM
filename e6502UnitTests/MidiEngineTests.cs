using System.Collections.Generic;
using System.Linq;
using e6502.Avalonia.Hardware;
using Melanchall.DryWetMidi.Common;
using Melanchall.DryWetMidi.Core;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
            var events = new List<MidiEvent>();
            events.Add(new ProgramChangeEvent((SevenBitNumber)(byte)prog) { Channel = (FourBitNumber)(byte)ch });
            for (int i = 0; i < count; i++)
            {
                events.Add(new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100) { Channel = (FourBitNumber)(byte)ch });
                events.Add(new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0) { Channel = (FourBitNumber)(byte)ch });
            }
            midi.Chunks.Add(new TrackChunk(events.ToArray()));
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

        // Explicit mapping: voice 1->ch3, voice 2->ch7
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
        // 96 + 24 = 120 ticks = quarter + sixteenth → "4&16"
        string result = MidiEngine.QuantizeDuration(120);
        Assert.AreEqual("4&16", result);
    }

    [TestMethod]
    public void QuantizeDuration_LongTie()
    {
        // 384 + 192 = 576 = dotted whole → "1."
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

    // ---------- GenerateMml tests ----------

    /// <summary>
    /// Build a track chunk with interleaved NoteOn/NoteOff events using delta-time encoding.
    /// notes = (noteNumber, velocity, deltaOnFromPrev, durationTicks)
    /// </summary>
    private static MidiFile BuildTimedMidi(int ppqn,
        params (int note, int velocity, long deltaOnFromPrev, long durationTicks)[] notes)
    {
        var events = new List<MidiEvent>();
        foreach (var (note, vel, delta, dur) in notes)
        {
            events.Add(new NoteOnEvent((SevenBitNumber)(byte)note, (SevenBitNumber)(byte)vel)
                { Channel = (FourBitNumber)0, DeltaTime = delta });
            events.Add(new NoteOffEvent((SevenBitNumber)(byte)note, (SevenBitNumber)0)
                { Channel = (FourBitNumber)0, DeltaTime = dur });
        }
        var midi = new MidiFile(new TrackChunk(events.ToArray()))
        {
            TimeDivision = new TicksPerQuarterNoteTimeDivision((short)ppqn)
        };
        return midi;
    }

    [TestMethod]
    public void GenerateMml_SimpleScale()
    {
        // C major scale: C4 D E F G A B C5, each a quarter note at PPQN=480
        // MIDI notes: 60, 62, 64, 65, 67, 69, 71, 72
        var midi = BuildTimedMidi(480,
            (60, 80, 0, 480),  // C4
            (62, 80, 0, 480),  // D4
            (64, 80, 0, 480),  // E4
            (65, 80, 0, 480),  // F4
            (67, 80, 0, 480),  // G4
            (69, 80, 0, 480),  // A4
            (71, 80, 0, 480),  // B4
            (72, 80, 0, 480)); // C5

        string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

        Assert.IsTrue(mml.Contains("C"), $"Expected C in: {mml}");
        Assert.IsTrue(mml.Contains("D"), $"Expected D in: {mml}");
        Assert.IsTrue(mml.Contains("E"), $"Expected E in: {mml}");
        Assert.IsTrue(mml.Contains("G"), $"Expected G in: {mml}");
        Assert.IsTrue(mml.Contains("A"), $"Expected A in: {mml}");
        Assert.IsTrue(mml.Contains("B"), $"Expected B in: {mml}");
        Assert.IsTrue(mml.Contains("O"), $"Expected octave O in: {mml}");
    }

    [TestMethod]
    public void GenerateMml_IncludesVelocity()
    {
        // Two notes with different velocities: vel=50 and vel=120
        var midi = BuildTimedMidi(480,
            (60, 50,  0, 480),  // C4 vel=50
            (64, 120, 0, 480)); // E4 vel=120

        string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

        Assert.IsTrue(mml.Contains("V"), $"Expected V in: {mml}");
    }

    [TestMethod]
    public void GenerateMml_RestsForGaps()
    {
        // Note at time 0-480, gap 480-960, note at 960-1440
        var events = new List<MidiEvent>
        {
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)80)  { Channel = (FourBitNumber)0, DeltaTime = 0 },
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0)  { Channel = (FourBitNumber)0, DeltaTime = 480 },
            // gap: NoteOn for second note starts 480 ticks after NoteOff
            new NoteOnEvent((SevenBitNumber)64, (SevenBitNumber)80)  { Channel = (FourBitNumber)0, DeltaTime = 480 },
            new NoteOffEvent((SevenBitNumber)64, (SevenBitNumber)0)  { Channel = (FourBitNumber)0, DeltaTime = 480 },
        };
        var midi = new MidiFile(new TrackChunk(events.ToArray()))
        {
            TimeDivision = new TicksPerQuarterNoteTimeDivision(480)
        };

        string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

        Assert.IsTrue(mml.Contains("R"), $"Expected R rest in: {mml}");
    }

    [TestMethod]
    public void GenerateMml_TopNoteWinsForChords()
    {
        // Two simultaneous notes: C4 (60) and E4 (64), both at time 0, both off at 480
        var events = new List<MidiEvent>
        {
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)80)  { Channel = (FourBitNumber)0, DeltaTime = 0 },
            new NoteOnEvent((SevenBitNumber)64, (SevenBitNumber)80)  { Channel = (FourBitNumber)0, DeltaTime = 0 },
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0)  { Channel = (FourBitNumber)0, DeltaTime = 480 },
            new NoteOffEvent((SevenBitNumber)64, (SevenBitNumber)0)  { Channel = (FourBitNumber)0, DeltaTime = 0 },
        };
        var midi = new MidiFile(new TrackChunk(events.ToArray()))
        {
            TimeDivision = new TicksPerQuarterNoteTimeDivision(480)
        };

        string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

        // Top note (E4=64) wins
        Assert.IsTrue(mml.Contains("E"), $"Expected E (top note) in: {mml}");
    }

    [TestMethod]
    public void GenerateMml_IncludesTempoChange()
    {
        // SetTempoEvent 500000 µs = 120 BPM
        var events = new List<MidiEvent>
        {
            new SetTempoEvent(500000) { DeltaTime = 0 },
            new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)80) { Channel = (FourBitNumber)0, DeltaTime = 0 },
            new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0) { Channel = (FourBitNumber)0, DeltaTime = 480 },
        };
        var midi = new MidiFile(new TrackChunk(events.ToArray()))
        {
            TimeDivision = new TicksPerQuarterNoteTimeDivision(480)
        };

        string mml = MidiEngine.GenerateMml(midi, channel: 0, ppqn: 480);

        Assert.IsTrue(mml.Contains("T120"), $"Expected T120 in: {mml}");
    }

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
        // Build a channel with many timed notes to produce MML that exceeds maxLineLen=80.
        // 40 quarter notes at PPQN=480, each generates ~2 chars of MML → ~80+ chars total.
        var noteArgs = Enumerable.Repeat((60, 80, 0L, 480L), 40).ToArray();
        var midi = BuildTimedMidi(480, noteArgs);
        string bas = MidiEngine.GenerateBasProgram(midi, maxLineLen: 80);

        // Multiple MUSIC 1,"..." lines expected
        int musicLines = bas.Split('\n').Count(l => l.Contains("MUSIC 1,"));
        Assert.IsTrue(musicLines > 1, $"Expected multiple MUSIC lines, got {musicLines}");
    }
}
