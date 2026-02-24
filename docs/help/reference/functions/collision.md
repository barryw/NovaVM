---
title: COLLISION
type: function
category: Sprites
keywords: [collision, sprite, hit, detect]
syntax: "COLLISION(n)"
see_also: [bumped, sprite, spritex, spritey]
---

Returns the sprite-sprite collision status for sprite n.

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| n | 0-15 | Sprite index to check |

## Return Value

Non-zero if sprite n collided with another sprite since the last read; 0 otherwise.

## Examples

```basic
100 C = COLLISION(0)
110 IF C THEN PRINT "HIT!"
```

## Notes

- The collision flag is cleared after reading
- Collision detection happens once per frame during rendering
- Only enabled sprites with non-transparent overlapping pixels register collisions
