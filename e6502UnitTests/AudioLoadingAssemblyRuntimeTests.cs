using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AudioLoadingAssemblyRuntimeTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort Result = 0x2200;

    private const ushort FioCmd = 0xB9A0;
    private const ushort FioStatus = 0xB9A1;
    private const ushort MusicStatus = 0xBA50;
    private const byte FioOk = 0x02;
    private const byte StatusLoading = 0x10;

    // Bus double: the FIO host accepts any command (latches FIO_STATUS=OK) and a
    // file-backed play then reports MUSIC_STATUS_LOADING for the first 3 reads
    // before clearing, modelling an asset still streaming in on the host.
    private sealed class AudioLoadingBus : IBusDevice
    {
        private readonly byte[] _ram = new byte[65536];
        private int _loadingReads;
        public int MusicStatusReads { get; private set; }

        public AudioLoadingBus(byte[] image, ushort loadAddr)
        {
            Array.Copy(image, 0, _ram, loadAddr, image.Length);
        }

        public byte Read(ushort address)
        {
            if (address == MusicStatus)
            {
                MusicStatusReads++;
                if (_loadingReads > 0)
                {
                    _loadingReads--;
                    return StatusLoading;
                }
                return 0;
            }
            return _ram[address];
        }

        public void Write(ushort address, byte value)
        {
            if (address == FioCmd)
            {
                _ram[FioStatus] = FioOk;   // host accepts + completes the command
                _loadingReads = 3;          // ...but the asset is still loading
                return;
            }
            _ram[address] = value;
        }
    }

    [TestMethod]
    public void FileBackedPlayWaitsForAssetLoad()
    {
        byte[] fixture = File.ReadAllBytes(
            RepoPath("software", "tests", "integration", "fixtures", "audioload_runtime.bin"));
        var bus = new AudioLoadingBus(fixture, Entry);

        bus.Write(Harness, 0xA2);
        bus.Write(Harness + 1, 0xFF);
        bus.Write(Harness + 2, 0x9A);
        bus.Write(Harness + 3, 0x20);
        bus.Write(Harness + 4, (byte)(Entry & 0xFF));
        bus.Write(Harness + 5, (byte)(Entry >> 8));
        bus.Write(Harness + 6, 0x4C);
        bus.Write(Harness + 7, (byte)(Done & 0xFF));
        bus.Write(Harness + 8, (byte)(Done >> 8));

        var cpu = new Cpu(bus, E6502Type.Cmos);
        cpu.Boot(Harness);
        for (int i = 0; i < 500_000; i++)
        {
            ushort prevPc = cpu.Pc;
            cpu.ExecuteNext();
            if (prevPc == Done && cpu.Pc == Done)
                break;
        }

        Assert.AreEqual(0xAA, bus.Read(Result + 1), "audioload fixture ran to completion");
        Assert.AreEqual(0x00, bus.Read(Result + 0), "play reports success");
        // The bug: audio_sidplay returns the instant the command dispatches,
        // never reading MUSIC_STATUS, so the caller proceeds while the asset is
        // still loading. The fix polls the LOADING bit until it clears.
        Assert.IsTrue(bus.MusicStatusReads >= 3,
            $"play must poll MUSIC_STATUS until LOADING clears (saw {bus.MusicStatusReads} reads)");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}
