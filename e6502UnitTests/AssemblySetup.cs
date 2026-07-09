using System;
using System.Collections.Generic;
using System.IO;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public sealed class AssemblySetup
{
    private static readonly string StorageRoot =
        Path.Combine(Path.GetTempPath(), $"e6502-tests-{Guid.NewGuid():N}");

    [AssemblyInitialize]
    public static void Initialize(TestContext context)
    {
        Directory.CreateDirectory(StorageRoot);
        SeedLanguageDisk();
        Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", StorageRoot);
        Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
        Environment.SetEnvironmentVariable("NOAUTO", "1");
    }

    [AssemblyCleanup]
    public static void Cleanup()
    {
        // Guard against test env leaks. Every test must leave the process-global
        // config env vars exactly as AssemblyInitialize set them; a test that
        // changes one without restoring it silently corrupts every later test in
        // the process (this once broke the whole Forth/NDK suite when a test leaked
        // NOVA_NO_AUTOMOUNT=1 and later tests could no longer automount their disk).
        // Use `using new EnvScope(name, value)` for any env change in a test.
        var leaks = new List<string>();
        CheckBaseline(leaks, "NOVA_NO_AUTOMOUNT", null);
        CheckBaseline(leaks, "NOAUTO", "1");
        CheckBaseline(leaks, "NOVA_STORAGE_ROOT", StorageRoot);

        Environment.SetEnvironmentVariable("NOAUTO", null);
        Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
        Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", null);

        if (Directory.Exists(StorageRoot))
            Directory.Delete(StorageRoot, recursive: true);

        if (leaks.Count > 0)
            throw new InvalidOperationException(
                "Test environment leak detected — a test changed a process-global config " +
                "env var without restoring it, which silently corrupts later tests. Wrap env " +
                "changes in `using new EnvScope(name, value)`. Leaked var(s):\n  " +
                string.Join("\n  ", leaks));
    }

    private static void CheckBaseline(List<string> leaks, string name, string? expected)
    {
        string? actual = Environment.GetEnvironmentVariable(name);
        if (actual != expected)
            leaks.Add($"{name} = \"{actual ?? "(unset)"}\" (expected \"{expected ?? "(unset)"}\")");
    }

    private static void SeedLanguageDisk()
    {
        string disksDir = Path.Combine(StorageRoot, "disks");
        Directory.CreateDirectory(disksDir);
        string imagePath = Path.Combine(disksDir, "fd0.ndi");
        NdiImage.CreateFormatted(imagePath, "TESTLANG", 1440);

        using var image = NdiImage.Open(imagePath);
        string forthSource = FindRepoPath("software", "languages", "novaforth", "forth");
        ushort forthDir = (ushort)image.MakeDirectory("forth", 0xFFFF);
        CopyDirectoryToImage(image, forthSource, forthDir);

        string logoHook = Path.Combine(AppContext.BaseDirectory, "Resources", "LOGOHK.BIN");
        if (!File.Exists(logoHook))
            logoHook = Path.Combine(FindRepoPath("software", "languages", "novalogo"), "LOGOHK.BIN");
        image.WriteFile("LOGOHK.BIN", NdiFileType.Bin, 0xFFFF, File.ReadAllBytes(logoHook));
    }

    private static string FindRepoPath(params string[] parts)
    {
        string? dir = AppContext.BaseDirectory;
        while (!string.IsNullOrEmpty(dir))
        {
            string candidate = Path.Combine([dir, .. parts]);
            if (Directory.Exists(candidate))
                return candidate;
            dir = Directory.GetParent(dir)?.FullName;
        }

        dir = Directory.GetCurrentDirectory();
        while (!string.IsNullOrEmpty(dir))
        {
            string candidate = Path.Combine([dir, .. parts]);
            if (Directory.Exists(candidate))
                return candidate;
            dir = Directory.GetParent(dir)?.FullName;
        }

        throw new DirectoryNotFoundException($"Could not find repository path: {Path.Combine(parts)}");
    }

    private static void CopyDirectoryToImage(NdiImage image, string source, ushort parent)
    {
        foreach (string sourceFile in Directory.GetFiles(source))
        {
            string fileName = Path.GetFileName(sourceFile);
            image.WriteFile(fileName, GetNdiFileType(fileName), parent, File.ReadAllBytes(sourceFile));
        }

        foreach (string sourceDir in Directory.GetDirectories(source))
        {
            ushort child = (ushort)image.MakeDirectory(Path.GetFileName(sourceDir), parent);
            CopyDirectoryToImage(image, sourceDir, child);
        }
    }

    private static NdiFileType GetNdiFileType(string fileName)
    {
        string ext = Path.GetExtension(fileName);
        return ext.Equals(".4th", StringComparison.OrdinalIgnoreCase) ||
               ext.Equals(".fth", StringComparison.OrdinalIgnoreCase) ||
               ext.Equals(".fs", StringComparison.OrdinalIgnoreCase) ||
               ext.Equals(".fr", StringComparison.OrdinalIgnoreCase)
            ? NdiFileType.Forth
            : NdiFileType.Bin;
    }
}
