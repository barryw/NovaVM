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

    [McpServerTool, Description("Read the graphics bitmap layer as ASCII art. '#'=both halves set, '^'=top, 'v'=bottom, '.'=empty. 80x25 grid mapping the 160x50 pixel layer.")]
    public static async Task<string> ReadGraphics(EmulatorClient client)
    {
        var resp = await client.SendAsync(new JsonObject
        {
            ["command"] = "read_graphics"
        });
        return FormatGraphics(resp);
    }

    [McpServerTool, Description("Read all enabled sprites: position, color, and 16x16 shape bitmap as ASCII art.")]
    public static async Task<string> ReadSprites(EmulatorClient client)
    {
        var resp = await client.SendAsync(new JsonObject
        {
            ["command"] = "read_sprites"
        });
        return FormatSprites(resp);
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
            int color = spr["color"]?.GetValue<int>() ?? 0;
            sb.AppendLine($"Sprite {idx}: pos=({x},{y}) color={color}");
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
