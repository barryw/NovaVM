// =====================================================================
//  NovaZ — BlorbPictures.cs
//
//  Decodes the Pict resources (PNG art + Rect placeholders) of a Blorb
//  file and packs them into the NovaZ PICS.PAK format, pre-quantized to a
//  16-colour palette so the 6502 never decodes or color-matches anything.
//
//  Copyright (C) 2026 Barry Walker
//  SPDX-License-Identifier: MIT
// =====================================================================

using System.Buffers.Binary;
using SixLabors.ImageSharp;
using SixLabors.ImageSharp.PixelFormats;

/// <summary>
/// Decodes the Pict resources of a Blorb file (PNG art + Rect placeholders)
/// and packs them into the NovaZ PICS.PAK format, pre-quantized to a 16-colour
/// palette so the 6502 never decodes or color-matches anything.
///
/// PICS.PAK layout (all multi-byte fields little-endian):
///   v1: "NZPK", version(1), count(2), release(2)
///   v2: "NZPK", version(1), count(2), release(2), palette[16 RGB triples]
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
/// bit 0 = picture has transparency; flags bits 4-7 = the transparent palette
/// index, chosen per picture as the lowest index unused by its opaque pixels.
/// Bit 2 marks small transparent framed icons (opaque outer frame with
/// transparent interior), letting the runtime align them to text flow without
/// scanning bitmap data. Bit 3 marks larger transparent framed flow images
/// that should snap to the top of a text cell when drawn. Bit 1 is reserved
/// for the FIO odd-width blit path, so PICS.PAK metadata never uses it.
/// Opaque pictures use all 16 colors freely and carry flags 0.
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
    private const int HeaderSizeV1 = 9;
    private const int HeaderSizeV2 = 9 + PaletteByteCount;
    private const int IndexEntrySize = 15;
    private const int PaletteByteCount = 16 * 3;
    private const byte Version1 = 1;
    private const byte Version2 = 2;
    private const byte FlagTransparent = 0x01;
    private const byte FlagFlowFramedIcon = 0x04;
    private const byte FlagFlowCellTop = 0x08;

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

    private readonly record struct Rgb(byte R, byte G, byte B);
    private readonly record struct ColorCount(byte R, byte G, byte B, long Count);
    private sealed record SourcePicture(int Number, int Width, int Height, Rgba32[]? Pixels, bool[]? Transparent);
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
        var sourcePictures = DecodePictures(blorb, sourceName);
        if (sourcePictures.Count == 0)
            throw new InvalidDataException($"No Pict resources found in Blorb: {sourceName}");
        if (sourcePictures.Count > 0xFFFF)
            throw new InvalidDataException($"Too many Pict resources ({sourcePictures.Count}).");

        int release = ReadReleaseNumber(blorb);
        Rgb[] palette = BuildGlobalPalette(sourcePictures);
        var pictures = sourcePictures.Select(p => EncodePicture(p, palette)).ToList();

        int headerSize = HeaderSizeV2 + pictures.Count * IndexEntrySize;
        int dataTotal = pictures.Sum(p => p.Bitmap.Length);
        var pack = new byte[headerSize + dataTotal];

        pack[0] = (byte)'N'; pack[1] = (byte)'Z'; pack[2] = (byte)'P'; pack[3] = (byte)'K';
        pack[4] = Version2;
        BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(5), (ushort)pictures.Count);
        BinaryPrimitives.WriteUInt16LittleEndian(pack.AsSpan(7), (ushort)release);
        WritePalette(pack.AsSpan(9, PaletteByteCount), palette);

        int idx = HeaderSizeV2;
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
        int headerSize = HeaderSizeForVersion(pack);
        int count = BinaryPrimitives.ReadUInt16LittleEndian(pack.AsSpan(5));
        int release = BinaryPrimitives.ReadUInt16LittleEndian(pack.AsSpan(7));
        int bitmaps = 0, transparent = 0;
        for (int i = 0; i < count; i++)
        {
            int e = headerSize + i * IndexEntrySize;
            if (BinaryPrimitives.ReadUInt32LittleEndian(pack.AsSpan(e + 11)) > 0)
                bitmaps++;
            if ((pack[e + 6] & FlagTransparent) != 0)
                transparent++;
        }
        return $"v{pack[4]}, {count} pictures ({bitmaps} bitmaps, {count - bitmaps} rects, {transparent} transparent), release {release}";
    }

    private static List<SourcePicture> DecodePictures(byte[] blorb, string sourceName)
    {
        if (blorb.Length < 12 ||
            Tag(blorb, 0) != "FORM" || Tag(blorb, 8) != "IFRS")
            throw new InvalidDataException($"Not a Blorb file (expected FORM/IFRS): {sourceName}");

        int ridx = FindChunk(blorb, "RIdx") ?? throw new InvalidDataException($"Blorb has no RIdx chunk: {sourceName}");
        int count = ReadBE32(blorb, ridx + 8);
        int entry = ridx + 12;

        var pictures = new List<SourcePicture>();
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
                    pictures.Add(new SourcePicture(number, w, h, null, null));
                    break;
                }
                default:
                    throw new InvalidDataException($"Pict {number} has unsupported chunk type '{tag}' in {sourceName}");
            }
        }

        return pictures;
    }

    private static SourcePicture DecodePng(int number, byte[] png)
    {
        using var img = Image.Load<Rgba32>(png);
        int width = img.Width;
        int height = img.Height;

        var pixels = new Rgba32[width * height];
        var transparent = new bool[width * height];

        img.ProcessPixelRows(accessor =>
        {
            for (int y = 0; y < height; y++)
            {
                var row = accessor.GetRowSpan(y);
                for (int x = 0; x < width; x++)
                {
                    var px = row[x];
                    int i = y * width + x;
                    pixels[i] = px;
                    if (px.A < 128)
                        transparent[i] = true;
                }
            }
        });

        return new SourcePicture(number, width, height, pixels, transparent);
    }

    private static Picture EncodePicture(SourcePicture source, Rgb[] palette)
    {
        if (source.Pixels is null || source.Transparent is null)
            return new Picture(source.Number, source.Width, source.Height, 0, []);

        int width = source.Width;
        int height = source.Height;
        var indices = new byte[width * height];
        bool hasTransparency = false;
        var used = new bool[16];

        for (int i = 0; i < source.Pixels.Length; i++)
        {
            if (source.Transparent[i])
            {
                hasTransparency = true;
                continue;
            }

            Rgba32 px = NormalizeSourcePixel(source.Number, source.Pixels[i]);
            byte index = QuantizeToPalette(px.R, px.G, px.B, palette);
            indices[i] = index;
            used[index] = true;
        }

        byte flags = 0;
        if (hasTransparency)
        {
            int free = Array.IndexOf(used, false);
            if (free < 0)
                throw new InvalidDataException(
                    $"Pict {source.Number}: transparent picture uses all 16 palette colors; no index left for transparency.");
            flags = (byte)(FlagTransparent | (free << 4));
            for (int i = 0; i < indices.Length; i++)
                if (source.Transparent[i])
                    indices[i] = (byte)free;
            bool hasOpaqueFrame = HasOpaqueFrameWithTransparentInterior(source);
            if (hasOpaqueFrame && IsSmallFlowFramedIcon(source))
                flags |= FlagFlowFramedIcon;
            else if (IsCellTopFlowImage(source, hasOpaqueFrame))
                flags |= FlagFlowCellTop;
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

        return new Picture(source.Number, width, height, flags, bitmap);
    }

    private static bool HasOpaqueFrameWithTransparentInterior(SourcePicture source)
    {
        if (source.Transparent is null || source.Width < 3 || source.Height < 3)
            return false;

        for (int x = 0; x < source.Width; x++)
            if (source.Transparent[x] || source.Transparent[(source.Height - 1) * source.Width + x])
                return false;

        for (int y = 0; y < source.Height; y++)
            if (source.Transparent[y * source.Width] ||
                source.Transparent[y * source.Width + source.Width - 1])
                return false;

        for (int y = 1; y < source.Height - 1; y++)
            for (int x = 1; x < source.Width - 1; x++)
                if (source.Transparent[y * source.Width + x])
                    return true;

        return false;
    }

    private static bool IsSmallFlowFramedIcon(SourcePicture source) =>
        source.Width <= 32 && source.Height <= 32;

    private static bool IsCellTopFlowImage(SourcePicture source, bool hasOpaqueFrame)
    {
        if (source.Transparent is null ||
            source.Width is <= 32 or > 64 ||
            source.Height is <= 32 or > 64 ||
            (source.Height & 0x03) != 0x03 ||
            !HasOpaquePixelOnEveryEdge(source))
            return false;

        return hasOpaqueFrame || IsMostlyOpaqueFullExtentImage(source);
    }

    private static bool HasOpaquePixelOnEveryEdge(SourcePicture source)
    {
        if (source.Transparent is null)
            return false;

        bool top = false, bottom = false, left = false, right = false;
        for (int x = 0; x < source.Width; x++)
        {
            top |= !source.Transparent[x];
            bottom |= !source.Transparent[(source.Height - 1) * source.Width + x];
        }

        for (int y = 0; y < source.Height; y++)
        {
            left |= !source.Transparent[y * source.Width];
            right |= !source.Transparent[y * source.Width + source.Width - 1];
        }

        return top && bottom && left && right;
    }

    private static bool IsMostlyOpaqueFullExtentImage(SourcePicture source)
    {
        if (source.Transparent is null)
            return false;

        int transparent = 0;
        foreach (bool px in source.Transparent)
            if (px)
                transparent++;

        return transparent > 0 && transparent <= (source.Width * source.Height) / 8;
    }

    private static Rgba32 NormalizeSourcePixel(int pictureNumber, Rgba32 px)
    {
        // Zork Zero picture $01E1 is the blank up/down indicator slot. The
        // Blorb stores it as solid EGA red, but the story draws it over the
        // banner ribbon to erase the arrow variants; match the ribbon fill.
        if (pictureNumber == 0x01E1 &&
            px.A >= 128 &&
            px.R == 170 && px.G == 0 && px.B == 0)
            return new Rgba32(170, 102, 68, px.A);

        return px;
    }

    private static Rgb[] BuildGlobalPalette(IReadOnlyList<SourcePicture> pictures)
    {
        var counts = new Dictionary<int, long>();
        foreach (var picture in pictures)
        {
            if (picture.Pixels is null || picture.Transparent is null)
                continue;

            for (int i = 0; i < picture.Pixels.Length; i++)
            {
                if (picture.Transparent[i])
                    continue;

                Rgba32 px = picture.Pixels[i];
                int key = (px.R << 16) | (px.G << 8) | px.B;
                counts.TryGetValue(key, out long count);
                counts[key] = count + 1;
            }
        }

        if (counts.Count == 0)
            return EgaPalette.Select(c => new Rgb(c.R, c.G, c.B)).ToArray();

        var colors = counts
            .Select(kv => new ColorCount(
                (byte)(kv.Key >> 16),
                (byte)(kv.Key >> 8),
                (byte)kv.Key,
                kv.Value))
            .ToList();

        List<Rgb> generated = colors.Count <= 16
            ? colors
                .OrderByDescending(c => c.Count)
                .ThenBy(c => c.R)
                .ThenBy(c => c.G)
                .ThenBy(c => c.B)
                .Select(c => new Rgb(c.R, c.G, c.B))
                .ToList()
            : MedianCut(colors, 16);

        return AssignToEgaSlots(generated);
    }

    private static List<Rgb> MedianCut(List<ColorCount> colors, int targetCount)
    {
        var boxes = new List<PaletteBox> { new(colors) };
        while (boxes.Count < targetCount)
        {
            PaletteBox? box = boxes
                .Where(b => b.Colors.Count > 1)
                .OrderByDescending(b => b.Range)
                .ThenByDescending(b => b.Total)
                .FirstOrDefault();
            if (box is null)
                break;

            boxes.Remove(box);
            var (left, right) = box.Split();
            boxes.Add(left);
            boxes.Add(right);
        }

        return boxes
            .OrderByDescending(b => b.Total)
            .Select(b => b.Average())
            .ToList();
    }

    private static Rgb[] AssignToEgaSlots(IReadOnlyList<Rgb> colors)
    {
        var palette = EgaPalette.Select(c => new Rgb(c.R, c.G, c.B)).ToArray();
        var used = new bool[16];

        foreach (Rgb color in colors
            .OrderBy(c => EgaPalette.Min(e => Distance(c.R, c.G, c.B, e.R, e.G, e.B)))
            .ThenBy(c => c.R)
            .ThenBy(c => c.G)
            .ThenBy(c => c.B))
        {
            int bestSlot = -1;
            int bestDistance = int.MaxValue;
            for (int i = 0; i < EgaPalette.Length; i++)
            {
                if (used[i])
                    continue;

                var ega = EgaPalette[i];
                int distance = Distance(color.R, color.G, color.B, ega.R, ega.G, ega.B);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    bestSlot = i;
                }
            }

            if (bestSlot < 0)
                break;

            palette[bestSlot] = color;
            used[bestSlot] = true;
        }

        return palette;
    }

    private static byte QuantizeToPalette(byte r, byte g, byte b, IReadOnlyList<Rgb> palette)
    {
        int best = 0;
        int bestDist = int.MaxValue;
        for (int i = 0; i < palette.Count; i++)
        {
            Rgb p = palette[i];
            int dist = Distance(r, g, b, p.R, p.G, p.B);
            if (dist < bestDist)
            {
                bestDist = dist;
                best = i;
            }
        }

        return (byte)best;
    }

    private static void WritePalette(Span<byte> destination, IReadOnlyList<Rgb> palette)
    {
        for (int i = 0; i < 16; i++)
        {
            Rgb color = palette[i];
            int offset = i * 3;
            destination[offset] = color.R;
            destination[offset + 1] = color.G;
            destination[offset + 2] = color.B;
        }
    }

    private static int HeaderSizeForVersion(byte[] pack)
    {
        if (pack.Length < HeaderSizeV1)
            throw new InvalidDataException("PICS.PAK header is too short.");

        return pack[4] switch
        {
            Version1 => HeaderSizeV1,
            Version2 => HeaderSizeV2,
            _ => throw new InvalidDataException($"Unsupported PICS.PAK version {pack[4]}.")
        };
    }

    private static int Distance(byte r, byte g, byte b, byte pr, byte pg, byte pb)
    {
        int dr = r - pr, dg = g - pg, db = b - pb;
        return dr * dr + dg * dg + db * db;
    }

    private sealed class PaletteBox
    {
        public PaletteBox(List<ColorCount> colors)
        {
            Colors = colors;
            Total = colors.Sum(c => c.Count);
            MinR = colors.Min(c => c.R); MaxR = colors.Max(c => c.R);
            MinG = colors.Min(c => c.G); MaxG = colors.Max(c => c.G);
            MinB = colors.Min(c => c.B); MaxB = colors.Max(c => c.B);
        }

        public List<ColorCount> Colors { get; }
        public long Total { get; }
        private int MinR { get; }
        private int MaxR { get; }
        private int MinG { get; }
        private int MaxG { get; }
        private int MinB { get; }
        private int MaxB { get; }
        public int Range => Math.Max(MaxR - MinR, Math.Max(MaxG - MinG, MaxB - MinB));

        public (PaletteBox Left, PaletteBox Right) Split()
        {
            Func<ColorCount, byte> key = MaxR - MinR >= MaxG - MinG && MaxR - MinR >= MaxB - MinB
                ? c => c.R
                : MaxG - MinG >= MaxB - MinB
                    ? c => c.G
                    : c => c.B;

            var ordered = Colors
                .OrderBy(key)
                .ThenBy(c => c.R)
                .ThenBy(c => c.G)
                .ThenBy(c => c.B)
                .ToList();

            long half = Math.Max(1, Total / 2);
            long accum = 0;
            int split = 1;
            for (; split < ordered.Count; split++)
            {
                accum += ordered[split - 1].Count;
                if (accum >= half)
                    break;
            }

            split = Math.Clamp(split, 1, ordered.Count - 1);
            return (
                new PaletteBox(ordered.GetRange(0, split)),
                new PaletteBox(ordered.GetRange(split, ordered.Count - split)));
        }

        public Rgb Average()
        {
            long r = 0, g = 0, b = 0;
            foreach (ColorCount color in Colors)
            {
                r += color.R * color.Count;
                g += color.G * color.Count;
                b += color.B * color.Count;
            }

            return new Rgb(
                (byte)((r + Total / 2) / Total),
                (byte)((g + Total / 2) / Total),
                (byte)((b + Total / 2) / Total));
        }
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
