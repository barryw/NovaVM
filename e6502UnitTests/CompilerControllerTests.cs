using System;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class CompilerControllerTests
{
    private readonly byte[] _xram = new byte[512 * 1024];

    private CompilerController CreateController()
    {
        return new CompilerController(
            addr => _xram[addr],
            (addr, data) => { });
    }

    [TestMethod]
    public void OwnsAddress_CmpBase_True()
    {
        var cc = CreateController();
        Assert.IsTrue(cc.OwnsAddress((ushort)VgcConstants.CmpBase));
    }

    [TestMethod]
    public void OwnsAddress_CmpEnd_True()
    {
        var cc = CreateController();
        Assert.IsTrue(cc.OwnsAddress((ushort)VgcConstants.CmpEnd));
    }

    [TestMethod]
    public void OwnsAddress_BelowRange_False()
    {
        var cc = CreateController();
        Assert.IsFalse(cc.OwnsAddress((ushort)(VgcConstants.CmpBase - 1)));
    }

    [TestMethod]
    public void OwnsAddress_AboveRange_False()
    {
        var cc = CreateController();
        Assert.IsFalse(cc.OwnsAddress((ushort)(VgcConstants.CmpEnd + 1)));
    }

    [TestMethod]
    public void InitialStatus_IsIdle()
    {
        var cc = CreateController();
        Assert.AreEqual(VgcConstants.CmpStatusIdle, cc.Read((ushort)VgcConstants.CmpStatus));
    }

    [TestMethod]
    public void Compile_EmptySource_SetsErrorStatus()
    {
        var cc = CreateController();
        // XRAM at 0 is already zero (empty source)
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        Assert.AreEqual(VgcConstants.CmpStatusError, cc.Read((ushort)VgcConstants.CmpStatus));
        Assert.IsTrue(cc.Read((ushort)VgcConstants.CmpErrCount) > 0);
    }

    [TestMethod]
    public void Compile_ValidSource_SetsOkStatus()
    {
        var source = System.Text.Encoding.ASCII.GetBytes("void main() { }");
        Array.Copy(source, _xram, source.Length);

        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        Assert.AreEqual(VgcConstants.CmpStatusOk, cc.Read((ushort)VgcConstants.CmpStatus));
        Assert.AreEqual(0, cc.Read((ushort)VgcConstants.CmpErrCount));
        // Code size should be non-zero
        int codeSize = cc.Read((ushort)VgcConstants.CmpCodeSizeL)
                     | (cc.Read((ushort)VgcConstants.CmpCodeSizeH) << 8);
        Assert.IsTrue(codeSize > 0, $"Code size should be > 0, was {codeSize}");
    }

    [TestMethod]
    public void ReadErrorMessage_ByteByByte()
    {
        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        // Error index 0
        cc.Write((ushort)VgcConstants.CmpErrIdx, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdGetError);

        // Read message "empty source" byte-by-byte
        var expected = "empty source";
        for (int i = 0; i < expected.Length; i++)
        {
            byte b = cc.Read((ushort)VgcConstants.CmpErrMsg);
            Assert.AreEqual((byte)expected[i], b, $"Mismatch at position {i}");
        }

        // After message ends, should return 0
        Assert.AreEqual(0, cc.Read((ushort)VgcConstants.CmpErrMsg));
    }

    [TestMethod]
    public void GetError_PopulatesLineFields()
    {
        var cc = CreateController();
        // Compile empty source → error at line 1
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        cc.Write((ushort)VgcConstants.CmpErrIdx, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdGetError);

        Assert.AreEqual(1, cc.Read((ushort)VgcConstants.CmpErrLineL));
        Assert.AreEqual(0, cc.Read((ushort)VgcConstants.CmpErrLineH));
    }

    [TestMethod]
    public void WriteRead_RegisterRoundTrip()
    {
        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0x42);
        Assert.AreEqual(0x42, cc.Read((ushort)VgcConstants.CmpSrcAddrL));

        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0xAB);
        Assert.AreEqual(0xAB, cc.Read((ushort)VgcConstants.CmpSrcAddrM));

        cc.Write((ushort)VgcConstants.CmpSrcAddrH, 0x07);
        Assert.AreEqual(0x07, cc.Read((ushort)VgcConstants.CmpSrcAddrH));
    }

    [TestMethod]
    public void Compile_24BitXramAddress_ReadsCorrectLocation()
    {
        // Place valid source at XRAM address 0x010000 (64K boundary)
        int addr = 0x010000;
        var source = System.Text.Encoding.ASCII.GetBytes("void main() { }");
        Array.Copy(source, 0, _xram, addr, source.Length);

        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, (byte)(addr & 0xFF));
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, (byte)((addr >> 8) & 0xFF));
        cc.Write((ushort)VgcConstants.CmpSrcAddrH, (byte)((addr >> 16) & 0xFF));
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        // Source at 24-bit address should compile successfully (not "empty source")
        Assert.AreEqual(VgcConstants.CmpStatusOk, cc.Read((ushort)VgcConstants.CmpStatus),
            "24-bit XRAM address should read source correctly");
    }

    [TestMethod]
    public void Compile_InvalidSource_SetsErrorWithMessage()
    {
        // Invalid source should produce compiler errors
        var source = System.Text.Encoding.ASCII.GetBytes("this is not valid C;");
        Array.Copy(source, _xram, source.Length);

        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        Assert.AreEqual(VgcConstants.CmpStatusError, cc.Read((ushort)VgcConstants.CmpStatus));
        Assert.IsTrue(cc.Read((ushort)VgcConstants.CmpErrCount) > 0);

        // Read error message byte-by-byte — should be non-empty
        cc.Write((ushort)VgcConstants.CmpErrIdx, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdGetError);
        byte first = cc.Read((ushort)VgcConstants.CmpErrMsg);
        Assert.AreNotEqual(0, first, "Error message should not be empty");
    }

    [TestMethod]
    public void GetError_InvalidIndex_DoesNotCrash()
    {
        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        // Set error index beyond count
        cc.Write((ushort)VgcConstants.CmpErrIdx, 99);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdGetError);

        // Reading message for invalid index should return 0
        Assert.AreEqual(0, cc.Read((ushort)VgcConstants.CmpErrMsg));
    }
}
