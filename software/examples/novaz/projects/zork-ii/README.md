# Zork II

NovaZ project for a local, user-supplied Zork II story file.

Expected local file:

```text
software/examples/novaz/projects/zork-ii/STORY.BIN
```

Build:

```sh
make -C software/examples/novaz ndi PROJECT=zork-ii
```

The generated image is written to `software/examples/novaz/dist/zork-ii/fd0.ndi`.
