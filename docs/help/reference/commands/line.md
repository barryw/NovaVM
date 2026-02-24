---
title: LINE
type: command
category: Graphics
keywords: [line, draw, graphics, straight]
syntax: "LINE x1,y1,x2,y2"
see_also: [plot, rect, circle, gcolor]
---

Draws a straight line between two points using the current graphics color.

## Parameters

| Parameter | Description |
|-----------|-------------|
| x1, y1 | Start point (0-319, 0-199) |
| x2, y2 | End point (0-319, 0-199) |

## Examples

::: tryit
```basic
10 MODE 2 : GCOLOR 1
20 LINE 0,0,319,199
30 LINE 319,0,0,199
```
:::

## Notes

- Requires graphics mode (`MODE 2` or `MODE 3`)
- Color is set by the most recent `GCOLOR` call
- Coordinates outside the screen are clipped
