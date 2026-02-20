# XMC Programmer Notes

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

## ROM Helper Labels (for asm)
These labels are in ROM and exported by the map file:
- `LAB_XM_SETADDR` (A=low, X=mid, Y=high)
- `LAB_XM_GETBYTE`
- `LAB_XM_PUTBYTE`
- `LAB_XM_STASH`
- `LAB_XM_FETCH`
- `LAB_XM_FILL`
- `LAB_XM_ALLOC`
- `LAB_XM_STATUS` (A=status, X=err)

All command helpers return `C=0` success, `C=1` error with `A = XMC_ERRCODE`.
