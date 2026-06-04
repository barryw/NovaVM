using System;
using System.IO;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
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
                             ARG0 = 0x0303, ARG1 = 0x0307, ARG2 = 0x030B, ARG3 = 0x030F,
                             RESULT = 0x0313, HOME_BANK = 0x0317, RESIDENT = 0x0318;
        private const byte   MODULE_ID_GRAPHICS = 0x01;
        // Draw-domain fn-ids (runtime/asm/libgraphics.inc).
        private const byte   GFN_GCLS = 0x00, GFN_GCOLOR = 0x01, GFN_PLOT = 0x02,
                             GFN_UNPLOT = 0x03, GFN_LINE = 0x04, GFN_CIRCLE = 0x05,
                             GFN_RECT = 0x06, GFN_FILL = 0x07, GFN_PAINT = 0x08,
                             GFN_GTEXT = 0x09;
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

        // Build an Axis-2 bus with the real graphics.bin injected + romswapped in.
        private static CompositeBusDevice MakeAxis2Bus()
        {
            var bus = new CompositeBusDevice(enableSound: false);
            bus.LoadExtensionRomBytesForTest(File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")));
            bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
            Assert.AreEqual(CompositeBusDevice.ActiveRom.Extension, bus.CurrentRom);
            Assert.AreEqual(0x4C, bus.Read(0xC000), "module $C000 must be JMP (the header trampoline)");
            return bus;
        }

        // Write a 32-bit LE arg cell (only the low word matters for s16 args).
        private static void SetArg(CompositeBusDevice bus, ushort cell, int value)
        {
            bus.WriteRam(cell, (byte)(value & 0xFF));
            bus.WriteRam((ushort)(cell + 1), (byte)((value >> 8) & 0xFF));
            bus.WriteRam((ushort)(cell + 2), (byte)((value >> 16) & 0xFF));
            bus.WriteRam((ushort)(cell + 3), (byte)((value >> 24) & 0xFF));
        }

        // Drive the module dispatch directly for fn-id `fn`; assert it RTSes and
        // returns LIB_STATUS = OK.
        private static void RunFn(CompositeBusDevice bus, byte fn)
        {
            bus.WriteRam(FN_ID, fn);
            bus.WriteRam(STATUS, 0xFF);   // poison so a real OK write is observable

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true /*I*/, s.Zf, s.Cf));

            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, $"fn ${fn:X2} dispatch did not RTS back to the sentinel");
            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), $"fn ${fn:X2} must set LIB_STATUS = OK");
        }

        // Read a single gfx-plane pixel (color index) at (x,y) via the real VGC API.
        private static byte GfxPixel(CompositeBusDevice bus, int x, int y)
        {
            int idx = y * VgcConstants.GfxWidth + x;
            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, idx, out byte v),
                $"failed to read gfx pixel ({x},{y})");
            return v;
        }

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

        // --- $01 GCOLOR + $02 PLOT: set draw colour, then plot a pixel in it. ---
        [TestMethod]
        public void Axis2_GcolorThenPlot_SetsPixelToColor()
        {
            using var bus = MakeAxis2Bus();

            // GCOLOR 7
            SetArg(bus, ARG0, 7);
            RunFn(bus, GFN_GCOLOR);

            // PLOT 50, 60
            SetArg(bus, ARG0, 50);
            SetArg(bus, ARG1, 60);
            RunFn(bus, GFN_PLOT);

            Assert.AreEqual(7, GfxPixel(bus, 50, 60), "PLOT must set (50,60) to the GCOLOR value 7");
            Assert.AreEqual(0, GfxPixel(bus, 51, 60), "PLOT must not touch the neighbouring pixel");
        }

        // --- $03 UNPLOT: clear a previously-set pixel. ---
        [TestMethod]
        public void Axis2_Unplot_ClearsPixel()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 5);                 // GCOLOR 5
            RunFn(bus, GFN_GCOLOR);
            SetArg(bus, ARG0, 80);
            SetArg(bus, ARG1, 90);
            RunFn(bus, GFN_PLOT);
            Assert.AreEqual(5, GfxPixel(bus, 80, 90), "setup PLOT should have set the pixel");

            SetArg(bus, ARG0, 80);                // UNPLOT 80,90
            SetArg(bus, ARG1, 90);
            RunFn(bus, GFN_UNPLOT);

            Assert.AreEqual(0, GfxPixel(bus, 80, 90), "UNPLOT must clear (80,90) back to 0");
        }

        // --- $04 LINE: a horizontal run of pixels takes the draw colour. ---
        [TestMethod]
        public void Axis2_Line_DrawsAlongPath()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 9);                 // GCOLOR 9
            RunFn(bus, GFN_GCOLOR);

            // LINE (10,30)-(40,30) — horizontal, y constant.
            SetArg(bus, ARG0, 10);
            SetArg(bus, ARG1, 30);
            SetArg(bus, ARG2, 40);
            SetArg(bus, ARG3, 30);
            RunFn(bus, GFN_LINE);

            Assert.AreEqual(9, GfxPixel(bus, 10, 30), "line endpoint (10,30) must be set");
            Assert.AreEqual(9, GfxPixel(bus, 25, 30), "line midpoint (25,30) must be set");
            Assert.AreEqual(9, GfxPixel(bus, 40, 30), "line endpoint (40,30) must be set");
            Assert.AreEqual(0, GfxPixel(bus, 25, 31), "pixel off the line must stay clear");
        }

        // --- $05 CIRCLE: a cardinal point on the circle is set. ---
        [TestMethod]
        public void Axis2_Circle_DrawsPerimeter()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 4);                 // GCOLOR 4
            RunFn(bus, GFN_GCOLOR);

            // CIRCLE centre (100,100), rx=20, ry=0 (-> circle of radius 20).
            SetArg(bus, ARG0, 100);
            SetArg(bus, ARG1, 100);
            SetArg(bus, ARG2, 20);
            SetArg(bus, ARG3, 0);
            RunFn(bus, GFN_CIRCLE);

            // Cardinal points of the midpoint circle (cx+/-r, cy) and (cx, cy+/-r).
            Assert.AreEqual(4, GfxPixel(bus, 120, 100), "circle right cardinal (cx+r,cy) must be set");
            Assert.AreEqual(4, GfxPixel(bus, 80, 100),  "circle left cardinal (cx-r,cy) must be set");
            Assert.AreEqual(4, GfxPixel(bus, 100, 120), "circle bottom cardinal (cx,cy+r) must be set");
            Assert.AreEqual(0, GfxPixel(bus, 100, 100), "circle centre must remain unfilled");
        }

        // --- $06 RECT: outline corners set, interior empty. ---
        [TestMethod]
        public void Axis2_Rect_DrawsOutline()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 6);                 // GCOLOR 6
            RunFn(bus, GFN_GCOLOR);

            // RECT (40,40)-(60,55).
            SetArg(bus, ARG0, 40);
            SetArg(bus, ARG1, 40);
            SetArg(bus, ARG2, 60);
            SetArg(bus, ARG3, 55);
            RunFn(bus, GFN_RECT);

            Assert.AreEqual(6, GfxPixel(bus, 40, 40), "rect top-left corner must be set");
            Assert.AreEqual(6, GfxPixel(bus, 60, 40), "rect top-right corner must be set");
            Assert.AreEqual(6, GfxPixel(bus, 40, 55), "rect bottom-left corner must be set");
            Assert.AreEqual(6, GfxPixel(bus, 60, 55), "rect bottom-right corner must be set");
            Assert.AreEqual(6, GfxPixel(bus, 50, 40), "rect top edge must be set");
            Assert.AreEqual(0, GfxPixel(bus, 50, 48), "rect interior must remain empty");
        }

        // --- $07 FILL: solid rectangle — interior and corners set. ---
        [TestMethod]
        public void Axis2_Fill_FillsInterior()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 3);                 // GCOLOR 3
            RunFn(bus, GFN_GCOLOR);

            // FILL (40,40)-(60,55).
            SetArg(bus, ARG0, 40);
            SetArg(bus, ARG1, 40);
            SetArg(bus, ARG2, 60);
            SetArg(bus, ARG3, 55);
            RunFn(bus, GFN_FILL);

            Assert.AreEqual(3, GfxPixel(bus, 40, 40), "fill corner must be set");
            Assert.AreEqual(3, GfxPixel(bus, 60, 55), "fill far corner must be set");
            Assert.AreEqual(3, GfxPixel(bus, 50, 48), "fill interior must be set");
            Assert.AreEqual(0, GfxPixel(bus, 61, 48), "pixel just outside fill must stay clear");
        }

        // --- $08 PAINT: flood-fill an enclosed region. ---
        [TestMethod]
        public void Axis2_Paint_FloodFillsRegion()
        {
            using var bus = MakeAxis2Bus();

            // Paint the whole (empty=color 0) plane starting at (10,10) with color 2.
            SetArg(bus, ARG0, 2);                 // GCOLOR 2
            RunFn(bus, GFN_GCOLOR);
            SetArg(bus, ARG0, 10);
            SetArg(bus, ARG1, 10);
            RunFn(bus, GFN_PAINT);

            // The flood replaces the contiguous 0-region (the whole plane) with 2.
            Assert.AreEqual(2, GfxPixel(bus, 10, 10), "paint seed pixel must take the fill colour");
            Assert.AreEqual(2, GfxPixel(bus, 200, 150), "paint must flood across the empty plane");
        }

        // --- $09 GTEXT: draw a string; assert a glyph pixel lands in the plane. ---
        [TestMethod]
        public void Axis2_Gtext_DrawsGlyphPixels()
        {
            using var bus = MakeAxis2Bus();

            // A bare CompositeBusDevice has no font wired in (the GUI sets it). Install
            // a synthetic 1-slot font whose char 'A' (0x41) is fully solid so every
            // glyph pixel is deterministic and assertable.
            var fontData = new byte[BitmapFont.FontDataSize];   // 256 glyphs * 8 rows
            for (int row = 0; row < BitmapFont.GlyphHeight; row++)
                fontData[0x41 * BitmapFont.GlyphHeight + row] = 0xFF;   // glyph 'A' all-on
            bus.Vgc.SetFont(new BitmapFont(fontData));

            SetArg(bus, ARG0, 12);                // GCOLOR 12
            RunFn(bus, GFN_GCOLOR);

            // Stage the string "A" in CPU RAM and point ARG2 at it (ptr16 low word,
            // len16 high word).
            const ushort strAddr = 0x0500;
            bus.WriteRam(strAddr, 0x41);          // 'A'
            SetArg(bus, ARG0, 30);                // x
            SetArg(bus, ARG1, 40);                // y
            // ARG2 = ptr16 (low word) | len16<<16 (high word).
            SetArg(bus, ARG2, strAddr | (1 << 16));
            RunFn(bus, GFN_GTEXT);

            // Solid glyph at scale 1, 8x8 origin (30,40): every pixel in the box set.
            Assert.AreEqual(12, GfxPixel(bus, 30, 40), "glyph top-left pixel must be set");
            Assert.AreEqual(12, GfxPixel(bus, 37, 47), "glyph bottom-right pixel must be set");
            Assert.AreEqual(0, GfxPixel(bus, 38, 40), "pixel past the 8px-wide glyph must stay clear");
        }

        // --- Loader-axis smoke: new fn-ids route through the real lib_call loader. ---
        [TestMethod]
        public void Axis1_Plot_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            // ARG0=x, ARG1=y; the loader path lands the VGC write in dead RAM, so we
            // only assert dispatch routed to the new id and returned OK.
            bus.PokeRam(ARG0, 50); bus.PokeRam((ushort)(ARG0 + 1), 0);
            bus.PokeRam(ARG1, 60); bus.PokeRam((ushort)(ARG1 + 1), 0);

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_PLOT);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "PLOT must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch bounds: an id above the dense table returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_UnimplementedId_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(FN_ID, 0x0A);   // first id past GFX_FN_COUNT-1
            bus.WriteRam(STATUS, 0x00);  // poison opposite to expected non-OK

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "dispatch must RTS even for an unknown id");
            Assert.AreEqual(0x83 /*LERR_NO_FN*/, bus.ReadRam(STATUS),
                "id $0A (>= GFX_FN_COUNT) must report LERR_NO_FN via the bounds check");
        }

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
