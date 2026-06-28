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
- Graphic characters can also be written through the VDC-style VRAM port at `$A0E0`--`$A0E4`
- Return to the default font anytime with `FONT 0`

## Inline Style Codes

Beyond the printable graphics, the VGC interprets a handful of **control bytes**
as it scans text out. Embed them in any printed string — every language
ultimately just prints bytes — and the styling travels with the text, exactly
like PETSCII colour/reverse codes on a Commodore.

| Byte | Effect |
|------|--------|
| `$01` | reset styling to normal (reverse/flash/bold off, char background transparent) |
| `$06` | **bold** — toggle (first occurrence on, next off) |
| `$07` | **flash** — toggle |
| `$0E` | **reverse** — toggle |
| `$10` *n* | foreground colour = *n* (0–15) |
| `$11` *n* | screen background colour = *n* (0–15) |
| `$14` *n* | **character background**: *n* = 0–15 opaque, *n* = 16 transparent |
| `$0C` `$13` `$08` `$0A` `$0D` | clear, home, backspace, line-feed, return |

The three colour codes (`$10`, `$11`, `$14`) consume the **next byte** as their
parameter. The style toggles need no parameter — print the code again to turn
the effect off, or use `$01` to reset everything at once. In assembly these are
named `VGC_CC_RESET`, `VGC_CC_BOLD`, `VGC_CC_FLASH`, `VGC_CC_REVERSE`,
`VGC_CC_FG`, `VGC_CC_BG`, `VGC_CC_CHARBG` in `nova.inc`.

### Character background colour

Every character cell carries its own background. By default it is
**transparent**: the cell shows whatever the screen background is at that
point — a solid colour, or a moving copper gradient — and the character's
background tracks it *live*. Give a character an **opaque** background and that
one cell paints a solid colour, independent of whatever is behind it.

It is fully per-character, so you can mix transparent and opaque backgrounds
freely across the screen. Transparent is the default precisely so that text
printed over a copper background (or any changing background) shows it through
instead of freezing a stale colour at print time.

There are two ways to set it: the **inline code** `$14 n` (travels in a string),
or a **dedicated command** in each language:

| | opaque colour *c* | transparent (default) |
|---|---|---|
| **NovaBASIC** | `CHARBG c` | `CHARBGOFF` |
| **NovaForth** | `c CHARBG` | `CHARBG-OFF` |
| **NovaLogo** | `SETCHARBG c` | `CLEARCHARBG` |
| **Assembly** | `LDX #c` / `JSR vgc_set_charbg` | `JSR vgc_charbg_off` |

### Examples

NovaBASIC — inline codes inside a `PRINT`:

```
10 PRINT CHR$($10);CHR$(2);"RED ";              : REM foreground red
20 PRINT CHR$($0E);"REVERSED";CHR$($0E);        : REM reverse on ... off
30 PRINT CHR$($14);CHR$(6);" BLUE PANEL ";      : REM opaque blue char background
40 PRINT CHR$($14);CHR$(16);"CLEAR AGAIN";      : REM transparent
50 PRINT CHR$($01)                              : REM reset to normal
```

NovaForth — the `vgc.4th` library gives one-word helpers (`INK`, `PAPER`,
`REVERSE`, `BOLD`, `FLASH`, `PLAIN`) plus `CHARBG` / `CHARBG-OFF`:

```forth
INCLUDE forth/lib/nova/vgc.4th
2 INK    S" RED " TYPE
REVERSE  S" HI " TYPE  REVERSE
6 CHARBG S" PANEL " TYPE  CHARBG-OFF
PLAIN
```

NovaLogo:

```
SETCHARBG 6   PRINT [BLUE PANEL]
CLEARCHARBG   PRINT [TRANSPARENT AGAIN]
```
