# Nova Showcase Demo Disk Curation

## Goal

Build a new curated Nova demo disk that feels intentional, polished, and
worth showing to someone who has never seen the machine before.

The current `docs/programs/demo.ndi` is broad, but it reads more like an asset
dump than a showcase. The new disk should be smaller, stronger, and tested on
the ULX3S.

## Success Criteria

- The disk boots into a clean demo browser or launcher without manual typing.
- The first highlighted item is immediately impressive.
- Every featured item has a clear visual reason to be on the disk, not just an
  audio reason.
- Music playback hands off to the keyboard visualizer and exits cleanly.
- MIDI files show title, WTS font, key activity, and elapsed/total time.
- SID files show title, chip/model/stereo state, voice legend, key activity,
  and elapsed time.
- Visuals show more than basic copper raster bars: use sprites, blitter/DMA,
  VTEXT, palette changes, keyboard animation, and music-reactive motion.
- Exiting any track stops audio.
- HTTP control remains responsive while music is playing.
- Every selected item is tested on real ULX3S hardware.

## Packaging Notes

- Use the current `assembly/apps/keyboard/keyboard.bin`; the existing
  `docs/programs/demo.ndi` contains an older keyboard binary.
- MIDI should be imported with the Nova CLI so `.mid` files become Nova Music
  Stream data. The known-good test path used `sousa-stars-stripes.mid` imported
  as an NMS stream.
- Keep a first showcase disk focused. A dense library disk can come later.
- The existing assembly demo browser is useful, but its fixed categories
  (`classical`, `movies`, `tv`, `games`, `sid`) are not ideal for a curated
  first impression. A `featured` category would make the disk stronger.

## Curation Bar

Songs should earn their spot. A famous title is not enough.

Each featured track should pass most of these checks:

- Strong first 10 seconds.
- Clean playback on the current default soundfont or SID path.
- Distinctive hardware angle: 8580 stereo SID, 6581 character, WTS sample
  playback, dense MIDI note motion, percussion, arpeggios, or wide arrangement.
- Good keyboard visualizer behavior: keys move in a readable way rather than
  sitting idle or flickering as noise.
- Visual pairing is obvious: the track suggests motion, color, structure, or a
  scene.
- No long loading delay unless the disk shows a polished loading state.

Cut aggressively. A small disk of excellent pieces is better than a big disk of
mixed-quality nostalgia.

## Visual Showcase Direction

The current copper bars are fine as a background accent, but not enough for the
new disk. The stronger direction is to give the music player several visual
presets and pick one per track.

| Visual | Hardware Shown | Best For |
| --- | --- | --- |
| Keyboard visualizer | Music metadata, SID/WTS note tracking, text + graphics | MIDI pieces and melodic SID tracks. |
| Stereo SID scope | Per-voice SID registers, 2SID activity, palette accents | `Love_Fileosophy_2SID.sid`, `Fratres_2SID.sid`. |
| Sprite dance floor | Hardware sprites, sprite priority, math/circular motion | Beat-heavy SID tracks. |
| Multiplex swarm | Copper rewriting sprite registers, many on-screen sprites | Fast game/anime-style tracks. |
| Blitter equalizer | DMA/blitter fills and copies, large fast screen changes | Percussive SID/MIDI tracks. |
| VTEXT terminal wall | VTEXT regions, scrolling status, metadata panes | Browser/menu and slower tracks. |
| Logo/title scene | Copper palette splits plus blitter/sprite logo motion | Opening sequence and disk attract mode. |

The first build can still use the keyboard visualizer for all music, but the
target disk should feel like a sequence of short audiovisual scenes, not a file
browser with a keyboard overlay.

## Proposed Disk Shape

- **Attract/opening:** Nova logo, short loading state, strong music sting.
- **Featured:** 8-12 hand-picked tracks with assigned visual presets.
- **SID:** smaller secondary list of proven SID files.
- **MIDI/WTS:** smaller secondary list of proven MIDI files.
- **Tech:** standalone visual demos: sprite multiplex, blitter/DMA, VTEXT, math
  sprite motion.
- **About:** credits, controls, hardware feature list.

## First-Pass Featured Set

These are the strongest candidates to test first.

| Track | Type | Why |
| --- | --- | --- |
| `Love_Fileosophy_2SID.sid` | SID, 8580, stereo | Catchy beat, great first impression, already tested successfully. Pair with stereo SID scope or sprite dance floor. |
| `Fratres_2SID.sid` | SID, 8580, stereo | Shows stereo SID can do something beautiful and musical. Pair with calm scope/keyboard visuals. |
| `Popel_Premiere_2SID.sid` | SID, RSID/2SID | Good stress test for the player and metadata path. Pair with multiplex swarm if it plays cleanly. |
| `tubular_bells_ii_shake_airwolf_style.sid` | SID, 8580 | Strong recognizable hook and 8580 coverage. Pair with keyboard plus split palette. |
| `commando.sid` | SID, 6581 | Classic Rob Hubbard energy. Pair with sprite/multiplex motion. |
| `monty-on-the-run.sid` | SID, 6581 | Another essential Hubbard showcase. Pair with blitter equalizer. |
| `parallax.sid` | SID, 6581 | Strong Martin Galway candidate. Pair with scope plus VTEXT metadata. |
| `wizball.sid` | SID, 6581 | Classic Galway sound, good for melody tracking. Pair with keyboard visualizer. |
| `sousa-stars-stripes.mid` | MIDI/WTS | Already tested with timer, metadata, and keyboard visualizer. Pair with keyboard visualizer. |
| `bach-toccata-dm.mid` | MIDI/WTS | Dramatic keyboard activity. Pair with keyboard visualizer as a feature piece. |
| `joplin-entertainer.mid` | MIDI/WTS | Fun, readable piano motion. Pair with keyboard visualizer and blitter title cards. |
| `debussy-clair-de-lune.mid` | MIDI/WTS | Contrast piece; shows softer playback. Pair with subdued VTEXT/keyboard visuals. |
| `miami-vice.mid` | MIDI/WTS | Synth-heavy pop/TV vibe. Pair with sprite dance floor or blitter equalizer. |
| `tetris-theme.mid` | MIDI/WTS | Short, recognizable, good keyboard motion. Pair with block/tile blitter visuals. |
| `castlevania-bloody-tears.mid` | MIDI/WTS | Game-music energy. Pair with multiplex swarm. |
| `star-wars.mid` | MIDI/WTS | Recognizable orchestral-style test. Pair with logo/title scene if WTS sounds strong enough. |

## SID Bench Queue

These should be auditioned after the featured set because they may be great,
but need real-hardware confirmation.

- `crazy-comets.sid`
- `delta.sid`
- `cybernoid.sid`
- `cybernoid-2.sid`
- `sanxion.sid`
- `lightforce.sid`
- `master-of-magic.sid`
- `intl-karate.sid`
- `ocean-loader-2.sid`
- `giana-sisters.sid`
- `last-ninja.sid`
- `r-type.sid`
- `turbo-outrun.sid`
- `to-be-on-top.sid`

## MIDI Bench Queue

- `pink-panther.mid`
- `mission-impossible.mid`
- `grieg-mountain-king.mid`
- `beethoven-fur-elise.mid`
- `beethoven-moonlight-1.mid`
- `chopin-nocturne-op9n2.mid`
- `mozart-eine-kleine.mid`
- `pachelbel-canon.mid`
- `zelda-overworld.mid`
- `sonic-green-hill.mid`
- `super-mario-bros.mid`

## Known Issues To Fix Before Final Disk

- Debug keyboard input can duplicate stale characters on the first typed line.
  This affects test automation and manual launch reliability.
- The old demo image needs to be rebuilt with the latest keyboard visualizer.
- Decide whether the showcase disk uses the existing five-category browser or a
  revised browser with `featured`, `sid`, `midi`, `games`, and `graphics`.
- Confirm the root autoboot naming convention for the new image. The design doc
  calls for `AUTOBOOT.bin`, while the current image contains `demo.bin`.
- Confirm every selected SID exits cleanly and stops audio.
- Add a polished loading state driven by the music loading flag so large
  soundfont/NMS loads do not look frozen.
- Decide how much visual work runs inside the keyboard visualizer versus a new
  higher-level demo scene/player.

## Hardware Test Checklist

For every final selected track:

1. Boot the disk from cold start.
2. Start the track from the demo browser.
3. Confirm metadata is human-readable.
4. Confirm the keyboard visualizer draws cleanly.
5. Confirm key indicators move.
6. Confirm MIDI elapsed/total time or SID elapsed time is correct.
7. Let it play for at least 30 seconds.
8. Confirm HTTP `/health` and `/audio-status` respond during playback.
9. Exit with `ESC` or `Q`.
10. Confirm audio stops and the browser returns.
