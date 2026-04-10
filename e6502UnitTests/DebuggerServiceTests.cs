using e6502.Avalonia.Debugging;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class DebuggerServiceTests
{
    private static (Cpu cpu, BusDevice bus, DebuggerService debugger) CreateSystem(byte[] program,
        ushort loadAddress = 0x0200)
    {
        var bus = new BusDevice(program, loadAddress);
        // Set reset vector to point at our program
        bus.Write(0xFFFC, (byte)(loadAddress & 0xFF));
        bus.Write(0xFFFD, (byte)(loadAddress >> 8));
        var cpu = new Cpu(bus);
        cpu.Boot();
        var debugger = new DebuggerService(cpu, bus);
        return (cpu, bus, debugger);
    }

    // ── RunCycles ──

    [TestMethod]
    public void RunCycles_ExecutesRequestedCycles()
    {
        // NOP sled — each NOP is 2 cycles
        var program = new byte[20];
        for (int i = 0; i < 20; i++) program[i] = 0xEA; // NOP

        var (cpu, _, dbg) = CreateSystem(program);
        dbg.Pause();

        var (state, executed) = dbg.RunCycles(10);

        // 10 cycles = 5 NOPs (2 cycles each)
        Assert.AreEqual(10, executed);
        Assert.AreEqual((ushort)0x0205, state.Pc);
    }

    [TestMethod]
    public void RunCycles_MayOvershootByOneInstruction()
    {
        var program = new byte[20];
        for (int i = 0; i < 20; i++) program[i] = 0xEA; // NOP (2 cycles)

        var (_, _, dbg) = CreateSystem(program);
        dbg.Pause();

        // Request 3 cycles — must complete the 2nd NOP, giving 4 cycles
        var (_, executed) = dbg.RunCycles(3);

        Assert.IsTrue(executed >= 3, $"Should execute at least 3 cycles, got {executed}");
        Assert.IsTrue(executed <= 4, $"Should overshoot by at most one instruction, got {executed}");
    }

    [TestMethod]
    public void RunCycles_PausesAutomaticallyIfNotPaused()
    {
        var program = new byte[20];
        for (int i = 0; i < 20; i++) program[i] = 0xEA;

        var (_, _, dbg) = CreateSystem(program);
        // Don't pause — RunCycles should handle it

        var (_, executed) = dbg.RunCycles(4);

        Assert.AreEqual(4, executed);
    }

    [TestMethod]
    public void RunCycles_ReturnsCorrectAccumulator()
    {
        // LDA #$42 (2 bytes, 2 cycles) + NOP
        var program = new byte[] { 0xA9, 0x42, 0xEA };

        var (_, _, dbg) = CreateSystem(program);
        dbg.Pause();

        var (state, _) = dbg.RunCycles(2);

        Assert.AreEqual(0x42, state.A);
    }

    [TestMethod]
    public void RunCycles_ExecutesMultiCycleInstructions()
    {
        // LDA #$AA (2 cycles) + STA $1234 (4 cycles) + NOP
        var program = new byte[] { 0xA9, 0xAA, 0x8D, 0x34, 0x12, 0xEA };

        var (_, bus, dbg) = CreateSystem(program);
        dbg.Pause();

        var (_, executed) = dbg.RunCycles(6);

        Assert.AreEqual(6, executed);
        Assert.AreEqual(0xAA, bus.Read(0x1234));
    }

    // ── WatchMemory ──

    [TestMethod]
    public void WatchMemory_ImmediateMatch_ReturnsTrue()
    {
        var program = new byte[] { 0xEA }; // NOP
        var (_, bus, dbg) = CreateSystem(program);
        bus.Write(0x2000, 0x42);
        dbg.Pause();

        var (matched, value) = dbg.WatchMemory(0x2000, 0x42, 1000);

        Assert.IsTrue(matched);
        Assert.AreEqual(0x42, value);
    }

    [TestMethod]
    public void WatchMemory_NoMatch_TimesOut()
    {
        var program = new byte[] { 0xEA }; // NOP
        var (_, bus, dbg) = CreateSystem(program);
        bus.Write(0x2000, 0x00);
        dbg.Pause();

        var (matched, value) = dbg.WatchMemory(0x2000, 0xFF, 100);

        Assert.IsFalse(matched);
        Assert.AreEqual(0x00, value);
    }

    [TestMethod]
    public void WatchMemory_ReturnsFinalValue()
    {
        var program = new byte[] { 0xEA };
        var (_, bus, dbg) = CreateSystem(program);
        bus.Write(0x3000, 0xBB);
        dbg.Pause();

        var (_, value) = dbg.WatchMemory(0x3000, 0xCC, 50);

        Assert.AreEqual(0xBB, value);
    }

    // ── Pause / Resume ──

    [TestMethod]
    public void Pause_SetsIsPaused()
    {
        var program = new byte[] { 0xEA };
        var (_, _, dbg) = CreateSystem(program);
        dbg.Pause();
        Assert.IsTrue(dbg.IsPaused);
    }

    [TestMethod]
    public void Resume_ClearsIsPaused()
    {
        var program = new byte[] { 0xEA };
        var (_, _, dbg) = CreateSystem(program);
        dbg.Pause();
        dbg.Resume();
        Assert.IsFalse(dbg.IsPaused);
    }
}
