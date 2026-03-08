using System.Collections.Generic;
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
