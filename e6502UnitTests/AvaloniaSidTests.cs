using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaSidTests
{
    private const ushort SidBase = 0xD400;

    [TestMethod]
    public void Sid_WriteRegister_ReadBack()
    {
        var sid = new SidChip(enableAudio: false);
        sid.Write(SidBase + 0x00, 0x37);  // voice 1 freq lo
        sid.Write(SidBase + 0x01, 0x1C);  // voice 1 freq hi
        Assert.AreEqual(0x37, sid.Read(SidBase + 0x00));
        Assert.AreEqual(0x1C, sid.Read(SidBase + 0x01));
    }

    [TestMethod]
    public void Sid_OwnsAddress_InRange()
    {
        var sid = new SidChip(enableAudio: false);
        Assert.IsTrue(sid.OwnsAddress(SidBase));
        Assert.IsTrue(sid.OwnsAddress(SidBase + 0x1C));
        Assert.IsFalse(sid.OwnsAddress(SidBase - 1));
        Assert.IsFalse(sid.OwnsAddress(SidBase + 0x1D));
    }

    [TestMethod]
    public void Sid_VoiceControl_GateOn_DoesNotCrash()
    {
        var sid = new SidChip(enableAudio: false);
        sid.Write(SidBase + 0x00, 0x37);
        sid.Write(SidBase + 0x01, 0x1C);
        sid.Write(SidBase + 0x05, 0x00);
        sid.Write(SidBase + 0x06, 0xF0);
        sid.Write(SidBase + 0x04, 0x11);  // gate on + triangle
        sid.Clock(1000);
        // No crash = pass
    }

    [TestMethod]
    public void Sid_MasterVolume_WrittenToRegister()
    {
        var sid = new SidChip(enableAudio: false);
        sid.Write(SidBase + 0x18, 0x0F);
        Assert.AreEqual(0x0F, sid.Read(SidBase + 0x18));
    }

    [TestMethod]
    public void Sid_RenderSamples_ProducesNonSilentOutput()
    {
        var sid = new SidChip(enableAudio: false);
        // Set up voice 1: triangle wave, 440Hz, gate on, max volume
        sid.Write(SidBase + 0x00, 0x37);  // freq lo
        sid.Write(SidBase + 0x01, 0x1C);  // freq hi
        sid.Write(SidBase + 0x05, 0x00);  // AD: instant attack, instant decay
        sid.Write(SidBase + 0x06, 0xF0);  // SR: max sustain, instant release
        sid.Write(SidBase + 0x04, 0x11);  // gate on + triangle
        sid.Write(SidBase + 0x18, 0x0F);  // max volume

        // Clock enough to get through attack
        sid.Clock(50000);

        // Now render samples — should have non-zero audio
        var samples = sid.RenderSamplesForTest(1024);
        bool hasNonZero = false;
        foreach (var s in samples)
            if (s != 0) { hasNonZero = true; break; }
        Assert.IsTrue(hasNonZero, "Expected non-silent output from SID");
    }

    // -------------------------------------------------------------------------
    // SID2 at $D420
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Sid2_OwnsAddress_AtD420()
    {
        var sid2 = new SidChip(enableAudio: false, baseAddress: 0xD420);
        Assert.AreEqual((ushort)0xD420, sid2.BaseAddress);
        Assert.AreEqual((ushort)0xD43C, sid2.EndAddress);
        Assert.IsTrue(sid2.OwnsAddress(0xD420));
        Assert.IsTrue(sid2.OwnsAddress(0xD43C));
        Assert.IsFalse(sid2.OwnsAddress(0xD41F));
        Assert.IsFalse(sid2.OwnsAddress(0xD43D));
        // Should NOT own SID1 range
        Assert.IsFalse(sid2.OwnsAddress(0xD400));
    }

    [TestMethod]
    public void Sid2_WriteRegister_ReadBack()
    {
        var sid2 = new SidChip(enableAudio: false, baseAddress: 0xD420);
        sid2.Write(0xD420, 0xAB);
        sid2.Write(0xD421, 0xCD);
        Assert.AreEqual(0xAB, sid2.Read(0xD420));
        Assert.AreEqual(0xCD, sid2.Read(0xD421));
    }

    [TestMethod]
    public void Sid1_And_Sid2_Independent()
    {
        var sid1 = new SidChip(enableAudio: false);
        var sid2 = new SidChip(enableAudio: false, baseAddress: 0xD420);
        sid1.Write(0xD400, 0x11);
        sid2.Write(0xD420, 0x22);
        Assert.AreEqual(0x11, sid1.Read(0xD400));
        Assert.AreEqual(0x22, sid2.Read(0xD420));
    }

    [TestMethod]
    public void CompositeBus_RoutesSid2Writes()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(0xD420, 0x37);
        bus.Write(0xD421, 0x1C);
        Assert.AreEqual(0x37, bus.Sid2.Read(0xD420));
        Assert.AreEqual(0x1C, bus.Sid2.Read(0xD421));
        // SID1 should be unaffected
        Assert.AreEqual(0x00, bus.Sid.Read(0xD400));
    }

    [TestMethod]
    public void CompositeBus_MirrorD500_RoutesToSid2()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        // Writes to $D500 should transparently route to SID2 at $D420
        bus.Write(0xD500, 0xAA);
        bus.Write(0xD501, 0xBB);
        Assert.AreEqual(0xAA, bus.Sid2.Read(0xD420));
        Assert.AreEqual(0xBB, bus.Sid2.Read(0xD421));
    }
}
