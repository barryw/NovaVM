# Arthur

NovaZ project for a local, user-supplied Arthur (V6) story file and graphics
Blorb.

Expected local files, both gitignored:

```text
software/examples/novaz/projects/arthur/STORY.BIN   # ARTHUR.Z6
software/examples/novaz/projects/arthur/PICS.BLB    # Arthur.blb
```

Build with local files:

```sh
make -C software/examples/novaz ndi PROJECT=arthur
```

Or point directly at the archive copy:

```sh
make -C software/examples/novaz ndi PROJECT=arthur \
  STORY="/Volumes/Software/Emulation/Infocom/Arthur/ARTHUR.Z6" \
  PICTURES="/Volumes/Software/Emulation/Infocom/Arthur/Arthur.blb"
```

The generated image is written to `software/examples/novaz/dist/arthur/fd0.ndi`.
