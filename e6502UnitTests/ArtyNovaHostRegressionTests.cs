using System;
using System.IO;
using System.Text.RegularExpressions;
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

    private static string LinuxNovaVmSrc(string file) =>
        RepoPath("e6502.FPGA", "boards", "arty_z7", "linux", "novavm", file);

    private static string PetalinuxMetaUser(params string[] parts) =>
        RepoPath(["e6502.FPGA", "boards", "arty_z7", "petalinux", "meta-user", .. parts]);

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

    [TestMethod]
    public void LinuxDebugReset_UsesLockedHelperWithoutNestedBridgeLock()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nservers.c"));

        StringAssert.Contains(src, "static void vm_reset_ctrl_locked(int cold)",
            "Linux novavm reset needs a locked helper so debug reset does not re-enter BRIDGE_LOCK.");
        StringAssert.Contains(src, "static void vm_reset_ctrl(int cold)",
            "Linux novavm still needs the public reset wrapper for management reset callers.");
        StringAssert.Contains(src, "vm_reset_ctrl_locked(0);",
            "The debug warm-reset handler runs under dbg_dispatch's BRIDGE_LOCK and must call the locked helper directly.");
    }

    [TestMethod]
    public void LinuxDebugColdStart_UsesHostColdBootPath()
    {
        string header = File.ReadAllText(LinuxNovaVmSrc("novavm.h"));
        string src = File.ReadAllText(LinuxNovaVmSrc("nservers.c"));

        StringAssert.Contains(header, "host_reboot_vm",
            "novavm.h must expose the real host cold-boot entry point to the debug server.");
        StringAssert.Contains(src, "host_reboot_vm();",
            "Linux debug cold_start must run the real host cold boot, not just pulse reset.");
        int start = src.IndexOf("static void do_cold_start", StringComparison.Ordinal);
        Assert.IsTrue(start >= 0, "Linux debug server must keep a dedicated cold-start handler.");
        int end = src.IndexOf("\n}", start, StringComparison.Ordinal);
        Assert.IsTrue(end > start, "The cold-start handler should be a normal C function body.");
        string body = src[start..end];
        Assert.IsFalse(body.Contains("vm_reset_ctrl", StringComparison.Ordinal),
            "The debug cold_start handler must not collapse back to a reset pulse; it must re-stage ROM/loader and re-run autoboot.");
    }

    [TestMethod]
    public void LinuxDebugTypeText_PacesInjectedKeys()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nservers.c"));

        StringAssert.Contains(src, "usleep(20000);",
            "Linux debug type_text must pace injected keys; blasting a line into R_KEY can drop input on hardware.");
    }

    [TestMethod]
    public void LinuxDebugSendKey_AltLetterQueuesEscapeThenLowercaseLetter()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nservers.c"));

        StringAssert.Contains(src, "ALT-",
            "Linux debug send_key must understand Alt-letter names used by the editor.");
        StringAssert.Contains(src, "kb_emit(0x1B);",
            "Alt-letter injection must emit Escape first, matching the editor's Alt prefix.");
        StringAssert.Contains(src, "kb_emit((unsigned char)ch);\n            usleep(20000);",
            "Alt-letter injection must pace the final key; otherwise hardware can acknowledge the command before the key is visible.");
        StringAssert.Contains(src, "kb_emit((unsigned char)code);\n    usleep(20000);",
            "Single-key injection must pace R_KEY like type_text; otherwise Enter/Ctrl-S can be dropped on hardware.");
        StringAssert.Contains(src, "Unknown key",
            "Linux debug send_key must fail loudly for unsupported names instead of returning ok without input.");
    }

    [TestMethod]
    public void LinuxPhysicalKeyboard_MenuModifierQueuesEditorHotkey()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nkbd.c"));

        StringAssert.Contains(src, "KEY_LEFTALT",
            "Physical USB keyboard Alt/Option must be tracked so editor menu hotkeys work on hardware.");
        StringAssert.Contains(src, "KEY_RIGHTALT",
            "Physical USB keyboard Alt/Option must handle both Apple Option keys.");
        StringAssert.Contains(src, "KEY_LEFTMETA",
            "Physical USB keyboard Command/Super must also act as the editor menu modifier.");
        StringAssert.Contains(src, "KEY_RIGHTMETA",
            "Physical USB keyboard Command/Super must handle both Apple Command keys.");
        StringAssert.Contains(src, "int lalt = 0, ralt = 0, lmeta = 0, rmeta = 0;",
            "Physical menu modifiers must be tracked per key; one release event must not clear another held Alt/Command key.");
        StringAssert.Contains(src, "menu_mod = lalt || ralt || lmeta || rmeta",
            "Physical menu-modifier state must be recomputed from all held Alt/Command keys.");
        Assert.IsFalse(src.Contains("menu_mod = down", StringComparison.Ordinal),
            "Physical menu-modifier state must not be a single shared boolean updated by whichever modifier event arrived last.");
        StringAssert.Contains(src, "wr(R_KEY, 0x1B)",
            "Physical menu-modifier hotkeys must use the editor hotkey path.");
    }

    [TestMethod]
    public void LinuxPhysicalKeyboard_ArrowKeysQueueEditorNavigationKeys()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nkbd.c"));

        foreach ((string key, string value) in new[]
        {
            ("KEY_LEFT", "0x1C"),
            ("KEY_RIGHT", "0x1D"),
            ("KEY_UP", "0x1E"),
            ("KEY_DOWN", "0x1F"),
        })
        {
            StringAssert.Contains(src, key,
                $"Physical USB keyboard {key} must be translated for editor/menu navigation on hardware.");
            StringAssert.Contains(src, $"wr(R_KEY, {value})",
                $"Physical USB keyboard {key} must emit Nova navigation byte {value}.");
        }
    }

    [TestMethod]
    public void PetalinuxUsbKeyboard_IsEnabledEndToEnd()
    {
        string dtsi = File.ReadAllText(PetalinuxMetaUser("recipes-bsp", "device-tree", "files", "system-user.dtsi"));
        string kernel = File.ReadAllText(PetalinuxMetaUser("recipes-kernel", "linux", "linux-xlnx", "bsp.cfg"));
        string conf = File.ReadAllText(PetalinuxMetaUser("conf", "petalinuxbsp.conf"));
        string recipe = File.ReadAllText(PetalinuxMetaUser("recipes-apps", "novavm", "novavm.bb"));
        string host = File.ReadAllText(LinuxNovaVmSrc("novavm.c"));

        StringAssert.Contains(dtsi, "drv-vbus;",
            "USB host mode needs the ULPI PHY VBUS drive declared.");
        StringAssert.Contains(dtsi, "&usb0 {\n\tstatus = \"okay\";",
            "The Arty appliance must not ship with the PS USB host disabled.");
        StringAssert.Contains(dtsi, "dr_mode = \"host\";",
            "The PS USB controller must be forced to host mode for keyboards.");
        StringAssert.Contains(dtsi, "usb-phy = <&usb_phy0>;",
            "The PS USB controller must bind the declared ULPI PHY.");
        foreach (string symbol in new[] { "CONFIG_USB_CHIPIDEA_HOST=y", "CONFIG_INPUT_EVDEV=y", "CONFIG_USB_HID=y" })
        {
            StringAssert.Contains(kernel, symbol,
                $"{symbol} must stay enabled so USB keyboards enumerate as evdev nodes.");
        }
        StringAssert.Contains(conf, "usbutils",
            "The image should keep usbutils so hardware USB enumeration is inspectable.");
        StringAssert.Contains(recipe, "nkbd.c",
            "The novavm image recipe must compile the evdev keyboard bridge.");
        StringAssert.Contains(host, "kbd_init();",
            "The running novavm host must start the physical keyboard reader.");
    }

    [TestMethod]
    public void BuildrootUsbKeyboard_IsEnabledEndToEnd()
    {
        string dts = File.ReadAllText(RepoPath("e6502.FPGA", "boards", "arty_z7", "linux", "br2-external", "board", "novavm", "arty_z7", "novavm-arty-z7.dts"));
        string kernel = File.ReadAllText(RepoPath("e6502.FPGA", "boards", "arty_z7", "linux", "br2-external", "board", "novavm", "arty_z7", "linux-uio.fragment"));
        string host = File.ReadAllText(LinuxNovaVmSrc("novavm.c"));

        StringAssert.Contains(dts, "drv-vbus;",
            "The live Buildroot Arty image needs the ULPI PHY VBUS drive declared.");
        StringAssert.Contains(dts, "&usb0",
            "The live Buildroot Arty image must enable the PS USB0 controller.");
        StringAssert.Contains(dts, "dr_mode = \"host\";",
            "The PS USB controller must be forced to host mode for keyboards.");
        StringAssert.Contains(dts, "usb-phy = <&usb_phy0>;",
            "The PS USB controller must bind the declared ULPI PHY.");
        foreach (string symbol in new[] { "CONFIG_INPUT_EVDEV=y", "CONFIG_USB_HID=y", "CONFIG_USB_CHIPIDEA_HOST=y" })
        {
            StringAssert.Contains(kernel, symbol,
                $"{symbol} must stay enabled so USB keyboards enumerate as evdev nodes.");
        }
        StringAssert.Contains(host, "kbd_init();",
            "The running novavm host must start the physical keyboard reader.");
    }

    [TestMethod]
    public void LinuxNfioSave_RollsBackPartialNdiEntriesOnFailure()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nfio.c"));

        StringAssert.Contains(src, "ndi_bam_free(m, start, sector_count);",
            "ndi_create must free allocated sectors if metadata publication fails.");
        StringAssert.Contains(src, "ndi_dir_clear_slot(m, slot);",
            "ndi_create must clear the directory slot if metadata publication fails.");
        StringAssert.Contains(src, "ndi_delete(img, fname, parent);\n        fio_fail(FIO_ERR_IO);",
            "nfio_disk_save must delete the just-created NDI entry if payload write fails, or BASIC DIR shows phantom zero-byte files.");
        StringAssert.Contains(src, "if (ndi_zero_tail(img, idx) != 0)",
            "nfio_disk_save must zero-fill the allocated sector tail and treat that as part of the atomic save.");
    }

    [TestMethod]
    public void LinuxNfioSaveAndMkdir_DoNotAutoCreateParentDirectories()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nfio.c"));

        StringAssert.Contains(src, "ndi_walk(img, path, 0, &parent, fname, sizeof fname)",
            "SAVE must require existing parent directories; missing dirs should be created only by MKDIR.");
        StringAssert.DoesNotMatch(src, new Regex(@"void nfio_disk_save[\s\S]*?ndi_walk\(img, path, 1, &parent, fname, sizeof fname\)"),
            "nfio_disk_save must not auto-create missing parent directories.");
        StringAssert.DoesNotMatch(src, new Regex(@"static int ndi_gfx_store[\s\S]*?ndi_walk\(img, path, 1, &parent, fname, sizeof fname\)"),
            "GSAVE must not auto-create missing parent directories.");
        StringAssert.DoesNotMatch(src, new Regex(@"void nfio_mkdir[\s\S]*?ndi_walk\(img, path, 1, &parent, fname, sizeof fname\)"),
            "MKDIR may create the named leaf, but it must not recursively create missing parents.");
    }

    [TestMethod]
    public void LinuxNfioDirRead_ReportsPayloadSizeAndClearsHighSizeByte()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("nfio.c"));

        StringAssert.Contains(src, "uint32_t display_size = e.size_bytes;",
            "NDI DIRREAD must compute a display size instead of exposing raw directory bytes.");
        StringAssert.Contains(src, "e.file_type == DT_BAS || e.file_type == DT_BIN",
            "BASIC/BIN files store a two-byte load address internally; DIR must subtract it from the user-visible size.");
        StringAssert.Contains(src, "display_size -= 2;",
            "DIR must report BASIC/BIN payload bytes, not the internal load-address prefix.");
        StringAssert.Contains(src, "poke(FIO_SIZE2, (display_size >> 16) & 0xFF);",
            "DIRREAD must clear/update the high size byte so stale 24-bit sizes cannot leak into callers.");
    }

    [TestMethod]
    public void LinuxLoadModule_StreamsEmbeddedModuleIntoRequestedXramSlot()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("novavm.c"));

        StringAssert.Contains(src, "static int load_module(int id, int slot)",
            "Linux LOAD_MODULE must honor the slot requested by the 6502 lib_call loader.");
        StringAssert.Contains(src, "memcpy((void *)(g_xram + SHELF_BASE + (unsigned)slot * SHELF_SLOT), img, MODULE_BYTES);",
            "Linux LOAD_MODULE must stream the module into the requested XRAM shelf slot before PGD page-in.");
        StringAssert.Contains(src, "load_module(id, slot)",
            "The FIO dispatch must pass FIO_END_LO's victim slot to load_module.");
    }

    [TestMethod]
    public void LinuxLoadModule_InvalidatesShelfTagsBeforeReportingOk()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("novavm.c"));

        const string call = "if (load_module(id, slot) == 0) {";
        const string clear = "for (int s = 0; s < 4; s++) poke(0x0418 + s, 0);";
        const string ok = "poke(FIO_ERRCODE, 0); poke(FIO_STATUS, FIO_OK);";

        int callIndex = src.IndexOf(call, StringComparison.Ordinal);
        int clearIndex = src.IndexOf(clear, callIndex >= 0 ? callIndex : 0, StringComparison.Ordinal);
        int okIndex = src.IndexOf(ok, callIndex >= 0 ? callIndex : 0, StringComparison.Ordinal);

        Assert.IsTrue(callIndex >= 0, "Linux LOAD_MODULE dispatch must keep a success branch around load_module.");
        Assert.IsTrue(clearIndex > callIndex,
            "The current Arty page-in path mirrors the loaded module into bank 1; shelf hits for older modules would validate the wrong bank unless LOAD_MODULE invalidates shelf tags.");
        Assert.IsTrue(okIndex > clearIndex,
            "LOAD_MODULE must clear shelf tags before FIO_STATUS=OK so the 6502 records only the freshly loaded victim slot.");
    }

    [TestMethod]
    public void LinuxColdBoot_ClearsRuntimeLowRamBeforeRestagingLoader()
    {
        string src = File.ReadAllText(LinuxNovaVmSrc("novavm.c"));

        StringAssert.Contains(src, "static void clear_runtime_low_ram(void)",
            "Linux cold boot must explicitly clear low runtime state while the 6502 is held in reset.");
        StringAssert.Contains(src, "for (unsigned a = 0x0275; a < 0x0320; a++) poke(a, 0);",
            "Cold boot must clear BASIC's input buffer plus the $0300 mailbox before release.");
        StringAssert.Contains(src, "for (unsigned a = 0x0418; a < 0x0900; a++) poke(a, 0);",
            "Cold boot must clear stale shelf tags/LRU and module BSS without wiping the resident loader body.");
        StringAssert.Contains(src, "poke(0x041C + i, (unsigned char)i);",
            "Cold boot must seed shelf LRU to valid slot indexes after clearing it.");
        StringAssert.Contains(src, "clear_runtime_low_ram();\n    for (unsigned i = 0; i < sizeof(LOADER_BIN); i++)",
            "Cold boot must clear low runtime RAM before restaging the resident loader at $0320.");
    }

    [TestMethod]
    public void PetalinuxUsbMouse_IsEnabledEndToEnd()
    {
        string kernel = File.ReadAllText(PetalinuxMetaUser("recipes-kernel", "linux", "linux-xlnx", "bsp.cfg"));
        string recipe = File.ReadAllText(PetalinuxMetaUser("recipes-apps", "novavm", "novavm.bb"));
        string host = File.ReadAllText(LinuxNovaVmSrc("novavm.c"));
        string mouse = File.ReadAllText(LinuxNovaVmSrc("nmouse.c"));

        foreach (string symbol in new[] { "CONFIG_INPUT_EVDEV=y", "CONFIG_USB_HID=y" })
        {
            StringAssert.Contains(kernel, symbol,
                $"{symbol} must stay enabled so USB mice enumerate as evdev nodes.");
        }
        StringAssert.Contains(recipe, "nmouse.c",
            "The novavm image recipe must compile the evdev mouse bridge.");
        StringAssert.Contains(File.ReadAllText(RepoPath("e6502.Nova", "Program.cs")), "\"nmouse.c\"",
            "nova arty build-linux-host must compile the evdev mouse bridge used by hardware deploys.");
        StringAssert.Contains(host, "mouse_init();",
            "The running novavm host must start the physical mouse reader.");
        StringAssert.Contains(mouse, "REL_X",
            "The mouse bridge must consume relative X motion from evdev.");
        StringAssert.Contains(mouse, "REL_Y",
            "The mouse bridge must consume relative Y motion from evdev.");
        StringAssert.Contains(mouse, "BTN_LEFT",
            "The mouse bridge must translate evdev buttons into VGC mouse button bits.");
        StringAssert.Contains(mouse, "VGC_MOUSE_XL",
            "The mouse bridge must write the VGC mouse register block, not a private shadow.");
        StringAssert.Contains(mouse, "set_cursor_visible(1)",
            "The mouse bridge should show the hardware pointer only after a mouse is detected.");
        StringAssert.Contains(mouse, "MOUSE_SHAPE_SLOT  255u",
            "The host must install a default cursor shape outside the 16 visible sprite slots.");
    }

    [TestMethod]
    public void BuildPsFio_RequiresNovaCliPayloadSyncBeforeVitis()
    {
        string cli = File.ReadAllText(RepoPath("e6502.Nova", "Program.cs"));
        string hook = File.ReadAllText(RepoPath("e6502.FPGA", "boards", "arty_z7", "vitis", "build_ps_fio.py"));

        StringAssert.Contains(cli, "DoArtySyncPayloads(repo, [])",
            "nova arty build-ps-fio must sync payloads before Vitis can package a PS firmware.");
        StringAssert.Contains(cli, "NOVA_ARTY_SYNC_PAYLOADS_DONE",
            "nova arty build-ps-fio must mark the Vitis hook as sync-guarded after payload sync succeeds.");
        StringAssert.Contains(hook, "NOVA_ARTY_SYNC_PAYLOADS_DONE",
            "The Vitis hook must fail direct execution so PS firmware cannot be built with stale payload headers.");
        Assert.IsFalse(hook.Contains("\"dotnet\", \"run\"", StringComparison.Ordinal),
            "The Vitis hook must not invoke dotnet inside Vitis' LD_LIBRARY_PATH; Nova CLI owns payload sync.");
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
