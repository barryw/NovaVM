using System;
using System.IO;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    /// <summary>
    /// TURTLE module (runtime/asm/libturtle.inc, modules/turtle/turtle.s,
    /// MODULE_ID_TURTLE = $07). The whole turtle (move-math semantics + rotate-blit
    /// render) was lifted verbatim out of the GRAPHICS module into this dedicated,
    /// demand-loadable module — turtle behavior is preserved EXACTLY (these tests are
    /// the retargeted GraphicsModuleTests turtle suite: same assertions, same $9F00
    /// state peeks; only the injected .bin and the fn-ids changed).
    ///
    /// These are Axis-2 (hardware-effect) tests: inject the real turtle.bin as the
    /// extension ROM, romswap it in at $C000, JSR the module's dispatch directly for
    /// a TUR_* fn-id, and assert the real VGC gfx plane / $9F00 turtle state. The
    /// loader/demand-load path is independently proven (NovaLogoTests drive the full
    /// Logo -> adapter -> lib_call(TURTLE) -> demand-load -> render chain).
    ///
    /// NOTE: the turtle module is NOT a thin NDK wrapper (it owns its move-math +
    /// render), so it is intentionally NOT in ModuleNdkContractTests.Modules[].
    /// </summary>
    [TestClass]
    public class TurtleModuleTests
    {
        // Mailbox cells mirror runtime/asm/libabi.inc.
        private const ushort MOD_ID = 0x0300, FN_ID = 0x0301, STATUS = 0x0302,
                             ARG0 = 0x0303, ARG1 = 0x0307, ARG2 = 0x030B, ARG3 = 0x030F,
                             RESULT = 0x0313, HOME_BANK = 0x0317, RESIDENT = 0x0318;
        private const byte   MODULE_ID_TURTLE = 0x07;
        // TURTLE module fn-ids (runtime/asm/libturtle.inc).
        private const byte   TUR_INIT = 0x00, TUR_DRAW = 0x01, TUR_ERASE = 0x02, TUR_OP = 0x03;
        private const byte   TURTLE_COL_WHITE = 0x01;
        // Turtle command-engine op-codes (carried in ARG2 byte0; value args in
        // ARG0/ARG1 as Logo 16.8: byte0=FRAC, byte1=LO, byte2=HI).
        private const byte   TOP_FD = 0x10, TOP_BK = 0x11, TOP_RT = 0x12, TOP_LT = 0x13,
                             TOP_CS = 0x14, TOP_PU = 0x15, TOP_PD = 0x16, TOP_ST = 0x17,
                             TOP_HT = 0x18, TOP_HOME = 0x19, TOP_TS = 0x1A, TOP_SS = 0x1B,
                             TOP_FS = 0x1C, TOP_SETXY = 0x1D, TOP_SETX = 0x1E, TOP_SETY = 0x1F,
                             TOP_SETH = 0x20, TOP_XCOR = 0x21, TOP_YCOR = 0x22, TOP_HEADING = 0x23,
                             TOP_PENDOWNP = 0x24, TOP_SHOWNP = 0x25, TOP_SETPC = 0x26,
                             TOP_SETBG = 0x27, TOP_TOWARDS = 0x28;
        // $29 (SETPOS [x y]) is intentionally absent: the module is 100% numeric;
        // SETPOS's list-unpack moved to the Logo adapter, which drives SETXY.
        // Turtle state cells at $9F00 (shared RAM — unchanged by the extraction).
        private const ushort TS_X_FRAC = 0x9F00, TS_X_LO = 0x9F01, TS_X_HI = 0x9F02,
                             TS_Y_FRAC = 0x9F03, TS_Y_LO = 0x9F04, TS_Y_HI = 0x9F05,
                             TS_HEADING_LO = 0x9F06, TS_HEADING_HI = 0x9F07,
                             TS_PEN = 0x9F08, TS_SHOWN = 0x9F09, TS_COLOR = 0x9F0A,
                             TS_INITED = 0x9F0C;
        // Internal screen-pixel home (the Logo origin (0,0) maps to screen center).
        private const int    TURTLE_CENTER_X = 160, TURTLE_CENTER_Y = 100;
        private const byte   LERR_OK = 0x00, LERR_NO_FN = 0x83, LERR_VSPRITE_FAIL = 0x84;
        private const ushort Sentinel = 0xFFF9;       // RTS lands here; loop stops

        // Build an Axis-2 bus with the real turtle.bin injected + romswapped in.
        private static CompositeBusDevice MakeAxis2Bus()
        {
            var bus = new CompositeBusDevice(enableSound: false);
            bus.LoadExtensionRomBytesForTest(File.ReadAllBytes(RepoPath("modules", "turtle", "turtle.bin")));
            bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
            Assert.AreEqual(CompositeBusDevice.ActiveRom.Extension, bus.CurrentRom);
            Assert.AreEqual(0x4C, bus.Read(0xC000), "module $C000 must be JMP (the header trampoline)");
            return bus;
        }

        // Zero the whole VGC gfx plane via the real VGC memory API. The turtle module
        // has no GCLS fn (fn $00 is TUR_INIT), so pre-clearing the plane goes through
        // the hardware memory interface directly rather than a module call.
        private static void ClearGfxPlane(CompositeBusDevice bus)
        {
            int n = VgcConstants.GfxWidth * VgcConstants.GfxHeight;
            for (int i = 0; i < n; i++)
                bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, i, 0);
        }

        // Write a 32-bit LE arg cell (only the low word matters for s16 args).
        private static void SetArg(CompositeBusDevice bus, ushort cell, int value)
        {
            bus.WriteRam(cell, (byte)(value & 0xFF));
            bus.WriteRam((ushort)(cell + 1), (byte)((value >> 8) & 0xFF));
            bus.WriteRam((ushort)(cell + 2), (byte)((value >> 16) & 0xFF));
            bus.WriteRam((ushort)(cell + 3), (byte)((value >> 24) & 0xFF));
        }

        // Read the 32-bit LE LIB_RESULT cell (reporter-style fns).
        private static uint GetResult(CompositeBusDevice bus) =>
            (uint)(bus.ReadRam(RESULT)
                 | (bus.ReadRam((ushort)(RESULT + 1)) << 8)
                 | (bus.ReadRam((ushort)(RESULT + 2)) << 16)
                 | (bus.ReadRam((ushort)(RESULT + 3)) << 24));

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

        // Like RunFn, but pumps bus.AdvanceCycles each CPU step so the DMA/blitter
        // engines advance (rotate-blit's frame-wait needs the bus pumped).
        private static void RunFnPumped(CompositeBusDevice bus, byte fn)
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
            {
                cpu.ExecuteNext();
                bus.AdvanceCycles(16);   // pump the DMA/blitter engines per step
            }
            Assert.AreEqual(Sentinel, cpu.Pc, $"fn ${fn:X2} dispatch did not RTS back to the sentinel");
            Assert.AreEqual(LERR_OK, bus.ReadRam(STATUS), $"fn ${fn:X2} must set LIB_STATUS = OK");
        }

        // Like RunFn but does NOT assert STATUS — for ids expected to fail (gap ids).
        private static void RunFnRaw(CompositeBusDevice bus, byte fn)
        {
            bus.WriteRam(FN_ID, fn);

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true /*I*/, s.Zf, s.Cf));

            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, $"fn ${fn:X2} dispatch did not RTS back to the sentinel");
        }

        // Read a single gfx-plane pixel (color index) at (x,y) via the real VGC API.
        private static byte GfxPixel(CompositeBusDevice bus, int x, int y)
        {
            int idx = y * VgcConstants.GfxWidth + x;
            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, idx, out byte v),
                $"failed to read gfx pixel ({x},{y})");
            return v;
        }

        // =====================================================================
        // Turtle-render domain ($B0-$B2), batch 4c.2-2. The turtle is a 16x16
        // color-keyed virtual sprite (an upward triangle icon) drawn on the gfx
        // plane with Amiga-BOB save/restore-bg. INIT installs the built-in icon
        // into the module BSS source buffer + clears bg bookkeeping; DRAW erases
        // the old stamp, saves the new background, then rotate-blits the icon at
        // the rotation angle; ERASE restores the saved background.
        //
        // DRAW frame-waits on VGC_FRAME inside vsprite_gfx_rotate_blit_keyed, so
        // it MUST be driven via RunFnPumped (advances the frame counter). INIT/
        // ERASE are synchronous (RunFn).
        // =====================================================================

        // The 16x16 turtle icon (replicates the ca65 .repeat formula in
        // graphics.s / extension.s exactly — integer division truncates toward
        // zero; all operands here are non-negative so it is floor division).
        // icon[row,col] = COL_WHITE for the upward triangle, else 0.
        private static byte TurtleIcon(int col, int row)
        {
            if (row < 1) return 0;
            if (row > 14) return 0;
            if (row >= 13)
            {
                int lo = 8 - (((row - 1) * 7 + 6) / 13);
                int hi = 8 + (((row - 1) * 6 + 6) / 13);
                return (byte)((col >= lo && col <= hi) ? TURTLE_COL_WHITE : 0);
            }
            if (col == 8 - (((row - 1) * 7 + 6) / 13)) return TURTLE_COL_WHITE;
            if (col == 8 + (((row - 1) * 6 + 6) / 13)) return TURTLE_COL_WHITE;
            return 0;
        }

        // The stamp top-left for a center (cx,cy) is (cx-8, cy-8) once clamped.
        private const int TURTLE_HALF = 8;

        // --- $B0 TURTLE_INIT: dispatches OK (id wired, not gfn_unimpl). ---
        [TestMethod]
        public void Axis2_TurtleInit_DispatchesOk()
        {
            using var bus = MakeAxis2Bus();
            RunFn(bus, TUR_INIT);   // RunFn asserts RTS + STATUS == LERR_OK
        }

        // --- $B0 then $B1: INIT installs the icon, DRAW stamps its white pixels
        // on the gfx plane (color-keyed: transparent icon cells stay background). ---
        [TestMethod]
        public void Axis2_TurtleInitThenDraw_StampsIconPixels()
        {
            using var bus = MakeAxis2Bus();

            ClearGfxPlane(bus);            // clear the gfx plane to 0
            RunFn(bus, TUR_INIT);   // install the built-in icon

            const int cx = 160, cy = 80;
            SetArg(bus, ARG0, cx);
            SetArg(bus, ARG1, cy);
            SetArg(bus, ARG2, 0);          // angle 0 -> upright icon
            RunFnPumped(bus, TUR_DRAW);

            int tlx = cx - TURTLE_HALF, tly = cy - TURTLE_HALF;   // stamp top-left

            // Probe several KNOWN icon white cells (the apex + base bar) and
            // several KNOWN transparent cells; assert each maps to the right gfx
            // pixel value. This is specific to the icon geometry, not "something
            // changed".
            (int col, int row)[] whites =
            {
                (8, 1),   // apex
                (7, 2), (8, 2),
                (2, 13), (8, 13), (14, 13),  // base bar
                (1, 14), (8, 14), (14, 14),  // base bar bottom row
            };
            foreach (var (col, row) in whites)
            {
                Assert.AreEqual(TURTLE_COL_WHITE, TurtleIcon(col, row),
                    $"test fixture: icon ({col},{row}) should be white");
                Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, tlx + col, tly + row),
                    $"TURTLE_DRAW must stamp icon white cell ({col},{row}) onto the gfx plane");
            }

            // Transparent icon cells must remain background (0) — color-key blit.
            (int col, int row)[] clears =
            {
                (0, 0),   // corner, above the icon
                (0, 8),   // left edge, transparent
                (8, 8),   // interior of the triangle, transparent
                (15, 7),  // right edge, transparent
            };
            foreach (var (col, row) in clears)
            {
                Assert.AreEqual(0, TurtleIcon(col, row),
                    $"test fixture: icon ({col},{row}) should be transparent");
                Assert.AreEqual(0, GfxPixel(bus, tlx + col, tly + row),
                    $"color-key blit must leave transparent icon cell ({col},{row}) as background");
            }

            // A pixel OUTSIDE the 16x16 stamp box must be untouched (bounded blit).
            Assert.AreEqual(0, GfxPixel(bus, tlx + 16, tly),
                "TURTLE_DRAW must not write past the 16px stamp width");
            Assert.AreEqual(0, GfxPixel(bus, tlx, tly + 16),
                "TURTLE_DRAW must not write past the 16px stamp height");
        }

        // --- $B1 then $B2: DRAW saves the background, ERASE restores it exactly. ---
        [TestMethod]
        public void Axis2_TurtleDrawThenErase_RestoresBackground()
        {
            using var bus = MakeAxis2Bus();

            ClearGfxPlane(bus);

            const int cx = 160, cy = 80;
            int tlx = cx - TURTLE_HALF, tly = cy - TURTLE_HALF;

            // Paint a distinct, non-uniform background under the whole 16x16 stamp
            // so the round-trip is a real test (not just "all zero").
            byte[,] bg = new byte[16, 16];
            for (int row = 0; row < 16; row++)
                for (int col = 0; col < 16; col++)
                {
                    byte v = (byte)(((row * 3 + col + 1) & 0x0F));   // 0..15, mostly nonzero
                    bg[row, col] = v;
                    Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx,
                        (tly + row) * VgcConstants.GfxWidth + (tlx + col), v), "bg setup write");
                }

            RunFn(bus, TUR_INIT);

            SetArg(bus, ARG0, cx);
            SetArg(bus, ARG1, cy);
            SetArg(bus, ARG2, 0);
            RunFnPumped(bus, TUR_DRAW);   // saves bg, stamps icon

            // The icon must have overwritten at least the apex cell (sanity: the
            // draw really happened before we erase).
            Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, tlx + 8, tly + 1),
                "DRAW should have stamped the apex before ERASE");

            RunFn(bus, TUR_ERASE);        // restore the saved background

            for (int row = 0; row < 16; row++)
                for (int col = 0; col < 16; col++)
                    Assert.AreEqual(bg[row, col], GfxPixel(bus, tlx + col, tly + row),
                        $"ERASE must restore the saved background at ({col},{row})");
        }

        // --- $B0 resets bg_saved: INIT after a DRAW makes a following ERASE a
        // no-op (proves INIT clears the bookkeeping so a stale bg is never
        // restored onto a freshly cleared plane). ---
        [TestMethod]
        public void Axis2_TurtleInit_ResetsBgSaved_MakesEraseNoop()
        {
            using var bus = MakeAxis2Bus();

            ClearGfxPlane(bus);
            RunFn(bus, TUR_INIT);

            const int cx = 100, cy = 90;
            SetArg(bus, ARG0, cx);
            SetArg(bus, ARG1, cy);
            SetArg(bus, ARG2, 0);
            RunFnPumped(bus, TUR_DRAW);   // bg_saved := 1, icon stamped

            // INIT again -> bg_saved := 0 (the CLEARSCREEN re-init hook).
            RunFn(bus, TUR_INIT);

            // Snapshot the whole gfx plane region around the stamp.
            int tlx = cx - TURTLE_HALF, tly = cy - TURTLE_HALF;
            byte[,] before = new byte[16, 16];
            for (int row = 0; row < 16; row++)
                for (int col = 0; col < 16; col++)
                    before[row, col] = GfxPixel(bus, tlx + col, tly + row);

            RunFn(bus, TUR_ERASE);        // must be a no-op (bg_saved == 0)

            for (int row = 0; row < 16; row++)
                for (int col = 0; col < 16; col++)
                    Assert.AreEqual(before[row, col], GfxPixel(bus, tlx + col, tly + row),
                        $"ERASE after INIT must NOT touch the plane at ({col},{row}) (bg_saved cleared)");
        }

        // --- $B1 honors VSPRITE_ROTANGLE: angle 0 vs angle 64 (90deg) produce
        // different white-pixel patterns on the plane. ---
        [TestMethod]
        public void Axis2_TurtleDraw_Rotated_DiffersFromUnrotated()
        {
            const int cx = 160, cy = 100;
            int tlx = cx - TURTLE_HALF, tly = cy - TURTLE_HALF;

            // Capture the 16x16 stamp white-mask for a given angle on a fresh bus.
            bool[,] DrawAndCapture(byte angle)
            {
                using var bus = MakeAxis2Bus();
                ClearGfxPlane(bus);
                RunFn(bus, TUR_INIT);
                SetArg(bus, ARG0, cx);
                SetArg(bus, ARG1, cy);
                SetArg(bus, ARG2, angle);
                RunFnPumped(bus, TUR_DRAW);
                var mask = new bool[16, 16];
                for (int row = 0; row < 16; row++)
                    for (int col = 0; col < 16; col++)
                        mask[row, col] = GfxPixel(bus, tlx + col, tly + row) == TURTLE_COL_WHITE;
                return mask;
            }

            bool[,] up = DrawAndCapture(0);
            bool[,] rot = DrawAndCapture(64);   // 90 degrees

            // The two patterns must differ somewhere (rotation honored).
            bool differs = false;
            for (int row = 0; row < 16 && !differs; row++)
                for (int col = 0; col < 16; col++)
                    if (up[row, col] != rot[row, col]) { differs = true; break; }
            Assert.IsTrue(differs,
                "angle 64 (90deg) must produce a different stamp than angle 0 (VSPRITE_ROTANGLE honored)");

            // And each pattern must actually have white pixels (the draw landed).
            int upCount = 0, rotCount = 0;
            for (int row = 0; row < 16; row++)
                for (int col = 0; col < 16; col++)
                {
                    if (up[row, col]) upCount++;
                    if (rot[row, col]) rotCount++;
                }
            Assert.IsTrue(upCount > 0, "angle-0 stamp must have white pixels");
            Assert.IsTrue(rotCount > 0, "angle-64 stamp must have white pixels");
        }

        // --- $B1 center clamp: a turtle near a corner clamps its 16x16 stamp to
        // the plane (set_turtle_pos @x_zero/@x_max/@y_zero/@y_max branches). Each
        // probe is white ONLY at the clamped top-left — an unclamped (naive cx-8)
        // stamp would put that exact gfx pixel off-plane or on a transparent cell,
        // so a white reading proves the clamp engaged. ---
        [TestMethod]
        public void Axis2_TurtleDraw_ClampsStampToPlaneEdges()
        {
            // Top-left corner: center (2,2) -> naive TL (-6,-6) clamps to (0,0).
            using (var bus = MakeAxis2Bus())
            {
                ClearGfxPlane(bus);
                RunFn(bus, TUR_INIT);
                SetArg(bus, ARG0, 2);
                SetArg(bus, ARG1, 2);
                SetArg(bus, ARG2, 0);
                RunFnPumped(bus, TUR_DRAW);

                // Stamp clamped to TL (0,0): apex icon(8,1) -> gfx(8,1);
                // base-bar icon(1,14) -> gfx(1,14). At the naive TL (-6,-6) the
                // (1,14) probe would be icon(7,20) = off-icon = transparent.
                Assert.AreEqual(TURTLE_COL_WHITE, TurtleIcon(8, 1), "fixture: icon(8,1) white");
                Assert.AreEqual(TURTLE_COL_WHITE, TurtleIcon(1, 14), "fixture: icon(1,14) white");
                Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, 8, 1),
                    "low-corner draw must clamp stamp top-left to (0,0): apex at gfx(8,1)");
                Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, 1, 14),
                    "low-corner draw must clamp to (0,0): base-bar pixel at gfx(1,14) (off-plane if unclamped)");
            }

            // Bottom-right corner: center (318,198) -> naive TL (310,190) clamps to
            // (TURTLE_MAX_X=304, TURTLE_MAX_Y=184) so the 16x16 stamp fits exactly.
            using (var bus = MakeAxis2Bus())
            {
                ClearGfxPlane(bus);
                RunFn(bus, TUR_INIT);
                SetArg(bus, ARG0, 318);
                SetArg(bus, ARG1, 198);
                SetArg(bus, ARG2, 0);
                RunFnPumped(bus, TUR_DRAW);

                // Stamp clamped to TL (304,184): apex icon(8,1) -> gfx(312,185);
                // base corner icon(14,14) -> gfx(318,198). At the naive TL (310,190)
                // gfx(318,198) would be icon(8,8) = transparent, so white proves clamp.
                Assert.AreEqual(TURTLE_COL_WHITE, TurtleIcon(14, 14), "fixture: icon(14,14) white");
                Assert.AreEqual(0, TurtleIcon(8, 8), "fixture: icon(8,8) transparent (the unclamped probe)");
                Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, 312, 185),
                    "high-corner draw must clamp stamp top-left to (304,184): apex at gfx(312,185)");
                Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, 318, 198),
                    "high-corner draw must clamp to (304,184): base corner at gfx(318,198) (transparent if unclamped)");
            }
        }

        // =====================================================================
        // Turtle command engine ($B3 TUR_OP), batch 4c.2-3. The whole
        // NovaLogo turtle (move-math + render + reporters) now lives IN the
        // module. Op rides in ARG2 byte0; value args in ARG0/ARG1 as Logo 16.8
        // (cell byte0 = FRAC, byte1 = LO, byte2 = HI). Move-math updates the
        // turtle state at $9F00 and renders through the module's own vgc.s +
        // $B0/$B1/$B2 render subs. Move/draw ops frame-wait inside the
        // rotate-blit, so they are driven via RunFnPumped.
        // =====================================================================

        // Set a Logo 16.8 integer value (frac=0) into an ARG cell: byte0=FRAC=0,
        // byte1=LO, byte2=HI. SetArg(cell, n<<8) lays it out exactly.
        private static void SetTurtleArg(CompositeBusDevice bus, ushort cell, int intValue) =>
            SetArg(bus, cell, (intValue & 0xFFFF) << 8);

        // Run a turtle op via TUR_OP (pumped — DRAW/move ops frame-wait).
        private static void RunTurtleOp(CompositeBusDevice bus, byte op)
        {
            SetArg(bus, ARG2, op);        // op id in ARG2 byte0
            RunFnPumped(bus, TUR_OP);
        }

        private static int Read16(CompositeBusDevice bus, ushort lo) =>
            bus.ReadRam(lo) | (bus.ReadRam((ushort)(lo + 1)) << 8);

        // Read the LIB_RESULT low word as a Logo-16.8 integer (frac=byte0 ignored).
        private static int ResultInt(CompositeBusDevice bus) =>
            bus.ReadRam((ushort)(RESULT + 1)) | (bus.ReadRam((ushort)(RESULT + 2)) << 8);

        // ResultInt sign-extended (coordinates are signed: left/below center are negative).
        private static int ResultS16(CompositeBusDevice bus) => (short)ResultInt(bus);

        // Run a reporter op (synchronous — reporters never frame-wait) and return
        // the signed 16-bit value. Used for XCOR/YCOR which can be negative.
        private static int Report(CompositeBusDevice bus, byte op)
        {
            SetArg(bus, ARG2, op);
            RunFn(bus, TUR_OP);
            return ResultS16(bus);
        }

        // --- $B3 op dispatch is wired (not gfn_unimpl): a command op returns OK. ---
        [TestMethod]
        public void Axis2_TurtleOp_DispatchesOk()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_PD);     // RunFnPumped asserts RTS + STATUS == LERR_OK
        }

        // --- FD at heading 0 (north) moves Y by exactly -distance (the cardinal
        // move must be exact — sin(0)=0 so X unchanged, cos(0)=+1 so dy=-dist).
        // Mirrors NovaLogoTests.TurtleCardinalForwardUsesFullDistance. ---
        [TestMethod]
        public void Axis2_TurtleFd_North_MovesYByFullDistance()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);     // init + center + heading 0

            int x0 = Read16(bus, TS_X_LO), y0 = Read16(bus, TS_Y_LO);
            Assert.AreEqual(TURTLE_CENTER_X, x0, "CS must center X at 160");
            Assert.AreEqual(TURTLE_CENTER_Y, y0, "CS must center Y at 100");

            SetTurtleArg(bus, ARG0, 50);  // FD 50
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(x0, Read16(bus, TS_X_LO),
                "FD at heading 0 must leave X unchanged (sin 0 = 0)");
            Assert.AreEqual(y0 - 50, Read16(bus, TS_Y_LO),
                "FD 50 at heading 0 must move Y by exactly -50 (cardinal exact)");
            Assert.AreEqual(0, bus.ReadRam(TS_Y_FRAC),
                "cardinal FD must leave the Y fraction at 0");
        }

        // --- FD as the FIRST turtle op (no prior CS): ensure_gfx_mode runs the
        // first-entry draw_turtle, which borrows the mailbox; the distance must
        // still survive (args-snapshot fix). FD 50 north from the center -> Y-50. ---
        [TestMethod]
        public void Axis2_TurtleFd_FirstOpNoCs_DistanceSurvivesGfxEntry()
        {
            using var bus = MakeAxis2Bus();
            // No CS: TURTLE_INITED / GFX_VISIBLE are 0, so the very first FD goes
            // through ensure_gfx_mode -> turtle_init_state -> enter_split -> draw_turtle
            // BEFORE the move-math reads the distance from ARG0.
            SetTurtleArg(bus, ARG0, 50);
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(1, bus.ReadRam(TS_INITED), "first FD must auto-init the turtle");
            Assert.AreEqual(TURTLE_CENTER_X, Read16(bus, TS_X_LO),
                "first FD at heading 0 leaves X centered");
            Assert.AreEqual(TURTLE_CENTER_Y - 50, Read16(bus, TS_Y_LO),
                "first FD 50 must move the full distance despite the gfx-entry draw");
        }

        // --- FD at heading 90 (east, after RT 90) moves X by +distance exactly. ---
        [TestMethod]
        public void Axis2_TurtleRt90ThenFd_East_MovesXByFullDistance()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            int x0 = Read16(bus, TS_X_LO), y0 = Read16(bus, TS_Y_LO);

            SetTurtleArg(bus, ARG0, 90);  // RT 90 -> heading 90 (east)
            RunTurtleOp(bus, TOP_RT);
            Assert.AreEqual(90, Read16(bus, TS_HEADING_LO), "RT 90 must set heading to 90");

            SetTurtleArg(bus, ARG0, 40);  // FD 40
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(x0 + 40, Read16(bus, TS_X_LO),
                "FD 40 at heading 90 must move X by exactly +40 (cardinal exact)");
            Assert.AreEqual(y0, Read16(bus, TS_Y_LO),
                "FD at heading 90 must leave Y unchanged (cos 90 = 0)");
        }

        // --- BK is FD with a negated distance: BK 30 at heading 0 moves Y by +30. ---
        [TestMethod]
        public void Axis2_TurtleBk_North_MovesYByPositiveDistance()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            int y0 = Read16(bus, TS_Y_LO);

            SetTurtleArg(bus, ARG0, 30);  // BK 30
            RunTurtleOp(bus, TOP_BK);

            Assert.AreEqual(y0 + 30, Read16(bus, TS_Y_LO),
                "BK 30 at heading 0 must move Y by +30 (forward negated)");
        }

        // --- RT / LT change the heading; LT from 0 wraps via +360. ---
        [TestMethod]
        public void Axis2_TurtleRtLt_ChangeHeading()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 270);
            RunTurtleOp(bus, TOP_RT);
            Assert.AreEqual(270, Read16(bus, TS_HEADING_LO), "RT 270 -> heading 270");

            SetTurtleArg(bus, ARG0, 30);
            RunTurtleOp(bus, TOP_RT);     // 270 + 30 = 300
            Assert.AreEqual(300, Read16(bus, TS_HEADING_LO), "RT past 360 wraps: 270+30 -> 300");

            SetTurtleArg(bus, ARG0, 90);
            RunTurtleOp(bus, TOP_RT);     // 300 + 90 = 390 -> 30
            Assert.AreEqual(30, Read16(bus, TS_HEADING_LO), "RT 90 from 300 wraps mod 360 -> 30");

            SetTurtleArg(bus, ARG0, 60);
            RunTurtleOp(bus, TOP_LT);     // 30 - 60 = -30 -> +360 -> 330
            Assert.AreEqual(330, Read16(bus, TS_HEADING_LO),
                "LT 60 from 30 wraps negative via +360 -> 330");
        }

        // --- SETXY sets the integer position; SETH sets the heading mod 360. ---
        [TestMethod]
        public void Axis2_TurtleSetxyAndSeth_SetState()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 50);    // SETXY 50 30 (centered Logo coords)
            SetTurtleArg(bus, ARG1, 30);
            RunTurtleOp(bus, TOP_SETXY);
            Assert.AreEqual(210, Read16(bus, TS_X_LO), "SETXY x=50 -> internal screen X = 50+160");
            Assert.AreEqual(70, Read16(bus, TS_Y_LO), "SETXY y=30 -> internal screen Y = 100-30");

            SetTurtleArg(bus, ARG0, 45);    // SETH 45
            RunTurtleOp(bus, TOP_SETH);
            Assert.AreEqual(45, Read16(bus, TS_HEADING_LO), "SETH must set heading");

            SetTurtleArg(bus, ARG0, 400);   // SETH 400 -> 40 (mod 360)
            RunTurtleOp(bus, TOP_SETH);
            Assert.AreEqual(40, Read16(bus, TS_HEADING_LO), "SETH 400 must normalize mod 360 -> 40");
        }

        // --- SETX / SETY set only one axis. ---
        [TestMethod]
        public void Axis2_TurtleSetxSety_SetOneAxis()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            int y0 = Read16(bus, TS_Y_LO);

            SetTurtleArg(bus, ARG0, 55);
            RunTurtleOp(bus, TOP_SETX);
            Assert.AreEqual(215, Read16(bus, TS_X_LO), "SETX 55 -> internal screen X = 55+160");
            Assert.AreEqual(y0, Read16(bus, TS_Y_LO), "SETX must leave Y unchanged");

            int x1 = Read16(bus, TS_X_LO);
            SetTurtleArg(bus, ARG0, 33);
            RunTurtleOp(bus, TOP_SETY);
            Assert.AreEqual(67, Read16(bus, TS_Y_LO), "SETY 33 -> internal screen Y = 100-33");
            Assert.AreEqual(x1, Read16(bus, TS_X_LO), "SETY must leave X unchanged");
        }

        // --- HOME returns to center, heading 0, after moving away. ---
        [TestMethod]
        public void Axis2_TurtleHome_ResetsToCenter()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 30);
            RunTurtleOp(bus, TOP_RT);
            SetTurtleArg(bus, ARG0, 25);
            RunTurtleOp(bus, TOP_FD);     // move off-center
            Assert.AreNotEqual(TURTLE_CENTER_X, Read16(bus, TS_X_LO), "fixture: FD moved off center");

            RunTurtleOp(bus, TOP_HOME);
            Assert.AreEqual(TURTLE_CENTER_X, Read16(bus, TS_X_LO), "HOME must recenter X to 160");
            Assert.AreEqual(TURTLE_CENTER_Y, Read16(bus, TS_Y_LO), "HOME must recenter Y to 100");
            Assert.AreEqual(0, Read16(bus, TS_HEADING_LO), "HOME must reset heading to 0");
        }

        // --- Pen-down FD draws a line on the gfx plane between old and new pos. ---
        [TestMethod]
        public void Axis2_TurtlePenDownFd_DrawsLineOnPlane()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);     // pen down + turtle SHOWN by default, gfx cleared

            int cx = TURTLE_CENTER_X, cy = TURTLE_CENTER_Y;
            SetTurtleArg(bus, ARG0, 40);  // FD 40 north -> vertical line cy..cy-40 at x=cx
            RunTurtleOp(bus, TOP_FD);

            // Turtle stays SHOWN: this proves turtle_render erases the old turtle
            // BEFORE the line, so the BOB restore-bg can't eat the line where it
            // passes under the old footprint. cy-1 (=79) is inside the old turtle's
            // 16x16 box (rows 72..87) — with the erase-after-line bug it reads 0.
            Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, cx, cy - 20),
                "pen-down FD must draw the pen line on the gfx plane");
            Assert.AreEqual(TURTLE_COL_WHITE, GfxPixel(bus, cx, cy - 1),
                "pen line must survive the turtle's BOB erase at the near endpoint (under the old footprint)");
        }

        // --- Pen-up FD does NOT draw a line. PU/PD toggle TURTLE_PEN. ---
        [TestMethod]
        public void Axis2_TurtlePenUpFd_DrawsNoLine_AndPenToggles()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            RunTurtleOp(bus, TOP_HT);     // hide turtle so only the (absent) line matters

            RunTurtleOp(bus, TOP_PU);
            Assert.AreEqual(0x01, bus.ReadRam(TS_PEN), "PU must set TURTLE_PEN = 1 (up)");

            int cx = TURTLE_CENTER_X, cy = TURTLE_CENTER_Y;
            SetTurtleArg(bus, ARG0, 40);
            RunTurtleOp(bus, TOP_FD);     // pen up -> no line
            Assert.AreEqual(0, GfxPixel(bus, cx, cy - 20),
                "pen-up FD must NOT draw a line on the gfx plane");

            RunTurtleOp(bus, TOP_PD);
            Assert.AreEqual(0x00, bus.ReadRam(TS_PEN), "PD must set TURTLE_PEN = 0 (down)");
        }

        // --- SETPC sets the pen color; a subsequent pen-down move draws in it. ---
        [TestMethod]
        public void Axis2_TurtleSetpc_SetsPenColor()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            RunTurtleOp(bus, TOP_HT);

            SetArg(bus, ARG0, 7);         // SETPC 7 (color byte in ARG0 byte0)
            RunTurtleOp(bus, TOP_SETPC);
            Assert.AreEqual(7, bus.ReadRam(TS_COLOR), "SETPC must set TURTLE_COLOR");

            int cx = TURTLE_CENTER_X, cy = TURTLE_CENTER_Y;
            SetTurtleArg(bus, ARG0, 30);
            RunTurtleOp(bus, TOP_FD);
            Assert.AreEqual(7, GfxPixel(bus, cx, cy - 15),
                "pen line after SETPC 7 must be drawn in color 7");
        }

        // --- XCOR / YCOR / HEADING reporters return the $9F00 state via LIB_RESULT. ---
        [TestMethod]
        public void Axis2_TurtleReporters_XcorYcorHeading()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 123);
            SetTurtleArg(bus, ARG1, 45);
            RunTurtleOp(bus, TOP_SETXY);
            SetTurtleArg(bus, ARG0, 200);
            RunTurtleOp(bus, TOP_SETH);

            SetArg(bus, ARG2, TOP_XCOR);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(123, ResultInt(bus), "XCOR must report the turtle X (123)");

            SetArg(bus, ARG2, TOP_YCOR);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(45, ResultInt(bus), "YCOR must report the turtle Y (45)");

            SetArg(bus, ARG2, TOP_HEADING);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(200, ResultInt(bus), "HEADING must report the heading (200)");
        }

        // --- PENDOWN? / SHOWN? boolean reporters. ---
        [TestMethod]
        public void Axis2_TurtleReporters_PendownpShownp()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);     // pen down + shown

            SetArg(bus, ARG2, TOP_PENDOWNP);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(1, ResultInt(bus), "PENDOWN? must be 1 when pen is down");

            RunTurtleOp(bus, TOP_PU);
            SetArg(bus, ARG2, TOP_PENDOWNP);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(0, ResultInt(bus), "PENDOWN? must be 0 when pen is up");

            SetArg(bus, ARG2, TOP_SHOWNP);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(1, ResultInt(bus), "SHOWN? must be 1 after CS (turtle shown)");

            RunTurtleOp(bus, TOP_HT);
            SetArg(bus, ARG2, TOP_SHOWNP);
            RunFn(bus, TUR_OP);
            Assert.AreEqual(0, ResultInt(bus), "SHOWN? must be 0 after HT (turtle hidden)");
        }

        // --- TOWARDS reports a LOGO HEADING toward a target (north=0, east=90,
        // clockwise) so SETHEADING TOWARDS [x y] points the turtle at the target,
        // exactly like UCBLogo/FMSLogo. Targets are centered Logo coords. ---
        [TestMethod]
        public void Axis2_TurtleTowards_ReportsLogoHeading()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);     // turtle at origin (0,0)

            // Due east (target to the right) -> Logo heading 90.
            SetTurtleArg(bus, ARG0, 100);
            SetTurtleArg(bus, ARG1, 0);
            SetArg(bus, ARG2, TOP_TOWARDS);
            RunFn(bus, TUR_OP);
            int east = ResultInt(bus);
            Assert.IsTrue(east >= 88 && east <= 92, $"TOWARDS due east must be ~90 (Logo heading) (got {east})");

            // Due north (target above) -> Logo heading 0.
            SetTurtleArg(bus, ARG0, 0);
            SetTurtleArg(bus, ARG1, 80);
            SetArg(bus, ARG2, TOP_TOWARDS);
            RunFn(bus, TUR_OP);
            int north = ResultInt(bus);
            Assert.IsTrue(north <= 2 || north >= 358, $"TOWARDS due north must be ~0 (Logo heading) (got {north})");

            // Due south (target below) -> Logo heading 180.
            SetTurtleArg(bus, ARG0, 0);
            SetTurtleArg(bus, ARG1, unchecked((short)-80));
            SetArg(bus, ARG2, TOP_TOWARDS);
            RunFn(bus, TUR_OP);
            int south = ResultInt(bus);
            Assert.IsTrue(south >= 178 && south <= 182, $"TOWARDS due south must be ~180 (Logo heading) (got {south})");

            // Due west (target to the left) -> Logo heading 270.
            SetTurtleArg(bus, ARG0, unchecked((short)-100));
            SetTurtleArg(bus, ARG1, 0);
            SetArg(bus, ARG2, TOP_TOWARDS);
            RunFn(bus, TUR_OP);
            int west = ResultInt(bus);
            Assert.IsTrue(west >= 268 && west <= 272, $"TOWARDS due west must be ~270 (Logo heading) (got {west})");
        }

        // --- SETXY x y: generic numeric move (the module's only positioning op).
        //     SETPOS [x y]'s former list-unpack now lives in the Logo adapter
        //     (it parses the cons list and drives THIS op); the module itself is
        //     100% numeric and no longer understands a Logo list — so this asserts
        //     the numeric SETXY contract that SETPOS routes through. ---
        [TestMethod]
        public void Axis2_TurtleSetxy_MovesToCoords()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 137);   // x = 137 (centered Logo, frac=0)
            SetTurtleArg(bus, ARG1, 64);    // y = 64
            RunTurtleOp(bus, TOP_SETXY);

            Assert.AreEqual(297, Read16(bus, TS_X_LO), "SETXY x=137 -> internal screen X = 137+160");
            Assert.AreEqual(36, Read16(bus, TS_Y_LO), "SETXY y=64 -> internal screen Y = 100-64");
        }

        // =====================================================================
        // Logo coordinate convention: origin (0,0) at screen CENTER, +Y up, +X
        // right. HOME reports (0,0); FD north reports positive YCOR; RT 90 + FD
        // reports positive XCOR; SETXY takes centered coords. Verified against
        // UCBLogo/FMSLogo/Terrapin/Python-logo-mode. The internal $9F00 state
        // stays screen-pixel (home = 160,100); only the op boundary translates.
        // =====================================================================

        // --- HOME / CS leave the turtle at the origin: XCOR = 0, YCOR = 0. ---
        [TestMethod]
        public void Axis2_TurtleHome_ReportsOrigin()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            Assert.AreEqual(0, Report(bus, TOP_XCOR), "at home XCOR must be 0 (origin at screen center)");
            Assert.AreEqual(0, Report(bus, TOP_YCOR), "at home YCOR must be 0 (origin at screen center)");
        }

        // --- FD north from home reports POSITIVE YCOR (+Y is up in Logo). ---
        [TestMethod]
        public void Axis2_TurtleFdNorth_ReportsPositiveYcor()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 50);  // FD 50 at heading 0 (north)
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(0, Report(bus, TOP_XCOR), "FD north leaves XCOR at 0");
            Assert.AreEqual(50, Report(bus, TOP_YCOR), "FD 50 north must report YCOR +50 (up is positive)");
        }

        // --- RT 90 + FD reports POSITIVE XCOR (+X is right). ---
        [TestMethod]
        public void Axis2_TurtleRt90Fd_ReportsPositiveXcor()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 90);
            RunTurtleOp(bus, TOP_RT);
            SetTurtleArg(bus, ARG0, 50);
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(50, Report(bus, TOP_XCOR), "RT 90 + FD 50 must report XCOR +50 (right is positive)");
            Assert.AreEqual(0, Report(bus, TOP_YCOR), "FD east leaves YCOR at 0");
        }

        // --- SETXY takes centered coords: SETXY -50 50 goes left-and-up, reports
        //     them back, and lands at the matching screen pixel (110, 50). ---
        [TestMethod]
        public void Axis2_TurtleSetxyCentered_ReportsAndPositions()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, unchecked((short)-50));  // SETXY -50 50
            SetTurtleArg(bus, ARG1, 50);
            RunTurtleOp(bus, TOP_SETXY);

            Assert.AreEqual(-50, Report(bus, TOP_XCOR), "SETXY -50 must report XCOR -50");
            Assert.AreEqual(50, Report(bus, TOP_YCOR), "SETXY 50 must report YCOR 50");
            // Internal screen pixel: sx = -50 + 160 = 110, sy = 100 - 50 = 50.
            Assert.AreEqual(110, Read16(bus, TS_X_LO), "internal screen X = lx + 160");
            Assert.AreEqual(50, Read16(bus, TS_Y_LO), "internal screen Y = 100 - ly");
        }

        // --- SETX / SETY take centered coords on one axis. ---
        [TestMethod]
        public void Axis2_TurtleSetxSetyCentered_Report()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, unchecked((short)-30));
            RunTurtleOp(bus, TOP_SETX);
            Assert.AreEqual(-30, Report(bus, TOP_XCOR), "SETX -30 must report XCOR -30");
            Assert.AreEqual(0, Report(bus, TOP_YCOR), "SETX must leave YCOR unchanged (0)");

            SetTurtleArg(bus, ARG0, unchecked((short)-40));
            RunTurtleOp(bus, TOP_SETY);
            Assert.AreEqual(-40, Report(bus, TOP_YCOR), "SETY -40 must report YCOR -40");
            Assert.AreEqual(-30, Report(bus, TOP_XCOR), "SETY must leave XCOR unchanged (-30)");
        }

        // =====================================================================
        // WRAP (default edge behavior): moving past an edge reappears on the
        // opposite edge. The stored screen position is reduced mod 320 x 200, so
        // reporters always return in-range values, and pen lines that cross an
        // edge are drawn wrapped (the tile-shift draws the opposite-side segment).
        // =====================================================================

        // --- FD past the top edge wraps Y to the bottom of the plane. ---
        [TestMethod]
        public void Axis2_TurtleWrapNorth_WrapsYIntoRange()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 99);     // SETY 99 -> screen y = 1 (one below the top edge)
            RunTurtleOp(bus, TOP_SETY);
            Assert.AreEqual(1, Read16(bus, TS_Y_LO), "fixture: SETY 99 -> screen y 1");

            SetTurtleArg(bus, ARG0, 5);      // FD 5 north -> unwrapped screen y = -4 -> wrap 196
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(196, Read16(bus, TS_Y_LO), "FD past the top edge wraps screen Y to 200-4");
            Assert.AreEqual(-96, Report(bus, TOP_YCOR), "wrapped YCOR = 100 - 196 = -96");
        }

        // --- FD past the right edge wraps X to the left of the plane. ---
        [TestMethod]
        public void Axis2_TurtleWrapEast_WrapsXIntoRange()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);

            SetTurtleArg(bus, ARG0, 159);    // SETX 159 -> screen x = 319 (right edge)
            RunTurtleOp(bus, TOP_SETX);
            Assert.AreEqual(319, Read16(bus, TS_X_LO), "fixture: SETX 159 -> screen x 319");

            SetTurtleArg(bus, ARG0, 90);
            RunTurtleOp(bus, TOP_RT);
            SetTurtleArg(bus, ARG0, 5);      // FD 5 east -> unwrapped screen x = 324 -> wrap 4
            RunTurtleOp(bus, TOP_FD);

            Assert.AreEqual(4, Read16(bus, TS_X_LO), "FD past the right edge wraps screen X to 320+4-320");
            Assert.AreEqual(-156, Report(bus, TOP_XCOR), "wrapped XCOR = 4 - 160 = -156");
        }

        // --- A pen-down move across the top edge draws BOTH segments: the part up
        //     to the edge AND the wrapped part at the opposite edge. ---
        [TestMethod]
        public void Axis2_TurtleWrapPenLine_DrawsOnBothEdges()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            RunTurtleOp(bus, TOP_HT);        // hide the icon so only the pen line marks the plane
            RunTurtleOp(bus, TOP_PU);
            SetTurtleArg(bus, ARG0, 99);     // move to screen y=1 (no line, pen up)
            RunTurtleOp(bus, TOP_SETY);
            RunTurtleOp(bus, TOP_PD);

            SetTurtleArg(bus, ARG0, 5);      // FD 5 north across the top edge (pen down)
            RunTurtleOp(bus, TOP_FD);

            Assert.AreNotEqual(0, GfxPixel(bus, 160, 0),
                "pen line must mark the top edge (the pre-wrap segment)");
            Assert.AreNotEqual(0, GfxPixel(bus, 160, 198),
                "pen line must mark the bottom edge (the wrapped segment)");
        }

        // --- ST after HT re-shows the turtle (TURTLE_SHOWN toggles). ---
        [TestMethod]
        public void Axis2_TurtleStHt_ToggleShown()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            Assert.AreEqual(0x01, bus.ReadRam(TS_SHOWN), "CS must leave the turtle shown");

            RunTurtleOp(bus, TOP_HT);
            Assert.AreEqual(0x00, bus.ReadRam(TS_SHOWN), "HT must hide the turtle");

            RunTurtleOp(bus, TOP_ST);
            Assert.AreEqual(0x01, bus.ReadRam(TS_SHOWN), "ST must re-show the turtle");
        }

        // --- An unknown turtle op (outside $10..$29) is a harmless no-op OK. ---
        [TestMethod]
        public void Axis2_TurtleOp_UnknownOp_IsOkNoop()
        {
            using var bus = MakeAxis2Bus();
            RunTurtleOp(bus, TOP_CS);
            int x0 = Read16(bus, TS_X_LO);

            SetArg(bus, ARG2, 0x00);      // op < $10 -> no-op
            RunFn(bus, TUR_OP);    // RunFn asserts STATUS == OK
            Assert.AreEqual(x0, Read16(bus, TS_X_LO), "unknown op must not move the turtle");

            SetArg(bus, ARG2, 0x7F);      // op past the table -> no-op
            RunFn(bus, TUR_OP);
            Assert.AreEqual(x0, Read16(bus, TS_X_LO), "out-of-range op must not move the turtle");
        }

        // --- Dispatch density: an id at/above TUR_FN_COUNT ($04) returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_AboveTurtleRange_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(STATUS, 0x00);   // poison opposite to expected non-OK
            RunFnRaw(bus, 0x04);          // first id past the last turtle fn (TUR_OP $03)
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $04 (>= TUR_FN_COUNT) must report LERR_NO_FN via the bounds check");
        }

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
