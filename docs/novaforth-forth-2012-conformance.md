# NovaForth Forth 2012 Conformance Statement

Status: provisional
Target standard: Forth 2012
Implementation: NovaForth v0.1 on Nova/e6502

This document is the current conformance claim for NovaForth. It is intentionally
limited to word sets that are implemented, advertised by `ENVIRONMENT?`, and
covered by local automated tests.

## Claimed Word Sets

| Word set | Claim | Evidence |
| --- | --- | --- |
| Core | yes | `Forth2012CoreSuitePasses` |
| Core Extensions | yes | `Forth2012CoreExtSuitePasses` |
| File Access | yes | `Forth2012FileAccessSuitePasses` |
| String | yes | `Forth2012StringSuitePasses` |
| String Extensions | yes | `Forth2012StringSuitePasses` |

The manual combined runner for the currently claimed word sets is:

```forth
INCLUDE forth/test/forth2012/nova-all.4th
```

The automated conformance gate uses the focused runners listed above. The
combined runner is intentionally manual because the preliminary and claimed word
set suites together take several billion emulated instructions.

Do not use upstream `forth/test/forth2012/src/runtests.fth` as a NovaForth
conformance claim. That file includes optional word sets NovaForth does not
currently advertise.

## Not Claimed

NovaForth currently reports these word sets as unavailable through
`ENVIRONMENT?` and does not claim conformance for them:

| Word set | Status |
| --- | --- |
| Block | not implemented |
| Block Extensions | not implemented |
| Double Number | not implemented |
| Double Number Extensions | not implemented |
| Exception | not implemented |
| Exception Extensions | not implemented |
| Facility | not claimed |
| Facility Extensions | not claimed |
| File Access Extensions | not claimed |
| Floating Point | not implemented |
| Floating Point Extensions | not implemented |
| Locals | not implemented |
| Locals Extensions | not implemented |
| Memory Allocation | not implemented |
| Memory Allocation Extensions | not implemented |
| Programming Tools | partial, not claimed |
| Programming Tools Extensions | not claimed |
| Search Order | not implemented |
| Search Order Extensions | not implemented |

Some words that appear in optional word sets may exist as implementation
extensions or as test support. Their existence is not a word-set conformance
claim unless the word set is listed under "Claimed Word Sets".

## Environmental Queries

The current `ENVIRONMENT?` table advertises:

| Query | Value |
| --- | --- |
| `/COUNTED-STRING` | `255` |
| `/HOLD` | `34` |
| `ADDRESS-UNIT-BITS` | `8` |
| `FLOORED` | true |
| `MAX-CHAR` | `255` |
| `MAX-D` | `2147483647` |
| `MAX-N` | `32767` |
| `MAX-U` | `65535` |
| `MAX-UD` | `4294967295` |
| `RETURN-STACK-CELLS` | `32` |
| `STACK-CELLS` | `64` |
| `CORE` | true |
| `CORE-EXT` | true |
| `FILE` | true |
| `STRING` | true |
| `STRING-EXT` | true |

The same table reports the unclaimed optional word sets listed above as false.

## Implementation Characteristics

NovaForth currently uses:

| Characteristic | Value |
| --- | --- |
| address unit | 8 bits |
| cell size | 16 bits |
| stack capacity | 64 cells |
| return stack capacity | 32 cells |
| signed integer range | `-32768..32767` |
| unsigned integer range | `0..65535` |
| double-cell signed range | `-2147483648..2147483647` |
| double-cell unsigned range | `0..4294967295` |
| division semantics | floored |
| counted string maximum | 255 characters |
| pictured numeric output buffer | 34 characters |
| include/evaluate source nesting | 8 levels |
| file name limit | 63 characters |
| dictionary limit | `$9800` |

The ROM boots `forth/lib/core.4th` and then `forth/autoexec.4th` when present.
The default `autoexec.4th` includes `forth/lib/core-ext.4th`.

## File Access Notes

Standard File Access words live in `forth/lib/file.4th` and are loaded by the
core library. Nova-specific file-domain NDK wrappers live in
`forth/lib/nova/files.4th` and use `FILES-*` names so they do not shadow standard
Forth words.

Paths are interpreted by the Nova storage host. The Forth test runner changes
the current directory with `NOVA-CD` before running the upstream File Access
suite so that helper files resolve correctly.

## Extensions

NovaForth provides implementation-specific words and libraries outside the
Forth 2012 claim, including:

| Area | Examples |
| --- | --- |
| runtime and host integration | `NOVA-LIBCALL`, `SAVE-FORTH`, `EDIT` |
| file convenience | `INCLUDE`, `REQUIRE`, `REQUIRED`, `NOVA-CD` |
| Nova NDK wrappers | `FILES-*`, `GFX-*`, `MEM-*`, `SND-*`, `NET-*`, `TUR-*`, `TURTLE-*` |
| graphics compatibility | `GRAPHICS`, `GCLS`, `GCOLOR`, `PLOT`, `CIRCLE` in `forth/lib/nova/vgc.4th` |
| turtle graphics | `FD`, `BK`, `RT`, `LT`, `CS`, `SS`, `TS`, `SETPC`, `SETBG` |

These words are useful NovaForth features, not portable Forth 2012 words.

## Ambiguous Conditions

NovaForth tries to fail loudly for common interactive errors, but behavior under
Forth 2012 ambiguous conditions is not a portability guarantee. Examples include:

| Condition | Current behavior |
| --- | --- |
| data stack underflow | reports `STACK UNDERFLOW` and returns to the prompt |
| return stack underflow | reports `RETURN STACK UNDERFLOW` and returns to the prompt |
| dictionary exhaustion | reports `DICTIONARY FULL` |
| source nesting exhaustion | reports `SOURCE NESTING TOO DEEP` |
| source line too long | reports `SOURCE LINE TOO LONG` |
| unknown word | reports `UNKNOWN WORD` |
| arithmetic overflow | not trapped |
| invalid memory address | not generally trapped |
| using unclaimed optional word sets | not portable; usually `UNKNOWN WORD` |

## Verification

Current automated verification is in `e6502UnitTests/NovaForthTests.cs`.

Focused tests:

```sh
dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --filter Forth2012CoreSuitePasses
dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --filter Forth2012CoreExtSuitePasses
dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --filter Forth2012FileAccessSuitePasses
dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --filter Forth2012StringSuitePasses
```

Full NovaForth test class:

```sh
dotnet test e6502UnitTests/e6502UnitTests.csproj -c Release --filter FullyQualifiedName~NovaForthTests
```

The conformance claim should only be updated after the relevant upstream
Forth 2012 runner passes and the `ENVIRONMENT?` table is updated to match.
