using System;
using System.IO;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    /// <summary>
    /// Stage 4b.1 — the pipeline-proof milestone for the paged GRAPHICS module
    /// (runtime/asm/libgraphics.inc, modules/graphics/graphics.s, MODULE_ID_GRAPHICS=$01).
    /// One function (GFN_GCLS) proven end-to-end on two independent axes:
    ///
    ///   Axis 1 (loader path, <see cref="LibLoaderBus"/>): stage the real graphics.bin into
    ///     XRAM, drive the real <c>lib_call</c> loader. Proves build->stage->page->validate
    ///     header->dispatch->return + resident caching. The VGC write lands in dead RAM here.
    ///
    ///   Axis 2 (hardware effect, <see cref="CompositeBusDevice"/>): inject graphics.bin as
    ///     the extension ROM, romswap it in at $C000, JSR the module's dispatch directly
    ///     (bypassing the loader — paging is independently Phase-3 proven), assert the real
    ///     VGC gfx plane is cleared. Proves wrapper->driver->hardware.
    /// </summary>
    [TestClass]
    public class GraphicsModuleTests
    {
        // Mailbox cells mirror runtime/asm/libabi.inc.
        private const ushort MOD_ID = 0x0300, FN_ID = 0x0301, STATUS = 0x0302,
                             RESULT = 0x0313, HOME_BANK = 0x0317, RESIDENT = 0x0318;
        private const byte   MODULE_ID_GRAPHICS = 0x01;
        private const byte   GFN_GCLS = 0x00;
        private const byte   LERR_OK = 0x00;
        private const ushort LibCallEntry = 0x9C00;   // libcall.bin blob load address
        private const int    ShelfBase = 0x060000;    // XRAM shelf slot 0 (shared with TEST)
        private const byte   RsBasic = 0x02;          // runtime home ROMSWAP value
        private const ushort Sentinel = 0xFFF9;       // RTS lands here; loop stops

        // ---------------------------------------------------------------------
        // Axis 1: loader path on LibLoaderBus (mirrors LibCallTests exactly).
        // ---------------------------------------------------------------------

        private static (LibLoaderBus bus, ushort entry) SetupLoader()
        {
            var bus = new LibLoaderBus();
            bus.LoadRam(LibCallEntry, File.ReadAllBytes(RepoPath("tests", "asm", "libcall.bin")));
            bus.LoadXram(ShelfBase, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")));
            bus.PokeRam(HOME_BANK, RsBasic);
            bus.PokeRam(RESIDENT, 0x00);
            return (bus, LibCallEntry);
        }

        private static void CallLib(LibLoaderBus bus, ushort entry, byte mod, byte fn)
        {
            bus.PokeRam(MOD_ID, mod);
            bus.PokeRam(FN_ID, fn);

            var cpu = new Cpu(bus, E6502Type.Cmos);

            // Fake a JSR to lib_call: push (Sentinel-1) so the loader's final RTS lands on Sentinel.
            bus.PokeRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.PokeRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, entry,
                                          s.Nf, s.Vf, s.Df, s.If, s.Zf, s.Cf));

            for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "lib_call did not return via RTS to the sentinel");
        }

        [TestMethod]
        public void Axis1_Gcls_Miss_PagesInOnce_DispatchesOk_CachesResident()
        {
            var (bus, entry) = SetupLoader();   // RESIDENT=0, ext_rom empty

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_GCLS);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "GCLS must report OK through the loader");
            Assert.AreEqual(1, bus.PageInCount, "MISS path must page in exactly once");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be cached as resident after the page-in");
            Assert.AreEqual(RsBasic, bus.CurrentBank, "home bank must be restored");
        }

        [TestMethod]
        public void Axis1_SecondGcls_Hits_NoSecondPageIn()
        {
            var (bus, entry) = SetupLoader();

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_GCLS);   // MISS -> page in
            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_GCLS);   // HIT  -> no page in

            Assert.AreEqual(1, bus.PageInCount,
                "resident after first call -> second GCLS must HIT, no re-page");
            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "second GCLS still OK");
        }

        // ---------------------------------------------------------------------
        // Axis 2: real VGC effect on CompositeBusDevice. Inject graphics.bin as the
        // extension ROM, romswap it to $C000, JSR dispatch directly (loader bypassed).
        // ---------------------------------------------------------------------

        [TestMethod]
        public void Axis2_Gcls_ClearsVgcGfxPlane_OnRealHardware()
        {
            using var bus = new CompositeBusDevice(enableSound: false);
            bus.LoadExtensionRomBytesForTest(File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")));

            // Pre-fill several gfx-plane bytes to nonzero via the real VGC memory API.
            int[] probes = { 0, 1, 100, 1234, 31999, VgcConstants.GfxWidth * VgcConstants.GfxHeight - 1 };
            foreach (int p in probes)
                Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, p, 0xAB),
                    $"failed to pre-fill gfx byte {p}");
            foreach (int p in probes)
            {
                Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, p, out byte v),
                    $"failed to read gfx byte {p}");
                Assert.AreEqual(0xAB, v, $"pre-fill did not take at gfx byte {p}");
            }

            // Romswap the module in at $C000 (physically copies graphics.bin into RAM).
            bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
            Assert.AreEqual(CompositeBusDevice.ActiveRom.Extension, bus.CurrentRom);
            Assert.AreEqual(0x4C, bus.Read(0xC000), "module $C000 must be JMP (the header trampoline)");

            // Drive the module dispatch directly: LIB_FN_ID = GCLS, PC = $C000, fake JSR
            // pushes (Sentinel-1) so the module's RTS returns to the sentinel.
            bus.WriteRam(FN_ID, GFN_GCLS);
            bus.WriteRam(STATUS, 0xFF);   // poison so a real OK write is observable

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true /*I*/, s.Zf, s.Cf));

            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "module dispatch did not RTS back to the sentinel");

            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "GCLS must set LIB_STATUS = OK");

            // The real VGC gfx plane must now be cleared.
            foreach (int p in probes)
            {
                Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, p, out byte v),
                    $"failed to read gfx byte {p} after GCLS");
                Assert.AreEqual(0x00, v, $"GCLS did not clear gfx byte {p}");
            }
        }

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
