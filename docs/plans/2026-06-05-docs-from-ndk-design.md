# docs-from-NDK — module docs generated from the NDK (Part A of NDK-sync)

**Date:** 2026-06-05
**Status:** Design approved, implementation pending
**Driver:** user — *"our module documentation should probably come from the ndk as
well.. no need to document in 2 places"*. See `feedback-ndk-source-of-truth`,
`feedback-rom-domain-placement`, `project-logo-rom-consolidation`.

## Problem

There are two annotation systems documenting the same functions at two altitudes:

- **NDK** (`runtime/asm/*.s`, `*.inc`): `; @label/@kind/@symbol/@abi/@summary/@in/@out/
  @requires`, parsed by `gen_runtime_abi_docs.parse_tagged_sources`, keyed by
  `@symbol`. Describes a routine at the **register / pseudo-register** altitude —
  e.g. `audio_tone`: `@in NVR0L/NVR0H: SID frequency word (lo/hi). A: duration`.
- **Module** (`modules/*/*.s`): `;@fn/@brief/@arg/@ret/@effect/@status`, parsed by
  `nmod_pack.parse_annotations` → the `.nmod` NDOC trailer. Describes a fn at the
  **mailbox** altitude — e.g. `SND_TONE`: `@arg freq u16 SID frequency word (ARG0)`.

The fn-id → NDK-routine mapping today is only an **informal comment** in
`libgraphics.inc` (`GFN_PLOT = $02 ; … -> vgc_plot`). Not machine-readable.

What is genuinely duplicated is the **one-line human summary** (module `;@brief`
restates the NDK `@summary`). The **per-arg bindings are NOT duplicated** — they are
the wrapper's own mailbox→register marshalling (`ARG0` → `NVR0L`), which exists
nowhere else. The NDK `@in` is register-altitude and the wrong view for a `lib_call`
consumer.

## Decision

**Single-source the summary only.** (User chose "Summary only" over a full arg
merge or moving mailbox docs into the NDK.)

1. **Mapping co-located in the module `.s`.** Each `;@fn` gains one optional
   `;@ndk <symbol>` line naming the NDK routine it wraps. The duplicated `;@brief`
   is deleted when `;@ndk` is present (summary pulled from the NDK). Fns with no
   single NDK routine — fan-out (`GFN_COLOR` → `vgc_set_fg/bg/border`), stubs
   (`gfn_unimpl`), non-routine wrappers (`SYS_FN_EDIT`, `SYS_FN_TIMER`) — keep a
   local `;@brief` and omit `;@ndk`.

2. **`nmod_pack.py`** gains `--ndk-dir runtime/asm`. It imports
   `parse_tagged_sources` from `gen_runtime_abi_docs`, indexes the NDK by `@symbol`,
   parses the new `ndk` tag, and in `build_doc`:
   - if a fn has `ndk` and no local `brief`, `brief = ndk_entry["summary"]`;
   - embeds `"ndk": "<symbol>"` in the NDOC entry (provenance).
   **Build-time drift guards (raise → build fails):** the `ndk` symbol must exist
   in the NDK index, AND the wrapper body (from `gfn_x:`/`snd_x:`/`sys_x:` to the
   next label) must contain `JSR <symbol>` (helper JSRs `copy_args_to_p`,
   `vgc_wait_cmd` ignored). A lying or stale mapping cannot produce a `.nmod`.

3. **CI-enforced drift test (`ModuleNdkContractTests.cs`).** The Python guard fails
   the module build; committed `.bin`/`.s` could still drift without a rebuild, so
   mirror the checks in C# (`dotnet test` / CI vs committed sources). New test:
   each `(;@fn, ;@ndk)` pair — symbol exists in NDK sources, and the wrapper body
   contains `JSR <symbol>`. Existing fn-count and no-direct-poke tests stay.

## Why the summary lives in the NDK, not the module

One implementation, one description, reachable by every runtime and module — no
drift. The mailbox arg view stays in the module because it is the wrapper's own
contract, not a copy of anything. The `;@ndk` tag plus the JSR drift guard closes
the last hand-maintained gap: the mapping is now machine-verified to match the code.

## Staging

- **Commit 1 — machinery + small modules.** `nmod_pack.py` + `tools/test_nmod_pack.py`
  cases; migrate **sound (3)** + **system (4)** fns; Makefile `--ndk-dir`; C# drift
  test; rebuild `.nmod`/`.bin`; full suite green.
- **Commit 2 — graphics (132).** Semi-automated: a throwaway script extracts each
  wrapper's canonical NDK JSR; review the diff; drift test + full suite gate it.

TDD throughout — Python `nmod_pack` tests and the C# drift test written red first,
then the source edits to green.

## Reused components

- `gen_runtime_abi_docs.parse_tagged_sources` — NDK annotation parser (keyed by
  `@symbol`; entries carry `summary`, `inputs`, `outputs`, `abi`, `requires`).
- `nmod_pack.parse_annotations` — module `;@` parser (extended with `ndk`).
- C# `CountJtableEntries` / `ModuleOwnCode` helpers in `ModuleNdkContractTests`.

## Out of scope

- Merging the mailbox arg docs into the NDK (rejected — breaks layering).
- Reconstructing arg descriptions from freeform NDK `@in` text (rejected — brittle).
- Phase C turtle repatriation — the next item, after this lands.
