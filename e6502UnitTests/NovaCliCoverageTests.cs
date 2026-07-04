using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests
{
    [TestClass]
    public class NovaCliCoverageTests
    {
        private static readonly string[] DocumentedCommandGroups =
        {
            "publish",
            "convert",
            "codegen",
            "docs",
            "build",
            "capture",
            "check",
            "fpga",
            "arty",
            "ci",
            "module",
            "vm",
        };

        private static readonly string[] AllowedScripts =
        {
            "e6502.FPGA/boards/arty_z7/vitis/build_ps_fio.py",
            "e6502.FPGA/boards/ulx3s/place_hdmi_serializer.py",
            "software/examples/novachess/vendor/chess6502-engine/tools/ld65_dbg_to_sim6502_sym.py",
            "software/examples/novachess/vendor/chess6502-engine/tools/report_size.py",
            "software/examples/novachess/vendor/chess6502-engine/tools/run_engine_benchmarks.py",
        };

        [TestMethod]
        public void HelpCommandsReturnUsage()
        {
            RunNova("help");
            foreach (string command in DocumentedCommandGroups)
                StringAssert.Contains(RunNova(command, "help"), "Usage:");
        }

        [TestMethod]
        public void NovaCliGuideDocumentsCommandGroups()
        {
            string repo = FindRepoRoot();
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));

            foreach (string command in DocumentedCommandGroups)
                StringAssert.Contains(guide, "nova " + command);
        }

        [TestMethod]
        public void RepoInstructionsRequireNovaCliDocs()
        {
            string repo = FindRepoRoot();
            string instructions = File.ReadAllText(Path.Combine(repo, "AGENTS.md"));

            StringAssert.Contains(instructions, "docs/books/nova-cli-guide/chapters/nova-cli.md");
            StringAssert.Contains(instructions, "docs/script-inventory.md");
            StringAssert.Contains(instructions, "e6502UnitTests/NovaCliCoverageTests.cs");
        }

        [TestMethod]
        public void ArtyEditorDemoDeployClearsMountedBootDrives()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));

            StringAssert.Contains(program, "UnmountAllBootDrives(host)");
            StringAssert.Contains(program, "MakeArgsWithCurrentNova");
            StringAssert.Contains(program, "WaitForRemotePort(host, 6504");
            StringAssert.Contains(program, "pidof novavm");
            StringAssert.Contains(guide, "editor-demo.ndi");
            StringAssert.Contains(guide, "mounts it as `fd0`");
        }

        [TestMethod]
        public void ArtyLinuxHostDeployDoesNotRequireManagementToRecover()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));

            StringAssert.Contains(program,
                "RunScp(host, Path.Combine(repo, \"software\", \"languages\", \"ehbasic\", \"basic.bin\"), \"/data/nova/roms/ehbasic.bin\")",
                "deploy-linux-host must use SSH/SCP for the base ROM so it can recover when the management service is down.");
            StringAssert.Contains(program,
                "RunScp(host, editorDemoImage!.Path, \"/data/nova/disks/floppy/editor-demo.ndi\")",
                "deploy-editor-demo must upload a bootable NDI over SSH/SCP before restarting NovaVM.");
            StringAssert.Contains(program,
                "DoDriveMount([\"fd0\", \"/disks/floppy/editor-demo.ndi\"], host)",
                "deploy-editor-demo must mount the uploaded NDI as fd0 instead of using a host fallback path.");
        }

        [TestMethod]
        public void ArtyDeployRefreshesBootableLanguageDisks()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));

            StringAssert.Contains(program,
                "Path.Combine(repo, \"software\", \"languages\", \"novalogo\"), \"ndi\"",
                "sync-payloads must rebuild novalogo.ndi before any Arty deploy can boot it.");
            StringAssert.Contains(program,
                "Path.Combine(repo, \"software\", \"languages\", \"novaforth\"), \"ndi\"",
                "sync-payloads must rebuild novaforth.ndi before any Arty deploy can boot it.");
            StringAssert.Contains(program,
                "RunScp(host, Path.Combine(repo, \"software\", \"languages\", \"novalogo\", \"novalogo.ndi\"), \"/data/nova/disks/floppy/novalogo.ndi\")",
                "deploy-linux-host must upload the rebuilt Logo disk, not leave hardware booting stale editor hooks.");
            StringAssert.Contains(program,
                "RunScp(host, Path.Combine(repo, \"software\", \"languages\", \"novaforth\", \"novaforth.ndi\"), \"/data/nova/disks/floppy/novaforth.ndi\")",
                "deploy-linux-host must upload the rebuilt Forth disk for the same stale-code reason.");
        }

        [TestMethod]
        public void ArtyLinuxHostDeployVerifiesInstalledBinaryHash()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));

            StringAssert.Contains(program,
                "VerifyRemoteFileSha256(host, Path.Combine(hostDir, \"novavm\"), \"/usr/bin/novavm\")",
                "deploy-linux-host must prove the installed Arty host binary matches the just-built local binary.");
            StringAssert.Contains(program,
                "RunSshCapture(host, \"sha256sum \" + remotePath",
                "deploy-linux-host must verify remote /usr/bin/novavm by content hash, not timestamp.");
        }

        [TestMethod]
        public void ConvertHex16ToBinWritesLittleEndianBytes()
        {
            string temp = Path.Combine(Path.GetTempPath(), "nova-cli-test-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(temp);
            try
            {
                string input = Path.Combine(temp, "in.hex");
                string output = Path.Combine(temp, "out.bin");
                File.WriteAllText(input, "1234\n0x00FF\n// comment\n\n");

                RunNova("convert", "hex16-to-bin", input, output);

                CollectionAssert.AreEqual(new byte[] { 0x34, 0x12, 0xFF, 0x00 }, File.ReadAllBytes(output));
            }
            finally
            {
                Directory.Delete(temp, recursive: true);
            }
        }

        [TestMethod]
        public void ToolsDirectoryHasNoShellOrPythonScripts()
        {
            string tools = Path.Combine(FindRepoRoot(), "tools");
            string[] scripts = Directory.GetFiles(tools, "*", SearchOption.TopDirectoryOnly)
                .Where(path => path.EndsWith(".sh", StringComparison.OrdinalIgnoreCase)
                               || path.EndsWith(".py", StringComparison.OrdinalIgnoreCase))
                .Select(Path.GetFileName)
                .ToArray()!;

            Assert.AreEqual(0, scripts.Length, "Move scripts into nova or delete them: " + string.Join(", ", scripts));
        }

        [TestMethod]
        public void RepoHasOnlyDocumentedScriptHooks()
        {
            string repo = FindRepoRoot();
            string[] scripts = Directory.GetFiles(repo, "*", SearchOption.AllDirectories)
                .Where(path => !path.Contains(Path.DirectorySeparatorChar + ".git" + Path.DirectorySeparatorChar, StringComparison.Ordinal))
                .Where(IsSourcePath)
                .Where(path => path.EndsWith(".sh", StringComparison.OrdinalIgnoreCase)
                               || path.EndsWith(".py", StringComparison.OrdinalIgnoreCase))
                .Select(path => Path.GetRelativePath(repo, path).Replace('\\', '/'))
                .OrderBy(path => path, StringComparer.Ordinal)
                .ToArray();

            CollectionAssert.AreEqual(
                AllowedScripts.OrderBy(path => path, StringComparer.Ordinal).ToArray(),
                scripts,
                "Unexpected script file. Move it into nova, document it as an external hook, or delete it.");
        }

        private static bool IsSourcePath(string path)
        {
            string normalized = path.Replace('\\', '/');
            return !new[] { "/.tmp-home/", "/bin/", "/obj/", "/build/", "/dist/", "/target/" }
                .Any(part => normalized.Contains(part, StringComparison.Ordinal));
        }

        private static string RunNova(params string[] args)
        {
            string repo = FindRepoRoot();
            var psi = new ProcessStartInfo(FindNovaExecutable(repo))
            {
                WorkingDirectory = repo,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
            };
            foreach (string arg in args)
                psi.ArgumentList.Add(arg);

            using Process process = Process.Start(psi) ?? throw new InvalidOperationException("dotnet did not start");
            string stdout = process.StandardOutput.ReadToEnd();
            string stderr = process.StandardError.ReadToEnd();
            Assert.IsTrue(process.WaitForExit(30000), "nova command timed out: " + string.Join(" ", args));
            Assert.AreEqual(0, process.ExitCode, stdout + stderr);
            return stdout + stderr;
        }

        private static string FindNovaExecutable(string repo)
        {
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

            throw new FileNotFoundException("Build e6502.Nova before running Nova CLI coverage tests.", Path.Combine(outputRoot, executableName));
        }

        private static string FindRepoRoot()
        {
            string? dir = AppContext.BaseDirectory;
            while (dir is not null)
            {
                if (File.Exists(Path.Combine(dir, "e6502.sln")))
                    return dir;
                dir = Directory.GetParent(dir)?.FullName;
            }

            throw new DirectoryNotFoundException("Could not find repo root from " + AppContext.BaseDirectory);
        }
    }
}
