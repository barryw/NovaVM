---
title: Sprites
type: guide
category: Graphics
keywords: [sprite, animation, collision, shape, movement]
see_also: [sprite, spritedata, spriteshape, spriteset, collision]
---

NovaBASIC supports up to 16 hardware sprites, each 16x16 pixels with 4-bit color (16 colors per sprite). Sprites are rendered by the VGC independently of the text and graphics layers, making them ideal for game characters, projectiles, and other moving objects.

## Enabling and Positioning

Each sprite has an index from 0 to 15. To use a sprite, enable it and set its position:

```basic
SPRITE 0,ON
SPRITE 0,100,80
```

The first command enables sprite 0. The second sets its position to x=100, y=80. Coordinates use the graphics resolution (0-319 for x, 0-199 for y).

## Defining Shapes

Sprites use shape slots (0-255) stored in dedicated shape RAM. Define pixel data with `SPRITEDATA`:

::: tryit
```basic
10 REM SIMPLE ARROW SPRITE
20 SPRITEDATA 0,0,"0000000100000000"
30 SPRITEDATA 0,1,"0000001110000000"
40 SPRITEDATA 0,2,"0000011111000000"
50 SPRITEDATA 0,3,"0000000100000000"
60 SPRITEDATA 0,4,"0000000100000000"
70 SPRITE 0,ON
80 SPRITESHAPE 0,0
90 SPRITE 0,160,100
```
:::

Each `SPRITEDATA` call defines one row (0-15) of a shape slot. The hex string encodes 16 pixels, each being a color index 0-F. Color 0 is transparent by default.

## Priority Layers

Sprites render at one of three priority levels:

| Priority | Constant | Rendering Order |
|----------|----------|-----------------|
| 0 | Behind | Behind text and graphics |
| 1 | Between | Between background and text |
| 2 | Front | In front of everything |

```basic
SPRITE 0,PRIORITY,2
```

## Collision Detection

The VGC detects two types of collisions each frame:

- Sprite-sprite: Two enabled sprites overlap on non-transparent pixels
- Sprite-background: A sprite overlaps a non-zero graphics pixel

Read collisions with the `COLLISION()` and `BUMPED()` functions:

```basic
IF COLLISION(0) THEN PRINT "SPRITE 0 HIT ANOTHER SPRITE"
IF BUMPED(0) THEN PRINT "SPRITE 0 HIT BACKGROUND"
```

::: note
Collision flags are cleared after reading. Read them once per frame and store the result if you need to check multiple times.
:::

## Flipping

Sprites can be flipped horizontally or vertically without modifying shape data:

```basic
SPRITE 0,FLIPX,ON
SPRITE 0,FLIPY,ON
```

::: warning
Flipping applies to the rendered sprite only. The underlying shape data in shape RAM is unchanged.
:::
