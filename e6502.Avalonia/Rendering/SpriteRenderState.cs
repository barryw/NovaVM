using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Rendering;

/// <summary>
/// Mutable snapshot of sprite state for the renderer.
/// Copper events can modify this mid-frame without affecting the canonical VGC state.
/// </summary>
public struct SpriteRenderState
{
    private readonly int[] _x;
    private readonly int[] _y;
    private readonly byte[] _shapeIndex;
    private readonly byte[] _flags;     // bit0=xFlip, bit1=yFlip, bit7=enable
    private readonly byte[] _priority;
    private readonly byte[] _transColor;

    private SpriteRenderState(int[] x, int[] y, byte[] shapeIndex, byte[] flags, byte[] priority, byte[] transColor)
    {
        _x = x;
        _y = y;
        _shapeIndex = shapeIndex;
        _flags = flags;
        _priority = priority;
        _transColor = transColor;
    }

    public static SpriteRenderState FromVgc(VirtualGraphicsController vgc)
    {
        int count = VgcConstants.MaxSprites;
        var x = new int[count];
        var y = new int[count];
        var shapeIndex = new byte[count];
        var flags = new byte[count];
        var priority = new byte[count];
        var transColor = new byte[count];

        for (int i = 0; i < count; i++)
        {
            var s = vgc.GetSpriteState(i);
            x[i] = s.x;
            y[i] = s.y;
            shapeIndex[i] = vgc.GetSpriteShapeIndex(i);
            flags[i] = (byte)((s.enabled ? VgcConstants.SprFlagEnable : 0)
                              | (s.flags & VgcConstants.SprFlagFlipMask));
            priority[i] = s.priority;
            transColor[i] = vgc.GetSpriteTransColor(i);
        }

        return new SpriteRenderState(x, y, shapeIndex, flags, priority, transColor);
    }

    public int GetX(int sprite) => _x[sprite];
    public int GetY(int sprite) => _y[sprite];
    public byte GetShapeIndex(int sprite) => _shapeIndex[sprite];
    public byte GetPriority(int sprite) => _priority[sprite];
    public byte GetTransColor(int sprite) => _transColor[sprite];
    public bool IsEnabled(int sprite) => (_flags[sprite] & VgcConstants.SprFlagEnable) != 0;
    public bool IsXFlip(int sprite) => (_flags[sprite] & VgcConstants.SprFlagXFlip) != 0;
    public bool IsYFlip(int sprite) => (_flags[sprite] & VgcConstants.SprFlagYFlip) != 0;

    public void Apply(byte registerIndex, byte value)
    {
        int baseOffset = VgcConstants.SpriteRegBase - VgcConstants.VgcBase;
        int offset = registerIndex - baseOffset;
        if (offset < 0 || offset >= VgcConstants.MaxSprites * VgcConstants.SpriteRegStride) return;

        int sprite = offset / VgcConstants.SpriteRegStride;
        int field = offset % VgcConstants.SpriteRegStride;

        switch (field)
        {
            case VgcConstants.SprRegXLo:
                _x[sprite] = (short)((_x[sprite] & ~0xFF) | value);
                break;
            case VgcConstants.SprRegXHi:
                _x[sprite] = (short)((value << 8) | (_x[sprite] & 0xFF));
                break;
            case VgcConstants.SprRegYLo:
                _y[sprite] = (short)((_y[sprite] & ~0xFF) | value);
                break;
            case VgcConstants.SprRegYHi:
                _y[sprite] = (short)((value << 8) | (_y[sprite] & 0xFF));
                break;
            case VgcConstants.SprRegShape:
                _shapeIndex[sprite] = value;
                break;
            case VgcConstants.SprRegFlags:
                _flags[sprite] = value;
                break;
            case VgcConstants.SprRegPriority:
                _priority[sprite] = (byte)Math.Min((int)value, 2);
                break;
            case VgcConstants.SprRegTransColor:
                _transColor[sprite] = value;
                break;
        }
    }

    public static bool IsSpriteRegister(byte registerIndex)
    {
        int baseOffset = VgcConstants.SpriteRegBase - VgcConstants.VgcBase;
        int offset = registerIndex - baseOffset;
        return offset >= 0 && offset < VgcConstants.MaxSprites * VgcConstants.SpriteRegStride;
    }
}
