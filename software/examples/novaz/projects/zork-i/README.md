# Zork I

NovaZ project for a local, user-supplied Zork I story file.

Expected local file:

```text
software/examples/novaz/projects/zork-i/STORY.BIN
```

Build:

```sh
make -C software/examples/novaz ndi PROJECT=zork-i
```

The generated image is written to `software/examples/novaz/dist/zork-i/fd0.ndi`.
