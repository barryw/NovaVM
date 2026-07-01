using System;
using System.IO;
using System.Linq;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using SixLabors.ImageSharp;
using SixLabors.ImageSharp.PixelFormats;

namespace e6502UnitTests;

/// <summary>
/// Pins the NovaZ PICS.PAK format produced by the packer's BlorbPictures
/// (software/examples/novaz/tools/Nova.NovaZ.Packer):
///   "NZPK" ver(1) count(2 LE) release(2 LE) palette[16 RGB triples]
///   index[count]: z_number(2) width(2) height(2) flags(1) offset(4) len(4)  (LE)
///   data: 4bpp row-packed bitmaps (two pixels/byte, high nibble = left pixel,
///         rows padded to whole bytes); offset is ABSOLUTE within the pak so
///         the 6502 can hand it straight to a FIO region load.
/// flags bit 0 = picture has transparency; bit 2 = align small framed icon flow;
/// bit 3 = align larger framed flow pictures to a text-cell top; bits 4-7 =
/// the transparent palette index (an index unused by the picture's opaque
/// pixels). Blorb 'Rect' placeholder pictures keep their dimensions but have
/// offset=0, len=0.
/// </summary>
[TestClass]
public sealed class NovaZPicturePackTests
{
    private const int FixedHeaderSize = 9;
    private const int HeaderSize = 57;
    private const int IndexEntrySize = 15;

    // --- EGA palette -------------------------------------------------------

    [TestMethod]
    public void EgaPalette_MatchesVgcRenderPalette()
    {
        Assert.AreEqual(16, BlorbPictures.EgaPalette.Length);
        for (int i = 0; i < 16; i++)
        {
            var vgc = ColorPalette.Get(i, VgcConstants.PaletteModeEga);
            var (r, g, b) = BlorbPictures.EgaPalette[i];
            Assert.AreEqual(vgc.R, r, $"R mismatch at index {i}");
            Assert.AreEqual(vgc.G, g, $"G mismatch at index {i}");
            Assert.AreEqual(vgc.B, b, $"B mismatch at index {i}");
        }
    }

    [TestMethod]
    public void QuantizeToEga_MapsKnownColors()
    {
        Assert.AreEqual(0, BlorbPictures.QuantizeToEga(0, 0, 0));        // black
        Assert.AreEqual(15, BlorbPictures.QuantizeToEga(255, 255, 255)); // white
        Assert.AreEqual(4, BlorbPictures.QuantizeToEga(170, 0, 0));      // EGA red, exact
        Assert.AreEqual(4, BlorbPictures.QuantizeToEga(180, 10, 10));    // near red
        Assert.AreEqual(7, BlorbPictures.QuantizeToEga(200, 200, 200));  // nearer light grey than white
        Assert.AreEqual(14, BlorbPictures.QuantizeToEga(255, 255, 85));  // yellow, exact
    }

    // --- pack layout, byte-exact -------------------------------------------

    [TestMethod]
    public void BuildPack_SinglePicture_HeaderIndexAndNibblesExact()
    {
        // 4x4: row 0 = black black white white, rows 1-3 = blue blue red red.
        using var img = new Image<Rgba32>(4, 4);
        img[0, 0] = new Rgba32(0, 0, 0);       img[1, 0] = new Rgba32(0, 0, 0);
        img[2, 0] = new Rgba32(255, 255, 255); img[3, 0] = new Rgba32(255, 255, 255);
        for (int y = 1; y < 4; y++)
        {
            img[0, y] = new Rgba32(0, 0, 170);  img[1, y] = new Rgba32(0, 0, 170);
            img[2, y] = new Rgba32(170, 0, 0);  img[3, y] = new Rgba32(170, 0, 0);
        }

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((11, PngBytes(img))));

        // Header: "NZPK" ver=2 count=1 release=0, followed by a 16-entry RGB palette.
        CollectionAssert.AreEqual(new byte[] { (byte)'N', (byte)'Z', (byte)'P', (byte)'K', 2, 1, 0, 0, 0 },
            pack.Take(FixedHeaderSize).ToArray());
        CollectionAssert.AreEqual(new byte[] { 0, 0, 0 }, pack.Skip(FixedHeaderSize).Take(3).ToArray());
        CollectionAssert.AreEqual(new byte[] { 0, 0, 170 }, pack.Skip(FixedHeaderSize + 3).Take(3).ToArray());
        CollectionAssert.AreEqual(new byte[] { 170, 0, 0 }, pack.Skip(FixedHeaderSize + 4 * 3).Take(3).ToArray());
        CollectionAssert.AreEqual(new byte[] { 255, 255, 255 }, pack.Skip(FixedHeaderSize + 15 * 3).Take(3).ToArray());

        // Index entry: z=11, w=4, h=4, flags=0, offset=HeaderSize+IndexEntrySize, len=8
        int dataStart = HeaderSize + IndexEntrySize;
        byte[] expectedEntry =
        [
            11, 0,        // z_number
            4, 0,         // width
            4, 0,         // height
            0,            // flags: opaque
            (byte)dataStart, 0, 0, 0, // offset (absolute)
            8, 0, 0, 0,   // len = 2 bytes/row * 4 rows
        ];
        CollectionAssert.AreEqual(expectedEntry, pack.Skip(HeaderSize).Take(IndexEntrySize).ToArray());

        // Bitmap: high nibble = left pixel. black=0 white=15 blue=1 red=4.
        byte[] expectedBitmap = [0x00, 0xFF, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44];
        Assert.AreEqual(dataStart + 8, pack.Length);
        CollectionAssert.AreEqual(expectedBitmap, pack.Skip(dataStart).ToArray());
    }

    [TestMethod]
    public void BuildPack_OddWidth_PadsRowsToWholeBytes()
    {
        // 3x2 all white: rows pack as F F F 0 -> 0xFF 0xF0 per row.
        using var img = new Image<Rgba32>(3, 2);
        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 3; x++)
                img[x, y] = new Rgba32(255, 255, 255);

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((1, PngBytes(img))));

        int len = BitConverter.ToInt32(pack, HeaderSize + 11);
        Assert.AreEqual(4, len, "2 bytes per padded 3px row, 2 rows");
        int dataStart = HeaderSize + IndexEntrySize;
        CollectionAssert.AreEqual(new byte[] { 0xFF, 0xF0, 0xFF, 0xF0 },
            pack.Skip(dataStart).ToArray());
    }

    [TestMethod]
    public void BuildPack_TwoPictures_IndexSortedByNumberWithAbsoluteOffsets()
    {
        using var imgA = new Image<Rgba32>(2, 1);
        imgA[0, 0] = new Rgba32(0, 0, 0); imgA[1, 0] = new Rgba32(255, 255, 255);
        using var imgB = new Image<Rgba32>(2, 2);
        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 2; x++)
                imgB[x, y] = new Rgba32(0, 0, 170);

        // Deliberately out of order in the blorb: 7 before 3.
        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((7, PngBytes(imgA)), (3, PngBytes(imgB))));

        Assert.AreEqual(2, BitConverter.ToUInt16(pack, 5));
        int e0 = HeaderSize, e1 = HeaderSize + IndexEntrySize;
        Assert.AreEqual(3, BitConverter.ToUInt16(pack, e0));
        Assert.AreEqual(7, BitConverter.ToUInt16(pack, e1));

        int dataStart = HeaderSize + 2 * IndexEntrySize;
        int off3 = BitConverter.ToInt32(pack, e0 + 7);
        int len3 = BitConverter.ToInt32(pack, e0 + 11);
        int off7 = BitConverter.ToInt32(pack, e1 + 7);
        int len7 = BitConverter.ToInt32(pack, e1 + 11);
        Assert.AreEqual(dataStart, off3);
        Assert.AreEqual(2, len3);          // 1 byte/row * 2 rows
        Assert.AreEqual(dataStart + 2, off7);
        Assert.AreEqual(1, len7);          // 1 byte/row * 1 row

        Assert.AreEqual(0x11, pack[off3]); // blue blue
        Assert.AreEqual(0x0F, pack[off7]); // black white
    }

    // --- transparency -------------------------------------------------------

    [TestMethod]
    public void BuildPack_TransparentPixels_FlaggedAndEncodedAsFreeIndex()
    {
        // 2x1: opaque black + fully transparent. Black uses index 0, so the
        // lowest EGA index unused by opaque pixels is 1 (blue).
        using var img = new Image<Rgba32>(2, 1);
        img[0, 0] = new Rgba32(0, 0, 0, 255);
        img[1, 0] = new Rgba32(0, 0, 0, 0);

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((5, PngBytes(img))));

        byte flags = pack[HeaderSize + 6];
        Assert.AreEqual(0x01, flags & 0x01, "transparency flag must be set");
        Assert.AreEqual(0, flags & 0x04, "inline alignment flag requires a small transparent framed icon");
        Assert.AreEqual(0, flags & 0x08, "cell-top flow alignment requires a larger transparent framed picture");
        int transparentIndex = flags >> 4;
        Assert.AreEqual(1, transparentIndex, "lowest EGA index unused by opaque pixels");

        int dataStart = HeaderSize + IndexEntrySize;
        Assert.AreEqual(0x01, pack[dataStart], "black(0) left, transparent(1) right");
    }

    [TestMethod]
    public void BuildPack_SmallOpaqueFrameWithTransparentInterior_SetsInlineAlignmentFlag()
    {
        // Z6 inline room icons in Zork Zero are small framed pictures with a
        // transparent interior; the runtime uses this bit to align the frame to
        // text flow without scanning bitmap data.
        using var img = new Image<Rgba32>(4, 4);
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                bool frame = x == 0 || x == 3 || y == 0 || y == 3;
                img[x, y] = frame
                    ? new Rgba32(0, 0, 0, 255)
                    : new Rgba32(0, 0, 0, 0);
            }
        }

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((6, PngBytes(img))));

        byte flags = pack[HeaderSize + 6];
        Assert.AreEqual(0x01, flags & 0x01, "transparency flag must be set");
        Assert.AreEqual(0x04, flags & 0x04, "small transparent framed icons should request flow alignment");
        Assert.AreEqual(0, flags & 0x08, "small framed icons use their own padded flow alignment");
    }

    [TestMethod]
    public void BuildPack_LargerOpaqueFrameWithTransparentInterior_SetsCellTopFlowFlag()
    {
        // Zork Zero's drop-cap pictures are framed transparent flow images.
        // They must snap to the top of a 4px text cell when drawn so the
        // following room heading lands below the bitmap, matching Frotz.
        using var img = new Image<Rgba32>(42, 35);
        for (int y = 0; y < img.Height; y++)
        {
            for (int x = 0; x < img.Width; x++)
            {
                bool frame = x == 0 || x == img.Width - 1 || y == 0 || y == img.Height - 1;
                img[x, y] = frame
                    ? new Rgba32(0, 0, 0, 255)
                    : new Rgba32(0, 0, 0, 0);
            }
        }

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((8, PngBytes(img))));

        byte flags = pack[HeaderSize + 6];
        Assert.AreEqual(0x01, flags & 0x01, "transparency flag must be set");
        Assert.AreEqual(0, flags & 0x04, "larger framed flow pictures are not small room icons");
        Assert.AreEqual(0x08, flags & 0x08, "larger framed flow pictures should request text-cell top alignment");
    }

    [TestMethod]
    public void BuildPack_OpaquePicture_NoTransparencyFlag()
    {
        using var img = new Image<Rgba32>(1, 1);
        img[0, 0] = new Rgba32(170, 0, 0, 255);

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb((9, PngBytes(img))));

        Assert.AreEqual(0, pack[HeaderSize + 6]);
    }

    // --- Rect placeholder pictures ------------------------------------------

    [TestMethod]
    public void BuildPack_RectResource_DimsKeptWithNoData()
    {
        byte[] pack = BlorbPictures.BuildPack(MakeBlorbWithRect(z: 100, width: 138, height: 0));

        Assert.AreEqual(1, BitConverter.ToUInt16(pack, 5));
        Assert.AreEqual(100, BitConverter.ToUInt16(pack, HeaderSize));
        Assert.AreEqual(138, BitConverter.ToUInt16(pack, HeaderSize + 2));
        Assert.AreEqual(0, BitConverter.ToUInt16(pack, HeaderSize + 4));
        Assert.AreEqual(0, pack[HeaderSize + 6]);                       // flags
        Assert.AreEqual(0, BitConverter.ToInt32(pack, HeaderSize + 7)); // offset
        Assert.AreEqual(0, BitConverter.ToInt32(pack, HeaderSize + 11)); // len
        Assert.AreEqual(HeaderSize + IndexEntrySize, pack.Length, "no bitmap data");
    }

    // --- release number ------------------------------------------------------

    [TestMethod]
    public void BuildPack_RelNChunk_PopulatesRelease()
    {
        using var img = new Image<Rgba32>(1, 1);
        img[0, 0] = new Rgba32(0, 0, 0);

        byte[] pack = BlorbPictures.BuildPack(MakeBlorb(releaseNumber: 393, (1, PngBytes(img))));

        Assert.AreEqual(393, BitConverter.ToUInt16(pack, 7));
    }

    // --- Zork Zero's real blorb (skipped when the local copy is absent) ------

    [TestMethod]
    public void BuildPack_ZorkZeroBlorb_PacksAllPictures()
    {
        string? path = FindZorkZeroBlorb();
        if (path is null)
        {
            Assert.Inconclusive("projects/zork-zero/PICS.BLB not present");
            return;
        }

        byte[] pack = BlorbPictures.BuildPack(File.ReadAllBytes(path));

        int count = BitConverter.ToUInt16(pack, 5);
        Assert.AreEqual(503, count, "396 PNGs + 107 Rect placeholders");

        // Index must be sorted ascending and every PNG entry's bitmap in bounds.
        int prev = -1;
        bool sawDropCap = false;
        bool sawRoomIcon = false;
        for (int i = 0; i < count; i++)
        {
            int e = HeaderSize + i * IndexEntrySize;
            int z = BitConverter.ToUInt16(pack, e);
            Assert.IsTrue(z > prev, "index sorted by z_number");
            prev = z;
            int w = BitConverter.ToUInt16(pack, e + 2);
            int h = BitConverter.ToUInt16(pack, e + 4);
            byte flags = pack[e + 6];
            int off = BitConverter.ToInt32(pack, e + 7);
            int len = BitConverter.ToInt32(pack, e + 11);
            Assert.IsTrue(w <= 320 && h <= 200, $"pic {z} dims {w}x{h}");
            if (z == 0x0002)
            {
                sawDropCap = true;
                Assert.AreEqual(0x08, flags & 0x08, "Zork Zero drop caps should snap to the text-cell top");
            }
            if (z == 0x00D8)
            {
                sawRoomIcon = true;
                Assert.AreEqual(0x04, flags & 0x04, "Zork Zero room icons should use padded framed-icon flow");
            }
            if (len > 0)
            {
                Assert.AreEqual(((w + 1) / 2) * h, len, $"pic {z} bitmap length");
                Assert.IsTrue(off >= HeaderSize + count * IndexEntrySize && off + len <= pack.Length,
                    $"pic {z} data in bounds");
            }
        }

        Assert.IsTrue(sawDropCap, "expected Zork Zero drop-cap picture $0002");
        Assert.IsTrue(sawRoomIcon, "expected Zork Zero room-icon picture $00D8");
    }

    // --- helpers --------------------------------------------------------------

    private static string? FindZorkZeroBlorb()
    {
        // Walk up from the test bin dir to the repo root.
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir is not null)
        {
            string candidate = Path.Combine(dir.FullName,
                "examples", "novaz", "projects", "zork-zero", "PICS.BLB");
            if (File.Exists(candidate))
                return candidate;
            dir = dir.Parent;
        }
        return null;
    }

    private static byte[] PngBytes(Image<Rgba32> img)
    {
        using var ms = new MemoryStream();
        img.SaveAsPng(ms);
        return ms.ToArray();
    }

    private static byte[] MakeBlorb(params (int Number, byte[] Png)[] picts) =>
        MakeBlorb(releaseNumber: null, picts);

    /// <summary>Builds a minimal FORM/IFRS blorb with Pict PNG resources.</summary>
    private static byte[] MakeBlorb(int? releaseNumber, params (int Number, byte[] Png)[] picts)
    {
        using var ms = new MemoryStream();
        using var w = new BinaryWriter(ms);

        w.Write("FORM"u8); w.Write(0); // size patched below
        w.Write("IFRS"u8);

        // RIdx
        int ridxSize = 4 + picts.Length * 12;
        w.Write("RIdx"u8); WriteBE32(w, ridxSize);
        WriteBE32(w, picts.Length);
        // Chunk positions: RIdx ends at 12 + 8 + ridxSize (+pad).
        int pos = 12 + 8 + ridxSize + (ridxSize & 1);
        if (releaseNumber is not null)
            pos += 8 + 2; // RelN chunk before the picture chunks
        var starts = new int[picts.Length];
        for (int i = 0; i < picts.Length; i++)
        {
            starts[i] = pos;
            pos += 8 + picts[i].Png.Length + (picts[i].Png.Length & 1);
        }
        for (int i = 0; i < picts.Length; i++)
        {
            w.Write("Pict"u8);
            WriteBE32(w, picts[i].Number);
            WriteBE32(w, starts[i]);
        }
        if ((ridxSize & 1) != 0) w.Write((byte)0);

        if (releaseNumber is not null)
        {
            w.Write("RelN"u8); WriteBE32(w, 2);
            w.Write((byte)(releaseNumber.Value >> 8));
            w.Write((byte)(releaseNumber.Value & 0xFF));
        }

        foreach (var (_, png) in picts)
        {
            w.Write("PNG "u8); WriteBE32(w, png.Length);
            w.Write(png);
            if ((png.Length & 1) != 0) w.Write((byte)0);
        }

        w.Flush();
        byte[] blorb = ms.ToArray();
        int formSize = blorb.Length - 8;
        blorb[4] = (byte)(formSize >> 24); blorb[5] = (byte)(formSize >> 16);
        blorb[6] = (byte)(formSize >> 8); blorb[7] = (byte)formSize;
        return blorb;
    }

    private static byte[] MakeBlorbWithRect(int z, int width, int height)
    {
        using var ms = new MemoryStream();
        using var w = new BinaryWriter(ms);
        w.Write("FORM"u8); w.Write(0);
        w.Write("IFRS"u8);
        w.Write("RIdx"u8); WriteBE32(w, 16);
        WriteBE32(w, 1);
        w.Write("Pict"u8); WriteBE32(w, z); WriteBE32(w, 12 + 8 + 16);
        w.Write("Rect"u8); WriteBE32(w, 8);
        WriteBE32(w, width); WriteBE32(w, height);
        w.Flush();
        byte[] blorb = ms.ToArray();
        int formSize = blorb.Length - 8;
        blorb[4] = (byte)(formSize >> 24); blorb[5] = (byte)(formSize >> 16);
        blorb[6] = (byte)(formSize >> 8); blorb[7] = (byte)formSize;
        return blorb;
    }

    private static void WriteBE32(BinaryWriter w, int value)
    {
        w.Write((byte)(value >> 24));
        w.Write((byte)(value >> 16));
        w.Write((byte)(value >> 8));
        w.Write((byte)value);
    }
}
