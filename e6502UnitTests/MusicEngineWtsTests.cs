using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MusicEngineWtsTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    private static void LoadTestBank(CompositeBusDevice bus)
    {
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
    }

    [TestMethod]
    public void VoiceCount_Is14()
    {
        var bus = MakeBus();
        Assert.AreEqual(14, bus.Music.TotalVoiceCount);
    }

    [TestMethod]
    public void DirectNoteOn_SidVoice_WritesSidRegisters()
    {
        var bus = MakeBus();
        bus.Music.DirectNoteOn(0, 60, 100, 0);
        byte ctrl = bus.Sid.Read(0xD404);
        Assert.IsTrue((ctrl & 0x01) != 0, "SID voice should be gated");
    }

    [TestMethod]
    public void DirectNoteOn_WtsVoice_ActivatesWts()
    {
        var bus = MakeBus();
        LoadTestBank(bus);
        bus.Music.DirectNoteOn(6, 60, 100, 0);
        Assert.IsTrue((bus.Wts.ActiveVoiceMask & 0x01) != 0, "WTS voice 0 should be active");
    }

    [TestMethod]
    public void DirectNoteOff_WtsVoice_ReleasesWts()
    {
        var bus = MakeBus();
        LoadTestBank(bus);
        bus.Music.DirectNoteOn(6, 60, 100, 0);
        bus.Music.DirectNoteOff(6);
        // Render to let release complete (test bank has default 0.3s release)
        bus.Wts.RenderSamples(44100);  // 1 second of audio
        Assert.AreEqual(0, bus.Wts.ActiveVoiceMask & 0x01);
    }

    [TestMethod]
    public void GetVoiceMidi_WorksForBothSidAndWts()
    {
        var bus = MakeBus();
        LoadTestBank(bus);
        bus.Music.DirectNoteOn(2, 48, 100, 0);
        bus.Music.DirectNoteOn(8, 72, 100, 0);
        Assert.AreEqual(48, bus.Music.GetVoiceMidi(2));
        Assert.AreEqual(72, bus.Music.GetVoiceMidi(8));
    }

    [TestMethod]
    public void DirectNoteOn_OutOfRange_Ignored()
    {
        var bus = MakeBus();
        // Should not throw
        bus.Music.DirectNoteOn(-1, 60, 100, 0);
        bus.Music.DirectNoteOn(14, 60, 100, 0);
    }

    [TestMethod]
    public void DirectNoteSlide_WtsVoice_UpdatesMidi()
    {
        var bus = MakeBus();
        LoadTestBank(bus);
        bus.Music.DirectNoteOn(7, 60, 100, 0);
        bus.Music.DirectNoteSlide(7, 64);
        Assert.AreEqual(64, bus.Music.GetVoiceMidi(7));
    }

    [TestMethod]
    public void GetVoiceMidi_InvalidVoice_ReturnsNegative()
    {
        var bus = MakeBus();
        Assert.AreEqual(-1, bus.Music.GetVoiceMidi(-1));
        Assert.AreEqual(-1, bus.Music.GetVoiceMidi(14));
    }

    [TestMethod]
    public void MusicStop_ReleasesWtsVoices()
    {
        var bus = MakeBus();
        LoadTestBank(bus);
        bus.Music.EnterMidiMode();
        bus.Music.DirectNoteOn(6, 60, 100, 0);
        bus.Music.DirectNoteOn(7, 64, 100, 0);
        bus.Music.MusicStop();
        // WTS voices should be in release
        bus.Wts.RenderSamples(44100);
        Assert.AreEqual(0, bus.Wts.ActiveVoiceMask & 0x03, "WTS voices should be released after MusicStop");
    }
}
