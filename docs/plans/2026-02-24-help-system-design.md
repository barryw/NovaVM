# NovaBASIC In-App Help System

**Date:** 2026-02-24
**Status:** Design

## Overview

An integrated, searchable help system that provides documentation alongside the BASIC editor. Users invoke help via a BASIC command (`HELP`), keyboard shortcut (F1), or clickable cross-references. Documentation is displayed in a modern styled panel on the left side of the window. Code examples include "Try this!" buttons that enter examples directly into the BASIC editor.

A canonical set of Markdown documentation serves as the single source of truth, generating both the in-app help system and the PDF user guide.

## Goals

- Searchable documentation accessible without leaving the emulator
- Two tiers: conceptual Guides and quick-lookup Reference
- "Try this!" buttons on examples for frictionless experimentation
- One source of truth (Markdown) that produces both the PDF user guide and in-app help
- PDF quality matches the current LaTeX output (tcolorbox, styled listings, retro palette)
- Modern, polished help panel UI — not retro, but delightful
- Single build command produces everything: VM, ROM, docs, help content

## Non-Goals

- Tutorials tier (can be added later without architectural changes)
- Typo-tolerant/fuzzy edit-distance search (simple substring matching is sufficient for ~150 topics)
- Online/web-hosted documentation

---

## Canonical Documentation Format

### Directory Structure

```
docs/help/
  book.yaml                    # Chapter ordering manifest for PDF generation
  guides/
    welcome.md
    first-session.md
    language-fundamentals.md
    graphics-and-display.md
    sprites.md
    sound-and-music.md
    copper.md
    dma-and-blitter.md
    expansion-memory.md
    networking.md
    assembly.md
    memory-map.md
  reference/
    commands/
      cls.md
      color.md
      line.md
      sprite.md
      music.md
      dmacopy.md
      ...  (~95 files, one per command)
    functions/
      collision.md
      nrecv.md
      fre.md
      ...  (~45 files, one per function)
```

### File Format

Each file is Markdown with YAML frontmatter:

```markdown
---
title: LINE
type: command
category: Graphics
keywords: [line, draw, graphics, straight]
syntax: "LINE x1,y1,x2,y2"
see_also: [plot, rect, circle, gcolor]
---

Draws a straight line between two points using the current
graphics color set by `GCOLOR`.

## Parameters

| Parameter | Description |
|-----------|-------------|
| x1, y1    | Start point (0-319, 0-199) |
| x2, y2    | End point (0-319, 0-199) |

## Examples

```basic
MODE 2 : GCOLOR 1
LINE 0,0,319,199
```

## Notes

- Requires graphics mode (`MODE 2` or `MODE 3`)
- Color is set by the most recent `GCOLOR` call
```

Guide files use `type: guide` with longer-form prose, multiple sections, and multiple examples. Special content blocks use fenced divs:

```markdown
::: note
The VGC processes drawing commands immediately when you write to RegCmd.
:::

::: warning
Calling LINE outside of a graphics mode will have no visible effect.
:::

::: tryit
```basic
10 MODE 2 : GCOLOR 1
20 FOR I = 0 TO 319 STEP 10
30 LINE I,0,319-I,199
40 NEXT I
```
:::
```

### Categories

**Guides:** Graphics and Display, Sprites, Sound and Music, Copper, DMA and Blitter, Expansion Memory, Networking, Assembly, Language Fundamentals, Getting Started

**Reference commands by category:** Program Control, Flow Control, Variables/Data, I/O, Memory, Bit Operations, Files, Text Display, Graphics, Sprites, Sound/Music, Graphics File I/O, Expansion Memory, DMA, Blitter, Network, Interrupts, Miscellaneous

**Reference functions by category:** Numeric, String, Sprite/Graphics, Music, DMA/Blitter, Network

---

## Help Panel UI

### Invocation

- `HELP` — opens panel to browse/home view
- `HELP <keyword>` — opens panel and navigates to that topic (or shows search results if ambiguous)
- `F1` — toggles panel open/closed, returns to last viewed topic
- `Escape` (while panel focused) — closes panel
- Clicking "See also" links within a topic navigates to the linked topic

### Panel Behavior

- Appears on the left side of the window, pushing the emulator canvas right (not an overlay)
- Default width ~350px, resizable via drag handle on the right edge
- Independent of dev mode — can be open alone, with dev mode, or neither
- If dev mode is also active: Help Panel | Emulator Canvas | Sprite Editor
- Remembers last viewed topic when closed and reopened within a session
- Close via X button in top-right corner, Escape key, or F1 toggle

### Layout (Top to Bottom)

1. **Header** — "Help" title + close button (X)

2. **Search bar** — Rounded input field, placeholder "Search docs...", real-time filtering as user types

3. **Category filter tabs** — Horizontal pill buttons: All | Guides | Commands | Functions. "All" selected by default. Subtle accent highlight on active tab.

4. **Content area** — Two states:

   **Browse/search state:**
   - When panel first opens (via `HELP` with no argument or F1): shows category groups with topic lists
   - When searching: shows ranked results, each displaying title, type badge (Guide/Command/Function), and a one-line preview
   - Clicking a result opens the topic

   **Topic view state:**
   - Back arrow at top to return to search/browse
   - Rendered topic with styled typography:
     - Section headers
     - Parameter tables with alternating row shading
     - Syntax blocks in monospace with syntax highlighting
     - Note/warning/tip callout boxes (styled, not retro — clean modern cards)
     - "See also" links as clickable pills at the bottom
   - "Try this!" buttons on multi-line code examples

### Visual Style

Modern documentation aesthetic:
- Light background, clean sans-serif body font
- Monospace font for code/syntax with subtle background shading
- Accent color for interactive elements (buttons, active tab, links)
- Callout boxes with left border accent (note = blue, warning = amber, tip = green)
- Comfortable spacing and readable line lengths within the panel width
- Type badges as small colored pills (Guide = purple, Command = blue, Function = teal)

---

## Search Implementation

### Index

At startup, all Markdown files are loaded from embedded resources. Each file is parsed once into a `HelpTopic` object:

- Metadata from frontmatter (title, type, category, keywords, syntax, see_also)
- Raw Markdown body (for rendering)
- Lowercased plain-text blob (Markdown stripped, for search matching)

### Search Algorithm

Scored ranking with no external dependencies:

1. Query is lowercased and split into terms
2. Each topic scores against all terms:
   - **Title exact match:** weight 100
   - **Title prefix match:** weight 80 (e.g., "spr" matches "SPRITE")
   - **Keyword match:** weight 60
   - **Syntax match:** weight 40
   - **Body match:** weight 10, boosted by frequency
3. Results filtered by active category tab (unless "All")
4. Sorted by total score descending
5. Top 20 results displayed

For ~150 topics this is instantaneous with no optimization needed.

---

## BASIC Integration

### HELP Command (ROM)

New extended token `XTK_HELP` added to the EhBASIC ROM:

1. Add `XTK_HELP` constant and "HELP" to the keyword string table
2. Add handler address to `TAB_XTKCMD` dispatch table
3. Handler implementation:
   - If no argument: write `$01` to `RegHelp` ($A020)
   - If argument follows: parse keyword text, write characters to a buffer register area ($A021-$A030), then write `$02` to `RegHelp`
4. Reassemble ROM with `make`

### Avalonia Side

`CompositeBusDevice` intercepts writes to `$A020`:
- `$01`: raise `HelpRequested` event with no search term
- `$02`: read buffered string from `$A021-$A030`, raise `HelpRequested` event with search term

`MainWindow` handles the event, opens the help panel, and navigates to the topic or search results.

### "Try This!" Flow

1. User clicks "Try this!" on a code example
2. Check if a BASIC program is in memory (peek at BASIC program start/end pointers in zero page)
3. If program exists: show confirmation dialog — "This will replace your current program. Continue?"
4. On confirm (or if memory empty):
   - Send `NEW` to the `ScreenEditor` key queue
   - Send each line of the example via the existing `EnterBasicLine` mechanism (same path used by MCP/TCP)
5. Help panel stays open so user can read docs alongside their code
6. User types `RUN` when ready

---

## PDF Generation Pipeline

### Manifest

`docs/help/book.yaml` defines chapter ordering:

```yaml
title: NovaBASIC User Guide
author: ""
chapters:
  - title: Welcome to NovaBASIC
    sources: [guides/welcome.md]
  - title: Your First Session
    sources: [guides/first-session.md]
  - title: Language Fundamentals
    sources: [guides/language-fundamentals.md]
  - title: Graphics and Sprites
    sources: [guides/graphics-and-display.md, guides/sprites.md]
  - title: Sound and Music
    sources: [guides/sound-and-music.md]
  - title: Expansion Memory
    sources: [guides/expansion-memory.md]
  - title: Assembly and Special Chips
    sources: [guides/assembly.md]
  - title: DMA and Blitter
    sources: [guides/dma-and-blitter.md]
  - title: Networking
    sources: [guides/networking.md]
appendices:
  - title: Command Reference
    auto_generate: commands
  - title: Function Reference
    auto_generate: functions
  - title: Memory Map
    sources: [guides/memory-map.md]
  - title: Limits, Errors, and Edge Cases
    sources: [guides/limits-and-errors.md]
```

### Build Process

1. A build script reads `book.yaml`
2. Concatenates Markdown sources in order, inserting chapter/appendix headers
3. Auto-generates reference appendices from `reference/commands/*.md` and `reference/functions/*.md` (sorted alphabetically, formatted into longtable layout)
4. Runs Pandoc with:
   - `--template=docs/help/latex-template.tex` (contains all current styling: tcolorbox definitions, NovaBASIC listings language, color palette, fonts)
   - A Lua filter mapping fenced divs to LaTeX environments (`::: note` → `\begin{notebox}`, `::: tryit` → `\begin{tryitbox}`, etc.)
   - ` ```basic ` code fences → `\begin{lstlisting}[language=NovaBASIC]`
5. Output: `docs/manual/novabasic_user_guide.pdf`

### LaTeX Template

Carries all presentation from the current `novabasic_user_guide.tex`:
- Document class, geometry, fonts
- `tcolorbox` environment definitions (notebox, warningbox, tipbox, tryitbox, retrobox)
- `listings` NovaBASIC language definition with all tokens
- Color palette (NovaCream, NovaNavy, NovaBlue, etc.)
- Chapter/section styling
- Header/footer formatting

---

## Unified Build

Single `dotnet build` produces everything. Build fails if any tool is missing.

### MSBuild Integration

The `e6502.Avalonia.csproj` gets custom build targets:

```xml
<!-- Assemble EhBASIC ROM -->
<Target Name="AssembleRom" BeforeTargets="PreBuildEvent">
  <Exec Command="make -C ehbasic" />
</Target>

<!-- Generate PDF user guide -->
<Target Name="GeneratePdf" BeforeTargets="PreBuildEvent">
  <Exec Command="docs/help/build-pdf.sh" />
</Target>

<!-- Embed help content as resources -->
<ItemGroup>
  <EmbeddedResource Include="docs/help/**/*.md" />
</ItemGroup>
```

### Tool Requirements

Build validates at the start:
- `dotnet` (obviously)
- `ca65` / `ld65` (6502 assembler/linker for ROM)
- `pandoc` (Markdown → LaTeX → PDF)
- `pdflatex` or `latexmk` (PDF rendering)

If any tool is missing, the build fails with a clear message naming the missing tool and how to install it.

---

## Migration Plan

One-time conversion of existing LaTeX to Markdown:

1. Write a conversion script that extracts content from each `.tex` chapter file
2. Strip LaTeX commands, convert to Markdown equivalents
3. Convert `\begin{notebox}` etc. to `::: note` fenced divs
4. Convert `\begin{lstlisting}` to fenced code blocks
5. Split the command reference appendix into individual `reference/commands/*.md` files
6. Split the function reference into individual `reference/functions/*.md` files
7. Hand-edit for quality — verify formatting, add frontmatter metadata, fix any conversion artifacts
8. Build PDF from new source, diff against current PDF to verify no content loss
9. Remove old `docs/manual/chapters/` LaTeX files once validated

---

## Implementation Components Summary

| Component | Location | Work |
|-----------|----------|------|
| Markdown docs | `docs/help/` | Migrate from LaTeX, add frontmatter |
| Help panel UI | `e6502.Avalonia/Help/` | New `HelpPanel.cs` UserControl |
| Panel management | `e6502.Avalonia/MainWindow.axaml.cs` | Open/close logic, layout adjustment |
| Search engine | `e6502.Avalonia/Help/HelpIndex.cs` | Parse, index, score, rank |
| Markdown renderer | `e6502.Avalonia/Help/MarkdownRenderer.cs` | Markdown → styled Avalonia controls |
| "Try this!" handler | `e6502.Avalonia/Help/TryThisHandler.cs` | Memory check, confirm dialog, line entry |
| HELP token | `ehbasic/basic.asm` | New XTK_HELP, handler, register writes |
| VGC register | `e6502.Avalonia/Hardware/CompositeBusDevice.cs` | Intercept $A020 writes |
| PDF pipeline | `docs/help/build-pdf.sh`, `latex-template.tex`, Lua filter | Pandoc + template |
| Unified build | `e6502.Avalonia.csproj` | MSBuild targets for ROM + docs |
| LaTeX migration | One-time script | Convert existing .tex → .md |
