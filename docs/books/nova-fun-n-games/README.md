# Nova Fun & Games Book

This folder contains the LaTeX source for `Nova Fun & Games`, a type-in
NovaBASIC book for the `fun_n_games.ndi` disk image.

The book uses the friendly style of early home-computer game books, but the
text, examples, structure, and visual treatment are original Nova material. Do
not copy prose, art, screenshots, or listings from older books into this tree.

## Build

```sh
cd docs/books/nova-fun-n-games
make pdf
```

## Source Relationship

- The shared framework listing is read from
  `../../programs/fun_n_games/common.bas`.
- The per-program starter listings are read from
  `../../programs/fun_n_games/stubs/*.bas`.
- The generated disk image is `../../programs/fun_n_games.ndi`.

The book should explain the machine, the framework, and each program in clear
Nova terms. The BASIC files remain the source of truth for the type-in listings.
