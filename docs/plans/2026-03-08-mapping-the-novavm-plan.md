# Mapping the NovaVM — Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Create a comprehensive address-by-address reference document for the e6502 virtual computer, styled as a companion to the NovaBASIC User Guide.

**Architecture:** LaTeX book class document with custom commands for memory-mapped register entries, bitfield diagrams, and command tables. 12 chapters (sequential $0000-$FFFF) plus 5 appendices. Reuses the User Guide's retro 1980s visual identity.

**Tech Stack:** LaTeX (pdflatex), tcolorbox, listings, tikz, imakeidx, booktabs, longtable

---

### Task 1: Create Directory Structure and Master Document

**Files:**
- Create: `docs/mapping/mapping_the_novavm.tex`
- Create: `docs/mapping/Makefile`

**Step 1: Create the directory**

Run: `mkdir -p docs/mapping/chapters`

**Step 2: Create the master LaTeX document**

Create `docs/mapping/mapping_the_novavm.tex` with the full preamble. This is the largest single file — it contains the document class, all package imports, color definitions, custom commands, and chapter includes.

```latex
% =============================================================================
% Mapping the NovaVM — A Complete Address-by-Address Reference
% For the e6502 Virtual Computer
% =============================================================================
\documentclass[11pt,a4paper]{book}

% ── Encoding and fonts ───────────────────────────────────────────────────────
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{lmodern}
\usepackage{microtype}
\usepackage{amsmath}

% ── Page geometry ────────────────────────────────────────────────────────────
\usepackage[a4paper, top=2.5cm, bottom=2.5cm, left=3cm, right=2.5cm,
            headheight=24pt]{geometry}

% ── Color palette (matches User Guide) ──────────────────────────────────────
\usepackage{xcolor}
\definecolor{retroblue}   {RGB}{28,  63,  168}
\definecolor{retroink}    {RGB}{29,  29,  29}
\definecolor{retrocream}  {RGB}{246, 242, 232}
\definecolor{retropale}   {RGB}{233, 238, 249}
\definecolor{retrogray}   {RGB}{107, 114, 128}
\definecolor{retrogreen}  {RGB}{34,  139, 34}
\definecolor{retroorange} {RGB}{202, 139, 66}
\definecolor{retrocyan}   {RGB}{0,   139, 139}

\pagecolor{retrocream}
\color{retroink}

% ── TikZ ─────────────────────────────────────────────────────────────────────
\usepackage{tikz}
\usetikzlibrary{calc,shapes,positioning}

% ── Tables ───────────────────────────────────────────────────────────────────
\usepackage{longtable}
\usepackage{array}
\usepackage{booktabs}
\usepackage{tabularx}

% ── Headers & footers ───────────────────────────────────────────────────────
\usepackage{fancyhdr}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[LE]{\small\color{retrogray}\leftmark}
\fancyhead[RO]{\small\color{retrogray}\rightmark}
\fancyfoot[C]{\small\color{retrogray}\thepage}
\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\footrulewidth}{0pt}

% ── Chapter & section titles ────────────────────────────────────────────────
\usepackage{titlesec}
\usepackage{titletoc}

\titleformat{\chapter}[display]
  {\normalfont\Huge\bfseries\color{retroblue}}
  {%
    \normalsize\raggedleft
    {\large\scshape\color{retrogray}\chaptername\quad}%
    {\fontsize{56}{56}\selectfont\color{retroblue!20!retrocream}\thechapter}%
  }
  {0.3ex}
  {%
    {\normalfont\color{retroblue}\rule{\linewidth}{1.2pt}}\\[0.4ex]%
    \raggedright\Huge\bfseries\color{retroblue}%
  }
  [\vspace{0.3ex}{\color{retroblue!35!retrocream}\rule{\linewidth}{0.4pt}}]

\titlespacing*{\chapter}{0pt}{0pt}{24pt}

\titleformat{\section}
  {\normalfont\large\bfseries\color{retroblue}}
  {\thesection}
  {0.7em}
  {}

\titleformat{\subsection}
  {\normalfont\normalsize\bfseries\color{retroblue!80!retroink}}
  {\thesubsection}
  {0.6em}
  {}

% ── Epigraphs ────────────────────────────────────────────────────────────────
\usepackage{epigraph}
\setlength{\epigraphwidth}{0.75\textwidth}
\renewcommand{\epigraphflush}{flushleft}

% ── Code listings ────────────────────────────────────────────────────────────
\usepackage{listings}

\lstdefinelanguage{NovaBASIC}{%
  morekeywords=[1]{%
    END, FOR, NEXT, DATA, INPUT, DIM, READ, LET, GOTO, RUN, IF, RESTORE,
    GOSUB, RETURN, REM, STOP, ON, PRINT, CONT, LIST, NEW, THEN, ELSE,
    TO, STEP, NOT, AND, OR, EOR, NULL, LOAD, SAVE, DEF, POKE, DOKE, DO, LOOP,
    CLEAR, GET, SWAP, CLS, COLOR, LOCATE, PLOT, UNPLOT, LINE, CIRCLE,
    RECT, FILL, PAINT, MODE, GCLS, GCOLOR, SPRITE, SPRITEDATA, COPPER, SOUND,
    VOLUME, INSTRUMENT, VSYNC, DIR, DEL, XMEM, XBANK, XPOKE, STASH, FETCH,
    XFREE, XRESET, XALLOC, XDIR, XDEL, XMAP, XUNMAP, DEC, INC, CALL, WIDTH,
    BITSET, BITCLR, IRQ, NMI, UNTIL, WHILE, OFF, SPRITESHAPE, SPRITECOLOR,
    MUSIC, WAIT, SIDPLAY, SIDSTOP, GSAVE, GLOAD, RETIRQ, RETNMI,
    NOPEN, NCLOSE, NLISTEN, NACCEPT, NSEND, DMACOPY, DMAFILL,
    BLITCOPY, BLITFILL, RESET%
  },
  morekeywords=[2]{%
    SGN, INT, ABS, USR, FRE, POS, SQR, RND, LOG, EXP, COS, SIN, TAN, ATN, PEEK,
    DEEK, SADD, LEN, STR\$, VAL, ASC, UCASE\$, LCASE\$, CHR\$, HEX\$, BIN\$, BITTST,
    MAX, MIN, PI, TWOPI, VARPTR, LEFT\$, RIGHT\$, MID\$, SPRITEX, SPRITEY,
    COLLISION, BUMPED, XPEEK, TAB, SPC, FN, PLAYING, MNOTE,
    NRECV\$, NSTATUS, NREADY, NLEN, DMASTATUS, DMAERR, DMACOUNT,
    BLITSTATUS, BLITERR, BLITCOUNT%
  },
  morecomment=[l]{REM},
  sensitive=false,
  morestring=[b]"%
}

\lstdefinestyle{basiclisting}{%
  language=NovaBASIC,
  basicstyle=\ttfamily\small,
  backgroundcolor=\color{retropale},
  frame=single,
  framerule=0pt,
  framesep=4pt,
  xleftmargin=6pt,
  xrightmargin=6pt,
  keywordstyle=[1]\color{retroblue}\bfseries,
  keywordstyle=[2]\color{retrocyan},
  commentstyle=\color{retrogray}\itshape,
  stringstyle=\color{retrogreen},
  showstringspaces=false,
  tabsize=2,
  breaklines=true,
  numbers=none,
  captionpos=b%
}

\lstset{style=basiclisting}

% ── Callout boxes (tcolorbox) ────────────────────────────────────────────────
\usepackage[skins,breakable]{tcolorbox}

\newtcolorbox{notebox}[1][]{%
  colback=retrocyan!8!retrocream,
  colframe=retrocyan!70!black,
  coltitle=retrocyan!70!black,
  fonttitle=\bfseries\small,
  title=Note,
  boxrule=0.8pt, arc=2pt,
  left=6pt, right=6pt, top=4pt, bottom=4pt,
  breakable, #1%
}

\newtcolorbox{warningbox}[1][]{%
  colback=retroorange!12!retrocream,
  colframe=retroorange,
  coltitle=retroorange!80!black,
  fonttitle=\bfseries\small,
  title=Warning,
  boxrule=0.8pt, arc=2pt,
  left=6pt, right=6pt, top=4pt, bottom=4pt,
  breakable, #1%
}

\newtcolorbox{tipbox}[1][]{%
  colback=retrogreen!10!retrocream,
  colframe=retrogreen!70!black,
  coltitle=retrogreen!70!black,
  fonttitle=\bfseries\small,
  title=Tip,
  boxrule=0.8pt, arc=2pt,
  left=6pt, right=6pt, top=4pt, bottom=4pt,
  breakable, #1%
}

% ── Hyperlinks ───────────────────────────────────────────────────────────────
\usepackage[colorlinks=true, linkcolor=retroblue, urlcolor=retroblue,
            citecolor=retroblue, bookmarks=true]{hyperref}

% ── Index ────────────────────────────────────────────────────────────────────
\usepackage{imakeidx}
\makeindex[title=Address Index, intoc,
           options=-s docs/mapping/addrindex.ist]

% ══════════════════════════════════════════════════════════════════════════════
% CUSTOM COMMANDS FOR MEMORY MAPPING
% ══════════════════════════════════════════════════════════════════════════════

% ── Memory entry header ─────────────────────────────────────────────────────
% Usage: \mementry{A001}{RegBgCol}{R/W}{0}
%   addr  = hex address (no $ prefix)
%   name  = register name
%   rw    = R, W, or R/W
%   def   = default value
\newcommand{\mementry}[4]{%
  \par\vspace{1.2em}%
  \noindent
  \begin{tikzpicture}[baseline=(addr.base)]
    \node[fill=retroblue, text=white, font=\ttfamily\bfseries\large,
          inner sep=4pt, minimum width=5.5em, anchor=west]
      (addr) {\$#1};
    \node[anchor=west, font=\sffamily\bfseries\large, text=retroink,
          xshift=6pt] at (addr.east)
      {#2};
    \node[anchor=east, font=\sffamily\small, text=retrogray]
      at ([xshift=-2pt]current page text area.east |- addr.east)
      {#3\enspace|\enspace Default: #4};
  \end{tikzpicture}%
  \par\nopagebreak\vspace{0.3em}%
  \label{addr:#1}%
  \index{\$#1@{\texttt{\$#1}} --- #2}%
}

% ── Memory entry for ranges ─────────────────────────────────────────────────
% Usage: \memrange{AA00}{B1CF}{Character RAM}{R/W}{---}
\newcommand{\memrange}[5]{%
  \par\vspace{1.2em}%
  \noindent
  \begin{tikzpicture}[baseline=(addr.base)]
    \node[fill=retroblue, text=white, font=\ttfamily\bfseries\large,
          inner sep=4pt, anchor=west]
      (addr) {\$#1--\$#2};
    \node[anchor=west, font=\sffamily\bfseries\large, text=retroink,
          xshift=6pt] at (addr.east)
      {#3};
    \node[anchor=east, font=\sffamily\small, text=retrogray]
      at ([xshift=-2pt]current page text area.east |- addr.east)
      {#4\enspace|\enspace Default: #5};
  \end{tikzpicture}%
  \par\nopagebreak\vspace{0.3em}%
  \label{addr:#1}%
  \index{\$#1@{\texttt{\$#1--\$#2}} --- #3}%
}

% ── Command sub-entry ───────────────────────────────────────────────────────
% Usage: \cmdentry{01}{CmdPlot}{Plot a pixel}
\newcommand{\cmdentry}[3]{%
  \par\vspace{0.8em}%
  \noindent
  \begin{tikzpicture}[baseline=(code.base)]
    \node[fill=retrocyan!20!retrocream, text=retroblue,
          font=\ttfamily\bfseries, inner sep=3pt, anchor=west]
      (code) {\$#1};
    \node[anchor=west, font=\sffamily\bfseries, text=retroink,
          xshift=4pt] at (code.east)
      {#2};
    \node[anchor=west, font=\sffamily\small\itshape, text=retrogray,
          xshift=8pt] at (code.east -| {$(code.east)+(7em,0)$})
      {#3};
  \end{tikzpicture}%
  \par\nopagebreak\vspace{0.2em}%
}

% ── 8-bit register bitfield ─────────────────────────────────────────────────
% Usage: \bitfield{MSB name}{b6}{b5}{b4}{b3}{b2}{b1}{LSB name}
\newcommand{\bitfield}[8]{%
  \par\vspace{0.5em}%
  \noindent\centering
  \begin{tikzpicture}[
    cell/.style={draw, minimum width=3.2em, minimum height=1.8em,
                 font=\ttfamily\small, anchor=west},
    bitlabel/.style={font=\tiny\sffamily, text=retrogray, anchor=south}
  ]
    \foreach \i/\val in {0/#8, 1/#7, 2/#6, 3/#5, 4/#4, 5/#3, 6/#2, 7/#1} {
      \node[cell] (b\i) at (\i*3.2em, 0) {\val};
      \pgfmathtruncatemacro{\bitnum}{7-\i}
      \node[bitlabel] at (b\i.north) {\bitnum};
    }
  \end{tikzpicture}%
  \par\vspace{0.5em}%
}

% ── Hex address cross-reference ─────────────────────────────────────────────
% Usage: see \addr{A001}
\newcommand{\addr}[1]{\hyperref[addr:#1]{\texttt{\$#1}}}

% ══════════════════════════════════════════════════════════════════════════════
% DOCUMENT
% ══════════════════════════════════════════════════════════════════════════════
\begin{document}

% ── Front matter ─────────────────────────────────────────────────────────────
\frontmatter
\input{cover}
\input{copyright}
\tableofcontents

% ── Main chapters ────────────────────────────────────────────────────────────
\mainmatter
\input{chapters/ch01-zero-page}
\input{chapters/ch02-stack}
\input{chapters/ch03-vector-table}
\input{chapters/ch04-basic-program-space}
\input{chapters/ch05-vgc}
\input{chapters/ch06-nic}
\input{chapters/ch07-wts}
\input{chapters/ch08-screen-memory}
\input{chapters/ch09-file-io}
\input{chapters/ch10-expansion-memory}
\input{chapters/ch11-rom}
\input{chapters/ch12-sid}

% ── Appendices ───────────────────────────────────────────────────────────────
\appendix
\input{chapters/appendix-a-vgc-offbus}
\input{chapters/appendix-b-xram}
\input{chapters/appendix-c-wts-samples}
\input{chapters/appendix-d-color-table}
\input{chapters/appendix-e-ascii-table}

% ── Back matter ──────────────────────────────────────────────────────────────
\backmatter
\printindex

\end{document}
```

**Step 3: Create the Makefile**

Create `docs/mapping/Makefile`:

```makefile
GUIDE = mapping_the_novavm

all: pdf

pdf:
	latexmk -pdf -interaction=nonstopmode $(GUIDE).tex

clean:
	latexmk -C

.PHONY: all pdf clean
```

**Step 4: Create index style file**

Create `docs/mapping/addrindex.ist`:

```
headings_flag 1
heading_prefix "\\textbf{\\large "
heading_suffix "}\\nopagebreak\n"
delim_0 " \\dotfill\\ "
delim_1 " \\dotfill\\ "
delim_2 " \\dotfill\\ "
```

**Step 5: Commit**

```bash
git add docs/mapping/mapping_the_novavm.tex docs/mapping/Makefile docs/mapping/addrindex.ist
git commit -m "feat: add master document and build for Mapping the NovaVM"
```

---

### Task 2: Cover Page and Copyright Page

**Files:**
- Create: `docs/mapping/cover.tex`
- Create: `docs/mapping/copyright.tex`

**Step 1: Create the cover page**

Create `docs/mapping/cover.tex` — adapt the User Guide cover with updated title and feature line. Same TikZ layout, same colors, same retro aesthetic:

```latex
% =============================================================================
% Mapping the NovaVM — Cover Page
% =============================================================================
\begin{titlepage}
\begin{tikzpicture}[remember picture, overlay]

  \fill[retroblue]
    (current page.south west) rectangle (current page.north east);

  \fill[retroblue!70!white, opacity=0.35]
    ([xshift=-60pt, yshift=120pt]current page.south west)
    -- ([xshift=260pt, yshift=120pt]current page.south west)
    -- ([xshift=260pt]current page.north west)
    -- ([xshift=-60pt]current page.north west)
    -- cycle;

  \fill[retroblue!50!white, opacity=0.18]
    ([xshift=180pt, yshift=120pt]current page.south west)
    -- ([xshift=340pt, yshift=120pt]current page.south west)
    -- ([xshift=340pt]current page.north west)
    -- ([xshift=180pt]current page.north west)
    -- cycle;

  \begin{scope}[opacity=0.06]
    \foreach \gx in {0,12,...,180}{
      \foreach \gy in {0,12,...,220}{
        \fill[white]
          ([xshift=\gx pt, yshift=-\gy pt]current page.north east)
          circle (1.2pt);
      }
    }
  \end{scope}

  \fill[retrocyan, opacity=0.75]
    ([xshift=52pt, yshift=-52pt]current page.north west)
    rectangle ++(18pt, 18pt);
  \fill[retrocyan, opacity=0.50]
    ([xshift=80pt, yshift=-52pt]current page.north west)
    rectangle ++(10pt, 18pt);
  \fill[white, opacity=0.30]
    ([xshift=100pt, yshift=-52pt]current page.north west)
    rectangle ++(5pt, 18pt);

  \node[
    anchor=north east, xshift=-44pt, yshift=-62pt,
    text=white, opacity=0.12, font=\ttfamily\large, align=left
  ] at (current page.north east) {%
    \$A000 VGC\\
    \$D400 SID\\
    \$BA00 XMC%
  };

  \node[
    anchor=west, xshift=52pt, yshift=72pt,
    text=white,
    font=\fontsize{42}{46}\selectfont\bfseries\sffamily
  ] at (current page.west) {MAPPING THE};

  \node[
    anchor=west, xshift=52pt, yshift=28pt,
    text=white,
    font=\fontsize{56}{60}\selectfont\bfseries\sffamily
  ] at (current page.west) {NOVAVM};

  \node[
    anchor=west, xshift=52pt, yshift=-16pt,
    text=retrocyan,
    font=\fontsize{14}{18}\selectfont\itshape\sffamily
  ] at (current page.west) {A Complete Address-by-Address Reference};

  \node[
    anchor=west, xshift=52pt, yshift=-46pt,
    text=white, opacity=0.55,
    font=\fontsize{10}{14}\selectfont\sffamily
  ] at (current page.west)
    {For the e6502 Virtual Computer\enspace|\enspace 2026 Edition};

  \draw[white, opacity=0.45, line width=1.0pt]
    ([xshift=52pt,  yshift=62pt]current page.south west)
    -- ([xshift=-52pt, yshift=62pt]current page.south east);

  \node[
    anchor=south, yshift=22pt,
    text=white, opacity=0.80,
    font=\fontsize{9}{12}\selectfont\sffamily\bfseries
  ] at (current page.south) {%
    64KB Address Space\enspace|\enspace
    12 Hardware Controllers\enspace|\enspace
    512KB Expansion\enspace|\enspace
    14-Voice Sound%
  };

\end{tikzpicture}
\end{titlepage}
```

**Step 2: Create the copyright page**

Create `docs/mapping/copyright.tex`:

```latex
% =============================================================================
% Mapping the NovaVM — Copyright Page
% =============================================================================
\thispagestyle{empty}
\vspace*{\fill}
\begin{flushleft}

{\large\bfseries Mapping the NovaVM}\\[0.4em]
{\normalsize A Complete Address-by-Address Reference --- 2026 Edition}

\bigskip
\noindent\rule{\textwidth}{0.4pt}
\bigskip

{\small
Copyright \textcopyright\ 2026 Barry Walker. All Rights Reserved.

\medskip
No part of this publication may be reproduced, distributed, or transmitted
in any form or by any means, including photocopying, recording, or other
electronic or mechanical methods, without the prior written permission of
the author, except in the case of brief quotations embodied in critical
reviews and certain other non-commercial uses permitted by copyright law.

\medskip
e6502 is an open source project.

\medskip
This reference covers the NovaVM hardware as implemented in the e6502
emulator. EhBASIC internals are derived from Enhanced BASIC 2.22p5 by
Lee Davison, used under the terms of its original licence.
}

\bigskip
\noindent\rule{\textwidth}{0.4pt}

\vspace{2em}
{\footnotesize\color{retrogray}
Typeset with \LaTeX\ and the \texttt{lmodern} font family.\\
Document class: \texttt{book}, 11\,pt, A4.\\
Build toolchain: \texttt{latexmk} with \texttt{pdflatex}.
}

\end{flushleft}
\vspace*{\fill}
```

**Step 3: Commit**

```bash
git add docs/mapping/cover.tex docs/mapping/copyright.tex
git commit -m "feat: add cover and copyright pages for Mapping the NovaVM"
```

---

### Task 3: Chapter 1 — Zero Page ($0000–$00FF)

**Files:**
- Create: `docs/mapping/chapters/ch01-zero-page.tex`

**Reference files:**
- `ehbasic/basic.asm` (lines 44–315 for zero page symbol definitions)
- `ehbasic/tokens.json` (token table)
- `e6502.Avalonia/Hardware/VgcConstants.cs` (ZpSmeml/h, ZpSvarl/h)

**Step 1: Write Chapter 1**

This chapter documents every byte from $00 to $FF. Structure:

1. Chapter introduction — what the zero page is, why it matters for 6502 performance
2. EhBASIC warm start vectors ($00–$02)
3. USR function jump ($0A–$0C)
4. Terminal control ($0D–$10)
5. Expression temporaries and interpreter flags ($11–$78)
6. **Key memory pointers ($79–$86)** — most important section, each gets a full \mementry with DEEK examples
7. Current line and DATA pointers ($87–$9E)
8. Floating point accumulators FAC1/FAC2 ($AC–$B9)
9. BASIC byte-fetch subroutine ($BC–$C4)
10. PRNG state ($D8–$DB)
11. NMI/IRQ handler setup ($DC–$E1)
12. Free regions

Every entry uses `\mementry`. Key pointers like Smeml/Smemh get full prose and BASIC examples:

```latex
\mementry{0079}{Smeml}{R/W}{---}
\textbf{Start of BASIC Program Text (Low Byte)}

The low byte of the 16-bit pointer to the first byte of BASIC program
text in memory.  Together with \addr{007A}, this defines where the
tokenized program begins.

At cold start, EhBASIC sets this to \texttt{\$80}\,---\,the first usable
byte after the zero-page workspace.  The NovaBASIC ROM overrides this
during initialisation, pointing it into the BASIC program space at
\addr{0280}.

\begin{lstlisting}
10 REM read the start-of-program pointer
20 PRINT "PROGRAM START: $"; HEX$(DEEK($79))
\end{lstlisting}

\begin{notebox}
The BASIC \texttt{NEW} command does not move this pointer.  It clears
the program by writing \texttt{\$00\,\$00} at the address Smeml/Smemh
points to, which the interpreter reads as ``no more lines.''
\end{notebox}
```

Group related bytes into logical sections. Bytes that share multiple aliases (EhBASIC reuses ZP locations for different purposes in different code paths) should note this.

**Step 2: Verify it compiles**

Run: `cd docs/mapping && latexmk -pdf -interaction=nonstopmode mapping_the_novavm.tex`

Note: This will produce errors for missing chapter files. Create stub files for chapters 2–12 and all appendices first:

```bash
for ch in ch02-stack ch03-vector-table ch04-basic-program-space ch05-vgc ch06-nic ch07-wts ch08-screen-memory ch09-file-io ch10-expansion-memory ch11-rom ch12-sid appendix-a-vgc-offbus appendix-b-xram appendix-c-wts-samples appendix-d-color-table appendix-e-ascii-table; do
  echo "% Placeholder" > docs/mapping/chapters/${ch}.tex
done
```

Then build. Expected: PDF generates with Chapter 1 content and placeholder chapters.

**Step 3: Commit**

```bash
git add docs/mapping/chapters/
git commit -m "feat: add Chapter 1 (Zero Page) and chapter stubs"
```

---

### Task 4: Chapter 2 — The Stack ($0100–$01FF)

**Files:**
- Modify: `docs/mapping/chapters/ch02-stack.tex`

**Step 1: Write Chapter 2**

Shorter chapter (~5 pages). Cover:

1. Introduction — 6502 stack mechanics (grows downward from $01FF, SP is 8-bit offset into page 1)
2. `\memrange{0100}{01FF}{Hardware Stack}{R/W}{---}`
3. Stack frame layouts:
   - JSR/RTS: 2 bytes (return address minus 1, high byte pushed first)
   - Interrupt (BRK/IRQ/NMI): 3 bytes (PCH, PCL, status register)
   - EhBASIC FOR/NEXT frame: token + limit + step + line + pointer (18 bytes)
   - EhBASIC GOSUB frame: token + line + pointer (5 bytes)
4. Stack pointer initialisation (monitor sets SP=$FF at reset)
5. BASIC examples showing stack inspection with PEEK

```latex
\begin{warningbox}
The stack is only 256 bytes.  Deeply nested \texttt{GOSUB} calls or
\texttt{FOR/NEXT} loops can overflow it, causing unpredictable crashes.
EhBASIC does not check for stack overflow.
\end{warningbox}
```

**Step 2: Build and verify**

Run: `cd docs/mapping && latexmk -pdf mapping_the_novavm.tex`

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch02-stack.tex
git commit -m "feat: add Chapter 2 (Stack)"
```

---

### Task 5: Chapter 3 — The Vector Table ($0200–$027F)

**Files:**
- Modify: `docs/mapping/chapters/ch03-vector-table.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (InitVectorTable method)
- `ehbasic/min_mon.asm` (monitor code)

**Step 1: Write Chapter 3**

Document each 16-bit pointer pair in the vector table. Each gets a `\mementry` with both bytes:

| Address | Name | Points To |
|---------|------|-----------|
| $0200-$0201 | VGC Base | $A000 |
| $0202-$0203 | RegCmd | $A010 |
| $0204-$0205 | CharRam Base | $AA00 |
| $0206-$0207 | ColorRam Base | $B1D0 |
| $0208-$0209 | SID Base | $D400 |
| $020A-$020B | FileIO Base | $B9A0 |
| $020C-$020D | XMC Base | $BA00 |
| $020E-$020F | Timer Base | $BA40 |
| $0210-$0211 | SID2 Base | $D420 |
| $0212-$0213 | NIC Base | $A100 |
| $0214-$0215 | DMA Base | $BA63 |
| $0216-$0217 | Blitter Base | $BA83 |

Include BASIC examples showing how to read vector table entries:

```latex
\begin{lstlisting}
10 REM read VGC base address from vector table
20 PRINT "VGC AT: $"; HEX$(DEEK($0200))
30 REM use it for indirect register access
40 BASE = DEEK($0200)
50 POKE BASE + 1, 6 : REM set background color
\end{lstlisting}
```

Explain that the monitor at $FF80 writes these at boot, and why programs should read them rather than hardcoding addresses (forward compatibility).

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch03-vector-table.tex
git commit -m "feat: add Chapter 3 (Vector Table)"
```

---

### Task 6: Chapter 4 — BASIC Program Space ($0280–$9FFF)

**Files:**
- Modify: `docs/mapping/chapters/ch04-basic-program-space.tex`

**Reference files:**
- `ehbasic/basic.asm` (tokenizer, line storage)
- `ehbasic/tokens.json` (complete token list)

**Step 1: Write Chapter 4**

This chapter covers BASIC's internal data structures. Sections:

1. **Memory layout overview** — diagram showing program text, simple variables, arrays, free space, string heap, top of memory
2. **Tokenized line format** — 2-byte next-line pointer, 2-byte line number, tokenized content, $00 terminator. Walk through an example line with hex dump.
3. **Token table** — full table of all token byte values ($80–$FF) mapped to keywords. Source from `ehbasic/tokens.json`.
4. **Simple variable storage** — 2-byte name (first char + second char with type bits in high bits), followed by value bytes. Numeric: 4-byte float. String: 1-byte length + 2-byte pointer.
5. **Array storage** — follows simple variables. Header: 2-byte name, 2-byte total size, 1-byte dimension count, then size per dimension, then packed values.
6. **String heap** — grows downward from Ememl/Ememh. Garbage collection compacts when allocation fails.
7. **Free memory** — between end of arrays and bottom of string heap.

Include BASIC programs that walk the data structures:

```latex
\begin{lstlisting}
10 REM walk the program line list
20 P = DEEK($79) : REM start of program
30 DO
40   NXT = DEEK(P) : REM next line pointer
50   IF NXT = 0 THEN EXIT DO
60   LN = DEEK(P+2) : REM line number
70   PRINT "LINE"; LN; "AT $"; HEX$(P)
80   P = NXT
90 LOOP
\end{lstlisting}
```

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch04-basic-program-space.tex
git commit -m "feat: add Chapter 4 (BASIC Program Space)"
```

---

### Task 7: Chapter 5 — The Virtual Graphics Controller ($A000–$A0BF)

**Files:**
- Modify: `docs/mapping/chapters/ch05-vgc.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/VgcConstants.cs` (all register and command constants)
- `e6502.Avalonia/Hardware/VirtualGraphicsController.cs` (command implementation)

**Step 1: Write Chapter 5**

The largest chapter (~60 pages). Structure:

1. **Introduction** — what the VGC is, three-layer rendering model, 60Hz frame rate
2. **Core registers ($A000–$A00F)** — one `\mementry` each for RegMode, RegBgCol, RegFgCol, RegCursorX/Y, RegScrollX/Y, RegFont, RegStatus, RegSpriteCount, RegCursorEnable, RegColSt, RegColBg, RegBorder, RegCharOut, RegCharIn
3. **Command interface ($A010–$A01E)** — `\mementry` for RegCmd with full command reference:
   - Summary table of all commands
   - Sub-entry per command using `\cmdentry`:
     - Graphics: CmdPlot ($01), CmdUnplot ($02), CmdLine ($03), CmdCircle ($04), CmdRect ($05), CmdFill ($06), CmdGcls ($07), CmdGcolor ($08), CmdPaint ($09)
     - Sprites: CmdSprDef ($10), CmdSprRow ($11), CmdSprClr ($12), CmdSprCopy ($13), CmdSprPos ($14), CmdSprEna ($15), CmdSprDis ($16), CmdSprFlip ($17), CmdSprPri ($18)
     - Memory I/O: CmdMemRead ($19), CmdMemWrite ($1A)
     - Copper: CmdCopperAdd ($1B), CmdCopperClear ($1C), CmdCopperEnable ($1D), CmdCopperDisable ($1E), CmdCopperList ($20), CmdCopperUse ($21), CmdCopperListEnd ($22)
     - System: CmdSysReset ($1F)
   - Each command entry shows: which P registers it reads, parameter meaning, BASIC POKE example
4. **IRQ control ($A01F)** — `\mementry` with `\bitfield`
5. **Help system ($A020–$A030)** — brief entries
6. **Compiler controller ($A031–$A03E)** — entries for each register
7. **ROM swap ($A03F)** — single entry
8. **Sprite registers ($A040–$A0BF)** — explain the 8-byte-per-sprite layout, then document each offset. Use one `\mementry` per sprite register showing the formula `$A040 + N*8 + offset`.

Each graphics command should include a complete POKE-based example and note the equivalent high-level BASIC command:

```latex
\cmdentry{03}{CmdLine}{Draw a line between two points}

Draws a straight line from (\texttt{P0|P1\textless\textless8},
\texttt{P2|P3\textless\textless8}) to
(\texttt{P4|P5\textless\textless8}, \texttt{P6|P7\textless\textless8})
in color \texttt{P8}.

\begin{tabular}{@{}ll@{}}
  \toprule
  Register & Value \\
  \midrule
  P0--P1 & X1 (little-endian) \\
  P2--P3 & Y1 (little-endian) \\
  P4--P5 & X2 (little-endian) \\
  P6--P7 & Y2 (little-endian) \\
  P8     & Color (0--15) \\
  \bottomrule
\end{tabular}

\begin{lstlisting}
10 REM draw a red diagonal line with POKE
20 POKE $A011, 0 : POKE $A012, 0   : REM X1=0
30 POKE $A013, 0 : POKE $A014, 0   : REM Y1=0
40 POKE $A015, 63: POKE $A016, 1   : REM X2=319
50 POKE $A017, 199:POKE $A018, 0   : REM Y2=199
60 POKE $A019, 2                    : REM color=red
70 POKE $A010, 3                    : REM execute CmdLine
\end{lstlisting}

\begin{notebox}
The BASIC command \texttt{LINE x1,y1,x2,y2,c} performs exactly these
register writes.
\end{notebox}
```

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch05-vgc.tex
git commit -m "feat: add Chapter 5 (VGC)"
```

---

### Task 8: Chapter 6 — The Network Interface Controller ($A100–$A13F)

**Files:**
- Modify: `docs/mapping/chapters/ch06-nic.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/VirtualNetworkController.cs`

**Step 1: Write Chapter 6**

Structure:
1. Introduction — message-oriented TCP, 4 slots, client and server modes
2. Command & status registers ($A100–$A104) with `\mementry` and `\bitfield` for status/IRQ
3. Connection parameters ($A108–$A10B)
4. DMA registers ($A110–$A113)
5. Per-slot status ($A118–$A11B) with `\bitfield`
6. Hostname buffer ($A120–$A13F)
7. Command reference — one `\cmdentry` each for Connect, Disconnect, Send, Recv, Listen, Accept
8. Complete client example and server example in BASIC

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch06-nic.tex
git commit -m "feat: add Chapter 6 (NIC)"
```

---

### Task 9: Chapter 7 — The Wavetable Synthesizer ($A140–$A1DF)

**Files:**
- Modify: `docs/mapping/chapters/ch07-wts.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/WavetableSynth.cs`

**Step 1: Write Chapter 7**

Structure:
1. Introduction — 8-voice sample-based synth, SF2 soundfonts, stereo output
2. Voice registers ($A140–$A17F) — 8 voices x 8 bytes, document each offset
3. Global registers ($A180–$A187) — reverb, chorus, master volume, soundfont status, commands
4. Instrument enumeration ($A1A0–$A1DF) — how to list available instruments
5. BASIC examples: loading a soundfont, playing notes, listing instruments

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch07-wts.tex
git commit -m "feat: add Chapter 7 (WTS)"
```

---

### Task 10: Chapter 8 — Screen Memory ($AA00–$B99F)

**Files:**
- Modify: `docs/mapping/chapters/ch08-screen-memory.tex`

**Step 1: Write Chapter 8**

Structure:
1. Introduction — 80x25 text display, character + color RAM
2. Character RAM ($AA00–$B1CF) — `\memrange`, screen coordinate math: `addr = $AA00 + y*80 + x`
3. Color RAM ($B1D0–$B99F) — `\memrange`, same coordinate math: `addr = $B1D0 + y*80 + x`
4. Character codes — standard ASCII mapping
5. Color values — reference to Appendix D
6. BASIC examples: direct screen writes, reading screen content, fast text rendering

```latex
\begin{lstlisting}
10 REM write "HELLO" directly to screen RAM
20 FOR I=0 TO 4
30   POKE $AA00+I, ASC(MID$("HELLO",I+1,1))
40   POKE $B1D0+I, 2  : REM red color
50 NEXT I
\end{lstlisting}
```

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch08-screen-memory.tex
git commit -m "feat: add Chapter 8 (Screen Memory)"
```

---

### Task 11: Chapter 9 — The File I/O Controller ($B9A0–$B9EF)

**Files:**
- Modify: `docs/mapping/chapters/ch09-file-io.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/FileIoController.cs`

**Step 1: Write Chapter 9**

Structure:
1. Introduction — coprocessor for file ops, SID playback, music, MIDI
2. Registers ($B9A0–$B9AF) — command, status, error, filename length, addresses, graphics space
3. Filename buffer ($B9B0–$B9EF)
4. File metadata buffer ($BAB0–$BB1F) — separate section covering all metadata fields
5. Command reference — one `\cmdentry` per command code ($01–$26)
6. Status codes and error codes tables
7. BASIC examples for each major operation

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch09-file-io.tex
git commit -m "feat: add Chapter 9 (File I/O Controller)"
```

---

### Task 12: Chapter 10 — The Expansion Memory System ($BA00–$BFFF)

**Files:**
- Modify: `docs/mapping/chapters/ch10-expansion-memory.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/ExpansionMemoryController.cs`
- `e6502.Avalonia/Hardware/TimerController.cs`
- `e6502.Avalonia/Hardware/DmaController.cs`
- `e6502.Avalonia/Hardware/BlitterController.cs`
- `e6502.Avalonia/Hardware/MusicEngine.cs` (music status)

**Step 1: Write Chapter 10**

This chapter covers the $BA00–$BFFF range which contains multiple subsystems. Structure:

1. **XMC Registers ($BA00–$BA3F)** — all registers + command reference
2. **Timer Controller ($BA40–$BA4F)** — registers + timing math + IRQ example
3. **Music Status ($BA50–$BA62)** — status flags, per-voice note readback, elapsed/total frames
4. **DMA Controller ($BA63–$BA82)** — registers, space IDs, command, fill mode, BASIC example
5. **Blitter Controller ($BA83–$BAA2)** — registers, 2D copy, color-key transparency, stride math
6. **File Metadata Buffer ($BAB0–$BB1F)** — all fields for SID/MIDI/BIN metadata
7. **XMC Windows ($BC00–$BFFF)** — how window mapping works, enable bits, BASIC example

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch10-expansion-memory.tex
git commit -m "feat: add Chapter 10 (Expansion Memory System)"
```

---

### Task 13: Chapter 11 — The ROM ($C000–$FFFF)

**Files:**
- Modify: `docs/mapping/chapters/ch11-rom.tex`

**Reference files:**
- `ehbasic/min_mon.asm` (monitor code)
- `ehbasic/basic.asm` (EhBASIC entry points)

**Step 1: Write Chapter 11**

Structure:
1. Introduction — 16KB ROM, write-protected (except vectors), EhBASIC + monitor
2. ROM layout overview — approximate ranges for cold start, interpreter loop, support routines, monitor
3. Key entry points — cold start, warm start, character I/O vectors
4. Monitor code ($FF80–$FFF9) — what it does at reset: CLD, set SP, init vector table, jump to BASIC
5. Cross-reference to SID: "The SID sound chips occupy $D400–$D43F within this ROM range. Although reads return ROM data, writes are intercepted and routed to the SID hardware. See Chapter~12 for complete SID register documentation."
6. Hardware vectors ($FFFA–$FFFF) — NMI, RESET, IRQ vectors. Note these are writable (exception to ROM write protection) for SID players and custom interrupt handlers.

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch11-rom.tex
git commit -m "feat: add Chapter 11 (ROM)"
```

---

### Task 14: Chapter 12 — The SID Sound Chips ($D400–$D43F)

**Files:**
- Modify: `docs/mapping/chapters/ch12-sid.tex`

**Reference files:**
- `e6502.Avalonia/Hardware/SidChip.cs`
- Real MOS 6581 datasheet (standard SID register layout)

**Step 1: Write Chapter 12**

Structure:
1. Introduction — two SID chips (SID 1 at $D400, SID 2 at $D420), software emulation of the MOS 6581, 3 voices per chip, write-only (inside ROM range, writes intercepted)
2. **Voice registers** — 7 bytes per voice, 3 voices:
   - Frequency low/high (offset +0, +1)
   - Pulse width low/high (offset +2, +3, only 12 bits)
   - Waveform control (offset +4) with `\bitfield`: noise, pulse, sawtooth, triangle, test, ring mod, sync, gate
   - Attack/Decay (offset +5): upper nibble attack, lower nibble decay
   - Sustain/Release (offset +6): upper nibble sustain, lower nibble release
3. **Filter registers** ($D415–$D418):
   - Filter cutoff low 3 bits / high 8 bits
   - Filter resonance + voice enable
   - Filter mode + master volume with `\bitfield`: 3off, HP, BP, LP, vol3-vol0
4. **Extended per-voice volume** ($D41D–$D41F) — NovaBASIC extension not on real SID
5. **SID 2** ($D420–$D43F) — same layout, $20 offset. Note mirror at $D500.
6. ADSR timing table — attack/decay/release time values
7. BASIC examples: playing a note, setting up a filter, using ring modulation

Each voice offset documented relative to voice base address (Voice 1 = +$00, Voice 2 = +$07, Voice 3 = +$0E).

**Step 2: Build and verify**

**Step 3: Commit**

```bash
git add docs/mapping/chapters/ch12-sid.tex
git commit -m "feat: add Chapter 12 (SID Sound Chips)"
```

---

### Task 15: Appendix A — VGC Bitmap & Sprite Shape RAM

**Files:**
- Modify: `docs/mapping/chapters/appendix-a-vgc-offbus.tex`

**Step 1: Write Appendix A**

1. **Graphics bitmap** — 64,000 bytes (320x200), 4-bit color per pixel (2 pixels per byte? or 1 byte per pixel — check VGC implementation). Address math: `offset = y * 320 + x`. Accessed via CmdMemRead/CmdMemWrite (space $02) or DMA (space 3).
2. **Sprite shape RAM** — 256 shape slots, each 16x16 pixels = 256 bytes per shape (1 byte per pixel, 4-bit color). Accessed via CmdMemRead/CmdMemWrite (space $03) or DMA (space 4). Total: 64KB.
3. Access examples using POKE sequences and DMA

**Step 2: Commit**

```bash
git add docs/mapping/chapters/appendix-a-vgc-offbus.tex
git commit -m "feat: add Appendix A (VGC off-bus memory)"
```

---

### Task 16: Appendix B — Expansion RAM (XRAM)

**Files:**
- Modify: `docs/mapping/chapters/appendix-b-xram.tex`

**Step 1: Write Appendix B**

1. Overview — 512KB (8 banks of 64KB), expandable
2. Bank structure and addressing (24-bit: bank byte + 16-bit offset)
3. Access methods: XMC commands (GetByte/PutByte/Stash/Fetch), XMC windows, DMA (space 5)
4. Named block allocation — how XALLOC/XDIR/XDEL work at the register level
5. Page usage tracking
6. BASIC examples

**Step 2: Commit**

```bash
git add docs/mapping/chapters/appendix-b-xram.tex
git commit -m "feat: add Appendix B (XRAM)"
```

---

### Task 17: Appendix C — WTS Sample Memory

**Files:**
- Modify: `docs/mapping/chapters/appendix-c-wts-samples.tex`

**Step 1: Write Appendix C**

1. Overview — samples stored in host memory, not 6502-addressable
2. SF2 soundfont loading — triggered via FileIO command $15 (FioCmdSfLoad)
3. Instrument mapping — GM program numbers, bank/preset structure
4. Memory model — samples decoded from SF2 at load time, stored as PCM in host RAM
5. Limitations and capacity

**Step 2: Commit**

```bash
git add docs/mapping/chapters/appendix-c-wts-samples.tex
git commit -m "feat: add Appendix C (WTS Sample Memory)"
```

---

### Task 18: Appendix D — Color Table

**Files:**
- Modify: `docs/mapping/chapters/appendix-d-color-table.tex`

**Reference files:**
- `e6502.Avalonia/Rendering/ColorPalette.cs`

**Step 1: Write Appendix D**

Full 16-color palette table with:
- Index (0–15)
- Name
- RGB values
- Color swatch (TikZ filled rectangle)

```latex
\begin{tabular}{clccc}
  \toprule
  \textbf{Index} & \textbf{Name} & \textbf{R} & \textbf{G} & \textbf{B} \\
  \midrule
  0  & Black       & 0   & 0   & 0   \\
  1  & White       & 255 & 255 & 255 \\
  2  & Red         & 136 & 0   & 0   \\
  3  & Cyan        & 170 & 255 & 238 \\
  4  & Purple      & 204 & 68  & 204 \\
  5  & Green       & 0   & 204 & 85  \\
  6  & Blue        & 0   & 0   & 170 \\
  7  & Yellow      & 238 & 238 & 119 \\
  8  & Orange      & 221 & 136 & 85  \\
  9  & Brown       & 102 & 68  & 0   \\
  10 & Light Red   & 255 & 119 & 119 \\
  11 & Dark Grey   & 51  & 51  & 51  \\
  12 & Medium Grey & 119 & 119 & 119 \\
  13 & Light Green & 170 & 255 & 102 \\
  14 & Light Blue  & 0   & 136 & 255 \\
  15 & Light Grey  & 187 & 187 & 187 \\
  \bottomrule
\end{tabular}
```

Include TikZ color swatches using `\definecolor` for each palette entry.

**Step 2: Commit**

```bash
git add docs/mapping/chapters/appendix-d-color-table.tex
git commit -m "feat: add Appendix D (Color Table)"
```

---

### Task 19: Appendix E — ASCII & Screen Code Table

**Files:**
- Modify: `docs/mapping/chapters/appendix-e-ascii-table.tex`

**Step 1: Write Appendix E**

1. Standard ASCII table (0–127) — code, hex, character, description
2. Control characters relevant to the screen editor (CR, LF, BS, DEL, etc.)
3. Note that NovaBASIC uses standard ASCII (not PETSCII like the C64)

**Step 2: Commit**

```bash
git add docs/mapping/chapters/appendix-e-ascii-table.tex
git commit -m "feat: add Appendix E (ASCII Table)"
```

---

### Task 20: Final Build, Review, and Polish

**Step 1: Full build**

Run: `cd docs/mapping && latexmk -pdf mapping_the_novavm.tex`

Run twice to resolve cross-references and index.

**Step 2: Review the PDF**

Check:
- Cover page renders correctly
- Table of contents is complete
- All `\mementry` headers render with blue address boxes
- `\bitfield` diagrams are properly aligned
- `\cmdentry` sub-entries are visually distinct
- Cross-references (\addr{XXXX}) are clickable hyperlinks
- Index at back is sorted by hex address
- Code listings have proper syntax highlighting
- Callout boxes (notebox, warningbox, tipbox) render correctly

**Step 3: Fix any LaTeX compilation warnings**

Address overfull hboxes, missing references, etc.

**Step 4: Commit final version**

```bash
git add docs/mapping/
git commit -m "feat: complete Mapping the NovaVM reference document"
```
