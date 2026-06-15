using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using e6502.NovaHost;
using e6502.Storage;

if (args.Length < 1)
{
    PrintUsage();
    return 1;
}

// Remote verbs talk to NovaHost's TCP management service. Accept --remote
// either before or after the command so the current `nova` CLI shape is natural.
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
    "vm" or "emulator" => DoVm(args[1..], remoteHost),
    "disk"       => DoDisk(args[1..], remoteHost),
    "rom"        => DoRom(args[1..], remoteHost),
    "soundfont"  => DoSoundfont(args[1..], remoteHost),
    "music"      => DoMusic(args[1..], remoteHost),
    "asset"      => DoAsset(args[1..], remoteHost),
    "runtime"    => DoRuntime(args[1..], remoteHost),
    "webserver" or "web" => DoWebServer(args[1..], remoteHost),
    "module"     => DoModule(args[1..], remoteHost),
    _            => UnknownVerb(verb),
};

// ===========================================================================
// Remote SD operations — talks to NovaHost's TCP management service.
// ===========================================================================

static int DoWebServer(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    var rest = args.ToList();
    bool help = rest.Count > 0 && rest[0].Equals("help", StringComparison.OrdinalIgnoreCase);
    help |= TakeFlag(rest, "--help", "-h");
    string? portOpt = TakeOptionValue(rest, "--port", "-p");
    string bind = TakeOptionValue(rest, "--bind") ?? "127.0.0.1";
    _ = TakeFlag(rest, "--open");
    bool open = !TakeFlag(rest, "--no-open");

    if (help)
    {
        PrintWebServerUsage();
        return 0;
    }

    if (rest.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected webserver argument: {rest[0]}");
        PrintWebServerUsage();
        return 1;
    }

    int port = 8080;
    if (portOpt is not null && (!int.TryParse(portOpt, out port) || port is < 1 or > 65535))
    {
        Console.Error.WriteLine($"Invalid webserver port: {portOpt}");
        return 1;
    }

    string boardHost = host
        ?? Environment.GetEnvironmentVariable("NOVAHOST")
        ?? "192.168.1.65";
    return NovaWebServer.Run(boardHost, bind, port, open);
}

static void PrintWebServerUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova webserver --remote <host> [--port 8080] [--bind 127.0.0.1] [--no-open]");
}

static int RunManagement(string host, string operation,
                         Func<NovaHostManagementClient, CancellationToken, Task<int>> action,
                         TimeSpan? timeout = null)
{
    using var cts = new CancellationTokenSource(timeout ?? TimeSpan.FromMinutes(30));
    using var management = new NovaHostManagementClient(host);
    try
    {
        return action(management, cts.Token).GetAwaiter().GetResult();
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"{operation}: {ex.Message}");
        return 1;
    }
}

// ===========================================================================
// module — inspect / validate / manage .nmod paged-library modules
// (NovaModule lives in e6502.Storage; shared with the web UI)
// ===========================================================================

static int DoModule(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintModuleUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string cmd = args[0].ToLowerInvariant();
    string[] rest = args[1..];
    return cmd switch
    {
        "info"              => DoModuleInfo(rest, host),
        "ls" or "list"      => DoModuleLs(host),
        "validate"          => DoModuleValidate(rest),
        "put" or "upload"   => DoModulePut(rest, host),
        "get" or "download" => DoModuleGet(rest, host),
        "rm" or "delete"    => DoModuleRm(rest, host),
        _                   => UnknownModuleCommand(cmd),
    };
}

static int UnknownModuleCommand(string cmd)
{
    Console.Error.WriteLine($"Unknown module command: {cmd}");
    PrintModuleUsage();
    return 1;
}

static void PrintModuleUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova module info <file|name> [--remote <host>]");
    Console.Error.WriteLine("  nova module ls --remote <host>");
    Console.Error.WriteLine("  nova module validate <file>");
    Console.Error.WriteLine("  nova module put <file> [name] --remote <host>");
    Console.Error.WriteLine("  nova module get <name> [local] --remote <host>");
    Console.Error.WriteLine("  nova module rm <name> --remote <host>");
}

// Read a module's bytes from a local file (if it exists), else from the board's /lib.
static byte[]? ReadModuleBytes(string arg, string? host, out string source)
{
    source = arg;
    if (File.Exists(arg))
        return File.ReadAllBytes(arg);

    if (host is not null)
    {
        string remote = arg.Contains('/') ? NormalizeSdRelativePath(arg)
                                           : JoinRemotePath("lib", arg);
        source = "/" + remote;
        byte[]? data = null;
        int rc = RunManagement(host, "module read", async (m, t) =>
        {
            data = await m.ReadFileAsync(remote, t);
            return 0;
        });
        return rc == 0 ? data : null;
    }

    Console.Error.WriteLine($"not found locally and no --remote given: {arg}");
    return null;
}

static int DoModuleInfo(string[] args, string? host)
{
    if (args.Length < 1) { PrintModuleUsage(); return 1; }
    byte[]? bytes = ReadModuleBytes(args[0], host, out string source);
    if (bytes is null) return 1;
    NovaModule mod = NovaModule.Parse(bytes);
    PrintModule(source, mod);
    return mod.Valid ? 0 : 1;
}

static void PrintModule(string source, NovaModule mod)
{
    if (!mod.Valid)
    {
        Console.Error.WriteLine($"{source}: INVALID — {mod.Error}");
        return;
    }

    if (mod is { HasDoc: true, Doc: not null })
    {
        NovaModuleMeta meta = mod.Doc.Module;
        Console.WriteLine($"Module: {meta.Name}  id=${meta.Id:X2}  abi={meta.AbiVersion}  " +
                          $"version={meta.Version ?? "?"}");
        if (!string.IsNullOrEmpty(meta.Brief)) Console.WriteLine($"  {meta.Brief}");
        if (!string.IsNullOrEmpty(meta.AbiNote)) Console.WriteLine($"  ({meta.AbiNote})");
        Console.WriteLine($"Functions ({mod.Doc.Functions.Count}):");
        foreach (NovaFn fn in mod.Doc.Functions)
        {
            Console.WriteLine($"  {fn.IdHex} {fn.Signature()}");
            if (!string.IsNullOrEmpty(fn.Brief)) Console.WriteLine($"       {fn.Brief}");
            if (!string.IsNullOrEmpty(fn.Effect)) Console.WriteLine($"       effect: {fn.Effect}");
            if (fn.Status.Count > 0) Console.WriteLine($"       status: {string.Join(", ", fn.Status)}");
        }
    }
    else
    {
        Console.WriteLine($"Module: id=${mod.Id:X2}  abi={mod.AbiVersion}  " +
                          $"fnCount={mod.FnCount}  (no NDOC documentation)");
    }
}

static int DoModuleValidate(string[] args)
{
    if (args.Length < 1) { PrintModuleUsage(); return 1; }
    string path = args[0];
    if (!File.Exists(path)) { Console.Error.WriteLine($"file not found: {path}"); return 1; }

    NovaModule mod = NovaModule.Parse(File.ReadAllBytes(path));
    (bool ok, string? reason) = mod.ValidateForStaging();
    if (!ok) { Console.Error.WriteLine($"{path}: INVALID — {reason}"); return 1; }

    string docNote = mod.HasDoc ? $"{mod.Doc!.Functions.Count} documented functions"
                                : "no NDOC doc";
    Console.WriteLine($"{path}: OK — id=${mod.Id:X2} abi={mod.AbiVersion} " +
                      $"fnCount={mod.FnCount} ({docNote})");
    return 0;
}

static int DoModulePut(string[] args, string? host)
{
    if (host is null || args.Length < 1) { PrintModuleUsage(); return 1; }
    string local = args[0];
    if (!File.Exists(local)) { Console.Error.WriteLine($"local file not found: {local}"); return 1; }

    NovaModule mod = NovaModule.Parse(File.ReadAllBytes(local));
    (bool ok, string? reason) = mod.ValidateForStaging();
    if (!ok)
    {
        Console.Error.WriteLine($"refusing to stage invalid module {local}: {reason}");
        return 1;
    }

    string name = args.Length > 1 ? args[1] : Path.GetFileName(local);
    return PutFile(local, JoinRemotePath("lib", name), host);
}

static int DoModuleGet(string[] args, string? host)
{
    if (host is null || args.Length < 1) { PrintModuleUsage(); return 1; }
    string remote = JoinRemotePath("lib", args[0]);
    string[] getArgs = args.Length > 1 ? new[] { remote, args[1] } : new[] { remote };
    return DoGet(getArgs, host);
}

static int DoModuleRm(string[] args, string? host)
{
    if (host is null || args.Length < 1) { PrintModuleUsage(); return 1; }
    return DoRm(new[] { JoinRemotePath("lib", args[0]) }, host);
}

static int DoModuleLs(string? host)
{
    if (host is null) { PrintModuleUsage(); return 1; }

    JsonNode? config = ReadBootConfig(host);
    var staged = new Dictionary<string, int>(StringComparer.OrdinalIgnoreCase);
    if (config?["libraries"] is JsonArray libs)
    {
        foreach (JsonNode? lib in libs)
        {
            string baseName = Path.GetFileName((string?)lib?["path"] ?? "");
            if (baseName.Length > 0)
                staged[baseName] = (int?)lib?["id"] ?? -1;
        }
    }

    return RunManagement(host, "module ls", async (m, t) =>
    {
        JsonArray rows = await m.ListDirectoryAsync("lib/", t);
        Console.WriteLine($"{"Name",-22} {"Id",-5} {"Ver",-6} {"Fns",-4} {"Valid",-6} {"Staged",-7} Notes");
        Console.WriteLine(new string('-', 78));
        foreach (JsonNode? node in rows)
        {
            var entry = node as JsonObject;
            string name = entry?["name"]?.GetValue<string>() ?? "?";
            bool isDir = entry?["dir"]?.GetValue<bool>() == true;
            if (isDir || (!name.EndsWith(".mod") && !name.EndsWith(".nmod")))
                continue;

            NovaModule mod = NovaModule.Parse(await m.ReadFileAsync($"lib/{name}", t));
            bool isStaged = staged.TryGetValue(name, out int stagedId);
            string ver = mod.Doc?.Module.Version ?? "-";
            string note = "";
            if (isStaged && mod.Valid && stagedId != mod.Id)
                note = $"! id boot.json=${stagedId:X2} != binary=${mod.Id:X2}";

            Console.WriteLine(
                $"{name,-22} {(mod.Valid ? $"${mod.Id:X2}" : "?"),-5} {ver,-6} " +
                $"{(mod.Valid ? mod.FnCount.ToString() : "-"),-4} " +
                $"{(mod.Valid ? "yes" : "NO"),-6} {(isStaged ? "yes" : "no"),-7} {note}");
        }
        return 0;
    }, TimeSpan.FromSeconds(30));
}

static void PrintJson(JsonNode? node)
{
    Console.WriteLine((node ?? new JsonObject()).ToJsonString(
        new JsonSerializerOptions { WriteIndented = true }));
}

static IProgress<long>? CreateProgressReporter(string operation, long total)
{
    if (total < 1024 * 1024)
        return null;

    long lastMs = 0;
    return new InlineProgress<long>(done =>
    {
        bool finished = done >= total;
        long now = Environment.TickCount64;
        if (!finished && now - lastMs < 500)
            return;
        lastMs = now;
        WriteTransferProgress(operation, done, total, finished);
    });
}

static IProgress<NovaTransferProgress> CreateTransferProgressReporter(string operation)
{
    long lastMs = 0;
    return new InlineProgress<NovaTransferProgress>(progress =>
    {
        if (progress.Total > 0 && progress.Total < 1024 * 1024)
            return;
        bool finished = progress.Total > 0 && progress.Done >= progress.Total;
        long now = Environment.TickCount64;
        if (!finished && now - lastMs < 500)
            return;
        lastMs = now;
        WriteTransferProgress(operation, progress.Done, progress.Total, finished);
    });
}

static void WriteTransferProgress(string operation, long done, long total,
                                  bool finished)
{
    if (total > 0)
    {
        long percent = Math.Clamp(done * 100 / total, 0, 100);
        Console.Error.Write($"\r{operation} {percent,3}% {done}/{total} bytes");
    }
    else
    {
        Console.Error.Write($"\r{operation} {done} bytes");
    }
    if (finished)
        Console.Error.WriteLine();
}

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

    return RunManagement(host, "device status", async (management, token) =>
    {
        PrintJson(await management.GetStatusAsync(token));
        return 0;
    }, TimeSpan.FromSeconds(30));
}

static int DoDeviceReboot(string? host)
{
    if (host is null)
    {
        PrintDeviceUsage();
        return 1;
    }

    return RunManagement(host, "device reboot", async (management, token) =>
    {
        await management.HostRebootAsync(token);
        Console.WriteLine("NovaHost reboot accepted.");
        return 0;
    }, TimeSpan.FromSeconds(30));
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

    return RunManagement(host, "drive list", async (management, token) =>
    {
        JsonObject status = await management.GetStatusAsync(token);
        JsonArray drives = status["drives"] as JsonArray ?? new JsonArray();

        Console.WriteLine($"Drive slots on {host}:");
        Console.WriteLine();
        Console.WriteLine("  Slot  Mounted  Current path                         Configured path");
        Console.WriteLine("  ----  -------  -----------------------------------  -----------------------------------");
        foreach (JsonNode? node in drives)
        {
            JsonObject? entry = node as JsonObject;
            string slot = entry?["slot"]?.GetValue<string>() ?? "?";
            bool mounted = entry?["mounted"]?.GetValue<bool>() == true;
            string current = entry?["currentPath"]?.GetValue<string>() ?? "";
            string configured = entry?["configuredPath"]?.GetValue<string>() ?? "";
            Console.WriteLine($"  {slot,-4}  {(mounted ? "yes" : "no"),-7}  {TruncateForTable(current, 35),-35}  {TruncateForTable(configured, 35),-35}");
        }

        return 0;
    }, TimeSpan.FromSeconds(30));
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

    return RunManagement(host, "drive mount", async (management, token) =>
    {
        await management.MountDriveAsync(slot, path, token);
        Console.WriteLine($"Mounted {slot} -> {path}");
        return 0;
    }, TimeSpan.FromSeconds(30));
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

    return RunManagement(host, "drive unmount", async (management, token) =>
    {
        await management.UnmountDriveAsync(slot, token);
        Console.WriteLine($"Unmounted {slot}");
        return 0;
    }, TimeSpan.FromSeconds(30));
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

    return RunManagement(host, "wifi status", async (management, token) =>
    {
        PrintJson(await management.WifiStatusAsync(token));
        return 0;
    }, TimeSpan.FromSeconds(30));
}

static int DoWifiScan(string? host)
{
    if (host is null)
    {
        PrintWifiUsage();
        return 1;
    }

    return RunManagement(host, "wifi scan", async (management, token) =>
    {
        PrintJson(await management.WifiScanAsync(token));
        return 0;
    }, TimeSpan.FromSeconds(45));
}

static int DoWifiAction(string action, string? host)
{
    if (host is null)
    {
        PrintWifiUsage();
        return 1;
    }

    return RunManagement(host, $"wifi {action}", async (management, token) =>
    {
        JsonObject response = await management.WifiActionAsync(action.ToLowerInvariant(), token);
        PrintJson(response);
        return 0;
    }, TimeSpan.FromSeconds(30));
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

    var settings = new Dictionary<string, object?>();
    if (ssid is not null)
        settings["ssid"] = ssid;
    if (password is not null)
        settings["password"] = password;
    if (dhcp)
        settings["dhcp"] = true;
    else if (useStatic)
        settings["useStatic"] = true;
    if (staticIp is not null)
        settings["staticIp"] = staticIp;
    if (gateway is not null)
        settings["gateway"] = gateway;
    if (subnet is not null)
        settings["subnet"] = subnet;
    if (dns is not null)
        settings["dns"] = dns;

    return RunManagement(host, "wifi set", async (management, token) =>
    {
        await management.WifiConfigAsync(settings, token);
        Console.WriteLine("WiFi config saved.");
        return 0;
    }, TimeSpan.FromSeconds(30));
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

    return RunManagement(host, "audio status", async (management, token) =>
    {
        JsonObject status = await management.GetStatusAsync(token);
        PrintJson(status["audio"]);
        return 0;
    }, TimeSpan.FromSeconds(30));
}

static int DoAudioStop(string? host)
{
    if (host is null)
    {
        PrintAudioUsage();
        return 1;
    }

    return RunManagement(host, "audio stop", async (management, token) =>
    {
        await management.AudioStopAsync(token);
        Console.WriteLine("Audio stopped.");
        return 0;
    }, TimeSpan.FromSeconds(30));
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
        Console.Error.WriteLine("Menus: use Alt+letter if your terminal supports Meta, Esc then letter, or Esc to close.");

        var repeatFilter = new KeyboardRepeatFilter();
        bool suppressNextRawLf = false;

        while (true)
        {
            bool mapped = oldTerminalMode is not null
                ? TryMapRawInput(rawInput!, ctrlCQuits, ref suppressNextRawLf, out string text, out bool shouldQuit)
                : TryMapConsoleKey(Console.ReadKey(intercept: true), ctrlCQuits, out text, out shouldQuit);
            if (!mapped)
                continue;

            if (shouldQuit)
                break;

            if (!repeatFilter.ShouldSend(text))
                continue;

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

    if (key.Modifiers.HasFlag(ConsoleModifiers.Alt) && TryMapAltKey(key, out char altChar))
    {
        text = "\x1B" + altChar;
        return true;
    }

    text = key.Key switch
    {
        ConsoleKey.Enter      => "\r",
        ConsoleKey.Backspace  => "\b",
        ConsoleKey.Tab        => "\t",
        ConsoleKey.Delete     => "\x7F",
        ConsoleKey.Escape     => "\x1B",
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
    ref bool suppressNextLf,
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

    if (b == 0x0D)
    {
        suppressNextLf = true;
        text = "\r";
        return true;
    }

    if (b == 0x0A)
    {
        if (suppressNextLf)
        {
            suppressNextLf = false;
            return false;
        }

        text = "\r";
        return true;
    }

    suppressNextLf = false;

    text = b switch
    {
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
        return "\x1B";

    if (second == 'O')
    {
        return TryReadPendingInputByte(input, out int final)
            ? MapArrowEscapeFinal(final)
            : "\x1B";
    }

    if (second != '[')
    {
        if (second >= 0x20 && second <= 0x7E)
            return "\x1B" + NormalizeAltChar((char)second);
        return "\x1B";
    }

    for (int i = 0; i < 16; i++)
    {
        if (!TryReadPendingInputByte(input, out int value))
            return "\x03";

        if (value >= 0x40 && value <= 0x7E)
            return MapArrowEscapeFinal(value);
    }

    return "\x1B";
}

static bool TryMapAltKey(ConsoleKeyInfo key, out char altChar)
{
    altChar = '\0';

    if (key.KeyChar is >= ' ' and <= '~')
    {
        altChar = NormalizeAltChar(key.KeyChar);
        return true;
    }

    if (key.Key is >= ConsoleKey.A and <= ConsoleKey.Z)
    {
        altChar = (char)('a' + (key.Key - ConsoleKey.A));
        return true;
    }

    return false;
}

static char NormalizeAltChar(char ch) =>
    ch is >= 'A' and <= 'Z' ? (char)(ch + 0x20) : ch;

static string MapArrowEscapeFinal(int value)
{
    return value switch
    {
        'D' => "\x1C",
        'C' => "\x1D",
        'A' => "\x1E",
        'B' => "\x1F",
        _   => "\x1B",
    };
}

static bool TryReadPendingInputByte(KeyboardInputReader input, out int value)
{
    return input.TryRead(out value, timeoutMs: 650);
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
    string request;
    if (text.Length > 1)
    {
        request = "{\"command\":\"type_text\",\"text\":" + JsonString(text) + "}";
    }
    else
    {
        string key = text switch
        {
            "\r"   => "ENTER",
            "\b"   => "BACKSPACE",
            "\x7F" => "BACKSPACE",
            "\x03" => "CTRL-C",
            _      => text,
        };
        request = "{\"command\":\"send_key\",\"key\":" + JsonString(key) + "}";
    }

    writer.WriteLine(request);
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
    Console.Error.WriteLine("  Menus: use Alt+letter if your terminal supports Meta, or press Esc then the letter.");
}

static int DoVm(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    var rest = args.ToList();
    bool missingCommand = rest.Count == 0;
    bool help = missingCommand || rest[0].Equals("help", StringComparison.OrdinalIgnoreCase);
    help |= TakeFlag(rest, "--help", "-h");
    string? portOpt = TakeOptionValue(rest, "--port", "--debug-port");
    bool pretty = TakeFlag(rest, "--pretty");
    bool json = TakeFlag(rest, "--json");
    bool allowError = TakeFlag(rest, "--allow-error");

    if (help)
    {
        PrintVmUsage();
        return missingCommand ? 1 : 0;
    }

    string targetHost = host ?? "127.0.0.1";
    int port = host is null ? 6502 : 6503;
    if (portOpt is not null && (!int.TryParse(portOpt, out port) || port is < 1 or > 65535))
    {
        Console.Error.WriteLine($"Invalid debug TCP port: {portOpt}");
        return 1;
    }

    string command = rest[0].ToLowerInvariant();
    rest.RemoveAt(0);

    try
    {
        if (host is not null && (command is "reset" or "vm-reset"))
            return DoRemoteVmReset(host, rest, allowError, json, pretty);

        JsonObject request = command switch
        {
            "raw" => BuildVmRawRequest(rest),
            "cold-start" or "coldstart" => BuildVmColdStartRequest(rest),
            "reset" or "vm-reset" => BuildVmResetRequest(rest),
            "wait" or "wait-ready" => BuildVmWaitRequest(rest),
            "screen" => new JsonObject { ["command"] = "read_screen" },
            "line" => BuildVmLineRequest(rest),
            "cursor" => new JsonObject { ["command"] = "get_cursor" },
            "type" or "type-text" => BuildVmTypeTextRequest(rest),
            "line-input" or "enter" => BuildVmLineInputRequest(rest),
            "key" => BuildVmKeyRequest(rest),
            "peek" => BuildVmPeekRequest(rest),
            "peek-block" => BuildVmPeekBlockRequest(rest),
            "poke" => BuildVmPokeRequest(rest),
            "read-vram" => BuildVmReadVramRequest(rest),
            "fill-vram" => BuildVmFillVramRequest(rest),
            "run-cycles" => BuildVmRunCyclesRequest(rest),
            "state" => new JsonObject { ["command"] = "dbg_state" },
            "pause" => new JsonObject { ["command"] = "dbg_pause" },
            "resume" => new JsonObject { ["command"] = "dbg_resume" },
            "step" => new JsonObject { ["command"] = "dbg_step" },
            "break-list" => new JsonObject { ["command"] = "dbg_break_list" },
            "break-clear-all" => new JsonObject { ["command"] = "dbg_break_clear_all" },
            _ => throw new ArgumentException($"Unknown vm command: {command}")
        };

        if (rest.Count > 0)
            throw new ArgumentException($"Unexpected vm argument: {rest[0]}");

        JsonNode response = SendVmRequest(targetHost, port, request);
        bool ok = response["ok"]?.GetValue<bool>() == true;
        if (!allowError && !ok)
        {
            Console.Error.WriteLine(response["error"]?.ToString() ?? response.ToJsonString());
            return 1;
        }

        PrintVmResponse(command, response, json || command == "raw", pretty);
        return ok || allowError ? 0 : 1;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"vm: {ex.Message}");
        return 1;
    }
}

static JsonObject BuildVmRawRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm raw requires a JSON object");

    var node = JsonNode.Parse(args[0]) as JsonObject
        ?? throw new ArgumentException("vm raw payload must be a JSON object");
    args.RemoveAt(0);
    if (node["command"] is null)
        throw new ArgumentException("vm raw payload must include command");
    return node;
}

static JsonObject BuildVmColdStartRequest(List<string> args)
{
    string? runtime = TakeOptionValue(args, "--runtime", "-r");
    string? text = TakeOptionValue(args, "--text");
    bool noWait = TakeFlag(args, "--no-wait");

    var req = new JsonObject
    {
        ["command"] = "cold_start",
        ["wait_ready"] = noWait ? 0 : 1
    };
    if (!string.IsNullOrWhiteSpace(runtime))
        req["runtime"] = runtime;
    if (!string.IsNullOrWhiteSpace(text))
        req["text"] = text;
    return req;
}

static JsonObject BuildVmResetRequest(List<string> args)
{
    string? text = TakeOptionValue(args, "--text");
    bool noWait = TakeFlag(args, "--no-wait");
    var req = new JsonObject
    {
        ["command"] = "vm_reset",
        ["wait_ready"] = noWait ? 0 : 1
    };
    if (!string.IsNullOrWhiteSpace(text))
        req["text"] = text;
    return req;
}

static int DoRemoteVmReset(string host, List<string> args, bool allowError,
                           bool json, bool pretty)
{
    string? text = TakeOptionValue(args, "--text");
    _ = TakeFlag(args, "--no-wait");
    if (!string.IsNullOrWhiteSpace(text))
        throw new ArgumentException("remote vm reset does not wait for screen text; run 'nova vm wait' after reset if needed");
    if (args.Count > 0)
        throw new ArgumentException($"Unexpected vm reset argument: {args[0]}");

    JsonObject response;
    try
    {
        using var management = new NovaHostManagementClient(host);
        JsonObject data = management.VmResetAsync(CancellationToken.None)
            .GetAwaiter().GetResult();
        response = new JsonObject { ["ok"] = true };
        foreach ((string key, JsonNode? value) in data)
        {
            if (value is not null)
                response[key] = value.DeepClone();
        }
    }
    catch (Exception ex)
    {
        response = new JsonObject
        {
            ["ok"] = false,
            ["error"] = ex.Message
        };
        if (!allowError)
        {
            Console.Error.WriteLine(ex.Message);
            return 1;
        }
    }

    PrintVmResponse("reset", response, json, pretty);
    return response["ok"]?.GetValue<bool>() == true || allowError ? 0 : 1;
}

static JsonObject BuildVmWaitRequest(List<string> args)
{
    string? timeout = TakeOptionValue(args, "--timeout-ms");
    string text = args.Count > 0 ? args[0] : "Ready";
    if (args.Count > 0)
        args.RemoveAt(0);
    var req = new JsonObject
    {
        ["command"] = "wait_ready",
        ["text"] = text
    };
    if (timeout is not null)
        req["timeout_ms"] = ParseVmNumber(timeout);
    return req;
}

static JsonObject BuildVmLineRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm line requires a row number");
    int row = ParseVmNumber(args[0]);
    args.RemoveAt(0);
    return new JsonObject { ["command"] = "read_line", ["row"] = row };
}

static JsonObject BuildVmTypeTextRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm type-text requires text");
    string text = args[0];
    args.RemoveAt(0);
    return new JsonObject { ["command"] = "type_text", ["text"] = text };
}

static JsonObject BuildVmLineInputRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm enter requires text");
    string text = args[0];
    args.RemoveAt(0);
    return new JsonObject { ["command"] = "type_text", ["text"] = text + "\r" };
}

static JsonObject BuildVmKeyRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm key requires a key name");
    string key = args[0];
    args.RemoveAt(0);
    return new JsonObject { ["command"] = "send_key", ["key"] = key };
}

static JsonObject BuildVmPeekRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm peek requires an address");
    int address = ParseVmNumber(args[0]);
    args.RemoveAt(0);
    return new JsonObject { ["command"] = "peek", ["address"] = address };
}

static JsonObject BuildVmPeekBlockRequest(List<string> args)
{
    if (args.Count < 2)
        throw new ArgumentException("vm peek-block requires address and count");
    int address = ParseVmNumber(args[0]);
    int count = ParseVmNumber(args[1]);
    args.RemoveRange(0, 2);
    return new JsonObject { ["command"] = "peek_block", ["address"] = address, ["count"] = count };
}

static JsonObject BuildVmPokeRequest(List<string> args)
{
    if (args.Count < 2)
        throw new ArgumentException("vm poke requires address and value");
    int address = ParseVmNumber(args[0]);
    int value = ParseVmNumber(args[1]);
    args.RemoveRange(0, 2);
    return new JsonObject { ["command"] = "poke", ["address"] = address, ["value"] = value };
}

static JsonObject BuildVmReadVramRequest(List<string> args)
{
    if (args.Count < 3)
        throw new ArgumentException("vm read-vram requires space, address, and length");
    int space = ParseVmNumber(args[0]);
    int address = ParseVmNumber(args[1]);
    int length = ParseVmNumber(args[2]);
    args.RemoveRange(0, 3);
    return new JsonObject { ["command"] = "read_vram", ["space"] = space, ["address"] = address, ["length"] = length };
}

static JsonObject BuildVmFillVramRequest(List<string> args)
{
    if (args.Count < 4)
        throw new ArgumentException("vm fill-vram requires space, address, value, and length");
    int space = ParseVmNumber(args[0]);
    int address = ParseVmNumber(args[1]);
    int value = ParseVmNumber(args[2]);
    int length = ParseVmNumber(args[3]);
    args.RemoveRange(0, 4);
    return new JsonObject { ["command"] = "fill_vram", ["space"] = space, ["address"] = address, ["value"] = value, ["length"] = length };
}

static JsonObject BuildVmRunCyclesRequest(List<string> args)
{
    if (args.Count < 1)
        throw new ArgumentException("vm run-cycles requires a cycle count");
    int cycles = ParseVmNumber(args[0]);
    args.RemoveAt(0);
    return new JsonObject { ["command"] = "run_cycles", ["cycles"] = cycles };
}

static JsonNode SendVmRequest(string host, int port, JsonObject request)
{
    using var client = new TcpClient();
    using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(15));
    client.ConnectAsync(host, port, cts.Token).GetAwaiter().GetResult();

    using NetworkStream stream = client.GetStream();
    stream.ReadTimeout = 15000;
    stream.WriteTimeout = 15000;
    using var reader = new StreamReader(stream, Encoding.UTF8, detectEncodingFromByteOrderMarks: false,
                                        bufferSize: 65536, leaveOpen: true);
    using var writer = new StreamWriter(stream, new UTF8Encoding(false), bufferSize: 4096,
                                        leaveOpen: true)
    {
        AutoFlush = true,
        NewLine = "\n",
    };

    writer.WriteLine(request.ToJsonString());
    string? line = reader.ReadLine();
    if (string.IsNullOrWhiteSpace(line))
        throw new IOException("empty response from VM debug server");

    return JsonNode.Parse(line)
        ?? throw new IOException("invalid JSON response from VM debug server");
}

static void PrintVmResponse(string command, JsonNode response, bool json, bool pretty)
{
    if (json)
    {
        Console.WriteLine(response.ToJsonString(new JsonSerializerOptions { WriteIndented = pretty }));
        return;
    }

    switch (command)
    {
        case "screen":
            if (response["lines"] is JsonArray lines)
                foreach (var line in lines)
                    Console.WriteLine(line?.GetValue<string>() ?? "");
            else
                Console.WriteLine(response.ToJsonString());
            break;
        case "line":
            Console.WriteLine(response["text"]?.GetValue<string>() ?? "");
            break;
        case "peek":
            Console.WriteLine($"${(response["value"]?.GetValue<int>() ?? 0) & 0xFF:X2}");
            break;
        default:
            Console.WriteLine(response.ToJsonString(new JsonSerializerOptions { WriteIndented = pretty }));
            break;
    }
}

static int ParseVmNumber(string value)
{
    string text = value.Trim();
    if (text.StartsWith('$'))
        return Convert.ToInt32(text[1..], 16);
    if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        return Convert.ToInt32(text[2..], 16);
    return int.Parse(text, System.Globalization.CultureInfo.InvariantCulture);
}

static void PrintVmUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova vm [--remote <host>] [--port <port>] <command>");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Defaults:");
    Console.Error.WriteLine("  no --remote: 127.0.0.1:6502 (Avalonia)");
    Console.Error.WriteLine("  with --remote: reset uses management TCP; debug commands use <host>:6503");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Commands:");
    Console.Error.WriteLine("  cold-start [--runtime basic|logo|forth] [--text <text>] [--no-wait]");
    Console.Error.WriteLine("  reset [--text <text>] [--no-wait]");
    Console.Error.WriteLine("  wait [text] [--timeout-ms <ms>]");
    Console.Error.WriteLine("  screen [--json]");
    Console.Error.WriteLine("  line <row>");
    Console.Error.WriteLine("  cursor");
    Console.Error.WriteLine("  type-text <text>");
    Console.Error.WriteLine("  enter <text>");
    Console.Error.WriteLine("  key <key>");
    Console.Error.WriteLine("  peek <addr>");
    Console.Error.WriteLine("  peek-block <addr> <count> [--json]");
    Console.Error.WriteLine("  poke <addr> <value>");
    Console.Error.WriteLine("  read-vram <space> <addr> <length> [--json]");
    Console.Error.WriteLine("  fill-vram <space> <addr> <value> <length>");
    Console.Error.WriteLine("  run-cycles <count>");
    Console.Error.WriteLine("  state|pause|resume|step|break-list|break-clear-all");
    Console.Error.WriteLine("  raw '<json>' [--json] [--allow-error]");
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

// ===========================================================================
// runtime — read/modify /config/boot.json on the remote NovaHost SD card
// ===========================================================================

static int DoRuntime(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintRuntimeUsage();
        return 1;
    }

    return args[0].ToLowerInvariant() switch
    {
        "list" or "ls" => DoRuntimeList(host),
        "status"       => DoRuntimeStatus(host),
        "set"          => DoRuntimeSet(args[1..], host),
        "add"          => DoRuntimeAdd(args[1..], host),
        "remove" or "rm" => DoRuntimeRemove(args[1..], host),
        "deploy"       => DoRuntimeDeploy(args[1..], host),
        "boot-floppy" or "bootfloppy" => DoRuntimeBootFloppy(args[1..], host),
        _              => UnknownRuntimeCommand(args[0]),
    };
}

static JsonNode? ReadBootConfig(string host)
{
    using var management = new NovaHostManagementClient(host);
    try
    {
        string json = management.ReadTextFileAsync("/config/boot.json",
            CancellationToken.None).GetAwaiter().GetResult();
        if (!string.IsNullOrWhiteSpace(json))
            return JsonNode.Parse(json);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"boot config read: {ex.Message}");
    }

    Console.Error.WriteLine("No boot config found - creating default.");
    return CreateDefaultBootConfig();
}

static JsonNode CreateDefaultBootConfig()
{
    return JsonNode.Parse("""
    {
      "vm": {
        "defaultRuntime": "novabasic"
      },
      "languages": {
        "novabasic": {
          "rom": "/roms/novabasic.bin",
          "extensionRom": "/roms/extension.bin",
          "autoboot": true
        }
      },
      "mounts": {},
      "network": {}
    }
    """)!;
}

static bool WriteBootConfig(string host, JsonNode config)
{
    string json = config.ToJsonString(new JsonSerializerOptions { WriteIndented = true });
    byte[] bytes = Encoding.UTF8.GetBytes(json + "\n");
    using var management = new NovaHostManagementClient(host);
    try
    {
        using var input = new MemoryStream(bytes);
        management.WriteFileAsync("/config/boot.json", input, bytes.Length,
            null, CancellationToken.None).GetAwaiter().GetResult();
        return true;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"boot config write: {ex.Message}");
        return false;
    }
}

static int DoRuntimeList(string? host)
{
    if (host is null) { PrintRuntimeUsage(); return 1; }

    var config = ReadBootConfig(host);
    if (config is null) return 1;

    string defaultRt = config["vm"]?["defaultRuntime"]?.GetValue<string>() ?? "";
    var langs = config["languages"]?.AsObject();
    if (langs is null)
    {
        Console.WriteLine("No runtimes configured.");
        return 0;
    }

    Console.WriteLine($"{"Name",-20} {"ROM",-32} {"Extension",-32} Default");
    Console.WriteLine(new string('-', 90));
    foreach (var (name, node) in langs)
    {
        string rom = node?["rom"]?.GetValue<string>() ?? "";
        string ext = node?["extensionRom"]?.GetValue<string>() ?? "";
        string def = name == defaultRt ? "*" : "";
        Console.WriteLine($"{name,-20} {rom,-32} {ext,-32} {def}");
    }
    return 0;
}

static int DoRuntimeStatus(string? host)
{
    if (host is null) { PrintRuntimeUsage(); return 1; }

    var config = ReadBootConfig(host);
    if (config is null) return 1;

    string defaultRt = config["vm"]?["defaultRuntime"]?.GetValue<string>() ?? "(not set)";
    Console.WriteLine($"Default runtime: {defaultRt}");

    var langs = config["languages"]?.AsObject();
    if (langs is not null && langs.ContainsKey(defaultRt))
    {
        var node = langs[defaultRt];
        string rom = node?["rom"]?.GetValue<string>() ?? "(not set)";
        string ext = node?["extensionRom"]?.GetValue<string>() ?? "(none)";
        Console.WriteLine($"  ROM:           {rom}");
        Console.WriteLine($"  Extension ROM: {ext}");
    }
    else if (defaultRt != "(not set)")
    {
        Console.WriteLine($"  Warning: runtime '{defaultRt}' not found in languages section.");
    }
    return 0;
}

static int DoRuntimeSet(string[] args, string? host)
{
    if (host is null || args.Length < 1) { PrintRuntimeUsage(); return 1; }

    string name = args[0];
    var config = ReadBootConfig(host);
    if (config is null) return 1;

    var langs = config["languages"]?.AsObject();
    if (langs is null || !langs.ContainsKey(name))
    {
        Console.Error.WriteLine($"Runtime '{name}' not found. Use 'nova runtime list' to see available runtimes.");
        return 1;
    }

    var vm = config["vm"];
    if (vm is null)
    {
        Console.Error.WriteLine("boot.json has no 'vm' section.");
        return 1;
    }
    vm["defaultRuntime"] = name;

    if (!WriteBootConfig(host, config)) return 1;

    Console.WriteLine($"Default runtime set to '{name}'. Reboot to apply.");
    return 0;
}

static int DoRuntimeAdd(string[] args, string? host)
{
    if (host is null) { PrintRuntimeUsage(); return 1; }

    var rest = args.ToList();
    string? romPath = TakeOptionValue(rest, "--rom");
    string? extPath = TakeOptionValue(rest, "--ext", "--extension");

    if (rest.Count < 1 || romPath is null)
    {
        PrintRuntimeUsage();
        return 1;
    }

    string name = rest[0];

    var config = ReadBootConfig(host);
    if (config is null) return 1;

    var langs = config["languages"]?.AsObject();
    if (langs is null)
    {
        Console.Error.WriteLine("boot.json has no 'languages' section.");
        return 1;
    }

    var entry = new JsonObject();
    entry["rom"] = romPath;
    if (extPath is not null)
        entry["extensionRom"] = extPath;

    langs[name] = entry;

    if (!WriteBootConfig(host, config)) return 1;

    Console.WriteLine($"Runtime '{name}' added (rom={romPath}{(extPath is not null ? $", ext={extPath}" : "")}).");
    return 0;
}

static int DoRuntimeRemove(string[] args, string? host)
{
    if (host is null || args.Length < 1) { PrintRuntimeUsage(); return 1; }

    string name = args[0];
    var config = ReadBootConfig(host);
    if (config is null) return 1;

    string defaultRt = config["vm"]?["defaultRuntime"]?.GetValue<string>() ?? "";
    if (name == defaultRt)
    {
        Console.Error.WriteLine($"Cannot remove the active default runtime '{name}'. Use 'nova runtime set' to change the default first.");
        return 1;
    }

    var langs = config["languages"]?.AsObject();
    if (langs is null || !langs.ContainsKey(name))
    {
        Console.Error.WriteLine($"Runtime '{name}' not found.");
        return 1;
    }

    langs.Remove(name);

    if (!WriteBootConfig(host, config)) return 1;

    Console.WriteLine($"Runtime '{name}' removed.");
    return 0;
}

static int DoRuntimeDeploy(string[] args, string? host)
{
    if (host is null) { PrintRuntimeUsage(); return 1; }

    var rest = args.ToList();
    string? localRom = TakeOptionValue(rest, "--rom");
    string? localExt = TakeOptionValue(rest, "--ext", "--extension");

    if (rest.Count < 1 || localRom is null)
    {
        PrintRuntimeUsage();
        return 1;
    }

    string name = rest[0];

    if (!File.Exists(localRom))
    {
        Console.Error.WriteLine($"ROM file not found: {localRom}");
        return 1;
    }

    if (localExt is not null && !File.Exists(localExt))
    {
        Console.Error.WriteLine($"Extension ROM file not found: {localExt}");
        return 1;
    }

    // Upload ROM files to /roms/ on SD
    string romFilename = Path.GetFileName(localRom);
    string remoteRomPath = $"/roms/{romFilename}";
    int rc = UploadRomFile(host, localRom, remoteRomPath);
    if (rc != 0) return rc;

    string? remoteExtPath = null;
    if (localExt is not null)
    {
        string extFilename = Path.GetFileName(localExt);
        remoteExtPath = $"/roms/{extFilename}";
        rc = UploadRomFile(host, localExt, remoteExtPath);
        if (rc != 0) return rc;
    }

    // Add/update the runtime entry in boot.json
    var config = ReadBootConfig(host);
    if (config is null) return 1;

    var langs = config["languages"]?.AsObject();
    if (langs is null)
    {
        Console.Error.WriteLine("boot.json has no 'languages' section.");
        return 1;
    }

    var entry = new JsonObject();
    entry["rom"] = remoteRomPath;
    if (remoteExtPath is not null)
        entry["extensionRom"] = remoteExtPath;

    langs[name] = entry;

    if (!WriteBootConfig(host, config)) return 1;

    Console.WriteLine($"Deployed runtime '{name}': rom={remoteRomPath}{(remoteExtPath is not null ? $", ext={remoteExtPath}" : "")}.");
    return 0;
}

static int UploadRomFile(string host, string localPath, string remoteSdPath)
{
    return PutFile(localPath, remoteSdPath, host);
}

static int UnknownRuntimeCommand(string command)
{
    Console.Error.WriteLine($"Unknown runtime command: {command}");
    PrintRuntimeUsage();
    return 1;
}

static void PrintRuntimeUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova runtime list --remote <host>");
    Console.Error.WriteLine("  nova runtime status --remote <host>");
    Console.Error.WriteLine("  nova runtime set <name> --remote <host>");
    Console.Error.WriteLine("  nova runtime add <name> --rom <sd-path> [--ext <sd-path>] --remote <host>");
    Console.Error.WriteLine("  nova runtime remove <name> --remote <host>");
    Console.Error.WriteLine("  nova runtime deploy <name> --rom <local-file> [--ext <local-file>] --remote <host>");
    Console.Error.WriteLine("  nova runtime boot-floppy <name> [remote-path] --remote <host>");
}

// Create a boot floppy: write an AUTOBOOT.bin launcher that swaps BASIC for the
// named /roms runtime and cold-boots it. Defaults the remote path to AUTOBOOT.bin.
static int DoRuntimeBootFloppy(string[] args, string? host)
{
    var rest = args.ToList();
    if (host is null || rest.Count < 1)
    {
        Console.Error.WriteLine("Usage: nova --remote=<host> runtime boot-floppy <runtime-name> [remote-path]");
        return 1;
    }

    string runtime = rest[0];
    string remote = (rest.Count > 1) ? NormalizeSdRelativePath(rest[1]) : "AUTOBOOT.bin";

    byte[] bytes;
    try { bytes = RuntimeLauncher.Build(runtime); }
    catch (ArgumentException ex) { Console.Error.WriteLine(ex.Message); return 1; }

    string tmp = Path.Combine(Path.GetTempPath(), "nova-autoboot-" + Guid.NewGuid().ToString("N") + ".bin");
    try
    {
        File.WriteAllBytes(tmp, bytes);
        Console.WriteLine($"boot floppy: AUTOBOOT launches runtime '{runtime}' ({bytes.Length} bytes) -> {remote}");
        return PutFile(tmp, remote, host);
    }
    finally { try { File.Delete(tmp); } catch { /* best effort */ } }
}

static int DoLs(string[] args, string? host)
{
    if (host is null) {
        Console.Error.WriteLine("Usage: nova --remote=<host> ls [path]");
        return 1;
    }
    string subdir = (args.Length > 0) ? NormalizeSdRelativePath(args[0]) : "";
    if (subdir.Length > 0 && !subdir.EndsWith('/')) subdir += "/";

    return RunManagement(host, "ls", async (management, token) =>
    {
        JsonArray rows = await management.ListDirectoryAsync(subdir, token);
        Console.WriteLine($"Listing of /sd/{subdir} on {host}:");
        Console.WriteLine();
        Console.WriteLine("  Name                              Size      Type");
        Console.WriteLine("  --------------------------------  --------  ----");
        foreach (JsonNode? node in rows)
        {
            JsonObject? entry = node as JsonObject;
            string name = entry?["name"]?.GetValue<string>() ?? "?";
            long size = entry?["size"]?.GetValue<long>() ?? 0;
            bool isDir = entry?["dir"]?.GetValue<bool>() == true;
            string sizeStr = isDir ? "<DIR>" : size.ToString();
            string typeStr = isDir ? "DIR" : "FILE";
            Console.WriteLine($"  {name,-32}  {sizeStr,8}  {typeStr}");
        }
        return 0;
    }, TimeSpan.FromSeconds(30));
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

    return RunManagement(host, "put", async (management, token) =>
    {
        using Stream stream = uploadBytes is not null
            ? new MemoryStream(uploadBytes)
            : new FileStream(local, FileMode.Open, FileAccess.Read);
        await management.WriteFileAsync(remote, stream, uploadLength,
            CreateProgressReporter("PUT", uploadLength), token);
        Console.WriteLine($"PUT {displayLocal} -> /sd/{NormalizeSdRelativePath(remote)} ({uploadLength} bytes) OK");
        return 0;
    });
}

static int DoGet(string[] args, string? host)
{
    if (host is null || args.Length < 1) {
        Console.Error.WriteLine("Usage: nova --remote=<host> get <remote-path> [local-path]");
        return 1;
    }
    string remote = NormalizeSdRelativePath(args[0]);
    string local  = (args.Length > 1) ? args[1] : Path.GetFileName(remote);

    return RunManagement(host, "get", async (management, token) =>
    {
        using var fs = new FileStream(local, FileMode.Create, FileAccess.Write);
        long size = await management.DownloadFileAsync(remote, fs,
            CreateTransferProgressReporter("GET"), token);
        Console.WriteLine($"GET /sd/{remote} -> {local} ({size} bytes) OK");
        return 0;
    });
}

static int DoRm(string[] args, string? host)
{
    if (host is null || args.Length < 1) {
        Console.Error.WriteLine("Usage: nova --remote=<host> rm <remote-path>");
        return 1;
    }
    string remote = NormalizeSdRelativePath(args[0]);

    return RunManagement(host, "rm", async (management, token) =>
    {
        await management.DeletePathAsync(remote, token);
        Console.WriteLine($"DELETE /sd/{remote} OK");
        return 0;
    }, TimeSpan.FromSeconds(30));
}

static string TruncateForTable(string value, int max)
{
    if (value.Length <= max)
        return value;
    if (max <= 1)
        return value[..max];
    return value[..(max - 1)] + "…";
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
        ".4th" or ".fth" or ".fs" or ".fr" => NdiFileType.Forth,
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
    Console.Error.WriteLine("  runtime list|status|set|add|remove|deploy ... --remote <host>");
    Console.Error.WriteLine("  webserver --remote <host> [--port 8080] [--no-open]");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Raw SD commands, kept for compatibility:");
    Console.Error.WriteLine("  --remote <host> ls [path]");
    Console.Error.WriteLine("  --remote <host> put <local-path> [remote-path]");
    Console.Error.WriteLine("  --remote <host> get <remote-path> [local-path]");
    Console.Error.WriteLine("  --remote <host> rm <remote-path>");
}

sealed class KeyboardRepeatFilter
{
    private const int DebounceMs = 120;
    private const int InitialRepeatDelayMs = 1000;
    private const int RepeatIntervalMs = 80;
    private const int SameKeyIdleResetMs = 180;

    private string? _activeText;
    private long _activeStartMs;
    private long _lastSeenMs;
    private long _lastSentMs;

    public bool ShouldSend(string text)
    {
        if (!IsFilteredKey(text))
        {
            Reset();
            return true;
        }

        long now = Environment.TickCount64;
        if (_activeText != text || now - _lastSeenMs > SameKeyIdleResetMs)
        {
            _activeText = text;
            _activeStartMs = now;
            _lastSeenMs = now;
            _lastSentMs = now;
            return true;
        }

        _lastSeenMs = now;
        if (now - _lastSentMs < DebounceMs)
            return false;

        if (now - _activeStartMs < InitialRepeatDelayMs)
            return false;

        if (now - _lastSentMs < RepeatIntervalMs)
            return false;

        _lastSentMs = now;
        return true;
    }

    private static bool IsFilteredKey(string text)
    {
        if (text.Length != 1)
            return false;

        char ch = text[0];
        return ch < ' ' || ch == '\x7F';
    }

    private void Reset()
    {
        _activeText = null;
        _activeStartMs = 0;
        _lastSeenMs = 0;
        _lastSentMs = 0;
    }
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

sealed class InlineProgress<T> : IProgress<T>
{
    private readonly Action<T> _handler;

    public InlineProgress(Action<T> handler)
    {
        _handler = handler;
    }

    public void Report(T value)
    {
        _handler(value);
    }
}
