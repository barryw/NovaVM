---
title: "SOUND"
type: command
category: Sound
keywords: [sound]
syntax: "SOUND note,dur[,inst]"
see_also: [sound-and-music]
---

Start a fire-and-forget SID note. *note* is a MIDI note byte, *dur* is the
duration in video frames, and optional *inst* selects instrument preset 0--15
(default 0). Call `VSYNC` in loops so NovaBASIC can advance durations and gate
notes off on time.

See the [Sound And Music](/docs/help/guides/sound-and-music.md) guide for more details.
