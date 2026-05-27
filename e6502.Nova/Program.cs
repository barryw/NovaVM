using System.Net.Sockets;
using System.Text;
using e6502.Storage;

if (args.Length < 1)
{
    PrintUsage();
    return 1;
}

// Remote verbs talk to NovaHost's HTTP file server. Accept --remote either
// before or after the command so the current `nova` CLI shape is natural.
string? remoteHost = ExtractRemoteHost(ref args, null);
if (args.Length < 1) { PrintUsage(); return 1; }

string verb = args[0].ToLowerInvariant();
if (verb is "help" or "--help" or "-h")
{
    PrintUsage();
    return 0;
}

return verb switch
{
    "create"     => DoCreate(args[1..]),
    "dir"        => DoDir(args[1..]),
    "info"       => DoInfo(args[1..]),
    "validate"   => DoValidate(args[1..]),
    "label"      => DoLabel(args[1..]),
    "import"     => DoImport(args[1..]),
    "export"     => DoExport(args[1..]),
    "delete"     => DoDelete(args[1..]),
    "mkdir"      => DoMkdir(args[1..]),
    "rmdir"      => DoRmdir(args[1..]),
    "tokenize"   => DoTokenize(args[1..]),
    "detokenize" => DoDetokenize(args[1..]),
    "ls"         => DoLs(args[1..], remoteHost),
    "put"        => DoPut(args[1..], remoteHost),
    "get"        => DoGet(args[1..], remoteHost),
    "rm"         => DoRm(args[1..], remoteHost),
    "device"     => DoDevice(args[1..], remoteHost),
    "drive" or "drives" => DoDrive(args[1..], remoteHost),
    "wifi"       => DoWifi(args[1..], remoteHost),
    "audio"      => DoAudio(args[1..], remoteHost),
    "keyboard"   => DoKeyboard(args[1..], remoteHost),
    "disk"       => DoDisk(args[1..], remoteHost),
    "rom"        => DoRom(args[1..], remoteHost),
    "soundfont"  => DoSoundfont(args[1..], remoteHost),
    "music"      => DoMusic(args[1..], remoteHost),
    "asset"      => DoAsset(args[1..], remoteHost),
    _            => UnknownVerb(verb),
};

// ===========================================================================
// Remote SD operations — talks to NovaHost's HTTP file server (port 80).
// Default endpoint base is http://<host>/sd/.
// ===========================================================================

static int DoDevice(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintDeviceUsage();
        return 1;
    }

    return args[0].ToLowerInvariant() switch
    {
        "status" => DoDeviceStatus(host),
        "reboot" or "restart" => DoDeviceReboot(host),
        _        => UnknownDeviceCommand(args[0]),
    };
}

static int DoDeviceStatus(string? host)
{
    if (host is null)
    {
        Console.Error.WriteLine("Usage: nova device status --remote <host>");
        return 1;
    }

    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(30) };
    try
    {
        foreach (string endpoint in new[] { "health", "sd-status" })
        {
            string url = $"http://{host}/{endpoint}";
            var resp = http.GetAsync(url).GetAwaiter().GetResult();
            if (!resp.IsSuccessStatusCode)
            {
                Console.Error.WriteLine($"GET {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
                return 1;
            }

            string body = resp.Content.ReadAsStringAsync().GetAwaiter().GetResult();
            Console.WriteLine($"{endpoint}: {body}");
        }

        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"device status: {ex.Message}");
        return 1;
    }
}

static int DoDeviceReboot(string? host)
{
    if (host is null)
    {
        PrintDeviceUsage();
        return 1;
    }

    return HttpPost(host, "/reboot", null, "device reboot");
}

static int UnknownDeviceCommand(string command)
{
    Console.Error.WriteLine($"Unknown device command: {command}");
    PrintDeviceUsage();
    return 1;
}

static void PrintDeviceUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova device status --remote <host>");
    Console.Error.WriteLine("  nova device reboot --remote <host>");
}

static int DoDrive(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintDriveUsage();
        return 1;
    }

    return args[0].ToLowerInvariant() switch
    {
        "list" or "ls"       => DoDriveList(host),
        "mount"              => DoDriveMount(args[1..], host),
        "unmount" or "umount" => DoDriveUnmount(args[1..], host),
        _                    => UnknownDriveCommand(args[0]),
    };
}

static int DoDriveList(string? host)
{
    if (host is null)
    {
        PrintDriveUsage();
        return 1;
    }

    string? body = HttpGetBody(host, "/drives", "drive list");
    if (body is null)
        return 1;

    Console.WriteLine($"Drive slots on {host}:");
    Console.WriteLine();
    Console.WriteLine("  Slot  Mounted  Current path                         Configured path");
    Console.WriteLine("  ----  -------  -----------------------------------  -----------------------------------");
    foreach (var entry in ParseJsonArray(body))
    {
        string slot = entry.GetValueOrDefault("slot") ?? "?";
        bool mounted = (entry.GetValueOrDefault("mounted") ?? "false") == "true";
        string current = entry.GetValueOrDefault("currentPath") ?? "";
        string configured = entry.GetValueOrDefault("configuredPath") ?? "";
        Console.WriteLine($"  {slot,-4}  {(mounted ? "yes" : "no"),-7}  {TruncateForTable(current, 35),-35}  {TruncateForTable(configured, 35),-35}");
    }

    return 0;
}

static int DoDriveMount(string[] args, string? host)
{
    if (host is null || args.Length < 2)
    {
        PrintDriveUsage();
        return 1;
    }

    string slot = args[0].ToLowerInvariant();
    if (!IsDriveSlot(slot))
    {
        Console.Error.WriteLine($"Invalid drive slot: {args[0]}");
        PrintDriveUsage();
        return 1;
    }

    string path = args[1];
    if (!path.StartsWith('/'))
        path = "/" + NormalizeSdRelativePath(path);
    if (!path.EndsWith(".ndi", StringComparison.OrdinalIgnoreCase))
    {
        Console.Error.WriteLine("Drive mount path must end in .ndi.");
        return 1;
    }

    string body = "{\"path\":" + JsonString(path) + "}";
    int rc = HttpPost(host, $"/drives/{slot}/mount", body, "drive mount", printResponse: false);
    if (rc == 0)
        Console.WriteLine($"Mounted {slot} -> {path}");
    return rc;
}

static int DoDriveUnmount(string[] args, string? host)
{
    if (host is null || args.Length < 1)
    {
        PrintDriveUsage();
        return 1;
    }

    string slot = args[0].ToLowerInvariant();
    if (!IsDriveSlot(slot))
    {
        Console.Error.WriteLine($"Invalid drive slot: {args[0]}");
        PrintDriveUsage();
        return 1;
    }

    int rc = HttpPost(host, $"/drives/{slot}/unmount", null, "drive unmount", printResponse: false);
    if (rc == 0)
        Console.WriteLine($"Unmounted {slot}");
    return rc;
}

static int UnknownDriveCommand(string command)
{
    Console.Error.WriteLine($"Unknown drive command: {command}");
    PrintDriveUsage();
    return 1;
}

static void PrintDriveUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova drive list --remote <host>");
    Console.Error.WriteLine("  nova drive mount <fd0|fd1|fd2|fd3|hd0|hd1> <sd-path.ndi> --remote <host>");
    Console.Error.WriteLine("  nova drive unmount <fd0|fd1|fd2|fd3|hd0|hd1> --remote <host>");
}

static bool IsDriveSlot(string slot) =>
    slot is "fd0" or "fd1" or "fd2" or "fd3" or "hd0" or "hd1";

static int DoWifi(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintWifiUsage();
        return 1;
    }

    return args[0].ToLowerInvariant() switch
    {
        "status" => DoWifiStatus(host),
        "scan" => DoWifiScan(host),
        "set" or "config" => DoWifiSet(args[1..], host),
        "connect" or "disconnect" or "reconnect" or "forget" => DoWifiAction(args[0], host),
        _ => UnknownWifiCommand(args[0]),
    };
}

static int DoWifiStatus(string? host)
{
    if (host is null)
    {
        PrintWifiUsage();
        return 1;
    }

    return HttpGet(host, "/wifi", "wifi status");
}

static int DoWifiScan(string? host)
{
    if (host is null)
    {
        PrintWifiUsage();
        return 1;
    }

    return HttpGet(host, "/wifi/scan", "wifi scan");
}

static int DoWifiAction(string action, string? host)
{
    if (host is null)
    {
        PrintWifiUsage();
        return 1;
    }

    return HttpPost(host, $"/wifi/{action.ToLowerInvariant()}", null, $"wifi {action}");
}

static int DoWifiSet(string[] args, string? host)
{
    if (host is null)
    {
        PrintWifiUsage();
        return 1;
    }

    var rest = args.ToList();
    string? ssid = TakeOptionValue(rest, "--ssid");
    string? password = TakeOptionValue(rest, "--password", "--pass");
    string? staticIp = TakeOptionValue(rest, "--static-ip", "--ip");
    string? gateway = TakeOptionValue(rest, "--gateway", "--gw");
    string? subnet = TakeOptionValue(rest, "--subnet", "--netmask");
    string? dns = TakeOptionValue(rest, "--dns");
    bool dhcp = TakeFlag(rest, "--dhcp");
    bool useStatic = TakeFlag(rest, "--static");

    if (dhcp && useStatic)
    {
        Console.Error.WriteLine("Choose either --dhcp or --static, not both.");
        return 1;
    }

    if (rest.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected wifi set argument: {rest[0]}");
        PrintWifiUsage();
        return 1;
    }

    if (ssid is null && password is null && staticIp is null && gateway is null &&
        subnet is null && dns is null && !dhcp && !useStatic)
    {
        Console.Error.WriteLine("wifi set needs at least one setting.");
        PrintWifiUsage();
        return 1;
    }

    if ((staticIp is not null || gateway is not null || subnet is not null || dns is not null) && !dhcp)
        useStatic = true;

    var body = new StringBuilder();
    body.Append('{');
    bool first = true;
    AppendJsonProperty(body, ref first, "ssid", ssid);
    AppendJsonProperty(body, ref first, "password", password);
    if (dhcp)
        AppendJsonBoolProperty(body, ref first, "dhcp", true);
    else if (useStatic)
        AppendJsonBoolProperty(body, ref first, "useStatic", true);
    AppendJsonProperty(body, ref first, "staticIp", staticIp);
    AppendJsonProperty(body, ref first, "gateway", gateway);
    AppendJsonProperty(body, ref first, "subnet", subnet);
    AppendJsonProperty(body, ref first, "dns", dns);
    body.Append('}');

    return HttpPut(host, "/wifi", body.ToString(), "wifi set");
}

static int UnknownWifiCommand(string command)
{
    Console.Error.WriteLine($"Unknown wifi command: {command}");
    PrintWifiUsage();
    return 1;
}

static void PrintWifiUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova wifi status --remote <host>");
    Console.Error.WriteLine("  nova wifi scan --remote <host>");
    Console.Error.WriteLine("  nova wifi set --remote <host> --ssid <ssid> [--password <password>] [--dhcp]");
    Console.Error.WriteLine("  nova wifi set --remote <host> --ssid <ssid> [--password <password>] --static --static-ip <ip> --gateway <ip> --subnet <mask> [--dns <ip>]");
    Console.Error.WriteLine("  nova wifi connect|disconnect|reconnect|forget --remote <host>");
}

static int DoAudio(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintAudioUsage();
        return 1;
    }

    return args[0].ToLowerInvariant() switch
    {
        "status" => DoAudioStatus(host),
        "stop" => DoAudioStop(host),
        _ => UnknownAudioCommand(args[0]),
    };
}

static int DoAudioStatus(string? host)
{
    if (host is null)
    {
        PrintAudioUsage();
        return 1;
    }

    return HttpGet(host, "/audio-status", "audio status");
}

static int DoAudioStop(string? host)
{
    if (host is null)
    {
        PrintAudioUsage();
        return 1;
    }

    return HttpPost(host, "/audio-stop", null, "audio stop");
}

static int UnknownAudioCommand(string command)
{
    Console.Error.WriteLine($"Unknown audio command: {command}");
    PrintAudioUsage();
    return 1;
}

static void PrintAudioUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova audio status --remote <host>");
    Console.Error.WriteLine("  nova audio stop --remote <host>");
}

static int DoKeyboard(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    var rest = args.ToList();
    bool help = rest.Count > 0 && rest[0].Equals("help", StringComparison.OrdinalIgnoreCase);
    help |= TakeFlag(rest, "--help", "-h");
    string? portOpt = TakeOptionValue(rest, "--port", "--debug-port");
    bool echo = TakeFlag(rest, "--echo");
    bool ctrlCQuits = TakeFlag(rest, "--ctrl-c-quits");

    if (help)
    {
        PrintKeyboardUsage();
        return 1;
    }

    if (host is null)
    {
        PrintKeyboardUsage();
        return 1;
    }

    if (rest.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected keyboard argument: {rest[0]}");
        PrintKeyboardUsage();
        return 1;
    }

    int port = 6503;
    if (portOpt is not null && (!int.TryParse(portOpt, out port) || port is < 1 or > 65535))
    {
        Console.Error.WriteLine($"Invalid debug TCP port: {portOpt}");
        return 1;
    }

    return RunKeyboardBridge(host, port, echo, ctrlCQuits);
}

static int RunKeyboardBridge(string host, int port, bool echo, bool ctrlCQuits)
{
    if (Console.IsInputRedirected)
    {
        Console.Error.WriteLine("keyboard needs an interactive terminal.");
        return 1;
    }

    bool oldTreatControlCAsInput = Console.TreatControlCAsInput;
    string? oldTerminalMode = null;
    Stream? input = null;
    KeyboardInputReader? rawInput = null;
    bool started = false;
    try
    {
        using var client = new TcpClient();
        using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(15));
        client.ConnectAsync(host, port, cts.Token).GetAwaiter().GetResult();

        using NetworkStream stream = client.GetStream();
        stream.ReadTimeout = 15000;
        stream.WriteTimeout = 15000;
        using var reader = new StreamReader(stream, Encoding.UTF8, detectEncodingFromByteOrderMarks: false,
                                            bufferSize: 4096, leaveOpen: true);
        using var writer = new StreamWriter(stream, new UTF8Encoding(false), bufferSize: 4096,
                                            leaveOpen: true)
        {
            AutoFlush = true,
            NewLine = "\n",
        };

        Console.TreatControlCAsInput = true;
        oldTerminalMode = TryEnterRawTerminalMode();
        input = OpenKeyboardInput(oldTerminalMode is not null);
        if (oldTerminalMode is not null)
            rawInput = new KeyboardInputReader(input);
        started = true;
        Console.Error.WriteLine($"Nova WiFi keyboard bridge: {host}:{port}");
        Console.Error.WriteLine("Type normally. Ctrl-] quits. Ctrl-C is sent to Nova.");

        while (true)
        {
            bool mapped = oldTerminalMode is not null
                ? TryMapRawInput(rawInput!, ctrlCQuits, out string text, out bool shouldQuit)
                : TryMapConsoleKey(Console.ReadKey(intercept: true), ctrlCQuits, out text, out shouldQuit);
            if (!mapped)
                continue;

            if (shouldQuit)
                break;

            SendDebugKey(writer, reader, text);
            if (echo)
                EchoKeyboardText(text);
        }

        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"keyboard: {ex.Message}");
        return 1;
    }
    finally
    {
        rawInput?.Dispose();
        input?.Dispose();
        RestoreTerminalMode(oldTerminalMode);
        Console.TreatControlCAsInput = oldTreatControlCAsInput;
        if (started)
            Console.Error.WriteLine("\nNova WiFi keyboard bridge closed.");
    }
}

static bool TryMapConsoleKey(
    ConsoleKeyInfo key,
    bool ctrlCQuits,
    out string text,
    out bool shouldQuit)
{
    shouldQuit = false;
    text = "";

    bool control = key.Modifiers.HasFlag(ConsoleModifiers.Control);
    if (key.KeyChar == '\x1D' ||
        (control && key.KeyChar == ']') ||
        (control && key.Key == ConsoleKey.Oem6) ||
        (key.Key == ConsoleKey.Oem6 && key.KeyChar == '\0'))
    {
        shouldQuit = true;
        return true;
    }

    if (ctrlCQuits && key.KeyChar == '\x03')
    {
        shouldQuit = true;
        return true;
    }

    text = key.Key switch
    {
        ConsoleKey.Enter      => "\r",
        ConsoleKey.Backspace  => "\b",
        ConsoleKey.Tab        => "\t",
        ConsoleKey.Delete     => "\x7F",
        ConsoleKey.Escape     => "\x03",
        ConsoleKey.F1         => "\x03",
        ConsoleKey.LeftArrow  => "\x1C",
        ConsoleKey.RightArrow => "\x1D",
        ConsoleKey.UpArrow    => "\x1E",
        ConsoleKey.DownArrow  => "\x1F",
        _                     => "",
    };

    if (text.Length > 0)
        return true;

    if (key.KeyChar is >= '\x01' and <= '\x1F')
    {
        text = key.KeyChar.ToString();
        return true;
    }

    if (key.KeyChar is >= ' ' and <= '~')
    {
        text = key.KeyChar.ToString();
        return true;
    }

    return false;
}

static bool TryMapRawInput(
    KeyboardInputReader input,
    bool ctrlCQuits,
    out string text,
    out bool shouldQuit)
{
    shouldQuit = false;
    text = "";

    if (!input.TryRead(out int value))
        return false;

    byte b = (byte)value;
    if (b == 0x1D)
    {
        shouldQuit = true;
        return true;
    }

    if (ctrlCQuits && b == 0x03)
    {
        shouldQuit = true;
        return true;
    }

    text = b switch
    {
        0x0A or 0x0D => "\r",
        0x08 or 0x7F => "\b",
        0x09         => "\t",
        0x03         => "\x03",
        0x1B         => ReadEscapeSequence(input),
        >= 0x01 and <= 0x1F => ((char)b).ToString(),
        >= 0x20 and <= 0x7E => ((char)b).ToString(),
        _            => "",
    };

    return text.Length > 0;
}

static string ReadEscapeSequence(KeyboardInputReader input)
{
    if (!TryReadPendingInputByte(input, out int second))
        return "\x03";

    if (second == 'O')
    {
        return TryReadPendingInputByte(input, out int final)
            ? MapArrowEscapeFinal(final)
            : "\x03";
    }

    if (second != '[')
        return "\x03";

    for (int i = 0; i < 16; i++)
    {
        if (!TryReadPendingInputByte(input, out int value))
            return "\x03";

        if (value >= 0x40 && value <= 0x7E)
            return MapArrowEscapeFinal(value);
    }

    return "\x03";
}

static string MapArrowEscapeFinal(int value)
{
    return value switch
    {
        'D' => "\x1C",
        'C' => "\x1D",
        'A' => "\x1E",
        'B' => "\x1F",
        _   => "\x03",
    };
}

static bool TryReadPendingInputByte(KeyboardInputReader input, out int value)
{
    return input.TryRead(out value, timeoutMs: 100);
}

static string? TryEnterRawTerminalMode()
{
    if (OperatingSystem.IsWindows())
        return null;

    try
    {
        string mode = RunSttyCapture("-g").Trim();
        if (string.IsNullOrWhiteSpace(mode))
            return null;
        RunStty("raw", "-echo", "min", "1", "time", "0");
        return mode;
    }
    catch
    {
        return null;
    }
}

static void RestoreTerminalMode(string? mode)
{
    if (string.IsNullOrWhiteSpace(mode))
        return;

    try
    {
        RunStty(mode);
    }
    catch
    {
        // The bridge is exiting; there is no better recovery path here.
    }
}

static Stream OpenKeyboardInput(bool useTerminal)
{
    if (useTerminal)
    {
        try
        {
            return new FileStream("/dev/tty", FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
        }
        catch
        {
            // Fall through to stdin; Console.ReadKey remains the mapper.
        }
    }

    return Console.OpenStandardInput();
}

static string RunSttyCapture(params string[] args)
{
    using var process = StartStty(SttyTerminalArgs(args), redirectOutput: true);
    string output = process.StandardOutput.ReadToEnd();
    process.WaitForExit();
    if (process.ExitCode != 0)
        throw new InvalidOperationException("stty failed");
    return output;
}

static void RunStty(params string[] args)
{
    using var process = StartStty(SttyTerminalArgs(args), redirectOutput: false);
    process.WaitForExit();
    if (process.ExitCode != 0)
        throw new InvalidOperationException("stty failed");
}

static string[] SttyTerminalArgs(string[] args)
{
    string flag = OperatingSystem.IsMacOS() || OperatingSystem.IsFreeBSD()
        ? "-f"
        : "-F";
    return [flag, "/dev/tty", .. args];
}

static System.Diagnostics.Process StartStty(string[] args, bool redirectOutput)
{
    var psi = new System.Diagnostics.ProcessStartInfo("stty")
    {
        RedirectStandardOutput = redirectOutput,
        RedirectStandardError = true,
        UseShellExecute = false,
    };
    foreach (string arg in args)
        psi.ArgumentList.Add(arg);
    return System.Diagnostics.Process.Start(psi)
           ?? throw new InvalidOperationException("could not start stty");
}

static void SendDebugKey(StreamWriter writer, StreamReader reader, string text)
{
    string key = text switch
    {
        "\r"   => "ENTER",
        "\b"   => "BACKSPACE",
        "\x7F" => "BACKSPACE",
        "\x03" => "CTRL-C",
        _      => text,
    };
    writer.WriteLine("{\"command\":\"send_key\",\"key\":" + JsonString(key) + "}");
    string? response = reader.ReadLine();
    if (string.IsNullOrWhiteSpace(response))
        throw new IOException("empty response from NovaHost debug server");

    if (!DebugResponseOk(response))
        throw new IOException($"NovaHost rejected key input: {response}");
}

static bool DebugResponseOk(string response) =>
    response.Contains("\"ok\":true", StringComparison.Ordinal) ||
    response.Contains("\"ok\": true", StringComparison.Ordinal);

static void EchoKeyboardText(string text)
{
    foreach (char ch in text)
    {
        switch (ch)
        {
            case '\r':
                Console.WriteLine();
                break;
            case '\b':
            case '\x7F':
                Console.Write("\b \b");
                break;
            case '\t':
                Console.Write('\t');
                break;
            default:
                if (ch >= ' ')
                    Console.Write(ch);
                break;
        }
    }
}

static void PrintKeyboardUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova keyboard --remote <host> [--port 6503] [--echo] [--ctrl-c-quits]");
}

static int DoDisk(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintDiskUsage();
        return 1;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    bool hard = TakeFlag(rest, "--hard", "--hd");
    bool floppy = TakeFlag(rest, "--floppy", "--fd");
    string? nameOpt = TakeOptionValue(rest, "--name", "-n");
    string? pathOpt = TakeOptionValue(rest, "--path");

    if (hard && floppy)
    {
        Console.Error.WriteLine("Choose either --hard or --floppy, not both.");
        return 1;
    }

    return command switch
    {
        "list" or "ls"       => DoDiskList(rest, host, hard, floppy, pathOpt),
        "upload" or "put"    => DoDiskUpload(rest, host, hard, floppy, nameOpt, pathOpt),
        "download" or "get"  => DoDiskDownload(rest, host, hard, floppy, pathOpt),
        "delete" or "rm"     => DoDiskDelete(rest, host, hard, floppy, pathOpt),
        _                    => UnknownDiskCommand(command),
    };
}

static int DoDiskList(List<string> args, string? host, bool hard, bool floppy, string? pathOpt)
{
    string dir = pathOpt is not null
        ? NormalizeSdRelativePath(pathOpt)
        : hard ? "disks/hard"
        : floppy ? "disks/floppy"
        : "disks";

    return DoLs(new[] { dir }, host);
}

static int DoDiskUpload(List<string> args, string? host, bool hard, bool floppy, string? nameOpt, string? pathOpt)
{
    if (args.Count < 1)
    {
        PrintDiskUsage();
        return 1;
    }

    string local = args[0];
    if (!File.Exists(local))
    {
        Console.Error.WriteLine($"local file not found: {local}");
        return 1;
    }

    if (!Path.GetExtension(local).Equals(".ndi", StringComparison.OrdinalIgnoreCase))
    {
        Console.Error.WriteLine("disk upload expects a .ndi image.");
        return 1;
    }

    string remote = pathOpt is not null
        ? NormalizeSdRelativePath(pathOpt)
        : ResolveDiskUploadPath(local, nameOpt ?? (args.Count > 1 ? args[1] : Path.GetFileName(local)), hard, floppy);

    return DoPut(new[] { local, remote }, host);
}

static int DoDiskDownload(List<string> args, string? host, bool hard, bool floppy, string? pathOpt)
{
    string? source = pathOpt ?? (args.Count > 0 ? args[0] : null);
    if (source is null)
    {
        PrintDiskUsage();
        return 1;
    }

    string remote = ResolveDiskReferencePath(source, hard, floppy);
    string[] getArgs;
    if (pathOpt is not null)
        getArgs = args.Count > 0 ? new[] { remote, args[0] } : new[] { remote };
    else
        getArgs = args.Count > 1 ? new[] { remote, args[1] } : new[] { remote };

    return DoGet(getArgs, host);
}

static int DoDiskDelete(List<string> args, string? host, bool hard, bool floppy, string? pathOpt)
{
    string? source = pathOpt ?? (args.Count > 0 ? args[0] : null);
    if (source is null)
    {
        PrintDiskUsage();
        return 1;
    }

    string remote = ResolveDiskReferencePath(source, hard, floppy);
    return DoRm(new[] { remote }, host);
}

static int UnknownDiskCommand(string command)
{
    Console.Error.WriteLine($"Unknown disk command: {command}");
    PrintDiskUsage();
    return 1;
}

static void PrintDiskUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova disk list --remote <host> [--hard|--floppy]");
    Console.Error.WriteLine("  nova disk upload <file.ndi> --remote <host> [--hard|--floppy] [--name <name>]");
    Console.Error.WriteLine("  nova disk download <name-or-path> --remote <host> [local-path] [--hard|--floppy]");
    Console.Error.WriteLine("  nova disk delete <name-or-path> --remote <host> [--hard|--floppy]");
}

static int DoRom(string[] args, string? host) =>
    DoManagedRemoteAsset(args, host, "rom", "roms", NormalizeRomUploadName);

static int DoSoundfont(string[] args, string? host) =>
    DoManagedRemoteAsset(args, host, "soundfont", "soundfonts", NormalizeSoundfontUploadName);

static int DoMusic(string[] args, string? host) =>
    DoManagedRemoteAsset(args, host, "music", "music", NormalizeMusicUploadName);

static int DoAsset(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintAssetUsage();
        return 1;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    string? type = TakeOptionValue(rest, "--type", "--kind", "-t");
    string? nameOpt = TakeOptionValue(rest, "--name", "-n");
    string? pathOpt = TakeOptionValue(rest, "--path");
    _ = TakeOptionValue(rest, "--bank", "--soundbank", "--soundfont");

    if (command is "list" or "ls" && type is null)
        return DoLs(new[] { "assets" }, host);

    if (type is null && pathOpt is null && (command is "upload" or "put"))
    {
        Console.Error.WriteLine("asset upload needs --type <boot|fonts|sid|...> so the CLI can place it correctly.");
        PrintAssetUsage();
        return 1;
    }

    string baseDir = type is null ? "assets" : AssetDirectoryFor(type);
    return DoManagedRemoteAssetCore(command, rest, host, "asset", baseDir, nameOpt, pathOpt, KeepUploadName);
}

static int DoManagedRemoteAsset(
    string[] args,
    string? host,
    string kind,
    string baseDir,
    Func<string, string, (bool Ok, string Name, string? Error)> normalizeUploadName)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintManagedAssetUsage(kind);
        return 1;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    string? nameOpt = TakeOptionValue(rest, "--name", "-n");
    string? pathOpt = TakeOptionValue(rest, "--path");
    _ = TakeOptionValue(rest, "--bank", "--soundbank", "--soundfont");
    return DoManagedRemoteAssetCore(command, rest, host, kind, baseDir, nameOpt, pathOpt, normalizeUploadName);
}

static int DoManagedRemoteAssetCore(
    string command,
    List<string> args,
    string? host,
    string kind,
    string baseDir,
    string? nameOpt,
    string? pathOpt,
    Func<string, string, (bool Ok, string Name, string? Error)> normalizeUploadName)
{
    return command switch
    {
        "list" or "ls"       => DoLs(new[] { baseDir }, host),
        "upload" or "put"    => DoManagedUpload(args, host, kind, baseDir, nameOpt, pathOpt, normalizeUploadName),
        "download" or "get"  => DoManagedDownload(args, host, kind, baseDir, pathOpt),
        "delete" or "rm"     => DoManagedDelete(args, host, kind, baseDir, pathOpt),
        _                    => UnknownManagedCommand(kind, command),
    };
}

static int DoManagedUpload(
    List<string> args,
    string? host,
    string kind,
    string baseDir,
    string? nameOpt,
    string? pathOpt,
    Func<string, string, (bool Ok, string Name, string? Error)> normalizeUploadName)
{
    _ = TakeOptionValue(args, "--bank", "--soundbank", "--soundfont");
    if (args.Count < 1)
    {
        PrintManagedAssetUsage(kind);
        return 1;
    }

    string local = args[0];
    if (!File.Exists(local))
    {
        Console.Error.WriteLine($"local file not found: {local}");
        return 1;
    }

    string remote;
    if (pathOpt is not null)
    {
        remote = NormalizeSdRelativePath(pathOpt);
    }
    else
    {
        string requestedName = nameOpt ?? (args.Count > 1 ? args[1] : Path.GetFileName(local));
        if (LooksExplicitRemotePath(requestedName))
        {
            remote = NormalizeSdRelativePath(requestedName);
        }
        else
        {
            var normalized = normalizeUploadName(local, requestedName);
            if (!normalized.Ok)
            {
                Console.Error.WriteLine(normalized.Error);
                return 1;
            }

            remote = JoinRemotePath(baseDir, normalized.Name);
        }
    }

    return PutFile(local, remote, host);
}

static int DoManagedDownload(List<string> args, string? host, string kind, string baseDir, string? pathOpt)
{
    string? source = pathOpt ?? (args.Count > 0 ? args[0] : null);
    if (source is null)
    {
        PrintManagedAssetUsage(kind);
        return 1;
    }

    string remote = ResolveManagedReferencePath(baseDir, source);
    string[] getArgs;
    if (pathOpt is not null)
        getArgs = args.Count > 0 ? new[] { remote, args[0] } : new[] { remote };
    else
        getArgs = args.Count > 1 ? new[] { remote, args[1] } : new[] { remote };

    return DoGet(getArgs, host);
}

static int DoManagedDelete(List<string> args, string? host, string kind, string baseDir, string? pathOpt)
{
    string? source = pathOpt ?? (args.Count > 0 ? args[0] : null);
    if (source is null)
    {
        PrintManagedAssetUsage(kind);
        return 1;
    }

    string remote = ResolveManagedReferencePath(baseDir, source);
    return DoRm(new[] { remote }, host);
}

static int UnknownManagedCommand(string kind, string command)
{
    Console.Error.WriteLine($"Unknown {kind} command: {command}");
    PrintManagedAssetUsage(kind);
    return 1;
}

static void PrintManagedAssetUsage(string kind)
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine($"  nova {kind} list --remote <host>");
    Console.Error.WriteLine($"  nova {kind} upload <file> --remote <host> [--name <name>]");
    Console.Error.WriteLine($"  nova {kind} download <name-or-path> --remote <host> [local-path]");
    Console.Error.WriteLine($"  nova {kind} delete <name-or-path> --remote <host>");
}

static void PrintAssetUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova asset list --remote <host> [--type <boot|fonts|sid|...>]");
    Console.Error.WriteLine("  nova asset upload <file> --remote <host> --type <boot|fonts|sid|...> [--name <name>]");
    Console.Error.WriteLine("  nova asset download <name-or-path> --remote <host> [--type <boot|fonts|sid|...>] [local-path]");
    Console.Error.WriteLine("  nova asset delete <name-or-path> --remote <host> [--type <boot|fonts|sid|...>]");
}

static int DoLs(string[] args, string? host)
{
    if (host is null) {
        Console.Error.WriteLine("Usage: nova --remote=<host> ls [path]");
        return 1;
    }
    string subdir = (args.Length > 0) ? NormalizeSdRelativePath(args[0]) : "";
    if (subdir.Length > 0 && !subdir.EndsWith('/')) subdir += "/";
    string url = SdUrl(host, subdir);
    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(30) };
    try {
        var resp = http.GetAsync(url).GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode) {
            Console.Error.WriteLine($"GET {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            return 1;
        }
        string body = resp.Content.ReadAsStringAsync().GetAwaiter().GetResult();
        // Body is a JSON array of {name,size,dir}. Print a simple table
        // without pulling in System.Text.Json — naive split is fine for
        // the simple shape NovaHost emits.
        Console.WriteLine($"Listing of /sd/{subdir} on {host}:");
        Console.WriteLine();
        Console.WriteLine("  Name                              Size      Type");
        Console.WriteLine("  --------------------------------  --------  ----");
        foreach (var entry in ParseJsonArray(body)) {
            string name = entry.GetValueOrDefault("name") ?? "?";
            string size = entry.GetValueOrDefault("size") ?? "0";
            bool isDir = (entry.GetValueOrDefault("dir") ?? "false") == "true";
            string sizeStr = isDir ? "<DIR>" : size;
            string typeStr = isDir ? "DIR" : "FILE";
            Console.WriteLine($"  {name,-32}  {sizeStr,8}  {typeStr}");
        }
        return 0;
    } catch (Exception ex) {
        Console.Error.WriteLine($"ls: {ex.Message}");
        return 1;
    }
}

static int DoPut(string[] args, string? host)
{
    var rest = args.ToList();
    _ = TakeOptionValue(rest, "--bank", "--soundbank", "--soundfont");
    if (host is null || rest.Count < 1) {
        Console.Error.WriteLine("Usage: nova --remote=<host> put <local-path> [remote-path]");
        return 1;
    }
    string local  = rest[0];
    string remote = (rest.Count > 1) ? NormalizeSdRelativePath(rest[1]) : Path.GetFileName(local);
    return PutFile(local, remote, host);
}

static int PutFile(string local, string remote, string? host)
{
    if (host is null)
    {
        Console.Error.WriteLine("Usage: nova --remote=<host> put <local-path> [remote-path]");
        return 1;
    }
    if (!File.Exists(local)) {
        Console.Error.WriteLine($"local file not found: {local}");
        return 1;
    }

    byte[]? uploadBytes = null;
    long uploadLength;
    string displayLocal = local;
    try
    {
        if (IsMidiPath(local))
        {
            var compiled = CompileMidiToNovaStream(local);
            uploadBytes = compiled.Data;
            uploadLength = uploadBytes.Length;
            remote = NormalizeMusicRemotePath(remote);
            displayLocal = $"{local} [compiled to .nms]";
            Console.WriteLine(
                $"Compiled MIDI {local} -> {remote} ({compiled.EventCount} events, {compiled.Data.Length} bytes)");
        }
        else
        {
            remote = NormalizeSdRelativePath(remote);
            uploadLength = new FileInfo(local).Length;
        }
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"music compile: {ex.Message}");
        return 1;
    }

    string url = SdUrl(host, remote);
    using var http = new HttpClient { Timeout = TimeSpan.FromMinutes(30) };
    try {
        using Stream stream = uploadBytes is not null
            ? new MemoryStream(uploadBytes)
            : new FileStream(local, FileMode.Open, FileAccess.Read);
        var content = new StreamContent(stream);
        // Mark the upload as opaque bytes so NovaHost stores the body exactly.
        content.Headers.ContentType = new System.Net.Http.Headers.MediaTypeHeaderValue("application/octet-stream");
        var resp = http.PutAsync(url, content).GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode) {
            Console.Error.WriteLine($"PUT {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            return 1;
        }
        Console.WriteLine($"PUT {displayLocal} → {url} ({uploadLength} bytes) OK");
        return 0;
    } catch (Exception ex) {
        Console.Error.WriteLine($"put: {ex.Message}");
        return 1;
    }
}

static int DoGet(string[] args, string? host)
{
    if (host is null || args.Length < 1) {
        Console.Error.WriteLine("Usage: nova --remote=<host> get <remote-path> [local-path]");
        return 1;
    }
    string remote = NormalizeSdRelativePath(args[0]);
    string local  = (args.Length > 1) ? args[1] : Path.GetFileName(remote);
    string url = SdUrl(host, remote);
    using var http = new HttpClient { Timeout = TimeSpan.FromMinutes(30) };
    try {
        using var resp = http.GetAsync(url, HttpCompletionOption.ResponseHeadersRead)
                              .GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode) {
            Console.Error.WriteLine($"GET {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            return 1;
        }
        using var src = resp.Content.ReadAsStream();
        using var fs  = new FileStream(local, FileMode.Create, FileAccess.Write);
        src.CopyTo(fs);
        Console.WriteLine($"GET {url} → {local} ({fs.Length} bytes) OK");
        return 0;
    } catch (Exception ex) {
        Console.Error.WriteLine($"get: {ex.Message}");
        return 1;
    }
}

static int DoRm(string[] args, string? host)
{
    if (host is null || args.Length < 1) {
        Console.Error.WriteLine("Usage: nova --remote=<host> rm <remote-path>");
        return 1;
    }
    string remote = NormalizeSdRelativePath(args[0]);
    string url = SdUrl(host, remote);
    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(30) };
    try {
        var resp = http.DeleteAsync(url).GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode) {
            Console.Error.WriteLine($"DELETE {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            return 1;
        }
        Console.WriteLine($"DELETE {url} OK");
        return 0;
    } catch (Exception ex) {
        Console.Error.WriteLine($"rm: {ex.Message}");
        return 1;
    }
}

static int HttpGet(string host, string endpoint, string operation)
{
    string? body = HttpGetBody(host, endpoint, operation);
    if (body is null)
        return 1;

    Console.WriteLine(body);
    return 0;
}

static string? HttpGetBody(string host, string endpoint, string operation)
{
    string url = HostUrl(host, endpoint);
    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(30) };
    try
    {
        var resp = http.GetAsync(url).GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode)
        {
            Console.Error.WriteLine($"GET {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            return null;
        }

        return resp.Content.ReadAsStringAsync().GetAwaiter().GetResult();
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"{operation}: {ex.Message}");
        return null;
    }
}

static int HttpPost(string host, string endpoint, string? jsonBody, string operation, bool printResponse = true)
{
    string url = HostUrl(host, endpoint);
    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(30) };
    try
    {
        using HttpContent content = jsonBody is null
            ? new ByteArrayContent(Array.Empty<byte>())
            : new StringContent(jsonBody, Encoding.UTF8, "application/json");
        var resp = http.PostAsync(url, content).GetAwaiter().GetResult();
        string body = resp.Content.ReadAsStringAsync().GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode)
        {
            Console.Error.WriteLine($"POST {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            if (!string.IsNullOrWhiteSpace(body))
                Console.Error.WriteLine(body);
            return 1;
        }

        if (printResponse && !string.IsNullOrWhiteSpace(body))
            Console.WriteLine(body);
        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"{operation}: {ex.Message}");
        return 1;
    }
}

static int HttpPut(string host, string endpoint, string jsonBody, string operation)
{
    string url = HostUrl(host, endpoint);
    using var http = new HttpClient { Timeout = TimeSpan.FromSeconds(30) };
    try
    {
        using var content = new StringContent(jsonBody, Encoding.UTF8, "application/json");
        var resp = http.PutAsync(url, content).GetAwaiter().GetResult();
        string body = resp.Content.ReadAsStringAsync().GetAwaiter().GetResult();
        if (!resp.IsSuccessStatusCode)
        {
            Console.Error.WriteLine($"PUT {url}: {(int)resp.StatusCode} {resp.ReasonPhrase}");
            if (!string.IsNullOrWhiteSpace(body))
                Console.Error.WriteLine(body);
            return 1;
        }

        if (!string.IsNullOrWhiteSpace(body))
            Console.WriteLine(body);
        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"{operation}: {ex.Message}");
        return 1;
    }
}

static string HostUrl(string host, string endpoint)
{
    string normalized = endpoint.StartsWith('/') ? endpoint[1..] : endpoint;
    return $"http://{host}/{normalized}";
}

static string TruncateForTable(string value, int max)
{
    if (value.Length <= max)
        return value;
    if (max <= 1)
        return value[..max];
    return value[..(max - 1)] + "…";
}

static void AppendJsonProperty(StringBuilder body, ref bool first, string name, string? value)
{
    if (value is null)
        return;
    AppendJsonSeparator(body, ref first);
    body.Append(JsonString(name));
    body.Append(':');
    body.Append(JsonString(value));
}

static void AppendJsonBoolProperty(StringBuilder body, ref bool first, string name, bool value)
{
    AppendJsonSeparator(body, ref first);
    body.Append(JsonString(name));
    body.Append(':');
    body.Append(value ? "true" : "false");
}

static void AppendJsonSeparator(StringBuilder body, ref bool first)
{
    if (first)
    {
        first = false;
        return;
    }

    body.Append(',');
}

static string JsonString(string value)
{
    var sb = new StringBuilder(value.Length + 2);
    sb.Append('"');
    foreach (char ch in value)
    {
        switch (ch)
        {
            case '"':
                sb.Append("\\\"");
                break;
            case '\\':
                sb.Append("\\\\");
                break;
            case '\b':
                sb.Append("\\b");
                break;
            case '\f':
                sb.Append("\\f");
                break;
            case '\n':
                sb.Append("\\n");
                break;
            case '\r':
                sb.Append("\\r");
                break;
            case '\t':
                sb.Append("\\t");
                break;
            default:
                if (ch < 0x20)
                    sb.Append("\\u").Append(((int)ch).ToString("X4"));
                else
                    sb.Append(ch);
                break;
        }
    }
    sb.Append('"');
    return sb.ToString();
}

// Tiny ad-hoc JSON parser for the array-of-flat-objects we get from
// NovaHost's listing endpoint. Avoids adding a System.Text.Json dep.
static IEnumerable<Dictionary<string, string>> ParseJsonArray(string json)
{
    int i = 0;
    while (i < json.Length && json[i] != '[') i++;
    i++;
    while (i < json.Length) {
        while (i < json.Length && (json[i] == ',' || char.IsWhiteSpace(json[i]))) i++;
        if (i >= json.Length || json[i] == ']') yield break;
        if (json[i] != '{') yield break;
        i++;
        var dict = new Dictionary<string, string>();
        while (i < json.Length && json[i] != '}') {
            // skip whitespace, comma
            while (i < json.Length && (json[i] == ',' || char.IsWhiteSpace(json[i]))) i++;
            if (json[i] != '"') break;
            i++;
            int keyStart = i;
            while (i < json.Length && json[i] != '"') i++;
            string key = json.Substring(keyStart, i - keyStart);
            i++; // closing quote
            while (i < json.Length && (json[i] == ':' || char.IsWhiteSpace(json[i]))) i++;
            string val;
            if (json[i] == '"') {
                i++;
                int vs = i;
                while (i < json.Length && json[i] != '"') i++;
                val = json.Substring(vs, i - vs);
                i++;
            } else {
                int vs = i;
                while (i < json.Length && json[i] != ',' && json[i] != '}'
                       && !char.IsWhiteSpace(json[i])) i++;
                val = json.Substring(vs, i - vs);
            }
            dict[key] = val;
        }
        if (i < json.Length && json[i] == '}') i++;
        yield return dict;
    }
}

static string? ExtractRemoteHost(ref string[] args, string? fallback)
{
    string? host = fallback;
    var filtered = new List<string>();

    for (int i = 0; i < args.Length; i++)
    {
        string arg = args[i];
        if (arg.StartsWith("--remote=", StringComparison.Ordinal))
        {
            string value = arg["--remote=".Length..];
            if (!string.IsNullOrWhiteSpace(value))
                host = value;
            continue;
        }

        if (arg == "--remote")
        {
            if (i + 1 < args.Length && !args[i + 1].StartsWith("--"))
            {
                host = args[++i];
                continue;
            }
        }

        filtered.Add(arg);
    }

    args = filtered.ToArray();
    return host;
}

static string? TakeOptionValue(List<string> args, params string[] names)
{
    for (int i = 0; i < args.Count; i++)
    {
        foreach (string name in names)
        {
            if (args[i].StartsWith(name + "=", StringComparison.Ordinal))
            {
                string value = args[i][(name.Length + 1)..];
                args.RemoveAt(i);
                return value;
            }

            if (args[i] == name && i + 1 < args.Count)
            {
                string value = args[i + 1];
                args.RemoveRange(i, 2);
                return value;
            }
        }
    }

    return null;
}

static bool TakeFlag(List<string> args, params string[] names)
{
    for (int i = 0; i < args.Count; i++)
    {
        if (names.Contains(args[i], StringComparer.Ordinal))
        {
            args.RemoveAt(i);
            return true;
        }
    }

    return false;
}

static string NormalizeSdRelativePath(string path)
{
    string normalized = path.Replace('\\', '/').Trim();
    while (normalized.StartsWith('/'))
        normalized = normalized[1..];

    return string.Join("/", normalized.Split('/', StringSplitOptions.RemoveEmptyEntries));
}

static string SdUrl(string host, string remotePath)
{
    string normalized = NormalizeSdRelativePath(remotePath);
    string escaped = normalized.Length == 0
        ? ""
        : string.Join("/", normalized.Split('/').Select(Uri.EscapeDataString));
    return $"http://{host}/sd/{escaped}";
}

static string JoinRemotePath(string directory, string filename)
{
    string dir = NormalizeSdRelativePath(directory).TrimEnd('/');
    string name = NormalizeSdRelativePath(filename).Trim('/');
    return dir.Length == 0 ? name : $"{dir}/{name}";
}

static bool LooksExplicitRemotePath(string path) =>
    path.StartsWith('/') || path.Contains('/') || path.Contains('\\');

static string ResolveDiskUploadPath(string localPath, string requestedName, bool hard, bool floppy)
{
    if (LooksExplicitRemotePath(requestedName))
        return NormalizeSdRelativePath(requestedName);

    var info = new FileInfo(localPath);
    string dir = DiskDirectoryFor(requestedName, hard, floppy, info.Length);
    return JoinRemotePath(dir, requestedName);
}

static string ResolveDiskReferencePath(string nameOrPath, bool hard, bool floppy)
{
    if (LooksExplicitRemotePath(nameOrPath))
        return NormalizeSdRelativePath(nameOrPath);

    string dir = DiskDirectoryFor(nameOrPath, hard, floppy, null);
    return JoinRemotePath(dir, nameOrPath);
}

static string DiskDirectoryFor(string name, bool hard, bool floppy, long? sizeBytes)
{
    if (hard) return "disks/hard";
    if (floppy) return "disks/floppy";

    string filename = Path.GetFileName(name);
    if (filename.StartsWith("hd", StringComparison.OrdinalIgnoreCase))
        return "disks/hard";

    if (sizeBytes is >= 16L * 1024 * 1024)
        return "disks/hard";

    return "disks/floppy";
}

static string ResolveManagedReferencePath(string baseDir, string nameOrPath)
{
    if (LooksExplicitRemotePath(nameOrPath))
        return NormalizeSdRelativePath(nameOrPath);

    return JoinRemotePath(baseDir, nameOrPath);
}

static bool IsMidiPath(string path)
{
    string ext = Path.GetExtension(path).ToLowerInvariant();
    return ext is ".mid" or ".midi";
}

static string NormalizeMusicFilename(string filename)
{
    string ext = Path.GetExtension(filename).ToLowerInvariant();
    if (ext.Length == 0)
        return filename + ".nms";
    if (ext is ".mid" or ".midi" or ".nms")
        return Path.ChangeExtension(filename, ".nms") ?? filename + ".nms";

    throw new InvalidOperationException("music upload target name must end in .nms, .mid, or .midi.");
}

static string NormalizeMusicRemotePath(string remotePath)
{
    string normalized = NormalizeSdRelativePath(remotePath);
    if (normalized.Length == 0)
        throw new InvalidOperationException("music upload target path is empty.");

    return NormalizeMusicFilename(normalized);
}

static (byte[] Data, int EventCount, uint TotalFrames, NovaMusicStreamCompiler.Result Result)
    CompileMidiToNovaStream(string midiPath)
{
    var result = NovaMusicStreamCompiler.Compile(midiPath);
    return (result.Data, result.EventCount, result.TotalFrames, result);
}

static string AssetDirectoryFor(string type)
{
    string normalized = NormalizeSdRelativePath(type).ToLowerInvariant();
    return normalized switch
    {
        "boot"          => "assets/boot",
        "font"          => "assets/fonts",
        "fonts"         => "assets/fonts",
        "sid"           => "assets/sid",
        "sid-assets"    => "assets/sid",
        "sid_assets"    => "assets/sid",
        "music"         => "music",
        "mid"           => "music",
        "midi"          => "music",
        ""              => "assets",
        _               => JoinRemotePath("assets", normalized),
    };
}

static (bool Ok, string Name, string? Error) KeepUploadName(string localPath, string requestedName) =>
    (true, requestedName, null);

static (bool Ok, string Name, string? Error) NormalizeRomUploadName(string localPath, string requestedName)
{
    if (Path.GetExtension(requestedName).Length == 0)
    {
        string ext = Path.GetExtension(localPath);
        if (ext.Length > 0)
            requestedName += ext;
    }

    return (true, requestedName, null);
}

static (bool Ok, string Name, string? Error) NormalizeSoundfontUploadName(string localPath, string requestedName)
{
    string localExt = Path.GetExtension(localPath).ToLowerInvariant();
    if (localExt == ".sf2")
    {
        return (false, requestedName,
            "SF2 upload must convert to Nova-native .nsfb first; that converter is not implemented yet. Upload a .nsfb bank for now.");
    }

    if (localExt != ".nsfb")
    {
        return (false, requestedName,
            "soundfont upload expects a Nova-native .nsfb bank for now.");
    }

    string requestedExt = Path.GetExtension(requestedName).ToLowerInvariant();
    if (requestedExt.Length == 0)
        requestedName += ".nsfb";
    else if (requestedExt != ".nsfb")
        return (false, requestedName, "soundfont upload target name must end in .nsfb.");

    return (true, requestedName, null);
}

static (bool Ok, string Name, string? Error) NormalizeMusicUploadName(string localPath, string requestedName)
{
    if (IsMidiPath(localPath))
    {
        try { return (true, NormalizeMusicFilename(requestedName), null); }
        catch (Exception ex) { return (false, requestedName, ex.Message); }
    }

    string localExt = Path.GetExtension(localPath).ToLowerInvariant();
    if (localExt != ".nms")
        return (false, requestedName, "music upload expects a .mid/.midi file or a precompiled .nms stream.");

    string requestedExt = Path.GetExtension(requestedName).ToLowerInvariant();
    if (requestedExt.Length == 0)
        requestedName += ".nms";
    else if (requestedExt != ".nms")
        return (false, requestedName, "precompiled music upload target name must end in .nms.");

    return (true, requestedName, null);
}

// ---------------------------------------------------------------------------
// create <file.ndi> [--size <KB>|--hd] [--label <name>]
// ---------------------------------------------------------------------------
static int DoCreate(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: nova create <file.ndi> [--size <KB>|--hd] [--label <name>]");
        return 1;
    }

    string path = args[0];
    int sizeKB = 800;
    string label = "DISK";

    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--hd":
                sizeKB = 65536;
                break;
            case "--size" when i + 1 < args.Length:
                if (!int.TryParse(args[++i], out sizeKB) || sizeKB < 1)
                {
                    Console.Error.WriteLine("--size must be a positive integer (KB)");
                    return 1;
                }
                break;
            case "--label" when i + 1 < args.Length:
                label = args[++i];
                break;
        }
    }

    try
    {
        NdiImage.CreateFormatted(path, label, sizeKB);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    using var img = NdiImage.Open(path);
    var h = img.Header;
    Console.WriteLine($"Created {path}");
    Console.WriteLine($"  Label:        {h.VolumeLabel}");
    Console.WriteLine($"  Size:         {sizeKB} KB  ({h.TotalSectors} sectors)");
    Console.WriteLine($"  Free sectors: {img.FreeSectors}");
    return 0;
}

// ---------------------------------------------------------------------------
// dir <file.ndi> [/path]
// ---------------------------------------------------------------------------
static int DoDir(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: nova dir <file.ndi> [/path]");
        return 1;
    }

    string imagePath = args[0];
    string ndiPath = args.Length > 1 ? args[1] : "/";

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        ushort parentIndex;
        try { parentIndex = ResolvePath(img, ndiPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        var entries = img.ListDirectory(parentIndex);

        Console.WriteLine($"Directory of {imagePath}:{ndiPath}");
        Console.WriteLine();

        if (entries.Length == 0)
        {
            Console.WriteLine("  (empty)");
        }
        else
        {
            Console.WriteLine($"  {"Name",-32}  {"Type",-4}  {"Size",8}");
            Console.WriteLine($"  {new string('-', 32)}  {new string('-', 4)}  {new string('-', 8)}");
            foreach (var e in entries)
            {
                string typeStr = e.IsDirectory ? "DIR" : e.FileType.ToString().ToUpperInvariant();
                string sizeStr = e.IsDirectory ? "<DIR>" : e.SizeBytes.ToString();
                Console.WriteLine($"  {e.Filename,-32}  {typeStr,-4}  {sizeStr,8}");
            }
        }

        Console.WriteLine();
        Console.WriteLine($"  {img.FreeSectors} sectors free  ({(long)img.FreeSectors * img.Header.SectorSize / 1024} KB)");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// info <file.ndi>
// ---------------------------------------------------------------------------
static int DoInfo(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: nova info <file.ndi>");
        return 1;
    }

    NdiImage img;
    try { img = NdiImage.Open(args[0]); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        var h = img.Header;
        long totalKB = (long)h.TotalSectors * h.SectorSize / 1024;
        long freeKB = (long)img.FreeSectors * h.SectorSize / 1024;
        int dirCapacity = checked((int)h.DirectorySectorCount) * NdiDirectory.EntriesPerSector;

        Console.WriteLine($"Image:            {args[0]}");
        Console.WriteLine($"Format version:   {h.FormatVersion}");
        Console.WriteLine($"Volume label:     {h.VolumeLabel}");
        Console.WriteLine($"Sector size:      {h.SectorSize} bytes");
        Console.WriteLine($"Total sectors:    {h.TotalSectors}  ({totalKB} KB)");
        Console.WriteLine($"Free sectors:     {img.FreeSectors}  ({freeKB} KB)");
        Console.WriteLine($"Dir start:        sector {h.DirectoryStartSector}");
        Console.WriteLine($"Dir sectors:      {h.DirectorySectorCount}");
        Console.WriteLine($"Data start:       sector {h.DataStartSector}");
        Console.WriteLine($"Dir capacity:     {dirCapacity} entries");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// validate <file.ndi>
// ---------------------------------------------------------------------------
static int DoValidate(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: nova validate <file.ndi>");
        return 1;
    }

    NdiImage img;
    try { img = NdiImage.Open(args[0]); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        var h = img.Header;
        int dataSectorCount = checked((int)(h.TotalSectors - h.DataStartSector));

        var entries = CollectAllEntries(img);
        var claimed = new bool[dataSectorCount];
        var errors = new List<string>();

        foreach (var e in entries)
        {
            if (e.IsDirectory) continue;

            uint start = e.StartSector;
            uint count = e.SectorCount;
            ulong end = (ulong)start + count;

            if (end > (uint)dataSectorCount)
            {
                errors.Add($"  [{e.Index}] '{e.Filename}': sectors {start}..{end - 1} extend beyond image bounds");
                continue;
            }

            for (uint s = start; s < end; s++)
            {
                if (claimed[(int)s])
                    errors.Add($"  [{e.Index}] '{e.Filename}': sector {s} is double-allocated");
                claimed[(int)s] = true;
            }
        }

        int dirAllocated = claimed.Count(c => c);
        int bamFree = img.FreeSectors;
        int bamAllocated = dataSectorCount - bamFree;

        if (bamAllocated != dirAllocated)
            errors.Add($"  BAM says {bamAllocated} sectors used, but directory accounts for {dirAllocated}");

        if (errors.Count == 0)
        {
            Console.WriteLine("OK — no errors found.");
            Console.WriteLine($"  {entries.Length} directory entries, {dirAllocated} sectors used, {bamFree} sectors free.");
        }
        else
        {
            Console.WriteLine($"Found {errors.Count} error(s):");
            foreach (string err in errors)
                Console.WriteLine(err);
            return 1;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// label <file.ndi> <name>
// ---------------------------------------------------------------------------
static int DoLabel(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova label <file.ndi> <name>");
        return 1;
    }

    string imagePath = args[0];
    string newLabel = args[1];
    if (newLabel.Length > 32)
    {
        Console.Error.WriteLine("Label must be 32 characters or fewer.");
        return 1;
    }

    try
    {
        using var fs = new FileStream(imagePath, FileMode.Open, FileAccess.ReadWrite, FileShare.None);

        var headerBuf = new byte[NdiHeader.Size];
        fs.Seek(0, SeekOrigin.Begin);
        fs.ReadExactly(headerBuf);

        try
        {
            _ = NdiHeader.FromBytes(headerBuf);
        }
        catch (InvalidDataException)
        {
            Console.Error.WriteLine("Not a valid NDI image.");
            return 1;
        }

        Array.Clear(headerBuf, NdiHeader.LabelOffset, NdiHeader.LabelLength);
        var labelBytes = Encoding.ASCII.GetBytes(newLabel);
        Array.Copy(labelBytes, 0, headerBuf, NdiHeader.LabelOffset, Math.Min(labelBytes.Length, NdiHeader.LabelLength));

        fs.Seek(0, SeekOrigin.Begin);
        fs.Write(headerBuf);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    Console.WriteLine($"Label set to: {newLabel}");
    return 0;
}

// ---------------------------------------------------------------------------
// import <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>] [--raw-midi]
// ---------------------------------------------------------------------------
static int DoImport(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova import <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>] [--raw-midi]");
        return 1;
    }

    string imagePath = args[0];
    string hostFile = args[1];
    string destPath = "/";
    bool doTokenize = false;
    bool rawMidi = false;
    string? tokensPath = null;

    for (int i = 2; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--tokenize":
                doTokenize = true;
                break;
            case "--raw-midi":
                rawMidi = true;
                break;
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
            case "--bank" or "--soundbank" or "--soundfont" when i + 1 < args.Length:
                i++;
                break;
            default:
                if (args[i].StartsWith('/'))
                    destPath = args[i];
                break;
        }
    }

    if (!File.Exists(hostFile))
    {
        Console.Error.WriteLine($"Host file not found: {hostFile}");
        return 1;
    }

    if (rawMidi && doTokenize)
    {
        Console.Error.WriteLine("Cannot combine --raw-midi with --tokenize.");
        return 1;
    }

    string filename = Path.GetFileName(hostFile);
    NdiFileType fileType = ExtensionToFileType(Path.GetExtension(hostFile));

    byte[] data;
    bool compiledMusic = false;
    NovaMusicStreamCompiler.Result? musicResult = null;
    if (IsMidiPath(hostFile) && !rawMidi)
    {
        if (doTokenize)
        {
            Console.Error.WriteLine("Cannot combine --tokenize with MIDI music compilation.");
            return 1;
        }

        try
        {
            var compiled = CompileMidiToNovaStream(hostFile);
            data = compiled.Data;
            musicResult = compiled.Result;
            filename = NormalizeMusicFilename(filename);
            fileType = NdiFileType.Mid;
            compiledMusic = true;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"music compile: {ex.Message}");
            return 1;
        }
    }
    else
    {
        try { data = File.ReadAllBytes(hostFile); }
        catch (Exception ex) { Console.Error.WriteLine($"Error reading host file: {ex.Message}"); return 1; }
    }

    if (doTokenize)
    {
        tokensPath ??= FindTokensJson();
        if (tokensPath is null)
        {
            Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
            return 1;
        }
        try
        {
            var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
            string[] lines = Encoding.ASCII.GetString(data).ReplaceLineEndings("\n")
                .Split('\n', StringSplitOptions.RemoveEmptyEntries);
            data = AddBasicLoadAddressPrefix(tokenizer.Tokenize(lines, 0x0301), 0x0301);
            fileType = NdiFileType.Bas;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Tokenize error: {ex.Message}");
            return 1;
        }
    }

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        ushort parentIndex;
        try { parentIndex = ResolvePath(img, destPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.WriteFile(filename, fileType, parentIndex, data); }
        catch (Exception ex) { Console.Error.WriteLine($"Error writing file: {ex.Message}"); return 1; }
    }

    if (compiledMusic && musicResult is not null)
    {
        Console.WriteLine(
            $"Imported {hostFile} -> {destPath.TrimEnd('/')}/{filename}  ({data.Length} bytes, type={fileType}, events={musicResult.EventCount})");
    }
    else
    {
        Console.WriteLine($"Imported {hostFile} -> {destPath.TrimEnd('/')}/{filename}  ({data.Length} bytes, type={fileType})");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// export <file.ndi> <path> [hostdir] [--detokenize] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoExport(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova export <file.ndi> <path> [hostdir] [--detokenize] [--tokens <path>]");
        return 1;
    }

    string imagePath = args[0];
    string ndiFilePath = args[1];
    string? hostDest = null;
    bool doDetokenize = false;
    string? tokensPath = null;

    for (int i = 2; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--detokenize":
                doDetokenize = true;
                break;
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
            default:
                if (!args[i].StartsWith("--"))
                    hostDest = args[i];
                break;
        }
    }

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    byte[] data;
    string filename;
    using (img)
    {
        string[] parts = ndiFilePath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path — must specify a file, not a directory.");
            return 1;
        }

        filename = parts[^1];
        string dirPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, dirPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { data = img.ReadFile(filename, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    if (doDetokenize)
    {
        tokensPath ??= FindTokensJson();
        if (tokensPath is null)
        {
            Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
            return 1;
        }
        try
        {
            var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
            string[] lines = tokenizer.Detokenize(StripBasicLoadAddressPrefix(data));
            data = Encoding.ASCII.GetBytes(string.Join("\n", lines) + "\n");
            if (Path.GetExtension(filename).Equals(".bas", StringComparison.OrdinalIgnoreCase))
                filename = Path.ChangeExtension(filename, ".txt");
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Detokenize error: {ex.Message}");
            return 1;
        }
    }

    string outputPath;
    if (hostDest is not null && Directory.Exists(hostDest))
        outputPath = Path.Combine(hostDest, filename);
    else if (hostDest is not null)
        outputPath = hostDest;
    else
        outputPath = filename;

    try { File.WriteAllBytes(outputPath, data); }
    catch (Exception ex) { Console.Error.WriteLine($"Error writing output: {ex.Message}"); return 1; }

    Console.WriteLine($"Exported {ndiFilePath} -> {outputPath}  ({data.Length} bytes)");
    return 0;
}

// ---------------------------------------------------------------------------
// delete <file.ndi> <path>
// ---------------------------------------------------------------------------
static int DoDelete(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova delete <file.ndi> <path>");
        return 1;
    }

    string imagePath = args[0];
    string ndiFilePath = args[1];

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        string[] parts = ndiFilePath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path.");
            return 1;
        }

        string filename = parts[^1];
        string dirPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, dirPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.DeleteFile(filename, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Deleted {ndiFilePath}");
    return 0;
}

// ---------------------------------------------------------------------------
// mkdir <file.ndi> <path>
// ---------------------------------------------------------------------------
static int DoMkdir(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova mkdir <file.ndi> <path>");
        return 1;
    }

    string imagePath = args[0];
    string ndiPath = args[1];

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        string[] parts = ndiPath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path.");
            return 1;
        }

        string dirName = parts[^1];
        string parentPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, parentPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.MakeDirectory(dirName, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Created directory {ndiPath}");
    return 0;
}

// ---------------------------------------------------------------------------
// rmdir <file.ndi> <path>
// ---------------------------------------------------------------------------
static int DoRmdir(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova rmdir <file.ndi> <path>");
        return 1;
    }

    string imagePath = args[0];
    string ndiPath = args[1];

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        string[] parts = ndiPath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path.");
            return 1;
        }

        string dirName = parts[^1];
        string parentPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, parentPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.RemoveDirectory(dirName, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Removed directory {ndiPath}");
    return 0;
}

static byte[] AddBasicLoadAddressPrefix(byte[] body, ushort baseAddr)
{
    var withHeader = new byte[2 + body.Length];
    withHeader[0] = (byte)(baseAddr & 0xFF);
    withHeader[1] = (byte)(baseAddr >> 8);
    Array.Copy(body, 0, withHeader, 2, body.Length);
    return withHeader;
}

static byte[] StripBasicLoadAddressPrefix(byte[] data)
{
    if (data.Length < 4)
        return data;

    ushort loadAddress = (ushort)(data[0] | (data[1] << 8));
    ushort firstLinePointer = (ushort)(data[2] | (data[3] << 8));
    if (firstLinePointer == 0 || (firstLinePointer > loadAddress && firstLinePointer - loadAddress <= data.Length - 2))
        return data[2..];

    return data;
}

// ---------------------------------------------------------------------------
// tokenize <input.txt> <output.bas> [--base <addr>] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoTokenize(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: nova tokenize <input.txt> <output.bas> [--base <addr>] [--tokens <path>]");
        return 1;
    }

    string inputPath = args[0];
    string outputPath = args[1];
    ushort baseAddr = 0x0301;
    string? tokensPath = null;

    for (int i = 2; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--base" when i + 1 < args.Length:
            {
                string baseStr = args[++i].Replace("0x", "").Replace("$", "");
                if (!ushort.TryParse(baseStr, System.Globalization.NumberStyles.HexNumber, null, out baseAddr))
                {
                    Console.Error.WriteLine($"Invalid base address: {args[i]}");
                    return 1;
                }
                break;
            }
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
        }
    }

    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    tokensPath ??= FindTokensJson();
    if (tokensPath is null)
    {
        Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
        return 1;
    }

    try
    {
        var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
        string text = File.ReadAllText(inputPath);
        string[] lines = text.ReplaceLineEndings("\n").Split('\n', StringSplitOptions.RemoveEmptyEntries);

        byte[] withHeader = AddBasicLoadAddressPrefix(tokenizer.Tokenize(lines, baseAddr), baseAddr);

        File.WriteAllBytes(outputPath, withHeader);
        Console.WriteLine($"Tokenized {inputPath} -> {outputPath}  ({withHeader.Length} bytes, base=${baseAddr:X4})");
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// detokenize <input.bas> [output.txt] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoDetokenize(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: nova detokenize <input.bas> [output.txt] [--tokens <path>]");
        return 1;
    }

    string inputPath = args[0];
    string? outputPath = null;
    string? tokensPath = null;

    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
            default:
                if (!args[i].StartsWith("--"))
                    outputPath = args[i];
                break;
        }
    }

    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    tokensPath ??= FindTokensJson();
    if (tokensPath is null)
    {
        Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
        return 1;
    }

    try
    {
        var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
        byte[] raw = File.ReadAllBytes(inputPath);

        string[] lines = tokenizer.Detokenize(StripBasicLoadAddressPrefix(raw));
        string text = string.Join("\n", lines) + "\n";

        if (outputPath is null)
        {
            Console.Write(text);
        }
        else
        {
            File.WriteAllText(outputPath, text, Encoding.ASCII);
            Console.WriteLine($"Detokenized {inputPath} -> {outputPath}  ({lines.Length} lines)");
        }
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

/// <summary>
/// Resolves an NDI path like "/" or "/games/rpg" to the parentIndex value
/// used by NdiImage (0xFFFF = root).
/// </summary>
static ushort ResolvePath(NdiImage img, string path)
{
    const ushort Root = 0xFFFF;

    string[] parts = path.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
    if (parts.Length == 0)
        return Root;

    ushort current = Root;
    foreach (string part in parts)
    {
        var entries = img.ListDirectory(current);
        var match = entries.FirstOrDefault(e =>
            e.IsDirectory &&
            string.Equals(e.Filename, part, StringComparison.OrdinalIgnoreCase));

        if (match is null)
            throw new DirectoryNotFoundException($"Directory '{part}' not found.");

        current = (ushort)match.Index;
    }

    return current;
}

/// <summary>Collects all active directory entries across all directories (BFS).</summary>
static NdiDirEntry[] CollectAllEntries(NdiImage img)
{
    var result = new List<NdiDirEntry>();
    var queue = new Queue<ushort>();
    queue.Enqueue(0xFFFF);

    while (queue.Count > 0)
    {
        ushort parent = queue.Dequeue();
        foreach (var e in img.ListDirectory(parent))
        {
            result.Add(e);
            if (e.IsDirectory)
                queue.Enqueue((ushort)e.Index);
        }
    }

    return result.ToArray();
}

/// <summary>Infers NdiFileType from file extension.</summary>
static NdiFileType ExtensionToFileType(string ext) =>
    ext.ToLowerInvariant() switch
    {
        ".bas"            => NdiFileType.Bas,
        ".sid"            => NdiFileType.Sid,
        ".bin"            => NdiFileType.Bin,
        ".mid" or ".midi" or ".nms" => NdiFileType.Mid,
        ".nvg"            => NdiFileType.Gfx,
        _                 => NdiFileType.Bin,
    };

/// <summary>
/// Searches upward from the executable directory, then from cwd,
/// for ehbasic/tokens.json.
/// </summary>
static string? FindTokensJson()
{
    string? dir = Path.GetDirectoryName(Environment.ProcessPath);
    while (dir is not null)
    {
        string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
        if (File.Exists(candidate))
            return candidate;
        dir = Path.GetDirectoryName(dir);
    }

    dir = Directory.GetCurrentDirectory();
    while (dir is not null)
    {
        string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
        if (File.Exists(candidate))
            return candidate;
        string? parent = Path.GetDirectoryName(dir);
        if (parent == dir) break;
        dir = parent;
    }

    return null;
}

static int UnknownVerb(string verb)
{
    Console.Error.WriteLine($"Unknown command: {verb}");
    PrintUsage();
    return 1;
}

static void PrintUsage()
{
    Console.Error.WriteLine("Nova CLI / NDI image and device management tool");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Local NDI image commands:");
    Console.Error.WriteLine("  create     <file.ndi> [--size <KB>|--hd] [--label <name>]");
    Console.Error.WriteLine("  dir        <file.ndi> [/path]");
    Console.Error.WriteLine("  info       <file.ndi>");
    Console.Error.WriteLine("  validate   <file.ndi>");
    Console.Error.WriteLine("  label      <file.ndi> <name>");
    Console.Error.WriteLine("  import     <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>] [--raw-midi]");
    Console.Error.WriteLine("  export     <file.ndi> <path> [hostdir] [--detokenize] [--tokens <path>]");
    Console.Error.WriteLine("  delete     <file.ndi> <path>");
    Console.Error.WriteLine("  mkdir      <file.ndi> <path>");
    Console.Error.WriteLine("  rmdir      <file.ndi> <path>");
    Console.Error.WriteLine("  tokenize   <input.txt> <output.bas> [--base <addr>] [--tokens <path>]");
    Console.Error.WriteLine("  detokenize <input.bas> [output.txt] [--tokens <path>]");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Typed NovaHost SD commands:");
    Console.Error.WriteLine("  device status|reboot --remote <host>");
    Console.Error.WriteLine("  drive list|mount|unmount ... --remote <host>");
    Console.Error.WriteLine("  wifi status|scan|set|connect|disconnect|reconnect|forget ... --remote <host>");
    Console.Error.WriteLine("  audio status|stop --remote <host>");
    Console.Error.WriteLine("  keyboard --remote <host> [--port 6503]");
    Console.Error.WriteLine("  disk list|upload|download|delete ... --remote <host>");
    Console.Error.WriteLine("  rom list|upload|download|delete ... --remote <host>");
    Console.Error.WriteLine("  soundfont list|upload|download|delete ... --remote <host>");
    Console.Error.WriteLine("  music list|upload|download|delete ... --remote <host>");
    Console.Error.WriteLine("  asset list|upload|download|delete ... --remote <host> --type <boot|fonts|sid|...>");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Raw SD commands, kept for compatibility:");
    Console.Error.WriteLine("  --remote <host> ls [path]");
    Console.Error.WriteLine("  --remote <host> put <local-path> [remote-path]");
    Console.Error.WriteLine("  --remote <host> get <remote-path> [local-path]");
    Console.Error.WriteLine("  --remote <host> rm <remote-path>");
}

sealed class KeyboardInputReader : IDisposable
{
    private readonly Stream _input;
    private readonly System.Collections.Concurrent.BlockingCollection<int> _queue = new();
    private readonly Thread _thread;
    private volatile bool _disposed;

    public KeyboardInputReader(Stream input)
    {
        _input = input;
        _thread = new Thread(ReadLoop)
        {
            IsBackground = true,
            Name = "Nova keyboard input reader",
        };
        _thread.Start();
    }

    public bool TryRead(out int value, int timeoutMs = Timeout.Infinite)
    {
        value = -1;
        try
        {
            return _queue.TryTake(out value, timeoutMs);
        }
        catch
        {
            return false;
        }
    }

    private void ReadLoop()
    {
        while (!_disposed)
        {
            int value;
            try
            {
                value = _input.ReadByte();
            }
            catch
            {
                break;
            }

            if (value < 0)
                continue;

            try
            {
                _queue.Add(value);
            }
            catch
            {
                break;
            }
        }
    }

    public void Dispose()
    {
        _disposed = true;
        _queue.CompleteAdding();
    }
}
