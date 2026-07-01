# Journey

NovaZ project for a local, user-supplied Journey (V6) story file and graphics
Blorb.

Expected local files, both gitignored:

```text
software/examples/novaz/projects/journey/STORY.BIN   # Journey.z6
software/examples/novaz/projects/journey/PICS.BLB    # Journey.blb
```

Build with local files:

```sh
make -C software/examples/novaz ndi PROJECT=journey
```

Or point directly at the archive copy:

```sh
make -C software/examples/novaz ndi PROJECT=journey \
  STORY="/Volumes/Software/Emulation/Infocom/Journey/Journey.z6" \
  PICTURES="/Volumes/Software/Emulation/Infocom/Journey/Journey.blb"
```

The generated image is written to `software/examples/novaz/dist/journey/fd0.ndi`.
