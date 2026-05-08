# NovaDraw — Nova Pixel Art Editor

macOS SwiftUI application for creating and editing graphic assets for Nova programs.

## Motivation

The current splash screen was auto-converted from PNG and looks poor. Chess pieces needed for Nova Chess. No existing tool understands Nova's 16-color palette and native asset formats.

## Canvas

Preset canvas sizes at creation time:
- 16x16 (hardware sprite)
- 32x32
- Custom WxH (user enters dimensions, max 320x200)
- 320x200 (full screen)

All canvases use the Nova 16-color palette. Pixel data stored as flat `[UInt8]` array, 1 byte per pixel (0-15).

## Layout

```
┌─────────────────────────────────────────────────┐
│  Toolbar: tool select, zoom, grid toggle, opacity│
├────────┬────────────────────────────┬───────────┤
│        │                            │           │
│ Color  │       Canvas               │  Inspector│
│ Palette│   (scrollable, zoomable)   │  (export, │
│ (16)   │                            │  canvas   │
│        │                            │  info,    │
│        │                            │  backdrop)│
│        │                            │           │
├────────┴────────────────────────────┴───────────┤
│  Status: cursor pos, canvas size, zoom level     │
└─────────────────────────────────────────────────┘
```

## Drawing Tools

| Tool | Key | Behavior |
|------|-----|----------|
| Pencil | P | Click/drag pixels in current color |
| Eraser | E | Sets pixels to color 0 (black/transparent) |
| Line | L | Click start, drag to end, preview while dragging |
| Rect | R | Click corner, drag opposite. Shift = filled |
| Circle | C | Click center, drag radius. Shift = filled |
| Fill | F | Flood fill from click, 4-connected |
| Eyedropper | I | Pick color, auto-returns to previous tool |
| Select | S | Rectangle select. Move, copy, paste, delete |

Color palette: left-click = foreground, right-click = background. Current fg/bg shown at top.

## Zoom & Navigation

- Scroll wheel zoom centered on cursor
- Keys 1-5 for 1x, 2x, 4x, 8x, 16x
- Cmd+0 fits to window
- Space+drag to pan

## Grid

Toggle with G key. Zoom-dependent density:
- Below 4x zoom: major gridlines only (every 8 or 16 pixels, selectable)
- At 4x+ zoom: full pixel grid
- Thin gray lines, major lines slightly darker

## Backdrop Tracing

Load any PNG/JPEG as a backdrop layer. Renders at full opacity beneath the drawing. Drawing layer opacity adjustable via slider (0.0-1.0) so the backdrop shows through. Cmd+B toggles backdrop visibility.

## Export Formats

| Format | Extension | Constraints | Layout |
|--------|-----------|-------------|--------|
| NVG1 | .nvg | 320x200 only | "NVG1" magic + uint16 width + uint16 height + uint32 span_count + spans (uint16 addr, uint8 len, uint8[] pixels). Black omitted. |
| GFX | .gfx | 320x200 only | Raw 64,000 bytes, 1 byte/pixel |
| Sprite | .spr | 16x16 or sheet | 128 bytes/shape, nibble-packed (low=left, high=right). Larger canvases sliced into 16x16 tiles. |
| VSprite | .vspr | Any size | 4-byte header (uint16 width, uint16 height) + raw WxH bytes |

Export buttons gray out when canvas size doesn't match format constraints.

## Import

| Source | Behavior |
|--------|----------|
| PNG/JPEG | Choose: backdrop (trace layer) or quantize to Nova palette |
| .nvg | Decode RLE into 320x200 canvas |
| .gfx | Load raw bytes into 320x200 canvas |
| .spr | Load shape(s) into 16x16 or sheet canvas |
| .vspr | Read header, load pixel data |
| .novadraw | Restore full project state |

PNG-to-palette quantization uses hue-aware mapping matching `tools/convert_novavm_logo.py`.

## Project File (.novadraw)

JSON header + raw pixel payload, separated by newline:

```json
{"version":1,"width":320,"height":200,"backdrop":"path.png","backdropOpacity":0.7,"gridOn":true,"gridMajor":16}
```

Followed by raw pixel bytes (width * height bytes).

## Nova Palette

| Index | Name | RGB |
|-------|------|-----|
| 0 | Black | (0, 0, 0) |
| 1 | White | (255, 255, 255) |
| 2 | Red | (136, 0, 0) |
| 3 | Cyan | (170, 255, 238) |
| 4 | Purple | (204, 68, 204) |
| 5 | Green | (0, 204, 85) |
| 6 | Blue | (0, 0, 170) |
| 7 | Yellow | (238, 238, 119) |
| 8 | Orange | (221, 136, 85) |
| 9 | Brown | (102, 68, 0) |
| 10 | Light Red | (255, 119, 119) |
| 11 | Dark Grey | (51, 51, 51) |
| 12 | Med Grey | (119, 119, 119) |
| 13 | Light Green | (170, 255, 102) |
| 14 | Light Blue | (0, 136, 255) |
| 15 | Light Grey | (187, 187, 187) |
