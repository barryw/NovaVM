# PixelCanvasKit

A standalone Swift package containing the **platform-agnostic indexed-pixel
editing engine** shared across Walker Heavy Industries' retro authoring tools
(NovaDraw today; FamiForge and Miggy Draw next). Phase 0 of the shared-editor
convergence (WAL-35, from the WAL-20 spike).

> **Working name.** `PixelCanvasKit` is an approved working name; the final
> product name is a brand call (Quill/CEO). Renaming the package/repo later is
> mechanical.

## What's in here

The engine extracted from NovaDraw, with the platform/product specifics raised
to injectable seams:

| Area | Type(s) | Notes |
|------|---------|-------|
| Document model | `PixelDocument`, `PixelSnapshot`, `CanvasImage`, `BackdropPlacementMode` | Indexed 4-bit pixel buffers + painted mask, up to 12 image slots, undo/redo, dirty tracking. `@Observable @MainActor`. |
| Tools | `ToolEngine`, `DrawingTool` | Pencil, eraser, line/rect/circle (Bresenham/midpoint), flood fill, eyedropper, floating-selection move/copy/cut/paste. |
| Viewport | `CanvasViewport` | Zoom/pan geometry (no rendering). |
| Palette | `Palette`, `PaletteColor` | **De-hardcoded.** Replaces NovaDraw's static `NovaPalette`. Colours + names + nearest-index + AppKit/SwiftUI mapping, all injected. `Palette.nova` ships the legacy C64-derived 16-colour table as data. |
| IO (neutral) | `GfxFormat` | Raw one-byte-per-pixel codec. |
| Seams | `DocumentCodec`, `Exporter` | Protocols for container persistence and target export. NovaDraw's `.novadraw`/`.nvg` and ca65 exporter conform to these and stay in the app. |

### What deliberately stays in the consuming app

SwiftUI views (`PixelCanvasView`, inspectors, sheets), the `.novadraw`/`.nvg`
container codecs, the ca65 assembly exporter, the trace-sheet renderer, and the
`NSImage`-based palette quantizer — these are product- or platform-specific and
conform to the seams above rather than living in the kit.

## Requirements

- Swift 6.0 (Swift 6 language mode), macOS 14+.
- Uses AppKit / SwiftUI / Observation, so it builds on **macOS only** — there is
  no Linux build. CI runs on `macos-15` / Xcode 16 (see `.github/workflows`).

## Usage (consumer)

```swift
// Package.swift
.package(url: "https://github.com/<org>/PixelCanvasKit.git", from: "0.1.0"),
// target deps:
.product(name: "PixelCanvasKit", package: "PixelCanvasKit"),
```

```swift
import PixelCanvasKit

let doc = PixelDocument(width: 320, height: 200)
let tools = ToolEngine(document: doc)
let palette = Palette.nova            // or any injected target palette
tools.mouseDown(x: 10, y: 10, shift: false)
let swatch = palette.swiftUIColor(Int(doc.getPixel(10, 10)))
```

## Status

This is the Phase-0 extraction. **Done is defined as: NovaDraw builds and its
test suite passes against PixelCanvasKit.** That final wiring (NovaDraw consumes
the package via SPM, the duplicate sources are removed) is tracked as the
follow-up consumer-rewire issue.

The API is **not yet stable** — it is declared stable only after FamiForge lands
as the second adopter (per the WAL-35 plan).

> **CI / repo note:** the macOS workflow activates once this directory is its own
> repository. It is staged here under `NovaVM/PixelCanvasKit/` pending repo
> creation (a CEO action — Forge's token cannot create repositories).
