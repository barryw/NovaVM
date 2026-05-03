---
title: "MIDPLAY"
type: command
category: Sound
keywords: [midi, music, play]
syntax: "MIDPLAY \"filename\""
see_also: [midstop, sfload, sound-and-music]
---

Load and play a standard MIDI file through the SID music engine.

```basic
10 MIDPLAY "entertainer"
20 IF PLAYING THEN GOTO 20
30 PRINT "DONE"
```

The engine auto-selects the busiest MIDI channels, maps GM instruments to SID
waveforms, and converts MIDI velocity to per-voice volume. Channel 10 drums use
the noise waveform.

See the [Sound And Music](/docs/help/guides/sound-and-music.md) guide for more details.
