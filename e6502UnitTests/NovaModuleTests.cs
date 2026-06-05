using System;
using System.IO;
using System.Linq;
using System.Text;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Tests for NovaModule — the shared parser for the .nmod / .mod module format
/// (NL header + optional "NDOC" JSON documentation trailer). See
/// docs/plans/2026-06-05-nmod-self-documenting-modules-design.md.
/// </summary>
[TestClass]
public class NovaModuleTests
{
    private static byte[] MakeImage(byte id, byte abi, byte fnCount)
    {
        var b = new byte[16384];
        b[0] = 0x4C;             // JMP opcode
        b[3] = 0x4E; b[4] = 0x4C; // "NL"
        b[5] = id; b[6] = abi; b[7] = fnCount;
        return b;
    }

    private static byte[] MakeNmod(byte[] image, string json)
    {
        var payload = Encoding.UTF8.GetBytes(json);
        using var ms = new MemoryStream();
        ms.Write(image);
        ms.Write(Encoding.ASCII.GetBytes("NDOC"));
        var len = new byte[4];
        len[0] = (byte)(payload.Length & 0xFF);
        len[1] = (byte)((payload.Length >> 8) & 0xFF);
        len[2] = (byte)((payload.Length >> 16) & 0xFF);
        len[3] = (byte)((payload.Length >> 24) & 0xFF);
        ms.Write(len);
        ms.Write(payload);
        return ms.ToArray();
    }

    private const string EchoJson =
        "{\"ndocVersion\":1,\"module\":{\"name\":\"TESTMOD\",\"id\":127," +
        "\"abiVersion\":1,\"version\":\"1.0\",\"brief\":\"b\",\"abiNote\":\"x\"}," +
        "\"functions\":[{\"id\":0,\"idHex\":\"$00\",\"name\":\"ECHO\",\"brief\":\"e\"," +
        "\"args\":[{\"i\":0,\"name\":\"v\",\"type\":\"s32\",\"desc\":\"d\"}]," +
        "\"ret\":{\"type\":\"s32\",\"desc\":\"r\"},\"status\":[\"LERR_OK\"]}]}";

    [TestMethod]
    public void Parse_ValidNmod_ReadsHeaderAndDoc()
    {
        NovaModule m = NovaModule.Parse(MakeNmod(MakeImage(0x7F, 1, 1), EchoJson));

        Assert.IsTrue(m.Valid, m.Error);
        Assert.AreEqual(0x7F, m.Id);
        Assert.AreEqual(1, m.AbiVersion);
        Assert.AreEqual(1, m.FnCount);
        Assert.IsTrue(m.HasDoc);
        Assert.IsNotNull(m.Doc);
        Assert.AreEqual("TESTMOD", m.Doc!.Module.Name);
        Assert.AreEqual("1.0", m.Doc.Module.Version);
        Assert.AreEqual(1, m.Doc.Functions.Count);

        NovaFn fn = m.Doc.Functions[0];
        Assert.AreEqual("ECHO", fn.Name);
        Assert.AreEqual("$00", fn.IdHex);
        Assert.AreEqual("s32", fn.Args[0].Type);
        Assert.AreEqual("s32", fn.Ret.Type);
        CollectionAssert.AreEqual(new[] { "LERR_OK" }, fn.Status.ToArray());
    }

    [TestMethod]
    public void Parse_PlainMod_NoTrailer_IsHeaderOnly()
    {
        NovaModule m = NovaModule.Parse(MakeImage(0x01, 1, 13));
        Assert.IsTrue(m.Valid, m.Error);
        Assert.AreEqual(0x01, m.Id);
        Assert.AreEqual(13, m.FnCount);
        Assert.IsFalse(m.HasDoc);
        Assert.IsNull(m.Doc);
    }

    [TestMethod]
    public void Parse_BadMagic_IsInvalid()
    {
        byte[] img = MakeImage(0x7F, 1, 1);
        img[3] = 0x00;                       // corrupt the "NL" magic
        NovaModule m = NovaModule.Parse(MakeNmod(img, EchoJson));
        Assert.IsFalse(m.Valid);
        Assert.IsNotNull(m.Error);
    }

    [TestMethod]
    public void Parse_TooShort_IsInvalid()
    {
        NovaModule m = NovaModule.Parse(new byte[100]);
        Assert.IsFalse(m.Valid);
        Assert.IsNotNull(m.Error);
    }

    [TestMethod]
    public void Parse_TruncatedTrailer_IsInvalid()
    {
        // "NDOC" + a length claiming far more JSON than is actually present.
        byte[] img = MakeImage(0x7F, 1, 1);
        using var ms = new MemoryStream();
        ms.Write(img);
        ms.Write(Encoding.ASCII.GetBytes("NDOC"));
        ms.Write(new byte[] { 0xFF, 0xFF, 0x00, 0x00 });   // len = 65535
        ms.Write(Encoding.ASCII.GetBytes("{}"));           // but only 2 bytes
        NovaModule m = NovaModule.Parse(ms.ToArray());
        Assert.IsFalse(m.Valid);
        Assert.IsNotNull(m.Error);
    }

    [TestMethod]
    public void Function_Signature_RendersArgsAndReturn()
    {
        var fn = new NovaFn(0xA4, "$A4", "GFN_LINE", "draw",
            new[] { new NovaArg(0, "x0", "s16", "sx"), new NovaArg(1, "y0", "s16", "sy") },
            new NovaRet("void", ""), null, new[] { "LERR_OK" });
        Assert.AreEqual("GFN_LINE(x0:s16, y0:s16) -> void", fn.Signature());
    }

    [TestMethod]
    public void Function_Signature_NoArgs()
    {
        var fn = new NovaFn(0, "$00", "CLS", null,
            Array.Empty<NovaArg>(), new NovaRet("s32", ""), null, Array.Empty<string>());
        Assert.AreEqual("CLS() -> s32", fn.Signature());
    }

    // -- ValidateForStaging: the shared gate used by `nova module put`/`validate`
    //    and the web `PUT /api/modules/{name}` so the CLI and web agree. --

    [TestMethod]
    public void ValidateForStaging_ValidDocumentedModule_Ok()
    {
        // EchoJson documents one fn with id 0; fnCount 1 → 0 is in range [0, 1).
        NovaModule m = NovaModule.Parse(MakeNmod(MakeImage(0x7F, 1, 1), EchoJson));
        (bool ok, string? reason) = m.ValidateForStaging();
        Assert.IsTrue(ok, reason);
        Assert.IsNull(reason);
    }

    [TestMethod]
    public void ValidateForStaging_PlainMod_Ok()
    {
        NovaModule m = NovaModule.Parse(MakeImage(0x01, 1, 13));
        (bool ok, string? reason) = m.ValidateForStaging();
        Assert.IsTrue(ok, reason);
        Assert.IsNull(reason);
    }

    [TestMethod]
    public void ValidateForStaging_InvalidModule_FailsWithParseReason()
    {
        byte[] img = MakeImage(0x7F, 1, 1);
        img[3] = 0x00;                       // corrupt the "NL" magic
        NovaModule m = NovaModule.Parse(MakeNmod(img, EchoJson));
        (bool ok, string? reason) = m.ValidateForStaging();
        Assert.IsFalse(ok);
        Assert.AreEqual(m.Error, reason);
    }

    [TestMethod]
    public void ValidateForStaging_DocIdOutOfRange_Fails()
    {
        // fnCount 1, but the doc declares a function with id 5 → out of [0, 1).
        const string outOfRange =
            "{\"ndocVersion\":1,\"module\":{\"name\":\"TESTMOD\",\"id\":127," +
            "\"abiVersion\":1,\"version\":\"1.0\",\"brief\":\"b\",\"abiNote\":\"x\"}," +
            "\"functions\":[{\"id\":5,\"idHex\":\"$05\",\"name\":\"OOPS\",\"brief\":\"e\"," +
            "\"args\":[],\"ret\":{\"type\":\"void\",\"desc\":\"\"}}]}";
        NovaModule m = NovaModule.Parse(MakeNmod(MakeImage(0x7F, 1, 1), outOfRange));
        (bool ok, string? reason) = m.ValidateForStaging();
        Assert.IsFalse(ok);
        Assert.IsNotNull(reason);
        StringAssert.Contains(reason, "dispatch range");
    }
}
