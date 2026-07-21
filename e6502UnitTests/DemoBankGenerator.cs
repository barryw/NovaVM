using System;
using System.IO;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

// Generates the spritebank_demo art: a shaded bouncing ball drawn as a 32x32
// (2x2) metasprite with an 8-frame squash-and-stretch cycle. The ball's bottom
// is anchored at in-cell y=29 so the squash frames read as landing on a floor
// once the demo drops the sprite's Y. Run with NSPR_DEMO_GEN=1 to (re)write
// software/assembly/apps/spritebank_demo/demo.nsp.
[TestClass]
public class DemoBankGenerator
{
    const int Cell = 16;
    // Warm shading ramp, highlight outward: white, yellow, orange, red, brown.
    static readonly int[] Ramp = { 1, 7, 8, 2, 9 };

    static int BallColor(int px, int py, double cx, double cy, double rx, double ry)
    {
        double nx = (px + 0.5 - cx) / rx;
        double ny = (py + 0.5 - cy) / ry;
        if (nx * nx + ny * ny > 1.0) return 0;                 // outside -> transparent
        double hx = cx - rx * 0.45, hy = cy - ry * 0.45;       // highlight toward upper-left
        double d = Math.Sqrt((px - hx) * (px - hx) + (py - hy) * (py - hy));
        int idx = (int)(d / (2.0 * Math.Max(rx, ry)) * Ramp.Length);
        return Ramp[Math.Min(idx, Ramp.Length - 1)];
    }

    static byte[] Quadrant(int qx, int qy, double cx, double cy, double rx, double ry)
    {
        var shape = new byte[128];
        for (int row = 0; row < Cell; row++)
            for (int colByte = 0; colByte < Cell / 2; colByte++)
            {
                int px = qx * Cell + colByte * 2, py = qy * Cell + row;
                int left = BallColor(px, py, cx, cy, rx, ry);
                int right = BallColor(px + 1, py, cx, cy, rx, ry);
                // VGC sprite format: high nibble = left (even) pixel, low = right.
                shape[row * (Cell / 2) + colByte] = (byte)(((left & 0x0F) << 4) | (right & 0x0F));
            }
        return shape;
    }

    [TestMethod]
    public void GenerateDemoBank()
    {
        if (Environment.GetEnvironmentVariable("NSPR_DEMO_GEN") != "1")
        { Assert.Inconclusive("set NSPR_DEMO_GEN=1 to regenerate demo.nsp"); return; }

        // (rx, ry) per frame, bottom-anchored: 0 round (air), 1 light squash,
        // 2 full squash (impact). The demo picks by distance to the floor.
        var frames = new (double rx, double ry)[] { (13, 13), (14, 12), (15, 10) };

        var bank = new NsprBank { Kind = NsprKind.HwMetasprite };
        var anim = new NsprAnimation { Name = "BOUNCE", Ticks = 3, Flags = NsprAnimFlags.Loop };
        foreach (var (rx, ry) in frames)
        {
            double cx = 16, cy = 29 - ry;                       // bottom anchored at y=29
            int b = bank.Shapes.Count;
            bank.Shapes.Add(Quadrant(0, 0, cx, cy, rx, ry));
            bank.Shapes.Add(Quadrant(1, 0, cx, cy, rx, ry));
            bank.Shapes.Add(Quadrant(0, 1, cx, cy, rx, ry));
            bank.Shapes.Add(Quadrant(1, 1, cx, cy, rx, ry));
            anim.Frames.Add(new byte[] { (byte)b, (byte)(b + 1), (byte)(b + 2), (byte)(b + 3) });
        }

        var ch = new NsprCharacter { Name = "BALL" };
        ch.Parts.Add(new NsprPart { Dx = 0, Dy = 0 });
        ch.Parts.Add(new NsprPart { Dx = 16, Dy = 0 });
        ch.Parts.Add(new NsprPart { Dx = 0, Dy = 16 });
        ch.Parts.Add(new NsprPart { Dx = 16, Dy = 16 });
        ch.Animations.Add(anim);
        bank.Characters.Add(ch);

        string? root = AppContext.BaseDirectory;
        while (root is not null && !File.Exists(Path.Combine(root, "e6502.sln")))
            root = Directory.GetParent(root)?.FullName;
        string path = Path.Combine(root!, "software", "assembly", "apps", "spritebank_demo", "demo.nsp");
        File.WriteAllBytes(path, bank.ToBytes());
        Console.WriteLine($"wrote {path}: {bank.Shapes.Count} shapes, {anim.Frames.Count} frames");
    }
}
