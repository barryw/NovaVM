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
        // Text/mode-domain fn-ids ($10-$1B), batch 4b.3.
        private const byte   GFN_COLOR = 0x10, GFN_FONT = 0x11, GFN_MODE = 0x12,
                             GFN_REVERSE = 0x13, GFN_REVERSEOFF = 0x14, GFN_FLASH = 0x15,
                             GFN_FLASHOFF = 0x16, GFN_LOCATE = 0x17, GFN_CLS = 0x18,
                             GFN_CLSWIN = 0x19, GFN_DISPLAYON = 0x1A, GFN_DISPLAYOFF = 0x1B;
        // Hardware-sprite-domain fn-ids ($20-$3B), batch 4b.4.
        private const byte   GFN_SPR_DEFINE = 0x20, GFN_SPR_ROW = 0x21, GFN_SPR_CLEAR = 0x22,
                             GFN_SPR_COPY = 0x23, GFN_SPR_POS = 0x24, GFN_SPR_ENABLE = 0x25,
                             GFN_SPR_DISABLE = 0x26, GFN_SPR_FLIP = 0x27, GFN_SPR_PRIORITY = 0x28,
                             GFN_SPR_SHAPE = 0x29, GFN_SPR_SETREG = 0x2A, GFN_SPR_SETREG16 = 0x2B,
                             GFN_SPR_GETX = 0x2C, GFN_SPR_GETY = 0x2D,
                             GFN_SPR_COLL_STATUS = 0x30, GFN_SPR_COLL_MASK = 0x31,
                             GFN_SPR_COLL_CLEAR = 0x32, GFN_SPR_COLL_READCLEAR = 0x33,
                             GFN_SPR_COLL_IRQON = 0x34, GFN_SPR_COLL_IRQOFF = 0x35,
                             GFN_SPR_BG_STATUS = 0x36, GFN_SPR_BG_MASK = 0x37,
                             GFN_SPR_BG_CLEAR = 0x38, GFN_SPR_BG_READCLEAR = 0x39,
                             GFN_SPR_BG_IRQON = 0x3A, GFN_SPR_BG_IRQOFF = 0x3B;
        // Copper-domain fn-ids ($40-$49), batch 4b.5.
        private const byte   GFN_COPPER_LIST = 0x40, GFN_COPPER_ADD = 0x41,
                             GFN_COPPER_CLEAR = 0x42, GFN_COPPER_ON = 0x43,
                             GFN_COPPER_OFF = 0x44, GFN_COPPER_USE = 0x45,
                             GFN_COPPER_END = 0x46, GFN_COPPER_SPLIT = 0x47,
                             GFN_COPPER_SET_REG = 0x48, GFN_COPPER_SET_SPRITE_REG = 0x49;
        // Blit/DMA-domain fn-ids ($50-$5B), batch 4b.6.
        private const byte   GFN_BLITCOPY = 0x50, GFN_BLITFILL = 0x51,
                             GFN_BLIT_START = 0x52, GFN_BLIT_WAIT = 0x53,
                             GFN_DMACOPY = 0x54, GFN_DMAFILL = 0x55,
                             GFN_BLIT_STATUS = 0x56, GFN_BLIT_ERR = 0x57,
                             GFN_BLIT_COUNT = 0x58, GFN_DMA_STATUS = 0x59,
                             GFN_DMA_ERR = 0x5A, GFN_DMA_COUNT = 0x5B,
                             GFN_BLIT_START_FILL = 0x5C;
        // Vsprite-domain fn-ids ($60-$71), batch 4b.7.
        private const byte   GFN_VS_BLIT = 0x60, GFN_VS_BLIT_START = 0x61,
                             GFN_VS_FILL = 0x62, GFN_VS_USE_ORIGINAL = 0x63,
                             GFN_VS_USE_ROTATED = 0x64, GFN_VS_ROTATE = 0x65,
                             GFN_VS_GFX_BLIT = 0x66, GFN_VS_GFX_BLIT_START = 0x67,
                             GFN_VS_GFX_FILL = 0x68, GFN_VS_GFX_SAVE_BG = 0x69,
                             GFN_VS_GFX_RESTORE_BG = 0x6A, GFN_VS_GFX_ROTATE_BLIT = 0x6B,
                             GFN_VS_GFX_ROTATE_BLIT_KEYED = 0x6C, GFN_VS_GFX_ROTATE_BLIT_NOWAIT = 0x6D,
                             GFN_VS_SCENE_BEGIN = 0x6E, GFN_VS_SCENE_COMMIT = 0x6F,
                             GFN_VS_SCENE_DRAW = 0x70, GFN_VS_SCENE_COMMIT_ATOMIC = 0x71;
        // vsprite config-struct field offsets (runtime/asm/libgraphics.inc GFX_VS_OFF_*).
        private const int VS_OFF_SRCSPACE = 0, VS_OFF_DSTSPACE = 1,
                          VS_OFF_SRCADDRL = 2, VS_OFF_SRCADDRM = 3, VS_OFF_SRCADDRH = 4,
                          VS_OFF_DSTADDRL = 5, VS_OFF_DSTADDRM = 6, VS_OFF_DSTADDRH = 7,
                          VS_OFF_WIDTHL = 8, VS_OFF_WIDTHH = 9, VS_OFF_HEIGHTL = 10, VS_OFF_HEIGHTH = 11,
                          VS_OFF_SRCSTRL = 12, VS_OFF_SRCSTRH = 13, VS_OFF_DSTSTRL = 14, VS_OFF_DSTSTRH = 15,
                          VS_OFF_BGSPACE = 16, VS_OFF_BGADDRL = 17, VS_OFF_BGADDRM = 18, VS_OFF_BGADDRH = 19,
                          VS_OFF_BGSTRL = 20, VS_OFF_BGSTRH = 21, VS_OFF_COLORKEY = 22,
                          VS_OFF_ORIGSPACE = 23, VS_OFF_ORIGADDRL = 24, VS_OFF_ORIGADDRM = 25, VS_OFF_ORIGADDRH = 26,
                          VS_OFF_ORIGSTRL = 27, VS_OFF_ORIGSTRH = 28,
                          VS_OFF_ROTSPACE = 29, VS_OFF_ROTADDRL = 30, VS_OFF_ROTADDRM = 31, VS_OFF_ROTADDRH = 32,
                          VS_OFF_ROTSTRL = 33, VS_OFF_ROTSTRH = 34, VS_OFF_ROTANGLE = 35,
                          VS_OFF_FILLVALUE = 36, VS_OFF_FLAGS = 37,
                          VS_OFF_XL = 38, VS_OFF_XH = 39, VS_OFF_Y = 40,
                          VS_OFF_SCENE_ADDRL = 41, VS_OFF_SCENE_ADDRH = 42, VS_OFF_SCENE_COUNT = 43;
        private const byte LERR_VSPRITE_FAIL = 0x84;
        // Turtle fn-ids/op-codes/state cells were extracted with the turtle into the
        // dedicated TURTLE module ($07); its tests now live in TurtleModuleTests.cs.
        // The $B4 dispatch-gap test below stays here (it asserts the GRAPHICS module
        // reports LERR_NO_FN for the now-vacated $B4 id).
        // Msprite/meta-sprite-domain fn-ids ($80-$8B), batch 4b.8.
        private const byte   GFN_MS_SPAWN = 0x80, GFN_MS_DESTROY = 0x81,
                             GFN_MS_SHOW = 0x82, GFN_MS_HIDE = 0x83,
                             GFN_MS_POS = 0x84, GFN_MS_FRAME = 0x85,
                             GFN_MS_ANIM = 0x86, GFN_MS_PRIORITY = 0x87,
                             GFN_MS_TRANSCOLOR = 0x88, GFN_MS_TICK = 0x89,
                             GFN_MS_COMMIT = 0x8A, GFN_MS_COMMIT_ONE = 0x8B;
        private const byte LERR_MSPRITE_FAIL = 0x85;
        private const byte MSPRITE_INVALID_HANDLE = 0xFF;
        // Image/mem-domain fn-ids ($A0-$A9), batch 4b.9.
        private const byte   GFN_MEMREAD = 0xA0, GFN_MEMWRITE = 0xA1,
                             GFN_VPEEK = 0xA2, GFN_VPOKE = 0xA3,
                             GFN_GSAVE = 0xA4, GFN_GLOAD = 0xA5,
                             GFN_NVGLOAD = 0xA6, GFN_NVGLOAD_AT = 0xA7,
                             GFN_NVGLOAD_NAMED = 0xA8, GFN_NVGLOAD_NAMED_AT = 0xA9;
        private const byte LERR_FILE_FAIL = 0x86;
        private const byte LERR_IMAGE_FAIL = 0x87;
        // Anim-domain fn-ids ($C0-$C7), batch 4b.11.
        private const byte   GFN_ANIM_INIT = 0xC0, GFN_ANIM_START = 0xC1,
                             GFN_ANIM_STOP = 0xC2, GFN_ANIM_TICK = 0xC3,
                             GFN_ANIM_TICK_ONE = 0xC4, GFN_ANIM_SET_FRAME = 0xC5,
                             GFN_ANIM_LOAD_XRAM = 0xC6, GFN_ANIM_LOAD_DISK = 0xC7;
        private const byte LERR_ANIM_FAIL = 0x88;
        private const byte ANIM_INVALID_HANDLE = 0xFF;
        private const byte ANIM_TARGET_SPRITE = 0x00;
        // Anim descriptor field offsets (runtime/asm/anim.inc ANIM_DESC_*).
        private const int    ANIM_DESC_FRAME_COUNT = 0, ANIM_DESC_TICKS = 1,
                             ANIM_DESC_FLAGS = 2, ANIM_DESC_FIRST = 3,
                             ANIM_DESC_STRIDE = 4, ANIM_DESC_RESERVED = 5;
        private const byte   ANIM_DESC_LOOP = 0x01;
        // Tween-domain fn-ids ($D0-$DA), batch 4b.11.
        private const byte   GFN_TWEEN_BEGIN = 0xD0, GFN_TWEEN_EVAL = 0xD1,
                             GFN_TWEEN_EVAL_LINEAR = 0xD2, GFN_TWEEN_EVAL_EASE_IN = 0xD3,
                             GFN_TWEEN_EVAL_EASE_OUT = 0xD4, GFN_TWEEN_EVAL_EASE_IN_OUT = 0xD5,
                             GFN_TWEEN_STEP = 0xD6, GFN_TWEEN_STEP_LINEAR = 0xD7,
                             GFN_TWEEN_STEP_EASE_IN = 0xD8, GFN_TWEEN_STEP_EASE_OUT = 0xD9,
                             GFN_TWEEN_STEP_EASE_IN_OUT = 0xDA;
        // Tween easing modes (runtime/asm/tween.inc TWEEN_MODE_*).
        private const byte   TWEEN_MODE_LINEAR = 0, TWEEN_MODE_EASE_IN = 1,
                             TWEEN_MODE_EASE_OUT = 2, TWEEN_MODE_EASE_IN_OUT = 3;
        // VGC sprite attribute field offsets (nova.inc VGC_SPR_*_OFF).
        private const byte   SPR_FLAGS_OFF = 0x05;
        private const byte   SPR_FLAG_ENABLE = 0x80, SPR_FLAG_XFLIP = 0x01, SPR_FLAG_YFLIP = 0x02;
        // VGC register addresses (runtime/asm/nova.inc / VgcConstants).
        private const ushort VGC_BGCOL = 0xA001, VGC_FGCOL = 0xA002, VGC_CURSX = 0xA003,
                             VGC_CURSY = 0xA004, VGC_FONT = 0xA007, VGC_MODE = 0xA000,
                             VGC_BORDER = 0xA00D, VGC_DIMMER = 0xA0E5, VGC_TXTFLAGS = 0xA0E6;
        private const byte   VTXT_REV = 0x01, VTXT_FLASH = 0x04;
        private const byte   LERR_NO_FN = 0x83;
        private const byte   LERR_OK = 0x00;
        private const ushort LibCallEntry = 0x9C00;   // libcall.bin blob load address
        private const byte   RsBasic = 0x02;          // runtime home ROMSWAP value
        private const ushort Sentinel = 0xFFF9;       // RTS lands here; loop stops

        // ---------------------------------------------------------------------
        // Axis 1: loader path on LibLoaderBus (mirrors LibCallTests exactly).
        // ---------------------------------------------------------------------

        private static (LibLoaderBus bus, ushort entry) SetupLoader()
        {
            var bus = new LibLoaderBus();
            bus.LoadRam(LibCallEntry, File.ReadAllBytes(RepoPath("tests", "asm", "libcall.bin")));
            bus.StageShelfModule(0, File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin")),
                                 MODULE_ID_GRAPHICS);
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

        // Like RunFn, but pumps bus.AdvanceCycles each CPU step. The blitter completes
        // synchronously on the BLT_CMD write, but the DMA controller only advances when
        // CompositeBusDevice.AdvanceCycles is called — so dma_wait's status spin needs
        // the bus pumped to ever leave BUSY (mirrors AvaloniaDmaTests.RunUntilDmaNotBusy).
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

        // =====================================================================
        // Text/mode domain ($10-$1B), batch 4b.3. These ops set VGC registers
        // (no gfx-plane effect), observed by reading the register back on the bus.
        // =====================================================================

        // --- $10 COLOR(fg,bg,border): the three colour registers take the values. ---
        [TestMethod]
        public void Axis2_Color_SetsFgBgBorderRegisters()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 7);    // fg
            SetArg(bus, ARG1, 3);    // bg
            SetArg(bus, ARG2, 11);   // border
            RunFn(bus, GFN_COLOR);

            Assert.AreEqual(7,  bus.Read(VGC_FGCOL),  "COLOR must set foreground from ARG0");
            Assert.AreEqual(3,  bus.Read(VGC_BGCOL),  "COLOR must set background from ARG1");
            Assert.AreEqual(11, bus.Read(VGC_BORDER), "COLOR must set border from ARG2");
        }

        // --- $11 FONT(slot): the font-slot register takes the value (masked to 0-7). ---
        [TestMethod]
        public void Axis2_Font_SetsFontRegister()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 5);
            RunFn(bus, GFN_FONT);

            Assert.AreEqual(5, bus.Read(VGC_FONT) & 0x07, "FONT must select font slot 5");
        }

        // --- $12 MODE(mode): the mode register takes the value. ---
        [TestMethod]
        public void Axis2_Mode_SetsModeRegister()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 2);   // mode 2 = text over gfx
            RunFn(bus, GFN_MODE);

            Assert.AreEqual(2, bus.Read(VGC_MODE), "MODE must set the active VGC mode");
        }

        // --- $17 LOCATE(col,row): cursor X/Y registers take the values. ---
        [TestMethod]
        public void Axis2_Locate_SetsCursorPosition()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 17);   // col (0-79)
            SetArg(bus, ARG1, 9);    // row (0-49)
            RunFn(bus, GFN_LOCATE);

            Assert.AreEqual(17, bus.Read(VGC_CURSX), "LOCATE must set cursor column from ARG0");
            Assert.AreEqual(9,  bus.Read(VGC_CURSY), "LOCATE must set cursor row from ARG1");
        }

        // --- $1B / $1A DISPLAYOFF then DISPLAYON: dimmer register $00 then $0F. ---
        [TestMethod]
        public void Axis2_DisplayOffThenOn_TogglesDimmerRegister()
        {
            using var bus = MakeAxis2Bus();

            RunFn(bus, GFN_DISPLAYOFF);
            Assert.AreEqual(0x00, bus.Read(VGC_DIMMER), "DISPLAYOFF must blank the dimmer to $00");

            RunFn(bus, GFN_DISPLAYON);
            Assert.AreEqual(0x0F, bus.Read(VGC_DIMMER), "DISPLAYON must restore the dimmer to $0F");
        }

        // --- $18 CLS: the text plane is cleared to spaces. ---
        [TestMethod]
        public void Axis2_Cls_ClearsTextPlaneToSpaces()
        {
            using var bus = MakeAxis2Bus();

            // Dirty a few text cells via the real VGC char-plane memory API.
            int Cell(int col, int row) => row * VgcConstants.ScreenCols + col;
            Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceScreen, Cell(0, 0),  (byte)'X'), "setup write");
            Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceScreen, Cell(40, 12), (byte)'Y'), "setup write");
            Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceScreen, Cell(79, 24), (byte)'Z'), "setup write");
            Assert.AreEqual((byte)'X', bus.Vgc.GetScreenChar(0, 0), "setup char must take");

            RunFn(bus, GFN_CLS);

            Assert.AreEqual(0x20, bus.Vgc.GetScreenChar(0, 0),   "CLS must clear (0,0) to a space");
            Assert.AreEqual(0x20, bus.Vgc.GetScreenChar(40, 12), "CLS must clear (40,12) to a space");
            Assert.AreEqual(0x20, bus.Vgc.GetScreenChar(79, 24), "CLS must clear (79,24) to a space");
        }

        // --- $13/$14 REVERSE then REVERSEOFF: VGC_TXTFLAGS reverse bit set then cleared. ---
        [TestMethod]
        public void Axis2_ReverseThenOff_TogglesTextFlagReverseBit()
        {
            using var bus = MakeAxis2Bus();

            RunFn(bus, GFN_REVERSE);
            Assert.AreEqual(VTXT_REV, bus.Read(VGC_TXTFLAGS) & VTXT_REV,
                "REVERSE must set the reverse bit in VGC_TXTFLAGS");

            RunFn(bus, GFN_REVERSEOFF);
            Assert.AreEqual(0, bus.Read(VGC_TXTFLAGS) & VTXT_REV,
                "REVERSEOFF must clear the reverse bit in VGC_TXTFLAGS");
        }

        // --- $15/$16 FLASH then FLASHOFF: VGC_TXTFLAGS flash bit set then cleared. ---
        [TestMethod]
        public void Axis2_FlashThenOff_TogglesTextFlagFlashBit()
        {
            using var bus = MakeAxis2Bus();

            RunFn(bus, GFN_FLASH);
            Assert.AreEqual(VTXT_FLASH, bus.Read(VGC_TXTFLAGS) & VTXT_FLASH,
                "FLASH must set the flash bit in VGC_TXTFLAGS");

            RunFn(bus, GFN_FLASHOFF);
            Assert.AreEqual(0, bus.Read(VGC_TXTFLAGS) & VTXT_FLASH,
                "FLASHOFF must clear the flash bit in VGC_TXTFLAGS");
        }

        // --- $19 CLSWIN: no vgc driver entry yet -> stubbed to LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_Clswin_ReturnsNoFn_StubbedUntilDriverExists()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(FN_ID, GFN_CLSWIN);
            bus.WriteRam(STATUS, 0x00);   // poison opposite to the expected non-OK

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "CLSWIN stub must still RTS to the sentinel");
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "CLSWIN ($19) is a gap in the dense table -> gfn_unimpl -> LERR_NO_FN");
        }

        // --- Loader-axis smoke: a new text/mode fn-id routes through lib_call. ---
        [TestMethod]
        public void Axis1_Color_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            bus.PokeRam(ARG0, 7); bus.PokeRam((ushort)(ARG0 + 1), 0);   // fg
            bus.PokeRam(ARG1, 3); bus.PokeRam((ushort)(ARG1 + 1), 0);   // bg
            bus.PokeRam(ARG2, 1); bus.PokeRam((ushort)(ARG2 + 1), 0);   // border

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_COLOR);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "COLOR must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
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

        // =====================================================================
        // Hardware-sprite domain ($20-$3B), batch 4b.4. Axis 2 drives the module
        // wrappers and asserts the real VGC sprite engine state (position, enable,
        // flip, priority, shape) + reporter fns (getx/gety + collision) via
        // LIB_RESULT. One Axis-1 loader smoke proves dispatch routing for the range.
        // =====================================================================

        // --- $24 SPRPOS: position lands in the VGC sprite X/Y state. ---
        [TestMethod]
        public void Axis2_SprPos_SetsSpritePosition()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 3);     // sprite index
            SetArg(bus, ARG1, 120);   // x (s16)
            SetArg(bus, ARG2, 80);    // y (byte)
            RunFn(bus, GFN_SPR_POS);

            var st = bus.Vgc.GetSpriteState(3);
            Assert.AreEqual(120, st.x, "SPRPOS must set sprite 3 X to 120");
            Assert.AreEqual(80,  st.y, "SPRPOS must set sprite 3 Y to 80");
        }

        // --- $24 SPRPOS: a NEGATIVE x round-trips through the signed-16 X register. ---
        [TestMethod]
        public void Axis2_SprPos_NegativeX_RoundTripsAsSigned16()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 5);      // sprite index
            SetArg(bus, ARG1, -100);   // x = -100 -> low word 0xFF9C
            SetArg(bus, ARG2, 10);     // y
            RunFn(bus, GFN_SPR_POS);

            // Sprite X is a full 16-bit register; -100 stored as 0xFF9C (65436).
            var st = bus.Vgc.GetSpriteState(5);
            Assert.AreEqual(0xFF9C, st.x, "SPRPOS must store the signed-16 X bit-pattern (-100 -> 0xFF9C)");
            Assert.AreEqual(-100, (short)st.x, "stored X reinterpreted as signed-16 must equal -100");

            // SPRGETX reporter must read the same 16-bit pattern back into LIB_RESULT.
            SetArg(bus, ARG0, 5);
            RunFn(bus, GFN_SPR_GETX);
            Assert.AreEqual(0xFF9Cu, GetResult(bus), "SPRGETX must report the signed-16 X pattern");
            Assert.AreEqual(-100, (short)(ushort)GetResult(bus), "SPRGETX result as signed-16 must equal -100");
        }

        // --- $25/$26 SPRENABLE then SPRDISABLE: the enable flag toggles. ---
        [TestMethod]
        public void Axis2_SprEnableThenDisable_TogglesEnableFlag()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 2);
            RunFn(bus, GFN_SPR_ENABLE);
            Assert.IsTrue(bus.Vgc.GetSpriteState(2).enabled, "SPRENABLE must enable sprite 2");

            SetArg(bus, ARG0, 2);
            RunFn(bus, GFN_SPR_DISABLE);
            Assert.IsFalse(bus.Vgc.GetSpriteState(2).enabled, "SPRDISABLE must disable sprite 2");
        }

        // --- $27 SPRFLIP: the X/Y flip flag bits land in the sprite flags. ---
        [TestMethod]
        public void Axis2_SprFlip_SetsFlipFlags()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 4);
            SetArg(bus, ARG1, SPR_FLAG_XFLIP | SPR_FLAG_YFLIP);   // both flips
            RunFn(bus, GFN_SPR_FLIP);

            Assert.AreEqual(SPR_FLAG_XFLIP | SPR_FLAG_YFLIP, bus.Vgc.GetSpriteState(4).flags,
                "SPRFLIP must set both flip bits on sprite 4");
        }

        // --- $28 SPRPRIORITY: the priority value lands (clamped 0-2 by the VGC). ---
        [TestMethod]
        public void Axis2_SprPriority_SetsPriority()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 1);
            SetArg(bus, ARG1, 2);    // priority 2 = in front
            RunFn(bus, GFN_SPR_PRIORITY);

            Assert.AreEqual(2, bus.Vgc.GetSpriteState(1).priority, "SPRPRIORITY must set sprite 1 priority to 2");
        }

        // --- $29 SPRSHAPE: the shape-slot index register takes the value. ---
        [TestMethod]
        public void Axis2_SprShape_SetsShapeSlotIndex()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 6);    // sprite index
            SetArg(bus, ARG1, 9);    // shape slot
            RunFn(bus, GFN_SPR_SHAPE);

            Assert.AreEqual(9, bus.Vgc.GetSpriteShapeIndex(6), "SPRSHAPE must set sprite 6 shape-slot index to 9");
        }

        // --- $2A SPRSETREG: writing the FLAGS field via setreg8 enables + flips. ---
        [TestMethod]
        public void Axis2_SprSetReg_WritesFlagsField()
        {
            using var bus = MakeAxis2Bus();

            // Field 5 = FLAGS; value enable|xflip.
            SetArg(bus, ARG0, 7);                              // sprite index
            SetArg(bus, ARG1, SPR_FLAGS_OFF);                  // field offset
            SetArg(bus, ARG2, SPR_FLAG_ENABLE | SPR_FLAG_XFLIP);
            RunFn(bus, GFN_SPR_SETREG);

            var st = bus.Vgc.GetSpriteState(7);
            Assert.IsTrue(st.enabled, "SPRSETREG of FLAGS with the enable bit must enable sprite 7");
            Assert.AreEqual(SPR_FLAG_XFLIP, st.flags, "SPRSETREG of FLAGS must set the X-flip bit on sprite 7");
        }

        // --- $20 SPRDEFINE: one shape pixel takes the requested colour nibble. ---
        [TestMethod]
        public void Axis2_SprDefine_SetsShapePixel()
        {
            using var bus = MakeAxis2Bus();

            // Define pixel (x=2,y=3) of sprite 0's shape to colour 0xD.
            SetArg(bus, ARG0, 0);    // sprite index
            SetArg(bus, ARG1, 2);    // x (0-15)
            SetArg(bus, ARG2, 3);    // y (0-15)
            SetArg(bus, ARG3, 0xD);  // colour
            RunFn(bus, GFN_SPR_DEFINE);

            // Shape byte for (x=2,y=3): row 3, byte x/2 = 1; even x -> high nibble.
            var shape = bus.Vgc.GetSpriteShape(0);
            int byteIdx = 3 * 8 /*bytes per row*/ + 2 / 2;
            Assert.AreEqual(0xD, (shape[byteIdx] >> 4) & 0x0F,
                "SPRDEFINE must set the high nibble of the (2,3) shape byte to colour 0xD");
        }

        // --- $21 SPRROW: a full 16px row (8 packed bytes) lands in the shape. ---
        [TestMethod]
        public void Axis2_SprRow_WritesEightShapeBytes()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 8);    // sprite index
            SetArg(bus, ARG1, 1);    // row 1
            // 8 shape bytes packed across ARG2 (lo 4) + ARG3 (hi 4), LE.
            // ARG2 = 0x44332211 -> bytes 0x11,0x22,0x33,0x44; ARG3 = 0x88776655.
            SetArg(bus, ARG2, unchecked((int)0x44332211));
            SetArg(bus, ARG3, unchecked((int)0x88776655));
            RunFn(bus, GFN_SPR_ROW);

            var shape = bus.Vgc.GetSpriteShape(8);
            int baseIdx = 1 * 8;
            byte[] expected = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
            for (int i = 0; i < 8; i++)
                Assert.AreEqual(expected[i], shape[baseIdx + i], $"SPRROW byte {i} mismatch");
        }

        // --- $23 SPRCOPY: shape data copies from src slot to dst slot. ---
        [TestMethod]
        public void Axis2_SprCopy_CopiesShapeData()
        {
            using var bus = MakeAxis2Bus();

            // Paint a row into sprite 0, then copy 0 -> 1; assert sprite 1 matches.
            SetArg(bus, ARG0, 0);
            SetArg(bus, ARG1, 0);    // row 0
            SetArg(bus, ARG2, unchecked((int)0xDEADBEEF));
            SetArg(bus, ARG3, unchecked((int)0xCAFEBABE));
            RunFn(bus, GFN_SPR_ROW);

            SetArg(bus, ARG0, 0);    // src
            SetArg(bus, ARG1, 1);    // dst
            RunFn(bus, GFN_SPR_COPY);

            var src = bus.Vgc.GetSpriteShape(0);
            var dst = bus.Vgc.GetSpriteShape(1);
            for (int i = 0; i < 8; i++)
                Assert.AreEqual(src[i], dst[i], $"SPRCOPY byte {i} must match the source slot");
        }

        // --- $22 SPRCLEAR: previously-painted shape data is zeroed. ---
        [TestMethod]
        public void Axis2_SprClear_ZeroesShapeData()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 2);
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, unchecked((int)0xFFFFFFFF));
            SetArg(bus, ARG3, unchecked((int)0xFFFFFFFF));
            RunFn(bus, GFN_SPR_ROW);
            Assert.AreNotEqual(0, bus.Vgc.GetSpriteShape(2)[0], "setup SPRROW should have painted sprite 2");

            SetArg(bus, ARG0, 2);
            RunFn(bus, GFN_SPR_CLEAR);

            var shape = bus.Vgc.GetSpriteShape(2);
            for (int i = 0; i < VgcConstants.SpriteShapeSize; i++)
                Assert.AreEqual(0, shape[i], $"SPRCLEAR must zero shape byte {i}");
        }

        // --- $2C SPRGETX / $2D SPRGETY: reporters round-trip a position. ---
        [TestMethod]
        public void Axis2_SprGetXY_ReportsPosition()
        {
            using var bus = MakeAxis2Bus();

            // Seed sprite 10 via the real VGC API, then GET its coords through the module.
            bus.Vgc.SetSpritePosition(10, 0x0123, 0x45);

            SetArg(bus, ARG0, 10);
            RunFn(bus, GFN_SPR_GETX);
            Assert.AreEqual(0x0123u, GetResult(bus), "SPRGETX must report the 16-bit X into LIB_RESULT");

            SetArg(bus, ARG0, 10);
            RunFn(bus, GFN_SPR_GETY);
            Assert.AreEqual(0x45u, GetResult(bus), "SPRGETY must report the Y byte into LIB_RESULT");
        }

        // --- $30/$31 sprite-sprite collision status + mask reporters. ---
        [TestMethod]
        public void Axis2_SprCollStatusAndMask_ReportRegisters()
        {
            using var bus = MakeAxis2Bus();

            // Renderer-side: sprites {1,3} and {9} collided -> mask 0x020A.
            bus.Vgc.SetCollisionRegisters(spriteToSprite: 0x020A, spriteToBg: 0x0000);

            RunFn(bus, GFN_SPR_COLL_STATUS);
            Assert.AreEqual(0x0Au, GetResult(bus), "SPRCOLLSTATUS must report the low byte (sprites 0-7)");

            RunFn(bus, GFN_SPR_COLL_MASK);
            Assert.AreEqual(0x020Au, GetResult(bus), "SPRCOLLMASK must report the full 16-bit collision mask");
        }

        // --- $33 SPRCOLLREADCLEAR: reports the mask, then clears it. ---
        [TestMethod]
        public void Axis2_SprCollReadClear_ReportsThenClears()
        {
            using var bus = MakeAxis2Bus();

            bus.Vgc.SetCollisionRegisters(spriteToSprite: 0x8001, spriteToBg: 0x0000);

            RunFn(bus, GFN_SPR_COLL_READCLEAR);
            Assert.AreEqual(0x8001u, GetResult(bus), "SPRCOLLREADCLEAR must report the mask before clearing");

            // After read-clear the mask must read back as zero.
            RunFn(bus, GFN_SPR_COLL_MASK);
            Assert.AreEqual(0x0000u, GetResult(bus), "SPRCOLLREADCLEAR must have cleared the collision mask");
        }

        // --- $32 SPRCOLLCLEAR: clears a standing collision mask. ---
        [TestMethod]
        public void Axis2_SprCollClear_ClearsMask()
        {
            using var bus = MakeAxis2Bus();

            bus.Vgc.SetCollisionRegisters(spriteToSprite: 0x1234, spriteToBg: 0x0000);
            RunFn(bus, GFN_SPR_COLL_CLEAR);

            RunFn(bus, GFN_SPR_COLL_MASK);
            Assert.AreEqual(0x0000u, GetResult(bus), "SPRCOLLCLEAR must clear the sprite-sprite collision mask");
        }

        // --- $36/$37 sprite-background collision status + mask reporters. ---
        [TestMethod]
        public void Axis2_SprBgStatusAndMask_ReportRegisters()
        {
            using var bus = MakeAxis2Bus();

            bus.Vgc.SetCollisionRegisters(spriteToSprite: 0x0000, spriteToBg: 0x0140);

            RunFn(bus, GFN_SPR_BG_STATUS);
            Assert.AreEqual(0x40u, GetResult(bus), "SPRBGSTATUS must report the bg-collision low byte");

            RunFn(bus, GFN_SPR_BG_MASK);
            Assert.AreEqual(0x0140u, GetResult(bus), "SPRBGMASK must report the full 16-bit bg-collision mask");
        }

        // --- $39 SPRBGREADCLEAR: reports the bg mask then clears it. ---
        [TestMethod]
        public void Axis2_SprBgReadClear_ReportsThenClears()
        {
            using var bus = MakeAxis2Bus();

            bus.Vgc.SetCollisionRegisters(spriteToSprite: 0x0000, spriteToBg: 0x00C3);

            RunFn(bus, GFN_SPR_BG_READCLEAR);
            Assert.AreEqual(0x00C3u, GetResult(bus), "SPRBGREADCLEAR must report the bg mask before clearing");

            RunFn(bus, GFN_SPR_BG_MASK);
            Assert.AreEqual(0x0000u, GetResult(bus), "SPRBGREADCLEAR must clear the bg-collision mask");
        }

        // --- $35/$3B IRQ-disable ops just route + report OK (no IRQ side effect to assert). ---
        [TestMethod]
        public void Axis2_SprCollIrqDisable_StatusOk()
        {
            using var bus = MakeAxis2Bus();
            RunFn(bus, GFN_SPR_COLL_IRQOFF);   // RunFn asserts STATUS = OK
            RunFn(bus, GFN_SPR_BG_IRQOFF);
        }

        // --- Loader-axis smoke: a new sprite fn-id routes through the real lib_call. ---
        [TestMethod]
        public void Axis1_SprPos_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            bus.PokeRam(ARG0, 3); bus.PokeRam((ushort)(ARG0 + 1), 0);   // sprite index
            bus.PokeRam(ARG1, 50); bus.PokeRam((ushort)(ARG1 + 1), 0);  // x
            bus.PokeRam(ARG2, 60); bus.PokeRam((ushort)(ARG2 + 1), 0);  // y

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_SPR_POS);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "SPRPOS must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch density: a gap id inside the sprite range ($2E) returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_SpriteRangeGap_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(FN_ID, 0x2E);   // gap between SPRGETY ($2D) and the collision block ($30)
            bus.WriteRam(STATUS, 0x00);  // poison opposite to expected non-OK

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "sprite-range gap must still RTS to the sentinel");
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $2E is a gap in the dense table -> gfn_unimpl -> LERR_NO_FN");
        }

        // =====================================================================
        // Copper domain ($40-$49), batch 4b.5. Axis 2 drives the module wrappers
        // and asserts the real VGC copper state: enable flag (IsCopperEnabled),
        // the compiled display list (GetCopperProgram, committed at a vblank via
        // IncrementFrameCounter), and the active-list swap. One Axis-1 loader
        // smoke proves dispatch routing for the range; one gap-id test confirms
        // the dense-table bounds behaviour.
        // =====================================================================

        // VGC core register index (offset from VgcBase) for the copper register specifier.
        private const byte   COPPER_REG_MODE  = 0x00;   // RegMode  - VgcBase
        private const byte   COPPER_REG_BGCOL = 0x01;   // RegBgCol - VgcBase

        // --- $43/$44 COPPER_ON then COPPER_OFF: the enable flag toggles. ---
        [TestMethod]
        public void Axis2_CopperOnThenOff_TogglesEnableFlag()
        {
            using var bus = MakeAxis2Bus();

            RunFn(bus, GFN_COPPER_ON);
            Assert.IsTrue(bus.Vgc.IsCopperEnabled, "COPPER_ON must enable copper execution");

            RunFn(bus, GFN_COPPER_OFF);
            Assert.IsFalse(bus.Vgc.IsCopperEnabled, "COPPER_OFF must disable copper execution");
        }

        // --- $41 COPPER_ADD: a register-write event lands on the active list. ---
        [TestMethod]
        public void Axis2_CopperAdd_AddsEventToActiveList()
        {
            using var bus = MakeAxis2Bus();

            // ADD: at column 40, scanline 10, write RegMode (index 0) = 2.
            SetArg(bus, ARG0, 40);                 // x
            SetArg(bus, ARG1, 10);                 // y
            SetArg(bus, ARG2, COPPER_REG_MODE);    // register specifier (direct index)
            SetArg(bus, ARG3, 2);                  // value
            RunFn(bus, GFN_COPPER_ADD);

            // The program compiles at the next vblank; default target/active = list 0.
            bus.Vgc.IncrementFrameCounter();
            var program = bus.Vgc.GetCopperProgram();
            Assert.AreEqual(1, program.Length, "COPPER_ADD must add one event to the default list");
            Assert.AreEqual((ushort)(10 * VgcConstants.GfxWidth + 40), program[0].Position,
                "event position must be y*GfxWidth + x");
            Assert.AreEqual(COPPER_REG_MODE, program[0].RegisterIndex, "event register index must be RegMode");
            Assert.AreEqual(2, program[0].Value, "event value must be the ARG3 value");
        }

        // --- $42 COPPER_CLEAR: empties the target list. ---
        [TestMethod]
        public void Axis2_CopperClear_EmptiesList()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 20);
            SetArg(bus, ARG1, 50);
            SetArg(bus, ARG2, COPPER_REG_BGCOL);
            SetArg(bus, ARG3, 4);
            RunFn(bus, GFN_COPPER_ADD);
            bus.Vgc.IncrementFrameCounter();
            Assert.AreEqual(1, bus.Vgc.GetCopperProgram().Length, "setup ADD should have produced one event");

            RunFn(bus, GFN_COPPER_CLEAR);
            bus.Vgc.IncrementFrameCounter();
            Assert.AreEqual(0, bus.Vgc.GetCopperProgram().Length, "COPPER_CLEAR must empty the list");
        }

        // --- $40/$45 COPPER_LIST + COPPER_USE: edit list 1, then make it active. ---
        [TestMethod]
        public void Axis2_CopperListThenUse_SwapsActiveListAtVblank()
        {
            using var bus = MakeAxis2Bus();

            // Target list 1 for subsequent edits.
            SetArg(bus, ARG0, 1);
            RunFn(bus, GFN_COPPER_LIST);

            // Add an event to list 1.
            SetArg(bus, ARG0, 10);
            SetArg(bus, ARG1, 5);
            SetArg(bus, ARG2, COPPER_REG_BGCOL);
            SetArg(bus, ARG3, 7);
            RunFn(bus, GFN_COPPER_ADD);

            // USE list 1 (becomes active at next vblank).
            SetArg(bus, ARG0, 1);
            RunFn(bus, GFN_COPPER_USE);

            // Before vblank, active is still the (empty) list 0.
            Assert.AreEqual(0, bus.Vgc.GetCopperProgram().Length, "active list 0 is empty before the swap");

            bus.Vgc.IncrementFrameCounter();
            var program = bus.Vgc.GetCopperProgram();
            Assert.AreEqual(1, program.Length, "COPPER_USE must swap the active list to list 1 at vblank");
            Assert.AreEqual(7, program[0].Value, "the active list-1 event value must be visible");
        }

        // --- $48 COPPER_SET_REG: composed add-a-register-write to a VGC core reg. ---
        [TestMethod]
        public void Axis2_CopperSetReg_AddsRegisterWriteEvent()
        {
            using var bus = MakeAxis2Bus();

            // At (0,30) write RegBgCol (index 1) = 9.
            SetArg(bus, ARG0, 0);                  // x
            SetArg(bus, ARG1, 30);                 // y
            SetArg(bus, ARG2, COPPER_REG_BGCOL);   // register index
            SetArg(bus, ARG3, 9);                  // value
            RunFn(bus, GFN_COPPER_SET_REG);

            bus.Vgc.IncrementFrameCounter();
            var program = bus.Vgc.GetCopperProgram();
            Assert.AreEqual(1, program.Length, "COPPER_SET_REG must add one register-write event");
            Assert.AreEqual((ushort)(30 * VgcConstants.GfxWidth + 0), program[0].Position, "event position");
            Assert.AreEqual(COPPER_REG_BGCOL, program[0].RegisterIndex, "event must target RegBgCol");
            Assert.AreEqual(9, program[0].Value, "event value must be ARG3");
        }

        // --- $49 COPPER_SET_SPRITE_REG: composed add-a-write to a sprite attr reg. ---
        [TestMethod]
        public void Axis2_CopperSetSpriteReg_AddsSpriteRegisterWriteEvent()
        {
            using var bus = MakeAxis2Bus();

            // At (0,60) write sprite 2's FLAGS field (offset 5) = 0x80 (enable).
            // Absolute reg = VGC_SPR_BASE + 2*8 + 5 = $A055 -> RegisterIndex 0x55.
            const byte sprIdx = 2, field = SPR_FLAGS_OFF;
            SetArg(bus, ARG0, 0);                          // x
            SetArg(bus, ARG1, 60);                         // y
            SetArg(bus, ARG2, sprIdx | (field << 8));      // ARG2 low=index, byte1=field
            SetArg(bus, ARG3, SPR_FLAG_ENABLE);            // value
            RunFn(bus, GFN_COPPER_SET_SPRITE_REG);

            bus.Vgc.IncrementFrameCounter();
            var program = bus.Vgc.GetCopperProgram();
            Assert.AreEqual(1, program.Length, "COPPER_SET_SPRITE_REG must add one sprite-register-write event");
            byte expectedRegIndex = (byte)((VgcConstants.SpriteRegBase + sprIdx * VgcConstants.SpriteRegStride
                                            + field) - VgcConstants.VgcBase);
            Assert.AreEqual(expectedRegIndex, program[0].RegisterIndex,
                "event must target sprite 2 FLAGS register (offset $55)");
            Assert.AreEqual(SPR_FLAG_ENABLE, program[0].Value, "event value must be the enable bit");
        }

        // --- $47 COPPER_SPLIT: builds a two-mode raster split and enables copper. ---
        [TestMethod]
        public void Axis2_CopperSplit_BuildsTwoModeListAndEnables()
        {
            using var bus = MakeAxis2Bus();

            // Split at scanline 100: mode 1 above, mode 2 below.
            SetArg(bus, ARG0, 0);    // list index
            SetArg(bus, ARG1, 100);  // split Y
            SetArg(bus, ARG2, 1);    // mode 0 (scanline 0..99)
            SetArg(bus, ARG3, 2);    // mode 1 (scanline 100+)
            RunFn(bus, GFN_COPPER_SPLIT);

            Assert.IsTrue(bus.Vgc.IsCopperEnabled, "COPPER_SPLIT must leave the copper enabled");

            bus.Vgc.IncrementFrameCounter();
            var program = bus.Vgc.GetCopperProgram();
            Assert.AreEqual(2, program.Length, "COPPER_SPLIT must build two VGC_MODE events");
            // Sorted by position: scanline 0 first (mode0), then scanline 100 (mode1).
            Assert.AreEqual(COPPER_REG_MODE, program[0].RegisterIndex, "first event writes RegMode");
            Assert.AreEqual(1, program[0].Value, "first event sets mode 0 at the top of the frame");
            Assert.AreEqual(COPPER_REG_MODE, program[1].RegisterIndex, "second event writes RegMode");
            Assert.AreEqual(2, program[1].Value, "second event sets mode 1 below the split");
            Assert.AreEqual((ushort)(100 * VgcConstants.GfxWidth), program[1].Position,
                "second event fires at the split scanline");
        }

        // --- $46 COPPER_END: routes + reports OK (target reset has no readable effect). ---
        [TestMethod]
        public void Axis2_CopperEnd_StatusOk()
        {
            using var bus = MakeAxis2Bus();
            RunFn(bus, GFN_COPPER_END);   // RunFn asserts STATUS = OK
        }

        // --- Loader-axis smoke: a copper fn-id routes through the real lib_call. ---
        [TestMethod]
        public void Axis1_CopperOn_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_COPPER_ON);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "COPPER_ON must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch density: a gap id below the copper range ($3C) returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_CopperRangeGap_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(FN_ID, 0x3C);   // gap between the sprite range ($3B) and copper ($40)
            bus.WriteRam(STATUS, 0x00);  // poison opposite to expected non-OK

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "copper-range gap must still RTS to the sentinel");
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $3C is a gap in the dense table -> gfn_unimpl -> LERR_NO_FN");
        }

        // =====================================================================
        // Blit/DMA domain ($50-$5B), batch 4b.6. The blitter completes synchronously
        // on the BLT_CMD write; the DMA controller advances only when the bus is
        // pumped (RunFnPumped). Both move bytes between unified memory spaces; the
        // gfx-plane effect is asserted via the real VGC memory API. The status/err/
        // count reporters surface the controller registers into LIB_RESULT.
        //
        // ARG layout (see runtime/asm/libgraphics.inc):
        //   ARG0 byte0=srcSpace, byte1=dstSpace, byte2=fillValue
        //   ARG1 = src offset (24-bit), ARG2 = dst offset (24-bit)
        //   ARG3 = MOVE: 24-bit length; BLIT: width(b0/b1)|height(b2/b3)
        // =====================================================================

        private const byte DMA_SPACE_GFX = 0x03;   // DmaSpaceVgcGfx / BltSpaceVgcGfx

        // Flat gfx-space byte accessors (offset = y*GfxWidth + x for 2-D content).
        private static byte GfxByte(CompositeBusDevice bus, int offset)
        {
            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, offset, out byte v),
                $"failed to read gfx byte {offset}");
            return v;
        }

        private static void SetGfxByte(CompositeBusDevice bus, int offset, byte value) =>
            Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, offset, value),
                $"failed to write gfx byte {offset}");

        // --- $54 DMACOPY: a contiguous run copies within the gfx space. ---
        [TestMethod]
        public void Axis2_DmaCopy_MovesBytesWithinGfxSpace()
        {
            using var bus = MakeAxis2Bus();

            // Seed a 16-byte source pattern at gfx offset 0; clear the dest at 0x1000.
            const int src = 0, dst = 0x1000, len = 16;
            for (int i = 0; i < len; i++) SetGfxByte(bus, src + i, (byte)(0xA0 + i));
            for (int i = 0; i < len; i++) SetGfxByte(bus, dst + i, 0x00);

            SetArg(bus, ARG0, DMA_SPACE_GFX | (DMA_SPACE_GFX << 8));   // src+dst space = gfx
            SetArg(bus, ARG1, src);
            SetArg(bus, ARG2, dst);
            SetArg(bus, ARG3, len);
            RunFnPumped(bus, GFN_DMACOPY);

            for (int i = 0; i < len; i++)
                Assert.AreEqual((byte)(0xA0 + i), GfxByte(bus, dst + i),
                    $"DMACOPY must move source byte {i} to the destination");
            Assert.AreEqual(0x00, GfxByte(bus, dst + len), "DMACOPY must not write past the length");
        }

        // --- $55 DMAFILL: a run is filled with the requested value. ---
        [TestMethod]
        public void Axis2_DmaFill_FillsRunWithValue()
        {
            using var bus = MakeAxis2Bus();

            const int dst = 0x2000, len = 32;
            for (int i = 0; i < len + 1; i++) SetGfxByte(bus, dst + i, 0x11);

            // dstSpace=gfx (byte1), fillValue=0x5E (byte2).
            SetArg(bus, ARG0, (DMA_SPACE_GFX << 8) | (0x5E << 16));
            SetArg(bus, ARG2, dst);
            SetArg(bus, ARG3, len);
            RunFnPumped(bus, GFN_DMAFILL);

            for (int i = 0; i < len; i++)
                Assert.AreEqual(0x5E, GfxByte(bus, dst + i), $"DMAFILL must fill byte {i} with 0x5E");
            Assert.AreEqual(0x11, GfxByte(bus, dst + len), "DMAFILL must not write past the length");
        }

        // --- $50 BLITCOPY: a 2-D rectangle copies within the gfx space. ---
        [TestMethod]
        public void Axis2_BlitCopy_MovesRectangleWithinGfxSpace()
        {
            using var bus = MakeAxis2Bus();

            // 4x3 rectangle: source rows at gfx (0,0); dest rows at gfx (0,100).
            // Tightly packed (stride=width), so the source rect occupies offsets
            // 0..11 and the dest rect 100*GfxWidth..+11.
            const int w = 4, h = 3;
            int dst = 100 * VgcConstants.GfxWidth;
            for (int i = 0; i < w * h; i++) SetGfxByte(bus, i, (byte)(0x20 + i));
            for (int i = 0; i < w * h; i++) SetGfxByte(bus, dst + i, 0x00);

            SetArg(bus, ARG0, DMA_SPACE_GFX | (DMA_SPACE_GFX << 8));
            SetArg(bus, ARG1, 0);                       // src offset
            SetArg(bus, ARG2, dst);                     // dst offset
            SetArg(bus, ARG3, w | (h << 16));           // width|height
            RunFn(bus, GFN_BLITCOPY);                   // blitter completes synchronously

            for (int i = 0; i < w * h; i++)
                Assert.AreEqual((byte)(0x20 + i), GfxByte(bus, dst + i),
                    $"BLITCOPY must move rect byte {i}");

            // BLIT_COUNT reporter must report w*h bytes written.
            RunFn(bus, GFN_BLIT_COUNT);
            Assert.AreEqual((uint)(w * h), GetResult(bus), "BLIT_COUNT must report bytes written");
        }

        // --- $51 BLITFILL: a 2-D rectangle is filled with the requested value. ---
        [TestMethod]
        public void Axis2_BlitFill_FillsRectangleWithValue()
        {
            using var bus = MakeAxis2Bus();

            const int w = 5, h = 4;
            int dst = 50 * VgcConstants.GfxWidth;
            for (int i = 0; i < w * h + 1; i++) SetGfxByte(bus, dst + i, 0x22);

            // dstSpace=gfx (byte1), fillValue=0x7C (byte2).
            SetArg(bus, ARG0, (DMA_SPACE_GFX << 8) | (0x7C << 16));
            SetArg(bus, ARG2, dst);
            SetArg(bus, ARG3, w | (h << 16));
            RunFn(bus, GFN_BLITFILL);

            for (int i = 0; i < w * h; i++)
                Assert.AreEqual(0x7C, GfxByte(bus, dst + i), $"BLITFILL must fill rect byte {i}");
            Assert.AreEqual(0x22, GfxByte(bus, dst + w * h), "BLITFILL must not write past the rect");
        }

        // --- $5C BLIT_START_FILL: run blitter_fill on caller-preloaded BLT_* regs. ---
        // This is the arbitrary-stride BLITFILL lib_call path: BASIC's LAB_BLITFILL
        // loads the BLT_* MMIO registers directly (dstStride != width is the whole
        // point — the packed $51 ABI cannot express it) and then calls $5C, which
        // runs the module's blitter_fill (zeros SRC/SRCSTRIDE/CKEY, MODE_FILL,
        // CMD_START, wait). Pre-poison the SRC/CKEY/MODE registers so the test also
        // proves the MODULE (not BASIC) does the zeroing/MODE setup.
        [TestMethod]
        public void Axis2_BlitStartFill_RunsFillOnPreloadedRegisters()
        {
            using var bus = MakeAxis2Bus();

            // Fill a 3-wide x 2-high rect into the gfx plane with dstStride=80
            // (rows are 80 bytes apart, NOT tightly packed). Row0 at dst, row1 at
            // dst+80. Seed the target bytes and a guard byte after each row.
            const int w = 3, h = 2, dstStride = 80;
            int dst = 10 * VgcConstants.GfxWidth;
            for (int r = 0; r < h; r++)
                for (int c = 0; c < w + 1; c++)
                    SetGfxByte(bus, dst + r * dstStride + c, 0x11);

            // Poison the registers the MODULE must overwrite/zero. If BASIC were
            // (wrongly) expected to do this, leaving them dirty would corrupt the
            // fill; the $5C wrapper's blitter_fill must zero SRC*/SRCSTRIDE/CKEY
            // and set MODE_FILL itself.
            bus.Write((ushort)VgcConstants.BltSrcSpace, 0x05);
            bus.Write((ushort)VgcConstants.BltSrcL, 0xEE);
            bus.Write((ushort)VgcConstants.BltSrcM, 0xEE);
            bus.Write((ushort)VgcConstants.BltSrcH, 0xEE);
            bus.Write((ushort)VgcConstants.BltSrcStrideL, 0xEE);
            bus.Write((ushort)VgcConstants.BltSrcStrideH, 0xEE);
            bus.Write((ushort)VgcConstants.BltColorKey, 0xEE);
            bus.Write((ushort)VgcConstants.BltMode, 0xFF);

            // Caller (BASIC) preloads dst/space/stride/dims/fill in the MMIO regs.
            bus.Write((ushort)VgcConstants.BltDstSpace, DMA_SPACE_GFX);
            bus.Write((ushort)VgcConstants.BltDstL, (byte)(dst & 0xFF));
            bus.Write((ushort)VgcConstants.BltDstM, (byte)((dst >> 8) & 0xFF));
            bus.Write((ushort)VgcConstants.BltDstH, 0x00);
            bus.Write((ushort)VgcConstants.BltDstStrideL, (byte)(dstStride & 0xFF));
            bus.Write((ushort)VgcConstants.BltDstStrideH, 0x00);
            bus.Write((ushort)VgcConstants.BltWidthL, (byte)w);
            bus.Write((ushort)VgcConstants.BltWidthH, 0x00);
            bus.Write((ushort)VgcConstants.BltHeightL, (byte)h);
            bus.Write((ushort)VgcConstants.BltHeightH, 0x00);
            bus.Write((ushort)VgcConstants.BltFillValue, 0x9D);

            RunFn(bus, GFN_BLIT_START_FILL);

            // Each row's w bytes must be filled with 0x9D; the byte past the rect
            // width (still inside the stride gap) must be untouched.
            for (int r = 0; r < h; r++)
            {
                for (int c = 0; c < w; c++)
                    Assert.AreEqual(0x9D, GfxByte(bus, dst + r * dstStride + c),
                        $"BLIT_START_FILL must fill row {r} byte {c}");
                Assert.AreEqual(0x11, GfxByte(bus, dst + r * dstStride + w),
                    $"BLIT_START_FILL must not write past width on row {r}");
            }

            // The module must have zeroed the SRC registers + CKEY and set MODE_FILL.
            Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.BltSrcSpace), "module must zero BLT_SRCSPACE");
            Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.BltColorKey), "module must zero BLT_CKEY");
            Assert.AreEqual(VgcConstants.BltModeFill, bus.Read((ushort)VgcConstants.BltMode) & VgcConstants.BltModeFill,
                "module must set MODE_FILL");
        }

        // --- $56/$57 BLIT_STATUS + BLIT_ERR: reporters after a clean blit. ---
        [TestMethod]
        public void Axis2_BlitStatusAndErr_ReportDoneAndNoError()
        {
            using var bus = MakeAxis2Bus();

            // Run a small blit so the controller lands in OK/no-error.
            const int w = 2, h = 2;
            for (int i = 0; i < w * h; i++) SetGfxByte(bus, i, (byte)(0x30 + i));
            SetArg(bus, ARG0, DMA_SPACE_GFX | (DMA_SPACE_GFX << 8));
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, 0x800);
            SetArg(bus, ARG3, w | (h << 16));
            RunFn(bus, GFN_BLITCOPY);

            RunFn(bus, GFN_BLIT_STATUS);
            Assert.AreEqual((uint)VgcConstants.BltStatusOk, GetResult(bus),
                "BLIT_STATUS must report OK after a clean blit");

            RunFn(bus, GFN_BLIT_ERR);
            Assert.AreEqual((uint)VgcConstants.BltErrNone, GetResult(bus),
                "BLIT_ERR must report no-error after a clean blit");
        }

        // --- $59/$5A/$5B DMA_STATUS + DMA_ERR + DMA_COUNT: reporters after a clean DMA. ---
        [TestMethod]
        public void Axis2_DmaStatusErrCount_ReportDoneNoErrorAndCount()
        {
            using var bus = MakeAxis2Bus();

            const int len = 24;
            for (int i = 0; i < len; i++) SetGfxByte(bus, i, (byte)(0x40 + i));
            SetArg(bus, ARG0, DMA_SPACE_GFX | (DMA_SPACE_GFX << 8));
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, 0x3000);
            SetArg(bus, ARG3, len);
            RunFnPumped(bus, GFN_DMACOPY);

            // Reporters are pure register reads (no engine advance needed) -> RunFn.
            RunFn(bus, GFN_DMA_STATUS);
            Assert.AreEqual((uint)VgcConstants.DmaStatusOk, GetResult(bus),
                "DMA_STATUS must report OK after a clean copy");

            RunFn(bus, GFN_DMA_ERR);
            Assert.AreEqual((uint)VgcConstants.DmaErrNone, GetResult(bus),
                "DMA_ERR must report no-error after a clean copy");

            RunFn(bus, GFN_DMA_COUNT);
            Assert.AreEqual((uint)len, GetResult(bus), "DMA_COUNT must report bytes moved");
        }

        // --- Loader-axis smoke: a blit fn-id routes through the real lib_call. ---
        // BLITCOPY is used (not a DMA op) because LibLoaderBus has no peripherals:
        // BLT/DMA status reads return dead RAM (0=idle), so blitter_wait/dma_wait
        // exit immediately (no hang) and the wrapper still reports OK on dispatch.
        [TestMethod]
        public void Axis1_BlitCopy_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            // ARG cells land in dead RAM on the loader bus; we only assert routing.
            bus.PokeRam(ARG0, DMA_SPACE_GFX); bus.PokeRam((ushort)(ARG0 + 1), DMA_SPACE_GFX);
            bus.PokeRam(ARG3, 1); bus.PokeRam((ushort)(ARG3 + 2), 1);   // width=1, height=1

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_BLITCOPY);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "BLITCOPY must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch density: an unimplemented gap id in the blit/dma range
        // ($5D, between BLIT_START_FILL/$5C and the vsprite domain/$60) routes to
        // gfn_unimpl and reports LERR_NO_FN. ($5C is now GFN_BLIT_START_FILL.) ---
        [TestMethod]
        public void Axis2_BlitDmaRangeGap_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(FN_ID, 0x5D);   // reserved gap slot ($5D-$5F -> gfn_unimpl)
            bus.WriteRam(STATUS, 0x00);  // poison opposite to expected non-OK

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "blit/dma-range gap must still RTS to the sentinel");
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $5D (gfn_unimpl gap slot) must report LERR_NO_FN");
        }

        // =====================================================================
        // Vsprite domain ($60-$71), batch 4b.7. Every op takes ONE BYTES arg
        // (ARG0): a caller struct in CPU RAM laid out byte-for-byte as the
        // VSPRITE_* BSS config block (offsets VS_OFF_*). The wrapper copies it
        // into the module's BSS via (LIB_ZP),Y then runs the driver. Axis 2
        // drives the real blitter through the VGC gfx-plane memory API. The
        // single-sprite blit/fill/save-restore/rotate ops self-wait inside the
        // blitter (RunFn); only the two frame-pacing rotate variants poll
        // VGC_FRAME and need a pumped bus (RunFnPumped).
        // =====================================================================

        // Layout of a CPU-RAM scratch arena for vsprite tests, all clear of the
        // mailbox ($0300-$031F), the resident loader band ($0320-$041F), the
        // module-BSS band ($0420-$08FF, owned by the paged module and fully
        // populated as of batch 4c.2-2 — the 3x256 B turtle buffers fill it), and
        // the stack. So the arena sits ABOVE the band at $0D00+.
        private const ushort VS_CFG    = 0x0F00;   // config struct
        private const ushort VS_SRC    = 0x0D00;   // source rectangle
        private const ushort VS_ORIG   = 0x0D00;   // immutable rotate source
        private const ushort VS_ROT    = 0x0D80;   // rotated output buffer
        private const ushort VS_BG     = 0x0E00;   // saved-background buffer

        // Point ARG0 at a config struct of `len` bytes at `addr` (BYTES arg:
        // ptr16 low word, len16 high word).
        private static void SetCfgArg(CompositeBusDevice bus, ushort addr, int len) =>
            SetArg(bus, ARG0, (addr & 0xFFFF) | (len << 16));

        private static void Cfg(CompositeBusDevice bus, int off, byte value) =>
            bus.WriteRam((ushort)(VS_CFG + off), value);

        // Common single-sprite config: src space/addr/stride, w/h, x/y, flags.
        private static void WriteSpriteCfg(CompositeBusDevice bus, byte srcSpace,
            ushort srcAddr, int w, int h, int x, int y, byte colorKey = 0, byte flags = 0)
        {
            Cfg(bus, VS_OFF_SRCSPACE, srcSpace);
            Cfg(bus, VS_OFF_SRCADDRL, (byte)(srcAddr & 0xFF));
            Cfg(bus, VS_OFF_SRCADDRM, (byte)((srcAddr >> 8) & 0xFF));
            Cfg(bus, VS_OFF_SRCADDRH, 0);
            Cfg(bus, VS_OFF_WIDTHL,  (byte)(w & 0xFF));
            Cfg(bus, VS_OFF_WIDTHH,  (byte)((w >> 8) & 0xFF));
            Cfg(bus, VS_OFF_HEIGHTL, (byte)(h & 0xFF));
            Cfg(bus, VS_OFF_HEIGHTH, (byte)((h >> 8) & 0xFF));
            Cfg(bus, VS_OFF_SRCSTRL, (byte)(w & 0xFF));    // tightly packed
            Cfg(bus, VS_OFF_SRCSTRH, (byte)((w >> 8) & 0xFF));
            Cfg(bus, VS_OFF_XL, (byte)(x & 0xFF));
            Cfg(bus, VS_OFF_XH, (byte)((x >> 8) & 0xFF));
            Cfg(bus, VS_OFF_Y,  (byte)y);
            Cfg(bus, VS_OFF_COLORKEY, colorKey);
            Cfg(bus, VS_OFF_FLAGS, flags);
        }

        // --- $66 VS_GFX_BLIT: copy a CPU-RAM rectangle to the gfx plane at X/Y. ---
        [TestMethod]
        public void Axis2_VsGfxBlit_CopiesRectToGfxPlane()
        {
            using var bus = MakeAxis2Bus();

            // 4x4 source of colour 0x05 in CPU RAM.
            const int w = 4, h = 4, x = 20, y = 30;
            for (int i = 0; i < w * h; i++) bus.WriteRam((ushort)(VS_SRC + i), 0x05);

            WriteSpriteCfg(bus, 0 /*CPU*/, VS_SRC, w, h, x, y);
            SetCfgArg(bus, VS_CFG, 41);
            RunFn(bus, GFN_VS_GFX_BLIT);

            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    Assert.AreEqual(5, GfxPixel(bus, x + col, y + row),
                        $"VS_GFX_BLIT must set gfx pixel ({x + col},{y + row}) to the source colour");
            Assert.AreEqual(0, GfxPixel(bus, x + w, y), "VS_GFX_BLIT must not write past the rect width");
            Assert.AreEqual(0, GfxPixel(bus, x, y + h), "VS_GFX_BLIT must not write past the rect height");
        }

        // --- $62 VS_FILL: fill a destination rectangle in gfx space with a value. ---
        [TestMethod]
        public void Axis2_VsFill_FillsDestinationRectangle()
        {
            using var bus = MakeAxis2Bus();

            const int w = 5, h = 3, x = 60, y = 70;
            int dstBase = y * VgcConstants.GfxWidth + x;
            for (int row = 0; row < h; row++)
                for (int col = 0; col < w + 1; col++)
                    SetGfxByte(bus, (y + row) * VgcConstants.GfxWidth + x + col, 0x11);

            // Direct VS_FILL: caller sets DST space/addr/stride + w/h + fill value.
            Cfg(bus, VS_OFF_DSTSPACE, DMA_SPACE_GFX);
            Cfg(bus, VS_OFF_DSTADDRL, (byte)(dstBase & 0xFF));
            Cfg(bus, VS_OFF_DSTADDRM, (byte)((dstBase >> 8) & 0xFF));
            Cfg(bus, VS_OFF_DSTADDRH, (byte)((dstBase >> 16) & 0xFF));
            Cfg(bus, VS_OFF_WIDTHL, w); Cfg(bus, VS_OFF_WIDTHH, 0);
            Cfg(bus, VS_OFF_HEIGHTL, h); Cfg(bus, VS_OFF_HEIGHTH, 0);
            Cfg(bus, VS_OFF_DSTSTRL, (byte)(VgcConstants.GfxWidth & 0xFF));
            Cfg(bus, VS_OFF_DSTSTRH, (byte)((VgcConstants.GfxWidth >> 8) & 0xFF));
            Cfg(bus, VS_OFF_FILLVALUE, 0x09);
            SetCfgArg(bus, VS_CFG, 41);
            RunFn(bus, GFN_VS_FILL);

            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    Assert.AreEqual(9, GfxPixel(bus, x + col, y + row),
                        $"VS_FILL must fill gfx pixel ({x + col},{y + row})");
            Assert.AreEqual(0x11, GfxPixel(bus, x + w, y), "VS_FILL must not write past the rect width");
        }

        // --- $69/$6A VS_GFX_SAVE_BG then RESTORE_BG: a region round-trips. ---
        [TestMethod]
        public void Axis2_VsGfxSaveThenRestoreBg_RestoresOriginalRegion()
        {
            using var bus = MakeAxis2Bus();

            const int w = 4, h = 4, x = 100, y = 50;
            // Paint a distinct gfx region (colour = 6) to be saved.
            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    SetGfxByte(bus, (y + row) * VgcConstants.GfxWidth + x + col, 0x06);

            // Config: X/Y/W/H + a CPU-RAM background buffer (BGSPACE=CPU, stride=w).
            Cfg(bus, VS_OFF_WIDTHL, w); Cfg(bus, VS_OFF_HEIGHTL, h);
            Cfg(bus, VS_OFF_XL, (byte)(x & 0xFF)); Cfg(bus, VS_OFF_XH, (byte)((x >> 8) & 0xFF));
            Cfg(bus, VS_OFF_Y, (byte)y);
            Cfg(bus, VS_OFF_BGSPACE, 0 /*CPU*/);
            Cfg(bus, VS_OFF_BGADDRL, (byte)(VS_BG & 0xFF));
            Cfg(bus, VS_OFF_BGADDRM, (byte)((VS_BG >> 8) & 0xFF));
            Cfg(bus, VS_OFF_BGADDRH, 0);
            Cfg(bus, VS_OFF_BGSTRL, w); Cfg(bus, VS_OFF_BGSTRH, 0);
            SetCfgArg(bus, VS_CFG, 41);

            RunFn(bus, GFN_VS_GFX_SAVE_BG);
            // The CPU-RAM background buffer must now hold the saved region.
            for (int i = 0; i < w * h; i++)
                Assert.AreEqual(0x06, bus.ReadRam((ushort)(VS_BG + i)),
                    $"VS_GFX_SAVE_BG must copy gfx byte {i} into the background buffer");

            // Clobber the gfx region (colour 0x0F).
            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    SetGfxByte(bus, (y + row) * VgcConstants.GfxWidth + x + col, 0x0F);

            RunFn(bus, GFN_VS_GFX_RESTORE_BG);
            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    Assert.AreEqual(6, GfxPixel(bus, x + col, y + row),
                        $"VS_GFX_RESTORE_BG must restore gfx pixel ({x + col},{y + row})");
        }

        // --- $65 VS_ROTATE + $66 VS_GFX_BLIT: a 90deg rotate moves a marker pixel. ---
        // The blitter rotates ORIG->ROT (and vsprite_rotate repoints SRC*->ROT*),
        // so a following VS_GFX_BLIT lands the rotated copy on the gfx plane. With
        // angle 64 (=90deg) on a 4x4, the source top-right marker (3,0) maps to the
        // rotated bottom-right (3,3) — verified against the blitter rotation maths.
        [TestMethod]
        public void Axis2_VsRotateThenGfxBlit_RotatesMarkerNinetyDegrees()
        {
            using var bus = MakeAxis2Bus();

            const int w = 4, h = 4, x = 40, y = 80;
            // ORIG: fill 0x05, marker 0x0C at source (col=3,row=0).
            for (int i = 0; i < w * h; i++) bus.WriteRam((ushort)(VS_ORIG + i), 0x05);
            bus.WriteRam((ushort)(VS_ORIG + 0 * w + 3), 0x0C);   // src (3,0)

            // Config: ORIG* (immutable source), ROT* (output buffer), W/H, angle,
            // X/Y for the follow-up gfx blit. ORIG and ROT are well-separated CPU
            // buffers so the rotate's same-space overlap guard does not trip.
            Cfg(bus, VS_OFF_WIDTHL, w); Cfg(bus, VS_OFF_HEIGHTL, h);
            Cfg(bus, VS_OFF_ORIGSPACE, 0 /*CPU*/);
            Cfg(bus, VS_OFF_ORIGADDRL, (byte)(VS_ORIG & 0xFF));
            Cfg(bus, VS_OFF_ORIGADDRM, (byte)((VS_ORIG >> 8) & 0xFF));
            Cfg(bus, VS_OFF_ORIGADDRH, 0);
            Cfg(bus, VS_OFF_ORIGSTRL, w); Cfg(bus, VS_OFF_ORIGSTRH, 0);
            Cfg(bus, VS_OFF_ROTSPACE, 0 /*CPU*/);
            Cfg(bus, VS_OFF_ROTADDRL, (byte)(VS_ROT & 0xFF));
            Cfg(bus, VS_OFF_ROTADDRM, (byte)((VS_ROT >> 8) & 0xFF));
            Cfg(bus, VS_OFF_ROTADDRH, 0);
            Cfg(bus, VS_OFF_ROTSTRL, w); Cfg(bus, VS_OFF_ROTSTRH, 0);
            Cfg(bus, VS_OFF_ROTANGLE, 64);            // 90 degrees
            // SRC* := ROT buffer so the follow-up VS_GFX_BLIT reads the rotated
            // output. Each wrapper re-copies the whole cfg block before running,
            // so the SRC fields must be set in the struct (the BSS SRC* repoint
            // that vsprite_rotate does internally is overwritten by the next copy).
            Cfg(bus, VS_OFF_SRCSPACE, 0 /*CPU*/);
            Cfg(bus, VS_OFF_SRCADDRL, (byte)(VS_ROT & 0xFF));
            Cfg(bus, VS_OFF_SRCADDRM, (byte)((VS_ROT >> 8) & 0xFF));
            Cfg(bus, VS_OFF_SRCADDRH, 0);
            Cfg(bus, VS_OFF_SRCSTRL, w); Cfg(bus, VS_OFF_SRCSTRH, 0);
            Cfg(bus, VS_OFF_XL, (byte)x); Cfg(bus, VS_OFF_XH, 0);
            Cfg(bus, VS_OFF_Y, (byte)y);
            SetCfgArg(bus, VS_CFG, 41);

            // Rotate ORIG->ROT (self-waits; vsprite_rotate repoints SRC*->ROT*).
            RunFn(bus, GFN_VS_ROTATE);
            // The rotated buffer must carry the marker at its bottom-right (3,3).
            Assert.AreEqual(0x0C, bus.ReadRam((ushort)(VS_ROT + 3 * w + 3)),
                "VS_ROTATE must move the (3,0) marker to the rotated (3,3) cell");
            Assert.AreEqual(0x05, bus.ReadRam((ushort)(VS_ROT + 0 * w + 3)),
                "the rotated (3,0) cell must no longer hold the marker (rotation, not identity)");

            // Blit the rotated copy to the gfx plane at X/Y (cfg SRC* = ROT*).
            RunFn(bus, GFN_VS_GFX_BLIT);

            Assert.AreEqual(0x0C, GfxPixel(bus, x + 3, y + 3),
                "rotated marker must land at gfx (x+3,y+3)");
            Assert.AreEqual(0x05, GfxPixel(bus, x + 3, y + 0),
                "the rotated sprite's (3,0) gfx cell must be the background colour, not the marker");
        }

        // --- $6D VS_GFX_ROTATE_BLIT_NOWAIT: rotate + start blit without frame wait. ---
        [TestMethod]
        public void Axis2_VsGfxRotateBlitNowait_DrawsRotatedSpriteToGfx()
        {
            using var bus = MakeAxis2Bus();

            const int w = 4, h = 4, x = 10, y = 10;
            for (int i = 0; i < w * h; i++) bus.WriteRam((ushort)(VS_ORIG + i), 0x07);

            Cfg(bus, VS_OFF_WIDTHL, w); Cfg(bus, VS_OFF_HEIGHTL, h);
            Cfg(bus, VS_OFF_ORIGSPACE, 0); Cfg(bus, VS_OFF_ORIGADDRL, (byte)(VS_ORIG & 0xFF));
            Cfg(bus, VS_OFF_ORIGADDRM, (byte)((VS_ORIG >> 8) & 0xFF)); Cfg(bus, VS_OFF_ORIGADDRH, 0);
            Cfg(bus, VS_OFF_ORIGSTRL, w); Cfg(bus, VS_OFF_ORIGSTRH, 0);
            Cfg(bus, VS_OFF_ROTSPACE, 0); Cfg(bus, VS_OFF_ROTADDRL, (byte)(VS_ROT & 0xFF));
            Cfg(bus, VS_OFF_ROTADDRM, (byte)((VS_ROT >> 8) & 0xFF)); Cfg(bus, VS_OFF_ROTADDRH, 0);
            Cfg(bus, VS_OFF_ROTSTRL, w); Cfg(bus, VS_OFF_ROTSTRH, 0);
            Cfg(bus, VS_OFF_ROTANGLE, 0);   // identity rotate -> a solid square stays solid
            Cfg(bus, VS_OFF_XL, (byte)x); Cfg(bus, VS_OFF_XH, 0); Cfg(bus, VS_OFF_Y, (byte)y);
            SetCfgArg(bus, VS_CFG, 41);
            RunFn(bus, GFN_VS_GFX_ROTATE_BLIT_NOWAIT);   // blitter completes synchronously

            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    Assert.AreEqual(7, GfxPixel(bus, x + col, y + row),
                        $"VS_GFX_ROTATE_BLIT_NOWAIT must draw rotated pixel ({x + col},{y + row})");
        }

        // --- $6B VS_GFX_ROTATE_BLIT: rotate + WAIT a frame + blit (needs a pumped bus). ---
        [TestMethod]
        public void Axis2_VsGfxRotateBlit_FramePaced_DrawsRotatedSprite()
        {
            using var bus = MakeAxis2Bus();

            const int w = 4, h = 4, x = 200, y = 120;
            for (int i = 0; i < w * h; i++) bus.WriteRam((ushort)(VS_ORIG + i), 0x0A);

            Cfg(bus, VS_OFF_WIDTHL, w); Cfg(bus, VS_OFF_HEIGHTL, h);
            Cfg(bus, VS_OFF_ORIGSPACE, 0); Cfg(bus, VS_OFF_ORIGADDRL, (byte)(VS_ORIG & 0xFF));
            Cfg(bus, VS_OFF_ORIGADDRM, (byte)((VS_ORIG >> 8) & 0xFF)); Cfg(bus, VS_OFF_ORIGADDRH, 0);
            Cfg(bus, VS_OFF_ORIGSTRL, w); Cfg(bus, VS_OFF_ORIGSTRH, 0);
            Cfg(bus, VS_OFF_ROTSPACE, 0); Cfg(bus, VS_OFF_ROTADDRL, (byte)(VS_ROT & 0xFF));
            Cfg(bus, VS_OFF_ROTADDRM, (byte)((VS_ROT >> 8) & 0xFF)); Cfg(bus, VS_OFF_ROTADDRH, 0);
            Cfg(bus, VS_OFF_ROTSTRL, w); Cfg(bus, VS_OFF_ROTSTRH, 0);
            Cfg(bus, VS_OFF_ROTANGLE, 0);
            Cfg(bus, VS_OFF_XL, (byte)x); Cfg(bus, VS_OFF_XH, 0); Cfg(bus, VS_OFF_Y, (byte)y);
            SetCfgArg(bus, VS_CFG, 41);
            // The frame-pacing variant spins on VGC_FRAME; the pumped runner advances
            // the frame counter so vsprite_wait_frame exits.
            RunFnPumped(bus, GFN_VS_GFX_ROTATE_BLIT);

            for (int row = 0; row < h; row++)
                for (int col = 0; col < w; col++)
                    Assert.AreEqual(0x0A, GfxPixel(bus, x + col, y + row),
                        $"VS_GFX_ROTATE_BLIT must draw rotated pixel ({x + col},{y + row})");
        }

        // --- $6F VS_SCENE_COMMIT: a two-sprite scene draws both in z-order. ---
        // Exercises the descriptor-list ABI: the config struct carries the scene
        // table pointer/count; each descriptor (VSPRITE_DESC_*, 24 bytes) is a
        // caller-owned record. SCENE_COMMIT saves each background then draws every
        // VISIBLE descriptor in list order.
        [TestMethod]
        public void Axis2_VsSceneCommit_DrawsVisibleDescriptorsInOrder()
        {
            using var bus = MakeAxis2Bus();

            const ushort sceneTable = 0x0A00;   // 2 descriptors * 24 bytes
            const ushort srcA = 0x0B00, srcB = 0x0B40, bgA = 0x0C00, bgB = 0x0C40;
            const int w = 4, h = 4;
            // Two 4x4 sources: A colour 3 at (10,10), B colour 8 at (30,40).
            for (int i = 0; i < w * h; i++) { bus.WriteRam((ushort)(srcA + i), 0x03); bus.WriteRam((ushort)(srcB + i), 0x08); }

            void Desc(ushort baseAddr, int x, int y, ushort src, ushort bg)
            {
                // VSPRITE_DESC_* offsets (vsprite.inc): FLAGS 0, XL 1, XH 2, Y 3,
                // WIDTHL 7, WIDTHH 8, HEIGHTL 9, HEIGHTH 10, SRCSPACE 11,
                // SRCADDRL 12..H 14, SRCSTRL 15/H 16, BGSPACE 17, BGADDRL 18..H 20,
                // BGSTRL 21/H 22, COLORKEY 23.
                bus.WriteRam((ushort)(baseAddr + 0), 0x80);                 // FLAGS = VISIBLE
                bus.WriteRam((ushort)(baseAddr + 1), (byte)(x & 0xFF));
                bus.WriteRam((ushort)(baseAddr + 2), (byte)((x >> 8) & 0xFF));
                bus.WriteRam((ushort)(baseAddr + 3), (byte)y);
                bus.WriteRam((ushort)(baseAddr + 7), w); bus.WriteRam((ushort)(baseAddr + 8), 0);
                bus.WriteRam((ushort)(baseAddr + 9), h); bus.WriteRam((ushort)(baseAddr + 10), 0);
                bus.WriteRam((ushort)(baseAddr + 11), 0 /*CPU*/);
                bus.WriteRam((ushort)(baseAddr + 12), (byte)(src & 0xFF));
                bus.WriteRam((ushort)(baseAddr + 13), (byte)((src >> 8) & 0xFF));
                bus.WriteRam((ushort)(baseAddr + 14), 0);
                bus.WriteRam((ushort)(baseAddr + 15), w); bus.WriteRam((ushort)(baseAddr + 16), 0);
                bus.WriteRam((ushort)(baseAddr + 17), 0 /*CPU*/);
                bus.WriteRam((ushort)(baseAddr + 18), (byte)(bg & 0xFF));
                bus.WriteRam((ushort)(baseAddr + 19), (byte)((bg >> 8) & 0xFF));
                bus.WriteRam((ushort)(baseAddr + 20), 0);
                bus.WriteRam((ushort)(baseAddr + 21), w); bus.WriteRam((ushort)(baseAddr + 22), 0);
                bus.WriteRam((ushort)(baseAddr + 23), 0);
            }
            Desc(sceneTable, 10, 10, srcA, bgA);
            Desc((ushort)(sceneTable + 24), 30, 40, srcB, bgB);

            // Config: scene table ptr/count at offsets 41-43.
            Cfg(bus, VS_OFF_SCENE_ADDRL, (byte)(sceneTable & 0xFF));
            Cfg(bus, VS_OFF_SCENE_ADDRH, (byte)((sceneTable >> 8) & 0xFF));
            Cfg(bus, VS_OFF_SCENE_COUNT, 2);
            SetCfgArg(bus, VS_CFG, 44);   // include the scene fields
            RunFn(bus, GFN_VS_SCENE_COMMIT);

            // Both sprites must be drawn at their positions.
            Assert.AreEqual(3, GfxPixel(bus, 10, 10), "scene sprite A must draw at (10,10)");
            Assert.AreEqual(3, GfxPixel(bus, 13, 13), "scene sprite A far corner must draw");
            Assert.AreEqual(8, GfxPixel(bus, 30, 40), "scene sprite B must draw at (30,40)");
            Assert.AreEqual(8, GfxPixel(bus, 33, 43), "scene sprite B far corner must draw");
        }

        // --- Loader-axis smoke: a vsprite fn-id routes through the real lib_call. ---
        // VS_USE_ORIGINAL is a pure register-move (no peripheral), so on the
        // LibLoaderBus (no VGC/blitter) it still returns OK on dispatch.
        [TestMethod]
        public void Axis1_VsUseOriginal_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            // ARG0 = BYTES(ptr=0x0700, len=0) — a zero-length cfg copy is a no-op,
            // and the driver entry just moves ORIG*->SRC* in dead RAM.
            bus.PokeRam(ARG0, 0x00); bus.PokeRam((ushort)(ARG0 + 1), 0x07);   // ptr16 = $0700
            bus.PokeRam((ushort)(ARG0 + 2), 0x00); bus.PokeRam((ushort)(ARG0 + 3), 0x00);  // len16 = 0

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_VS_USE_ORIGINAL);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "VS_USE_ORIGINAL must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch density: a gap id past the vsprite range ($72) returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_VspriteRangeGap_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(FN_ID, 0x72);   // first id past GFX_FN_COUNT-1 ($71)
            bus.WriteRam(STATUS, 0x00);  // poison opposite to expected non-OK

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, "vsprite-range gap must still RTS to the sentinel");
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $72 (>= GFX_FN_COUNT) must report LERR_NO_FN via the bounds check");
        }

        // =====================================================================
        // Msprite / meta-sprite domain ($80-$8B), batch 4b.8. A meta-sprite OBJECT
        // owns a range of hardware sprites; SPAWN reads a caller-owned visual
        // descriptor (via the MSPRITE_DESC pointer) and returns an object HANDLE;
        // per-object ops mutate the object's state; COMMIT writes the owned $A040+
        // sprite attribute registers, observable via the real VGC sprite state.
        // The module's MSPRITE_* BSS object table persists across RunFn calls
        // (same bus), assumed zeroed at cold boot. No engine op issues a VGC
        // command, so all ops complete synchronously (RunFn, no pump needed).
        // =====================================================================

        // CPU-RAM scratch for descriptors, clear of the mailbox ($0300-$031F), the
        // module BSS ($0320-$03EF) and the stack.
        private const ushort MS_DESC = 0x0700;   // visual descriptor
        private const ushort MS_ANIM = 0x0780;   // animation descriptor

        // Write a 1-part visual descriptor at `addr`: partCount=1, flags=0, then
        // one part record {dx, dy, shapeBase, partFlags}. msprite reads partCount
        // from byte 0 and the parts from byte 2 onward (MSPRITE_VIS_PART* layout).
        private static void WriteDesc(CompositeBusDevice bus, ushort addr,
            byte dx = 0, byte dy = 0, byte shapeBase = 0, byte partFlags = 0)
        {
            bus.WriteRam(addr, 1);                 // part count
            bus.WriteRam((ushort)(addr + 1), 0);   // descriptor flags
            bus.WriteRam((ushort)(addr + 2), dx);
            bus.WriteRam((ushort)(addr + 3), dy);
            bus.WriteRam((ushort)(addr + 4), shapeBase);
            bus.WriteRam((ushort)(addr + 5), partFlags);
        }

        // Point ARG0 at a BYTES(descriptor): ptr16 low word (len ignored by spawn).
        private static void SetDescArg(CompositeBusDevice bus, ushort addr) =>
            SetArg(bus, ARG0, addr & 0xFFFF);

        // Spawn an object from the descriptor at MS_DESC; return its handle.
        private static byte SpawnDefault(CompositeBusDevice bus, byte dx = 0, byte dy = 0,
            byte shapeBase = 0)
        {
            WriteDesc(bus, MS_DESC, dx, dy, shapeBase);
            SetDescArg(bus, MS_DESC);
            RunFn(bus, GFN_MS_SPAWN);
            return (byte)GetResult(bus);
        }

        // Drive dispatch for `fn` allowing a non-OK STATUS (for failure-path tests);
        // returns nothing, leaves STATUS/RESULT for the caller to assert.
        private static void RunFnRaw(CompositeBusDevice bus, byte fn)
        {
            bus.WriteRam(FN_ID, fn);
            bus.WriteRam(STATUS, 0x7F);   // poison: neither OK ($00) nor the fail code

            var cpu = new Cpu(bus, E6502Type.Cmos);
            bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
            bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));
            var s = cpu.GetState();
            cpu.RestoreState(new CpuState(s.A, s.X, s.Y, 0xFD, 0xC000,
                                          s.Nf, s.Vf, s.Df, true, s.Zf, s.Cf));
            for (int guard = 0; guard < 2_000_000 && cpu.Pc != Sentinel; guard++)
                cpu.ExecuteNext();
            Assert.AreEqual(Sentinel, cpu.Pc, $"fn ${fn:X2} dispatch did not RTS to the sentinel");
        }

        // --- $80 MS_SPAWN: a valid descriptor yields a valid handle + STATUS=OK. ---
        [TestMethod]
        public void Axis2_MsSpawn_ReturnsValidHandle()
        {
            using var bus = MakeAxis2Bus();

            WriteDesc(bus, MS_DESC);
            SetDescArg(bus, MS_DESC);
            RunFn(bus, GFN_MS_SPAWN);   // RunFn asserts STATUS=OK

            // First spawn from a cold object table -> handle 0.
            Assert.AreEqual(0u, GetResult(bus), "first MS_SPAWN must return object handle 0");
        }

        // --- $80 MS_SPAWN: a NULL descriptor pointer fails ($FF + LERR_MSPRITE_FAIL). ---
        [TestMethod]
        public void Axis2_MsSpawn_NullDescriptor_FailsWithInvalidHandle()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 0);   // ptr16 = $0000 -> msprite_spawn rejects it
            RunFnRaw(bus, GFN_MS_SPAWN);

            Assert.AreEqual(MSPRITE_INVALID_HANDLE, (byte)GetResult(bus),
                "MS_SPAWN with a null descriptor must report the invalid handle $FF");
            Assert.AreEqual(LERR_MSPRITE_FAIL, bus.ReadRam(STATUS),
                "a failed MS_SPAWN must map to LERR_MSPRITE_FAIL");
        }

        // --- $80 MS_SPAWN: spawning beyond MSPRITE_MAX_OBJECTS (8) fails. ---
        [TestMethod]
        public void Axis2_MsSpawn_BeyondCapacity_Fails()
        {
            using var bus = MakeAxis2Bus();

            WriteDesc(bus, MS_DESC);   // 1 part each -> 8 objects use sprites 0-7
            SetDescArg(bus, MS_DESC);

            // 8 successful spawns (object slots 0-7).
            for (int i = 0; i < 8; i++)
            {
                RunFn(bus, GFN_MS_SPAWN);
                Assert.AreEqual((uint)i, GetResult(bus), $"spawn {i} must return handle {i}");
            }

            // 9th spawn: no free object slot -> $FF + fail.
            RunFnRaw(bus, GFN_MS_SPAWN);
            Assert.AreEqual(MSPRITE_INVALID_HANDLE, (byte)GetResult(bus),
                "the 9th MS_SPAWN must fail (only 8 object slots)");
            Assert.AreEqual(LERR_MSPRITE_FAIL, bus.ReadRam(STATUS),
                "spawn-beyond-capacity must map to LERR_MSPRITE_FAIL");
        }

        // --- $84 MS_POS + $8A MS_COMMIT: position lands in the owned hw sprite. ---
        [TestMethod]
        public void Axis2_MsPosThenCommit_WritesHwSpritePositionAndEnable()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus);
            Assert.AreEqual(0, h, "spawn handle");

            // MS_POS handle=0, x=120, y=80.
            SetArg(bus, ARG0, h);
            SetArg(bus, ARG1, 120);
            SetArg(bus, ARG2, 80);
            RunFn(bus, GFN_MS_POS);

            // Commit writes the owned hw sprite (index 0) registers.
            RunFn(bus, GFN_MS_COMMIT);

            var st = bus.Vgc.GetSpriteState(0);
            Assert.AreEqual(120, st.x, "MS_COMMIT must write the object X to hw sprite 0");
            Assert.AreEqual(80, st.y, "MS_COMMIT must write the object Y to hw sprite 0");
            Assert.IsTrue(st.enabled, "a committed visible object must enable its hw sprite");
            Assert.AreEqual(2, st.priority, "default priority is FRONT (2)");
        }

        // --- $84 MS_POS: a part dx/dy offset is added to the object position. ---
        [TestMethod]
        public void Axis2_MsCommit_AddsPartOffsetToPosition()
        {
            using var bus = MakeAxis2Bus();

            // Part offset dx=+5, dy=+3.
            byte h = SpawnDefault(bus, dx: 5, dy: 3);

            SetArg(bus, ARG0, h);
            SetArg(bus, ARG1, 50);
            SetArg(bus, ARG2, 40);
            RunFn(bus, GFN_MS_POS);
            RunFn(bus, GFN_MS_COMMIT);

            var st = bus.Vgc.GetSpriteState(0);
            Assert.AreEqual(55, st.x, "hw sprite X = object X (50) + part dx (5)");
            Assert.AreEqual(43, st.y, "hw sprite Y = object Y (40) + part dy (3)");
        }

        // --- $85 MS_FRAME + commit: the hw sprite shape index = shapeBase + frame. ---
        [TestMethod]
        public void Axis2_MsFrameThenCommit_SelectsShapeSlot()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus, shapeBase: 10);

            SetArg(bus, ARG0, h);
            SetArg(bus, ARG1, 4);   // frame 4
            RunFn(bus, GFN_MS_FRAME);
            RunFn(bus, GFN_MS_COMMIT);

            Assert.AreEqual(14, bus.Vgc.GetSpriteShapeIndex(0),
                "committed shape index = shapeBase (10) + frame (4)");
        }

        // --- $87 MS_PRIORITY + commit: the shared priority reaches the hw sprite. ---
        [TestMethod]
        public void Axis2_MsPriorityThenCommit_SetsHwSpritePriority()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus);

            SetArg(bus, ARG0, h);
            SetArg(bus, ARG1, 0);   // priority 0 = behind
            RunFn(bus, GFN_MS_PRIORITY);
            RunFn(bus, GFN_MS_COMMIT);

            Assert.AreEqual(0, bus.Vgc.GetSpriteState(0).priority,
                "MS_PRIORITY 0 must reach the committed hw sprite");
        }

        // --- $83 MS_HIDE: hiding disables the owned hw sprite immediately. ---
        [TestMethod]
        public void Axis2_MsHide_DisablesHwSprite()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus);
            SetArg(bus, ARG0, h);
            RunFn(bus, GFN_MS_COMMIT);
            Assert.IsTrue(bus.Vgc.GetSpriteState(0).enabled, "object must be enabled after commit");

            SetArg(bus, ARG0, h);
            RunFn(bus, GFN_MS_HIDE);
            Assert.IsFalse(bus.Vgc.GetSpriteState(0).enabled,
                "MS_HIDE must immediately disable the owned hw sprite");
        }

        // --- $82 MS_SHOW after hide + commit: re-enables the owned hw sprite. ---
        [TestMethod]
        public void Axis2_MsShowAfterHide_ReEnablesOnCommit()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus);
            SetArg(bus, ARG0, h); RunFn(bus, GFN_MS_HIDE);
            Assert.IsFalse(bus.Vgc.GetSpriteState(0).enabled, "hidden");

            SetArg(bus, ARG0, h); RunFn(bus, GFN_MS_SHOW);
            RunFn(bus, GFN_MS_COMMIT);
            Assert.IsTrue(bus.Vgc.GetSpriteState(0).enabled,
                "MS_SHOW + commit must re-enable the hw sprite");
        }

        // --- $81 MS_DESTROY: frees the object + disables/releases its hw sprites. ---
        [TestMethod]
        public void Axis2_MsDestroy_FreesObjectAndDisablesHwSprite()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus);
            SetArg(bus, ARG0, h); RunFn(bus, GFN_MS_COMMIT);
            Assert.IsTrue(bus.Vgc.GetSpriteState(0).enabled, "spawned + committed");

            SetArg(bus, ARG0, h);
            RunFn(bus, GFN_MS_DESTROY);
            Assert.IsFalse(bus.Vgc.GetSpriteState(0).enabled,
                "MS_DESTROY must disable the owned hw sprite");

            // The freed slot must be reusable: a fresh spawn returns handle 0 again
            // and re-acquires hw sprite 0.
            byte h2 = SpawnDefault(bus);
            Assert.AreEqual(0, h2, "a freed object slot must be reused by the next spawn");
        }

        // --- $89 MS_TICK + $86 MS_ANIM: tick advances the animation frame. ---
        // Anim descriptor: frameCount, ticksPerFrame, flags, reserved (MSPRITE_ANIM_*).
        // With ticksPerFrame=1 and a 4-frame loop, one tick advances frame 0 -> 1,
        // so the committed shape index moves from shapeBase to shapeBase+1.
        [TestMethod]
        public void Axis2_MsTick_AdvancesAnimationFrame()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus, shapeBase: 20);

            // Animation descriptor at MS_ANIM: 4 frames, 1 tick/frame, loop.
            bus.WriteRam(MS_ANIM + 0, 4);          // frame count
            bus.WriteRam(MS_ANIM + 1, 1);          // ticks per frame
            bus.WriteRam(MS_ANIM + 2, 0x01);       // flags = LOOP
            bus.WriteRam(MS_ANIM + 3, 0);          // reserved

            // Attach the anim (ARG1 = BYTES anim ptr low word).
            SetArg(bus, ARG0, h);
            SetArg(bus, ARG1, MS_ANIM & 0xFFFF);
            RunFn(bus, GFN_MS_ANIM);

            // Commit at frame 0 -> shape 20.
            RunFn(bus, GFN_MS_COMMIT);
            Assert.AreEqual(20, bus.Vgc.GetSpriteShapeIndex(0), "frame 0 -> shapeBase 20");

            // One tick -> frame 1 -> shape 21.
            RunFn(bus, GFN_MS_TICK);
            RunFn(bus, GFN_MS_COMMIT);
            Assert.AreEqual(21, bus.Vgc.GetSpriteShapeIndex(0),
                "MS_TICK must advance the frame so the committed shape is shapeBase+1");
        }

        // --- $8B MS_COMMIT_ONE: writes one object even when not dirty. ---
        [TestMethod]
        public void Axis2_MsCommitOne_WritesSingleObject()
        {
            using var bus = MakeAxis2Bus();

            byte h = SpawnDefault(bus);
            SetArg(bus, ARG0, h);
            SetArg(bus, ARG1, 33);
            SetArg(bus, ARG2, 44);
            RunFn(bus, GFN_MS_POS);

            SetArg(bus, ARG0, h);
            RunFn(bus, GFN_MS_COMMIT_ONE);

            var st = bus.Vgc.GetSpriteState(0);
            Assert.AreEqual(33, st.x, "MS_COMMIT_ONE must write the object X to hw sprite 0");
            Assert.AreEqual(44, st.y, "MS_COMMIT_ONE must write the object Y to hw sprite 0");
        }

        // --- Per-object op on a bad handle -> LERR_MSPRITE_FAIL. ---
        [TestMethod]
        public void Axis2_MsOp_BadHandle_FailsCleanly()
        {
            using var bus = MakeAxis2Bus();

            // No spawn -> handle 0 is inactive; MS_SHOW must reject it.
            SetArg(bus, ARG0, 0);
            RunFnRaw(bus, GFN_MS_SHOW);
            Assert.AreEqual(LERR_MSPRITE_FAIL, bus.ReadRam(STATUS),
                "an op on an inactive/bad handle must map to LERR_MSPRITE_FAIL");
        }

        // --- Loader-axis smoke: an msprite fn-id routes through the real lib_call. ---
        // MS_TICK is engine-wide with no peripheral dependency, so on the
        // LibLoaderBus (no VGC) it still returns OK on dispatch (cold BSS -> no
        // active objects -> the tick loop is a no-op).
        [TestMethod]
        public void Axis1_MsTick_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_MS_TICK);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "MS_TICK must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch density: a gap id below the msprite range ($72) and a gap
        // above it ($8C, between MS_COMMIT_ONE $8B and the image/mem range $A0)
        // both return LERR_NO_FN. ($A0 is now MEMREAD; the image/mem domain's own
        // bounds-check is exercised by Axis2_ImageMemRangeGap_ReturnsNoFn.) ---
        [TestMethod]
        public void Axis2_MspriteRangeGaps_ReturnNoFn()
        {
            using var bus = MakeAxis2Bus();

            // $72: a gap below the msprite range ($72-$7F all gfn_unimpl).
            bus.WriteRam(FN_ID, 0x72);
            bus.WriteRam(STATUS, 0x00);
            RunFnRaw(bus, 0x72);
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $72 (gap in the dense table) must report LERR_NO_FN");

            // $8C: a gap between the msprite range ($8B) and image/mem ($A0).
            bus.WriteRam(STATUS, 0x00);
            RunFnRaw(bus, 0x8C);
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $8C (gap in the dense table) must report LERR_NO_FN");
        }

        // =====================================================================
        // Image/mem domain ($A0-$A9), batch 4b.9. Drivers: vgc.s byte mem-I/O
        // (MEMREAD/MEMWRITE/VPEEK/VPOKE), fio.s GSAVE/GLOAD, nvg.s NVGload x4.
        //
        // MEMWRITE/MEMREAD/VPOKE/VPEEK are proven end-to-end against the real VGC
        // memory spaces (Axis 2). GSAVE/GLOAD and NVGLOAD are proven end-to-end
        // through the real FileIoController, which is wired into the test bus with
        // the VGC read/write/length accessors and a hermetic per-assembly save
        // directory (AssemblySetup sets NOVA_STORAGE_ROOT; HD0 is always mounted).
        // =====================================================================

        // The HD0 host directory the FileIoController saves into (NOVA_STORAGE_ROOT
        // is set by AssemblySetup to a unique per-run temp dir).
        private static string Hd0Dir()
        {
            string? root = Environment.GetEnvironmentVariable("NOVA_STORAGE_ROOT");
            Assert.IsNotNull(root, "AssemblySetup must set NOVA_STORAGE_ROOT");
            string dir = Path.Combine(root!, "hd0");
            Directory.CreateDirectory(dir);
            return dir;
        }

        private static byte[] BuildNvg2(int width, int height, byte[] pixels)
        {
            Assert.AreEqual(width * height, pixels.Length, "fixture pixel dimensions must match");

            var packed = new System.Collections.Generic.List<byte>();
            for (int y = 0; y < height; y++)
            {
                int row = y * width;
                for (int x = 0; x < width; x += 2)
                {
                    byte left = (byte)(pixels[row + x] & 0x0F);
                    byte right = x + 1 < width ? (byte)(pixels[row + x + 1] & 0x0F) : (byte)0;
                    packed.Add((byte)((left << 4) | right));
                }
            }

            var nvg = new System.Collections.Generic.List<byte>
            {
                (byte)'N', (byte)'V', (byte)'G', (byte)'2',
                (byte)(width & 0xFF), (byte)(width >> 8),
                (byte)(height & 0xFF), (byte)(height >> 8),
                0x00, 0xFF,             // no embedded palette, no transparent key
                16, 0,                  // payload offset
                (byte)(packed.Count & 0xFF), (byte)(packed.Count >> 8), 0, 0,
            };
            nvg.AddRange(packed);
            return nvg.ToArray();
        }

        // --- $A1 MEMWRITE then $A0 MEMREAD: round-trip a byte through a VGC space. ---
        [TestMethod]
        public void Axis2_MemWriteThenMemRead_RoundTripsAByte()
        {
            using var bus = MakeAxis2Bus();

            // Write 0x5A to gfx space at offset 0x1234.
            SetArg(bus, ARG0, VgcConstants.MemSpaceGfx);   // space
            SetArg(bus, ARG1, 0x1234);                     // address (16-bit)
            SetArg(bus, ARG2, 0x5A);                       // value
            RunFn(bus, GFN_MEMWRITE);

            // The real VGC gfx plane must hold the byte.
            Assert.AreEqual(0x5A, GfxByte(bus, 0x1234), "MEMWRITE must land the byte in the gfx plane");

            // MEMREAD must report it back into LIB_RESULT.
            SetArg(bus, ARG0, VgcConstants.MemSpaceGfx);
            SetArg(bus, ARG1, 0x1234);
            RunFn(bus, GFN_MEMREAD);
            Assert.AreEqual(0x5Au, GetResult(bus), "MEMREAD must report the byte written at the same address");

            // A neighbouring byte must be untouched (autoinc off -> single byte).
            Assert.AreEqual(0x00, GfxByte(bus, 0x1235), "MEMWRITE must not touch the neighbouring byte");
        }

        // --- MEMWRITE/MEMREAD on a DIFFERENT space (color RAM) -> space arg honored. ---
        [TestMethod]
        public void Axis2_MemWriteRead_HonorsTheSpaceArgument()
        {
            using var bus = MakeAxis2Bus();

            // Write 0x0C to color space at offset 200; read it back.
            SetArg(bus, ARG0, VgcConstants.MemSpaceColor);
            SetArg(bus, ARG1, 200);
            SetArg(bus, ARG2, 0x0C);
            RunFn(bus, GFN_MEMWRITE);

            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceColor, 200, out byte v),
                "color-space read must succeed");
            Assert.AreEqual(0x0C, v, "MEMWRITE must write the color space when ARG0 = MemSpaceColor");

            // The same offset in the gfx space must be untouched (proves the space arg routes).
            Assert.AreEqual(0x00, GfxByte(bus, 200), "MEMWRITE to color space must not touch the gfx space");

            SetArg(bus, ARG0, VgcConstants.MemSpaceColor);
            SetArg(bus, ARG1, 200);
            RunFn(bus, GFN_MEMREAD);
            Assert.AreEqual(0x0Cu, GetResult(bus), "MEMREAD must report the color-space byte");
        }

        // --- $A3 VPOKE then $A2 VPEEK: round-trip a gfx-plane byte (fixed space). ---
        [TestMethod]
        public void Axis2_VPokeThenVPeek_RoundTripsAGfxByte()
        {
            using var bus = MakeAxis2Bus();

            // VPOKE offset 500 = 0x0E (a visible color index in the gfx plane).
            SetArg(bus, ARG0, 500);    // gfx offset
            SetArg(bus, ARG1, 0x0E);   // value
            RunFn(bus, GFN_VPOKE);

            // The gfx pixel/byte at that offset must have changed.
            Assert.AreEqual(0x0E, GfxByte(bus, 500), "VPOKE must set the gfx byte at the offset");
            Assert.AreEqual(0x00, GfxByte(bus, 501), "VPOKE must not touch the neighbouring gfx byte");

            // VPEEK must read it back into LIB_RESULT.
            SetArg(bus, ARG0, 500);
            RunFn(bus, GFN_VPEEK);
            Assert.AreEqual(0x0Eu, GetResult(bus), "VPEEK must report the gfx byte VPOKE wrote");
        }

        // --- $A4 GSAVE then $A5 GLOAD: save a gfx region, clobber it, reload it. ---
        [TestMethod]
        public void Axis2_GSaveThenGLoad_RoundTripsAGfxRegion()
        {
            using var bus = MakeAxis2Bus();

            // Seed a 32-byte region in the gfx plane at offset 0.
            const int addr = 0, len = 32;
            for (int i = 0; i < len; i++) SetGfxByte(bus, addr + i, (byte)(0x10 + i));

            // Stage the filename "GMEMTEST" in CPU RAM; ARG3 = ptr16 | len16<<16.
            const ushort nameAddr = 0x0500;
            byte[] name = System.Text.Encoding.ASCII.GetBytes("GMEMTEST");
            for (int i = 0; i < name.Length; i++) bus.WriteRam((ushort)(nameAddr + i), name[i]);

            // GSAVE space=gfx, addr=0, len=32, name BYTES.
            SetArg(bus, ARG0, VgcConstants.MemSpaceGfx);
            SetArg(bus, ARG1, addr);
            SetArg(bus, ARG2, len);
            SetArg(bus, ARG3, nameAddr | (name.Length << 16));
            RunFn(bus, GFN_GSAVE);   // RunFn asserts STATUS = OK
            Assert.AreEqual(0u, GetResult(bus), "GSAVE must report driver result 0 (OK)");

            // The .gfx file must exist on HD0.
            Assert.IsTrue(File.Exists(Path.Combine(Hd0Dir(), "GMEMTEST.gfx")),
                "GSAVE must write the .gfx file to the host save directory");

            // Clobber the gfx region.
            for (int i = 0; i < len; i++) SetGfxByte(bus, addr + i, 0xFF);

            // GLOAD the same region back.
            SetArg(bus, ARG0, VgcConstants.MemSpaceGfx);
            SetArg(bus, ARG1, addr);
            SetArg(bus, ARG2, len);
            SetArg(bus, ARG3, nameAddr | (name.Length << 16));
            RunFn(bus, GFN_GLOAD);
            Assert.AreEqual(0u, GetResult(bus), "GLOAD must report driver result 0 (OK)");

            for (int i = 0; i < len; i++)
                Assert.AreEqual((byte)(0x10 + i), GfxByte(bus, addr + i),
                    $"GLOAD must restore gfx byte {i} from the saved file");
        }

        // --- $A5 GLOAD of a missing file -> A=1 -> LERR_FILE_FAIL. ---
        [TestMethod]
        public void Axis2_GLoad_MissingFile_ReportsFileFail()
        {
            using var bus = MakeAxis2Bus();

            const ushort nameAddr = 0x0500;
            byte[] name = System.Text.Encoding.ASCII.GetBytes("NOSUCHGFX");
            for (int i = 0; i < name.Length; i++) bus.WriteRam((ushort)(nameAddr + i), name[i]);

            SetArg(bus, ARG0, VgcConstants.MemSpaceGfx);
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, 16);
            SetArg(bus, ARG3, nameAddr | (name.Length << 16));
            RunFnRaw(bus, GFN_GLOAD);

            Assert.AreEqual(1u, GetResult(bus), "GLOAD of a missing file must report driver result 1");
            Assert.AreEqual(LERR_FILE_FAIL, bus.ReadRam(STATUS),
                "a failed GLOAD must map to LERR_FILE_FAIL");
        }

        // --- $A6 NVGLOAD: load a native packed NVG2 fixture into the gfx plane at offset 0. ---
        [TestMethod]
        public void Axis2_NvgLoad_DecodesFixtureIntoGfxPlane()
        {
            using var bus = MakeAxis2Bus();

            byte[] pixels = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
            File.WriteAllBytes(Path.Combine(Hd0Dir(), "NVGFIX.NVG"), BuildNvg2(4, 2, pixels));

            // Stage filename "NVGFIX"; NVGLOAD reads FIO_NAME via the BYTES arg.
            const ushort nameAddr = 0x0500;
            byte[] name = System.Text.Encoding.ASCII.GetBytes("NVGFIX");
            for (int i = 0; i < name.Length; i++) bus.WriteRam((ushort)(nameAddr + i), name[i]);

            SetArg(bus, ARG0, nameAddr | (name.Length << 16));
            RunFn(bus, GFN_NVGLOAD);   // RunFn asserts STATUS = OK
            Assert.AreEqual(0u, GetResult(bus), "NVGLOAD must report driver result 0 (OK)");

            // Row 0 lands at gfx offsets 0..3; row 1 at 320..323 (linear 320-wide rows).
            for (int x = 0; x < 4; x++)
                Assert.AreEqual(pixels[x], GfxByte(bus, x), $"NVGLOAD row 0 pixel {x}");
            for (int x = 0; x < 4; x++)
                Assert.AreEqual(pixels[4 + x], GfxByte(bus, VgcConstants.GfxWidth + x),
                    $"NVGLOAD row 1 pixel {x}");
        }

        // --- $A7 NVGLOAD_AT: load the fixture at a non-zero destination offset. ---
        [TestMethod]
        public void Axis2_NvgLoadAt_DecodesFixtureAtDestOffset()
        {
            using var bus = MakeAxis2Bus();

            byte[] pixels = { 0x0A, 0x0B, 0x0C, 0x0D };
            File.WriteAllBytes(Path.Combine(Hd0Dir(), "NVGAT.NVG"), BuildNvg2(4, 1, pixels));

            const ushort nameAddr = 0x0500;
            byte[] name = System.Text.Encoding.ASCII.GetBytes("NVGAT");
            for (int i = 0; i < name.Length; i++) bus.WriteRam((ushort)(nameAddr + i), name[i]);

            // dest = row 5 (linear offset 5*320). baseX=0, baseY=5.
            int dest = 5 * VgcConstants.GfxWidth;
            SetArg(bus, ARG0, nameAddr | (name.Length << 16));
            SetArg(bus, ARG1, dest);
            RunFn(bus, GFN_NVGLOAD_AT);
            Assert.AreEqual(0u, GetResult(bus), "NVGLOAD_AT must report driver result 0 (OK)");

            for (int x = 0; x < 4; x++)
                Assert.AreEqual(pixels[x], GfxByte(bus, dest + x), $"NVGLOAD_AT pixel {x} at dest offset");
        }

        // --- $A8 NVGLOAD_NAMED: the named-arg path loads the same fixture. ---
        [TestMethod]
        public void Axis2_NvgLoadNamed_DecodesViaNamedArgPath()
        {
            using var bus = MakeAxis2Bus();

            byte[] pixels = { 0x09, 0x08, 0x07, 0x06 };
            File.WriteAllBytes(Path.Combine(Hd0Dir(), "NVGNAMED.NVG"), BuildNvg2(4, 1, pixels));

            const ushort nameAddr = 0x0500;
            byte[] name = System.Text.Encoding.ASCII.GetBytes("NVGNAMED");
            for (int i = 0; i < name.Length; i++) bus.WriteRam((ushort)(nameAddr + i), name[i]);

            SetArg(bus, ARG0, nameAddr | (name.Length << 16));
            RunFn(bus, GFN_NVGLOAD_NAMED);
            Assert.AreEqual(0u, GetResult(bus), "NVGLOAD_NAMED must report driver result 0 (OK)");

            for (int x = 0; x < 4; x++)
                Assert.AreEqual(pixels[x], GfxByte(bus, x), $"NVGLOAD_NAMED pixel {x}");
        }

        // --- $A6 NVGLOAD of a missing file -> A=1 -> LERR_IMAGE_FAIL. ---
        [TestMethod]
        public void Axis2_NvgLoad_MissingFile_ReportsImageFail()
        {
            using var bus = MakeAxis2Bus();

            const ushort nameAddr = 0x0500;
            byte[] name = System.Text.Encoding.ASCII.GetBytes("NOSUCHNVG");
            for (int i = 0; i < name.Length; i++) bus.WriteRam((ushort)(nameAddr + i), name[i]);

            SetArg(bus, ARG0, nameAddr | (name.Length << 16));
            RunFnRaw(bus, GFN_NVGLOAD);

            Assert.AreEqual(1u, GetResult(bus), "NVGLOAD of a missing file must report driver result 1");
            Assert.AreEqual(LERR_IMAGE_FAIL, bus.ReadRam(STATUS),
                "a failed NVGLOAD must map to LERR_IMAGE_FAIL");
        }

        // --- Loader-axis smoke: an image/mem fn-id routes through the real lib_call. ---
        // MEMWRITE is used (a VGC mem op): on LibLoaderBus the VGC command write
        // lands in dead RAM and vgc_wait_cmd reads a 0 busy bit, so the wrapper
        // returns OK on dispatch without a peripheral.
        [TestMethod]
        public void Axis1_MemWrite_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            bus.PokeRam(ARG0, VgcConstants.MemSpaceGfx);
            bus.PokeRam(ARG1, 0x00); bus.PokeRam((ushort)(ARG1 + 1), 0x10);   // addr $1000
            bus.PokeRam(ARG2, 0x42);                                          // value

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_MEMWRITE);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "MEMWRITE must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }

        // --- Dispatch density: a gap id above the image/mem range ($AA) returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_ImageMemRangeGap_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(STATUS, 0x00);   // poison opposite to expected non-OK
            RunFnRaw(bus, 0xAA);          // first id past GFX_FN_COUNT-1 ($A9)
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $AA (>= GFX_FN_COUNT) must report LERR_NO_FN via the bounds check");
        }

        // =====================================================================
        // Tween domain ($D0-$DA), batch 4b.11. Pure 16-bit easing math — no
        // hardware. Every op marshals start/end/duration/mode/frame from the ARG
        // cells (ARG0=start s16, ARG1=end s16, ARG2.b0=duration, ARG2.b1=mode,
        // ARG3.b0=frame) and packs the readbacks into LIB_RESULT:
        //   byte0=VALUEL, byte1=VALUEH, byte2=DONE, byte3=PROGRESS.
        // These are cleanly unit-testable: the interpolation is deterministic, so
        // we assert exact values (verified against runtime/asm/tween.s).
        // =====================================================================

        // Decode the packed tween result. byte0/1 = 16-bit value, byte2 = done,
        // byte3 = progress.
        private static (ushort value, byte done, byte progress) TweenResult(CompositeBusDevice bus)
        {
            uint r = GetResult(bus);
            return ((ushort)(r & 0xFFFF), (byte)((r >> 16) & 0xFF), (byte)((r >> 24) & 0xFF));
        }

        // Set the five tween inputs in the standard ARG layout.
        private static void SetTweenArgs(CompositeBusDevice bus, int start, int end,
                                         byte duration, byte frame, byte mode = 0)
        {
            SetArg(bus, ARG0, start);
            SetArg(bus, ARG1, end);
            SetArg(bus, ARG2, duration | (mode << 8));
            SetArg(bus, ARG3, frame);
        }

        // --- $D2 TWEEN_EVAL_LINEAR: the midpoint frame interpolates halfway. ---
        // start=0, end=200, duration=10, frame=5 -> progress=floor(5*255/10)=127,
        // value=(0x80 + 200*127)>>8 = 99 (8.8 rounded). Verified against tween.s.
        [TestMethod]
        public void Axis2_TweenEvalLinear_MidpointInterpolates()
        {
            using var bus = MakeAxis2Bus();

            SetTweenArgs(bus, start: 0, end: 200, duration: 10, frame: 5);
            RunFn(bus, GFN_TWEEN_EVAL_LINEAR);

            var (value, done, progress) = TweenResult(bus);
            Assert.AreEqual(99, value, "linear midpoint of 0->200 at frame 5/10 must be 99");
            Assert.AreEqual(127, progress, "progress at frame 5/10 must be floor(5*255/10)=127");
            Assert.AreEqual(0, done, "frame 5 of 10 is not the end -> DONE must be 0");
        }

        // --- $D2 TWEEN_EVAL_LINEAR: frame == duration snaps exactly to END + done. ---
        [TestMethod]
        public void Axis2_TweenEvalLinear_AtDuration_SnapsToEndAndSetsDone()
        {
            using var bus = MakeAxis2Bus();

            SetTweenArgs(bus, start: 0, end: 200, duration: 10, frame: 10);
            RunFn(bus, GFN_TWEEN_EVAL_LINEAR);

            var (value, done, progress) = TweenResult(bus);
            Assert.AreEqual(200, value, "at frame == duration the value must snap to END (200)");
            Assert.AreNotEqual(0, done, "at frame == duration DONE must be non-zero");
            Assert.AreEqual(255, progress, "the snap sets PROGRESS to $FF (255)");
        }

        // --- $D0 TWEEN_BEGIN: resets to START with DONE clear (frame 0). ---
        [TestMethod]
        public void Axis2_TweenBegin_PublishesStartValue()
        {
            using var bus = MakeAxis2Bus();

            // start=500, end=1000, duration=8.
            SetTweenArgs(bus, start: 500, end: 1000, duration: 8, frame: 0);
            RunFn(bus, GFN_TWEEN_BEGIN);

            var (value, done, _) = TweenResult(bus);
            Assert.AreEqual(500, value, "TWEEN_BEGIN must publish the START value (500)");
            Assert.AreEqual(0, done, "TWEEN_BEGIN must clear DONE");
        }

        // --- $D6 TWEEN_STEP: advances the frame by one before evaluating. ---
        // Loading frame=4 then stepping evaluates at frame 5 -> same as the eval
        // midpoint (value 99). Proves the step path increments TWEEN_FRAME.
        [TestMethod]
        public void Axis2_TweenStep_AdvancesFrameBeforeEval()
        {
            using var bus = MakeAxis2Bus();

            SetTweenArgs(bus, start: 0, end: 200, duration: 10, frame: 4,
                         mode: TWEEN_MODE_LINEAR);
            RunFn(bus, GFN_TWEEN_STEP);

            var (value, done, progress) = TweenResult(bus);
            Assert.AreEqual(99, value, "STEP from frame 4 evaluates at frame 5 -> value 99");
            Assert.AreEqual(127, progress, "STEP must evaluate progress at the advanced frame (5/10)");
            Assert.AreEqual(0, done, "frame 5 of 10 is not done");
        }

        // --- $D3/$D4 ease-in vs ease-out: the curves differ from linear + each other. ---
        // At the same progress (127), ease-in (slow start) lags linear and
        // ease-out (slow end) leads it. eased value computed from TWEEN_EASE.
        [TestMethod]
        public void Axis2_TweenEaseInVsEaseOut_DifferFromLinear()
        {
            using var bus = MakeAxis2Bus();

            // Linear reference at frame 5/10 of 0->200 = 99.
            SetTweenArgs(bus, 0, 200, 10, 5);
            RunFn(bus, GFN_TWEEN_EVAL_LINEAR);
            int lin = TweenResult(bus).value;

            SetTweenArgs(bus, 0, 200, 10, 5);
            RunFn(bus, GFN_TWEEN_EVAL_EASE_IN);
            int easeIn = TweenResult(bus).value;

            SetTweenArgs(bus, 0, 200, 10, 5);
            RunFn(bus, GFN_TWEEN_EVAL_EASE_OUT);
            int easeOut = TweenResult(bus).value;

            Assert.IsTrue(easeIn < lin,
                $"ease-in (slow start) must lag linear at the midpoint ({easeIn} < {lin})");
            Assert.IsTrue(easeOut > lin,
                $"ease-out (slow end) must lead linear at the midpoint ({easeOut} > {lin})");
        }

        // --- Loader-axis smoke: a tween fn-id routes through the real lib_call. ---
        // Tween is pure math with no peripheral dependency, so it returns OK on
        // dispatch through the loader.
        [TestMethod]
        public void Axis1_TweenEvalLinear_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            bus.PokeRam(ARG0, 0);  bus.PokeRam((ushort)(ARG0 + 1), 0);    // start 0
            bus.PokeRam(ARG1, 200); bus.PokeRam((ushort)(ARG1 + 1), 0);   // end 200
            bus.PokeRam(ARG2, 10);                                        // duration
            bus.PokeRam(ARG3, 5);                                         // frame

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_TWEEN_EVAL_LINEAR);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS),
                "TWEEN_EVAL_LINEAR must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
            // The result also round-trips through the loader (RESULT low word = 99).
            Assert.AreEqual(99, bus.PeekRam(RESULT) | (bus.PeekRam((ushort)(RESULT + 1)) << 8),
                "TWEEN result low word must come back through the loader as 99");
        }

        // =====================================================================
        // Anim domain ($C0-$C7), batch 4b.11. Track-based sprite animation. The
        // wrapper marshals the descriptor ptr/target/type into the ANIM_* cells;
        // the effect (sprite shape-slot change) is observable on the real VGC
        // after START + TICK, because anim_apply_selected calls sprite_set_shape.
        // =====================================================================

        private const ushort ANIM_DESC = 0x0600;   // caller-resident anim descriptor

        // Write a linear anim descriptor at ANIM_DESC: frameCount, ticksPerFrame,
        // flags, firstShape, stride (resolved shape = first + frame*stride).
        private static void WriteAnimDesc(CompositeBusDevice bus, byte frameCount,
                                          byte ticks, byte flags, byte first, byte stride)
        {
            bus.WriteRam((ushort)(ANIM_DESC + ANIM_DESC_FRAME_COUNT), frameCount);
            bus.WriteRam((ushort)(ANIM_DESC + ANIM_DESC_TICKS), ticks);
            bus.WriteRam((ushort)(ANIM_DESC + ANIM_DESC_FLAGS), flags);
            bus.WriteRam((ushort)(ANIM_DESC + ANIM_DESC_FIRST), first);
            bus.WriteRam((ushort)(ANIM_DESC + ANIM_DESC_STRIDE), stride);
            bus.WriteRam((ushort)(ANIM_DESC + ANIM_DESC_RESERVED), 0);
        }

        // --- $C1 ANIM_START: attaching a descriptor to a sprite sets the frame-0 shape. ---
        [TestMethod]
        public void Axis2_AnimStart_SetsFirstShapeOnTargetSprite()
        {
            using var bus = MakeAxis2Bus();

            // 4-frame loop, 1 tick/frame, linear shapes from slot 20, stride 1.
            WriteAnimDesc(bus, frameCount: 4, ticks: 1, flags: ANIM_DESC_LOOP,
                          first: 20, stride: 1);

            SetArg(bus, ARG0, ANIM_DESC & 0xFFFF);   // BYTES descriptor ptr
            SetArg(bus, ARG1, 0);                    // target sprite 0
            SetArg(bus, ARG2, ANIM_TARGET_SPRITE);   // target type
            RunFn(bus, GFN_ANIM_START);

            Assert.AreEqual(0u, GetResult(bus), "first ANIM_START must return track handle 0");
            Assert.AreEqual(20, bus.Vgc.GetSpriteShapeIndex(0),
                "ANIM_START must apply frame 0 -> firstShape (20) to the target sprite");
        }

        // --- $C3 ANIM_TICK: one tick advances the frame so the applied shape steps. ---
        [TestMethod]
        public void Axis2_AnimTick_AdvancesShapeOnTargetSprite()
        {
            using var bus = MakeAxis2Bus();

            WriteAnimDesc(bus, frameCount: 4, ticks: 1, flags: ANIM_DESC_LOOP,
                          first: 20, stride: 1);
            SetArg(bus, ARG0, ANIM_DESC & 0xFFFF);
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, ANIM_TARGET_SPRITE);
            RunFn(bus, GFN_ANIM_START);
            Assert.AreEqual(20, bus.Vgc.GetSpriteShapeIndex(0), "setup: frame 0 -> shape 20");

            // One engine-wide tick: frame 0 -> 1 (1 tick/frame), shape -> 21.
            RunFn(bus, GFN_ANIM_TICK);
            Assert.AreEqual(21, bus.Vgc.GetSpriteShapeIndex(0),
                "ANIM_TICK must advance frame 0 -> 1 so the applied shape becomes firstShape+stride");
        }

        // --- $C5 ANIM_SET_FRAME: jumping to a frame applies that frame's shape. ---
        [TestMethod]
        public void Axis2_AnimSetFrame_AppliesRequestedFrameShape()
        {
            using var bus = MakeAxis2Bus();

            WriteAnimDesc(bus, frameCount: 4, ticks: 1, flags: ANIM_DESC_LOOP,
                          first: 20, stride: 1);
            SetArg(bus, ARG0, ANIM_DESC & 0xFFFF);
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, ANIM_TARGET_SPRITE);
            RunFn(bus, GFN_ANIM_START);   // handle 0

            // Jump track 0 to frame 3 -> shape 20 + 3*1 = 23.
            SetArg(bus, ARG0, 0);
            SetArg(bus, ARG1, 3);
            RunFn(bus, GFN_ANIM_SET_FRAME);
            Assert.AreEqual(23, bus.Vgc.GetSpriteShapeIndex(0),
                "ANIM_SET_FRAME to frame 3 must apply firstShape + 3*stride (23)");
        }

        // --- $C1 ANIM_START with a zero-frame (invalid) descriptor -> LERR_ANIM_FAIL. ---
        [TestMethod]
        public void Axis2_AnimStart_BadDescriptor_FailsCleanly()
        {
            using var bus = MakeAxis2Bus();

            // frameCount 0 is rejected by anim_start (@bad_desc -> $FF handle).
            WriteAnimDesc(bus, frameCount: 0, ticks: 1, flags: 0, first: 0, stride: 1);
            SetArg(bus, ARG0, ANIM_DESC & 0xFFFF);
            SetArg(bus, ARG1, 0);
            SetArg(bus, ARG2, ANIM_TARGET_SPRITE);
            RunFnRaw(bus, GFN_ANIM_START);

            Assert.AreEqual(ANIM_INVALID_HANDLE, (byte)GetResult(bus),
                "a zero-frame descriptor must return the invalid handle ($FF)");
            Assert.AreEqual(LERR_ANIM_FAIL, bus.ReadRam(STATUS),
                "a failed ANIM_START must map to LERR_ANIM_FAIL");
        }

        // --- $C2 ANIM_STOP on an unattached handle -> LERR_ANIM_FAIL (bad handle). ---
        [TestMethod]
        public void Axis2_AnimStop_BadHandle_FailsCleanly()
        {
            using var bus = MakeAxis2Bus();

            SetArg(bus, ARG0, 0);   // no track started -> handle 0 is inactive
            RunFnRaw(bus, GFN_ANIM_STOP);
            Assert.AreEqual(LERR_ANIM_FAIL, bus.ReadRam(STATUS),
                "ANIM_STOP on an inactive handle must map to LERR_ANIM_FAIL");
        }

        // --- Loader-axis smoke: an anim fn-id routes through the real lib_call. ---
        // ANIM_INIT clears the track table with no peripheral dependency, so it
        // returns OK on dispatch through the loader.
        [TestMethod]
        public void Axis1_AnimInit_RoutesThroughLoader_StatusOk()
        {
            var (bus, entry) = SetupLoader();

            CallLib(bus, entry, MODULE_ID_GRAPHICS, GFN_ANIM_INIT);

            Assert.AreEqual(LERR_OK, bus.PeekRam(STATUS), "ANIM_INIT must report OK through the loader");
            Assert.AreEqual(MODULE_ID_GRAPHICS, bus.PeekRam(RESIDENT),
                "GRAPHICS module must be resident after the page-in");
        }


        // --- Dispatch density: the $B0-$BF range is now VACANT in GRAPHICS (the
        // turtle $B0-$B3 was extracted into the TURTLE module $07); every id in it
        // is gfn_unimpl -> LERR_NO_FN. Probe $B4 (always was a gap) and $B0 (the
        // vacated former-turtle id) to prove the range reports LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_TurtleRenderRangeGap_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(STATUS, 0x00);   // poison opposite to expected non-OK
            RunFnRaw(bus, 0xB4);          // $B4-$BF are reserved
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $B4 (reserved gap) must report LERR_NO_FN");

            bus.WriteRam(STATUS, 0x00);
            RunFnRaw(bus, 0xB0);          // former TURTLE_INIT id — now vacated
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $B0 (turtle extracted -> module $07) must now report LERR_NO_FN");
        }

        // --- Dispatch bounds: an id at/above GFX_FN_COUNT ($DB) returns LERR_NO_FN. ---
        [TestMethod]
        public void Axis2_AboveTweenRange_ReturnsNoFn()
        {
            using var bus = MakeAxis2Bus();

            bus.WriteRam(STATUS, 0x00);
            RunFnRaw(bus, 0xDB);          // first id past the last tween fn ($DA)
            Assert.AreEqual(LERR_NO_FN, bus.ReadRam(STATUS),
                "id $DB (>= GFX_FN_COUNT) must report LERR_NO_FN via the bounds check");
        }

        private static string RepoPath(params string[] parts)
        {
            string root = Path.GetFullPath(Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", ".."));
            return Path.Combine([root, .. parts]);
        }
    }
}
