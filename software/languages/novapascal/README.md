# NovaPascal shell

`novapascal.ndi` boots into `NovaPascal Shell v1.0`. Run `HELP` for the native
file, editor, compiler, assembler, linker, build, and run commands.

The shell and native Pascal compiler (NPC) remain resident in the
`$C000-$FFFF` language ROM. For each active primary or `{$I}` source, NPC
allocates one 16 KiB block through the NDK Memory module and treats its halves
as alternating 8 KiB cache pages. Refills use the generic Files-module
`FILE_PAGE` service and a 24-bit disk offset, so source size is bounded by the
Nova file format rather than the cache allocation. Parser marks preserve the
page cursor across lookahead, and every allocation is released after generated
assembly has been streamed to disk.

O2 is a separate disk-loaded compiler stage, `NPO2.BIN`, so optimization does
not consume scarce resident ROM and NAS/NL remain language-neutral. NPC emits a
compact typed linear IR into per-source `.ASM` or direct-build `.S` streams.
Before NPO2 runs, NAS preprocessing expands generic `.INCLUDETEXT` composition
so the optimizer sees one typed program without expanding ordinary assembly
includes. NPO2 uses two transient
16 KiB NDK-managed XRAM buffers for six streaming passes and releases both on
every exit path: typed dataflow optimization;
three rounds of leaf/caller inlining and dead-routine removal; 65C02 instruction
selection; and final machine peepholes. The typed pass strength-reduces byte and
word self-updates, canonicalizes unsigned zero comparisons, forwards repeated
effect-free array loads, removes stack temporaries from byte accumulations, and
forms guarded constant-offset array windows once for indexed-indirect reuse.
Call-free function tails keep their result in compiler scratch, while the stack
ABI remains the conservative fallback. Inlining is limited to local leaf
routines with exactly one static call site and iterates so their callers can
become eligible. NPO2 overwrites the `.S` file with ordinary readable 65C02
assembly before NAS runs; no `.O2` records reach NAS or the final `.S`.
Standalone unit `.ASM` is intentionally published typed IR so another Pascal
build can retain routine boundaries for whole-program O2.
Both `COMPILE` and `BUILD` print the optimizer banner and every pass as it runs.

## Executable language slice

`FIZZBUZZ/MAIN.PAS` with `FIZZBUZZ/FIZZBUZZ.NPP` and `LIFE/MAIN.PAS` with
`LIFE/LIFE.NPP` are executable end-to-end compiler slices. Both build and run
on Nova through NPC, NPO2, NAS, NL, and `PASCAL.NLIB`. FizzBuzz exercises byte
expressions and structured control flow. Life seeds a random, roughly half-full
80-by-25 Conway board, applies B3/S23 simultaneously through two 2,000-cell
buffers, and renders across all 320-by-200 pixels until Enter is pressed.

NPC now uses recursive-descent statement and expression parsers rather than a
single fixed statement loop. The implemented language core is case-insensitive
and supports `Byte`, `Boolean`, `Char`, and unsigned 16-bit `Word` variables;
nested `begin`/`end`; assignment; `if`/`then`/
`else`, `while`/`do`, `+`, `-`, `mod`, parentheses, all six byte comparisons,
unsigned `Word` arithmetic and comparisons, zero-based `array[0..N] of Byte`
and `array[0..N] of Boolean` storage with byte or word indices, Boolean
literals, string `writeln`, decimal byte `writeln`, parameterless procedures,
and parameterless `Byte`/`Boolean` functions. A function returns through
standard Pascal assignment to its own name. Duplicate variables are rejected.
Generated branches remain valid when a structured statement grows beyond the
65C02 relative-branch range.

Routine declarations appear after global variables and before the program's
main `begin`. NPC records routine boundaries, calls, and function-result effects
in typed IR. NPO2 lowers ordinary `JSR`/`RTS` and stack-resident results when a
routine must remain callable; at O2 it removes a single-call leaf and substitutes
its body at the call site, and it uses an NDK scratch byte when no call can occur
after a function-result assignment. Routines using Pascal parameter/local
frames remain callable until O2 has a dedicated frame-elision transform; it
never deletes the caller return pair that the frame ABI uses to find packed
arguments. Life remains readable Pascal built from
`RandomCell`, `Seed`, `Draw`, `CountNeighbors`, `NextCell`, `Evolve`, and
`Commit`; optimization does not require source-level flattening.

## Inline assembly

Small 65C02 fragments use a Pascal `asm` statement and ordinary NAS syntax:

```pascal
asm
  lda #'A'
  jsr P_WRITE_CHAR
end;
```

`asm` must end its source line, and the matching `end` starts its own line.
NPC copies every intervening line into generated assembly, preserving spelling
and NAS comments while normalizing line endings to LF. NPO2 treats those lines
as opaque barriers: it neither interprets them as compiler IR nor carries
register or memory facts across them. An assembly block may therefore clobber
A, X, Y, flags, and memory, but it must restore the 65C02 stack pointer and fall
through to the following Pascal statement. O2 also keeps routines containing
assembly out of source inlining, retains Pascal routines named by assembly,
uses stack-backed function results, and rebuilds cached array addresses after
the block.

Symbols made available by a Pascal `uses` clause remain available to NAS in the
block. Substantial assembly belongs in the existing unit bundle (`.PAS`, `.NPI`,
`.INC`, and `.S`) so it retains a Pascal-facing contract and the canonical NDK
implementation rather than growing inside a Pascal source file.

NPC loads `NPCFE.OVL` at `$1E00-$9FFF` only while compiling. The overlay owns
the recursive-descent parser and typed tables; its source bytes stay in
NDK-allocated XRAM pages. Include buffers are keyed by active nesting depth, so
finished sequential `{$I}` files reuse the same 16 KiB double buffer; generated
assembly is written incrementally through `FILE_FWRITE`. Generated arithmetic uses the canonical NDK
pseudo-register mailbox declared by `NVR.INC`; compact word, comparison, and
indexed-array helpers are ordinary members of `PASCAL.NLIB`, so NL extracts
them only when referenced. Decimal byte output is a separate
`P_WRITE_BYTE` archive member, so NL omits it from programs that only print
strings. NPC emits zero-, one-, and two-character `writeln` literals as direct
character calls. Longer literals use Nova's inline-parameter ABI:

```asm
JSR I_P_WRITE_LINE
.BYTE $46,$69,$7A,$7A,$00
```

The `I_` prefix means immutable parameters follow the call. The routine walks
the bytes and advances the saved return address past them before returning;
NAS and NL require no special handling. Syntax failures are reported as
`file:line:column` diagnostics.

`Word` is unsigned; `Integer` is signed 16-bit; `Boolean` and `Char` occupy one
byte. `Real` is signed Q16.16 and executes in the shared paged `LANGRT` module.
Multiplication, square root, sine, cosine, and arctangent use Nova's math
coprocessor; the runtime scales its integer and fixed-point results back into
Pascal `Real` values. General Q16.16 division remains software because the
hardware divider intentionally returns only a signed 16-bit quotient.
The MicroCalc compatibility slice exercises enumerations, subranges,
sets, records, strings, multidimensional arrays, `Real`, nested and recursive
routines, value and `var` parameters, locals, typed/text files, and formatted
output. Public project-unit declarations remain intentionally narrower:
parameterless procedures and parameterless `Byte`/`Boolean` functions. Pointers,
wider integers, unit initialization/finalization, and Object Pascal are not yet
language contracts.

## Pascal standard units

Pascal programs consume Nova hardware through typed Pascal units, not raw NDK
registers or assembly calling conventions. The first native adapter is
`NovaGraphics`:

```pascal
uses NovaGraphics;

GraphicsOpen;
GraphicsClear(0);
GraphicsColor(10);
GraphicsFill(96, 36, 102, 42);
GraphicsWait(30);
GraphicsClose;
```

`GraphicsTile16(Column, Row)` fills a 16-pixel cell in a full-width 20-by-12
grid. `GraphicsTile4x8(Column, Row)` maps an 80-by-25 logical grid onto the
entire 320-by-200 plane, including horizontal coordinates that a Pascal `Byte`
cannot express directly. `NovaInput.PollKey()` provides nonblocking native key input. Life
waits for vertical sync before each generation and repaints only changed cells,
so it never clears the visible plane between generations.

`NOVAGFX.PAS` is the human-readable unit contract. Its precompiled
`GRAPHICS.NPI` interface supplies NPC's checked signatures, while
`GRAPHICS.INC` and `GRAPHICS.S` implement those Pascal procedures over the
canonical `vgc.inc` and `vgc.s` NDK sources. Only the adapter knows the VGC
parameter layout and wait protocol. `Life` consequently contains no MMIO
addresses or NDK pseudo-registers. Its `Commit` procedure is the inline-NAS
example: a direct 2,000-byte `Next`-to-`Cells` copy using ordinary Pascal symbol
names. Multi-argument unit procedures use a byte-stack ABI; NPC emits an exact
arity assertion and removes the arguments after the call.

`NovaRandom` follows the same model. Its Pascal-facing `RandomByte(): Byte`
function adapts the canonical `rng_get8`, `RNG_VALUE0`, and FIO implementation;
Life neither invents a private pseudo-random generator nor sees their ABI.
Stable, Pascal-shaped unit APIs backed by canonical NDK implementations remain
the model for the rest of the NDK. Standard platform units remain precompiled,
generated components tied to canonical NDK metadata. Project-owned units are
ordinary `.PAS` sources compiled as part of an NPP 2 build.

`NovaMemory` exposes allocation without leaking platform address widths into
Pascal. `RamBlock` is an opaque pointer/length descriptor over the unused tail
of the project's configured application-RAM region; `RamAlloc`, `RamFree`,
`RamRead`, and `RamWrite` use 16-bit sizes and offsets, bounds-check every
transfer, and coalesce released blocks without a fixed allocation table.
`XRamBlock` similarly hides the allocator's 24-bit address while
`XRamAlloc`, `XRamFree`, `XRamRead`, and `XRamWrite` retain a Pascal-sized
16-bit allocation/transfer contract and call the canonical NDK Memory module.
The readable `NOVAMEMORY.PAS` contract declares both descriptor types. Current
`.NPI` metadata validates routine calls but does not yet import unit type
declarations, so source clients temporarily repeat those two array aliases;
compiled-unit type metadata is the clean removal path.

The development disk keeps those roles physically separate. `/SYSTEM` contains
generated NDK contracts, declarations, implementations, and `PASCAL.NLIB`;
users do not edit it. `/USER` contains reusable user unit projects and their
published build artifacts. New manifests search both roots in this order:

```text
UNITPATH SYSTEM
UNITPATH USER
```

NAS performs the actual case-insensitive include lookup through those generic
paths. NPC contains no disk layout, NDK symbol table, or hardware API table.

## Low-level NDK bindings

NPC accepts a standard `USES` clause for canonical NDK source units. A unit
named `NovaFoo` maps mechanically to `FOO.INC` and `FOO.S`; NPC embeds neither
an API table nor hardware addresses. The first typed bridge uses the NDK's
one-byte accumulator ABI and named byte storage directly:

```pascal
program RandomDemo;
uses NovaRng, NovaFio;
var Status, Sample: Byte;
begin
  Status := rng_get8();
  Status := fio_exec(Byte(FIO_CMD_RNG));
  fio_issue(Status);
  Sample := RNG_VALUE0;
  VGC_BORDER := Sample;
  writeln('Pascal NDK');
end.
```

Static variables occupy linker-managed `BSS`. When BSS is the final live
segment, NL omits its zero bytes and appends the eight-byte `NBS1` trailer;
`MEM_EXEC_IMAGE` validates that trailer and zeroes the exact range before entry.
Legacy flat binaries remain valid. A
no-argument `function()` returns its byte in A; a one-byte routine argument is
passed in A. Decimal, `$` hexadecimal, and single-character byte values are
accepted. `Byte(NDK_CONSTANT)` loads a generated, range-checked canonical byte
constant such as `FIO_CMD_RNG`. An identifier used as a value or assignment
target remains a symbolic byte address, which gives Pascal direct access to NDK
pseudo-registers such as `RNG_VALUE0` without teaching NPC their addresses.

NPC emits declaration includes before program code and implementation includes
after its terminating `RTS`. NAS uses `.referenced()`/`.REFTO` to omit unused
routines, and NL links only live NOBJ sections. Each unit follows the canonical
NDK include graph and appends dependency implementations after its own body.
Consequently `uses NovaRng;` can call RNG without naming or ordering its internal
FIO dependency, while a program that directly calls FIO still declares
`NovaFio` for its typed interface.

The development disk installs generated unit bundles for all 42 annotated NDK
libraries: Nova, Array, RAM Heap, FIO, Audio, VGC, Sprite, Meta-Sprite, Virtual
Sprite, Virtual Text, NUI and its component libraries, Copper, DMA, Blitter, XRAM, XMC, Pager,
RNG, NVG, Animation, Tween, NIC, Game Server, Overlay, Mouse, Sprite Bank, WTS,
and the focused VGC/FIO/text helpers. Each bundle has a human-readable
`NOVA*.PAS` contract, checked `.NPI` ABI, declaration `.INC`, and implementation
`.S`. Library underscores are removed from Pascal names and disk stems:
`vgc_vsync` is `NovaVgcVsync` and `VGCVSYNC.*`; `fio_clear_error` is
`NovaFioClearError` and `FIOCLEARERROR.*`.

The generator composes split libraries from every canonical source containing
Pascal-callable entries rather than maintaining copied implementations.
Assembly-only inline-parameter sources are deliberately omitted, and header-only
dependencies remain `.INC` dependencies instead of inventing nonexistent `.S`
assets. This preserves dead stripping and the typed unit boundary. NPC supports
16 units per program and embeds no API catalog or hardware address.
It emits signature assertions for every typed call, so NAS rejects the wrong
byte arity or use of a procedure as a function before linking. Generated
contracts currently describe byte constants, byte storage, and routines whose
explicit CPU-register ABI uses A. X/Y and wider-value ABIs remain available in
the assembly implementation but are omitted from the typed contract until the
Pascal type system can represent them cleanly. Pascal-shaped adapters such as
`NovaGraphics`, `NovaInput`, and `NovaRandom` continue to hide multi-register
NDK protocols for common application code.

Commands load these ordinary Nova load-address-prefixed binaries from disk
into the shared `$1D00-$7FFF` tool slot, one at a time. NPC is the exception:
its small driver is resident and its frontend is the `NPCFE.OVL` described
above. The frontend allocates its 4,864-byte symbol and constant/type-name
arena through the NDK Memory module, maps individual records through XRAM
Windows 0 and 1, and releases the arena on every exit path. Keeping this
growth-sensitive state out of overlay BSS leaves more than 5 KiB in NPCFE for
the Turbo Pascal compatibility work; focused coverage requires at least 4 KiB
to remain.

- `NPEDIT.BIN` — thin file/type adapter for the shared editor (`Alt-X` or
  `Ctrl-Q` returns to the shell). It streams documents into transient XRAM;
  generic Editor-module paging keeps only the current window in lower RAM.
  `EDIT project` opens its manifest-selected main source, and `Ctrl-O` uses the
  NDK file picker to switch to any text file in that project, including `.NPP`,
  `.PAS`, `.S`, `.ASM`, `.INC`, `.NPI`, linker configuration, and maps.
  Pascal files install NPEDIT's lexical hook for identifiers, numbers, quoted
  strings, and Pascal comments; other file types remain plain editor clients.
- `NPO2.BIN` — Pascal-specific typed dataflow, inlining, instruction selection,
  and O2 machine peepholes
- `NAS.BIN` — Nova assembler
- `NL.BIN` — Nova linker

NAS loads `NASPP.OVL` at `$7000` for preprocessing. The resident frontend
passes input and output XRAM allocations through a stable mailbox and uses the
NDK `SYS_OVL_LOAD` and `SYS_OVL_UNLOAD` lifecycle. It calls the overlay's
declared fixed entry from resident RAM because `.INCLUDETEXT` callbacks may
page in the NDK Memory module; a banked `SYS_OVL_MAIN` frame cannot safely
survive that nested module call. The first
overlay implementation expands case-insensitive `.MACRO name p1, p2` through
`.ENDMACRO` definitions and nested `.IF`, `.IFDEF`, `.IFNDEF`, `.ELSEIF`,
`.ELSE`, and `.ENDIF` blocks. `.DEFINE name value` performs case-insensitive identifier
substitution outside strings and comments; `.UNDEFINE name` removes it. `.IF`
evaluates 16-bit constant expressions after one level of definition expansion.
`.INCLUDETEXT "file"` recursively splices text through the same caller-supplied,
case-insensitive include paths before macro expansion and source-to-source
optimization. Definitions from the including source are visible in nested
text; definitions declared by included text are scoped to that included source.
Ordinary `.INCLUDE` remains an assembly-phase directive, so
normal NDK sources are not pulled into Pascal's typed optimizer.
After preprocessing, NAS replaces that overlay with `NASBE.OVL`. It owns the
complete language-neutral two-pass assembly core and W65C02 opcode/addressing
tables; the small resident executable supplies XRAM/file I/O callbacks through
a fixed mailbox. Parsing, symbols, sections, relocations, expressions, and the
opcode matrix therefore consume the overlay slot rather than lower RAM.
NL invokes `NLWORK.OVL` through the same NDK overlay API. No toolchain stage has
a private overlay loader.

Direct links default to `$7000`; `NEW` projects instead configure the complete
`$0900-$9FFF` application window so code, static BSS, and the dynamic RAM tail
share one checked region. `RUN` stages the
complete load-address-prefixed image in allocator-owned XRAM, then delegates
validation, DMA placement, allocation release, and control transfer to the
shared NDK `MEM_EXEC_IMAGE` service. Pascal `Halt` delegates to
`MEM_EXIT_IMAGE`, which restores the launcher's hardware stack and returns to
the shell from any Pascal call depth. The accepted application window is
`$0900-$9FFF`; executable size is independent of the shell's project buffer.
NAS allocates source, preprocessed text, constants, and include files through
`MEM_ALLOC`, loads files with `MEM_XLOAD`, and balances every allocation with
`MEM_RELEASE`, including errors. Lower RAM is reserved for the object, symbol,
and relocation working set; large scratch buffers are uninitialized workspace
and are not redundantly cleared at startup.

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
object set are rejected even if no live relocation references them. NOBJ's
one-byte object count permits 255 selected objects. NL keeps per-object
state in one NDK-managed XRAM allocation rather than a fixed lower-RAM table;
the direct shell command currently exposes two explicit object arguments.
The Pascal-specific code lives in `PASCAL.NLIB`, whose machine operands are
assembled from NDK definitions in `nova.inc`. `P_WRITE_CHAR` currently pulls a
separate `P_CHAR_DEVICE` hardware shim through an ordinary NOBJ import, proving
that archive members may depend on other members. The bulk byte and inline-line
writers are leaf members backed directly by the same NDK definition, keeping
mixed numeric/string programs compact.
Neither NAS nor NL knows Pascal symbols or Nova hardware addresses.

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
Direct source preprocessing uses a 32 KiB XRAM buffer; already-preprocessed
project output pages from disk and is not held as one source buffer. Include
buffers start at 4 KiB and retry up to 52 KiB, large enough for the canonical
`nova.inc`. Local
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
NAS can emit a 65,535-byte NOBJ and keeps up to 1,024 case-insensitive symbols
and 32,768 relocations in NDK-managed XRAM. NL likewise streams root objects
into XRAM, allocates output for the full application window, and keeps its
selected-object state in XRAM. The remaining explicit ceilings are the 16-bit
NOBJ/file lengths, eight sections per object, a 12 KiB linker library, and a
12 KiB map or label report; none of those tables consume the application's
lower RAM.
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

`NEW HelloWorld` creates a complete project directory without overwriting an
existing project:

```text
HELLOWORLD/
    MAIN.PAS
    HELLOWORLD.NPP
```

`MAIN.PAS` is a minimal program that prints `Hello, world!`.
`HELLOWORLD.NPP` is the single end-to-end NPC, NAS, and NL configuration. It
selects O2, defines `NOVA=1` for NAS, links initialized `CODE` and zero-fill
`BSS` in the full `$0900-$9FFF` application region, and requests `.MAP` and
`.LBL` reports. Project and unit names
are case-insensitive Pascal identifiers of at most 15 characters.

The project commands are:

```text
NEW project
NEWUNIT unit
ADDUNIT project unit
DELUNIT project unit
DELPROJECT project
BUILD project
RUN project
```

`ADDUNIT HELLOWORLD GREETER` creates `GREETER.PAS` with a normal Pascal
`interface`/`implementation` skeleton and inserts `UNIT GREETER.PAS` into the
manifest. `DELUNIT` removes that manifest entry, its source, and its generated
unit files. Both commands invalidate stale project output. A project may list
up to 16 units. Names beginning with `Nova` are reserved for generated NDK
platform units.

`NEWUNIT GREETLIB` creates `/USER/GREETLIB` with `MAIN.PAS` and
`GREETLIB.NPP`. Its `TARGET UNIT` manifest makes `BUILD GREETLIB` publish
typed `/USER/GREETLIB.ASM`, readable optimized `/USER/GREETLIB.S`, and
language-neutral `/USER/GREETLIB.OBJ`, then stops without invoking NL or making
a fake executable. Public interface routines are NOBJ exports. Another Pascal
project can simply `uses GreetLib`; NPC selects the typed stream through
`UNITPATH USER` with NAS's generic `.INCLUDETEXT`, preserves signature checking
and routine boundaries, and lets whole-program O2 remove unused unit code.
Assembly projects use the ordinary
`.S` or link the `.OBJ`; future BASIC, Forth, and Logo bindings can target that
NOBJ ABI instead of adopting a Pascal-specific object format. Named project
commands search the current directory first and then `/USER`.

`DELPROJECT` validates that the directory contains an NPP 2 project, refuses a
project containing nested directories, then deletes the ordinary files in the
project and removes the directory. The nested-directory check prevents the
command from silently deleting a directory tree.

## Project-owned Pascal units

A program names a project unit with the familiar Pascal `uses` clause:

```pascal
program HelloWorld;
uses Greeter;
begin
  Greet;
end.
```

The unit declares its public interface separately from its implementation:

```pascal
unit Greeter;

interface
procedure Greet;

implementation
procedure Greet;
begin
  writeln('Hello, world!');
end;

end.
```

Public declarations and definitions must match in order and signature. The
current project-unit ABI supports parameterless procedures and parameterless
`Byte`/`Boolean` functions. Unit parameters, local storage, and
initialization/finalization blocks are intentionally deferred until NPC can
implement their Pascal semantics cleanly.

The NPP manifest owns build membership; source files do not include one another.
`uses` owns Pascal visibility and calls. Every listed unit is compiled, and O2
removes routines that are unreachable from the program. Project units do not
create another persistent interface artifact: NPC emits checked signatures into
their generated assembly, and the whole-project optimizer sees the validated
program and unit streams together.

## NPP project manifests

NPP 2 adds repeated `UNIT` entries to the original project format:

```text
NPP 2
MAIN MAIN.PAS
UNIT GREETER.PAS
UNIT FORMAT.PAS
UNITPATH SYSTEM
UNITPATH USER
OUTPUT HELLOWORLD.BIN
OPTIMIZE O2
DEFINE NOVA=1
CONFIG INLINE
MAP HELLOWORLD.MAP
LABEL HELLOWORLD.LBL
MEMORY {
    RAM: start = $0900, size = $9700, file = %O;
}
SEGMENTS {
    CODE: load = RAM, type = ro;
    BSS: load = RAM, type = bss;
}
```

`BUILD HELLOWORLD` enters the project directory and validates the complete NPP
and linker configuration before creating intermediates. It compiles
`MAIN.PAS` and each `UNIT` to readable typed `.ASM` streams, combines the unit
streams in manifest order with the program entry point, expands reusable-unit
`.INCLUDETEXT` directives, and runs NPO2 across the whole
program, assembles one NOBJ with NAS, and links it with NL and `PASCAL.NLIB`.
Each tool prints its banner, inputs, outputs, pass names, and completion status.
Per-source `.ASM`, combined project `.S`, `.OBJ`, `.BIN`, `.MAP`, and `.LBL`
files remain in the project directory. Distinct assembly extensions prevent a
project named `MAIN` from colliding with the compiler output for `MAIN.PAS`.
`RUN HELLOWORLD` loads that project's `.BIN` and starts it at its encoded load
address.

NPP 1 remains accepted for compatible flat, single-source projects. It permits
one `MAIN` and no `UNIT` entries. NPP 2 is required by the project mutation
commands. Either version may name one optional ordinary NAS-produced `OBJECT`,
or one `ASM` source that BUILD assembles before linking; the forms are mutually
exclusive. Optional `DEFINE name=value` uses the same case-insensitive NAS
preprocessor path as direct `ASSEMBLE file.s -Dname=value`.

`TARGET PROGRAM` is the default. `TARGET UNIT` makes `OUTPUT` name an NOBJ,
publishes sibling typed `.ASM` and optimized `.S` files from the same stem,
exports every public interface routine in the standalone object, and ends the
build after NAS. A program consumes the typed stream so O2 can discard public
routines it never calls. Repeated `UNITPATH` directives
(currently up to two) are shared NAS settings; generated projects use `SYSTEM`
followed by `USER`.

`OPTIMIZE O2` is optional and explicitly selects NPC's current optimized 65C02
lowering; O2 is also the default. `LOAD` is optional and defaults to `$7000`.
It accepts exactly four hexadecimal digits and controls NL placement and
relocation through the shared linker mailbox. `CONFIG INLINE` makes NL read the
project file's own linker sections. `CONFIG file.cfg` remains available for a
shared linker configuration and is authoritative when combined with `LOAD`.
NL accepts this useful ld65-shaped subset:

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
non-overlapping writable application-RAM region from `$0900` through `$9FFF`;
`SEGMENTS` maps each NOBJ section into one of those regions. NL aligns sections,
relocates across regions, zero-fills interior gaps, represents final BSS with an
`NBS1` trailer, and rejects missing mappings, overlap, overflow, and output spans
beyond the application window.
An optional `SYMBOLS` block after `SEGMENTS` defines up to four absolute linker
symbols. NAS sources import them normally; NL resolves them case-insensitively
at the exact configured value without load-address rebasing and rejects any
same-name object export.

`MAP` is optional and writes an LF-only placement/export map. `LABEL` writes an
LF-only VICE label file (`al 00hhhh .NAME`) from the same live placed exports.
Direct `LINK main.obj [more.obj] [-C file.cfg] [-M file.map] [-Ln file.lbl]`
uses the same config, map, and label paths as project builds. NL loads
`NLWORK.OVL` at `$8000` after saving the binary to generate each requested
report through the same fixed language-neutral mailbox, then unloads it and
saves the report. Reports use a 12 KiB NDK-managed XRAM buffer and fail loudly if they
do not fit.

`DIR` prints complete filenames, including extensions. `EDIT` accepts ASCII
Pascal source/projects, assembly source/includes, linker configs, and ordinary
text while labeling the active type in the status bar. `TYPE` uses the same
ASCII validation and refuses binary files. Direct `BUILD file.pas`,
`BUILD file.npp`, and `RUN file.bin` remain available for compatible flat-file
workflows.
