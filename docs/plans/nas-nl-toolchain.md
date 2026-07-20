# NAS/NL native toolchain roadmap

NAS and NL are Nova's language-neutral assembler and linker. Pascal is their
first client; the assembly development environment is the next. The target is
the useful ca65/ld65 class of capability, implemented natively rather than by
embedding Pascal or NDK knowledge in either tool.

## End-state contract

NAS must provide the complete W65C02 instruction/addressing matrix, expressions,
local and global symbols, imports/exports, multiple named sections, data/storage
directives, includes, conditionals, macros, diagnostics with source locations,
listings, and dependency output. Its command surface should cover the practical
ca65 options: CPU selection, include paths, command-line definitions, output and
listing names, case policy, auto-import, debug data, warning policy, and
verbosity.

Source is case-insensitive by default for mnemonics, directives, and symbols;
developers must not need to coordinate spelling case across files. A future
strict-case option may diagnose style, but cannot change the default ABI.

NL must link multiple objects and libraries, place/merge/alignment-check named
sections through a source-controlled linker configuration, resolve globals and
weak/forced imports, pull archive members to a fixed point, apply 8/16-bit and
relative relocations with range checking, emit map/label/debug files, define
configuration symbols, choose an entry point and output/load format, and remove
unreachable code/data at section or finer granularity when metadata permits.

Projects pass ordinary filenames and tool options through the shared tool
mailbox. Pascal `.NPP` and the future assembly project format may select NAS
options and an NL configuration, but neither owns a private assembler/linker
dialect. Direct shell commands and project builds invoke the same binaries and
configuration parser.

The shared option path is now concrete. `NPTOOL_LINK_BASE` carries the default
16-bit placement address into NL, while mailbox `ARG4` optionally names an
ASCII linker configuration. NPP accepts `CONFIG file` after optional
`LOAD $hhhh`; NL parses a case-insensitive ld65-shaped `MEMORY` region and uses
its `start` and `size` for relocation and output bounds. `MEMORY` now accepts up
to four non-overlapping regions and `SEGMENTS` maps up to eight case-insensitive
NOBJ section names to them. Direct `LINK main.obj [more.obj] -C file.cfg` uses
the same parser. Optional NPP `MAP file` and direct `-M file` output an LF-only
map of placed sections and selected global exports. `RUN` honors the resulting
executable prefix after validating its RAM range. NPP `LABEL file` and direct
`LINK ... -Ln file` now emit LF-only VICE labels from those same live placed
exports. Debug files and broader command options remain.

NAS command/project options now include one case-insensitive preprocessor
definition and direct object naming: `ASSEMBLE file.s [-Dname=value]
[-o file.obj]`; NPP `DEFINE name=value` shares the same fixed mailbox and
preprocessing implementation.

NPC owns Pascal-specific lowering and optimization. It emits readable,
symbolic W65C02 source, uses target-aware instruction selection and constant
folding, removes redundant/dead operations before emission, and leaves
whole-program reachability stripping to NL. Generated source uses LF line
endings and annotates lowered statement blocks with the nearest Pascal filename
and line number (for example, `; HELLO.PAS:3 writeln(...)`). NAS treats those as
ordinary comments and remains completely language-neutral.

## Object and archive direction

NOBJ v2 is section-aware and NAS emits named initialized and zerofill sections:

- an object header locates section, symbol, and relocation records;
- sections carry a name, allocation flags, alignment, memory size, and data;
- symbols are indexed and identify a section, absolute value, or undefined
  import, plus local/global binding;
- fixed-size relocations identify the patch section, relocation type, symbol,
  and signed addend.

NLIB v2 contains length-prefixed complete NOBJ members. Each member's global
symbol table is its export index, avoiding duplicate archive metadata while
allowing one member to expose several names and import other members. NL's
reachability overlay pulls a member only when a live relocation needs one of
its exports, then scans newly live member sections to a fixed point.

## Delivery order

1. NOBJ v2, two-pass local labels, indexed relocations, local-symbol linking.
2. Complete W65C02 opcode/addressing table and numeric/expression engine.
3. Data/storage, imports/exports, named sections, includes and diagnostics.
4. Multiple-object NL, global resolution, NLIB v2 and fixed-point extraction.
5. Linker configuration, placement/alignment, maps/labels and project options.
6. Macros/conditionals/debug metadata, reachability stripping and assembly IDE.

Step 4's object/archive path is implemented: NL's core accepts up to four
ordered NOBJ inputs, maintains per-object section placement, resolves global
imports against object exports, and adds demanded NLIB v2 NOBJ members to that
same table. The growing table is scanned along live relocation edges to a fixed
point before placement, so transitive imports inside selected archive members
work normally while dead imports and unused members remain absent. The native
shell currently exposes two explicit inputs as `LINK MAIN.OBJ UTIL.OBJ`. Once
selection stabilizes, NL rejects duplicate case-insensitive globals across the
selected object set even when no relocation references them. NPP object lists
are started with one optional ordinary `OBJECT file.obj` or `ASM file.s`; BUILD
assembles the latter before linking. Repeated project inputs remain.

NAS now emits and NL applies every NOBJ v2 relocation kind: range-checked
absolute bytes and relative branches, low/high-byte extraction, and absolute
words. Ordinary `.BYTE`/`.WORD`, unary low/high-byte expressions, immediate
operands, and cross-section/imported branches drive those records.
`.BYTE` now mixes expressions with case-preserving quoted ASCII and common
backslash escapes, using the same checked section emitter as numeric data.
Case-insensitive `NAME = expression` assignments now emit absolute symbols that
fold locally or resolve through ordinary cross-object exports without rebasing;
forward dependency chains converge before emission and cycles fail loudly.
Ca65-style `@name` cheap locals are now qualified by the preceding non-local
label in the shared identifier path, allowing repeated case-insensitive loop
names without changing NOBJ or NL.
Ca65-style anonymous `:` definitions and forward/backward `:+`/`:-` references,
including repeated signs, use deterministic internal symbols on every pass.
Case-insensitive `.ASSERT expression` now checks absolute assembly-time
invariants during the final pass, emits no bytes, and has a distinct
source-located failure diagnostic.

NAS and NL are currently 2,290 and 6,905 bytes on disk. NAS now allocates its
3 KB source input through `MEM_ALLOC`, streams it with `MEM_XLOAD`, parses it
through an XRAM window, and releases it on every exit path. Nested `.INCLUDE`
sources use independent XRAM allocations and retain their filenames for
diagnostics. NAS loads the 5,668-byte `NASPP.OVL` NOVO
preprocessing image at `$7000` and exchanges source/result streams through
XRAM. That overlay provides case-insensitive parameterized macros and nested
16-bit constant-expression `.IF`/`.ELSEIF` conditionals plus
`.DEFINE`/`.UNDEFINE` identifier
substitution and the shared direct/project definition option. Expressions
cover arithmetic, shifts, bitwise logic, comparisons, boolean operators,
grouping, unary transforms, ca65's case-insensitive spelled operator aliases,
and one-level definition expansion. Ordinary source lines normalize those same
aliases before backend assembly. Its complete image ends at `$8C4D`, leaving
5,043 bytes below `$A000`. Recursive expansion remains in this overlay phase.
NAS then loads the 8,202-byte `NASBE.OVL` into the same slot. It contains the complete two-pass
assembler core plus opcode matrix. Its expression engine now handles arithmetic,
shifts, bitwise logic, comparisons, and boolean operators. Its full image ends
at `$99B3`, leaving 1,613 bytes below `$A000`. The resident file/XRAM frontend now ends at `$371A`,
leaving 14,565 bytes below the overlay boundary. NL's complete
lower-RAM image now ends at `$4F08`, leaving 8,439 bytes below the overlay
boundary. The 2,251-byte `NLWORK.OVL` receives direct objects and the validated
archive through a fixed mailbox, marks direct entry/export roots, follows
relocation edges to a fixed
point, selects archive members only for live unresolved globals, and returns
the selected object table plus one live-section bitmap per object. Resident NL
then places and relocates only live sections. Explicit exports are the keep
mechanism; unreferenced local sections are omitted from the binary,
configuration, and map. The same overlay emits optional placement/export maps
and VICE label files after linking; both reports use the identical live-export
walk.

NL configuration now accepts an optional ld65-shaped `SYMBOLS` block after
`SEGMENTS`. Up to four case-insensitive names resolve as absolute 16-bit values
without rebasing, participate in GC/archive lookup, and conflict loudly with
same-name object exports. This is the first generic path for generated NDK and
hardware constants; neither NAS nor NL hardcodes a Pascal or Nova API table.
