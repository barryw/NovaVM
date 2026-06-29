# NovaVM Backlog

Things we'd like to include, captured so they don't get lost. Not in priority order.

## NovaLogo

- **Property lists** (`PPROP` / `GPROP` / `REMPROP` / `PLIST`). Classic Logo associative
  store — per-symbol key→value, from Logo's Lisp heritage and used throughout the canonical
  curriculum (Harvey, *Computer Science Logo Style*) for structured records, graphs, and
  small databases. Genuinely useful for non-trivial programs and moderately expected by
  knowledgeable Logo users. Self-contained: per-symbol plist storage + the four builtins +
  a `PPROP "name "key value` serialization pass in `SAVE` (LOAD re-runs it for free, like
  the `MAKE` lines). **ROM-gated** — the 16 KB foundation ROM is essentially full (~9 bytes
  to the monitor boundary after workspace-save), so this needs a deliberate space-reclaim
  pass first. Do it as its own focused feature.

- **Restore unlimited `PRINT`/`SHOW` of long lists.** Fitting workspace-variable
  serialization into the full ROM forced `print_list` (lists.s) to render via the 126-byte
  `input_buf` (`render_list_to_buf`), so `PRINT`/`SHOW` of a list whose printed text exceeds
  ~126 chars now silently truncates — a regression in a core command. Restore the streaming
  (unbounded) emit path. Needs ~200 B reclaimed; competes with property lists for the 16 KB.

- **Hardware-verify Logo `SAVE`/`LOAD` *procedure* round-trip.** Verified on real hardware:
  `CATALOG`/`DIR` (directory listing), the `SAVE`/`LOAD` round-trip for **variables**
  (number/word/list all restore correctly after overwrite), and the Logo file type
  (`CATALOG` shows the saved workspace as type `LOGO` — the `0x07` type works end-to-end).
  Still pending: the **procedure** round-trip, which needs the full-screen editor to define
  a proc; debug key-injection couldn't trigger the editor's `Ctrl-K S` save (the `^K` prefix
  is recognized, the completion key isn't landing). Verify with a real keyboard, or crack
  the key-injection. SAVE reuses the editor's own serializer, so high confidence it works.
