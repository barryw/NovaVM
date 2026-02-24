using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaTimerTests
{
    [TestMethod]
    public void Timer_OwnsAddress_InRange()
    {
        var timer = new VirtualTimerController();
        Assert.IsTrue(timer.OwnsAddress((ushort)VgcConstants.TimerBase));
        Assert.IsTrue(timer.OwnsAddress((ushort)VgcConstants.TimerEnd));
        Assert.IsFalse(timer.OwnsAddress((ushort)(VgcConstants.TimerBase - 1)));
    }

    [TestMethod]
    public void Timer_WriteRegister_ReadBack()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x67);
        Assert.AreEqual(0x67, timer.Read((ushort)VgcConstants.TimerDivL));
    }

    [TestMethod]
    public void Timer_Disabled_NeverFires()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x10);
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        for (int i = 0; i < 100; i++) timer.Tick();
        Assert.IsFalse(timer.IrqPending);
    }

    [TestMethod]
    public void Timer_Enabled_FiresAfterDivisorTicks()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x0A);  // divisor = 10
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01);
        for (int i = 0; i < 9; i++) timer.Tick();
        Assert.IsFalse(timer.IrqPending);
        timer.Tick();
        Assert.IsTrue(timer.IrqPending);
    }

    [TestMethod]
    public void Timer_Acknowledge_ClearsPending()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x01);
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01);
        timer.Tick();
        Assert.IsTrue(timer.IrqPending);
        timer.Read((ushort)VgcConstants.TimerStatus);
        Assert.IsFalse(timer.IrqPending);
    }

    [TestMethod]
    public void Timer_AdvanceCycles_FiresUsingQuantum()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x02); // 2 timer ticks
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01);

        timer.AdvanceCycles(VgcConstants.TimerTickQuantumCycles);
        Assert.IsFalse(timer.IrqPending);

        timer.AdvanceCycles(VgcConstants.TimerTickQuantumCycles);
        Assert.IsTrue(timer.IrqPending);
    }

    [TestMethod]
    public void Timer_Disable_ResetsAccumulators()
    {
        var timer = new VirtualTimerController();
        timer.Write((ushort)VgcConstants.TimerDivL, 0x02);
        timer.Write((ushort)VgcConstants.TimerDivH, 0x00);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01);

        timer.AdvanceCycles(VgcConstants.TimerTickQuantumCycles);
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x00); // disable clears state
        timer.Write((ushort)VgcConstants.TimerCtrl, 0x01); // re-enable

        timer.AdvanceCycles(VgcConstants.TimerTickQuantumCycles);
        Assert.IsFalse(timer.IrqPending);
    }
}
