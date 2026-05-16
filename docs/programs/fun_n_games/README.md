# Nova Fun & Games

This directory holds the source listings for `fun_n_games.ndi`, a NovaBASIC
type-in style disk inspired by *Commodore 64 Fun and Games*.

- `common.bas` is the shared framework. It starts at line 60000 and owns all
  `Z*` variables.
- `stubs/*.bas` are short game listings. Each sets `ZT$`, `ZA$`, `ZS$`, and
  `ZP`, then calls `GOSUB 60000`.
- `../fun_n_games.ndi` is the generated disk image. Its game files include the
  stub plus the common lines, so each one can be loaded and run directly.

Rebuild the disk image from the repository root with:

```sh
bash docs/programs/fun_n_games/build.sh
```

NovaBASIC accepts longer variable names, but only the first two characters are
significant. Game code should use one- or two-character names and avoid `Z*`.
