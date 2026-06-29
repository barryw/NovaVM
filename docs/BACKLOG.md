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

## Arty / FPGA

- **Watchdog (SWDT) races the PL-reload boot → cold-boot loop.** The board has a 120 s
  `cdns-wdt` hardware watchdog (`f8005000.watchdog`). Each reboot reloads the PL and the boot
  takes ~2–2.5 min; a slow boot — especially with a dirty/`rw` rootfs that needs an fsck —
  can exceed 120 s and reset *mid-boot*, before `novavm` starts kicking the watchdog, which
  loops. Hit during 2026-06 reboot-heavy hardware testing; recovered with a clean power-cycle
  (fsck + faster boot) + restoring a known-good `fd0`.

  **Decided: kill the watchdog — it's pointless on a hobby 8-bit box, never needed here.**
  Remove it at the source so it's never armed. Three coordinated changes in the committed
  `meta-user` layer — they MUST land together, since disabling the DT node while the FSBL still
  arms it would brick boot:
  1. **FSBL: don't arm it.** Add `meta-user/recipes-bsp/fsbl/fsbl_%.bbappend` that excludes the
     watchdog (`-DFSBL_WDT_EXCLUDE_VAL`; confirm the exact compiler-flags var for the
     `fsbl-firmware` recipe in PetaLinux 2024.2).
  2. **Device-tree:** in `system-user.dtsi` change `&watchdog0 { timeout-sec = <120>; }` to
     `&watchdog0 { status = "disabled"; }` so the kernel never manages it.
  3. **Rootfs:** drop `watchdogd` so nothing re-arms `/dev/watchdog` after boot.
  Build + test during a full `.wic` rebuild — PetaLinux was NOT configured on the dev box where
  this was diagnosed (just an unpacked 2024.2 tree, no `settings.sh`), so it couldn't be
  verified there; do not commit it unbuilt. Once gone, reboot-heavy hardware testing stops
  being fragile.
