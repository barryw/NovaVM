using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

// Regression guard for the Arty Z7-20 PS NovaHost port: the 6504 management
// server (NVH1 + CBOR, mgmt.c) and the 6503 debug server (newline JSON, debug.c)
// that let the `nova` CLI drive the board with `--remote`. The protocols match
// e6502.NovaHost/{NovaHostManagementClient,NovaDebugClient}.cs and the ESP32
// reference (e6502.ESP32/novahost). Firmware C isn't CI-buildable (needs Vitis),
// so these assert the load-bearing wiring + handlers stay in the source.
// End-to-end proof was against the live CLI (nova device/put/ls/get/rm,
// nova vm screen/peek/poke/enter, nova keyboard) on 192.168.1.213.
[TestClass]
public class ArtyNovaHostRegressionTests
{
    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }

    private static string ArtySrc(string file) =>
        RepoPath("e6502.FPGA", "boards", "arty_z7", "ps_fio", "src", file);

    // mgmt.c: the 6504 management server must bind the NovaHost port and frame
    // responses with the NVH1 magic the C# client validates.
    [TestMethod]
    public void MgmtC_BindsPort6504_WithNvh1Framing()
    {
        string src = File.ReadAllText(ArtySrc("mgmt.c"));
        StringAssert.Contains(src, "MGMT_PORT       6504",
            "mgmt.c must serve the NovaHost management protocol on TCP 6504.");
        StringAssert.Contains(src, "\"NVH1\"",
            "mgmt.c must emit the NVH1 frame magic the C# management client checks.");
    }

    // mgmt.c: the command dispatch must cover the verified surface -- Hello(1),
    // GetStatus(2), ListDirectory(3), Write{Begin/Chunk/Commit/Abort}(5..8),
    // DeletePath(9), VmReset(14), ReadFileChunk(19).
    [TestMethod]
    public void MgmtC_HandlesVerifiedCommands()
    {
        string src = File.ReadAllText(ArtySrc("mgmt.c"));
        foreach (string handler in new[]
        {
            "handle_hello", "handle_get_status", "handle_list_directory",
            "handle_read_file_chunk", "handle_write_begin", "handle_write_chunk",
            "handle_write_commit", "handle_delete_path", "handle_vm_reset",
        })
        {
            StringAssert.Contains(src, handler,
                $"mgmt.c must keep the {handler} handler (verified against the nova CLI).");
        }
    }

    // mgmt.c GOTCHA: ListDirectory must collect entries in one forward pass. An
    // earlier count->f_readdir(rewind)->emit approach broke (rewind didn't work)
    // and the CBOR array count mismatched the emitted entries ("CBOR payload
    // ended early"). Guard the single-pass collection so it can't regress.
    [TestMethod]
    public void MgmtC_ListDirectory_SinglePassCollection()
    {
        string src = File.ReadAllText(ArtySrc("mgmt.c"));
        StringAssert.Contains(src, "LS_MAX",
            "ListDirectory must single-pass-collect entries (rewind-free) to keep the CBOR array count exact.");
    }

    // debug.c: the 6503 debug server must bind its port and handle the verified
    // VM/keyboard commands the `nova vm`/`keyboard` verbs send.
    [TestMethod]
    public void DebugC_BindsPort6503_HandlesVmCommands()
    {
        string src = File.ReadAllText(ArtySrc("debug.c"));
        StringAssert.Contains(src, "DBG_PORT   6503",
            "debug.c must serve the NovaHost debug protocol on TCP 6503.");
        foreach (string cmd in new[]
        {
            "peek", "poke", "peek_block", "read_vram", "send_key", "type_text",
            "read_screen", "read_line", "get_cursor", "vm_reset", "cold_start",
        })
        {
            StringAssert.Contains(src, $"\"{cmd}\"",
                $"debug.c must dispatch the \"{cmd}\" debug command.");
        }
    }

    // Both servers ride lwIP and are started after net_init() (lwip_init) -- so
    // main.c must call them once the stack is up.
    [TestMethod]
    public void MainC_StartsBothServersAfterNet()
    {
        string src = File.ReadAllText(ArtySrc("main.c"));
        StringAssert.Contains(src, "mgmt_init();",
            "main.c must start the 6504 management server.");
        StringAssert.Contains(src, "debug_init();",
            "main.c must start the 6503 debug server.");
    }
}
