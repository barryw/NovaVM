# Pac-Man Nova BASIC Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a playable Pac-Man clone as a single self-contained Nova BASIC program.

**Architecture:** Hardware sprites for player and ghosts, bitmap graphics for the maze, text overlay for score. 20x12 tile grid (16x16 px), VSYNC game loop at 60 Hz, WASD input. All data self-contained in DATA statements.

**Tech Stack:** Nova BASIC (EhBASIC on 6502 emulator with Nova virtual hardware)

**Output file:** `docs/programs/pacman.bas`

**How to test:** Load the program in the emulator via the MCP server or by pasting into the BASIC interpreter. Each task builds on the previous one and produces a runnable program.

---

### Task 1: Maze Data and Drawing

Draw the maze on screen from DATA statements. This is the visual foundation everything else builds on.

**Files:**
- Create: `docs/programs/pacman.bas`

**Step 1: Write the maze DATA and drawing routine**

The program should:
- DIM M(19,11) for the tile array
- Read 12 strings from DATA, parse each character into M(x,y)
- Set MODE 2, GCLS, set background to black (POKE 40961,0 for $A001)
- Draw walls as blue filled rectangles (GCOLOR 6, FILL)
- Draw pellets as small white dots (GCOLOR 1, FILL for 4x4 centered dots)
- Draw power pellets as larger white dots (GCOLOR 1, FILL for 8x8 centered dots)
- Draw ghost house door as pink line (GCOLOR 10)
- Print score header on text row 0

The maze layout (20x12):
```
WWWWWWWWWWWWWWWWWWWW
W........WW........W
W.WW.WWW.WW.WWW.WW.W
O.WW.WWW.WW.WWW.WW.O
W.................W
X.WW.WW.WWWW.WW.WW.X
W....WW..DD..WW....W
W.WW.WW.W__W.WW.WW.W
W.WW....W__W....WW.W
W.WW.WWW.WWWW.WWW.WW.W
W........X..X........W
WWWWWWWWWWWWWWWWWWWW
```

Note: This is a rough layout. During implementation, ensure exactly 20 chars per row and all corridors connect properly. Adjust as needed — the important thing is it looks and plays like Pac-Man.

**Step 2: Run and verify**

Run the program. You should see:
- Blue maze walls on black background
- White dots for pellets
- Larger dots for power pellets
- Score text at top
- Ghost house in center with door

**Step 3: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): maze data and drawing"
```

---

### Task 2: Pac-Man Sprite and Movement

Add the player sprite and WASD movement on the tile grid.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Add Pac-Man sprite shapes**

Add DATA statements for Pac-Man shapes:
- Shape 0: mouth open right
- Shape 1: mouth closed (circle)
- Use FLIPX for left-facing, FLIPY for vertical — so only 2 shapes needed initially
- Yellow body (color 7), black eye

Load shapes at startup with SPRITEDATA loops reading from DATA.

**Step 2: Add movement variables and game loop**

Add variables:
- PX, PY (pixel position), PT, PU (tile position)
- PD (current direction: 0=up,1=down,2=left,3=right), BD (buffered direction)
- Player starts at tile (10,10)

Game loop structure:
```
main loop:
  VSYNC
  GET K
  if K=119 or K=87 then BD=0  (w/W = up)
  if K=115 or K=83 then BD=1  (s/S = down)
  if K=97  or K=65 then BD=2  (a/A = left)
  if K=100 or K=68 then BD=3  (d/D = right)
  GOSUB move_pacman
  SPRITE 0, PX, PY+8   (offset by 8 for score area)
  animate mouth (toggle shape every 4 frames)
  GOTO main loop
```

Move subroutine:
- Track sub-tile pixel offset (SP = 0 to 12 in steps of 4)
- When SP=0 (at tile boundary): check if BD direction is open, take it; else check if PD is open, continue; else stop
- When moving: increment SP by 4, update PX/PY accordingly
- When SP reaches 16: arrived at next tile, reset SP=0, update PT/PU

Wall check helper: look up M(tx,ty) — if W, blocked; if D, blocked for Pac-Man.

**Step 3: Run and verify**

- Pac-Man sprite visible on the maze
- WASD moves Pac-Man through corridors
- Pac-Man stops at walls
- Mouth animates open/closed
- Movement is smooth (4px per frame)

**Step 4: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): player sprite and WASD movement"
```

---

### Task 3: Pellet Eating and Score

Detect when Pac-Man enters a pellet tile, eat it, update score.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Add pellet eating logic**

When Pac-Man arrives at a tile center (SP resets to 0):
- Check M(PT,PU)
- If pellet: SC=SC+10, M(PT,PU)=0, erase dot from bitmap (GCOLOR 0, FILL over the 4x4 dot area), PC=PC-1
- If power pellet: SC=SC+50, same erase but larger area, PC=PC-1, set fright timer FT=360
- WK toggle: alternate SOUND pitch each pellet for waka-waka

**Step 2: Add score display**

- LOCATE 7,0 : PRINT SC (right-justified with leading zeros or just the number)
- Update only when score changes (track old score OS, compare)
- Count total pellets at init time (PC variable)

**Step 3: Run and verify**

- Moving over pellets removes them from screen
- Score increases (10 per pellet, 50 per power pellet)
- Waka-waka sound alternates pitch
- Pellet count decreases

**Step 4: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): pellet eating and score display"
```

---

### Task 4: Ghost Sprites and Basic Movement

Add 4 ghost sprites that move around the maze. No AI yet — just random direction at intersections.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Add ghost sprite shapes**

DATA for ghost shapes (dome head, wavy bottom, eyes):
- Shapes 8-9: Blinky frames (red body = color 2)
- Shapes 10-11: Pinky frames (pink = color 10)
- Shapes 12-13: Inky frames (cyan = color 3)
- Shapes 14-15: Clyde frames (orange = color 8)
- Each ghost needs 2 wiggle frames (bottom edge alternates)

Load at startup. Assign SPRITESHAPE for sprites 1-4.

**Step 2: Add ghost state arrays**

```
DIM GX(3),GY(3)   : REM pixel position
DIM GT(3),GU(3)   : REM tile position
DIM GD(3),GS(3)   : REM direction, state
DIM GP(3)          : REM sub-tile progress
```

Initialize ghosts inside ghost house (tiles 9-10, rows 7-8). Release them one at a time with a short delay.

**Step 3: Add ghost movement subroutine**

For each ghost (FOR I=0 TO 3):
- Same sub-tile movement system as Pac-Man (4px per frame)
- At tile boundary: list valid exits (not wall, not reverse direction)
- Pick random valid exit: use RND to choose among options
- Update pixel position, set SPRITE position

Ghost house exit: when state=initial, move up through door tile to row 5, then start normal movement.

**Step 4: Run and verify**

- 4 colored ghosts visible in/near ghost house
- Ghosts move through corridors randomly
- Ghosts don't walk through walls
- Ghosts don't reverse direction
- Ghost wiggle animation works

**Step 5: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): ghost sprites and random movement"
```

---

### Task 5: Ghost AI — Chase and Scatter

Replace random movement with the personality-based targeting system.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Add mode timer and state machine**

Global variables:
- MD (current mode: 0=scatter, 1=chase)
- MT (mode timer, counts down each frame)
- Scatter duration and chase duration from difficulty table

At each frame: MT=MT-1. When MT=0, toggle MD, reset MT to new duration.

**Step 2: Implement targeting per ghost**

Replace random direction choice with target-based choice:

At tile boundary for ghost I:
- If MD=0 (scatter): target = corner tile (stored in arrays SX(3),SY(3))
- If MD=1 (chase): target depends on ghost index:
  - I=0 (Blinky): target = PT,PU (Pac-Man's tile)
  - I=1 (Pinky): target = 4 tiles ahead of Pac-Man in PD direction
  - I=2 (Inky): target = random valid direction (keep random for simplicity)
  - I=3 (Clyde): if distance > 8 tiles, target = Pac-Man; else target = scatter corner

Direction selection: for each valid exit, compute distance² to target (DX*DX+DY*DY), pick smallest.

**Step 3: Run and verify**

- Blinky chases directly — follows you
- Pinky tries to get ahead of you
- Inky is erratic
- Clyde approaches then retreats
- Ghosts periodically switch between chasing and scattering to corners
- Mode switches cause ghosts to briefly reverse direction

**Step 4: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): ghost AI with chase and scatter modes"
```

---

### Task 6: Frightened Mode and Ghost Eating

Power pellets make ghosts vulnerable. Pac-Man can eat them for points.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Add frightened ghost shapes**

DATA for shapes 24-25: blue body, pink squiggly mouth, white eyes. Two frames for animation.

**Step 2: Implement frightened state**

When power pellet eaten:
- FT = fright duration (from difficulty table)
- All ghosts in chase/scatter → state = frightened (GS(I)=2)
- Ghosts reverse direction immediately
- Swap all ghost sprites to frightened shape (SPRITESHAPE I+1, 24)
- Ghost speed halved in frightened mode (move every other frame)
- Reset eat multiplier EM=1

Each frame: if FT>0, FT=FT-1. Last 120 frames (2 sec): flash shapes (alternate 24/25 every 4 frames). When FT=0: restore normal shapes, restore chase/scatter state.

**Step 3: Implement ghost eating**

Check COLLISION(0) each frame:
- If bit set for ghost I+1 AND GS(I)=2 (frightened):
  - Score += 200*EM, EM=EM*2 (200, 400, 800, 1600)
  - GS(I)=3 (eaten)
  - Play eat sound
  - Swap to eyes shape (SPRITESHAPE I+1, 26+GD(I))

**Step 4: Add eaten ghost return**

When GS(I)=3:
- Target = ghost house door tile
- Move at double speed (8px/frame or move twice per frame)
- Eyes shape faces movement direction
- When reaching door tile: move down into house, GS(I)=0 (scatter), restore normal shape, move back out

**Step 5: Run and verify**

- Eating power pellet turns ghosts blue
- Blue ghosts move slower, wander randomly
- Ghosts flash near end of fright time
- Eating a ghost shows eyes returning to house
- Score multiplier works (200/400/800/1600)
- Ghosts respawn from house after being eaten

**Step 6: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): frightened mode and ghost eating"
```

---

### Task 7: Death, Lives, and Game Over

Handle Pac-Man getting caught, lives system, and game over screen.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Implement death detection**

Check COLLISION(0) each frame:
- If bit set for ghost I+1 AND GS(I)<2 (chase or scatter):
  - Trigger death sequence

**Step 2: Death sequence**

- Stop all movement
- Pause 60 frames (1 second)
- Play death sound (SOUND 40,60,3)
- Hide all ghost sprites
- Optional: simple death animation (shrink Pac-Man by swapping to smaller shapes, or just blink)
- Wait for sound to finish
- LV=LV-1
- Update lives display
- If LV=0: GOTO game over
- Else: reset positions (Pac-Man to start tile, ghosts to house), brief "READY!" display, resume

**Step 3: Lives display**

Show lives as small Pac-Man icons in text row 0, or just a number: `LIVES:3`

**Step 4: Game over**

- Display "GAME OVER" centered on screen (LOCATE 36,12 or similar)
- If SC>HI then HI=SC, update high score display
- Wait for keypress (GET loop)
- Restart game (GOTO init, but preserve HI)

**Step 5: Run and verify**

- Touching a chase/scatter ghost triggers death
- Death sound plays, brief pause
- Lives decrease, display updates
- Positions reset after death, pellets remain as-is
- Game over screen appears at 0 lives
- Keypress restarts with high score preserved

**Step 6: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): death, lives, and game over"
```

---

### Task 8: Sound Effects and Music

Add all sound effects and background music.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Define instruments at startup**

```
INSTRUMENT 0,$40,0,9,0,6    : REM waka (pulse, snappy)
INSTRUMENT 1,$10,2,8,10,8   : REM power pellet (triangle, sustained)
INSTRUMENT 2,$40,0,3,0,4    : REM eat ghost (pulse, quick)
INSTRUMENT 3,$80,0,12,0,8   : REM death (noise, long decay)
INSTRUMENT 4,$20,2,4,12,6   : REM siren (sawtooth, sustained)
```

**Step 2: Add sound triggers throughout the code**

- Pellet eat (in Task 3 code): `SOUND 60+WK*4, 3, 0` where WK toggles 0/1
- Power pellet: `SOUND 72, 30, 1`
- Ghost eaten: `SOUND 80, 8, 2`
- Death: `SOUND 40, 60, 3`

**Step 3: Add background siren**

After level start:
```
MUSIC 1, "T180 I4 L8 O4 C D E F E D C2"
MUSIC LOOP ON
MUSIC PLAY
```

Stop music during death sequence, restart after respawn.

**Step 4: Add level start jingle**

Before each level:
```
MUSIC 1, "T160 I0 L16 O4 C E G >C8 R4"
MUSIC LOOP OFF
MUSIC PLAY
wait_loop: IF PLAYING THEN GOTO wait_loop
```

**Step 5: Run and verify**

- Waka-waka alternates when eating pellets
- Power pellet has distinct sound
- Ghost eating has rising blip
- Death has noise burst
- Background siren plays during gameplay
- Level start has a jingle that completes before play begins

**Step 6: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): sound effects and music"
```

---

### Task 9: Level Progression and Difficulty

Add level completion and difficulty ramping.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Level completion detection**

In the game loop, after pellet eating: if PC=0 then GOTO level_complete.

Level complete sequence:
- Stop music
- Flash maze 3 times (toggle background color or GCOLOR swap with VSYNC timing)
- LN=LN+1
- Adjust difficulty variables from table
- Redraw maze with all pellets (re-read DATA with RESTORE, redraw)
- Reset positions
- GOTO level start

**Step 2: Difficulty table**

Store in DATA or use IF/THEN:
```
Level 1: ghost speed=2, fright=360, scatter=420, chase=1200
Level 2: ghost speed=2, fright=240, scatter=300, chase=1200
Level 3: ghost speed=4, fright=180, scatter=240, chase=1500
Level 4+: ghost speed=4, fright=120, scatter=180, chase=1800
```

Read into variables GV (ghost speed), FD (fright duration), SD (scatter duration), CD (chase duration) at level start.

**Step 3: Tunnel wrap**

Ensure tunnel works: if PX<0 then PX=304, if PX>304 then PX=0 (and same for ghosts). Test in both directions.

**Step 4: Run and verify**

- Clearing all pellets triggers level complete
- Maze redraws with fresh pellets
- Level number increments and displays
- Ghosts get faster at level 3
- Fright duration gets shorter
- Tunnel wraps correctly

**Step 5: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): level progression and difficulty"
```

---

### Task 10: Polish and Playtesting

Final adjustments, bug fixes, and feel improvements.

**Files:**
- Modify: `docs/programs/pacman.bas`

**Step 1: Playtest and fix**

Run the full game and fix:
- Ghost house exit timing (stagger ghost releases)
- Collision edge cases (eaten at exact same frame as fright ending)
- Score display formatting
- Maze connectivity issues (any unreachable pellets?)
- Tunnel wrap edge cases

**Step 2: Visual polish**

- Ensure ghost house door is visible but distinct
- Power pellets blink (toggle visibility every 15 frames)
- "READY!" text before each life
- Clean up any visual artifacts

**Step 3: Final verification**

Full playthrough checklist:
- [ ] Game starts with jingle
- [ ] WASD moves Pac-Man smoothly
- [ ] Pellets disappear with waka-waka
- [ ] Power pellets turn ghosts blue
- [ ] Eating ghosts gives escalating score
- [ ] Eaten ghosts return as eyes
- [ ] Death sequence plays correctly
- [ ] Lives decrement, game over at 0
- [ ] Level clears when all pellets eaten
- [ ] Difficulty increases each level
- [ ] High score persists across games
- [ ] Tunnel wrap works
- [ ] No visual glitches or stuck states

**Step 4: Commit**

```bash
git add docs/programs/pacman.bas
git commit -m "feat(pacman): polish and playtesting fixes"
```
