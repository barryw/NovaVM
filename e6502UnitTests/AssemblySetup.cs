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
}
