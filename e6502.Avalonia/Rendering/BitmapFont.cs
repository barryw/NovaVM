using e6502.Avalonia.Input;

namespace e6502.Avalonia.Rendering;

public class BitmapFont
{
    public const int MaxSlots = 8;
    public const int GlyphWidth = 8;
    public const int GlyphHeight = 8;
    public const int GlyphCount = 256;
    public const int FontDataSize = GlyphCount * GlyphHeight; // 2048

    private readonly byte[][] _slots;
    private readonly FontKeymap[] _keymaps;

    /// <summary>Single-font constructor (backward compatible).</summary>
    public BitmapFont(byte[] fontData) : this(new[] { fontData }) { }

    /// <summary>Multi-font constructor. Slot 0 is the default/fallback.</summary>
    public BitmapFont(byte[][] fontSlots)
    {
        if (fontSlots.Length == 0)
            throw new ArgumentException("At least one font slot required");
        if (fontSlots.Length > MaxSlots)
            throw new ArgumentException($"Maximum {MaxSlots} font slots");
        foreach (var slot in fontSlots)
            if (slot.Length != FontDataSize)
                throw new ArgumentException($"Font data must be {FontDataSize} bytes");
        _slots = fontSlots;
        _keymaps = new FontKeymap[fontSlots.Length];
        for (int i = 0; i < _keymaps.Length; i++)
            _keymaps[i] = FontKeymap.None;
    }

    public int SlotCount => _slots.Length;

    public FontKeymap GetKeymap(int slot) =>
        slot >= 0 && slot < _keymaps.Length ? _keymaps[slot] : FontKeymap.None;

    public void SetKeymap(int slot, FontKeymap keymap)
    {
        if (slot >= 0 && slot < _keymaps.Length)
            _keymaps[slot] = keymap;
    }

    public static BitmapFont LoadFromFile(string path) =>
        new(File.ReadAllBytes(path));

    /// <summary>Returns true if pixel (x,y) is set in slot 0.</summary>
    public bool GetPixel(byte charCode, int x, int y) =>
        GetPixel(0, charCode, x, y);

    /// <summary>Returns true if pixel (x,y) is set in the given font slot.</summary>
    public bool GetPixel(int slot, byte charCode, int x, int y)
    {
        byte row = GetRow(slot, charCode, y);
        return (row & (0x80 >> x)) != 0;
    }

    /// <summary>Returns raw row byte from slot 0.</summary>
    public byte GetRow(byte charCode, int y) =>
        GetRow(0, charCode, y);

    /// <summary>Returns raw row byte from the given font slot (falls back to slot 0).</summary>
    public byte GetRow(int slot, byte charCode, int y)
    {
        if (slot < 0 || slot >= _slots.Length)
            slot = 0;
        return _slots[slot][charCode * GlyphHeight + y];
    }
}
