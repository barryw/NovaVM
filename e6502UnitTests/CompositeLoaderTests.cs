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

        // ---------------------------------------------------------------------
        // 4c.0c: the legacy ext path must survive a module page-in clobbering bank 1.
        //
        // Faithful coexistence sequence on the REAL CompositeBusDevice:
        //   1. Boot Logo to the prompt -> cold_start seeds LIB_RESIDENT=$FF, stages
        //      novalogo_ext in XRAM at $07C000, _extBank = novalogo_ext.
        //   2. Clobber bank 1 with the GRAPHICS module via a routed PGD page-in (no CPU),
        //      and poke LIB_RESIDENT=GRAPHICS to mimic the loader's cached state.
        //   3. Run a STILL-LEGACY ext command (TIMER, the frame-counter reporter in
        //      extension.s ext_timer) through the normal Logo input path. NOTE: turtle/
        //      graphics commands now route to the GRAPHICS module via lib_call (4c.2-3)
        //      and would NOT take the legacy re-page path — so SETXY can no longer prove
        //      this; TIMER stays MODULE_ID_NONE / legacy.
        //      ext_invoke's legacy branch -> ensure_ext_resident (RESIDENT != $FF ->
        //      re-page novalogo_ext, PageInCount++, RESIDENT -> $FF) -> EXT_TRAMPOLINE
        //      -> the REAL novalogo_ext TIMER handler.
        //   4. Assert the line ran with no "I DON'T KNOW" (TIMER ran the real re-paged
        //      handler, not clobbered module bytes), RESIDENT==$FF, and PageInCount rose.
        // ---------------------------------------------------------------------
        [TestMethod]
        public void LegacyExtCommand_AfterModuleClobbersBank1_RepagesHostExtAndRuns()
        {
            using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);

            // 1. Run to the Logo prompt — cold_start has run.
            RunUntilScreenContains(cpu, bus, "?", 10_000_000, "boot to prompt");
            Assert.AreEqual(0xFF, bus.ReadRam(RESIDENT),
                "cold_start must seed LIB_RESIDENT=$FF (host ext resident in bank 1)");

            // 2. Clobber bank 1 with the GRAPHICS module via a routed PGD page-in (no CPU).
            bus.LoadXram(ShelfBase, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")));
            bus.Write(0xBA78, 0x00);                 // PGD_SRCL  ] $060000 (shelf slot 0)
            bus.Write(0xBA79, 0x00);                 // PGD_SRCM  ]
            bus.Write(0xBA7A, 0x06);                 // PGD_SRCH  ]
            bus.Write(0xBA7B, 0x00);                 // PGD_WORDSL ] $2000 words = 16KB
            bus.Write(0xBA7C, 0x20);                 // PGD_WORDSH ]
            bus.Write(0xBA76, 0x01);                 // PGD_CMD = START -> DMA graphics -> bank 1
            Assert.AreEqual(1, bus.PageInCount, "the forced clobber must page in exactly once");
            bus.WriteRam(RESIDENT, MODULE_ID_GRAPHICS); // mimic the loader's cached state
            int pageInsBeforeLegacy = bus.PageInCount;

            // 3. Run a still-legacy ext command (TIMER) through the normal Logo path,
            //    then PRINT a unique marker so we can detect the line fully evaluated.
            foreach (char ch in "PRINT TIMER\rPRINT \"DONE4C0C\r")
                editor.QueueInput((byte)ch);
            RunUntilScreenContains(cpu, bus, "DONE4C0C", 80_000_000, "TIMER after clobber");
            // settle so the line fully evaluates and returns to the prompt
            for (int i = 0; i < 1_000_000; i++)
            {
                int cycles = cpu.ClocksForNext();
                cpu.ExecuteNext();
                bus.AdvanceCycles(cycles);
            }

            string screen = SnapshotScreen(bus.Vgc);
            Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
                $"TIMER must not fall through to the unknown-word path after re-page.\n{screen}");

            // 4. The real novalogo_ext TIMER handler ran (the line completed to the
            //    marker without an unknown-word error) via the re-paged host ext.
            Assert.AreEqual(0xFF, bus.ReadRam(RESIDENT),
                "ensure_ext_resident must mark the host ext resident (LIB_RESIDENT=$FF) after re-page");
            Assert.IsTrue(bus.PageInCount > pageInsBeforeLegacy,
                "the legacy path must re-page the host ext (PageInCount must increase)");
        }

        private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps, string what)
        {
            for (int i = 0; i < maxSteps; i++)
            {
                int cycles = cpu.ClocksForNext();
                cpu.ExecuteNext();
                bus.AdvanceCycles(cycles);
                if ((i & 0x3FF) == 0 && SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                    return;
            }
            Assert.Fail($"timed out waiting for '{marker}' ({what}).\n{SnapshotScreen(bus.Vgc)}");
        }

        private static string SnapshotScreen(VirtualGraphicsController vgc)
        {
            var sb = new StringBuilder();
            for (int row = 0; row < VgcConstants.ScreenRows; row++)
            {
                for (int col = 0; col < VgcConstants.ScreenCols; col++)
                {
                    byte ch = vgc.GetScreenChar(col, row);
                    sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
                }
                sb.Append('\n');
            }
            return sb.ToString();
        }

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
