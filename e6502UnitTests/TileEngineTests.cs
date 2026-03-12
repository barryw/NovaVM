using System;
using System.IO;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class TileEngineTests
{
    private VirtualGraphicsController _vgc = null!;
    private byte[] _ram = null!;

    [TestInitialize]
    public void Setup()
    {
        _vgc = new VirtualGraphicsController();
        _ram = new byte[65536];
        _vgc.SetBusMemory(_ram);
    }

    // -- Helper methods -------------------------------------------------------

    private void WriteTileReg(int addr, byte val) => _vgc.Write(addr, val);

    private void ExecuteTileCmd(byte cmd,
        byte p0 = 0, byte p1 = 0, byte p2 = 0, byte p3 = 0,
        ushort addr = 0, byte palP0 = 0, byte palP1 = 0)
    {
        WriteTileReg(VgcConstants.TileP0, p0);
        WriteTileReg(VgcConstants.TileP1, p1);
        WriteTileReg(VgcConstants.TileP2, p2);
        WriteTileReg(VgcConstants.TileP3, p3);
        WriteTileReg(VgcConstants.TileAddrL, (byte)(addr & 0xFF));
        WriteTileReg(VgcConstants.TileAddrH, (byte)(addr >> 8));
        WriteTileReg(VgcConstants.TilePalP0, palP0);
        WriteTileReg(VgcConstants.TilePalP1, palP1);
        WriteTileReg(VgcConstants.TileCmd, cmd);
    }

    // -- Initial state --------------------------------------------------------

    [TestMethod]
    public void InitialTileConfig_IsZero() =>
        Assert.AreEqual(0, _vgc.Read(VgcConstants.TileConfig));

    [TestMethod]
    public void InitialTileTransColor_IsZero() =>
        Assert.AreEqual(0, _vgc.GetTileTransColor());

    [TestMethod]
    public void InitialTileScroll_IsZero()
    {
        Assert.AreEqual(0, _vgc.GetTileScrollX());
        Assert.AreEqual(0, _vgc.GetTileScrollY());
    }

    [TestMethod]
    public void InitialNametables_AreZero()
    {
        for (int nt = 0; nt < VgcConstants.NametableCount; nt++)
        {
            var data = _vgc.GetNametable(nt);
            for (int i = 0; i < VgcConstants.NametableSize; i++)
                Assert.AreEqual(0, data[i], $"NT{nt}[{i}] should be 0");
        }
    }

    [TestMethod]
    public void InitialPalette0_MatchesDefaultColors()
    {
        var pal = _vgc.GetTilePaletteRam();
        for (int i = 0; i < VgcConstants.TilePaletteColors; i++)
        {
            var expected = ColorPalette.Get(i);
            Assert.AreEqual(expected.R, pal[i * 3], $"Color {i} R");
            Assert.AreEqual(expected.G, pal[i * 3 + 1], $"Color {i} G");
            Assert.AreEqual(expected.B, pal[i * 3 + 2], $"Color {i} B");
        }
    }

    // -- Config registers -----------------------------------------------------

    [TestMethod]
    public void TileConfig_RoundTrips()
    {
        WriteTileReg(VgcConstants.TileConfig, 0x05); // 16x16 + mirror mode 2
        Assert.AreEqual(0x05, _vgc.Read(VgcConstants.TileConfig));
        Assert.IsTrue(_vgc.IsTileSize16);
        Assert.AreEqual(2, _vgc.GetTileMirrorMode());
    }

    [TestMethod]
    public void TileTransColor_ClampedTo4Bits()
    {
        WriteTileReg(VgcConstants.TileTransColor, 0xFF);
        Assert.AreEqual(0x0F, _vgc.GetTileTransColor());
    }

    [TestMethod]
    public void TileScroll_16Bit_RoundTrips()
    {
        WriteTileReg(VgcConstants.TileScrollXL, 0x34);
        WriteTileReg(VgcConstants.TileScrollXH, 0x02);
        WriteTileReg(VgcConstants.TileScrollYL, 0x90);
        WriteTileReg(VgcConstants.TileScrollYH, 0x01);

        Assert.AreEqual(0x0234, _vgc.GetTileScrollX());
        Assert.AreEqual(0x0190, _vgc.GetTileScrollY());

        Assert.AreEqual(0x34, _vgc.Read(VgcConstants.TileScrollXL));
        Assert.AreEqual(0x02, _vgc.Read(VgcConstants.TileScrollXH));
    }

    // -- TDEF (define single tile) --------------------------------------------

    [TestMethod]
    public void TileDef_8x8_CopiesFromRam()
    {
        // Put known pattern at $2000 in CPU RAM
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            _ram[0x2000 + i] = (byte)(i + 1);

        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 5, addr: 0x2000);

        var data = _vgc.GetTileData();
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            Assert.AreEqual((byte)(i + 1), data[5 * VgcConstants.TileSize8 + i], $"Byte {i}");
    }

    [TestMethod]
    public void TileDef_16x16_CopiesFromRam()
    {
        WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileCfgSize16);

        for (int i = 0; i < VgcConstants.TileSize16; i++)
            _ram[0x3000 + i] = (byte)(i & 0xFF);

        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 0, addr: 0x3000);

        var data = _vgc.GetTileData();
        for (int i = 0; i < VgcConstants.TileSize16; i++)
            Assert.AreEqual((byte)(i & 0xFF), data[i], $"Byte {i}");
    }

    // -- TDEFBULK (define multiple tiles) ------------------------------------

    [TestMethod]
    public void TileDefBulk_DefinesMultipleTiles()
    {
        int tileSize = VgcConstants.TileSize8;
        for (int t = 0; t < 3; t++)
            for (int i = 0; i < tileSize; i++)
                _ram[0x2000 + t * tileSize + i] = (byte)(t + 10);

        ExecuteTileCmd(VgcConstants.TileCmdDefBulk, p0: 0, p1: 3, addr: 0x2000);

        var data = _vgc.GetTileData();
        for (int t = 0; t < 3; t++)
            Assert.AreEqual((byte)(t + 10), data[t * tileSize], $"Tile {t} first byte");
    }

    // -- TPUT (set single nametable entry) ------------------------------------

    [TestMethod]
    public void TilePut_SetsEntry()
    {
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 5, p2: 3, p3: 42);

        var nt = _vgc.GetNametable(0);
        Assert.AreEqual(42, nt[3 * VgcConstants.NametableCols + 5]);
    }

    [TestMethod]
    public void TilePut_OutOfBounds_NoEffect()
    {
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 50, p2: 3, p3: 42);

        var nt = _vgc.GetNametable(0);
        // No crash, no change
        Assert.AreEqual(0, nt[3 * VgcConstants.NametableCols]);
    }

    // -- TATTR (set single attribute entry) -----------------------------------

    [TestMethod]
    public void TileAttr_SetsEntry()
    {
        byte attr = VgcConstants.TileAttrVFlip | VgcConstants.TileAttrHFlip | 0x03;
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 5, p2: 3, p3: attr);

        var at = _vgc.GetAttrTable(0);
        Assert.AreEqual(attr, at[3 * VgcConstants.NametableCols + 5]);
    }

    // -- TFILL (fill entire nametable) ----------------------------------------

    [TestMethod]
    public void TileFill_FillsNametable()
    {
        ExecuteTileCmd(VgcConstants.TileCmdFill, p0: 0, p1: 7);

        var nt = _vgc.GetNametable(0);
        for (int i = 0; i < VgcConstants.NametableSize; i++)
            Assert.AreEqual(7, nt[i]);
    }

    [TestMethod]
    public void TileFill_ClearsAttributes()
    {
        // Set an attribute first
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 0, p2: 0, p3: 0xFF);
        // Now fill — should clear attrs
        ExecuteTileCmd(VgcConstants.TileCmdFill, p0: 0, p1: 1);

        var at = _vgc.GetAttrTable(0);
        Assert.AreEqual(0, at[0]);
    }

    // -- TROW / TCOL (write row/column) ---------------------------------------

    [TestMethod]
    public void TileRow_WritesFullRow()
    {
        for (int i = 0; i < VgcConstants.NametableCols; i++)
            _ram[0x4000 + i] = (byte)(i + 1);

        ExecuteTileCmd(VgcConstants.TileCmdRow, p0: 0, p1: 5, addr: 0x4000);

        var nt = _vgc.GetNametable(0);
        for (int i = 0; i < VgcConstants.NametableCols; i++)
            Assert.AreEqual((byte)(i + 1), nt[5 * VgcConstants.NametableCols + i]);
    }

    [TestMethod]
    public void TileCol_WritesFullColumn()
    {
        for (int i = 0; i < VgcConstants.NametableRows; i++)
            _ram[0x4000 + i] = (byte)(i + 10);

        ExecuteTileCmd(VgcConstants.TileCmdCol, p0: 0, p1: 3, addr: 0x4000);

        var nt = _vgc.GetNametable(0);
        for (int r = 0; r < VgcConstants.NametableRows; r++)
            Assert.AreEqual((byte)(r + 10), nt[r * VgcConstants.NametableCols + 3]);
    }

    // -- TLOAD / TALOAD (load full nametable / attr table) --------------------

    [TestMethod]
    public void TileLoadNametable_Loads1000Bytes()
    {
        for (int i = 0; i < VgcConstants.NametableSize; i++)
            _ram[0x2000 + i] = (byte)(i & 0xFF);

        ExecuteTileCmd(VgcConstants.TileCmdLoad, p0: 2, addr: 0x2000);

        var nt = _vgc.GetNametable(2);
        for (int i = 0; i < VgcConstants.NametableSize; i++)
            Assert.AreEqual((byte)(i & 0xFF), nt[i]);
    }

    [TestMethod]
    public void TileLoadAttrTable_Loads1000Bytes()
    {
        for (int i = 0; i < VgcConstants.NametableSize; i++)
            _ram[0x5000 + i] = (byte)((i * 3) & 0xFF);

        ExecuteTileCmd(VgcConstants.TileCmdALoad, p0: 1, addr: 0x5000);

        var at = _vgc.GetAttrTable(1);
        for (int i = 0; i < VgcConstants.NametableSize; i++)
            Assert.AreEqual((byte)((i * 3) & 0xFF), at[i]);
    }

    // -- TPAL / TPALC (palette operations) ------------------------------------

    [TestMethod]
    public void TilePal_LoadsSubPalette()
    {
        // Write 48 bytes of RGB data at $6000
        for (int i = 0; i < 48; i++)
            _ram[0x6000 + i] = (byte)(i + 100);

        ExecuteTileCmd(VgcConstants.TileCmdPal, addr: 0x6000, palP0: 3);

        var pal = _vgc.GetTilePaletteRam();
        int offset = 3 * VgcConstants.TilePaletteColors * 3;
        for (int i = 0; i < 48; i++)
            Assert.AreEqual((byte)(i + 100), pal[offset + i]);
    }

    [TestMethod]
    public void TilePalC_SetsSingleColor()
    {
        ExecuteTileCmd(VgcConstants.TileCmdPalC, p0: 255, p1: 128, p2: 64,
            palP0: 2, palP1: 5);

        var pal = _vgc.GetTilePaletteRam();
        int offset = (2 * VgcConstants.TilePaletteColors + 5) * 3;
        Assert.AreEqual(255, pal[offset]);
        Assert.AreEqual(128, pal[offset + 1]);
        Assert.AreEqual(64, pal[offset + 2]);
    }

    // -- TPEEK (peek tile + attribute) ----------------------------------------

    [TestMethod]
    public void TilePeek_ReturnsCorrectValues()
    {
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 10, p2: 7, p3: 99);
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 10, p2: 7, p3: 0xC3);

        ExecuteTileCmd(VgcConstants.TileCmdPeek, p0: 0, p1: 10, p2: 7);

        Assert.AreEqual(99, _vgc.Read(VgcConstants.TilePeekVal));
        Assert.AreEqual(0xC3, _vgc.Read(VgcConstants.TilePeekAttr));
    }

    // -- TROWATTR / TCOLATTR (write attribute row/column) ---------------------

    [TestMethod]
    public void TileRowAttr_WritesFullRow()
    {
        for (int i = 0; i < VgcConstants.NametableCols; i++)
            _ram[0x4000 + i] = (byte)(i | 0x80);

        ExecuteTileCmd(VgcConstants.TileCmdRowAttr, p0: 0, p1: 2, addr: 0x4000);

        var at = _vgc.GetAttrTable(0);
        for (int i = 0; i < VgcConstants.NametableCols; i++)
            Assert.AreEqual((byte)(i | 0x80), at[2 * VgcConstants.NametableCols + i]);
    }

    [TestMethod]
    public void TileColAttr_WritesFullColumn()
    {
        for (int i = 0; i < VgcConstants.NametableRows; i++)
            _ram[0x4000 + i] = (byte)(i | 0x40);

        ExecuteTileCmd(VgcConstants.TileCmdColAttr, p0: 0, p1: 7, addr: 0x4000);

        var at = _vgc.GetAttrTable(0);
        for (int r = 0; r < VgcConstants.NametableRows; r++)
            Assert.AreEqual((byte)(r | 0x40), at[r * VgcConstants.NametableCols + 7]);
    }

    // -- TCLS (clear all) -----------------------------------------------------

    [TestMethod]
    public void TileCls_ClearsAllNametablesAndAttrs()
    {
        // Put data in multiple nametables
        ExecuteTileCmd(VgcConstants.TileCmdFill, p0: 0, p1: 5);
        ExecuteTileCmd(VgcConstants.TileCmdFill, p0: 1, p1: 10);
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 2, p1: 0, p2: 0, p3: 0xFF);

        ExecuteTileCmd(VgcConstants.TileCmdCls);

        for (int nt = 0; nt < VgcConstants.NametableCount; nt++)
        {
            var data = _vgc.GetNametable(nt);
            var attr = _vgc.GetAttrTable(nt);
            for (int i = 0; i < VgcConstants.NametableSize; i++)
            {
                Assert.AreEqual(0, data[i], $"NT{nt}[{i}]");
                Assert.AreEqual(0, attr[i], $"Attr{nt}[{i}]");
            }
        }
    }

    // -- Mirroring ------------------------------------------------------------

    [TestMethod]
    public void Mirror_Horizontal_NT1WriteGoesToNT0()
    {
        // Horizontal mirror: NT0=NT1, NT2=NT3
        WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileMirrorH << VgcConstants.TileCfgMirrorShift);

        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 1, p1: 5, p2: 3, p3: 77);

        // Should be in NT0 (since NT1 mirrors to NT0)
        var nt0 = _vgc.GetNametable(0);
        Assert.AreEqual(77, nt0[3 * VgcConstants.NametableCols + 5]);
    }

    [TestMethod]
    public void Mirror_Vertical_NT2WriteGoesToNT0()
    {
        // Vertical mirror: NT0=NT2, NT1=NT3
        WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileMirrorV << VgcConstants.TileCfgMirrorShift);

        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 2, p1: 1, p2: 1, p3: 55);

        var nt0 = _vgc.GetNametable(0);
        Assert.AreEqual(55, nt0[1 * VgcConstants.NametableCols + 1]);
    }

    [TestMethod]
    public void Mirror_Single_AllWriteGoToNT0()
    {
        WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileMirrorSingle << VgcConstants.TileCfgMirrorShift);

        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 3, p1: 0, p2: 0, p3: 33);

        var nt0 = _vgc.GetNametable(0);
        Assert.AreEqual(33, nt0[0]);
    }

    [TestMethod]
    public void Mirror_FourScreen_AllIndependent()
    {
        WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileMirrorFour << VgcConstants.TileCfgMirrorShift);

        for (byte nt = 0; nt < 4; nt++)
            ExecuteTileCmd(VgcConstants.TileCmdPut, p0: nt, p1: 0, p2: 0, p3: (byte)(nt + 10));

        for (int nt = 0; nt < 4; nt++)
        {
            var data = _vgc.GetNametable(nt);
            Assert.AreEqual((byte)(nt + 10), data[0], $"NT{nt}");
        }
    }

    // -- Collision registers (read-clears) ------------------------------------

    [TestMethod]
    public void TileCollision_ReadClears()
    {
        _vgc.SetTileCollision(0x0305);

        byte lo = _vgc.Read(VgcConstants.TileColL);
        Assert.AreEqual(0x05, lo);
        // After reading low, both should be cleared
        byte hi = _vgc.Read(VgcConstants.TileColH);
        Assert.AreEqual(0, hi);

        // Second read should be 0
        Assert.AreEqual(0, _vgc.Read(VgcConstants.TileColL));
    }

    [TestMethod]
    public void TileCollision_AccumulatesOrMask()
    {
        _vgc.SetTileCollision(0x0001);
        _vgc.SetTileCollision(0x0004);

        byte lo = _vgc.Read(VgcConstants.TileColL);
        Assert.AreEqual(0x05, lo);
    }

    // -- Memory space integration ---------------------------------------------

    [TestMethod]
    public void MemSpaceTile_ReadWriteRoundTrips()
    {
        Assert.IsTrue(_vgc.TryWriteMemorySpace(VgcConstants.MemSpaceTile, 100, 0xAB));
        Assert.IsTrue(_vgc.TryReadMemorySpace(VgcConstants.MemSpaceTile, 100, out byte val));
        Assert.AreEqual(0xAB, val);
    }

    [TestMethod]
    public void MemSpaceTile_Length_Is32KB()
    {
        Assert.AreEqual(VgcConstants.TileRamSize16, _vgc.GetMemorySpaceLength(VgcConstants.MemSpaceTile));
    }

    // -- Address ownership ----------------------------------------------------

    [TestMethod]
    public void OwnsAddress_TileRegisters()
    {
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.TileRegBase));
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.TileRegEnd));
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.TileCmd));
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.TileColL));
    }

    // -- Reset clears tile state ----------------------------------------------

    [TestMethod]
    public void Reset_ClearsTileState()
    {
        // Set some tile state
        WriteTileReg(VgcConstants.TileConfig, 0x07);
        WriteTileReg(VgcConstants.TileTransColor, 5);
        WriteTileReg(VgcConstants.TileScrollXL, 100);
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 42);

        // Reset
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSysReset);

        Assert.AreEqual(0, _vgc.GetTileConfig());
        Assert.AreEqual(0, _vgc.GetTileTransColor());
        Assert.AreEqual(0, _vgc.GetTileScrollX());
        var nt = _vgc.GetNametable(0);
        Assert.AreEqual(0, nt[0]);
    }

    // -- Unknown command returns error status ---------------------------------

    [TestMethod]
    public void TileCmd_Unknown_SetsErrorStatus()
    {
        WriteTileReg(VgcConstants.TileCmd, 0xFF);
        Assert.AreNotEqual(0, _vgc.Read(VgcConstants.TileStatus));
    }

    // =========================================================================
    // Renderer tests
    // =========================================================================

    [TestMethod]
    public void TileRenderState_FromVgc_SnapshotsCorrectly()
    {
        WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileCfgSize16 | (VgcConstants.TileMirrorFour << VgcConstants.TileCfgMirrorShift));
        WriteTileReg(VgcConstants.TileTransColor, 3);
        WriteTileReg(VgcConstants.TileScrollXL, 0x20);
        WriteTileReg(VgcConstants.TileScrollXH, 0x01);

        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 42);

        var state = TileRenderState.FromVgc(_vgc);

        Assert.IsTrue(state.IsSize16);
        Assert.AreEqual(VgcConstants.TileMirrorFour, state.MirrorMode);
        Assert.AreEqual(3, state.TransColor);
        Assert.AreEqual(0x0120, state.ScrollX);
        Assert.AreEqual(16, state.TileWidth);
        Assert.AreEqual(42, state.Nametables[0][0]);
    }

    [TestMethod]
    public void TileRenderer_RasterizeScanline_BasicTile()
    {
        // Define tile 1 with all pixels set to color 5 (8x8 mode)
        // Packed nibbles: each byte = 0x55 (color 5 in both nibbles)
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            _ram[0x2000 + i] = 0x55;
        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 1, addr: 0x2000);

        // Place tile 1 at NT0 position (0,0)
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 1);

        var state = TileRenderState.FromVgc(_vgc);
        var tileLine = new uint[VgcConstants.GfxWidth];
        var tileOpaque = new byte[VgcConstants.GfxWidth];

        TileRenderer.RasterizeScanline(0, state, tileLine, tileOpaque);

        // First 8 pixels should be opaque with color from sub-palette 0, index 5
        for (int x = 0; x < 8; x++)
        {
            Assert.AreNotEqual(0u, tileLine[x], $"Pixel {x} should be drawn");
            Assert.AreEqual(1, tileOpaque[x], $"Pixel {x} should have behind priority");
        }
        // Pixel 8 should be transparent (tile 0 is all zeros = trans color)
        Assert.AreEqual(0u, tileLine[8]);
    }

    [TestMethod]
    public void TileRenderer_TransparentColor_SkipsPixels()
    {
        // Define tile 1 with all pixels = color 3
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            _ram[0x2000 + i] = 0x33;
        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 1, addr: 0x2000);
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 1);

        // Set transparent color to 3 — all pixels should be skipped
        WriteTileReg(VgcConstants.TileTransColor, 3);

        var state = TileRenderState.FromVgc(_vgc);
        var tileLine = new uint[VgcConstants.GfxWidth];
        var tileOpaque = new byte[VgcConstants.GfxWidth];

        TileRenderer.RasterizeScanline(0, state, tileLine, tileOpaque);

        for (int x = 0; x < 8; x++)
            Assert.AreEqual(0u, tileLine[x], $"Pixel {x} should be transparent");
    }

    [TestMethod]
    public void TileRenderer_Scroll_ShiftsOutput()
    {
        // Define tile 1 as solid color 7
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            _ram[0x2000 + i] = 0x77;
        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 1, addr: 0x2000);
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 1);

        // Scroll right by 4 pixels — tile should appear starting 4 pixels earlier
        WriteTileReg(VgcConstants.TileScrollXL, 4);

        var state = TileRenderState.FromVgc(_vgc);
        var tileLine = new uint[VgcConstants.GfxWidth];
        var tileOpaque = new byte[VgcConstants.GfxWidth];

        TileRenderer.RasterizeScanline(0, state, tileLine, tileOpaque);

        // With scroll=4, screen pixel 0 maps to world pixel 4, which is still in tile 1
        // Screen pixels 0-3 should show tile 1 (world pixels 4-7)
        for (int x = 0; x < 4; x++)
            Assert.AreNotEqual(0u, tileLine[x], $"Pixel {x} should show tile 1");
        // Screen pixel 4 maps to world pixel 8, which is tile 0 (empty)
        Assert.AreEqual(0u, tileLine[4]);
    }

    [TestMethod]
    public void TileRenderer_HFlip_ReversesPixels()
    {
        // Define tile 1: first 4 pixels = color 1, last 4 = color 2
        // Row: [1,1,1,1,2,2,2,2] → bytes: 0x11, 0x11, 0x22, 0x22
        for (int row = 0; row < 8; row++)
        {
            _ram[0x2000 + row * 4 + 0] = 0x11;
            _ram[0x2000 + row * 4 + 1] = 0x11;
            _ram[0x2000 + row * 4 + 2] = 0x22;
            _ram[0x2000 + row * 4 + 3] = 0x22;
        }
        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 1, addr: 0x2000);
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 1);
        // Set H-flip
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 0, p2: 0, p3: VgcConstants.TileAttrHFlip);

        var state = TileRenderState.FromVgc(_vgc);
        var tileLine = new uint[VgcConstants.GfxWidth];
        var tileOpaque = new byte[VgcConstants.GfxWidth];

        TileRenderer.RasterizeScanline(0, state, tileLine, tileOpaque);

        // After H-flip: first 4 pixels should be color 2, last 4 should be color 1
        // Color 2 in sub-palette 0
        uint color2 = state.Palette[2];
        uint color1 = state.Palette[1];
        for (int x = 0; x < 4; x++)
            Assert.AreEqual(color2, tileLine[x], $"Pixel {x} should be color 2 (flipped)");
        for (int x = 4; x < 8; x++)
            Assert.AreEqual(color1, tileLine[x], $"Pixel {x} should be color 1 (flipped)");
    }

    [TestMethod]
    public void TileRenderer_SubPalette_UsesCorrectColors()
    {
        // Define tile 1 as solid color index 1
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            _ram[0x2000 + i] = 0x11;
        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 1, addr: 0x2000);
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 1);

        // Set sub-palette 2 with color 1 = pure red
        ExecuteTileCmd(VgcConstants.TileCmdPalC, p0: 255, p1: 0, p2: 0, palP0: 2, palP1: 1);

        // Set attribute to use sub-palette 2
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 0, p2: 0, p3: 2);

        var state = TileRenderState.FromVgc(_vgc);
        var tileLine = new uint[VgcConstants.GfxWidth];
        var tileOpaque = new byte[VgcConstants.GfxWidth];

        TileRenderer.RasterizeScanline(0, state, tileLine, tileOpaque);

        // Sub-palette 2, color 1 = (255,0,0) → BGRA = 0xFF FF0000
        uint expectedBgra = 0xFF000000u | (255u << 16);
        Assert.AreEqual(expectedBgra, tileLine[0]);
    }

    [TestMethod]
    public void TileRenderer_Priority_SetsOpaqueFlag()
    {
        // Tile with priority bit set in attribute
        for (int i = 0; i < VgcConstants.TileSize8; i++)
            _ram[0x2000 + i] = 0x11;
        ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 1, addr: 0x2000);
        ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 0, p2: 0, p3: 1);
        ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 0, p2: 0, p3: VgcConstants.TileAttrPriority);

        var state = TileRenderState.FromVgc(_vgc);
        var tileLine = new uint[VgcConstants.GfxWidth];
        var tileOpaque = new byte[VgcConstants.GfxWidth];

        TileRenderer.RasterizeScanline(0, state, tileLine, tileOpaque);

        Assert.AreEqual(2, tileOpaque[0], "Priority bit should produce opaque=2 (front)");
    }

    // -- File I/O (TSAVE / TLOAD) ---------------------------------------------

    [TestMethod]
    public void TSave_TLoad_RoundTrips_AllTileState()
    {
        var tempDir = Path.Combine(Path.GetTempPath(), "tile_fio_" + Guid.NewGuid().ToString("N")[..8]);
        Directory.CreateDirectory(tempDir);
        try
        {
            // Set up tile state: config (8×8, mirror=vertical), trans color, palette, tiles, NTs, attrs
            WriteTileReg(VgcConstants.TileConfig, (1 << VgcConstants.TileCfgMirrorShift)); // mirror=V, size=8x8
            WriteTileReg(VgcConstants.TileTransColor, 3);

            // Define tile 0 with pattern
            for (int i = 0; i < VgcConstants.TileSize8; i++)
                _ram[0x4000 + i] = (byte)(i * 3);
            ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 0, addr: 0x4000);

            // Put tile 0 at NT0 (5,3)
            ExecuteTileCmd(VgcConstants.TileCmdPut, p0: 0, p1: 5, p2: 3, p3: 42);
            ExecuteTileCmd(VgcConstants.TileCmdAttr, p0: 0, p1: 5, p2: 3, p3: 0xC2); // flip H+V, palette 2

            // Set a palette color: sub-palette 2, color 1 = (0xAA, 0xBB, 0xCC)
            ExecuteTileCmd(VgcConstants.TileCmdPalC, palP0: 2, palP1: 1, p0: 0xAA, p1: 0xBB, p2: 0xCC);

            // Set scroll
            WriteTileReg(VgcConstants.TileScrollXL, 0x34);
            WriteTileReg(VgcConstants.TileScrollXH, 0x01);
            WriteTileReg(VgcConstants.TileScrollYL, 0x56);
            WriteTileReg(VgcConstants.TileScrollYH, 0x00);

            // Create FIO with VGC reference and save
            var fio = new FileIoController(
                address => _ram[address],
                (address, data) => _ram[address] = data,
                tempDir,
                vgc: _vgc);

            WriteFilename(fio, "testworld");
            fio.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTSave);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read(VgcConstants.FioStatus), "TSAVE should succeed");

            // Verify file exists
            Assert.IsTrue(File.Exists(Path.Combine(tempDir, "testworld.tile")));

            // Create a fresh VGC and load into it
            var vgc2 = new VirtualGraphicsController();
            var ram2 = new byte[65536];
            vgc2.SetBusMemory(ram2);

            var fio2 = new FileIoController(
                address => ram2[address],
                (address, data) => ram2[address] = data,
                tempDir,
                vgc: vgc2);

            WriteFilename(fio2, "testworld");
            fio2.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTLoad);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio2.Read(VgcConstants.FioStatus), "TLOAD should succeed");

            // Verify mode set to 4
            Assert.AreEqual(4, vgc2.Read(VgcConstants.RegMode), "TLOAD should set mode to 4");

            // Verify config round-tripped
            Assert.AreEqual(1, vgc2.GetTileMirrorMode(), "Mirror mode should be vertical (1)");
            Assert.IsFalse(vgc2.IsTileSize16, "Tile size should be 8x8");
            Assert.AreEqual(3, vgc2.GetTileTransColor(), "Trans color should be 3");

            // Verify tile definition data
            var origTile = _vgc.GetTileData()[..VgcConstants.TileSize8].ToArray();
            var loadedTile = vgc2.GetTileData()[..VgcConstants.TileSize8].ToArray();
            CollectionAssert.AreEqual(origTile, loadedTile, "Tile 0 definition should match");

            // Verify nametable entry
            var nt0 = vgc2.GetNametable(0);
            Assert.AreEqual(42, nt0[3 * VgcConstants.NametableCols + 5], "NT0 entry at (5,3) should be tile 42");

            // Verify attribute entry
            var at0 = vgc2.GetAttrTable(0);
            Assert.AreEqual(0xC2, at0[3 * VgcConstants.NametableCols + 5], "Attr at (5,3) should be 0xC2");

            // Verify palette color
            var palRam = vgc2.GetTilePaletteRam();
            int palOffset = 2 * 16 * 3 + 1 * 3; // sub-palette 2, color 1
            Assert.AreEqual(0xAA, palRam[palOffset], "Palette R");
            Assert.AreEqual(0xBB, palRam[palOffset + 1], "Palette G");
            Assert.AreEqual(0xCC, palRam[palOffset + 2], "Palette B");
        }
        finally
        {
            try { Directory.Delete(tempDir, true); } catch { }
        }
    }

    [TestMethod]
    public void TLoad_FileNotFound_ReturnsError()
    {
        var tempDir = Path.Combine(Path.GetTempPath(), "tile_fio_" + Guid.NewGuid().ToString("N")[..8]);
        Directory.CreateDirectory(tempDir);
        try
        {
            var fio = new FileIoController(
                address => _ram[address],
                (address, data) => _ram[address] = data,
                tempDir,
                vgc: _vgc);

            WriteFilename(fio, "nonexistent");
            fio.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTLoad);
            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read(VgcConstants.FioStatus));
            Assert.AreEqual(VgcConstants.FioErrNotFound, fio.Read(VgcConstants.FioErrCode));
        }
        finally
        {
            try { Directory.Delete(tempDir, true); } catch { }
        }
    }

    [TestMethod]
    public void TSave_TLoad_16x16_RoundTrips()
    {
        var tempDir = Path.Combine(Path.GetTempPath(), "tile_fio_" + Guid.NewGuid().ToString("N")[..8]);
        Directory.CreateDirectory(tempDir);
        try
        {
            // Set 16×16 mode
            WriteTileReg(VgcConstants.TileConfig, VgcConstants.TileCfgSize16);

            // Define tile 5 with recognizable pattern
            for (int i = 0; i < VgcConstants.TileSize16; i++)
                _ram[0x3000 + i] = (byte)(i ^ 0xAA);
            ExecuteTileCmd(VgcConstants.TileCmdDef, p0: 5, addr: 0x3000);

            var fio = new FileIoController(
                address => _ram[address],
                (address, data) => _ram[address] = data,
                tempDir,
                vgc: _vgc);

            WriteFilename(fio, "big");
            fio.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTSave);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read(VgcConstants.FioStatus));

            // Load into fresh VGC
            var vgc2 = new VirtualGraphicsController();
            var ram2 = new byte[65536];
            vgc2.SetBusMemory(ram2);

            var fio2 = new FileIoController(
                address => ram2[address],
                (address, data) => ram2[address] = data,
                tempDir,
                vgc: vgc2);

            WriteFilename(fio2, "big");
            fio2.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTLoad);
            Assert.AreEqual(VgcConstants.FioStatusOk, fio2.Read(VgcConstants.FioStatus));

            Assert.IsTrue(vgc2.IsTileSize16, "Should be 16x16 mode after load");

            // Verify tile 5 data
            int start = 5 * VgcConstants.TileSize16;
            var orig = _vgc.GetTileData().Slice(start, VgcConstants.TileSize16).ToArray();
            var loaded = vgc2.GetTileData().Slice(start, VgcConstants.TileSize16).ToArray();
            CollectionAssert.AreEqual(orig, loaded, "Tile 5 definition should match");
        }
        finally
        {
            try { Directory.Delete(tempDir, true); } catch { }
        }
    }

    [TestMethod]
    public void TSave_NoVgc_ReturnsError()
    {
        var tempDir = Path.Combine(Path.GetTempPath(), "tile_fio_" + Guid.NewGuid().ToString("N")[..8]);
        Directory.CreateDirectory(tempDir);
        try
        {
            var fio = new FileIoController(
                address => _ram[address],
                (address, data) => _ram[address] = data,
                tempDir);

            WriteFilename(fio, "test");
            fio.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTSave);
            Assert.AreEqual(VgcConstants.FioStatusError, fio.Read(VgcConstants.FioStatus));
        }
        finally
        {
            try { Directory.Delete(tempDir, true); } catch { }
        }
    }

    private static void WriteFilename(FileIoController fio, string name)
    {
        fio.Write(VgcConstants.FioNameLen, (byte)name.Length);
        for (int i = 0; i < name.Length; i++)
            fio.Write((ushort)(VgcConstants.FioName + i), (byte)name[i]);
    }
}
