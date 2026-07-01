# Zork III

NovaZ project for a local, user-supplied Zork III story file.

Expected local file:

```text
software/examples/novaz/projects/zork-iii/STORY.BIN
```

Build:

```sh
make -C software/examples/novaz ndi PROJECT=zork-iii
```

The generated image is written to `software/examples/novaz/dist/zork-iii/fd0.ndi`.
