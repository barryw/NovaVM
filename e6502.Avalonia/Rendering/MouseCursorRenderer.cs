namespace e6502.Avalonia.Rendering;

using e6502.Avalonia.Hardware;

public static class MouseCursorRenderer
{
    public static bool TrySample(
        VirtualGraphicsController.MouseCursorState cursor,
        ReadOnlySpan<byte> shapeRam,
        int x,
        int y,
        byte underlyingColor,
        out byte color)
    {
        color = 0;
        if (!cursor.Enabled)
            return false;

        int localX = x - (cursor.X - cursor.HotX);
        int localY = y - (cursor.Y - cursor.HotY);
        if ((uint)localX >= VgcConstants.SpritePixelSize || (uint)localY >= VgcConstants.SpritePixelSize)
            return false;

        int byteIdx = cursor.Shape * VgcConstants.SpriteShapeSize +
                      localY * VgcConstants.SpriteBytesPerRow +
                      localX / 2;
        if ((uint)byteIdx >= (uint)shapeRam.Length)
            return false;

        byte raw = (localX & 1) == 0
            ? (byte)((shapeRam[byteIdx] >> 4) & 0x0F)
            : (byte)(shapeRam[byteIdx] & 0x0F);
        if (raw == 0)
            return false;

        color = cursor.AutoContrast
            ? (byte)((underlyingColor & 0x08) == 0 ? 15 : 0)
            : cursor.Colorize ? cursor.Color : raw;
        return true;
    }
}
