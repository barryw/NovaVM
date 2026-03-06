# Keyboard Visualizer Design

**Date:** 2026-03-05
**Status:** Draft

## Overview

A reusable 6502 assembly program that displays an 8-octave piano keyboard and animates note indicators in real-time. Works with any music source — MML (MUSIC PLAY), MIDI (MIDPLAY), or SID (SIDPLAY). The visualizer is completely source-agnostic: it reads hardware status registers and knows nothing about how music is produced.

## Design Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Where to load | High BASIC RAM at $9000 | Proven C64-era pattern; 4KB budget is generous |
| Keyboard layout | 8 octaves, 2 rows of 4 | Matches existing BASIC programs exactly |
| Note indicators | Sprites (hybrid approach) | 6 voices = 6 sprites; smooth overlay without redraw |
| Key depression | 2 sprite shapes per voice | White-pressed + black-pressed; swap by note type |
| Player controls | Moderate (v1) | Title, elapsed/total time, progress bar, voice legend |
| Exit condition | Music stops OR keypress | Both checked each frame |
| Assembly tooling | ca65/ld65 (already used for ROM) | Shared `novavm.inc` for all future assembly work |
| Include file maintenance | Auto-generated from VgcConstants.cs | Python script; no manual maintenance |
| File extension | `.bin` for binary executables | Distinct from `.bas` (BASIC) and `.sid` (music) |

## Interface Contract

The visualizer's only interface to the music system is six registers:

| Address | Name | Description |
|---------|------|-------------|
| $BA50 | MusicStatus | Bit 0 = SFX playing, Bit 1 = music playing |
| $BA51-$BA56 | MusicNote1-6 | Per-voice MIDI note (0 = silent, 1-127 = note) |
| $BA57-$BA58 | MusicElapsedL/H | Frames elapsed since playback start (60Hz) |
| $BA59-$BA5A | MusicTotalL/H | Total frames for current piece (0 = unknown) |

All three music sources (MML, MIDI, SID) must populate these registers. The visualizer never touches SID registers, MML data, or MIDI state directly.

### Source-specific notes

- **MML (MUSIC PLAY):** MusicEngine already populates $BA51-$BA56. Add frame counters.
- **MIDI (MIDPLAY):** MidiPlayback uses MusicEngine's DirectNoteOn/Off. Verify note registers reflect this. Total frames computable from MIDI timeline.
- **SID (SIDPLAY):** SidPlayer drives SidChip directly, bypassing MusicEngine. Must reverse-map SID frequency registers to closest MIDI note and populate $BA51-$BA53. Frequency-to-note: `round(12 * log2(freq / 440) + 69)`. Won't be perfect for detuned/gliding voices — closest key lights up, which is fine.

## Memory Layout

```
$0000-$01FF  Zero page + stack (visualizer borrows a few ZP bytes)
$0200-$027F  Vector table (untouched)
$0280-$8FFF  BASIC program space (reduced from $9FFF)
$9000-$9FFF  Keyboard visualizer binary (~4KB)
$A000+       Hardware registers, ROM (untouched)
```

BASIC programs set top-of-memory below $9000 before loading:
```
POKE $0073,$00 : POKE $0074,$90   ; set Rone (top of memory) to $9000
```
(Exact ZP locations TBD from EhBASIC source.)

## Invocation

```basic
10 REM set up music (INSTRUMENT, MUSIC, etc.)
20 MUSIC PLAY
30 LOAD "KEYBOARD", $9000
40 SYS $9000
50 REM returns here when music ends or key pressed
```

Also works with MIDI and SID:
```basic
10 MIDPLAY "bach-fugue"
20 LOAD "KEYBOARD", $9000
30 SYS $9000
```

## Screen Layout

320x200 graphics, mode 1 (graphics over text):

```
y=0-8       Title bar: song name, decorative lines (text layer)
y=36-76     Upper keyboard: C0-B3 (28 white keys, 11px wide, 40px tall)
y=90-130    Lower keyboard: C4-B7 (28 white keys, same dimensions)
y=140-170   Player chrome: progress bar, time, voice legend
y=176-199   Status: "Press any key to exit"
```

### Keyboard dimensions (matching current BASIC programs)

- White key: 11px wide, 40px tall
- Black key: 7px wide, 24px tall
- Keyboard X origin: 6px
- Key border color: gray (11)
- White key color: white (1)
- Black key color: black (0)
- Octave labels: dark gray (8), positioned below each octave

## Sprite System

### Shape allocation (12 of 256 slots)

| Slot | Purpose | Voice | Color |
|------|---------|-------|-------|
| 0 | White key pressed | Voice 1 | Red (2) |
| 1 | White key pressed | Voice 2 | Orange (8) |
| 2 | White key pressed | Voice 3 | Yellow (7) |
| 3 | White key pressed | Voice 4 | Green (5) |
| 4 | White key pressed | Voice 5 | Cyan (3) |
| 5 | White key pressed | Voice 6 | Blue (6) |
| 6 | Black key pressed | Voice 1 | Red (2) |
| 7 | Black key pressed | Voice 2 | Orange (8) |
| 8 | Black key pressed | Voice 3 | Yellow (7) |
| 9 | Black key pressed | Voice 4 | Green (5) |
| 10 | Black key pressed | Voice 5 | Cyan (3) |
| 11 | Black key pressed | Voice 6 | Blue (6) |

### Sprite behavior

- Sprites 0-5 assigned to voices 1-6 (one sprite per voice)
- When voice plays a note: calculate key position, set sprite shape (white or black variant), position sprite, enable
- When voice goes silent: disable sprite
- Sprite priority: 2 (in front of everything)
- Transparent color in sprite: set to a color not used in the key graphic

### "Depressed key" appearance

White key pressed sprite: 16x16, mostly white (1) with a darkened bottom edge and the voice's accent color as a highlight line at the top. Gives visual impression of being pushed down.

Black key pressed sprite: 16x16, mostly black (0) with the voice's accent color as a small indicator. Slightly lighter shade to distinguish from unpressed black keys.

## Voice Color Scheme

| Voice | SID | Color | Index | Role |
|-------|-----|-------|-------|------|
| 1 | SID1 | Red | 2 | Warm |
| 2 | SID1 | Orange | 8 | Warm |
| 3 | SID1 | Yellow | 7 | Warm |
| 4 | SID2 | Green | 5 | Cool |
| 5 | SID2 | Cyan | 3 | Cool |
| 6 | SID2 | Blue | 6 | Cool |

## Main Loop

```
init:
    set mode 1 (graphics over text)
    clear graphics, clear text
    draw upper keyboard (white keys, borders, black keys)
    draw lower keyboard (same)
    draw octave labels (C0-C7)
    define 12 sprite shapes via VGC commands
    draw player chrome (progress bar outline, voice legend, status text)
    zero previous-note array (6 bytes)

loop:
    VSYNC                               ; sync to 60Hz

    for voice = 0 to 5:
        LDA $BA51+voice                 ; current MIDI note
        CMP previous[voice]
        BEQ next_voice                  ; no change, skip

        ; clear old indicator
        old = previous[voice]
        if old > 0:
            disable sprite[voice]

        ; set new indicator
        new = current note
        if new > 0:
            subtract 12 (MIDI C0)
            if out of range (0-95): skip
            divide by 12 -> octave
            mod 12 -> pitch class
            table lookup -> white/black, key index

            if octave >= 4:
                use lower row Y, octave -= 4
            else:
                use upper row Y

            X = 6 + (octave * 7 + key_index) * 11

            if black key:
                X += 11 - 7/2  (center on black key)
                shape = 6 + voice
            else:
                shape = 0 + voice

            set sprite[voice] position (X, Y)
            set sprite[voice] shape
            enable sprite[voice]

        store as previous[voice]
    next_voice:

    ; update progress bar
    LDA $BA59                           ; total frames low
    ORA $BA5A                           ; total frames high
    BEQ skip_progress                   ; 0 = unknown duration, skip
    ; compute fill width = (elapsed / total) * bar_width
    ; update FILL command for progress bar
    skip_progress:

    ; update time display
    ; elapsed_seconds = elapsed_frames / 60
    ; MM = elapsed_seconds / 60, SS = elapsed_seconds MOD 60
    ; write to text layer at fixed position

    ; check exit: keypress
    JSR V_INPT                          ; $E741 - non-halting keyboard scan
    BCS exit                            ; carry set = key received

    ; check exit: music stopped
    LDA $BA50
    AND #$02                            ; bit 1 = music playing
    BNE loop                            ; still playing, continue

exit:
    ; disable all 6 sprites
    ; RTS back to BASIC
```

### Note-to-key lookup table (12 bytes each)

```
; pitch class:  C  C# D  D# E  F  F# G  G# A  A# B
key_type:  .byte 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0
key_index: .byte 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6
```

### ROM routine usage

The visualizer calls `V_INPT` for non-halting keyboard scan. This is a `JMP (VEC_IN)` — an indirect jump through a RAM vector. No other ROM routines are needed; all VGC/sprite operations go through hardware registers.

Addresses come from `novavm.inc` (auto-generated from `basic.sym`), so the visualizer never hardcodes ROM addresses. If the ROM is rebuilt and symbols move, regenerating `novavm.inc` and reassembling the visualizer picks up the new addresses automatically.

Note: There is no formal ROM jump table. `V_INPT` and `V_OUTP` are at fixed addresses in ROM but accessed symbolically via `novavm.inc`.

## C# Infrastructure Changes

### 1. Rename CALL to SYS

In `ehbasic/basic.asm`:
- Change token text from "ALL",TK_CALL to "YS",TK_CALL (or equivalent)
- Rename TK_CALL to TK_SYS, LAB_CALL to LAB_SYS, LBB_CALL to LBB_SYS
- Implementation (evaluate address, push return, JMP indirect) stays identical
- Rebuild ROM

### 2. New `.bin` file extension support

In `FileIoController.cs`:
- LOAD: when raw-mode address is specified, try `.bin` extension if `.bas` not found (or add explicit logic)
- DIR: enumerate `.bin` files alongside `.bas` and `.sid`
- DIR display for `.bin`: show load address from first 2 bytes (e.g., "KEYBOARD  $9000  BIN")
- FioDirType: add new type value for BIN files (e.g., 2)

### 3. Elapsed/total frame registers

New registers at $BA57-$BA5A (after the 6 note registers):

| Address | Name | Description |
|---------|------|-------------|
| $BA57 | MusicElapsedL | Frames elapsed, low byte |
| $BA58 | MusicElapsedH | Frames elapsed, high byte |
| $BA59 | MusicTotalL | Total frames, low byte |
| $BA5A | MusicTotalH | Total frames, high byte |

16-bit at 60Hz = max 1092 seconds = ~18 minutes. Sufficient for most pieces.

**MusicEngine changes:**
- Add `_elapsedFrames` counter, incremented each Tick() when playing
- Add `TotalFrames` property, computed from longest voice's parsed MML event list
- Reset elapsed on Play(), set total on sequence load

**MidiPlayback changes:**
- Expose total duration as frame count from timeline
- Elapsed tracked via MusicEngine (shares the same tick)

**SidPlayer changes:**
- Elapsed: increment frame counter each play-routine call
- Total: unknown for most .sid files (set to 0, visualizer skips progress bar)

**CompositeBusDevice changes:**
- Read handler for $BA57-$BA5A returns elapsed/total from MusicEngine

### 4. SidPlayer note register population

SidPlayer writes SID frequency registers directly. To populate $BA51-$BA53:
- On each play-routine tick, read SID voice frequency registers (2 bytes each)
- Check voice gate bit (bit 0 of control register $D404/$D40B/$D412)
- If gate on: convert frequency to MIDI note via `round(12 * log2(freq / 440) + 69)`
- If gate off: set note to 0
- Write to MusicNote1-3 (or expose via MusicEngine interface)

SID frequency to Hz: `freq_hz = freq_reg * clock / 16777216` where clock depends on PAL/NTSC. For our emulator, use the configured clock rate.

### 5. Verify MidiPlayback note registers

Confirm that MidiPlayback's DirectNoteOn/DirectNoteOff calls update MusicEngine's voice note tracking so that GetVoiceNote() returns the correct MIDI note for each voice.

## Assembly Tooling

### File structure

```
ehbasic/
    novavm.inc          ; auto-generated hardware constants
    keyboard.s          ; keyboard visualizer source
    keyboard.cfg        ; ld65 linker configuration
    Makefile            ; updated with keyboard target
tools/
    gen_novavm_inc.py   ; generates novavm.inc from VgcConstants.cs
```

### novavm.inc auto-generation

Two sources feed into `novavm.inc`:

1. **VgcConstants.cs** — all hardware register addresses, commands, status codes
2. **basic.sym** — VICE label file generated by `ld65 -Ln` during ROM build (1,347 symbols)

The `basic.sym` file format is: `al ADDRESS .SYMBOL` (one per line). It contains every ROM symbol including `V_INPT`, `V_OUTP`, `LAB_CALL`, zero page locations, etc.

`tools/gen_novavm_inc.py` merges both sources into ca65-compatible equates:

```
; novavm.inc — auto-generated, DO NOT EDIT
; Sources: VgcConstants.cs, basic.sym
; Run: python tools/gen_novavm_inc.py

; === Hardware Registers (from VgcConstants.cs) ===

; Memory map
ZeroPageBase      = $0000
BasicBase         = $0280
...

; VGC registers
RegMode           = $A000
RegCmd            = $A010
...

; VGC commands
CmdFill           = $06
CmdGcolor         = $08
...

; === ROM Entry Points (from basic.sym) ===

V_INPT            = $E741    ; non-halting keyboard scan
V_OUTP            = $E744    ; character output
...
```

The script:
- Parses `public const int/byte NAME = VALUE;` from VgcConstants.cs
- Parses `al ADDR .SYMBOL` from basic.sym
- Groups hardware constants by comment section headers
- Filters ROM symbols to a curated safe list (V_INPT, V_OUTP, etc.) to avoid exposing unstable internal labels
- Outputs to `ehbasic/novavm.inc`
- Must be re-run whenever VgcConstants.cs changes or the ROM is rebuilt

### keyboard.cfg (ld65 linker config)

```
MEMORY {
    HDR:  start = $0000, size = 2, fill = yes;
    CODE: start = $9000, size = $1000;
}
SEGMENTS {
    HEADER: load = HDR, type = ro;
    CODE:   load = CODE, type = ro;
    RODATA: load = CODE, type = ro;
    BSS:    load = CODE, type = bss;
}
```

The HEADER segment emits the 2-byte load address ($00 $90).

### Makefile additions

The ROM build now generates `basic.sym` via `ld65 -Ln`:

```makefile
# ROM build (updated)
basic.bin basic.sym: basic.o ehbasic.cfg
    ld65 -C ehbasic.cfg -vm -m basic.map -Ln basic.sym -o basic.bin basic.o

# Auto-generate hardware + ROM symbol include
novavm.inc: ../e6502.Avalonia/Hardware/VgcConstants.cs basic.sym ../tools/gen_novavm_inc.py
    python3 ../tools/gen_novavm_inc.py \
        ../e6502.Avalonia/Hardware/VgcConstants.cs \
        basic.sym \
        -o novavm.inc

# Keyboard visualizer
keyboard.bin: keyboard.s keyboard.cfg novavm.inc
    ca65 keyboard.s -o keyboard.o
    ld65 -C keyboard.cfg keyboard.o -o keyboard.bin

install-keyboard: keyboard.bin
    cp keyboard.bin ~/e6502-programs/keyboard.bin

clean:
    $(RM) *.o *.lst *.map *.bin *.sym

.PHONY: install-keyboard clean
```

## Implementation Phases

### Phase 1 — Infrastructure (C# + ROM)

1. Rename CALL to SYS in basic.asm, rebuild ROM
2. Add `.bin` extension support to FileIoController (LOAD + DIR)
3. Add elapsed/total frame registers ($BA57-$BA5A) to MusicEngine + CompositeBusDevice
4. Verify MidiPlayback populates voice note registers correctly
5. Add SidPlayer frequency-to-MIDI-note reverse mapping for $BA51-$BA53

### Phase 2 — Assembly tooling

6. Create `tools/gen_novavm_inc.py` to auto-generate from VgcConstants.cs
7. Generate `ehbasic/novavm.inc`
8. Create `ehbasic/keyboard.cfg` linker config
9. Add Makefile targets, verify build produces valid `.bin` with correct load address

### Phase 3 — Visualizer (assembly)

10. Skeleton: load at $9000, VSYNC loop, exit on keypress/music-stop, RTS
11. Draw static keyboards (white keys, borders, black keys, octave labels)
12. Note tracking: poll 6 voices, note-to-key lookup, position sprites
13. Sprite shape definition: 12 shapes (6 white-pressed, 6 black-pressed)
14. Player chrome: progress bar, MM:SS time display, voice legend

### Phase 4 — Testing

15. Test with MML (BASIC program using MUSIC PLAY + SYS)
16. Test with MIDPLAY
17. Test with SIDPLAY
18. Edge cases: notes outside 8-octave range, all 6 voices active, rapid note changes, pieces longer than 18 minutes (counter wraps gracefully)

## Future Considerations (not in v1)

- Full transport controls (pause, volume, per-voice mute) via keyboard shortcuts
- Scrolling/adaptive octave range for unusual pieces
- 24-bit frame counters for pieces longer than 18 minutes
- Additional visual effects (note trails, waveform display)
- ROM jump table for stable entry points (when/if added)
