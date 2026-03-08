using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Linq;

namespace e6502UnitTests;

[TestClass]
public class MmlParserWtsTests
{
    [TestMethod]
    public void AtW_ParsesWtsInstrumentIndex()
    {
        var events = MmlParser.Parse("@W5 C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.IsNotNull(setWts);
        Assert.AreEqual(5, setWts.Param1);
    }

    [TestMethod]
    public void AtW_DefaultsToZero()
    {
        var events = MmlParser.Parse("@W C4");
        var setWts = events.FirstOrDefault(e => e.Type == MmlEventType.SetWtsInstrument);
        Assert.IsNotNull(setWts);
        Assert.AreEqual(0, setWts.Param1);
    }

    [TestMethod]
    public void AtW_DoesNotInterfereWithExistingAt()
    {
        var events = MmlParser.Parse("@0 C4");
        Assert.IsFalse(events.Any(e => e.Type == MmlEventType.SetWtsInstrument));
    }
}
