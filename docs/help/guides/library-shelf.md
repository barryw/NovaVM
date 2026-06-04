# Paged Library Shelf (XRAM)

How shared library modules are staged in XRAM and paged into the bank-1 ROM window at
runtime. This is the boot-staging contract for the paged-library loader (`lib_call`).

See also: `docs/plans/2026-06-03-paged-library-loader-3b-design.md` (the ABI),
`docs/plans/2026-06-03-paged-library-loader-3b-impl.md` (the build), and
`runtime/asm/libabi.inc` (the constants).

## The shelf

Library modules are 16 KB binaries (ORG `$C000`, `"NL"` header). At boot, NovaHost streams
each module from SD into a **fixed XRAM band** — the *shelf* — and the 6502 loader pages the
active module from there into the bank-1 `ext_rom` BRAM on demand (`$BA76` page-in).

**Option A (current, no Phase-1 dependency):** the shelf is a fixed, 16 KB-slotted band in
the *unmanaged* high 256 KB of XRAM (the XMC allocator only manages the low 256 KB, so it can
never hand out or stomp these pages).

```
SHELF_BASE = $060000          ; libabi.inc: SHELF_BASE_H=$06, _M=$00, _L=$00
slot i     = SHELF_BASE + i * $4000        ; 16 KB per slot, SHELF_SLOT_WORDS = $2000 words

  slot 0   $060000 - $063FFF
  slot 1   $064000 - $067FFF
  slot 2   $068000 - $06BFFF
  slot 3   $06C000 - $06FFFF
```

This band (`$060000-$06FFFF`, 64 KB = 4 slots) is the only permanently-free XRAM region. It
clears every fixed region (NovaZ `$040000`, NovaZ cache `$050000`, NovaZ-save/EDITUI `$054000`,
NVG `$070000`, XMC metadata/dir `$07FA00+`). A 5th/6th slot requires reclaiming the `$054000`
transient band — a later decision.

> The page-in HW smoke (`tools/run-page-in-hardware-smoke.py`) historically staged a test
> pattern at `$060000` (slot 0). It and the shelf are never live simultaneously, but retarget
> the smoke if needed.

## Module-id → slot map (the single source of truth)

The map lives in two places that **MUST agree**:
1. `modtab_lookup` in `runtime/asm/libcall.s` (compile-time, what the 6502 loader uses).
2. NovaHost's boot manifest (firmware, where each `.mod` file is staged in XRAM).

| id   | module   | slot | base     | phase |
|------|----------|------|----------|-------|
| `$7F` | TEST     | 0    | `$060000` | 3b proof |
| `$01` | GRAPHICS | 0    | `$060000` | Phase 4 (reclaims slot 0 from TEST) |
| `$02` | SOUND    | 1    | `$064000` | Phase 5 |
| `$03` | SYSTEM   | 2    | `$068000` | Phase 5 |

Keep these in sync, or a page-in loads the wrong bytes — caught at runtime by the loader's
`"NL"` magic / module-id header check (`LERR_BAD_MAGIC` / `LERR_BAD_MODULE`), not silently.

## Migration to System B (Phase 1)

When the 512 KB XRAM directory allocator lands, the shelf becomes a normal `XALLOC` and the
loader fills `modtab` by directory lookup (by module name) at boot; the fixed slots become
visible reserved-at-address directory entries. `lib_call` does not change — `modtab` is the
abstraction that hides constants-vs-directory.
