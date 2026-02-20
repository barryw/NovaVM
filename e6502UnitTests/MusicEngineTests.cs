using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace e6502UnitTests;

[TestClass]
public class MusicEngineTests
{
    private const ushort SidBase = 0xD400;

    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    // -------------------------------------------------------------------------
    // Instrument tests
    // -------------------------------------------------------------------------

    [TestMethod]
    public void DefineInstrument_StoresInTable()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.DefineInstrument(3, 0x20, 2, 4, 8, 3);
        SidInstrument inst = engine.GetInstrument(3);

        Assert.AreEqual(0x20, inst.Waveform);
        Assert.AreEqual(2,    inst.Attack);
        Assert.AreEqual(4,    inst.Decay);
        Assert.AreEqual(8,    inst.Sustain);
        Assert.AreEqual(3,    inst.Release);
    }

    [TestMethod]
    public void DefaultInstrument_IsPulseBeep()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        SidInstrument inst = engine.GetInstrument(0);

        Assert.AreEqual(0x40, inst.Waveform); // pulse
        Assert.AreEqual(0,    inst.Attack);
        Assert.AreEqual(9,    inst.Decay);
        Assert.AreEqual(0,    inst.Sustain);
        Assert.AreEqual(6,    inst.Release);
    }

    // -------------------------------------------------------------------------
    // SFX tests
    // -------------------------------------------------------------------------

    [TestMethod]
    public void PlaySound_WritesGateAndWaveformToSid()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // Instrument 0 is pulse (0x40), gate should be set (bit 0)
        engine.PlaySound(69, 30); // A4, 30 frames

        // Control register for voice 0 = SidBase + 4
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(0x41, ctrl, "Expected pulse waveform (0x40) | gate (0x01)");
    }

    [TestMethod]
    public void PlaySound_FrequencyCorrectForA440()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(69, 10); // MIDI 69 = A4 = 440 Hz

        byte lo = bus.Sid.Read(SidBase + 0);
        byte hi = bus.Sid.Read(SidBase + 1);
        int  freq = lo | (hi << 8);

        // Expected: 440 * 16777216 / 985248 ≈ 7482 (PAL)
        // Allow ±10 for rounding
        Assert.IsTrue(Math.Abs(freq - 7482) <= 10, $"Expected ~7482, got {freq}");
    }

    [TestMethod]
    public void PlaySound_GatesOffAfterDuration()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(69, 2); // 2 frames

        engine.Tick(); // frame 1 — gate still on
        byte ctrlAfterTick1 = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(1, ctrlAfterTick1 & 0x01, "Gate should still be on after 1 tick");

        engine.Tick(); // frame 2 — gate off
        byte ctrlAfterTick2 = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(0, ctrlAfterTick2 & 0x01, "Gate should be off after duration expires");
    }

    [TestMethod]
    public void IsPlaying_TrueWhileSfxActive()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(60, 5);
        Assert.IsTrue(engine.IsPlaying, "IsPlaying should be true immediately after PlaySound");

        // Tick until done
        for (int i = 0; i < 5; i++)
            engine.Tick();

        Assert.IsFalse(engine.IsPlaying, "IsPlaying should be false after duration expires");
    }

    [TestMethod]
    public void SfxCompleted_SetWhenSfxEnds()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.PlaySound(60, 1);
        Assert.IsFalse(engine.SfxCompleted, "SfxCompleted should not be set before expiry");

        engine.Tick(); // expires here

        Assert.IsTrue(engine.SfxCompleted, "SfxCompleted should be true after SFX expires");
        Assert.IsFalse(engine.SfxCompleted, "SfxCompleted is one-shot — should clear on read");
    }

    // -------------------------------------------------------------------------
    // Volume
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SetVolume_WritesMasterVolume()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetVolume(12);

        byte vol = bus.Sid.Read(SidBase + 0x18);
        Assert.AreEqual(12, vol & 0x0F, "Volume bits 0-3 should be 12");
    }

    [TestMethod]
    public void SetVolume_PreservesFilterMode()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // Pre-set filter mode bits (bits 4-7) to 0x50
        bus.Sid.Write(SidBase + 0x18, 0x50);

        engine.SetVolume(9);

        byte vol = bus.Sid.Read(SidBase + 0x18);
        Assert.AreEqual(0x50, vol & 0xF0, "Filter mode bits 4-7 should be preserved");
        Assert.AreEqual(9,    vol & 0x0F, "Volume bits 0-3 should be updated");
    }

    // -------------------------------------------------------------------------
    // Music basic
    // -------------------------------------------------------------------------

    [TestMethod]
    public void MusicPlay_WritesFirstNoteToSid()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // C4 = MIDI 60, quarter note
        engine.SetSequence(0, "C4");
        engine.MusicPlay();

        // Tick once to trigger the first event (WaitTicks starts at 0, accumulates ticks)
        engine.Tick();

        // Control register voice 0 should have gate on
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(1, ctrl & 0x01, "Gate should be on after first note event");
    }

    [TestMethod]
    public void MusicStop_GatesOffMusicVoices()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        engine.SetSequence(0, "C1"); // whole note
        engine.MusicPlay();
        engine.Tick(); // play the note

        engine.MusicStop();

        // Gate should be off on voice 0
        byte ctrl = bus.Sid.Read(SidBase + 4);
        Assert.AreEqual(0, ctrl & 0x01, "Gate should be cleared after MusicStop");
    }

    // -------------------------------------------------------------------------
    // Voice allocation
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SfxUseFreeVoice_WhenMusicOnOtherVoices()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // Put music on voices 0 and 1 only
        engine.SetSequence(0, "C1");
        engine.SetSequence(1, "E1");
        engine.MusicPlay();
        engine.Tick();

        // Voice 2 has no sequence — SFX should use it
        engine.PlaySound(69, 5);

        // Voice 2 control register should have gate on
        byte ctrl2 = bus.Sid.Read(SidBase + 14 + 4); // voice 2 ctrl = base + 14 + 4
        Assert.AreEqual(1, ctrl2 & 0x01, "SFX should use free voice 2");
    }

    [TestMethod]
    public void SfxStealsLowestPriorityVoice_WhenAllBusy()
    {
        var bus    = MakeBus();
        var engine = new MusicEngine(bus);

        // All 3 voices have sequences
        engine.SetSequence(0, "C1");
        engine.SetSequence(1, "E1");
        engine.SetSequence(2, "G1");
        engine.MusicPlay();
        engine.Tick();

        // Default steal priority is {2,1,0} — voice 2 stolen first
        engine.PlaySound(60, 5);

        // SFX should write to voice 2
        byte ctrl2 = bus.Sid.Read(SidBase + 14 + 4);
        Assert.AreEqual(1, ctrl2 & 0x01, "SFX should steal voice 2 (lowest priority)");
    }
}
