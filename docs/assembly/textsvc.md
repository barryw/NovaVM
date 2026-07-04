# Text Service Runtime

`software/runtime/asm/textsvc.inc` and `textsvc.s` provide shared editor text
services for all language front ends. The service owns clipboard and undo/redo
payload storage in platform-managed XRAM so language editors do not each invent
their own storage policy.

## XRAM Storage

The current backend uses full-buffer snapshots:

| Range | Use |
| --- | --- |
| `$040000-$047FFF` | Undo snapshot stack |
| `$048000-$04FFFF` | Redo snapshot stack |
| `$050000-$053FFF` | Shared clipboard |

These ranges are reserved by the platform. Applications should request XRAM
through `MEM_ALLOC` or `xmc_alloc_block` instead of using them directly.
Shared open-document storage is handled separately by `docbuf.inc` in
`$055000-$05CFFF`.

## Calling Convention

Clipboard copy and fetch use the XMC argument registers:

| Register | Meaning |
| --- | --- |
| `XMC_RAML/H` | CPU RAM source or destination pointer |
| `XMC_LENL/H` | Byte length for copy |

Undo/redo capture also reads `TEXTSVC_CURL/H` for the cursor position and uses
`XMC_RAML/H` plus `XMC_LENL/H` for the current buffer snapshot.

| Routine | Behavior |
| --- | --- |
| `textsvc_init` | Clears clipboard and undo/redo stack metadata. |
| `textsvc_clip_copy_from_ram` | Copies `XMC_LENL/H` bytes from `XMC_RAML/H` into the shared clipboard. |
| `textsvc_clip_fetch_to_ram` | Copies the shared clipboard to `XMC_RAML/H`. |
| `textsvc_undo_capture` | Pushes the current buffer and cursor onto the undo stack and clears redo history. |
| `textsvc_undo_apply` | Restores the latest undo snapshot and pushes the current buffer to redo. |
| `textsvc_redo_apply` | Restores the latest redo snapshot and pushes the current buffer to undo. |

Routines return `A=0` on success and `A=1` when the requested operation cannot
be performed, such as undo with no history or copying a block larger than the
clipboard arena.
