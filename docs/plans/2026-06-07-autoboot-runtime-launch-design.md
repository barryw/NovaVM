# Autoboot Runtime Launch

**Date:** 2026-06-07
**Status:** Design approved (brainstorming). Ready for implementation plan.

## Goal

Let a boot floppy autoboot **any runtime** (Forth, Pascal, assembler systems, …),
not just BASIC programs. BASIC stays the always-present default; a floppy whose
`AUTOBOOT.bin` is a tiny launcher swaps the `$C000` ROM bank for a named runtime
from `/roms` and cold-boots into it. *"That way we always have autoboot."*

## Model (locked during brainstorming)

- **`/roms` = runtimes only.** A "runtime" is a 16K full-map ROM image that owns
  `$C000` and cold-boots. A Pascal/Forth/asm program that wants the whole memory
  map *is* such an image — the runtime/full-map-binary distinction dissolves.
- **A regular program** is an `AUTOBOOT.bin` that loads in low RAM (~`$1000`) and
  streams assets from disk — the **existing** `.bin` autoboot path, unchanged.
- **Bare launch only (v1).** Autoboot swaps to the runtime and cold-boots it to
  its own prompt/state. Running a program *inside* the launched runtime is a
  later follow-up.
- **The launcher binary IS the `AUTOBOOT.bin`.** No new BASIC ROM code — the
  existing `.bin` autoboot path already loads it to RAM and `JMP`s to it.

## What already exists (do not rebuild)

The runtime-load-by-name mechanism is built end-to-end and currently **unused**:

- CPU: `fio_load_runtime` (`runtime/asm/fio.s`) — `LDA #FIO_CMD_LOADRUNTIME; JMP fio_exec`.
- Module: `FILE_LOAD_RUNTIME` ($0B) in `modules/files/files.s`.
- Host (NovaHost): `CMD_LOADRUNTIME` ($28) / `handle_load_runtime` resolves
  `FIO_NAME` against `/roms`, appends `.bin`, validates 16K, streams into the
  `$C000` bank. Comment: *"The caller must be running from RAM… overwrites the
  primary ROM bank so the launcher can jump into another runtime."*
- Host (emulator): `FileIoController.DoLoadRuntime` → `LoadPrimaryRuntimeRom`
  (overwrites `_basicRom` + mapped `$C000`). Currently loads from the save-dir
  root, **not** `/roms` — the one parity gap.
- CPU swap register `$A03F` (`RegRomSwap`) already swaps among *pre-loaded*
  built-ins (BASIC/Logo/NCC); not used by this feature (those are built-in, this
  is for arbitrary `/roms` images via the load-then-coldboot path).

So this feature is **wiring**, not new infrastructure.

## Components

### 1. Runtime launcher binary — `assembly/apps/runtimeboot/`

Standalone 6502 `.bin`, pure MMIO (no ROM dependencies), ORG'd at the autoboot
`.bin` load address (low RAM; **must be `< $C000`** to survive the bank swap).
Mirrors `fio_exec`:

```asm
        ; copy embedded runtime name into FIO_NAME
        ldx #0
@cp:    lda name,x
        sta FIO_NAME,x
        inx
        cpx #namelen
        bne @cp
        lda #namelen
        sta FIO_NAMELEN
        stz FIO_STATUS
        stz FIO_ERRCODE
        lda #FIO_CMD_LOADRUNTIME
        sta FIO_CMD
@wait:  lda FIO_STATUS
        beq @wait
        jmp ($FFFC)        ; success: new runtime cold-boots
name:   .byte "/roms/novaforth", 0
```

**Self-correcting error path:** on not-found/wrong-size the host sets the error
latch and leaves the ROM bank untouched, so `$FFFC` still holds BASIC's reset
vector → `JMP ($FFFC)` reboots BASIC. v1 issues the unconditional jump after the
command completes (no separate error UI).

The runtime name is embedded data, stamped per-runtime by the build/tool.

### 2. `/roms` standardization (emulator parity)

`DoLoadRuntime` (`FileIoController.cs`) resolves the runtime name against
`<SaveDirectory>/roms/<name>.bin` instead of the save-dir root, mirroring
NovaHost's SD `/roms/`. Validate exactly 16K (already done).

### 3. `nova boot-floppy` CLI subcommand

`nova boot-floppy --runtime <name> [--disk <target>]`:
- Assembles/stamps the launcher with the embedded `/roms/<name>` path.
- Writes the result as `AUTOBOOT` (`.bin`) onto the target disk over the
  existing TCP/NDI host path.
One command produces a turnkey runtime-boot floppy.

### 4. Stub test runtime — `assembly/apps/stubruntime/`

A minimal 16K runtime image whose cold-boot prints a unique marker (e.g.
`STUB RUNTIME OK`) to the screen, then halts/loops. Installed into `/roms` as a
test fixture. Proves the swap+coldboot end-to-end without needing a real Forth/
Pascal.

## Data flow

```
power-on -> BASIC cold start -> LAB_AUTOBOOT finds AUTOBOOT.bin
  -> loads to low RAM, JMP -> launcher: FIO_NAME=/roms/<X>; fire CMD_LOADRUNTIME
  -> host streams /roms/<X>.bin into the $C000 bank
  -> launcher JMP ($FFFC) -> runtime X cold-boots (bare)
```

## Error handling

- Runtime not found / not 16K → host error latch, ROM bank untouched →
  `JMP ($FFFC)` reboots BASIC (graceful fallback).
- **RAM-resident invariant:** the launcher must never live in `$C000–$FFFF`.
  Enforce via ORG and a comment; the autoboot `.bin` load address is low RAM.

## Testing

- **Emulator integration** (Sim6502TestRunner / BasicRegressionTests style):
  install the stub runtime at `<savedir>/roms/STUB.bin`, place a launcher
  `AUTOBOOT.bin` naming `STUB`, cold-boot, assert the stub's marker appears on
  screen (proves load → swap → coldboot).
- **Not-found path:** launcher names a missing runtime → assert BASIC's `Ready`
  still comes up (self-correcting reboot).
- **CLI:** `nova boot-floppy` unit/integration — produces a launcher whose bytes
  poke the right FIO regs and embed the right name.
- FPGA HW test: out of scope v1 (NovaHost path already exists and is exercised
  by the integration suite when run against the NovaHost backend).

## Out of scope (v1)

- Running a program *inside* the launched runtime (turnkey app floppy).
- Real Forth/Pascal/assembler runtimes (separate projects; this delivers the
  launch mechanism + a stub).
- Error UI beyond reboot-to-BASIC.
- A text-manifest authoring format (the launcher-binary approach was chosen).
- Using `$A03F` RegRomSwap (that path is for pre-loaded built-ins only).

## Risks

- **Launcher load address.** Must match where the `.bin` autoboot path loads and
  be `< $C000`. Confirm the autoboot `.bin` dest (and the assembly-apps ORG
  convention) during planning; ORG the launcher to match.
- **`fio_load_runtime` / `FILE_LOAD_RUNTIME` are emitted only when referenced**
  (selective-emit guards). The launcher pokes FIO MMIO directly (no ROM call) so
  it doesn't depend on these being linked into BASIC — but the emulator host
  command and FIO command id must be active (they are).
