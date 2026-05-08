# Nova Chess Design

## Runtime Shape

Nova Chess should be a runtime, not a BASIC program. The image contains an
`AUTOBOOT.bin` launcher and `NOVACHESS.BIN`; the launcher streams the runtime
into the primary `$C000` bank and jumps through its reset vector. That leaves
BASIC out of the hot path and matches the model we need for NovaZ, Forth, and
other language/runtime images.

## Display Direction

The C64 version uses a 40x25 text-era layout with hardware sprites and a compact
right-side status area. Nova has a denser text plane, a graphics plane, and a
blitter, so the Nova version should not look like a direct C64 screen lift.

Current first-screen layout:

- 320x200 graphics plane is the game surface.
- Board is drawn as 8x8 graphics rectangles, 22x22 pixels per square.
- Pieces are virtual sprites blitted onto the graphics plane with color-key
  transparency. Each piece is 20x20, centered with a one-pixel margin inside
  its square.
- Rank/file indicators are 5x7 graphics-plane glyphs, pixel-centered against
  the board squares so they do not inherit the coarser text-grid alignment.
- Text plane overlays the game surface in mode 2 for title, turn, clock, search,
  last move, and command/status information.
- Hardware cursor is enabled only while menus or coordinate-entry input are
  waiting for a key; it is kept off during engine thinking and animation.

## Front Door And Game Flow

The default image is still useful as a hardware torture test: it shows the
splash/menu briefly and then autostarts the engine-vs-engine demo unless a key
arrives. A non-autostart build (`CHESS_AUTOSTART_DEMO=0`) stays on the title
screen and waits for setup input.

The title/setup shell now owns the choices that should remain stable as Nova
Chess grows:

- `D`: engine-vs-engine demo/self-play.
- `N`: new game setup.
- player mode: human vs Nova, Nova vs Nova, or human vs human.
- difficulty: Easy, Medium, Hard. This writes the engine's exported
  `difficulty` byte before the game starts.
- side selection for human-vs-Nova: White or Black.

Human input currently uses coordinate entry (`E2E4`, optionally `E7E8N` for a
knight promotion). The runtime asks the engine for the current legal move list
through exported `MoveListFrom`/`MoveListTo` labels and only commits a matching
move. That keeps chess rules inside the engine instead of duplicating them in
the UI. Promotion UI is still deliberately thin: the engine currently supports
queen and knight promotions.

Longer term, opponent selection should be modeled as a provider rather than as
engine-specific UI: local Nova engine, second local human, and network opponent.
The network provider can later route positions/moves through NovaHost to a game
server backed by Stockfish without changing board rendering, move animation, or
game-state presentation.

## Virtual Sprites

Chess is a good proving ground for virtual sprites because it needs 32 pieces,
which exceeds the 16 hardware sprite registers. The shared `vsprite` library is
deliberately small:

- Callers own the asset buffers and object tables.
- `VSPRITE.BLIT` copies a configured rectangle through the blitter.
- `VSPRITE.GFX_BLIT` computes `y * 320 + x` and draws into the graphics plane.
- `VSPRITE.FILL` and `VSPRITE.GFX_FILL` provide the rectangle fill path used by
  boards, panels, and highlights.
- `VSPRITE.GFX_SAVE_BG` and `VSPRITE.GFX_RESTORE_BG` provide BOB-style
  saved-background movement: save the graphics-plane pixels under the object,
  draw it, restore those pixels before moving, then save/draw at the next
  position.

The graphics plane and the virtual sprite source have separate transparency
rules. `VSPRITE_COLORKEY` tells the blitter which source byte to skip.
`VGC_GFXTRANS` tells the compositor which graphics-plane color is transparent.
Nova Chess sets both to color 2 so palette 0 can be used for visible black
pieces.

Gameplay should keep caller-owned virtual sprite records:

- current x/y
- width/height
- asset address
- saved-background buffer address/stride
- whether the saved background is valid

The shared library only knows about rectangles and buffers. Chess owns board
state, legal moves, captures, and which piece asset to draw.

## Move Animation

Accepted moves should animate by default. The game state should still update as
a single logical move; animation is only a renderer concern layered over the
stable board model. The renderer test mode runs a sparse sequence before the
chess engine is linked. It moves each white piece type from the first rank, then
each black piece type from the eighth rank, so both the raw NovaDraw art and the
runtime black-piece color remap are exercised.

Single-piece animation uses the shared vsprite saved-background helpers for the
moving piece's 20x20 rectangle. It does not redraw the full board every frame,
and it no longer has chess-specific square repair logic. Position changes are
computed through the shared tween library using ease-in/ease-out dampening over
a vblank-counted duration. Straight movers use one tweened line segment. Knights
use explicit L-shaped paths (`g1-g3-f3` and `g8-g6-f6`) made from two tweened
segments so the visual motion matches the move's character instead of sliding
diagonally.

Multi-piece animation uses the shared vsprite scene helpers. Simple scenes can
restore old backgrounds, let the game update descriptor positions, save every
new background from the clean playfield, then draw descriptors in list order.
That order is z-order, so overlapping or crossing pieces behave predictably
without each game writing its own repair code.

Scenes that need to move multiple pieces atomically use
`VSPRITE.SCENE_COMMIT_ATOMIC`. The caller supplies a dirty graphics rectangle
and work buffer. The helper copies that rectangle into the work buffer, restores
old saved backgrounds there, saves the new clean backgrounds, draws the new
sprite positions in z-order, then copies the completed rectangle back to the
graphics plane as a single visible blit. Castling is the first consumer: the
rook descriptor draws behind the king descriptor while both pieces are tweened
over the same duration, and the final state no longer needs a visible square
repair pass.

The tween ABI is intentionally hardware-ready. Callers provide start, end,
duration, frame, and easing mode, then read back value/done state. The current
implementation is software, but a future math coprocessor can replace the
implementation behind the same shared routine names.

The render-test speed is build-time configurable through `RENDER_STEP_PAUSE`,
`RENDER_FINAL_PAUSE`, `RENDER_PIXEL_STEP`, and `RENDER_REPEAT`. Hardware demos
use one-pixel steps and repeat. Automated tests use larger pixel steps, no frame
delay, and hold the final board so they can still cover the full white and
black movement suite quickly. `castle-test` builds a synthetic castling fixture
that runs both kingside and queenside animation through the scene path.

## Engine Boundary

The C64 engine split is vendored under `vendor/chess6502-engine` and built as a
separate RAM binary. The NDI contains:

- `AUTOBOOT.BIN`, the launcher.
- `NOVACHESS.BIN`, the 16K runtime loaded into the primary `$C000` bank.
- `CHESSENG.BIN`, the engine image loaded through generic FIO before the board
  is drawn.

`CHESSENG.BIN` carries a normal two-byte load address and currently loads at
`$4000`. The Nova build assembles this image for 65C02 and enables
`ENGINE_CPU_65C02`, so engine code can use conditional 65C02 fast paths while
the vendored 6502 harness remains valid. Runtime BSS is capped below `$2000`;
engine BSS lives at `$2000-$3FFF`; engine code/data loads at `$4000` and must
stay below Nova's `$A000` MMIO window. The engine's 2 KB transposition table is
hosted at `$A200-$A9FF`. That keeps the engine out of the `$C000` runtime ROM
and leaves the opening book free to move to XRAM later.

Nova's pseudo-registers `$20-$2F` take priority. The Nova engine build remaps
the engine's old low-ZP scratch to `$50-$83`, and the runtime linker limits its
own ZP allocation to `$40-$4F` so future UI growth cannot silently collide with
the engine. Any future callback that needs FIO, pager, text, vsprite, or other
shared libraries should treat the pseudo-registers as ephemeral call scratch,
not persistent engine state.

The wrapper exports the stable engine API labels from `engine/api.s`:

- `ChessInitPieceLists`
- `ChessGenerateLegalMoves`
- `ChessFindBestMove`
- `ChessMakeMove`
- `ChessUnmakeMove`
- `ChessIsSquareAttacked`
- `ChessCheckKingInCheck`

Nova-specific adapter work still needed:

- implement real search timer hooks
- page the opening book from XRAM
- add cursor-square input on top of the current coordinate input
- map engine status to the side panel
- drive move animation from accepted engine moves

## Visual Passes

This scaffold uses compact 20x20 silhouettes inside 22x22 squares. Piece
buffers are built from a base mask plus a small detail mask: white pieces render
as white with dark-gray accents, and black pieces render as black with light-gray
accents. That keeps the asset format cheap while making the pieces read less
like flat blobs, especially the knight's horse-head silhouette. Once input and
engine integration start moving, we can replace the masks with richer multi-color
assets if the extra space is worth it.
