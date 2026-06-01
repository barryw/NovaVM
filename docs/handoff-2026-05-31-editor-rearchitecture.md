# NovaLogo / shared editor — rendering rearchitecture (research-backed)

Date: 2026-05-31. Supersedes the editbuf redisplay approach. Research sources at bottom.

## The fundamental mistake (why smears persist)

editbuf keeps the document in a gap buffer in a SEPARATE RAM area, then on every
edit **re-derives the affected screen row(s) from the buffer and writes every cell
with the CPU** (`editbuf_render` / `editbuf_render_row` / `render_current_line`).
The 60 Hz raster catches the CPU mid-row-write → "character smears that progress
down the screen." This is the path **no well-built 8-bit screen editor took.**

Canonical rule (Finseth, *Craft of Text Editing*, Ch.7, memory-mapped displays):
> the screen RAM **is** the rendered representation. You mutate it in place with the
> smallest possible block op and never touch a cell that didn't change.

We are Finseth "Case 3" (raster reading mid-write only yields a 1-frame-stale cell,
never corruption). So edit char RAM ($AA00) + color RAM ($B1D0) **in place** as the
live render; keep the gap buffer as the authoritative document (mirrored edits, NOT
rebuilds). No third shadow buffer.

## Redesign (each maps to our VGC + DMA/blitter)

1. **Char insert mid-line**: shift cells `[col..eol]` RIGHT by 1 via ONE block move
   in char RAM + ONE in color RAM, then write the new char+color cell. Touches
   `eol-col` cells, not 80. **Overlap direction matters: insert shifts right
   (dst>src) → copy HIGH-address-first (descending)** or it smears the source byte.
2. **Char delete**: shift `[col+1..eol]` LEFT by 1 (ascending, dst<src), clear the
   vacated last cell. One char + one color block move + 2 cell writes.
3. **Line insert/delete mid-screen** (Enter split / Backspace-at-col0 join): block-
   move the rows BELOW the edit down/up by one full row (char+color RAM), clear the
   freed row. Rows above never move.
4. **Scroll viewport = biggest win**: lay char/color RAM as a RING larger than the
   window, **page-aligned stride (pad 80-col rows to 128 or 256)** so base math is a
   single add + mask. Scroll = write the screen-base / top_row (ZERO copies), paint
   only the one newly-exposed row. **VERIFY: does the VGC have a screen-base/top-row
   register?** If not, fall back to a DMA block-move scroll (still only on scroll,
   not per keystroke) or the text scroll regs.
5. **Cursor**: hardware x/y register only (we have VGC_CURSX/Y). Move = write x/y,
   touch ZERO cells. (Already mostly true after the after_move fix.)
6. **Tearing**: minimal-update alone removes keystroke tearing (2–4 cells/edit vs
   ~160). For the few bulk moves (mid-screen line insert, exposed scroll row),
   **vblank-gate the blitter/DMA** (kick the copy at top-of-vblank so it races ahead
   of the beam) — NOT a per-render vsync. Prefer base-pointer scroll (atomic) over
   block-move when possible.
7. **Dirty tracking**: per-row dirty byte array (50 entries), + optional LDTB1-style
   "starts-logical-line" bit if we add wrapping. It's a validation aid — there is NO
   per-frame "scan buffer + repaint dirty lines" pass in steady state. Delete that.

## HARDWARE VERIFICATION RESULTS (done 2026-05-31)

1. **DMA char/color block copies: YES.** `DmaSpaceVgcChar=$01`, `DmaSpaceVgcColor=$02`,
   24-bit src/dst, length, fill mode. Registers `$BA63..$BA75` (Cmd/Status/SrcSpace/
   DstSpace/Src24/Dst24/Len24/Mode/Fill/Count). Status busy/ok/error at `$BA64`.
   ⚠️ **DMA copies ASCENDING ONLY** (`VirtualDmaController` line 92: `for i=0..len:
   dst[i]=src[i]`) — **no direction/descending bit.** So an overlapping RIGHT-shift
   (char insert, dst>src) **smears the source byte.** Two ways to fix:
   - **(A) XRAM-staging (no RTL change, do this first):** copy `[col..eol]` → XRAM
     scratch (space `$05`), then XRAM → `[col+1..eol+1]`. Two DMA ops, no overlap.
     Lines ≤80 cells so it's cheap. Same for delete (left) and row moves.
   - **(B) add a descending/direction bit** to DMA (emulator + FPGA) later for 1-op
     shifts. Blitter (`$BA83`, `VirtualBlitterController`) is ALSO ascending row-by-row
     — same caveat; prefer the 1-D DMA for cell/line shifts.
2. **NO text screen-base / top-row register.** Char RAM fixed at `$AA00`, color `$B1D0`.
   ⚠️ **Ring-buffer base-pointer scroll is NOT available for the text layer.** Scroll =
   **DMA block-move the visible char+color region up/down one row + paint the exposed
   row** (only on scroll, still a huge win vs per-keystroke). Smooth/pixel scroll exists
   (`RegScrollY=$A006` + `ScrollCtlText` bit in `$A0EA`) but that's sub-line cosmetic,
   not coarse row scroll — combine if you want smoothness.
3. **Vblank gate = VGC_FRAME ($A008) tick.** No separate in-vblank status bit needed;
   wait for the frame counter to change (like `ext_waitvbl`) then kick the bulk DMA at
   the frame boundary so the copy races the beam. Single-cell keystroke writes need no
   gate.

**Net for the rewrite:** keep gap buffer; mutate char/color RAM in place via DMA;
do overlapping shifts with **XRAM-staging** (no RTL change to start); scroll via
DMA region-move (no base register); vblank-gate bulk DMA via VGC_FRAME. The
descending-DMA-bit and smooth-scroll are optional later optimizations.

## Hardware facts (original checklist — now answered above)

- Char RAM `$AA00`, color RAM `$B1D0`; screen is **80×50 = 4000 cells** (NOVA_SCREEN_ROWS
  =50; CLAUDE.md's "80x25/2000 bytes" map line is stale).
- **DMA**: `DmaSpaceVgcChar=$01`, `DmaSpaceVgcColor=$02` (VgcConstants). Confirm DMA
  can do char→char and color→color block copies AND has a **direction/descending bit**
  for overlapping right-shifts (insert). If not, the blitter ($BA83) — check it.
- Confirm whether a **screen-base / top-line register** exists (for ring scroll). If
  not, scroll via DMA block-move of the visible region (only on scroll).
- Confirm the **vblank gate** mechanism: VGC_FRAME ($A008) ticks per frame; whether
  there's a raster/scanline compare or a "in vblank" status bit to start a copy at
  top-of-vblank.

## Implementation order (rewrites editbuf's render path; keep the gap buffer)

1. Verify hardware (above). Write a tiny `editbuf_cell_addr(row,col)->char/color RAM`.
2. `editbuf_shift_right(row,col,count)` / `editbuf_shift_left` via DMA (char+color).
3. Rewrite insert_char/backspace/delete to mutate screen RAM in place (no row render).
4. Rewrite newline/line-join to block-move rows + paint freed row.
5. Replace `editbuf_render`'s per-edit role; keep a one-time full paint for editor
   OPEN + a from-scratch paint only on scroll/window re-anchor.
6. Scroll: ring base register if available, else DMA region move; paint exposed row.
7. Update EditBufVmTests (they assert CPU-rendered rows; now screen RAM is mutated —
   final content identical, so most assertions hold; verify timing/settle).

This is a from-scratch render-path rewrite (keep gap buffer + chrome). Best done with
fresh context. The chrome redesign (no frame / 48 lines / status bar name·dirty·x,y,
see project_shared_editor_stage2.md Increment 3) folds in naturally during this.

## Sources
- Finseth, *Craft of Text Editing* Ch.7 (redisplay, memory-mapped Case 1/2/3,
  per-line dirty + unique-id line tracking): https://web.mit.edu/~yandros/doc/craft-text-editing/Chapter-7.html
- 6502.org "Text Editor Shifting Mechanics" (gap buffer, block-move shift, overlap dir):
  https://6502.org/forum/viewtopic.php?f=2&t=7898
- Player/Missile Atari scrolling (LMS base-pointer scroll = no copy; page-per-row stride):
  https://playermissile.com/scrolling_tutorial/index.html
- 1amstudios / c64brain (why block-moving screen+color RAM tears; vblank/raster gating):
  http://1amstudios.com/2014/12/07/c64-smooth-scrolling/
- C64 LDTB1 line-link table: https://www.zimmers.net/anonftp/pub/cbm/maps/C64.MemoryMap.txt
- Gap buffer: https://en.wikipedia.org/wiki/Gap_buffer
