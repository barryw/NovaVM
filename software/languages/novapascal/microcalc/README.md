# MicroCalc acceptance corpus

This compatibility project is based on MicroCalc from the Borland Turbo Pascal
3.01A CP/M distribution archived at:

`https://oldcomputers.dyndns.org/public/pub/archiv/maslin/masl-dsk/images-97/cpmprog/tpascal3.zip`

Archive SHA-256:

`f58bfb34999cc9dd2093c0df2039a682c51d5b495aac0e7d7a7514f523bb96dc`

The CP/M 128-byte record layout and `$1A` logical EOF are preserved, including
stale padding after logical EOF. Nova compatibility edits remain visible in the
Pascal sources: the worksheet array is represented by one managed `XRamBlock`,
and small `LoadCell`/`StoreCell` helpers transfer each 78-byte record through
`NovaMemory`. The compiler must still stream and compile the historical record
format without normalizing it.

`MICROCALC.NPP` is the only Nova-specific file. It names the original `MC.PAS`
as its main source; the six `MC-MOD00.INC` through `MC-MOD05.INC` files remain
selected by the original `{$I ...}` directives.

The end-to-end acceptance test builds the project through NPC, NPO2, NAS, and
NL; exercises navigation, entry, formulas, help, save/load, clear, formatting,
and repeated builds; and verifies that `Q`/`Halt` releases XRAM and returns to
the NovaPascal shell. The current O2 executable is 25,171 bytes on disk. Its
`NBS1` trailer represents 1,256 zero-filled static bytes, while the 11,466-byte
worksheet exists only in dynamically allocated XRAM while MicroCalc runs. Its
six sequential source modules reuse one include slot, keeping NPC's source-page
peak at 32 KiB instead of retaining seven separate buffers.

The copyright and use notice embedded in `MC.PAS` applies to the original
MicroCalc material. No Turbo Pascal compiler binaries are included here.
