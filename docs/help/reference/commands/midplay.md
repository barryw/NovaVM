---
title: "MIDPLAY"
type: command
category: Sound
keywords: [midi, music, play]
syntax: "MIDPLAY \"filename\""
see_also: [midstop, sound-and-music]
---

Load and play a standard MIDI file through the SID music engine.

`MIDPLAY "song"` -- load `song.mid` from `~/e6502-programs` and start playback.

The engine auto-selects the 6 busiest MIDI channels, maps GM instruments
to SID waveforms, and converts MIDI note velocity to per-voice volume.
Channel 10 (drums) is mapped to the noise waveform.

MIDI playback uses the MusicEngine (voices 1--6). Use `MIDSTOP` to stop.

```basic
10 MIDPLAY "entertainer"
20 REM WAIT FOR FINISH
30 IF PLAYING THEN GOTO 30
40 PRINT "DONE"
```

See the [Sound And Music](/docs/help/guides/sound-and-music.md) guide for more details.
