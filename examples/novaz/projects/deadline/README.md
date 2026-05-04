# Deadline

NovaZ project for a local, user-supplied Deadline story file.

Expected local file:

```text
examples/novaz/projects/deadline/STORY.BIN
```

Build:

```sh
make -C examples/novaz ndi PROJECT=deadline
```

Test:

```sh
make -C examples/novaz test-project PROJECT=deadline
```

The generated image is written to `examples/novaz/dist/deadline/fd0.ndi`.
