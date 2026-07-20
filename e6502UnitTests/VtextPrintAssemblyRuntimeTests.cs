using System;
using System.IO;
using System.Linq;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class VtextPrintAssemblyRuntimeTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort Result = 0x2200;
    private const byte PlaneChar = 0x01;

    // Bus double modelling the VGC VRAM plane interface ($A0E0-$A0E4): writes to
    // VGC_VRAM_DATA land in the selected plane at the current VRAM address (with
    // optional autoincrement). Lets a flat-CPU test read back what vtext drew.
    private sealed class VgcVramBus : IBusDevice
    {
        private const ushort Plane = 0xA0E0, AddrL = 0xA0E1, AddrH = 0xA0E2, Data = 0xA0E3, Ctrl = 0xA0E4;

        private readonly byte[] _ram = new byte[65536];
        private readonly byte[][] _planes = new byte[8][];
        private byte _plane;
        private ushort _addr;
        private bool _autoinc;

        public VgcVramBus(byte[] image, ushort loadAddr)
        {
            Array.Copy(image, 0, _ram, loadAddr, image.Length);
            for (int i = 0; i < _planes.Length; i++)
                _planes[i] = new byte[4096];
        }

        public byte Cell(byte plane, int offset) => _planes[plane][offset];

        public byte Read(ushort address)
        {
            if (address == Data)
            {
                byte v = _planes[_plane][_addr];
                if (_autoinc) _addr++;
                return v;
            }
            return _ram[address];
        }

        public void Write(ushort address, byte value)
        {
            switch (address)
            {
                case Plane: _plane = (byte)(value & 7); break;
                case AddrL: _addr = (ushort)((_addr & 0xFF00) | value); break;
                case AddrH: _addr = (ushort)((_addr & 0x00FF) | (value << 8)); break;
                case Ctrl: _autoinc = (value & 1) != 0; break;
                case Data:
                    _planes[_plane][_addr] = value;
                    if (_autoinc) _addr++;
                    break;
                default: _ram[address] = value; break;
            }
        }
    }

    [TestMethod]
    public void InlineParametersAdvanceReturnAndPopulateTextAndFio()
    {
        byte[] fixture = File.ReadAllBytes(
            RepoPath("software", "tests", "integration", "fixtures", "vtextprint_runtime.bin"));
        var bus = new VgcVramBus(fixture, Entry);

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

        Assert.AreEqual(0xAA, bus.Read(Result + 3), "all inline data was skipped on return");
        Assert.AreEqual(0x00, bus.Read(Result + 0), "i_vtext_print_at reports success");
        Assert.AreEqual(0x00, bus.Read(Result + 1), "i_vtext_puts reports success");
        Assert.AreEqual(0x00, bus.Read(Result + 2), "i_fio_name reports success");

        // "HI" printed at col 3, row 2 on an 80-wide region -> cells 163, 164.
        int cell = 2 * 80 + 3;
        Assert.AreEqual((byte)'H', bus.Cell(PlaneChar, cell), "first char at the cursor cell");
        Assert.AreEqual((byte)'I', bus.Cell(PlaneChar, cell + 1), "second char in the next cell");
        Assert.AreEqual((byte)'!', bus.Cell(PlaneChar, cell + 2), "inline puts continues at the cursor");

        Assert.AreEqual(8, bus.Read(0xB9A3), "FIO filename length");
        CollectionAssert.AreEqual(
            "CHESSENG"u8.ToArray(),
            Enumerable.Range(0, 8).Select(i => bus.Read((ushort)(0xB9B0 + i))).ToArray(),
            "FIO filename bytes");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}
