# XRAM Shared Runtime

NovaVM exposes 512 KB of expansion RAM. BASIC presents it as eight 64 KB banks
selected with `XBANK`; assembly should treat it as one flat 24-bit address
space and use the shared runtime in `ehbasic/lib/xram.inc` and
`ehbasic/lib/xram.s`.

Do not duplicate the XMC, DMA, or FIO register sequences in every program. Link
or include the shared runtime and call the exported labels below.

## Addressing Model

| Caller | Address model |
| --- | --- |
| BASIC | `XBANK` selects the high byte; commands take a 16-bit offset. |
| Assembly | `XRAM_ADDRL/M/H` is a flat 24-bit address from `$000000` to `$07FFFF`. |

The conversion is:

```
flat_address = XBANK * 65536 + offset
```

For example, BASIC `XBANK 2: XPOKE $0100,$AA` targets flat assembly address
`$020100`.

## Building Against The Runtime

For a small standalone program, include the runtime directly:

```asm
.setcpu "65c02"
.include "lib/xram.inc"

; your code here

.include "lib/xram.s"
```

Assemble with the XRAM include directory on the ca65 path:

```sh
ca65 --cpu 65c02 -I ehbasic -o myprog.o myprog.s
ld65 -C myprog.cfg -o myprog.bin myprog.o
```

Larger projects can assemble `ehbasic/lib/xram.s` as a separate object and link it
with the rest of the program. The ABI is the same either way.

The named-block/XMC command processor lives in `ehbasic/lib/xmc.s`. BASIC links
that shared implementation for `XALLOC`, named `STASH`/`FETCH`, `XDIR`, and
`XDEL`. Assembly programs only need `xmc.s` if they want the same XMC register
command contract and allocator; flat-address byte, DMA, fill, and file
streaming routines only need `xram.s`.

## Pseudo-Register ABI

The runtime uses the Nova pseudo-register block in zero page:

| Symbol | Address | Meaning |
| --- | --- | --- |
| `XRAM_ADDRL` | `$20` | Flat XRAM address low byte |
| `XRAM_ADDRM` | `$21` | Flat XRAM address middle byte |
| `XRAM_ADDRH` | `$22` | Flat XRAM address high byte |
| `XRAM_DATA` | `$23` | Byte value for read/write/fill |
| `XRAM_RAML` | `$24` | CPU RAM pointer low byte |
| `XRAM_RAMH` | `$25` | CPU RAM pointer high byte |
| `XRAM_LENL` | `$26` | Transfer length low byte |
| `XRAM_LENH` | `$27` | Transfer length high byte |
| `XRAM_NAMELEN` | `$28` | Filename length for file streaming |
| `XRAM_NAMEPTR_L` | `$29` | Filename pointer low byte |
| `XRAM_NAMEPTR_H` | `$2A` | Filename pointer high byte |

Routines return `A=0` on success and `A=1` on error. They also update
`XMC_STATUS` and `XMC_ERRCODE`, so BASIC and assembly callers share the same
error path. `A`, `X`, and `Y` are scratch; preserve anything your caller needs.

## Routine Reference

| Label | Inputs | Behavior |
| --- | --- | --- |
| `xram_read8` | `XRAM_ADDRL/M/H` | Reads one byte into `XRAM_DATA`. |
| `xram_write8` | `XRAM_ADDRL/M/H`, `XRAM_DATA` | Writes one byte. |
| `xram_copy_from_ram` | `XRAM_RAML/H`, `XRAM_ADDRL/M/H`, `XRAM_LENL/H` | DMA copy from CPU RAM to XRAM. |
| `xram_copy_to_ram` | `XRAM_ADDRL/M/H`, `XRAM_RAML/H`, `XRAM_LENL/H` | DMA copy from XRAM to CPU RAM. |
| `xram_fill` | `XRAM_ADDRL/M/H`, `XRAM_LENL/H`, `XRAM_DATA` | DMA fill of an XRAM range. |
| `xram_xload` | file ABI below | Stream file contents directly into XRAM. |
| `xram_xsave` | file ABI below | Stream XRAM directly to a file. |
| `xram_wait_dma` | DMA registers already loaded | Wait for DMA and translate status. |

Optional XMC layer:

| Label | Inputs | Behavior |
| --- | --- | --- |
| `xmc_process` | `XMC_CMD` plus XMC argument registers | Executes the XMC command/register contract used by BASIC. |

Bulk operations use a 16-bit length. For transfers larger than 65535 bytes,
issue multiple calls and advance `XRAM_ADDRL/M/H` between chunks.

`xram_read8` and `xram_write8` use XMC window 3 (`$BF00-$BFFF`) internally and
leave that window mapped to the last accessed page. If your program also uses
window 3 directly, remap it after calling the byte helpers.

## File Streaming ABI

`xram_xload` and `xram_xsave` use the same flat XRAM address registers plus a
filename pointer:

| Symbol | Meaning |
| --- | --- |
| `XRAM_ADDRL/M/H` | XRAM destination for load, source for save |
| `XRAM_LENL/H` | Save length, or load limit; zero means full file for `xram_xload` |
| `XRAM_NAMELEN` | Filename length, 1 through 63 bytes |
| `XRAM_NAMEPTR_L/H` | CPU RAM pointer to filename bytes |

On success, `XRAM_LENL/H` is replaced with the actual transferred byte count.
Filenames without an extension use `.xram`; explicit extensions such as `.z3`
are honored.

```asm
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
        stz XRAM_LENL
        stz XRAM_LENH
        jsr xram_xload
        bne load_error
```

## Minimal Copy Example

This copies 16 bytes from CPU RAM `$2300` into flat XRAM address `$020100`,
clears the CPU destination, then copies the bytes back to `$2310`:

```asm
        lda #$00
        sta XRAM_ADDRL
        lda #$01
        sta XRAM_ADDRM
        lda #$02
        sta XRAM_ADDRH

        lda #<$2300
        sta XRAM_RAML
        lda #>$2300
        sta XRAM_RAMH
        lda #$10
        sta XRAM_LENL
        stz XRAM_LENH
        jsr xram_copy_from_ram
        bne error

        lda #<$2310
        sta XRAM_RAML
        lda #>$2310
        sta XRAM_RAMH
        jsr xram_copy_to_ram
        bne error
```

## Tests And Examples

- BASIC examples: `docs/programs/xram_smoke.bas`,
  `docs/programs/xram_window_demo.bas`, `docs/programs/xram_named_demo.bas`,
  and `docs/programs/xram_stream_demo.bas`.
- Assembly example: `docs/programs/xram_shared_runtime_demo.s`.
- Assembly integration suite: `tests/integration/xram_asm.6502`.
- Fixture source: `tests/integration/asm/xram_runtime.s`.
