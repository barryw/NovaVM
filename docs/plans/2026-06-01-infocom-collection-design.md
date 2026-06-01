# Infocom NovaZ Collection — Design

Date: 2026-06-01

## Goal

Produce a collection of clearly-named, auto-booting NovaZ disk images — one
`.ndi` per Infocom game found under `/Volumes/Software/Emulation/Infocom`. Each
image boots straight into NovaZ running that game's story file. Games that ship
sampled sound (`.blb`) bundle their sound, and the soundfont is loaded at game
start, not lazily on first effect.

## Decisions (locked)

- **Scope:** auto-discover every supported (Z3/Z4/Z5) game in the archive.
  Skip Z6 graphical games (Arthur, Journey, Shogun, Zork Zero), the
  `zzInvisiclues` hint folder, and any folder without exactly one clear story
  (each skip logged with a reason).
- **Naming:** kebab-case, `dist/collection/<slug>.ndi`
  (e.g. `zork-i.ndi`, `the-lurking-horror.ndi`, `sherlock.ndi`).
- **Sound:** eager preload at boot (runtime change), not lazy-on-first-effect.
- **Validation:** smoke-test every image (boots → reaches story prompt); sound
  games additionally gated on the soundfont being loaded.

## Background — how sound loads today

Both runtimes load a bundled font lazily on the first `sound_effect` *start*:

- Avalonia: `FileIoController.DoZSound` → `TryLoadSoundPack()` reads `SOUND.PAK`.
- NovaHost/FPGA: `handle_zsound()` → `ensure_zsound_bank_loaded()` loads
  `ZSOUND.NSF` into the WTS bank.

The Packer already bundles **both** assets when given `--sounds <blb>`. The
jukebox proves the trigger path, but the runtime's `prepare` (effect 1) path is
currently a no-op (`zvm.s:3595`), so nothing loads the font until an in-game
`start`. That is the gap.

## Changes

### 1. Runtime: real `prepare` + boot preload (`examples/novaz/src`)

- `zvm.s` sampled-sound `prepare` (effect 1): issue `FIO_CMD_ZSOUND` with
  `FIO_SRCH = 1` so the host actually loads the bank (was a bare `RTS`).
- New `nz_sound_preload`: sets `FIO_SRCL=0`, `FIO_SRCH=1`, writes
  `FIO_CMD_ZSOUND`. Host loads the font if a pack is present, returns
  `ERR_NOT_FOUND`/`ERR_IO` (ignored) otherwise. Harmless for soundless games.
- `runtime.s` `@header_ok`: `JSR nz_sound_preload` before `init_game_screen`,
  so the font is the active bank the moment the game starts.

### 2. Avalonia: honor the effect operand (`FileIoController.DoZSound`)

`prepare` (effect 1) must load the pack and return OK **without** playing,
matching NovaHost. Today every non-stop effect plays. Fix:

```
if (effect == 3) { Stop; ok; return; }
if (!PackLoaded) TryLoadSoundPack();
if (effect == 1) { ok; return; }   // prepare: loaded, do not play
... play ...
```

### 3. Headless soundfont assertion (`Nova.NovaZ.Smoke`)

- Expose `CompositeBusDevice.ZSound` (currently private).
- New `--expect-soundfont` flag: after boot/script, assert
  `bus.ZSound.PackLoaded`. With boot preload, a sound game satisfies this right
  at the prompt — no fragile in-game walkthrough needed to prove *loading*.

### 4. Collection builder (`examples/novaz/tools/build-infocom-collection.py`)

1. `make all` once (AUTOBOOT + runtime).
2. Scan top-level archive folders. Per folder: pick the one supported story;
   detect a same-basename `.blb`; derive a kebab slug + human title (reuse
   `projects/<slug>/project.mk` title/label when present).
3. Pack each via the Packer → `dist/collection/<slug>.ndi`, passing `--sounds`
   only when the Blorb yields a **non-empty** sample pack (cover-art-only
   Blorbs, e.g. Beyond Zork, get no sound automatically).
4. Smoke each (`--generic-boot` reaches the prompt); sound games add
   `--expect-soundfont`.
5. Print a results table: game / version / sound / boot / font.

### 5. Makefile `collection` convenience target

`make -C examples/novaz collection` runs the builder.

## Tests

- Unit (CI-safe): `DoZSound` prepare loads the pack and does not play; start
  plays; stop stops. Uses a small synthetic pack.
- Integration (local, archive present): the builder's `--expect-soundfont` gate
  on the real sound games (Lurking Horror, Sherlock) proves the bundled font
  loads at boot end-to-end.

## Out of scope

- Z6 graphical games. - Mounting UX on hardware (each image is still a single
  auto-boot floppy; the user picks which to mount). - Per-game sound-trigger
  walkthroughs (boot preload makes them unnecessary for the load guarantee).
