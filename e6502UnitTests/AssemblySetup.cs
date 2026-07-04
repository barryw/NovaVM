using System;
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
        Environment.SetEnvironmentVariable("NOAUTO", null);
        Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
        Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", null);

        if (Directory.Exists(StorageRoot))
            Directory.Delete(StorageRoot, recursive: true);
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
