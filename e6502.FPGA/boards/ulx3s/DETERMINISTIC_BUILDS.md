# Deterministic FPGA Builds

The production ULX3S flow must fail on real routed timing. No-route placement
timing is useful for diagnostics, but it is not authoritative enough to block a
bitstream on this design.

## Default Timing Gate

`make -C e6502.FPGA/boards/ulx3s bitstream` runs the full route and writes
`build/nextpnr-report.json`. Check that routed report with
`nova fpga check-timing`; it is the production timing authority.

`make placecheck` remains available as an early diagnostic. It uses nextpnr
`--no-route`, parses the placement Fmax lines from `build/nextpnr-place.log`,
and fails if any clock misses its constraint plus `PLACE_TIMING_MARGIN_MHZ`
(default `0.5`). Treat a placecheck failure as a warning to investigate, not as
proof that a routed bitstream cannot pass.

Useful overrides:

```sh
make -C e6502.FPGA/boards/ulx3s bitstream
PLACE_TIMING_MARGIN_MHZ=0.0 make -C e6502.FPGA/boards/ulx3s placecheck
```

## Floorplan Modes

`NOVA_FLOORPLAN_MODE` is consumed by `place_hdmi_serializer.py`.

- `critical`: default; enforces only the HDMI serializer/GPDI island.
- `chip` or `edge`: enforces critical plus proven edge neighborhoods.
- `core`: also enforces broad CPU/audio/math/bus regions; tune boxes before
  using this as a default.
- `video`, `aggressive`, `all`: also enforces VGC analysis regions.
- `report` or `off`: creates regions but does not constrain cells.

Extra one-off regions can be enforced with:

```sh
NOVA_FLOORPLAN_ENFORCE=host_io,sdram_edge make -C e6502.FPGA/boards/ulx3s placecheck
```

## Seed Sweeps

Use seed sweeps to measure robustness after the placement gate passes, not to
hunt for a lucky bitstream:

```sh
NEXTPNR_SEEDS="1 2 3 4 5" make -C e6502.FPGA/boards/ulx3s seed-sweep
```

If one seed passes and others fail, the design is still not deterministic.
