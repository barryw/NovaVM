using System;
using System.IO;
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
        SeedForthLibraries();
        Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", StorageRoot);
        Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", "1");
        Environment.SetEnvironmentVariable("NOAUTO", "1");
    }

    [AssemblyCleanup]
    public static void Cleanup()
    {
        Environment.SetEnvironmentVariable("NOAUTO", null);
        Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
        Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", null);

        if (Directory.Exists(StorageRoot))
            Directory.Delete(StorageRoot, recursive: true);
    }

    private static void SeedForthLibraries()
    {
        string source = FindRepoPath("novaforth", "forth");
        string target = Path.Combine(StorageRoot, "hd0", "forth");
        CopyDirectory(source, target);
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

    private static void CopyDirectory(string source, string target)
    {
        Directory.CreateDirectory(target);
        foreach (string sourceFile in Directory.GetFiles(source))
        {
            string targetFile = Path.Combine(target, Path.GetFileName(sourceFile));
            File.Copy(sourceFile, targetFile, overwrite: true);
        }

        foreach (string sourceDir in Directory.GetDirectories(source))
        {
            string targetDir = Path.Combine(target, Path.GetFileName(sourceDir));
            CopyDirectory(sourceDir, targetDir);
        }
    }
}
