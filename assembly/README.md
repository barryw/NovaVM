# NovaVM Assembly Applications

This tree contains standalone 65C02 applications that are not part of the
NovaBASIC ROM or extension ROM. Shared libraries live in `runtime/asm`; these
applications include those libraries through the `assembly/Makefile` include
path.

Build all standalone apps:

```sh
make -C assembly
```

Current apps:

| App | Load address | Purpose |
| --- | --- | --- |
| `apps/keyboard` | `$9000` | Piano keyboard visualizer for SID/MIDI/WTS playback. |
| `apps/demo` | `$7200` | Demo disk music browser/player. |
| `apps/vsprite_rotate` | `$7200` | 64x64 virtual-sprite hardware rotation demo driven by arrow keys. |
| `apps/turtle` | `$7200` | 16x16 white turtle virtual-sprite rotation demo driven by arrow keys. |
| `apps/editui_shell` | `$7200` | Visual smoke demo for the shared Nova TUI shell: CP437 frames, marked menu hotkeys, title band, panels, and bottom shortcut bar. |
