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

## M1 status (text-only V6)

This project runs against the M1 V6 milestone: the NOVAZ6 segment provides the
8-window model, user stacks, and honest no-graphics stubs (`picture_data`
branches "not available", `draw_picture`/`erase_picture` are no-ops). Zork Zero
boots to its first prompt and accepts commands in text-only mode; the pictures,
borders, and the graphical status area are M3 scope. Layout is rougher than
the original — that is expected at M1.

Known M1 limitations (captured during bring-up, for the M2/M3 reader):

- **Bottom-anchored layout** — window 0 spans all 50 rows and the game writes
  near the bottom, leaving the top ~37 rows blank until M2 window sizing.
- **Stale line ends** — long responses can leave fragments of earlier text at
  the end of wrapped lines (no partial-line erase yet; M2 erase work).
- **Premature ready-detection** — the game's event pump polls `read_char`
  between output paragraphs, so the smoke runner may see "ready" before a
  response finishes; `smoke.txt` pins only the first line of each response.
- **Step budget** — booting the 300KB story is paging-bound and needs the
  larger `NOVAZ_SMOKE_MAX_STEPS` exported by `project.mk` (~200-300M steps).
