# Multi-Font System for Nova BASIC

**Date:** 2026-02-26

## Motivation

Nova BASIC targets learning and play, not production use. The most compelling showcase programs are small games — like those in *Commodore 64 Fun and Games* (Jeffries, Fisher, Sawyer, 1984). Those programs relied on PETSCII graphical characters for visual charm. Games like ROADHOG, SHEEP, REVERSI, and DIVE created their entire visual language from keyboard graphics. Porting or reimagining those programs in Nova requires support for character sets beyond the current CP437 (DOS) font.

## Design

### Font Slot Infrastructure

The VGC holds up to 8 font slots (indices 0-7). Each slot is a 2048-byte bitmap font: 256 characters × 8 bytes per character, 8×8 pixels per glyph.

Built-in fonts shipped with Nova:

| Slot | Font | Description |
|------|------|-------------|
| 0 | CP437 | DOS/IBM — current default, always present |
| 1 | PETSCII Upper/Graphics | C64 character set 1: uppercase + graphic symbols |
| 2 | PETSCII Lower/Upper | C64 character set 2: lowercase + uppercase |

Additional fonts (Atari, ZX Spectrum, Amstrad CPC, etc.) fit the same format without architecture changes.

### VGC Register

`RegFont` at address `$A007` (currently `RegBank`, reserved and unused). Low 3 bits store the active font index (0-7). Upper bits reserved.

### BASIC Keyword

`FONT n` switches the active font globally. `FONT 0` returns to CP437. Implementation follows the same pattern as `COLOR` and `MODE`: a new EhBASIC token that writes the argument to `$A00B`.

### BitmapFont Changes

`BitmapFont` grows from a single `byte[2048]` to a `byte[8][]` array of font slots. Fonts load at construction from embedded resources. Empty slots fall back to CP437 glyphs. `GetRow(charCode, y)` becomes `GetRow(fontIndex, charCode, y)`, where `fontIndex` comes from the VGC state.

### Renderer Changes

`EmulatorCanvas.RenderFramebuffer` already calls `_font.GetRow(ch, glyphY)`. The only change: pass the active font index from the VGC state snapshot. No other rendering logic changes.

### Keyboard Mapping

Each font slot has an optional keymap: a table mapping (modifier + keypress) to character code. Slots without a keymap (including CP437) behave exactly as today.

PETSCII keymap behavior:

| Input | Output |
|-------|--------|
| Unmodified A-Z | Normal letters (uppercase in set 1, lowercase in set 2) |
| Shift + A-Z | Graphic characters from the upper range (hearts, diamonds, line-drawing, etc.) — matches C64 keyboard layout |
| Ctrl + A-Z | Remaining graphic characters (equivalent to C64 Commodore key) |

`ScreenEditor` checks the active font's keymap when translating keypresses. No keymap means no behavior change.

Keymap data lives as a companion structure per font slot, loaded alongside the font binary.

### Help Panel Character Map

When a keymap font is active (any font other than CP437), the help panel automatically displays a character map grid. The grid shows all graphic characters and their key combinations, grouped by modifier:

```
Shift+A  ♥    Shift+B  ░    Shift+C  ─  ...
Ctrl+A   ┼    Ctrl+B   ▓    Ctrl+C   │  ...
```

Switching to `FONT 0` returns the help panel to its default state. This is implemented as a new `HelpTopic` entry per font (e.g., "PETSCII Characters"). The help panel already supports dynamic content.

### Screen Editor Interaction

The font affects display only. The BASIC interpreter stores the same byte values in program memory regardless of the active font. A program written with PETSCII `PRINT` statements works regardless of which font was active during editing — the font determines how bytes render, not what bytes are stored.

## What Does Not Change

- Screen RAM format (`$AA00-$B1CF`)
- Color RAM format (`$B1D0-$B99F`)
- Character dimensions (8×8 pixels, rendered at 8×16 on screen)
- Screen size (80×25)
- All existing BASIC programs
- Rendering pipeline structure
- Scroll behavior

## Out of Scope

- Per-character font mixing (global switch only)
- Custom font loading from BASIC (no `FONT LOAD "file.bin"` — built-in fonts only)
- Character map picker UI (help panel grid covers discoverability)
- Variable font sizes (all fonts are 8×8)
- PETSCII control code emulation (C64 embedded cursor/color codes in strings — Nova uses `LOCATE`, `COLOR`, etc.)

## Deliverables

1. Font slot infrastructure in `BitmapFont` (8-slot `byte[8][]` array)
2. `RegFont` VGC register at `$A00B`
3. `FONT` BASIC keyword (new EhBASIC token)
4. Font binaries: CP437 (slot 0), PETSCII Upper/Graphics (slot 1), PETSCII Lower/Upper (slot 2)
5. PETSCII keymap for Shift+key and Ctrl+key graphic character entry
6. Help panel character map topic (auto-displayed when keymap font is active)
7. MCP `SetFont` tool

## Testing

| Test | Verification |
|------|-------------|
| Font rendering | Correct glyph data for each slot |
| Font switching | `FONT` command changes active rendering |
| Keymap | Modifier+key produces correct character code per font |
| Fallback | Empty slots render CP437 glyphs |
| Help panel | Character map topic appears and disappears with font switch |
| Regression | All existing programs render identically with `FONT 0` |
