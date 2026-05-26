# NovaBASIC User Guide

This folder owns the NovaBASIC User Guide book build and PDF output. The guide
content is assembled from the shared help Markdown under `docs/help`, then
rendered through Pandoc/LaTeX using the book manifest, Lua filter, and LaTeX
header in this directory.

## Build

```sh
cd docs/books/basic-user-guide
make pdf
```

Direct command:

```sh
./build-pdf.sh
```

## Structure

- `book.yaml`: book order and appendix generation rules.
- `build-pdf.sh`: assembles the guide and writes `novabasic_user_guide.pdf`.
- `latex-header.tex`: book-specific LaTeX styling injected into Pandoc output.
- `nova-filter.lua`: Pandoc filter for NovaBASIC help markup.
- `cover.tex` and `copyright.tex`: retained book front matter assets.
- `../../help/guides/`: shared guide chapters.
- `../../help/reference/`: shared command and function reference source.

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
