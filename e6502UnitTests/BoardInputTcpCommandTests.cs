using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Ipc;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Text.Json;

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
    private static EmulatorTcpServer NewServer(out CompositeBusDevice bus) =>
        NewServer(out bus, out _);

    private static EmulatorTcpServer NewServer(out CompositeBusDevice bus, out ScreenEditor editor)
    {
        bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        editor = new ScreenEditor(bus.Vgc);
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

    [DataTestMethod]
    [DataRow("LEFT", 0x1C)]
    [DataRow("RIGHT", 0x1D)]
    [DataRow("UP", 0x1E)]
    [DataRow("DOWN", 0x1F)]
    [DataRow("HOME", 0x02)]
    [DataRow("END", 0x05)]
    [DataRow("PGUP", 0x10)]
    [DataRow("PAGEDOWN", 0x12)]
    [DataRow("CTRL-HOME", 0x80)]
    [DataRow("CTRL-END", 0x81)]
    [DataRow("DELETE", 0x7F)]
    [DataRow("ESCAPE", 0x1B)]
    [DataRow("TAB", 0x09)]
    [DataRow("SPACE", 0x20)]
    [DataRow("SCREEN-HOME", 0x13)]
    public void SendKeyCommand_MapsNamedKeysToEditorInputBytes(string key, int expected)
    {
        using var server = NewServer(out _, out var editor);
        string resp = SendKey(server, key);

        StringAssert.Contains(resp, "\"ok\":true");
        Assert.AreEqual((byte)expected, editor.DequeueInput());
        Assert.IsFalse(editor.HasQueuedInput);
    }

    [DataTestMethod]
    [DataRow("CTRL-A", 0x01)]
    [DataRow("CTRL-G", 0x07)]
    [DataRow("CTRL-K", 0x0B)]
    [DataRow("CONTROL-Q", 0x11)]
    [DataRow("CTRL-X", 0x18)]
    public void SendKeyCommand_MapsControlLetterNamesToAsciiControlBytes(string key, int expected)
    {
        using var server = NewServer(out _, out var editor);
        string resp = SendKey(server, key);

        StringAssert.Contains(resp, "\"ok\":true");
        Assert.AreEqual((byte)expected, editor.DequeueInput());
        Assert.IsFalse(editor.HasQueuedInput);
    }

    [TestMethod]
    public void SendKeyCommand_AltLetterQueuesEscapeThenLowercaseLetter()
    {
        using var server = NewServer(out _, out var editor);
        string resp = SendKey(server, "ALT-X");

        StringAssert.Contains(resp, "\"ok\":true");
        Assert.AreEqual((byte)0x1B, editor.DequeueInput());
        Assert.AreEqual((byte)'x', editor.DequeueInput());
        Assert.IsFalse(editor.HasQueuedInput);
    }

    private static string SendKey(EmulatorTcpServer server, string key)
    {
        string jsonKey = JsonSerializer.Serialize(key);
        return server.ProcessRequest($$"""{"command":"send_key","key":{{jsonKey}}}""");
    }
}
