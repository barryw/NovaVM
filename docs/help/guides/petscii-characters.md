---
title: "PETSCII Characters"
type: guide
category: Display
keywords: [petscii, characters, graphics, keyboard, c64, commodore, font]
---

# PETSCII Character Map

The PETSCII character sets reproduce the Commodore 64's graphical characters. Activate them with `FONT 1` (uppercase/graphics) or `FONT 2` (lowercase/uppercase).

## Typing Graphic Characters

When a PETSCII font is active, these key combinations produce graphic characters:

### Shift + Key (codes $80-$99)

| Key | Code | Key | Code | Key | Code |
|-----|------|-----|------|-----|------|
| Shift+A | $80 | Shift+J | $89 | Shift+S | $92 |
| Shift+B | $81 | Shift+K | $8A | Shift+T | $93 |
| Shift+C | $82 | Shift+L | $8B | Shift+U | $94 |
| Shift+D | $83 | Shift+M | $8C | Shift+V | $95 |
| Shift+E | $84 | Shift+N | $8D | Shift+W | $96 |
| Shift+F | $85 | Shift+O | $8E | Shift+X | $97 |
| Shift+G | $86 | Shift+P | $8F | Shift+Y | $98 |
| Shift+H | $87 | Shift+Q | $90 | Shift+Z | $99 |
| Shift+I | $88 | Shift+R | $91 | | |

### Ctrl + Key (codes $A0-$B9)

| Key | Code | Key | Code | Key | Code |
|-----|------|-----|------|-----|------|
| Ctrl+A | $A0 | Ctrl+J | $A9 | Ctrl+S | $B2 |
| Ctrl+B | $A1 | Ctrl+K | $AA | Ctrl+T | $B3 |
| Ctrl+C | $A2 | Ctrl+L | $AB | Ctrl+U | $B4 |
| Ctrl+D | $A3 | Ctrl+M | $AC | Ctrl+V | $B5 |
| Ctrl+E | $A4 | Ctrl+N | $AD | Ctrl+W | $B6 |
| Ctrl+F | $A5 | Ctrl+O | $AE | Ctrl+X | $B7 |
| Ctrl+G | $A6 | Ctrl+P | $AF | Ctrl+Y | $B8 |
| Ctrl+H | $A7 | Ctrl+Q | $B0 | Ctrl+Z | $B9 |
| Ctrl+I | $A8 | Ctrl+R | $B1 | | |

**Note:** Ctrl+V is reserved for paste. Use `PRINT CHR$($B5)` instead.

## Using in Programs

```basic
10 FONT 1
20 REM Print a horizontal line using shifted graphic chars
30 FOR I=1 TO 40
40 PRINT CHR$($82);
50 NEXT I
60 PRINT
```

## Character Sets

**Font 1 (Upper/Graphics):** Letters are uppercase only. The shifted positions contain the classic C64 line-drawing and geometric shapes.

**Font 2 (Lower/Upper):** Regular keys produce lowercase, Shift produces uppercase. Graphic characters are available via Ctrl+key.

## Tips

- Use `CHR$()` with hex codes to place specific graphic characters: `PRINT CHR$($80)`
- Graphic characters can also be POKEd directly into screen RAM at $AA00-$B1CF
- Return to the default font anytime with `FONT 0`
