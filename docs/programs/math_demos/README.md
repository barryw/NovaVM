# Nova math coprocessor sprite demos

Each public NovaBASIC math function has two standalone demos:

- `math_<name>_basic.bas` exercises the BASIC function.
- `math_<name>_asm.s` exercises the matching assembly/register path.

Every demo writes the tested function name on screen and moves green hardware
sprites using that function's result. Press `Q` in the BASIC demos to exit.

Public BASIC math functions covered here:

- `MSIN(angle)`
- `MCOS(angle)`
- `MMULFX(a,b)`
- `MDIST(dx,dy)`
- `MRND`
- `MDIV(n,d)`
- `MREM(n,d)`
- `MATAN2(dy,dx)`
- `MDOTFX(ax,ay,bx,by)`
- `MLEN2(x,y)`
- `MSCALX(x,y,s)`
- `MSCALY(x,y,s)`
- `MMUL16L(a,b)`
- `MMUL16H(a,b)`
- `MDOTS16L(ax,ay,bx,by)`
- `MDOTS16H(ax,ay,bx,by)`
- `MCROSSL(ax,ay,bx,by)`
- `MCROSSH(ax,ay,bx,by)`

Example assembly build:

```sh
ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -I ehbasic -o /tmp/math_msin.o docs/programs/math_demos/math_msin_asm.s
ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_msin.bin /tmp/math_msin.o
```
