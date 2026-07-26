# Text Service Runtime

`software/runtime/asm/textsvc.inc` and `textsvc.s` provide shared editor text
services for all language front ends. The service owns clipboard and undo/redo
payload storage in platform-managed XRAM so language editors do not each invent
their own storage policy.

## XRAM Storage

Undo and redo records are deltas, so history depth does not depend on document
size — a typed character costs about 15 bytes rather than a whole buffer:

| Range | Use |
| --- | --- |
| `$040000-$047FFF` | Undo record stack |
| `$048000-$04FFFF` | Redo record stack |
| `$050000-$053FFF` | Shared clipboard |
| `$070000-$07BFFF` | Pre-image of the buffer being edited |

Each record is a 14-byte header (back-link, buffer length and cursor to restore,
splice point, bytes to cut, bytes to insert) followed by only the bytes its edit
removed. Applying one describes a splice for the caller to perform.

The pre-image bounds the buffer TEXTSVC can shadow. A buffer larger than the
reservation gets no undo rather than a corrupt one: capture reports failure and
no record is made.

These ranges are reserved by the platform. Applications should request XRAM
through `MEM_ALLOC` or `xmc_alloc_block` instead of using them directly.
Shared open-document storage is handled separately by `docbuf.inc` in
`$055000-$05CFFF`, and the paged-library shelf owns `$060000-$06FFFF`
(`SHELF_N` slots of 16 KB).

## Calling Convention

Clipboard copy and fetch use the XMC argument registers:

| Register | Meaning |
| --- | --- |
| `XMC_RAML/H` | CPU RAM source or destination pointer |
| `XMC_LENL/H` | Byte length for copy |

Undo capture also reads `TEXTSVC_CURL/H` for the cursor position and uses
`XMC_RAML/H` plus `XMC_LENL/H` for the buffer being edited. The two reporting
routines take an offset in `TEXTSVC_SPANL/H` and a byte count in
`TEXTSVC_CNTL/H`.

An edit is bracketed: capture before it, commit after, and every byte movement
in between reported. Applying a record is the mirror image — TEXTSVC says where
the splice goes and how wide it is, the caller moves its own buffer to fit and
then asks for the payload, so there is only one implementation of the move.

Everything inside one bracket becomes a single undo step, so a block indent or
a replace-all comes back in one keystroke.

| Routine | Behavior |
| --- | --- |
| `textsvc_init` | Clears clipboard and undo/redo stack metadata. |
| `textsvc_clip_copy_from_ram` | Copies `XMC_LENL/H` bytes from `XMC_RAML/H` into the shared clipboard. |
| `textsvc_clip_fetch_to_ram` | Copies the shared clipboard to `XMC_RAML/H`. |
| `textsvc_undo_capture` | Opens an undo group: keeps a pre-image of the buffer and clears redo history. |
| `textsvc_undo_inserted` | Reports `TEXTSVC_CNT` bytes opened up at `TEXTSVC_SPAN`. |
| `textsvc_undo_deleted` | Reports `TEXTSVC_CNT` bytes about to be removed at `TEXTSVC_SPAN`. |
| `textsvc_undo_commit` | Closes the group and pushes one record describing everything it did. |
| `textsvc_undo_apply` | Pops the newest undo record, pushes its inverse to redo, and publishes the splice in `TEXTSVC_SPLICE_OFF/CUT/INS` plus `TEXTSVC_REST_LEN/CUR`. |
| `textsvc_redo_apply` | The same in the other direction. |
| `textsvc_undo_paste` | Writes the popped record's payload to `XMC_RAML/H` once the caller has made room for it. |
| `textsvc_undo_reset` | Drops all history. Required whenever the RAM buffer stops describing the same text, such as a pager window swap. |

Routines return `A=0` on success and `A=1` when the requested operation cannot
be performed, such as undo with no history or copying a block larger than the
clipboard arena.
