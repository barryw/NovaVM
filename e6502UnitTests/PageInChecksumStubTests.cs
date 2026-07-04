using System;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    /// <summary>
    /// Proves the Phase-3 Step-3a page-in checksum stub (tools/page_in_checksum_stub.s)
    /// reads the bank-1 (ext_rom) window correctly while ROMSWAP is flipped to the
    /// extension bank, and restores ROMSWAP afterwards. The stub runs from RAM (loaded
    /// at $9F80) because reading $C000-$FFFF unmaps the running ROM; this test runs the
    /// real e6502 CPU over the stub against a bus that models only the $A03F ROMSWAP
    /// contract — the same contract the FPGA enforces. The page_dma copy itself is
    /// Verilator-proven (Phase 2); here we prove the 6502-side readback contract.
    /// </summary>
    [TestClass]
    public class PageInChecksumStubTests
    {
        // Assembled from tools/page_in_checksum_stub.s (ca65). Regenerate with:
        //   ca65 tools/page_in_checksum_stub.s -o /tmp/s.o && ld65 -C <cfg> /tmp/s.o -o /tmp/s.bin
        // Hardware checks use identical bytes. This test is the correctness guard.
        private static readonly byte[] StubBytes =
        {
            0x78, 0xA5, 0xFB, 0x8D, 0xE2, 0x9F, 0xA5, 0xFC, 0x8D, 0xE3, 0x9F, 0xA9,
            0x00, 0x8D, 0xE0, 0x9F, 0x8D, 0xE1, 0x9F, 0x85, 0xFB, 0xA9, 0xC0, 0x85,
            0xFC, 0xA9, 0x04, 0x8D, 0x3F, 0xA0, 0xA2, 0x40, 0xA0, 0x00, 0xB1, 0xFB,
            0x18, 0x6D, 0xE0, 0x9F, 0x8D, 0xE0, 0x9F, 0x90, 0x03, 0xEE, 0xE1, 0x9F,
            0xC8, 0xD0, 0xEF, 0xE6, 0xFC, 0xCA, 0xD0, 0xEA, 0xA9, 0x02, 0x8D, 0x3F,
            0xA0, 0xAD, 0xE2, 0x9F, 0x85, 0xFB, 0xAD, 0xE3, 0x9F, 0x85, 0xFC, 0x58,
            0x60,
        };

        private const ushort StubOrg = 0x9F80;
        private const ushort SumAddr = 0x9FE0;
        private const ushort Sentinel = 0x8000; // stub RTS returns here; loop stops

        private const byte RomSwapBasic = 0x02;
        private const byte RomSwapExtension = 0x04;

        private sealed class RomSwapBus : IBusDevice
        {
            private readonly byte[] _ram = new byte[0x10000];
            private readonly byte[] _ext; // 16 KB pattern visible at $C000-$FFFF when ROMSWAP=EXT
            private byte _romswap = RomSwapBasic;

            public RomSwapBus(byte[] ext) => _ext = ext;

            public byte Read(ushort address)
            {
                if (address == 0xA03F) return _romswap;
                if (address >= 0xC000 && _romswap == RomSwapExtension)
                    return _ext[address - 0xC000];
                return _ram[address];
            }

            public void Write(ushort address, byte data)
            {
                if (address == 0xA03F) { _romswap = data; return; }
                _ram[address] = data;
            }

            public byte Peek(ushort address) => _ram[address];
            public byte RomSwap => _romswap;
            public void Load(ushort at, byte[] bytes) => Array.Copy(bytes, 0, _ram, at, bytes.Length);
        }

        [TestMethod]
        public void Stub_ChecksumsPagedExtRom_ByteExact()
        {
            // The pattern page_dma would have copied into ext_rom: ext[i] = (i ^ (i>>8)) & 0xFF
            var ext = new byte[16384];
            int expected = 0;
            for (int i = 0; i < ext.Length; i++)
            {
                ext[i] = (byte)((i ^ (i >> 8)) & 0xFF);
                expected += ext[i];
            }
            expected &= 0xFFFF; // stub accumulates in 16 bits

            var bus = new RomSwapBus(ext);
            bus.Load(StubOrg, StubBytes);

            var cpu = new Cpu(bus, E6502Type.Cmos);

            // Simulate a JSR to the stub: push (Sentinel-1) so the stub's RTS lands on Sentinel.
            bus.Write(0x01FF, (byte)((Sentinel - 1) >> 8));   // PCH
            bus.Write(0x01FE, (byte)((Sentinel - 1) & 0xFF)); // PCL
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, StubOrg,
                                          s.Nf, s.Vf, s.Df, s.If, s.Zf, s.Cf));

            for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();

            Assert.AreEqual(Sentinel, cpu.Pc, "stub did not return via RTS to the sentinel");
            int actual = bus.Peek(SumAddr) | (bus.Peek((ushort)(SumAddr + 1)) << 8);
            Assert.AreEqual(expected, actual, "checksum of paged ext_rom window mismatched");
            Assert.AreEqual(RomSwapBasic, bus.RomSwap, "stub did not restore ROMSWAP to BASIC");
        }
    }
}
