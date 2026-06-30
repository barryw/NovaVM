# Building & Testing NovaVM on Linux

> Orientation note from the first build/test of `e6502.sln` on a headless Linux
> workspace (.NET 10). CI runs on Windows, so the Linux native-toolchain path
> was previously undocumented. This captures a known-good recipe and the exact
> dependencies each part of the tree needs.

## TL;DR

```bash
export DOTNET_ROOT="$HOME/.dotnet"
export PATH="$HOME/.dotnet:$PATH"
export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1   # no libicu on minimal images

# Core library + managed unit tests (no native 6502 toolchain required):
dotnet build -c Release -p:SkipRomBuild=true e6502UnitTests/e6502UnitTests.csproj
dotnet test  -c Release -p:SkipRomBuild=true --no-build e6502UnitTests/e6502UnitTests.csproj
```

## Dependency map

| Component | Extra dependency | Notes |
|-----------|------------------|-------|
| `e6502` (core CPU/bus/opcodes) | none beyond the .NET 10 SDK | builds clean |
| `e6502UnitTests` (managed tests) | none, **with `-p:SkipRomBuild=true`** | references Avalonia (headless) |
| `e6502.Avalonia` ROM staging | `cc65` (`ca65`/`ld65`) + `make` | `AssembleRom` MSBuild target; gate with `SkipRomBuild` |
| `e6502.Browser` (Blazor wasm) | `wasm-tools` workload | `dotnet workload install wasm-tools` (no root) |
| `e6502.FPGA` / RTL sim | `verilator` | hardware simulation only |
| any `dotnet` invocation | `libicu` **or** invariant-globalization flag | else the runtime FailFast's on startup |

## What `SkipRomBuild=true` trades off

The `AssembleRom` target in `e6502.Avalonia.csproj` shells out to `make`/`ca65`
to assemble the EhBASIC ROM and the graphics/system/editor/sound module images,
then stages the `.bin`s into `Resources/`. Skipping it lets everything build
without `cc65`, but any test that **runs an on-device program** (NovaBASIC,
NovaLogo, NovaForth, graphics/sound modules, the screen editor) will fail because
its ROM/module image is absent. Pure CPU/opcode/storage/type tests are unaffected.

## Baseline test result (2026-06-30, SkipRomBuild=true)

```
Total tests: 1802 | Passed: 1321 | Failed: 462 | Skipped: 19
```

All 462 failures are environmental (missing assembled ROM/module images), **not**
code regressions — they cluster in `NovaLogoTests`, `GraphicsModuleTests`,
`NovaForthTests`, `BasicRegressionTests`, `EhBasicTokenizationTests`, `Editor*`,
`TurtleModuleTests`, and `SystemModuleTests`. To get a fully green suite on Linux,
install `cc65` + `make` (and `verilator` for the FPGA projects) in the image, then
drop `SkipRomBuild`.

## Suggested follow-ups

1. Add a `cc65`, `make`, `verilator`, `libicu`-provisioned devcontainer / CI image so
   the full Linux suite is green and reproducible (mirrors the Windows CI lane).
2. Add a Linux CI lane running the command in the TL;DR as a fast managed-test gate.
