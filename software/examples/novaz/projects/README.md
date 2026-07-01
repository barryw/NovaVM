# NovaZ Story Projects

Each directory under `projects/` is a separate Nova story-disk project. The
shared Nova-specific NovaZ runtime lives in `../src`; project directories only
describe the disk image we want to build around that runtime.

Story files are intentionally not committed. Place a legally owned story file
at `STORY.BIN` inside the project directory, then build with:

```sh
make -C software/examples/novaz ndi PROJECT=zork-i
```

The packer imports the local story as `story.bin` in the generated `.ndi`.
The runtime should identify the story by its Z-machine header, not by a special
NDI file type or filename extension.

Projects may also provide `smoke.txt`, a scripted command transcript for that
specific game. Run it with:

```sh
make -C software/examples/novaz test-project PROJECT=zork-iii
```

When the local Infocom archive is mounted, the checked-in project smoke scripts
can be run without copying story files into the project directories:

```sh
make -C software/examples/novaz test-infocom-z3-smokes
make -C software/examples/novaz test-infocom-v45-smokes
make -C software/examples/novaz test-infocom-smokes
```

Pass `PROJECT_SMOKE_ARGS=--generic-boot --expect-more` when a game script is
intended to force and verify the `[ MORE ]` pagination prompt.

Deadline is included as a time-game regression. Its smoke runs with
`--expect-time-status`, verifies that the runtime recognizes the Z3 time-status
header bit, and keeps the parser running across time-advancing turns.
