# Zork Zero Window-Op Capture — M2 Research Input

Every V6 window/screen operation Zork Zero issues through the NOVAZ6 segment
(plus the three ROM-side layout ops), captured turn-by-turn with the new NZ6
dispatch trace in `Nova.NovaZ.Smoke`. This is the empirical input for the M2
(full window-model semantics) plan. All numbers below come from the committed
capture, not from the Z-Machine spec.

## Raw data + regeneration

- Raw trace (committed): `docs/plans/data/2026-06-10-nz6-trace-zork-zero.txt`
  (1837 lines, 57KB; `.txt` because `*.log` is gitignored).
- Regenerate: `make -C examples/novaz capture-z6-trace PROJECT=zork-zero`
  (writes `examples/novaz/build/nz6-trace-zork-zero.log`). Requires
  `projects/zork-zero/STORY.BIN` locally. The run is **bit-for-bit
  deterministic** (verified: two consecutive runs produced identical md5) —
  the emulator has no wall-clock inputs and the script is fixed.
- Trace mode: env `NOVAZ_SMOKE_TRACE_NZ6=<path>` arms a per-instruction PC
  watch on `nz6_entry` ($2002, from `build/novaz6.sym`) plus the ROM handlers
  `zvm_erase_line`/`zvm_set_text_style`/`zvm_buffer_mode` (from
  `build/runtime.sym`). Line format:
  `<seq> <id> <opname> count=<n> ops=<op0>,<op1>,<op2>,<op3>` (hex; only the
  first `count` operands are valid — later slots are stale decode residue).
  `--- turn: <command>` markers group ops by input; `--- key:` markers mark
  harness-injected keys. Capture script:
  `examples/novaz/projects/zork-zero/capture.txt` (20 turns; `!cmd` lines are
  the harness's fire-and-settle mode added for this capture).
- Capture stats: 20 scripted turns, **475 traced ops** (86 boot,
  389 in-turn), total wall time **~21 s** (per-instruction tracing included —
  no hybrid loop needed).
- Game timeline in this capture: turns 1–3 are full game turns in the
  Banquet Hall prologue; Megaboz's fireball kills the servant during turn
  4's settle window (the prologue is interlude-clocked, not turn-clocked);
  turns 5–14 are death-menu re-prompts; turns 15–20 sit at the
  RESTART/RESTORE menu and emit **zero** window ops.

## Per-op totals (boot vs per-turn)

| op | boot | turns | total |
|---|---:|---:|---:|
| pull (VAR:9, V6 store form) | 0 | 130 | 130 |
| push_stack (EXT:24) | 0 | 112 | 112 |
| set_cursor (VAR) | 14 | 43 | 57 |
| get_wind_prop (EXT:19) | 13 | 27 | 40 |
| set_window (VAR) | 9 | 28 | 37 |
| set_colour (VAR) | 8 | 28 | 36 |
| draw_picture (EXT:5) | 12 | 1 | 13 |
| picture_data (EXT:6) | 12 | 1 | 13 |
| pop_stack (EXT:21) | 0 | 12 | 12 |
| put_wind_prop (EXT:25) | 4 | 2 | 6 |
| window_size (EXT:17) | 4 | 0 | 4 |
| set_text_style (ROM-side) | 2 | 2 | 4 |
| set_margins (EXT:8) | 2 | 1 | 3 |
| move_window (EXT:16) | 2 | 0 | 2 |
| mouse_window (EXT:23) | 1 | 0 | 1 |
| reset (segment id $00, ROM boot hook) | 1 | 0 | 1 |
| scroll_window (EXT:20) | 0 | 1 | 1 |
| picture_table (EXT:28) | 1 | 0 | 1 |
| get_cursor (VAR) | 0 | 1 | 1 |
| erase_window (VAR) | 1 | 0 | 1 |

Never called in this capture: **split_window, erase_line, buffer_mode,
window_style, buffer_screen, erase_picture, print_form, make_menu,
read_mouse** (the last two were exercised only by the synthetic z6-spec
story, not by Zork Zero).

## Boot-time op sequence (annotated)

Sequence numbers from the trace; full list is seqs 0–85.

```
0   reset                          ; ROM hook after segment load
1   mouse_window -1                ; mouse unrestricted
2   window_size 7, 50, 80          ; window 7 := full screen (50 rows x 80 cols)
3-8 picture_data {17E,17F,17D,1E2,1E3,1DE}, table 6E3B   ; probe art assets
9   set_colour 2, 9                ; black on white
10  set_window 7                   ; full-screen window for the title
11  set_colour 2, 9
12  set_window 0
13  erase_window -1                ; reset + clear (the only erase_window ever)
14  picture_data 183, 6E3B
15  move_window 0, 6, 6            ; *** the layout ***
16  window_size 0, 45, 70          ;   window 0 = rows 6-50, cols 6-75
17  move_window 1, 1, 1            ;   (bordered playfield, inset 6,6)
18  window_size 1, 5, 80           ;   window 1 = top 5 rows, full width banner
19  get_wind_prop 0, 0             ;   read back y-coord
20  get_wind_prop 0, 3             ;   read back x-size
21  window_size 0, 45, 70          ;   re-asserted
22  set_window 7                   ; title screen drawing, full screen:
23  draw_picture 5, 1, 1           ;   title art at 1,1
24-26 picture_data {5,1F1,1F2}
27  picture_table CFF0             ; cache hint
28  set_window 1
29-45 set_cursor / set_colour interleave   ; title text placement, incl.
    ;   NEGATIVE coordinates: set_cursor 3,-8; 2,-17; 2,-8; 3,-2
    ;   (game centres off header metrics; see unit-mismatch finding)
46-54 draw_picture {11,A,B,14,D,16,F,18} at 3,3 + 1E1 at 5,5  ; title compositing
55  set_window 0
56  picture_data 2, 710F
57-61 get_wind_prop 0, {2,3,4,4,4}  ; measure window 0 (y-size, x-size, y-cursor)
62  draw_picture 2, 6, 6            ; border art around the playfield
63  set_margins 1, 0                ; window 0 margins
64  put_wind_prop 0, 9, FFFF        ; *** newline-interrupt countdown := -1
65  put_wind_prop 0, 8, 384B        ; *** newline-interrupt routine := $384B
66  get_wind_prop 0, 9
[ MORE ] paging begins; per page the game re-runs its screen-refresh motif:
67  set_text_style 2 ... set_text_style 0      ; bold room/banner title
69-78  picture_data D8 -> get_wind_props -> draw_picture D8,6,6
       -> set_margins 1,0 -> put_wind_prop 0,9,-1 -> put_wind_prop 0,8,$384B
79-85  set_window 1; set_cursor 0,1; set_colour 2,-1; set_cursor 2,9 (x2);
       set_colour 2,9; set_window 0           ; banner update + return
```

What the game is building: a **full-screen window 7** for the title page, a
**5-row full-width banner (window 1)** at the top, and a **45x70 playfield
(window 0) inset at (6,6)** framed by border art (picture 2 / picture $D8
redrawn at 6,6 on every refresh). It sizes windows with
`move_window`+`window_size` — **never `split_window`**.

## Per-turn op pattern

A full game turn (turns 1–3, e.g. `inventory` = 91 ops) is:

1. **Engine bookkeeping, not screen work**: ~43 `pull` + ~37 `push_stack` +
   ~4 `pop_stack` on two user stacks in dynamic memory (`$696C` hot,
   `$69C0` secondary). Zork Zero runs its parser/daemon machinery on V6 user
   stacks; `pull` appears in both forms — count=1 (user-stack operand, 76
   calls) and count=0 (game-stack form, 54 calls).
2. **The pre-read banner redraw** (the input-line redraw sequence; identical
   9-op block before *every* read, turns 1–14):

   ```
   set_window 1          ; banner
   set_cursor 0, 1       ; row 0 (!), col 1 — verbatim; game's own value
   set_colour 2, -1      ; black fg, keep bg
   set_cursor 2, 9       ; row 2, col 9 (header units = cells, see below)
   set_cursor 2, 9
   set_colour 2, 9       ; black on white
   set_window 0          ; back to playfield
   get_wind_prop 0, 3    ; x-size  ─ the game measures window 0 to lay out
   get_wind_prop 0, 5    ; x-cursor ┘ the input line itself
   ```

   The game then prints the prompt and echoes input with **plain prints — no
   erase_line, no erase_window, no window op clears the old input line**. The
   M1 stale-text-at-line-end bug therefore cannot be fixed by implementing
   `erase_line` (Zork Zero never calls it); the clearing has to come from the
   window model itself (scrolling must blank vacated rows; prints overwrite
   within margins).
3. **Response printing**: zero window ops for ordinary responses (buffered
   prints into window 0). Room titles are bracketed by
   `set_text_style 2` / `set_text_style 0` (bold).
4. **Screen refresh / paging** (turn 1 only in this capture): the same
   refresh motif as boot —
   `picture_data $D8 → get_wind_props → get_cursor(table $710F) →
   scroll_window 0, $97 → set_cursor $2E,1 → draw_picture $D8 →
   set_margins 1,0 → put_wind_prop 0,9,-1 → put_wind_prop 0,8,$384B`.
5. **Death/menu turns** (15–20): the `(Type RESTART, RESTORE, ...)` re-prompt
   loop issues **zero** window ops — pure prints + read.

## Distinct operand patterns per op

- `set_window`: only windows **0** (x18), **1** (x17), **7** (x2). Windows
  2–6 never selected.
- `set_cursor` (count=2 always; no 3-operand window form): dominant
  `2,9` (x32) and `0,1` (x16); singles `1,1`, `2,1`, `3,1`, `2E,1`, and the
  boot-title negatives `3,FFF8`, `2,FFEF`, `2,FFF8`, `3,FFFE`. Coordinates
  are **arbitrary cell values, not 4k+1-aligned** — because the M1 header
  advertises 80x50 **units with a 1x1 font** (`zstory.s` writes $22/$24 =
  80/50, $26/$27 = 1), the game computes and sends coordinates in *cells*.
- `get_wind_prop`: always window 0; props **3** x-size (x15), **5** x-cursor
  (x11), **4** y-cursor (x9), **2** y-size (x3), **0** y-coord (x1),
  **9** countdown (x1).
- `put_wind_prop`: only `0,9,FFFF` (x3) and `0,8,384B` (x3) — the
  newline-interrupt countdown/routine pair, re-armed on every screen refresh.
- `window_size`: `7,50,80` (x1), `0,45,70` (x2), `1,5,80` (x1).
- `move_window`: `0,6,6` and `1,1,1` (boot only).
- `erase_window`: `-1` once (boot). `-2`/`n` forms never used.
- `set_colour`: `2,9` (x19) and `2,-1` (x17) only.
- `set_margins`: `1,0` (x3) — count=2, no window operand.
- `scroll_window`: `0,$97` once (151 units — corrupted by the unit-mismatch
  bug below, so treat the *presence* of the call as the signal, not the
  value).
- `draw_picture`: 3 operands everywhere; title pics at `1,1`/`3,3`/`5,5`,
  border pic `2`/`$D8` at `6,6` and once `$D8` at `$33,6`.
- `picture_data`: pic ids probed at boot ($17D–$1E3, $183, 5, $1F1, $1F2, 2)
  into tables $6E3B/$710F; per-refresh `$D8,710F`.
- `mouse_window`: `-1` once (no restriction).
- `pull`: count=1 user-stack form (stack $696C/$69C0) x76, count=0 game-stack
  form x54. `push_stack`/`pop_stack`: always 2 operands, same two stacks.

## Findings M2 must act on

1. **Unit-system mismatch (confirmed M1 bug).** `zstory.s` advertises
   80x50 units, font 1x1 (header $22/$24/$26/$27) — so the game thinks
   **1 unit = 1 cell** and sends cell coordinates. But `zvm6.s` implements
   1 unit = 1 gfx pixel with a 4x4 font (`cell = (unit-1)>>2`,
   `get_cursor` returns `cell*4+1`). Consequences visible in the capture:
   every `set_cursor 2,9` lands at cell (0,2) instead of (1,8), and
   `get_cursor` feeds back inflated values — the game's refresh computed
   `scroll_window 0, 151` from a 4x-scaled cursor. M2 must pick ONE unit
   system across header + segment (units = cells is the simplest match to
   what the game already sends).
2. **Layout is move_window/window_size, not split_window.** M1 maps
   window 1 to the ROM's `zvm_split_lines` upper-window machinery via
   `split_window`, which Zork Zero never calls. M2's renderer must honor the
   stored prop table: window 1 = rows 1–5 full width, window 0 = 45x70 at
   (6,6), window 7 = full screen. This is the "new text lands above old
   text" bug: M1 stores these but the ROM still renders window 0
   full-screen.
3. **No erase_line anywhere.** Stale text at line ends must be fixed by
   window-0 scroll semantics (vacated rows blanked, prints clipped to the
   45x70 rect + margins), not by implementing erase_line for Zork Zero's
   sake.
4. **scroll_window is real, not de-scopable.** It's the game's own
   page-reset mechanism (called from the refresh routine after measuring the
   cursor). M1 stubs it to RTS; M2 needs a working implementation (blank the
   vacated area).
5. **Newline-interrupt props 8/9 are actively armed** (routine $384B,
   countdown -1) on every refresh. With countdown -1 the interrupt never
   fires, so M2 can keep *storing* them — but the read-back of prop 9 means
   the values must round-trip exactly.
6. **User stacks + V6 pull are hot-path** (254 calls in 3 real turns). The
   M1 implementations are load-bearing; M2 must not regress them.
7. **draw_picture/picture_data run every refresh** (border pic $D8). The
   honest "no pictures" picture_data answer is what keeps the game in
   text-only mode; keep it exact. `picture_table`/`mouse_window` are
   one-shot boot no-ops — fine as stubs.
8. **De-scope candidates for M2** (never called by Zork Zero): window_style,
   buffer_screen, buffer_mode, erase_line, split_window (V6 path),
   print_form, make_menu, read_mouse, erase_picture, get_cursor beyond the
   single refresh use, erase_window -2/n forms.
9. **Harness side-finding:** the boot [ MORE ] auto-answer fired **1337**
   times — stale `[ MORE ]` text (never erased, finding 3) keeps re-matching
   after the game moved on, so the harness spams CR into live reads. Any M2
   erase work will collapse this; until then capture scripts must use the
   fire-and-settle `!cmd` mode (`Nova.NovaZ.Smoke`), added with this
   capture.
10. **set_window targets only 0/1/7** — an 8-window table is still right
    (props are indexed by number), but rendering work can prioritize those
    three.

## Post-M2 capture (2026-06-10)

Regenerated with M2 shipped (units = cells, windows = vtext regions,
in-window scroll, real `scroll_window`). Raw trace committed beside the M1
one: `docs/plans/data/2026-06-10-nz6-trace-zork-zero-post-m2.txt`
(1988 lines, 60KB). Same 20-turn capture script, still bit-deterministic.

### Stats vs the M1 capture

- **474 traced ops** (88 boot + 386 in-turn) vs 475 (86 + 389).
- New: 2 × `select` hooks (id $08, the M2 `zvm_select_active_window` V6
  path) at boot.
- **Gone: `get_cursor` (was 1) and `scroll_window` (was 1).** Both belonged
  to the `<G? .Y .YLEFT>` branch of `MARGINAL-PIC` (`pic.zil`): with the 4×
  cursor inflation the game thought the drop-cap picture didn't fit and
  issued `get_cursor` → `scroll_window 0,$97` → `set_cursor $2E,1`. With
  cell units the picture height (0, no pictures) never exceeds the room
  below the cursor, so the branch is rightly skipped. The plan's risk-5
  detector (residual get_cursor/scroll asymmetry) is clean.
- All coordinates in the trace are sane cells; the per-turn refresh motif's
  `draw_picture $D8` lands at `$32,6` (row 50 = window bottom + 1, the
  game's border row) instead of M1's `$33,6`.
- `morePrompts` = **1489**, all during boot, all real (see below).

### Finding 9 was wrong: the boot MORE storm is the game's

The M1 capture blamed the 1337 boot `[ MORE ]` auto-answers on stale MORE
text re-matching (finding 9). The post-M2 evidence overturns that:

- The storm is one contiguous block between boot seq 68 (`get_wind_prop
  0,9`) and seq 69 — i.e. during the prologue, before any read.
- The game is executing a `new_line` loop at zvm_pc $1CC7B with a local
  counting down from $FFFF: it is `CLEAR-CRCNT` (`prologue.zil`), which
  reads the CR-interrupt countdown (window prop 9) back and prints that
  many newlines ("in case illuminated letter is taller than intro").
- The countdown was armed by `MARGINAL-PIC` (`pic.zil`): `WINPUT 0 ,WCRCNT
  <pic height in lines>` — with the drop-cap picture absent the armed value
  is -1 ($FFFF), so the loop prints ~65.5K newlines.
- **dfrotz reproduces the identical flood** (~1340 screen lines of blanks
  between the prologue paragraph and Banquet Hall on an 80×50 dumb
  terminal).
- The MORE counts are exact page math of the storm: 65525 newlines through
  M2's 44-line playfield pages = **1489**; through M1's 49-line full-screen
  pages = **1337**. Both numbers were real `[ MORE ]` waits, not harness
  spam. (The M2 count being *higher* is the layout fix working: smaller
  window, more pages.)

### Finding 5 refined: the CR interrupt is real interpreter work (M3)

Per the Infocom YZIP spec ("Carriage Return Interrupt"): *"Before the
interpreter outputs a carriage return, it checks CRCNT, and if it is
non-zero, decrements it. If CRCNT reaches zero by such an operation, the
contents of CRFUNC are called as a function address."* Frotz implements
exactly this (`screen.c countdown()`, plus a Zork Zero r393 fire-at-end
quirk). M2 still stores props 8/9 verbatim and never fires — in text-only
mode the only observable delta is the storm length (65535 vs ~65525
newlines), but in M3 the countdown is load-bearing: it releases the
drop-cap margins (`RESET-MARGIN` = the $384B routine the game arms every
refresh). Implement the decrement+fire with the picture work.

### Banner overlap diagnosed: game-side text-only artifact

The banner garble ("Moves:Sc3" on row 1, score "0" on row 2) is the game's
own layout under missing picture metadata: `INIT-STATUS-LINE` /
`UPDATE-STATUS-LINE` (`globals.zil`) derive every banner `CURSET` from
`PICINF` results (HERE-LOC/REGION-LOC pics), and `picture_data` honestly
fails. The rendered cells match the issued coordinates exactly (e.g. the
moves counter prints at `set_cursor 2,9` → abs (1,8), right after the
"Sc" residue of a once-printed "Score:" label). Two spec cross-checks:

- Our `set_cursor` clamp of out-of-range coordinates (`0,1` → row 1)
  matches YZIP: *"If either of the arguments is outside the area covered by
  the window, it will be set to the appropriate dimension of the window."*
  (Frotz instead treats 0 as "keep current coordinate" — a Frotz-ism, not
  YZIP.)
- dfrotz's banner is equally garbled on the same story file.

No runtime fix; M3's picture metrics restore the intended banner.

### Smoke re-pin

`projects/zork-zero/smoke.txt` now pins the M2 layout: boot-screen
`--expect-at` args in `project.mk` (banner row 0, blank gap row, room title
at inset col 6, prompt at abs (6,49)) plus post-turn `.expect-at` script
directives (new in `Nova.NovaZ.Smoke`): banner intact after three turns,
echoed command + response inside the inset, "Time passes..." followed by
pinned blank cells (the stale-text regression trap — those cells held
longer inventory text one turn earlier), prompt at the window bottom. A
trailing `|` in a `.expect-at` script line protects trailing spaces from
the script reader's trim.
