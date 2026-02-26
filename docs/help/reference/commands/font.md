---
title: "FONT"
type: command
category: Display
keywords: [font, petscii, character set, charset, c64]
syntax: "FONT n"
see_also: [graphics-and-display]
---

Switches the active character font for the entire screen.

## Syntax

```
FONT n
```

**n** -- Font slot number (0-7):

| Slot | Font | Description |
|------|------|-------------|
| 0 | CP437 | DOS/IBM character set (default) |
| 1 | PETSCII Upper | C64 uppercase + graphic characters |
| 2 | PETSCII Lower | C64 lowercase + uppercase |

## Examples

```basic
10 FONT 1
20 PRINT "HELLO IN PETSCII"
30 FONT 0
```

## Keyboard Graphics

When a PETSCII font is active (slots 1 or 2), special key combinations produce graphic characters:

- **Shift + A-Z** -- Shifted graphic characters (line-drawing, shapes)
- **Ctrl + A-Z** -- Commodore-key graphic characters (additional shapes)

See the [PETSCII Characters](/docs/help/guides/petscii-characters.md) guide for the full character map.

## Notes

- Font affects display only -- stored program data is unchanged
- Works with POKE too: `POKE 40967,1` (40967 = $A007)
- The BASIC prompt and all text output use the active font
