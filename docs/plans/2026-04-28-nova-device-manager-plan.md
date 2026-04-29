# Nova Device Manager, SD Layout, Soundfonts, and Personality ROMs

## Goal

Define the long-term host/device model for NovaVM on ULX3S:

- The ESP owns SD card access, filesystem parsing, wireless management APIs, and asset conversion/transfer.
- The FPGA owns deterministic real-time devices: CPU, VGC, sprites, audio playback, RAM/ROM banks, and bus-visible registers.
- User tooling should be a first-class `nova` CLI, not raw `curl` calls or a disk-image-only tool.

This plan captures direction only. It is not an immediate implementation task.

## Core Decisions

### Requirements Captured From Discussion

- Users should manage SD card assets wirelessly through Nova tooling, not `curl`.
- Disk images are whole-file upload/download assets. A hard-disk `.ndi` may be
  64 MB or larger, so the ESP API and CLI must stream transfers without
  buffering entire files in RAM.
- No live remote editing of mounted `.ndi` files.
- The current `e6502.NDI` CLI has outgrown its name. The long-term tool should be `nova`.
- Uploading a raw `.sf2` soundfont should automatically convert it to Nova-native format before transfer.
- Converted soundfont banks are also whole-file assets. First-pass `.nsfb`
  banks should be expected to be roughly the same order of size as their
  source sample data unless we intentionally downsample or compress.
- BASIC programmers should be able to list/load/unload soundfonts from a stable soundfont directory.
- Language/runtime ROMs are "personality ROMs" that own the machine while active.
- NovaBasic should not be split into a separate shared KERNAL/runtime right now.
- Anything that can realistically live on SD should live on SD: personality
  ROMs, soundfont banks, boot artwork, fonts, lookup tables, disk images, and
  other bulky assets.

### SD Is ESP-Owned

The FPGA should not know about SD cards, FAT, filenames, directories, or file formats. The ESP already has stable SD/FAT libraries and is the right place for those concerns.

The FPGA should expose small, deterministic transport/control surfaces:

- bulk transfer into FPGA RAM/SDRAM/ROM banks,
- status/error registers,
- event or IRQ notification,
- device-specific load/commit commands.

### Whole-File Disk Image Management

Nova disk images are managed as whole files on the SD card. We do not support live remote mutation of a mounted disk image.

Wireless management flow:

- upload/download/delete/list `.ndi` files over the ESP file API,
- prevent overwrite/delete of currently mounted images,
- unmount/remount explicitly when changing an image,
- keep NDI filesystem edits local in the CLI, then upload the resulting image.

This keeps the ESP-side implementation simple and avoids corruption risk from concurrent BASIC file I/O and remote writes.

### Rename `ndi` Tool To `nova`

`e6502.NDI` started as a disk-image CLI, but the scope is now broader:

- device status and logs,
- disk image management,
- soundfont management,
- language/personality ROM management,
- future configuration and deployment flows.

The user-facing command should become `nova`. The existing `ndi` command can remain as a compatibility alias or legacy subcommand during transition.

Suggested command shape:

```bash
nova device status
nova device logs

nova disk create fd0.ndi --size 800 --label BOOT
nova disk list
nova disk upload fd0.ndi --slot fd0
nova disk download fd0.ndi
nova disk delete fd0.ndi

nova soundfont list
nova soundfont upload GeneralUser.sf2
nova soundfont delete GeneralUser

nova rom list
nova rom upload novabasic.rom
nova rom activate novabasic
nova rom default novabasic
```

## SD Card Layout

Managed assets should live under stable directories:

```text
/disks/floppy/
/disks/hard/
/soundfonts/
/roms/
/assets/boot/
/assets/fonts/
/assets/sid/
/config/
```

Root-level `hd0.ndi` / `hd1.ndi` may remain supported as a boot/default convenience, but new CLI-managed assets should use the directory layout.

Current boot asset names used by NovaHost:

```text
/roms/novabasic.bin
/roms/extension.bin
/assets/sid/f6581_curve.bin
/assets/boot/novavm_logo.nvg
```

Boot artwork is converted from host formats into a Nova-native display asset
under `/assets/boot/`. The initial source asset is `NovaVM-logo.png`; NovaHost
loads `novavm_logo.nvg` into the VGC graphics plane while the CPU is held in
reset, forces black background/border colors, then uses the VGC display-dimmer
register for fade-in/fade-out before the configured personality/runtime starts.

Suggested config files:

```text
/config/boot.json
/config/mounts.json
```

`boot.json` can eventually hold default ROM/personality and boot behavior. `mounts.json` can map slots like `hd0`, `hd1`, `fd0` to image paths.

## Soundfont Strategy

### Do Not Load Raw SF2 In Hardware

SF2 is a rich RIFF-based format with preset bags, generator/modulator tables, sample headers, timecents, centibels, loops, and optional behaviors. Parsing or interpreting that in FPGA logic would waste resources and make timing/debug much harder.

The Nova tool should convert `.sf2` files to a Nova-native bank during upload:

```bash
nova soundfont upload GeneralUser.sf2
```

Internally:

1. parse the SF2 on the PC,
2. flatten the supported instruments/regions,
3. quantize/precompute hardware-friendly metadata,
4. upload the converted file to `/soundfonts/GeneralUser.nsfb`.

If the input is already Nova-native, upload it directly:

```bash
nova soundfont upload GeneralUser.nsfb
```

An explicit `convert` command may exist for development, but normal users should not need it.

### Nova Soundfont Bank Format

Working extension: `.nsfb` for "Nova SoundFont Bank".

Proposed contents:

- header: magic, version, flags, instrument count, region count, sample bytes, CRC,
- instrument table: name offset, MIDI bank, MIDI program, first region, region count,
- region table: key range, velocity range, sample offset, sample length, loop start/end, root key, tuning, envelope values,
- sample blob: initially mono signed PCM16 little-endian,
- string table: null-terminated instrument names.

The first version should support the subset already modeled by the emulator:

- presets/instruments,
- key and velocity regions,
- mono sample data,
- sample rate,
- root key,
- tuning cents,
- loop start/end and loop enable,
- AHDSR-ish envelope values.

Deferred or optional SF2 features:

- modulators,
- filters,
- LFOs,
- stereo regions,
- exclusive classes,
- per-zone pan/attenuation,
- advanced velocity/filter mappings.

Those can be added by format-version bumps if hardware support appears later.

### Expected Soundfont Bank Size

The sample blob dominates file size. Metadata should usually be small by
comparison: kilobytes to a few hundred kilobytes depending on instrument and
region count.

Practical expectations:

- Preserve mono PCM16: `.nsfb` is roughly the same size as the source sample
  payload, often near the `.sf2` file size after removing unsupported metadata.
- Downsample to lower rates: sample bytes drop roughly in proportion to the
  sample-rate reduction.
- Convert to unsigned/signed 8-bit PCM: sample bytes drop roughly 2:1 versus
  PCM16, with audible fidelity cost.
- Add ADPCM later: likely 3:1 to 4:1 versus PCM16, with decoder complexity.

The firmware and CLI should not assume "small" soundfonts. Tens of megabytes
must be normal; curated banks can be smaller, but the transport should handle
large banks the same way it handles large `.ndi` files.

### What We Lose By Converting SF2

The cost of Nova-native conversion is fidelity, not usability. Nova will not be a bit-perfect SF2 synthesizer at first.

Likely first-pass losses:

- SF2 modulators,
- SF2 filters,
- LFO routing,
- stereo sample regions,
- exclusive class behavior,
- advanced velocity-to-filter or velocity-to-attenuation mappings,
- per-zone pan/attenuation if hardware does not implement them yet,
- exact host-synth rendering behavior.

Benefits:

- FPGA never parses RIFF/SF2.
- ESP does not need a heavy SF2 runtime.
- Load time is predictable.
- Metadata is hardware-friendly.
- The format can enforce hard limits before upload instead of failing at playback time.
- The CLI can regenerate Nova-native banks as the format evolves.

If we later need more SF2 fidelity, add fields to `.nsfb` and bump the format version. Do not make the FPGA interpret raw SF2.

### Soundfont CLI Requirements

Normal user flow should be:

```bash
nova soundfont upload GeneralUser.sf2
nova soundfont list
nova soundfont delete GeneralUser
```

Behavior:

- `.sf2` input is parsed and converted locally before upload.
- `.nsfb` input is validated and uploaded directly.
- Uploaded files land under `/soundfonts/`.
- The CLI should print what was generated: instrument count, region count, sample bytes, warnings about unsupported SF2 features.
- The CLI should reject banks that exceed current hardware limits unless `--force` is explicitly supported later.

Optional developer-only flow:

```bash
nova soundfont convert GeneralUser.sf2 GeneralUser.nsfb
```

This can exist for debugging, tests, and reproducible asset builds, but it should not be required for normal users.

### Runtime Soundfont Load

BASIC remains simple:

```basic
SFDIR
SFLOAD "GeneralUser"
SFUNLOAD
```

Runtime flow:

1. BASIC issues `SFLOAD`.
2. ESP opens `/soundfonts/GeneralUser.nsfb`.
3. ESP validates header and CRC.
4. ESP sets WTS status to loading.
5. ESP streams sample data into FPGA SDRAM.
6. ESP streams metadata into WTS metadata memory/registers.
7. FPGA flips the active soundfont only after the whole load succeeds.
8. Note-on/off playback is fully FPGA-local after load.

The FPGA must not ask the ESP which sample or region to use during note-on. Region lookup, phase accumulation, looping, envelopes, and mixing should be deterministic hardware behavior.

## Personality ROM Model

Do not split NovaBasic into a Commodore-style BASIC ROM plus shared KERNAL now. The refactor cost is high and the immediate payoff is low.

Instead, define a 16 KB "Nova personality ROM":

- mapped at `$C000-$FFFF`,
- owns reset/NMI/IRQ vectors,
- owns language runtime and system routines,
- may include duplicated helper code,
- is selected and loaded by the ESP.

NovaBasic is one personality. Future examples:

- `novabasic.rom`,
- `forth.rom`,
- `logo.rom`,
- `monitor.rom`,
- `ncc-runtime.rom`.

Duplicated routines are acceptable because inactive ROMs live on SD, not in FPGA BRAM. The repo can still use shared source modules and link them into each personality ROM.

### Personality ROM Header

Reserve a small header near `$C000`:

- magic/signature,
- ROM format version,
- display name,
- semantic version,
- ABI compatibility level,
- optional flags,
- optional entry table for tooling.

The vectors at `$FFFA-$FFFF` remain authoritative for execution.

### ROM Management Flow

Suggested CLI:

```bash
nova rom upload novabasic.rom
nova rom list
nova rom activate novabasic
nova rom default novabasic
```

Suggested ESP behavior:

1. read selected ROM from `/roms/`,
2. validate size/header/checksum,
3. hold CPU reset,
4. stream ROM into FPGA ROM BRAM/bank,
5. release CPU reset,
6. update boot config if requested.

## ESP API Shape

The ESP should keep a generic file API for CLI transport, plus higher-level endpoints for device operations.

Generic file endpoints may remain:

```text
GET    /sd/<path>
PUT    /sd/<path>
DELETE /sd/<path>
GET    /sd/<dir>/
```

Higher-level endpoints should wrap policy:

```text
GET  /api/status
GET  /api/disks
POST /api/disks/upload
POST /api/disks/mount
POST /api/disks/unmount

GET  /api/soundfonts
POST /api/soundfonts/upload
POST /api/soundfonts/load
POST /api/soundfonts/unload

GET  /api/roms
POST /api/roms/upload
POST /api/roms/activate
POST /api/roms/default
```

The CLI should use the higher-level endpoints where policy matters.

## CLI Rename And Compatibility

The current `e6502.NDI` project can evolve in place initially, but the installed command should become `nova`.

Migration plan:

1. Add `nova` as the primary executable/command name.
2. Keep `ndi` as a compatibility alias for disk-image-only workflows.
3. Move current verbs under `nova disk` where practical.
4. Keep old `ndi create`, `ndi import`, etc. working until docs and scripts migrate.

Proposed command groups:

```text
nova device ...
nova disk ...
nova soundfont ...
nova rom ...
nova config ...
```

`nova disk` should cover both local NDI image manipulation and remote whole-file SD management.

Examples:

```bash
nova disk create boot.ndi --size 800 --label BOOT
nova disk import boot.ndi AUTOBOOT.bas / --tokenize
nova disk upload boot.ndi --remote novahost.local --floppy
nova disk upload hd0.ndi --remote novahost.local --hard
nova disk download hd0.ndi --remote novahost.local ./hd0.ndi --hard
nova rom upload novabasic.bin --remote novahost.local
nova soundfont upload GeneralUser.nsfb --remote novahost.local
nova asset upload f6581_curve.bin --remote novahost.local --type sid
nova disk mount --remote novahost.local fd0 /disks/floppy/boot.ndi
nova disk unmount --remote novahost.local fd0
```

The CLI should choose managed SD locations from the command group and type:

```text
disk --floppy  -> /disks/floppy/
disk --hard    -> /disks/hard/
rom            -> /roms/
soundfont      -> /soundfonts/
asset --type X -> /assets/X/
```

`--path` may remain available as an explicit override, but normal users should
not need to know the SD directory layout.

Remote disk upload should refuse to overwrite a currently mounted image unless the caller unmounts it first.

## Phased Implementation

1. Rename/scaffold `nova` CLI while keeping `ndi` compatibility.
2. Formalize SD layout and remote whole-file disk image commands. *(First-pass typed upload/download/list/delete commands exist in `e6502.NDI`; installed command rename is still pending.)*
3. Add soundfont upload command that converts SF2 to `.nsfb`.
4. Add ESP `/soundfonts` listing and `SFLOAD` handling for `.nsfb`.
5. Implement FPGA WTS metadata/sample memory loader.
6. Add `/roms` management and personality ROM loading.
7. Add boot config and default ROM/mount behavior.

## Non-Goals For First Pass

- Live editing mounted disk images over WiFi.
- FPGA-side SD/FAT support.
- FPGA-side SF2 parsing.
- Bit-perfect SF2 playback.
- Splitting NovaBasic into separate language and KERNAL ROMs.
