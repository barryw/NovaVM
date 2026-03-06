---
title: "VOLUME"
type: command
category: Sound
keywords: [volume, sound, voice]
syntax: "VOLUME level [, voice]"
see_also: [sound-and-music]
---

Set SID volume.

- `VOLUME level` -- set master volume (0--15).
- `VOLUME level, voice` -- set per-voice volume (0--15) on voice 1--6.

Both master and per-voice volume share the same 0--15 range, matching the
4-bit resolution of the original SID. Per-voice volume is a NovaBASIC
extension not present on the real MOS 6581. Default per-voice volume is 15
(full). Setting it to 0 silences that voice without affecting others.

```basic
10 VOLUME 15           : REM MASTER VOLUME MAX
20 VOLUME 12, 1        : REM VOICE 1 SLIGHTLY REDUCED
30 VOLUME 5, 2         : REM VOICE 2 QUIET
```

See the [Sound And Music](/docs/help/guides/sound-and-music.md) guide for more details.
