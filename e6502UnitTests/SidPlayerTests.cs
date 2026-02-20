using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidPlayerTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    private static byte[] MakeTestSid(ushort loadAddr = 0x1000, ushort initAddr = 0x1000,
        ushort playAddr = 0x1003, bool ciaTiming = false)
    {
        var data = new byte[130];
        data[0] = (byte)'P'; data[1] = (byte)'S'; data[2] = (byte)'I'; data[3] = (byte)'D';
        data[4] = 0x00; data[5] = 0x02;
        data[6] = 0x00; data[7] = 0x7C;
        data[8]  = (byte)(loadAddr >> 8); data[9]  = (byte)(loadAddr & 0xFF);
        data[10] = (byte)(initAddr >> 8); data[11] = (byte)(initAddr & 0xFF);
        data[12] = (byte)(playAddr >> 8); data[13] = (byte)(playAddr & 0xFF);
        data[14] = 0x00; data[15] = 0x01;
        data[16] = 0x00; data[17] = 0x01;
        if (ciaTiming) data[21] = 0x01;
        data[124] = 0xA9; data[125] = 0x00; data[126] = 0x60;
        data[127] = 0xA9; data[128] = 0x00; data[129] = 0x60;
        return data;
    }

    private static SidFileInfo Parse(byte[] raw) => SidFileParser.Parse(raw);

    [TestMethod]
    public void Play_InjectsTrampoline_AtExpectedAddress()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info, song: 1);

        Assert.AreEqual(0x01, bus.Read(0x03D0), "init_flag should be 1");
        Assert.AreEqual(0x00, bus.Read(0x03D1), "song_num should be 0 (song-1)");
        Assert.AreEqual(0x48, bus.Read(0x03D2), "PHA at trampoline entry");
        Assert.AreEqual(0x40, bus.Read(0x03EF), "RTI at trampoline end");
    }

    [TestMethod]
    public void Play_PatchesIrqVector()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);

        Assert.AreEqual(0xD2, bus.Read(0xFFFE), "IRQ vector low = $D2");
        Assert.AreEqual(0x03, bus.Read(0xFFFF), "IRQ vector high = $03");
    }

    [TestMethod]
    public void Play_EnablesRasterIrq()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);

        byte irqCtrl = bus.Read((ushort)VgcConstants.RegIrqCtrl);
        Assert.AreEqual(0x01, irqCtrl & 0x01, "Raster IRQ should be enabled");
    }

    [TestMethod]
    public void Play_CopiesPayloadToRam()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid(loadAddr: 0x1000));

        player.Play(info);

        Assert.AreEqual(0xA9, bus.Read(0x1000), "First payload byte at load address");
    }

    [TestMethod]
    public void Stop_DisablesRasterIrq()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);
        player.Stop();

        byte irqCtrl = bus.Read((ushort)VgcConstants.RegIrqCtrl);
        Assert.AreEqual(0x00, irqCtrl & 0x01, "Raster IRQ should be disabled after Stop");
    }

    [TestMethod]
    public void Stop_GatesOffSidVoices()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);
        bus.Write(0xD404, 0x11);
        bus.Write(0xD40B, 0x11);
        bus.Write(0xD412, 0x11);

        player.Stop();

        Assert.AreEqual(0x00, bus.Sid.Read(0xD404), "Voice 1 gated off");
        Assert.AreEqual(0x00, bus.Sid.Read(0xD40B), "Voice 2 gated off");
        Assert.AreEqual(0x00, bus.Sid.Read(0xD412), "Voice 3 gated off");
        Assert.AreEqual(0x00, bus.Sid.Read(0xD418), "Volume zeroed");
    }

    [TestMethod]
    public void Stop_RestoresOriginalIrqState()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        // Raster IRQ starts disabled
        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegIrqCtrl) & 0x01);

        player.Play(info);
        Assert.AreEqual(0x01, bus.Read((ushort)VgcConstants.RegIrqCtrl) & 0x01);

        player.Stop();
        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegIrqCtrl) & 0x01);
    }
}
