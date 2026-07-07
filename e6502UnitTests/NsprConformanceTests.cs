using System;
using System.IO;
using System.Linq;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Cross-implementation conformance for the NSPR sprite-bank format. Every codec
/// (this C# one, NovaDraw's Swift one, the on-device 6502 loader) must agree on
/// the exact bytes of the checked-in golden fixture testdata/nspr/sample.nsp:
/// decoding it yields the canonical bank, and re-encoding the canonical bank
/// reproduces the fixture byte-for-byte. If either side drifts, this test (or the
/// Swift twin, NsprConformanceTests.swift) fails before merge. See
/// docs/sprite-editor/NSPR.md for the human-readable spec + the canonical sample.
/// </summary>
[TestClass]
public class NsprConformanceTests
{
    private static string RepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "e6502.sln"))) return dir;
            dir = Directory.GetParent(dir)?.FullName;
        }
        throw new Exception("repo root (e6502.sln) not found");
    }

    private static string FixturePath() =>
        Path.Combine(RepoRoot(), "testdata", "nspr", "sample.nsp");

    /// <summary>
    /// The canonical conformance sample, spelled out in docs/sprite-editor/NSPR.md.
    /// This is the single authoritative structure both sides assert against.
    /// </summary>
    public static NsprBank CanonicalBank()
    {
        var bank = new NsprBank { Version = 1, Kind = NsprKind.HwMetasprite };
        bank.Palette = Enumerable.Range(0, NsprBank.PaletteBytes).Select(i => (byte)i).ToArray();
        for (byte i = 0; i < 5; i++)
        {
            var shape = new byte[NsprBank.ShapeBytes];
            Array.Fill(shape, (byte)(0x10 + i));
            bank.Shapes.Add(shape);
        }

        var hero = new NsprCharacter { Name = "HERO" };
        hero.Parts.Add(new NsprPart { Dx = 0, Dy = 0, Flags = 0 });
        var idle = new NsprAnimation { Name = "IDLE", Ticks = 8, Flags = NsprAnimFlags.Loop };
        idle.Frames.Add(new byte[] { 0 });
        idle.Frames.Add(new byte[] { 1 });
        idle.Frames.Add(new byte[] { 2 });
        hero.Animations.Add(idle);
        bank.Characters.Add(hero);

        var boss = new NsprCharacter { Name = "BOSS" };
        boss.Parts.Add(new NsprPart { Dx = 0, Dy = 0, Flags = 0 });
        boss.Parts.Add(new NsprPart { Dx = 16, Dy = 0, Flags = 0 });
        boss.Parts.Add(new NsprPart { Dx = 0, Dy = 16, Flags = 0 });
        boss.Parts.Add(new NsprPart { Dx = 16, Dy = 16, Flags = 1 });
        var walk = new NsprAnimation { Name = "WALK", Ticks = 4, Flags = NsprAnimFlags.Loop | NsprAnimFlags.PingPong };
        walk.Frames.Add(new byte[] { 0, 1, 2, 3 });
        walk.Frames.Add(new byte[] { 4, 3, 2, 1 });
        boss.Animations.Add(walk);
        bank.Characters.Add(boss);
        return bank;
    }

    /// <summary>
    /// Regenerate the golden fixture from the canonical bank. Guarded so a normal
    /// test run never writes the repo: set NSPR_GEN=1 to (re)write it, then commit.
    /// </summary>
    [TestMethod]
    public void GenerateGoldenFixture()
    {
        if (Environment.GetEnvironmentVariable("NSPR_GEN") != "1")
        {
            Assert.Inconclusive("set NSPR_GEN=1 to (re)write testdata/nspr/sample.nsp");
            return;
        }
        string path = FixturePath();
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllBytes(path, CanonicalBank().ToBytes());
    }

    [TestMethod]
    public void GoldenFixtureDecodesToCanonicalBank()
    {
        byte[] fixture = File.ReadAllBytes(FixturePath());
        NsprBank decoded = NsprBank.FromBytes(fixture);

        NsprBank canonical = CanonicalBank();
        Assert.AreEqual(canonical.Shapes.Count, decoded.Shapes.Count, "shape count");
        Assert.AreEqual(canonical.Characters.Count, decoded.Characters.Count, "character count");
        Assert.AreEqual("HERO", decoded.Characters[0].Name);
        Assert.AreEqual(1, decoded.Characters[0].Parts.Count);
        Assert.AreEqual("IDLE", decoded.Characters[0].Animations[0].Name);
        Assert.AreEqual(3, decoded.Characters[0].Animations[0].Frames.Count);
        Assert.AreEqual("BOSS", decoded.Characters[1].Name);
        Assert.AreEqual(4, decoded.Characters[1].Parts.Count);
        Assert.AreEqual((byte)16, decoded.Characters[1].Parts[1].Dx);
        NsprAnimation walk = decoded.Characters[1].Animations[0];
        Assert.AreEqual(NsprAnimFlags.Loop | NsprAnimFlags.PingPong, walk.Flags);
        CollectionAssert.AreEqual(new byte[] { 4, 3, 2, 1 }, walk.Frames[1]);
    }

    [TestMethod]
    public void CanonicalBankReencodesByteIdenticalToFixture()
    {
        byte[] fixture = File.ReadAllBytes(FixturePath());
        byte[] reencoded = CanonicalBank().ToBytes();
        CollectionAssert.AreEqual(fixture, reencoded,
            "the canonical bank must re-encode to the exact golden-fixture bytes — a mismatch means the format drifted");
    }
}
