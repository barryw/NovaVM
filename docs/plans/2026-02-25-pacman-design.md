# Pac-Man for Nova BASIC — Design

## Overview

A playable Pac-Man clone written entirely in Nova BASIC. Single self-contained program using hardware sprites, bitmap graphics, and the SID sound engine. 20x12 tile grid with 16x16 pixel tiles, 5 hardware sprites (player + 4 ghosts), WASD input, 60 Hz game loop via VSYNC.

## Screen Layout

- **Mode 2** (text over graphics)
- **Top 2 text rows**: `SCORE:000000  HI:000000  LIVES:## LEVEL:##`
- **Bitmap**: 320x192 usable area (rows 8-199), 20x12 tile maze
- Maze fills exactly: 20 tiles x 16px = 320, 12 tiles x 16px = 192

## Tile Grid (20x12)

Each tile is one of: Wall, Pellet, Power Pellet, Empty, Ghost House Door. Maze defined in DATA statements as 12 strings of 20 characters:

```
W = Wall      . = Pellet
O = Power     _ = Empty (ghost house interior)
D = Door      X = Empty (no pellet)
```

### Maze Layout

```
WWWWWWWWWWWWWWWWWWWW
W........WW........W
W.WW.WWW.WW.WWW.WW.W
O.WW.WWW.WW.WWW.WW.O
W....................W
X.WW.WW.WWWW.WW.WW.X   <- tunnel wrap (col 0/19)
W....WW..DD..WW....W
W.WW.WW.W__W.WW.WW.W
W.WW.....W__W....WW.W
W.WW.WWW.WWWW.WW.WW.W
W........X..X........W
WWWWWWWWWWWWWWWWWWWW
```

- **Ghost house**: 2x2 interior (cols 9-10, rows 7-8) with door at cols 9-10 row 6
- **Tunnel**: Row 5, cols 0 and 19 wrap (x=0 <-> x=304)
- **Power pellets**: 4 corners (row 3 cols 0/19, plus two lower corners)
- **Player start**: Tile (10, 10)
- **Ghost start**: Inside ghost house (9-10, 7-8)

Note: Exact layout will need tweaking during implementation to ensure all paths connect and pellet count is correct.

## Sprites and Animation

### Sprite Allocation

| Sprite | Purpose | Shape Slots |
|--------|---------|-------------|
| 0 | Pac-Man | 0-7: two mouth frames x 4 directions |
| 1 | Blinky (red) | 8-11: two wiggle frames x 2 (use FLIPX) |
| 2 | Pinky (pink) | 12-15: same pattern |
| 3 | Inky (cyan) | 16-19: same pattern |
| 4 | Clyde (orange) | 20-23: same pattern |
| -- | Frightened | 24-25: two frames (blue, flashing) |
| -- | Eyes (eaten) | 26-29: four directions |

~30 shape slots total. All defined in DATA statements, loaded via SPRITEDATA loops at startup.

### Animation Rates

- Pac-Man mouth: toggle every 4 frames (15 Hz)
- Ghost wiggle: toggle every 8 frames
- Frightened flash: toggle every 4 frames in last 2 seconds
- Eaten ghosts: eyes-only shape facing movement direction

### Colors

- Pac-Man: yellow body, black eye
- Ghosts: red / pink / cyan / orange body, white eyes, blue pupils
- Frightened: blue body, pink face
- All color baked into shape data (4-bit per pixel)

## Movement and Collision

### Tile-Aligned Movement

- Sprites move 4 pixels per frame (4 frames to cross one 16px tile)
- Direction changes only at tile boundaries
- Input is buffered: press WASD early, direction taken when next tile boundary reached
- At each tile boundary: try buffered direction first, then continue current direction, else stop

### Wall Checking

- Maze stored in DIM M(19,11) array, loaded from DATA at startup
- Check `M(tx,ty)` before entering a tile — Wall and Door (for Pac-Man) block movement

### Pellet Eating

- At tile center, check `M(tx,ty)`
- Pellet (`.`): score +10, set to `X`, erase dot from bitmap
- Power (`O`): score +50, set to `X`, erase pellet, trigger frightened mode
- Track pellet count — when 0, level complete

### Ghost Collision

- `COLLISION(0)` each frame returns bitmask of sprites overlapping Pac-Man
- Bits 1-4 = ghosts
- Chase/Scatter ghost: lose a life
- Frightened ghost: eat ghost, score +200/400/800/1600 (doubles per ghost eaten per power pellet)

### Tunnel Wrap

- If sprite x < 0, set x = 304; if x > 304, set x = 0

## Ghost AI

### State Machine

Each ghost has one of 4 states:

| State | Behavior | Transition |
|-------|----------|------------|
| Scatter | Head toward assigned corner | Timer -> Chase |
| Chase | Pursue Pac-Man (unique per ghost) | Timer -> Scatter |
| Frightened | Wander randomly, edible | Timer -> previous mode |
| Eaten | Eyes return to ghost house | Reaching door -> Scatter |

Scatter and Chase alternate on a global timer. Power pellet overrides all Chase/Scatter ghosts to Frightened.

### Direction Choosing (at tile boundary)

1. List valid exits (not wall, not reverse of current direction)
2. Calculate straight-line distance from each exit to target tile
3. Pick the exit with shortest distance

### Chase Targeting (per ghost)

- **Blinky** (red): Target = Pac-Man's current tile. Direct pursuit.
- **Pinky** (pink): Target = 4 tiles ahead of Pac-Man's facing direction. Ambush.
- **Inky** (cyan): Random valid direction at intersections. Unpredictable.
- **Clyde** (orange): If >8 tiles from Pac-Man, target = Pac-Man. If <=8, target = scatter corner. Shy.

### Scatter Corners

- Blinky: top-right
- Pinky: top-left
- Inky: bottom-right
- Clyde: bottom-left

## Sound Design

### Instruments

| Slot | Sound | Waveform | ADSR | Purpose |
|------|-------|----------|------|---------|
| 0 | Waka | Pulse | Fast A, short D, no S, fast R | Pellet eating |
| 1 | Power | Triangle | Med A, long S, slow R | Power pellet |
| 2 | Eat ghost | Pulse | Fast sweep up | Ghost eaten |
| 3 | Death | Noise | Long D, no S | Pac-Man dies |
| 4 | Siren | Sawtooth | Long S | Background loop |

### Sound Triggers

- **Pellet eat**: `SOUND 60,3,0` alternating with `SOUND 64,3,0` for waka-waka
- **Power pellet**: `SOUND 72,30,1`
- **Ghost eaten**: `SOUND 80,8,2`
- **Death**: `SOUND 40,60,3`
- **Background siren**: MML on voice 1 with `MUSIC LOOP ON`, tempo increases per level
- **Level start**: Short jingle via MML, game waits for `PLAYING=0`

SFX on voice 2 (non-blocking), music on voice 1. No conflicts.

## Game Flow

```
INIT
  +-- Define instruments, load sprite shapes from DATA
  +-- Draw maze from DATA into bitmap
  +-- Set Mode 2, display score/lives
  +-- Play start jingle, wait for PLAYING=0

LEVEL START
  +-- Reset sprite positions (Pac-Man at 10,10; ghosts in house)
  +-- Reset ghost states to Scatter
  +-- Reset scatter/chase timer
  +-- Brief "READY!" text, 2-second pause

GAME LOOP (60 Hz via VSYNC)
  +-- GET key -> buffer direction
  +-- Move Pac-Man (4px/frame, check walls at tile boundary)
  +-- Check pellet/power at Pac-Man's tile
  +-- Update ghost mode timer (scatter<->chase toggle)
  +-- Move each ghost (choose direction at tile boundary)
  +-- Update sprite positions
  +-- Check COLLISION(0) -> death or eat ghost
  +-- Animate sprites (frame counter mod 4/8)
  +-- Update score display if changed
  +-- If pellets=0 -> level complete
  +-- VSYNC, loop

DEATH
  +-- Freeze 1 second
  +-- Play death sound
  +-- Lives -= 1
  +-- If lives=0 -> GAME OVER
  +-- Else -> LEVEL START (maze intact, remaining pellets stay)

LEVEL COMPLETE
  +-- Flash maze 3 times
  +-- Level += 1, adjust difficulty
  +-- Redraw maze with all pellets
  +-- LEVEL START

GAME OVER
  +-- Display "GAME OVER" centered
  +-- Wait for keypress
  +-- Restart
```

### Key Variables

- `SC` (score), `HI` (high score), `LV` (lives=3), `LN` (level number)
- `PX,PY` (Pac-Man pixel), `PT,PU` (Pac-Man tile), `PD` (direction), `BD` (buffered input)
- `GX(4),GY(4),GT(4),GU(4),GD(4),GS(4)` — ghost pixel/tile/direction/state arrays
- `PC` (pellet count), `FT` (fright timer), `MT` (mode timer)
- `M(19,11)` — maze tile array
- `WK` — waka toggle (alternates pellet eat pitch)

## Difficulty Progression

| Level | Ghost Speed | Fright Duration | Scatter Duration | Chase Duration |
|-------|-------------|-----------------|------------------|----------------|
| 1 | 2 px/frame | 6 sec (360 frames) | 7 sec | 20 sec |
| 2 | 2 px/frame | 4 sec | 5 sec | 20 sec |
| 3 | 4 px/frame | 3 sec | 4 sec | 25 sec |
| 4+ | 4 px/frame | 2 sec | 3 sec | 30 sec |

Ghost speeds (2 and 4) divide evenly into 16px tiles. Pac-Man always moves at 4 px/frame. Difficulty ramps through faster ghosts, shorter fright, and more chase time.

## Estimated Program Size

- Maze DATA: ~20 lines
- Sprite DATA: ~250 lines (30 shapes x 8 rows)
- Init/drawing: ~80 lines
- Game loop: ~100 lines
- Ghost AI: ~80 lines
- Sound/score: ~30 lines
- Game flow (death, level, game over): ~40 lines
- **Total: ~600 lines of BASIC**

Well within Nova BASIC's capacity.
