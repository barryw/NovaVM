using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MmlParserTests
{
    [TestMethod]
    public void Parse_SingleNote_DefaultQuarter()
    {
        var events = MmlParser.Parse("C");
        Assert.AreEqual(1, events.Count);
        Assert.AreEqual(MmlEventType.NoteOn, events[0].Type);
        Assert.AreEqual(60, events[0].Param1);  // C4 = MIDI 60
        Assert.AreEqual(96, events[0].Param2);  // quarter note = 96 ticks
    }
}
