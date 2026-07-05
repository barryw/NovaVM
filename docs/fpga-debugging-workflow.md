# FPGA Debugging Workflow

This project has enough moving parts that "build a bitstream and stare at HDMI"
should be the last step, not the first step. Use the cheapest tool that can
answer the question.

## The Ladder

1. **Unit test in Verilator**

   Use this when the question is pure logic: register decode, command FSM state,
   sprite publish timing, BRAM read latency, FIFO behavior, math, DMA address
   generation.

   ```bash
   make -C e6502.FPGA/test test_vgc_sprites
   make -C e6502.FPGA/test test_vgc_frame_counter
   ```

2. **Waveform trace with GTKWave**

   Use this when the test fails or the behavior is timing-sensitive and the
   console output is not enough. This answers "what signal changed on which
   clock?"

   ```bash
   make -C e6502.FPGA/test test_vgc_sprites TRACE=1
   gtkwave e6502.FPGA/test/build/test_vgc_sprites/test_vgc_sprites.fst
   ```

   Useful knobs:

   ```bash
   make -C e6502.FPGA/test test_vgc_sprites TRACE=1 TRACE_DEPTH=3
   make -C e6502.FPGA/test test_vgc_sprites TRACE=1 TRACE_MAX_ARRAY=256
   ```

3. **Diagnostic synthesis**

   Use this when the question is resource mapping or whether Yosys inferred the
   thing you think it inferred. This is much cheaper than full place-and-route.

   ```bash
   make -C e6502.FPGA/boards/ulx3s diag
   make -C e6502.FPGA/boards/ulx3s diag-vgc
   make -C e6502.FPGA/boards/ulx3s diag-sprites
   ```

4. **Full bitstream**

   Use this only when the question depends on hardware timing, routing, HDMI,
   SDRAM pins, ESP/FPGA UART, reset behavior, or the physical board.

   ```bash
   make -C e6502.FPGA/boards/ulx3s bitstream
   openFPGALoader --board ulx3s -f e6502.FPGA/boards/ulx3s/build/e6502.bit
   ```

5. **Hardware diagnostic through NovaHost**

   Use the ESP debug bridge for repeatable board tests. Port `6503` speaks
   line-delimited JSON: `type_text`, `send_key`, `read_screen`, `peek`, `poke`,
   `cold_start`.

   For BASIC-visible behavior, prefer typing real BASIC commands through the
   bridge instead of poking registers. That tests the same path users exercise.

   Run live-board debug checks through the Nova CLI:

   ```bash
   dotnet run --project e6502.Nova -c Release -- check spi-bridge --remote 192.168.1.188
   dotnet run --project e6502.Nova -c Release -- check vgc-reset-stale --remote 192.168.1.188
   dotnet run --project e6502.Nova -c Release -- vm --remote 192.168.1.188 screen
   ```

   Use the static IP for normal hardware testing. mDNS (`novahost.local`) is
   convenient when it works, but it is not reliable enough for the standard test
   path.

   The hardware debug bridge also exposes interactive commands on TCP port
   `6503`:

   ```json
   {"command":"dbg_break_set","address":49152,"enabled":1}
   {"command":"dbg_break_set","slot":0,"address":49152,"enabled":1}
   {"command":"dbg_break_list"}
   {"command":"dbg_break_clear","address":49152}
   {"command":"dbg_break_clear","slot":0}
   {"command":"dbg_break_clear_all"}
   {"command":"dbg_step"}
   {"command":"dbg_trace","count":64}
   ```

   Trace records are oldest-to-newest and include PC, address bus, data
   in/out, A, SP, flags, CPU micro-state, opcode, RDY, WE, IRQ, and NMI.

6. **Exact NovaVM capture**

   Use this first when the question is what NovaVM is outputting after the OSD
   compositor. It reads the standard V4L2 capture device on the Arty and writes
   a PNG locally.

   ```bash
   dotnet run --project e6502.Nova -c Release -- capture screen screenshots/hardware/current.png --remote 192.168.1.188
   dotnet run --project e6502.Nova -c Release -- capture devices --remote 192.168.1.188
   dotnet run --project e6502.Nova -c Release -- capture record /tmp/novavm.mkv --remote 192.168.1.188 --duration 30
   ```

   The video source is `/dev/video0` (V4L2). The audio source is `hw:NovaVM,0`
   (ALSA). Remote recordings are copied back to the local output path after
   ffmpeg finishes on the board. Capture-card mode configures standard UVC/UAC2
   gadget endpoints; verify streaming on hardware with `nova capture devices`
   and standard Linux capture tools. The native V4L2 pixel format is `bgr0`.

7. **External HDMI frame capture**

   Use this when memory/debug reads are not enough and the question is what the
   monitor or an external capture device sees.

   ```bash
   dotnet run --project e6502.Nova -c Release -- capture hdmi
   dotnet run --project e6502.Nova -c Release -- capture hdmi screenshots/hardware/current.png
   dotnet run --project e6502.Nova -c Release -- capture hdmi --list
   ```

   The default backend is `avfoundation` on macOS and `v4l2` elsewhere. On
   Linux the default device is `/dev/video0`; on the current Mac the default is
   AVFoundation device `0:none`, which is the Cam Link 4K. Override either if
   device ordering changes:

   ```bash
   HDMI_BACKEND=v4l2 HDMI_DEVICE=/dev/video0 dotnet run --project e6502.Nova -c Release -- capture hdmi
   HDMI_DEVICE='1:none' dotnet run --project e6502.Nova -c Release -- capture hdmi
   ```

## What GTKWave Is Good For

GTKWave is the right tool when you need causality:

- Did `spr_next_x` publish to `spr_x` at the intended frame boundary?
- Did `shape_publish_fire` happen during visible display?
- Did `cmd_busy` block a sprite publish?
- Did a BRAM read capture happen one clock too early?
- Did reset release before counters and HDMI encoder were aligned?

GTKWave is not the right tool for:

- monitor/capture-card cadence conversion,
- physical signal integrity,
- timing closure after place-and-route,
- ESP WiFi or SD-card behavior.

## Useful VGC Signals

Start with these when debugging display/sprite behavior:

- `h_count`, `v_count`
- `visible`, `vid_de`, `vid_hsync`, `vid_vsync`
- `frame_counter`, `vblank_start`, `sprite_frame_publish`
- `cmd_busy`, `cmd_op`
- `spr_next_x`, `spr_next_y`, `spr_x`, `spr_y`
- `spr_next_enable`, `spr_enable`
- `shape_publish_fire`, `shape_sync_busy`, `active_shape_bank`
- `spr_pixel_hit`, `spr_pixel`, `spr_pixel_pri`

For text bugs, add:

- `cursor_x`, `cursor_y`
- `char_a_addr`, `char_a_din`, `char_a_we`
- `color_a_addr`, `color_a_din`, `color_a_we`
- `text_col`, `text_row`, `font_line`, `font_pixel`

For command bugs, add:

- CPU address/write signals around `$A000-$B99F`
- `regs[16]` / `VGC_CMD`
- command parameter registers `regs[17]..regs[31]`
- command-specific FSM state and memory port signals.

## Reading A Waveform

1. Add clocks and resets first.
2. Add the trigger signal: for sprites, usually `sprite_frame_publish` or
   `cmd_op`.
3. Add the "before" state and "after" state. Example: `spr_next_x[0]` and
   `spr_x[0]`.
4. Add the write source. Example: CPU write decode or command FSM assignment.
5. Measure clock edges, not wall-clock intuition.

If the trace is huge, do not trace the whole machine longer. Write a smaller
test that drives only the subsystem and stops right after the event.

## Common FPGA Failure Modes

- **BRAM latency:** reads are usually valid one clock after the address is set.
- **Simulation vs synthesis mismatch:** uninitialized regs, inferred memories,
  and async resets can differ between Verilator and ECP5 hardware.
- **Timing closure:** Verilator says logic is correct; nextpnr says whether the
  routed hardware can meet the clock.
- **Clock-domain crossings:** pixel, TMDS shift, SDRAM, UART, and ESP-facing
  paths need explicit synchronization or a proven handshake.
- **Read-modify-write hazards:** sprite shape bytes and packed pixels need a
  deliberate two/three-cycle sequence.
- **Mid-frame visible writes:** anything the renderer reads during active video
  needs buffering, vblank publication, or a raster-aware design.

## Rule Of Thumb

If you can express the bug as "given these inputs, these RTL signals should do
X within N clocks", write a Verilator test and use GTKWave if needed.

If the bug depends on "what does the actual board, monitor, ESP, or SDRAM chip
do", run hardware diagnostics and expect a full bitstream.
