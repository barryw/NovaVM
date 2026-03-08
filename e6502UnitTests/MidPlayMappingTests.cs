using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace e6502UnitTests;

[TestClass]
public class MidPlayMappingTests
{
    [TestMethod]
    public void ParseMidPlayString_NoMapping()
    {
        var (filename, mapping) = MidPlayParser.Parse("sousa-stars-stripes");
        Assert.AreEqual("sousa-stars-stripes", filename);
        Assert.IsNull(mapping);
    }

    [TestMethod]
    public void ParseMidPlayString_SingleMapping()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(1, mapping!.Count);
        Assert.AreEqual(0, mapping[6]); // voice 7 (1-based) = index 6 (0-based)
    }

    [TestMethod]
    public void ParseMidPlayString_MultipleMapping()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0,8=1,1=9");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(3, mapping!.Count);
        Assert.AreEqual(0, mapping[6]);
        Assert.AreEqual(1, mapping[7]);
        Assert.AreEqual(9, mapping[0]);
    }

    [TestMethod]
    public void ParseMidPlayString_InvalidPairIgnored()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0,bad,8=1");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(2, mapping!.Count);
    }

    [TestMethod]
    public void ParseMidPlayString_VoiceOutOfRangeIgnored()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,0=0,15=1,7=2");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(1, mapping!.Count);
        Assert.AreEqual(2, mapping[6]);
    }

    [TestMethod]
    public void ParseMidPlayString_TrailingComma()
    {
        var (filename, mapping) = MidPlayParser.Parse("song,7=0,");
        Assert.AreEqual("song", filename);
        Assert.IsNotNull(mapping);
        Assert.AreEqual(1, mapping!.Count);
    }
}
