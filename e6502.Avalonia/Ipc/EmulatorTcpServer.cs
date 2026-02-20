using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;

namespace e6502.Avalonia.Ipc;

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
                // Graphics commands
                "gfx_plot" => CmdGfxPlot(req),
                "gfx_line" => CmdGfxLine(req),
                "gfx_circle" => CmdGfxCircle(req),
                "gfx_rect" => CmdGfxRect(req),
                "gfx_fill" => CmdGfxFill(req),
                "gfx_clear" => CmdGfxClear(),
                "gfx_color" => CmdGfxColor(req),
                "gfx_paint" => CmdGfxPaint(req),
                // Sprite commands
                "sprite_define_row" => CmdSpriteDefineRow(req),
                "sprite_set_pixel" => CmdSpriteSetPixel(req),
                "sprite_position" => CmdSpritePosition(req),
                "sprite_enable" => CmdSpriteEnable(req),
                "sprite_disable" => CmdSpriteDisable(req),
                "sprite_clear" => CmdSpriteClear(req),
                "sprite_priority" => CmdSpritePriority(req),
                "sprite_flip" => CmdSpriteFlip(req),
                "sprite_copy" => CmdSpriteCopy(req),
                // SID commands
                "sid_play" => CmdSidPlay(req),
                "sid_stop" => CmdSidStop(),
                "sid_info" => CmdSidInfo(req),
                _ => Error($"Unknown command: {cmd}")
            };
        }
        catch (Exception ex)
        {
            return Error(ex.Message);
        }
    }

    // ── Existing commands ────────────────────────────────────────────────

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
        var rows = new string[VgcConstants.ScreenRows];
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            var sb = new StringBuilder(VgcConstants.ScreenCols);
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                // Sample 4x8 block of gfx pixels per ASCII char (320/80=4, 200/25=8)
                int gxBase = col * 4;
                int gyBase = row * 8;
                bool topHalf = false;
                bool botHalf = false;
                for (int dy = 0; dy < 4 && !topHalf; dy++)
                    for (int dx = 0; dx < 4 && !topHalf; dx++)
                        if (vgc.GetGfxPixelColor(gxBase + dx, gyBase + dy) != 0)
                            topHalf = true;
                for (int dy = 4; dy < 8 && !botHalf; dy++)
                    for (int dx = 0; dx < 4 && !botHalf; dx++)
                        if (vgc.GetGfxPixelColor(gxBase + dx, gyBase + dy) != 0)
                            botHalf = true;
                if (topHalf && botHalf) sb.Append('#');
                else if (topHalf) sb.Append('^');
                else if (botHalf) sb.Append('v');
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
            var state = vgc.GetSpriteState(i);
            if (!state.enabled) continue;

            var shape = vgc.GetSpriteShape(i);
            var shapeRows = new JsonArray();
            for (int r = 0; r < 16; r++)
            {
                var sb = new StringBuilder(16);
                for (int c = 0; c < 16; c++)
                {
                    int byteIdx = r * VgcConstants.SpriteBytesPerRow + c / 2;
                    byte color = (c % 2 == 0)
                        ? (byte)((shape[byteIdx] >> 4) & 0x0F)
                        : (byte)(shape[byteIdx] & 0x0F);
                    sb.Append(color.ToString("X1"));
                }
                shapeRows.Add(JsonValue.Create(sb.ToString()));
            }

            sprites.Add(new JsonObject
            {
                ["index"] = i,
                ["x"] = state.x,
                ["y"] = state.y,
                ["enabled"] = state.enabled,
                ["flags"] = state.flags,
                ["priority"] = state.priority,
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

        string cmd = $"SAVE \"{name}\"";
        foreach (char c in cmd)
        {
            _editor.QueueInput((byte)c);
            Thread.Sleep(2);
        }
        _editor.QueueInput(0x0D);

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

        string cmd = $"LOAD \"{name}\"";
        foreach (char c in cmd)
        {
            _editor.QueueInput((byte)c);
            Thread.Sleep(2);
        }
        _editor.QueueInput(0x0D);

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

    // ── Graphics commands ─────────────────────────────────────────────────

    private void WriteGfxParams(int x0, int y0, int x1 = 0, int y1 = 0, int color = -1)
    {
        _bus.Write(VgcConstants.RegP0, (byte)(x0 & 0xFF));
        _bus.Write(VgcConstants.RegP1, (byte)((x0 >> 8) & 0xFF));
        _bus.Write(VgcConstants.RegP2, (byte)(y0 & 0xFF));
        _bus.Write(VgcConstants.RegP3, (byte)((y0 >> 8) & 0xFF));
        _bus.Write(VgcConstants.RegP4, (byte)(x1 & 0xFF));
        _bus.Write(VgcConstants.RegP5, (byte)((x1 >> 8) & 0xFF));
        _bus.Write(VgcConstants.RegP6, (byte)(y1 & 0xFF));
        _bus.Write(VgcConstants.RegP7, (byte)((y1 >> 8) & 0xFF));
        if (color >= 0)
            _bus.Write(VgcConstants.RegP8, (byte)color);
    }

    private string CmdGfxPlot(JsonNode req)
    {
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (x is null || y is null || color is null)
            return Error("Need x, y, color");

        // Enable graphics mode if not already
        _bus.Write(VgcConstants.RegMode, 1);

        WriteGfxParams(x.Value, y.Value, color: color.Value);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdPlot);
        return Ok();
    }

    private string CmdGfxLine(JsonNode req)
    {
        int? x0 = req["x0"]?.GetValue<int>();
        int? y0 = req["y0"]?.GetValue<int>();
        int? x1 = req["x1"]?.GetValue<int>();
        int? y1 = req["y1"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (x0 is null || y0 is null || x1 is null || y1 is null || color is null)
            return Error("Need x0, y0, x1, y1, color");

        _bus.Write(VgcConstants.RegMode, 1);
        WriteGfxParams(x0.Value, y0.Value, x1.Value, y1.Value, color.Value);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdLine);
        return Ok();
    }

    private string CmdGfxCircle(JsonNode req)
    {
        int? cx = req["cx"]?.GetValue<int>();
        int? cy = req["cy"]?.GetValue<int>();
        int? r = req["r"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (cx is null || cy is null || r is null || color is null)
            return Error("Need cx, cy, r, color");

        _bus.Write(VgcConstants.RegMode, 1);
        WriteGfxParams(cx.Value, cy.Value, r.Value, color: color.Value);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdCircle);
        return Ok();
    }

    private string CmdGfxRect(JsonNode req)
    {
        int? x0 = req["x0"]?.GetValue<int>();
        int? y0 = req["y0"]?.GetValue<int>();
        int? x1 = req["x1"]?.GetValue<int>();
        int? y1 = req["y1"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (x0 is null || y0 is null || x1 is null || y1 is null || color is null)
            return Error("Need x0, y0, x1, y1, color");

        _bus.Write(VgcConstants.RegMode, 1);
        WriteGfxParams(x0.Value, y0.Value, x1.Value, y1.Value, color.Value);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdRect);
        return Ok();
    }

    private string CmdGfxFill(JsonNode req)
    {
        int? x0 = req["x0"]?.GetValue<int>();
        int? y0 = req["y0"]?.GetValue<int>();
        int? x1 = req["x1"]?.GetValue<int>();
        int? y1 = req["y1"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (x0 is null || y0 is null || x1 is null || y1 is null || color is null)
            return Error("Need x0, y0, x1, y1, color");

        _bus.Write(VgcConstants.RegMode, 1);
        WriteGfxParams(x0.Value, y0.Value, x1.Value, y1.Value, color.Value);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdFill);
        return Ok();
    }

    private string CmdGfxClear()
    {
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdGcls);
        return Ok();
    }

    private string CmdGfxColor(JsonNode req)
    {
        int? color = req["color"]?.GetValue<int>();
        if (color is null) return Error("Need color");

        _bus.Write(VgcConstants.RegP0, (byte)color);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdGcolor);
        return Ok();
    }

    private string CmdGfxPaint(JsonNode req)
    {
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (x is null || y is null || color is null)
            return Error("Need x, y, color");

        _bus.Write(VgcConstants.RegMode, 1);
        WriteGfxParams(x.Value, y.Value, color: color.Value);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdPaint);
        return Ok();
    }

    // ── Sprite commands ──────────────────────────────────────────────────

    private string CmdSpriteDefineRow(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        int? row = req["row"]?.GetValue<int>();
        var data = req["data"]?.AsArray();
        if (sprite is null || row is null || data is null || data.Count != 8)
            return Error("Need sprite, row, and data (8 bytes)");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegP1, (byte)row);
        for (int i = 0; i < 8; i++)
            _bus.Write((ushort)(VgcConstants.RegP2 + i), (byte)data[i]!.GetValue<int>());
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);
        return Ok();
    }

    private string CmdSpriteSetPixel(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        if (sprite is null || x is null || y is null || color is null)
            return Error("Need sprite, x, y, color");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegP1, (byte)x);
        _bus.Write(VgcConstants.RegP2, (byte)y);
        _bus.Write(VgcConstants.RegP3, (byte)color);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDef);
        return Ok();
    }

    private string CmdSpritePosition(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        if (sprite is null || x is null || y is null)
            return Error("Need sprite, x, y");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegP1, (byte)(x & 0xFF));
        _bus.Write(VgcConstants.RegP2, (byte)((x >> 8) & 0xFF));
        _bus.Write(VgcConstants.RegP3, (byte)(y & 0xFF));
        _bus.Write(VgcConstants.RegP4, (byte)((y >> 8) & 0xFF));
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);
        return Ok();
    }

    private string CmdSpriteEnable(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        if (sprite is null) return Error("Need sprite");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        return Ok();
    }

    private string CmdSpriteDisable(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        if (sprite is null) return Error("Need sprite");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDis);
        return Ok();
    }

    private string CmdSpriteClear(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        if (sprite is null) return Error("Need sprite");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprClr);
        return Ok();
    }

    private string CmdSpritePriority(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        int? priority = req["priority"]?.GetValue<int>();
        if (sprite is null || priority is null)
            return Error("Need sprite, priority");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegP1, (byte)priority);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPri);
        return Ok();
    }

    private string CmdSpriteFlip(JsonNode req)
    {
        int? sprite = req["sprite"]?.GetValue<int>();
        int? flags = req["flags"]?.GetValue<int>();
        if (sprite is null || flags is null)
            return Error("Need sprite, flags");

        _bus.Write(VgcConstants.RegP0, (byte)sprite);
        _bus.Write(VgcConstants.RegP1, (byte)flags);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);
        return Ok();
    }

    private string CmdSpriteCopy(JsonNode req)
    {
        int? src = req["src"]?.GetValue<int>();
        int? dest = req["dest"]?.GetValue<int>();
        if (src is null || dest is null)
            return Error("Need src, dest");

        _bus.Write(VgcConstants.RegP0, (byte)src);
        _bus.Write(VgcConstants.RegP1, (byte)dest);
        _bus.Write(VgcConstants.RegCmd, VgcConstants.CmdSprCopy);
        return Ok();
    }

    // ── SID commands ──────────────────────────────────────────────────────

    private string CmdSidPlay(JsonNode req)
    {
        string? path = req["path"]?.GetValue<string>();
        if (path is null) return Error("Missing 'path'");
        int song = req["song"]?.GetValue<int>() ?? 1;

        if (!File.Exists(path))
            return Error($"File not found: {path}");

        byte[] data = File.ReadAllBytes(path);
        var info = SidFileParser.Parse(data);
        if (!info.IsValid)
            return Error("Invalid SID file");

        // Validate load address fits in usable RAM
        if (info.LoadAddress < VgcConstants.BasicBase)
            return Error($"Load address ${info.LoadAddress:X4} below ${VgcConstants.BasicBase:X4}");
        if (info.LoadAddress + info.Payload.Length > VgcConstants.BasicEnd + 1)
            return Error($"Payload extends past ${VgcConstants.BasicEnd:X4}");

        _bus.SidPlayer.Play(info, song);

        var result = new JsonObject
        {
            ["ok"] = true,
            ["title"] = info.Title,
            ["author"] = info.Author,
            ["copyright"] = info.Copyright,
            ["songs"] = info.Songs,
            ["start_song"] = info.StartSong,
            ["load_address"] = info.LoadAddress,
            ["init_address"] = info.InitAddress,
            ["play_address"] = info.PlayAddress,
            ["speed"] = info.UsesCiaTiming ? "CIA" : "VBI",
            ["song"] = song
        };
        return result.ToJsonString();
    }

    private string CmdSidStop()
    {
        _bus.SidPlayer.Stop();
        return Ok();
    }

    private string CmdSidInfo(JsonNode req)
    {
        string? path = req["path"]?.GetValue<string>();
        if (path is null) return Error("Missing 'path'");

        if (!File.Exists(path))
            return Error($"File not found: {path}");

        byte[] data = File.ReadAllBytes(path);
        var info = SidFileParser.Parse(data);
        if (!info.IsValid)
            return Error("Invalid SID file");

        var result = new JsonObject
        {
            ["ok"] = true,
            ["title"] = info.Title,
            ["author"] = info.Author,
            ["copyright"] = info.Copyright,
            ["songs"] = info.Songs,
            ["start_song"] = info.StartSong,
            ["load_address"] = info.LoadAddress,
            ["init_address"] = info.InitAddress,
            ["play_address"] = info.PlayAddress,
            ["version"] = info.Version,
            ["speed"] = info.UsesCiaTiming ? "CIA" : "VBI"
        };
        return result.ToJsonString();
    }

    // ── Helpers ──────────────────────────────────────────────────────────

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
