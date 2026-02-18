using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using KDS.e6502;

namespace e6502.TUI.Ipc;

public sealed class EmulatorTcpServer : IDisposable
{
    private readonly CompositeBusDevice _bus;
    private readonly ScreenEditor _editor;
    private readonly Cpu _cpu;
    private TcpListener _listener;
    private readonly CancellationTokenSource _cts = new();
    private Task? _acceptTask;

    public int Port { get; private set; }

    public EmulatorTcpServer(CompositeBusDevice bus, ScreenEditor editor, Cpu cpu, int port = 6502)
    {
        _bus = bus;
        _editor = editor;
        _cpu = cpu;
        Port = port;
        _listener = new TcpListener(IPAddress.Loopback, port);
    }

    public void Start()
    {
        try
        {
            _listener.Start();
        }
        catch (SocketException)
        {
            // Port busy — bind to port 0 to get an OS-assigned free port
            _listener = new TcpListener(IPAddress.Loopback, 0);
            _listener.Start();
            Port = ((IPEndPoint)_listener.LocalEndpoint).Port;
        }
        Console.Error.WriteLine($"Emulator TCP server listening on port {Port}");
        _acceptTask = AcceptLoop(_cts.Token);
    }

    private async Task AcceptLoop(CancellationToken ct)
    {
        try
        {
            while (!ct.IsCancellationRequested)
            {
                var client = await _listener.AcceptTcpClientAsync(ct);
                _ = HandleClient(client, ct);
            }
        }
        catch (OperationCanceledException) { }
        catch (ObjectDisposedException) { }
    }

    private async Task HandleClient(TcpClient client, CancellationToken ct)
    {
        try
        {
            using (client)
            await using (var stream = client.GetStream())
            using (var reader = new StreamReader(stream, Encoding.UTF8))
            await using (var writer = new StreamWriter(stream, Encoding.UTF8) { AutoFlush = true })
            {
                while (!ct.IsCancellationRequested)
                {
                    var line = await reader.ReadLineAsync(ct);
                    if (line is null) break;

                    var response = ProcessRequest(line);
                    await writer.WriteLineAsync(response);
                }
            }
        }
        catch (OperationCanceledException) { }
        catch (IOException) { }
    }

    private string ProcessRequest(string json)
    {
        try
        {
            var req = JsonNode.Parse(json);
            if (req is null) return Error("Invalid JSON");

            string? cmd = req["command"]?.GetValue<string>();
            if (cmd is null) return Error("Missing 'command'");

            return cmd switch
            {
                "type_text" => CmdTypeText(req),
                "send_key" => CmdSendKey(req),
                "read_screen" => CmdReadScreen(),
                "read_line" => CmdReadLine(req),
                "get_cursor" => CmdGetCursor(),
                "wait_ready" => CmdWaitReady(req),
                "cold_start" => CmdColdStart(),
                "warm_start" => CmdWarmStart(),
                "peek" => CmdPeek(req),
                "poke" => CmdPoke(req),
                "read_graphics" => CmdReadGraphics(),
                "read_sprites" => CmdReadSprites(),
                "save_program" => CmdSaveProgram(req),
                "load_program" => CmdLoadProgram(req),
                "list_programs" => CmdListPrograms(),
                _ => Error($"Unknown command: {cmd}")
            };
        }
        catch (Exception ex)
        {
            return Error(ex.Message);
        }
    }

    private string CmdTypeText(JsonNode req)
    {
        string? text = req["text"]?.GetValue<string>();
        if (text is null) return Error("Missing 'text'");

        int delayMs = req["delay_ms"]?.GetValue<int>() ?? 2;

        foreach (char c in text)
        {
            _editor.QueueInput((byte)c);
            if (delayMs > 0)
                Thread.Sleep(delayMs);
        }

        return Ok();
    }

    private string CmdSendKey(JsonNode req)
    {
        string? key = req["key"]?.GetValue<string>();
        if (key is null) return Error("Missing 'key'");

        byte code = key.ToUpperInvariant() switch
        {
            "ENTER" or "CR" or "RETURN" => 0x0D,
            "BACKSPACE" or "BS" => 0x08,
            "CTRL-C" or "BREAK" => 0x03,
            "HOME" => 0x13,
            "CLEAR" or "FF" => 0x0C,
            "LF" => 0x0A,
            _ => (byte)(key.Length == 1 ? key[0] : 0)
        };

        if (code == 0) return Error($"Unknown key: {key}");

        _editor.QueueInput(code);
        return Ok();
    }

    private string CmdReadScreen()
    {
        var vgc = _bus.Vgc;
        var lines = new string[VgcConstants.ScreenRows];

        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            var sb = new StringBuilder(VgcConstants.ScreenCols);
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte b = vgc.GetScreenChar(col, row);
                sb.Append(b >= 0x20 ? (char)b : ' ');
            }
            lines[row] = sb.ToString().TrimEnd();
        }

        int cx = vgc.GetCursorX();
        int cy = vgc.GetCursorY();

        var result = new JsonObject
        {
            ["ok"] = true,
            ["lines"] = new JsonArray(lines.Select(l => JsonValue.Create(l)).ToArray()),
            ["cursor_x"] = cx,
            ["cursor_y"] = cy
        };
        return result.ToJsonString();
    }

    private string CmdReadLine(JsonNode req)
    {
        int? row = req["row"]?.GetValue<int>();
        var vgc = _bus.Vgc;

        int targetRow = row ?? vgc.GetCursorY();
        if (targetRow < 0 || targetRow >= VgcConstants.ScreenRows)
            return Error("Row out of range");

        var sb = new StringBuilder(VgcConstants.ScreenCols);
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            byte b = vgc.GetScreenChar(col, targetRow);
            sb.Append(b >= 0x20 ? (char)b : ' ');
        }

        var result = new JsonObject
        {
            ["ok"] = true,
            ["row"] = targetRow,
            ["text"] = sb.ToString().TrimEnd()
        };
        return result.ToJsonString();
    }

    private string CmdGetCursor()
    {
        var vgc = _bus.Vgc;
        var result = new JsonObject
        {
            ["ok"] = true,
            ["x"] = vgc.GetCursorX(),
            ["y"] = vgc.GetCursorY()
        };
        return result.ToJsonString();
    }

    private string CmdWaitReady(JsonNode req)
    {
        string? text = req["text"]?.GetValue<string>();
        if (text is null) text = "Ready";

        int timeoutMs = req["timeout_ms"]?.GetValue<int>() ?? 5000;
        int pollMs = req["poll_ms"]?.GetValue<int>() ?? 50;

        var vgc = _bus.Vgc;
        var deadline = DateTime.UtcNow.AddMilliseconds(timeoutMs);

        while (DateTime.UtcNow < deadline)
        {
            for (int row = 0; row < VgcConstants.ScreenRows; row++)
            {
                var sb = new StringBuilder(VgcConstants.ScreenCols);
                for (int col = 0; col < VgcConstants.ScreenCols; col++)
                {
                    byte b = vgc.GetScreenChar(col, row);
                    sb.Append(b >= 0x20 ? (char)b : ' ');
                }

                if (sb.ToString().Contains(text, StringComparison.OrdinalIgnoreCase))
                {
                    var result = new JsonObject
                    {
                        ["ok"] = true,
                        ["found"] = true,
                        ["row"] = row
                    };
                    return result.ToJsonString();
                }
            }

            Thread.Sleep(pollMs);
        }

        var timeout = new JsonObject
        {
            ["ok"] = true,
            ["found"] = false
        };
        return timeout.ToJsonString();
    }

    private string CmdColdStart()
    {
        _bus.Vgc.Reset();
        _cpu.Boot(0xC000);
        return Ok();
    }

    private string CmdWarmStart()
    {
        _cpu.Boot(0x0000);
        return Ok();
    }

    private string CmdPeek(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        if (address is null) return Error("Missing 'address'");

        byte value = _bus.Read((ushort)address.Value);
        var result = new JsonObject
        {
            ["ok"] = true,
            ["address"] = address.Value,
            ["value"] = value
        };
        return result.ToJsonString();
    }

    private string CmdPoke(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        int? value = req["value"]?.GetValue<int>();
        if (address is null) return Error("Missing 'address'");
        if (value is null) return Error("Missing 'value'");

        _bus.Write((ushort)address.Value, (byte)value.Value);
        return Ok();
    }

    private string CmdReadGraphics()
    {
        var vgc = _bus.Vgc;

        // Render the 160x50 gfx bitmap as 80x25 text using block characters.
        // Each terminal cell = 2x2 gfx pixels. Use '#' for both set, top-half
        // '^' for top only, bottom-half 'v' for bottom only, '.' for empty.
        var rows = new string[VgcConstants.ScreenRows];
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            var sb = new StringBuilder(VgcConstants.ScreenCols);
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                int gx = col * 2;
                int gyTop = row * 2;
                int gyBottom = row * 2 + 1;
                byte top = vgc.GetGfxPixelColor(gx, gyTop);
                byte bot = vgc.GetGfxPixelColor(gx, gyBottom);
                if (top != 0 && bot != 0) sb.Append('#');
                else if (top != 0) sb.Append('^');
                else if (bot != 0) sb.Append('v');
                else sb.Append('.');
            }
            rows[row] = sb.ToString().TrimEnd('.');
        }

        var result = new JsonObject
        {
            ["ok"] = true,
            ["mode"] = vgc.GetMode(),
            ["rows"] = new JsonArray(rows.Select(r => JsonValue.Create(r)).ToArray())
        };
        return result.ToJsonString();
    }

    private string CmdReadSprites()
    {
        var vgc = _bus.Vgc;
        var sprites = new JsonArray();

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var (x, y, color, enabled, shapeIdx, flags, priority) = vgc.GetSpriteState(i);
            if (!enabled) continue;

            // Read the shape data as a 16-row bitmap
            var shape = vgc.GetSpriteShape(shapeIdx);
            var shapeRows = new JsonArray();
            for (int r = 0; r < 16; r++)
            {
                int b1 = shape[r * 2];
                int b2 = shape[r * 2 + 1];
                int word = (b1 << 8) | b2;
                var sb = new StringBuilder(16);
                for (int bit = 15; bit >= 0; bit--)
                    sb.Append((word & (1 << bit)) != 0 ? '#' : '.');
                shapeRows.Add(JsonValue.Create(sb.ToString()));
            }

            sprites.Add(new JsonObject
            {
                ["index"] = i,
                ["x"] = x,
                ["y"] = y,
                ["color"] = color,
                ["shape_index"] = shapeIdx,
                ["shape"] = shapeRows
            });
        }

        var result = new JsonObject
        {
            ["ok"] = true,
            ["sprites"] = sprites
        };
        return result.ToJsonString();
    }

    private string CmdSaveProgram(JsonNode req)
    {
        string? name = req["name"]?.GetValue<string>();
        if (name is null) return Error("Missing 'name'");

        // Type SAVE "name" + Enter into the emulator
        string cmd = $"SAVE \"{name}\"";
        foreach (char c in cmd)
        {
            _editor.QueueInput((byte)c);
            Thread.Sleep(2);
        }
        _editor.QueueInput(0x0D);

        // Wait for Ready prompt
        Thread.Sleep(500);
        var vgc = _bus.Vgc;
        var deadline = DateTime.UtcNow.AddMilliseconds(5000);
        while (DateTime.UtcNow < deadline)
        {
            for (int row = 0; row < VgcConstants.ScreenRows; row++)
            {
                var sb = new StringBuilder(VgcConstants.ScreenCols);
                for (int col = 0; col < VgcConstants.ScreenCols; col++)
                {
                    byte b = vgc.GetScreenChar(col, row);
                    sb.Append(b >= 0x20 ? (char)b : ' ');
                }
                if (sb.ToString().Contains("Ready", StringComparison.OrdinalIgnoreCase))
                {
                    var result = new JsonObject
                    {
                        ["ok"] = true,
                        ["saved"] = name,
                        ["directory"] = _bus.Fio.SaveDirectory
                    };
                    return result.ToJsonString();
                }
            }
            Thread.Sleep(50);
        }

        return Error("Timeout waiting for save to complete");
    }

    private string CmdLoadProgram(JsonNode req)
    {
        string? name = req["name"]?.GetValue<string>();
        if (name is null) return Error("Missing 'name'");

        // Type LOAD "name" + Enter into the emulator
        string cmd = $"LOAD \"{name}\"";
        foreach (char c in cmd)
        {
            _editor.QueueInput((byte)c);
            Thread.Sleep(2);
        }
        _editor.QueueInput(0x0D);

        // Wait for Ready prompt
        Thread.Sleep(500);
        var vgc = _bus.Vgc;
        var deadline = DateTime.UtcNow.AddMilliseconds(5000);
        while (DateTime.UtcNow < deadline)
        {
            for (int row = 0; row < VgcConstants.ScreenRows; row++)
            {
                var sb = new StringBuilder(VgcConstants.ScreenCols);
                for (int col = 0; col < VgcConstants.ScreenCols; col++)
                {
                    byte b = vgc.GetScreenChar(col, row);
                    sb.Append(b >= 0x20 ? (char)b : ' ');
                }
                if (sb.ToString().Contains("Ready", StringComparison.OrdinalIgnoreCase))
                {
                    var result = new JsonObject
                    {
                        ["ok"] = true,
                        ["loaded"] = name
                    };
                    return result.ToJsonString();
                }
            }
            Thread.Sleep(50);
        }

        return Error("Timeout waiting for load to complete");
    }

    private string CmdListPrograms()
    {
        string dir = _bus.Fio.SaveDirectory;
        if (!Directory.Exists(dir))
        {
            var empty = new JsonObject
            {
                ["ok"] = true,
                ["directory"] = dir,
                ["programs"] = new JsonArray()
            };
            return empty.ToJsonString();
        }

        var files = Directory.GetFiles(dir, "*.bas")
            .Select(Path.GetFileNameWithoutExtension)
            .OrderBy(n => n)
            .ToArray();

        var result = new JsonObject
        {
            ["ok"] = true,
            ["directory"] = dir,
            ["programs"] = new JsonArray(files.Select(f => JsonValue.Create(f)).ToArray())
        };
        return result.ToJsonString();
    }

    private static string Ok() => """{"ok":true}""";

    private static string Error(string message)
    {
        var result = new JsonObject
        {
            ["ok"] = false,
            ["error"] = message
        };
        return result.ToJsonString();
    }

    public void Dispose()
    {
        _cts.Cancel();
        _listener.Stop();
        _cts.Dispose();
    }
}
