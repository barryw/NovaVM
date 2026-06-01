# NovaZ Sampled Sound on Hardware — WTS Soundfont Piggyback

Date: 2026-06-01
Status: **Shipped + verified on the ULX3S** (Sherlock + Lurking Horror, stereo).

Follows `2026-06-01-novaz-sound-design.md` (the Avalonia POC). That design plays
sampled `sound_effect` (number ≥ 3) on Avalonia via `ZSoundController` + a flat
`SOUND.PAK`. This document covers making the same sounds play on the FPGA/ESP32.

## The realization

The deferred-hardware plan assumed a brand-new PCM subsystem (a new FPGA player
streaming from SDRAM, or a new ESP32 audio path). It turned out unnecessary:

- On hardware the audio synthesizer is the **WTS chip on the FPGA**
  (`e6502.FPGA/rtl/wts_chip.sv`): it reads sample bytes from **SDRAM port B**,
  sign-extends 8-bit samples, and mixes voices to the jack.
- **NovaHost (ESP32)** already drives it for MIDI: it loads soundfont samples
  into SDRAM and streams **WTS note events** over the debug bridge
  (`CMD_WRITE_WTS_EVENTS`, advertised by `CAP_WTS_EVENT_STREAM`).
- A WTS region plays at its **native rate, one-shot**, when the triggered note
  equals the region's `RootKey` (`pitchRatio = 2^((note-RootKey)/12) = 1`).

A Z-machine sampled sound is exactly a mono 8-bit one-shot at a fixed rate — i.e.
a single WTS region. So we piggyback on the proven soundfont/WTS path instead of
building anything new. **No FPGA RTL change** (`fio.sv` already raises an event
for any non-zero `FioCmd` write; the WTS chip + event stream already exist).

Decision (user): *"ESP32 plays PCM via WTS (runtime unchanged)"* — smallest blast
radius. The 6502 runtime and the Avalonia path are untouched; all new code is the
packer's soundfont emitter and one ESP32 handler.

## Components

### 1. Packer — emit `ZSOUND.NSF` (`BlorbSounds.BuildSoundfont`)
Alongside `SOUND.PAK` (Avalonia), the packer emits a WTS soundfont built from the
same decoded Blorb PCM. Format mirrors the canonical writer
(`e6502.Tools/NovaSoundBankCommand.cs`) so the ESP32's existing loader parses it:
magic `NSFB`, PCM8, 34-byte header, 40-byte instrument + region records,
frame-indexed sample offsets. One instrument ("ZSOUND", bank 0 / program 0) with
**one region per sound number**, where `key_lo == key_hi == root_key == number`
and `sample_rate` = the sound's native rate, `flags = 0` (one-shot), instant
full-volume envelope. So note == number plays that sample at native pitch.

### 2. ESP32 — `handle_zsound` (`fio_dispatcher.cpp`, `CMD_ZSOUND = 0x16`)
`sound_effect` ≥ 3 issues `FIO_CMD_ZSOUND`; `fio.sv` raises the FIO event;
`handle_zsound`:
- reads `number`/`effect`/`level` from the FIO register bank;
- `effect 3` (stop) → `WTS_CMD_EVENT_RESET`;
- otherwise `ensure_zsound_bank_loaded()` — lazily `load_wts_bank_by_name("ZSOUND.NSF")`
  (the same path `SFLOAD` uses), then `configure_wts_for_midi()` for audible
  volume, and **centers the SFX voice** (pan 128 — the MIDI voice-spread leaves
  voice 0 hard left, and these mono samples want both speakers);
- builds a note-on song event (note == number, voice 0) and reuses
  `build_music_wts_event` → `find_wts_region` (native-rate phase step) →
  `WTS_CMD_EVENT_RESET` + `writeWtsEvents` + `WTS_CMD_EVENT_START`. The non-looping
  region ends the voice on its own.

### 3. Cache invalidation
The loaded soundfont is cached (`_zsound_bank_ready`). A `vm reset` cold-boots the
6502 but **not** the ESP32, so a disk swap would keep playing the previous game's
samples. `novaVmReset()` now calls `invalidate_zsound_bank()`, forcing the next
`sound_effect` to reload `ZSOUND.NSF` from the currently-mounted disk.

## Deploy / test

`make -C e6502.ESP32/novahost upload` (OTA), then via the `nova` management CLI
(TCP 6504): `put` the jukebox `.ndi` → `drive mount fd0` → `vm reset`; BASIC
autoboots the jukebox; read with `vm screen`. See
`reference_novahost_deploy_and_mgmt_cli` for the single-client gotcha.

Verified by ear on the ULX3S: both the Sherlock and Lurking Horror jukeboxes play
their correct, distinct samples in stereo.

## V5 finish callback on hardware

Deferred. The jukebox and `read`-driven games don't need it; wiring the WTS voice
"done" status back to `$BA76` for the V5 `sound_effect` routine is a follow-up.
