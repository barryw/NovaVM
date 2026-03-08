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

        int voiceBase = VgcConstants.WtsVoiceBase;
        bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceInstrument), 0);
        bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceVelocity), 100);
        bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceNote), 60);

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

        bus.Write((ushort)VgcConstants.WtsEnumIndex, 0);
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsEnumBank));
        Assert.AreEqual(0, bus.Read((ushort)VgcConstants.WtsEnumProgram));
        Assert.AreEqual((byte)'G', bus.Read((ushort)VgcConstants.WtsEnumName));
        Assert.AreEqual((byte)'r', bus.Read((ushort)(VgcConstants.WtsEnumName + 1)));
    }
}
