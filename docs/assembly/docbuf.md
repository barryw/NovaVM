# Document Buffer Runtime

`software/runtime/asm/docbuf.inc` and `docbuf.s` provide shared open-document
buffer storage for native editors. The active document remains in the caller's
RAM edit buffer. Inactive documents are snapshotted to XRAM, so Pascal, Logo,
Forth, and future language shells can share one buffer-switching model.

## XRAM Storage

The current backend uses fixed 4 KiB full-buffer snapshots:

| Range | Use |
| --- | --- |
| `$055000-$055FFF` | Document slot 0 |
| `$056000-$056FFF` | Document slot 1 |
| `$057000-$057FFF` | Document slot 2 |
| `$058000-$058FFF` | Document slot 3 |
| `$059000-$059FFF` | Document slot 4 |
| `$05A000-$05AFFF` | Document slot 5 |
| `$05B000-$05BFFF` | Document slot 6 |
| `$05C000-$05CFFF` | Document slot 7 |

This range is platform-owned editor scratch. Applications should not use it
directly. Each document slot can hold 4 KiB. The active RAM edit buffer can be
smaller than the document slot when a host uses the windowed APIs below.

## Host Contract

Before calling a store, add, switch, or select routine, the host copies the
current editor state into `DOCBUF_*`:

| Symbol | Meaning |
| --- | --- |
| `DOCBUF_RAML/H` | Active RAM edit buffer pointer. |
| `DOCBUF_CAPL/H` | Active RAM edit buffer capacity. |
| `DOCBUF_LENL/H` | Current document length. |
| `DOCBUF_CURL/H` | Current cursor byte offset. |
| `DOCBUF_DIRTY` | Current dirty flag. |
| `DOCBUF_RANGE_OFFL/H` | Full-document offset for a byte-range replacement. |
| `DOCBUF_DELETE_LENL/H` | Number of bytes to remove at `DOCBUF_RANGE_OFFL/H`. |
| `DOCBUF_INSERT_LENL/H` | Number of bytes to insert from `DOCBUF_RAML/H`. |
| `DOCBUF_WINDOW_OFFL/H` | Full-document offset to load into the RAM window. |
| `DOCBUF_WINDOW_LENL/H` | Requested window length; updated to the loaded length. |
| `DOCBUF_TITLEL/H` | NUL-terminated title string pointer. |
| `DOCBUF_TYPEL/H` | NUL-terminated type/status string pointer. |

After a successful load, switch, or select routine, the host copies those values
back into the editor and redraws.

For full-snapshot XRAM-backed editor sessions, the host should also install
`EDITBUF_CHANGED_VECL/H`. That hook runs after editbuf changes document bytes or
dirty-state metadata. The hook should copy editbuf state into `DOCBUF_*` and
call `docbuf_store_active`, making the active XRAM slot the canonical document
copy even before save or buffer switch.

For windowed XRAM-backed sessions, the active XRAM slot is still canonical. The
host loads a RAM slice with `docbuf_load_window`, tracks that slice's full-file
offset and previous length, and calls `docbuf_replace_range` from the changed
hook to replace that slice with the current RAM edit buffer contents. This first
backend supports one loaded window at a time inside a fixed 4 KiB slot. Open
should populate the canonical XRAM slot first, then load the visible window.
Save should write or stream the canonical XRAM document, not only the visible
RAM slice.

## Routine Reference

Routines return `A=0` on success and `A=1` on error.

| Routine | Behavior |
| --- | --- |
| `docbuf_init` | Clears the document list and active selection. |
| `docbuf_add_from_ram` | Adds a new document from the current RAM buffer. |
| `docbuf_store_active` | Stores the active document metadata and RAM bytes into its XRAM slot. |
| `docbuf_load_active` | Loads the active slot into the configured RAM buffer and updates `DOCBUF_*`. |
| `docbuf_replace_range` | Replaces bytes in the active XRAM slot at `DOCBUF_RANGE_OFFL/H` using `DOCBUF_RAML/H`. |
| `docbuf_load_window` | Loads a bounded slice of the active XRAM slot into `DOCBUF_RAML/H`. |
| `docbuf_next` | Stores the active document, advances with wrap, then loads the new active document. |
| `docbuf_previous` | Stores the active document, moves backward with wrap, then loads the new active document. |
| `docbuf_select` | Stores the active document, selects `DOCBUF_SELECTED`, then loads it. |
| `docbuf_build_list_rows` | Builds fixed-width rows in `DOCBUF_LIST_ROWS` for `nui_pick_list`. |

## Buffer Picker

`docbuf_build_list_rows` creates `DOCBUF_COUNT` rows of `DOCBUF_ROW_WIDTH`
bytes. Each row starts with `>` for the active document and `*` for a dirty
document, followed by a padded title. Hosts can pass `DOCBUF_LIST_ROWS` to
`nui_pick_list` and then call `docbuf_select` when `NUI_RESULT` is OK.
