# NovaPascal shell

`novapascal.ndi` boots into `NovaPascal Shell v1.0`. Run `HELP` for the native
file, editor, compiler, assembler, linker, build, and run commands.

The shell and native Pascal compiler (NPC) remain resident in the
`$C000-$FFFF` language ROM. NPC streams Pascal source into the NDK document
XRAM slot, parses it there, and writes generated assembly to disk.

Commands load these ordinary Nova load-address-prefixed binaries from disk
into the shared `$2000-$6FFF` tool slot, one at a time:

- `NPEDIT.BIN` — text editor (`Alt-X` or `Ctrl-Q` returns to the shell)
- `NAS.BIN` — Nova assembler
- `NL.BIN` — Nova linker

NAS loads `NASPP.OVL` at `$7000` for preprocessing. The resident frontend
passes input and output XRAM allocations through a stable mailbox, invokes the
standard validated NOVO loader, then assembles the returned stream. The first
overlay implementation expands case-insensitive `.MACRO name p1, p2` through
`.ENDMACRO` definitions and nested `.IF`, `.IFDEF`, `.IFNDEF`, `.ELSEIF`,
`.ELSE`, and `.ENDIF` blocks. `.DEFINE name value` performs case-insensitive identifier
substitution outside strings and comments; `.UNDEFINE name` removes it. `.IF`
evaluates 16-bit constant expressions after one level of definition expansion.
After preprocessing, NAS replaces that overlay with `NASBE.OVL`. It owns the
complete language-neutral two-pass assembly core and W65C02 opcode/addressing
tables; the small resident executable supplies XRAM/file I/O callbacks through
a fixed mailbox. Parsing, symbols, sections, relocations, expressions, and the
opcode matrix therefore consume the overlay slot rather than lower RAM.

Linked programs default to `$7000`, outside the tool slot. `RUN` reads the
binary's load-address prefix, validates that the payload fits writable
application RAM below Nova MMIO, and invokes that address.
NAS allocates its source text in XRAM for each invocation and releases it on
exit; lower RAM is reserved for the object, symbol, and relocation working set.

NAS and NL are language-neutral toolchain programs. NAS emits NOBJ external
relocations for symbolic calls. NLIB v2 stores complete NOBJ members; NL resolves
their exports, imports, sections, and relocations through the same object path as
explicit inputs. It extracts dependencies to a fixed point before placement,
but only imports reached from live relocation edges can pull archive members;
dead sections may contain unresolved imports without affecting the link.
Unreferenced members are omitted. NL applies range-checked `ABS8`,
`ABS16`, `LO8`, `HI8`, and `REL8` relocations.
NL takes its 16-bit placement address from the shared `NPTOOL_LINK_BASE`
mailbox cell; the shell initializes it to `$7000` for each direct link or
build, while projects can override it without changing NL.
`LINK MAIN.OBJ UTIL.OBJ` places both objects in order and resolves exported
globals across them before searching `PASCAL.NLIB`; the output name is derived
from the first object. Duplicate case-insensitive globals in the final selected
object set are rejected even if no live relocation references them. The current
core ABI reserves four object slots, while the shell command exposes two until
project/config input lists land.
The Pascal-specific code lives in `PASCAL.NLIB`, whose machine operands are
assembled from NDK definitions in `nova.inc`. `P_WRITE_CHAR` currently pulls a
separate `P_CHAR_DEVICE` hardware shim through an ordinary NOBJ import, proving
that archive members may depend on other members. Neither NAS nor NL knows
Pascal symbols or Nova hardware addresses.

Their executable frontends, mailbox ABI, project/config readers, preprocessing
and backend overlays, linker worker, and linker configurations live under
`software/toolchain`. NovaPascal retains only NPC, NPEDIT, its runtime library,
and shell orchestration.

Build the language-neutral tools independently with
`make -C software/toolchain all`. The NovaPascal build consumes the resulting
`NAS.BIN`, `NL.BIN`, and overlay files directly from `software/toolchain/build`.

NAS is case-insensitive for mnemonics, directives, and symbols. Its current
NOBJ v2 path supports local/forward labels, relative branches, implied and
immediate W65C02 instructions, accumulator operations, zero-page/absolute
direct and X/Y-indexed operands, all parenthesized indirect modes, symbolic
byte/word relocations, every relative and bit-branch form, and `.BYTE`/`.WORD`.
`.BYTE` lists may mix expressions with case-preserving ASCII strings; quoted
text supports `\\n`, `\\r`, `\\t`, `\\0`, escaped quotes, and backslashes.
The complete documented W65C02 instruction/addressing matrix is covered;
the shared 16-bit expression engine supports decimal, hexadecimal, binary, and
character literals; grouping; unary `+`, `-`, `~`, `!`, `<`, and `>`;
arithmetic, shifts, bitwise logic, comparisons, boolean operators, and all
ca65 spelled aliases; and relocatable `symbol +/- constant` addends.
Ca65-style cheap local labels use `@name` and are scoped to the preceding
non-local label, so names such as `@loop` may be reused case-insensitively.
Anonymous `:` labels support `:+`, `:-`, and repeated signs such as `:++`;
all references use the ordinary range-checked symbol and relocation paths.
Case-insensitive `.ASSERT expression` emits no bytes when its absolute value is
nonzero and stops assembly with a source-located `assertion failed` diagnostic
when it is zero. Relocatable assertions are rejected as unresolved symbols.
Case-insensitive `NAME = expression` assignments create NOBJ absolute symbols;
they fold in operands, resolve forward dependency chains to a fixed point, and
may be exported/imported across objects without load-address rebasing. Cycles
fail as symbol errors; forward constants cannot yet size `.RES`/`.ALIGN`.
`.BYTE symbol`, `.BYTE <symbol`, `.BYTE >symbol`, `.WORD symbol`, immediate
low/high-byte operands, and cross-section/imported branches emit range-checked
`ABS8`, `LO8`, `HI8`, `ABS16`, and `REL8` NOBJ records for NL.
`.IMPORT` and `.EXPORT` declare case-insensitive global module boundaries;
undeclared externals and unresolved exports are rejected. `.RES count[,fill]`
and `.ALIGN boundary[,fill]` provide expression-driven storage and section
alignment. Case-insensitive `.SEGMENT "name"` selections emit independent
initialized sections; `BSS` is zerofill storage. NL places every section with
its requested alignment and resolves section-relative symbols and relocations.
Before placement, `NLWORK.OVL` marks the entry sections and explicitly exported
sections of direct objects, then follows local and global relocation edges to a
fixed point. NL places and relocates only those live sections; stripped sections
need no linker-config rule and do not appear in maps.
`.INCLUDE "file"` supports eight nested XRAM-backed sources. NAS errors are
reported against the active source as `file:line:column: error: message`.
Case-insensitive `.INCBIN "file"` streams arbitrary bytes through the same
frontend-owned XRAM callback into the current initialized section.
Primary and preprocessed sources may be 32 KB; include buffers start at 4 KB
and retry up to 52 KB, large enough for the canonical `nova.inc`. Local
absolute constants stay in an XRAM hash table and are not serialized into
NOBJ, so assembly programs can include `nova.inc`, an NDK declaration such as
`dma.inc`, and its canonical implementation source directly without a flattened
header or converted library artifact. NAS accepts the ca65 `.SETCPU`, `.GLOBAL`,
`.GLOBALZP`, `.IF`, `.IFDEF`, `.IFNDEF`, `.IFREF`, `.ELSE`, and `.ENDIF` forms
used by canonical NDK sources. `.GLOBALZP` selects zero-page instruction forms
and emits NL's range-checked `ABS8` relocation for unresolved symbols.
`.referenced(symbol)` and `.REFTO symbol` implement the NDK's selective source
emission directly: ordinary operand references seed the live set, `.REFTO`
propagates dependencies, and inactive routines never enter the NOBJ. The
end-to-end test assembles canonical `rng.s` and `fio.s`, proves an unrelated FIO
routine is absent, links with NL unchanged, and runs the resulting executable.
Direct `ASSEMBLE file.s [-Dname=value] [-o file.obj]` seeds one case-insensitive
preprocessor definition and optionally selects the object filename. Options may
appear in either order.
The current preprocessor accepts eight combined macro/definition names, four
parameters per macro, and eight conditional levels. `.IF` accepts decimal,
hexadecimal, binary, and character literals; grouping; unary `+`, `-`, `~`,
`!`, `<`, and `>`; arithmetic, shifts, bitwise operators, comparisons, and
`&&`/`||`, with case-insensitive `.DEFINE` operands and ca65's spelled operator
aliases. `.ELSEIF` accepts the same expressions and selects only the first true
branch. Undefined operands and division by zero fail as syntax errors.
Recursive macro/definition expansion remains under development.

## Creating a project

`NEW name` creates a complete, buildable Pascal project without overwriting
existing files:

- `name.PAS` — a single program that prints `Hello, world!`
- `name.NPP` — the project/build manifest, including `OPTIMIZE O2` and the
  `NOVA=1` NAS definition plus NL `MEMORY` and `SEGMENTS` configuration

The `.NPP` manifest is also the NAS project configuration; a separate inert
assembler- or linker-config file is not required. `BUILD name.NPP` validates
the embedded NL configuration before creating intermediates, then compiles, assembles,
and links the generated project with `PASCAL.NLIB`, which supplies the Pascal
standard runtime and its NDK-backed Nova hardware calls. NPC currently has one
supported optimization level, O2, and uses it when `OPTIMIZE` is omitted as
well as when the generated project states it explicitly.
NAS receives the project filename and owns extraction and validation of its
case-insensitive `DEFINE`; the shell only routes the file to the tool. Direct
`ASSEMBLE -Dname=value` uses the same NAS preprocessor path.

## NPP 1 projects

An `.NPP` project is a small, source-controlled build manifest:

```text
NPP 1
MAIN HELLO.PAS
OUTPUT HELLO.BIN
OPTIMIZE O2
DEFINE feature=$01
OBJECT SUPPORT.OBJ
CONFIG INLINE
MAP HELLO.MAP
LABEL HELLO.LBL
MEMORY {
    RAM: start = $8000, size = $0100, file = %O;
}
SEGMENTS {
    CODE: load = RAM, type = ro;
}
```

`OBJECT` is optional and links one ordinary NAS-produced NOBJ after the main
Pascal object through the same NL path used by direct `LINK` commands. `ASM`
may be used instead of `OBJECT`; BUILD runs NAS on that source and links its
derived `.OBJ`. The two forms are mutually exclusive in NPP 1. Optional
`DEFINE name=value` passes the same NAS preprocessor definition to project
assembly steps.
`OPTIMIZE O2` is optional, may appear after `OUTPUT`, and explicitly selects
NPC's current optimized 65C02 lowering.
`LOAD` is optional and defaults to `$7000`. It accepts exactly four hexadecimal
digits and controls NL placement and relocation through the shared linker
mailbox. `CONFIG INLINE` makes NL read the project file's own linker sections;
`CONFIG file.cfg` remains available for direct/shared linker configurations and
is authoritative when combined with `LOAD`. NL accepts the first useful
ld65-shaped subset:

```text
MEMORY {
    MAIN: start = $8000, size = $0100, file = %O;
    DATA: start = $8100, size = $0080, file = %O;
}

SEGMENTS {
    CODE: load = MAIN, type = ro;
    RODATA: load = DATA, type = ro;
    BSS: load = DATA, type = bss;
}

SYMBOLS {
    CHAR_DEVICE: value = $A00E;
}
```

Keywords and names are case-insensitive. Each `start`/`size` pair defines a
non-overlapping writable application-RAM region from `$7000` through `$9FFF`;
`SEGMENTS` maps each NOBJ section into one of those regions. NL aligns sections,
relocates across regions, zero-fills gaps and BSS, and rejects missing mappings,
overlap, overflow, and output spans larger than its current 1 KB buffer.
An optional `SYMBOLS` block after `SEGMENTS` defines up to four absolute linker
symbols. NAS sources import them normally; NL resolves them case-insensitively
at the exact configured value without load-address rebasing and rejects any
same-name object export.
`MAP` is optional and writes an LF-only placement/export map. `LABEL` writes an
LF-only VICE label file (`al 00hhhh .NAME`) from the same live placed exports.
Direct `LINK main.obj [more.obj] [-C file.cfg] [-M file.map] [-Ln file.lbl]`
uses the same config, map, and label paths as project builds. NL loads
`NLWORK.OVL` at `$7000` after saving the binary to generate each requested
report through the same fixed language-neutral mailbox, then unloads it and
saves the report. Reports share NL's 1 KB output buffer and fail loudly if they
do not fit.

`BUILD HELLO.NPP` visibly runs resident NPC, then loads NAS, then loads NL.
Each stage prints its banner, input, output, and completion status. It compiles
the main Pascal source to textual 65C02 assembly, assembles that into an NOBJ,
and links the requested binary with `PASCAL.NLIB`. The generated `.S`, `.OBJ`,
and `.BIN` files remain on disk.

`DIR` prints complete filenames, including extensions. `EDIT` accepts ASCII
Pascal source/projects, assembly source/includes, linker configs, and ordinary
text while labeling the active type in the status bar. `TYPE` uses the same
ASCII validation and refuses binary files.

NPP 1 intentionally accepts one `MAIN` plus one optional prebuilt `OBJECT` or
one `ASM` source. NL now has real cross-object symbols, but repeated inputs and
multiple Pascal units still need expanded NPP syntax and unit semantics; the
project parser will not pretend source concatenation is linking.
