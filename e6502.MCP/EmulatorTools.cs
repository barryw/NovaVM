using System.ComponentModel;
using System.Text;
using System.Text.Json.Nodes;
using ModelContextProtocol.Server;

namespace e6502.MCP;

[McpServerToolType]
public static class EmulatorTools
{
    [McpServerTool, Description("Enter a BASIC line. Uppercases text outside quotes, sends Enter, waits for Ready prompt.")]
    public static async Task<string> EnterBasicLine(
        EmulatorClient client,
        [Description("The BASIC line to enter (e.g. '10 PRINT \"HELLO\"')")] string line)
    {
        string converted = ConvertToUpperOutsideQuotes(line);
        await client.SendAsync(new JsonObject
        {
            ["command"] = "type_text",
            ["text"] = converted
        });
        await client.SendAsync(new JsonObject
        {
            ["command"] = "send_key",
            ["key"] = "ENTER"
        });
        var wait = await client.SendAsync(new JsonObject
        {
            ["command"] = "wait_ready",
            ["text"] = "Ready",
            ["timeout_ms"] = 5000
        });
        return wait.ToJsonString();
    }

    [McpServerTool, Description("Type RUN + Enter, wait for program to finish, return screen output.")]
    public static async Task<string> RunProgram(
        EmulatorClient client,
        [Description("Timeout in milliseconds to wait for Ready prompt after RUN (default 10000)")] int timeoutMs = 10000)
    {
        await client.SendAsync(new JsonObject
        {
            ["command"] = "type_text",
            ["text"] = "RUN"
        });
        await client.SendAsync(new JsonObject
        {
            ["command"] = "send_key",
            ["key"] = "ENTER"
        });
        await client.SendAsync(new JsonObject
        {
            ["command"] = "wait_ready",
            ["text"] = "Ready",
            ["timeout_ms"] = timeoutMs
        });
        var screen = await client.SendAsync(new JsonObject
        {
            ["command"] = "read_screen"
        });
        return FormatScreen(screen);
    }

    [McpServerTool, Description("Read the current screen contents and cursor position.")]
    public static async Task<string> ReadScreen(EmulatorClient client)
    {
        var screen = await client.SendAsync(new JsonObject
        {
            ["command"] = "read_screen"
        });
        return FormatScreen(screen);
    }

    [McpServerTool, Description("Type raw text into the emulator. Does NOT send Enter. Does NOT uppercase.")]
    public static async Task<string> TypeLine(
        EmulatorClient client,
        [Description("Text to type")] string text,
        [Description("Inter-character delay in ms (default 2)")] int delayMs = 2)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "type_text",
            ["text"] = text,
            ["delay_ms"] = delayMs
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Send a single key code: Enter, Backspace, Ctrl-C, Home, Clear, LF")]
    public static async Task<string> SendKey(
        EmulatorClient client,
        [Description("Key name: Enter, Backspace, Ctrl-C, Home, Clear, LF")] string key)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "send_key",
            ["key"] = key
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Block until specified text appears on screen.")]
    public static async Task<string> WaitForPrompt(
        EmulatorClient client,
        [Description("Text to wait for (default: Ready)")] string text = "Ready",
        [Description("Timeout in milliseconds (default 5000)")] int timeoutMs = 5000)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "wait_ready",
            ["text"] = text,
            ["timeout_ms"] = timeoutMs
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Cold start the emulator (reset to boot).")]
    public static async Task<string> ColdStart(EmulatorClient client)
    {
        await client.SendAsync(new JsonObject
        {
            ["command"] = "cold_start"
        });
        var wait = await client.SendAsync(new JsonObject
        {
            ["command"] = "wait_ready",
            ["text"] = "Ready",
            ["timeout_ms"] = 5000
        });
        return wait.ToJsonString();
    }

    [McpServerTool, Description("Read a byte from emulator memory.")]
    public static async Task<string> Peek(
        EmulatorClient client,
        [Description("Memory address (0-65535)")] int address)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "peek",
            ["address"] = address
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Write a byte to emulator memory.")]
    public static async Task<string> Poke(
        EmulatorClient client,
        [Description("Memory address (0-65535)")] int address,
        [Description("Byte value (0-255)")] int value)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "poke",
            ["address"] = address,
            ["value"] = value
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Read the graphics bitmap layer as ASCII art. '#'=both halves set, '^'=top, 'v'=bottom, '.'=empty. 80x25 grid mapping the 320x200 pixel layer.")]
    public static async Task<string> ReadGraphics(EmulatorClient client)
    {
        var resp = await client.SendAsync(new JsonObject
        {
            ["command"] = "read_graphics"
        });
        return FormatGraphics(resp);
    }

    [McpServerTool, Description("Read all enabled sprites: position, priority, flags, and 16x16 multicolor shape (hex digit per pixel, 0=transparent).")]
    public static async Task<string> ReadSprites(EmulatorClient client)
    {
        var resp = await client.SendAsync(new JsonObject
        {
            ["command"] = "read_sprites"
        });
        return FormatSprites(resp);
    }

    [McpServerTool, Description("Plot a single pixel on the 320x200 graphics layer.")]
    public static async Task<string> GfxPlot(
        EmulatorClient client,
        [Description("X coordinate (0-319)")] int x,
        [Description("Y coordinate (0-199)")] int y,
        [Description("Color index (1-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_plot",
            ["x"] = x, ["y"] = y, ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Draw a line on the 320x200 graphics layer.")]
    public static async Task<string> GfxLine(
        EmulatorClient client,
        [Description("Start X (0-319)")] int x0,
        [Description("Start Y (0-199)")] int y0,
        [Description("End X (0-319)")] int x1,
        [Description("End Y (0-199)")] int y1,
        [Description("Color index (1-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_line",
            ["x0"] = x0, ["y0"] = y0, ["x1"] = x1, ["y1"] = y1, ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Draw a circle on the 320x200 graphics layer.")]
    public static async Task<string> GfxCircle(
        EmulatorClient client,
        [Description("Center X (0-319)")] int cx,
        [Description("Center Y (0-199)")] int cy,
        [Description("Radius in pixels")] int r,
        [Description("Color index (1-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_circle",
            ["cx"] = cx, ["cy"] = cy, ["r"] = r, ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Draw a rectangle outline on the 320x200 graphics layer.")]
    public static async Task<string> GfxRect(
        EmulatorClient client,
        [Description("Top-left X (0-319)")] int x0,
        [Description("Top-left Y (0-199)")] int y0,
        [Description("Bottom-right X (0-319)")] int x1,
        [Description("Bottom-right Y (0-199)")] int y1,
        [Description("Color index (1-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_rect",
            ["x0"] = x0, ["y0"] = y0, ["x1"] = x1, ["y1"] = y1, ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Draw a filled rectangle on the 320x200 graphics layer.")]
    public static async Task<string> GfxFill(
        EmulatorClient client,
        [Description("Top-left X (0-319)")] int x0,
        [Description("Top-left Y (0-199)")] int y0,
        [Description("Bottom-right X (0-319)")] int x1,
        [Description("Bottom-right Y (0-199)")] int y1,
        [Description("Color index (1-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_fill",
            ["x0"] = x0, ["y0"] = y0, ["x1"] = x1, ["y1"] = y1, ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Flood fill a region starting from (x,y) with the specified color.")]
    public static async Task<string> GfxPaint(
        EmulatorClient client,
        [Description("Seed X coordinate (0-319)")] int x,
        [Description("Seed Y coordinate (0-199)")] int y,
        [Description("Fill color index (1-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_paint",
            ["x"] = x, ["y"] = y, ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Clear the graphics layer (all pixels to 0).")]
    public static async Task<string> GfxClear(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_clear"
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set the current drawing color for graphics commands.")]
    public static async Task<string> GfxColor(
        EmulatorClient client,
        [Description("Color index (0-15)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "gfx_color",
            ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set the active font slot (0=CP437, 1=PETSCII Upper, 2=PETSCII Lower)")]
    public static async Task<string> SetFont(
        EmulatorClient client,
        [Description("Font slot index (0-7)")] int index)
    {
        await client.SendAsync(new JsonObject
        {
            ["command"] = "set_font",
            ["index"] = index
        });
        return $"Font set to slot {index}";
    }

    [McpServerTool, Description("Define one row (8 bytes) of a multicolor sprite shape. Each byte encodes 2 pixels: high nibble=left, low nibble=right. Color 0=transparent.")]
    public static async Task<string> SpriteDefineRow(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite,
        [Description("Row index (0-15)")] int row,
        [Description("8 bytes of pixel data (each byte = 2 pixels: high nibble left, low nibble right)")] int[] data)
    {
        var arr = new JsonArray();
        foreach (var b in data) arr.Add(b);
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_define_row",
            ["sprite"] = sprite,
            ["row"] = row,
            ["data"] = arr
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set a single pixel in a sprite shape. Color 0-15 (0=transparent).")]
    public static async Task<string> SpriteSetPixel(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite,
        [Description("X coordinate within sprite (0-15)")] int x,
        [Description("Y coordinate within sprite (0-15)")] int y,
        [Description("Color index (0-15, 0=transparent)")] int color)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_set_pixel",
            ["sprite"] = sprite,
            ["x"] = x,
            ["y"] = y,
            ["color"] = color
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set sprite screen position. X range 0-319, Y range 0-199 (graphics coordinates).")]
    public static async Task<string> SpritePosition(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite,
        [Description("X position (0-319)")] int x,
        [Description("Y position (0-199)")] int y)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_position",
            ["sprite"] = sprite,
            ["x"] = x,
            ["y"] = y
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Enable a sprite (make it visible).")]
    public static async Task<string> SpriteEnable(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_enable",
            ["sprite"] = sprite
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Disable a sprite (make it invisible).")]
    public static async Task<string> SpriteDisable(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_disable",
            ["sprite"] = sprite
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Clear a sprite's shape data (all pixels to 0/transparent).")]
    public static async Task<string> SpriteClear(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_clear",
            ["sprite"] = sprite
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set sprite display priority. 0=behind all, 1=between text and graphics, 2=in front of everything.")]
    public static async Task<string> SpritePriority(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite,
        [Description("Priority (0=behind all, 1=between text/gfx, 2=in front)")] int priority)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_priority",
            ["sprite"] = sprite,
            ["priority"] = priority
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set sprite flip flags. 0=none, 1=horizontal, 2=vertical, 3=both.")]
    public static async Task<string> SpriteFlip(
        EmulatorClient client,
        [Description("Sprite index (0-15)")] int sprite,
        [Description("Flip flags (0=none, 1=horizontal, 2=vertical, 3=both)")] int flags)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_flip",
            ["sprite"] = sprite,
            ["flags"] = flags
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Copy sprite shape from one sprite to another.")]
    public static async Task<string> SpriteCopy(
        EmulatorClient client,
        [Description("Source sprite index (0-15)")] int src,
        [Description("Destination sprite index (0-15)")] int dest)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sprite_copy",
            ["src"] = src,
            ["dest"] = dest
        });
        return result.ToJsonString();
    }

    // ── Music engine commands ─────────────────────────────────────────────

    [McpServerTool, Description("Define a SID instrument with waveform and ADSR envelope.")]
    public static async Task<string> Instrument(
        EmulatorClient client,
        [Description("Instrument slot (0-15)")] int id,
        [Description("SID waveform: 16=triangle, 32=sawtooth, 64=pulse, 128=noise")] int waveform,
        [Description("Attack (0-15)")] int a,
        [Description("Decay (0-15)")] int d,
        [Description("Sustain (0-15)")] int s,
        [Description("Release (0-15)")] int r)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "instrument",
            ["id"] = id, ["waveform"] = waveform,
            ["a"] = a, ["d"] = d, ["s"] = s, ["r"] = r
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Play a sound effect on the SID chip. Uses voice stealing with priority.")]
    public static async Task<string> Sound(
        EmulatorClient client,
        [Description("MIDI note number (0-127, 60=middle C)")] int note,
        [Description("Duration in 60Hz frames")] int duration,
        [Description("Instrument slot to use (0-15, default 0)")] int instrument = 0)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sound",
            ["note"] = note, ["duration"] = duration, ["instrument"] = instrument
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set the SID master volume level.")]
    public static async Task<string> Volume(
        EmulatorClient client,
        [Description("Volume level (0-15)")] int level)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "volume",
            ["level"] = level
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set an MML (Music Macro Language) sequence for a SID voice. Call music_play after setting all voices.")]
    public static async Task<string> MusicSeq(
        EmulatorClient client,
        [Description("Voice number (1-6)")] int voice,
        [Description("MML string (e.g. 'T120 O4 L4 CDEFG')")] string mml)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "music_seq",
            ["voice"] = voice, ["mml"] = mml
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Start music playback of loaded MML sequences.")]
    public static async Task<string> MusicPlay(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "music_play"
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Stop music playback and silence all music voices.")]
    public static async Task<string> MusicStop(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "music_stop"
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set the music tempo in beats per minute.")]
    public static async Task<string> MusicTempo(
        EmulatorClient client,
        [Description("Tempo in BPM (32-480)")] int bpm)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "music_tempo",
            ["bpm"] = bpm
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Enable or disable music looping.")]
    public static async Task<string> MusicLoop(
        EmulatorClient client,
        [Description("true to loop, false to play once")] bool on)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "music_loop",
            ["on"] = on
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Get music engine status: whether SFX and/or music are currently playing.")]
    public static async Task<string> MusicStatus(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "music_status"
        });
        return result.ToJsonString();
    }

    // ── SID file commands ────────────────────────────────────────────────

    [McpServerTool, Description("Load and play a SID music file. Sets up IRQ trampoline and timer. Returns init_address that should be called to initialize playback.")]
    public static async Task<string> SidPlay(
        EmulatorClient client,
        [Description("Path to the .sid file")] string path,
        [Description("Song number (1-based, default 1)")] int song = 1)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sid_play",
            ["path"] = path,
            ["song"] = song
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Stop SID playback and silence all voices.")]
    public static async Task<string> SidStop(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sid_stop"
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Read metadata from a SID file without loading it.")]
    public static async Task<string> SidInfo(
        EmulatorClient client,
        [Description("Path to the .sid file")] string path)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "sid_info",
            ["path"] = path
        });
        return result.ToJsonString();
    }

    // ── Debugger tools ─────────────────────────────────────────────────

    [McpServerTool, Description("Get CPU state: registers A, X, Y, SP, PC and all flags. Also reports whether the CPU is paused.")]
    public static async Task<string> DbgState(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_state" });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Pause CPU execution. Returns current CPU state.")]
    public static async Task<string> DbgPause(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_pause" });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Resume CPU execution after pause or breakpoint.")]
    public static async Task<string> DbgResume(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_resume" });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Execute one CPU instruction and re-pause. Returns CPU state and the next instruction disassembly.")]
    public static async Task<string> DbgStep(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_step" });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Set a breakpoint at a memory address. Optionally add a condition (e.g., register='A', op='==', value=1).")]
    public static async Task<string> DbgBreakSet(
        EmulatorClient client,
        [Description("Memory address to break at (0-65535)")] int address,
        [Description("Register to test: A, X, Y, SP (optional)")] string? register = null,
        [Description("Comparison operator: ==, !=, <, >, <=, >= (optional)")] string? op = null,
        [Description("Value to compare against (0-255, optional)")] int? value = null)
    {
        var req = new JsonObject { ["command"] = "dbg_break_set", ["address"] = address };
        if (register is not null) req["register"] = register;
        if (op is not null) req["op"] = op;
        if (value is not null) req["value"] = value;
        var result = await client.SendAsync(req);
        return result.ToJsonString();
    }

    [McpServerTool, Description("Remove a breakpoint at the specified address.")]
    public static async Task<string> DbgBreakClear(
        EmulatorClient client,
        [Description("Memory address of breakpoint to remove")] int address)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_break_clear", ["address"] = address });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Remove all breakpoints.")]
    public static async Task<string> DbgBreakClearAll(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_break_clear_all" });
        return result.ToJsonString();
    }

    [McpServerTool, Description("List all set breakpoints and their conditions.")]
    public static async Task<string> DbgBreakList(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_break_list" });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Read a range of memory bytes as a hex string. Max 4096 bytes.")]
    public static async Task<string> DbgReadMemory(
        EmulatorClient client,
        [Description("Start address (0-65535)")] int address,
        [Description("Number of bytes to read (1-4096)")] int length)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_read_memory", ["address"] = address, ["length"] = length });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Disassemble instructions starting at an address.")]
    public static async Task<string> DbgDisasm(
        EmulatorClient client,
        [Description("Start address (0-65535)")] int address,
        [Description("Number of instructions to disassemble (default 10)")] int count = 10)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_disasm", ["address"] = address, ["count"] = count });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Read the current CPU stack contents.")]
    public static async Task<string> DbgStack(EmulatorClient client)
    {
        var result = await client.SendAsync(new JsonObject { ["command"] = "dbg_stack" });
        return result.ToJsonString();
    }

    private static string FormatGraphics(JsonNode resp)
    {
        var sb = new StringBuilder();
        int mode = resp["mode"]?.GetValue<int>() ?? 0;
        sb.AppendLine($"Graphics mode: {mode}");
        if (resp["rows"] is JsonArray rows)
        {
            for (int i = 0; i < rows.Count; i++)
            {
                string? row = rows[i]?.GetValue<string>();
                if (!string.IsNullOrEmpty(row))
                    sb.AppendLine($"{i,2}: {row}");
            }
        }
        return sb.ToString();
    }

    private static string FormatSprites(JsonNode resp)
    {
        var sb = new StringBuilder();
        if (resp["sprites"] is not JsonArray sprites || sprites.Count == 0)
        {
            sb.AppendLine("No enabled sprites.");
            return sb.ToString();
        }
        foreach (var spr in sprites)
        {
            if (spr is null) continue;
            int idx = spr["index"]?.GetValue<int>() ?? 0;
            int x = spr["x"]?.GetValue<int>() ?? 0;
            int y = spr["y"]?.GetValue<int>() ?? 0;
            int flags = spr["flags"]?.GetValue<int>() ?? 0;
            int pri = spr["priority"]?.GetValue<int>() ?? 2;
            sb.AppendLine($"Sprite {idx}: pos=({x},{y}) priority={pri} flags={flags}");
            if (spr["shape"] is JsonArray shape)
            {
                foreach (var row in shape)
                    sb.AppendLine($"  {row?.GetValue<string>()}");
            }
        }
        return sb.ToString();
    }

    private static string ConvertToUpperOutsideQuotes(string text)
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

    [McpServerTool, Description("Save the current BASIC program to disk. Types SAVE \"name\" into the emulator.")]
    public static async Task<string> SaveProgram(
        EmulatorClient client,
        [Description("Program name (alphanumeric, dash, underscore, dot). .bas extension added automatically.")] string name)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "save_program",
            ["name"] = name
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Load a BASIC program from disk. Types LOAD \"name\" into the emulator. Replaces current program.")]
    public static async Task<string> LoadProgram(
        EmulatorClient client,
        [Description("Program name to load (without .bas extension).")] string name)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "load_program",
            ["name"] = name
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Load a BASIC program from a .bas text file on disk. Types NEW, then enters each line directly in the emulator. Use this for .bas files outside ~/e6502-programs.")]
    public static async Task<string> EnterBasicFromFile(
        EmulatorClient client,
        [Description("Absolute path to the .bas file (e.g. '/Users/barry/Git/e6502/docs/programs/reversi.bas')")] string filePath)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "enter_file",
            ["path"] = filePath
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("List all saved BASIC programs on disk.")]
    public static async Task<string> ListPrograms(EmulatorClient client)
    {
        var resp = await client.SendAsync(new JsonObject
        {
            ["command"] = "list_programs"
        });

        var sb = new StringBuilder();
        string? dir = resp["directory"]?.GetValue<string>();
        sb.AppendLine($"Save directory: {dir}");
        if (resp["programs"] is JsonArray programs)
        {
            if (programs.Count == 0)
                sb.AppendLine("No saved programs.");
            else
                foreach (var p in programs)
                    sb.AppendLine($"  {p?.GetValue<string>()}");
        }
        return sb.ToString();
    }

    // ── Network (NIC) ─────────────────────────────────────────────────────

    [McpServerTool, Description("Connect a NIC slot to a remote host. Poll nic_status to check connection.")]
    public static async Task<string> NicConnect(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot,
        [Description("Remote hostname or IP")] string host,
        [Description("Remote port")] int port)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_connect",
            ["slot"] = slot,
            ["host"] = host,
            ["port"] = port
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Disconnect a NIC slot.")]
    public static async Task<string> NicDisconnect(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_disconnect",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Send data on a NIC slot. Data is base64-encoded, max 256 bytes.")]
    public static async Task<string> NicSend(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot,
        [Description("Base64-encoded data to send (max 256 bytes)")] string data)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_send",
            ["slot"] = slot,
            ["data"] = data
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Receive a message from a NIC slot. Returns base64-encoded data.")]
    public static async Task<string> NicRecv(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_recv",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Listen for incoming connections on a NIC slot.")]
    public static async Task<string> NicListen(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot,
        [Description("Local port to listen on")] int port)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_listen",
            ["slot"] = slot,
            ["port"] = port
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Accept a pending connection on a listening NIC slot.")]
    public static async Task<string> NicAccept(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_accept",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Get the status of a NIC slot (connected, data ready, errors).")]
    public static async Task<string> NicStatus(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_status",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    private static string FormatScreen(JsonNode screen)
    {
        var sb = new StringBuilder();
        if (screen["lines"] is JsonArray lines)
        {
            for (int i = 0; i < lines.Count; i++)
            {
                string? line = lines[i]?.GetValue<string>();
                if (!string.IsNullOrEmpty(line))
                    sb.AppendLine($"{i,2}: {line}");
            }
        }
        int cx = screen["cursor_x"]?.GetValue<int>() ?? 0;
        int cy = screen["cursor_y"]?.GetValue<int>() ?? 0;
        sb.AppendLine($"Cursor: ({cx},{cy})");
        return sb.ToString();
    }
}
