using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class WtsTcpTests
{
    [TestMethod]
    public void WtsListInstruments_EmptyBank_ReturnsZeroCount()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        Assert.AreEqual(0, bus.Wts.InstrumentCount);
    }

    [TestMethod]
    public void WtsListInstruments_WithBank_ReturnsList()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument { Name = "Piano", MidiBank = 0, MidiProgram = 0 });
        bank.Instruments.Add(new SampleInstrument { Name = "Strings", MidiBank = 0, MidiProgram = 48 });
        bus.Wts.LoadBank(bank);

        Assert.AreEqual(2, bus.Wts.InstrumentCount);
        Assert.AreEqual("Piano", bus.Wts.GetInstrumentName(0));
        Assert.AreEqual("Strings", bus.Wts.GetInstrumentName(1));
        Assert.AreEqual(0, bus.Wts.GetInstrumentProgram(0));
        Assert.AreEqual(48, bus.Wts.GetInstrumentProgram(1));
    }

    [TestMethod]
    public void WtsGetInstrumentBank_ReturnsCorrectBank()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        var bank = new SampleBank();
        bank.Instruments.Add(new SampleInstrument { Name = "Test", MidiBank = 5, MidiProgram = 10 });
        bus.Wts.LoadBank(bank);

        Assert.AreEqual(5, bus.Wts.GetInstrumentBank(0));
        Assert.AreEqual(10, bus.Wts.GetInstrumentProgram(0));
    }
}
