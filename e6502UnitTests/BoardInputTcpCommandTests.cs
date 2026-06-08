using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Ipc;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// The board_input TCP command lets host tooling (nova CLI / MCP / integration
/// tests) inject joystick/switch state so JOY()/SW() can be driven without
/// physical buttons. The $BA9C/$BA9D registers are read-only to bus writes, so
/// the command sets the logical BoardButtonState/BoardSwitchState directly.
/// </summary>
[TestClass]
public class BoardInputTcpCommandTests
{
    private static EmulatorTcpServer NewServer(out CompositeBusDevice bus)
    {
        bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        var editor = new ScreenEditor(bus.Vgc);
        var debugger = new DebuggerService(cpu, bus);
        // null UI args + port 0: the ctor builds a listener but never Start()s it.
        return new EmulatorTcpServer(bus, editor, cpu, debugger, null, null, 0);
    }

    [TestMethod]
    public void BoardInputCommand_SetsButtonsAndSwitches()
    {
        using var server = NewServer(out var bus);
        string resp = server.ProcessRequest(
            """{"command":"board_input","buttons":10,"switches":5}""");
        StringAssert.Contains(resp, "\"ok\":true");
        Assert.AreEqual(10, bus.BoardButtonState);
        Assert.AreEqual(5, bus.BoardSwitchState);
    }

    [TestMethod]
    public void BoardInputCommand_ButtonsOnly_LeavesSwitchesUnchanged()
    {
        using var server = NewServer(out var bus);
        bus.BoardSwitchState = 0x0F;
        string resp = server.ProcessRequest(
            """{"command":"board_input","buttons":64}""");
        StringAssert.Contains(resp, "\"ok\":true");
        Assert.AreEqual(64, bus.BoardButtonState);
        Assert.AreEqual(0x0F, bus.BoardSwitchState);
    }

    [TestMethod]
    public void BoardInputCommand_NoFields_ReturnsError()
    {
        using var server = NewServer(out _);
        string resp = server.ProcessRequest("""{"command":"board_input"}""");
        StringAssert.Contains(resp, "\"ok\":false");
    }
}
