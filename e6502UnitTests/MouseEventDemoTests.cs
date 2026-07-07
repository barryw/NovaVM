using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

// Runs the NDK mouse event demo (software/assembly/apps/mousedemo) headless on
// the emulator and drives the VGC mouse registers to prove the mouse_events
// dispatch + areas-of-interest work end to end.
[TestClass]
public class MouseEventDemoTests
{
    private const ushort Load = 0x7200;

    [TestMethod]
    public void MouseDemo_DispatchesClickAndAreaHit()
    {
        // The .bin is a cc65 build artifact (gitignored). Skip when it hasn't been
        // built (e.g. the dotnet-only CI); `make -C software/assembly mousedemo`
        // produces it and this then runs for real.
        string binPath = Path.Combine(
            RepoRoot(), "software", "assembly", "apps", "mousedemo", "mousedemo.bin");
        if (!File.Exists(binPath))
            Assert.Inconclusive("mousedemo.bin not built (run `make -C software/assembly mousedemo`).");
        byte[] img = File.ReadAllBytes(binPath);

        var bus = new CompositeBusDevice(enableSound: false);
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        for (int i = 2; i < img.Length; i++)               // skip the 2-byte load-address header
            bus.WriteRam((ushort)(Load + (i - 2)), img[i]);

        var cpu = new Cpu(bus);
        cpu.Boot(Load);
        RunSteps(cpu, bus, 300_000);                       // init: draw UI + register areas -> main loop

        StringAssert.Contains(Snapshot(bus.Vgc), "NOVA MOUSE EVENT DEMO");

        // Left-click inside the RESET area (mouse x 32..76, y 32..44).
        Click(cpu, bus, x: 50, y: 38, button: 0x01);
        string s = Snapshot(bus.Vgc);
        StringAssert.Contains(s, "button=$01");
        StringAssert.Contains(s, "RESET");

        // Release, then right-click outside every area -> zone (none).
        Release(cpu, bus);
        Click(cpu, bus, x: 10, y: 10, button: 0x02);
        s = Snapshot(bus.Vgc);
        StringAssert.Contains(s, "button=$02");
        StringAssert.Contains(s, "(none)");
    }

    private static void Click(Cpu cpu, CompositeBusDevice bus, int x, int y, byte button)
    {
        bus.Write(0xA0D0, (byte)(x & 0xFF));
        bus.Write(0xA0D1, (byte)((x >> 8) & 0x01));
        bus.Write(0xA0D2, (byte)y);
        bus.Write(0xA0D3, button);
        Pump(cpu, bus);
    }

    private static void Release(Cpu cpu, CompositeBusDevice bus)
    {
        bus.Write(0xA0D3, 0x00);
        Pump(cpu, bus);
    }

    // Advance one frame so wait_vsync returns (and the mouse regs latch), then
    // run enough steps for one mouse_poll + handler pass.
    private static void Pump(Cpu cpu, CompositeBusDevice bus)
    {
        bus.Vgc.IncrementFrameCounter();
        RunSteps(cpu, bus, 60_000);
    }

    private static void RunSteps(Cpu cpu, CompositeBusDevice bus, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
        }
    }

    private static string Snapshot(VirtualGraphicsController vgc)
    {
        var sb = new StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = vgc.GetScreenChar(col, row);
                sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
            }
            sb.Append('\n');
        }
        return sb.ToString();
    }

    private static string RepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "software", "assembly", "apps", "mousedemo", "mousedemo.s")))
                return dir;
            dir = Directory.GetParent(dir)?.FullName;
        }
        throw new Exception("repo root not found");
    }
}
