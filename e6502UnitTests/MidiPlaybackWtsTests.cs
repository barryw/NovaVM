using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MidiPlaybackWtsTests
{
    [TestMethod]
    public void RoutingMode_DefaultIsAuto()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        Assert.AreEqual(MidiRoutingMode.Auto, bus.MidiPlayback.RoutingMode);
    }

    [TestMethod]
    public void RoutingMode_SidOnly_Uses6Voices()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.MidiPlayback.RoutingMode = MidiRoutingMode.SidOnly;
        Assert.AreEqual(6, bus.MidiPlayback.AvailableVoiceCount);
    }

    [TestMethod]
    public void RoutingMode_Auto_Uses14Voices()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.MidiPlayback.RoutingMode = MidiRoutingMode.Auto;
        Assert.AreEqual(14, bus.MidiPlayback.AvailableVoiceCount);
    }

    [TestMethod]
    public void RoutingMode_Manual_Uses14Voices()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.MidiPlayback.RoutingMode = MidiRoutingMode.Manual;
        Assert.AreEqual(14, bus.MidiPlayback.AvailableVoiceCount);
    }
}
