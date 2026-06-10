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
