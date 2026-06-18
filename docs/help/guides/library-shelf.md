# Paged Library Shelf (XRAM)

How shared library modules are staged in XRAM and paged into the bank-1 ROM window at
runtime. This is the boot-staging contract for the paged-library loader (`lib_call`).

See also: `docs/plans/2026-06-03-paged-library-loader-3b-design.md` (the ABI),
`docs/plans/2026-06-03-paged-library-loader-3b-impl.md` (the build), and
`runtime/asm/libabi.inc` (the constants).

## The Shelf

Library modules are 16 KB binaries (ORG `$C000`, `"NL"` header). At boot, NovaHost streams
each module from SD into a **fixed XRAM band** — the *shelf* — and the 6502 loader pages the
active module from there into the bank-1 `ext_rom` BRAM on demand (`$BA76` page-in).

The shelf is platform-owned. Applications and runtimes must not use it as
scratch memory. If code needs XRAM, request it from the MEMORY module or XMC
allocator; do not pick a free-looking `$06xxxx` address.

```
SHELF_BASE = $060000          ; libabi.inc: SHELF_BASE_H=$06, _M=$00, _L=$00
slot i     = SHELF_BASE + i * $4000        ; 16 KB per slot, SHELF_SLOT_WORDS = $2000 words

  slot 0   $060000 - $063FFF
  slot 1   $064000 - $067FFF
  slot 2   $068000 - $06BFFF
  slot 3   $06C000 - $06FFFF
```

This band (`$060000-$06FFFF`, 64 KB = 4 slots) is not app workspace. It is safe
from the XMC allocator because the allocator owns the low 256 KB heap only, but
that does not make it available to runtimes.

## Directory and LRU

The runtime-owned directory lives in CPU RAM at `$0418-$041F`:

```
SHELF_TAG[0..3] = module id in each slot, or 0 for empty
SHELF_LRU[0..3] = slot indices, most-recently-used first
```

At boot, NovaHost preloads as many modules from `/config/boot.json` as fit in
the four slots, then writes this directory. On a `lib_call`, the resident loader
checks `SHELF_TAG`. A hit pages that slot into bank 1 and moves the slot to the
front of `SHELF_LRU`. A miss asks the host to stream the requested module into
an empty slot or the least-recently-used slot, then updates the directory.

The host does not update `SHELF_TAG` or `SHELF_LRU` during a miss. The 6502
loader owns those bytes.

## Failure Checks

After paging, the loader validates the module header. Wrong bytes in a shelf
slot produce `LERR_BAD_MAGIC`; the wrong module id produces `LERR_BAD_MODULE`.
Those errors usually mean a runtime wrote into `$060000-$06FFFF` or the SD
module image is stale.
