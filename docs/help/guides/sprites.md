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

The first command enables sprite 0. The second sets its position to x=100, y=80. Sprite coordinates use the full half-resolution video plane. In 720x480 output, X is 0-359 and Y is 0-239, including the border area; the centered 640x400 canvas starts at sprite coordinate 20,20.

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

Read full 16-sprite collision masks with the `SPRCOLL` and `SPRBG` functions.
Bit 0 is sprite 0, bit 8 is sprite 8, and bit 15 is sprite 15:

```basic
10 VSYNC
20 C = SPRCOLL
30 B = SPRBG
40 IF (C AND 1) <> 0 THEN PRINT "SPRITE 0 HIT ANOTHER SPRITE"
50 IF (C AND 256) <> 0 THEN PRINT "SPRITE 8 HIT ANOTHER SPRITE"
60 IF (B AND 32768) <> 0 THEN PRINT "SPRITE 15 HIT BACKGROUND"
70 GOTO 10
```

For interrupt-driven sprite-sprite collision handling from BASIC, set a handler
with `ON SPRITE COLLISION GOSUB line`, read `SPRCOLL` inside the handler, and
finish with `RETIRQ`:

```basic
10 ON SPRITE COLLISION GOSUB 1000
20 REM GAME LOOP
30 GOTO 20
1000 C = SPRCOLL
1010 IF (C AND 1) <> 0 THEN PRINT "SPRITE 0 COLLIDED"
1020 IF (C AND 512) <> 0 THEN PRINT "SPRITE 9 COLLIDED"
1030 RETIRQ
```

::: note
`SPRCOLL` and `SPRBG` clear their latched masks after reading them. Store the
returned value if you need to check multiple bits.
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
