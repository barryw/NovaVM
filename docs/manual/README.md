# NovaBASIC User Guide (LaTeX)

This folder contains a retro-styled NovaBASIC manual with a chapter-based layout.

## Build

```bash
cd docs/manual
make pdf
```

Direct command:

```bash
latexmk -pdf novabasic_user_guide.tex
```

## Structure

- `novabasic_user_guide.tex`: preamble, style, title page, chapter includes.
- `chapters/01-welcome.tex` to `chapters/07-assembly-and-special-chips.tex`: main guide flow.
- `chapters/appendix-command-reference.tex`: syntax and token index.
- `chapters/appendix-limits-and-errors.tex`: limits, status codes, and edge-case behavior.

## Writing Flow

The guide is intentionally progressive:

1. Friendly introduction and first session.
2. Core language patterns.
3. Graphics/sprites and sound.
4. Expanded memory workflows.
5. Assembly and future special-chip integration.

## Style Direction

- 1980s home-computer handbook tone.
- Cream-paper background with blue accent bars.
- Clear, practical examples in monospace command blocks.

## Runtime Timing Notes

The user guide now documents VM clock controls:

- `NOVA_CPU_HZ` (default `12000000`)
- `NOVA_TURBO` (`1|true|yes|on` for unthrottled mode)
- `NOVA_TIMING_LOG` (timing telemetry)
