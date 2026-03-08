using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class MmlParserWtsTests
{
    [TestMethod]
    public void AtI_ParsesWtsInstrumentProgram()
    {
        var events = MmlParser.Parse("@I5 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(MmlEventType.SetWtsInstrument, setWts.Type);
        Assert.AreEqual(5, setWts.Param1);
    }

    [TestMethod]
    public void AtI_DefaultsToZero()
    {
        var events = MmlParser.Parse("@I C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(MmlEventType.SetWtsInstrument, setWts.Type);
        Assert.AreEqual(0, setWts.Param1);
    }

    [TestMethod]
    public void AtI_MaxProgram127()
    {
        var events = MmlParser.Parse("@I127 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(127, setWts.Param1);
    }

    [TestMethod]
    public void AtD_ParsesDrumKit()
    {
        var events = MmlParser.Parse("@D0 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(MmlEventType.SetWtsInstrument, setWts.Type);
        Assert.AreEqual(32768, setWts.Param1);
    }

    [TestMethod]
    public void AtD_ParsesDrumKitWithProgram()
    {
        var events = MmlParser.Parse("@D25 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.AreEqual(32793, setWts.Param1);
    }

    [TestMethod]
    public void AtW_NoLongerRecognized()
    {
        var events = MmlParser.Parse("@W5 C4");
        Assert.IsFalse(events.Any(e => e.Type == MmlEventType.SetWtsInstrument));
    }

    [TestMethod]
    public void AtI_DoesNotInterfereWithAtP()
    {
        var events = MmlParser.Parse("@P2048 @I5 C4");
        Assert.IsTrue(events.Any(e => e.Type == MmlEventType.SetPulseWidth));
        Assert.IsTrue(events.Any(e => e.Type == MmlEventType.SetWtsInstrument));
    }
}
