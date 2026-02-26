# Fun & Games Showcase: Design Document

**Date:** 2026-02-26

## Goal

Port and reimagine 4 games inspired by *Commodore 64 Fun and Games* (Jeffries, Fisher, Sawyer, 1984) as a curated showcase for Nova BASIC. Each game demonstrates different platform features while staying true to the type-in program spirit: short enough to type by hand, fun enough to keep playing.

## Game Selection

| # | Game | Type | Key Features Showcased |
|---|------|------|----------------------|
| 1 | REVERSI | Strategy/board | PETSCII board art, color RAM, cursor input, basic AI, SID sound |
| 2 | AVALANCHE | Action/dodge | Frame-timed game loop, sprites, collision detection, difficulty ramping, copper effects |
| 3 | SHEEP | Herding/AI | Multi-entity AI, sprite management at scale (14 sprites), timer pressure, emergent behavior |
| 4 | ROADHOG | Scrolling action | Character RAM scrolling, multiple simultaneous systems, fuel mechanic, road generation |

## Common Design Decisions

### Screen Layout
All games redesigned for **80x25 text mode** (not the original C64 40x25). Games use the full width. This changes game balance — wider roads, bigger boards, more playfield — which is intentional.

### Feature Philosophy
**Full Nova showcase.** These aren't strict ports — they're reimaginings that use sprites, SID sound, block graphics, copper effects, and PETSCII fonts wherever it improves the game. The original games are inspiration, not specification.

### Code Conventions
Following existing patterns from `docs/programs/`:
- Line numbers increment by 10
- `REM` comments with `=== SECTION ===` headers for major blocks
- Structure: initialization → data loading → subroutines → game loop → data statements
- Single/double letter variable names with consistent mnemonics
- GOSUB for modular subroutines
- VSYNC for frame synchronization
- Each game is a standalone `.bas` file, no shared libraries

### Target Size
100-300 lines per game. Typeable in an afternoon. Pac-Man (630 lines) is the upper bound of what's reasonable; these should be well under that.

### File Locations
`docs/programs/reversi.bas`, `avalanche.bas`, `sheep.bas`, `roadhog.bas`

---

## Game 1: REVERSI

### Concept
Classic Othello on an 8x8 board. Player vs computer AI. Cursor-based piece placement.

### Display
- PETSCII font (FONT 1) for board grid using box-drawing characters
- Board centered on the 80-column screen
- Left side: captured piece counts per player
- Right side: current player indicator, move number
- Board squares alternate colors via color RAM
- Pieces rendered as filled PETSCII circles, different colors per player

### Input
Arrow keys to move a highlighted cursor across the board. RETURN/SPACE to place a piece. Legal moves shown as dimmed markers so the player knows their options.

### AI
Simple greedy: evaluate each legal move by number of pieces flipped, pick the best. Ties broken randomly. Beatable but not trivial.

### Sound
- Click on piece placement
- Cascading tone on multi-piece captures (pitch rises with chain length)
- Game-over jingle (win vs lose variants)

### Sprites
- Animated cursor highlight (pulsing or color-cycling)
- Optional: flipping animation on captured pieces

### Game Flow
Title screen → choose who goes first → game loop (highlight legal moves → player picks → flip pieces → AI responds → flip pieces) → game over when board full or no moves → show winner with score → play again?

### Estimated Size
~150-200 lines

---

## Game 2: AVALANCHE

### Concept
Player at bottom of screen dodges rocks falling from a mountain/cliff above. Survive as long as possible. Difficulty ramps over time.

### Display
- Mountain/cliff face across top 3-4 rows in PETSCII block characters (FONT 1)
- Full 80-column playfield below the mountain
- Cave walls or scenery framing the left/right edges
- Block graphics for sky gradient or cave texture
- Copper color cycling on the mountain face for lava/shimmer effect

### Input
Left/right arrow keys (or A/D) to move player. Simple and responsive — polled every frame.

### Rocks
Rocks spawn at random positions along the mountain edge. Fall downward with gravity (accelerating). Initial fall speed and spawn rate increase with score. Different rock sizes: small (1 char, fast), large (2 chars, slower but harder to dodge).

### Scoring
Points per frame survived. Bonus for near-misses (rock lands within 1 column). Difficulty ramps every 500 points — more rocks, faster base speed.

### Sprites
- Player character as a sprite (smooth sub-character horizontal movement)
- Rocks as sprites (smooth vertical falling, up to ~12 simultaneous)
- Explosion particles on rock impact with ground

### Sound
- Low rumbling background drone
- Impact thud when rocks hit ground (pitch varies with rock size)
- Whoosh on near-misses
- Death explosion
- Tension music that speeds up with difficulty level

### Game Flow
Title screen with rumbling SFX → 3-2-1 countdown → game loop (spawn rocks, gravity update, player movement, collision detect, score update) → death animation (screen shake?) → high score display → play again?

### Estimated Size
~200-250 lines

---

## Game 3: SHEEP

### Concept
Player controls a sheepdog herding sheep into a pen. Sheep flee from the dog when close, wander randomly when far. Get all sheep penned before time runs out.

### Display
- Top-down farmyard, full 80-column screen
- Fences and pen drawn with PETSCII box-drawing characters (FONT 1)
- Green grass background via color RAM
- Obstacles: trees, rocks, water (different PETSCII characters and colors)
- Pen along one edge with a gate opening
- Countdown timer displayed prominently (top of screen)

### Input
Arrow keys or WASD to move the dog. Continuous movement while key held. Optional: bark button (SPACE) that has a larger flee radius for one frame — strategic tool.

### Sheep AI
Each sheep has a position and state:
- **Flee**: when dog is within flee radius (~5-8 chars), sheep moves directly away from dog
- **Wander**: when dog is far, small random chance of moving each frame
- **Penned**: once inside the pen boundary, sheep stops and stays put
- Sheep don't overlap each other — if movement would collide with another sheep, skip that frame
- This simple behavior creates emergent herding: circle behind the flock and push toward the pen

### Sprites
- Dog: 1 sprite, maybe 2-frame walk animation
- Sheep: up to 12 sprites (start with 8 on level 1, increase per level)
- Total: 13-14 sprites within the 16-sprite limit
- Penned sheep could revert to character-drawn to free up sprite slots for later levels

### Sound
- Dog bark on SPACE press
- Sheep baa when startled (random pitch variation)
- Gentle pastoral melody in background
- Urgency music when timer drops below 30 seconds
- Victory jingle when all sheep penned
- Failure sound when time expires

### Game Flow
Title screen → level 1 (8 sheep, 120 seconds) → herd loop → win: next level (more sheep, less time, more obstacles) → lose: time expires → final score (total sheep penned across levels) → play again?

### Estimated Size
~250-300 lines

---

## Game 4: ROADHOG

### Concept
Drive up a continuously scrolling road, weaving through oncoming traffic. Collect fuel pickups to avoid running dry. Survive as long as possible.

### Display
- Road occupies center ~40 columns, shoulders and scenery on each side
- Road scrolls downward via character RAM shifting (move all rows down, draw new content at top)
- Shoulder scenery in PETSCII: trees, signs, buildings, fences
- Lane markings via color RAM on road surface
- Fuel gauge displayed at top of screen
- Score and speed indicator

### Input
- Left/right: steer across lanes
- Up: accelerate (scrolls faster, scores faster, less reaction time)
- Down: brake (slower, safer, less points)

### Road Generation
New road content generated one row at a time at the top. Road can curve (shift center left/right gradually). Shoulders fill with random scenery objects. Lane markings repeat at regular intervals.

### Traffic
- Oncoming vehicles spawn at top, scroll down with road
- Types: cars (1 char wide), trucks (1 wide, 2-3 tall), wide loads (2 wide)
- Vehicles stay in lanes but occasionally drift
- Density increases with score/speed
- Rendered as sprites for smooth movement

### Fuel Mechanic
- Fuel gauge depletes over time (faster at higher speed)
- Fuel cans appear on road as pickups (sprites)
- Running empty = game over (forces player to keep moving and collecting, can't camp in one safe lane)

### Sprites
- Player car: 1 sprite
- Oncoming vehicles: up to 8-10 sprites
- Fuel cans: 2-3 sprites
- Total within 16-sprite limit

### Sound
- Engine drone (pitch tied to speed)
- Horn honk (player press)
- Crash explosion on collision
- Fuel pickup ding
- Driving music (tempo increases with speed)

### Copper
- Road edge color striping for speed-line illusion
- Sky gradient at top of screen

### Game Flow
Title screen with engine rev → game starts at low speed → scroll loop (generate road, spawn traffic/fuel, player steers, collision check, fuel drain) → crash or empty tank → explosion/sputter animation → high score → play again?

### Estimated Size
~250-300 lines

---

## Implementation Order

1. **REVERSI** — establishes PETSCII drawing, color, sound, and input patterns
2. **AVALANCHE** — introduces frame-timed action loop, sprites, collision
3. **SHEEP** — scales up sprite count, adds multi-entity AI
4. **ROADHOG** — combines everything: scrolling, sprites, fuel system, road generation

Each game is designed, implemented, and playtested independently before starting the next. Lessons from each inform the next.
