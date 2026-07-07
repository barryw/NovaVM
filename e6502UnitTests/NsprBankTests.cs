using System;
using System.IO;
using System.Linq;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NsprBankTests
{
    private static byte[] Shape(byte fill)
    {
        var s = new byte[NsprBank.ShapeBytes];
        Array.Fill(s, fill);
        return s;
    }

    private static NsprBank SampleBank()
    {
        var bank = new NsprBank { Version = 1, Kind = NsprKind.HwMetasprite };
        bank.Palette = Enumerable.Range(0, NsprBank.PaletteBytes).Select(i => (byte)i).ToArray();
        for (byte i = 0; i < 5; i++) bank.Shapes.Add(Shape((byte)(0x10 + i)));

        // Character 0: a plain 1-part (16x16) sprite with a 3-frame looping idle.
        var hero = new NsprCharacter { Name = "HERO" };
        hero.Parts.Add(new NsprPart { Dx = 0, Dy = 0, Flags = 0 });
        var idle = new NsprAnimation { Name = "IDLE", Ticks = 8, Flags = NsprAnimFlags.Loop };
        idle.Frames.Add(new byte[] { 0 });
        idle.Frames.Add(new byte[] { 1 });
        idle.Frames.Add(new byte[] { 2 });
        hero.Animations.Add(idle);
        bank.Characters.Add(hero);

        // Character 1: a 2x2 (4-part, 32x32) metasprite with a 2-frame ping-pong walk.
        var boss = new NsprCharacter { Name = "BOSS" };
        boss.Parts.Add(new NsprPart { Dx = 0, Dy = 0, Flags = 0 });
        boss.Parts.Add(new NsprPart { Dx = 16, Dy = 0, Flags = 0 });
        boss.Parts.Add(new NsprPart { Dx = 0, Dy = 16, Flags = 0 });
        boss.Parts.Add(new NsprPart { Dx = 16, Dy = 16, Flags = 1 });
        var walk = new NsprAnimation
        {
            Name = "WALK",
            Ticks = 4,
            Flags = NsprAnimFlags.Loop | NsprAnimFlags.PingPong
        };
        walk.Frames.Add(new byte[] { 0, 1, 2, 3 });
        walk.Frames.Add(new byte[] { 4, 3, 2, 1 });
        boss.Animations.Add(walk);
        bank.Characters.Add(boss);
        return bank;
    }

    [TestMethod]
    public void NsprBank_RoundTrips()
    {
        NsprBank a = SampleBank();
        NsprBank b = NsprBank.FromBytes(a.ToBytes());

        Assert.AreEqual(a.Version, b.Version);
        Assert.AreEqual(a.Kind, b.Kind);
        CollectionAssert.AreEqual(a.Palette, b.Palette);
        Assert.AreEqual(a.Shapes.Count, b.Shapes.Count);
        for (int i = 0; i < a.Shapes.Count; i++) CollectionAssert.AreEqual(a.Shapes[i], b.Shapes[i]);

        Assert.AreEqual(a.Characters.Count, b.Characters.Count);
        for (int c = 0; c < a.Characters.Count; c++)
        {
            NsprCharacter ca = a.Characters[c], cb = b.Characters[c];
            Assert.AreEqual(ca.Name, cb.Name);
            Assert.AreEqual(ca.Parts.Count, cb.Parts.Count);
            for (int p = 0; p < ca.Parts.Count; p++)
            {
                Assert.AreEqual(ca.Parts[p].Dx, cb.Parts[p].Dx);
                Assert.AreEqual(ca.Parts[p].Dy, cb.Parts[p].Dy);
                Assert.AreEqual(ca.Parts[p].Flags, cb.Parts[p].Flags);
            }
            Assert.AreEqual(ca.Animations.Count, cb.Animations.Count);
            for (int an = 0; an < ca.Animations.Count; an++)
            {
                NsprAnimation aa = ca.Animations[an], ab = cb.Animations[an];
                Assert.AreEqual(aa.Name, ab.Name);
                Assert.AreEqual(aa.Ticks, ab.Ticks);
                Assert.AreEqual(aa.Flags, ab.Flags);
                Assert.AreEqual(aa.Frames.Count, ab.Frames.Count);
                for (int f = 0; f < aa.Frames.Count; f++) CollectionAssert.AreEqual(aa.Frames[f], ab.Frames[f]);
            }
        }
    }

    [TestMethod]
    public void NsprBank_HeaderByteLayout()
    {
        byte[] b = SampleBank().ToBytes();
        Assert.AreEqual((byte)'N', b[0]);
        Assert.AreEqual((byte)'S', b[1]);
        Assert.AreEqual((byte)'P', b[2]);
        Assert.AreEqual((byte)'R', b[3]);
        Assert.AreEqual(1, b[4]);            // version
        Assert.AreEqual(0, b[5]);            // kind = hw metasprite
        Assert.AreEqual(0x01, b[6] & 0x01);  // palette present
        Assert.AreEqual(5, b[7]);            // shape count
        Assert.AreEqual(2, b[8]);            // character count
        Assert.AreEqual(16, b[9]);           // cell w
        Assert.AreEqual(16, b[10]);          // cell h
    }

    [TestMethod]
    [ExpectedException(typeof(InvalidDataException))]
    public void NsprBank_RejectsBadMagic()
    {
        byte[] b = SampleBank().ToBytes();
        b[0] = (byte)'X';
        NsprBank.FromBytes(b);
    }

    [TestMethod]
    [ExpectedException(typeof(InvalidDataException))]
    public void NsprBank_ValidatesFramePartCount()
    {
        var bank = new NsprBank();
        bank.Shapes.Add(new byte[NsprBank.ShapeBytes]);
        var ch = new NsprCharacter { Name = "X" };
        ch.Parts.Add(new NsprPart());
        ch.Parts.Add(new NsprPart());          // 2 parts
        var an = new NsprAnimation { Name = "A" };
        an.Frames.Add(new byte[] { 0 });       // only 1 shape index for 2 parts -> invalid
        ch.Animations.Add(an);
        bank.Characters.Add(ch);
        bank.ToBytes();                        // Validate() throws
    }
}
