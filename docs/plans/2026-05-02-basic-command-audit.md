# NovaBASIC Command and Function Audit

Date: 2026-05-02

## Scope

This audit is based on:

- `ehbasic/tokens.json`: 128 primary token strings plus 98 extended token
  strings, for 226 tokenized names.
- `ehbasic/basic.asm`: primary dispatch table, extended token IDs, and handlers.
- `ehbasic/extension.s`: extension ROM command handlers.
- `tests/integration/*.6502`: current BASIC and assembly integration coverage.
- `docs/help/reference`: generated command/function reference pages.

The extended token table reserves 113 IDs, but only 98 are currently named.
Some IDs are intentionally unusable or reserved, such as `$20`, `$26`, and
`$33` through `$3F`. Do not compact token IDs when removing a feature.

## Decision Rules

Use these states for every command/function before removing anything:

| State | Meaning |
| --- | --- |
| Public | Stable user-facing BASIC. Must have docs and integration coverage. |
| Expert | Supported, but low-level. Keep docs, move out of beginner flows. |
| Library | Keep the implementation as an assembly/runtime API, but stop teaching it as first-class BASIC. |
| Host-only | Works in the Avalonia host or desktop tooling but is not supported on current ESP/FPGA hardware. |
| Deprecated | Keep the token as a tombstone or compatibility alias, document the replacement, and add a regression test for the deprecation behavior. |
| Remove later | Only after the token has been tombstoned and old programs cannot be mis-tokenized. |

Physical deletion is the last step. Primary token values are saturated from
`$80` to `$FF`; deleting or reordering them would change token values for other
keywords. Extended tokens also need stable IDs because tokenized programs store
the ID byte.

## Main Findings

NovaBASIC currently exposes too many implementation layers as peer-level
keywords:

- Core BASIC language features.
- User-facing file, graphics, sound, text, and sprite commands.
- Direct hardware access primitives.
- Assembly/runtime ABIs.
- Host maintenance commands.
- Desktop-only asset playback/loading commands.
- Editor/monitor launchers.

That makes the language feel larger than it is and makes the test surface hard
to reason about. The right cleanup is not to delete every sharp tool. It is to
declare which layer each token belongs to, then make docs and tests enforce that
boundary.

There are also real support mismatches:

- `WAVE` is already documented as deprecated in the sound guide and the source
  says to use `INSTRUMENT`.
- ESP FIO currently returns unsupported errors for `SIDPLAY`, `MIDPLAY`,
  `SFLOAD`, `TSAVE`, `TLOAD`, and `FORMAT`.
- BASIC-level NIC commands are documented, but there is no BASIC integration
  coverage. There is assembly NIC runtime coverage.
- Several newer or low-level tokens have no reference page, including
  `FIOCLR`, `PWD`, `NCC`, `SFLOAD`, `GTEXT`, most tile commands,
  directory metadata commands, and device-management commands.
- Some docs are compatibility notes or pseudo-commands rather than real tokens,
  such as `CALL`, `MUSIC PLAY`, and `COPPER ADD`.

## Keep as Public BASIC

These should remain part of the stable language surface.

| Area | Keep | Rationale |
| --- | --- | --- |
| Core language | `END`, `FOR`, `NEXT`, `DATA`, `INPUT`, `DIM`, `READ`, `LET`, `GOTO`, `RUN`, `IF`, `RESTORE`, `GOSUB`, `RETURN`, `REM`, `STOP`, `ON`, `DEF`, `PRINT`, `CONT`, `LIST`, `CLEAR`, `NEW`, `GET`, `TAB`, `SPC`, `THEN`, `ELSE`, `TO`, `FN`, `STEP`, `DO`, `LOOP`, `UNTIL`, `WHILE`, `OFF`, arithmetic/logical operators | These are the BASIC language. Keep compatibility even when individual keywords are not exciting. |
| Core functions | `SGN`, `INT`, `ABS`, `FRE`, `POS`, `SQR`, `RND`, `LOG`, `EXP`, `COS`, `SIN`, `TAN`, `ATN`, `LEN`, `STR$`, `VAL`, `ASC`, `CHR$`, `LEFT$`, `RIGHT$`, `MID$` | Standard BASIC surface. |
| Small conveniences | `INC`, `DEC`, `SWAP`, `MAX`, `MIN`, `PI`, `TWOPI`, `UCASE$`, `LCASE$`, `HEX$`, `BIN$` | Useful, easy to understand, and low conceptual cost. |
| Basic files | `SAVE`, `LOAD`, `DIR`, `DEL`, `FIOCLR` | Core workflows. `FIOCLR` is needed on real hardware to clear latched file I/O errors. |
| Practical filesystem | `PWD`, `CD`, `MKDIR`, `RMDIR` | Useful as SD-card shell commands. Keep only if they get docs and integration tests. |
| Text display | `CLS`, `COLOR`, `LOCATE`, `FONT`, `REVERSE`, `REVERSEOFF`, `FLASH`, `FLASHOFF` | User-facing display features with current integration coverage for most of the group. |
| Bitmap graphics | `MODE`, `GCLS`, `GCOLOR`, `PLOT`, `UNPLOT`, `LINE`, `CIRCLE`, `RECT`, `FILL`, `PAINT`, `GTEXT`, `VSYNC` | This is the approachable graphics API. Keep it public and tested. |
| Sprites | `SPRITE`, `SPRITESHAPE`, `SPRITEDATA`, `SPRITEX`, `SPRITEY`, `COLLISION`, `BUMPED` | Essential game-facing sprite API. |
| Simple sound | `SOUND`, `VOLUME`, `INSTRUMENT`, `MUSIC`, `PLAYING`, `MNOTE` | The hardware-local sound API. Keep. |
| File-backed sound | `SIDPLAY`, `SIDSTOP`, `MIDPLAY`, `MIDSTOP`, `SFLOAD` | BASIC is the convenience layer. Asset playback and soundfont loading should stay easy even though the low-level implementation lives in shared audio/FIO libraries. |
| Help/reset | `HELP`, `RESET` | `HELP` helps discoverability; `RESET` is a real machine operation. |

## Keep as Expert BASIC

These should stay supported, but they are not beginner commands. Docs should mark
them as expert/low-level and examples should show the safer high-level command
first.

| Area | Keep as expert | Rationale |
| --- | --- | --- |
| Direct memory/machine code | `POKE`, `PEEK`, `DOKE`, `DEEK`, `SYS`, `USR`, `SADD`, `VARPTR`, `WAIT`, `NULL`, `WIDTH` | Old-school BASIC compatibility and hardware bring-up tools. |
| Bit operations | `BITSET`, `BITCLR`, `BITTGL`, `BITTST` | Useful low-level primitives. Already partly tested. |
| VGC memory | `VPOKE`, `VPEEK` | Required for advanced graphics/text/tile work without leaking raw memory maps into every program. |
| DMA/blitter | `DMACOPY`, `DMAFILL`, `DMASTATUS`, `DMAERR`, `DMACOUNT`, `BLITCOPY`, `BLITFILL`, `BLITSTATUS`, `BLITERR`, `BLITCOUNT` | Real hardware capability. DMA is 1D; blitter is 2D. Keep, but classify as expert. |
| XRAM primitives | `XMEM`, `XBANK`, `XPOKE`, `XPEEK`, `STASH`, `FETCH`, `XFREE`, `XRESET`, `XALLOC`, `XDIR`, `XDEL`, `XMAP`, `XUNMAP`, `XLOAD`, `XSAVE` | These are distinct access modes: byte, bulk, mapped window, allocator/object store, and direct file streaming. Keep, but keep them in the expansion-memory/expert tier. |
| Copper | `COPPER` and its subcommands | Powerful display timing API. Keep expert. |
| Sprite raw fields | `SPRITESET` | Duplicates higher-level sprite commands, but it is currently the escape hatch for fields without friendly commands. Keep expert until typed sprite commands cover the same behavior. |

## Demote to Library or Hide from Public BASIC

These are the biggest contributors to surface-area sprawl. Keep implementation
where useful, but stop presenting them as everyday BASIC unless a demo or test
proves the user-facing need.

| Area | Demote | Rationale |
| --- | --- | --- |
| Interrupt BASIC | `IRQ`, `NMI`, `RETIRQ`, `RETNMI` | High complexity, no integration coverage found, and easy to misuse. Prefer assembly/runtime interrupt hooks. If kept public, add focused tests first. |
| Programmatic directory metadata | `DIROPEN`, `DIRNEXT`, `DIRNAM$`, `DIRSIZ`, `DIRTYP`, `META$` | `DIR` covers the common case. These are useful for file browsers, but they look like runtime/library APIs rather than core language commands. |
| Tile bulk helpers | `TROW`, `TCOL`, `TNTLOAD`, `TBUF`, `TBSET`, `TBFILL`, `TBPUT` | These duplicate loops over `TPUT`, `TFILL`, `TDEF`, and memory buffers. Keep only if tile demos need them enough to justify docs and tests. |
| NIC server API | `NLISTEN`, `NACCEPT` | Server mode is cool but not the minimum network surface. Keep in the assembly/runtime library until there is a BASIC server demo and hardware integration test. |
| NIC last length | `NLEN` | Duplicates `LEN(NRECV$(slot))` for ordinary BASIC string use. Keep only if binary/NUL-string receive semantics require it. |
| Host/device image management | `MOUNT`, `UNMOUNT` | Useful host/admin operations, but not an everyday BASIC language feature. Prefer serial/REST/admin tooling unless a BASIC workflow really needs them. |

## Deprecate or Ditch from the Supported Surface

These should leave the public BASIC surface first. Keep token tombstones until
compatibility risk is retired.

| Command | Recommendation | Reason |
| --- | --- | --- |
| `WAVE` | Deprecate now; remove later by tombstone. | Duplicate of `INSTRUMENT`; source and docs already say it is deprecated. |
| `NCC` | Ditch from BASIC surface; expose via monitor, boot menu, or host command. | This launches an editor. It is not a language command. |
| `FORMAT` | Ditch from BASIC surface; keep as host/admin only if needed. | Destructive, ESP currently reports unsupported, and there is no integration coverage. |
| `TSAVE`, `TLOAD` | Deprecated or host-only until ESP support exists. | ESP currently reports unsupported; no public reference pages. |

## Tile API Recommendation

Keep this as the public tile subset:

- `TILESIZE`
- `TMIRROR`
- `TTRANS`
- `TDEF`
- `TPUT`
- `TATTR`
- `TFILL`
- `TCLS`
- `TSCROLL`
- `TSCROLLX`
- `TSCROLLY`
- `TPAL`
- `TPALC`
- `TPEEK`
- `TPATTR`
- `TILECOL`

Demote or deprecate these unless a real BASIC tile editor/demo depends on them:

- `TROW`
- `TCOL`
- `TNTLOAD`
- `TBUF`
- `TBSET`
- `TBFILL`
- `TBPUT`
- `TSAVE`
- `TLOAD`

Reasoning: the public subset gives users tile setup, placement, attributes,
fill, scrolling, palette control, readback, and collision state. The demoted
subset is mostly bulk-loading or column-buffer machinery that can live in an
assembly library or a future tile-editor runtime.

## NIC API Recommendation

Keep the runtime/assembly NIC layer separate from BASIC. The minimal BASIC API
should be client-first:

- `NOPEN slot,"host",port`
- `NSEND slot,A$`
- `NRECV$(slot)`
- `NREADY(slot)`
- `NSTATUS(slot)`
- `NCLOSE slot`

Do not advertise `NLISTEN`, `NACCEPT`, or `NLEN` as public BASIC until there is
a tested BASIC workflow that needs them. This aligns with the current WiFi plan:
the ESP owns configuration and connectivity, while FPGA-side code exposes a
small communication library usable from BASIC and assembly.

## File I/O Recommendation

Keep all SD-touching commands on one status contract:

- Each command clears stale FIO status/error before issuing work.
- Each command sets status consistently on success, error, and unsupported
  operation.
- `FIOCLR` clears the latched error/status explicitly.

Public file commands should be:

- `SAVE`
- `LOAD`
- `DIR`
- `DEL`
- `GSAVE`
- `GLOAD`
- `XLOAD`
- `XSAVE`
- `FIOCLR`
- `PWD`
- `CD`
- `MKDIR`
- `RMDIR`

Demote programmatic directory enumeration and host/device image management until
they have clear user workflows and tests.

## Documentation Gaps

Reference pages should be generated or added for every Public and Expert token.
Current obvious gaps:

- `FIOCLR`
- `PWD`
- `CD`
- `MKDIR`
- `RMDIR`
- `FORMAT`
- `MOUNT`
- `UNMOUNT`
- `NCC`
- `HELP`
- `GTEXT`
- Tile commands/functions: `TILESIZE`, `TMIRROR`, `TTRANS`, `TDEF`, `TPUT`,
  `TATTR`, `TFILL`, `TROW`, `TCOL`, `TNTLOAD`, `TCLS`, `TSCROLL`, `TSCROLLX`,
  `TSCROLLY`, `TPAL`, `TPALC`, `TSAVE`, `TLOAD`, `TPEEK`, `TPATTR`,
  `TILECOL`, `TBUF`, `TBSET`, `TBFILL`, `TBPUT`
- Directory metadata: `DIROPEN`, `DIRNEXT`, `DIRNAM$`, `DIRSIZ`, `DIRTYP`,
  `META$`
- `WAVE` should have a deprecated reference page or a deliberate redirect to
  `INSTRUMENT`.

Also keep compatibility-note pages such as `CALL`, `MUSIC PLAY`, and
`COPPER ADD` clearly marked as non-token aliases/subcommands.

## Integration Coverage Gaps

Before calling a command Public, add coverage for:

- BASIC NIC client commands, and possibly server commands if they stay public.
- `IRQ`, `NMI`, `RETIRQ`, `RETNMI` if they stay public or expert.
- `INC`, `DEC`, `SWAP`, `DOKE`, `DEEK`, `BITTST`, `WIDTH`, `WAIT`, `NULL`.
- `CD`, `MKDIR`, `RMDIR`, `PWD`, and unsupported/deprecated `FORMAT`.
- `DIROPEN`, `DIRNEXT`, `DIRNAM$`, `DIRSIZ`, `DIRTYP`, `META$` if any remain
  public/expert.
- `TMIRROR`, `TNTLOAD`, `TPALC`, `TPEEK`, `TPATTR`, `TBUF`, `TBSET`,
  `TBFILL`, `TBPUT`, `TSAVE`, `TLOAD` if any remain public/expert.
- `SIDPLAY`, `MIDPLAY`, and `SFLOAD` should have token/listing coverage and
  hardware-status tests that prove unsupported hosts report the expected FIO
  status, not a hang or stale error.
- `TSAVE`, `TLOAD` only if hardware support is implemented. Otherwise add tests
  that they fail with the expected unsupported status.
- `WAVE` deprecation behavior.

## Tests to Add for the Audit Itself

Add a small command-surface contract so this does not rot:

1. A machine-readable status file, for example `ehbasic/command_surface.json`,
   keyed by token name with fields:
   - `state`: `public`, `expert`, `library`, `host_only`, `deprecated`,
     `reserved`
   - `category`
   - `replacement`
   - `integration_suite`
   - `doc`
2. A test that every token in `ehbasic/tokens.json` has an entry in that status
   file.
3. A test that every `public` or `expert` command/function has a reference page.
4. A test that every `deprecated` token has either a tombstone behavior test or
   an explicit compatibility alias test.
5. A test that reserved extended IDs remain reserved and are never silently
   reused without an explicit migration note.

## Recommended Next Moves

1. Create `ehbasic/command_surface.json` and classify every current token.
2. Mark these deprecated immediately: `WAVE`.
3. Mark these library/demoted: `IRQ`, `NMI`, `RETIRQ`, `RETNMI`,
   `DIROPEN`, `DIRNEXT`, `DIRNAM$`, `DIRSIZ`, `DIRTYP`, `META$`,
   `TROW`, `TCOL`, `TNTLOAD`, `TBUF`, `TBSET`, `TBFILL`, `TBPUT`,
   `NLISTEN`, `NACCEPT`, `NLEN`, `MOUNT`, `UNMOUNT`.
4. Add the command-surface contract test.
5. Fill integration gaps for anything still marked Public or Expert.
6. Only then remove or hide docs for deprecated/library commands.

## Primary ROM Cut Applied

Implemented on 2026-05-02 to free primary ROM immediately.

Primary CODE segment changed from `$3F8F` bytes to `$3DEA` bytes after
restoring the BASIC audio convenience commands, freeing `$1A5` bytes. The free
gap before the monitor segment increased from about `$48` bytes to about
`$1ED` bytes.

Removed from tokenization and routed old tokenized uses to syntax error:

- NIC server/redundant BASIC surface: `NLISTEN`, `NACCEPT`, `NLEN`
- Programmatic directory metadata: `DIROPEN`, `DIRNEXT`, `DIRNAM$`, `DIRSIZ`,
  `DIRTYP`, `META$`
- Editor/admin/device commands: `NCC`, `FORMAT`, `MOUNT`, `UNMOUNT`
- Tile bulk/file helpers: `TROW`, `TCOL`, `TNTLOAD`, `TSAVE`, `TLOAD`,
  `TBUF`, `TBSET`, `TBFILL`, `TBPUT`

`SIDPLAY`, `SIDSTOP`, `MIDPLAY`, `MIDSTOP`, and `SFLOAD` remain public BASIC
convenience wrappers because BASIC users should not have to write assembly for
common asset playback.

The removed token IDs remain reserved. The extended token list now exposes 76
named tokens instead of 98. Assembly/runtime helper files still default to
providing their full APIs unless the primary BASIC ROM build explicitly
disables the server shims.

## BASIC to Assembly Bridge

`SYS` now accepts optional byte register arguments:

```basic
SYS addr[,a[,x[,y]]]
```

Before jumping, NovaBASIC loads the 6502 `A`, `X`, and `Y` registers from those
arguments. When the machine-code routine returns with `RTS`, the returned
registers are stored in the SYS mailbox labels exported in `ehbasic/lib/nova.inc`:

- `SYS_REGA`
- `SYS_REGX`
- `SYS_REGY`

This is the small bridge for now. The next layer should be symbolic address
lookup, for example `ADDR("AUDIO.PLAY_SOUND")`, backed by a curated runtime
label table instead of making BASIC users know raw addresses.
