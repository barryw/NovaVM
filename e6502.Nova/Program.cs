using System.Net.Sockets;
using System.IO.Compression;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
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
    "capture"    => DoCapture(args[1..], remoteHost),
    "check"      => DoCheck(args[1..], remoteHost),
    "disk"       => DoDisk(args[1..], remoteHost),
    "rom"        => DoRom(args[1..], remoteHost),
    "soundfont"  => DoSoundfont(args[1..], remoteHost),
    "music"      => DoMusic(args[1..], remoteHost),
    "asset"      => DoAsset(args[1..], remoteHost),
    "runtime"    => DoRuntime(args[1..], remoteHost),
    "webserver" or "web" => DoWebServer(args[1..], remoteHost),
    "module"     => DoModule(args[1..], remoteHost),
    "arty"       => DoArty(args[1..], remoteHost),
    "publish"    => DoPublish(args[1..]),
    "convert"    => DoConvert(args[1..]),
    "codegen"    => DoCodegen(args[1..]),
    "build"      => DoBuild(args[1..]),
    "docs"       => DoDocs(args[1..]),
    "fpga"       => DoFpga(args[1..]),
    "ci"         => DoCi(args[1..]),
    _            => UnknownVerb(verb),
};

// ===========================================================================
// Remote SD operations — talks to NovaHost's TCP management service.
// ===========================================================================

static int DoArty(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintArtyUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    string repo = ResolveRepoRoot(rest);

    try
    {
        return command switch
        {
            "sync-payloads" => DoArtySyncPayloads(repo, rest),
            "build-linux-host" => DoArtyBuildLinuxHost(repo, rest),
            "build-ps-fio" => DoArtyBuildPsFio(repo, rest),
            "deploy-linux-host" => DoArtyDeployLinuxHost(repo, rest, host, editorDemo: TakeFlag(rest, "--editor-demo")),
            "deploy-editor-demo" => DoArtyDeployLinuxHost(repo, rest, host, editorDemo: true),
            "make-boot-bin" => DoArtyMakeBootBin(repo, rest),
            _ => UnknownArtyCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"arty {command}: {ex.Message}");
        return 1;
    }
}

static void PrintArtyUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova arty sync-payloads [--repo <repo>] [--check]");
    Console.Error.WriteLine("  nova arty build-linux-host [--repo <repo>]");
    Console.Error.WriteLine("  nova arty build-ps-fio [--repo <repo>] [--vitis <path>]");
    Console.Error.WriteLine("  nova arty deploy-linux-host [--repo <repo>] [--remote <ip>] [--editor-demo]");
    Console.Error.WriteLine("  nova arty deploy-editor-demo [--repo <repo>] [--remote <ip>]");
    Console.Error.WriteLine("  nova arty make-boot-bin [--repo <repo>] [--workspace <path>] [--bootgen <path>]");
}

static int UnknownArtyCommand(string command)
{
    Console.Error.WriteLine($"Unknown arty command: {command}");
    PrintArtyUsage();
    return 1;
}

static string ResolveRepoRoot(List<string> args)
{
    string? repo = TakeOptionValue(args, "--repo");
    if (!string.IsNullOrWhiteSpace(repo))
        return Path.GetFullPath(repo);

    string dir = Directory.GetCurrentDirectory();
    while (!string.IsNullOrEmpty(dir))
    {
        if (File.Exists(Path.Combine(dir, "e6502.sln")))
            return dir;

        string? parent = Directory.GetParent(dir)?.FullName;
        if (parent == dir)
            break;
        dir = parent ?? "";
    }

    return Directory.GetCurrentDirectory();
}

static int DoArtySyncPayloads(string repo, List<string> args)
{
    bool check = TakeFlag(args, "--check");
    if (args.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected arty sync-payloads argument: {args[0]}");
        PrintArtyUsage();
        return 1;
    }

    if (!check)
    {
        int rc = RunCommand("make", MakeArgsWithCurrentNova(repo, "-C", Path.Combine(repo, "e6502.FPGA"), "hex"));
        if (rc != 0) return rc;

        rc = RunCommand("make", ["-C", Path.Combine(repo, "software", "runtime", "asm")]);
        if (rc != 0) return rc;

        foreach (string module in ArtyModuleNames())
        {
            rc = RunCommand("make", MakeArgsWithCurrentNova(repo, "-C", Path.Combine(repo, "software", "modules", module)));
            if (rc != 0) return rc;
        }

        rc = RunCommand("make", MakeArgsWithCurrentNova(repo, "-C", Path.Combine(repo, "software", "languages", "novalogo"), "ndi"));
        if (rc != 0) return rc;

        rc = RunCommand("make", MakeArgsWithCurrentNova(repo, "-C", Path.Combine(repo, "software", "languages", "novaforth"), "ndi"));
        if (rc != 0) return rc;

        rc = RunCommand("make", ["-C", Path.Combine(repo, "software", "assembly"), "editbuf-demo"]);
        if (rc != 0) return rc;
    }

    bool dirty = false;
    dirty |= WriteBinHeader(
        Path.Combine(repo, "software", "runtime", "asm", "libcall.bin"),
        Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "linux", "novavm", "loader_bin.h"),
        "LOADER_BIN",
        check);
    dirty |= WriteBinHeader(
        Path.Combine(repo, "software", "runtime", "asm", "libcall.bin"),
        Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "ps_fio", "src", "loader_bin.h"),
        "LOADER_BIN",
        check);
    dirty |= WriteBinHeader(
        Path.Combine(repo, "software", "languages", "ehbasic", "basic.bin"),
        Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "ps_fio", "src", "ehbasic_rom.h"),
        "EHBASIC_ROM",
        check);

    string modules = RenderArtyModules(repo);
    dirty |= WriteTextIfNeeded(
        Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "linux", "novavm", "modules_embedded.h"),
        modules,
        check);
    dirty |= WriteTextIfNeeded(
        Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "ps_fio", "src", "modules_embedded.h"),
        modules,
        check);

    return check && dirty ? 1 : 0;
}

static string[] MakeArgsWithCurrentNova(string repo, params string[] args) =>
    [.. args, "NOVA_CLI=" + CurrentNovaExecutable(repo)];

static string CurrentNovaExecutable(string repo)
{
    string? processPath = Environment.ProcessPath;
    if (IsNovaExecutable(processPath))
        return processPath!;

    string outputRoot = Path.Combine(repo, "e6502.Nova", "bin", "Release", "net10.0");
    string executableName = OperatingSystem.IsWindows() ? "nova.exe" : "nova";
    if (Directory.Exists(outputRoot))
    {
        string? executable = Directory.GetFiles(outputRoot, executableName, SearchOption.AllDirectories)
            .OrderByDescending(File.GetLastWriteTimeUtc)
            .FirstOrDefault();
        if (executable is not null)
            return executable;
    }

    throw new FileNotFoundException("Build the Nova CLI before running Arty payload builds.", Path.Combine(outputRoot, executableName));
}

static bool IsNovaExecutable(string? path)
{
    if (string.IsNullOrWhiteSpace(path))
        return false;
    string name = Path.GetFileName(path);
    return name.Equals("nova", StringComparison.OrdinalIgnoreCase)
        || name.Equals("nova.exe", StringComparison.OrdinalIgnoreCase);
}

static int DoArtyBuildLinuxHost(string repo, List<string> args)
{
    if (args.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected arty build-linux-host argument: {args[0]}");
        PrintArtyUsage();
        return 1;
    }

    int rc = DoArtySyncPayloads(repo, []);
    if (rc != 0) return rc;

    string hostDir = Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "linux", "novavm");
    string cc = Environment.GetEnvironmentVariable("CC") ?? "arm-linux-gnueabihf-gcc";
    string[] sources =
    [
        "novavm.c", "naudio.c", "nservers.c", "nkbd.c", "nmouse.c", "nfio.c",
        "nsplash.c", "nbootcfg.c", "nosd.c", "cJSON.c"
    ];

    rc = RunCommand(cc, ["-O2", "-static", "-pthread", "-I.", "-o", "novavm", .. sources, "-lm"], hostDir);
    if (rc != 0) return rc;

    _ = RunCommand(cc, ["--version"], hostDir);
    _ = RunCommand("arm-linux-gnueabihf-size", ["novavm"], hostDir);
    Console.WriteLine($"built: {Path.Combine(hostDir, "novavm")}");
    return 0;
}

static int DoArtyBuildPsFio(string repo, List<string> args)
{
    string vitis = TakeOptionValue(args, "--vitis") ?? Environment.GetEnvironmentVariable("VITIS") ?? "/tools/Xilinx/Vitis/2024.2/bin/vitis";
    if (args.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected arty build-ps-fio argument: {args[0]}");
        PrintArtyUsage();
        return 1;
    }

    int rc = DoArtySyncPayloads(repo, []);
    if (rc != 0) return rc;

    string boardDir = Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7");
    return RunCommand(vitis, ["-s", Path.Combine(boardDir, "vitis", "build_ps_fio.py")], boardDir,
        new Dictionary<string, string?>
        {
            ["NOVA_ARTY_SYNC_PAYLOADS_DONE"] = "1",
        });
}

static int DoArtyDeployLinuxHost(string repo, List<string> args, string? host, bool editorDemo)
{
    if (args.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected arty deploy argument: {args[0]}");
        PrintArtyUsage();
        return 1;
    }

    host ??= "192.168.1.188";
    string remote = "root@" + host;
    string hostDir = Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7", "linux", "novavm");

    int rc = RunSsh(host, "true");
    if (rc != 0)
    {
        Console.Error.WriteLine($"cannot reach {remote} over SSH");
        Console.Error.WriteLine($"If SSH says the host key changed, run: ssh-keygen -R {host}");
        return rc;
    }

    rc = DoArtyBuildLinuxHost(repo, []);
    if (rc != 0) return rc;

    using TempFile? editorDemoImage = editorDemo ? TempFile.Create(".ndi") : null;
    if (editorDemoImage is not null)
    {
        rc = CreateEditorDemoImage(repo, editorDemoImage.Path);
        if (rc != 0) return rc;
    }

    rc = RunSsh(host, "mkdir -p /data/nova/roms /data/nova/disks/floppy");
    if (rc != 0) return rc;

    rc = RunScp(host, Path.Combine(hostDir, "novavm"), "/run/novavm.new");
    if (rc != 0) return rc;

    rc = RunScp(host, Path.Combine(repo, "software", "languages", "ehbasic", "basic.bin"), "/data/nova/roms/ehbasic.bin");
    if (rc != 0) return rc;

    rc = RunScp(host, Path.Combine(repo, "software", "languages", "novalogo", "novalogo.ndi"), "/data/nova/disks/floppy/novalogo.ndi");
    if (rc != 0) return rc;

    rc = RunScp(host, Path.Combine(repo, "software", "languages", "novaforth", "novaforth.ndi"), "/data/nova/disks/floppy/novaforth.ndi");
    if (rc != 0) return rc;

    if (editorDemo)
    {
        rc = RunScp(host, editorDemoImage!.Path, "/data/nova/disks/floppy/editor-demo.ndi");
        if (rc != 0) return rc;

        rc = UnmountAllBootDrives(host);
        if (rc != 0) return rc;

        rc = DoDriveMount(["fd0", "/disks/floppy/editor-demo.ndi"], host);
        if (rc != 0) return rc;
    }

    string installAndStop = "mount -o remount,rw / || exit 1; " +
                            "cp /run/novavm.new /usr/bin/novavm && chmod 0755 /usr/bin/novavm && sync; " +
                            "rc=$?; mount -o remount,ro / 2>/dev/null || true; rm -f /run/novavm.new; " +
                            "[ \"$rc\" -eq 0 ] || exit \"$rc\"; " +
                            "for p in $(pidof novavm 2>/dev/null); do kill \"$p\" 2>/dev/null || true; done; sleep 1";
    rc = RunSsh(host, installAndStop);
    if (rc != 0) return rc;

    rc = VerifyRemoteFileSha256(host, Path.Combine(hostDir, "novavm"), "/usr/bin/novavm");
    if (rc != 0) return rc;

    rc = RunSsh(host, "/etc/init.d/novavm start");
    if (rc != 0) return rc;

    rc = WaitForRemotePort(host, 6504, TimeSpan.FromSeconds(10));
    if (rc != 0)
    {
        _ = RunSsh(host, "tail -80 /run/novavm.log");
        return rc;
    }

    if (editorDemo)
    {
        Thread.Sleep(TimeSpan.FromSeconds(2));
        return RunSsh(host, "tail -80 /run/novavm.log");
    }

    return 0;
}

static int CreateEditorDemoImage(string repo, string imagePath)
{
    try
    {
        NdiImage.CreateFormatted(imagePath, "EDITDEMO", 800);
        using var image = NdiImage.Open(imagePath);
        byte[] autoboot = File.ReadAllBytes(
            Path.Combine(repo, "software", "assembly", "apps", "editbuf_demo", "editbuf_demo.bin"));
        image.WriteFile("AUTOBOOT.BIN", NdiFileType.Bin, 0xFFFF, autoboot);
        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"editor demo image: {ex.Message}");
        return 1;
    }
}

static int WaitForRemotePort(string host, int port, TimeSpan timeout)
{
    DateTime deadline = DateTime.UtcNow + timeout;
    Exception? lastError = null;
    while (DateTime.UtcNow < deadline)
    {
        try
        {
            using var client = new TcpClient();
            Task connect = client.ConnectAsync(host, port);
            if (connect.Wait(TimeSpan.FromMilliseconds(500)) && client.Connected)
                return 0;
        }
        catch (Exception ex)
        {
            lastError = ex;
        }

        Thread.Sleep(250);
    }

    Console.Error.WriteLine(lastError is null
        ? $"timed out waiting for {host}:{port}"
        : $"timed out waiting for {host}:{port}: {lastError.Message}");
    return 1;
}

static int UnmountAllBootDrives(string host)
{
    foreach (string slot in new[] { "fd0", "fd1", "fd2", "fd3", "hd0", "hd1" })
    {
        int rc = DoDriveUnmount([slot], host);
        if (rc != 0) return rc;
    }

    return 0;
}

static int DoArtyMakeBootBin(string repo, List<string> args)
{
    string workspace = TakeOptionValue(args, "--workspace") ?? Environment.GetEnvironmentVariable("NOVA_FIO_WS") ?? "/tmp/nova_fio_ws";
    string bootgen = TakeOptionValue(args, "--bootgen") ?? Environment.GetEnvironmentVariable("BOOTGEN") ?? "/tools/Xilinx/Vitis/2024.2/bin/bootgen";
    if (args.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected arty make-boot-bin argument: {args[0]}");
        PrintArtyUsage();
        return 1;
    }

    int rc = DoArtySyncPayloads(repo, []);
    if (rc != 0) return rc;

    string boardDir = Path.Combine(repo, "e6502.FPGA", "boards", "arty_z7");
    string fsbl = Path.Combine(workspace, "nova_fio_plat", "export", "nova_fio_plat", "sw", "boot", "fsbl.elf");
    string bit = Path.Combine(boardDir, "build", "ps_full", "ps_full.runs", "impl_1", "arty_z7_full.bit");
    string app = Path.Combine(workspace, "ps_fio", "build", "ps_fio.elf");
    foreach (string file in new[] { fsbl, bit, app })
    {
        if (!File.Exists(file))
        {
            Console.Error.WriteLine($"ERROR: missing {file}");
            return 1;
        }
    }

    DateTime appTime = File.GetLastWriteTimeUtc(app);
    string psSrc = Path.Combine(boardDir, "ps_fio", "src");
    string? newer = Directory.GetFiles(psSrc, "*", SearchOption.AllDirectories)
                             .FirstOrDefault(path => File.GetLastWriteTimeUtc(path) > appTime);
    if (newer is not null)
    {
        Console.Error.WriteLine($"ERROR: {app} is stale; rebuild the PS app with Vitis before packaging BOOT.bin");
        Console.Error.WriteLine($"newer source: {newer}");
        return 1;
    }

    string buildDir = Path.Combine(boardDir, "build");
    Directory.CreateDirectory(buildDir);
    string bif = Path.Combine(buildDir, "boot.bif");
    File.WriteAllText(bif,
        "the_ROM_image:\n" +
        "{\n" +
        $"\t[bootloader]{fsbl}\n" +
        $"\t{bit}\n" +
        $"\t{app}\n" +
        "}\n");

    string output = Path.Combine(buildDir, "BOOT.bin");
    rc = RunCommand(bootgen, ["-arch", "zynq", "-image", bif, "-w", "on", "-o", output]);
    if (rc != 0) return rc;
    Console.WriteLine($"BOOT.bin -> {output}");
    Console.WriteLine(new FileInfo(output).FullName);
    return 0;
}

static string[] ArtyModuleNames() =>
[
    "graphics", "system", "editor", "sound", "files", "memory", "net", "turtle"
];

static (string Symbol, string Name)[] ArtyEmbeddedModules() =>
[
    ("MOD_GRAPHICS", "graphics"),
    ("MOD_SOUND", "sound"),
    ("MOD_SYSTEM", "system"),
    ("MOD_FILES", "files"),
    ("MOD_MEMORY", "memory"),
    ("MOD_NET", "net"),
    ("MOD_TURTLE", "turtle"),
    ("MOD_EDITOR", "editor"),
];

static bool WriteBinHeader(string input, string output, string symbol, bool check, string generator = "nova arty sync-payloads")
{
    byte[] data = File.ReadAllBytes(input);
    var lines = new List<string>
    {
        $"// Auto-generated from {Path.GetFileName(input)} by {generator} - do not edit.",
        $"// Size: {data.Length} bytes",
        "#pragma once",
        "",
        "#include <stdint.h>",
        "#include <stddef.h>",
        "",
        $"static const size_t {symbol}_LEN = {data.Length};",
        $"static const uint8_t {symbol}[{data.Length}] = {{",
    };

    for (int offset = 0; offset < data.Length; offset += 16)
    {
        string bytes = string.Join(", ", data.Skip(offset).Take(16).Select(b => $"0x{b:X2}"));
        lines.Add($"    {bytes},");
    }

    lines.Add("};");
    lines.Add("");
    return WriteTextIfNeeded(output, string.Join("\n", lines), check);
}

static string RenderArtyModules(string repo)
{
    var sb = new StringBuilder();
    sb.Append("// Auto-generated: embedded 16KB module images from modules/<name>/<name>.bin\n");
    sb.Append("#ifndef MODULES_EMBEDDED_H\n#define MODULES_EMBEDDED_H\n");

    foreach ((string symbol, string name) in ArtyEmbeddedModules())
    {
        string path = Path.Combine(repo, "software", "modules", name, name + ".bin");
        byte[] data = File.ReadAllBytes(path);
        if (data.Length != 16384)
            throw new InvalidOperationException($"{path}: expected 16384 bytes, got {data.Length}");

        sb.Append($"static const unsigned char {symbol}[16384] = {{\n");
        for (int offset = 0; offset < data.Length; offset += 24)
        {
            sb.Append(' ');
            sb.Append(string.Join(",", data.Skip(offset).Take(24)));
            sb.Append(",\n");
        }
        sb.Append("};\n");
    }

    sb.Append("static const unsigned char *const EMBEDDED_MOD[9] = {0, MOD_GRAPHICS, MOD_SOUND, MOD_SYSTEM, MOD_FILES, MOD_MEMORY, MOD_NET, MOD_TURTLE, MOD_EDITOR};\n");
    sb.Append("#endif\n");
    return sb.ToString();
}

static bool WriteTextIfNeeded(string output, string text, bool check)
{
    if (File.Exists(output) && File.ReadAllText(output) == text)
        return false;

    if (check)
    {
        Console.Error.WriteLine($"stale: {output}");
        return true;
    }

    Directory.CreateDirectory(Path.GetDirectoryName(output)!);
    File.WriteAllText(output, text);
    Console.WriteLine($"updated: {output}");
    return true;
}

static int RunSsh(string host, string remoteCommand) =>
    RunCommand("ssh", ["-o", "BatchMode=yes", "-o", "ConnectTimeout=3", "-o", "StrictHostKeyChecking=accept-new", "root@" + host, remoteCommand]);

static int RunScp(string host, string localPath, string remotePath) =>
    RunCommand("scp", ["-o", "BatchMode=yes", "-o", "ConnectTimeout=3", "-o", "StrictHostKeyChecking=accept-new", localPath, "root@" + host + ":" + remotePath]);

static int VerifyRemoteFileSha256(string host, string localPath, string remotePath)
{
    string localSha = Convert.ToHexString(SHA256.HashData(File.ReadAllBytes(localPath))).ToLowerInvariant();
    int rc = RunSshCapture(host, "sha256sum " + remotePath, out string stdout);
    if (rc != 0) return rc;

    string[] parts = stdout.Split((char[]?)null, StringSplitOptions.RemoveEmptyEntries);
    if (parts.Length == 0)
    {
        Console.Error.WriteLine($"could not read remote sha256 for {remotePath}");
        return 1;
    }

    string remoteSha = parts[0].ToLowerInvariant();
    if (!string.Equals(localSha, remoteSha, StringComparison.Ordinal))
    {
        Console.Error.WriteLine($"ERROR: stale deployed artifact: {remotePath}");
        Console.Error.WriteLine($"local  {localSha}  {localPath}");
        Console.Error.WriteLine($"remote {remoteSha}  {remotePath}");
        return 1;
    }

    Console.WriteLine($"verified: {remotePath} sha256 {localSha}");
    return 0;
}

static int RunSshCapture(string host, string remoteCommand, out string stdout) =>
    RunCommandCapture("ssh", ["-o", "BatchMode=yes", "-o", "ConnectTimeout=3", "-o", "StrictHostKeyChecking=accept-new", "root@" + host, remoteCommand], out stdout);

static int RunCommandCapture(string fileName, IEnumerable<string> args, out string stdout)
{
    var psi = new ProcessStartInfo(fileName)
    {
        UseShellExecute = false,
        RedirectStandardOutput = true,
        RedirectStandardError = true,
        WorkingDirectory = Directory.GetCurrentDirectory(),
    };
    foreach (string arg in args)
        psi.ArgumentList.Add(arg);

    using Process process = Process.Start(psi)
        ?? throw new InvalidOperationException($"could not start {fileName}");
    stdout = process.StandardOutput.ReadToEnd();
    string stderr = process.StandardError.ReadToEnd();
    process.WaitForExit();
    if (!string.IsNullOrEmpty(stderr))
        Console.Error.Write(stderr);
    return process.ExitCode;
}

static int RunCommand(
    string fileName,
    IEnumerable<string> args,
    string? workingDirectory = null,
    IReadOnlyDictionary<string, string?>? environment = null)
{
    var psi = new ProcessStartInfo(fileName)
    {
        UseShellExecute = false,
        WorkingDirectory = workingDirectory ?? Directory.GetCurrentDirectory(),
    };
    foreach (string arg in args)
        psi.ArgumentList.Add(arg);
    if (environment is not null)
    {
        foreach ((string key, string? value) in environment)
        {
            if (value is null)
                psi.Environment.Remove(key);
            else
                psi.Environment[key] = value;
        }
    }

    using Process process = Process.Start(psi)
        ?? throw new InvalidOperationException($"could not start {fileName}");
    process.WaitForExit();
    return process.ExitCode;
}

static int DoCi(string[] args)
{
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintCiUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    try
    {
        return command switch
        {
            "install-linux-deps" => DoCiInstallLinuxDeps(rest),
            "install-macos-cc65" => DoCiInstallMacosCc65(rest),
            "mint-github-token" => DoCiMintGithubToken(rest),
            _ => UnknownCiCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"ci {command}: {ex.Message}");
        return 1;
    }
}

static int DoCiInstallLinuxDeps(List<string> args)
{
    string mode = "build";
    if (args.Count > 0 && !args[0].StartsWith("-", StringComparison.Ordinal))
    {
        mode = args[0].ToLowerInvariant();
        args.RemoveAt(0);
    }
    if (args.Count > 0 || mode is not ("build" or "release"))
    {
        PrintCiUsage();
        return 1;
    }

    int rc = RunCommand("apt-get", ["update", "-qq"]);
    if (rc != 0) return rc;

    string[] packages = mode == "build"
        ? [
            "ca-certificates", "clang", "curl", "fonts-lmodern", "git", "gzip",
            "latexmk", "lmodern", "make", "pandoc", "python3-yaml", "tar",
            "texlive-fonts-recommended", "texlive-latex-extra",
            "texlive-latex-recommended", "zlib1g-dev"
        ]
        : ["ca-certificates", "curl", "git", "gzip", "tar"];

    rc = RunCommand("apt-get", ["install", "-y", "-qq", "--no-install-recommends", .. packages]);
    if (rc != 0) return rc;

    if (mode == "build" && !Ca65SupportsW65c02())
    {
        rc = BuildPinnedCc65("/usr/local", "clang");
        if (rc != 0) return rc;
    }

    if (!CommandExists("cog"))
    {
        rc = InstallTarballMember(
            "https://github.com/cocogitto/cocogitto/releases/download/6.5.0/cocogitto-6.5.0-x86_64-unknown-linux-musl.tar.gz",
            "/usr/local/bin",
            "x86_64-unknown-linux-musl/cog",
            stripComponents: 1);
        if (rc != 0) return rc;
    }

    if (mode == "release" && !CommandExists("gh"))
    {
        rc = InstallTarballMember(
            "https://github.com/cli/cli/releases/download/v2.74.0/gh_2.74.0_linux_amd64.tar.gz",
            "/usr/local/bin",
            "gh_2.74.0_linux_amd64/bin/gh",
            stripComponents: 2);
        if (rc != 0) return rc;
    }

    _ = RunCommand("apt-get", ["clean"]);
    CleanDirectoryContents("/var/lib/apt/lists");
    return 0;
}

static int DoCiInstallMacosCc65(List<string> args)
{
    string cc65Ref = TakeOptionValue(args, "--cc65-ref") ?? DefaultCc65Ref();
    string? prefix = TakeOptionValue(args, "--prefix") ?? Environment.GetEnvironmentVariable("CC65_PREFIX");
    if (args.Count > 0)
    {
        PrintCiUsage();
        return 1;
    }

    prefix ??= Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".local");
    string bin = Path.Combine(prefix, "bin");
    string path = Environment.GetEnvironmentVariable("PATH") ?? "";
    Environment.SetEnvironmentVariable("PATH", bin + Path.PathSeparator + path);

    if (Ca65SupportsW65c02())
    {
        Console.WriteLine("cc65 already provides a w65c02-capable ca65; skipping build.");
        return RunCommand("ca65", ["--cpu", "w65c02", "--version"]);
    }

    Console.WriteLine($"Building pinned cc65 ({cc65Ref}) from source into {prefix} ...");
    int rc = BuildPinnedCc65(prefix, cc: null, cc65Ref);
    if (rc != 0) return rc;
    rc = RunCommand("ca65", ["--cpu", "w65c02", "--version"]);
    if (rc == 0)
        Console.WriteLine($"cc65 installed to {bin}");
    return rc;
}

static int DoCiMintGithubToken(List<string> args)
{
    string api = TakeOptionValue(args, "--api") ?? Environment.GetEnvironmentVariable("GITHUB_API_URL") ?? "https://api.github.com";
    string? reposText = TakeOptionValue(args, "--repos") ?? Environment.GetEnvironmentVariable("MINT_REPOS");
    string permsText = TakeOptionValue(args, "--permissions", "--perms") ?? Environment.GetEnvironmentVariable("MINT_PERMS") ?? "contents=write,metadata=read";
    if (args.Count > 0)
    {
        PrintCiUsage();
        return 1;
    }

    string appId = RequiredEnvironment("GH_APP_ID");
    string installationId = RequiredEnvironment("GH_APP_INSTALLATION_ID");
    string privateKey = RequiredEnvironment("GH_APP_PRIVATE_KEY");
    reposText ??= CurrentCiRepoName();

    string jwt = CreateGithubAppJwt(appId, privateKey);
    JsonObject body = CreateGithubInstallationTokenBody(reposText, permsText);

    using var request = new HttpRequestMessage(HttpMethod.Post, api.TrimEnd('/') + $"/app/installations/{installationId}/access_tokens");
    request.Headers.TryAddWithoutValidation("Authorization", "Bearer " + jwt);
    request.Headers.TryAddWithoutValidation("Accept", "application/vnd.github+json");
    request.Headers.TryAddWithoutValidation("X-GitHub-Api-Version", "2022-11-28");
    request.Headers.TryAddWithoutValidation("User-Agent", "nova-cli");
    request.Content = new StringContent(body.ToJsonString(), Encoding.UTF8, "application/json");

    using var http = new HttpClient();
    using HttpResponseMessage response = http.Send(request);
    string text = response.Content.ReadAsStringAsync().GetAwaiter().GetResult();
    if (!response.IsSuccessStatusCode)
    {
        Console.Error.WriteLine($"token request failed: {(int)response.StatusCode} {response.ReasonPhrase}");
        Console.Error.WriteLine(text);
        return 1;
    }

    string? token = JsonNode.Parse(text)?["token"]?.GetValue<string>();
    if (string.IsNullOrEmpty(token))
    {
        Console.Error.WriteLine("token response did not contain a token");
        Console.Error.WriteLine(text);
        return 1;
    }

    Console.WriteLine(token);
    return 0;
}

static int UnknownCiCommand(string command)
{
    Console.Error.WriteLine($"Unknown ci command: {command}");
    PrintCiUsage();
    return 1;
}

static void PrintCiUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova ci install-linux-deps [build|release]");
    Console.Error.WriteLine("  nova ci install-macos-cc65 [--prefix <dir>] [--cc65-ref <git-ref>]");
    Console.Error.WriteLine("  nova ci mint-github-token [--repos <repo,...>] [--permissions <key=level,...>] [--api <url>]");
}

static string DefaultCc65Ref() => "cc3c40c54e51b2d9a22b63c85c418a2b11763377";

static bool Ca65SupportsW65c02() => CommandSucceeds("ca65", ["--cpu", "w65c02", "--version"]);

static bool CommandSucceeds(string fileName, IEnumerable<string> args)
{
    try
    {
        return RunCommand(fileName, args) == 0;
    }
    catch
    {
        return false;
    }
}

static int BuildPinnedCc65(string prefix, string? cc, string? cc65Ref = null)
{
    cc65Ref ??= DefaultCc65Ref();
    using TempDir temp = TempDir.Create("cc65-");
    string source = Path.Combine(temp.Path, "cc65");
    int rc = RunCommand("git", ["clone", "--filter=blob:none", "-q", "https://github.com/cc65/cc65.git", source]);
    if (rc != 0) return rc;
    rc = RunCommand("git", ["-C", source, "checkout", "-q", cc65Ref]);
    if (rc != 0) return rc;

    var makeArgs = new List<string> { "-C", source, "-j" + Math.Max(1, Environment.ProcessorCount).ToString(CultureInfo.InvariantCulture) };
    if (!string.IsNullOrEmpty(cc))
        makeArgs.Add("CC=" + cc);
    rc = RunCommand("make", makeArgs);
    if (rc != 0) return rc;

    return RunCommand("make", ["-C", source, "install", "PREFIX=" + prefix]);
}

static int InstallTarballMember(string url, string destination, string member, int stripComponents)
{
    using TempDir temp = TempDir.Create("nova-ci-tar-");
    string archive = Path.Combine(temp.Path, "package.tar.gz");
    int rc = RunCommand("curl", ["-fsSL", "-o", archive, url]);
    if (rc != 0) return rc;

    Directory.CreateDirectory(destination);
    return RunCommand("tar", ["-xzf", archive, "--strip-components=" + stripComponents.ToString(CultureInfo.InvariantCulture), "-C", destination, member]);
}

static void CleanDirectoryContents(string directory)
{
    if (!Directory.Exists(directory))
        return;

    foreach (string file in Directory.GetFiles(directory))
        File.Delete(file);
    foreach (string child in Directory.GetDirectories(directory))
        Directory.Delete(child, recursive: true);
}

static string RequiredEnvironment(string name)
{
    string? value = Environment.GetEnvironmentVariable(name);
    if (string.IsNullOrWhiteSpace(value))
        throw new InvalidOperationException($"{name} is not set");
    return value;
}

static string CurrentCiRepoName()
{
    string ciRepo = RequiredEnvironment("CI_REPO");
    string[] parts = ciRepo.Split('/', StringSplitOptions.RemoveEmptyEntries);
    return parts.Length == 0 ? ciRepo : parts[^1];
}

static string CreateGithubAppJwt(string appId, string privateKey)
{
    string pem = privateKey.Contains('\n')
        ? privateKey
        : privateKey.Replace("\\n", "\n", StringComparison.Ordinal);

    using RSA rsa = RSA.Create();
    rsa.ImportFromPem(pem);

    long now = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
    string header = """{"alg":"RS256","typ":"JWT"}""";
    string payload = $"{{\"iat\":{now - 60},\"exp\":{now + 540},\"iss\":\"{appId}\"}}";
    string unsigned = Base64Url(Encoding.UTF8.GetBytes(header)) + "." + Base64Url(Encoding.UTF8.GetBytes(payload));
    byte[] signature = rsa.SignData(Encoding.UTF8.GetBytes(unsigned), HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
    return unsigned + "." + Base64Url(signature);
}

static string Base64Url(byte[] bytes) =>
    Convert.ToBase64String(bytes).TrimEnd('=').Replace('+', '-').Replace('/', '_');

static JsonObject CreateGithubInstallationTokenBody(string reposText, string permissionsText)
{
    var repositories = new JsonArray();
    foreach (string repo in reposText.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
        repositories.Add(repo);

    var permissions = new JsonObject();
    foreach (string item in permissionsText.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries))
    {
        int equals = item.IndexOf('=', StringComparison.Ordinal);
        if (equals <= 0 || equals == item.Length - 1)
            throw new InvalidOperationException($"bad permission '{item}' (want key=level)");
        permissions[item[..equals]] = item[(equals + 1)..];
    }

    return new JsonObject
    {
        ["repositories"] = repositories,
        ["permissions"] = permissions,
    };
}

static int DoPublish(string[] args)
{
    var rest = args.ToList();
    bool help = rest.Count > 0 && rest[0].Equals("help", StringComparison.OrdinalIgnoreCase);
    help |= TakeFlag(rest, "--help", "-h");
    string repo = ResolveRepoRoot(rest);
    string? rid = TakeOptionValue(rest, "--rid", "--runtime");

    if (help)
    {
        PrintPublishUsage();
        return 0;
    }

    if (rid is null && rest.Count > 0 && !rest[0].StartsWith("-", StringComparison.Ordinal))
    {
        rid = rest[0];
        rest.RemoveAt(0);
    }

    if (rest.Count > 0)
    {
        Console.Error.WriteLine($"Unexpected publish argument: {rest[0]}");
        PrintPublishUsage();
        return 1;
    }

    rid ??= DetectRuntimeIdentifier();
    if (rid is null)
    {
        Console.Error.WriteLine("Could not detect a runtime identifier. Pass one, e.g. linux-x64, linux-arm64, osx-arm64, win-x64.");
        return 1;
    }

    string output = Path.Combine(repo, "artifacts", "nova-cli", rid);
    string csproj = Path.Combine(repo, "e6502.Nova", "e6502.Nova.csproj");
    int rc = RunCommand("dotnet",
    [
        "publish", csproj,
        "-c", "Release",
        "-r", rid,
        "--self-contained", "true",
        "-p:PublishAot=true",
        "--tl:off",
        "-o", output
    ]);
    if (rc != 0)
        return rc;

    string exe = Path.Combine(output, rid.StartsWith("win-", StringComparison.OrdinalIgnoreCase) ? "nova.exe" : "nova");
    if (!File.Exists(exe))
    {
        Console.Error.WriteLine($"NativeAOT publish completed without producing {exe}");
        return 1;
    }

    Console.WriteLine($"Published Nova CLI: {exe}");
    return 0;
}

static string? DetectRuntimeIdentifier()
{
    string os = OperatingSystem.IsWindows() ? "win"
              : OperatingSystem.IsMacOS() ? "osx"
              : OperatingSystem.IsLinux() ? "linux"
              : "";
    string arch = RuntimeInformation.OSArchitecture switch
    {
        Architecture.X64 => "x64",
        Architecture.Arm64 => "arm64",
        _ => "",
    };
    return os.Length > 0 && arch.Length > 0 ? $"{os}-{arch}" : null;
}

static void PrintPublishUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova publish [rid] [--repo <repo>]");
    Console.Error.WriteLine("  nova publish --rid <rid> [--repo <repo>]");
    Console.Error.WriteLine("Examples:");
    Console.Error.WriteLine("  nova publish");
    Console.Error.WriteLine("  nova publish linux-x64");
    Console.Error.WriteLine("  nova publish linux-arm64");
    Console.Error.WriteLine("  nova publish osx-arm64");
    Console.Error.WriteLine("  nova publish win-x64");
}

static int DoConvert(string[] args)
{
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintConvertUsage();
        return args.Length < 1 ? 1 : 0;
    }

    try
    {
        return args[0].ToLowerInvariant() switch
        {
            "hex16-to-bin" or "hex16" => DoConvertHex16ToBin(args[1..]),
            "bin-header" or "bin2header" => DoConvertBinHeader(args[1..]),
            _ => UnknownConvertCommand(args[0]),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"convert {args[0]}: {ex.Message}");
        return 1;
    }
}

static int DoConvertHex16ToBin(string[] args)
{
    if (args.Length != 2)
    {
        PrintConvertUsage();
        return 1;
    }

    string input = args[0];
    string output = args[1];
    var bytes = new List<byte>();

    int lineNo = 0;
    foreach (string raw in File.ReadLines(input))
    {
        lineNo++;
        string token = raw.Trim();
        if (token.Length == 0 || token.StartsWith("//", StringComparison.Ordinal))
            continue;

        string hex = token.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? token[2..] : token;
        if (!ushort.TryParse(hex, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ushort value))
        {
            Console.Error.WriteLine($"{input}:{lineNo}: not a 16-bit hex value: {token}");
            return 1;
        }

        bytes.Add((byte)(value & 0xFF));
        bytes.Add((byte)(value >> 8));
    }

    EnsureParentDirectory(output);
    File.WriteAllBytes(output, bytes.ToArray());
    Console.WriteLine($"{input} -> {output} ({bytes.Count} bytes)");
    return 0;
}

static int DoConvertBinHeader(string[] args)
{
    if (args.Length != 3)
    {
        PrintConvertUsage();
        return 1;
    }

    WriteBinHeader(args[0], args[1], args[2], check: false, generator: "nova convert bin-header");
    return 0;
}

static int UnknownConvertCommand(string command)
{
    Console.Error.WriteLine($"Unknown convert command: {command}");
    PrintConvertUsage();
    return 1;
}

static void PrintConvertUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova convert hex16-to-bin <input.hex> <output.bin>");
    Console.Error.WriteLine("  nova convert bin-header <input.bin> <output.h> <symbol>");
}

static int DoCodegen(string[] args)
{
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintCodegenUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    try
    {
        return command switch
        {
            "tokens" => DoCodegenTokens(rest),
            "novavm-inc" => DoCodegenNovaVmInc(rest),
            "runtime-abi" => DoCodegenRuntimeAbi(rest),
            "ndk-reference" => DoCodegenNdkReference(rest),
            _ => UnknownCodegenCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"codegen {command}: {ex.Message}");
        return 1;
    }
}

static int DoCodegenTokens(List<string> args)
{
    string output = TakeOptionValue(args, "-o", "--output") ?? "tokens.json";
    if (args.Count != 1)
    {
        PrintCodegenUsage();
        return 1;
    }

    NovaBuildTools.GenerateTokens(args[0], output);
    return 0;
}

static int DoCodegenNovaVmInc(List<string> args)
{
    string output = TakeOptionValue(args, "-o", "--output") ?? "novavm.inc";
    if (args.Count is < 1 or > 2)
    {
        PrintCodegenUsage();
        return 1;
    }

    string? symPath = args.Count == 2 ? args[1] : null;
    NovaBuildTools.GenerateNovaVmInc(args[0], symPath, output);
    return 0;
}

static int DoCodegenRuntimeAbi(List<string> args)
{
    string? sym = TakeOptionValue(args, "--sym");
    string? json = TakeOptionValue(args, "--json");
    string? md = TakeOptionValue(args, "--md");
    string? asm = TakeOptionValue(args, "--asm");
    if (args.Count < 1 || json is null || md is null)
    {
        PrintCodegenUsage();
        return 1;
    }

    NovaBuildTools.GenerateRuntimeAbiDocs(args, sym, json, md, asm);
    return 0;
}

static int DoCodegenNdkReference(List<string> args)
{
    string? runtimeDir = TakeOptionValue(args, "--runtime-dir");
    string? tex = TakeOptionValue(args, "--tex");
    string? json = TakeOptionValue(args, "--json");
    if (runtimeDir is null || tex is null || json is null || args.Count > 0)
    {
        PrintCodegenUsage();
        return 1;
    }

    NovaBuildTools.GenerateNdkReference(runtimeDir, tex, json);
    return 0;
}

static int UnknownCodegenCommand(string command)
{
    Console.Error.WriteLine($"Unknown codegen command: {command}");
    PrintCodegenUsage();
    return 1;
}

static void PrintCodegenUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova codegen tokens <basic.asm> [-o tokens.json]");
    Console.Error.WriteLine("  nova codegen novavm-inc <VgcConstants.cs> [basic.sym] [-o novavm.inc]");
    Console.Error.WriteLine("  nova codegen runtime-abi <sources...> --sym <basic.sym> --json <out.json> --md <out.md> [--asm <out.inc>]");
    Console.Error.WriteLine("  nova codegen ndk-reference --runtime-dir <runtime/asm> --tex <out.tex> --json <out.json>");
}

static int DoDocs(string[] args)
{
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintDocsUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    string repo = ResolveRepoRoot(rest);
    try
    {
        return command switch
        {
            "basic-user-guide" => DoDocsBasicUserGuide(repo, rest),
            "nova-cli-guide" => DoDocsNovaCliGuide(repo, rest),
            "fun-n-games" => DoDocsFunNGames(repo, rest),
            "showcase-demo" => DoDocsShowcaseDemo(repo, rest),
            _ => UnknownDocsCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"docs {command}: {ex.Message}");
        return 1;
    }
}

static int DoDocsBasicUserGuide(string repo, List<string> args)
{
    if (args.Count > 0)
    {
        PrintDocsUsage();
        return 1;
    }

    if (!CommandExists("pandoc") || !CommandExists("latexmk"))
    {
        Console.WriteLine("docs basic-user-guide: pandoc/latexmk not found -- skipping PDF user guide.");
        return 0;
    }

    string bookDir = Path.Combine(repo, "docs", "books", "basic-user-guide");
    string assembled = Path.Combine(bookDir, "assembled.md");
    NovaBuildTools.AssembleHelpBook(
        Path.Combine(bookDir, "book.yaml"),
        Path.Combine(repo, "docs", "help"),
        assembled);

    var pandocArgs = new List<string>
    {
        assembled,
        "--lua-filter=" + Path.Combine(bookDir, "nova-filter.lua"),
        "--pdf-engine=latexmk",
        "--pdf-engine-opt=-pdf",
        "-V", "geometry:margin=1in",
        "-V", "fontsize=11pt",
        "-o", Path.Combine(bookDir, "novabasic_user_guide.pdf"),
    };
    string template = Path.Combine(bookDir, "latex-template.tex");
    if (File.Exists(template))
        pandocArgs.Add("--template=" + template);
    string header = Path.Combine(bookDir, "latex-header.tex");
    if (File.Exists(header))
    {
        pandocArgs.Add("-H");
        pandocArgs.Add(header);
    }

    try
    {
        int rc = RunCommand("pandoc", pandocArgs);
        if (rc == 0)
            Console.WriteLine($"PDF generated: {Path.Combine(bookDir, "novabasic_user_guide.pdf")}");
        return rc;
    }
    finally
    {
        if (File.Exists(assembled))
            File.Delete(assembled);
    }
}

static int DoDocsNovaCliGuide(string repo, List<string> args)
{
    if (args.Count > 0)
    {
        PrintDocsUsage();
        return 1;
    }
    foreach (string tool in new[] { "pandoc", "latexmk" })
    {
        if (!CommandExists(tool))
        {
            Console.Error.WriteLine($"ERROR: {tool} is required but not installed.");
            return 1;
        }
    }

    string bookDir = Path.Combine(repo, "docs", "books", "nova-cli-guide");
    int rc = RunCommand("pandoc",
    [
        Path.Combine(bookDir, "metadata.yaml"),
        Path.Combine(bookDir, "chapters", "nova-cli.md"),
        Path.Combine(bookDir, "chapters", "ndi-format.md"),
        "--from=gfm+smart",
        "--pdf-engine=latexmk",
        "--pdf-engine-opt=-pdf",
        "--toc",
        "--number-sections",
        "-V", "documentclass=book",
        "-V", "classoption=oneside",
        "-V", "geometry:margin=0.85in",
        "-V", "fontsize=10pt",
        "-H", Path.Combine(bookDir, "latex-header.tex"),
        "-o", Path.Combine(bookDir, "nova-cli-guide.pdf"),
    ]);
    if (rc == 0)
        Console.WriteLine($"PDF generated: {Path.Combine(bookDir, "nova-cli-guide.pdf")}");
    return rc;
}

static int DoDocsFunNGames(string repo, List<string> args)
{
    if (args.Count > 0)
    {
        PrintDocsUsage();
        return 1;
    }

    string sourceDir = Path.Combine(repo, "docs", "programs", "fun_n_games");
    string buildDir = Path.Combine(Path.GetTempPath(), "fun_n_games_build_" + Environment.ProcessId);
    string image = Path.Combine(repo, "docs", "programs", "fun_n_games.ndi");
    Directory.CreateDirectory(buildDir);

    try
    {
        string common = File.ReadAllText(Path.Combine(sourceDir, "common.bas"));
        foreach (string stub in Directory.GetFiles(Path.Combine(sourceDir, "stubs"), "*.bas").Order(StringComparer.Ordinal))
        {
            string text = File.ReadAllText(stub).TrimEnd() + "\n\n" + common;
            File.WriteAllText(Path.Combine(buildDir, Path.GetFileName(stub)), text);
        }

        int rc = DoCreate([image, "--size", "800", "--label", "FUNNGAMES"]);
        if (rc != 0) return rc;
        foreach (string source in Directory.GetFiles(buildDir, "*.bas").Order(StringComparer.Ordinal))
        {
            rc = DoImport([image, source, "/", "--tokenize"]);
            if (rc != 0) return rc;
        }
        return DoValidate([image]);
    }
    finally
    {
        if (Directory.Exists(buildDir))
            Directory.Delete(buildDir, recursive: true);
    }
}

static int DoDocsShowcaseDemo(string repo, List<string> args)
{
    if (args.Count > 0)
    {
        PrintDocsUsage();
        return 1;
    }

    string image = Path.Combine(repo, "docs", "programs", "demo.ndi");
    string sourceDir = Path.Combine(repo, "docs", "programs");
    using TempDir temp = TempDir.Create("nova-showcase-");

    int rc = RunCommand("make", ["-C", Path.Combine(repo, "software", "assembly"), "keyboard", "demo"]);
    if (rc != 0) return rc;

    rc = DoCreate([image, "--size", "4096", "--label", "SHOWCASE"]);
    if (rc != 0) return rc;

    foreach (string dir in new[] { "featured", "2sid", "sid", "wts", "arcade" })
    {
        rc = DoMkdir([image, "/" + dir]);
        if (rc != 0) return rc;
    }

    string autoboot = Path.Combine(temp.Path, "AUTOBOOT.bin");
    string keyboard = Path.Combine(temp.Path, "KEYBOARD.bin");
    File.Copy(Path.Combine(repo, "software", "assembly", "apps", "demo", "demo.bin"), autoboot, overwrite: true);
    File.Copy(Path.Combine(repo, "software", "assembly", "apps", "keyboard", "keyboard.bin"), keyboard, overwrite: true);
    rc = DoImport([image, autoboot, "/"]);
    if (rc != 0) return rc;
    rc = DoImport([image, keyboard, "/"]);
    if (rc != 0) return rc;

    foreach ((string Source, string Dest, string Dir) entry in ShowcaseSidFiles())
    {
        string staged = Path.Combine(temp.Path, entry.Dest);
        File.Copy(Path.Combine(sourceDir, "sid", entry.Source), staged, overwrite: true);
        rc = DoImport([image, staged, entry.Dir]);
        if (rc != 0) return rc;
    }

    foreach ((string Name, string Dir) entry in ShowcaseMidiFiles())
    {
        rc = DoImport([image, Path.Combine(sourceDir, "midi", entry.Name), entry.Dir, "--raw-midi"]);
        if (rc != 0) return rc;
    }

    rc = DoValidate([image]);
    if (rc != 0) return rc;
    rc = DoDir([image]);
    if (rc != 0) return rc;
    return DoDir([image, "/featured"]);
}

static (string Source, string Dest, string Dir)[] ShowcaseSidFiles() =>
[
    ("Love_Fileosophy_2SID.sid", "Love_Fileosophy_2SID.sid", "/featured"),
    ("Fratres_2SID.sid", "Fratres_2SID.sid", "/featured"),
    ("commando.sid", "commando.sid", "/featured"),
    ("monty-on-the-run.sid", "monty-on-the-run.sid", "/featured"),
    ("tubular_bells_ii_shake_airwolf_style.sid", "tubular-airwolf.sid", "/featured"),

    ("Love_Fileosophy_2SID.sid", "Love_Fileosophy_2SID.sid", "/2sid"),
    ("Fratres_2SID.sid", "Fratres_2SID.sid", "/2sid"),
    ("Popel_Premiere_2SID.sid", "Popel_Premiere_2SID.sid", "/2sid"),

    ("commando.sid", "commando.sid", "/sid"),
    ("monty-on-the-run.sid", "monty-on-the-run.sid", "/sid"),
    ("parallax.sid", "parallax.sid", "/sid"),
    ("wizball.sid", "wizball.sid", "/sid"),
    ("crazy-comets.sid", "crazy-comets.sid", "/sid"),
    ("delta.sid", "delta.sid", "/sid"),
    ("cybernoid.sid", "cybernoid.sid", "/sid"),
    ("sanxion.sid", "sanxion.sid", "/sid"),
    ("lightforce.sid", "lightforce.sid", "/sid"),
    ("master-of-magic.sid", "master-of-magic.sid", "/sid"),
];

static (string Name, string Dir)[] ShowcaseMidiFiles() =>
[
    ("sousa-stars-stripes.mid", "/featured"),
    ("bach-toccata-dm.mid", "/featured"),
    ("joplin-entertainer.mid", "/featured"),
    ("miami-vice.mid", "/featured"),
    ("tetris-theme.mid", "/featured"),
    ("castlevania-bloody-tears.mid", "/featured"),
    ("star-wars.mid", "/featured"),

    ("sousa-stars-stripes.mid", "/wts"),
    ("bach-toccata-dm.mid", "/wts"),
    ("joplin-entertainer.mid", "/wts"),
    ("debussy-clair-de-lune.mid", "/wts"),
    ("miami-vice.mid", "/wts"),
    ("tetris-theme.mid", "/wts"),
    ("castlevania-bloody-tears.mid", "/wts"),
    ("star-wars.mid", "/wts"),
    ("grieg-mountain-king.mid", "/wts"),
    ("pink-panther.mid", "/wts"),

    ("sonic-green-hill.mid", "/arcade"),
    ("super-mario-bros.mid", "/arcade"),
    ("zelda-overworld.mid", "/arcade"),
    ("megaman2-wily.mid", "/arcade"),
    ("street-fighter-2.mid", "/arcade"),
    ("contra-theme.mid", "/arcade"),
    ("mission-impossible.mid", "/arcade"),
    ("x-files.mid", "/arcade"),
    ("ghostbusters.mid", "/arcade"),
    ("hawaii-five-o.mid", "/arcade"),
];

static int DoBuild(string[] args)
{
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintBuildUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    string repo = ResolveRepoRoot(rest);
    try
    {
        return command switch
        {
            "browser-rust-core" => DoBuildBrowserRustCore(repo, rest),
            _ => UnknownBuildCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"build {command}: {ex.Message}");
        return 1;
    }
}

static int DoBuildBrowserRustCore(string repo, List<string> args)
{
    if (args.Count > 0)
    {
        PrintBuildUsage();
        return 1;
    }

    string outDir = Path.Combine(repo, "e6502.Browser", "wwwroot", "rust");
    int rc = RunCommand("rustup",
    [
        "run", "stable", "cargo", "build",
        "--manifest-path", Path.Combine(repo, "e6502.Browser.RustCore", "Cargo.toml"),
        "--target", "wasm32-unknown-unknown",
        "--release"
    ]);
    if (rc != 0) return rc;

    Directory.CreateDirectory(outDir);
    File.Copy(
        Path.Combine(repo, "e6502.Browser.RustCore", "target", "wasm32-unknown-unknown", "release", "nova_browser_core.wasm"),
        Path.Combine(outDir, "nova_browser_core.wasm"),
        overwrite: true);

    foreach (string resource in BrowserRustResourceFiles())
    {
        File.Copy(
            Path.Combine(repo, "e6502.Avalonia", "Resources", resource),
            Path.Combine(outDir, resource),
            overwrite: true);
    }

    string forthDisk = Path.Combine(repo, "e6502.Browser", "wwwroot", "forth.ndi");
    using TempFile tempForth = TempFile.Create(".ndi");
    rc = DoCreate([tempForth.Path, "--size", "800", "--label", "FORTH"]);
    if (rc != 0) return rc;

    string forthRoot = Path.Combine(repo, "software", "languages", "novaforth");
    foreach (string dir in Directory.GetDirectories(Path.Combine(forthRoot, "forth"), "*", SearchOption.AllDirectories)
                                    .Order(StringComparer.Ordinal))
    {
        string rel = Path.GetRelativePath(forthRoot, dir).Replace(Path.DirectorySeparatorChar, '/');
        rc = DoMkdir([tempForth.Path, "/" + rel]);
        if (rc != 0) return rc;
    }

    foreach (string file in Directory.GetFiles(Path.Combine(forthRoot, "forth"), "*", SearchOption.AllDirectories)
                                     .Where(IsForthSourcePath)
                                     .Order(StringComparer.Ordinal))
    {
        string rel = Path.GetRelativePath(forthRoot, file).Replace(Path.DirectorySeparatorChar, '/');
        string destDir = "/" + Path.GetDirectoryName(rel)!.Replace(Path.DirectorySeparatorChar, '/');
        rc = DoImport([tempForth.Path, file, destDir]);
        if (rc != 0) return rc;
    }

    rc = DoValidate([tempForth.Path]);
    if (rc != 0) return rc;
    File.Copy(tempForth.Path, forthDisk, overwrite: true);
    Console.WriteLine($"Rust browser core assets written to {outDir}");
    return 0;
}

static string[] BrowserRustResourceFiles() =>
[
    "ehbasic.bin", "novalogo.bin", "novaforth.bin", "extension.bin", "cp437.bin",
    "libcall.bin", "graphics.bin", "system.bin", "sound.bin", "editor.bin",
    "files.bin", "memory.bin", "net.bin", "turtle.bin",
];

static bool IsForthSourcePath(string path)
{
    string ext = Path.GetExtension(path).ToLowerInvariant();
    return ext is ".4th" or ".fth" or ".fr" or ".fs";
}

static int UnknownBuildCommand(string command)
{
    Console.Error.WriteLine($"Unknown build command: {command}");
    PrintBuildUsage();
    return 1;
}

static void PrintBuildUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova build browser-rust-core [--repo <repo>]");
}

static int DoCapture(string[] args, string? host)
{
    _ = host;
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintCaptureUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    try
    {
        return command switch
        {
            "hdmi" => DoCaptureHdmi(rest),
            _ => UnknownCaptureCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"capture {command}: {ex.Message}");
        return 1;
    }
}

static int DoCaptureHdmi(List<string> args)
{
    bool list = TakeFlag(args, "--list");
    string device = TakeOptionValue(args, "--device") ?? Environment.GetEnvironmentVariable("HDMI_DEVICE") ?? "0:none";
    string size = TakeOptionValue(args, "--size") ?? Environment.GetEnvironmentVariable("HDMI_SIZE") ?? "720x480";
    string framerate = TakeOptionValue(args, "--framerate") ?? Environment.GetEnvironmentVariable("HDMI_FRAMERATE") ?? "30";

    if (list)
        return RunCommand("ffmpeg", ["-hide_banner", "-f", "avfoundation", "-list_devices", "true", "-i", ""]);

    string output = args.Count > 0 && !args[0].StartsWith("-", StringComparison.Ordinal)
        ? args[0]
        : Path.Combine("screenshots", "hardware", "novavm-hdmi-" + DateTime.Now.ToString("yyyyMMdd_HHmmss", CultureInfo.InvariantCulture) + ".png");
    if (args.Count > 0 && !args[0].StartsWith("-", StringComparison.Ordinal))
        args.RemoveAt(0);
    if (args.Count > 0)
    {
        PrintCaptureUsage();
        return 1;
    }

    EnsureParentDirectory(output);
    int rc = RunCommand("ffmpeg",
    [
        "-hide_banner",
        "-loglevel", "warning",
        "-y",
        "-f", "avfoundation",
        "-pixel_format", "uyvy422",
        "-framerate", framerate,
        "-video_size", size,
        "-i", device,
        "-frames:v", "1",
        "-update", "1",
        output
    ]);
    if (rc == 0)
        Console.WriteLine(output);
    return rc;
}

static int UnknownCaptureCommand(string command)
{
    Console.Error.WriteLine($"Unknown capture command: {command}");
    PrintCaptureUsage();
    return 1;
}

static void PrintCaptureUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova capture hdmi [out.png] [--device <avfoundation-device>] [--size 720x480] [--framerate 30]");
    Console.Error.WriteLine("  nova capture hdmi --list");
}

static int DoCheck(string[] args, string? host)
{
    host = ExtractRemoteHost(ref args, host);
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintCheckUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    try
    {
        return command switch
        {
            "vgc-reset-stale" => DoCheckVgcResetStale(rest, host),
            "spi-bridge" => DoCheckSpiBridge(rest, host),
            _ => UnknownCheckCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"check {command}: {ex.Message}");
        return 1;
    }
}

static int DoCheckVgcResetStale(List<string> args, string? host)
{
    string? portOpt = TakeOptionValue(args, "--port", "--debug-port");
    string? settleOpt = TakeOptionValue(args, "--settle");
    if (args.Count > 0)
    {
        PrintCheckUsage();
        return 1;
    }

    host ??= Environment.GetEnvironmentVariable("NOVAHOST") ?? "192.168.1.65";
    int port = 6503;
    if (portOpt is not null && (!int.TryParse(portOpt, out port) || port is < 1 or > 65535))
    {
        Console.Error.WriteLine($"Invalid debug TCP port: {portOpt}");
        return 1;
    }
    double settleSeconds = 0.5;
    if (settleOpt is not null && !double.TryParse(settleOpt, NumberStyles.Float, CultureInfo.InvariantCulture, out settleSeconds))
    {
        Console.Error.WriteLine($"Invalid settle seconds: {settleOpt}");
        return 1;
    }

    JsonNode status = SendVmRequest(host, port, new JsonObject { ["command"] = "boot_status" });
    if (status["fpgaBridgeAvailable"]?.GetValue<bool>() == false)
    {
        Console.Error.WriteLine($"FPGA bridge unavailable: {status.ToJsonString()}");
        return 1;
    }

    _ = SendVmRequest(host, port, new JsonObject { ["command"] = "dbg_pause" });
    Console.WriteLine("Dirtying VGC memory planes...");
    foreach (VgcResetPlane plane in VgcResetPlanes())
    {
        _ = SendVmRequest(host, port, new JsonObject
        {
            ["command"] = "fill_vram",
            ["space"] = plane.Space,
            ["address"] = 0,
            ["value"] = plane.DirtyValue,
            ["length"] = plane.Length,
        });
        Console.WriteLine($"  dirtied {plane.Name}");
    }

    Console.WriteLine("Resetting VM...");
    using (var management = new NovaHostManagementClient(host))
        _ = management.VmResetAsync(CancellationToken.None).GetAwaiter().GetResult();
    Thread.Sleep(TimeSpan.FromSeconds(settleSeconds));

    Console.WriteLine("Scanning for stale data...");
    bool failed = false;
    foreach (VgcResetPlane plane in VgcResetPlanes())
    {
        (int failures, string? first) = ScanVgcResetPlane(host, port, plane);
        if (failures > 0)
        {
            failed = true;
            Console.WriteLine($"FAIL {plane.Name}: {failures} stale bytes; first {first}");
        }
        else
        {
            string expectation = plane.ExpectZero ? "zeroed" : "dirty sentinel removed";
            Console.WriteLine($"PASS {plane.Name}: {expectation}");
        }
    }

    return failed ? 1 : 0;
}

static VgcResetPlane[] VgcResetPlanes() =>
[
    new("text chars", 1, 4000, 0x01, false),
    new("text colors", 2, 4000, 0x06, false),
    new("text attrs", 7, 4000, 0x80, true),
    new("graphics bitmap", 3, 64000, 0x0D, true),
    new("sprite shapes", 4, 2048, 0xA5, true),
];

static (int Failures, string? First) ScanVgcResetPlane(string host, int port, VgcResetPlane plane)
{
    const int Block = 256;
    int failures = 0;
    string? first = null;
    for (int offset = 0; offset < plane.Length; offset += Block)
    {
        int length = Math.Min(Block, plane.Length - offset);
        int[] data = ReadVramValues(host, port, plane.Space, offset, length);
        for (int i = 0; i < data.Length; i++)
        {
            bool bad = plane.ExpectZero ? data[i] != 0 : data[i] == plane.DirtyValue;
            if (!bad)
                continue;
            failures++;
            first ??= $"offset {offset + i}: got {data[i]}, expected {(plane.ExpectZero ? "0" : "not " + plane.DirtyValue.ToString(CultureInfo.InvariantCulture))}";
        }
    }
    return (failures, first);
}

static int[] ReadVramValues(string host, int port, int space, int address, int length)
{
    JsonNode response = SendVmRequest(host, port, new JsonObject
    {
        ["command"] = "read_vram",
        ["space"] = space,
        ["address"] = address,
        ["length"] = length,
    });
    if (length == 1 && response["value"] is JsonNode value)
        return [value.GetValue<int>() & 0xFF];
    JsonArray? data = response["data"] as JsonArray ?? response["values"] as JsonArray;
    if (data is null)
        throw new IOException($"read_vram returned no data at space={space} address={address}: {response.ToJsonString()}");
    return data.Select(v => v?.GetValue<int>() & 0xFF ?? 0).ToArray();
}

static int DoCheckSpiBridge(List<string> args, string? host)
{
    string? portOpt = TakeOptionValue(args, "--port", "--debug-port");
    string baseText = TakeOptionValue(args, "--base") ?? "0x7F0000";
    string blocksText = TakeOptionValue(args, "--blocks") ?? "64";
    string iterationsText = TakeOptionValue(args, "--iterations") ?? "2";
    bool noRestore = TakeFlag(args, "--no-restore");
    if (args.Count > 0)
    {
        PrintCheckUsage();
        return 1;
    }

    host ??= Environment.GetEnvironmentVariable("NOVAHOST") ?? "192.168.1.65";
    int port = 6503;
    if (portOpt is not null && (!int.TryParse(portOpt, out port) || port is < 1 or > 65535))
    {
        Console.Error.WriteLine($"Invalid debug TCP port: {portOpt}");
        return 1;
    }

    int baseAddress = ParseVmNumber(baseText);
    if (!int.TryParse(blocksText, NumberStyles.Integer, CultureInfo.InvariantCulture, out int blocks) || blocks < 1)
    {
        Console.Error.WriteLine("--blocks must be >= 1");
        return 1;
    }
    if (!int.TryParse(iterationsText, NumberStyles.Integer, CultureInfo.InvariantCulture, out int iterations) || iterations < 1)
    {
        Console.Error.WriteLine("--iterations must be >= 1");
        return 1;
    }

    JsonNode status = SendVmRequest(host, port, new JsonObject { ["command"] = "boot_status" });
    if (status["fpgaBridgeAvailable"]?.GetValue<bool>() == false)
    {
        Console.Error.WriteLine($"FPGA bridge unavailable: {status.ToJsonString()}");
        return 1;
    }

    const int BlockSize = 256;
    var originals = new List<int[]>(blocks);
    Console.WriteLine($"SPI bridge SDRAM stress: base=0x{baseAddress:X6} blocks={blocks} iterations={iterations}");
    for (int block = 0; block < blocks; block++)
        originals.Add(ReadSdramBlock(host, port, baseAddress + block * BlockSize, BlockSize));

    var started = DateTime.UtcNow;
    try
    {
        for (int iteration = 0; iteration < iterations; iteration++)
        {
            for (int block = 0; block < blocks; block++)
            {
                int address = baseAddress + block * BlockSize;
                int[] expected = SpiPattern(block, iteration, BlockSize);
                WriteSdramBlock(host, port, address, expected);
                int[] actual = ReadSdramBlock(host, port, address, BlockSize);
                for (int i = 0; i < expected.Length; i++)
                {
                    if (actual[i] != expected[i])
                    {
                        Console.Error.WriteLine($"SDRAM mismatch at 0x{address + i:X6}: got 0x{actual[i]:X2}, expected 0x{expected[i]:X2}");
                        return 1;
                    }
                }
            }
            Console.WriteLine($"  iteration {iteration + 1}/{iterations}: ok");
        }
    }
    finally
    {
        if (!noRestore)
            for (int block = 0; block < originals.Count; block++)
                WriteSdramBlock(host, port, baseAddress + block * BlockSize, originals[block]);
    }

    double elapsed = (DateTime.UtcNow - started).TotalSeconds;
    Console.WriteLine($"PASS: {blocks * BlockSize * iterations} patterned bytes verified in {elapsed:F2}s");
    return 0;
}

static int[] SpiPattern(int block, int iteration, int length)
{
    int[] result = new int[length];
    int mode = (block + iteration) % 5;
    for (int i = 0; i < result.Length; i++)
    {
        result[i] = mode switch
        {
            0 => (i + block + iteration) & 0xFF,
            1 => (i & 1) == 0 ? 0x00 : 0xFF,
            2 => (i & 1) == 0 ? 0xAA : 0x55,
            3 => ((i * 17) + (block * 31) + iteration) & 0xFF,
            _ => 0xFF - ((i + block) & 0xFF),
        };
    }
    return result;
}

static int[] ReadSdramBlock(string host, int port, int address, int count)
{
    JsonNode response = SendVmRequest(host, port, new JsonObject
    {
        ["command"] = "read_sdram",
        ["address"] = address,
        ["count"] = count,
    });
    JsonArray? values = response["values"] as JsonArray;
    if (values is null || values.Count != count)
        throw new IOException($"read_sdram returned malformed values at 0x{address:X6}: {response.ToJsonString()}");
    return values.Select(v => v is null ? 0 : v.GetValue<int>() & 0xFF).ToArray();
}

static void WriteSdramBlock(string host, int port, int address, IReadOnlyList<int> values)
{
    var jsonValues = new JsonArray();
    foreach (int value in values)
        jsonValues.Add(value & 0xFF);
    JsonNode response = SendVmRequest(host, port, new JsonObject
    {
        ["command"] = "write_sdram",
        ["address"] = address,
        ["values"] = jsonValues,
    });
    if (response["ok"]?.GetValue<bool>() == false)
        throw new IOException(response["error"]?.ToString() ?? response.ToJsonString());
}

static int UnknownCheckCommand(string command)
{
    Console.Error.WriteLine($"Unknown check command: {command}");
    PrintCheckUsage();
    return 1;
}

static void PrintCheckUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova check vgc-reset-stale [--remote <host>] [--port 6503] [--settle 0.5]");
    Console.Error.WriteLine("  nova check spi-bridge [--remote <host>] [--port 6503] [--base 0x7F0000] [--blocks 64] [--iterations 2] [--no-restore]");
}

static bool CommandExists(string name)
{
    string pathVariable = Environment.GetEnvironmentVariable("PATH") ?? "";
    foreach (string directory in pathVariable.Split(Path.PathSeparator, StringSplitOptions.RemoveEmptyEntries))
    {
        string path = Path.Combine(directory, name);
        if (File.Exists(path))
            return true;
        if (OperatingSystem.IsWindows() && File.Exists(path + ".exe"))
            return true;
    }
    return false;
}

static int UnknownDocsCommand(string command)
{
    Console.Error.WriteLine($"Unknown docs command: {command}");
    PrintDocsUsage();
    return 1;
}

static void PrintDocsUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova docs basic-user-guide [--repo <repo>]");
    Console.Error.WriteLine("  nova docs nova-cli-guide [--repo <repo>]");
    Console.Error.WriteLine("  nova docs fun-n-games [--repo <repo>]");
    Console.Error.WriteLine("  nova docs showcase-demo [--repo <repo>]");
}

static int DoFpga(string[] args)
{
    if (args.Length < 1 || args[0].Equals("help", StringComparison.OrdinalIgnoreCase))
    {
        PrintFpgaUsage();
        return args.Length < 1 ? 1 : 0;
    }

    string command = args[0].ToLowerInvariant();
    var rest = args[1..].ToList();
    try
    {
        return command switch
        {
            "check-timing" => DoFpgaCheckTiming(rest),
            "check-bitstream" => DoFpgaCheckBitstream(rest),
            _ => UnknownFpgaCommand(command),
        };
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"fpga {command}: {ex.Message}");
        return 1;
    }
}

static int DoFpgaCheckTiming(List<string> args)
{
    string? log = TakeOptionValue(args, "--log");
    string marginText = TakeOptionValue(args, "--margin-mhz") ?? "0";
    if (args.Count != 1 || !double.TryParse(marginText, CultureInfo.InvariantCulture, out double marginMhz))
    {
        PrintFpgaUsage();
        return 1;
    }

    return NovaBuildTools.CheckFpgaTiming(args[0], log, marginMhz);
}

static int DoFpgaCheckBitstream(List<string> args)
{
    string repo = TakeOptionValue(args, "--repo-root") ?? Directory.GetCurrentDirectory();
    bool includeRoms = TakeFlag(args, "--include-roms");
    if (args.Count != 1)
    {
        PrintFpgaUsage();
        return 1;
    }

    return NovaBuildTools.CheckBitstreamFreshness(args[0], repo, includeRoms);
}

static int UnknownFpgaCommand(string command)
{
    Console.Error.WriteLine($"Unknown fpga command: {command}");
    PrintFpgaUsage();
    return 1;
}

static void PrintFpgaUsage()
{
    Console.Error.WriteLine("Usage:");
    Console.Error.WriteLine("  nova fpga check-timing <nextpnr-report.json> [--log <nextpnr.log>] [--margin-mhz <mhz>]");
    Console.Error.WriteLine("  nova fpga check-bitstream <bitstream> [--repo-root <repo>] [--include-roms]");
}

static void EnsureParentDirectory(string path)
{
    string? directory = Path.GetDirectoryName(Path.GetFullPath(path));
    if (!string.IsNullOrEmpty(directory))
        Directory.CreateDirectory(directory);
}

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
        "pack"              => DoModulePack(rest),
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
    Console.Error.WriteLine("  nova module pack --src <module.s> --bin <module.bin> --out <module.nmod> [--syms <file>] [--ndk-dir <dir>]");
}

static int DoModulePack(string[] args)
{
    var rest = args.ToList();
    string? src = TakeOptionValue(rest, "--src");
    string? bin = TakeOptionValue(rest, "--bin");
    string? output = TakeOptionValue(rest, "--out");
    var syms = TakeRepeatedOptionValues(rest, "--syms");
    var ndkDirs = TakeRepeatedOptionValues(rest, "--ndk-dir");

    if (src is null || bin is null || output is null || rest.Count > 0)
    {
        PrintModuleUsage();
        return 1;
    }

    try
    {
        NovaBuildTools.PackNmod(src, bin, output, syms, ndkDirs);
        return 0;
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"module pack: {src}: {ex.Message}");
        return 1;
    }
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

        if (command is "screenshot" or "shot")
            return DoVmScreenshot(targetHost, port, rest);

        JsonObject request = command switch
        {
            "raw" => BuildVmRawRequest(rest),
            "reload-rom" or "reloadrom" => new JsonObject { ["command"] = "reload_rom" },
            "cold-start" or "coldstart" => BuildVmColdStartRequest(rest),
            "reset" or "vm-reset" => BuildVmResetRequest(rest),
            "wait" or "wait-ready" => BuildVmWaitRequest(rest),
            "screen" => new JsonObject { ["command"] = "read_screen" },
            "line" => BuildVmLineRequest(rest),
            "cursor" => new JsonObject { ["command"] = "get_cursor" },
            "mute-sid" or "mutesid" => new JsonObject { ["command"] = "type_text", ["text"] = "FOR I=54272 TO 54335:POKE I,0:NEXT I\r" },
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

// Capture the VGC output over the 6503 debug protocol and write a PNG, so the
// display can be inspected without HDMI. The default is a TRUE composite that
// flattens the layers the desktop renderer draws (border + background + gfx +
// text + sprites) into the native 720x480 frame, matching
// EmulatorCanvas.RenderFramebuffer. `--gfx-only` keeps the fast gfx-plane-only
// path (320x200, space 3). One persistent connection batches every
// peek/peek_block/read_vram round-trip.
static int DoVmScreenshot(string host, int port, List<string> args)
{
    bool gfxOnly = TakeFlag(args, "--gfx-only", "--gfx");
    string outPath = args.Count > 0 && !args[0].StartsWith('-') ? args[0] : "screenshot.png";
    const int PAL_IDX = 0xA0F4;        // RegPaletteIndex
    const int PAL_DATA = 0xA0F5;       // RegPaletteData (quantized RGB444 byte, auto-inc)

    using var client = new TcpClient { NoDelay = true };
    using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(120));
    client.ConnectAsync(host, port, cts.Token).GetAwaiter().GetResult();
    using NetworkStream ns = client.GetStream();
    ns.ReadTimeout = 60000; ns.WriteTimeout = 60000;
    using var reader = new StreamReader(ns, Encoding.UTF8, false, 1 << 16, leaveOpen: true);
    using var writer = new StreamWriter(ns, new UTF8Encoding(false), 4096, leaveOpen: true)
        { AutoFlush = true, NewLine = "\n" };

    JsonNode Send(string js)
    {
        writer.WriteLine(js);
        string? l = reader.ReadLine();
        return JsonNode.Parse(l ?? "{}") ?? new JsonObject();
    }

    // Read the 16x3 RGB palette (RegPaletteData returns the quantized RGB444 byte).
    byte[] pal = new byte[48];
    bool palOk = true, palNonZero = false;
    for (int i = 0; i < 48; i++)
    {
        Send($"{{\"command\":\"poke\",\"address\":{PAL_IDX},\"value\":{i}}}");
        var r = Send($"{{\"command\":\"peek\",\"address\":{PAL_DATA}}}");
        int v = r["value"]?.GetValue<int>() ?? -1;
        if (v < 0) { palOk = false; break; }
        byte q = (byte)(v & 0xF0);
        pal[i] = (byte)(q | (q >> 4));          // expand RGB444 high nibble -> 8-bit
        if (pal[i] != 0) palNonZero = true;
    }
    bool useColor = palOk && palNonZero;

    if (gfxOnly)
        return WriteGfxOnlyScreenshot(Send, outPath, pal, useColor);

    return WriteCompositeScreenshot(Send, outPath, pal, useColor);
}

// Fast path: gfx plane only (320x200, 4bpp low-nibble palette index, space 3).
static int WriteGfxOnlyScreenshot(Func<string, JsonNode> Send, string outPath, byte[] pal, bool useColor)
{
    const int W = 320, H = 200;
    byte[] idx = new byte[W * H];
    int p = 0;
    for (int a = 0; a < W * H; a += 256)
    {
        int len = Math.Min(256, W * H - a);
        var r = Send($"{{\"command\":\"read_vram\",\"space\":3,\"address\":{a},\"length\":{len}}}");
        JsonArray? arr = r["data"] as JsonArray;
        if (arr is null && r["value"] is JsonNode one) { arr = new JsonArray(); arr.Add(one.GetValue<int>()); }
        if (arr is null) { Console.Error.WriteLine($"screenshot: no gfx data at {a}: {r.ToJsonString()}"); return 1; }
        foreach (var n in arr) idx[p++] = (byte)(n!.GetValue<int>() & 0x0F);
    }

    byte[] img = new byte[W * H * 3];
    for (int i = 0; i < W * H; i++)
    {
        byte ci = idx[i];
        byte rr, gg, bb;
        if (useColor) { rr = pal[ci * 3]; gg = pal[ci * 3 + 1]; bb = pal[ci * 3 + 2]; }
        else { rr = gg = bb = (byte)(ci * 17); }   // grayscale-by-index fallback
        img[i * 3] = rr; img[i * 3 + 1] = gg; img[i * 3 + 2] = bb;
    }
    WritePng(outPath, W, H, img);
    Console.WriteLine($"Screenshot {W}x{H} (gfx-only) -> {outPath}{(useColor ? "" : " (grayscale; palette unavailable)")}");
    return 0;
}

// Loads the embedded CP437 charset (slot 0, 2048 bytes = 256 glyphs x 8 rows,
// MSB = leftmost pixel). Returns null if the resource is missing.
static byte[]? LoadNovaFont()
{
    using var s = System.Reflection.Assembly.GetExecutingAssembly()
        .GetManifestResourceStream("cp437.bin");
    if (s is null) return null;
    byte[] font = new byte[256 * 8];
    int got = 0, n;
    while (got < font.Length && (n = s.Read(font, got, font.Length - got)) > 0) got += n;
    return got == font.Length ? font : null;
}

// peek_block helper: reads `count` (<=256) contiguous CPU-visible bytes.
static int[] PeekBlock(Func<string, JsonNode> Send, int address, int count)
{
    var r = Send($"{{\"command\":\"peek_block\",\"address\":{address},\"count\":{count}}}");
    var arr = r["values"] as JsonArray;
    int[] result = new int[count];
    if (arr is not null)
        for (int i = 0; i < count && i < arr.Count; i++)
            result[i] = arr[i]!.GetValue<int>();
    return result;
}

// read_vram helper: reads `length` bytes from a VGC memory space in 256-byte
// batches. Returns null if the board returns no data for a batch.
static byte[]? ReadVramRange(Func<string, JsonNode> Send, int space, int length)
{
    byte[] buf = new byte[length];
    int p = 0;
    for (int a = 0; a < length; a += 256)
    {
        int len = Math.Min(256, length - a);
        var r = Send($"{{\"command\":\"read_vram\",\"space\":{space},\"address\":{a},\"length\":{len}}}");
        JsonArray? arr = r["data"] as JsonArray;
        if (arr is null && r["value"] is JsonNode one) { arr = new JsonArray(); arr.Add(one.GetValue<int>()); }
        if (arr is null) { Console.Error.WriteLine($"screenshot: no data for space {space} at {a}: {r.ToJsonString()}"); return null; }
        foreach (var n in arr) buf[p++] = (byte)(n!.GetValue<int>() & 0xFF);
    }
    return buf;
}

// Full composite: replicates EmulatorCanvas.RenderFramebuffer client-side into a
// 720x480 RGB frame. Reads the VGC registers (peek/peek_block), the char/color/
// text-attr planes (read_vram spaces 1/2/7), the gfx plane (space 3) and the
// referenced sprite shapes (space 4), then flattens the layers in the renderer's
// order. Copper raster lists are NOT replicated (no debug read path); custom/
// alternate fonts fall back to the embedded CP437 charset.
static int WriteCompositeScreenshot(Func<string, JsonNode> Send, string outPath, byte[] palBytes, bool useColor)
{
    const int NW = 720, NH = 480, OX = 40, OY = 40, GW = 320, GH = 200;
    const int COLS = 80, ROWS = 50, GLYPH = 8, CELLS = COLS * ROWS;

    // C64 default palette (RGB triples), used when the board palette reads back
    // empty. Mirrors e6502.Avalonia/Rendering/ColorPalette.cs.
    byte[] c64 =
    [
        0,0,0,        255,255,255,  136,0,0,      170,255,238,
        204,68,204,   0,204,85,     0,0,170,      238,238,119,
        221,136,85,   102,68,0,     255,119,119,  51,51,51,
        119,119,119,  170,255,102,  0,136,255,    187,187,187,
    ];

    // 16-entry RGB palette: board palette if available, else C64 default.
    byte[] pr = new byte[16], pg = new byte[16], pb = new byte[16];
    for (int i = 0; i < 16; i++)
    {
        if (useColor) { pr[i] = palBytes[i * 3]; pg[i] = palBytes[i * 3 + 1]; pb[i] = palBytes[i * 3 + 2]; }
        else { pr[i] = c64[i * 3]; pg[i] = c64[i * 3 + 1]; pb[i] = c64[i * 3 + 2]; }
    }

    // ---- VGC registers ----
    int[] core = PeekBlock(Send, 0xA000, 16);   // $A000-$A00F core regs
    int[] ext = PeekBlock(Send, 0xA0E0, 16);    // $A0E0-$A0EF VRAM port + scroll/dim
    int[] sreg = PeekBlock(Send, 0xA040, 128);  // $A040-$A0BF sprite registers (16 x 8)

    int mode = core[0x00] & 0xFF;               // RegMode
    int bgColor = core[0x01] & 0x0F;            // RegBgCol
    int cursorX = core[0x03] & 0xFF;            // RegCursorX
    int cursorY = core[0x04] & 0xFF;            // RegCursorY
    int scrollX = core[0x05] & 0xFF;            // RegScrollX
    int scrollY = core[0x06] & 0xFF;            // RegScrollY
    bool cursorOn = (core[0x0A] & 0x01) != 0;   // RegCursorEnable
    int borderCol = core[0x0D] & 0x0F;          // RegBorder

    int displayDim = ext[0x05] & 0x0F;          // $A0E5 DisplayDim (15 = full brightness)
    int gfxTrans = ext[0x08] & 0xFF;            // $A0E8 RegGfxTransparentColor
    int scrollCtl = ext[0x0A] & 0x07;           // $A0EA RegScrollCtl
    int textTopRow = ext[0x0D] & 0xFF;          // $A0ED RegTextTopRow
    int textScrStart = ext[0x0E] & 0xFF;        // $A0EE RegTextScrollStart
    int textScrRows = ext[0x0F] & 0xFF;         // $A0EF RegTextScrollRows

    bool textVisible = mode != 3 && mode != 4;  // ModeGfxOnly/ModeGfxSprites hide text
    bool gfxUsed = mode >= 1;                    // gfx layer sampled in modes 1-4
    bool cursorEnabled = cursorOn && textVisible;

    // Scroll decomposition (RenderVideoState).
    int Norm320(int v) => v >= GW ? v - GW : v;
    int Norm200(int v) => v >= GH ? v - GH : v;
    int scrollXFull = Norm320(scrollX | ((scrollCtl & 0x01) != 0 ? 0x100 : 0));
    int scrollYMod = Norm200(scrollY);
    int gfxSX = (scrollCtl & 0x02) != 0 ? scrollXFull : 0;
    int gfxSY = (scrollCtl & 0x02) != 0 ? scrollYMod : 0;
    int textSX = (scrollCtl & 0x04) != 0 ? scrollXFull : 0;
    int textSY = (scrollCtl & 0x04) != 0 ? scrollYMod : 0;

    int Wrap320(int v) { if (v >= GW) v -= GW; if (v >= GW) v -= GW; return v; }
    int Wrap200(int v) { if (v >= GH) v -= GH; if (v >= GH) v -= GH; return v; }
    int Wrap640(int v) { if (v >= 640) v -= 640; return v; }
    int Wrap400(int v) { if (v >= 400) v -= 400; if (v >= 400) v -= 400; return v; }

    // ---- Plane memory ----
    byte[]? chr = null, col = null, attr = null, gfx = null;
    byte[]? font = LoadNovaFont();
    if (textVisible)
    {
        if (font is null)
            Console.Error.WriteLine("screenshot: CP437 font resource missing; text layer skipped.");
        chr = ReadVramRange(Send, 1, CELLS);
        col = ReadVramRange(Send, 2, CELLS);
        attr = ReadVramRange(Send, 7, CELLS);
        if (chr is null || col is null) return 1;
        attr ??= new byte[CELLS];   // text-attr optional; default plain style
    }
    if (gfxUsed)
    {
        gfx = ReadVramRange(Send, 3, GW * GH);
        if (gfx is null) return 1;
    }

    // ---- Sprite shapes: only the slots referenced by enabled sprites ----
    var shapes = new Dictionary<int, byte[]>();
    for (int i = 0; i < 16; i++)
    {
        if ((sreg[i * 8 + 5] & 0x80) == 0) continue;   // SprRegFlags bit7 = enable
        int slot = sreg[i * 8 + 4];                     // SprRegShape
        if (shapes.ContainsKey(slot)) continue;
        var sh = ReadVramRangeAt(Send, 4, slot * 128, 128);  // 128 bytes per shape slot
        if (sh is not null) shapes[slot] = sh;
    }

    // Per-scanline sprite line buffers (visible canvas is 0..319).
    byte[] lineBehind = new byte[GW], lineBetween = new byte[GW], lineFront = new byte[GW];

    void RasterizeSprites(int scanY)
    {
        Array.Clear(lineBehind); Array.Clear(lineBetween); Array.Clear(lineFront);
        for (int i = 0; i < 16; i++)
        {
            int flags = sreg[i * 8 + 5];
            if ((flags & 0x80) == 0) continue;                 // enable
            int sy = sreg[i * 8 + 2];                          // YLo (Y is an unsigned byte)
            if (scanY < sy || scanY >= sy + 16) continue;
            int rowInSprite = scanY - sy;
            bool xFlip = (flags & 0x01) != 0;
            bool yFlip = (flags & 0x02) != 0;
            int srcRow = yFlip ? 15 - rowInSprite : rowInSprite;
            int pri = Math.Min(sreg[i * 8 + 6], 2);
            byte[] target = pri == 0 ? lineBehind : pri == 1 ? lineBetween : lineFront;
            int slot = sreg[i * 8 + 4];
            int transColor = sreg[i * 8 + 7];
            int spriteX = sreg[i * 8 + 0] | (sreg[i * 8 + 1] << 8);
            if (!shapes.TryGetValue(slot, out var shape)) continue;
            int rowBase = srcRow * 8;                          // SpriteBytesPerRow
            for (int c = 0; c < 16; c++)
            {
                int srcCol = xFlip ? 15 - c : c;
                int byteIdx = rowBase + srcCol / 2;
                int color = (srcCol % 2 == 0) ? (shape[byteIdx] >> 4) & 0x0F : shape[byteIdx] & 0x0F;
                if (color == transColor) continue;
                int sxp = spriteX + c;
                if ((uint)sxp >= GW) continue;
                target[sxp] = (byte)color;
            }
        }
    }

    int PhysicalTextRow(int displayRow)
    {
        int start = textScrStart;
        int rows = Math.Max(1, Math.Min(textScrRows, ROWS - start));
        if (displayRow < start || displayRow >= start + rows) return displayRow;
        return start + ((displayRow - start + textTopRow) % rows);
    }

    // Returns true + colour index if the text layer is opaque at this canvas pixel.
    bool SampleText(int cpx, int cpy, out int idx)
    {
        idx = 0;
        if (font is null) return false;
        int srcPx = Wrap640(cpx + (textSX << 1));
        int srcPy = Wrap400(cpy + (textSY << 1));
        int c = srcPx / GLYPH;
        int displayRow = srcPy / GLYPH;
        int row = PhysicalTextRow(displayRow);
        int cell = row * COLS + c;
        byte ch = chr![cell];
        byte colorAttr = col![cell];
        byte textAttr = attr![cell];
        int fg = colorAttr & 0x0F;
        int cellBg = (colorAttr >> 4) & 0x0F;
        bool reverse = (textAttr & 0x02) != 0;               // TextAttrReverse
        if (reverse) (fg, cellBg) = (cellBg, fg);
        bool isCursor = cursorEnabled && c == cursorX && displayRow == cursorY;
        if (isCursor) (fg, cellBg) = (cellBg, fg);
        int gx = srcPx % GLYPH, gy = srcPy % GLYPH;
        int rowBits = font[ch * GLYPH + gy];
        if ((textAttr & 0x04) != 0) rowBits |= rowBits >> 1; // TextAttrBold
        bool set = (rowBits & (0x80 >> gx)) != 0;
        // Flash (TextAttrFlash bit0) is shown lit in a still capture.
        if (mode == 2 && !set && !isCursor && !reverse && cellBg == bgColor) return false;
        idx = set ? fg : cellBg;
        return true;
    }

    // ---- Compose 720x480 ----
    byte[] img = new byte[NW * NH * 3];

    void Put(int px, int py, int ci)
    {
        int r = pr[ci], g = pg[ci], b = pb[ci];
        if (displayDim != 15)
        {
            if (displayDim == 0) { r = g = b = 0; }
            else { r = (r * displayDim) >> 4; g = (g * displayDim) >> 4; b = (b * displayDim) >> 4; }
        }
        int o = (py * NW + px) * 3;
        img[o] = (byte)r; img[o + 1] = (byte)g; img[o + 2] = (byte)b;
    }

    // Border fill (the whole frame; the gfx/text/sprite area overwrites it).
    for (int py = 0; py < NH; py++)
        for (int px = 0; px < NW; px++)
            Put(px, py, borderCol);

    for (int gy = 0; gy < GH; gy++)
    {
        RasterizeSprites(gy);
        for (int gx = 0; gx < GW; gx++)
        {
            int spriteBehind = lineBehind[gx];
            int spriteBetween = lineBetween[gx];
            int spriteFront = lineFront[gx];

            int gfxColorIndex = 0;
            bool gfxOpaque = false;
            if (gfxUsed)
            {
                int sgx = Wrap320(gx + gfxSX), sgy = Wrap200(gy + gfxSY);
                gfxColorIndex = gfx![sgy * GW + sgx];
                gfxOpaque = gfxColorIndex != gfxTrans;
            }

            for (int dy = 0; dy < 2; dy++)
            {
                int cpy = gy * 2 + dy;
                int py = OY + cpy;
                for (int dx = 0; dx < 2; dx++)
                {
                    int cpx = gx * 2 + dx;
                    int px = OX + cpx;

                    int idx = bgColor;
                    if (spriteBehind != 0) idx = spriteBehind & 0x0F;

                    bool textOpaque = false;
                    int textIdx = 0;
                    if (textVisible) textOpaque = SampleText(cpx, cpy, out textIdx);

                    if (mode == 3 || mode == 4)
                    {
                        if (gfxOpaque) idx = gfxColorIndex & 0x0F;
                        if (spriteBetween != 0) idx = spriteBetween & 0x0F;
                    }
                    else if (mode == 2)
                    {
                        if (gfxOpaque) idx = gfxColorIndex & 0x0F;
                        if (spriteBetween != 0) idx = spriteBetween & 0x0F;
                        if (textOpaque) idx = textIdx;
                    }
                    else // mode 0 or 1
                    {
                        if (textOpaque) idx = textIdx;
                        if (spriteBetween != 0) idx = spriteBetween & 0x0F;
                        if (mode >= 1 && gfxOpaque) idx = gfxColorIndex & 0x0F;
                    }

                    if (spriteFront != 0) idx = spriteFront & 0x0F;

                    Put(px, py, idx);
                }
            }
        }
    }

    WritePng(outPath, NW, NH, img);
    string fontNote = font is null ? " (text skipped: font missing)" : "";
    string palNote = useColor ? "" : " (C64 default palette; board palette unavailable)";
    Console.WriteLine($"Screenshot {NW}x{NH} composite (mode {mode}) -> {outPath}{palNote}{fontNote}");
    return 0;
}

// read_vram helper that starts at an arbitrary space offset (single batch, <=256).
static byte[]? ReadVramRangeAt(Func<string, JsonNode> Send, int space, int address, int length)
{
    var r = Send($"{{\"command\":\"read_vram\",\"space\":{space},\"address\":{address},\"length\":{length}}}");
    JsonArray? arr = r["data"] as JsonArray;
    if (arr is null && r["value"] is JsonNode one) { arr = new JsonArray(); arr.Add(one.GetValue<int>()); }
    if (arr is null) return null;
    byte[] buf = new byte[length];
    int p = 0;
    foreach (var n in arr) { if (p >= length) break; buf[p++] = (byte)(n!.GetValue<int>() & 0xFF); }
    return buf;
}

// ---- minimal PNG (truecolour 8-bit) ----------------------------------------
static void WritePng(string path, int w, int h, byte[] rgb)
{
    using var fs = File.Create(path);
    fs.Write(new byte[] { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A });
    void Chunk(string type, byte[] data)
    {
        Span<byte> len = stackalloc byte[4];
        System.Buffers.Binary.BinaryPrimitives.WriteUInt32BigEndian(len, (uint)data.Length);
        fs.Write(len);
        byte[] t = Encoding.ASCII.GetBytes(type);
        fs.Write(t); fs.Write(data);
        uint crc = Crc32(t, data);
        Span<byte> c = stackalloc byte[4];
        System.Buffers.Binary.BinaryPrimitives.WriteUInt32BigEndian(c, crc);
        fs.Write(c);
    }
    byte[] ihdr = new byte[13];
    System.Buffers.Binary.BinaryPrimitives.WriteUInt32BigEndian(ihdr.AsSpan(0), (uint)w);
    System.Buffers.Binary.BinaryPrimitives.WriteUInt32BigEndian(ihdr.AsSpan(4), (uint)h);
    ihdr[8] = 8; ihdr[9] = 2;   // 8-bit depth, colour type 2 (truecolour RGB)
    Chunk("IHDR", ihdr);

    using var raw = new MemoryStream();
    for (int y = 0; y < h; y++) { raw.WriteByte(0); raw.Write(rgb, y * w * 3, w * 3); }
    Chunk("IDAT", ZlibCompress(raw.ToArray()));
    Chunk("IEND", Array.Empty<byte>());
}

static byte[] ZlibCompress(byte[] data)
{
    using var ms = new MemoryStream();
    ms.WriteByte(0x78); ms.WriteByte(0x9C);
    using (var ds = new DeflateStream(ms, CompressionLevel.Optimal, leaveOpen: true))
        ds.Write(data, 0, data.Length);
    uint a = Adler32(data);
    ms.WriteByte((byte)(a >> 24)); ms.WriteByte((byte)(a >> 16));
    ms.WriteByte((byte)(a >> 8)); ms.WriteByte((byte)a);
    return ms.ToArray();
}

static uint Adler32(byte[] d)
{
    uint a = 1, b = 0;
    foreach (byte x in d) { a = (a + x) % 65521; b = (b + a) % 65521; }
    return (b << 16) | a;
}

static uint Crc32(byte[] a, byte[] b)
{
    uint crc = 0xFFFFFFFF;
    void Up(byte[] d) { foreach (byte x in d) { crc ^= x; for (int k = 0; k < 8; k++) crc = (crc & 1) != 0 ? (crc >> 1) ^ 0xEDB88320 : crc >> 1; } }
    Up(a); Up(b);
    return crc ^ 0xFFFFFFFF;
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
    Console.Error.WriteLine("  reload-rom");
    Console.Error.WriteLine("  reset [--text <text>] [--no-wait]");
    Console.Error.WriteLine("  wait [text] [--timeout-ms <ms>]");
    Console.Error.WriteLine("  screen [--json]");
    Console.Error.WriteLine("  screenshot [out.png] [--gfx-only]   (default: full 720x480 composite)");
    Console.Error.WriteLine("  line <row>");
    Console.Error.WriteLine("  cursor");
    Console.Error.WriteLine("  mute-sid");
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

static List<string> TakeRepeatedOptionValues(List<string> args, params string[] names)
{
    var values = new List<string>();
    for (int i = 0; i < args.Count;)
    {
        string arg = args[i];
        string? matchedName = names.FirstOrDefault(name => arg.StartsWith(name + "=", StringComparison.Ordinal));
        if (matchedName is not null)
        {
            values.Add(arg[(matchedName.Length + 1)..]);
            args.RemoveAt(i);
            continue;
        }

        if (names.Contains(arg, StringComparer.Ordinal) && i + 1 < args.Count)
        {
            values.Add(args[i + 1]);
            args.RemoveRange(i, 2);
            continue;
        }

        i++;
    }

    return values;
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
        ".pas"            => NdiFileType.Pascal,
        ".logo" or ".lgo" => NdiFileType.Logo,
        ".s" or ".asm" or ".inc" => NdiFileType.Assembly,
        ".sid"            => NdiFileType.Sid,
        ".bin"            => NdiFileType.Bin,
        ".mid" or ".midi" or ".nms" => NdiFileType.Mid,
        ".nvg"            => NdiFileType.Gfx,
        ".4th" or ".fth" or ".fs" or ".fr" => NdiFileType.Forth,
        _                 => NdiFileType.Bin,
    };

/// <summary>
/// Searches upward from the executable directory, then from cwd,
/// for software/languages/ehbasic/tokens.json.
/// </summary>
static string? FindTokensJson()
{
    string? dir = Path.GetDirectoryName(Environment.ProcessPath);
    while (dir is not null)
    {
        string candidate = Path.Combine(dir, "software", "languages", "ehbasic", "tokens.json");
        if (File.Exists(candidate))
            return candidate;
        dir = Path.GetDirectoryName(dir);
    }

    dir = Directory.GetCurrentDirectory();
    while (dir is not null)
    {
        string candidate = Path.Combine(dir, "software", "languages", "ehbasic", "tokens.json");
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
    Console.Error.WriteLine("  convert    hex16-to-bin|bin-header ...");
    Console.Error.WriteLine("  codegen    tokens|novavm-inc|runtime-abi|ndk-reference ...");
    Console.Error.WriteLine("  build      browser-rust-core ...");
    Console.Error.WriteLine("  docs       basic-user-guide|nova-cli-guide|fun-n-games|showcase-demo ...");
    Console.Error.WriteLine("  fpga       check-timing|check-bitstream ...");
    Console.Error.WriteLine("  ci         install-linux-deps|install-macos-cc65|mint-github-token ...");
    Console.Error.WriteLine("  publish    [rid]");
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

sealed class TempDir : IDisposable
{
    private TempDir(string path) => Path = path;
    public string Path { get; }

    public static TempDir Create(string prefix)
    {
        string path = System.IO.Path.Combine(System.IO.Path.GetTempPath(), prefix + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(path);
        return new TempDir(path);
    }

    public void Dispose()
    {
        if (Directory.Exists(Path))
            Directory.Delete(Path, recursive: true);
    }
}

sealed class TempFile : IDisposable
{
    private TempFile(string path) => Path = path;
    public string Path { get; }

    public static TempFile Create(string suffix)
    {
        string path = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "nova-" + Guid.NewGuid().ToString("N") + suffix);
        if (File.Exists(path))
            File.Delete(path);
        return new TempFile(path);
    }

    public void Dispose()
    {
        if (File.Exists(Path))
            File.Delete(Path);
    }
}

readonly record struct VgcResetPlane(string Name, int Space, int Length, int DirtyValue, bool ExpectZero);

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
