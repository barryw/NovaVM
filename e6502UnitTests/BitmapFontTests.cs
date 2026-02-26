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

    [TestMethod]
    public void MultiFontConstructor_LoadsMultipleFonts()
    {
        var font0 = new byte[2048];
        var font1 = new byte[2048];
        font1[0] = 0xFF; // first row of char 0 = all pixels set
        var font = new BitmapFont(new[] { font0, font1 });
        Assert.AreEqual(0x00, font.GetRow(0, 0, 0)); // font 0, char 0, row 0
        Assert.AreEqual(0xFF, font.GetRow(1, 0, 0)); // font 1, char 0, row 0
    }

    [TestMethod]
    public void MultiFontGetRow_InvalidSlot_FallsBackToSlot0()
    {
        var font0 = new byte[2048];
        font0[0] = 0xAA;
        var font = new BitmapFont(new[] { font0 });
        Assert.AreEqual(0xAA, font.GetRow(5, 0, 0)); // slot 5 empty → falls back to 0
    }

    [TestMethod]
    public void MultiFontGetPixel_UsesCorrectSlot()
    {
        var font0 = new byte[2048];
        var font1 = new byte[2048];
        font1[0x41 * 8] = 0x80; // char 0x41, row 0, leftmost pixel set
        var font = new BitmapFont(new[] { font0, font1 });
        Assert.IsFalse(font.GetPixel(0, 0x41, 0, 0)); // font 0: not set
        Assert.IsTrue(font.GetPixel(1, 0x41, 0, 0));  // font 1: set
    }
}
