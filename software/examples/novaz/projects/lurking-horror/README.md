# The Lurking Horror

NovaZ project for a local, user-supplied The Lurking Horror story file, packaged
with its sampled sounds so `sound_effect` plays the real Blorb audio.

Expected local files:

```text
software/examples/novaz/projects/lurking-horror/STORY.BIN   # or pass STORY=...
Lurking.blb                                          # the sampled-sound Blorb
```

The story (`Lurking.z3`) and its sounds (`Lurking.blb`) ship together on the IF
Archive (`infocom/media/sound/`). `Lurking.blb` carries 14 samples — numbers
3,4,6-13,15-18 — which are exactly the `sound_effect` numbers the game emits
(tomb hatch, the Inner Lair creature, the endgame, and so on; most are deep in
the game). To audition them without a walkthrough, use the jukebox image:

```sh
make -C software/examples/novaz jukebox-lurking
```

Build the game itself (story + sounds):

```sh
make -C software/examples/novaz ndi PROJECT=lurking-horror STORY="/path/to/Lurking.z3"
```

`PROJECT_SOUNDS` defaults to `$(LURKING_SOUNDS)`; override it if your `.blb`
lives elsewhere.

Test:

```sh
make -C software/examples/novaz test-project PROJECT=lurking-horror STORY="/path/to/Lurking.z3"
```
