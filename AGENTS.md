# Repository Guidelines

## Project Structure & Module Organization
This repository is a multi-project .NET solution (`e6502.sln`) centered on a 6502 emulator.

- `e6502/`: core CPU and opcode implementation (shared library).
- `e6502.Avalonia/`: desktop UI, rendering, virtual hardware, and TCP bridge.
- `e6502.CLI/`: terminal-hosted EhBASIC runner.
- `e6502.MCP/`: MCP server/tools for external control.
- `e6502Debugger/`: WinForms debugger project.
- `e6502UnitTests/`: MSTest suite plus binary fixtures under `Resources/`.
- `ehbasic/`: EhBASIC source and `Makefile` (produces `basic.bin`).
- `docs/plans/`: architecture and implementation planning docs.

## Build, Test, and Development Commands
- `dotnet restore e6502.sln`: restore NuGet packages.
- `dotnet build e6502.sln -c Release`: compile all projects.
- `dotnet test e6502.sln -c Release --no-build`: run all unit tests.
- `dotnet run --project e6502.Avalonia`: launch the Avalonia app.
- `dotnet run --project e6502.CLI`: run CLI EhBASIC host.
- `make -C ehbasic`: build `ehbasic/basic.bin` manually (requires `ca65` and `ld65` on `PATH`).

Note: `e6502.Avalonia` triggers `make` for EhBASIC during build, so missing cc65 tools will fail that project build.

## Coding Style & Naming Conventions
Use C# conventions already present in the codebase:

- 4-space indentation, braces on new lines.
- `PascalCase` for types/methods/properties; `camelCase` for locals/parameters.
- Private fields use `_camelCase` (for example, `_opCodeTable`).
- Keep nullable reference types respected (`<Nullable>enable</Nullable>` in all active projects).

## Testing Guidelines
Tests use MSTest (`[TestClass]`, `[TestMethod]`) in `e6502UnitTests/`.

- Name test classes with `*Tests` suffix (for example, `AvaloniaVgcTests`).
- Keep deterministic binary-based CPU tests in `e6502UnitTests/Resources/`.
- Run `dotnet test e6502.sln -c Release` before opening a PR.
- Coverage tooling (`coverlet.collector`) is installed; no enforced threshold is configured, so prioritize coverage for CPU execution, opcodes, and virtual hardware paths.

## Commit & Pull Request Guidelines
Recent history follows Conventional Commit style (`feat:`, `fix:`, `docs:`, `chore:`). Keep subject lines imperative and scoped.

For pull requests:
- Include a clear summary of behavior changes and touched projects.
- Link related issues/tasks.
- Include UI screenshots for `e6502.Avalonia` changes.
- Confirm build and test commands pass locally (or explain exceptions).

These rules apply to every task in this project unless explicitly overridden.
Bias: caution over speed on non-trivial work.

## Rule 1 — Think Before Coding
State assumptions explicitly. Ask rather than guess.
Push back when a simpler approach exists. Stop when confused.

## Rule 2 — Simplicity First
Minimum code that solves the problem. Nothing speculative.
No abstractions for single-use code.

## Rule 3 — Surgical Changes
Touch only what you must. Don't improve adjacent code.
Match existing style. Don't refactor what isn't broken.

## Rule 4 — Goal-Driven Execution
Define success criteria. Loop until verified.
Strong success criteria let Claude loop independently.

## Rule 5 — Use the model only for judgment calls
Use for: classification, drafting, summarization, extraction.
Do NOT use for: routing, retries, deterministic transforms.
If code can answer, code answers.

## Rule 6 — Token budgets are not advisory
Per-task: 4,000 tokens. Per-session: 30,000 tokens.
If approaching budget, summarize and start fresh.
Surface the breach. Do not silently overrun.

## Rule 7 — Surface conflicts, don't average them
If two patterns contradict, pick one (more recent / more tested).
Explain why. Flag the other for cleanup.

## Rule 8 — Read before you write
Before adding code, read exports, immediate callers, shared utilities.
If unsure why existing code is structured a certain way, ask.

## Rule 9 — Tests verify intent, not just behavior
Tests must encode WHY behavior matters, not just WHAT it does.
A test that can't fail when business logic changes is wrong.

## Rule 10 — Checkpoint after every significant step
Summarize what was done, what's verified, what's left.
Don't continue from a state you can't describe back.

## Rule 11 — Match the codebase's conventions, even if you disagree
Conformance > taste inside the codebase.
If you think a convention is harmful, surface it. Don't fork silently.

## Rule 12 — Fail loud
"Completed" is wrong if anything was skipped silently.
"Tests pass" is wrong if any were skipped.
Default to surfacing uncertainty, not hiding it.
