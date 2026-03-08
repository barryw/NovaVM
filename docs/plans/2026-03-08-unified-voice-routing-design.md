# Unified Voice Routing Design

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Unify SID and WTS into a single coherent music engine where instrument selection determines the engine, MIDPLAY supports explicit voice-to-channel mapping, and the user has clear, deterministic control over routing.

**Architecture:** Fixed engine boundaries (voices 1-6 SID, 7-14 WTS), unified MML instrument commands, C#-side MIDPLAY mapping parser.

**Tech Stack:** C# (.NET), 65C02 assembly (ca65/ld65), Avalonia

---

## 1. Unified Voice Model

14 voices with fixed engine assignment:
- Voices 1-6: SID (3 per chip — 1-3 on SID1, 4-6 on SID2)
- Voices 7-14: WTS (8 wavetable voices)

Internally 0-indexed (0-5 SID, 6-13 WTS), exposed as 1-14 to BASIC and MML.

Three MML instrument commands:
- `I<0-15>` — select SID instrument slot (voices 1-6 only)
- `@I<0-127>` — select WTS GM program (voices 7-14 only)
- `@D<0-127>` — select WTS drum kit (voices 7-14 only, bank 128)

Wrong type for a voice is silently ignored (e.g., `@I` on voice 3 does nothing).

`INSTRUMENT` BASIC command remains SID-only, unchanged.

## 2. MIDPLAY Voice Mapping

Default auto-routing: 8 busiest MIDI channels → WTS (voices 7-14), overflow → SID (voices 1-6).

New optional syntax for explicit overrides:

```
MIDPLAY "song"                         : REM auto-route (current behavior)
MIDPLAY "song,7=0,8=1"                 : REM map voice 7→ch 0, voice 8→ch 1
MIDPLAY "song,1=9,7=0,8=1,9=2"         : REM mix: SID voice 1 gets ch 9, WTS gets 0,1,2
```

Format: `<voice>=<midi_channel>` pairs, comma-separated after the filename. Voice numbers are 1-14 (BASIC convention). MIDI channels are 0-15.

Behavior:
- Explicitly mapped voices are assigned first
- Remaining MIDI channels auto-route to remaining voices (busiest first, WTS preferred)
- SID voices (1-6) get SID instrument buckets automatically
- WTS voices (7-14) get GM program from the MIDI channel's program change events
- Drum channel (MIDI ch 9) on a WTS voice automatically uses bank 128

Auto-soundfont loading: if any WTS voices are used and no soundfont is loaded, auto-load GeneralUser_GS from `~/e6502-programs/soundfonts/GeneralUser_GS.sf2`. If not found, WTS voices fall back silently.

## 3. C#-Side MIDPLAY String Parsing

FileIoController's `DoMidPlay()` parses the mapping from the filename string — no ROM changes needed.

- Split on first `,` — left side is filename, right side is mapping pairs
- Parse pairs: `7=0,8=1,1=9` → dictionary of voice(1-14) → MIDI channel(0-15)
- Voice numbers converted from 1-based to 0-based internally
- Pass mapping to `MidiEngine.RouteVoices` as explicit assignments

The ROM doesn't change — it already sends the full evaluated string expression. BASIC string concatenation works:
```
10 M$="7=0,8=1"
20 MIDPLAY "song,"+M$
```

TCP server's `mid_play` command gets the same voice=channel semantics via its `voices` JSON parameter.

## 4. MML Parser Changes

**Remove:** `@W<n>` command (replaced by `@I<n>`)

**Add:** `@I<0-127>` — emits `SetWtsInstrument` event with GM program number

**Add:** `@D<0-127>` — emits `SetWtsInstrument` event with `(128 << 8) | program` encoding

**Keep:** `I<0-15>` — unchanged, emits `SetInstrument` for SID

`@W` was only used internally by MIDPLAY, never in user-written MML. Zero backward-compatibility impact.

## 5. MusicEngine Voice Routing

`IsWtsVoice(v)` stays as `v >= 6` — fixed boundary.

Event processing per-voice:
- `SetInstrument` (from `I<n>`) — apply only on SID voices (0-5), ignore on WTS
- `SetWtsInstrument` (from `@I<n>` / `@D<n>`) — apply only on WTS voices (6-13), ignore on SID
- SID-specific effects (`@P`, `@F`, `@PS`, `@FS`) — ignored on WTS voices
- Universal effects (volume, tempo, vibrato, portamento, arpeggio) — work on both engines

Same MML loaded into different voices produces different results — SID commands are no-ops on WTS and vice versa.

## 6. Default Soundfont

GeneralUser GS by S. Christian Collins. License permits bundling with software projects, commercial or otherwise. Credit the author in code and documentation.

Auto-load mechanism (`MidiAutoSoundfont.TryLoad`) is unchanged.

## 7. Testing

**Unit tests:**
- MML parser: `@I<n>` emits `SetWtsInstrument` with correct program
- MML parser: `@D<n>` emits `SetWtsInstrument` with bank 128 encoding
- MML parser: `@W` no longer recognized
- MusicEngine: `SetInstrument` ignored on WTS voice
- MusicEngine: `SetWtsInstrument` ignored on SID voice
- FileIoController: `"song,7=0,8=1"` parses filename and mapping correctly
- FileIoController: `"song"` (no mapping) auto-routes as before
- MidiEngine.RouteVoices: explicit voice mappings respected, remainder auto-routed

**Manual integration tests:**
- `MIDPLAY "stars-stripes"` — auto-routes, sounds correct
- `MIDPLAY "stars-stripes,7=0,8=1,9=2"` — explicit WTS mapping
- `MIDPLAY "stars-stripes,1=0,7=1"` — mix of SID and WTS
- MML with `@I0` on voice 7 plays piano via WTS
- MML with `@D0` on voice 7 plays drums via WTS
- MML with `I1` on voice 1 plays SID instrument
- MML with `@I0` on voice 1 is silently ignored
