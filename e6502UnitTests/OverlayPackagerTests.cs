using System;
using System.IO;
using e6502.Tools;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class OverlayPackagerTests
{
    [TestMethod]
    public void PackWritesNovoHeaderAndPayload()
    {
        string dir = NewTempDir();
        string input = Path.Combine(dir, "payload.bin");
        string symbols = Path.Combine(dir, "payload.sym");
        string output = Path.Combine(dir, "module.ovl");

        File.WriteAllBytes(input, [0xA9, 0x01, 0x60, 0xEA]);
        File.WriteAllText(symbols,
            """
            al 007000 .overlay_init
            al 007002 .overlay_main
            al 007003 .overlay_tick
            """);

        OverlayPackageResult result = OverlayPackager.Pack(new OverlayPackageOptions
        {
            InputPath = input,
            OutputPath = output,
            SymbolPath = symbols,
            LoadAddress = 0x7000,
            MaxSize = 0x2000,
            InitSymbol = "overlay_init",
            MainSymbol = "overlay_main",
            TickSymbol = ".overlay_tick",
            ModuleId = 0x1234,
            ModuleVersion = 0x0002,
            BssSize = 0x0040,
        });

        byte[] data = File.ReadAllBytes(output);
        Assert.AreEqual(OverlayPackager.HeaderSize + 4, data.Length);
        CollectionAssert.AreEqual(new byte[] { (byte)'N', (byte)'O', (byte)'V', (byte)'O' }, data[..4]);
        Assert.AreEqual(OverlayPackager.FormatVersion, data[0x04]);
        Assert.AreEqual(OverlayPackager.AbiMajor, data[0x05]);
        Assert.AreEqual(OverlayPackager.AbiMinor, data[0x06]);
        AssertU16(data, 0x08, 0x7000, "load address");
        AssertU16(data, 0x0A, 0x0004, "payload size");
        AssertU16(data, 0x0C, 0x0040, "BSS size");
        AssertU16(data, 0x0E, 0x7000, "init entry");
        AssertU16(data, 0x10, 0x7002, "main entry");
        AssertU16(data, 0x12, 0x7003, "tick entry");
        AssertU16(data, 0x14, 0x0000, "unload entry");
        AssertU16(data, 0x16, 0x1234, "module id");
        AssertU16(data, 0x18, 0x0002, "module version");
        AssertU16(data, 0x1E, 0x01F4, "payload checksum");
        CollectionAssert.AreEqual(new byte[] { 0xA9, 0x01, 0x60, 0xEA }, data[OverlayPackager.HeaderSize..]);

        Assert.AreEqual(4, result.PayloadSize);
        Assert.AreEqual(0x7000, result.InitEntry);
        Assert.AreEqual(0x7002, result.MainEntry);
        Assert.AreEqual(0x7003, result.TickEntry);
        Assert.AreEqual(0x0000, result.UnloadEntry);
        Assert.AreEqual(0x01F4, result.Checksum);
    }

    [TestMethod]
    public void PackRejectsEntryOutsidePayloadRange()
    {
        string dir = NewTempDir();
        string input = Path.Combine(dir, "payload.bin");
        string output = Path.Combine(dir, "module.ovl");
        File.WriteAllBytes(input, [0x60, 0xEA]);

        var options = new OverlayPackageOptions
        {
            InputPath = input,
            OutputPath = output,
            LoadAddress = 0x7000,
            MainEntry = 0x7100,
        };

        InvalidOperationException ex = Assert.ThrowsException<InvalidOperationException>(() =>
            OverlayPackager.Pack(options));
        StringAssert.Contains(ex.Message, "outside overlay payload range");
    }

    [TestMethod]
    public void PackRejectsPayloadAndBssOutsideSlot()
    {
        string dir = NewTempDir();
        string input = Path.Combine(dir, "payload.bin");
        string output = Path.Combine(dir, "module.ovl");
        File.WriteAllBytes(input, [0x60, 0xEA]);

        var options = new OverlayPackageOptions
        {
            InputPath = input,
            OutputPath = output,
            LoadAddress = 0x7000,
            MaxSize = 0x20,
            BssSize = 0x1F,
        };

        InvalidOperationException ex = Assert.ThrowsException<InvalidOperationException>(() =>
            OverlayPackager.Pack(options));
        StringAssert.Contains(ex.Message, "payload+BSS");
    }

    [TestMethod]
    public void ParseU16AcceptsNovaAddressFormats()
    {
        Assert.AreEqual(0x7000, OverlayPackager.ParseU16("$7000", "test"));
        Assert.AreEqual(0x7000, OverlayPackager.ParseU16("0x7000", "test"));
        Assert.AreEqual(7000, OverlayPackager.ParseU16("7000", "test"));
    }

    private static void AssertU16(byte[] data, int offset, ushort expected, string message)
    {
        ushort actual = (ushort)(data[offset] | (data[offset + 1] << 8));
        Assert.AreEqual(expected, actual, message);
    }

    private static string NewTempDir()
    {
        string dir = Path.Combine(Path.GetTempPath(), "e6502-overlay-packager-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(dir);
        return dir;
    }
}
