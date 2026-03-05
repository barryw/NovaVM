using e6502.Avalonia.Hardware;
using Melanchall.DryWetMidi.Common;
using Melanchall.DryWetMidi.Core;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MidiPlaybackTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    [TestMethod]
    public void Play_SetsIsPlaying()
    {
        var bus = MakeBus();
        var engine = new MusicEngine(bus);
        var playback = new MidiPlayback(engine);

        var midi = BuildSimpleMidi();
        playback.Play(midi, voiceToChannel: new[] { 0 }, instrumentSlots: new[] { 0 });

        Assert.IsTrue(playback.IsPlaying);
    }

    [TestMethod]
    public void Stop_ClearsIsPlaying()
    {
        var bus = MakeBus();
        var engine = new MusicEngine(bus);
        var playback = new MidiPlayback(engine);

        var midi = BuildSimpleMidi();
        playback.Play(midi, voiceToChannel: new[] { 0 }, instrumentSlots: new[] { 0 });
        playback.Stop();

        Assert.IsFalse(playback.IsPlaying);
    }

    [TestMethod]
    public void Tick_AdvancesAndStops()
    {
        var bus = MakeBus();
        var engine = new MusicEngine(bus);
        var playback = new MidiPlayback(engine);

        // 1 note, 1 quarter @ 120 BPM, PPQN=96 (DryWetMidi default)
        // ticks/frame = 96 * 120 / 3600 = 3.2
        // 96 ticks / 3.2 ticks/frame = 30 frames to consume the note-off
        var midi = BuildSimpleMidi();
        playback.Play(midi, voiceToChannel: new[] { 0 }, instrumentSlots: new[] { 0 });

        for (int i = 0; i < 60; i++)
            playback.Tick();

        Assert.IsFalse(playback.IsPlaying);
    }

    private static MidiFile BuildSimpleMidi()
    {
        // PPQN=96, 120 BPM, one note on channel 0
        // Default MidiFile PPQN is 96.
        // NoteOn at tick 0, NoteOff at tick 96 (one quarter note at PPQN=96)
        // ticks/frame = 96 * 120 / 3600 = 3.2
        // 96 ticks / 3.2 ticks/frame = 30 frames to consume the note-off
        var midi = new MidiFile(
            new TrackChunk(
                new NoteOnEvent((SevenBitNumber)60, (SevenBitNumber)100) { Channel = (FourBitNumber)0, DeltaTime = 0 },
                new NoteOffEvent((SevenBitNumber)60, (SevenBitNumber)0) { Channel = (FourBitNumber)0, DeltaTime = 96 }
            )
        );
        return midi;
    }
}
