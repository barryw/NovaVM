# Fixed-Address Overlay Runtime

Nova overlays let an application keep a small resident kernel while loading
larger optional code paths into a shared CPU RAM slot. The first overlay runtime
is intentionally fixed-address: every overlay is linked for the exact RAM
address where it will execute.

## Files

Applications should include:

```asm
.include "overlay.inc"
...
.include "overlay.s"
```

Compatibility wrappers also exist at `ehbasic/overlay.inc` and
`ehbasic/overlay.s`, but new code should include `ehbasic/lib`.

## Overlay File Format

An overlay file starts with a 32-byte `NOVO` header, followed by the executable
payload. The payload is streamed into CPU RAM from file offset `$20`.

| Offset | Field | Notes |
| --- | --- | --- |
| `$00-$03` | Magic | ASCII `NOVO` |
| `$04` | Format | `OVL_FORMAT_VERSION`, currently `1` |
| `$05` | ABI major | `OVL_ABI_MAJOR`, currently `1` |
| `$06` | ABI minor | Informational |
| `$07` | Flags | Reserved for later |
| `$08-$09` | Load address | Required fixed CPU RAM load address |
| `$0A-$0B` | Payload size | Bytes copied from file offset `$20` |
| `$0C-$0D` | BSS size | Informational for now |
| `$0E-$0F` | Init entry | Absolute address, or `$0000` |
| `$10-$11` | Main entry | Absolute address, required by `overlay_call_main` |
| `$12-$13` | Tick entry | Absolute address, or `$0000` |
| `$14-$15` | Unload entry | Absolute address, or `$0000` |
| `$16-$17` | Module id | Product-defined numeric id |
| `$18-$19` | Module version | Product-defined version |
| `$1A-$1D` | Reserved | Must not be required by current loaders |
| `$1E-$1F` | Checksum | Reserved for later validation |

The current loader validates magic, format, ABI major, load address, nonzero
payload size, slot size, and 64K RAM wrapping. Checksum validation is reserved
for a later pass.

## Packaging

Use `e6502.Tools pack-overlay` to prepend the `NOVO` header to a linked payload:

```bash
dotnet run --project e6502.Tools -- pack-overlay \
  --input build/net_lobby.bin \
  --output build/NETLOBBY.OVL \
  --load $7000 \
  --max-size $2000 \
  --sym build/net_lobby.sym \
  --init net_lobby_init \
  --main net_lobby_main \
  --tick net_lobby_tick \
  --unload net_lobby_unload \
  --module-id 1 \
  --module-version 1
```

Entry arguments may be either absolute addresses (`$7000`, `0x7000`) or labels
from an ld65 `-Ln` symbol file. The packager validates that nonzero entry
addresses live inside the payload range and that the payload fits the fixed RAM
slot.

## Loader ABI

Before calling `overlay_load_fixed`, populate:

| Symbol | Meaning |
| --- | --- |
| `OVL_NAMEPTR_L/H` | Pointer to overlay filename |
| `OVL_NAMELEN` | Filename length |
| `OVL_LOADL/H` | Expected fixed load address |
| `OVL_MAXLENL/H` | Maximum bytes available in the overlay slot |

Then call:

```asm
        lda #<overlay_name
        sta OVL_NAMEPTR_L
        lda #>overlay_name
        sta OVL_NAMEPTR_H
        lda #overlay_name_end - overlay_name
        sta OVL_NAMELEN
        lda #<$7000
        sta OVL_LOADL
        lda #>$7000
        sta OVL_LOADH
        lda #<$2000
        sta OVL_MAXLENL
        lda #>$2000
        sta OVL_MAXLENH
        jsr overlay_load_fixed
        bne load_failed
        jsr overlay_call_init
        bne init_failed
```

`overlay_load_fixed` uses `PAGER.LOAD_FILE_PAGE`, so the backing host/NDI path is
the same direct file streaming path used by XRAM and other paged assets.

## Entry Calls

The resident application calls overlay entries through:

| Routine | Behavior |
| --- | --- |
| `overlay_call_init` | Calls init if present; missing init is OK |
| `overlay_call_main` | Calls main; missing main is an error |
| `overlay_call_tick` | Calls tick if present; missing tick is OK |
| `overlay_unload` | Calls unload if present, then clears active metadata |

Overlay entries return `A=0` for success. Any nonzero return is mapped to
`OVL_ERR_ENTRY`.

## Rules

- Link overlays for the fixed load address in their header.
- Keep the resident kernel, loader, and service vectors outside the overlay
  slot.
- Do not call one overlay directly from another overlay.
- Store long-lived product state in resident memory or an explicit state block,
  not inside disposable overlay code.
- Treat XRAM as cache/staging/data. CPU RAM is where overlay code executes.
