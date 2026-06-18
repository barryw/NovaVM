# NovaZ Save/Load Design

## Goals

- Use one save file per save slot.
- Use the same save-file format for Z3, Z4, Z5, and Z6 stories.
- Support multiple save files for a story.
- Store a short user description with each save.
- Present a reusable text UI picker instead of silently overwriting one file.
- Keep reusable file/UI/time pieces in the NDK, not in NovaZ-only code.
- Reserve timestamp fields now, even before Nova has platform date/time.

## Non-Goals

- Do not make Quetzal the on-machine runtime format yet.
- Do not add a NovaZ-only custom UI framework.
- Do not block save/load on platform date/time support.
- Do not require exact transcript/cursor restoration after restore. The required
  behavior is a resumable VM state with restored location, score/moves, dynamic
  state, and current V6 graphics.

## Native Save Format

The `.NZS` format is for NovaZ, not for one Z-machine version. Z3-Z6 should
share the same outer container and header. Version differences stay inside the
VM state and opcode return path:

- Z1-Z3 `save` / `restore` return through branch opcodes.
- Z4+ `save` / `restore` return through store opcodes.
- Z6 adds durable graphical state that the VM state already needs to preserve.

The container header records the story version and the saved return mode, so the
same loader can validate and restore all supported versions.

Use a single `.NZS` file per slot.

Suggested slot names:

```text
SAVE00.NZS
SAVE01.NZS
...
SAVE09.NZS
```

The file is a simple linear container:

```text
offset  size  field
$00     4     magic: "NZS1"
$04     1     header size, initially $40
$05     1     format version, currently $0A
$06     1     flags
$07     1     description length, 0..20
$08     20    description, ASCII, space or NUL padded
$1C     6     timestamp: YY MM DD HH MM SS, BCD or binary, zero when unknown
$22     1     Z-machine story version
$23     2     release number
$25     6     serial
$2B     2     story checksum
$2D     2     dynamic memory size
$2F     2     VM state size
$31     3     dynamic memory file offset
$34     3     VM state file offset
$37     1     saved return kind: branch or store
$38     1     saved store variable for Z4+ save/restore
$39     1     saved branch condition for Z1-Z3 save/restore
$3A     2     saved branch offset for Z1-Z3 save/restore
$3C     3     saved PC for save/restore return
$3F     1     reserved, zero
$40     n     dynamic memory bytes
...     n     VM state bytes
...     n     VTEXT window state bytes
...     n     optional V6 window/graphics interpreter state bytes
...     n     VGC text char plane, 80x50 bytes
...     n     VGC text color plane, 80x50 bytes
...     n     VGC text attribute plane, 80x50 bytes
```

The existing `NZSV` validation data maps directly into this header. Restore
must reject saves whose story version, release, serial, checksum, dynamic size,
or state size do not match the loaded story/runtime.

Current format flags:

- `$01`: VTEXT window state is present.
- `$02`: V6 display/retained graphics state is present.
- `$04`: VGC text planes are present.

The `.NZS` file must not store the raw VGC graphics framebuffer. V6 restore is
implemented as semantic display state plus retained picture/erase replay state
inside the optional V6 block:

- `NZ6_SAVE_DISPLAY_STATE_*`: the V6 window table and current colour state. This
  is durable interpreter state because after restore the next story output must
  land in the same selected window with the same colour semantics.
- `NZ6_SAVE_GFX_STATE_*`: the retained native-picture replay list. A full
  graphics clear records the base fill colour and resets the list; successful
  `draw_picture` and `erase_picture` calls append compact absolute-position
  entries.

Restore fills the gfx plane with the saved base colour and replays retained
entries from `PICS.PAK`. V6 then clears the text plane before returning to the
story, so the story redraws the current location, score/moves, banner text, and
prompt from restored VM state instead of NovaZ trying to preserve an exact
scrollback transcript. That keeps saves tied to Nova-native picture data and
current Z-machine state instead of freezing a full screen of pixels into every
save.

## File Streaming

The existing two-file implementation writes:

- `NOVAZ.DYN`: dynamic story memory from XRAM.
- `NOVAZ.SAV`: save header plus VM state staged through XRAM.

Single-file saves should use FIO file handles. The platform now supports direct
file-handle transfers to and from XRAM:

- `FOPEN` / `FCREATE`: `FIO_DIRTYPE & $03` selects access mode.
- `FREAD` / `FWRITE`: `FIO_DIRTYPE & $30` selects transfer target.
- `$00` target reads/writes CPU RAM at `FIO_ENDL/H`.
- `$10` target reads/writes flat XRAM at `FIO_GSPACE:GADDRH:GADDRL`.

That means large runtime state does not need a NovaZ-only CPU bounce buffer.

1. `FCreate SAVE##.NZS`.
2. `FWrite` the CPU-resident header.
3. `FWrite` dynamic memory directly from XRAM.
4. `FWrite` the CPU-resident VM state bytes.
5. `FWrite` VTEXT state, optional V6 state, and text planes.
6. `FClose`.

Restore is the inverse:

1. `FOpen SAVE##.NZS`.
2. `FRead` header.
3. Validate story/runtime identity and sizes.
4. `FRead` dynamic memory directly into XRAM.
5. `FRead` VM state bytes into CPU memory.
6. `FRead` VTEXT state, optional V6 state, and text planes.
7. Rebuild display/window state. Z6 calls the V6 segment restore-display
   dispatch so retained graphics are replayed; Z3-Z5 use the generic text/window
   restore path.
8. `FClose`.

The reusable NDK piece is the file-handle XRAM target convention, not a
NovaZ-specific serializer.

Restore compatibility is a hard backend contract, not just a dialog nicety.
The backend must reject any save whose magic/header format, Z-machine story
version, release number, serial, checksum, dynamic memory size, or VM state size
does not match the running story/runtime. The overlay should make that friendly:
hide or mark incompatible slots when possible, and if the user still selects one
show `Cannot restore: invalid game or version.` before returning a failed
restore result to the story. After dismissal, the Z-machine-visible result is
the normal restore failure path: branch false for Z1-Z3 and store `0` for Z4+.

## NovaZ Runtime Placement

The NovaZ base runtime is essentially full, so the picker must not be linked
directly into `NOVAZ.BIN`. It should live in shared SYSTEM/NUI code and/or a
fixed-address overlay. NovaZ now reserves the standard lib-call band
`$0300-$041F` and the standard module-BSS band `$0420-$08FF` by starting its
runtime BSS at `$0900`; that keeps the mailbox, resident loader, and paged
module scratch space available for shared module calls.

The NovaZ autoboot launcher also arms the shared-library ABI before jumping into
the runtime by setting `LIB_HOME_BANK=ROMSWAP_PRIMARY` and clearing
`LIB_RESIDENT`. That makes NovaZ a normal lib_call client instead of a runtime
with a private module/overlay path.

`SAVLOAD.OVL` is the fixed-address save/load UI and file-stream overlay. It loads
at `$4800`, above the V6 segment image (`$2000-$3FFF`) and V6 BSS
(`$4000-$47FF`). The base runtime loads and calls it directly through the NDK
overlay loader while the primary runtime ROM is mapped. The overlay calls
SYSTEM/NUI through `lib_call`, uses `SYS_NUI_PICK_LIST` for the slot picker, and
owns the native `.NZS` header/dynamic-memory/VM-state streaming code. It reaches
base runtime state only through the generated `runtime_abi.inc` symbol list.

## Save Dialog

Add reusable TUI controls under the NDK/SYSTEM UI layer, not in NovaZ.
`runtime/asm/nui.*` now owns the first reusable primitive:

- `nui_save_under`
- `nui_restore_under`
- `nui_pick_list`
- `SYS_NUI_SAVE_UNDER`
- `SYS_NUI_RESTORE_UNDER`
- `SYS_NUI_PICK_LIST`

These copy a text-plane rectangle to/from caller-owned XRAM buffers. The caller
chooses the XRAM addresses so a runtime with large XRAM state, such as NovaZ,
does not get clobbered by a hardcoded UI scratch area.

Minimum control behavior:

- Saves and restores the text-plane rectangle underneath the dialog.
- Draws a bordered modal with title.
- Displays a selectable list of rows.
- Supports Up/Down, Enter/OK, Esc/Cancel.
- Optionally supports a short edit field for Save descriptions.
- Returns selected row index and command: OK or Cancel.

NovaZ should use that control for both save and restore:

- Save: list slots, allow existing slot overwrite or empty slot selection, then
  prompt for a 20-character description.
- Restore: list only compatible existing saves; OK restores selected save.

## Timestamp Handling

Nova does not currently expose date/time to programs.

Save headers should reserve timestamp bytes immediately and write zero until the
platform provides time. The dialog should display blank or `--` for unknown
timestamps.

Platform follow-up:

- Add a shared date/time API to the NDK.
- Add a machine-visible date/time register block or FIO command.
- On hardware, NovaHost/ESP32 should fetch time over Wi-Fi at boot and publish
  it to Nova.
- Consider FPGA-side registers if software needs cheap reads without host FIO.
- Provide a simple BCD or binary `YY MM DD HH MM SS` representation that 6502
  code can display without expensive conversion.

## Test Plan

- Unit test `.NZS` header layout constants and story validation rules.
- Smoke-test save, quit/restart, restore for Z3-Z6 spec stories.
- Smoke-test two save slots with different descriptions.
- Add a V6 restore smoke that saves after a real graphics-area/header-art
  update, reboots, restores, and verifies the restored picture replay. Current
  coverage follows Zork Zero through the prologue to Roof, where OUTSIDEBIT
  switches the border art, saves there, reboots to the Banquet Hall castle
  frame, restores, and asserts the outside-frame gfx pixel at 43,43 after
  restore.
- Hardware smoke-test Zork Zero save/restore from the TUI dialog.
- Regression-test incompatible save rejection by changing checksum/serial in a
  fixture save.
