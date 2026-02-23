using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;

namespace e6502.Avalonia.Ipc;

public sealed class EmulatorTcpServer : IDisposable
{
    private readonly CompositeBusDevice _bus;
    private readonly ScreenEditor _editor;
    private readonly Cpu _cpu;
    private readonly DebuggerService _debugger;
    private TcpListener _listener;
    private readonly CancellationTokenSource _cts = new();
    private Task? _acceptTask;

    public int Port { get; private set; }

    public EmulatorTcpServer(CompositeBusDevice bus, ScreenEditor editor, Cpu cpu, DebuggerService debugger, int port = 6502)
    {
        _bus = bus;
        _editor = editor;
        _cpu = cpu;
        _debugger = debugger;
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
                // Music engine commands
                "instrument" => CmdInstrument(req),
                "sound" => CmdSound(req),
                "volume" => CmdVolume(req),
                "music_seq" => CmdMusicSeq(req),
                "music_play" => CmdMusicPlay(),
                "music_stop" => CmdMusicStop(),
                "music_tempo" => CmdMusicTempo(req),
                "music_loop" => CmdMusicLoop(req),
                "music_status" => CmdMusicStatus(),
                // Network commands
                "nic_connect" => CmdNicConnect(req),
                "nic_disconnect" => CmdNicDisconnect(req),
                "nic_send" => CmdNicSend(req),
                "nic_recv" => CmdNicRecv(req),
                "nic_listen" => CmdNicListen(req),
                "nic_accept" => CmdNicAccept(req),
                "nic_status" => CmdNicStatus(req),
                // Debugger commands
                "dbg_state" => CmdDbgState(),
                "dbg_pause" => CmdDbgPause(),
                "dbg_resume" => CmdDbgResume(),
                "dbg_step" => CmdDbgStep(),
                "dbg_break_set" => CmdDbgBreakSet(req),
                "dbg_break_clear" => CmdDbgBreakClear(req),
                "dbg_break_clear_all" => CmdDbgBreakClearAll(),
                "dbg_break_list" => CmdDbgBreakList(),
                "dbg_read_memory" => CmdDbgReadMemory(req),
                "dbg_disasm" => CmdDbgDisasm(req),
                "dbg_stack" => CmdDbgStack(),
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
        bool wasPaused = _debugger.IsPaused;
        if (!wasPaused) _debugger.Pause();
        Thread.Sleep(5); // let CPU thread reach the pause gate

        _bus.Vgc.Reset();
        _bus.Music.MusicStop();
        _bus.SidPlayer.Stop();
        _editor.ClearInputQueue();
        _cpu.Boot();

        if (!wasPaused) _debugger.Resume();
        return Ok();
    }

    private string CmdWarmStart()
    {
        bool wasPaused = _debugger.IsPaused;
        if (!wasPaused) _debugger.Pause();
        Thread.Sleep(5); // let CPU thread reach the pause gate

        _bus.Music.MusicStop();
        _editor.ClearInputQueue();
        _cpu.Boot(0x0000);

        if (!wasPaused) _debugger.Resume();
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
        _cpu.EnableInterrupts();

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

    // ── Music engine commands ─────────────────────────────────────────────

    private string CmdInstrument(JsonNode req)
    {
        int? id = req["id"]?.GetValue<int>();
        int? waveform = req["waveform"]?.GetValue<int>();
        int? a = req["a"]?.GetValue<int>();
        int? d = req["d"]?.GetValue<int>();
        int? s = req["s"]?.GetValue<int>();
        int? r = req["r"]?.GetValue<int>();
        if (id is null || waveform is null || a is null || d is null || s is null || r is null)
            return Error("Need id, waveform, a, d, s, r");

        _bus.Music.DefineInstrument(id.Value, (byte)waveform.Value,
            (byte)a.Value, (byte)d.Value, (byte)s.Value, (byte)r.Value);
        return Ok();
    }

    private string CmdSound(JsonNode req)
    {
        int? note = req["note"]?.GetValue<int>();
        int? duration = req["duration"]?.GetValue<int>();
        if (note is null || duration is null)
            return Error("Need note, duration");

        int instrument = req["instrument"]?.GetValue<int>() ?? 0;
        _bus.Music.PlaySound(note.Value, duration.Value, instrument);
        return Ok();
    }

    private string CmdVolume(JsonNode req)
    {
        int? level = req["level"]?.GetValue<int>();
        if (level is null) return Error("Need level");

        _bus.Music.SetVolume(level.Value);
        return Ok();
    }

    private string CmdMusicSeq(JsonNode req)
    {
        int? voice = req["voice"]?.GetValue<int>();
        string? mml = req["mml"]?.GetValue<string>();
        if (voice is null || mml is null)
            return Error("Need voice (1-6), mml");

        _bus.Music.SetSequence(voice.Value - 1, mml);
        return Ok();
    }

    private string CmdMusicPlay()
    {
        _bus.Music.MusicPlay();
        return Ok();
    }

    private string CmdMusicStop()
    {
        _bus.Music.MusicStop();
        return Ok();
    }

    private string CmdMusicTempo(JsonNode req)
    {
        int? bpm = req["bpm"]?.GetValue<int>();
        if (bpm is null) return Error("Need bpm");

        _bus.Music.SetTempo(bpm.Value);
        return Ok();
    }

    private string CmdMusicLoop(JsonNode req)
    {
        bool? on = req["on"]?.GetValue<bool>();
        if (on is null) return Error("Need on (true/false)");

        _bus.Music.SetLoop(on.Value);
        return Ok();
    }

    private string CmdMusicStatus()
    {
        var result = new JsonObject
        {
            ["ok"] = true,
            ["sfx_playing"] = _bus.Music.IsPlaying,
            ["music_playing"] = _bus.Music.IsMusicPlaying
        };
        return result.ToJsonString();
    }

    // ── Debugger commands ──────────────────────────────────────────────────

    private string StateJson(CpuState s) => new JsonObject
    {
        ["ok"] = true,
        ["a"] = s.A, ["x"] = s.X, ["y"] = s.Y, ["sp"] = s.Sp, ["pc"] = s.Pc,
        ["nf"] = s.Nf, ["vf"] = s.Vf, ["df"] = s.Df, ["if"] = s.If, ["zf"] = s.Zf, ["cf"] = s.Cf,
        ["paused"] = _debugger.IsPaused
    }.ToJsonString();

    private string CmdDbgState() => StateJson(_cpu.GetState());

    private string CmdDbgPause()
    {
        _debugger.Pause();
        Thread.Sleep(5); // let CPU thread settle
        return StateJson(_cpu.GetState());
    }

    private string CmdDbgResume()
    {
        _debugger.Resume();
        return Ok();
    }

    private string CmdDbgStep()
    {
        var state = _debugger.Step();
        var (text, _) = _cpu.Disassemble(state.Pc);
        var result = new JsonObject
        {
            ["ok"] = true,
            ["a"] = state.A, ["x"] = state.X, ["y"] = state.Y, ["sp"] = state.Sp, ["pc"] = state.Pc,
            ["nf"] = state.Nf, ["vf"] = state.Vf, ["df"] = state.Df, ["if"] = state.If, ["zf"] = state.Zf, ["cf"] = state.Cf,
            ["paused"] = true,
            ["next_instruction"] = text
        };
        return result.ToJsonString();
    }

    private string CmdDbgBreakSet(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        if (address is null) return Error("Missing 'address'");

        BreakpointCondition? condition = null;
        string? register = req["register"]?.GetValue<string>();
        string? op = req["op"]?.GetValue<string>();
        int? value = req["value"]?.GetValue<int>();
        if (register is not null && op is not null && value is not null)
            condition = new BreakpointCondition(register, op, (byte)value.Value);

        _debugger.AddBreakpoint((ushort)address.Value, condition);

        var result = new JsonObject
        {
            ["ok"] = true,
            ["address"] = address.Value,
            ["conditional"] = condition is not null
        };
        return result.ToJsonString();
    }

    private string CmdDbgBreakClear(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        if (address is null) return Error("Missing 'address'");

        bool removed = _debugger.RemoveBreakpoint((ushort)address.Value);
        var result = new JsonObject
        {
            ["ok"] = true,
            ["removed"] = removed
        };
        return result.ToJsonString();
    }

    private string CmdDbgBreakClearAll()
    {
        _debugger.ClearAllBreakpoints();
        return Ok();
    }

    private string CmdDbgBreakList()
    {
        var bps = _debugger.ListBreakpoints();
        var arr = new JsonArray();
        foreach (var (addr, conditions) in bps)
        {
            var condArr = new JsonArray();
            foreach (var c in conditions)
            {
                if (c is null)
                    condArr.Add(JsonValue.Create("unconditional"));
                else
                    condArr.Add(JsonValue.Create($"{c.Register} {c.Op} {c.Value}"));
            }
            arr.Add(new JsonObject
            {
                ["address"] = addr,
                ["conditions"] = condArr
            });
        }
        var result = new JsonObject { ["ok"] = true, ["breakpoints"] = arr };
        return result.ToJsonString();
    }

    private string CmdDbgReadMemory(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        int? length = req["length"]?.GetValue<int>();
        if (address is null || length is null) return Error("Need address, length");

        byte[] data = _debugger.ReadMemory((ushort)address.Value, length.Value);
        var result = new JsonObject
        {
            ["ok"] = true,
            ["address"] = address.Value,
            ["length"] = data.Length,
            ["hex"] = Convert.ToHexString(data)
        };
        return result.ToJsonString();
    }

    private string CmdDbgDisasm(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        int count = req["count"]?.GetValue<int>() ?? 10;
        if (address is null) return Error("Missing 'address'");

        var instructions = _debugger.DisassembleRange((ushort)address.Value, count);
        var arr = new JsonArray();
        foreach (var (addr, text, bytes) in instructions)
        {
            arr.Add(new JsonObject
            {
                ["address"] = addr,
                ["text"] = text,
                ["bytes"] = bytes
            });
        }
        var result = new JsonObject { ["ok"] = true, ["instructions"] = arr };
        return result.ToJsonString();
    }

    private string CmdDbgStack()
    {
        var state = _cpu.GetState();
        byte[] data = _debugger.ReadStack();
        var result = new JsonObject
        {
            ["ok"] = true,
            ["sp"] = state.Sp,
            ["entries"] = Convert.ToHexString(data)
        };
        return result.ToJsonString();
    }

    // ── Network commands ──────────────────────────────────────────────────

    private string CmdNicConnect(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        string? host = req["host"]?.GetValue<string>();
        int? port = req["port"]?.GetValue<int>();
        if (slot is null || host is null || port is null)
            return Error("Need slot, host, port");

        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        for (int i = 0; i < host.Length && i < 31; i++)
            _bus.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _bus.Write((ushort)(VgcConstants.NicNameBuf + Math.Min(host.Length, 31)), 0);
        _bus.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port.Value & 0xFF));
        _bus.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port.Value >> 8));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);
        return Ok();
    }

    private string CmdNicDisconnect(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        return Ok();
    }

    private string CmdNicSend(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        string? data = req["data"]?.GetValue<string>();
        if (slot is null || data is null) return Error("Need slot, data (base64)");

        byte[] payload = Convert.FromBase64String(data);
        if (payload.Length == 0 || payload.Length > 256) return Error("Data must be 1-256 bytes");

        ushort scratchAddr = 0x0280;
        for (int i = 0; i < payload.Length; i++)
            _bus.WriteRam((ushort)(scratchAddr + i), payload[i]);

        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicDmaAddrL, (byte)(scratchAddr & 0xFF));
        _bus.Write((ushort)VgcConstants.NicDmaAddrH, (byte)(scratchAddr >> 8));
        _bus.Write((ushort)VgcConstants.NicDmaLen, (byte)(payload.Length == 256 ? 0 : payload.Length));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdSend);
        return Ok();
    }

    private string CmdNicRecv(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");

        ushort scratchAddr = 0x0280;
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicDmaAddrL, (byte)(scratchAddr & 0xFF));
        _bus.Write((ushort)VgcConstants.NicDmaAddrH, (byte)(scratchAddr >> 8));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        int len = _bus.Read((ushort)VgcConstants.NicMsgLen);
        if (len == 0)
            return new JsonObject { ["ok"] = true, ["data"] = "", ["length"] = 0 }.ToJsonString();

        var payload = new byte[len];
        for (int i = 0; i < len; i++)
            payload[i] = _bus.Read((ushort)(scratchAddr + i));

        return new JsonObject
        {
            ["ok"] = true,
            ["data"] = Convert.ToBase64String(payload),
            ["length"] = len
        }.ToJsonString();
    }

    private string CmdNicListen(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        int? port = req["port"]?.GetValue<int>();
        if (slot is null || port is null) return Error("Need slot, port");

        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicLocalPortL, (byte)(port.Value & 0xFF));
        _bus.Write((ushort)VgcConstants.NicLocalPortH, (byte)(port.Value >> 8));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdListen);
        return Ok();
    }

    private string CmdNicAccept(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdAccept);
        return Ok();
    }

    private string CmdNicStatus(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");

        int slotAddr = VgcConstants.NicSlotStatus0 + slot.Value;
        byte slotStatus = _bus.Read((ushort)slotAddr);
        byte globalStatus = _bus.Read((ushort)VgcConstants.NicStatus);

        return new JsonObject
        {
            ["ok"] = true,
            ["slot"] = slot.Value,
            ["connected"] = (slotStatus & VgcConstants.NicSlotConnected) != 0,
            ["data_ready"] = (slotStatus & VgcConstants.NicSlotDataReady) != 0,
            ["send_ready"] = (slotStatus & VgcConstants.NicSlotSendReady) != 0,
            ["error"] = (slotStatus & VgcConstants.NicSlotError) != 0,
            ["remote_closed"] = (slotStatus & VgcConstants.NicSlotRemoteClosed) != 0,
            ["global_status"] = globalStatus
        }.ToJsonString();
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
