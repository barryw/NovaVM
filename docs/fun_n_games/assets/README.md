# Nova Fun & Games Cover Assets

Save the supplied front-cover artwork as:

```text
docs/fun_n_games/assets/front-cover.png
```

The LaTeX cover uses that image as a full-bleed page. If the file is missing,
the book falls back to the simple TikZ placeholder cover so the PDF can still
be built in a clean checkout.

An optional back-cover image can be saved as:

```text
docs/fun_n_games/assets/back-cover.png
```

If it is missing, the book uses the generated Nova-style back cover in
`../backcover.tex`.
