---
title: GCOLOR
type: command
category: Graphics
keywords: [color, graphics, draw, palette]
syntax: "GCOLOR c"
see_also: [line, plot, rect, circle, fill, color]
---

Sets the current graphics drawing color for subsequent graphics commands.

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| c | 0-15 | Color index from the 16-color palette |

## Examples

```basic
MODE 2
GCOLOR 1 : LINE 0,0,319,199
GCOLOR 5 : CIRCLE 160,100,50
```

## Notes

- Affects `PLOT`, `LINE`, `RECT`, `CIRCLE`, `FILL`, and `PAINT`
- Does not affect text color (use `COLOR` for that)
- Color 0 is the background color
