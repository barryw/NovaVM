# NovaZ — Z-machine Sound (`sound_effect`)

Date: 2026-06-01
Goal: full `sound_effect` support — the bleeps AND the sampled sounds used by
The Lurking Horror (V3) and Sherlock (V5) — so each game ships as a mount-and-run
`.ndi`. Avalonia POC first, hardware (FPGA/ESP32) after.

## Background

`zvm_sound_effect` is currently a no-op. The opcode is
`sound_effect number effect volume routine` (VAR:21):

- `number` 1 = high bleep, 2 = low bleep — synthesized, no assets needed.
- `number` ≥ 3 = sampled sound, from external resources (a **Blorb** file).
- `effect` 1=prepare, 2=start, 3=stop, 4=finish-with.
- `volume` low byte = level (1..8, 255=loudest); high byte = repeats (0/255 = forever).
- `routine` (V5+) = called once when the sound finishes (an interrupt, like timed input).

The sample assets exist as standard Blorb files (`Lurking.blb`, `Sherlock.blb`
on the IF Archive): `FORM/IFRS` with an `RIdx` mapping each sound number to a
`Snd` resource that is a `FORM/AIFF` holding 8-bit mono PCM (~9.7 kHz). The
`RIdx` resource number IS the `sound_effect` number.

Samples are far too big for 64 KB, so — exactly like Nova's WTS soundfonts —
they live host-side and the 6502 triggers them by number.

## Packaging — pre-decode in the packer

The packer decodes the Blorb to raw PCM at build time and embeds a flat
`SOUND.PAK` in the `.ndi`, so **neither** the Avalonia host nor the ESP32 ever
parses IFF/AIFF:

```
SOUND.PAK
  "NZSP", version(1), count(1)
  index[count]: z_number(1)  rate_hz(2, LE)  pcm_offset(4, LE)  pcm_len(4, LE)
  pcm: concatenated signed 8-bit mono samples
```

## Components

### 1. Packer (`Nova.NovaZ.Packer`, C#)
- New `--sounds <file.blb>` option (and `PROJECT_SOUNDS` in `project.mk`).
- `BlorbReader`: walk the IFF, read `RIdx`, decode each `Snd`/`FORM/AIFF`
  (COMM → rate; SSND → 8-bit PCM). Downmix/convert only if needed; Infocom
  sounds are 8-bit mono.
- Emit `SOUND.PAK`, `image.WriteFile` it into the `.ndi`.

### 2. Host audio (`FileIoController` + a PCM player, C#)
- New `FIO_CMD_ZSOUND` with params number / effect / volume(+repeats).
- Lazy-load `SOUND.PAK` from the mounted floppy; build number → (pcm, rate).
- `effect 2` start: play sample N on a dedicated OpenAL source at its rate;
  volume → gain; repeats 0/255 → loop, else one-shot. `effect 3` stop.
- Completion is **deterministic**: on start, compute duration in frames/cycles;
  count down on `AdvanceCycles` (the MusicEngine `_sfxFrames`/`SfxCompleted`
  model); when it reaches zero, raise a "Z-sound finished" status bit. Identical
  in the headless smoke, real-time play, and on the ESP32.

### 3. Runtime (`zvm.s`)
- `zvm_sound_effect`: `number` 1/2 → bleep via the existing `FIO_CMD_SOUND`
  (high/low note); `number` ≥ 3 → set FIO regs, trigger `FIO_CMD_ZSOUND`.
- V5 finish-routine: record the routine in `zvm_sound_routine_lo/hi`; the
  `read`/`read_char` loops poll the "Z-sound finished" bit alongside the timer
  and fire the routine via the shared `nz_call_z_routine` (generalized from the
  Z4 `nz_fire_timed_routine`), then clear it.
- Read dispatch: pure 2-instruction spin only when no interrupts are pending
  (timed input OR a sound-with-callback); otherwise the interrupt-poll loop
  (already recognized by the smoke via `zvm_read_timed_poll`). Sound state is
  fixed at read entry — the game cannot issue `sound_effect` while blocked in
  `read`.

## Testing

- **Jukebox stories (both games):** small Z-programs that play each sample by
  number on a keypress, packaged with the real `.blb`. Run in the Avalonia GUI
  to hear every sound with no walkthrough — the primary "for real" check, and a
  showcase "Infocom sound museum" `.ndi` in its own right.
- **Headless smoke:** deterministic data-path assertion (the host exposes a
  "last Z-sound played" / "finished" state) including the V5 callback — a story
  plays a short sample with a finish-routine that sets a global, then sits in
  `read_char` (timed abort so it can't hang); the smoke asserts the global was
  set. CI-safe.
- **In-game triggers** (researched, for optional authentic listening):
  Lurking Horror sounds are mostly deep (tomb hatch, Inner Lair, endgame);
  Sherlock's violin and hansom-cab are early, Big Ben chimes are a puzzle.

## Hardware (later)

The host PCM path is Avalonia/NovaHost first. On FPGA, sampled PCM routes
through ESP32 audio (the WTS-on-ESP32 area) — additional work, deliberately
deferred after the Avalonia POC.

## Status — Avalonia POC complete (2026-06-01)

All four components shipped and verified on Avalonia:

1. **Packer** — `BlorbSounds.cs` + `--sounds <blb>`; `PROJECT_SOUNDS` /
   `LURKING_SOUNDS` / `SHERLOCK_SOUNDS` in the Makefile embed `SOUND.PAK`
   automatically. Verified against the real files (Lurking 14 sounds, Sherlock
   15 sounds).
2. **Host** — `ZSoundController.cs` + `FIO_CMD_ZSOUND` ($16) in
   `FileIoController.DoZSound`; cycle-driven completion via `ZSOUND_STATUS`
   ($BA76, bit0 finished / bit1 playing) routed in `CompositeBusDevice`.
3. **Runtime** — `zvm_sound_effect` (zvm.s): number 1/2 → bleep via
   `FIO_CMD_SOUND`; ≥3 → `FIO_CMD_ZSOUND`; V5 finish routine registered and
   fired from the read/read_char poll loops via the generalized
   `nz_call_z_routine` (refactored out of the Z4 `nz_fire_timed_routine`).
   `FIO_CMD_ZSOUND` + `ZSOUND_STATUS` added to `nova.inc`.
4. **Tests** — `test-z5-spec` now drives a sampled sound with a V5 finish
   routine and a `read_char` timed abort, asserting the callback ran
   (`z5 sound ok`); proven RED before #11. The interrupt poll loop grew past a
   power-of-two length and defeated the smoke's fixed PC sampling stride — fixed
   durably in the harness (`SettledAtReadLoop` micro-steps to the loop head
   instead of hand-tuning the assembly loop length). 4 spec smokes + 10 Infocom
   game smokes (incl. timed-input Border Zone) green.

**Showcase:** `make jukebox-lurking` / `jukebox-sherlock` build mount-and-run
"Infocom sound museum" `.ndi`s (RETURN auditions each sample). `test-jukebox`
is the CI-safe headless check. The Lurking Horror and Sherlock game projects now
ship with their real sounds.
