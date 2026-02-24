---
title: SPRITE
type: command
category: Sprites
keywords: [sprite, enable, position, flip, priority]
syntax: "SPRITE n,ON|OFF | SPRITE n,x,y | SPRITE n,FLIPX|FLIPY,ON|OFF | SPRITE n,PRIORITY,p"
see_also: [spritedata, spriteshape, spriteset, collision, bumped]
---

Controls sprite properties: enable/disable, position, flipping, and priority.

## Syntax Variants

| Form | Description |
|------|-------------|
| `SPRITE n,ON` | Enable sprite n |
| `SPRITE n,OFF` | Disable sprite n |
| `SPRITE n,x,y` | Set sprite position |
| `SPRITE n,FLIPX,ON` | Enable horizontal flip |
| `SPRITE n,FLIPY,ON` | Enable vertical flip |
| `SPRITE n,PRIORITY,p` | Set render priority (0-2) |

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| n | 0-15 | Sprite index |
| x | 0-319 | Horizontal position |
| y | 0-199 | Vertical position |
| p | 0-2 | Priority layer (0=behind, 1=between, 2=front) |

## Examples

::: tryit
```basic
10 SPRITE 0,ON
20 SPRITE 0,160,100
30 SPRITE 0,PRIORITY,2
40 SPRITE 0,FLIPX,ON
```
:::

## Notes

- Sprites must have shape data defined via `SPRITEDATA` or `SPRITESHAPE` to be visible
- Position coordinates use the 320x200 graphics resolution
- See the Sprites guide for a full overview
