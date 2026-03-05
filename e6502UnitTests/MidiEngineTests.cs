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
}
