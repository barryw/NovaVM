namespace e6502.Avalonia.Rendering;

/// <summary>
/// Static block graphics drawing algorithms operating on a flat byte array
/// representing a 320x200 bitmap. Each byte is a color index (0-15, 0 = transparent).
/// Layout: index = y * width + x.
/// </summary>
public static class BlockGraphics
{
    public const int Width  = 320;
    public const int Height = 200;

    public static void Plot(byte[] bmp, int x, int y, byte color)
    {
        if (x < 0 || x >= Width || y < 0 || y >= Height)
            return;
        bmp[y * Width + x] = color;
    }

    public static void Clear(byte[] bmp) =>
        Array.Clear(bmp, 0, bmp.Length);

    public static void Line(byte[] bmp, int x0, int y0, int x1, int y1, byte color)
    {
        int dx  =  Math.Abs(x1 - x0);
        int dy  = -Math.Abs(y1 - y0);
        int sx  = x0 < x1 ? 1 : -1;
        int sy  = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true)
        {
            Plot(bmp, x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    public static void Ellipse(byte[] bmp, int cx, int cy, int rx, int ry, byte color)
    {
        if (rx < 0 || ry < 0) return;

        if (rx == ry)
        {
            // Circle: use octant-symmetric midpoint algorithm
            int x = 0;
            int y = rx;
            int d = 1 - rx;

            PlotOctants(bmp, cx, cy, x, y, color);

            while (x < y)
            {
                if (d < 0)
                {
                    d += 2 * x + 3;
                }
                else
                {
                    d += 2 * (x - y) + 5;
                    y--;
                }
                x++;
                PlotOctants(bmp, cx, cy, x, y, color);
            }
            return;
        }

        // Ellipse: midpoint ellipse algorithm (two regions)
        long rx2 = (long)rx * rx;
        long ry2 = (long)ry * ry;
        long twoRx2 = 2 * rx2;
        long twoRy2 = 2 * ry2;

        int x2 = 0;
        int y2 = ry;
        long px = 0;
        long py = twoRx2 * y2;

        // Region 1: slope magnitude < 1
        long d2 = ry2 - rx2 * ry + rx2 / 4;
        while (px < py)
        {
            PlotQuadrants(bmp, cx, cy, x2, y2, color);
            x2++;
            px += twoRy2;
            if (d2 < 0)
            {
                d2 += ry2 + px;
            }
            else
            {
                y2--;
                py -= twoRx2;
                d2 += ry2 + px - py;
            }
        }

        // Region 2: slope magnitude >= 1
        d2 = ry2 * (long)(x2 * 2 + 1) * (x2 * 2 + 1) / 4
           + rx2 * ((long)y2 - 1) * (y2 - 1)
           - rx2 * ry2;
        while (y2 >= 0)
        {
            PlotQuadrants(bmp, cx, cy, x2, y2, color);
            y2--;
            py -= twoRx2;
            if (d2 > 0)
            {
                d2 += rx2 - py;
            }
            else
            {
                x2++;
                px += twoRy2;
                d2 += rx2 - py + px;
            }
        }
    }

    private static void PlotQuadrants(byte[] bmp, int cx, int cy, int x, int y, byte color)
    {
        Plot(bmp, cx + x, cy + y, color);
        Plot(bmp, cx - x, cy + y, color);
        Plot(bmp, cx + x, cy - y, color);
        Plot(bmp, cx - x, cy - y, color);
    }

    private static void PlotOctants(byte[] bmp, int cx, int cy, int x, int y, byte color)
    {
        Plot(bmp, cx + x, cy + y, color);
        Plot(bmp, cx - x, cy + y, color);
        Plot(bmp, cx + x, cy - y, color);
        Plot(bmp, cx - x, cy - y, color);
        Plot(bmp, cx + y, cy + x, color);
        Plot(bmp, cx - y, cy + x, color);
        Plot(bmp, cx + y, cy - x, color);
        Plot(bmp, cx - y, cy - x, color);
    }

    public static void Rect(byte[] bmp, int x0, int y0, int x1, int y1, byte color)
    {
        Line(bmp, x0, y0, x1, y0, color);
        Line(bmp, x0, y1, x1, y1, color);
        Line(bmp, x0, y0, x0, y1, color);
        Line(bmp, x1, y0, x1, y1, color);
    }

    public static void FloodFill(byte[] bmp, int x, int y, byte color)
    {
        if (x < 0 || x >= Width || y < 0 || y >= Height)
            return;

        byte target = bmp[y * Width + x];
        if (target == color)
            return;

        var queue = new Queue<(int x, int y)>();
        queue.Enqueue((x, y));

        while (queue.Count > 0)
        {
            var (cx, cy) = queue.Dequeue();
            if (cx < 0 || cx >= Width || cy < 0 || cy >= Height)
                continue;

            int idx = cy * Width + cx;
            if (bmp[idx] != target)
                continue;

            // Scan left
            int left = cx;
            while (left > 0 && bmp[cy * Width + left - 1] == target)
                left--;

            // Scan right
            int right = cx;
            while (right < Width - 1 && bmp[cy * Width + right + 1] == target)
                right++;

            // Fill the scanline
            for (int i = left; i <= right; i++)
                bmp[cy * Width + i] = color;

            // Enqueue adjacent rows (only at transitions to avoid duplicates)
            for (int dy = -1; dy <= 1; dy += 2)
            {
                int ny = cy + dy;
                if (ny < 0 || ny >= Height)
                    continue;
                bool prev = false;
                for (int i = left; i <= right; i++)
                {
                    bool curr = bmp[ny * Width + i] == target;
                    if (curr && !prev)
                        queue.Enqueue((i, ny));
                    prev = curr;
                }
            }
        }
    }

    public static void Fill(byte[] bmp, int x0, int y0, int x1, int y1, byte color)
    {
        int left   = Math.Min(x0, x1);
        int right  = Math.Max(x0, x1);
        int top    = Math.Min(y0, y1);
        int bottom = Math.Max(y0, y1);

        left   = Math.Max(left,   0);
        right  = Math.Min(right,  Width  - 1);
        top    = Math.Max(top,    0);
        bottom = Math.Min(bottom, Height - 1);

        for (int y = top; y <= bottom; y++)
        {
            int rowBase = y * Width;
            for (int x = left; x <= right; x++)
                bmp[rowBase + x] = color;
        }
    }

    public static void Text(byte[] bmp, int startX, int startY, int scale,
        BitmapFont font, int fontSlot, byte[] charCodes, int charCount, byte color)
    {
        if (scale < 1) scale = 1;
        int penX = startX;
        int glyphAdvance = BitmapFont.GlyphWidth * scale;

        for (int i = 0; i < charCount; i++)
        {
            byte ch = charCodes[i];
            for (int row = 0; row < BitmapFont.GlyphHeight; row++)
            {
                byte rowBits = font.GetRow(fontSlot, ch, row);
                if (rowBits == 0) continue;
                for (int col = 0; col < BitmapFont.GlyphWidth; col++)
                {
                    if ((rowBits & (0x80 >> col)) == 0) continue;
                    int px = penX + col * scale;
                    int py = startY + row * scale;
                    for (int sy = 0; sy < scale; sy++)
                        for (int sx = 0; sx < scale; sx++)
                            Plot(bmp, px + sx, py + sy, color);
                }
            }
            penX += glyphAdvance;
        }
    }
}
