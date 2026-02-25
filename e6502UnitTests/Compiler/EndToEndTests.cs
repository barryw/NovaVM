using KDS.e6502;
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class EndToEndTests
{
    // ── Harness ───────────────────────────────────────────────────────────────

    private const ushort LoadAddr = 0x0280;
    private const int    MaxCycles = 50_000;

    /// <summary>
    /// Compiles NCC source, loads it into a fresh BusDevice, runs the CPU
    /// until it hits the BRK-halt at $FF00 or exhausts the cycle budget,
    /// and returns cpu + bus + compile result.
    /// </summary>
    private static (Cpu cpu, BusDevice bus, CompileResult result)
        CompileAndRun(string source, int maxCycles = MaxCycles)
    {
        var compiler = new NccCompiler { BaseAddress = LoadAddr };
        var result   = compiler.Compile(source);
        Assert.IsTrue(result.Success, string.Join("\n", result.Errors));

        // 64 KB RAM, all zeroes.
        var bus = new BusDevice(new byte[65536]);

        // Load compiled code at LoadAddr.
        byte[] code = result.Code;
        for (int i = 0; i < code.Length; i++)
            bus.Write((ushort)(LoadAddr + i), code[i]);

        // Reset vector → LoadAddr (entry trampoline).
        bus.Write(0xFFFC, (byte)(LoadAddr & 0xFF));
        bus.Write(0xFFFD, (byte)(LoadAddr >> 8));

        // BRK vector → $FF00.  Put a BRK there so the CPU halts.
        bus.Write(0xFFFE, 0x00);
        bus.Write(0xFFFF, 0xFF);
        bus.Write(0xFF00, 0x00);  // BRK — halts execution

        var cpu = new Cpu(bus, E6502Type.Nmos);
        cpu.Boot();

        for (int i = 0; i < maxCycles; i++)
        {
            cpu.ExecuteNext();
            if (cpu.Pc == 0xFF00) break;
        }

        return (cpu, bus, result);
    }

    // ── Test 1: SimpleByteAssignment ─────────────────────────────────────────

    [TestMethod]
    public void SimpleByteAssignment()
    {
        const string src = "byte x; void main() { x = 42; }";
        var (_, bus, result) = CompileAndRun(src);
        ushort xAddr = result.Symbols["x"].Address;
        Assert.AreEqual(42, bus.Read(xAddr));
    }

    // ── Test 2: ByteAddition ─────────────────────────────────────────────────

    [TestMethod]
    public void ByteAddition()
    {
        const string src = "byte x; void main() { x = 10 + 32; }";
        var (_, bus, result) = CompileAndRun(src);
        ushort xAddr = result.Symbols["x"].Address;
        Assert.AreEqual(42, bus.Read(xAddr));
    }

    // ── Test 3: ByteSubtraction ──────────────────────────────────────────────

    [TestMethod]
    public void ByteSubtraction()
    {
        const string src = "byte x; void main() { x = 50 - 8; }";
        var (_, bus, result) = CompileAndRun(src);
        ushort xAddr = result.Symbols["x"].Address;
        Assert.AreEqual(42, bus.Read(xAddr));
    }

    // ── Test 4: GlobalByteReadWrite ──────────────────────────────────────────

    [TestMethod]
    public void GlobalByteReadWrite()
    {
        const string src = "byte a; byte b; void main() { a = 10; b = a; }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(10, bus.Read(result.Symbols["a"].Address));
        Assert.AreEqual(10, bus.Read(result.Symbols["b"].Address));
    }

    // ── Test 5: IntAssignment ────────────────────────────────────────────────

    [TestMethod]
    public void IntAssignment()
    {
        const string src = "int x; void main() { x = 1000; }";
        var (_, bus, result) = CompileAndRun(src);
        ushort xAddr = result.Symbols["x"].Address;
        int value = bus.Read(xAddr) | (bus.Read((ushort)(xAddr + 1)) << 8);
        Assert.AreEqual(1000, value);
    }

    // ── Test 6: IntAddition ──────────────────────────────────────────────────

    [TestMethod]
    public void IntAddition()
    {
        const string src = "int x; void main() { x = 500 + 500; }";
        var (_, bus, result) = CompileAndRun(src);
        ushort xAddr = result.Symbols["x"].Address;
        int value = bus.Read(xAddr) | (bus.Read((ushort)(xAddr + 1)) << 8);
        Assert.AreEqual(1000, value);
    }

    // ── Test 7: FunctionCall ─────────────────────────────────────────────────

    [TestMethod]
    public void FunctionCall()
    {
        const string src = @"
byte result;
void set() { result = 99; }
void main() { set(); }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(99, bus.Read(result.Symbols["result"].Address));
    }

    // ── Test 8: FunctionWithReturn ───────────────────────────────────────────

    [TestMethod]
    public void FunctionWithReturn()
    {
        const string src = @"
byte result;
byte get42() { return 42; }
void main() { result = get42(); }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(42, bus.Read(result.Symbols["result"].Address));
    }

    // ── Test 9: IfTrue ───────────────────────────────────────────────────────

    [TestMethod]
    public void IfTrue()
    {
        const string src = "byte x; void main() { x = 0; if (1) { x = 1; } }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(1, bus.Read(result.Symbols["x"].Address));
    }

    // ── Test 10: IfFalse ─────────────────────────────────────────────────────

    [TestMethod]
    public void IfFalse()
    {
        const string src = "byte x; void main() { x = 1; if (0) { x = 0; } }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(1, bus.Read(result.Symbols["x"].Address));
    }

    // ── Test 11: WhileLoop ───────────────────────────────────────────────────

    [TestMethod]
    public void WhileLoop()
    {
        // x counts up to 5.
        const string src = @"
byte x;
void main() {
    x = 0;
    while (x != 5) { x = x + 1; }
}";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(5, bus.Read(result.Symbols["x"].Address));
    }

    // ── Test 12: ForLoop ─────────────────────────────────────────────────────

    [TestMethod]
    public void ForLoop()
    {
        // Uses a local loop variable i; x accumulates.
        const string src = @"
byte x;
void main() {
    byte i;
    x = 0;
    for (i = 0; i < 10; i = i + 1) { x = x + 1; }
}";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(10, bus.Read(result.Symbols["x"].Address));
    }

    // ── Test 13: NestedCalls ─────────────────────────────────────────────────

    [TestMethod]
    public void NestedCalls()
    {
        const string src = @"
byte x;
void add1() { x = x + 1; }
void main() { x = 0; add1(); add1(); add1(); }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(3, bus.Read(result.Symbols["x"].Address));
    }

    // ── Test 14: ByteComparison ──────────────────────────────────────────────

    [TestMethod]
    public void ByteComparison()
    {
        const string src = @"
byte x;
void main() { if (5 > 3) { x = 1; } else { x = 0; } }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(1, bus.Read(result.Symbols["x"].Address));
    }

    // ── Test 15: FunctionWithByteArg ─────────────────────────────────────────

    [TestMethod]
    public void FunctionWithByteArg()
    {
        const string src = @"
byte result;
void store(byte val) { result = val; }
void main() { store(77); }";
        var (_, bus, result) = CompileAndRun(src);
        Assert.AreEqual(77, bus.Read(result.Symbols["result"].Address));
    }

    // ── Test 16: IntComparison_HighBytesDiffer ───────────────────────────────

    [TestMethod]
    public void IntComparison_HighBytesDiffer()
    {
        // 256 > 1 should be true (tests high-byte comparison)
        var source = "byte result; void main() { int a; int b; a = 256; b = 1; if (a > b) { result = 1; } else { result = 0; } }";
        var (cpu, bus, result) = CompileAndRun(source);
        Assert.AreEqual(1, bus.Read(result.Symbols["result"].Address));
    }

    // ── Test 17: IntComparison_Equal ─────────────────────────────────────────

    [TestMethod]
    public void IntComparison_Equal()
    {
        var source = "byte result; void main() { int a; int b; a = 1000; b = 1000; if (a == b) { result = 1; } else { result = 0; } }";
        var (cpu, bus, result) = CompileAndRun(source);
        Assert.AreEqual(1, bus.Read(result.Symbols["result"].Address));
    }

    // ── Test 18: LineMap_HasEntries ──────────────────────────────────────────

    [TestMethod]
    public void LineMap_HasEntries()
    {
        const string src = @"
byte x;
void main() {
    x = 1;
    x = x + 1;
}";
        var compiler = new NccCompiler { BaseAddress = LoadAddr };
        var result = compiler.Compile(src);
        Assert.IsTrue(result.Success, string.Join("\n", result.Errors));
        Assert.IsNotNull(result.LineMap);
        Assert.IsTrue(result.LineMap.Count > 0, "LineMap should have entries");
    }

    // ── Test 19: LineMap_MapsDistinctLines ───────────────────────────────────

    [TestMethod]
    public void LineMap_MapsDistinctLines()
    {
        // Line numbers are 0-based in the map (source line 3 → index 2)
        const string src = @"byte x;
byte y;
void main() {
    x = 10;
    y = 20;
}";
        var compiler = new NccCompiler { BaseAddress = LoadAddr };
        var result = compiler.Compile(src);
        Assert.IsTrue(result.Success, string.Join("\n", result.Errors));
        Assert.IsNotNull(result.LineMap);

        // Lines "x = 10;" and "y = 20;" should map to different addresses
        // (0-based line 3 = "x = 10;", line 4 = "y = 20;")
        Assert.IsTrue(result.LineMap.ContainsKey(3), "Line 3 (x=10) should be in map");
        Assert.IsTrue(result.LineMap.ContainsKey(4), "Line 4 (y=20) should be in map");
        Assert.AreNotEqual(result.LineMap[3], result.LineMap[4],
            "Different source lines should map to different addresses");
    }
}
