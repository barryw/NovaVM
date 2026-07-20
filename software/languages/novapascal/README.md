# NovaPascal shell

`novapascal.ndi` boots into `NovaPascal Shell v1.0`. Run `HELP` for the native
file, editor, compiler, assembler, linker, build, and run commands.

The shell and native Pascal compiler (NPC) remain resident in the
`$C000-$FFFF` language ROM. NPC streams Pascal source into the NDK document
XRAM slot, parses it there, and writes generated assembly to disk.

Commands load these ordinary Nova load-address-prefixed binaries from disk
into the shared `$4000-$6FFF` tool slot, one at a time:

- `NPEDIT.BIN` — text editor (`Alt-X` or `Ctrl-Q` returns to the shell)
- `NAS.BIN` — Nova assembler
- `NL.BIN` — Nova linker

Linked programs load and run at `$7000`, outside the tool slot.

NAS and NL are language-neutral toolchain programs. NAS emits NOBJ external
relocations for symbolic calls. NL resolves those names against ordinary NLIB
archives and copies each referenced member once; unused members are omitted.
The Pascal-specific code lives in `PASCAL.NLIB`, whose machine operands are
assembled from NDK definitions in `nova.inc`. Neither NAS nor NL knows Pascal
symbols or Nova hardware addresses.

## NPP 1 projects

An `.NPP` project is a small, source-controlled build manifest:

```text
NPP 1
MAIN HELLO.PAS
OUTPUT HELLO.BIN
```

`BUILD HELLO.NPP` visibly runs resident NPC, then loads NAS, then loads NL.
Each stage prints its banner, input, output, and completion status. It compiles
the main Pascal source to textual 65C02 assembly, assembles that into an NOBJ,
and links the requested binary with `PASCAL.NLIB`. The generated `.S`, `.OBJ`,
and `.BIN` files remain on disk.

`DIR` prints complete filenames, including extensions. `EDIT` accepts ASCII
Pascal source/projects, assembly source/includes, linker configs, and ordinary
text while labeling the active type in the status bar. `TYPE` uses the same
ASCII validation and refuses binary files.

NPP 1 intentionally accepts one `MAIN`. Multiple Pascal units and assembly
objects require the next increment of multi-object symbol-table support; the
project parser will not pretend concatenation is linking.
