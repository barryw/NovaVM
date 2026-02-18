using System;
using System.IO;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class BitmapFontTests
{
    [TestMethod]
    public void GetPixel_SpaceChar_AllFalse()
    {
        var font = BitmapFont.LoadFromFile(Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin"));
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                Assert.IsFalse(font.GetPixel(0x20, x, y));
    }

    [TestMethod]
    public void GetPixel_FullBlock_AllTrue()
    {
        var font = BitmapFont.LoadFromFile(Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin"));
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                Assert.IsTrue(font.GetPixel(0xDB, x, y));
    }

    [TestMethod]
    public void FontData_IsExactly2048Bytes()
    {
        var data = File.ReadAllBytes(Path.Combine(AppContext.BaseDirectory, "Resources", "cp437.bin"));
        Assert.AreEqual(2048, data.Length);
    }

    [TestMethod]
    public void Constructor_WrongSize_Throws()
    {
        Assert.ThrowsException<ArgumentException>(() => new BitmapFont(new byte[100]));
    }
}
