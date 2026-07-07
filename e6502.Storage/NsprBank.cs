namespace e6502.Storage;

/// <summary>
/// NSPR ("Nova SPRite bank", <c>.nsp</c>) — a metasprite animation sheet.
///
/// One file holds many <b>characters</b>; each character is a metasprite (a
/// layout of 16×16 <b>parts</b>, each part = one hardware sprite at a dx/dy
/// offset) with many named <b>animations</b>. Animation is cheap: the part
/// layout is fixed and each frame only reassigns which <b>shape</b> (16×16 4bpp
/// pixel cell, 128 bytes) each part shows. This mirrors the on-device runtime
/// exactly — <c>msprite.s</c> (visual descriptor = parts) + <c>anim.s</c>
/// (frames swap shape indices). Loading a bank = DMA the shape pool to XRAM,
/// spawn each character's metasprite, hand the anim runtime the frame tables.
///
/// A plain 16×16 sprite is just a 1-part metasprite, so the format never needs
/// a v1→v2 break; <see cref="NsprKind"/> reserves room for vsprites later.
///
/// Byte layout (little-endian, all counts are u8 → ≤255):
/// <code>
/// Header (16 B)
///   0   "NSPR"
///   4   version
///   5   kind          (0 = hw metasprite)
///   6   flags         (bit0 = palette present)
///   7   shapeCount
///   8   charCount
///   9   cellW  (16)
///   10  cellH  (16)
///   11  reserved[5]
/// Palette (48 B, only if flags bit0)   16 × {R,G,B}
/// Shape pool: shapeCount × 128 B
/// Characters × charCount {
///   name[8]  (ASCII, NUL-padded)
///   partCount
///   parts × { dx, dy, flags }
///   animCount
///   animations × {
///     name[8]
///     frameCount
///     ticks
///     flags        (bit0 loop, bit1 ping-pong)
///     frames × ( partCount × shapeIndex )
///   }
/// }
/// </code>
/// </summary>
public sealed class NsprBank
{
    public const int ShapeBytes = 128;   // 16×16, 4bpp, 2px/byte
    public const int CellW = 16;
    public const int CellH = 16;
    public const int NameLen = 8;
    public const int PaletteBytes = 48;  // 16 × RGB
    private static readonly byte[] Magic = "NSPR"u8.ToArray();

    public byte Version { get; set; } = 1;
    public NsprKind Kind { get; set; } = NsprKind.HwMetasprite;
    /// <summary>16×RGB (48 bytes) if the intended palette is embedded, else null (use current VGC palette).</summary>
    public byte[]? Palette { get; set; }
    /// <summary>Shared pool of 16×16 shapes (128 bytes each); animations reference these by index.</summary>
    public List<byte[]> Shapes { get; } = new();
    public List<NsprCharacter> Characters { get; } = new();

    public void Write(Stream s)
    {
        Validate();
        bool pal = Palette is not null;
        s.Write(Magic);
        s.WriteByte(Version);
        s.WriteByte((byte)Kind);
        s.WriteByte((byte)(pal ? 0x01 : 0x00));
        s.WriteByte((byte)Shapes.Count);
        s.WriteByte((byte)Characters.Count);
        s.WriteByte(CellW);
        s.WriteByte(CellH);
        s.Write(new byte[5]);                          // reserved
        if (pal) s.Write(Palette!);
        foreach (byte[] shape in Shapes) s.Write(shape);

        foreach (NsprCharacter ch in Characters)
        {
            WriteName(s, ch.Name);
            s.WriteByte((byte)ch.Parts.Count);
            foreach (NsprPart p in ch.Parts) { s.WriteByte(p.Dx); s.WriteByte(p.Dy); s.WriteByte(p.Flags); }
            s.WriteByte((byte)ch.Animations.Count);
            foreach (NsprAnimation a in ch.Animations)
            {
                WriteName(s, a.Name);
                s.WriteByte((byte)a.Frames.Count);
                s.WriteByte(a.Ticks);
                s.WriteByte((byte)a.Flags);
                foreach (byte[] frame in a.Frames) s.Write(frame);   // partCount bytes each
            }
        }
    }

    public static NsprBank Read(Stream s)
    {
        var bank = new NsprBank();
        byte[] header = ReadExactly(s, 16);
        if (!header.AsSpan(0, 4).SequenceEqual(Magic))
            throw new InvalidDataException("not an NSPR bank (bad magic)");
        bank.Version = header[4];
        bank.Kind = (NsprKind)header[5];
        bool pal = (header[6] & 0x01) != 0;
        int shapeCount = header[7];
        int charCount = header[8];
        // header[9]/[10] = cellW/cellH (fixed 16×16 for v1); reserved[11..15] ignored.

        if (pal) bank.Palette = ReadExactly(s, PaletteBytes);
        for (int i = 0; i < shapeCount; i++) bank.Shapes.Add(ReadExactly(s, ShapeBytes));

        for (int c = 0; c < charCount; c++)
        {
            var ch = new NsprCharacter { Name = ReadName(s) };
            int partCount = s.ReadByte();
            if (partCount < 0) throw new EndOfStreamException();
            for (int p = 0; p < partCount; p++)
            {
                byte[] part = ReadExactly(s, 3);
                ch.Parts.Add(new NsprPart { Dx = part[0], Dy = part[1], Flags = part[2] });
            }
            int animCount = s.ReadByte();
            if (animCount < 0) throw new EndOfStreamException();
            for (int a = 0; a < animCount; a++)
            {
                var anim = new NsprAnimation { Name = ReadName(s) };
                int frameCount = s.ReadByte();
                int ticks = s.ReadByte();
                int flags = s.ReadByte();
                if (frameCount < 0 || ticks < 0 || flags < 0) throw new EndOfStreamException();
                anim.Ticks = (byte)ticks;
                anim.Flags = (NsprAnimFlags)flags;
                for (int f = 0; f < frameCount; f++) anim.Frames.Add(ReadExactly(s, partCount));
                ch.Animations.Add(anim);
            }
            bank.Characters.Add(ch);
        }
        return bank;
    }

    public byte[] ToBytes() { using var ms = new MemoryStream(); Write(ms); return ms.ToArray(); }
    public static NsprBank FromBytes(byte[] data) { using var ms = new MemoryStream(data); return Read(ms); }

    /// <summary>Throws if the model can't be encoded (u8 count overflow, wrong shape/frame sizes, oversized names).</summary>
    public void Validate()
    {
        if (Shapes.Count > 255) throw new InvalidDataException("shape count > 255");
        if (Characters.Count > 255) throw new InvalidDataException("character count > 255");
        if (Palette is { Length: not PaletteBytes }) throw new InvalidDataException($"palette must be {PaletteBytes} bytes");
        foreach (byte[] sh in Shapes)
            if (sh.Length != ShapeBytes) throw new InvalidDataException($"shape must be {ShapeBytes} bytes");
        foreach (NsprCharacter ch in Characters)
        {
            if (ch.Parts.Count > 255 || ch.Animations.Count > 255)
                throw new InvalidDataException("part/anim count > 255");
            foreach (NsprAnimation a in ch.Animations)
            {
                if (a.Frames.Count > 255) throw new InvalidDataException("frame count > 255");
                foreach (byte[] frame in a.Frames)
                {
                    if (frame.Length != ch.Parts.Count)
                        throw new InvalidDataException("frame must hold one shape index per part");
                    foreach (byte idx in frame)
                        if (idx >= Shapes.Count) throw new InvalidDataException($"frame references shape {idx} (only {Shapes.Count} shapes)");
                }
            }
        }
    }

    private static void WriteName(Stream s, string name)
    {
        var buf = new byte[NameLen];
        int n = Math.Min(name.Length, NameLen);
        for (int i = 0; i < n; i++) buf[i] = (byte)name[i];
        s.Write(buf);
    }

    private static string ReadName(Stream s)
    {
        byte[] buf = ReadExactly(s, NameLen);
        int len = Array.IndexOf(buf, (byte)0);
        if (len < 0) len = NameLen;
        return System.Text.Encoding.ASCII.GetString(buf, 0, len);
    }

    private static byte[] ReadExactly(Stream s, int count)
    {
        var buf = new byte[count];
        int off = 0;
        while (off < count)
        {
            int r = s.Read(buf, off, count - off);
            if (r <= 0) throw new EndOfStreamException($"expected {count} bytes, got {off}");
            off += r;
        }
        return buf;
    }
}

public enum NsprKind : byte { HwMetasprite = 0, Vsprite = 1 }

[Flags]
public enum NsprAnimFlags : byte { None = 0, Loop = 1, PingPong = 2 }

/// <summary>One hardware-sprite part of a metasprite: a 16×16 cell at a pixel offset.</summary>
public sealed class NsprPart
{
    public byte Dx { get; set; }
    public byte Dy { get; set; }
    public byte Flags { get; set; }   // per-part flip bits (matches msprite PART_FLAGS)
}

public sealed class NsprAnimation
{
    public string Name { get; set; } = "";
    public byte Ticks { get; set; } = 6;                       // frames of delay per animation frame
    public NsprAnimFlags Flags { get; set; } = NsprAnimFlags.Loop;
    /// <summary>Each frame is one shape index per part (length == owning character's part count).</summary>
    public List<byte[]> Frames { get; } = new();
}

public sealed class NsprCharacter
{
    public string Name { get; set; } = "";
    public List<NsprPart> Parts { get; } = new();
    public List<NsprAnimation> Animations { get; } = new();
}
