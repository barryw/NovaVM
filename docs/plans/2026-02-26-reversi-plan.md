# REVERSI Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build an Othello/Reversi game in Nova BASIC — player vs greedy AI, with PETSCII title screen, SID sound, and sprite cursor — as the first of four Fun & Games showcase programs.

**Architecture:** Single standalone `.bas` file. CP437 font (FONT 0) for gameplay board (best box-drawing characters), PETSCII (FONT 1) for title screen. Board stored in 2D array, rendered via POKE to character/color RAM for speed. Sprite cursor for cell selection. Greedy AI picks the move that flips the most pieces.

**Tech Stack:** Nova BASIC on e6502 emulator. VSYNC game loop, SID sound via INSTRUMENT/SOUND, sprites via SPRITE/SPRITEDATA commands.

**Reference docs:**
- `docs/plans/2026-02-26-fun-games-design.md` — overall design
- `docs/programs/pacman.bas` — reference game structure and patterns
- `CLAUDE.md` — memory map, VGC commands, build/run commands

**Run command:** `dotnet run --project e6502.Avalonia` then `LOAD "reversi"` or type in directly.

---

## Constants & Memory Layout

These values are used throughout the plan:

```
Screen: 80 columns × 25 rows
Character RAM: $AA00 (43520 decimal), 2000 bytes
Color RAM:     $B1D0 (45520 decimal), 2000 bytes
Color byte:    high nibble = foreground, low nibble = background

Board position on screen: column 24, row 4
Cell width: 3 chars    Cell height: 1 row
Grid: 9 horizontal lines × 9 vertical lines with 8×8 cells between
Board total: 33 chars wide × 17 rows tall (cols 24-56, rows 4-20)

CP437 box-drawing characters:
  196=─  179=│  218=┌  191=┐  192=└  217=┘
  195=├  180=┤  194=┬  193=┴  197=┼

Piece character: CHR$(254) = ■ (small filled square)
Board colors: green background (5), pieces white (1) or black (0)
Color palette: 0=black 1=white 2=red 3=cyan 4=purple 5=green
               6=blue 7=yellow 8=orange 9=brown 10=light red
               11=dark grey 12=medium grey 13=light green 14=light blue 15=light grey

Board array: B(x,y) where 0=empty, 1=black(player), 2=white(computer)
Direction vectors: 8 directions for scanning (DX,DY arrays)
```

## Line Number Allocation

```
10-49:      Initialization (DIM, MODE, constants, instruments, GOSUB title)
50-99:      New game setup (clear board, place starting pieces, set turn)
100-199:    Main game loop (VSYNC, turn dispatch, game-over check)
200-299:    Player turn (cursor input, move selection)
300-399:    AI turn (find best move, execute)
400-499:    Board rendering subroutine
500-599:    Legal move subroutines (check single move, find all moves)
600-699:    Move execution + flipping subroutine
700-799:    Game over detection + display
800-899:    Title screen
900-949:    Sound effect subroutines
950-999:    Utility (score update, status message)
```

---

### Task 1: Program Skeleton + Board Rendering

**Files:**
- Create: `docs/programs/reversi.bas`

**Goal:** Display an empty 8×8 Reversi board using CP437 box-drawing characters, with column labels (A-H) and row labels (1-8).

**Step 1: Write the initial program**

Create `docs/programs/reversi.bas` with the initialization and board drawing code:

```basic
1 REM === REVERSI FOR NOVA BASIC ===
2 REM === FUN & GAMES SHOWCASE #1 ===
10 MODE 2:FONT 0:CLS:GCLS
11 DIM B(7,7),DX(7),DY(7)
12 DIM VM(63,2)
15 VOLUME 15
16 REM Board screen position
17 BL=24:BT=4
18 REM Colors: board bg, black piece, white piece
19 CB=5:C1=0:C2=1
20 REM Direction vectors for 8 directions
21 FOR I=0 TO 7:READ DX(I),DY(I):NEXT I
22 DATA -1,-1,0,-1,1,-1,-1,0,1,0,-1,1,0,1,1,1
25 REM Draw green background fill
26 GCOLOR 5:FILL 0,0,319,199
40 GOSUB 800:REM Title screen
50 GOSUB 400:REM Draw board
60 GOTO 60:REM Placeholder loop
400 REM === DRAW BOARD ===
401 COLOR 7,0
402 REM Column labels
403 FOR I=0 TO 7
404 LOCATE BL+1+I*4,BT-1:PRINT CHR$(65+I);
405 NEXT I
406 REM Top border
407 LOCATE BL,BT:PRINT CHR$(218);
408 FOR I=0 TO 6:PRINT CHR$(196);CHR$(196);CHR$(196);CHR$(194);:NEXT I
409 PRINT CHR$(196);CHR$(196);CHR$(196);CHR$(191);
410 REM Board rows
411 FOR R=0 TO 7
412 REM Row with cells
413 LOCATE BL,BT+1+R*2
414 PRINT CHR$(179);
415 FOR C=0 TO 7
416 PRINT "   ";
417 IF C<7 THEN PRINT CHR$(179);
418 NEXT C
419 PRINT CHR$(179);
420 REM Row label
421 LOCATE BL-2,BT+1+R*2:PRINT STR$(R+1);
422 REM Separator (except after last row)
423 IF R=7 THEN GOTO 440
424 LOCATE BL,BT+2+R*2
425 PRINT CHR$(195);
426 FOR I=0 TO 6:PRINT CHR$(196);CHR$(196);CHR$(196);CHR$(197);:NEXT I
427 PRINT CHR$(196);CHR$(196);CHR$(196);CHR$(180);
430 NEXT R
440 REM Bottom border
441 LOCATE BL,BT+16
442 PRINT CHR$(192);
443 FOR I=0 TO 6:PRINT CHR$(196);CHR$(196);CHR$(196);CHR$(193);:NEXT I
444 PRINT CHR$(196);CHR$(196);CHR$(196);CHR$(217);
445 REM Color the board cells green
446 FOR R=0 TO 7:FOR C=0 TO 7
447 SY=BT+1+R*2:SX=BL+1+C*4
448 FOR I=0 TO 2
449 POKE 45520+SY*80+SX+I,CB*16+CB
450 NEXT I
451 NEXT C:NEXT R
460 RETURN
800 REM === TITLE SCREEN (placeholder) ===
801 LOCATE 30,12:PRINT "R E V E R S I"
802 LOCATE 28,14:PRINT "Press any key to play"
803 GET K:IF K=0 THEN 803
804 CLS
810 RETURN
```

**Step 2: Test the board display**

Run: `dotnet run --project e6502.Avalonia`
Then type: `LOAD "reversi"` and `RUN`

Expected:
- Title message appears, press any key
- 8×8 grid with box-drawing borders
- Column labels A-H above, row labels 1-8 on left
- Green-colored cell interiors
- Grid lines in light grey

**Step 3: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): board rendering with CP437 box-drawing grid"
```

---

### Task 2: Game State + Piece Rendering

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** Initialize the board with 4 starting pieces (standard Othello opening) and render pieces as colored characters in the board cells.

**Step 1: Add game setup and piece rendering**

Replace the placeholder loop at line 60 and add the new game setup at lines 50-59. Add a piece rendering subroutine.

Add/modify these lines:

```basic
50 REM === NEW GAME ===
51 FOR Y=0 TO 7:FOR X=0 TO 7:B(X,Y)=0:NEXT X:NEXT Y
52 B(3,3)=2:B(4,4)=2:REM White
53 B(3,4)=1:B(4,3)=1:REM Black
54 PT=1:REM Player turn: 1=black(human), 2=white(AI)
55 GOSUB 400:REM Draw board
56 GOSUB 450:REM Render all pieces
57 GOSUB 950:REM Update score display
58 GOTO 58:REM Placeholder loop
```

Add piece rendering subroutine after the board drawing (insert after line 460):

```basic
450 REM === RENDER ALL PIECES ===
451 FOR RR=0 TO 7:FOR CC=0 TO 7
452 GOSUB 470:REM Render single cell
453 NEXT CC:NEXT RR
455 RETURN
470 REM === RENDER CELL (CC,RR) ===
471 SY=BT+1+RR*2:SX=BL+1+CC*4
472 P=B(CC,RR)
473 IF P=0 THEN PC=CB:PA=32:GOTO 478
474 IF P=1 THEN PC=C1:PA=254:GOTO 478
475 PC=C2:PA=254
478 REM Write center character
479 POKE 43520+SY*80+SX+1,PA
480 REM Set color: piece foreground on green background
481 POKE 45520+SY*80+SX+1,PC*16+CB
485 RETURN
```

Add score display subroutine:

```basic
950 REM === UPDATE SCORE DISPLAY ===
951 S1=0:S2=0
952 FOR YY=0 TO 7:FOR XX=0 TO 7
953 IF B(XX,YY)=1 THEN S1=S1+1
954 IF B(XX,YY)=2 THEN S2=S2+1
955 NEXT XX:NEXT YY
956 COLOR 1,0
957 LOCATE 62,6:PRINT "BLACK: ";S1;" "
958 LOCATE 62,8:PRINT "WHITE: ";S2;" "
960 RETURN
```

**Step 2: Test**

Run the program. Expected:
- Board displays with 4 starting pieces in the center
- D4/E5 = white (■ in white), D5/E4 = black (■ in black)
- Pieces centered in green cells
- Score shows BLACK: 2  WHITE: 2 on right side

**Step 3: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): game state initialization and piece rendering"
```

---

### Task 3: Cursor + Keyboard Input

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** Add a cursor that moves with arrow keys, visually highlighting the selected board cell. Use a sprite for a blinking cursor overlay.

**Step 1: Add cursor variables and sprite setup**

Add cursor init after line 22:

```basic
23 CX=0:CY=0:REM Cursor position on board (0-7)
24 REM Define cursor sprite (shape 0): hollow square outline
30 FOR I=0 TO 15:SPRITEDATA 0,I,0,0,0,0,0,0,0,0:NEXT I
31 SPRITEDATA 0,0,$77,$77,$77,$77,$77,$77,$77,$77
32 SPRITEDATA 0,1,$70,$00,$00,$00,$00,$00,$00,$07
33 FOR I=2 TO 5:SPRITEDATA 0,I,$70,$00,$00,$00,$00,$00,$00,$07:NEXT I
34 SPRITEDATA 0,6,$70,$00,$00,$00,$00,$00,$00,$07
35 SPRITEDATA 0,7,$77,$77,$77,$77,$77,$77,$77,$77
36 SPRITESHAPE 0,0:SPRITE 0,ON
37 SPRITESET 0,6,2:REM Priority: in front
```

Add cursor position update subroutine:

```basic
460 REM === UPDATE CURSOR SPRITE POSITION ===
461 QX=BL*4+4+CX*16:QY=BT*8+8+CY*16
462 SPRITE 0,QX,QY
465 RETURN
```

Replace the placeholder loop at line 58 with cursor input:

```basic
58 GOSUB 460:REM Position cursor
100 REM === MAIN GAME LOOP ===
101 VSYNC
102 GET K
103 IF K=0 THEN GOTO 101
110 OX=CX:OY=CY
111 IF K=29 THEN CX=CX+1:REM Right arrow
112 IF K=28 THEN CX=CX-1:REM Left arrow
113 IF K=31 THEN CY=CY+1:REM Down arrow
114 IF K=30 THEN CY=CY-1:REM Up arrow
115 IF CX<0 THEN CX=7
116 IF CX>7 THEN CX=0
117 IF CY<0 THEN CY=7
118 IF CY>7 THEN CY=0
120 GOSUB 460:REM Update cursor sprite
130 GOTO 101
```

Note: Arrow key codes in Nova BASIC: 28=left, 29=right, 30=up, 31=down. Check `docs/help/` if these differ — they may be platform-specific. Pac-Man uses WASD (w=119, a=97, s=115, d=100), so also support those as fallback:

```basic
111 IF K=29 OR K=100 THEN CX=CX+1
112 IF K=28 OR K=97 THEN CX=CX-1
113 IF K=31 OR K=115 THEN CY=CY+1
114 IF K=30 OR K=119 THEN CY=CY-1
```

**Step 2: Test**

Run the program. Expected:
- Yellow/highlighted sprite square visible on the board
- Arrow keys (and WASD) move the cursor
- Cursor wraps around board edges
- Cursor is positioned over each cell correctly

Verify: move cursor to each corner (A1, H1, A8, H8) and check alignment.

**Step 3: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): sprite cursor with arrow key movement"
```

---

### Task 4: Legal Move Detection + Display

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** Implement the core Reversi move validation. For a given player, find all legal moves and display them as dimmed markers on the board.

**Step 1: Add legal move checking subroutines**

The key algorithm: a move at (MX,MY) is legal for player P if:
1. Cell is empty
2. In at least one of 8 directions, there is a contiguous line of opponent pieces followed by one of the player's pieces

```basic
500 REM === CHECK SINGLE MOVE (MX,MY) FOR PLAYER PT ===
501 REM Input: MX,MY=position, PT=current player
502 REM Output: NF=total pieces that would flip
503 NF=0
504 IF B(MX,MY)<>0 THEN RETURN
505 OP=3-PT:REM Opponent
506 FOR D=0 TO 7
507 NX=MX+DX(D):NY=MY+DY(D):DF=0
508 IF NX<0 OR NX>7 OR NY<0 OR NY>7 THEN GOTO 518
509 IF B(NX,NY)<>OP THEN GOTO 518
510 REM Walk in this direction
511 DF=DF+1:NX=NX+DX(D):NY=NY+DY(D)
512 IF NX<0 OR NX>7 OR NY<0 OR NY>7 THEN GOTO 518
513 IF B(NX,NY)=OP THEN GOTO 511
514 IF B(NX,NY)=PT THEN NF=NF+DF
518 NEXT D
520 RETURN
530 REM === FIND ALL LEGAL MOVES FOR PLAYER PT ===
531 REM Output: NV=count of valid moves, VM(i,0)=x, VM(i,1)=y, VM(i,2)=flips
532 NV=0
533 FOR MY=0 TO 7:FOR MX=0 TO 7
534 GOSUB 500:REM Check this position
535 IF NF=0 THEN GOTO 539
536 VM(NV,0)=MX:VM(NV,1)=MY:VM(NV,2)=NF
537 NV=NV+1
539 NEXT MX:NEXT MY
540 RETURN
550 REM === DISPLAY VALID MOVES AS MARKERS ===
551 FOR I=0 TO NV-1
552 VX=VM(I,0):VY=VM(I,1)
553 SY=BT+1+VY*2:SX=BL+1+VX*4+1
554 POKE 43520+SY*80+SX,250:REM CHR$(250) = middle dot
555 POKE 45520+SY*80+SX,12*16+CB:REM Medium grey on green
556 NEXT I
560 RETURN
565 REM === CLEAR VALID MOVE MARKERS ===
566 FOR I=0 TO NV-1
567 VX=VM(I,0):VY=VM(I,1)
568 IF B(VX,VY)<>0 THEN GOTO 572
569 SY=BT+1+VY*2:SX=BL+1+VX*4+1
570 POKE 43520+SY*80+SX,32:REM Space
571 POKE 45520+SY*80+SX,CB*16+CB:REM Green on green
572 NEXT I
575 RETURN
```

Now integrate into the game flow. After setting up the game (line 57), find and display legal moves:

```basic
57 GOSUB 530:REM Find legal moves
58 GOSUB 550:REM Display them
59 GOSUB 460:REM Position cursor
```

Also show current turn in the status area:

```basic
959 LOCATE 62,10
960 IF PT=1 THEN PRINT "YOUR TURN    "
961 IF PT=2 THEN PRINT "AI THINKING.."
965 RETURN
```

Update line 57 to also call 959:

```basic
57 GOSUB 530:GOSUB 550:GOSUB 959
```

**Step 2: Test**

Run the program. Expected:
- Starting position shows 4 valid moves for black: D3, C4, F5, E6 (the classic Othello opening moves)
- Valid moves displayed as grey dots (·) on the board
- 4 dots visible in correct positions

Verify by counting: black can play at any empty cell adjacent to white that creates a flank.

**Step 3: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): legal move detection and valid move display"
```

---

### Task 5: Move Execution + Piece Flipping

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** When the player presses SPACE/ENTER on a legal move, place the piece and flip all captured pieces. Brief visual feedback on flipped pieces.

**Step 1: Add move execution subroutine**

```basic
600 REM === EXECUTE MOVE (MX,MY) FOR PLAYER PT ===
601 REM Assumes move is legal (NF>0 from GOSUB 500)
602 B(MX,MY)=PT:OP=3-PT
603 FOR D=0 TO 7
604 NX=MX+DX(D):NY=MY+DY(D):DF=0
605 IF NX<0 OR NX>7 OR NY<0 OR NY>7 THEN GOTO 625
606 IF B(NX,NY)<>OP THEN GOTO 625
607 REM Walk to find our piece
608 FX=NX:FY=NY:DF=1
609 FX=FX+DX(D):FY=FY+DY(D)
610 IF FX<0 OR FX>7 OR FY<0 OR FY>7 THEN GOTO 625
611 IF B(FX,FY)=OP THEN DF=DF+1:GOTO 609
612 IF B(FX,FY)<>PT THEN GOTO 625
613 REM Flip all opponent pieces in this direction
614 FX=NX:FY=NY
615 B(FX,FY)=PT
616 REM Flash the flipped piece briefly (red highlight)
617 SY=BT+1+FY*2:SX=BL+1+FX*4+1
618 POKE 45520+SY*80+SX,2*16+CB:REM Red flash
619 FX=FX+DX(D):FY=FY+DY(D)
620 IF B(FX,FY)=OP THEN GOTO 615
625 NEXT D
626 REM Brief pause to show flip animation
627 FOR W=1 TO 8:VSYNC:NEXT W
628 REM Re-render all pieces (clears flash colors)
629 GOSUB 450
630 RETURN
```

**Step 2: Wire move selection into cursor input**

Add SPACE/ENTER handling in the game loop. Modify/add lines in the 100-block:

```basic
125 IF K<>32 AND K<>13 THEN GOTO 130
126 REM Check if cursor is on a legal move
127 MX=CX:MY=CY:GOSUB 500
128 IF NF=0 THEN GOTO 130:REM Not a legal move, ignore
129 GOSUB 565:REM Clear move markers
131 GOSUB 600:REM Execute the move
132 GOSUB 900:REM Play placement sound
133 GOSUB 950:REM Update score
134 REM Switch turns
135 PT=3-PT
136 GOSUB 530:REM Find legal moves for next player
137 IF NV=0 THEN PT=3-PT:GOSUB 530:REM Pass — switch back
138 IF NV=0 THEN GOTO 700:REM Both pass — game over
139 GOSUB 550:REM Display new valid moves
140 GOSUB 959:REM Update turn display
141 IF PT=2 THEN GOTO 300:REM AI turn
130 GOTO 101
```

Add placeholder sound subroutine:

```basic
900 REM === PLACEMENT SOUND (placeholder) ===
901 SOUND 72,3
905 RETURN
```

**Step 3: Test**

Run the program. Expected:
- Move cursor to a valid move position (grey dot)
- Press SPACE — piece is placed, opponent pieces flip
- Brief red flash on flipped pieces, then normal colors
- Score updates, valid moves recalculate for next turn
- Pressing SPACE on invalid position does nothing
- Turn indicator changes

Test by playing the opening: place black at D3 → white's E3 should flip to black. Score becomes BLACK: 4, WHITE: 1.

**Step 4: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): move execution with piece flipping and visual feedback"
```

---

### Task 6: AI Opponent

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** Add a computer opponent that evaluates all legal moves and picks the one that flips the most pieces. Add a brief "thinking" delay for feel.

**Step 1: Add AI subroutine**

```basic
300 REM === AI TURN ===
301 GOSUB 959:REM Show "AI THINKING.."
302 REM Brief thinking delay
303 FOR W=1 TO 30:VSYNC:NEXT W
304 REM Find best move (most flips)
305 BM=0:BF=VM(0,2)
306 FOR I=1 TO NV-1
307 IF VM(I,2)<=BF THEN GOTO 310
308 BF=VM(I,2):BM=I
310 NEXT I
312 REM Add corner bonus: corners are strategically valuable
313 FOR I=0 TO NV-1
314 VX=VM(I,0):VY=VM(I,1)
315 IF (VX=0 OR VX=7) AND (VY=0 OR VY=7) THEN BM=I:GOTO 320
316 NEXT I
320 REM Move cursor to chosen cell for visual feedback
321 CX=VM(BM,0):CY=VM(BM,1)
322 GOSUB 460:REM Move cursor sprite
323 FOR W=1 TO 15:VSYNC:NEXT W
324 REM Execute the move
325 GOSUB 565:REM Clear move markers
326 MX=CX:MY=CY:GOSUB 600
327 GOSUB 910:REM Play AI move sound
328 GOSUB 950:REM Update score
329 REM Switch to player
330 PT=3-PT
331 GOSUB 530:REM Find player's legal moves
332 IF NV=0 THEN PT=3-PT:GOSUB 530:REM Player must pass
333 IF NV=0 THEN GOTO 700:REM Both pass — game over
334 GOSUB 550:REM Show valid moves
335 GOSUB 959:REM Show "YOUR TURN"
340 GOTO 101:REM Back to main loop
```

Add AI sound placeholder:

```basic
910 REM === AI MOVE SOUND ===
911 SOUND 60,3
915 RETURN
```

**Step 2: Test**

Run the program. Expected:
- After player makes a move, "AI THINKING.." appears
- Cursor moves to AI's chosen cell
- AI places piece, flips happen
- Turn returns to player with new valid moves shown
- AI always picks a legal move
- If player has no moves, AI plays again (and vice versa)

Test: play several turns. AI should generally pick moves that flip the most pieces, and always pick corners when available.

**Step 3: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): greedy AI opponent with corner preference"
```

---

### Task 7: Game Over + Restart

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** Detect game over (both players have no legal moves), display the winner, and offer to play again.

**Step 1: Add game over routine**

```basic
700 REM === GAME OVER ===
701 SPRITE 0,OFF:REM Hide cursor
702 GOSUB 450:REM Final board render
703 GOSUB 950:REM Final score
704 REM Determine winner
705 IF S1>S2 THEN W$="YOU WIN!":GOTO 720
706 IF S2>S1 THEN W$="AI WINS!":GOTO 720
707 W$="IT'S A DRAW!"
720 REM Display result
721 COLOR 7,0
722 LOCATE 62,14:PRINT W$
723 LOCATE 62,16:PRINT "FINAL SCORE"
724 LOCATE 62,17:PRINT "BLACK: ";S1
725 LOCATE 62,18:PRINT "WHITE: ";S2
726 GOSUB 920:REM Play game-over sound
730 REM Play again?
731 LOCATE 62,21:PRINT "PLAY AGAIN?"
732 LOCATE 62,22:PRINT "(Y/N)"
733 GET K:IF K=0 THEN GOTO 733
734 IF K=89 OR K=121 THEN GOTO 45:REM Y/y — restart
735 IF K=78 OR K=110 THEN GOTO 740:REM N/n — quit
736 GOTO 733
740 CLS:FONT 0:LOCATE 0,0:PRINT "THANKS FOR PLAYING!":END
```

Add line 45 as a restart entry point (jump back to new game, skipping title):

```basic
45 CLS:GOSUB 400:GOTO 50
```

Add game-over sound placeholder:

```basic
920 REM === GAME OVER SOUND ===
921 IF S1>S2 THEN SOUND 72,8:SOUND 76,8:SOUND 79,8:SOUND 84,16:RETURN
922 SOUND 60,8:SOUND 55,8:SOUND 50,16
925 RETURN
```

**Step 2: Test**

This is hard to test fully without playing a complete game. Quick test approach:
- Modify starting position temporarily to near-end state for testing
- Or just play through (it's Reversi — games are ~30 moves each side)
- Verify: when no moves remain for either player, game-over screen appears
- Verify: Y restarts, N quits
- Verify: correct winner announced

**Step 3: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): game over detection, winner display, and replay"
```

---

### Task 8: Title Screen + Sound Effects + Polish

**Files:**
- Modify: `docs/programs/reversi.bas`

**Goal:** Add a PETSCII art title screen (FONT 1), proper SID instruments and sound effects, and polish the overall experience.

**Step 1: Replace title screen with PETSCII art version**

```basic
800 REM === TITLE SCREEN ===
801 FONT 1:CLS
802 COLOR 5,0
803 LOCATE 25,3:PRINT "OOOOOOO  OOOOOO O     O OOOOOO OOOOO   OOOO  O"
804 LOCATE 25,4:PRINT "O     O  O      O     O O      O    O O      O"
805 LOCATE 25,5:PRINT "OOOOOOO  OOOOO  O     O OOOOO  OOOOO   OOOO  O"
806 LOCATE 25,6:PRINT "O  O     O       O   O  O      O  O        O  O"
807 LOCATE 25,7:PRINT "O   OO   OOOOOO   OOO   OOOOOO O   OO  OOOO  O"
810 COLOR 12,0
811 LOCATE 24,10:PRINT "INSPIRED BY COMMODORE 64 FUN & GAMES (1984)"
812 COLOR 7,0
813 LOCATE 30,13:PRINT "YOU ARE BLACK  ";CHR$(254)
814 LOCATE 30,14:PRINT "AI  IS WHITE  ";CHR$(254)
815 COLOR 1,0
816 LOCATE 28,17:PRINT "ARROW KEYS TO MOVE CURSOR"
817 LOCATE 28,18:PRINT "SPACE TO PLACE YOUR PIECE"
820 COLOR 15,0
821 LOCATE 28,21:PRINT "PRESS ANY KEY TO START"
822 REM Play title jingle
823 GOSUB 930
825 GET K:IF K=0 THEN 825
826 FONT 0:CLS
830 RETURN
```

**Step 2: Add proper SID instruments and sound effects**

Replace the sound placeholder subroutines with proper SID sounds:

```basic
13 REM Define instruments
14 INSTRUMENT 0,$40,0,8,0,6:REM Pulse — click/tap
15 INSTRUMENT 1,$10,2,6,8,8:REM Triangle — mellow tone
16 INSTRUMENT 2,$80,0,4,0,2:REM Noise — impact
```

```basic
900 REM === PLAYER MOVE SOUND ===
901 SOUND 72,2,0:REM Click
902 RETURN
910 REM === AI MOVE SOUND ===
911 SOUND 60,2,0:REM Lower click
912 RETURN
920 REM === GAME OVER SOUND ===
921 IF S1>S2 THEN GOTO 923
922 SOUND 55,6,1:SOUND 50,6,1:SOUND 45,12,1:RETURN
923 SOUND 67,4,1:SOUND 72,4,1:SOUND 76,4,1:SOUND 79,8,1
924 RETURN
930 REM === TITLE JINGLE ===
931 MUSIC 1,"T140 I1 L8 O4 E G >C4 <B8 G E4."
932 MUSIC PLAY
935 RETURN
```

**Step 3: Add flip cascade sound**

In the move execution subroutine (line 600 block), add a rising tone for each flipped piece. After line 618 (the red flash):

```basic
618 POKE 45520+SY*80+SX,2*16+CB
619 SOUND 60+NF*2,2,0:REM Rising pitch with each flip
```

Wait — NF is total flips, not per-piece. We need a counter. Add a flip counter FC in the execute routine. Modify the execute subroutine to track individual flips:

```basic
600 REM === EXECUTE MOVE ===
601 B(MX,MY)=PT:OP=3-PT:FC=0
...
615 B(FX,FY)=PT:FC=FC+1
616 REM Flash + sound
617 SY=BT+1+FY*2:SX=BL+1+FX*4+1
618 POKE 45520+SY*80+SX,2*16+CB
619 SOUND 60+FC*3,2,0
```

Note: FC here is a flip counter (distinct from FC used for frame counting in Pac-Man). Name it FL instead to be safe:

```basic
601 B(MX,MY)=PT:OP=3-PT:FL=0
615 B(FX,FY)=PT:FL=FL+1
619 SOUND 60+FL*3,2,0
```

**Step 4: Add status area polish**

Add a decorative side panel:

```basic
955 REM === STATUS PANEL ===
956 COLOR 7,0
957 LOCATE 62,4:PRINT CHR$(218);STRING$(15,196);CHR$(191)
958 LOCATE 62,5:PRINT CHR$(179);"   REVERSI    ";CHR$(179)
959 LOCATE 62,20:PRINT CHR$(192);STRING$(15,196);CHR$(217)
```

Update the score/turn display to fit within the panel:

```basic
960 LOCATE 64,7:PRINT "BLACK: ";S1;" "
961 LOCATE 64,9:PRINT "WHITE: ";S2;" "
963 LOCATE 64,12
964 IF PT=1 THEN PRINT "YOUR TURN    "
965 IF PT=2 THEN PRINT "AI THINKING.."
966 RETURN
```

**Step 5: Test end-to-end**

Run the complete game. Verify:
- PETSCII title screen with "REVERSI" text and jingle
- Press key → board appears in CP437 with green cells
- Valid moves shown as grey dots
- Arrow keys move cursor sprite
- SPACE places piece with click sound
- Flipped pieces flash red with rising cascade sound
- AI "thinks" briefly, moves cursor, places piece
- Score updates correctly
- Game over detects properly, shows winner
- Win/lose have different jingles
- Y restarts, N quits cleanly

**Step 6: Commit**

```bash
git add docs/programs/reversi.bas
git commit -m "feat(reversi): title screen, SID sound effects, and polish"
```

---

## Final Checklist

After all tasks complete, verify:
- [ ] Game plays from start to finish without errors
- [ ] All legal moves are correctly identified (test a few known positions)
- [ ] Flipping works in all 8 directions
- [ ] AI always makes a legal move
- [ ] Pass works correctly (when one player has no moves)
- [ ] Game over triggers when both players have no moves
- [ ] Score is accurate at game end
- [ ] Sound effects play at correct moments
- [ ] Title screen displays and jingle plays
- [ ] Restart works cleanly
- [ ] Program is under 200 lines
