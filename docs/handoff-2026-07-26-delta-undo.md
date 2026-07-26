# Handoff — delta undo for the shared editor

**Goal: make editor undo depth independent of document size.** Everything below is
verified against the tree at `e61efcaf`; no re-derivation needed. Read this, then
`software/runtime/asm/textsvc.s` (622 lines — read it whole, it is the entire subject).

## State at handoff

- HEAD `e61efcaf`, clean, pushed. Suite green: **1685** MSTest + **73** NovaPanel, Verilator 36/36.
- This session shipped the editor/shell work in `e3e1e26a`, `6b8feccd`, `18a7b801`, then
  `e61efcaf` (arena doubling). Delta undo is **not started** — no partial work to clean up.

## The problem

`ts_stack_capture` (textsvc.s:333) pushes a **whole-buffer snapshot** per edit:
record = `buflen + 6` bytes. Depth is therefore `arena ÷ document size`.

| document | depth before `e61efcaf` | depth now |
|---|---|---|
| 1 KB | ~31 | ~63 |
| 4 KB | ~7 | ~15 |
| 32 KB | 1 | 2 |

Undo is *already* XRAM-backed — that is not the issue. More XRAM does not help, and
cannot: see the ceiling below.

### Two traps, both already hit

1. **`TEXTSVC_UNDO_ARENA_BYTES` cannot exceed `$FF00`.** Every stack offset in
   textsvc.s (`ts_used*`, `ts_top*`, `ts_off*`, `ts_record*`, `ts_end*`) is 16-bit.
   Setting `$10000` assembles cleanly and **silently disables undo**: `>$10000`
   evaluates to `$00`, so `ts_record_fits_arena` (textsvc.s:504) rejects every record.
   There is a comment in `textsvc.inc` saying this. Growing past 64 KB means widening
   every one of those offsets to 24-bit — do not do that; deltas make it moot.
2. **XRAM above `$040000` is nearly all spoken for.** Module slots 0-2 own
   `$060000-$06BFFF` (`libabi.inc`), host-extension staging is `$07C000`
   (`HOST_EXT_XRAM_*`), docbuf owns `$055000-$05CFFF` (`docbuf.inc`). Current map after
   `e61efcaf`: undo `$040000-$04FFFF`, redo `$06C000-$07BFFF`, clipboard `$050000-$053FFF`.
   **Free: `$054000-$054FFF` (4 KB) and `$05D000-$05FFFF` (12 KB).**

## The design

The key fact that makes this simple: **editbuf has both halves of every edit.**

- `editbuf_capture_undo` (editbuf.s:2929) — 11 call sites, runs **before** each mutation.
- `editbuf_after_change` (editbuf.s:2227) — 17 call sites, runs **after** each mutation
  (calls changed-hook, recount lines, compute line/col, adjust scroll, render).

So the delta can be computed directly, with no pending/lazy-flush state. An earlier
sketch of this work assumed records had to land one step behind; they do not.

### New XRAM

```
TEXTSVC_PRE_XRAM_L/M/H = $00/$D0/$05      ; $05D000, pre-image of the buffer
```
4 KB is enough (the editor's RAM window is `SOURCE_CAP = $1000`); the free block there
is 12 KB, so there is headroom.

### New/changed entry points

**`textsvc_undo_capture`** (textsvc.s:175) — stops pushing. Becomes:
copy live buffer → pre-image, save `ts_pre_len` / `ts_pre_cur`, set `ts_pre_valid`,
reset the redo stack (as today).

**`textsvc_undo_commit`** (new) — called from `editbuf_after_change`. If `ts_pre_valid`
is clear, return. Otherwise diff live against the pre-image:

```
p = length of common prefix
s = length of common suffix   (p + s must not overrun either length)
old_span = ts_pre_len  - p - s     ; bytes the edit removed  -> payload length
new_span = EDITBUF_LEN - p - s     ; bytes the edit added    -> bytes to cut on restore
```

Push one record, then clear `ts_pre_valid`. If `old_span == 0 && new_span == 0` push
nothing — this matters because several of the 17 `after_change` sites fire without a
buffer change and must be no-ops.

### Record format

Header grows 6 → 14 bytes (`TEXTSVC_STACK_HEADER_BYTES`). Payload is `old_span` bytes,
not `buflen`.

| offset | field | note |
|---|---|---|
| 0,1 | prev_top | unchanged — back-link, `$FFFF` = bottom |
| 2,3 | target_len | buffer length **after** restoring this record |
| 4,5 | target_cursor | cursor to restore |
| 6,7 | prefix `p` | splice point |
| 8,9 | new_span | bytes to remove from live at `p` |
| 10,11 | old_span | payload length = bytes to insert at `p` |
| 12,13 | reserved | keep the header even-sized |

Typing one character stores ~15 bytes instead of ~4096.

### Restore becomes a splice

`ts_stack_restore` (textsvc.s:421) currently copies the payload over the whole buffer.
It becomes:

1. read header;
2. memmove the live tail so the `new_span` bytes at `p` become room for `old_span`
   (grow → move up from the end; shrink → move down from the front);
3. `ts_copy_xram_to_ram` the payload into `live + p`;
4. publish `TEXTSVC_REST_LENL/H` and `TEXTSVC_REST_CURL/H` from the header;
5. pop (`ts_used = ts_top`, `ts_top = prev_top`) — unchanged.

### Redo

`textsvc_undo_apply` (textsvc.s:190) already pushes onto redo before restoring. The
push becomes the **inverse** of the record it is about to apply, built from live RAM
*before* the splice: `prefix = p`, `new_span = old_span`, `old_span = new_span`,
payload = `live[p .. p+new_span)`, `target_len` = current `EDITBUF_LEN`,
`target_cursor` = current cursor. `textsvc_redo_apply` (textsvc.s:213) is symmetric.

### The one guard you must not forget

`editbuf_do_undo` and `editbuf_do_redo` both end in `JMP editbuf_after_change`
(editbuf.s:2969, and the redo equivalent below it). Without a suppression flag, applying an undo
immediately commits a new delta onto the undo stack. Set a flag around the apply and
have `textsvc_undo_commit` return early when it is set.

## Primitives you already have

All in textsvc.s, driven by `ts_xl/ts_xm/ts_xh` (XRAM), `ts_raml/ts_ramh` (RAM),
`ts_worklenl/h` (count):

- `ts_map_xram` (548) — points XMC window 3 at `ts_x*`
- `ts_advance_xram` (559) / `ts_advance_ram` (571) — increment with remap on page cross
- `ts_copy_ram_to_xram` (584) / `ts_copy_xram_to_ram` (603)
- `ts_xaddr_from_active_off` (532) — arena offset → absolute XRAM address
- `ts_dec_worklen` (577)

The diff needs a *compare* loop (live RAM vs XRAM pre-image) — there is no such helper
yet; write one alongside `ts_copy_xram_to_ram` using the same window/advance pattern.
A suffix compare needs to walk backwards; `ts_advance_*` only go forward, so either add
decrement variants or compute the suffix by mapping from the end.

## Test to write

`e6502UnitTests/NovaPascalTests.cs`, next to `EditorRemembersCaretPositionPerFile`
(same harness: copy `novapascal.ndi` to `fd0.ndi`, boot, `EDIT` a saved file).

- Open a file, type **40** distinct characters, then press Ctrl-Z 40 times, and assert
  the buffer is byte-identical to the original. Under snapshots this fails around step
  15 on a 4 KB file; that failure is the regression test.
- Assert redo walks back forward.
- Keep `GenericEditorPagesAndSavesLargeXramDocuments` passing — it is the paged-document
  canary and the splice path must not break it.

## Sequencing suggestion

1. Add the pre-image + `ts_diff` compare helper + `textsvc_undo_commit`, but have commit
   still push a **full** record. Tree stays green, no behaviour change.
2. Switch the record to the delta format and rewrite restore as a splice. This is the
   risky step — the depth test above is what proves it.
3. Redo inverse.
4. Only then consider whether the arena can shrink (it can; deltas make 64 KB absurd).

## Still open after this (not part of delta undo)

- 4 KB document window: caret-position memory is skipped past the first window (the
  pager cannot describe a window-relative line — `editpager.s`, the `ep_window_off`
  guard), and project search only scans what it loads.
- Project search is directory-scoped, 16 files (`SEARCH_FILES`), first-match-forward,
  no results list.
- No bracket matching / `begin`-`end` navigation. Cheap, and the most Pascal-shaped gap.
- Search-and-replace has no confirm-each-occurrence mode.
- Nothing here has run on real ULX3S hardware — emulator and Verilator only.
