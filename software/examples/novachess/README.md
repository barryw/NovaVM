# Nova Chess

Nova Chess is a standalone Nova runtime for the shared Caissa engine in the
sibling `caissa` repository. The same C board, move generator, evaluation, and
search now build for both the stock C64 and Nova. It boots like NovaZ:
`AUTOBOOT.bin` loads a
16K runtime into the primary `$C000` bank, swaps that bank in, and jumps through
the runtime reset vector.

The current checkpoint is visual and platform-facing:

- Nova-owned board layout on the VGC graphics plane.
- Blitter-backed virtual sprites for the pieces.
- Text overlay for turn, clocks, engine status, and move information.
- The shared Caissa engine, built by llvm-mos and loaded from the NDI as
  `CHESSENG.BIN` (full search) or `CHESSRUL.BIN` (rules-only).
- A `src/nchess_net.*` game-server adapter layer that can build chess
  `PLAYER_ACTION` messages and parse chess table events/state chunks.
- New-game menu modes for local one-player, local two-player,
  computer-vs-computer, and network games. The network path asks the generic
  Nova Game Server for typed opponents and can select AI providers such as
  Stockfish without hard-coding Stockfish as a game mode or carrying its own
  server hostname.

Engine layout:

- `NOVACHESS.BIN` remains the 16K runtime at `$C000`.
- Each engine has a 27,024-byte low PRG payload (`CHESSENG.BIN` / `CHESSRUL.BIN`) for
  `$3670-$9FFF` (fixed ABI at `$4000`) and a raw 16K bank file (`CHESSENX.BIN` / `CHESSRUX.BIN`)
  paged into bank 1 at `$C000-$FFFF`.
- The runtime stages that banked image in its private `$060000` XRAM shelf slot;
  NovaChess does not use the shared libcall module cache concurrently.
- Nova Chess runtime BSS is capped below `$2000`.
- Engine BSS and its downward-growing software stack share `$2000-$366F`.
- Engine code/data loads at `$3670` and must stay below Nova's `$A000` MMIO
  window.
- The rules-only low image is link-checked below `$6000`, where the network
  overlay begins.
- The full engine reserves a 40 KiB, 4096-entry exact TT from the allocator-managed low-XRAM page
  bitmap. It falls back cleanly to no TT if no contiguous block is available.
- Nova pseudo-registers remain Nova-owned. Caissa uses `$50-$6F` for llvm-mos
  imaginary registers while the engine is active.
- The engine includes a deterministic 103-position opening book, generated from
  the shared verified Caissa repertoire.

Build:

```sh
make -C software/examples/novachess
make -C software/examples/novachess ndi
make -C software/examples/novachess test
```

By default the Makefile expects Caissa at the sibling path `../caissa` from the
NovaVM repository. Override `CAISSA_ROOT=/absolute/path/to/caissa` when needed.
Easy, Medium, Hard, and Expert are wall-clock effort budgets of 5, 15, 45, and
120 seconds. They do not deliberately weaken move choice. Nova always completes
a full-width depth-2 floor before honoring the deadline, then returns only the
last completed iterative-deepening result.

The generated image is `software/examples/novachess/dist/fd0.ndi`.
The renderer animation fixture is `software/examples/novachess/dist/render-test.ndi`.
Tune its demo speed with `RENDER_STEP_PAUSE` and `RENDER_FINAL_PAUSE`.
