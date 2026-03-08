# 14-Voice Keyboard Visualizer Design

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Extend the existing 6-voice keyboard visualizer to support all 14 music engine voices (6 SID + 8 WTS), and auto-load a soundfont when MIDI playback needs WTS voices.

**Architecture:** Add 8 new contiguous note registers in the music status block, extend the 65C02 assembly visualizer to poll all 14, and add auto-soundfont loading to the MIDI play command.

**Tech Stack:** C# (.NET), 65C02 assembly (ca65/ld65), Avalonia

---

## 1. Register Layout

Reorganized music status block at `$BA50`:

```
$BA50       MusicStatus      (unchanged — bit 0=SFX, bit 1=music)
$BA51-$BA56 MusicNote1-6     (unchanged — SID voices 0-5)
$BA57-$BA5E MusicNote7-14    (NEW — WTS voices 6-13)
$BA5F-$BA60 MusicElapsedL/H  (moved from $BA57-$BA58)
$BA61-$BA62 MusicTotalL/H    (moved from $BA59-$BA5A)
```

All 14 note registers are contiguous at `$BA51-$BA5E`. The assembly polls them with a single indexed loop (`lda MusicNote1,x` with x=0..13).

C# changes:
- `VgcConstants.cs` — add `MusicNote7-14`, move elapsed/total constants
- `CompositeBusDevice.cs` — extend the read handler to serve notes 7-14 from `MusicEngine.GetVoiceNote(6..13)`
- `gen_novavm_inc.py` — regenerate `novavm.inc` from updated constants

## 2. Voice Color Mapping

14 voices, each with a unique color from the VGC's 16-color palette:

| Voice | Engine | Color        | Palette Index |
|-------|--------|--------------|---------------|
| 0     | SID    | Red          | 2             |
| 1     | SID    | Orange       | 8             |
| 2     | SID    | Yellow       | 7             |
| 3     | SID    | Green        | 5             |
| 4     | SID    | Cyan         | 3             |
| 5     | SID    | Blue         | 6             |
| 6     | WTS    | Brown        | 9             |
| 7     | WTS    | Light Red    | 10            |
| 8     | WTS    | Dark Gray    | 11            |
| 9     | WTS    | Medium Gray  | 12            |
| 10    | WTS    | Light Green  | 13            |
| 11    | WTS    | Light Blue   | 14            |
| 12    | WTS    | Light Gray   | 15            |
| 13    | WTS    | Purple       | 4             |

Uses all 14 non-black, non-white palette entries. SID voices keep existing colors.

## 3. Legend Layout

Two rows of voice labels:

```
Row 20:  █ S1  █ S2  █ S3  █ S4  █ S5  █ S6
Row 21:  █ W1  █ W2  █ W3  █ W4  █ W5  █ W6  █ W7  █ W8
```

Each label is 5 columns wide: color block + space + letter + digit + space. 6 SID labels = 30 columns, 8 WTS labels = 40 columns — both fit in 80 columns.

"S" prefix for SID voices, "W" for WTS voices.

Other chrome:
- Progress bar at y=145 (unchanged)
- Time display at row 19 (unchanged)
- "PRESS ANY KEY TO EXIT" at row 24, column 29 (centered on 80 columns)

## 4. Assembly Changes (keyboard.s)

1. `NUM_VOICES = 14`
2. `prev_notes: .res 14`
3. `voice_colors` — extend to 14 entries
4. `update_voices` — already uses `NUM_VOICES` and indexed addressing, works as-is
5. `draw_chrome` — rewrite legend to two rows with S/W prefixes
6. Center "PRESS ANY KEY TO EXIT" at column 29
7. `novavm.inc` — regenerate from updated VgcConstants

No changes to keyboard drawing, key painting, progress bar, or math routines.

## 5. Auto-Soundfont Loading

In `EmulatorTcpServer.CmdMidPlay`, when auto routing is selected and MIDI has >6 channels:

- Check `_bus.Wts.InstrumentCount == 0`
- If no instruments loaded, auto-load "GeneralUser_GS" from the soundfonts directory
- Reuse existing `FileIoController` soundfont load path (FIO registers + `FioCmdSfLoad`)
- If soundfont not found, silently fall back to SID-only 6-voice routing
- No error surfaced to the caller — just fewer voices

## 6. Testing

**C# unit tests:**
- New register addresses read correct voice note values for voices 6-13
- Moved elapsed/total registers work at new addresses
- Auto-soundfont triggers when WTS empty and MIDI has >6 channels
- Auto-soundfont falls back to SID-only when file not found

**Manual integration test:**
- Build updated keyboard.bin
- Play 14-channel MIDI (Sousa), run visualizer
- Confirm all 14 voices light up with distinct colors
- Confirm two-row legend (S1-S6, W1-W8)
- Confirm progress bar and time work at new addresses
- Confirm "PRESS ANY KEY TO EXIT" is centered

**Breaking change:** Only `MusicElapsedL/H` and `MusicTotalL/H` addresses move. The sole consumer is `keyboard.s` itself, which is updated in the same change.
