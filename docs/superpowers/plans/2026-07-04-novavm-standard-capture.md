# NovaVM Standard Capture Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Expose exact NovaVM HDMI output and final mixed audio through standard Linux capture interfaces. USB gadget capture-card mode remains a separate completion gate until the UVC/UAC2 bridge daemon is implemented and verified on hardware.

**Architecture:** RTL taps post-OSD RGB and post-mix PCM, writes rings into reserved PS DDR, and exposes capture control/status through a dedicated AXI-lite register range. A Linux platform driver exposes those rings as V4L2 video capture and ALSA PCM capture. A configfs helper prepares UVC/UAC2 gadget endpoints, but host-side USB capture still needs a bridge daemon before it can be treated as complete. Nova CLI consumes `/dev/videoN` and ALSA devices.

**Tech Stack:** SystemVerilog, Vivado block design Tcl, Zynq PS DDR over HP0, Linux V4L2/videobuf2, ALSA PCM, USB configfs UVC/UAC2, Buildroot/PetaLinux packaging, .NET Nova CLI, MSTest, Verilator.

---

## File Map

- Create `e6502.FPGA/boards/arty_z7/hdmi_capture.sv`: post-OSD video and post-mix audio capture writer.
- Create `e6502.FPGA/boards/arty_z7/axi_lite_demux2.sv`: routes PS GP0 AXI-lite to FIO or capture registers.
- Modify `e6502.FPGA/boards/arty_z7/osd_overlay.sv`: expose aligned active-video signal with `rgb_out`.
- Modify `e6502.FPGA/boards/arty_z7/arty_z7_full.sv`: insert capture block, demux GP0, wire second HP0 SmartConnect input.
- Modify `e6502.FPGA/boards/arty_z7/build_full_bd.tcl`: widen GP0 external range and expose a second HP0 SmartConnect slave for capture writes.
- Modify `e6502.FPGA/boards/arty_z7/build_full.tcl`: include new RTL files.
- Modify `e6502.FPGA/boards/arty_z7/linux/br2-external/board/novavm/arty_z7/novavm-arty-z7.dts`: reserve capture DDR and add `novavm,capture`.
- Modify `e6502.FPGA/boards/arty_z7/linux/dts/novavm-arty-z7-pl.dtsi`: mirror capture reserved memory and device node.
- Modify `e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-bsp/device-tree/files/system-user.dtsi`: mirror capture reserved memory and device node.
- Create `e6502.FPGA/boards/arty_z7/linux/novacap/novacap.c`: kernel platform driver exposing V4L2 and ALSA capture.
- Create `e6502.FPGA/boards/arty_z7/linux/novacap/Makefile`: out-of-tree kernel module build.
- Create `e6502.FPGA/boards/arty_z7/linux/br2-external/package/novacap/Config.in` and `novacap.mk`: Buildroot package.
- Create `e6502.FPGA/boards/arty_z7/petalinux/meta-user/recipes-kernel/novacap/novacap.bb`: PetaLinux module recipe.
- Pending: create `e6502.FPGA/boards/arty_z7/linux/novacapd/novacapd.c`: UVC/UAC2 bridge daemon.
- Pending: create package/recipe entries for `novacapd`.
- Modify `e6502.Nova/Program.cs`: add `nova capture screen|record|devices|gadget`.
- Modify `docs/fpga-debugging-workflow.md` and `docs/books/nova-cli-guide/chapters/nova-cli.md`: document standard capture.
- Modify `e6502UnitTests/NovaCliCoverageTests.cs`: enforce CLI/docs/RTL/Linux contract.
- Create Verilator tests under `e6502.FPGA/test/` for video capture and back-pressure behavior.

## Task 1: Contract Tests

**Files:**
- Modify `e6502UnitTests/NovaCliCoverageTests.cs`

- [ ] **Step 1: Write failing test**

Add a test that asserts:

```csharp
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
```

- [ ] **Step 2: Verify red**

Run:

```bash
rtk dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release -m:1 --filter FullyQualifiedName~NovaCliCoverageTests.StandardCaptureUsesV4l2AlsaAndUsbGadgetInterfaces -v:minimal
```

Expected: fail because `capture screen`, `hdmi_capture`, and `novavm,capture` are missing.

## Task 2: RTL Video/Audio Capture Core

**Files:**
- Create `e6502.FPGA/boards/arty_z7/hdmi_capture.sv`
- Create `e6502.FPGA/test/test_hdmi_capture.sv`

- [ ] **Step 1: Write failing Verilator test**

Create a test that drives `active=1` for a small synthetic frame, feeds known RGB words and PCM samples, accepts AXI writes immediately, and checks the writer emits aligned BGRX32-in-memory video words plus stereo PCM words into distinct address ranges.

- [ ] **Step 2: Verify red**

Run the existing FPGA test target pattern for the new test. Expected: fail because `hdmi_capture.sv` does not exist.

- [ ] **Step 3: Implement core**

Implement `hdmi_capture` with:

- `VIDEO_BASE = 32'h1100_0000`
- `AUDIO_BASE = 32'h1180_0000`
- BGRX32 video writes
- stereo PCM packed as `{right,left}`
- ring sequence counters
- AXI burst write state machine
- drop-on-overflow counters instead of scanout back-pressure

- [ ] **Step 4: Verify green**

Run the Verilator test and confirm it passes.

## Task 3: RTL Integration

**Files:**
- Modify `e6502.FPGA/boards/arty_z7/osd_overlay.sv`
- Create `e6502.FPGA/boards/arty_z7/axi_lite_demux2.sv`
- Modify `e6502.FPGA/boards/arty_z7/arty_z7_full.sv`
- Modify `e6502.FPGA/boards/arty_z7/build_full.tcl`
- Modify `e6502.FPGA/boards/arty_z7/build_full_bd.tcl`

- [ ] **Step 1: Add aligned OSD active signal**

Add `output reg active_out` to `osd_overlay` and assign it beside `rgb_out` from `de_2`.

- [ ] **Step 2: Add AXI-lite demux**

Route PS GP0 addresses below `0x40010000` to `fio_bridge` and addresses at `0x40010000..0x4001ffff` to capture registers.

- [ ] **Step 3: Integrate capture**

Instantiate `hdmi_capture` in `arty_z7_full.sv` between `osd_overlay` and `hdmi`, using `osd_rgb_out`, `osd_active_out`, `audio_sample_strobe`, and `hdmi_audio_mixed`.

- [ ] **Step 4: Update BD**

Change SmartConnect `NUM_SI` from `1` to `2`, expose `S_AXI_CAPTURE`, and map both external HP masters to DDR.

- [ ] **Step 5: Build RTL syntax**

Run:

```bash
rtk /tools/Xilinx/Vivado/2024.2/bin/vivado -mode batch -source build_full_bd.tcl
rtk /tools/Xilinx/Vivado/2024.2/bin/vivado -mode batch -source build_full.tcl
```

Expected: bitstream build reaches implementation without unresolved ports.

## Task 4: Device Tree And Kernel Packaging

**Files:**
- Modify Buildroot, common, and PetaLinux device trees listed in File Map
- Create `e6502.FPGA/boards/arty_z7/linux/novacap/`
- Create Buildroot and PetaLinux package files

- [ ] **Step 1: Add reserved capture memory**

Reserve `0x11000000..0x11ffffff` as `novavm_capture` with `no-map`.

- [ ] **Step 2: Add capture node**

Add:

```dts
novavm_capture: capture@40010000 {
    compatible = "novavm,capture";
    reg = <0x40010000 0x00010000>;
    memory-region = <&novavm_capture_mem>;
};
```

- [ ] **Step 3: Add kernel package**

Package `novacap.ko` so it is installed and loaded on boot.

## Task 5: Linux V4L2/ALSA Driver

**Files:**
- Create `e6502.FPGA/boards/arty_z7/linux/novacap/novacap.c`

- [ ] **Step 1: Write driver skeleton**

Bind `compatible = "novavm,capture"`, map registers, map reserved memory, register one V4L2 device and one ALSA card.

- [ ] **Step 2: Implement V4L2 capture**

Expose BGRX32 at 720x480. Use videobuf2 mmap buffers. Standard userspace tooling can convert to RGB24 or YUYV when needed.

- [ ] **Step 3: Implement ALSA capture**

Expose stereo 48 kHz S16_LE capture. Copy available PCM ring data into ALSA capture periods.

- [ ] **Step 4: Build module**

Run the Arty Linux build through the Nova CLI, not ad hoc scripts:

```bash
rtk dotnet run --project e6502.Nova -c Release -- arty build-linux-host
```

Expected: `novacap.ko` builds and is included in the image/rootfs.

## Task 6: USB Capture-Card Mode

Status: configfs helper exists as `novacap-gadget`; the bridge daemon below is not implemented in this pass and must not be described as working until hardware-verified.

**Files:**
- Create `e6502.FPGA/boards/arty_z7/linux/novacapd/novacapd.c`
- Add Buildroot/PetaLinux packaging
- Add init/service config

- [ ] **Step 1: Add gadget config command path**

Configure configfs with UVC YUYV 720x480 30000/1001 and UAC2 stereo 48 kHz S16_LE.

- [ ] **Step 2: Add bridge daemon**

Read from NovaVM `/dev/videoN`, write frames to the UVC gadget video-output node, read `hw:NovaVM,0`, and feed the UAC2 gadget endpoint.

- [ ] **Step 3: Add mode switch**

Normal mode keeps USB host keyboard/mouse. Capture-card mode switches USB0 to gadget mode and starts `novacapd`.

## Task 7: Nova CLI Capture Commands

**Files:**
- Modify `e6502.Nova/Program.cs`
- Modify CLI guide and debugging docs

- [ ] **Step 1: Implement `capture devices`**

List local or remote `/dev/video*` and ALSA capture devices.

- [ ] **Step 2: Implement `capture screen`**

Open V4L2 through ffmpeg and request one BGRX32 frame as PNG.

- [ ] **Step 3: Implement `capture record`**

Invoke `ffmpeg` with V4L2 and ALSA inputs and fail loudly if `ffmpeg` is missing.

- [ ] **Step 4: Implement `capture gadget`**

Use SSH through existing Nova CLI remote command helpers to enable, disable, or query capture-card mode.

- [ ] **Step 5: Verify CLI tests**

Run:

```bash
rtk dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release -m:1 --filter FullyQualifiedName~NovaCliCoverageTests.StandardCaptureUsesV4l2AlsaAndUsbGadgetInterfaces -v:minimal
```

Expected: pass.

## Task 8: Hardware Proof

**Files:**
- Build artifacts only

- [ ] **Step 1: Synthesize and flash**

Run the Nova CLI or documented Vivado flow. Do not use ad hoc scripts.

- [ ] **Step 2: Deploy Linux host/rootfs**

Use Nova CLI deploy commands.

- [ ] **Step 3: Capture exact screenshot**

Run:

```bash
rtk dotnet run --project e6502.Nova -c Release -- capture screen /tmp/arty-hdmi.png --remote 192.168.1.188
```

Expected: PNG shows the exact current HDMI output including OSD if OSD is visible.

- [ ] **Step 4: Capture standard A/V**

Run on the Arty or over SSH:

```bash
ffmpeg -f v4l2 -video_size 720x480 -framerate 60000/1001 -pixel_format bgr0 -i /dev/video0 \
       -f alsa -i hw:NovaVM,0 /tmp/novavm-capture.mkv
```

Expected: file records video and audio with no external HDMI capture device.

## Self-Review

- Spec coverage: video, audio, local screenshot, standard recording, and USB capture-card mode are covered.
- Placeholder scan: no placeholder tasks remain; every task has a file path and observable verification.
- Type consistency: docs use `hdmi_capture`, `novavm,capture`, `/dev/video0`, and `hw:NovaVM,0` consistently.
