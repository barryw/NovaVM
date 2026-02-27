using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RomSwapTests
{
    [TestMethod]
    public void InitialRom_IsBasic()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Basic, bus.CurrentRom);
    }

    [TestMethod]
    public void WriteRomSwapNcc_SetsNccRom()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Ncc, bus.CurrentRom);
    }

    [TestMethod]
    public void WriteRomSwapBasic_SetsBasicRom()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapBasic);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Basic, bus.CurrentRom);
    }

    [TestMethod]
    public void RomSwap_FiresEvent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bool fired = false;
        bus.RomSwapRequested += (_, _) => fired = true;
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        Assert.IsTrue(fired);
    }

    [TestMethod]
    public void RomSwap_ChangesRomContent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        byte basicByte = bus.Read(0xC000);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        byte nccByte = bus.Read(0xC000);
        // NCC stub ROM has JMP $C000 = $4C at $C000
        // BASIC ROM has different content at $C000
        Assert.AreNotEqual(basicByte, nccByte);
    }

    [TestMethod]
    public void WriteInvalidSwapValue_NoOp()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bool fired = false;
        bus.RomSwapRequested += (_, _) => fired = true;
        bus.Write(VgcConstants.RegRomSwap, 0xFF);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Basic, bus.CurrentRom);
        Assert.IsFalse(fired);
    }

    [TestMethod]
    public void SwapToSameRom_NoEvent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bool fired = false;
        bus.RomSwapRequested += (_, _) => fired = true;
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapBasic); // already Basic
        Assert.IsFalse(fired);
    }

    [TestMethod]
    public void RomSwapBack_RestoresOriginalContent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        byte originalByte = bus.Read(0xC000);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapNcc);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapBasic);
        Assert.AreEqual(originalByte, bus.Read(0xC000));
    }
}
