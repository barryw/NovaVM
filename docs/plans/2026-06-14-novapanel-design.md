# NovaPanel — Avalonia remote control for Nova hardware

**Date:** 2026-06-14
**Status:** Design approved, ready for implementation plan

## Goal

A standalone Avalonia desktop app that connects to NovaHost running on FPGA/ESP32
hardware and gives full parity with the existing web admin page, plus the one thing
the web has never had: a **visible on-screen keyboard** that lets you click keys or
type physically while it highlights the keys you press.

The window opens just big enough to show the keyboard and grows/shrinks like a web
page — panels appear and collapse at responsive breakpoints. It is a plain
resizable window meant to sit next to a QuickTime capture of Nova's HDMI output so
you can see what's running while you type into it.

## Constraints / confirmed decisions

- **New project** `e6502.NovaPanel` in the existing solution (not bolted into
  `e6502.Avalonia`, which is the local emulator GUI).
- **Full keyboard** — every key the firmware actually accepts (verified below).
- **Standalone window** — user places it next to QuickTime themselves. No window
  automation.
- **Full web parity** — drives, runtime, status, library, modules, wifi, audio.

## Two wire protocols (both reused, not reinvented)

NovaHost speaks two independent TCP protocols. The app uses both, tracked
separately so one can be down while the other works.

| Concern | Port | Wire format | Existing code |
|---|---|---|---|
| Mount/unmount, runtimes, status, files, reset, reboot, wifi, audio | 6504 | binary `NVH1` header + CBOR | `NovaHostManagementClient.cs` (e6502.Nova) |
| Keyboard (`type_text`, `send_key`), screen/debug | 6503 | newline-delimited JSON | pattern from `EmulatorClient.cs` (e6502.MCP) |

## Architecture

### Project layout

```
e6502.NovaHost   (NEW shared library, net10.0)
  ├─ NovaHostManagementClient.cs   ← MOVED from e6502.Nova (6504 CBOR)
  ├─ CborLite, NovaTransferProgress, NovaFileChunk, NovaHostCommandException (moved)
  └─ NovaDebugClient.cs            ← NEW (6503 newline-JSON: type_text / send_key)

e6502.Nova       → references e6502.NovaHost (drops the moved file; CLI unchanged)
e6502.NovaPanel  (NEW Avalonia 12.0.2 app, net10.0)
  → references e6502.NovaHost + e6502.Storage (NovaModule, for the modules panel)
```

**Why a shared library, not a project-reference to e6502.Nova:** e6502.Nova is an
Exe that pulls DryWetMidi and CLI plumbing. The management client is already 100%
self-contained (only `System.Net.Sockets` / `System.Text.Json` / `System.Buffers`
+ `e6502.Storage`). Extracting it is the correct fix — reuse without duplication —
and the CLI keeps working by referencing the moved type.

**No MVVM framework** — code-behind + UserControls, matching the `e6502.Avalonia`
convention.

### Connection model

One host field (e.g. `novahost.local` or `192.168.1.65`). Management client →
`:6504`, debug client → `:6503`. Status polled on a `DispatcherTimer` (~2s,
configurable). Last host + window size/position persisted to
`~/.e6502/novapanel.json`.

## On-screen keyboard (centerpiece)

`KeyboardView` UserControl. Real keyboard layout as a grid of key buttons. Two
input paths, one dispatch:

1. **Click a key** → send.
2. **Type physically** (window focused) → capture `KeyDown`/`KeyUp` → send + flash
   the matching on-screen key.

### Verified firmware key vocabulary

From `e6502.ESP32/novahost/debug_server.cpp` `cmdSendKey` (and mirrored in
`e6502.Avalonia/Ipc/EmulatorTcpServer.cs`). **There are no function keys** — F1–F12
are not accepted, so they are omitted from the layout (no dead keys).

Named tokens (case-insensitive): `ENTER`/`CR`/`RETURN` (0x0D), `BACKSPACE`/`BS`
(0x08), `TAB` (0x09), `ESC`/`ESCAPE` (0x1B), `SPACE` (0x20), `LEFT` (0x1C), `RIGHT`
(0x1D), `UP` (0x1E), `DOWN` (0x1F), `HOME` (0x02), `END` (0x05), `PGUP` (0x10),
`PGDN` (0x12), `CTRL-HOME` (0x80), `CTRL-END` (0x81), `DELETE`/`DEL` (0x7F),
`CTRL-C`/`BREAK` (0x03), `SCREEN-HOME`/`VGC-HOME` (0x13).

Dynamic: `ALT-<letter>` → ESC + lowercase letter; `CTRL-<letter>` / `CONTROL-<letter>`
→ 0x01–0x1A; any single printable ASCII (0x20–0x7E) → passed through.

`type_text` streams printable ASCII and converts `\n` → `\r`.

### Layout (ANSI-ish, F-row dropped)

```
Esc                                              [Break = Ctrl-C]
` 1 2 3 4 5 6 7 8 9 0 - =  Backspace
Tab  Q W E R T Y U I O P [ ] \
     A S D F G H J K L ; ' Enter
Shift Z X C V B N M , . /  Shift
Ctrl Alt  ───Space───  Alt Ctrl   |  Home End  ↑     PgUp
                                   |       ←  ↓  →   PgDn  Del
```

### Modifiers

Sticky toggles: click = latch, physical hold = held.
- **Shift** → resolves shifted ASCII locally (`a`→`A`, `1`→`!`); sends one byte via
  `send_key`.
- **Ctrl** + letter → `CTRL-<L>`. **Alt** + letter → `ALT-<L>`.
- Dedicated **Break** key → `CTRL-C`.

### Dispatch map (Avalonia `Key` + modifiers → protocol token)

- Letters/digits/punctuation → ASCII char (shift-resolved).
- Enter→`ENTER`, Back→`BACKSPACE`, Tab→`TAB`, Esc→`ESC`, Space→`SPACE`, Del→`DELETE`.
- Arrows→`UP/DOWN/LEFT/RIGHT`, Home→`HOME`, End→`END`, PgUp→`PGUP`, PgDn→`PGDN`.
- Ctrl+Home→`CTRL-HOME`, Ctrl+End→`CTRL-END`.
- Send via `NovaDebugClient.SendKeyAsync(token)`; runs of printable chars may batch
  via `type_text`.

### Highlight + paste

Each key is a button with a `.active` pseudo-class. Physical keydown flashes the
cell, fade ~120ms on keyup; click animates the same. Dead keys render dimmed and
non-interactive. Ctrl+V (host) → `type_text` of the whole clipboard.

The keyboard is always visible — it is the minimum window size.

## Responsive layout (webpage-style breakpoints)

Avalonia has no CSS media queries, so a `ResponsiveController` subscribes to
`Window.Bounds`, maps width → a `SizeClass` enum, and panels bind `IsVisible` to it.
The keyboard cell has a fixed minimum; all extra space goes to panels — the keyboard
never shrinks.

```
COMPACT  (start, ~keyboard width):
   ┌─────────────────────────────┐
   │ connbar: host ● status chips │   ← always
   ├─────────────────────────────┤
   │        KEYBOARD             │   ← always
   └─────────────────────────────┘
   collapsed panels reachable via "≡" chips → overlay/flyout

WIDE  (+ horizontal room): status + drives + runtime dock as a LEFT rail
   ┌──────────┬──────────────────┐
   │ status   │  connbar         │
   │ drives   ├──────────────────┤
   │ runtime  │  KEYBOARD        │
   └──────────┴──────────────────┘

TALL / FULL  (+ vertical room): library + modules + disk-images stack BELOW keyboard
```

Mechanism: outer `Grid`; the rail column and bottom row animate width/height
`0`→`Auto` driven by `SizeClass`; panels are `IsVisible`-bound. Each collapsible
panel also has a manual chip toggle so it can be force-shown as an overlay in COMPACT.

Min-width = keyboard width; min-height = keyboard + connection bar.

## Management panels (full web parity)

Each is a UserControl fed by the polled `GetStatus` and directory listings. Every
action already exists as a client method (verified).

- **ConnBar** (always): host textbox, Connect/Disconnect, status chips (bootPhase,
  SD, FPGA bridge, WiFi, audio), VM Reset, Host Reboot (reboot behind confirm).
- **StatusPanel**: expanded lamps from `health` / `sdStatus` / `wifi` / `audio` —
  the same six the web shows, color-coded.
- **DrivesPanel**: rows fd0–3, hd0–1 from `status.drives`. Mounted badge,
  currentPath, configuredPath. Mount (disk picker) / Recall (`MountDriveAsync(slot,
  null)`) / Unmount.
- **DiskImagesPanel**: `ListDirectory("disks/floppy")` + `disks/hard`. name / size /
  bootStatus. Mount→slot picker, Delete (`DeletePathAsync`), Upload `.ndi` via
  `WriteFileAsync` with progress.
- **RuntimePanel**: parse `bootConfig.languages` → list with rom / extensionRom;
  active = `defaultRuntime`. Set Active (`RuntimeSetAsync`), Add (`RuntimeAddAsync`),
  Remove (`RuntimeRemoveAsync`, disabled when active).
- **LibraryPanel**: kind selector (roms / soundfonts / music / boot / sid) →
  `ListDirectory`. name / size, Delete, Upload.
- **ModulesPanel**: `ListDirectory("lib")` → read each → `NovaModule.Parse` →
  name / id / version / fnCount / staged / idMismatch. Functions button → searchable
  modal. Toggle staged (boot.json edit), Delete.

Uploads run off the UI thread with progress; writes refresh status afterward.

## Errors, threading, connection state

- UI never blocks. Poll loop awaits off-thread, marshals back via the dispatcher.
- Keyboard sends are queued so fast typing doesn't race the socket (the debug client
  is semaphore-guarded, like `EmulatorClient`).
- States: Disconnected / Connecting / Connected / Error, reflected in chips. On drop:
  keyboard greys, panels dim with a "stale" badge, auto-reconnect with 1→5s backoff.
- The two sockets are independent — keyboard works even if status polling fails, and
  vice versa.
- Every client call returns `{ok,error}`; failures surface as toast/inline, never
  silent. Reboot / Reset / Delete behind confirm dialogs.

## Testing (each test fails without its code)

- **e6502.NovaHost**: `NovaDebugClient` framing against a fake TCP server — exact
  bytes `{"command":"send_key","key":"ENTER"}\n` per token; `type_text` batching and
  `\n`→`\r`. Key-map table test: every `Key`+modifier → expected token. CborLite
  frame encode/decode round-trip.
- **KeyboardView** (Avalonia.Headless): physical keydown highlights the right cell;
  shift resolves `1`→`!`; Ctrl+letter → `CTRL-A`; click dispatches the same token as
  the physical key.
- **ResponsiveController**: width → SizeClass thresholds; panel `IsVisible` flips at
  each breakpoint (pure logic, no render).
- **Smoke**: headless build + launch; assert window min-size equals keyboard size.

No hardware required — fake servers + headless rendering.

## Out of scope (v1)

- Window automation / launching QuickTime (standalone window only).
- Runtime "package" install wizard (use Upload ROM + Add for now).
- Screen mirroring inside the app (QuickTime handles the video).
