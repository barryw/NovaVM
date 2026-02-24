# e6502

This is a fully implemented Motorola 6502 emulation written in C#.

## Runtime Timing Controls

The emulator now runs from a single cycle clock source.

- `NOVA_CPU_HZ`: target CPU speed in cycles/second. Default: `12000000`.
- `NOVA_TURBO`: if set (`1`, `true`, `yes`, `on`), disables real-time pacing and runs as fast as possible.
- `NOVA_TIMING_LOG`: if set, emits once-per-second timing telemetry.

Examples:

```bash
NOVA_CPU_HZ=12000000 dotnet run --project e6502.Avalonia
NOVA_TURBO=1 dotnet run --project e6502.CLI
NOVA_TIMING_LOG=1 NOVA_CPU_HZ=12000000 dotnet run --project e6502.Avalonia
```
