using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    /// <summary>
    /// Stage 4c.0b — the paged-library loader path proven on the REAL emulator bus
    /// (<see cref="CompositeBusDevice"/>), not the test-only <see cref="LibLoaderBus"/>.
    ///
    /// This is the emulator analogue of <c>GraphicsModuleTests</c> Axis 1, but using the
    /// real PGD page-in MMIO ($BA76-$BA7C), the real $C000 ext-bank overlay, the real
    /// ROMSWAP state machine, and the resident <c>lib_call</c> loader POKEd at $0320.
    /// It exercises build -> stage(XRAM) -> page-in -> validate header -> dispatch ->
    /// home-bank restore + resident caching, AND the real VGC hardware effect of a draw
    /// function routed through the full loader (Axis-2 analogue, but through lib_call).
    /// </summary>
    [TestClass]
    public class CompositeLoaderTests
    {
        // Mailbox cells mirror runtime/asm/libabi.inc.
        private const ushort MOD_ID = 0x0300, FN_ID = 0x0301, STATUS = 0x0302,
                             ARG0 = 0x0303, ARG1 = 0x0307, RESULT = 0x0313,
                             HOME_BANK = 0x0317, RESIDENT = 0x0318;
        private const ushort LIB_CALL = 0x0320;       // resident loader entry (libabi.inc LIB_LOADER_BAND)
        private const byte   MODULE_ID_GRAPHICS = 0x01;
        private const byte   GFN_GCLS = 0x00, GFN_GCOLOR = 0x01, GFN_PLOT = 0x02;
        private const byte   LERR_OK = 0x00;
        private const byte   LERR_BAD_MODULE = 0x81;  // libabi.inc LERR_BAD_MODULE
        private const int    ShelfBase = 0x060000;    // XRAM shelf slot 0 (matches libabi.inc SHELF_BASE)
        private const ushort Sentinel = 0xFFF9;       // loader's final RTS lands here; loop stops

        // Stage graphics.bin in XRAM, POKE the loader mailbox to a fresh Logo-runtime
        // cold state (RESIDENT=none, HOME_BANK=Logo), and return a booted bus.
        private static CompositeBusDevice SetupLoaderBus()
        {
            var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
            Assert.AreEqual(CompositeBusDevice.ActiveRom.Logo, bus.CurrentRom, "must boot into the Logo runtime");

            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot();

            bus.StageShelfModule(0, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                                 MODULE_ID_GRAPHICS);
            bus.WriteRam(RESIDENT, 0x00);
            bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);   // loader restores the Logo bank
            return bus;
        }

        // Drive the resident lib_call loader for (mod, fn). Fakes a JSR to $0320 by pushing
        // (Sentinel-1) so the loader's final RTS lands on Sentinel; runs until it returns.
        private static void CallLib(CompositeBusDevice bus, byte mod, byte fn)
        {
            bus.WriteRam(MOD_ID, mod);
            bus.WriteRam(FN_ID, fn);
            bus.WriteRam(STATUS, 0xFF);   // poison so a real OK write is observable

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, LIB_CALL,
                                          s.Nf, s.Vf, s.Df, s.If, s.Zf, s.Cf));

            for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "lib_call did not return via RTS to the sentinel");
        }

        // Write a 32-bit LE arg cell.
        private static void SetArg(CompositeBusDevice bus, ushort cell, int value)
        {
            bus.WriteRam(cell, (byte)(value & 0xFF));
            bus.WriteRam((ushort)(cell + 1), (byte)((value >> 8) & 0xFF));
            bus.WriteRam((ushort)(cell + 2), (byte)((value >> 16) & 0xFF));
            bus.WriteRam((ushort)(cell + 3), (byte)((value >> 24) & 0xFF));
        }

        private static byte GfxPixel(CompositeBusDevice bus, int x, int y)
        {
            int idx = y * VgcConstants.GfxWidth + x;
            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, idx, out byte v),
                $"failed to read gfx pixel ({x},{y})");
            return v;
        }

        [TestMethod]
        public void Gcls_ModuleAtNonZeroSlot_DispatchesViaDirectory()
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
            var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
            bus.StageShelfModule(2, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                                 MODULE_ID_GRAPHICS);
            bus.WriteRam(RESIDENT, 0x00);
            bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

            CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "must dispatch from slot 2 via the directory");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.ReadRam(RESIDENT));
            Assert.AreEqual(1, bus.PageInCount);
        }

        [TestMethod]
        public void LibCall_UnknownId_ReturnsBadModule()
        {
            using var bus = SetupLoaderBus();       // only GRAPHICS (id 1) staged at slot 0
            CallLib(bus, 0x42, 0x00);               // id $42 is not in the directory
            Assert.AreEqual(LERR_BAD_MODULE, bus.ReadRam(STATUS));
        }

        [TestMethod]
        public void LibCall_IdZero_ReturnsBadModule()
        {
            using var bus = SetupLoaderBus();      // GRAPHICS at slot 0; slots 1-3 empty (tag $00)
            CallLib(bus, 0x00, 0x00);              // MODULE_ID_NONE must not false-hit an empty slot
            Assert.AreEqual(LERR_BAD_MODULE, bus.ReadRam(STATUS));
        }

        [TestMethod]
        public void LibCall_Hit_MovesSlotToFrontOfLru()
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
            var cpu = new Cpu(bus, E6502Type.Cmos); cpu.Boot();
            bus.StageShelfModule(2, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                                 MODULE_ID_GRAPHICS);
            // Force a known LRU order where slot 2 is at the back.
            bus.WriteRam(0x041C, 0); bus.WriteRam(0x041D, 1); bus.WriteRam(0x041E, 3); bus.WriteRam(0x041F, 2);
            bus.WriteRam(RESIDENT, 0x00);
            bus.WriteRam(HOME_BANK, VgcConstants.RomSwapLogo);

            CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

            Assert.AreEqual(2, bus.ReadRam(0x041C), "hit slot must move to LRU front");
            Assert.AreEqual(0, bus.ReadRam(0x041D));
            Assert.AreEqual(1, bus.ReadRam(0x041E));
            Assert.AreEqual(3, bus.ReadRam(0x041F));
        }

        [TestMethod]
        public void Gcls_Miss_PagesInOnce_DispatchesOk_CachesResident_RestoresHomeBank()
        {
            using var bus = SetupLoaderBus();   // RESIDENT=0, ext-bank holds Logo's static ext

            CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);

            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "GCLS must report OK through the real loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.ReadRam(RESIDENT),
                "GRAPHICS module must be cached resident after the page-in");
            Assert.AreEqual(1, bus.PageInCount, "MISS path must page in exactly once");
            Assert.AreEqual(CompositeBusDevice.ActiveRom.Logo, bus.CurrentRom,
                "loader must restore the Logo home bank after dispatch");
        }

        [TestMethod]
        public void SecondGcls_Hits_NoSecondPageIn()
        {
            using var bus = SetupLoaderBus();

            CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);   // MISS -> page in
            CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCLS);   // HIT  -> no page in

            Assert.AreEqual(1, bus.PageInCount,
                "resident after first call -> second GCLS must HIT, no re-page");
            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "second GCLS still OK");
            Assert.AreEqual(CompositeBusDevice.ActiveRom.Logo, bus.CurrentRom, "home bank restored on the HIT path too");
        }

        // Full loader path through to a real VGC effect: GCOLOR then PLOT lands a pixel.
        [TestMethod]
        public void GcolorThenPlot_ThroughLoader_SetsRealVgcPixel()
        {
            using var bus = SetupLoaderBus();

            SetArg(bus, ARG0, 7);                 // GCOLOR 7
            CallLib(bus, MODULE_ID_GRAPHICS, GFN_GCOLOR);
            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "GCOLOR must report OK through the loader");

            SetArg(bus, ARG0, 50);                // PLOT 50,60
            SetArg(bus, ARG1, 60);
            CallLib(bus, MODULE_ID_GRAPHICS, GFN_PLOT);
            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), "PLOT must report OK through the loader");

            Assert.AreEqual(7, GfxPixel(bus, 50, 60),
                "PLOT routed through the full loader must set the real VGC gfx pixel to GCOLOR 7");
            Assert.AreEqual(0, GfxPixel(bus, 51, 60), "PLOT must not touch the neighbour pixel");
            Assert.AreEqual(1, bus.PageInCount, "only one page-in across GCOLOR+PLOT (resident after the first)");
        }

        // Turtle state lives at $9F00 (extension.s TURTLE_STATE_BASE); offsets match
        // NovaLogoHarnessTests (X lo/hi = +1/+2, Y lo/hi = +4/+5).
        private const ushort TurtleStateBase = 0x9F00;
        private const ushort TurtleXLo = TurtleStateBase + 1, TurtleXHi = TurtleStateBase + 2;
        private const ushort TurtleYLo = TurtleStateBase + 4, TurtleYHi = TurtleStateBase + 5;

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
