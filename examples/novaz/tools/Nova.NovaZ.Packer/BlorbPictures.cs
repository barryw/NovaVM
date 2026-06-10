using System.Buffers.Binary;
using SixLabors.ImageSharp;
using SixLabors.ImageSharp.PixelFormats;

/// <summary>
/// Decodes the Pict resources of a Blorb file (PNG art + Rect placeholders)
/// and packs them into the NovaZ PICS.PAK format, pre-quantized to the VGC's
/// EGA palette so the 6502 never decodes or color-matches anything.
///
/// PICS.PAK layout (all multi-byte fields little-endian):
///   "NZPK", version(1), count(2), release(2)
///   index[count]: z_number(2) width(2) height(2) flags(1) offset(4) len(4)
///   data: concatenated 4bpp row-packed bitmaps (two pixels per byte, high
///         nibble = left pixel, rows padded to whole bytes), blitter-ready.
///
/// offset is ABSOLUTE within the pak file so the 6502 can pass it straight to
/// a FIO region load. The index is sorted by z_number ascending. width/height
/// are in PIXELS; the V6 segment converts to cells (÷4 rounded up) when
/// answering picture_data.
///
/// Transparency (verified against Zork Zero's blorb, 2026-06-10): 361 of the
/// 396 PNGs carry tRNS — the title-screen overlays and the border picture
/// $D8 redrawn over the playfield every refresh are mostly transparent, so
/// transparent-skip blitting is required, not optional. Encoding: flags
/// bit 0 = picture has transparency; flags bits 4-7 = the transparent EGA
/// index, chosen per picture as the lowest index unused by its opaque
/// pixels (the source PNGs are 4-bit palette, so a transparent picture has
/// at most 15 opaque colors and a free index always exists). Opaque
/// pictures use all 16 colors freely and carry flags 0.
///
/// Blorb 'Rect' resources are invisible placeholder pictures (dimensions
/// only — Zork Zero probes 107 of them for layout spacing): their entries
/// keep width/height with offset=0, len=0, and drawing them is a no-op.
///
/// release comes from the Blorb 'RelN' chunk when present, else 0
/// (Zork Zero's blorb has no RelN).
/// </summary>
public static class BlorbPictures
{
    private const int HeaderSize = 9;
    private const int IndexEntrySize = 15;
    private const byte FlagTransparent = 0x01;

    /// <summary>
    /// The canonical IBM EGA 16-color palette. MUST match the VGC's EGA
    /// rendering table (e6502.Avalonia/Rendering/ColorPalette.cs) — a unit
    /// test pins the two against each other.
    /// </summary>
    public static readonly (byte R, byte G, byte B)[] EgaPalette =
    [
        (0,   0,   0),      // 0  Black
        (0,   0,   170),    // 1  Blue
        (0,   170, 0),      // 2  Green
        (0,   170, 170),    // 3  Cyan
        (170, 0,   0),      // 4  Red
        (170, 0,   170),    // 5  Magenta
        (170, 85,  0),      // 6  Brown
        (170, 170, 170),    // 7  Light Grey
        (85,  85,  85),     // 8  Dark Grey
        (85,  85,  255),    // 9  Bright Blue
        (85,  255, 85),     // 10 Bright Green
        (85,  255, 255),    // 11 Bright Cyan
        (255, 85,  85),     // 12 Bright Red
        (255, 85,  255),    // 13 Bright Magenta
        (255, 255, 85),     // 14 Yellow
        (255, 255, 255),    // 15 White
    ];

    private sealed record Picture(int Number, int Width, int Height, byte Flags, byte[] Bitmap);

    /// <summary>Nearest EGA index by squared RGB distance (ties → lower index).</summary>
    public static byte QuantizeToEga(byte r, byte g, byte b)
    {
        int best = 0;
        int bestDist = int.MaxValue;
        for (int i = 0; i < EgaPalette.Length; i++)
        {
            var (pr, pg, pb) = EgaPalette[i];
            int dr = r - pr, dg = g - pg, db = b - pb;
            int dist = dr * dr + dg * dg + db * db;
            if (dist < bestDist)
            {
                bestDist = dist;
                best = i;
            }
        }
        return (byte)best;
    }

    public static byte[] BuildPack(string blorbPath) => BuildPack(File.ReadAllBytes(blorbPath), blorbPath);

    public static byte[] BuildPack(byte[] blorb, string sourceName = "<memory>")
    {
        var pictures = DecodePictures(blorb, sourceName);
        if (pictures.Count == 0)
            throw new InvalidDataException($"No Pict resources found in Blorb: {sourceName}");
        if (pictures.Count > 0xFFFF)
            throw new InvalidDataException($"Too many Pict resources ({pictures.Count}).");

        int release = ReadReleaseNumber(blorb);

        int headerSize = HeaderSize + pictures.Count * IndexEntrySize;
        int dataTotal = pictures.Sum(p => p.Bitmap.Length);
        var pack = new byte[headerSize + dataTotal];

        pack[0] = (byte)'N'; pack[1] = (byte)'Z'; pack[2] = (byte)'P'; pack[3] = (byte)'K';
        pack[4] = 1; // version
        BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(5), (ushort)pictures.Count);
        BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(7), (ushort)release);

        int idx = HeaderSize;
        int dataOffset = headerSize; // offsets are absolute within the pak
        foreach (var p in pictures.OrderBy(p => p.Number))
        {
            BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(idx), (ushort)p.Number);
            BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(idx + 2), (ushort)p.Width);
            BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(idx + 4), (ushort)p.Height);
            pack[idx + 6] = p.Flags;
            if (p.Bitmap.Length > 0)
            {
                BinaryPrimitives.WriteUInt32LittleEndian(pack.AsSpan(idx + 7), (uint)dataOffset);
                BinaryPrimitives.WriteUInt32LittleEndian(pack.AsSpan(idx + 11), (uint)p.Bitmap.Length);
                Array.Copy(p.Bitmap, 0, pack, dataOffset, p.Bitmap.Length);
                dataOffset += p.Bitmap.Length;
            }
            // Rect placeholders: offset/len stay 0.
            idx += IndexEntrySize;
        }

        return pack;
    }

    /// <summary>Human-readable summary for the packer's console output.</summary>
    public static string Summary(byte[] pack)
    {
        int count = BinaryPrimitives.ReadUInt16LittleEndian(pack.AsSpan(5));
        int release = BinaryPrimitives.ReadUInt16LittleEndian(pack.AsSpan(7));
        int bitmaps = 0, transparent = 0;
        for (int i = 0; i < count; i++)
        {
            int e = HeaderSize + i * IndexEntrySize;
            if (BinaryPrimitives.ReadUInt32LittleEndian(pack.AsSpan(e + 11)) > 0)
                bitmaps++;
            if ((pack[e + 6] & FlagTransparent) != 0)
                transparent++;
        }
        return $"{count} pictures ({bitmaps} bitmaps, {count - bitmaps} rects, {transparent} transparent), release {release}";
    }

    private static List<Picture> DecodePictures(byte[] blorb, string sourceName)
    {
        if (blorb.Length < 12 ||
            Tag(blorb, 0) != "FORM" || Tag(blorb, 8) != "IFRS")
            throw new InvalidDataException($"Not a Blorb file (expected FORM/IFRS): {sourceName}");

        int ridx = FindChunk(blorb, "RIdx") ?? throw new InvalidDataException($"Blorb has no RIdx chunk: {sourceName}");
        int count = ReadBE32(blorb, ridx + 8);
        int entry = ridx + 12;

        var pictures = new List<Picture>();
        for (int i = 0; i < count; i++, entry += 12)
        {
            if (Tag(blorb, entry) != "Pict")
                continue;
            int number = ReadBE32(blorb, entry + 4);
            int start = ReadBE32(blorb, entry + 8);
            string tag = Tag(blorb, start);
            int size = ReadBE32(blorb, start + 4);
            switch (tag)
            {
                case "PNG ":
                {
                    var png = blorb.AsSpan(start + 8, size).ToArray();
                    pictures.Add(DecodePng(number, png));
                    break;
                }
                case "Rect":
                {
                    int w = ReadBE32(blorb, start + 8);
                    int h = ReadBE32(blorb, start + 12);
                    pictures.Add(new Picture(number, w, h, 0, []));
                    break;
                }
                default:
                    throw new InvalidDataException($"Pict {number} has unsupported chunk type '{tag}' in {sourceName}");
            }
        }

        return pictures;
    }

    private static Picture DecodePng(int number, byte[] png)
    {
        using var img = Image.Load<Rgba32>(png);
        int width = img.Width;
        int height = img.Height;

        // Quantize opaque pixels; collect which EGA indices they use.
        var indices = new byte[width * height];
        var transparent = new bool[width * height];
        bool hasTransparency = false;
        var used = new bool[16];

        img.ProcessPixelRows(accessor =>
        {
            for (int y = 0; y < height; y++)
            {
                var row = accessor.GetRowSpan(y);
                for (int x = 0; x < width; x++)
                {
                    var px = row[x];
                    int i = y * width + x;
                    if (px.A < 128)
                    {
                        transparent[i] = true;
                        hasTransparency = true;
                    }
                    else
                    {
                        byte ega = QuantizeToEga(px.R, px.G, px.B);
                        indices[i] = ega;
                        used[ega] = true;
                    }
                }
            }
        });

        byte flags = 0;
        if (hasTransparency)
        {
            int free = Array.IndexOf(used, false);
            if (free < 0)
                throw new InvalidDataException(
                    $"Pict {number}: transparent picture uses all 16 EGA colors; no index left for transparency.");
            flags = (byte)(FlagTransparent | (free << 4));
            for (int i = 0; i < indices.Length; i++)
                if (transparent[i])
                    indices[i] = (byte)free;
        }

        // 4bpp row-packed, high nibble = left pixel, rows padded to bytes.
        int rowBytes = (width + 1) / 2;
        var bitmap = new byte[rowBytes * height];
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                byte v = indices[y * width + x];
                int o = y * rowBytes + x / 2;
                bitmap[o] |= (x & 1) == 0 ? (byte)(v << 4) : v;
            }
        }

        return new Picture(number, width, height, flags, bitmap);
    }

    private static int ReadReleaseNumber(byte[] blorb)
    {
        int? reln = FindChunk(blorb, "RelN");
        if (reln is null)
            return 0;
        return (blorb[reln.Value + 8] << 8) | blorb[reln.Value + 9];
    }

    private static int? FindChunk(byte[] data, string tag)
    {
        int p = 12; // skip FORM + size + IFRS
        while (p + 8 <= data.Length)
        {
            int sz = ReadBE32(data, p + 4);
            if (Tag(data, p) == tag)
                return p;
            p += 8 + sz + (sz & 1);
        }
        return null;
    }

    private static string Tag(byte[] d, int o) => System.Text.Encoding.ASCII.GetString(d, o, 4);
    private static int ReadBE32(byte[] d, int o) => (d[o] << 24) | (d[o + 1] << 16) | (d[o + 2] << 8) | d[o + 3];
}
