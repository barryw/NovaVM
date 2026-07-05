# NovaVM Standard Capture Design

## Goal

NovaVM must expose the exact HDMI output as standard Linux capture devices, so agents and users can inspect, screenshot, record, and stream the machine without an external HDMI capture dongle.

## Non-Negotiable Interfaces

- Video is exposed as V4L2 capture at `/dev/videoN`.
- Audio is exposed as ALSA PCM capture as `hw:NovaVM,0`.
- USB capture-card mode targets UVC video plus UAC2 audio through Linux USB gadget configfs.
- Nova CLI consumes the standard capture devices. It does not use a Nova-only screen transport as the primary hardware screenshot path.
- RTL captures raw pixels and PCM only. PNG, video containers, and USB class framing are Linux/userspace work.

## Capture Points

Video capture taps the final active pixel stream after `osd_overlay` and before the HDMI encoder in `arty_z7_full.sv`.

```text
VGC RGB -> OSD overlay -> capture tap -> HDMI encoder -> TMDS pins
```

This includes OSD menus, dimming, mouse pointer, text, graphics, sprites, border, and any final compositor artifact visible on HDMI.

Audio capture taps `hdmi_audio_mixed` after the final PS PCM plus SID mix and before the HDMI encoder.

```text
PS PCM + RTL SID mix -> hdmi_audio_mixed -> capture tap -> HDMI encoder
```

## Raw Data Format

The RTL frame writer stores active video only: 720x480 pixels, no blanking.

Primary RTL frame-buffer format is BGRX32 in CPU-visible little-endian memory:

- 4 bytes per pixel: B, G, R, X
- 720 * 480 * 4 = 1,382,400 bytes per frame
- Frame cadence follows the HDMI mode, 60000/1001 Hz

The Linux V4L2 driver advertises `V4L2_PIX_FMT_XBGR32`, which is B,G,R,X byte order in memory. RGB24/YUYV conversion belongs in standard userspace tooling such as ffmpeg unless a measured need appears.

Audio is stereo signed 16-bit little-endian at 48 kHz:

- ALSA format: `S16_LE`
- Channels: 2
- Rate: 48000

## RTL Contract

Add a small PL capture block with:

- pixel clock input
- reset input
- active RGB24 input from `osd_rgb_out`
- active video valid derived from the same post-OSD active timing used by HDMI
- PCM sample input from `hdmi_audio_mixed`
- AXI master writes into reserved PS DDR frame buffers
- AXI-lite control/status registers reachable from Linux

The video writer uses a small FIFO between pixel capture and AXI bursts. It writes whole frames to a ring of DDR buffers and publishes completed frame index plus sequence counter. It must never back-pressure HDMI scanout.

The audio writer uses a small FIFO between the 48 kHz sample strobe and AXI bursts. It writes PCM frames to a DDR ring and publishes available byte counts. It must never starve or alter HDMI audio.

## Linux Kernel Contract

Add a NovaVM capture platform driver bound by device tree.

The driver owns:

- the capture AXI-lite register map
- reserved capture DDR memory
- one V4L2 video capture node
- one ALSA PCM capture device

The V4L2 node supports:

- `V4L2_CAP_VIDEO_CAPTURE`
- `V4L2_CAP_STREAMING`
- `V4L2_PIX_FMT_XBGR32`, 720x480
- mmap streaming buffers through videobuf2

The ALSA node supports:

- capture only
- `S16_LE`
- 2 channels
- 48000 Hz
- mmap/read capture

## USB Capture-Card Mode

The Arty Z7 PS USB controller cannot be USB host and USB device at the same time. NovaVM therefore has two explicit boot/runtime modes:

- normal mode: USB host keyboard/mouse enabled; V4L2/ALSA capture available on the Arty Linux side and over network tools
- capture-card mode: USB gadget enabled; UVC/UAC2 exported to a PC; input comes through Ethernet/debug/serial paths

Capture-card mode uses standard Linux gadget configfs. The configfs helper is in tree; the userspace bridge must exist and be hardware-verified before claiming host-side USB webcam/microphone streaming works:

- UVC function advertises YUYV 720x480 at 30000/1001 for reliable USB high-speed capture
- UAC2 function advertises stereo 48 kHz S16_LE
- a small userspace bridge feeds UVC frames from `/dev/videoN` and ALSA samples from `hw:NovaVM,0`

The UVC 30 fps mode is the default because USB high-speed bandwidth is tight for uncompressed 720x480 at 60 fps. Local V4L2 still exposes the full-rate source.

## Nova CLI Contract

Add a standard capture command:

```text
nova capture screen [out.png] [--device /dev/videoN] [--format bgr0|rgb24|yuyv]
nova capture record <out.mkv> [--video /dev/videoN] [--audio hw:NovaVM,0]
nova capture devices
nova capture gadget enable|disable|status --remote <host>
```

`nova capture screen` grabs one V4L2 frame through ffmpeg and writes PNG.

`nova capture record` shells out to `ffmpeg`; when used with `--remote`, it records on the board to a temporary file and copies the recording back to the requested local path.

The existing debug `nova vm screenshot` remains a diagnostic fallback and must be documented as approximate because it reconstructs layers from VGC state instead of sampling final HDMI output.

## Verification

Acceptance requires:

- Verilator test proves the capture block writes a known post-OSD RGB pattern to frame memory.
- RTL test proves capture does not stall scanout when the DDR writer is busy.
- Linux driver smoke test reports `/dev/videoN` capabilities and captures one frame.
- ALSA smoke test records one second of stereo 48 kHz audio.
- Nova CLI test verifies `capture screen` consumes V4L2, not debug VRAM.
- Hardware proof captures the current Arty HDMI output as PNG through V4L2.
- Standard tool proof records A/V with:

```bash
ffmpeg -f v4l2 -video_size 720x480 -framerate 60000/1001 -pixel_format bgr0 -i /dev/video0 \
       -f alsa -i hw:NovaVM,0 out.mkv
```

## References

- Linux V4L2 capture API: https://docs.kernel.org/userspace-api/media/v4l/dev-capture.html
- Linux USB gadget configfs: https://docs.kernel.org/usb/gadget_configfs.html
- Linux UVC gadget: https://docs.kernel.org/usb/gadget_uvc.html
- ALSA driver API: https://docs.kernel.org/sound/kernel-api/writing-an-alsa-driver.html
