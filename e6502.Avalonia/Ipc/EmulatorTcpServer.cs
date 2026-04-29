using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using Avalonia.Threading;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Help;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;

namespace e6502.Avalonia.Ipc;

public sealed class EmulatorTcpServer : IDisposable
{
    private readonly CompositeBusDevice _bus;
    private readonly ScreenEditor _editor;
    private readonly Cpu _cpu;
    private readonly DebuggerService _debugger;
    private readonly EmulatorCanvas? _canvas;
    private readonly MainWindow? _mainWindow;
    private TcpListener _listener;
    private readonly CancellationTokenSource _cts = new();
    private Task? _acceptTask;

    public int Port { get; private set; }

    public EmulatorTcpServer(CompositeBusDevice bus, ScreenEditor editor, Cpu cpu, DebuggerService debugger,
        EmulatorCanvas? canvas, MainWindow? mainWindow, int port = 6502)
    {
        _bus = bus;
        _editor = editor;
        _cpu = cpu;
        _debugger = debugger;
        _canvas = canvas;
        _mainWindow = mainWindow;
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

                    var response = await ProcessRequestAsync(line);
                    await writer.WriteLineAsync(response);
                }
            }
        }
        catch (OperationCanceledException) { }
        catch (IOException) { }
    }

    private async Task<string> ProcessRequestAsync(string json)
    {
        try
        {
            var req = JsonNode.Parse(json);
            if (req is null) return Error("Invalid JSON");

            string? cmd = req["command"]?.GetValue<string>();
            if (cmd == "screenshot")
                return await CmdScreenshotAsync(req);
        }
        catch (Exception ex)
        {
            return Error(ex.Message);
        }

        return ProcessRequest(json);
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
                "enter_file" => CmdEnterFile(req),
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
                "set_font" => CmdSetFont(req),
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
                "mid_play" => CmdMidPlay(req),
                "mid_stop" => CmdMidStop(),
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
                // WTS commands
                "wts_load_soundfont" => CmdWtsLoadSoundfont(req),
                "wts_list_instruments" => CmdWtsListInstruments(),
                "wts_note_on" => CmdWtsNoteOn(req),
                "wts_note_off" => CmdWtsNoteOff(req),
                "wts_set_instrument" => CmdWtsSetInstrument(req),
                "wts_set_volume" => CmdWtsSetVolume(req),
                "wts_set_panning" => CmdWtsSetPanning(req),
                "wts_set_reverb" => CmdWtsSetReverb(req),
                "wts_set_chorus" => CmdWtsSetChorus(req),
                // Tile commands
                "tile_def" => CmdTileDef(req),
                "tile_def_bulk" => CmdTileDefBulk(req),
                "tile_put" => CmdTilePut(req),
                "tile_attr" => CmdTileAttr(req),
                "tile_fill" => CmdTileFill(req),
                "tile_row" => CmdTileRow(req),
                "tile_col" => CmdTileCol(req),
                "tile_scroll" => CmdTileScroll(req),
                "tile_config" => CmdTileConfig(req),
                "tile_palette" => CmdTilePalette(req),
                "tile_palette_color" => CmdTilePaletteColor(req),
                "tile_cls" => CmdTileCls(),
                "tile_peek" => CmdTilePeek(req),
                "tile_collision" => CmdTileCollision(),
                "tile_save" => CmdTileSave(req),
                "tile_load" => CmdTileLoad(req),
                // Integration test commands
                "run_cycles" => CmdRunCycles(req),
                "watch" => CmdWatch(req),
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
                // screenshot handled in ProcessRequestAsync
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
                shapeRows.Add((JsonNode?)JsonValue.Create(sb.ToString()));
            }

            sprites.Add((JsonNode)new JsonObject
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

    private string CmdEnterFile(JsonNode req)
    {
        string? path = req["path"]?.GetValue<string>();
        if (path is null) return Error("Missing 'path'");
        if (!File.Exists(path)) return Error($"File not found: {path}");

        var lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();
        if (lines.Length == 0) return Error("File is empty");

        // Queue everything at once: NEW + all lines, CR-separated
        _editor.QueueInput((byte)'N');
        _editor.QueueInput((byte)'E');
        _editor.QueueInput((byte)'W');
        _editor.QueueInput(0x0D);

        foreach (var line in lines)
        {
            string upper = UpperOutsideQuotes(line);
            foreach (char c in upper)
                _editor.QueueInput((byte)c);
            _editor.QueueInput(0x0D);
        }

        // Wait for queue to drain and Ready to appear
        var vgc = _bus.Vgc;
        var deadline = DateTime.UtcNow.AddMilliseconds(120_000);
        while (DateTime.UtcNow < deadline)
        {
            if (!_editor.HasQueuedInput)
            {
                // Queue drained — give BASIC a moment to finish the last line
                Thread.Sleep(200);

                // Track loaded program and parse companion help
                string programName = Path.GetFileNameWithoutExtension(path);
                _bus.CurrentProgramName = programName;
                _bus.CurrentProgramHelp = null;
                string mdPath = Path.ChangeExtension(path, ".md");
                if (File.Exists(mdPath))
                {
                    try
                    {
                        string markdown = File.ReadAllText(mdPath);
                        _bus.CurrentProgramHelp = Help.HelpTopic.Parse(markdown, mdPath);
                    }
                    catch { /* ignore parse errors */ }
                }

                return new JsonObject
                {
                    ["ok"] = true,
                    ["entered"] = lines.Length,
                    ["file"] = Path.GetFileName(path)
                }.ToJsonString();
            }
            Thread.Sleep(100);
        }

        return Error("Timeout entering file");
    }

    private static string UpperOutsideQuotes(string text)
    {
        var sb = new StringBuilder(text.Length);
        bool inQuotes = false;
        foreach (char c in text)
        {
            if (c == '"') inQuotes = !inQuotes;
            sb.Append(!inQuotes && c >= 'a' && c <= 'z' ? (char)(c - 32) : c);
        }
        return sb.ToString();
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

    private string CmdSetFont(JsonNode req)
    {
        int index = req["index"]?.GetValue<int>() ?? 0;
        _bus.Write(VgcConstants.RegFont, (byte)(index & 0x07));
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
        _bus.Write(VgcConstants.RegP4, 0);
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

        int? voice = req["voice"]?.GetValue<int>();
        if (voice is not null && voice.Value >= 1 && voice.Value <= 6)
            _bus.Music.SetVoiceVolume(voice.Value, level.Value);
        else
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

    // ── MIDI commands ──────────────────────────────────────────────────────

    private string CmdMidPlay(JsonNode req)
    {
        string? path = req["path"]?.GetValue<string>();
        if (path is null) return Error("Missing 'path'");

        if (!File.Exists(path))
            return Error($"File not found: {path}");

        Melanchall.DryWetMidi.Core.MidiFile midi;
        try { midi = Melanchall.DryWetMidi.Core.MidiFile.Read(path); }
        catch (Exception ex) { return Error($"Failed to read MIDI: {ex.Message}"); }

        var analysis = MidiEngine.AnalyzeChannels(midi);
        var mode = _bus.MidiPlayback.RoutingMode;

        // Auto-load soundfont if WTS is empty and not in SID-only mode
        if (mode != MidiRoutingMode.SidOnly && _bus.Wts.InstrumentCount == 0)
            MidiAutoSoundfont.TryLoad(_bus.Fio, _bus.Wts);

        bool sidOnly = mode == MidiRoutingMode.SidOnly || _bus.Wts.InstrumentCount == 0;

        Dictionary<int, int>? voiceMapping = null;
        if (req["voices"] is JsonNode voicesNode)
        {
            voiceMapping = new Dictionary<int, int>();
            foreach (var prop in voicesNode.AsObject())
            {
                // Keys are 1-based voice numbers, values are MIDI channels
                if (int.TryParse(prop.Key, out int voice) && voice >= 1 && voice <= 14)
                    voiceMapping[voice - 1] = prop.Value!.GetValue<int>();
            }
            if (voiceMapping.Count == 0) voiceMapping = null;
        }

        int maxVoices = sidOnly ? 6 : 14;
        var rawChannels = MidiEngine.SelectChannels(midi, maxVoices);

        var routing = MidiEngine.RouteVoices(analysis, rawChannels, sidOnly,
            (slot, bucket) => _bus.Music.DefineInstrument(slot, bucket.Waveform,
                bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release),
            voiceMapping);

        _bus.Music.SetVolume(15);
        _bus.MidiPlayback.Play(midi, routing.VoiceToChannel, routing.InstrumentSlots);

        var result = new JsonObject
        {
            ["ok"] = true,
            ["voices"] = routing.VoiceToChannel.Length,
            ["routing"] = routing.SidOnly ? "sid" : "auto",
            ["channels"] = new JsonArray(routing.VoiceToChannel.Where(c => c >= 0).Select(c => (JsonNode)JsonValue.Create(c)).ToArray())
        };
        return result.ToJsonString();
    }

    private string CmdMidStop()
    {
        _bus.MidiPlayback.Stop();
        return Ok();
    }

    // ── Integration test commands ─────────────────────────────────────────

    private string CmdRunCycles(JsonNode req)
    {
        int? cycles = req["cycles"]?.GetValue<int>();
        if (cycles is null) return Error("Missing 'cycles'");
        if (cycles.Value <= 0) return Error("'cycles' must be positive");

        var (state, executed) = _debugger.RunCycles(cycles.Value);
        var result = new JsonObject
        {
            ["ok"] = true,
            ["cycles_executed"] = executed,
            ["a"] = state.A, ["x"] = state.X, ["y"] = state.Y, ["sp"] = state.Sp, ["pc"] = state.Pc,
            ["nf"] = state.Nf, ["vf"] = state.Vf, ["df"] = state.Df, ["if"] = state.If, ["zf"] = state.Zf, ["cf"] = state.Cf,
            ["paused"] = true
        };
        return result.ToJsonString();
    }

    private string CmdWatch(JsonNode req)
    {
        int? address = req["address"]?.GetValue<int>();
        if (address is null) return Error("Missing 'address'");
        int? value = req["value"]?.GetValue<int>();
        if (value is null) return Error("Missing 'value'");
        int timeoutMs = req["timeout_ms"]?.GetValue<int>() ?? 5000;

        var (matched, finalValue) = _debugger.WatchMemory((ushort)address.Value, (byte)value.Value, timeoutMs);
        var result = new JsonObject
        {
            ["ok"] = true,
            ["matched"] = matched,
            ["address"] = address.Value,
            ["expected"] = value.Value,
            ["actual"] = finalValue
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
                    condArr.Add((JsonNode?)JsonValue.Create("unconditional"));
                else
                    condArr.Add((JsonNode?)JsonValue.Create($"{c.Register} {c.Op} {c.Value}"));
            }
            arr.Add((JsonNode)new JsonObject
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
            arr.Add((JsonNode)new JsonObject
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

    // ── Screenshot commands ───────────────────────────────────────────────

    private async Task<string> CmdScreenshotAsync(JsonNode req)
    {
        string? path = req["path"]?.GetValue<string>();
        if (path is null) return Error("Missing 'path'");

        string type = req["type"]?.GetValue<string>() ?? "frame";

        if (type == "frame")
        {
            if (_canvas is null) return Error("Canvas not available");
            // Frame capture is thread-safe via _renderLock — no UI dispatch needed
            _canvas.SaveScreenshot(path);
            return new JsonObject
            {
                ["ok"] = true,
                ["path"] = Path.GetFullPath(path),
                ["width"] = EmulatorCanvas.NativeWidth,
                ["height"] = EmulatorCanvas.NativeHeight
            }.ToJsonString();
        }

        if (type == "window")
        {
            if (_mainWindow is null) return Error("Window not available");
            // Window capture requires UI thread — must await, never block
            var (w, h) = await Dispatcher.UIThread.InvokeAsync(() => _mainWindow.SaveWindowScreenshot(path));
            return new JsonObject
            {
                ["ok"] = true,
                ["path"] = Path.GetFullPath(path),
                ["width"] = w,
                ["height"] = h
            }.ToJsonString();
        }

        return Error($"Unknown screenshot type: {type}");
    }

    // ── Network commands ──────────────────────────────────────────────────

    private string CmdNicConnect(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        string? host = req["host"]?.GetValue<string>();
        int? port = req["port"]?.GetValue<int>();
        if (slot is null || host is null || port is null)
            return Error("Need slot, host, port");
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");

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
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        return Ok();
    }

    private string CmdNicSend(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        string? data = req["data"]?.GetValue<string>();
        if (slot is null || data is null) return Error("Need slot, data (base64)");
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");

        byte[] payload = Convert.FromBase64String(data);
        if (payload.Length == 0 || payload.Length > 256) return Error("Data must be 1-256 bytes");

        _bus.Nic.SendDirect(slot.Value, payload);
        return Ok();
    }

    private string CmdNicRecv(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");

        byte[]? payload = _bus.Nic.RecvDirect(slot.Value);
        if (payload is null)
            return new JsonObject { ["ok"] = true, ["data"] = "", ["length"] = 0 }.ToJsonString();

        return new JsonObject
        {
            ["ok"] = true,
            ["data"] = Convert.ToBase64String(payload),
            ["length"] = payload.Length
        }.ToJsonString();
    }

    private string CmdNicListen(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        int? port = req["port"]?.GetValue<int>();
        if (slot is null || port is null) return Error("Need slot, port");
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");

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
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdAccept);
        return Ok();
    }

    private string CmdNicStatus(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");
        if (slot.Value < 0 || slot.Value > 3)
            return Error("Slot must be 0-3");

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

    // ── WTS commands ──────────────────────────────────────────────────────

    private string CmdWtsLoadSoundfont(JsonNode req)
    {
        string? filename = req["filename"]?.GetValue<string>();
        if (filename is null) return Error("Missing 'filename'");

        // Write filename to FIO buffer
        byte[] nameBytes = Encoding.ASCII.GetBytes(filename);
        int len = Math.Min(nameBytes.Length, 63);
        _bus.Write((ushort)VgcConstants.FioNameLen, (byte)len);
        for (int i = 0; i < len; i++)
            _bus.Write((ushort)(VgcConstants.FioName + i), nameBytes[i]);

        // Trigger SfLoad command
        _bus.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdSfLoad);

        // Check status
        byte status = _bus.Read((ushort)VgcConstants.FioStatus);
        if (status == VgcConstants.FioStatusOk)
            return Ok();

        byte errCode = _bus.Read((ushort)VgcConstants.FioErrCode);
        return Error($"SfLoad failed: status={status}, error={errCode}");
    }

    private string CmdWtsListInstruments()
    {
        var wts = _bus.Wts;
        int count = wts.InstrumentCount;
        var instruments = new JsonArray();

        for (int i = 0; i < count; i++)
        {
            instruments.Add((JsonNode)new JsonObject
            {
                ["index"] = i,
                ["name"] = wts.GetInstrumentName(i),
                ["bank"] = wts.GetInstrumentBank(i),
                ["program"] = wts.GetInstrumentProgram(i)
            });
        }

        return new JsonObject
        {
            ["ok"] = true,
            ["instruments"] = instruments
        }.ToJsonString();
    }

    private string CmdWtsNoteOn(JsonNode req)
    {
        int? voice = req["voice"]?.GetValue<int>();
        int? note = req["note"]?.GetValue<int>();
        int? velocity = req["velocity"]?.GetValue<int>();
        int? instrument = req["instrument"]?.GetValue<int>();
        if (voice is null || note is null || velocity is null || instrument is null)
            return Error("Need voice, note, velocity, instrument");

        _bus.Wts.NoteOn(voice.Value, note.Value, velocity.Value, instrument.Value);
        return Ok();
    }

    private string CmdWtsNoteOff(JsonNode req)
    {
        int? voice = req["voice"]?.GetValue<int>();
        if (voice is null) return Error("Need voice");

        _bus.Wts.NoteOff(voice.Value);
        return Ok();
    }

    private string CmdWtsSetInstrument(JsonNode req)
    {
        int? voice = req["voice"]?.GetValue<int>();
        int? instrument = req["instrument"]?.GetValue<int>();
        if (voice is null || instrument is null)
            return Error("Need voice, instrument");

        // Write instrument via register protocol
        int voiceBase = VgcConstants.WtsVoiceBase + voice.Value * VgcConstants.WtsVoiceStride;
        _bus.Write((ushort)(voiceBase + VgcConstants.WtsVoiceInstrument), (byte)instrument.Value);
        return Ok();
    }

    private string CmdWtsSetVolume(JsonNode req)
    {
        int? voice = req["voice"]?.GetValue<int>();
        int? volume = req["volume"]?.GetValue<int>();
        if (voice is null || volume is null)
            return Error("Need voice, volume");

        _bus.Wts.SetVolume(voice.Value, (byte)volume.Value);
        return Ok();
    }

    private string CmdWtsSetPanning(JsonNode req)
    {
        int? voice = req["voice"]?.GetValue<int>();
        int? pan = req["pan"]?.GetValue<int>();
        if (voice is null || pan is null)
            return Error("Need voice, pan");

        _bus.Wts.SetPanning(voice.Value, (byte)pan.Value);
        return Ok();
    }

    private string CmdWtsSetReverb(JsonNode req)
    {
        int? level = req["level"]?.GetValue<int>();
        if (level is null) return Error("Need level");

        _bus.Wts.ReverbLevel = (byte)level.Value;
        return Ok();
    }

    private string CmdWtsSetChorus(JsonNode req)
    {
        int? level = req["level"]?.GetValue<int>();
        if (level is null) return Error("Need level");

        _bus.Wts.ChorusLevel = (byte)level.Value;
        return Ok();
    }

    // ── Tile commands ────────────────────────────────────────────────────

    private string CmdTileDef(JsonNode req)
    {
        int? tile = req["tile"]?.GetValue<int>();
        string? hex = req["data"]?.GetValue<string>();
        if (tile is null || hex is null) return Error("Need tile, data (hex)");

        byte[] data = Convert.FromHexString(hex);
        var vgc = _bus.Vgc;
        int tileSize = vgc.IsTileSize16 ? VgcConstants.TileSize16 : VgcConstants.TileSize8;
        if (data.Length < tileSize) return Error($"Need {tileSize} bytes of tile data");

        // Write data to CPU RAM at a temp location and use TDEF
        ushort addr = 0x0200; // use vector table area temporarily
        for (int i = 0; i < tileSize; i++)
            _bus.Write((ushort)(addr + i), data[i]);

        vgc.Write(VgcConstants.TileP0, (byte)tile);
        vgc.Write(VgcConstants.TileAddrL, (byte)(addr & 0xFF));
        vgc.Write(VgcConstants.TileAddrH, (byte)(addr >> 8));
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdDef);
        return Ok();
    }

    private string CmdTileDefBulk(JsonNode req)
    {
        int? start = req["start"]?.GetValue<int>();
        int? count = req["count"]?.GetValue<int>();
        string? hex = req["data"]?.GetValue<string>();
        if (start is null || count is null || hex is null)
            return Error("Need start, count, data (hex)");

        byte[] data = Convert.FromHexString(hex);
        var vgc = _bus.Vgc;
        int tileSize = vgc.IsTileSize16 ? VgcConstants.TileSize16 : VgcConstants.TileSize8;
        int needed = count.Value * tileSize;
        if (data.Length < needed) return Error($"Need {needed} bytes of tile data");

        // Write bulk data into CPU RAM starting at a safe address
        ushort addr = 0x0200;
        for (int i = 0; i < needed; i++)
            _bus.Write((ushort)(addr + i), data[i]);

        vgc.Write(VgcConstants.TileP0, (byte)start);
        vgc.Write(VgcConstants.TileP1, (byte)count);
        vgc.Write(VgcConstants.TileAddrL, (byte)(addr & 0xFF));
        vgc.Write(VgcConstants.TileAddrH, (byte)(addr >> 8));
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdDefBulk);
        return Ok();
    }

    private string CmdTilePut(JsonNode req)
    {
        int? nt = req["nt"]?.GetValue<int>();
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        int? tile = req["tile"]?.GetValue<int>();
        if (nt is null || x is null || y is null || tile is null)
            return Error("Need nt, x, y, tile");

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileP0, (byte)nt);
        vgc.Write(VgcConstants.TileP1, (byte)x);
        vgc.Write(VgcConstants.TileP2, (byte)y);
        vgc.Write(VgcConstants.TileP3, (byte)tile);
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdPut);
        return Ok();
    }

    private string CmdTileAttr(JsonNode req)
    {
        int? nt = req["nt"]?.GetValue<int>();
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        int? attr = req["attr"]?.GetValue<int>();
        if (nt is null || x is null || y is null || attr is null)
            return Error("Need nt, x, y, attr");

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileP0, (byte)nt);
        vgc.Write(VgcConstants.TileP1, (byte)x);
        vgc.Write(VgcConstants.TileP2, (byte)y);
        vgc.Write(VgcConstants.TileP3, (byte)attr);
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdAttr);
        return Ok();
    }

    private string CmdTileFill(JsonNode req)
    {
        int? nt = req["nt"]?.GetValue<int>();
        int? tile = req["tile"]?.GetValue<int>();
        if (nt is null || tile is null) return Error("Need nt, tile");

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileP0, (byte)nt);
        vgc.Write(VgcConstants.TileP1, (byte)tile);
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdFill);
        return Ok();
    }

    private string CmdTileRow(JsonNode req)
    {
        int? nt = req["nt"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        string? hex = req["data"]?.GetValue<string>();
        if (nt is null || y is null || hex is null) return Error("Need nt, y, data (hex)");

        byte[] data = Convert.FromHexString(hex);
        if (data.Length < VgcConstants.NametableCols)
            return Error($"Need {VgcConstants.NametableCols} bytes");

        ushort addr = 0x0200;
        for (int i = 0; i < VgcConstants.NametableCols; i++)
            _bus.Write((ushort)(addr + i), data[i]);

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileP0, (byte)nt);
        vgc.Write(VgcConstants.TileP1, (byte)y);
        vgc.Write(VgcConstants.TileAddrL, (byte)(addr & 0xFF));
        vgc.Write(VgcConstants.TileAddrH, (byte)(addr >> 8));
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdRow);
        return Ok();
    }

    private string CmdTileCol(JsonNode req)
    {
        int? nt = req["nt"]?.GetValue<int>();
        int? x = req["x"]?.GetValue<int>();
        string? hex = req["data"]?.GetValue<string>();
        if (nt is null || x is null || hex is null) return Error("Need nt, x, data (hex)");

        byte[] data = Convert.FromHexString(hex);
        if (data.Length < VgcConstants.NametableRows)
            return Error($"Need {VgcConstants.NametableRows} bytes");

        ushort addr = 0x0200;
        for (int i = 0; i < VgcConstants.NametableRows; i++)
            _bus.Write((ushort)(addr + i), data[i]);

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileP0, (byte)nt);
        vgc.Write(VgcConstants.TileP1, (byte)x);
        vgc.Write(VgcConstants.TileAddrL, (byte)(addr & 0xFF));
        vgc.Write(VgcConstants.TileAddrH, (byte)(addr >> 8));
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdCol);
        return Ok();
    }

    private string CmdTileScroll(JsonNode req)
    {
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        if (x is null || y is null) return Error("Need x, y");

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileScrollXL, (byte)(x & 0xFF));
        vgc.Write(VgcConstants.TileScrollXH, (byte)((x >> 8) & 0xFF));
        vgc.Write(VgcConstants.TileScrollYL, (byte)(y & 0xFF));
        vgc.Write(VgcConstants.TileScrollYH, (byte)((y >> 8) & 0xFF));
        return Ok();
    }

    private string CmdTileConfig(JsonNode req)
    {
        int? size = req["size"]?.GetValue<int>();
        int? mirror = req["mirror"]?.GetValue<int>();
        int? trans = req["trans"]?.GetValue<int>();

        var vgc = _bus.Vgc;

        if (size is not null || mirror is not null)
        {
            byte config = 0;
            if (size == 16) config |= VgcConstants.TileCfgSize16;
            if (mirror is not null)
                config |= (byte)((mirror.Value & 0x03) << VgcConstants.TileCfgMirrorShift);
            vgc.Write(VgcConstants.TileConfig, config);
        }

        if (trans is not null)
            vgc.Write(VgcConstants.TileTransColor, (byte)trans);

        // Set mode to 4
        vgc.Write(VgcConstants.RegMode, 4);
        return Ok();
    }

    private string CmdTilePalette(JsonNode req)
    {
        int? index = req["index"]?.GetValue<int>();
        string? hex = req["data"]?.GetValue<string>();
        if (index is null || hex is null) return Error("Need index, data (hex)");

        byte[] data = Convert.FromHexString(hex);
        if (data.Length < 48) return Error("Need 48 bytes (16 × RGB)");

        ushort addr = 0x0200;
        for (int i = 0; i < 48; i++)
            _bus.Write((ushort)(addr + i), data[i]);

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TilePalP0, (byte)index);
        vgc.Write(VgcConstants.TileAddrL, (byte)(addr & 0xFF));
        vgc.Write(VgcConstants.TileAddrH, (byte)(addr >> 8));
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdPal);
        return Ok();
    }

    private string CmdTilePaletteColor(JsonNode req)
    {
        int? sub = req["sub"]?.GetValue<int>();
        int? color = req["color"]?.GetValue<int>();
        int? r = req["r"]?.GetValue<int>();
        int? g = req["g"]?.GetValue<int>();
        int? b = req["b"]?.GetValue<int>();
        if (sub is null || color is null || r is null || g is null || b is null)
            return Error("Need sub, color, r, g, b");

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TilePalP0, (byte)sub);
        vgc.Write(VgcConstants.TilePalP1, (byte)color);
        vgc.Write(VgcConstants.TileP0, (byte)r);
        vgc.Write(VgcConstants.TileP1, (byte)g);
        vgc.Write(VgcConstants.TileP2, (byte)b);
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdPalC);
        return Ok();
    }

    private string CmdTileCls()
    {
        _bus.Vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdCls);
        return Ok();
    }

    private string CmdTilePeek(JsonNode req)
    {
        int? nt = req["nt"]?.GetValue<int>();
        int? x = req["x"]?.GetValue<int>();
        int? y = req["y"]?.GetValue<int>();
        if (nt is null || x is null || y is null) return Error("Need nt, x, y");

        var vgc = _bus.Vgc;
        vgc.Write(VgcConstants.TileP0, (byte)nt);
        vgc.Write(VgcConstants.TileP1, (byte)x);
        vgc.Write(VgcConstants.TileP2, (byte)y);
        vgc.Write(VgcConstants.TileCmd, VgcConstants.TileCmdPeek);

        byte tileIndex = vgc.Read(VgcConstants.TilePeekVal);
        byte attr = vgc.Read(VgcConstants.TilePeekAttr);

        return new JsonObject
        {
            ["ok"] = true,
            ["tile"] = tileIndex,
            ["attr"] = attr
        }.ToJsonString();
    }

    private string CmdTileCollision()
    {
        var vgc = _bus.Vgc;
        byte lo = vgc.Read(VgcConstants.TileColL);
        byte hi = vgc.Read(VgcConstants.TileColH);
        int mask = lo | (hi << 8);

        return new JsonObject
        {
            ["ok"] = true,
            ["collision"] = mask
        }.ToJsonString();
    }

    private string CmdTileSave(JsonNode req)
    {
        string? filename = req["filename"]?.GetValue<string>();
        if (filename is null) return Error("Need filename");

        var fio = _bus.Fio;
        fio.Write(VgcConstants.FioNameLen, (byte)filename.Length);
        for (int i = 0; i < filename.Length; i++)
            fio.Write((ushort)(VgcConstants.FioName + i), (byte)filename[i]);
        fio.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTSave);

        byte status = fio.Read(VgcConstants.FioStatus);
        if (status != VgcConstants.FioStatusOk)
            return Error("TSAVE failed");
        return Ok();
    }

    private string CmdTileLoad(JsonNode req)
    {
        string? filename = req["filename"]?.GetValue<string>();
        if (filename is null) return Error("Need filename");

        var fio = _bus.Fio;
        fio.Write(VgcConstants.FioNameLen, (byte)filename.Length);
        for (int i = 0; i < filename.Length; i++)
            fio.Write((ushort)(VgcConstants.FioName + i), (byte)filename[i]);
        fio.Write(VgcConstants.FioCmd, VgcConstants.FioCmdTLoad);

        byte status = fio.Read(VgcConstants.FioStatus);
        if (status != VgcConstants.FioStatusOk)
        {
            byte err = fio.Read(VgcConstants.FioErrCode);
            return Error(err == VgcConstants.FioErrNotFound ? "File not found" : "TLOAD failed");
        }
        return Ok();
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
