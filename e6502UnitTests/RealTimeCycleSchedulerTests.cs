using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RealTimeCycleSchedulerTests
{
    [TestMethod]
    public void TakeCycleBudget_FirstCallReturnsZero()
    {
        long ticks = 0;
        var scheduler = new RealTimeCycleScheduler(
            targetCyclesPerSecond: 12_000_000,
            maxBacklogCycles: 1_000_000,
            timestampProvider: () => ticks,
            timestampFrequency: 1_000_000);

        Assert.AreEqual(0, scheduler.TakeCycleBudget());
    }

    [TestMethod]
    public void TakeCycleBudget_ProducesExpectedCyclesFromElapsedTime()
    {
        long ticks = 0;
        var scheduler = new RealTimeCycleScheduler(
            targetCyclesPerSecond: 12_000_000,
            maxBacklogCycles: 1_000_000,
            timestampProvider: () => ticks,
            timestampFrequency: 1_000_000);

        _ = scheduler.TakeCycleBudget();
        ticks += 1_000; // 1 ms

        Assert.AreEqual(12_000, scheduler.TakeCycleBudget());
    }

    [TestMethod]
    public void TakeCycleBudget_CarriesFractionalCyclesAcrossCalls()
    {
        long ticks = 0;
        var scheduler = new RealTimeCycleScheduler(
            targetCyclesPerSecond: 3,
            maxBacklogCycles: 100,
            timestampProvider: () => ticks,
            timestampFrequency: 2);

        _ = scheduler.TakeCycleBudget();

        ticks += 1; // +0.5s => +1.5 cycles
        Assert.AreEqual(1, scheduler.TakeCycleBudget());

        ticks += 1; // +0.5s => +1.5 + prior 0.5 = 2 cycles
        Assert.AreEqual(2, scheduler.TakeCycleBudget());
    }

    [TestMethod]
    public void TakeCycleBudget_RespectsBacklogClamp()
    {
        long ticks = 0;
        var scheduler = new RealTimeCycleScheduler(
            targetCyclesPerSecond: 12_000_000,
            maxBacklogCycles: 10_000,
            timestampProvider: () => ticks,
            timestampFrequency: 1_000_000);

        _ = scheduler.TakeCycleBudget();
        ticks += 1_000_000; // 1 second => 12,000,000 cycles without clamp

        Assert.AreEqual(10_000, scheduler.TakeCycleBudget());
    }

    [TestMethod]
    public void TakeCycleBudget_RespectsMaxCyclesArgument()
    {
        long ticks = 0;
        var scheduler = new RealTimeCycleScheduler(
            targetCyclesPerSecond: 12_000_000,
            maxBacklogCycles: 1_000_000,
            timestampProvider: () => ticks,
            timestampFrequency: 1_000_000);

        _ = scheduler.TakeCycleBudget();
        ticks += 1_000; // 12,000 cycles pending

        Assert.AreEqual(5_000, scheduler.TakeCycleBudget(5_000));
        Assert.AreEqual(5_000, scheduler.TakeCycleBudget(5_000));
        Assert.AreEqual(2_000, scheduler.TakeCycleBudget(5_000));
    }
}
