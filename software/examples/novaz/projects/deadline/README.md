# Deadline

NovaZ project for a local, user-supplied Deadline story file.

Expected local file:

```text
software/examples/novaz/projects/deadline/STORY.BIN
```

Build:

```sh
make -C software/examples/novaz ndi PROJECT=deadline
```

Test:

```sh
make -C software/examples/novaz test-project PROJECT=deadline
```

The generated image is written to `software/examples/novaz/dist/deadline/fd0.ndi`.
