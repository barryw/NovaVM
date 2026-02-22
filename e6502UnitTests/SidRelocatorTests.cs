using System;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SidRelocator = e6502.Avalonia.Hardware.SidRelocator;

namespace e6502UnitTests;

[TestClass]
public class SidRelocatorTests
{
    private static byte[] MakeTestSid(ushort loadAddr, ushort initAddr, ushort playAddr, byte[] payload)
    {
        var data = new byte[124 + payload.Length];
        data[0]=(byte)'P'; data[1]=(byte)'S'; data[2]=(byte)'I'; data[3]=(byte)'D';
        data[4]=0x00; data[5]=0x02; data[6]=0x00; data[7]=0x7C;
        data[8]=(byte)(loadAddr>>8); data[9]=(byte)(loadAddr&0xFF);
        data[10]=(byte)(initAddr>>8); data[11]=(byte)(initAddr&0xFF);
        data[12]=(byte)(playAddr>>8); data[13]=(byte)(playAddr&0xFF);
        data[14]=0x00; data[15]=0x01; data[16]=0x00; data[17]=0x01;
        Array.Copy(payload, 0, data, 124, payload.Length);
        return data;
    }

    [TestMethod]
    public void Relocate_PatchesAbsoluteJsr()
    {
        // JSR $1003 / RTS / LDA #$FF / RTS
        byte[] payload = [0x20, 0x03, 0x10, 0x60, 0xA9, 0xFF, 0x60];
        var sid = MakeTestSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);
        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.IsTrue(result.IsValid);
        Assert.AreEqual(0x2000, result.LoadAddress);
        Assert.AreEqual(0x20, result.Payload[0]); // JSR opcode unchanged
        Assert.AreEqual(0x03, result.Payload[1]); // low byte
        Assert.AreEqual(0x20, result.Payload[2]); // high byte patched from $10 to $20
    }

    [TestMethod]
    public void Relocate_DoesNotPatchImmediateValues()
    {
        // LDA #$10 / RTS
        byte[] payload = [0xA9, 0x10, 0x60];
        var sid = MakeTestSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);
        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.AreEqual(0xA9, result.Payload[0]);
        Assert.AreEqual(0x10, result.Payload[1]); // NOT patched
    }

    [TestMethod]
    public void Relocate_DoesNotPatchSidRegisters()
    {
        // STA $D400 / RTS
        byte[] payload = [0x8D, 0x00, 0xD4, 0x60];
        var sid = MakeTestSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);
        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.AreEqual(0x00, result.Payload[1]); // NOT patched (outside player range)
        Assert.AreEqual(0xD4, result.Payload[2]);
    }

    [TestMethod]
    public void Relocate_FollowsBranches()
    {
        // BNE +2 / JMP $1004 / RTS
        byte[] payload = [0xD0, 0x02, 0x4C, 0x04, 0x10, 0x60];
        var sid = MakeTestSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);
        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.AreEqual(0xD0, result.Payload[0]); // BNE unchanged
        Assert.AreEqual(0x02, result.Payload[1]); // relative offset unchanged
        Assert.AreEqual(0x4C, result.Payload[2]); // JMP opcode
        Assert.AreEqual(0x04, result.Payload[3]); // low byte patched
        Assert.AreEqual(0x20, result.Payload[4]); // high byte patched from $10 to $20
    }

    [TestMethod]
    public void Relocate_UpdatesHeaderAddresses()
    {
        byte[] payload = [0x60, 0x60];
        var sid = MakeTestSid(0x1000, 0x1000, 0x1001, payload);
        var info = SidFileParser.Parse(sid);
        var result = SidRelocator.Relocate(info, 0x2000);

        Assert.AreEqual(0x2000, result.LoadAddress);
        Assert.AreEqual(0x2000, result.InitAddress);
        Assert.AreEqual(0x2001, result.PlayAddress);
    }

    [TestMethod]
    public void Relocate_ZeroDelta_ReturnsUnchanged()
    {
        byte[] payload = [0x60];
        var sid = MakeTestSid(0x1000, 0x1000, 0x1000, payload);
        var info = SidFileParser.Parse(sid);
        var result = SidRelocator.Relocate(info, 0x1000);

        Assert.AreEqual(0x1000, result.LoadAddress);
        Assert.AreEqual(0x60, result.Payload[0]);
    }
}
