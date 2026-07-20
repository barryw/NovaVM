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
        public void PascalDirectoryTypesAreDocumentedAndDistinct()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova", "Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs", "books", "nova-cli-guide", "chapters", "nova-cli.md"));

            StringAssert.Contains(program, "PASCAL SOURCE");
            StringAssert.Contains(program, "PASCAL PROJECT");
            StringAssert.Contains(guide, "`PASCAL SOURCE`");
            StringAssert.Contains(guide, "`PASCAL PROJECT`");
        }

        [TestMethod]
        public void ShowcaseDemoBuildsCanonicalLinkedMusicDisk()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));
            int start = program.IndexOf("static int DoDocsShowcaseDemo", StringComparison.Ordinal);
            int end = program.IndexOf("static (string Source, string Dest, string Dir)[] ShowcaseSidFiles", StringComparison.Ordinal);

            Assert.IsTrue(start >= 0 && end > start);
            string showcase = program[start..end];
            StringAssert.Contains(showcase, "\"apps\", \"music\", \"music.bin\"");
            StringAssert.Contains(showcase, "\"music\"");
            Assert.IsFalse(showcase.Contains("KEYBOARD.bin", StringComparison.Ordinal),
                "The visualizer is linked into music.bin; a second keyboard binary is dead payload.");
            Assert.IsFalse(showcase.Contains("\"apps\", \"demo\"", StringComparison.Ordinal),
                "The retired demo.s browser must not return as a second showcase implementation.");
            StringAssert.Contains(program, "(\"champagne-supernova.mid\", \"/featured\")",
                "The first embedded FEATURED entry must also be packaged on the disk.");
            StringAssert.Contains(program, "(\"washington-post-march.mid\", \"/featured\")",
                "The selected Washington Post showcase sequence must be packaged.");
            StringAssert.Contains(program, "(\"semper-fidelis.mid\", \"/featured\")",
                "The selected Semper Fidelis showcase sequence must be packaged.");
            StringAssert.Contains(showcase, "\"CREDITS.txt\"",
                "The user-supplied showcase sequence must carry its provenance on the disk.");
            StringAssert.Contains(showcase, "\"e6502.Browser\", \"wwwroot\", \"showcase\", \"demo.ndi\"");
            StringAssert.Contains(showcase, "\"website\", \"emulator\", \"showcase\", \"demo.ndi\"");
            StringAssert.Contains(guide, "linked keyboard visualizer");
            StringAssert.Contains(guide, "obsolete standalone `KEYBOARD.bin`");
            StringAssert.Contains(guide, "browser copies");
            StringAssert.Contains(guide, "The Washington Post");
        }

        [TestMethod]
        public void CaptureHdmiSupportsLinuxV4l2AndMacAvfoundation()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/fpga-debugging-workflow.md"));

            StringAssert.Contains(program, "HDMI_BACKEND",
                "capture hdmi must allow selecting the host capture backend.");
            StringAssert.Contains(program, "OSPlatform.OSX",
                "capture hdmi must keep avfoundation for macOS capture hosts.");
            StringAssert.Contains(program, "\"v4l2\"",
                "capture hdmi must support Linux capture hosts.");
            StringAssert.Contains(program, "\"/dev/video0\"",
                "Linux capture should default to the first v4l2 capture device.");
            StringAssert.Contains(guide, "HDMI_BACKEND=v4l2",
                "hardware screenshot docs must show the Linux capture backend.");
        }

        [TestMethod]
        public void StandardCaptureUsesV4l2AlsaAndUsbGadgetInterfaces()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string artyTop = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/arty_z7_full.sv"));
            string dts = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/linux/br2-external/board/novavm/arty_z7/novavm-arty-z7.dts"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));

            StringAssert.Contains(program, "capture screen");
            StringAssert.Contains(program, "/dev/video0");
            StringAssert.Contains(program, "hw:NovaVM,0");
            StringAssert.Contains(artyTop, "hdmi_capture");
            StringAssert.Contains(artyTop, "osd_active_out");
            StringAssert.Contains(dts, "novavm,capture");
            StringAssert.Contains(guide, "UVC");
            StringAssert.Contains(guide, "UAC2");
        }

        [TestMethod]
        public void StandardCaptureScreenUsesFreshV4l2FrameWithoutDebugFallback()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            int start = program.IndexOf("static int DoCaptureScreen", StringComparison.Ordinal);
            int end = program.IndexOf("static int DoCaptureRecord", StringComparison.Ordinal);

            Assert.IsTrue(start >= 0 && end > start, "DoCaptureScreen must stay a distinct hardware capture path.");
            string captureScreen = program[start..end];
            StringAssert.Contains(program, "BuildFfmpegStillArgs");
            StringAssert.Contains(captureScreen, "BuildFfmpegStillCommand",
                "Remote hardware screenshots must use the standard V4L2 capture command on the Arty.");
            StringAssert.Contains(program, "\"-frames:v\", \"2\"",
                "Hardware screenshots must ask V4L2 for two frames and write the last one; one-frame ffmpeg captures leave buffers owned on close and can return a stale first frame.");
            StringAssert.Contains(program, "capture screen reads the Arty V4L2 device",
                "The CLI source must make the hardware screenshot contract explicit so debug-composite capture is not reintroduced by accident.");
            Assert.IsFalse(captureScreen.Contains("DoVmScreenshot", StringComparison.Ordinal),
                "nova capture screen must not route through the VM/debug-composite screenshot path.");
        }

        [TestMethod]
        public void NovaCapReportsStreamParametersForFfmpeg()
        {
            string repo = FindRepoRoot();
            string driver = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-kernel/novacap/files/novacap.c"));

            StringAssert.Contains(driver, "novacap_g_parm",
                "The V4L2 driver must implement VIDIOC_G_PARM so standard tools can read the capture cadence without warnings.");
            StringAssert.Contains(driver, ".vidioc_g_parm = novacap_g_parm",
                "The stream-parameter ioctl must be registered with V4L2.");
            StringAssert.Contains(driver, "V4L2_CAP_TIMEPERFRAME",
                "The driver must advertise that timeperframe is fixed and known.");
            StringAssert.Contains(driver, "1001");
            StringAssert.Contains(driver, "60000");
        }

        [TestMethod]
        public void ArtyCaptureUsesBlockRamBuffering()
        {
            string repo = FindRepoRoot();
            string capture = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/hdmi_capture.sv"));

            StringAssert.Contains(capture, "ram_style = \"block\"",
                "HDMI capture must not burn distributed LUT RAM; the full Arty design is LUT-bound.");
            StringAssert.Contains(capture, "VIDEO_BURST_BEATS",
                "Post-OSD capture cannot issue one AXI transaction per pixel; hardware showed massive video_drops.");
            StringAssert.Contains(capture, "capture_prefetch_queue",
                "The capture writer must stream AXI burst data from a prefetch queue; a registered FIFO read plus per-beat load bubble corrupts and drops frames.");
            Assert.IsFalse(capture.Contains("assign m_axi_awlen   = 8'd0", StringComparison.Ordinal),
                "Single-beat video writes cannot keep up with 720x480 HDMI capture.");
            Assert.IsFalse(capture.Contains("S_LOAD", StringComparison.Ordinal),
                "A load state between W beats prevents the capture writer from draining one pixel per pixel clock.");
            Assert.IsFalse(capture.Contains("reg         video_rd", StringComparison.Ordinal),
                "A registered FIFO read strobe is observed by the BRAM FIFO one cycle too late.");
            Assert.IsFalse(capture.Contains("sfifo", StringComparison.Ordinal),
                "Use a capture-local BRAM FIFO instead of the generic distributed-RAM sfifo.");
        }

        [TestMethod]
        public void ArtyCaptureHasMappedControlRegisters()
        {
            string repo = FindRepoRoot();
            string artyTop = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/arty_z7_full.sv"));
            string build = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/build_full.tcl"));
            string captureRegsPath = Path.Combine(repo, "e6502.FPGA/boards/arty_z7/capture_regs.sv");

            Assert.IsTrue(File.Exists(captureRegsPath), "capture@40010000 must have real RTL registers.");
            string captureRegs = File.ReadAllText(captureRegsPath);
            StringAssert.Contains(captureRegs, "32'h4E43_4150",
                "Capture regs must expose a stable NCAP magic value for the Linux driver.");
            StringAssert.Contains(captureRegs, "axi_lite_demux2",
                "PS GP0 must be split between fio_bridge and capture_regs inside the PL.");
            StringAssert.Contains(artyTop, "capture_regs");
            StringAssert.Contains(artyTop, "axi_lite_demux2");
            StringAssert.Contains(build, "capture_regs.sv");
        }

        [TestMethod]
        public void ArtyLinuxImageIncludesStandardCaptureDriver()
        {
            string repo = FindRepoRoot();
            string driver = Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-kernel/novacap/files/novacap.c");
            string makefile = Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-kernel/novacap/files/Makefile");
            string recipe = Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-kernel/novacap/novacap.bb");
            string gadget = Path.Combine(repo, "e6502.FPGA/boards/arty_z7/linux/novavm/novacap-gadget.c");
            string novavmRecipe = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-apps/novavm/novavm.bb"));
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string config = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-kernel/linux/linux-xlnx/bsp.cfg"));
            string image = File.ReadAllText(Path.Combine(repo, "e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-core/images/petalinux-image-minimal.bbappend"));

            Assert.IsTrue(File.Exists(driver), "novavm,capture must bind to a real Linux driver.");
            Assert.IsTrue(File.Exists(makefile), "The out-of-tree capture module recipe must ship a kernel Makefile.");
            Assert.IsTrue(File.Exists(recipe), "The capture driver must be packaged into the PetaLinux image.");
            Assert.IsTrue(File.Exists(gadget), "nova capture gadget must call an installed first-class utility, not a missing binary.");

            string driverText = File.ReadAllText(driver);
            string makefileText = File.ReadAllText(makefile);
            string recipeText = File.ReadAllText(recipe);
            StringAssert.Contains(driverText, "video_register_device");
            StringAssert.Contains(driverText, "snd_card_new");
            StringAssert.Contains(driverText, "novavm,capture");
            StringAssert.Contains(driverText, "div_u64",
                "ARM kernel modules must use kernel division helpers instead of emitting __aeabi_*divmod.");
            Assert.IsFalse(driverText.Contains("frames_to_bytes", StringComparison.Ordinal),
                "NovaVM capture audio is fixed S16_LE stereo; avoid ALSA helper divisions in the kernel module.");
            Assert.IsFalse(driverText.Contains("bytes_to_frames", StringComparison.Ordinal),
                "NovaVM capture audio is fixed S16_LE stereo; avoid ALSA helper divisions in the kernel module.");
            StringAssert.Contains(makefileText, "modules_install",
                "Yocto module.bbclass invokes the module Makefile directly; obj-m alone fails with 'No targets'.");
            StringAssert.Contains(recipeText, "KERNEL_MODULE_AUTOLOAD += \"novacap\"",
                "The hardware capture device must appear after boot without a manual modprobe.");
            StringAssert.Contains(novavmRecipe, "novacap-gadget.c");
            StringAssert.Contains(novavmRecipe, "novacap-gadget");
            StringAssert.Contains(program, "\"novacap-gadget.c\"");
            StringAssert.Contains(program, "\"/usr/bin/novacap-gadget\"");
            StringAssert.Contains(config, "CONFIG_MEDIA_SUPPORT=y");
            StringAssert.Contains(config, "CONFIG_SND=y");
            StringAssert.Contains(config, "CONFIG_USB_CONFIGFS_F_UVC=y");
            StringAssert.Contains(config, "CONFIG_USB_CONFIGFS_F_UAC2=y");
            StringAssert.Contains(image, "kernel-module-novacap");
            StringAssert.Contains(image, "ffmpeg");
            StringAssert.Contains(image, "v4l-utils");
            StringAssert.Contains(image, "alsa-utils");
        }

        [TestMethod]
        public void ArtyLinuxImageBuildAndDeployAreFirstClassNovaCommands()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));

            StringAssert.Contains(program, "\"build-linux-image\" => DoArtyBuildLinuxImage",
                "Capture depends on the Linux DT/module/rootfs, so the PetaLinux image build must be a supported nova command.");
            StringAssert.Contains(program, "\"deploy-boot-image\" => DoArtyDeployBootImage",
                "Installing capture-capable Linux must be a supported nova command, not a one-off scp.");
            StringAssert.Contains(program, "petalinux-config");
            StringAssert.Contains(program, "petalinux-build");
            StringAssert.Contains(program, "petalinux-package");
            StringAssert.Contains(program, "VerifyPetalinuxCaptureImage",
                "The build must fail loudly if image.ub does not contain the novavm capture node/packages.");
            StringAssert.Contains(program, "EnsurePetalinuxBitstream",
                "The build must use the bitstream embedded in the XSA when the loose Vivado .bit file is absent.");
            StringAssert.Contains(program, "ExtractBitstreamFromXsa",
                "The XSA is the authoritative hardware handoff; do not require stale loose bitstream files for Linux image packaging.");
            StringAssert.Contains(program, "VerifyVivadoBitstreamFresh(bitstream)",
                "Linux image packaging must fail loudly when the latest Vivado implementation run failed, instead of packaging a stale bitstream.");
            StringAssert.Contains(program, "VerifyVivadoBitstreamFresh(bit)",
                "Baremetal BOOT.bin packaging must fail loudly when the latest Vivado implementation run failed, instead of packaging a stale bitstream.");
            StringAssert.Contains(program, "\".*.error.rst\"",
                "Vivado writes hidden error marker files beside failed implementation runs; the CLI must treat those as stale-bitstream blockers.");
            StringAssert.Contains(program, "refusing stale bitstream",
                "The CLI error should clearly explain that an existing .bit file is stale after a failed Vivado run.");
            StringAssert.Contains(program, "EnsureFreshPsFioBuild",
                "make-boot-bin must rebuild a missing or stale PS FIO ELF itself, so hardware packaging never depends on a remembered second command.");
            StringAssert.Contains(program, "NOVA_ARTY_SYNC_PAYLOADS_DONE",
                "PS FIO rebuilds launched by make-boot-bin must reuse the payloads it just synchronized.");
            StringAssert.Contains(program, "RunScp(host, imageUb, \"/run/image.ub.new\")");
            StringAssert.Contains(program, "RunScp(host, bootBin, \"/run/BOOT.BIN.new\")");
            StringAssert.Contains(program, "const string bootMount = \"/run/media/boot-mmcblk0p1\"",
                "The Arty boot partition is mounted by udev under /run/media, not at the rootfs /boot directory.");
            StringAssert.Contains(program, "mount -o remount,rw \" + bootMount",
                "The appliance boot partition may be read-only; deploy must remount it before replacing image.ub/BOOT.BIN.");
            StringAssert.Contains(program, "mount -o remount,ro \" + bootMount",
                "After replacing boot files, deploy should leave /boot read-only again.");
            StringAssert.Contains(program, "RunScp(host, rootfsExt4, \"/data/nova/rootfs.ext4.new\")",
                "Capture packages live in the rootfs, so deploy must not stop at /boot/image.ub.");
            StringAssert.Contains(program, "VerifyRemoteStagingMount(host, \"/data\", \"/dev/mmcblk0p2\")",
                "Rootfs staging must refuse to read the new image from the root partition it is about to overwrite.");
            StringAssert.Contains(program, "WriteAndVerifyRemoteRootfs",
                "Rootfs deploy must verify the bytes immediately after dd and before reboot; mounted ext4 metadata can change after boot.");
            StringAssert.Contains(program, "VerifyRemoteFileSha256(host, imageUb, bootMount + \"/image.ub\")");
            StringAssert.Contains(program, "VerifyRemoteFileSha256(host, bootBin, bootMount + \"/BOOT.BIN\")");
            Assert.IsFalse(program.Contains("VerifyRemoteBlockSha256(host, rootfsExt4, \"/dev/mmcblk0p2\")", StringComparison.Ordinal),
                "Do not byte-compare the rootfs block device after boot; the mounted ext4 filesystem is not a stable raw image.");
            Assert.IsFalse(program.Contains("static int VerifyRemoteBlockSha256", StringComparison.Ordinal),
                "Dead post-boot raw rootfs verification code should not stay around.");
            Assert.IsFalse(program.Contains("conv=fsync", StringComparison.Ordinal),
                "The Arty BusyBox dd does not support conv=fsync; use dd followed by sync.");
            StringAssert.Contains(program, "cp /bin/busybox /data/nova/busybox.deploy",
                "Rootfs deploy must stage a reboot-capable binary on /data before overwriting /.");
            StringAssert.Contains(program, "busybox + \" sync && \"");
            StringAssert.Contains(program, "/data/nova/busybox.deploy reboot -f");
            StringAssert.Contains(program, "ReadRemoteBootId(host)",
                "Deploy must capture the current Linux boot ID before rebooting.");
            StringAssert.Contains(program, "WaitForRemoteBoot(host, previousBootId, TimeSpan.FromSeconds(120))",
                "Fast reboots can be missed by a down-only poll; deploy must wait for boot_id to change.");
            Assert.IsFalse(program.Contains("WaitForSshDown(host, TimeSpan.FromSeconds(30))", StringComparison.Ordinal),
                "Do not require observing SSH down; the board can reboot and come back between polls.");

            StringAssert.Contains(guide, "nova arty build-linux-image");
            StringAssert.Contains(guide, "nova arty deploy-boot-image --remote 192.168.1.188");
            StringAssert.Contains(guide, "rebuilds the PS FIO app when the ELF is missing or older");
        }

        [TestMethod]
        public void CaptureRecordRemoteCopiesRecordingBackToCaller()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));

            StringAssert.Contains(program, "RunScpFrom(host, remotePath, output)",
                "Remote capture recordings must come back to the caller like screenshots; leaving them on /tmp on the board is not a usable capture workflow.");
            StringAssert.Contains(program, "\"/run/novavm-screen-\"",
                "Remote screenshots must use writable tmpfs; /tmp is on the read-only rootfs.");
            StringAssert.Contains(program, "\"/data/nova/novavm-record-\"",
                "Remote recordings can be large and must use the writable data partition, not rootfs /tmp.");
            Assert.IsFalse(program.Contains("\"/tmp/novavm-screen-", StringComparison.Ordinal),
                "Do not write remote screenshots to read-only rootfs /tmp.");
            Assert.IsFalse(program.Contains("\"/tmp/novavm-record-", StringComparison.Ordinal),
                "Do not write remote recordings to read-only rootfs /tmp.");
            StringAssert.Contains(program, "novavm-record-",
                "Remote capture recordings should use a board-side temporary path so local output paths are not confused with remote paths.");
        }

        [TestMethod]
        public void CaptureDocsUseCurrentArtyAddress()
        {
            string repo = FindRepoRoot();
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));
            string workflow = File.ReadAllText(Path.Combine(repo, "docs/fpga-debugging-workflow.md"));

            StringAssert.Contains(guide, "192.168.1.188",
                "The CLI guide must use the current Arty address from the hardware notes.");
            StringAssert.Contains(workflow, "192.168.1.188",
                "The FPGA workflow must use the current Arty address from the hardware notes.");
            Assert.IsFalse(guide.Contains("192.168.1.65", StringComparison.Ordinal),
                "Do not document the stale Arty IP; stale hardware addresses waste debugging time.");
            Assert.IsFalse(workflow.Contains("192.168.1.65", StringComparison.Ordinal),
                "Do not document the stale Arty IP; stale hardware addresses waste debugging time.");
        }

        [TestMethod]
        public void FpgaVivadoUtilizationIsFirstClassNovaCommand()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));

            StringAssert.Contains(program, "\"vivado-utilization\" => DoFpgaVivadoUtilization",
                "Vivado utilization reports must be generated through nova, not through one-off Tcl scripts.");
            StringAssert.Contains(program, "report_utilization -hierarchical",
                "The Arty placement failures need hierarchical utilization, not just a flat summary.");
            StringAssert.Contains(program, "File.Delete(tcl)",
                "The CLI may generate Vivado Tcl internally, but must not leave ad hoc helper scripts behind.");
            StringAssert.Contains(guide, "nova fpga vivado-utilization");
        }

        [TestMethod]
        public void NovaZMakefileSerializesSharedBuildDirectory()
        {
            string repo = FindRepoRoot();
            string makefile = File.ReadAllText(Path.Combine(repo, "software/examples/novaz/Makefile"));

            StringAssert.Contains(makefile, "NOVAZ_BUILD_LOCK",
                "NovaZ make invocations share build/ and must take an inter-process lock before building.");
            StringAssert.Contains(makefile, "NOVAZ_MAKE_LOCKED",
                "The lock must cover prerequisites too, not only individual target recipes.");
            StringAssert.Contains(makefile, "flock",
                "Separate make processes must serialize instead of corrupting shared linker inputs.");
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
                "RunScp(host, Path.Combine(repo, \"software\", \"languages\", \"novalogo\", \"novalogo.ndi\"), \"/data/nova/disks/languages/novalogo.ndi\")",
                "deploy-linux-host must upload the rebuilt Logo disk, not leave hardware booting stale editor hooks.");
            StringAssert.Contains(program,
                "RunScp(host, Path.Combine(repo, \"software\", \"languages\", \"novaforth\", \"novaforth.ndi\"), \"/data/nova/disks/languages/novaforth.ndi\")",
                "deploy-linux-host must upload the rebuilt Forth disk for the same stale-code reason.");
            StringAssert.Contains(program, "/data/nova/disks/languages",
                "deploy-linux-host must create the languages disk folder for Logo, Forth, and future Pascal images.");
            StringAssert.Contains(program,
                "rm -f /data/nova/disks/floppy/novalogo.ndi /data/nova/disks/floppy/novaforth.ndi",
                "deploy-linux-host must clean the old flat floppy language image locations when moving them under languages/.");
        }

        [TestMethod]
        public void ArtyDeployRefreshesAndVerifiesSuperNovaDisk()
        {
            string repo = FindRepoRoot();
            string program = File.ReadAllText(Path.Combine(repo, "e6502.Nova/Program.cs"));
            string guide = File.ReadAllText(Path.Combine(repo, "docs/books/nova-cli-guide/chapters/nova-cli.md"));

            StringAssert.Contains(program, "DoDocsShowcaseDemo(repo, [])",
                "sync-payloads must rebuild the canonical showcase before deployment.");
            StringAssert.Contains(program,
                "RunScp(host, showcaseImage, \"/data/nova/disks/demos/supernova.ndi\")",
                "deploy-linux-host must install SuperNova in the OSD demos folder.");
            StringAssert.Contains(program,
                "VerifyRemoteFileSha256(host, showcaseImage, \"/data/nova/disks/demos/supernova.ndi\")",
                "deploy-linux-host must reject a stale or incomplete remote showcase image.");
            StringAssert.Contains(guide, "/data/nova/disks/demos/supernova.ndi");
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
        public void CodegenNdkReferenceWritesPascalByteBindings()
        {
            string temp = Path.Combine(Path.GetTempPath(), "nova-cli-test-" + Guid.NewGuid().ToString("N"));
            Directory.CreateDirectory(temp);
            try
            {
                string repo = FindRepoRoot();
                string bindings = Path.Combine(temp, "pascal");
                RunNova("codegen", "ndk-reference",
                    "--runtime-dir", Path.Combine(repo, "software/runtime/asm"),
                    "--tex", Path.Combine(temp, "reference.tex"),
                    "--json", Path.Combine(temp, "reference.json"),
                    "--pascal-dir", bindings);

                string rng = File.ReadAllText(Path.Combine(bindings, "RNG.NPI"));
                string fio = File.ReadAllText(Path.Combine(bindings, "FIO.NPI"));
                string nova = File.ReadAllText(Path.Combine(bindings, "NOVA.NPI"));
                StringAssert.Contains(rng, "; RNG_GET8\n__S8578E3 = $02");
                StringAssert.Contains(fio, "; FIO_ISSUE\n__S13E4EB = $01");
                StringAssert.Contains(fio, "; FIO_EXEC\n__S34C9C7 = $03");
                Assert.IsFalse(fio.Contains("I_FIO_NAME", StringComparison.Ordinal),
                    "Inline-parameter routines are not ordinary Pascal-callable bindings.");
                StringAssert.Contains(nova, "; FIO_CMD_RNG\n__CBAB93C = 1");
                StringAssert.Contains(RunNova("codegen", "help"), "--pascal-dir");
                Assert.IsFalse(rng.Contains('\r'), "Generated Pascal bindings must use Nova's LF-only text convention.");
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
