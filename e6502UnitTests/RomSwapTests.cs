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
    public void WriteRomSwapPrimary_SelectsPrimaryRuntimeRom()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapPrimary);
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

    [TestMethod]
    public void WriteRomSwapExtension_SetsExtensionRom()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        Assert.AreEqual(CompositeBusDevice.ActiveRom.Extension, bus.CurrentRom);
    }

    [TestMethod]
    public void ExtensionRom_HasEntryPoint()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        // Extension ROM entry at $C000: LDA $E4 = opcode $A5 $E4
        Assert.AreEqual(0xA5, bus.Read(0xC000));
        Assert.AreEqual(0xE4, bus.Read(0xC001));
    }

    [TestMethod]
    public void ExtensionRom_SwapBackRestoresBasic()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        byte basicByte = bus.Read(0xC000);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        Assert.AreNotEqual(basicByte, bus.Read(0xC000));
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapBasic);
        Assert.AreEqual(basicByte, bus.Read(0xC000));
    }

    [TestMethod]
    public void ExtensionSwap_DoesNotFireEvent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        int count = 0;
        bus.RomSwapRequested += (_, _) => count++;
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapBasic);
        Assert.AreEqual(0, count, "Extension ROM round-trip should not fire RomSwapRequested");
    }

    [TestMethod]
    public void ExtensionRom_VectorsPointToRamHandlers()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        // NMI vector at $FFFA should point to $0217 (RAM NMI handler)
        Assert.AreEqual(0x17, bus.Read(0xFFFA));
        Assert.AreEqual(0x02, bus.Read(0xFFFB));
        // IRQ vector at $FFFE should point to $020D (RAM IRQ handler)
        Assert.AreEqual(0x0D, bus.Read(0xFFFE));
        Assert.AreEqual(0x02, bus.Read(0xFFFF));
    }
}
