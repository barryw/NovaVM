# Nova Chess

Nova Chess is a standalone Nova runtime scaffold for the chess engine being
split out of the C64 chess project. It boots like NovaZ: `AUTOBOOT.bin` loads a
16K runtime into the primary `$C000` bank, swaps that bank in, and jumps through
the runtime reset vector.

The current checkpoint is visual and platform-facing:

- Nova-owned board layout on the VGC graphics plane.
- Blitter-backed virtual sprites for the pieces.
- Text overlay for turn, clocks, engine status, and move information.
- A vendored `chess6502-engine` build loaded from the NDI as `CHESSENG.BIN`.
- A `src/nchess_net.*` game-server adapter layer that can build chess
  `PLAYER_ACTION` messages and parse chess table events/state chunks.
- New-game menu modes for local one-player, local two-player,
  computer-vs-computer, and network games. The network path asks the generic
  Nova Game Server for typed opponents and can select AI providers such as
  Stockfish without hard-coding Stockfish as a game mode or carrying its own
  server hostname.

Engine layout:

- `NOVACHESS.BIN` remains the 16K runtime at `$C000`.
- `CHESSENG.BIN` is a normal FIO-loaded 65C02 binary with a `$4000` load
  address.
- Nova Chess runtime BSS is capped below `$2000`.
- Engine BSS lives at `$2000-$3FFF`.
- Engine code/data loads at `$4000` and must stay below Nova's `$A000` MMIO
  window.
- `$A200-$A9FF` is reserved for the engine transposition table.
- Nova pseudo-registers `$20-$2F` remain Nova-owned. The Nova engine build
  remaps the engine's old low-ZP scratch to `$50-$83`.
- Opening book support is intentionally stubbed and can later page data into
  XRAM without changing this runtime split.

Build:

```sh
make -C examples/novachess
make -C examples/novachess ndi
make -C examples/novachess test
```

The generated image is `examples/novachess/dist/fd0.ndi`.
The renderer animation fixture is `examples/novachess/dist/render-test.ndi`.
Tune its demo speed with `RENDER_STEP_PAUSE` and `RENDER_FINAL_PAUSE`.
