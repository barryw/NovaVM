using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class EditbufAssemblyRuntimeTests
{
    private const ushort GotoEntry = 0x4000;
    private const ushort CutEntry = 0x4003;
    private const ushort UndoEntry = 0x4006;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort GotoResult = 0x2200;
    private const ushort CutResult = 0x2240;
    private const ushort UndoResult = 0x2260;
    private const int UndoText = 1700;
    private const int UndoSteps = 40;

    [TestMethod]
    public void GotoLineParsesLineNumbersAbove255()
    {
        BusDevice bus = Run(GotoEntry);

        Assert.AreEqual(0xAA, bus.Read(GotoResult + 0x1F), "goto fixture ran to completion");

        // 1-based line input -> 0-based buffer line index.
        AssertGoto(bus, slot: 0, line: 0, "goto 1");
        AssertGoto(bus, slot: 3, line: 4, "goto 5");
        AssertGoto(bus, slot: 6, line: 255, "goto 256");
        AssertGoto(bus, slot: 9, line: 999, "goto 1000");
    }

    [TestMethod]
    public void CutNeverDeletesMoreThanItCopies()
    {
        BusDevice bus = Run(CutEntry);

        Assert.AreEqual(0xAA, bus.Read(CutResult + 4), "cut fixture ran to completion");

        int clip = bus.Read(CutResult + 2) | (bus.Read(CutResult + 3) << 8);
        int len = bus.Read(CutResult) | (bus.Read(CutResult + 1) << 8);

        // XRAM-backed clipboard should hold the full 300-byte selection.
        Assert.AreEqual(300, clip, "clipboard holds the selected text");
        // Cut must delete exactly what it copied.
        Assert.AreEqual(300 - clip, len, "buffer keeps only bytes that could not be clipped");
    }

    /// <summary>
    /// Undo depth must not depend on document size, and a compound edit must
    /// still be one step. 40 edits to a 1700-byte document is 68 KB of history as
    /// whole-buffer snapshots — which no arena could have held, since every
    /// stack offset in textsvc.s is 16-bit. As deltas it is about 600 bytes.
    /// </summary>
    [TestMethod]
    public void UndoWalksBackFortyEditsAndTreatsABlockIndentAsOneStep()
    {
        var bus = new XramWindowBus(Fixture(), 0x4000);
        Run(bus, UndoEntry);

        Assert.AreEqual(0xAA, bus.Read(UndoResult + 9), "undo fixture ran to completion");

        int indentedLen = bus.Read(UndoResult + 6) | (bus.Read(UndoResult + 7) << 8);
        Assert.IsTrue(indentedLen > UndoText, $"block indent should have grown the document (got {indentedLen})");
        Assert.AreEqual(1, bus.Read(UndoResult + 8),
            "one undo should take a whole block indent back off, not one space at a time");

        int undoneLen = bus.Read(UndoResult) | (bus.Read(UndoResult + 1) << 8);
        Assert.AreEqual(UndoText, undoneLen, $"{UndoSteps} undos should restore the original length");
        Assert.AreEqual(1, bus.Read(UndoResult + 2), "undone text should match the original byte for byte");

        int redoneLen = bus.Read(UndoResult + 3) | (bus.Read(UndoResult + 4) << 8);
        Assert.AreEqual(UndoText + UndoSteps, redoneLen, $"{UndoSteps} redos should restore the typed length");
        Assert.AreEqual(1, bus.Read(UndoResult + 5), "redone text should match what was typed");
    }

    /// <summary>
    /// Flat RAM plus the one XMC window TEXTSVC uses (window 3), backed by a
    /// 512 KB shelf. Without it every XRAM page aliases the same 256 bytes.
    /// </summary>
    private sealed class XramWindowBus : IBusDevice
    {
        private const ushort Win3Lo = 0xBA21;
        private const ushort Win3Mi = 0xBA22;
        private const ushort Win3Hi = 0xBA23;
        private const ushort Win3Base = 0xBF00;

        private readonly byte[] _ram = new byte[0x10000];
        private readonly byte[] _xram = new byte[512 * 1024];

        public XramWindowBus(byte[] image, ushort loadAddr)
        {
            Array.Copy(image, 0, _ram, loadAddr, image.Length);
        }

        private int WindowOffset(ushort address)
            => ((_ram[Win3Hi] << 16) | (_ram[Win3Mi] << 8) | _ram[Win3Lo]) + (address - Win3Base);

        public byte Read(ushort address)
        {
            if (address >= Win3Base)
                return _xram[WindowOffset(address) % _xram.Length];
            return _ram[address];
        }

        public void Write(ushort address, byte value)
        {
            if (address >= Win3Base)
            {
                _xram[WindowOffset(address) % _xram.Length] = value;
                return;
            }
            _ram[address] = value;
        }
    }

    private static void AssertGoto(BusDevice bus, int slot, int line, string label)
    {
        Assert.AreEqual(1, bus.Read((ushort)(GotoResult + slot)),
            $"{label}: parse should report a valid line");
        int got = bus.Read((ushort)(GotoResult + slot + 1))
                | (bus.Read((ushort)(GotoResult + slot + 2)) << 8);
        Assert.AreEqual(line, got, $"{label}: 0-based line number");
    }

    private static byte[] Fixture() => File.ReadAllBytes(
        RepoPath("software", "tests", "integration", "fixtures", "editbuf_runtime.bin"));

    private static BusDevice Run(ushort entry)
    {
        var bus = new BusDevice(Fixture(), 0x4000);
        Run(bus, entry);
        return bus;
    }

    private static void Run(IBusDevice bus, ushort entry)
    {
        // LDX #$FF; TXS; JSR entry; JMP Done
        bus.Write(Harness, 0xA2);
        bus.Write(Harness + 1, 0xFF);
        bus.Write(Harness + 2, 0x9A);
        bus.Write(Harness + 3, 0x20);
        bus.Write(Harness + 4, (byte)(entry & 0xFF));
        bus.Write(Harness + 5, (byte)(entry >> 8));
        bus.Write(Harness + 6, 0x4C);
        bus.Write(Harness + 7, (byte)(Done & 0xFF));
        bus.Write(Harness + 8, (byte)(Done >> 8));

        var cpu = new Cpu(bus, E6502Type.Cmos);
        cpu.Boot(Harness);

        // The undo fixture renders the full screen after every one of its ~120
        // edits, so it needs a far higher ceiling than the other two.
        for (int i = 0; i < 400_000_000; i++)
        {
            ushort prevPc = cpu.Pc;
            cpu.ExecuteNext();
            if (prevPc == Done && cpu.Pc == Done)
                return;
        }

        Assert.Fail($"editbuf fixture did not return; PC=${cpu.Pc:X4}.");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}
