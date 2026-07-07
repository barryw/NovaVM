using System;
using System.IO;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

// Runs the spritebank_open harness headless: pokes the shared golden fixture
// testdata/nspr/sample.nsp into 6502 RAM, parses it with the on-device loader,
// and checks the header validation + shape-pool/character-table indexing. Same
// fixture the C#/Swift codecs conform to (docs/sprite-editor/NSPR.md).
[TestClass]
public class SpriteBankLoadTests
{
    private const ushort Harness = 0x7200;
    private const ushort Buffer = 0x6000;   // where the harness expects the NSPR buffer
    private const ushort Results = 0x7F00;

    [TestMethod]
    public void SpriteBankOpen_ParsesGoldenFixture()
    {
        string harnessPath = Path.Combine(
            RepoRoot(), "software", "assembly", "apps", "spritebank_test", "spritebank_test.bin");
        if (!File.Exists(harnessPath))
            Assert.Inconclusive("spritebank_test.bin not built (needs cc65: make -C software/runtime/asm && ld65).");

        byte[] harness = File.ReadAllBytes(harnessPath);
        byte[] fixture = File.ReadAllBytes(Path.Combine(RepoRoot(), "testdata", "nspr", "sample.nsp"));

        var bus = new CompositeBusDevice(enableSound: false);
        for (int i = 2; i < harness.Length; i++)                 // skip the 2-byte load-address header
            bus.WriteRam((ushort)(Harness + (i - 2)), harness[i]);
        for (int i = 0; i < fixture.Length; i++)
            bus.WriteRam((ushort)(Buffer + i), fixture[i]);

        var cpu = new Cpu(bus);
        cpu.Boot(Harness);
        RunSteps(cpu, bus, 50_000);

        Assert.AreEqual(0, bus.ReadRam(Results + 0), "result should be OK");
        Assert.AreEqual(5, bus.ReadRam(Results + 1), "shape count");
        Assert.AreEqual(2, bus.ReadRam(Results + 2), "character count");

        int shapesPtr = bus.ReadRam(Results + 3) | (bus.ReadRam(Results + 4) << 8);
        int charsPtr = bus.ReadRam(Results + 5) | (bus.ReadRam(Results + 6) << 8);
        Assert.AreEqual(Buffer + 16 + 48, shapesPtr, "shapes pointer = past 16B header + 48B palette");
        Assert.AreEqual(Buffer + 16 + 48 + 5 * 128, charsPtr, "chars pointer = past the 5-shape pool");

        // The character table starts with the first character's name, "HERO".
        Assert.AreEqual((byte)'H', bus.ReadRam((ushort)charsPtr));
        Assert.AreEqual((byte)'E', bus.ReadRam((ushort)(charsPtr + 1)));
        Assert.AreEqual((byte)'R', bus.ReadRam((ushort)(charsPtr + 2)));
        Assert.AreEqual((byte)'O', bus.ReadRam((ushort)(charsPtr + 3)));

        // spritebank_load_shapes DMA'd the 5-shape pool to XRAM at $020000
        // (harness sets spritebank_xram_base). Shape i is 128 bytes of 0x10+i.
        const int xramBase = 0x020000;
        Assert.AreEqual(0x10, bus.ReadXram(xramBase), "shape 0 at XRAM base");
        Assert.AreEqual(0x10, bus.ReadXram(xramBase + 127), "shape 0 last byte");
        Assert.AreEqual(0x11, bus.ReadXram(xramBase + 128), "shape 1");
        Assert.AreEqual(0x14, bus.ReadXram(xramBase + 4 * 128), "shape 4");
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

    private static string RepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "e6502.sln"))) return dir;
            dir = Directory.GetParent(dir)?.FullName;
        }
        throw new Exception("repo root (e6502.sln) not found");
    }
}
