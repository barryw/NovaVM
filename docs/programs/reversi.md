---
title: Reversi
type: program
category: Programs
keywords: [reversi, othello, board game, strategy]
---

## How to Play

Reversi (also known as Othello) is a classic two-player strategy board game played on an 8x8 grid. You play as **Black** against the computer (White).

## Controls

| Key | Action |
|-----|--------|
| Arrow keys | Move the cursor around the board |
| Enter / Space | Place your piece at the cursor |

## Rules

- Black always moves first
- Place your piece so that it **flanks** one or more of your opponent's pieces in a straight line (horizontal, vertical, or diagonal)
- All flanked opponent pieces are flipped to your color
- If you cannot make a valid move, your turn is skipped
- The game ends when neither player can move
- The player with the most pieces on the board wins

## Strategy Tips

- **Corners are king** — once captured, corner pieces can never be flipped
- **Avoid edges adjacent to empty corners** — your opponent can use them to take the corner
- **Fewer pieces early, more pieces late** — having fewer pieces in the mid-game gives your opponent fewer targets to flip, while limiting their moves
- **Control the center** — central positions give you more flipping opportunities
- The AI uses a greedy strategy with corner preference — try to bait it into bad positions

## About

This is the first Fun & Games showcase program for Nova BASIC. It features SID chip sound effects, sprite-based cursor, and a full AI opponent.
