using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    /// <summary>
    /// Self-tests for <see cref="LibLoaderBus"/> — proves the behavioral bus itself models the
    /// ROMSWAP overlay and the $BA76 page-in correctly before the T5 loader depends on it.
    /// </summary>
    [TestClass]
    public class LibLoaderBusTests
    {
        private const ushort REG_ROMSWAP = 0xA03F;
        private const byte    RS_BASIC   = 0x02;
        private const byte    RS_EXT     = 0x04;
        private const ushort  PGD_CMD = 0xBA76,
                              PGD_SRCL = 0xBA78, PGD_SRCM = 0xBA79, PGD_SRCH = 0xBA7A,
                              PGD_WORDSL = 0xBA7B, PGD_WORDSH = 0xBA7C;

        [TestMethod]
        public void PageIn_CopiesXramToExtOverlay()
        {
            var bus = new LibLoaderBus();

            // Known 16-byte pattern parked in XRAM at $060000 (8 words).
            var pattern = new byte[16];
            for (int i = 0; i < pattern.Length; i++) pattern[i] = (byte)(0xA0 + i);
            const int src = 0x060000;
            bus.LoadXram(src, pattern);

            // Program the page-in descriptor: SRC=$060000, WORDS=8.
            bus.Write(PGD_SRCL, (byte)(src & 0xFF));
            bus.Write(PGD_SRCM, (byte)((src >> 8) & 0xFF));
            bus.Write(PGD_SRCH, (byte)((src >> 16) & 0xFF));
            bus.Write(PGD_WORDSL, 8);
            bus.Write(PGD_WORDSH, 0);

            // Fire the page-in.
            bus.Write(PGD_CMD, 1);
            Assert.AreEqual(1, bus.PageInCount, "page-in did not run exactly once");

            // Flip the overlay in and read back the 16 ext_rom bytes at $C000.
            bus.Write(REG_ROMSWAP, RS_EXT);
            for (int i = 0; i < pattern.Length; i++)
                Assert.AreEqual(pattern[i], bus.Read((ushort)(0xC000 + i)),
                    $"ext_rom[{i}] mismatch after page-in");

            // $A03F is NOT readable on HW — a read returns the RAM cell, NOT the live bank.
            // The loader must never rely on reading it. Prove the model: poke RAM[$A03F]=$99
            // while the live bank is $04, and confirm the read surfaces RAM ($99), not $04.
            bus.PokeRam(REG_ROMSWAP, 0x99);
            Assert.AreEqual(0x99, bus.Read(REG_ROMSWAP),
                "$A03F read must return the RAM cell, not the live bank (not readable on HW)");
            Assert.AreEqual(RS_EXT, bus.CurrentBank,
                "live overlay bank must still be EXT despite the RAM poke at $A03F");
        }

        [TestMethod]
        public void ExtOverlay_OnlyVisible_WhenRomSwapExt()
        {
            var bus = new LibLoaderBus();

            // Distinct sentinels: RAM[$C000]=$11, ext_rom[0]=$EE (via XRAM page-in).
            bus.PokeRam(0xC000, 0x11);
            var one = new byte[2] { 0xEE, 0xFF };
            bus.LoadXram(0, one);
            bus.Write(PGD_SRCL, 0);
            bus.Write(PGD_SRCM, 0);
            bus.Write(PGD_SRCH, 0);
            bus.Write(PGD_WORDSL, 1);
            bus.Write(PGD_WORDSH, 0);
            bus.Write(PGD_CMD, 1);

            // BASIC bank: $C000 shows RAM.
            bus.Write(REG_ROMSWAP, RS_BASIC);
            Assert.AreEqual(0x11, bus.Read(0xC000), "BASIC bank must expose RAM at $C000");

            // EXT bank: $C000 shows the ext_rom overlay.
            bus.Write(REG_ROMSWAP, RS_EXT);
            Assert.AreEqual(0xEE, bus.Read(0xC000), "EXT bank must expose ext_rom overlay at $C000");
        }
    }
}
