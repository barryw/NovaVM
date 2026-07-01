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

    // mgmt.c must handle drive mount/unmount (CBOR 10/11) -> `nova drive mount`.
    [TestMethod]
    public void MgmtC_HandlesDriveMountUnmount()
    {
        string src = File.ReadAllText(ArtySrc("mgmt.c"));
        StringAssert.Contains(src, "handle_mount_drive",
            "mgmt.c must handle MountDrive (CBOR 10) for `nova drive mount`.");
        StringAssert.Contains(src, "handle_unmount_drive",
            "mgmt.c must handle UnmountDrive (CBOR 11) for `nova drive unmount`.");
    }

    // The FIO host must service the Z-machine boot/story commands so a mounted
    // .ndi game (Zork I verified) autoboots: LOAD_RUNTIME (0x28), XLOAD (0x18),
    // XPAGE (0x29, story paging), and the FOPEN/FREAD/FSEEK file handles.
    [TestMethod]
    public void MainC_HandlesZMachineFioCommands()
    {
        string src = File.ReadAllText(ArtySrc("main.c"));
        foreach (string h in new[]
        {
            "fio_load_runtime", "fio_xload", "fio_xpage",
            "fio_fopen", "fio_fread", "fio_fseek",
        })
        {
            StringAssert.Contains(src, h,
                $"main.c must keep the {h} FIO handler (Z-machine boot path).");
        }
    }

    // The .ndi parser must be the SHARED ndi_image.cpp from the ESP32 NovaHost
    // tree (single source of truth), reused via the FatFs shim -- NOT a duplicate
    // parser. Guard the build wiring + the shim so the reuse can't silently
    // regress back into a fork.
    [TestMethod]
    public void Build_ReusesSharedNdiParser()
    {
        string py = File.ReadAllText(RepoPath("e6502.FPGA", "boards", "arty_z7", "vitis", "build_ps_fio.py"));
        StringAssert.Contains(py, "e6502.ESP32",
            "build_ps_fio.py must import ndi_image.cpp from the shared ESP32 NovaHost tree.");
        StringAssert.Contains(py, "ndi_image.cpp",
            "build must compile the shared ndi_image.cpp (no duplicate parser).");

        string shim = File.ReadAllText(ArtySrc("ndi_shim.cpp"));
        StringAssert.Contains(shim, "ndi::NdiImage",
            "ndi_shim.cpp must delegate to the shared ndi::NdiImage, not reimplement parsing.");

        Assert.IsFalse(File.Exists(ArtySrc("ndi.c")),
            "the duplicate ndi.c parser must stay deleted (reuse ndi_image.cpp instead).");
    }

    // Z-machine V6 SAVE (SAVLOAD.OVL: create_file -> write chunks -> close) needs
    // the FIO host to dispatch FCREATE (0x2E) + FWRITE (0x31). These were missing
    // -> novaz's create_file fell through to the error branch -> "SAVE failed!"
    // before any file op. Verified on HW: SAVE00.NZS (48851 bytes) written, then
    // RESTORE read it back. Guard the dispatch + handlers so save can't regress.
    [TestMethod]
    public void MainC_HandlesSaveWriteFioCommands()
    {
        string src = File.ReadAllText(ArtySrc("main.c"));
        StringAssert.Contains(src, "FIO_CMD_FCREATE) fio_fcreate",
            "main.c must dispatch FCREATE (0x2E) -> fio_fcreate for save file creation.");
        StringAssert.Contains(src, "FIO_CMD_FWRITE)  fio_fwrite",
            "main.c must dispatch FWRITE (0x31) -> fio_fwrite for save data.");
        foreach (string h in new[] { "fio_fcreate", "fio_fwrite", "fio_commit_write" })
        {
            StringAssert.Contains(src, h,
                $"main.c must implement the {h} save-write handler.");
        }
    }

    // FCLOSE on a write handle commits the staged save into the mounted .ndi via
    // the shared ndi_image writer. The shim must open the image read-write and
    // expose the write API (create_file + chunked writes + zero-tail + delete).
    [TestMethod]
    public void NdiShim_ExposesWritePath()
    {
        string shim = File.ReadAllText(ArtySrc("ndi_shim.cpp"));
        StringAssert.Contains(shim, "FA_READ | FA_WRITE",
            "ndi_shim must open the .ndi read-write so saves persist to the image.");
        foreach (string fn in new[] { "ndi_create", "ndi_write", "ndi_zero_tail", "ndi_delete" })
        {
            StringAssert.Contains(shim, fn,
                $"ndi_shim must expose {fn} over the shared ndi::NdiImage write API.");
        }
        StringAssert.Contains(shim, "write_file_chunk_by_index",
            "ndi_shim must stream save data via the shared write_file_chunk_by_index (no fork).");
    }

    // The V6 save UI lives in SAVLOAD.OVL, loaded by overlay_load_fixed from the
    // mounted image. The .ndi MUST bundle it or the overlay load fails and SAVE
    // returns "SAVE failed!" before any file op (root cause #2 of save-broken).
    [TestMethod]
    public void NovazPack_BundlesSaveLoadOverlay()
    {
        string mk = File.ReadAllText(RepoPath("software", "examples", "novaz", "Makefile"));
        StringAssert.Contains(mk, "SAVLOAD.OVL",
            "novaz PACK_ARGS must bundle SAVLOAD.OVL into the .ndi for V6 save/restore.");
    }
}
