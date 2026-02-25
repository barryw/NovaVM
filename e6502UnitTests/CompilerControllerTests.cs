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
    public void Compile_WithSource_SetsErrorStatus_NotYetImplemented()
    {
        // Put some source in XRAM
        var source = System.Text.Encoding.ASCII.GetBytes("int main() { return 0; }");
        Array.Copy(source, _xram, source.Length);

        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        // Compiler not yet implemented, so error expected
        Assert.AreEqual(VgcConstants.CmpStatusError, cc.Read((ushort)VgcConstants.CmpStatus));
        Assert.AreEqual(1, cc.Read((ushort)VgcConstants.CmpErrCount));
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
        // Place source at XRAM address 0x010000 (64K boundary)
        int addr = 0x010000;
        var source = System.Text.Encoding.ASCII.GetBytes("int x;");
        Array.Copy(source, 0, _xram, addr, source.Length);

        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, (byte)(addr & 0xFF));
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, (byte)((addr >> 8) & 0xFF));
        cc.Write((ushort)VgcConstants.CmpSrcAddrH, (byte)((addr >> 16) & 0xFF));
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        // Source is non-empty so we should NOT get "empty source" error
        // (compiler not yet implemented, so we get that error instead)
        cc.Write((ushort)VgcConstants.CmpErrIdx, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdGetError);

        // Read first few bytes of the error message - should be "compiler" not "empty"
        byte first = cc.Read((ushort)VgcConstants.CmpErrMsg);
        Assert.AreEqual((byte)'c', first, "Expected 'compiler not yet implemented', got 'empty source' - 24-bit address not working");
    }

    [TestMethod]
    public void ReadWarningMessage_ByteByByte()
    {
        // Put some source in XRAM so we hit the "not yet implemented" path
        var source = System.Text.Encoding.ASCII.GetBytes("int main() { return 0; }");
        Array.Copy(source, _xram, source.Length);

        var cc = CreateController();
        cc.Write((ushort)VgcConstants.CmpSrcAddrL, 0);
        cc.Write((ushort)VgcConstants.CmpSrcAddrM, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdCompile);

        // Should have at least one warning
        Assert.IsTrue(cc.Read((ushort)VgcConstants.CmpWarnCount) > 0);

        // Select warning index 0 and issue GetWarn command
        cc.Write((ushort)VgcConstants.CmpErrIdx, 0);
        cc.Write((ushort)VgcConstants.CmpCmd, VgcConstants.CmpCmdGetWarn);

        // Read warning message byte-by-byte
        var expected = "compilation support is experimental";
        for (int i = 0; i < expected.Length; i++)
        {
            byte b = cc.Read((ushort)VgcConstants.CmpErrMsg);
            Assert.AreEqual((byte)expected[i], b, $"Mismatch at position {i}");
        }

        // After message ends, should return 0
        Assert.AreEqual(0, cc.Read((ushort)VgcConstants.CmpErrMsg));
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
