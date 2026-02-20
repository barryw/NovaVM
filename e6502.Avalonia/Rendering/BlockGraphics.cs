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

    public static void Circle(byte[] bmp, int cx, int cy, int r, byte color)
    {
        if (r < 0) return;

        int x   = 0;
        int y   = r;
        int d   = 1 - r;

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
}
