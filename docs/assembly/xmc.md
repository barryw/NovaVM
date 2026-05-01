# XMC / XRAM Programmer Notes

For the reusable assembly-facing runtime ABI, see `docs/assembly/xram.md`.

## Registers
- Base: `$BA00`
- Key registers: `XMC_CMD`, `XMC_STATUS`, `XMC_ERRCODE`, `XMC_XAL/XAM/XAH`, `XMC_RAML/RAMH`, `XMC_LENL/LENH`, `XMC_DATA`, `XMC_BANK`
- Named block registers: `XMC_NAMELEN`, `XMC_NAME` (`$BA24-$BA3F`)
- Window mapping: `XMC_WINCTL`, `XMC_W0AL..XMC_W3AH`

## Command IDs
- `01 GET`, `02 PUT`, `03 STASH`, `04 FETCH`, `05 FILL`
- `07 STATS`, `08 RESET_USAGE`, `09 RELEASE_RANGE`
- `0A ALLOC`, `0B NAMED_STASH`, `0C NAMED_FETCH`, `0D NAMED_DELETE`
- `0E NAMED_DIROPEN`, `0F NAMED_DIRREAD`

## BASIC Commands
- Raw: `XMEM`, `XBANK n`, `XPOKE off,val`, `XPEEK(off)`, `STASH ram,off,len`, `FETCH ram,off,len`, `XFREE off,len`, `XRESET`
- Allocator/named: `XALLOC len`, `STASH "name",ram,len`, `FETCH "name",ram`, `XDIR`, `XDEL "name"`
- Window mapping: `XMAP win,off`, `XUNMAP win`

## Shared XRAM Runtime

Assembly code should use `ehbasic/lib/xram.inc` and `ehbasic/lib/xram.s` instead of
duplicating XMC/DMA register setup. BASIC still exposes banked 64K offsets with
`XBANK`, but the shared runtime takes a flat 24-bit address in
`XRAM_ADDRL/M/H`.

The assembly ABI is backed by the Nova pseudo-register block:

- `XRAM_ADDRL/M/H` = `NVR0L/NVR0H/NVR1L`
- `XRAM_DATA` = `NVR1H`
- `XRAM_RAML/H` = `NVR2L/H`
- `XRAM_LENL/H` = `NVR3L/H`

All routines return `A=0` on success and `A=1` on error. They also update
`XMC_STATUS` and `XMC_ERRCODE`, so BASIC and assembly can share the same status
checks. BASIC enters the same runtime through `xram_xmc_*` wrappers that copy
the XMC MMIO registers into the pseudo-register ABI.

| Label | Purpose |
| --- | --- |
| `xram_read8` | Read `XRAM_ADDRL/M/H` into `XRAM_DATA` through XMC window 3. |
| `xram_write8` | Write `XRAM_DATA` to `XRAM_ADDRL/M/H` through XMC window 3. |
| `xram_copy_from_ram` | DMA copy from `XRAM_RAML/H` in CPU RAM to `XRAM_ADDRL/M/H`. |
| `xram_copy_to_ram` | DMA copy from `XRAM_ADDRL/M/H` to `XRAM_RAML/H` in CPU RAM. |
| `xram_fill` | DMA fill XRAM at `XRAM_ADDRL/M/H` with `XRAM_DATA`. |
| `xram_xload` | Stream a file directly into `XRAM_ADDRL/M/H`; `XRAM_LENL/H=0` loads the full file. |
| `xram_xsave` | Stream `XRAM_LENL/H` bytes from `XRAM_ADDRL/M/H` directly to a file. |
| `xram_wait_dma` | Wait for the DMA controller and translate DMA errors to XMC errors. |

Bulk routines use `XRAM_LENL/H` for a 16-bit length. Valid flat XRAM addresses
are `$000000` through `$07FFFF`; byte helpers reject addresses at or above
`$080000`, and DMA-backed helpers rely on the DMA range check for transfers.

## Direct File Streaming

Use `xram_xload` and `xram_xsave` instead of setting FIO registers by hand.
Assembly code supplies the shared XRAM pseudo-registers plus a pointer to the
filename:

- `XRAM_ADDRL/M/H` = flat 24-bit XRAM source/destination
- `XRAM_LENL/H` = transfer length (`0` means full file for `xram_xload`)
- `XRAM_NAMELEN` = filename length, 1--63 bytes
- `XRAM_NAMEPTR_L/H` = pointer to filename bytes in CPU RAM

On success, the routines update `XRAM_LENL/H` with the actual byte count from
FIO and return `A=0`. On error, they return `A=1` and update both FIO and XMC
status/error registers.

```
story_name:
  .byte "ZORK1.Z3"
story_name_end:

  lda #<story_name
  sta XRAM_NAMEPTR_L
  lda #>story_name
  sta XRAM_NAMEPTR_H
  lda #(story_name_end - story_name)
  sta XRAM_NAMELEN

  stz XRAM_ADDRL
  stz XRAM_ADDRM
  stz XRAM_ADDRH
  stz XRAM_LENL      ; length 0 = full file for xram_xload
  stz XRAM_LENH
  jsr xram_xload
  bne load_error
```

Filenames without an extension use `.xram`; explicit extensions such as `.z3`
are honored. BASIC `XLOAD`/`XSAVE` also routes through these shared routines.
