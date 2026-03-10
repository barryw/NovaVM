using e6502.Avalonia.Hardware;
using KDS.e6502;
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
        // init: LDA #$00 / RTS
        data[124] = 0xA9; data[125] = 0x00; data[126] = 0x60;
        // play: LDA #$00 / RTS
        data[127] = 0xA9; data[128] = 0x00; data[129] = 0x60;
        return data;
    }

    private static SidFileInfo Parse(byte[] raw) => SidFileParser.Parse(raw);

    [TestMethod]
    public void Play_WritesTrapAtExpectedAddress()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info, song: 1);

        // Trap: JMP $03D0
        Assert.AreEqual(0x4C, bus.ReadRam(0x03D0), "JMP opcode at trap");
        Assert.AreEqual(0xD0, bus.ReadRam(0x03D1), "trap low byte");
        Assert.AreEqual(0x03, bus.ReadRam(0x03D2), "trap high byte");
    }

    [TestMethod]
    public void Play_HasPendingInit()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);

        Assert.IsTrue(player.HasPendingCall, "Should have pending init call");
        Assert.IsTrue(player.IsPlaying, "Should be playing");
    }

    [TestMethod]
    public void Play_DoesNotModifyIrqVector()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        byte origLo = bus.ReadRam(0xFFFE);
        byte origHi = bus.ReadRam(0xFFFF);
        var info = Parse(MakeTestSid());

        player.Play(info);

        Assert.AreEqual(origLo, bus.ReadRam(0xFFFE), "IRQ vector low should be unchanged");
        Assert.AreEqual(origHi, bus.ReadRam(0xFFFF), "IRQ vector high should be unchanged");
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
    public void ExecutePendingCalls_RunsInitAndPlay()
    {
        using var bus = MakeBus();
        var cpu = new Cpu(bus, E6502Type.Cmos);
        var player = new SidPlayer(bus);

        // Write init routine at $1000: STA $D418 / RTS  (sets SID volume)
        bus.WriteRam(0x1000, 0x8D); // STA abs
        bus.WriteRam(0x1001, 0x18);
        bus.WriteRam(0x1002, 0xD4);
        bus.WriteRam(0x1003, 0x60); // RTS

        // Write play routine at $1004: LDA #$42 / STA $D400 / RTS
        bus.WriteRam(0x1004, 0xA9); // LDA #$42
        bus.WriteRam(0x1005, 0x42);
        bus.WriteRam(0x1006, 0x8D); // STA $D400
        bus.WriteRam(0x1007, 0x00);
        bus.WriteRam(0x1008, 0xD4);
        bus.WriteRam(0x1009, 0x60); // RTS

        var info = Parse(MakeTestSid(loadAddr: 0x2000, initAddr: 0x1000, playAddr: 0x1004));
        player.Play(info);

        // Save host CPU state
        cpu.RestoreState(new CpuState(A: 0x77, X: 0x88, Y: 0x99, Sp: 0xFB, Pc: 0x4000,
            Nf: false, Vf: false, Df: false, If: false, Zf: false, Cf: true));

        // Run init
        player.ExecutePendingCalls(cpu);

        // Queue and run a play frame
        player.SetPlayPending();
        player.ExecutePendingCalls(cpu);

        // Host CPU state should be fully restored
        var state = cpu.GetState();
        Assert.AreEqual((ushort)0x4000, state.Pc, "Host PC restored");
        Assert.AreEqual((byte)0x77, state.A, "Host A restored");
        Assert.AreEqual((byte)0x88, state.X, "Host X restored");
        Assert.AreEqual((byte)0x99, state.Y, "Host Y restored");
        Assert.AreEqual((byte)0xFB, state.Sp, "Host SP restored");
        Assert.IsTrue(state.Cf, "Host carry flag restored");

        // SID register should have been written by the play routine
        Assert.AreEqual(0x42, bus.Sid.Read(0xD400), "Play routine wrote to SID");
    }

    [TestMethod]
    public void ExecutePendingCalls_PreservesHostZeroPage()
    {
        using var bus = MakeBus();
        var cpu = new Cpu(bus, E6502Type.Cmos);
        var player = new SidPlayer(bus);

        // Play routine: STA $10 / RTS (writes A to zero page $10)
        bus.WriteRam(0x1003, 0xA9); // LDA #$FF
        bus.WriteRam(0x1004, 0xFF);
        bus.WriteRam(0x1005, 0x85); // STA $10
        bus.WriteRam(0x1006, 0x10);
        bus.WriteRam(0x1007, 0x60); // RTS

        var info = Parse(MakeTestSid(playAddr: 0x1003));
        player.Play(info);
        // Consume the init
        player.ExecutePendingCalls(cpu);

        // Set host zero page value
        bus.WriteRam(0x10, 0x42);

        // Queue and execute play
        player.SetPlayPending();
        player.ExecutePendingCalls(cpu);

        // Host zero page should be restored even though play wrote to $10
        Assert.AreEqual(0x42, bus.ReadRam(0x10), "Host zero page $10 should be preserved");
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
        Assert.AreEqual(0x00, bus.Sid2.Read(0xD424), "SID2 Voice 1 gated off");
        Assert.AreEqual(0x00, bus.Sid2.Read(0xD42B), "SID2 Voice 2 gated off");
        Assert.AreEqual(0x00, bus.Sid2.Read(0xD432), "SID2 Voice 3 gated off");
        Assert.AreEqual(0x00, bus.Sid2.Read(0xD438), "SID2 Volume zeroed");
    }

    [TestMethod]
    public void Stop_ClearsPlayingState()
    {
        using var bus = MakeBus();
        var player = new SidPlayer(bus);
        var info = Parse(MakeTestSid());

        player.Play(info);
        Assert.IsTrue(player.IsPlaying);

        player.Stop();
        Assert.IsFalse(player.IsPlaying);
        Assert.IsFalse(player.HasPendingCall);
    }
}
