# Delta undo for the shared editor — done

Undo depth no longer depends on document size. Typing a character costs about
15 bytes of history instead of a whole buffer, so the 40-step walk-back the old
scheme could not manage on a 4 KB source now works on any document the editor
can open.

## What shipped

**Records are deltas.** A record is a 14-byte header — back-link, length and
cursor to restore, splice point, bytes to cut, bytes to insert — followed only
by the bytes its edit removed. `docs/assembly/textsvc.md` has the API.

**An edit is bracketed, not diffed.** `textsvc_undo_capture` opens a group and
shadows the buffer; `editbuf_make_gap` and `editbuf_close_gap` — the only two
routines that move buffer bytes — report each movement; `textsvc_undo_commit`
turns the accumulated bounds into one record. Everything between capture and
commit is a single undo step, so a block indent or a replace-all still comes
back in one keystroke.

The earlier sketch diffed the buffer against the pre-image instead. It worked,
but cost ~200 bytes more than the module had (see below), and an O(n) prefix and
suffix walk per keystroke on top of the pre-image copy.

**Applying a record is a splice the caller performs.** TEXTSVC publishes
`TEXTSVC_SPLICE_OFF/CUT/INS` and `editbuf_apply_result` widens or narrows the
run with the gap routines it already had, then asks for the payload with
`textsvc_undo_paste`. One implementation of the move, not two.

## Two traps this hit, both worth remembering

**Shelf slot 3 is at `$06C000`.** `SHELF_N = 4`, so the library shelf owns
`$060000-$06FFFF`, not `$060000-$06BFFF` as `libabi.inc` and `textsvc.inc` both
said. The redo arena had been sitting on slot 3 and got away with it only
because nothing writes there until the user presses redo; a per-keystroke
pre-image in the same place corrupted the paged-in module immediately. The map
is now undo `$040000`, redo `$048000`, clipboard `$050000`, pre-image `$070000`
(the largest genuinely free block, below the host-extension staging at
`$07C000`). A buffer too big to shadow gets no undo rather than a corrupt one.

**The editor module had 192 bytes of ROM free and the feature needed ~650.**
`EditorModule_HasRomHeadroom` enforces a $00C0 floor and says to reclaim space
rather than lower it, so this reclaimed it:

| Where | Bytes |
|---|---|
| `textsvc.s` 16-bit scratch into one indexed array with shared primitives | ~500 |
| key → command, command dispatch, raw-key and hook-vector tables | ~320 |
| the save/discard/cancel dialog's six runs from a table | ~115 |
| undo/redo apply merged into one body; the buffer move dropped for editbuf's | ~200 |

The module now has 296 bytes free. Note the CMP-chain-to-table transformation
is mechanical and there are more chains left (`editui.s`, `editbuf_paste`,
`editbuf_update_status`) if the next feature needs room.

## Test

`EditbufAssemblyRuntimeTests.UndoWalksBackFortyEditsAndTreatsABlockIndentAsOneStep`
runs the real editbuf/textsvc stack on a bus that models XMC window 3 (the flat
bus the other two fixtures use aliases every XRAM page onto 256 bytes, which
delta records outgrow immediately). It block-indents a 1700-byte document and
undoes it in one step, then types 40 characters, undoes all 40 and redoes them,
comparing the buffer byte for byte. As snapshots that history would be 68 KB —
more than the arena can ever hold, since every stack offset is 16-bit.

Suite at the end of this work: 1686 MSTest + 73 NovaPanel, Verilator 36/36.

## Still open (unchanged by this work)

- 4 KB document window: caret-position memory is skipped past the first window
  (the pager cannot describe a window-relative line — `editpager.s`, the
  `ep_window_off` guard), and project search only scans what it loads.
- Project search is directory-scoped, 16 files (`SEARCH_FILES`),
  first-match-forward, no results list.
- No bracket matching / `begin`-`end` navigation. Cheap, and the most
  Pascal-shaped gap.
- Search-and-replace has no confirm-each-occurrence mode.
- Nothing here has run on real ULX3S hardware — emulator and Verilator only.
