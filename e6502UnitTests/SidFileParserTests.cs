using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidFileParserTests
{
    private static byte[] MakeMinimalPsid(ushort loadAddr = 0x1000, ushort initAddr = 0x1000, ushort playAddr = 0x1003)
    {
        var header = new byte[124 + 3];

        header[0] = (byte)'P'; header[1] = (byte)'S'; header[2] = (byte)'I'; header[3] = (byte)'D';
        header[4] = 0x00; header[5] = 0x02;  // version 2
        header[6] = 0x00; header[7] = 0x7C;  // data offset = 124
        header[8] = (byte)(loadAddr >> 8); header[9] = (byte)(loadAddr & 0xFF);
        header[10] = (byte)(initAddr >> 8); header[11] = (byte)(initAddr & 0xFF);
        header[12] = (byte)(playAddr >> 8); header[13] = (byte)(playAddr & 0xFF);
        header[14] = 0x00; header[15] = 0x01;  // 1 song
        header[16] = 0x00; header[17] = 0x01;  // start song 1
        // speed at 18-21 = 0 (VBI)

        // Payload: NOP NOP NOP
        header[124] = 0xEA; header[125] = 0xEA; header[126] = 0xEA;

        return header;
    }

    [TestMethod]
    public void Parse_ValidPsid_ReturnsHeader()
    {
        var data = MakeMinimalPsid();
        var result = SidFileParser.Parse(data);
        Assert.IsTrue(result.IsValid);
        Assert.AreEqual("PSID", result.Magic);
        Assert.AreEqual(2, result.Version);
    }

    [TestMethod]
    public void Parse_LoadAddress_Correct()
    {
        var data = MakeMinimalPsid(loadAddr: 0x1000, initAddr: 0x1000, playAddr: 0x1003);
        var result = SidFileParser.Parse(data);
        Assert.AreEqual((ushort)0x1000, result.LoadAddress);
        Assert.AreEqual((ushort)0x1000, result.InitAddress);
        Assert.AreEqual((ushort)0x1003, result.PlayAddress);
    }

    [TestMethod]
    public void Parse_Payload_ExtractedCorrectly()
    {
        var data = MakeMinimalPsid();
        var result = SidFileParser.Parse(data);
        Assert.AreEqual(3, result.Payload.Length);
        Assert.AreEqual(0xEA, result.Payload[0]);
    }

    [TestMethod]
    public void Parse_InvalidMagic_ReturnsInvalid()
    {
        var data = MakeMinimalPsid();
        data[0] = (byte)'X';
        var result = SidFileParser.Parse(data);
        Assert.IsFalse(result.IsValid);
    }

    [TestMethod]
    public void Parse_SpeedFlag_Bit0_IsCiaTiming()
    {
        var data = MakeMinimalPsid();
        data[21] = 0x01;
        var result = SidFileParser.Parse(data);
        Assert.IsTrue(result.UsesCiaTiming);
    }

    [TestMethod]
    public void Parse_LoadAddressZero_ReadsFromPayload()
    {
        // loadAddress=0 means first 2 bytes of data are the actual load address (little-endian)
        var header = new byte[124 + 5];
        header[0] = (byte)'P'; header[1] = (byte)'S'; header[2] = (byte)'I'; header[3] = (byte)'D';
        header[4] = 0x00; header[5] = 0x02;
        header[6] = 0x00; header[7] = 0x7C;
        // loadAddress = 0
        header[8] = 0x00; header[9] = 0x00;
        header[10] = 0x10; header[11] = 0x00;  // init = $1000
        header[12] = 0x10; header[13] = 0x03;  // play = $1003
        header[14] = 0x00; header[15] = 0x01;
        header[16] = 0x00; header[17] = 0x01;
        // Payload: load address (little-endian) + data
        header[124] = 0x00; header[125] = 0x10;  // $1000 LE
        header[126] = 0xEA; header[127] = 0xEA; header[128] = 0xEA;

        var result = SidFileParser.Parse(header);
        Assert.IsTrue(result.IsValid);
        Assert.AreEqual((ushort)0x1000, result.LoadAddress);
        Assert.AreEqual(3, result.Payload.Length);  // the 2-byte address prefix is stripped
    }

    [TestMethod]
    public void Parse_TooShort_ReturnsInvalid()
    {
        var result = SidFileParser.Parse(new byte[50]);
        Assert.IsFalse(result.IsValid);
    }
}
