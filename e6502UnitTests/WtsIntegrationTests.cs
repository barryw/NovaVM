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
        Assert.IsTrue(audio.Any(s => Math.Abs(s) > 100), "Should produce audio");

        // Verify status register shows active
        byte status = bus.Read((ushort)(v0 + VgcConstants.WtsVoiceStatus));
        Assert.IsTrue((status & 0x01) != 0, "Voice should be active");
    }

    [TestMethod]
    public void MusicEngine_MixedSidAndWts()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Wts.LoadBank(MakeTestBank());

        // SID voice 0
        bus.Music.DirectNoteOn(0, 48, 100, 0);
        // WTS voice (MusicEngine voice 6 = WTS voice 0)
        bus.Music.DirectNoteOn(6, 72, 100, 0);

        // Both should show active notes
        Assert.AreEqual(48, bus.Music.GetVoiceMidi(0));
        Assert.AreEqual(72, bus.Music.GetVoiceMidi(6));

        // SID voice 0 control register should have gate bit set
        byte sidCtrl = bus.Sid.Read(0xD404);
        Assert.IsTrue((sidCtrl & 0x01) != 0, "SID voice should be gated");

        // WTS should have voice 0 active
        Assert.IsTrue((bus.Wts.ActiveVoiceMask & 0x01) != 0, "WTS voice should be active");
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
        Assert.AreEqual(0, bus.Wts.ActiveVoiceMask);
    }

    [TestMethod]
    public void MultipleVoices_IndependentVolumeAndPan()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Wts.LoadBank(MakeTestBank());

        // Voice 0: loud, left
        bus.Wts.SetVolume(0, 255);
        bus.Wts.SetPanning(0, 0);
        bus.Wts.NoteOn(0, 60, 127, 0);

        // Voice 1: quiet, right
        bus.Wts.SetVolume(1, 64);
        bus.Wts.SetPanning(1, 255);
        bus.Wts.NoteOn(1, 72, 127, 0);

        short[] audio = bus.Wts.RenderSamples(512);

        // Should have audio
        Assert.IsTrue(audio.Any(s => Math.Abs(s) > 50));
        // Both voices active
        Assert.AreEqual(0b00000011, bus.Wts.ActiveVoiceMask & 0b00000011);
    }

    [TestMethod]
    public void SoundfontStatus_ReflectsLoadState()
    {
        var bus = new CompositeBusDevice(enableSound: false);

        // No bank loaded
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsSoundfontStatus));
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsInstrumentCount));

        // Load bank
        bus.Wts.LoadBank(MakeTestBank());
        Assert.AreEqual(1, bus.Read((ushort)VgcConstants.WtsSoundfontStatus));
        Assert.AreEqual(2, bus.Read((ushort)VgcConstants.WtsInstrumentCount));
    }
}
