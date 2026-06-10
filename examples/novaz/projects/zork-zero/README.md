# Zork Zero

NovaZ project for a local, user-supplied Zork Zero (V6) story file.

Expected local file:

```text
examples/novaz/projects/zork-zero/STORY.BIN
```

Build:

```sh
make -C examples/novaz ndi PROJECT=zork-zero
```

The generated image is written to `examples/novaz/dist/zork-zero/fd0.ndi`.

## M2 status (real window rendering, text-only V6)

This project runs against the M2 V6 milestone: units are cells, and the
window property table drives real vtext regions — Zork Zero's own layout
renders as designed: window 1 = 5-row full-width banner (rows 0-4), window 0
= 45x70 playfield inset at (5,5) with the game's 1-cell left margin, prompt
at the window bottom row. Text scrolls inside the playfield rect with vacated
rows blanked — the M1 bottom-anchored layout and stale-line-end artifacts are
gone — and `scroll_window` is real. `smoke.txt` + `project.mk` pin the layout
with absolute-cell `--expect-at` / `.expect-at` asserts.

Remaining limitations (verified against the post-M2 capture, dfrotz as the
text-only reference, and the published Zork Zero ZIL source — see the
"post-M2 capture" section of `docs/plans/2026-06-10-zork-zero-window-ops.md`):

- **Pictures are M3 scope** — `picture_data` still gives the honest
  "no pictures" answer; `draw_picture` is a no-op. Borders, title art, and
  the graphical status area need the picture file + cell→pixel blitting.
- **Garbled banner detail is game-side** — the game positions every banner
  element from picture metadata (`PICINF` results feed the `CURSET` calls in
  `INIT-STATUS-LINE`/`UPDATE-STATUS-LINE`, `globals.zil`). With no pictures
  those metrics are absent and the labels collide ("Moves:Sc3" on row 1).
  dfrotz renders the same garble; our `set_cursor` out-of-range clamp matches
  the Infocom YZIP spec ("it will be set to the appropriate dimension of the
  window"). Not our bug; M3's real picture metrics fix it.
- **Boot newline storm (game-side)** — with the drop-cap picture missing the
  game arms its CR-interrupt countdown (window prop 9) with -1, and its
  `CLEAR-CRCNT` routine (`prologue.zil`) then reads the countdown back and
  prints that many newlines: a ~65.5K-newline flood ending the prologue.
  dfrotz exhibits the same blank-line flood. Through our 45-row playfield
  that is floor(65525/44) full pages = exactly the **1489** boot `[ MORE ]` prompts the
  smoke auto-answers (M1's 1337 was the same storm through 49-row pages —
  the old capture's "stale-MORE harness spam" theory, finding 9, was wrong).
  The storm is why `NOVAZ_SMOKE_MAX_STEPS` stays large (~480M steps).
- **CR-interrupt countdown not decremented** — per the YZIP spec the
  interpreter decrements prop 9 before each newline and calls the prop-8
  routine when it hits zero (Frotz implements this, with a Zork Zero r393
  ordering quirk). M2 stores the props verbatim and never fires (per-plan
  de-scope). In text-only mode the only observable effect is the storm
  running 65535 instead of ~65525 newlines; in M3 the countdown becomes
  load-bearing (drop-cap margin release via `RESET-MARGIN`).
- **Premature ready-detection** — the game's event pump polls `read_char`
  between output paragraphs, so the smoke runner may see "ready" before a
  response finishes; `smoke.txt` pins only the first line of each response.
