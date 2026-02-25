using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class CompilerBusRoutingTests
{
    [TestMethod]
    public void ReadCmpStatus_RoutesToCompilerController()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        byte status = bus.Read(VgcConstants.CmpStatus);
        Assert.AreEqual(VgcConstants.CmpStatusIdle, status);
    }

    [TestMethod]
    public void WriteCmpCmd_RoutesToCompilerController()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Write(VgcConstants.CmpCmd, 0xFF); // should not throw
    }

    [TestMethod]
    public void CompilerProperty_NotNull()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        Assert.IsNotNull(bus.Compiler);
    }
}
