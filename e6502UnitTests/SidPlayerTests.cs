using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SidPlayerTests
{
    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    /// <summary>
    /// Build a minimal but valid PSID v2 file in memory.
    /// Header is 124 bytes (dataOffset = 0x7C), payload starts immediately after.
    /// loadAddr/initAddr/playAddr are big-endian in the header.
    /// speed bit 0 set → CIA timing.
    /// </summary>
    private static byte[] MakeTestSid(ushort loadAddr = 0x1000, ushort initAddr = 0x1000,
        ushort playAddr = 0x1003, bool ciaTiming = false)
    {
        var data = new byte[130];

        // Magic "PSID"
        data[0] = (byte)'P'; data[1] = (byte)'S'; data[2] = (byte)'I'; data[3] = (byte)'D';

        // Version = 2 (big-endian)
        data[4] = 0x00; data[5] = 0x02;

        // Data offset = 0x007C = 124 (big-endian)
        data[6] = 0x00; data[7] = 0x7C;

        // Load address (big-endian)
        data[8]  = (byte)(loadAddr >> 8); data[9]  = (byte)(loadAddr & 0xFF);

        // Init address (big-endian)
        data[10] = (byte)(initAddr >> 8); data[11] = (byte)(initAddr & 0xFF);

        // Play address (big-endian)
        data[12] = (byte)(playAddr >> 8); data[13] = (byte)(playAddr & 0xFF);

        // Songs = 1
        data[14] = 0x00; data[15] = 0x01;

        // Start song = 1
        data[16] = 0x00; data[17] = 0x01;

        // Speed: bit 0 = CIA timing flag
        if (ciaTiming) data[21] = 0x01;

        // Minimal payload: init stub (LDA #0; RTS) + play stub (LDA #0; RTS)
        data[124] = 0xA9; data[125] = 0x00; data[126] = 0x60;  // init: LDA #0, RTS
        data[127] = 0xA9; data[128] = 0x00; data[129] = 0x60;  // play: LDA #0, RTS

        return data;
    }

    private static SidFileInfo Parse(byte[] raw) => SidFileParser.Parse(raw);

    // -------------------------------------------------------------------------
    // Tests
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Play_InjectsTrampoline_AtExpectedAddress()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid(loadAddr: 0x1000, initAddr: 0x1000, playAddr: 0x1003));

        player.Play(info, song: 1);

        Assert.AreEqual(0x01, bus.Read(0x03D0), "init_flag should be 1 at $03D0");
        Assert.AreEqual(0x00, bus.Read(0x03D1), "song_num should be 0 (song-1) at $03D1");
        Assert.AreEqual(0x48, bus.Read(0x03D2), "PHA opcode at $03D2 (trampoline entry)");
        Assert.AreEqual(0x40, bus.Read(0x03F2), "RTI opcode at $03F2 (trampoline end)");
    }

    [TestMethod]
    public void Play_PatchesIrqVector()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);

        Assert.AreEqual(0xD2, bus.Read(0xFFFE), "$FFFE should be low byte of $03D2");
        Assert.AreEqual(0x03, bus.Read(0xFFFF), "$FFFF should be high byte of $03D2");
    }

    [TestMethod]
    public void Play_EnablesTimer()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);

        Assert.AreEqual(0x01, bus.Read((ushort)VgcConstants.TimerCtrl), "Timer ctrl bit 0 should be set");
    }

    [TestMethod]
    public void Play_CopiesPayloadToRam()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid(loadAddr: 0x1000));

        player.Play(info);

        // Payload starts at offset 124 in the raw SID; first byte is 0xA9 (LDA #)
        Assert.AreEqual(0xA9, bus.Read(0x1000), "First payload byte should be at load address");
    }

    [TestMethod]
    public void Stop_DisablesTimer()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);
        player.Stop();

        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.TimerCtrl), "Timer ctrl should be 0 after Stop");
    }

    [TestMethod]
    public void Stop_GatesOffSidVoices()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);
        player.Stop();

        // SID reads are not routed through CompositeBusDevice.Read, so verify
        // via the SID chip's own Read(), which returns the internal register array.
        Assert.AreEqual(0x00, bus.Sid.Read(0xD404), "Voice 1 control should be gated off");
        Assert.AreEqual(0x00, bus.Sid.Read(0xD40B), "Voice 2 control should be gated off");
        Assert.AreEqual(0x00, bus.Sid.Read(0xD412), "Voice 3 control should be gated off");
        Assert.AreEqual(0x00, bus.Sid.Read(0xD418), "Volume register should be zeroed");
    }

    [TestMethod]
    public void Play_SetsTimerDivisor_VBI()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid(ciaTiming: false));

        player.Play(info);

        int divisor = bus.Read((ushort)VgcConstants.TimerDivL) |
                      (bus.Read((ushort)VgcConstants.TimerDivH) << 8);
        Assert.AreEqual(200, divisor, "VBI timing should use divisor 200");
    }

    [TestMethod]
    public void Play_SetsTimerDivisor_CIA()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid(ciaTiming: true));

        player.Play(info);

        int divisor = bus.Read((ushort)VgcConstants.TimerDivL) |
                      (bus.Read((ushort)VgcConstants.TimerDivH) << 8);
        Assert.AreEqual(167, divisor, "CIA timing should use divisor 167");
    }
}
