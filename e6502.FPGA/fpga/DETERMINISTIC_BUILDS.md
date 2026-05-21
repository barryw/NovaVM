# Deterministic FPGA Builds

The production ULX3S flow must fail on real routed timing. No-route placement
timing is useful for diagnostics, but it is not authoritative enough to block a
bitstream on this design.

## Default Timing Gate

`tools/beast-synth.sh bitstream <label>` runs the full route and checks
`build/nextpnr-report.json`. That routed report is the production timing
authority.

`make placecheck` remains available as an early diagnostic. It uses nextpnr
`--no-route`, parses the placement Fmax lines from `build/nextpnr-place.log`,
and fails if any clock misses its constraint plus `PLACE_TIMING_MARGIN_MHZ`
(default `0.5`). Treat a placecheck failure as a warning to investigate, not as
proof that a routed bitstream cannot pass.

Useful overrides:

```sh
PLACE_TIMING_GATE=1 tools/beast-synth.sh bitstream strict-placegate
PLACE_TIMING_MARGIN_MHZ=0.0 tools/beast-synth.sh placecheck no-margin
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
NOVA_FLOORPLAN_ENFORCE=host_io,sdram_edge tools/beast-synth.sh placecheck edge-test
```

## Seed Sweeps

Use seed sweeps to measure robustness after the placement gate passes, not to
hunt for a lucky bitstream:

```sh
BEAST_CLEAN=0 NEXTPNR_SEEDS="1 2 3 4 5" tools/beast-synth.sh seed-sweep timing-sweep
```

If one seed passes and others fail, the design is still not deterministic.
