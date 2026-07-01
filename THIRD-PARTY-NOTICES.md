# Third-Party Notices — NovaVM

NovaVM's own code is licensed **MIT** (see [`License.txt`](License.txt)). It
incorporates the third-party components below, each of which **retains its own
license**. Relicensing NovaVM to MIT does not, and cannot, change the license of
these components — they are used as isolated subcomponents (test fixtures,
separately-built ROMs, or vendored hardware cores), not linked into NovaVM's
managed code.

## Origin

- **amensch/e6502** — MIT. NovaVM began as a fork of this 6502 emulator core.
  <https://github.com/amensch/e6502>

## NuGet dependencies (all permissive; none copyleft)

Avalonia and Avalonia.* (MIT), Dock.Avalonia / Dock.Model.Mvvm (MIT),
Melanchall.DryWetMidi (MIT), Microsoft.Extensions.Hosting (MIT),
ModelContextProtocol (MIT), SixLabors.ImageSharp (Six Labors Split License /
Apache-2.0), MSTest / xUnit / coverlet / Microsoft.NET.Test.Sdk (MIT /
Apache-2.0). None are GPL, so none impose copyleft on NovaVM.

## Bundled test fixtures

- **Klaus Dormann 6502/65C02 functional test suite**
  (`e6502UnitTests/Resources/6502_functional_test.*`,
  `6502_interrupt_test.*`, `65C02_extended_opcodes_test.*`) —
  **GPLv3**, Copyright (C) 2013 Klaus Dormann. Used only as unit-test input;
  not linked into or distributed as part of the NovaVM product binary
  (mere aggregation). Retains GPLv3.

## Bundled 6502 software (separate ROM artifacts)

- **EhBASIC (Enhanced BASIC)** by Lee Davison (`software/languages/ehbasic/`) — "free but not
  copyright free." Non-commercial use requires that any binary image derived
  from it include the string "Derived from EhBASIC" and that human-readable
  distributions carry that acknowledgment. Assembled into a separate ROM image,
  not linked into NovaVM's managed code. Retains its own license; contact
  Lee Davison for commercial terms.

## Vendored FPGA / HDL cores (`e6502.FPGA/rtl/`)

These are synthesized into the FPGA **bitstream** artifact, which is a separate
build output from NovaVM's software. Their licenses attach to that bitstream.

- **redip_sid** (`rtl/vendor/redip_sid/`) — **CERN-OHL-S v2** (Strongly
  Reciprocal). Any distributed hardware/bitstream that includes this SID core
  must comply with CERN-OHL-S reciprocity for that artifact. This obligation
  applies to the FPGA build output regardless of NovaVM's MIT license.
- **hdl-util-hdmi** (`rtl/thirdparty/hdl-util-hdmi/`) — dual Apache-2.0 / MIT.
- **usb_hid_host**, **usb_hid_host_fs** (`rtl/thirdparty/`) — Apache-2.0.

---

_Recorded 2026-06-30 as part of the WAL-21 licensing convergence (NovaVM
GPLv3 → MIT), CEO-approved. If any component is added or removed, update this
file._
